// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를 
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된 
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해 
// 추가적으로 제공되는 내용입니다.  
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.  
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은 
// http://msdn.microsoft.com/officeui를 참조하십시오.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "ItemTool.h"

#include "MainFrm.h"

#include "DockSceneGraphWnd.h"
#include "Chara.h"

#include "i3Base/smart_ptr/make_shared.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// #define _CRTDBG_MAP_ALLOC
// #include "crtdbg.h"

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnUpdateApplicationLook)
	
	// 탭 정의 =====================================================================
	//weapon TAB
	ON_COMMAND(ID_VIEW_WEAPON_TAB, &CMainFrame::OnViewWeaponTab)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WEAPON_TAB, &CMainFrame::OnUpdateViewWeaponTab)
	
	//good TAB
	ON_COMMAND(ID_VIEW_GOODS_TAB, &CMainFrame::OnViewGoodsTab)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GOODS_TAB, &CMainFrame::OnUpdateViewGoodsTab)
	
	//Equip TAB
	ON_COMMAND(ID_VIEW_EQUIPMENT_TAB, &CMainFrame::OnViewEquipmentTab)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EQUIPMENT_TAB, &CMainFrame::OnUpdateViewEquipmentTab)
	
	//Chara TAB
	ON_COMMAND(ID_VIEW_CHARACTER_TAB, &CMainFrame::OnViewCharacterTab)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CHARACTER_TAB, &CMainFrame::OnUpdateViewCharacterTab)
	
	//Avility TAB
	ON_COMMAND(ID_VIEW_ABILITY_TAB, &CMainFrame::OnViewAbilityTab)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ABILITY_TAB, &CMainFrame::OnUpdateViewAbilityTab)
	// ===================================================================== 탭 정의
	
	//Rootpath 정의==================================================================
	//ribbon Path edit
	ON_COMMAND(ID_BTN_WEAPON_EDITOR, &CMainFrame::OnBtnOpenWeaponEditor)
	ON_UPDATE_COMMAND_UI(ID_BTN_WEAPON_EDITOR, &CMainFrame::OnUpdateOpenWeaponEditor)
	
	//ribbon path edit
	ON_COMMAND(IDC_EDIT_PATH, &CMainFrame::OnPath)
	ON_UPDATE_COMMAND_UI(IDC_EDIT_PATH, &CMainFrame::OnUpdatePath)
	ON_EN_SETFOCUS(IDC_EDIT_PATH, &CMainFrame::OnEnSetfocusEditPath)

	//ribbon Path Btn
	ON_COMMAND(IDC_OPEN_PATH_DLG, &CMainFrame::OnPathButton)
	ON_UPDATE_COMMAND_UI(IDC_OPEN_PATH_DLG, &CMainFrame::OnUpdatePathButton)
	//==================================================================Rootpath 정의

	//Uitility TAB 정의 ============================================================
	//ribbon Category combobox
	ON_COMMAND(IDC_COMBO_PANNEL_CATEGORY, &CMainFrame::OnUtilityCategoryCombo)
	ON_UPDATE_COMMAND_UI(IDC_COMBO_PANNEL_CATEGORY, &CMainFrame::OnUpdateUtilityCategoryCombo)

	//ribbon Category Rotate Button
	ON_COMMAND(IDC_CHECK_DO_ROTATE, &CMainFrame::OnUtilityCategoryRotateBtn)
	ON_UPDATE_COMMAND_UI(IDC_CHECK_DO_ROTATE, &CMainFrame::OnUpdateUtilityCategoryRotateBtn)
	//============================================================ Uitility TAB 정의 

	ON_WM_DROPFILES()
	ON_WM_SETFOCUS()
	ON_WM_MOUSEHOVER()
END_MESSAGE_MAP()

char* g_szPefList[MAX_PEFLIST] = 
{
	("Weapon.pef"),
	("Goods.pef"),
	("Equipment.pef"),
	("Character.pef")
};

// CMainFrame 생성/소멸
CMainFrame::CMainFrame()
:	m_nSelectedTabIdx(-1)
,	m_pDlgWeapon(NULL)
,	m_pDlgFileLoader(NULL)
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2005);
}

CMainFrame::~CMainFrame()
{
	I3_SAFE_DELETE(m_pDlgWeapon);
	I3_SAFE_DELETE(m_pDlgFileLoader);

}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;


