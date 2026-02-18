
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "i3UIAnimation.h"
#include "MainFrm.h"
#include "GlobalInstance.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ID_WND_PANE_UVPROPERTY		101
#define ID_WND_PANE_TEXTUREVIEW		102
#define ID_WND_PANE_IMAGELIST		103
#define ID_WND_PANE_PIECELIST		104
#define ID_WND_PANE_TIMELINE		105
#define ID_WND_PANE_ANIMATIONVIEW	106
#define ID_WND_PANE_ANIMATIONLIST	107
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_WM_SETTINGCHANGE()
	ON_WM_SIZE()
	ON_WM_MOVE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);

	m_pTextureViewer = NULL;
	m_pPropertyPane = NULL;
	m_pImageListPane = NULL;
	m_pPieceListPane = NULL;
	m_pTimeLinePane = NULL;
	m_pAnimationViewPane = NULL;
	m_pAnimationListPane = NULL;
}

CMainFrame::~CMainFrame()
{
	delete CGlobalInstance::getSingletonPtr();

	I3_SAFE_DELETE( m_pPropertyPane);
	I3_SAFE_DELETE(m_pTextureViewer);
	I3_SAFE_DELETE(m_pImageListPane);
	I3_SAFE_DELETE(m_pPieceListPane);
	I3_SAFE_RELEASE(m_pViewer);
	I3_SAFE_DELETE(m_pTimeLinePane);
	I3_SAFE_DELETE(m_pAnimationViewPane);
	I3_SAFE_DELETE(m_pAnimationListPane);
	i3System::TerminateSys();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("메뉴 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 메뉴 모음을 활성화해도 포커스가 이동하지 않게 합니다.
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	m_wndToolBar.SetWindowText("UVAnimation");

	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, "사용자 지정...");

	// 사용자 정의 도구 모음 작업을 허용합니다.
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));

	// TODO: 도구 모음 및 메뉴 모음을 도킹할 수 없게 하려면 이 다섯 줄을 삭제하십시오.
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// Visual Studio 2005 스타일 도킹 창 동작을 활성화합니다.
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 스타일 도킹 창 자동 숨김 동작을 활성화합니다.
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 메뉴 항목 이미지를 로드합니다(표준 도구 모음에 없음).
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// 도킹 창을 만듭니다.
	if (!CreateDockingWindows())
	{
		TRACE0("도킹 창을 만들지 못했습니다.\n");
		return -1;
	}

	CreateRenderer();


	// 보관된 값에 따라 비주얼 관리자 및 스타일을 설정합니다.
	OnApplicationLook(theApp.m_nAppLook);

	// 도구 모음 및 도킹 창 메뉴 바꾸기를 활성화합니다.
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, "사용자 지정...", ID_VIEW_TOOLBAR);

	// 빠른(<Alt> 키를 누른 채 끌기) 도구 모음 사용자 지정을 활성화합니다.
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// 사용자 정의 도구 모음 이미지를 로드합니다.
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// 메뉴 개인 설정을 활성화합니다(가장 최근에 사용한 명령).
	// TODO: 사용자의 기본 명령을 정의하여 각 풀다운 메뉴에 하나 이상의 기본 명령을 포함시킵니다.
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
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
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);
	lstBasicCommands.AddTail(ID_SORTING_SORTALPHABETIC);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYTYPE);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYACCESS);
	lstBasicCommands.AddTail(ID_SORTING_GROUPBYTYPE);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWndEx::PreCreateWindow(cs))
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	DWORD dwStyle;

	dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI;
	m_pPropertyPane = new CPropertyPane;
	m_pPropertyPane->Create("Propertys", this, CRect(0, 0, 200, 200), TRUE, ID_WND_PANE_UVPROPERTY, dwStyle);
	m_pPropertyPane->EnableDocking(CBRS_ALIGN_ANY);
	DockPane(m_pPropertyPane);
	m_pPropertyPane->Init();

	CRect rc;
	GetClientRect(rc);
	m_pTextureViewer = new CTextureViewer;
	m_pTextureViewer->Create(WS_CHILD | WS_VISIBLE, this, ID_WND_PANE_TEXTUREVIEW);

	m_pImageListPane = new CImageListPane;
	m_pImageListPane->Create("ImageList", this, CRect(0, 0, 200, 200), TRUE, ID_WND_PANE_IMAGELIST, dwStyle);
	m_pImageListPane->EnableDocking(CBRS_ALIGN_ANY);
	DockPane(m_pImageListPane);
	m_pImageListPane->Init();

	m_pPieceListPane = new CPieceListPane;
	m_pPieceListPane->Create("PieceList", this, CRect(0, 0, 200, 200), TRUE, ID_WND_PANE_PIECELIST, dwStyle);
	m_pPieceListPane->EnableDocking(CBRS_ALIGN_ANY);
	DockPane(m_pPieceListPane);

	m_pTimeLinePane = new CTimeLinePane;
	m_pTimeLinePane->Create("Timeline", this, CRect(0, 0, 200, 200), TRUE, ID_WND_PANE_TIMELINE, dwStyle);
	m_pTimeLinePane->EnableDocking(CBRS_ALIGN_ANY);
	DockPane(m_pTimeLinePane);

	m_pAnimationViewPane = new CAnimationViewPane;
	m_pAnimationViewPane->Create("AnimationView", this, CRect(0, 0, 200, 200), TRUE, ID_WND_PANE_ANIMATIONVIEW, dwStyle);
	m_pAnimationViewPane->EnableDocking(CBRS_ALIGN_ANY);
	DockPane(m_pAnimationViewPane);

	m_pAnimationListPane = new CAnimationListPane;
	m_pAnimationListPane->Create("AnimationList", this, CRect(0, 0, 200, 200), TRUE, ID_WND_PANE_ANIMATIONLIST, dwStyle);
	m_pAnimationListPane->EnableDocking(CBRS_ALIGN_ANY);
	DockPane(m_pAnimationListPane);

	DragAcceptFiles(TRUE);
	SetDockingWindowIcons(theApp.m_bHiColorIcons);

	new CGlobalInstance;
	
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	

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

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp, LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp, lp);
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

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
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
	for (int i = 0; i < iMaxUserToolbars; i++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, "사용자 지정...");
		}
	}

	return TRUE;
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);

}

bool CMainFrame::CreateRenderer(void)
{
	CRect rt;

	GetClientRect(&rt);

	i3VideoInfo video;

	video.m_Width = max(rt.Width(), 1);
	video.m_Height = max(rt.Height(), 1);
	video.m_bFullScreen = FALSE;
	video.m_bMultiThread = TRUE;

	m_pViewer = i3Viewer::new_object();
	m_pViewer->Create(m_hWnd, &video, I3I_DEVICE_MASK_CONTROLLER);

	return true;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWndEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_pTextureViewer)
	{
		if (IsWindow(m_pTextureViewer->m_hWnd))
		{
			GetWindowRect(&m_MainWindowRect);

			m_pTextureViewer->SetWindowPos(NULL, m_ViewWindowRect.left - m_MainWindowRect.left - 4,
				m_ViewWindowRect.top - m_MainWindowRect.top - 26,
				m_ViewWindowRect.Width() - 8,
				m_ViewWindowRect.Height() - 8, SWP_NOACTIVATE | SWP_NOZORDER);
		}
	}
} 

void CMainFrame::OnMove(int x, int y)
{
	CFrameWndEx::OnMove(x, y);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	GetWindowRect(&m_MainWindowRect);
}

