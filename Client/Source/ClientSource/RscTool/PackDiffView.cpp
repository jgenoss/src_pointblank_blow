// PackDiffView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "PackDiffView.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/get_module_filename.h"
#include "i3Base/string/ext/extract_directoryname.h"
#include "PathMgr.h"
#include "MainFrm.h"
#include "ExecuteSVN.h"
#include "FindPefDiff.h"

#include "PreprocessMessageCallbackDerived.h"

#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/extract_filename.h"

#include "i3Base/string/system/traverse_file.h"

#include "PefTxtDiffDlg.h"
#include "PathMode.h"

// PackDiffView
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
		PackDiffView* owner;
		ListCallback(PackDiffView* owner) : owner(owner) {}
		virtual void OnDblClk(i3MFCListCtrl* Ctrl, INT32 idx) 
		{
			owner->OpenPef(idx);
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

	struct Scripti3PackWatcherCallback : ShellChangeCallback
	{
		PackDiffView* m_owner;
		Scripti3PackWatcherCallback(PackDiffView* owner) : m_owner(owner) {}

		virtual void OnCreate_Directory( PIDLIST_ABSOLUTE pidl) {}	
		virtual void OnDelete_Directory( PIDLIST_ABSOLUTE pidl) {}	
		virtual void OnRename_Directory( PIDLIST_ABSOLUTE from_pidl, PIDLIST_ABSOLUTE to_pidl ) {}	

		virtual void OnCreate_File( PIDLIST_ABSOLUTE pidl) {}	
		virtual void OnDelete_File( PIDLIST_ABSOLUTE pidl) {}	
		virtual void OnRename_File( PIDLIST_ABSOLUTE from_pidl, PIDLIST_ABSOLUTE to_pidl ) {}	

		virtual void OnUpdate_Directory(PIDLIST_ABSOLUTE pidl) {} 
		virtual void OnUpdate_File( PIDLIST_ABSOLUTE pidl) 
		{
			// 현재 Script.i3Pack파일에만 적용됬기 때문에 , pidl 경유한 파일명 검증은 생략한다..
		//	PreprocessMessageCallbackDerived cb;
		//	cb.OnStart();
		//	m_owner->LoadLocalScripti3Pack(&cb);
		//	cb.OnEnd();
		}
	};


}


IMPLEMENT_DYNCREATE(PackDiffView, CView)

PackDiffView::PackDiffView() : m_exported_revision(-1), m_exp_volume(NULL), m_local_volume(NULL)
{
	m_listCtrl.SetCallback(new ListCallback(this));

	m_watcher.SetCallback( new Scripti3PackWatcherCallback(this) );

}

PackDiffView::~PackDiffView()
{

	I3_SAFE_RELEASE(m_exp_volume);
	I3_SAFE_RELEASE(m_local_volume);

	delete m_watcher.GetCallback();
	delete m_listCtrl.GetCallback();

	::DeleteFileA(m_Expi3PackPath.c_str());
}

BEGIN_MESSAGE_MAP(PackDiffView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// PackDiffView 그리기입니다.

void PackDiffView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// PackDiffView 진단입니다.

#ifdef _DEBUG
void PackDiffView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void PackDiffView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// PackDiffView 메시지 처리기입니다.

int PackDiffView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	
	m_listCtrl.CreateEx(this, 1, LVS_NOSORTHEADER);
	m_listCtrl.InsertColumn(0, L"Pef files from i3Pack");
	


	return 0;
}

void PackDiffView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

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
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void	PackDiffView::Reset()
{
	m_exported_revision = -1;
	m_listCtrl.DeleteAllItems();
	m_rel_path_list.clear();
	m_filename_list.clear();

	
}


