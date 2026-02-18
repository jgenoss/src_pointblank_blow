
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "RscTool.h"

#include "MainFrm.h"

#include "PackScriptTree.h"

#include "i3Base/itl/vector_util.h"
#include "i3Base/itl/container_util.h"

#include "PackScriptFileFrameWnd.h"
#include "ItemDetailFrame.h"

#include "i3Base/string/ext/format_string.h"

#include "PathMgr.h"
#include "AssocFile.h"

#include "i3Base/string/ext/mb_to_utf16.h"

#include "ExecutePackFrameWnd.h"
#include "PefDiffFrameWnd.h"
#include "LocaleStringFrameWnd.h"
#include "PefTxtDiffInputInfo.h"
#include "FileCheck_MD5FrameWnd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

//const UINT WM_DESTROY_PACKSCRIPTFILEPANE = ::RegisterWindowMessage(_T("WM_DESTROY_PACKSCRIPTFILEPANE"));

namespace
{
	i3::vector<CFrameWndEx*>	g_wndPopupList;
}


IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_COMMAND(ID_FILE_NEW_RESOURCE_SOLUTION, &CMainFrame::OnFileNewResourceSolution)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
//	ON_REGISTERED_MESSAGE(WM_DESTROY_PACKSCRIPTFILEPANE, &CMainFrame::OnDestroyPackScriptFilePane)

	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnUpdateApplicationLook)
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸
//#include "ExecuteSVN.h"

CMainFrame::CMainFrame() : m_packscript_view(NULL), m_item_view(NULL), m_ExecutePackFrameWnd(NULL)
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2005);

	//
//	int out;
//	svn::FindLatestRevision(i3::as_literal("D:/SVN_PB/NEW15/V2/Media/config/Weapon.pef"), out);
	//
	
//	i3::string url;
//	svn::FindURL(i3::as_literal("D:/SVN_PB/NEW15/v2/Media"), url);

	int iDebug = 1;
}

CMainFrame::~CMainFrame()
{
//	i3::cu::for_each_delete(m_PackScriptFilePaneList);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	
//	CTheme::i()->OpenTheme(this->GetSafeHwnd());

	BOOL bNameValid;
	// 보관된 값에 따라 비주얼 관리자 및 스타일을 설정합니다.
	OnApplicationLook(theApp.m_nAppLook);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("메뉴 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	m_wndMenuBar.SetPaneStyle(	m_wndMenuBar.GetPaneStyle() | 
								CBRS_SIZE_DYNAMIC | 
								CBRS_TOOLTIPS | CBRS_FLYBY );
	

	// 메뉴 모음을 활성화해도 포커스가 이동하지 않게 합니다.
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// 사용자 정의 도구 모음 작업을 허용합니다.
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 도구 모음 및 메뉴 모음을 도킹할 수 없게 하려면 이 다섯 줄을 삭제하십시오.
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// Visual Studio 2005 스타일 도킹 창 동작을 활성화합니다.
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 스타일 도킹 창 자동 숨김 동작을 활성화합니다.
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 도구 모음 및 도킹 창 메뉴 바꾸기를 활성화합니다.
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 빠른(<Alt> 키를 누른 채 끌기) 도구 모음 사용자 지정을 활성화합니다.
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// 사용자 정의 도구 모음 이미지를 로드합니다.
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			m_UserImages.SetImageSize(CSize(16, 16), FALSE);
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// 메뉴 개인 설정을 활성화합니다(가장 최근에 사용한 명령).
	// TODO: 사용자의 기본 명령을 정의하여 각 풀다운 메뉴에 하나 이상의 기본 명령을 포함시킵니다.

	// 자동숨김 기능이 강제로 켜지는걸 막기 위해 개인메뉴기능을 주석처리한다.(2013.12.24.수빈)
/*
	CList<UINT, UINT> lstBasicCommands;

//	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);
*/


	
//	GetDockingManager()->DisableRestoreDockState(TRUE);
	
//	m_tmpPane.DockTo
		
//	m_tmpPane.ShowWindow(SW_SHOW);
	
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~(FWS_ADDTOTITLE|FWS_PREFIXTITLE);





	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.


	int fullscreen_w = ::GetSystemMetrics(SM_CXSCREEN);
	int fullscreen_h = ::GetSystemMetrics(SM_CYSCREEN);

	cs.cx = fullscreen_w * 9 / 10;
	cs.cy = fullscreen_h * 5 / 6;

	cs.y = (fullscreen_h - cs.cy) / 2;
	cs.x = (fullscreen_w - cs.cx) / 2;



	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 메뉴를 검색합니다. */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 기본 클래스가 실제 작업을 수행합니다.

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// 모든 사용자 도구 모음에 사용자 지정 단추를 활성화합니다.
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}

