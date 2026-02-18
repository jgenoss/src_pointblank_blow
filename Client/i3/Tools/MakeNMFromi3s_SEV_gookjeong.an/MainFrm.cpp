// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "i3Viewer2.h"
#include "MainFrm.h"
#include "GlobalVar.h"
#include "i3Viewer2Doc.h"
#include "DlgNoticeConvertState.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnUpdateApplicationLook)
	ON_WM_DROPFILES()
	ON_COMMAND(ID_VIEW_NORMAL, &CMainFrame::OnViewNormal)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NORMAL, &CMainFrame::OnUpdateViewNormal)
	ON_COMMAND(ID_VIEW_MESH, &CMainFrame::OnViewMesh)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MESH, &CMainFrame::OnUpdateViewMesh)
	ON_COMMAND(ID_VIEW_COLLIDER, &CMainFrame::OnViewCollider)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COLLIDER, &CMainFrame::OnUpdateViewCollider)
	ON_COMMAND(ID_VIEW_WIRE, &CMainFrame::OnViewWire)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WIRE, &CMainFrame::OnUpdateViewWire)
	ON_COMMAND(ID_VIEW_SELONLY, &CMainFrame::OnViewSelonly)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SELONLY, &CMainFrame::OnUpdateViewSelonly)
	ON_COMMAND(ID_VIEW_SHADOW, &CMainFrame::OnViewShadow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHADOW, &CMainFrame::OnUpdateViewShadow)
	ON_COMMAND(ID_VIEW_GRID, &CMainFrame::OnViewGrid)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GRID, &CMainFrame::OnUpdateViewGrid)
	ON_COMMAND(ID_TOOLS_GEOMETRY, &CMainFrame::OnToolsGeometry)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_GEOMETRY, &CMainFrame::OnUpdateToolsGeometry)

	ON_COMMAND(ID_TOOLS_TEXTURES, &CMainFrame::OnToolsTextures)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_TEXTURES, &CMainFrame::OnUpdateToolsTextures)

	ON_COMMAND(ID_TOOLS_PROJECTCONFIG, &CMainFrame::OnToolsProjectconfig)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_PROJECTCONFIG, &CMainFrame::OnUpdateToolsProjectconfig)

	ON_COMMAND(ID_TOOLS_PHYSIXTOOL, &CMainFrame::OnToolsPhysics)

	// Environment
	ON_BN_CLICKED( ID_OPTIONS_CLEARCOLOR,			&CMainFrame::OnClicked_ClearColor)
	ON_BN_CLICKED( ID_OPTIONS_AMBIENTCOLOR,			&CMainFrame::OnClicked_AmbientColor)
	ON_BN_CLICKED( ID_OPTIONS_LIGHTCOLOR,			&CMainFrame::OnClicked_LightColor)

	ON_CONTROL( 0,	ID_OPTIONS_LIGHTINTENSITY,		&CMainFrame::OnChanged_LightIntensity)
	ON_UPDATE_COMMAND_UI( ID_OPTIONS_LIGHTINTENSITY,&CMainFrame::OnUpdate_LightIntensity)

	ON_BN_CLICKED( ID_OPTIONS_REFLECTMAP,			&CMainFrame::OnClicked_ReflectMap)
	ON_UPDATE_COMMAND_UI( ID_OPTIONS_REFLECTMAP,	&CMainFrame::OnUpdate_ReflectMap)

	ON_CONTROL( 0,		ID_OPTIONS_SHADOWSIZE,		&CMainFrame::OnCBChange_ShadowSize)
	ON_UPDATE_COMMAND_UI( ID_OPTIONS_SHADOWSIZE,	&CMainFrame::OnUpdate_ShadowSize)

	// Panes
	ON_BN_CLICKED( ID_PANE_SG,						&CMainFrame::OnClicked_PaneSg)
	ON_UPDATE_COMMAND_UI( ID_PANE_SG,				&CMainFrame::OnUpdate_PaneSg)

	ON_BN_CLICKED( ID_PANE_SPEC,					&CMainFrame::OnClicked_PaneSpec)
	ON_UPDATE_COMMAND_UI( ID_PANE_SPEC,				&CMainFrame::OnUpdate_PaneSpec)
END_MESSAGE_MAP()

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2005);
}

CMainFrame::~CMainFrame()
{
}

CDlgNoticeConvertState g_Dlg;

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	m_wndRibbonBar.Create(this);
	InitializeRibbon();

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode( DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Load menu item image (not placed on any standard toolbars):
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}
	
	g_Dlg.Create( IDD_NOTICE_CONVERT_STATE );
	g_Dlg.ShowWindow( SW_SHOW );

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}

