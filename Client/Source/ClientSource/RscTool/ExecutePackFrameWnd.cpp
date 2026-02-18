// ExecutePackFrameWnd.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "ExecutePackFrameWnd.h"



#include "PackQueue.h"
#include "RscTool.h"

#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/string/ext/num_to_str.h"
#include "i3Base/string/ext/format_string.h"
#include "PathMgr.h"

#include "TextProgressCtrl.h"

#include "MainFrm.h"

namespace
{
	void GetModeString(PackMode mode, i3::wstring& res)
	{
		switch(mode)
		{
		case PACKMODE_MEDIA_TO_QA_COPY:
			res = L"Media -> QA (Copy)";
			break;

		case PACKMODE_QA_TO_LIVE_PACK:
			res = L"QA -> Live (Pack)";
			break;

		default:
			res.clear();
			break;
		}

	}

	// 
	struct ProgressInfo_RscFile
	{
		i3::wstring				strRscFile;
		bool					is_enc;
		bool					is_success;
	};

	typedef i3::shared_ptr<ProgressInfo_RscFile>  ProgressInfo_RscFilePtr;


	struct ProgressInfo_CreatePack
	{
		i3::wstring				strPackFileName;
		i3::vector<ProgressInfo_RscFilePtr>	strRscFileList;
		int numCurrRscFile;
		int totRscFile;
		int successRscFileCount;
		PackMode mode;
	};

	typedef i3::shared_ptr<ProgressInfo_CreatePack>		ProgressInfo_CreatePackPtr;


	struct PackFileList_CustomDrawCallback :  i3MFCListCtrl_CustomDrawCallback
	{
		ExecutePackFrameWnd* m_owner;
		PackFileList_CustomDrawCallback(ExecutePackFrameWnd* owner) : m_owner(owner) {}


		CTextProgressCtrl*		GetProgressCtrl(i3MFCListCtrl_Custom* pCtrl, int ListItemIdx)
		{
			int num_idx = pCtrl->GetItemCount();
			int first = pCtrl->GetTopIndex();
			int last =  first + pCtrl->GetCountPerPage() + 1;
			if ( last > num_idx ) 
				last = num_idx;
			const int num_vis_item = last - first;
			int progress_idx = ListItemIdx - first;

			if (progress_idx < 0 || progress_idx >= num_vis_item )
				return NULL;
			if (progress_idx >= (int)m_progressList.size() )
				return NULL;

			return m_progressList[progress_idx];
		}


		virtual ~PackFileList_CustomDrawCallback() 
		{
			i3::cu::for_each_delete(m_progressList);
		}

