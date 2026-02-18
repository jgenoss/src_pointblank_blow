// FileCheck_NoFileView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "FileCheck_NoFileView.h"

#include "DiffResult_PackScriptResult.h"
#include "FileCheck_PackScriptMgr.h"
#include "PathMode.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/extract_filename.h"

// FileCheck_NoFileView

IMPLEMENT_DYNCREATE(FileCheck_NoFileView, CView)

FileCheck_NoFileView::FileCheck_NoFileView() : m_diff_result(NULL)
{
	m_apm[0] = PATHMODE_MEDIA;
	m_apm[1] = PATHMODE_QA;
}

FileCheck_NoFileView::~FileCheck_NoFileView()
{
}

BEGIN_MESSAGE_MAP(FileCheck_NoFileView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// FileCheck_NoFileView 그리기입니다.

void FileCheck_NoFileView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// FileCheck_NoFileView 진단입니다.

#ifdef _DEBUG
void FileCheck_NoFileView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void FileCheck_NoFileView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// FileCheck_NoFileView 메시지 처리기입니다.

int FileCheck_NoFileView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_listCtrl.Create(this, 1);
	m_listCtrl.InsertColumn(0, L"File");
	m_listCtrl.InsertColumn(1, L"Diff", LVCFMT_CENTER, 100);

	m_apm[0] = FileCheck_PackScriptMgr::i()->GetCurrLeftPathMode();
	m_apm[1] = FileCheck_PackScriptMgr::i()->GetCurrRightPathMode();
	
	const wchar_t* str_lm = (m_apm[0] == PATHMODE_MEDIA) ? L"MEDIA FILE" : L"QA FILE";
	const wchar_t* str_rm = (m_apm[1] == PATHMODE_QA) ? L"QA FILE" : L"LIVE FILE";

	m_listCtrl.InsertColumn(2, str_lm, LVCFMT_CENTER, 80);
	m_listCtrl.InsertColumn(3, str_rm, LVCFMT_CENTER, 80);

	
	m_listCtrl.InsertColumn(4, L"PACSCRIPT", LVCFMT_LEFT, 200);
	m_listCtrl.InsertColumn(5, L"RSC_FILE", LVCFMT_LEFT, 200);
	
	m_listCtrl.InsertColumn(6, L"PACSCRIPT", LVCFMT_LEFT, 200);
	m_listCtrl.InsertColumn(7, L"RSC_FILE", LVCFMT_LEFT, 200);


	m_title.Create(NULL, WS_VISIBLE|WS_CHILD|SS_CENTERIMAGE, CRect(0,0,0,0), this);
	m_title.SetFont(m_listCtrl.GetFont(), FALSE);

	return 0;
}

void FileCheck_NoFileView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_title.MoveWindow(0,0, cx, GetSystemMetrics(SM_CYCAPTION) );
	m_listCtrl.MoveWindow(0, GetSystemMetrics(SM_CYCAPTION), cx, cy-GetSystemMetrics(SM_CYCAPTION) );

	SCROLLINFO info;
	m_listCtrl.GetScrollInfo(SB_VERT, &info, SIF_RANGE | SIF_PAGE );
	// Range와 Page 관계를 통해 스크롤바의 출력 여부를 확인했다.. ( 리스트 콘트롤 기준이며, 그외는 사용자임의 기준이다.)
	// Max + 1 > Page 인 경우 스크롤바가 출력된다..
	if ( UINT(info.nMax) + 1 > info.nPage )
	{
		// horz크기를 조정해야한다. (종 스크롤바 너비만큼을 빼야한다.)
		cx -= GetSystemMetrics(SM_CXVSCROLL) + GetSystemMetrics(SM_CXBORDER);
	}

	m_listCtrl.SetColumnWidth(0, cx - 80 * 2 - 100 - 10);

	m_listCtrl.RedrawWindow();

}

BOOL FileCheck_NoFileView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	return TRUE;
//	return CView::OnEraseBkgnd(pDC);
}

void	FileCheck_NoFileView::InitCaption(const wchar_t* caption)
{
	m_title.SetWindowText(caption);
}