void CMainFrame::InitializeRibbon()
{
	BOOL bNameValid;

	CString strTemp;
	bNameValid = strTemp.LoadString(IDS_RIBBON_FILE);
	ASSERT(bNameValid);

	// Load panel images:
	m_PanelImages.SetImageSize(CSize(16, 16));
	m_PanelImages.Load(IDB_BUTTONS);

	// Init main button:
	m_MainButton.SetImage(IDB_MAIN);
	m_MainButton.SetText(_T("\nf"));
	m_MainButton.SetToolTipText(strTemp);

	m_wndRibbonBar.SetApplicationButton( &m_MainButton, CSize (45, 45));
	CMFCRibbonMainPanel* pMainPanel = m_wndRibbonBar.AddMainCategory(strTemp, IDB_FILESMALL, IDB_FILELARGE);

	bNameValid = strTemp.LoadString(IDS_RIBBON_NEW);
	ASSERT(bNameValid);
	pMainPanel->Add(new CMFCRibbonButton(ID_FILE_NEW, strTemp, 0, 0));
	bNameValid = strTemp.LoadString(IDS_RIBBON_OPEN);
	ASSERT(bNameValid);
	pMainPanel->Add(new CMFCRibbonButton(ID_FILE_OPEN, strTemp, 1, 1));
	bNameValid = strTemp.LoadString(IDS_RIBBON_SAVE);
	ASSERT(bNameValid);
	pMainPanel->Add(new CMFCRibbonButton(ID_FILE_SAVE, strTemp, 2, 2));
	bNameValid = strTemp.LoadString(IDS_RIBBON_SAVEAS);
	ASSERT(bNameValid);
	pMainPanel->Add(new CMFCRibbonButton(ID_FILE_SAVE_AS, strTemp, 3, 3));

	pMainPanel->Add(new CMFCRibbonSeparator(TRUE));

	bNameValid = strTemp.LoadString(IDS_RIBBON_CLOSE);
	ASSERT(bNameValid);
	pMainPanel->Add(new CMFCRibbonButton(ID_FILE_CLOSE, strTemp, 9, 9));

	bNameValid = strTemp.LoadString(IDS_RIBBON_RECENT_DOCS);
	ASSERT(bNameValid);
	pMainPanel->AddRecentFilesList(strTemp);

	bNameValid = strTemp.LoadString(IDS_RIBBON_EXIT);
	ASSERT(bNameValid);
	pMainPanel->AddToBottom(new CMFCRibbonMainPanelButton(ID_APP_EXIT, strTemp, 15));

	InitRibbon_Home();

	// Add elements to the right side of tabs:
	bNameValid = strTemp.LoadString(IDS_RIBBON_STYLE);
	ASSERT(bNameValid);
	CMFCRibbonButton* pVisualStyleButton = new CMFCRibbonButton(-1, strTemp, -1, -1);

	pVisualStyleButton->SetMenu(IDR_THEME_MENU, FALSE /* No default command */, TRUE /* Right align */);

	bNameValid = strTemp.LoadString(IDS_RIBBON_STYLE_TIP);
	ASSERT(bNameValid);
	pVisualStyleButton->SetToolTipText(strTemp);
	bNameValid = strTemp.LoadString(IDS_RIBBON_STYLE_DESC);
	ASSERT(bNameValid);
	pVisualStyleButton->SetDescription(strTemp);
	m_wndRibbonBar.AddToTabs(pVisualStyleButton);

	// Add quick access toolbar commands:
	CList<UINT, UINT> lstQATCmds;

	lstQATCmds.AddTail(ID_FILE_NEW);
	lstQATCmds.AddTail(ID_FILE_OPEN);
	lstQATCmds.AddTail(ID_FILE_SAVE);
	lstQATCmds.AddTail(ID_FILE_PRINT_DIRECT);

	m_wndRibbonBar.SetQuickAccessCommands(lstQATCmds);
	m_wndRibbonBar.AddToTabs(new CMFCRibbonButton(ID_APP_ABOUT, _T("\na"), m_PanelImages.ExtractIcon (0)));
}

BOOL CMainFrame::CreateDockingWindows()
{
	{
		DWORD dwStyle;

		dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_FLOAT_MULTI;

		m_Panel_Sg.Create( "SceneGraph", this, CRect( 0, 0, 200, 200), TRUE, ID_PANEL_SG, dwStyle | CBRS_LEFT);

		m_Dock_Spec.Create( "Specifications", this, CRect( 0, 0, 200, 200), TRUE, ID_PANEL_SPEC, dwStyle | CBRS_RIGHT);
		if( m_Panel_Spec.Create( IDD_PANEL_SPEC, &m_Dock_Spec) == FALSE)
		{
			I3TRACE( "Error\n");
		}

		m_Panel_Sg.EnableDocking(CBRS_ALIGN_ANY);
		m_Dock_Spec.EnableDocking(CBRS_ALIGN_ANY);


		DockPane( &m_Panel_Sg);
		DockPane( &m_Dock_Spec);
	}

	DragAcceptFiles( TRUE);

	SetDockingWindowIcons( theApp.m_bHiColorIcons);

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


void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	int count; 
	char FileName[256];

	count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	DragQueryFile( hDropInfo, 0, FileName, sizeof(FileName) - 1);

	Ci3Viewer2Doc * pDoc = (Ci3Viewer2Doc *) GetActiveDocument();

	pDoc->LoadFile( (char *)LPCTSTR(FileName));
}

class i3TDKRibbonBar : public CMFCRibbonBar
{
public:
	void		Refresh( CFrameWnd * pTarget)
	{
		OnUpdateCmdUI( pTarget, FALSE);
	}
};

void CMainFrame::RefreshRibbon(void)
{
	i3TDKRibbonBar * pBar = (i3TDKRibbonBar *) &m_wndRibbonBar;

	pBar->Refresh( this);
}