void CMainFrame::OnFileNewResourceSolution()
{
//	int iDEbug = 1;	
	wchar_t szFilter[] = L"Resource Solution (*.rscsln) | *.rscsln ||";

	CFileDialog dlg(FALSE, L"rscsln", NULL, OFN_HIDEREADONLY, szFilter);

//	dlg.m_ofn

	if (IDOK == dlg.DoModal())
	{
		CString str = dlg.GetPathName();
		
		int iDebug = 1;

	}
}

static int s_count = 0;


void	CMainFrame::CreatePackScriptFileFrame(PackScriptFile* psf)
{
	
	PackScriptFileFrameWnd* newWnd = static_cast<PackScriptFileFrameWnd*>(PackScriptFileFrameWnd::CreateObject());

	g_wndPopupList.push_back(newWnd);		// 일단, 생성시에 적재..

	i3::wstring wstrFilename = i3::format_string(L"%s.i3PackScript (%s)", psf->GetFileTitle(), PathMgr::i()->GetPathName());

	CRect rcMainFrame;
	GetWindowRect(&rcMainFrame);		// 메인 프레임 중앙에 올수 있도록 처리해본다..  
	
	newWnd->Create(0, wstrFilename.c_str(), WS_CAPTION|WS_SYSMENU | WS_THICKFRAME| WS_MINIMIZEBOX|WS_MAXIMIZEBOX		
		, rcMainFrame, NULL);		

	//
	// 메인프레임의 자식으로 두었다가, 관련성을 제거했다.. 메인프레임 종료시에 메모리 릭 리포트가 되므로..
	// 모아놓고 관리해서 다 지워야 한다.. (일단 delete만 잘 하면 될것 같다..)
	//

		// WS_OVERLAPPEDWINDOW, rcPane, this);
	newWnd->SetPackScriptFile(psf);
//	psff->SetWindowPos(NULL, rcPane.left, rcPane.top, rcPane.Width(), rcPane.Height(), SWP_NOACTIVATE | SWP_NOZORDER );
	newWnd->ShowWindow(SW_SHOW);
	newWnd->UpdateWindow();

}

void		CMainFrame::CreateItemDetailFrame(const AssocFileSetPtr& ptr)
{
	
	ItemDetailFrame* newWnd = static_cast<ItemDetailFrame*>(ItemDetailFrame::CreateObject());

	g_wndPopupList.push_back(newWnd);		// 일단, 생성시에 적재..

	i3::wstring wstrFilename = i3::format_string(L"%s (%s)", ptr->item_name, PathMgr::i()->GetPathName());

	CRect rcMainFrame;
	GetWindowRect(&rcMainFrame);		// 메인 프레임 중앙에 올수 있도록 처리해본다..  

	newWnd->Create(0, wstrFilename.c_str(), WS_CAPTION|WS_SYSMENU | WS_THICKFRAME| WS_MINIMIZEBOX|WS_MAXIMIZEBOX		
		, rcMainFrame, NULL);		

	//
	// 메인프레임의 자식으로 두었다가, 관련성을 제거했다.. 메인프레임 종료시에 메모리 릭 리포트가 되므로..
	// 모아놓고 관리해서 다 지워야 한다.. (일단 delete만 잘 하면 될것 같다..)
	//

	// WS_OVERLAPPEDWINDOW, rcPane, this);
	newWnd->SetAssocFileSet(ptr);

	newWnd->ShowWindow(SW_SHOW);
	newWnd->UpdateWindow();
	
}

