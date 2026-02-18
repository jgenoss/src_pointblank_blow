// FileCheck_ProgressMD5View.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "FileCheck_ProgressMD5View.h"

#include "FileCheck_PackScriptMgr.h"
#include "DiffResult_PackScriptResult.h"

#include "i3Base/string/ext/mb_to_utf16.h"

#include "i3Base/itl/bind/bind.h"

#include "PathMgr.h"

#include "FileCheck_ProgressBarView.h"
#include "FileCheck_MD5ModifiedView.h"

#include "PackMode.h"
#include "i3Base/string/ext/extract_filename.h"

// FileCheck_ProgressMD5View
namespace
{

	struct FileCheck_Progress_CB : FileCheck_MD5Thread_Callback
	{
		FileCheck_ProgressMD5View*	owner;
		bool						left_success;

		FileCheck_Progress_CB(FileCheck_ProgressMD5View* owner) : owner(owner), left_success(false){}
		virtual ~FileCheck_Progress_CB() {}	

		virtual void	OnRedrawBlockStart() 
		{
			i3MFCListCtrl* ctrl = owner->GetListCtrl();
			ctrl->SetRedraw(FALSE);

			FileCheck_ProgressBarView* barView = owner->GetProgressBarView();
			barView->SetRedrawProgressCtrl(FALSE);
			
			FileCheck_MD5ModifiedView* modView = owner->GetModifiedView();
			modView->SetRedrawListCtrl(FALSE);
		}

		virtual void	OnRedrawBlockEnd() 
		{
			i3MFCListCtrl* ctrl = owner->GetListCtrl();

			INT32 item_count = ctrl->GetItemCount();
			ctrl->SetRedraw(TRUE);

			if (item_count > 0)
				ctrl->EnsureVisible(item_count-1, FALSE);		// EnsureVisible이 SetRedraw구간의 깜박임 문제를
			// 야기시키는것으로 의심되어 마지막에 처리해준다.
			ctrl->RedrawWindow();

			FileCheck_ProgressBarView* barView = owner->GetProgressBarView();
			barView->SetRedrawProgressCtrl(TRUE);

			FileCheck_MD5ModifiedView* modView = owner->GetModifiedView();
			modView->SetRedrawListCtrl(TRUE);
		}

		virtual void	OnStart(size_t num_list) 
		{
			// 스타트업 되면 일련의 리스트콘트롤을 삭제하고, 이웃뷰 정보도 갱신해둔다.
			left_success = false;

			i3MFCListCtrl* ctrl = owner->GetListCtrl();
			ctrl->DeleteAllItems();
			
			ctrl->SetItemCount(num_list);

			FileCheck_ProgressBarView* barView = owner->GetProgressBarView();

			barView->StartProgress(num_list);
			
			FileCheck_MD5ModifiedView* modView = owner->GetModifiedView();
			modView->DeleteAllResult();
		}