		virtual void OnPaint(i3MFCListCtrl_Custom* pCtrl, int first, int last) 
		{
			const int num_vis_item = last - first;

			if ( num_vis_item > (int)m_progressList.size() )
				m_progressList.insert(m_progressList.end(),  
				num_vis_item - (int)m_progressList.size(), NULL);

			const size_t num_list = m_progressList.size();

			int count_ctrl = 0;

			for (int i = first ; i < last ; ++i, ++count_ctrl)
			{
				CTextProgressCtrl* progress_ctrl = m_progressList[count_ctrl];

				ProgressInfo_CreatePack* curr_obj = (ProgressInfo_CreatePack*)pCtrl->GetItemData(i);

				if (progress_ctrl)
				{
					if (curr_obj->totRscFile > 0)
					{
						CRect rc;
						progress_ctrl->GetWindowRect(&rc);
						pCtrl->ScreenToClient(&rc);		// button rect..
						CRect rc_new;
						pCtrl->GetSubItemRect(i, 1, LVIR_BOUNDS, rc_new);
						
						if ( rc != rc_new )					// 가시영역안에서의 위치 이동인 경우..
						{									// 이전 이미지 삭제가 안되는 경우가 있어 지우고 렌더링 해야함..
							if ( progress_ctrl->IsWindowVisible() )
								progress_ctrl->ShowWindow(SW_HIDE);

							progress_ctrl->MoveWindow(rc_new, FALSE); 
							progress_ctrl->ShowWindow(SW_SHOW);
						}
						else
						{
							progress_ctrl->ShowWindow(SW_SHOW);
						}
					}
					else
					{
						progress_ctrl->ShowWindow(SW_HIDE);
					}
				}
				else
				{
					// 만든다..
					CTextProgressCtrl* new_ctrl = new CTextProgressCtrl();

					CRect rc_new;
					pCtrl->GetSubItemRect(i, 1, LVIR_BOUNDS, rc_new);

					new_ctrl->Create(WS_VISIBLE|WS_CHILD, rc_new, pCtrl, i);

					//	SetWindowTheme(new_ctrl->GetSafeHwnd(), L"explorer", 0);
					m_progressList[count_ctrl] = new_ctrl;
					progress_ctrl = new_ctrl;

					if (curr_obj->totRscFile == 0)
					{
						progress_ctrl->ShowWindow(SW_HIDE);
					}
				}

				if (curr_obj->totRscFile > 0)
				{
					progress_ctrl->SetRange( 0, curr_obj->totRscFile);
					progress_ctrl->SetPos(int(curr_obj->numCurrRscFile));

					i3::stack_wstring wstrTemp;
					i3::sprintf(wstrTemp, L" ( %d / %d )", curr_obj->numCurrRscFile, curr_obj->totRscFile);

					progress_ctrl->SetWindowText(wstrTemp.c_str() );
				}

				progress_ctrl->RedrawWindow();
			}

			for (size_t i = count_ctrl ; i < num_list ; ++i )
			{
				CTextProgressCtrl* progress_ctrl = m_progressList[i];
				if (progress_ctrl && progress_ctrl->IsWindowVisible() )
					progress_ctrl->ShowWindow(SW_HIDE);
			}

		}

		i3::vector<CTextProgressCtrl*>		m_progressList;

	};

	struct PackFileList_Callback : i3MFCListCtrlCallback
	{
		ExecutePackFrameWnd* owner;


		PackFileList_Callback(ExecutePackFrameWnd* owner) : owner(owner) {}

		virtual void OnSelected(i3MFCListCtrl* Ctrl, int index) 
		{
			ProgressInfo_CreatePack* info	= (ProgressInfo_CreatePack*)Ctrl->GetItemData(index);

			i3MFCListCtrl* contents_listctrl = owner->GetPackContentsListCtrl();

			contents_listctrl->SetRedraw(FALSE);

			contents_listctrl->DeleteAllItems();

			const size_t num = info->strRscFileList.size();

			if (num > 0)
			{
				contents_listctrl->SetItemCount(num); // info->totRscFile);
				const size_t num_m_1 = num - 1;

				for ( size_t i = 0 ; i < num_m_1 ; ++i )
				{
					ProgressInfo_RscFile* file_info = info->strRscFileList[i].get();
					contents_listctrl->AddItem(file_info->strRscFile.c_str(), file_info);
								
					const wchar_t* str_result = (file_info->is_success) ? L"Sucess" : L"Failed";
					contents_listctrl->SetItemText(i, 1, str_result);

					if (file_info->is_enc)
						contents_listctrl->SetItemText(i, 2, L"ENC");

				}
				
				ProgressInfo_RscFile* file_info = info->strRscFileList[num_m_1].get();
				contents_listctrl->AddItem(file_info->strRscFile.c_str(), file_info);		// Done 표시..
			}

			contents_listctrl->SetRedraw(TRUE);
		}

	};

	struct PackQueue_ProgressCallback_Concrete : PackQueue_ProgressCallback
	{
		ExecutePackFrameWnd* m_owner;
		i3::vector<ProgressInfo_CreatePackPtr>	m_vecProgressInfo;

		PackQueue_ProgressCallback_Concrete(ExecutePackFrameWnd* owner) : m_owner(owner) {}

		virtual ~PackQueue_ProgressCallback_Concrete() 
		{

		}

		virtual void OnPack_Start( const i3::string& strPackFilePath, int total_file, PackMode mode) 
		{
			OnPackOrCopy_Start(strPackFilePath, total_file, mode, L"PACK");
		}

