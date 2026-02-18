// PefTxtDiffDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "PefTxtDiffDlg.h"
#include "PathMgr.h"
#include "PathMode.h"
#include "i3Base/string/system/traverse_file.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/format_string.h"
#include "FindPefDiff.h"

namespace
{
	struct PathListCtrlCallback : i3MFCListCtrlCallback
	{
		PefTxtDiffDlg* owner;	int l_r;
	
		PathListCtrlCallback(PefTxtDiffDlg* owner, int l_r) : owner(owner), l_r(l_r) {}
		
		virtual void OnSelected(i3MFCListCtrl* Ctrl, int index) 
		{
			PathMode mode = PathMode(Ctrl->GetItemData(index));
			switch(mode)
			{
			case PATHMODE_MEDIA:
				owner->UpdateFileList_MediaQAPath(PathMgr::i()->GetMainMediaPath(), l_r);
			break;

			case PATHMODE_QA: 
				owner->UpdateFileList_MediaQAPath(PathMgr::i()->GetQAPath(), l_r);
			break;
			
			case PATHMODE_LIVE:
				owner->UpdateFileList_LivePath(PathMgr::i()->GetLivePath(), l_r);
			break;
			}
		}
	};

	struct FileListCtrlCallback : i3MFCListCtrlCallback
	{
		PefTxtDiffDlg* owner;	int l_r;

		FileListCtrlCallback(PefTxtDiffDlg* owner, int l_r) : owner(owner), l_r(l_r) {}

		virtual void OnSelected(i3MFCListCtrl* Ctrl, int index) 
		{
			owner->UpdateLogList(index, l_r);
		}

	};
	
	struct LogListCtrlCallback : i3MFCListCtrlCallback
	{
		PefTxtDiffDlg* owner;	int l_r;
		LogListCtrlCallback(PefTxtDiffDlg* owner, int l_r) : owner(owner), l_r(l_r) {}

		virtual void OnSelected(i3MFCListCtrl* Ctrl, int index) 
		{
			owner->UpdateLogText(index, l_r);
		}
	};
	
	struct NextLogButtonCallback : i3MFCButtonCallback
	{
		PefTxtDiffDlg* owner;
		NextLogButtonCallback(PefTxtDiffDlg* owner) : owner(owner) {}
		virtual void OnClick(i3MFCButton* ctrl) 
		{
			owner->UpdateNextLogList(0);
			owner->UpdateNextLogList(1);
		}
	};

}



// PefTxtDiffDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(PefTxtDiffDlg, CDialog)

PefTxtDiffDlg::PefTxtDiffDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PefTxtDiffDlg::IDD, pParent), m_curr_file_format(PEFTXT_DIFF_FILE_FORMAT_PEF), 
	m_init_path_mode(PATHMODE_LIVE)
{
	m_PathCtrl[0].SetCallback(new PathListCtrlCallback(this, 0));
	m_PathCtrl[1].SetCallback(new PathListCtrlCallback(this, 1));
	m_FileListCtrl[0].SetCallback(new FileListCtrlCallback(this, 0));
	m_FileListCtrl[1].SetCallback(new FileListCtrlCallback(this, 1));
	m_LogListCtrl[0].SetCallback(new LogListCtrlCallback(this, 0));
	m_LogListCtrl[1].SetCallback(new LogListCtrlCallback(this, 1));
	m_button_NextLog.SetCallback(new NextLogButtonCallback(this));
}

PefTxtDiffDlg::~PefTxtDiffDlg()
{
	delete m_button_NextLog.GetCallback();
	delete m_LogListCtrl[1].GetCallback();
	delete m_LogListCtrl[0].GetCallback();
	delete m_FileListCtrl[1].GetCallback();
	delete m_FileListCtrl[0].GetCallback();
	delete m_PathCtrl[1].GetCallback();
	delete m_PathCtrl[0].GetCallback();
}

void PefTxtDiffDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, ID_BUTTON_NEXTLOGLIST, m_button_NextLog);

	DDX_Control(pDX, IDC_LIST_PATH_LEFT, m_PathCtrl[0]);
	DDX_Control(pDX, IDC_LIST_FILES_LEFT, m_FileListCtrl[0]);
	DDX_Control(pDX, IDC_LIST_LOGS_LEFT, m_LogListCtrl[0]);
	DDX_Control(pDX, IDC_EDIT_LOGDATA_LEFT, m_LogDataCtrl[0]);

	DDX_Control(pDX, IDC_LIST_PATH_RIGHT, m_PathCtrl[1]);
	DDX_Control(pDX, IDC_LIST_FILES_RIGHT, m_FileListCtrl[1]);
	DDX_Control(pDX, IDC_LIST_LOGS_RIGHT, m_LogListCtrl[1]);
	DDX_Control(pDX, IDC_EDIT_LOGDATA_RIGHT, m_LogDataCtrl[1]);

	::SetWindowTheme(m_PathCtrl[0].GetSafeHwnd(), L"explorer", 0);
	::SetWindowTheme(m_PathCtrl[1].GetSafeHwnd(), L"explorer", 0);
	::SetWindowTheme(m_FileListCtrl[0].GetSafeHwnd(), L"explorer", 0);
	::SetWindowTheme(m_FileListCtrl[1].GetSafeHwnd(), L"explorer", 0);
	::SetWindowTheme(m_LogListCtrl[0].GetSafeHwnd(), L"explorer", 0);
	::SetWindowTheme(m_LogListCtrl[1].GetSafeHwnd(), L"explorer", 0);
	
	m_PathCtrl[0].SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER );
	m_PathCtrl[1].SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER );

	m_PathCtrl[0].InsertColumn(0,L"");	m_PathCtrl[1].InsertColumn(0,L"");

	m_FileListCtrl[0].SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER );
	m_FileListCtrl[1].SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER );
	m_FileListCtrl[0].InsertColumn(0,L"");	m_FileListCtrl[1].InsertColumn(0,L"");


	m_LogListCtrl[0].SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER );
	m_LogListCtrl[1].SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER );

	m_LogListCtrl[0].InsertColumn(0, L"Revision", LVCFMT_RIGHT, 80);
	m_LogListCtrl[1].InsertColumn(0, L"Revision", LVCFMT_RIGHT, 80);
	m_LogListCtrl[0].InsertColumn(1, L"Writer", LVCFMT_CENTER, 100);
	m_LogListCtrl[1].InsertColumn(1, L"Writer", LVCFMT_CENTER, 100);
	m_LogListCtrl[0].InsertColumn(2, L"Date", LVCFMT_LEFT, 300);
	m_LogListCtrl[1].InsertColumn(2, L"Date", LVCFMT_LEFT, 300);


	CRect rc;	
	m_PathCtrl[0].GetClientRect(rc);	m_PathCtrl[0].SetColumnWidth(0, rc.right);
	m_PathCtrl[1].GetClientRect(rc);	m_PathCtrl[1].SetColumnWidth(0, rc.right);
	m_FileListCtrl[0].GetClientRect(rc);	m_FileListCtrl[0].SetColumnWidth(0, rc.right - GetSystemMetrics(SM_CXVSCROLL));
	m_FileListCtrl[1].GetClientRect(rc);	m_FileListCtrl[1].SetColumnWidth(0, rc.right - GetSystemMetrics(SM_CXVSCROLL));


	m_LogDataCtrl[0].SetReadOnly(TRUE);
	m_LogDataCtrl[1].SetReadOnly(TRUE);


}


BEGIN_MESSAGE_MAP(PefTxtDiffDlg, CDialog)
END_MESSAGE_MAP()


// PefTxtDiffDlg 메시지 처리기입니다.

void PefTxtDiffDlg::InitParam(PathMode pm, PefTxtDiffFileFormat fmt, const i3::rc_wstring& filename)
{
	m_curr_file_format = fmt;
	m_init_path_mode = pm;
	m_selected_filename = filename;
}