		virtual void	OnLeftFileCheckStart(size_t index) 
		{
			i3MFCListCtrl* ctrl = owner->GetListCtrl();
			const DiffResult_PackScriptResult*	diff_result = owner->GetDiffResult();

//			const i3::vector<const PackScriptUnitResult*>& v_unit_res = diff_result->in_out_found_lhs_res;
//			const i3::vector<size_t>& v_string_idx = diff_result->in_out_found_lhs_string_idx;

			const i3::vector<PSUnitResult_MapResFilePtr>& l_vec = diff_result->in_out_alist[DIFFRESULT_PS_FOUND_LEFT];
			const i3::vector<PSUnitResult_MapResFilePtr>& r_vec = diff_result->in_out_alist[DIFFRESULT_PS_FOUND_RIGHT];


			const PSUnitResult_MapResFile* l_resFile = l_vec[index].get();
			const PSUnitResult_MapResFile* r_resFile = r_vec[index].get();

			i3::wstring wstr;
			const i3::string& strFile = l_resFile->unit->result_file_list[ l_resFile->string_idx ];		// v_unit_res[index]->result_file_list[v_string_idx[index] ];
			i3::mb_to_utf16(strFile, wstr);

			ctrl->InsertItem(index, wstr.c_str() );		
			ctrl->SetItemText(index, 1, L"Calculating");
			ctrl->SetItemText(index, 2, L"Waiting");
			ctrl->SetItemText(index, 3, L"");

			//
			if ( l_resFile->unit->parent)
			{
				const i3::wstring& ps_path = l_resFile->unit->parent->parent->packscript_path;
				i3::extract_filename(ps_path, wstr);
				ctrl->SetItemText(index, 4, wstr.c_str());					
			}

			const RSCFileResultMapElement* elem = FileCheck_PackScriptMgr::i()->FindRSCFileMapElement(strFile);

			if (elem && !elem->contents_result_list_by_path[0].empty() )
			{
				const RSCFileContentsResult* contents =	elem->contents_result_list_by_path[0][0];

				const i3::rc_string& rscFileName = contents->parent->rsc_file_path;

				i3::mb_to_utf16(rscFileName, wstr);
				ctrl->SetItemText(index, 5, wstr.c_str());
			}

			if ( r_resFile->unit->parent)
			{
				const i3::wstring& ps_path = r_resFile->unit->parent->parent->packscript_path;
				i3::extract_filename(ps_path, wstr);
				ctrl->SetItemText(index, 6, wstr.c_str());					
			}

			if (elem && !elem->contents_result_list_by_path[1].empty() )
			{
				const RSCFileContentsResult* contents =	elem->contents_result_list_by_path[1][0];

				const i3::rc_string& rscFileName = contents->parent->rsc_file_path;

				i3::mb_to_utf16(rscFileName, wstr);
				ctrl->SetItemText(index, 7, wstr.c_str());
			}



			//
		}

		virtual void	OnLeftFileCheckEnd(size_t index, bool bSuccess, const UINT128& md5) 
		{
			i3MFCListCtrl* ctrl = owner->GetListCtrl();

			left_success = bSuccess;

			if (bSuccess)
			{
				i3::wstring wstr;
				i3::sprintf(wstr, L"%d%d", md5[0], md5[1]);
				ctrl->SetItemText(index, 1, wstr.c_str());
			}
			else
			{
				ctrl->SetItemText(index, 1, L"Failed");
			}
		}

		virtual void	OnRightFileCheckStart(size_t index) 
		{
			i3MFCListCtrl* ctrl = owner->GetListCtrl();
			ctrl->SetItemText(index, 2, L"Calculating");
		}

		virtual void	OnRightFileCheckEnd(size_t index, bool bSuccess, bool is_enc,
			const UINT128& md5, bool is_same) 
		{
			i3MFCListCtrl* ctrl = owner->GetListCtrl();

			if (bSuccess)
			{
				i3::wstring wstr;
				i3::sprintf(wstr, L"%d%d", md5[0], md5[1]);
				ctrl->SetItemText(index, 2, wstr.c_str());
			}
			else
			{
				ctrl->SetItemText(index, 2, L"Failed");
			}
			
			if ( is_same )
			{
				ctrl->SetItemText(index, 3, L"");
			}
			else
			if ( left_success == false || bSuccess == false)
			{
				ctrl->SetItemText(index, 3, L"Failed");	// 에러..
			}
			else
			{
				ctrl->SetItemText(index, 3, L"O");		// MD5체크되고, 서로 다른 파일..
				
				FileCheck_MD5ModifiedView* modView = owner->GetModifiedView();
				modView->InsertModifiedResult(index);
			}

			FileCheck_ProgressBarView* barView = owner->GetProgressBarView();
			
			barView->SetProgressPos( index + 1 );

		}

		virtual void	OnPause(size_t next_index, int next_l_r) 
		{
			FileCheck_ProgressBarView* barView = owner->GetProgressBarView();
			
			barView->PauseProgress(next_index);

		}

		virtual void	OnResume(size_t next_index, int next_l_r) 
		{
			FileCheck_ProgressBarView* barView = owner->GetProgressBarView();

			barView->ResumeProgress(next_index);

		}
		virtual void	OnInterrupt(size_t next_index, int next_l_r) 
		{
			FileCheck_ProgressBarView* barView = owner->GetProgressBarView();
			
			barView->StopProgress(next_index);
		}