		virtual void OnProgress_BeforePack( const i3::string& strResFilePath, bool is_enc, int curr_file_count, int total_file) 
		{
			OnProgress_BeforePackOrCopy(strResFilePath, is_enc); 
		}

		virtual void OnProgress_AfterPack(const i3::string& strResFilePath, bool is_enc, bool is_success, int curr_file_count, int success_count, int total_file) 
		{
			OnProgress_AfterPackOrCopy(is_success, curr_file_count, success_count, total_file);
		}

		virtual void OnPack_End(const i3::string& strPackFilePath, int curr_file_count, int success_count, int total_file, PackMode mode) 
		{
			// ProgressInfo_CreatePack* curr_obj = m_vecProgressInfo.back().get();
			// 가장 최근 리스트 아이템의 작업 종료..(현재는 특별한 것은 없다..)
			OnPackOrCopy_End(success_count, total_file, true);
		}

		virtual void OnCopy_Start(const i3::string& strPackFilePath, int total_file, PackMode mode) 
		{
			OnPackOrCopy_Start(strPackFilePath, total_file, mode, L"COPY");
		}

		virtual void OnProgress_BeforeCopy( const i3::string& strResFilePath, int curr_file_count, int total_file) 
		{
			OnProgress_BeforePackOrCopy(strResFilePath, false); 
		}

		virtual void OnProgress_AfterCopy( const i3::string& strResFilePath, bool is_success, int curr_file_count, int success_count, int total_file) 
		{
			OnProgress_AfterPackOrCopy(is_success, curr_file_count, success_count, total_file);
		}

		virtual void OnCopy_End(const i3::string& strPackFilePath, int curr_file_count, int success_count, int total_file, PackMode mode) 
		{
			OnPackOrCopy_End(success_count, total_file, false);
		}