void	FileCheck_NoFileView::InitList( const DiffResult_PackScriptResult& diff_result)
{
	m_listCtrl.SetRedraw(FALSE);

	m_diff_result = &diff_result;
	
	int		itemCount = 0;



	{
		const wchar_t* str_lm = (m_apm[0] == PATHMODE_MEDIA) ? L"MEDIA ONLY" : L"QA ONLY";
	
		const i3::vector<PSUnitResult_MapResFilePtr>& vec = diff_result.in_out_alist[DIFFRESULT_PS_LEFT_ONLY];

		const size_t num_resFile = vec.size();
		
		i3::wstring wstrTemp;

		for (size_t j = 0 ; j < num_resFile ; ++j )
		{
			const PSUnitResult_MapResFile* resFile = vec[j].get();
			
			if (!resFile->file_exist)
			{
				const i3::string& strFile = resFile->unit->result_file_list[ resFile->string_idx ];
				i3::mb_to_utf16(strFile, wstrTemp);
				m_listCtrl.InsertItem(itemCount, wstrTemp.c_str());
				m_listCtrl.SetItemText(itemCount, 1, str_lm );
				m_listCtrl.SetItemText(itemCount, 2, L"X");
				
				if ( resFile->unit->parent)
				{
					const i3::wstring& ps_path = resFile->unit->parent->parent->packscript_path;
					i3::extract_filename(ps_path, wstrTemp);
					m_listCtrl.SetItemText(itemCount, 4, wstrTemp.c_str());					
				}

				const RSCFileResultMapElement* elem = FileCheck_PackScriptMgr::i()->FindRSCFileMapElement(strFile);

				if (elem && !elem->contents_result_list_by_path[0].empty() )
				{
					const RSCFileContentsResult* contents =	elem->contents_result_list_by_path[0][0];

					const i3::rc_string& rscFileName = contents->parent->rsc_file_path;

					i3::mb_to_utf16(rscFileName, wstrTemp);
					m_listCtrl.SetItemText(itemCount, 5, wstrTemp.c_str());
				}

				++itemCount;
			}
		}
			
	}

	{
		const wchar_t* str_rm = (m_apm[1] == PATHMODE_QA) ? L"QA ONLY" : L"LIVE ONLY";

		const i3::vector<PSUnitResult_MapResFilePtr>& vec = diff_result.in_out_alist[DIFFRESULT_PS_RIGHT_ONLY];

		const size_t num_resFile = vec.size();

		i3::wstring wstrTemp;

		for (size_t j = 0 ; j < num_resFile ; ++j )
		{
			const PSUnitResult_MapResFile* resFile = vec[j].get();

			if (!resFile->file_exist)
			{
				const i3::string& strFile = resFile->unit->result_file_list[ resFile->string_idx ];

				i3::mb_to_utf16(strFile, wstrTemp);

				m_listCtrl.InsertItem(itemCount, wstrTemp.c_str());
				m_listCtrl.SetItemText(itemCount, 1, str_rm);
				m_listCtrl.SetItemText(itemCount, 3, L"X");

				if ( resFile->unit->parent)
				{
					const i3::wstring& ps_path = resFile->unit->parent->parent->packscript_path;
					i3::extract_filename(ps_path, wstrTemp);
					m_listCtrl.SetItemText(itemCount, 6, wstrTemp.c_str());					
				}

				const RSCFileResultMapElement* elem = FileCheck_PackScriptMgr::i()->FindRSCFileMapElement(strFile);

				if (elem && !elem->contents_result_list_by_path[1].empty() )
				{
					const RSCFileContentsResult* contents =	elem->contents_result_list_by_path[1][0];

					const i3::rc_string& rscFileName = contents->parent->rsc_file_path;

					i3::mb_to_utf16(rscFileName, wstrTemp);
					m_listCtrl.SetItemText(itemCount, 7, wstrTemp.c_str());
				}


				++itemCount;
			}
		}

	}

	{
		const wchar_t* str_lm = (m_apm[0] == PATHMODE_MEDIA) ? L"MEDIA-QA" : L"QA-LIVE";

		const i3::vector<PSUnitResult_MapResFilePtr>& l_vec = diff_result.in_out_alist[DIFFRESULT_PS_FOUND_LEFT];
		const i3::vector<PSUnitResult_MapResFilePtr>& r_vec = diff_result.in_out_alist[DIFFRESULT_PS_FOUND_RIGHT];
		
		const size_t num_resFile = l_vec.size();

		i3::wstring wstrTemp;

		for (size_t j = 0 ; j < num_resFile ; ++j )
		{
			const PSUnitResult_MapResFile* l_resFile = l_vec[j].get();
			const PSUnitResult_MapResFile* r_resFile = r_vec[j].get();

			if (!l_resFile->file_exist || !r_resFile->file_exist)
			{
				const i3::string& strFile = l_resFile->unit->result_file_list[ l_resFile->string_idx ];

				i3::mb_to_utf16(strFile, wstrTemp);

				m_listCtrl.InsertItem(itemCount, wstrTemp.c_str());

				m_listCtrl.SetItemText(itemCount, 1, str_lm);

				m_listCtrl.SetItemText(itemCount, 2, (l_resFile->file_exist) ? L"O" : L"X");

				m_listCtrl.SetItemText(itemCount, 3, (r_resFile->file_exist) ? L"O" : L"X");


				if ( l_resFile->unit->parent)
				{
					const i3::wstring& ps_path = l_resFile->unit->parent->parent->packscript_path;
					i3::extract_filename(ps_path, wstrTemp);
					m_listCtrl.SetItemText(itemCount, 4, wstrTemp.c_str());					
				}

				const RSCFileResultMapElement* elem = FileCheck_PackScriptMgr::i()->FindRSCFileMapElement(strFile);

				if (elem && !elem->contents_result_list_by_path[0].empty() )
				{
					const RSCFileContentsResult* contents =	elem->contents_result_list_by_path[0][0];

					const i3::rc_string& rscFileName = contents->parent->rsc_file_path;

					i3::mb_to_utf16(rscFileName, wstrTemp);
					m_listCtrl.SetItemText(itemCount, 5, wstrTemp.c_str());
				}

				if ( r_resFile->unit->parent)
				{
					const i3::wstring& ps_path = r_resFile->unit->parent->parent->packscript_path;
					i3::extract_filename(ps_path, wstrTemp);
					m_listCtrl.SetItemText(itemCount, 6, wstrTemp.c_str());					
				}

				if (elem && !elem->contents_result_list_by_path[1].empty() )
				{
					const RSCFileContentsResult* contents =	elem->contents_result_list_by_path[1][0];

					const i3::rc_string& rscFileName = contents->parent->rsc_file_path;

					i3::mb_to_utf16(rscFileName, wstrTemp);
					m_listCtrl.SetItemText(itemCount, 7, wstrTemp.c_str());
				}


				++itemCount;
			}
		}
	}

	m_listCtrl.SetRedraw(TRUE);

}