BOOL PefTxtDiffDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	PefTxtDiffDlg::InitDialog_PathCtrls();
	PefTxtDiffDlg::InitDialog_FileListCtrls();
	PefTxtDiffDlg::InitDialog_LogListCtrls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void	PefTxtDiffDlg::InitDialog_PathCtrls()
{
	if ( PathMgr::i()->IsMediaPathExist() )
	{
		m_PathCtrl[0].AddItem(L"Media Path", (void*)PATHMODE_MEDIA);
		m_PathCtrl[1].AddItem(L"Media Path", (void*)PATHMODE_MEDIA);
	}

	if ( PathMgr::i()->IsQAPathExist() )
	{
		m_PathCtrl[0].AddItem(L"QA Path", (void*)PATHMODE_QA);
		m_PathCtrl[1].AddItem(L"QA Path", (void*)PATHMODE_QA);
	}

	if ( PathMgr::i()->IsLivePathExist() )
	{
		m_PathCtrl[0].AddItem(L"Live Path", (void*) PATHMODE_LIVE);
		m_PathCtrl[1].AddItem(L"Live Path", (void*) PATHMODE_LIVE);
	}


	LVFINDINFO info;
	info.flags = LVFI_PARAM;
	info.lParam = (LPARAM) m_init_path_mode;

	int idx = m_PathCtrl[0].FindItem(&info);
	if (idx != -1)
	{
		m_PathCtrl[0].SetItemState(idx, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_PathCtrl[1].SetItemState(idx, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}
}

void	PefTxtDiffDlg::InitDialog_FileListCtrls()
{
	LVFINDINFO info;
	info.flags = LVFI_STRING;
	info.psz = m_selected_filename.c_str();

	int idx = m_FileListCtrl[0].FindItem(&info);

	if (idx != -1)
	{
		m_FileListCtrl[0].SetItemState(idx, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_FileListCtrl[1].SetItemState(idx, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

		m_FileListCtrl[0].EnsureVisible(idx, FALSE);
		m_FileListCtrl[1].EnsureVisible(idx, FALSE);
	}
	
}

void	PefTxtDiffDlg::InitDialog_LogListCtrls()
{
	// 왼쪽은 HEAD rev 오른쪽은 로컬이 되도록 초기화..
	int item_count = m_LogListCtrl[0].GetItemCount();

	if ( item_count > 0)
	{
		if (item_count > 1)
			m_LogListCtrl[0].SetItemState(1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		else
			m_LogListCtrl[0].SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		
		m_LogListCtrl[1].SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}
}


void PefTxtDiffDlg::UpdateFileList_MediaQAPath(const i3::wstring& base_path, int l_r)
{

	m_FileList[l_r].clear();

	struct filecallback : i3::traverse_file_callback_wc
	{
		const i3::wstring&		rel_base_path;
		i3::vector<i3::rc_string>& rel_path_list;
		i3::vector<i3::rc_wstring>& filename_list;
		
		filecallback(const i3::wstring& rel_base_path,
			i3::vector<i3::rc_string>& rel_path_list, 
			i3::vector<i3::rc_wstring>& filename_list) : rel_base_path(rel_base_path),
		rel_path_list(rel_path_list), filename_list(filename_list) {}

		virtual void on_file(const WIN32_FIND_DATA& fd,  const i3::rc_wstring& base_path, const i3::rc_wstring& subdir_path) 
		{
			i3::rc_wstring wstr_rel_path = rel_base_path; //L"config/";
			wstr_rel_path += fd.cFileName;
			i3::rc_string str_rel_path;
			i3::utf16_to_mb(wstr_rel_path, str_rel_path);
			rel_path_list.push_back(str_rel_path);
			filename_list.push_back(fd.cFileName);
		}

		virtual bool on_dir(const WIN32_FIND_DATA& fd,  const i3::rc_wstring& base_path, const i3::rc_wstring& subdir_path) { return false; }
	};
	
	i3::vector<i3::rc_wstring>	filename_list;

	i3::wstring rel_base_path;
	i3::wstring path_to_find;

	if ( m_curr_file_format == PEFTXT_DIFF_FILE_FORMAT_PEF)
	{
		rel_base_path = L"config/";
		path_to_find = base_path;
		path_to_find += L"/config/*.pef";
	}
	else
	{
		rel_base_path = L"Script/String/";
		rel_base_path += PathMgr::i()->GetNation();		rel_base_path += L'/';
		path_to_find = base_path;
		path_to_find += L'/';
		path_to_find += rel_base_path;
		path_to_find += L"*.txt";
	}

	filecallback cb(rel_base_path, m_FileList[l_r], filename_list);
	traverse_file(path_to_find, &cb, false);
	
	
	m_FileListCtrl[l_r].SetRedraw(FALSE);
	m_FileListCtrl[l_r].DeleteAllItems();

	const size_t num_file = filename_list.size();

	for (size_t i = 0 ; i < num_file ; ++i)
	{
		m_FileListCtrl[l_r].InsertItem(i, filename_list[i].c_str());
	}

	m_FileListCtrl[l_r].SetRedraw(TRUE);
	m_FileListCtrl[l_r].RedrawWindow();

	LVFINDINFO lvfi;
	lvfi.flags = LVFI_STRING;
	lvfi.psz = m_selected_filename.c_str();
	int found_idx = m_FileListCtrl[l_r].FindItem(&lvfi);

	if (found_idx != -1)
	{
		m_FileListCtrl[l_r].SetItemState(found_idx, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_FileListCtrl[l_r].EnsureVisible(found_idx, FALSE);
	}

}

namespace
{
	// 재귀처리 하지 않음..

	void GatherInPackFileNode(i3PackNode* n, const i3::string& parent_path,
		i3::vector<i3::rc_string>& rel_path_list,
		i3::vector<i3::rc_wstring>& filename_list, const i3::const_char_range& rng_ext)
	{
		
		i3::string curr_path = parent_path;
//		if ( n->getParent() )
//		{
//			if (!curr_path.empty())
//				curr_path += '/';
//
//			const i3::rc_string& name = n->GetNameString();
//			curr_path.append(name.begin(), name.end());
//		}

		const INT32 num_file = n->getFileCount();

		i3::string strext;
	
		for (INT32 i = 0 ; i < num_file ; ++i )
		{
			I3_PACK_FILE_REF *	info = n->getFile(i);

			i3::const_char_range rng = i3::as_literal(info->m_szName);

			i3::extract_fileext(rng, strext);

			if ( i3::generic_is_iequal(strext, rng_ext) )
			{
				i3::string	rel_path = curr_path;
				rel_path += '/';
				rel_path.append(rng.begin(), rng.end());
				rel_path_list.push_back(rel_path);

				i3::wstring wstr_filename;
				i3::mb_to_utf16( rng, wstr_filename);
				filename_list.push_back(wstr_filename);
			}
		}
		
	}

	
}


void PefTxtDiffDlg::UpdateFileList_LivePath(const i3::wstring& base_path, int l_r)
{

	m_FileList[l_r].clear();

	i3::string	pack_path;	i3::utf16_to_mb(base_path, pack_path);
	i3::string  strDir_inPack;
	i3::const_char_range rng_ext;

	if (m_curr_file_format == PEFTXT_DIFF_FILE_FORMAT_PEF)
	{
		pack_path += "/Pack/Script.i3Pack";
		strDir_inPack = "config";
		rng_ext = i3::as_literal("pef");
	}
	else
	{
		pack_path += "/Pack/String.i3Pack";
		strDir_inPack = "Script/String/";

		i3::string strNation;	i3::utf16_to_mb(PathMgr::i()->GetNation(), strNation);
		strDir_inPack += strNation;
		rng_ext = i3::as_literal("txt");
	}

	i3PackVolume* pVolume = LoadPackVolume(pack_path);
	if (pVolume == NULL)
	{
		m_FileListCtrl[l_r].DeleteAllItems();
		return;
	}
	
	i3PackNode* root = pVolume->getRoot();
	i3::vector<i3::rc_wstring>	filename_list;

	I3_PACK_FILE_REF* dummy = NULL;

	i3PackNode* start_node = root->FindPath(strDir_inPack.c_str(), &dummy);
	if (start_node == NULL)
	{
		m_FileListCtrl[l_r].DeleteAllItems();
		pVolume->Release();
		return;
	}
	
	GatherInPackFileNode(start_node, strDir_inPack, m_FileList[l_r], filename_list, rng_ext);

	pVolume->Release();

	m_FileListCtrl[l_r].SetRedraw(FALSE);
	m_FileListCtrl[l_r].DeleteAllItems();

	const size_t num_file = filename_list.size();

	for (size_t i = 0 ; i < num_file ; ++i)
	{
		m_FileListCtrl[l_r].InsertItem(i, filename_list[i].c_str());
	}

	m_FileListCtrl[l_r].SetRedraw(TRUE);

	m_FileListCtrl[l_r].RedrawWindow();

	LVFINDINFO lvfi;
	lvfi.flags = LVFI_STRING;
	lvfi.psz = m_selected_filename.c_str();
	int found_idx = m_FileListCtrl[l_r].FindItem(&lvfi);

	if (found_idx != -1)
	{
		m_FileListCtrl[l_r].SetItemState(found_idx, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_FileListCtrl[l_r].EnsureVisible(found_idx, FALSE);
	}

}

void PefTxtDiffDlg::UpdateLogList(int file_list_index, int l_r)
{
	CString strSelFileName = m_FileListCtrl[l_r].GetItemText(file_list_index, 0);
	
	m_selected_filename.assign( LPCWSTR(strSelFileName), strSelFileName.GetLength());

//	int prev_sel_loglist_idx = m_LogListCtrl[l_r].GetFirstSelectedItem();

	m_LogInfoList[l_r].info_list.clear();

	int path_index = m_PathCtrl[l_r].GetFirstSelectedItem();
	if (path_index == -1)
	{
		m_LogListCtrl[l_r].DeleteAllItems();	
		return;
	}

	PathMode mode = PathMode(m_PathCtrl[l_r].GetItemData(path_index));

	i3::string full_path;

	if (mode == PATHMODE_LIVE)
	{
		// LIVE의 경우는 log기준이 .i3Pack파일이므로, 어떤 file_list_index가 오더라도
		// 영향을 받지 않는다..일단은 강제업데이트로 처리한다..
		
		i3::utf16_to_mb(PathMgr::i()->GetLivePath(), full_path);

		if (m_curr_file_format == PEFTXT_DIFF_FILE_FORMAT_PEF)
			full_path += "/Pack/Script.i3Pack";
		else
			full_path += "/Pack/String.i3Pack";
			
	}
	else
	{
		if (mode == PATHMODE_MEDIA) 
			i3::utf16_to_mb(PathMgr::i()->GetMainMediaPath(), full_path);
		else
			i3::utf16_to_mb(PathMgr::i()->GetQAPath(), full_path);

		const i3::rc_string& file_path = m_FileList[l_r][file_list_index];
		
		full_path += '/';
		full_path.append(file_path.begin(), file_path.end());
	}

	svn::GetLogInfos(full_path, 50, m_LogInfoList[l_r]);

	int base_rev = 0;
	svn::FindBaseRevision(full_path, base_rev);

	const size_t num_info = m_LogInfoList[l_r].info_list.size();
	
	m_LogListCtrl[l_r].SetRedraw(FALSE);
	m_LogListCtrl[l_r].DeleteAllItems();	

	i3::wstring temp;
	
	if (base_rev != 0)
		temp = i3::format_string(L"BaseRev : %d", base_rev);
	
	m_LogListCtrl[l_r].InsertItem(0, L"Local Data");
	m_LogListCtrl[l_r].SetItemText(0, 1, temp.c_str());
	m_LogListCtrl[l_r].SetItemData(0, NULL);
	

	for (size_t i = 0 ; i < num_info ; ++i)
	{
		svn::LogInfo* info = m_LogInfoList[l_r].info_list[i].get();
		temp = i3::format_string(L"r%d", info->revision);
		m_LogListCtrl[l_r].InsertItem( i + 1, temp.c_str());

		i3::mb_to_utf16(info->writer, temp);
		m_LogListCtrl[l_r].SetItemText(i+1, 1, temp.c_str());

		i3::mb_to_utf16(info->date, temp);
		m_LogListCtrl[l_r].SetItemText(i+1, 2, temp.c_str());
		m_LogListCtrl[l_r].SetItemData(i+1, DWORD_PTR( info) );
	}
	
	m_LogListCtrl[l_r].SetRedraw(TRUE);	
	
	// 급한대로 우선 HEAD 셀렉션을 걸어둔다..
	m_LogListCtrl[l_r].SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	
}

void PefTxtDiffDlg::UpdateLogText(int log_list_index, int l_r)
{
	m_LogDataCtrl[l_r].SetWindowText(L"");
	
	int path_index = m_PathCtrl[l_r].GetFirstSelectedItem();
	if (path_index == -1)
		return;

	int file_list_index = m_FileListCtrl[l_r].GetFirstSelectedItem();
	if (file_list_index == -1)
		return;

	PathMode mode = PathMode(m_PathCtrl[l_r].GetItemData(path_index));

	i3::string full_path;

	if (mode == PATHMODE_LIVE)
	{
		// LIVE의 경우는 log기준이 .i3Pack파일이므로, 어떤 file_list_index가 오더라도
		// 영향을 받지 않는다..일단은 강제업데이트로 처리한다..

		i3::utf16_to_mb(PathMgr::i()->GetLivePath(), full_path);

		if (m_curr_file_format == PEFTXT_DIFF_FILE_FORMAT_PEF)
			full_path += "/Pack/Script.i3Pack";
		else
			full_path += "/Pack/String.i3Pack";

	}
	else
	{
		if (mode == PATHMODE_MEDIA) 
			i3::utf16_to_mb(PathMgr::i()->GetMainMediaPath(), full_path);
		else
			i3::utf16_to_mb(PathMgr::i()->GetQAPath(), full_path);

		const i3::rc_string& file_path = m_FileList[l_r][file_list_index];

		full_path += '/';
		full_path.append(file_path.begin(), file_path.end());
	}
	
	svn::LogInfo* info = reinterpret_cast<svn::LogInfo*>(m_LogListCtrl[l_r].GetItemData(log_list_index));

	if (info != NULL)
	{
		i3::string result;
		svn::GetLogText(full_path, result, info->revision);
		
		i3::wstring wresult;	
		i3::mb_to_utf16(result, wresult);
		m_LogDataCtrl[l_r].SetWindowText(wresult.c_str());
	}
	
	
}

void PefTxtDiffDlg::UpdateNextLogList(int l_r)
{
	int path_index = m_PathCtrl[l_r].GetFirstSelectedItem();
	if (path_index == -1)
		return;
	
	int file_list_index = m_FileListCtrl[l_r].GetFirstSelectedItem();

	if (file_list_index == -1)
		return;

	PathMode mode = PathMode(m_PathCtrl[l_r].GetItemData(path_index));

	i3::string full_path;

	if (mode == PATHMODE_LIVE)
	{
		// LIVE의 경우는 log기준이 .i3Pack파일이므로, 어떤 file_list_index가 오더라도
		// 영향을 받지 않는다..일단은 강제업데이트로 처리한다..

		i3::utf16_to_mb(PathMgr::i()->GetLivePath(), full_path);

		if (m_curr_file_format == PEFTXT_DIFF_FILE_FORMAT_PEF)
			full_path += "/Pack/Script.i3Pack";
		else
			full_path += "/Pack/String.i3Pack";

	}
	else
	{
		if (mode == PATHMODE_MEDIA) 
			i3::utf16_to_mb(PathMgr::i()->GetMainMediaPath(), full_path);
		else
			i3::utf16_to_mb(PathMgr::i()->GetQAPath(), full_path);

		const i3::rc_string& file_path = m_FileList[l_r][file_list_index];

		full_path += '/';
		full_path.append(file_path.begin(), file_path.end());
	}
	
	const size_t num_prev_info =  m_LogInfoList[l_r].info_list.size();

	svn::GetLogInfos(full_path, 50, m_LogInfoList[l_r]);
	
	const size_t num_info = m_LogInfoList[l_r].info_list.size();
	i3::wstring temp;

	for (size_t i = num_prev_info ; i < num_info ; ++i)
	{
		svn::LogInfo* info = m_LogInfoList[l_r].info_list[i].get();
		temp = i3::format_string(L"r%d", info->revision);
		m_LogListCtrl[l_r].InsertItem( i + 1, temp.c_str());

		i3::mb_to_utf16(info->writer, temp);
		m_LogListCtrl[l_r].SetItemText(i+1, 1, temp.c_str());

		i3::mb_to_utf16(info->date, temp);
		m_LogListCtrl[l_r].SetItemText(i+1, 2, temp.c_str());
		m_LogListCtrl[l_r].SetItemData(i+1, DWORD_PTR( info) );
	}
	
}

void PefTxtDiffDlg::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	int index; 
	for (int l_r = 0 ; l_r < 2; ++l_r)
	{
		index =	m_PathCtrl[l_r].GetFirstSelectedItem();
		
		if (index != -1)
			m_result.mode[l_r] = PathMode(m_PathCtrl[l_r].GetItemData(index));
		else
			m_result.mode[l_r] = PATHMODE_LIVE;
		
		index = m_FileListCtrl[l_r].GetFirstSelectedItem();

		if (index != -1)
			m_result.rel_path[l_r] = m_FileList[l_r][index];
		else
			m_result.rel_path[l_r].clear();
		
		index = m_LogListCtrl[l_r].GetFirstSelectedItem();

		if (index != -1)
		{
			svn::LogInfo* info = (svn::LogInfo*) m_LogListCtrl[l_r].GetItemData(index);

			if (info)
				m_result.revision[l_r] = info->revision;
			else
				m_result.revision[l_r] = -1;
		}
		else
		{
			m_result.revision[l_r] = -1;
		}
	}

	CDialog::OnOK();
}
