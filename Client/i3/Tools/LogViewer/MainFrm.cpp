
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"

#include "LogViewer.h"
#include "LogViewerDoc.h"
#include "LogViewerView.h"
#include "i3Base/string/ext/atoi.h"
#include "MainFrm.h"

#include "CalenderDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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

	ON_COMMAND(ID_MENU_CALENDER, &CMainFrame::OnMenuCalender)
	ON_COMMAND(ID_MENU_CONFIGDIR, &CMainFrame::OnMenuConfigDir)

	ON_COMMAND(ID_LOG_CONTROL, &CMainFrame::OnLogControl)
	ON_COMMAND(ID_LOG_CAST, &CMainFrame::OnLogCast)
	ON_COMMAND(ID_LOG_CLAN, &CMainFrame::OnLogClan)
	ON_COMMAND(ID_LOG_TRANS, &CMainFrame::OnLogTrans)
	ON_COMMAND(ID_LOG_MESSENGER, &CMainFrame::OnLogMessenger)
	ON_COMMAND(ID_LOG_GAME, &CMainFrame::OnLogGame)
	ON_COMMAND(ID_LOG_DEDI, &CMainFrame::OnLogDedi)
	ON_COMMAND(ID_LOG_DEDI_HACK, &CMainFrame::OnLogDediHack)
	ON_COMMAND(ID_LOG_SIA, &CMainFrame::OnLogSIA)
	ON_COMMAND(ID_LOG_SIA_SERVICE, &CMainFrame::OnLogSIA_SERVICE)
	ON_COMMAND(ID_LOG_ZLOG, &CMainFrame::OnLogZLog)
	ON_COMMAND(ID_LOG_ZLOG_SERVICE, &CMainFrame::OnLogZLog_SERVICE)

	ON_UPDATE_COMMAND_UI(ID_LOG_CONTROL, &CMainFrame::OnUpdateLogControl)
 	ON_UPDATE_COMMAND_UI(ID_LOG_CAST, &CMainFrame::OnUpdateLogCast)
 	ON_UPDATE_COMMAND_UI(ID_LOG_CLAN, &CMainFrame::OnUpdateLogClan)
 	ON_UPDATE_COMMAND_UI(ID_LOG_TRANS, &CMainFrame::OnUpdateLogTrans)
 	ON_UPDATE_COMMAND_UI(ID_LOG_MESSENGER, &CMainFrame::OnUpdateLogMessenger)
 	ON_UPDATE_COMMAND_UI(ID_LOG_GAME, &CMainFrame::OnUpdateLogGame)
 	ON_UPDATE_COMMAND_UI(ID_LOG_DEDI, &CMainFrame::OnUpdateLogDedi)
	ON_UPDATE_COMMAND_UI(ID_LOG_DEDI_HACK, &CMainFrame::OnUpdateLogDediHACK)
	ON_UPDATE_COMMAND_UI(ID_LOG_SIA, &CMainFrame::OnUpdateLogSIA)
	ON_UPDATE_COMMAND_UI(ID_LOG_SIA_SERVICE, &CMainFrame::OnUpdateLogSIA_SERVICE)
	ON_UPDATE_COMMAND_UI(ID_LOG_ZLOG, &CMainFrame::OnUpdateLogZLog)
	ON_UPDATE_COMMAND_UI(ID_LOG_ZLOG_SERVICE, &CMainFrame::OnUpdateLogZLog_SERVICE)
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
}