		void	OnPackOrCopy_Start(const i3::string& strPackFilePath, int total_file, PackMode mode, 
			const wchar_t* pack_or_copy)
		{
			ProgressInfo_CreatePackPtr new_obj = i3::make_shared<ProgressInfo_CreatePack>();

			i3::mb_to_utf16(strPackFilePath, new_obj->strPackFileName);
			i3::make_unix_path(new_obj->strPackFileName);

			new_obj->totRscFile = total_file;
			new_obj->numCurrRscFile = 0;
			new_obj->successRscFileCount = 0;
			new_obj->mode = mode;


			i3MFCListCtrl_Custom* ctrl = m_owner->GetPackFileListCtrl();

			BOOL last_index_visible = ctrl->IsLastItemIndexVisible();

			ctrl->AddItem(new_obj->strPackFileName.c_str(), new_obj.get());	

			i3::wstring str_mode;	GetModeString(mode, str_mode);
			int last_idx = ctrl->GetItemCount()-1;

			ctrl->SetItemText(last_idx, 2, str_mode.c_str());
			ctrl->SetItemText(last_idx, 3, pack_or_copy);

			int num_fail = 0;
			i3::stack_wstring wstrNum;
			i3::integral_to_str(num_fail, wstrNum);
			ctrl->SetItemText(last_idx, 4, wstrNum.c_str());

			if (last_index_visible )
			{
				ctrl->EnsureVisible(last_idx, FALSE);
				//	ctrl->UnselectAll();
				ctrl->SetItemState(last_idx, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

				// ctrl->RedrawWindow();
				//				ctrl->RedrawItems(last_idx, last_idx);
				//				ctrl->UpdateWindow();
			}

			m_vecProgressInfo.push_back(new_obj);	

		}

		virtual void OnEnterIdle() 
		{
			ProgressInfo_CreatePackPtr new_obj = i3::make_shared<ProgressInfo_CreatePack>();

			new_obj->strPackFileName = L"Process Done. Idle State.";
		
			new_obj->totRscFile = 0;
			new_obj->numCurrRscFile = 0;
			new_obj->successRscFileCount = 0;
			new_obj->mode = PackMode(0);

			i3MFCListCtrl_Custom* ctrl = m_owner->GetPackFileListCtrl();

			BOOL last_index_visible = ctrl->IsLastItemIndexVisible();

			ctrl->AddItem(new_obj->strPackFileName.c_str(), new_obj.get());	

			int last_idx = ctrl->GetItemCount()-1;

			ctrl->SetItemText(last_idx, 2, L"");
			ctrl->SetItemText(last_idx, 3, L"");
			ctrl->SetItemText(last_idx, 4, L"");

			if (last_index_visible )
			{
				ctrl->EnsureVisible(last_idx, FALSE);
			//	ctrl->SetItemState(last_idx, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			}

			m_vecProgressInfo.push_back(new_obj);	
		}

		void OnProgress_BeforePackOrCopy(const i3::string& strResFilePath, bool is_enc) 
		{
			ProgressInfo_CreatePack* curr_obj = m_vecProgressInfo.back().get();

			ProgressInfo_RscFilePtr info_rscfile = i3::make_shared<ProgressInfo_RscFile>();

			curr_obj->strRscFileList.push_back( info_rscfile );		
			++curr_obj->numCurrRscFile;


			i3::wstring wstrResFilePath;
			i3::mb_to_utf16(strResFilePath, wstrResFilePath);

			info_rscfile->strRscFile = wstrResFilePath;

			i3::make_unix_path(info_rscfile->strRscFile);

			info_rscfile->is_enc = is_enc;
			info_rscfile->is_success = false;



			i3MFCListCtrl* filelist_ctrl = m_owner->GetPackFileListCtrl();

			int last_idx = int(m_vecProgressInfo.size()) - 1;

			if (filelist_ctrl->GetItemState(last_idx, LVIS_FOCUSED) & LVIS_FOCUSED )
			{
				i3MFCListCtrl* contents_listctrl = m_owner->GetPackContentsListCtrl();

				BOOL last_visible = contents_listctrl->IsLastItemIndexVisible();

				contents_listctrl->AddItem( info_rscfile->strRscFile.c_str(), info_rscfile.get());
				int last_contents_idx = contents_listctrl->GetItemCount()-1;

				contents_listctrl->SetItemText(last_contents_idx, 1, L"Doing");

				if (info_rscfile->is_enc)
					contents_listctrl->SetItemText(last_contents_idx, 2, L"ENC");

				if (last_visible)
				{
					contents_listctrl->EnsureVisible(last_contents_idx, FALSE);
				}
			}
			// 리스트 갱신해야한다.. (인덱스가 일치하는 것을 이용한다..)
		}

		void OnProgress_AfterPackOrCopy(bool is_success, int curr_file_count, int success_count, int total_file)
		{		
			ProgressInfo_CreatePack* curr_obj = m_vecProgressInfo.back().get();
			curr_obj->successRscFileCount = success_count;

			ProgressInfo_RscFile* rsc_obj =	curr_obj->strRscFileList.back().get();
			rsc_obj->is_success = is_success;

			// 리스트 갱신해야한다.. (인덱스가 일치하는 것을 이용한다..)
			i3MFCListCtrl_Custom* ctrl = m_owner->GetPackFileListCtrl();
			PackFileList_CustomDrawCallback* ctrl_callback = (PackFileList_CustomDrawCallback*)ctrl->GetCallback_CustomDraw();
			CTextProgressCtrl* progress = ctrl_callback->GetProgressCtrl(ctrl, m_vecProgressInfo.size() - 1);
			if (progress)
			{
				progress->SetRange(0, total_file);
				progress->SetPos(curr_file_count);

				i3::stack_wstring wstrTemp;
				i3::sprintf(wstrTemp, L" ( %d / %d )", curr_file_count, total_file);

				progress->SetWindowText(wstrTemp.c_str() );

				if (ctrl->IsLastItemIndexVisible() )
				{
					ctrl->EnsureVisible(ctrl->GetItemCount()-1, FALSE);
				}

				// progress->RedrawWindow();

				// ctrl->RedrawWindow();
			}

			int last_idx = int(m_vecProgressInfo.size()) - 1;

			int num_fail = curr_file_count - success_count;
			i3::stack_wstring wstrNum;
			i3::integral_to_str(num_fail, wstrNum);
			ctrl->SetItemText(last_idx, 4, wstrNum.c_str());
			//			ctrl->RedrawItems(last_idx, last_idx);
			//			ctrl->UpdateWindow();		// UpdateWindow()를 쓰면 멀티쓰레드 혜택이 날아간다..(쓰지말것)


			if (ctrl->GetItemState(last_idx, LVIS_FOCUSED) & LVIS_FOCUSED )
			{
				i3MFCListCtrl* contents_listctrl = m_owner->GetPackContentsListCtrl();

				BOOL last_visible = contents_listctrl->IsLastItemIndexVisible();

				int last_contents_idx = contents_listctrl->GetItemCount()-1;

				const wchar_t* str_result = (rsc_obj->is_success) ? L"Sucess" : L"Failed";

				contents_listctrl->SetItemText(last_contents_idx, 1, str_result);

				if (last_visible)
				{
					contents_listctrl->EnsureVisible(last_contents_idx, FALSE);
				}
			}
		}

		void OnPackOrCopy_End(int success_count, int total_file, bool is_pack)
		{
			ProgressInfo_CreatePack* curr_obj = m_vecProgressInfo.back().get();

			ProgressInfo_RscFilePtr info_rscfile = i3::make_shared<ProgressInfo_RscFile>();

			curr_obj->strRscFileList.push_back( info_rscfile );

			i3::const_wchar_range rng = (is_pack) ? i3::as_literal(L"Pack") : i3::as_literal(L"Copy");

			i3::wstring wstrResFilePath = i3::format_string(L"%s Done. (%d of %d)", rng, success_count, total_file);
		
			info_rscfile->strRscFile = wstrResFilePath;

			info_rscfile->is_enc = false;
			info_rscfile->is_success = false;

			i3MFCListCtrl* filelist_ctrl = m_owner->GetPackFileListCtrl();

			int last_idx = int(m_vecProgressInfo.size()) - 1;

			if (filelist_ctrl->GetItemState(last_idx, LVIS_FOCUSED) & LVIS_FOCUSED )
			{
				i3MFCListCtrl* contents_listctrl = m_owner->GetPackContentsListCtrl();

				BOOL last_visible = contents_listctrl->IsLastItemIndexVisible();

				contents_listctrl->AddItem( info_rscfile->strRscFile.c_str(), info_rscfile.get());
				int last_contents_idx = contents_listctrl->GetItemCount()-1;

				contents_listctrl->SetItemText(last_contents_idx, 1, L"");
				contents_listctrl->SetItemText(last_contents_idx, 2, L"");

				if (last_visible)
				{
					contents_listctrl->EnsureVisible(last_contents_idx, FALSE);
				}
			}

		}


	};

}

// ExecutePackFrameWnd

IMPLEMENT_DYNCREATE(ExecutePackFrameWnd, CFrameWndEx)

ExecutePackFrameWnd::ExecutePackFrameWnd()
{
	m_packfile_list.SetCallback_CustomDraw( new PackFileList_CustomDrawCallback(this));
	m_packfile_list.SetCallback(new PackFileList_Callback(this));

	m_progress_callback = new PackQueue_ProgressCallback_Concrete(this);

	((CRscToolApp*)AfxGetApp())->SetProgressCallback(m_progress_callback);
}

ExecutePackFrameWnd::~ExecutePackFrameWnd()
{

	delete m_packfile_list.GetCallback();
	delete m_packfile_list.GetCallback_CustomDraw();

	((CRscToolApp*)AfxGetApp())->SetProgressCallback(NULL);
	delete m_progress_callback;

}


BEGIN_MESSAGE_MAP(ExecutePackFrameWnd, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// ExecutePackFrameWnd 메시지 처리기입니다.

int ExecutePackFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_packfile_list.CreateEx(this, -1, LVS_NOSORTHEADER|LVS_SINGLESEL);
	m_packfile_list.InsertColumn(0, L"PackFile Name", LVCFMT_LEFT, 450);
	m_packfile_list.InsertColumn(1, L"Progress", LVCFMT_CENTER, 200);
	m_packfile_list.InsertColumn(2, L"Mode", LVCFMT_LEFT, 150);
	m_packfile_list.InsertColumn(3, L"Action" , LVCFMT_RIGHT, 50);
	m_packfile_list.InsertColumn(4, L"Fail", LVCFMT_RIGHT, 50);

	m_packcontents_list.CreateEx(this, -1, LVS_NOSORTHEADER);
	m_packcontents_list.InsertColumn(0, L"Contents Source Filename", LVCFMT_LEFT, 500);
	m_packcontents_list.InsertColumn(1, L"Result", LVCFMT_CENTER, 100);
	m_packcontents_list.InsertColumn(2, L"ENC", LVCFMT_RIGHT, 100);

	m_packfile_list.RedrawWindow();
	m_packcontents_list.RedrawWindow();

	return 0;
}

void ExecutePackFrameWnd::OnSize(UINT nType, int cx, int cy)
{
	CFrameWndEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (this->GetSafeHwnd() == NULL)
		return;

	// 가로는 꽉채우고 세로는 1/3을 쓴다..

	if (m_packfile_list.GetSafeHwnd() )
	{
		CRect rcDlg;	this->GetClientRect(rcDlg);
		CRect rcPackFileListCtrl = rcDlg;
		rcPackFileListCtrl.bottom = rcDlg.top + (rcDlg.bottom - rcDlg.top) / 3;

		m_packfile_list.SetWindowPos(NULL, rcPackFileListCtrl.left, 
			rcPackFileListCtrl.top, rcPackFileListCtrl.Width(), 
			rcPackFileListCtrl.Height(), SWP_NOZORDER|SWP_NOACTIVATE);	

		CRect rcPackContentsListCtrl = rcDlg;
		rcPackContentsListCtrl.top = rcPackFileListCtrl.bottom;

		m_packcontents_list.SetWindowPos(NULL, rcPackContentsListCtrl.left, 
			rcPackContentsListCtrl.top, rcPackContentsListCtrl.Width(), 
			rcPackContentsListCtrl.Height(), SWP_NOZORDER|SWP_NOACTIVATE);	
	}


}

void ExecutePackFrameWnd::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	GetMainFrame()->DestroyExecuteFrameWndFromList(this);