		virtual void	OnEnd() 
		{
			FileCheck_ProgressBarView* barView = owner->GetProgressBarView();

			barView->EndProgress();
		}
	};

}

IMPLEMENT_DYNCREATE(FileCheck_ProgressMD5View, CView)

FileCheck_ProgressMD5View::FileCheck_ProgressMD5View() : m_diff_result(NULL), 
m_barView(NULL), m_modView(NULL)
{
	m_cb = new FileCheck_Progress_CB(this);

	i3::signal<void()>* sig = FileCheck_PackScriptMgr::i()->GetProgressSignal();

	sig->connect_embedded(&m_md5_queue, 
		i3::bind(&FileCheck_MD5Queue::UpdateProgress_InMainThread, &m_md5_queue, m_cb)  );

}

FileCheck_ProgressMD5View::~FileCheck_ProgressMD5View()
{
	delete m_cb;
}

BEGIN_MESSAGE_MAP(FileCheck_ProgressMD5View, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// FileCheck_ProgressMD5View 그리기입니다.

void FileCheck_ProgressMD5View::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// FileCheck_ProgressMD5View 진단입니다.

#ifdef _DEBUG
void FileCheck_ProgressMD5View::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void FileCheck_ProgressMD5View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// FileCheck_ProgressMD5View 메시지 처리기입니다.

int FileCheck_ProgressMD5View::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_listCtrl.Create(this, 1);
	m_listCtrl.InsertColumn(0, L"File");

	PackMode m = FileCheck_PackScriptMgr::i()->GetCurrPackMode();
	
	const wchar_t* left_md5, * right_md5;

	if (m == PACKMODE_MEDIA_TO_QA_COPY)
	{
		left_md5 = L"MediaPath MD5";
		right_md5 = L"QAPath MD5";
	}
	else
	{
		left_md5 = L"QAPath MD5";
		right_md5 = L"LivePath MD5";
	}
	
	
	m_listCtrl.InsertColumn(1, left_md5, LVCFMT_LEFT, 100);
	m_listCtrl.InsertColumn(2, right_md5, LVCFMT_LEFT, 100);
	m_listCtrl.InsertColumn(3, L"Different", LVCFMT_CENTER, 80);
	m_listCtrl.InsertColumn(4, L"PACKSCRIPT", LVCFMT_LEFT, 200);
	m_listCtrl.InsertColumn(5, L"RSC_FILE", LVCFMT_LEFT, 200);
	m_listCtrl.InsertColumn(6, L"PACKSCRIPT", LVCFMT_LEFT, 200);
	m_listCtrl.InsertColumn(7, L"RSC_FILE", LVCFMT_LEFT, 200);



	m_title.Create(NULL, WS_VISIBLE|WS_CHILD|SS_CENTERIMAGE, CRect(0,0,0,0), this);
	m_title.SetFont(m_listCtrl.GetFont(), FALSE);

	return 0;
}

void FileCheck_ProgressMD5View::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_title.MoveWindow(0,0, cx, GetSystemMetrics(SM_CYCAPTION) );
	m_listCtrl.MoveWindow(0, GetSystemMetrics(SM_CYCAPTION), cx, cy-GetSystemMetrics(SM_CYCAPTION) );

	//m_listCtrl.SetSize(cx, cy);

	SCROLLINFO info;
	m_listCtrl.GetScrollInfo(SB_VERT, &info, SIF_RANGE | SIF_PAGE );
	// Range와 Page 관계를 통해 스크롤바의 출력 여부를 확인했다.. ( 리스트 콘트롤 기준이며, 그외는 사용자임의 기준이다.)
	// Max + 1 > Page 인 경우 스크롤바가 출력된다..
	if ( UINT(info.nMax) + 1 > info.nPage )
	{
		// horz크기를 조정해야한다. (종 스크롤바 너비만큼을 빼야한다.)
		cx -= GetSystemMetrics(SM_CXVSCROLL) + GetSystemMetrics(SM_CXBORDER);
	}

	m_listCtrl.SetColumnWidth(0, cx / 2);

	m_listCtrl.RedrawWindow();

}

BOOL FileCheck_ProgressMD5View::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

//	return CView::OnEraseBkgnd(pDC);
	return TRUE;
}