void	CMainFrame::CreatePefDiffFrame(const PefTxtDiffInputInfo& input_info, i3RegKey* exp_key, i3RegKey* local_key)
{
	
	PefDiffFrameWnd* newWnd = static_cast<PefDiffFrameWnd*>(PefDiffFrameWnd::CreateObject());

	g_wndPopupList.push_back(newWnd);		// 일단, 생성시에 적재..
	
	i3::string str_rev[2];

	if (input_info.revision[0] != -1)
		str_rev[0] = i3::format_string("r%d", input_info.revision[0]);
	else
		str_rev[0] = "Local";
	
	if (input_info.revision[1] != -1)
		str_rev[1] = i3::format_string("r%d", input_info.revision[1]);
	else
		str_rev[1] = "Local";
	
	i3::const_char_range str_mode[3] = { i3::as_literal("MEDIA_PATH"), 
		i3::as_literal("QA_PATH"), i3::as_literal("LIVE_PATH") };

	i3::string strWindowTitle = i3::format_string("Pef Diff : [ %s (%s of %s) ] VS [ %s (%s of %s) ]", 
		input_info.rel_path[0], str_rev[0], str_mode[input_info.mode[0]],
		input_info.rel_path[1], str_rev[1], str_mode[input_info.mode[1]]);

	i3::wstring wstrWindowTitle;
	i3::mb_to_utf16(strWindowTitle, wstrWindowTitle);
	
	CRect rcMainFrame;
	GetWindowRect(&rcMainFrame);		// 메인 프레임 중앙에 올수 있도록 처리해본다..  

	newWnd->Create(0, wstrWindowTitle.c_str(), WS_CAPTION|WS_SYSMENU | WS_THICKFRAME| WS_MINIMIZEBOX|WS_MAXIMIZEBOX		
		, rcMainFrame, NULL);		

	newWnd->SetInfo(input_info, exp_key, local_key);
	newWnd->ShowWindow(SW_SHOW);
	newWnd->UpdateWindow();
		
}

void	CMainFrame::CreateLocaleStringDiffFrame(const PefTxtDiffInputInfo& input_info, const LocaleStringSetPtr& exp, const LocaleStringSetPtr& local)
{
	LocaleStringFrameWnd* newWnd = 
		static_cast<LocaleStringFrameWnd*>(LocaleStringFrameWnd::CreateObject());

	g_wndPopupList.push_back(newWnd);		// 일단, 생성시에 적재..

	i3::string str_rev[2];

	if (input_info.revision[0] != -1)
		str_rev[0] = i3::format_string("r%d", input_info.revision[0]);
	else
		str_rev[0] = "Local";

	if (input_info.revision[1] != -1)
		str_rev[1] = i3::format_string("r%d", input_info.revision[1]);
	else
		str_rev[1] = "Local";
	
	i3::const_char_range str_mode[3] = { i3::as_literal("MEDIA_PATH"), 
							i3::as_literal("QA_PATH"), i3::as_literal("LIVE_PATH") };


	i3::string strWindowTitle = i3::format_string("Txt Diff : [ %s (%s of %s) ] VS [ %s (%s of %s) ]", 
		input_info.rel_path[0], str_rev[0], str_mode[ input_info.mode[0] ],
		input_info.rel_path[1], str_rev[1], str_mode[ input_info.mode[1] ] );

	i3::wstring wstrWindowTitle;
	i3::mb_to_utf16(strWindowTitle, wstrWindowTitle);

	
	CRect rcMainFrame;
	GetWindowRect(&rcMainFrame);		// 메인 프레임 중앙에 올수 있도록 처리해본다..  

	newWnd->Create(0, wstrWindowTitle.c_str(), WS_CAPTION|WS_SYSMENU | WS_THICKFRAME| WS_MINIMIZEBOX|WS_MAXIMIZEBOX		
		, rcMainFrame, NULL);		

	newWnd->SetInfo(input_info, exp, local);
	newWnd->ShowWindow(SW_SHOW);
	newWnd->UpdateWindow();
}

void	CMainFrame::CreateFileCheck_MD5Frame(const i3::vector<PackScriptResultPtr>& psr_list)
{
	FileCheck_MD5FrameWnd* newWnd = 
		static_cast<FileCheck_MD5FrameWnd*>(FileCheck_MD5FrameWnd::CreateObject());

	g_wndPopupList.push_back(newWnd);		// 일단, 생성시에 적재..

	i3::wstring wstrWindowTitle = L"MD5 Check Frame Window";

	CRect rcMainFrame;
	GetWindowRect(&rcMainFrame);		// 메인 프레임 중앙에 올수 있도록 처리해본다..  

	newWnd->Create(0, wstrWindowTitle.c_str(), WS_CAPTION|WS_SYSMENU | WS_THICKFRAME| WS_MINIMIZEBOX|WS_MAXIMIZEBOX		
		, rcMainFrame, NULL);		

	newWnd->SetInfo(psr_list);

	newWnd->ShowWindow(SW_SHOW);
	newWnd->UpdateWindow();
}