CMainFrame::~CMainFrame()
{
	i3System::TerminateSys();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	if (!m_wndRibbonBar.Create(this))
	{
		TRACE0("메뉴 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	m_wndRibbonBar.SetPaneStyle(m_wndRibbonBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);
	
	i3MemoryInit();
	i3BaseRegisterMetas();
	CreateRibbon();

	// 메뉴 모음을 활성화해도 포커스가 이동하지 않게 합니다.
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	// 사용자 정의 도구 모음 작업을 허용합니다.
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	CMFCRibbonStatusBarPane* pStatuPane = new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, "준비", TRUE);

	m_wndStatusBar.AddElement(pStatuPane, "준비");
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonProgressBar(ID_STATUSBAR_PANE2), "prograsbar");
	((CMFCRibbonProgressBar*)m_wndStatusBar.GetExElement(0))->SetRange(0, 100);


	// TODO: 도구 모음 및 메뉴 모음을 도킹할 수 없게 하려면 이 다섯 줄을 삭제하십시오.
	EnableDocking(CBRS_ALIGN_ANY);



	// Visual Studio 2005 스타일 도킹 창 동작을 활성화합니다.
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 스타일 도킹 창 자동 숨김 동작을 활성화합니다.
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	// 보관된 값에 따라 비주얼 관리자 및 스타일을 설정합니다.
	OnApplicationLook(theApp.m_nAppLook);

	// 도구 모음 및 도킹 창 메뉴 바꾸기를 활성화합니다.
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

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

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	cs.style &= ~FWS_ADDTOTITLE;

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

void CMainFrame::CreateRibbon()
{
	CMFCRibbonCategory* pCategoryHome = m_wndRibbonBar.AddCategory("Memu", IDB_BITMAP_TOOLBAR_SMALL, IDB_BITMAP_TOOLBAR_LARGE);
	CMFCRibbonPanel * pPanelDirectory = pCategoryHome->AddPanel("PopUp Dialog");

	CMFCRibbonButton * pBtnCalendr = new CMFCRibbonButton(ID_MENU_CALENDER, "Calender", 6, 6, TRUE);
	pPanelDirectory->Add(pBtnCalendr);

	pBtnCalendr = new CMFCRibbonButton(ID_MENU_CONFIGDIR, "Directory", 0, 0, TRUE);
	pPanelDirectory->Add(pBtnCalendr);


	pPanelDirectory = pCategoryHome->AddPanel("Log");

	CMFCRibbonButton * pBtn = new CMFCRibbonButton(ID_LOG_CAST, "Cast", 5, 5, TRUE);
	pPanelDirectory->Add(pBtn);
	pBtn = new CMFCRibbonButton(ID_LOG_CLAN, "Clan", 5, 5, TRUE);
	pPanelDirectory->Add(pBtn);
	pBtn = new CMFCRibbonButton(ID_LOG_CONTROL, "Control", 5, 5, TRUE);
	pPanelDirectory->Add(pBtn);
	pBtn = new CMFCRibbonButton(ID_LOG_DEDI, "Dedi", 5, 5, TRUE);
	pPanelDirectory->Add(pBtn);
	pBtn = new CMFCRibbonButton(ID_LOG_DEDI_HACK, "Dedi_Hack", 5, 5, TRUE);
	pPanelDirectory->Add(pBtn);
	pBtn = new CMFCRibbonButton(ID_LOG_GAME, "Game", 5, 5, TRUE);
	pPanelDirectory->Add(pBtn);
	pBtn = new CMFCRibbonButton(ID_LOG_MESSENGER, "Messenger", 5, 5, TRUE);
	pPanelDirectory->Add(pBtn);
	pBtn = new CMFCRibbonButton(ID_LOG_TRANS, "Trans", 5, 5, TRUE);
	pPanelDirectory->Add(pBtn);
	pBtn = new CMFCRibbonButton(ID_LOG_SIA, "SIA", 5, 5, TRUE);
	pPanelDirectory->Add(pBtn);
	pBtn = new CMFCRibbonButton(ID_LOG_SIA_SERVICE, "SIA_SERVICE", 5, 5, TRUE);
	pPanelDirectory->Add(pBtn);
	pBtn = new CMFCRibbonButton(ID_LOG_ZLOG, "ZLog", 5, 5, TRUE);
	pPanelDirectory->Add(pBtn);
	pBtn = new CMFCRibbonButton(ID_LOG_ZLOG_SERVICE, "ZLog_SERVICE", 5, 5, TRUE);
	pPanelDirectory->Add(pBtn);
	

	m_tmBegin = m_tmEnd = COleDateTime::GetCurrentTime();
}

void CMainFrame::OnMenuCalender()
{
	CCalenderDlg dlg(m_tmBegin, m_tmEnd);

	if( dlg.DoModal() == IDOK)
	{
		m_tmBegin = dlg.m_tmBegin;
		m_tmEnd = dlg.m_tmEnd;

		char str[MAX_PATH];
		i3::sprintf(str, "%d%.2d%.2d%", m_tmBegin.GetYear(), m_tmBegin.GetMonth(), m_tmBegin.GetDay());

		INT32 nBegin = i3::atoi(str);

		i3::sprintf(str, "%d%.2d%.2d%", m_tmEnd.GetYear(), m_tmEnd.GetMonth(), m_tmEnd.GetDay()+1);

		INT32 nEnd = i3::atoi(str);

		if (GetView())
		{
			GetView()->Search(nBegin, nEnd);
		}
	}
}

CLogViewerView* CMainFrame::GetView()
{
	CView* pView = GetActiveView();

	if (pView != NULL)
	{
		if (pView->IsKindOf(RUNTIME_CLASS(CLogViewerView)))
		{
			CLogViewerView* pLogView = (CLogViewerView*)pView;
			return pLogView;
		}
	}

	return NULL;
}

void CMainFrame::OnMenuConfigDir()
{
	if (GetView())
	{
		i3TDKFolderSelectDialog dlg;

		i3::string strPrevWorkingDirectory = GetView()->GetWorkingDirectory();
		if (dlg.Execute(m_hWnd, LPCTSTR(strPrevWorkingDirectory.c_str()), BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT, 0, 0, LPCTSTR(strPrevWorkingDirectory.c_str())))
		{
			GetView()->SetWorkingDirectory(dlg.GetSelectedFolderPath());
		}
	}
}
void CMainFrame::OnLogControl()
{
	if (GetView())
	{
		GetView()->SetLogName("Control");
	}
}
void CMainFrame::OnLogCast()
{
	if (GetView())
	{
		GetView()->SetLogName("Cast");
	}
}
void CMainFrame::OnLogClan()
{
	if (GetView())
	{
		GetView()->SetLogName("Clan");
	}
}
void CMainFrame::OnLogTrans()
{
	if (GetView())
	{
		GetView()->SetLogName("Trans");
	}
}
void CMainFrame::OnLogMessenger()
{
	if (GetView())
	{
		GetView()->SetLogName("Messenger");
	}
}
void CMainFrame::OnLogGame()
{
	if (GetView())
	{
		GetView()->SetLogName("Game");
	}
}
void CMainFrame::OnLogDedi()
{
	if (GetView())
	{
		GetView()->SetDediHack(false);
		GetView()->SetLogName("Dedi");
	}
}
void CMainFrame::OnLogDediHack()
{
	if (GetView())
	{
		GetView()->SetDediHack(true);
		GetView()->SetLogName("Dedi");
	}
}
void CMainFrame::OnLogSIA()
{
	if (GetView())
	{
		GetView()->SetLogName("SIA");
	}
}
void CMainFrame::OnLogSIA_SERVICE()
{
	if (GetView())
	{
		GetView()->SetLogName("SIA_SERVICE");
	}
}
void CMainFrame::OnLogZLog()
{
	if (GetView())
	{
		GetView()->SetLogName("ZLog");
	}
}
void CMainFrame::OnLogZLog_SERVICE()
{
	if (GetView())
	{
		GetView()->SetLogName("ZLog_SERVICE");
	}
}
void CMainFrame::OnUpdateLogControl(CCmdUI *pCmdUI)
{
	if (GetView())
	{
		if (i3::generic_compare(GetView()->GetLogName(), "Control") == 0)
			pCmdUI->SetCheck(BST_CHECKED);
		else
			pCmdUI->SetCheck(BST_UNCHECKED);
	}
}

void CMainFrame::OnUpdateLogCast(CCmdUI *pCmdUI)
{
	if (GetView())
	{
		if( i3::generic_compare(GetView()->GetLogName(), "Cast") == 0)
			pCmdUI->SetCheck(BST_CHECKED);
		else
			pCmdUI->SetCheck(BST_UNCHECKED);
	}
}

void CMainFrame::OnUpdateLogClan(CCmdUI *pCmdUI)
{
	if (GetView())
	{
		if (i3::generic_compare(GetView()->GetLogName(), "Clan") == 0)
			pCmdUI->SetCheck(BST_CHECKED);
		else
			pCmdUI->SetCheck(BST_UNCHECKED);
	}
}
void CMainFrame::OnUpdateLogTrans(CCmdUI *pCmdUI)
{
	if (GetView())
	{
		if (i3::generic_compare(GetView()->GetLogName(), "Trans") == 0)
			pCmdUI->SetCheck(BST_CHECKED);
		else
			pCmdUI->SetCheck(BST_UNCHECKED);
	}
}

void CMainFrame::OnUpdateLogMessenger(CCmdUI *pCmdUI)
{
	if (GetView())
	{
		if (i3::generic_compare(GetView()->GetLogName(), "Messenger") == 0)
			pCmdUI->SetCheck(BST_CHECKED);
		else
			pCmdUI->SetCheck(BST_UNCHECKED);
	}
}

void CMainFrame::OnUpdateLogGame(CCmdUI *pCmdUI)
{
	if (GetView())
	{
		if (i3::generic_compare(GetView()->GetLogName(), "Game") == 0)
			pCmdUI->SetCheck(BST_CHECKED);
		else
			pCmdUI->SetCheck(BST_UNCHECKED);
	}
}

void CMainFrame::OnUpdateLogDedi(CCmdUI *pCmdUI)
{
	if (GetView())
	{
		if (i3::generic_compare(GetView()->GetLogName(), "Dedi") == 0 
			&& GetView()->GetDediHack() == false)
			pCmdUI->SetCheck(BST_CHECKED);
		else
			pCmdUI->SetCheck(BST_UNCHECKED);
	}
}

void CMainFrame::OnUpdateLogDediHACK(CCmdUI * pCmdUI)
{
	if (GetView())
	{
		if (i3::generic_compare(GetView()->GetLogName(), "Dedi") == 0 
			&& GetView()->GetDediHack() == true)
			pCmdUI->SetCheck(BST_CHECKED);
		else
			pCmdUI->SetCheck(BST_UNCHECKED);
	}
}

void CMainFrame::OnUpdateLogSIA(CCmdUI *pCmdUI)
{
	if (GetView())
	{
		if (i3::generic_compare(GetView()->GetLogName(), "SIA") == 0)
			pCmdUI->SetCheck(BST_CHECKED);
		else
			pCmdUI->SetCheck(BST_UNCHECKED);
	}
}

void CMainFrame::OnUpdateLogSIA_SERVICE(CCmdUI *pCmdUI)
{
	if (GetView())
	{
		if (i3::generic_compare(GetView()->GetLogName(), "SIA_SERVICE") == 0)
			pCmdUI->SetCheck(BST_CHECKED);
		else
			pCmdUI->SetCheck(BST_UNCHECKED);
	}
}

void CMainFrame::OnUpdateLogZLog(CCmdUI *pCmdUI)
{
	if (GetView())
	{
		if (i3::generic_compare(GetView()->GetLogName(), "ZLog") == 0)
			pCmdUI->SetCheck(BST_CHECKED);
		else
			pCmdUI->SetCheck(BST_UNCHECKED);
	}
}

void CMainFrame::OnUpdateLogZLog_SERVICE(CCmdUI *pCmdUI)
{
	if (GetView())
	{
		if (i3::generic_compare(GetView()->GetLogName(), "ZLog_SERVICE") == 0)
			pCmdUI->SetCheck(BST_CHECKED);
		else
			pCmdUI->SetCheck(BST_UNCHECKED);
	}
}

void CMainFrame::SetStateText(const char* pchText)
{
	m_wndStatusBar.GetElement(0)->SetText(pchText);
	m_wndStatusBar.RecalcLayout();
	m_wndStatusBar.Invalidate();
}

void CMainFrame::SetPrograssBar(INT32 nPos)
{
	((CMFCRibbonProgressBar*)m_wndStatusBar.GetExElement(0))->SetPos(nPos);
}

void CMainFrame::SetPrograssRange(INT32 nMin, INT32 nMax)
{
	((CMFCRibbonProgressBar*)m_wndStatusBar.GetExElement(0))->SetRange(nMin, nMax);
}