	CFrameWndEx::PostNcDestroy();
}

BOOL ExecutePackFrameWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (!CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
/*
	CRect rcMainWnd;
	rcMainWnd.left = cs.x;
	rcMainWnd.top = cs.y;
	rcMainWnd.right = rcMainWnd.left + cs.cx;
	rcMainWnd.bottom = rcMainWnd.top + cs.cy;

	CRect rcDlg = rcMainWnd;
	rcDlg.DeflateRect(400, 50);

	cs.x = rcDlg.left;
	cs.y = rcDlg.top;
	cs.cx = rcDlg.Width();
	cs.cy = rcDlg.Height();
*/
	return TRUE;
}


void	ExecutePackFrameWnd::ClearPackList()
{

	m_packfile_list.DeleteAllItems();
	m_packcontents_list.DeleteAllItems();

	PackQueue_ProgressCallback_Concrete* cb = 
		static_cast<PackQueue_ProgressCallback_Concrete*>(m_progress_callback);

	cb->m_vecProgressInfo.clear();

}
void	ExecutePackFrameWnd::AddPackScriptFile(const i3::wstring& strFilePath, PackMode mode)
{
	// 음...너무 간단한듯...		

	PackQueue::i()->push_save_function(strFilePath, mode);

}

void	ExecutePackFrameWnd::AddNormalFileList(const i3::vector<i3::rc_string>& resFileList, const i3::string& item_name, 
								   PackMode mode)
{
	PackQueue::i()->push_save_function(resFileList, item_name, mode);	
}

void	ExecutePackFrameWnd::AddRscTask(const i3::rc_string& rel_rsc_path, const i3::vector<i3::rc_string>& rscnode_pathlist)
{
	PackQueue::i()->push_save_function(rel_rsc_path, rscnode_pathlist);
}


BOOL ExecutePackFrameWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

//	return CFrameWndEx::OnEraseBkgnd(pDC);
	return TRUE;
}