void	CMainFrame::CreateFileCheck_MD5Frame(const i3::vector<RSCFileResultPtr>& rsc_list)
{
	FileCheck_MD5FrameWnd* newWnd = 
		static_cast<FileCheck_MD5FrameWnd*>(FileCheck_MD5FrameWnd::CreateObject());

	g_wndPopupList.push_back(newWnd);		// 일단, 생성시에 적재..

	i3::wstring wstrWindowTitle = L"MD5 Check Frame Window";

	CRect rcMainFrame;
	GetWindowRect(&rcMainFrame);		// 메인 프레임 중앙에 올수 있도록 처리해본다..  

	newWnd->Create(0, wstrWindowTitle.c_str(), WS_CAPTION|WS_SYSMENU | WS_THICKFRAME| WS_MINIMIZEBOX|WS_MAXIMIZEBOX		
		, rcMainFrame, NULL);		

	newWnd->SetInfo(rsc_list);

	newWnd->ShowWindow(SW_SHOW);
	newWnd->UpdateWindow();
}


ExecutePackFrameWnd*	CMainFrame::GetExecutePackFrameWnd()
{
	if ( m_ExecutePackFrameWnd )
	{
		m_ExecutePackFrameWnd->SetActiveWindow();
		m_ExecutePackFrameWnd->SetFocus();
		return m_ExecutePackFrameWnd;
	}

	ExecutePackFrameWnd* newWnd = static_cast<ExecutePackFrameWnd*>(ExecutePackFrameWnd::CreateObject());
	g_wndPopupList.push_back(newWnd);
	
	i3::wstring wstrName = L"Packing Dialog";



	newWnd->Create(0, wstrName.c_str(), WS_CAPTION|WS_SYSMENU | WS_THICKFRAME| WS_MINIMIZEBOX|WS_MAXIMIZEBOX,
		CRect(0,0,1024,768), NULL);


	m_ExecutePackFrameWnd = newWnd;
	
	//	CRect rcMainFrame;
	//	GetWindowRect(&rcMainFrame);
	CRect rcWnd;
	CWnd* pActiveWnd = GetActiveWindow();
		//::GetActiveFrame();
	pActiveWnd->GetWindowRect(&rcWnd);
	
	CPoint center = rcWnd.CenterPoint();
	
	rcWnd.left = center.x - 1024/2;
	rcWnd.top  = center.y - 768/2;
	rcWnd.right = rcWnd.left + 1024;
	rcWnd.bottom = rcWnd.top + 768;
//	rcWnd.DeflateRect(400, 50);

	newWnd->MoveWindow(&rcWnd);
	newWnd->ShowWindow(SW_SHOW);
	newWnd->UpdateWindow();

	newWnd->SetActiveWindow();
	newWnd->SetFocus();

	return newWnd;
}
/*
PackDlg*	CMainFrame::GetPackDlg() 
{
	if (!m_currPackDlg)
		m_currPackDlg = new PackDlg();

	if (!m_currPackDlg->IsVisible())
	{
		m_currPackDlg->SetVisible();
	}

	return m_currPackDlg;
}
*/