// 	while(1)
// 	{
// 		if (GetAsyncKeyState(VK_F2))
// 		{
// 			break;
// 		}
// 	}
	
	g_strRootPath = AfxGetApp()->GetProfileString( "Option", "WorkPath", "");
	if (g_strRootPath.IsEmpty())
	{
		GUTIL::SetWorkingFolder();
	}

	if (!g_strRootPath.IsEmpty())
	{
		i3ResourceFile::SetWorkingFolder((LPSTR)(LPCSTR)g_strRootPath);

		char szCurrPath[ MAX_PATH ];
		::GetCurrentDirectory( MAX_PATH, szCurrPath );
		g_strCurrentDir = szCurrPath;

		m_pDlgFileLoader = new cDlgFileLoader();
		m_pDlgFileLoader->DoModal();
	}

	//m_pDlgFileLoader->Create(IDD_DLG_FILE_LOADER,this);
	//m_pDlgFileLoader->ShowWindow(SW_SHOW);
	//m_pDlgFileLoader->OnStartThread();

	OnCreateLater();

	return 0;
}

void CMainFrame::OnCreateLater()
{
	//AfxGetMainWnd();
	//_CrtSetBreakAlloc (11457);

	// 보관된 값에 따라 비주얼 관리자 및 스타일을 설정합니다.
	OnApplicationLook(theApp.m_nAppLook);

	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW | WS_VSCROLL, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		//TRACE0("Failed to create view window\n");
		//return -1;
	}

	InitDockWnd(&m_wndSceneGraph,	"SceneGraph",	CBRS_LEFT,	ID_PANEL_SCENEGRAPH,	IDR_TOOLBAR2);
	//InitDockWnd(&m_wndAttribute,	"Attribute",	CBRS_LEFT,	ID_PANEL_ATTR,			IDR_TOOLBAR2);
	InitDockWnd(&m_wndPropertyEdit,	"PropertyEdit",	CBRS_RIGHT,	ID_PANEL_PROPERTY,		IDR_TOOLBAR2);
 
	CDockingManager::SetDockingMode(DT_SMART);
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	
	DragAcceptFiles(TRUE);

	m_pDlgWeapon = new cDlgWeaponEditor();
	m_pDlgWeapon->Create(IDD_WEAPON_EDIT);
	//m_pDlgWeapon->ShowWindow(SW_SHOW);
	m_pDlgWeapon->SetVisible(false);

	//g_pViewer->SetWeaponHwnd(m_wndView.GetSafeHwnd(),E_SWAPCHAIN_MAIN);
	g_pViewer->SetWeaponHwnd(m_wndSceneGraph.m_SceneViewTree.GetSafeHwnd(),E_SWAPCHAIN_ITEM);

	
//	m_wndSceneGraph.GetSafeHwnd();

	_LoadPefFiles();

	//시작은 무기탭으로 셋팅
	OnViewWeaponTab();
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}

bool CMainFrame::InitDockWnd(cDockBaseWnd* wndDock,CString strName, DWORD dwPosition, UINT resID, UINT resToolBarID)
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_FLOAT_MULTI;

	BOOL hr;
	hr = wndDock->Create(strName, this, CRect(0, 0, 800, 200), TRUE, resID, dwStyle | dwPosition);
	if (hr == FALSE)
		return false;

	//if (resToolBarID != 0)
	//	wndDock->LoadToolBar(resToolBarID);

	wndDock->EnableDocking(CBRS_ALIGN_ANY);
	DockPane(wndDock);

	return true;
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

// 
 CMainFrame *	AfxGetMainFrame()
 {
 	return (CMainFrame *)AfxGetMainWnd();
 }