void	FileCheck_ProgressMD5View::InitCaption(const wchar_t* caption)
{
	m_title.SetWindowText(caption);
}

void	FileCheck_ProgressMD5View::InitList( const DiffResult_PackScriptResult& diff_result)
{

	m_diff_result = &diff_result;

//	const i3::vector<const PackScriptUnitResult*>*	pv_unit_res;
//	const i3::vector<size_t>*					pv_string_idx;


//	{
		const i3::vector<PSUnitResult_MapResFilePtr>& l_vec = diff_result.in_out_alist[DIFFRESULT_PS_FOUND_LEFT];
		const i3::vector<PSUnitResult_MapResFilePtr>& r_vec = diff_result.in_out_alist[DIFFRESULT_PS_FOUND_RIGHT];

//		pv_unit_res = &diff_result.in_out_found_lhs_res;
//		pv_string_idx = &diff_result.in_out_found_lhs_string_idx;
//	}
	
		
//	const i3::vector<const PackScriptUnitResult*>&	v_unit_res = *pv_unit_res;
//	const i3::vector<size_t>&					v_string_idx = *pv_string_idx;

	const size_t num = l_vec.size();

	i3::wstring wstr;

	m_listCtrl.SetRedraw(FALSE);

	for (size_t i = 0 ; i < num ; ++i)
	{
		const PSUnitResult_MapResFile* l_resFile = l_vec[i].get();
		const PSUnitResult_MapResFile* r_resFile = r_vec[i].get();

		const i3::string& strFile = l_resFile->unit->result_file_list[l_resFile->string_idx];

		i3::mb_to_utf16(strFile, wstr);
		m_listCtrl.InsertItem(i, wstr.c_str());

		if ( l_resFile->unit->parent)
		{
			const i3::wstring& ps_path = l_resFile->unit->parent->parent->packscript_path;
			i3::extract_filename(ps_path, wstr);
			m_listCtrl.SetItemText(i, 4, wstr.c_str());					
		}

		const RSCFileResultMapElement* elem = FileCheck_PackScriptMgr::i()->FindRSCFileMapElement(strFile);

		if (elem && !elem->contents_result_list_by_path[0].empty() )
		{
			const RSCFileContentsResult* contents =	elem->contents_result_list_by_path[0][0];

			const i3::rc_string& rscFileName = contents->parent->rsc_file_path;

			i3::mb_to_utf16(rscFileName, wstr);
			m_listCtrl.SetItemText(i, 5, wstr.c_str());
		}

		if ( r_resFile->unit->parent)
		{
			const i3::wstring& ps_path = r_resFile->unit->parent->parent->packscript_path;
			i3::extract_filename(ps_path, wstr);
			m_listCtrl.SetItemText(i, 6, wstr.c_str());					
		}

		if (elem && !elem->contents_result_list_by_path[1].empty() )
		{
			const RSCFileContentsResult* contents =	elem->contents_result_list_by_path[1][0];

			const i3::rc_string& rscFileName = contents->parent->rsc_file_path;

			i3::mb_to_utf16(rscFileName, wstr);
			m_listCtrl.SetItemText(i, 7, wstr.c_str());
		}





	}
	
	m_listCtrl.SetRedraw(TRUE);

	PackMode curr_pm =	FileCheck_PackScriptMgr::i()->GetCurrPackMode();

	m_md5_queue.set_curr_path_unit(&PathMgr::i()->GetCurrPathUnit() );
	m_md5_queue.SetPackScriptResult(m_diff_result, curr_pm);
	
	/// 여기서부터는 테스트 코드...나중에 정리할 것..
//	m_listCtrl.DeleteAllItems();		//
	
//	m_md5_queue.Start();
	
}


// 직접 리스트콘트롤을 통제하지 말고, 콜백쪽에서 리스트콘트롤 처리를 할 것..
void	FileCheck_ProgressMD5View::Start()
{
	m_md5_queue.Start();
}

void	FileCheck_ProgressMD5View::Pause()
{
	m_md5_queue.Pause();
}

void	FileCheck_ProgressMD5View::Resume()
{
	m_md5_queue.Resume();	
}

void	FileCheck_ProgressMD5View::Stop()
{
	m_md5_queue.Interrupt();
}