void	PackDiffView::UpdatePefItems()
{

	PackDiffView::Reset();

	i3::utf16_to_mb(PathMgr::i()->GetLivePath(), m_Scripti3PackPath);
	
	{
		i3::string module_path;	i3::get_module_filename(module_path);
		i3::extract_directoryname(module_path, m_Expi3PackPath);
		m_Expi3PackPath += "/Script(Temp).i3Pack";
	}

	if (!m_Scripti3PackPath.empty() )
	{
		m_Scripti3PackPath += "/Pack/Script.i3Pack";					// 하드코딩된 상대경로.. Script.i3Pack..
		if (::GetFileAttributesA(m_Scripti3PackPath.c_str()) == INVALID_FILE_ATTRIBUTES )
			m_Scripti3PackPath.clear();
	}
	
	if (m_Scripti3PackPath.empty())
		return;



	// Media폴더의 Config폴더 내 pef 파일을 긁어온다...
	
	struct filecallback : i3::traverse_file_callback_wc
	{
		i3::vector<i3::rc_string>& rel_path_list;
		i3::vector<i3::rc_wstring>& filename_list;
		filecallback(i3::vector<i3::rc_string>& rel_path_list, i3::vector<i3::rc_wstring>& filename_list) : 
			rel_path_list(rel_path_list), filename_list(filename_list) {}

		virtual void on_file(const WIN32_FIND_DATA& fd, const i3::rc_wstring& base_path, const i3::rc_wstring& subdir_path) 
		{
			i3::rc_wstring wstr_rel_path = L"config/";
			wstr_rel_path += fd.cFileName;
			i3::rc_string str_rel_path;
			i3::utf16_to_mb(wstr_rel_path, str_rel_path);
			rel_path_list.push_back(str_rel_path);
			filename_list.push_back(fd.cFileName);
		}

		virtual bool on_dir(const WIN32_FIND_DATA& fd, const i3::rc_wstring& base_path, const i3::rc_wstring& subdir_path) { return false; }
	};
	
	filecallback cb(m_rel_path_list, m_filename_list);

	i3::wstring path_to_find = PathMgr::i()->GetMainMediaPath();
	path_to_find += L"/config/*.pef";
	traverse_file(path_to_find, &cb, false);

	const size_t num_file = m_filename_list.size();

	for (size_t i = 0 ; i < num_file ; ++i)
	{
		m_listCtrl.InsertItem(i, m_filename_list[i].c_str());
	}

	i3::wstring wstrScripti3PackPath;
	i3::mb_to_utf16(m_Scripti3PackPath, wstrScripti3PackPath);

	m_watcher.Start( this->GetSafeHwnd(), wstrScripti3PackPath.c_str() );

}

void	PackDiffView::OpenPef( int list_idx )
{
	
	if (list_idx == -1)
	{
		return;
	}
	
	int curr_rev = m_exported_revision;

	PreprocessMessageCallbackDerived cb;
	cb.OnStart();

	cb.OnTextMessage(L"Find Latest Revision Number");

	if (! svn::FindLatestRevision(m_Scripti3PackPath, curr_rev) )		// 서브버전 연결이 안되면 다 황임..
	{
		m_exported_revision = -1;
		cb.OnEnd();
		return;
	}
	
	PackDiffView::LoadLocalScripti3Pack(&cb);
	
	if (curr_rev != m_exported_revision)				// PackVolume보관이 어렵다..(로컬과 SVN의 i3Pack 내 pef 상대경로가 동일해서 양쪽을 동시에 갖기 어렵다.)
	{													// pef의 root_node 획득이 끝나면 바로 털어서 버려야함...
		m_exported_revision = curr_rev;
		
		PackDiffView::LoadSVNScripti3Pack(&cb);
	}
	
	i3::const_char_range rng = m_rel_path_list[list_idx];
		
	i3::wstring txtmsg = i3::format_string(L"Load %s within Script.i3Pack From SVN", rng);

	cb.OnTextMessage(txtmsg.c_str());

	PushbackPackVolume(m_exp_volume);
	i3RegKey* exp_key = NULL;
	LoadPefForDiff( PathMgr::i()->GetLivePath(), rng, PathMgr::i()->GetNationIndex(), PathMgr::i()->GetNumNationIndex(), exp_key);
	ClearAllPackVolume();
	
	txtmsg = i3::format_string(L"Load %s within Script.i3Pack From Local Disk", rng);

	cb.OnTextMessage(txtmsg.c_str());

	PushbackPackVolume(m_local_volume);
	i3RegKey* local_key = NULL;
	LoadPefForDiff(PathMgr::i()->GetLivePath(), rng, PathMgr::i()->GetNationIndex(), PathMgr::i()->GetNumNationIndex(), local_key);
	ClearAllPackVolume();
		
	if (exp_key && local_key)
	{
		txtmsg = i3::format_string(L"Comparing %s with SVN & Local Data.", rng);
		cb.OnTextMessage(txtmsg.c_str());

		PefTxtDiffInputInfo info;
		info.mode[0] = info.mode[1] = PATHMODE_LIVE;
		info.rel_path[0] =  info.rel_path[1] = m_rel_path_list[list_idx];
		info.revision[0] = m_exported_revision; 
		info.revision[1] = -1;


		GetMainFrame()->CreatePefDiffFrame( info, exp_key, local_key);
	}

	I3_SAFE_RELEASE(local_key);
	I3_SAFE_RELEASE(exp_key);
	
	if (m_local_volume)
	{
		cb.OnTextMessage(L"Release Prev Local Script.i3Pack File");
		m_local_volume->Release();
		m_local_volume = NULL;
	}

	cb.OnEnd();
}

void	PackDiffView::LoadLocalScripti3Pack(PreprocessMessageCallback* cb)
{
	if (m_local_volume)
	{
		cb->OnTextMessage(L"Release Prev Local Script.i3Pack File");
		m_local_volume->Release();
	}
	
	cb->OnTextMessage(L"Load Local Script.i3Pack File");

	m_local_volume = LoadPackVolume( m_Scripti3PackPath );
}