void CMainFrame::InitStatusBar()
{
//	m_wndStatusBar.SetPaneInfo(0, ID_INDICATOR_STRING1, SBPS_NORMAL | SBPS_NOBORDERS, 0 );
//	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_STRING2, SBPS_NORMAL | SBPS_NOBORDERS, 0 );
//	m_wndStatusBar.SetPaneInfo(2, ID_SEPARATOR, SBPS_NORMAL | SBPS_STRETCH | SBPS_NOBORDERS, 0);
}
void CMainFrame::ShowStatusBar()
{
//	m_wndStatusBar.SetPaneInfo(0, ID_INDICATOR_STRING1, SBPS_NORMAL, 100 );
//	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_STRING2, SBPS_NORMAL, 250 );
}
void CMainFrame::HideStatusBar()
{
//	m_wndStatusBar.SetPaneInfo(0, ID_INDICATOR_STRING1, SBPS_NORMAL | SBPS_NOBORDERS, 0 );
//	m_wndStatusBar.SetPaneText(0, "" );
//	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_STRING2, SBPS_NORMAL | SBPS_NOBORDERS, 0 );
//	m_wndStatusBar.SetPaneText(1, "" );
}

void CMainFrame::ResizeStatusBar(int cxWidth)
{

}

void CMainFrame::ResetStatusText1()
{
//	m_wndStatusBar.SetPaneText( 0, "" );
}
void CMainFrame::ResetStatusText2()
{
//	m_wndStatusBar.SetPaneText( 1, "" );
}
void CMainFrame::ResetStatusText3()
{
//	m_wndStatusBar.SetPaneText( 2, "" );
}

void CMainFrame::SetStatusText1(const CString & Text)
{
//	m_wndStatusBar.SetPaneText( 0, Text );
}
void CMainFrame::SetStatusText2(const CString & Text)
{
//	m_wndStatusBar.SetPaneText( 1, Text );
}
void CMainFrame::SetStatusText3(const CString & Text)
{
//	m_wndStatusBar.SetPaneText( 2, Text );
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	return;

	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
//	int count	= DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	char FileName[256];
	DragQueryFile( hDropInfo, 0, FileName, sizeof(FileName) - 1);

	//	Ci3Viewer2Doc * pDoc = (Ci3Viewer2Doc *) GetActiveDocument();

	//	pDoc->LoadFile( (char *)LPCTSTR(FileName));
}

void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	CFrameWndEx::OnSetFocus(pOldWnd);

	I3TRACE("Mainframe Setfocus\n");
	
//   	if (g_pMainSCFramework)
//   	{
//   		g_pViewer->SetFramework(g_pMainSCFramework);
//   		g_pViewer->SetWndProc(m_wndView.m_ViewerCtrl.GetSafeHwnd());
//   	}
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CMainFrame::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CFrameWndEx::OnMouseHover(nFlags, point);
}


bool CMainFrame::_LoadPefFiles(void)
{
	if (g_strRootPath.IsEmpty())
		return false;

	for( int i = 0; i < MAX_PEFLIST; i++ )
	{
		//Root에서 찾는다..
		//root + config + *.pef;
		CString strPefPath  = g_strRootPath +"\\"+ "config" +"\\"+ g_szPefList[i];

		//F:\pb15\PointBlank_15\Media\config
		if( _LoadPef(strPefPath, static_cast<ePefType>(i)) == false )
		{
			//CString	strMsg;

			//strMsg.Format("Pef File LoadFail - %s", strPefPath);
			//AfxMessageBox(strMsg);
		}
	}

	_SetCurrentType(E_PEF_WEAPON);

	return true;
}

bool CMainFrame::_LoadPef(LPCSTR pFilePath, const ePefType& pef)
{
	if( g_pPefList )
	{
		if( g_pPefList->LoadFile(pFilePath, pef) )
		{
			m_wndPropertyEdit.SetPefData(pFilePath, g_pPefList->getRoot());

			char	szPath[MAX_PATH];

			strcpy_s(szPath, pFilePath);
			PathAppend(szPath,"..\\..\\gui");

			return _LoadImageList(szPath, pef);
		}
	}

	return false;
}

bool CMainFrame::_LoadImageList(LPCSTR pFilePath, const ePefType& pef)
{
	if( pFilePath == NULL || PathFileExists(pFilePath) == FALSE || g_pMainSCFramework == NULL )
		return false;

	return g_pMainSCFramework->LoadTexture(pFilePath, (INT32)pef);
}

void CMainFrame::_SetCurrentType(ePefType type)
{
	if( g_pPefList )
	{
		g_pPefList->SetCurrentType(type);
	}

	if( IsWindow( m_wndView ) )
		m_wndView.ResetItemList();

	g_pItemSCFramework->SetResetCamera(true);
}
