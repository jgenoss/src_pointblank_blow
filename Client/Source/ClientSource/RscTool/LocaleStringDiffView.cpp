// LocaleStringDiffView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "RscTool.h"
#include "LocaleStringDiffView.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/get_module_filename.h"
#include "i3Base/string/ext/extract_directoryname.h"
#include "FindPefDiff.h"
#include "PathMgr.h"
#include "PreprocessMessageCallbackDerived.h"
#include "ExecuteSVN.h"
#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/extract_filename.h"

#include "LocaleStringDiff.h"
#include "MainFrm.h"

#include "PefTxtDiffDlg.h"
#include "PathMode.h"

// LocaleStringDiffView

namespace
{

	enum
	{
		COMMAND_OPEN_MEDIAPATH_DIFF_DLG = 1,
		COMMAND_OPEN_QAPATH_DIFF_DLG,
		COMMAND_OPEN_LIVEPATH_DIFF_DLG,
	};


	struct ListCallback : i3MFCListCtrlCallback
	{
		LocaleStringDiffView* owner;
		ListCallback(LocaleStringDiffView* owner) : owner(owner) {}

		virtual void OnDblClk(i3MFCListCtrl* Ctrl, INT32 idx) 
		{
			owner->OpenTxt(idx);
		}		

		virtual void OnContextMenu(i3MFCListCtrl* Ctrl, const CPoint& pt) 
		{
			CPoint ptClient = pt;
			Ctrl->ScreenToClient(&ptClient);
			int target_idx = Ctrl->HitTest(ptClient);

			CMenu menu;
			menu.CreatePopupMenu();

			// 나중에 Path 유효 여부를 두고 분기처리해야한다..
			menu.AppendMenu(MF_STRING, COMMAND_OPEN_MEDIAPATH_DIFF_DLG, L"Open Diff Dialog in Media Path");		
			menu.AppendMenu(MF_STRING, COMMAND_OPEN_QAPATH_DIFF_DLG, L"Open Diff Dialog in QA Path");		
			menu.AppendMenu(MF_STRING, COMMAND_OPEN_LIVEPATH_DIFF_DLG, L"Open Diff Dialog in Live Path");		

			long result_cmd = menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, pt.x, pt.y, Ctrl);
			menu.DestroyMenu();

			if (Ctrl->GetFirstSelectedItem() == -1)
			{
				if (target_idx != -1)
					Ctrl->SetItemState(target_idx, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			}
			else
			{
				target_idx = Ctrl->GetFirstSelectedItem();
			}

			switch( result_cmd )
			{
			case COMMAND_OPEN_MEDIAPATH_DIFF_DLG:
				owner->OpenPefTxtDiffDlg( target_idx, PATHMODE_MEDIA);
			break;
			case COMMAND_OPEN_QAPATH_DIFF_DLG:
				owner->OpenPefTxtDiffDlg( target_idx, PATHMODE_QA);
			break;
			case COMMAND_OPEN_LIVEPATH_DIFF_DLG:
				owner->OpenPefTxtDiffDlg( target_idx, PATHMODE_LIVE);
			break;
			default:
				break;
			}

		}
	};


}


IMPLEMENT_DYNCREATE(LocaleStringDiffView, CView)

LocaleStringDiffView::LocaleStringDiffView() :  
m_exported_revision(-1), m_exp_volume(NULL), m_local_volume(NULL)
{
	m_listCtrl.SetCallback(new ListCallback(this));

}

LocaleStringDiffView::~LocaleStringDiffView()
{
	I3_SAFE_RELEASE(m_exp_volume);
	I3_SAFE_RELEASE(m_local_volume);

	delete m_listCtrl.GetCallback();

	::DeleteFileA(m_Expi3PackPath.c_str());
}

BEGIN_MESSAGE_MAP(LocaleStringDiffView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// LocaleStringDiffView 그리기입니다.

void LocaleStringDiffView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// LocaleStringDiffView 진단입니다.

#ifdef _DEBUG
void LocaleStringDiffView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void LocaleStringDiffView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// LocaleStringDiffView 메시지 처리기입니다.

int LocaleStringDiffView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_listCtrl.CreateEx(this, 1, LVS_NOSORTHEADER);
	m_listCtrl.InsertColumn(0, L"Text Files from i3Pack");

	return 0;
}