void	PackDiffView::LoadSVNScripti3Pack(PreprocessMessageCallback* cb)
{
	// 익스포트부터 먼저 수행..
	if (m_exp_volume)
	{
		cb->OnTextMessage(L"Release Prev SVN Script.i3Pack File");
		m_exp_volume->Release();
	}

	cb->OnTextMessage(L"Find SVN Script.i3Pack URL");
	i3::string url;
	svn::FindURL( m_Scripti3PackPath, url);

	cb->OnTextMessage(L"Export SVN Script.i3Pack File From Server");
	svn::ExportSync(m_exported_revision, url, m_Expi3PackPath);

	cb->OnTextMessage(L"Load SVN Script.i3Pack File");
	m_exp_volume = LoadPackVolume(m_Expi3PackPath);
	
}

namespace
{
	i3RegKey*		Createi3RegKey_LivePath( const i3::wstring& base_path, const i3::rc_string& rel_path, int rev,
		int nation_idx, int num_nation, PreprocessMessageCallback* cb)
	{
		i3PackVolume* vol = NULL;

		i3::string exp_path;
		i3::string pack_path; 	i3::utf16_to_mb(base_path, pack_path);
		pack_path += "/Pack/Script.i3Pack";
		if (::GetFileAttributesA(pack_path.c_str()) == INVALID_FILE_ATTRIBUTES)
			return NULL;

		if (rev == -1) // 로컬
		{
			cb->OnTextMessage(L"Load Local Script.i3Pack File");

			vol = LoadPackVolume(pack_path);

			if (vol == NULL)
				return NULL;
		}
		else
		{
			cb->OnTextMessage(L"Find SVN Script.i3Pack URL");
			i3::string url;
			svn::FindURL( pack_path, url);

			cb->OnTextMessage(L"Export SVN Script.i3Pack File From Server");

			{
				i3::string module_path;	i3::get_module_filename(module_path);
				i3::extract_directoryname(module_path, exp_path);
				exp_path += "/Script(Temp2).i3Pack";		
			}
			svn::ExportSync(rev, url, exp_path);

			cb->OnTextMessage(L"Load SVN Script.i3Pack File");
			vol = LoadPackVolume(exp_path);

			if (vol == NULL)
			{
				::DeleteFileA(exp_path.c_str());
				return NULL;
			}
		}


		i3::string		file_name;
		i3::extract_filename(rel_path, file_name);

		i3::wstring txtmsg = i3::format_string(L"Load %s within Script.i3Pack", file_name);

		cb->OnTextMessage(txtmsg.c_str());

		PushbackPackVolume(vol);
		i3RegKey* key = NULL;
		LoadPefForDiff( base_path, rel_path, nation_idx, num_nation, key);
		ClearAllPackVolume();

		vol->Release();

		if (!exp_path.empty())
			::DeleteFileA(exp_path.c_str());

		return key;	
	}

	i3RegKey*		Createi3RegKey_MediaQAPath( const i3::wstring& base_path, const i3::rc_string& rel_path, int rev,
		int nation_idx, int num_nation, PreprocessMessageCallback* cb)
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
				return NULL;

			active_rel_path = file_name;
		}

		i3::wstring txtmsg = i3::format_string(L"Load %s", file_name);

		cb->OnTextMessage(txtmsg.c_str());


		i3RegKey* key = NULL;
		LoadPefForDiff( active_base_path, active_rel_path, nation_idx, num_nation, key);

		if (!exp_path.empty())
			::DeleteFileA(exp_path.c_str());

		return key;		
	}
	
	i3RegKey*		Createi3RegKey( PathMode m, const i3::wstring& base_path, const i3::rc_string& rel_path, int rev,
									int nation_idx, int num_nation, PreprocessMessageCallback* cb)
	{
		if ( m == PATHMODE_LIVE )
			return Createi3RegKey_LivePath(base_path, rel_path, rev, nation_idx, num_nation, cb);
		
		return Createi3RegKey_MediaQAPath(base_path, rel_path, rev, nation_idx, num_nation, cb);
	}
	
}



void	PackDiffView::OpenPefTxtDiffDlg(int list_idx, PathMode m)
{
	i3::rc_wstring filename;

	if (list_idx != -1)
		filename = m_filename_list[list_idx];

	PefTxtDiffDlg dlg;
	dlg.InitParam(m, PEFTXT_DIFF_FILE_FORMAT_PEF, filename);

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

		i3RegKey* pleft = Createi3RegKey(res.mode[0], *base_path[0], res.rel_path[0],
			res.revision[0], PathMgr::i()->GetNationIndex(), PathMgr::i()->GetNumNationIndex(), &cb);

		i3RegKey* pright = Createi3RegKey(res.mode[1], *base_path[1], res.rel_path[1],
			res.revision[1], PathMgr::i()->GetNationIndex(), PathMgr::i()->GetNumNationIndex(), &cb);

		if ( pleft && pright)
		{
			cb.OnTextMessage(L"Comparing Files");

			GetMainFrame()->CreatePefDiffFrame(res, pleft, pright);
		}

		I3_SAFE_RELEASE(pright);
		I3_SAFE_RELEASE(pleft);

		cb.OnEnd();
	}
	
	
}