/*
BOOL CMainFrame::OnCloseMiniFrame(CPaneFrameWnd* pWnd) 
{ 
	if ( pWnd->GetPaneCount() == 0 )			// 페인없으면 빨리 스킵..
		return TRUE;

	CDockingManager* docking_mgr = GetDockingManager();		
	if (docking_mgr == NULL) 
		return TRUE;
	
	CWnd* paneWnd =	pWnd->GetPane();
	
	if (paneWnd->IsKindOf( RUNTIME_CLASS(CTabbedPane) ) )
	{
		CTabbedPane* tabbedPane = static_cast<CTabbedPane*>(paneWnd);

		CWnd* ActiveWnd = tabbedPane->GetTabWnd()->GetActiveWnd();
		
		if (ActiveWnd)
		{
			paneWnd = ActiveWnd;
		}
	}

	if (paneWnd->IsKindOf( RUNTIME_CLASS(PackScriptFilePane) ) )
	{
		paneWnd->PostMessage(WM_CLOSE);			// 플로팅된 페인에서 팩스크립트창을 실제로 해제하도록 포스팅.
		return FALSE;							// FALSE를 통해서 디폴트 창닫기 동작이 막히는것 같다..
	}
	
/*	
	CObList paneList;
	docking_mgr->GetPaneList(paneList, TRUE, RUNTIME_CLASS( PackScriptFilePane), TRUE);		// 스크립트파이 페인만 모은다..
																						// 하지만, 3번째인수가 복수개의 타입을 요구하면
																						// 그냥 NULL값을 넣는게 낫다..
	for ( POSITION pos = paneList.GetHeadPosition() ; pos != NULL ; )
	{
		CDockablePane* pane = DYNAMIC_DOWNCAST(CDockablePane, paneList.GetNext(pos) );

		if (pane == NULL) continue;
		
		if (pWnd == pane->GetParentMiniFrame() )
		{
			pane->PostMessage(WM_CLOSE);			// 플로팅된 페인에서 팩스크립트창을 실제로 해제하도록 포스팅.
		}
	}
// 	
	
	return TRUE; 
}
*/

// 아래의 경우는 플로팅 아닌 메인프레임에 도킹된 경우의 창닫힘 콜백이다..
// 이 함수가 필요한 이유는 순전이 TabbedPane때문이다....다른 이유는 없는것 같다..
/*
BOOL CMainFrame::OnCloseDockingPane(CDockablePane* pWnd) 
{ 

	CTabbedPane* pTabbed = DYNAMIC_DOWNCAST(CTabbedPane, pWnd);

	if (pTabbed == NULL )		// Tabbed Pane이 아니라면 평소대로 처리한다.. 여기서는 닫는걸 기준으로 삼는다...
	{
		if (pWnd->IsKindOf(RUNTIME_CLASS(PackScriptFilePane) ) )
		{
			pWnd->ShowWindow(SW_HIDE);
			pWnd->PostMessage(WM_CLOSE);
			AdjustDockingLayout();
		}
		return TRUE;
	}
	
	CMFCTabCtrl* tab_ctrl = pTabbed->GetTabWnd();

	if (tab_ctrl)
	{
		PackScriptFilePane* pane = DYNAMIC_DOWNCAST(PackScriptFilePane, tab_ctrl->GetActiveWnd());
		if (pane)
		{
			pane->ShowWindow(SW_HIDE);
			pane->PostMessage(WM_CLOSE);
			AdjustDockingLayout();
		}
	}

	
/*   이 코드는 모든 탭이 지워지게 되어 곤란하다...
	CObList paneList;
	pTabbed->GetPaneList(paneList);
	
	for ( POSITION pos = paneList.GetHeadPosition() ; pos != NULL ; )
	{
		PackScriptFilePane* pane = DYNAMIC_DOWNCAST(PackScriptFilePane, paneList.GetNext(pos) );

		if (pane != NULL )			// PackScriptFilePane만 따진다..
		{
			pane->ShowWindow(SW_HIDE);
			pane->PostMessage(WM_CLOSE);
			AdjustDockingLayout();
		}
	}
//
	return TRUE; 
}
*/

void CMainFrame::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class


	CMainFrame::DestroyAllPopupWnds();

	CFrameWndEx::PostNcDestroy();

}

void	CMainFrame::SetViews(PackScriptView* view1, ItemView* view2)
{
	m_packscript_view = view1;
	m_item_view = view2;
}


void CMainFrame::DestroyAllPopupWnds()
{
	i3::vector<CFrameWndEx*> copy_list = g_wndPopupList;		// 순회중 내부 삭제가 존재하므로
	// 복사된 컨테이너로 루프를 돌게한다.
	i3::cu::for_each_container(copy_list, i3::mem_fun(&CFrameWndEx::DestroyWindow));

	if (GetMainFrame())
	{
		GetMainFrame()->m_ExecutePackFrameWnd = NULL;
	}
}

void	CMainFrame::DestroyPopupWndFromList(CFrameWndEx* wnd)
{
	i3::vu::remove(g_wndPopupList, wnd);
}

void CMainFrame::DestroyExecuteFrameWndFromList(ExecutePackFrameWnd* wnd)
{
	i3::vu::remove(g_wndPopupList, wnd);
	if (GetMainFrame())
	{
		GetMainFrame()->m_ExecutePackFrameWnd = NULL;
	}
}