void LocaleStringDiffView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_listCtrl.SetSize(cx, cy);

	SCROLLINFO info;
	m_listCtrl.GetScrollInfo(SB_VERT, &info, SIF_RANGE | SIF_PAGE );
	// Range와 Page 관계를 통해 스크롤바의 출력 여부를 확인했다.. ( 리스트 콘트롤 기준이며, 그외는 사용자임의 기준이다.)
	// Max + 1 > Page 인 경우 스크롤바가 출력된다..
	if ( UINT(info.nMax) + 1 > info.nPage )
	{
		// horz크기를 조정해야한다. (종 스크롤바 너비만큼을 빼야한다.)
		cx -= GetSystemMetrics(SM_CXVSCROLL);
	}

	m_listCtrl.SetColumnWidth( 0, cx);

}

void	LocaleStringDiffView::Reset()
{
	m_exported_revision = -1;
	m_listCtrl.DeleteAllItems();
	m_rel_path_list.clear();
	m_filename_list.clear();
}

namespace
{


	void GatherTxtFileNode( i3PackNode* n, const i3::string& parent_path,
		i3::vector<i3::rc_string>& rel_path_list,
		i3::vector<i3::rc_wstring>& filename_list) 
	{
		i3::string curr_path = parent_path;
		if ( n->getParent() )
		{
			if (!curr_path.empty())
				curr_path += '/';
			const i3::rc_string& name = n->GetNameString();
			curr_path.append(name.begin(), name.end());
		}
		
		const INT32 num_file = n->getFileCount();
		
		i3::string strext;
		i3::const_char_range rng_txt = i3::as_literal("txt");

		for (INT32 i = 0 ; i < num_file ; ++i )
		{
			I3_PACK_FILE_REF *	info = n->getFile(i);

			i3::const_char_range rng = i3::as_literal(info->m_szName);

			i3::extract_fileext(rng, strext);

			if ( i3::generic_is_iequal(strext, rng_txt) )
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
		
		const INT32 num_child = n->getChildCount();

		for (INT32 i = 0 ; i < num_child ; ++i)
		{
			GatherTxtFileNode((i3PackNode*)n->getChild(i), 
				curr_path, rel_path_list, filename_list);
		}
	}

}

void	LocaleStringDiffView::UpdateLocaleTextFiles()
{
	LocaleStringDiffView::Reset();
	
	i3::utf16_to_mb(PathMgr::i()->GetLivePath(), m_Stringi3PackPath);
	
	{
		i3::string module_path;	i3::get_module_filename(module_path);
		i3::extract_directoryname(module_path, m_Expi3PackPath);
		m_Expi3PackPath += "/String(Temp).i3Pack";		
	}

	if ( !m_Stringi3PackPath.empty() )
	{
		m_Stringi3PackPath += "/Pack/String.i3Pack";
		if (::GetFileAttributesA(m_Stringi3PackPath.c_str()) == INVALID_FILE_ATTRIBUTES)
			m_Stringi3PackPath.clear();
	}
	
	if (m_Stringi3PackPath.empty())
		return;
	
	// txt의 경우 Media에서 목록을 읽지 말고...
	// LivePath의 i3Pack파일 내부를 직접 조회해서 목록을 얻는다..

	i3PackVolume* pVolume = LoadPackVolume(m_Stringi3PackPath);
	
	if (!pVolume)
		return;

	i3PackNode* root = pVolume->getRoot();
	
//	if (root == NULL)
//		__debugbreak();

	GatherTxtFileNode(root, i3::string(), m_rel_path_list, m_filename_list);

	pVolume->Release();

	const size_t num_file = m_filename_list.size();

	for (size_t i = 0 ; i < num_file ; ++i)
	{
		m_listCtrl.InsertItem(i, m_filename_list[i].c_str());
	}
}

void	LocaleStringDiffView::OpenTxt( int list_idx )
{
	if (list_idx == -1)
		return;

	int curr_rev = m_exported_revision;

	PreprocessMessageCallbackDerived cb;
	cb.OnStart();

	cb.OnTextMessage(L"Find Latest revision Number");

	if (! svn::FindLatestRevision(m_Stringi3PackPath, curr_rev) )
	{
		m_exported_revision = -1;
		cb.OnEnd();
		return;
	}
	
	LocaleStringDiffView::LoadLocalScripti3Pack(&cb);

	if (curr_rev != m_exported_revision)				// PackVolume보관이 어렵다..(로컬과 SVN의 i3Pack 내 pef 상대경로가 동일해서 양쪽을 동시에 갖기 어렵다.)
	{													// pef의 root_node 획득이 끝나면 바로 털어서 버려야함...
		m_exported_revision = curr_rev;

		LocaleStringDiffView::LoadSVNScripti3Pack(&cb);
	}
	
	//
	{
		i3::const_char_range rng = m_rel_path_list[list_idx];

		i3::wstring txtmsg = i3::format_string(L"Load %s within String.i3Pack From SVN", rng);

		cb.OnTextMessage(txtmsg.c_str());

		PushbackPackVolume(m_exp_volume);
		
		LocaleStringSetPtr ptr_exp = i3::make_shared<LocaleStringSet>();
		
		LoadLocaleStringForDiff( PathMgr::i()->GetLivePath(), rng, *ptr_exp);
		ClearAllPackVolume();

		txtmsg = i3::format_string(L"Load %s within String.i3Pack From Local Disk", rng);

		cb.OnTextMessage(txtmsg.c_str());

		PushbackPackVolume(m_local_volume);
		
		LocaleStringSetPtr ptr_local = i3::make_shared<LocaleStringSet>();

		LoadLocaleStringForDiff(PathMgr::i()->GetLivePath(), rng, *ptr_local);
		ClearAllPackVolume();

		if (!ptr_exp->m_list.empty() && !ptr_local->m_list.empty())
		{
			txtmsg = i3::format_string(L"Comparing %s with SVN & Local Data.", rng);
			cb.OnTextMessage(txtmsg.c_str());

			PefTxtDiffInputInfo info;
			info.mode[0] = info.mode[1] = PATHMODE_LIVE;
			info.rel_path[0] =  info.rel_path[1] = m_rel_path_list[list_idx];
			info.revision[0] = m_exported_revision; 
			info.revision[1] = -1;

			GetMainFrame()->CreateLocaleStringDiffFrame( info, 
				ptr_exp, ptr_local);
		}

	}

	if (m_local_volume)
	{
		cb.OnTextMessage(L"Release Prev Local Script.i3Pack File");
		m_local_volume->Release();
		m_local_volume = NULL;
	}

	cb.OnEnd();

}

void	LocaleStringDiffView::LoadLocalScripti3Pack(PreprocessMessageCallback* cb)
{
	if (m_local_volume)
	{
		cb->OnTextMessage(L"Release Prev Local String.i3Pack File");
		m_local_volume->Release();
	}
	
	cb->OnTextMessage(L"Load Local String.i3Pack File");
	m_local_volume = LoadPackVolume(m_Stringi3PackPath);

}

void	LocaleStringDiffView::LoadSVNScripti3Pack(PreprocessMessageCallback* cb)
{
	// 익스포트부터 먼저 수행..
	if (m_exp_volume)
	{
		cb->OnTextMessage(L"Release Prev SVN String.i3Pack File");
		m_exp_volume->Release();
	}

	cb->OnTextMessage(L"Find SVN String.i3Pack URL");
	i3::string url;
	svn::FindURL( m_Stringi3PackPath, url);

	cb->OnTextMessage(L"Export SVN String.i3Pack File From Server");
	svn::ExportSync(m_exported_revision, url, m_Expi3PackPath);

	cb->OnTextMessage(L"Load SVN String.i3Pack File");
	m_exp_volume = LoadPackVolume(m_Expi3PackPath);
}

namespace
{
	LocaleStringSetPtr  CreateLocaleStringSet_LivePath(const i3::wstring& base_path, const i3::rc_string& rel_path, int rev, PreprocessMessageCallback* cb)
	{
		i3PackVolume* vol = NULL;

		i3::string exp_path;

		i3::string pack_path; 	i3::utf16_to_mb(base_path, pack_path);
		pack_path += "/Pack/String.i3Pack";
		if (::GetFileAttributesA(pack_path.c_str()) == INVALID_FILE_ATTRIBUTES)
			return LocaleStringSetPtr();

		if (rev == -1) // 로컬
		{
			cb->OnTextMessage(L"Load Local String.i3Pack File");

			vol = LoadPackVolume(pack_path);

			if (vol == NULL)
				return LocaleStringSetPtr();
		}
		else
		{
			cb->OnTextMessage(L"Find SVN String.i3Pack URL");
			i3::string url;
			svn::FindURL( pack_path, url);

			cb->OnTextMessage(L"Export SVN String.i3Pack File From Server");
		
			{
				i3::string module_path;	i3::get_module_filename(module_path);
				i3::extract_directoryname(module_path, exp_path);
				exp_path += "/String(Temp2).i3Pack";		
			}
			svn::ExportSync(rev, url, exp_path);

			cb->OnTextMessage(L"Load SVN String.i3Pack File");
			vol = LoadPackVolume(exp_path);

			if (vol == NULL)
			{
				::DeleteFileA(exp_path.c_str());
				return LocaleStringSetPtr();
			}
		}
		

		i3::string		file_name;
		i3::extract_filename(rel_path, file_name);

		i3::wstring txtmsg = i3::format_string(L"Load %s within String.i3Pack", file_name);

		cb->OnTextMessage(txtmsg.c_str());

		PushbackPackVolume(vol);
		LocaleStringSetPtr ptr_set = i3::make_shared<LocaleStringSet>();
		LoadLocaleStringForDiff( base_path, rel_path, *ptr_set);
		ClearAllPackVolume();
		
		vol->Release();

		if (!exp_path.empty())
			::DeleteFileA(exp_path.c_str());
		
		return ptr_set;
	}
	
	LocaleStringSetPtr  CreateLocaleStringSet_MediaQAPath(const i3::wstring& base_path, const i3::rc_string& rel_path, int rev, PreprocessMessageCallback* cb)
	{
		i3::wstring		active_base_path;
		i3::rc_string	active_rel_path;
		i3::string		exp_path;
		i3::string		file_name;
		i3::extract_filename(rel_path, file_name);

		if (rev == -1) // 로컬
		{
			active_base_path = base_path;
			active_rel_path = rel_path;
		}
		else
		{
			i3::wstring module_path;	i3::get_module_filename(module_path);
			i3::extract_directoryname(module_path, active_base_path);
			
			i3::wstring txtmsg = i3::format_string(L"Find SVN %s URL", file_name);
			cb->OnTextMessage(txtmsg.c_str());

			i3::string file_path;
			i3::utf16_to_mb(base_path, file_path);
			file_path += '/';
			file_path.append(rel_path.begin(), rel_path.end());
			
			i3::utf16_to_mb(active_base_path, exp_path);
			exp_path += '/';
			exp_path += file_name;

			i3::string url;
			svn::FindURL( file_path, url);

			txtmsg = i3::format_string(L"Export SVN %s File From Server", file_name);
			cb->OnTextMessage(txtmsg.c_str());

			svn::ExportSync(rev, url, exp_path);
			
			if ( ::GetFileAttributesA(exp_path.c_str()) == INVALID_FILE_ATTRIBUTES)
				return LocaleStringSetPtr();

			active_rel_path = file_name;
		}

		i3::wstring txtmsg = i3::format_string(L"Load %s", file_name);

		cb->OnTextMessage(txtmsg.c_str());
		
		LocaleStringSetPtr ptr_set = i3::make_shared<LocaleStringSet>();
		LoadLocaleStringForDiff( active_base_path, active_rel_path, *ptr_set);

		if (!exp_path.empty())
			::DeleteFileA(exp_path.c_str());

		return ptr_set;
	}

	LocaleStringSetPtr  CreateLocaleStringSet(PathMode m, const i3::wstring& base_path, const i3::rc_string& rel_path, int rev, PreprocessMessageCallback* cb )
	{
		if (m == PATHMODE_LIVE)
			return CreateLocaleStringSet_LivePath(base_path, rel_path, rev, cb);

		return CreateLocaleStringSet_MediaQAPath(base_path, rel_path, rev, cb);
	}
		
}



void	LocaleStringDiffView::OpenPefTxtDiffDlg(int list_idx, PathMode m)
{
	i3::rc_wstring filename;

	if (list_idx != -1)
		filename = m_filename_list[list_idx];

	PefTxtDiffDlg dlg;
	dlg.InitParam(m, PEFTXT_DIFF_FILE_FORMAT_TXT, filename);
	
	if (dlg.DoModal() == IDOK)
	{
		const PefTxtDiffInputInfo& res = dlg.GetResult();		
		PreprocessMessageCallbackDerived cb;
		cb.OnStart();
		
		const i3::wstring* base_path[2] = { NULL, NULL};
		if ( res.mode[0] == PATHMODE_MEDIA )
			base_path[0] = &PathMgr::i()->GetMainMediaPath();
		else 
		if ( res.mode[0] == PATHMODE_QA )
			base_path[0] = &PathMgr::i()->GetQAPath();
		else
			base_path[0] = &PathMgr::i()->GetLivePath();

		if ( res.mode[1] == PATHMODE_MEDIA )
			base_path[1] = &PathMgr::i()->GetMainMediaPath();
		else 
		if ( res.mode[1] == PATHMODE_QA )
			base_path[1] = &PathMgr::i()->GetQAPath();
		else
			base_path[1] = &PathMgr::i()->GetLivePath();
		
		LocaleStringSetPtr ptr_left = CreateLocaleStringSet(res.mode[0], *base_path[0], res.rel_path[0],
			res.revision[0], &cb);
		
		LocaleStringSetPtr ptr_right = CreateLocaleStringSet(res.mode[1], *base_path[1], res.rel_path[1],
			res.revision[1], &cb);

		if (!ptr_left->m_list.empty() && !ptr_left->m_list.empty())
		{
			cb.OnTextMessage(L"Comparing Files");

			GetMainFrame()->CreateLocaleStringDiffFrame(res, 
				ptr_left, ptr_right);
		}
		
		cb.OnEnd();
	}
	

}
