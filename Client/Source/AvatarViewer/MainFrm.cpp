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

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "pch.h"
#include "AvatarViewer.h"

#include "MainFrm.h"
#include "GlobalVar.h"

#include "i3Base/string/ext/extract_fileext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnUpdateApplicationLook)

	ON_COMMAND(ID_VIEW_SHADOW, &CMainFrame::OnViewShadow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHADOW, &CMainFrame::OnUpdateViewShadow)

	// Camera
	ON_COMMAND(ID_CAMERA_SET, &CMainFrame::OnCameraSetBtn)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_SET, &CMainFrame::OnUpdateCameraSetBtn)
	ON_COMMAND(ID_CAMERA_SAVE, &CMainFrame::OnCameraSaveBtn)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_SAVE, &CMainFrame::OnUpdateCameraSaveBtn)
	ON_COMMAND(ID_CAMERA_LOAD, &CMainFrame::OnCameraLoadBtn)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_LOAD, &CMainFrame::OnUpdateCameraLoadBtn)
	ON_COMMAND(ID_CAMERA_1PV, &CMainFrame::OnCamera1PVBtn)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_1PV, &CMainFrame::OnUpdateCamera1PVBtn)

	// View
	ON_COMMAND(ID_VIEW_GRID, &CMainFrame::OnViewGrid)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GRID, &CMainFrame::OnUpdateViewGrid)

	ON_CONTROL( 0,	ID_OPTIONS_LOD,			&CMainFrame::OnChanged_LOD)
	ON_UPDATE_COMMAND_UI( ID_OPTIONS_LOD,	&CMainFrame::OnUpdate_LOD)

	//
	ON_COMMAND(ID_TOOLS_GEOMETRY, &CMainFrame::OnToolsGeometry)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_GEOMETRY, &CMainFrame::OnUpdateToolsGeometry)

	ON_COMMAND(ID_TOOLS_TEXTURES, &CMainFrame::OnToolsTextures)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_TEXTURES, &CMainFrame::OnUpdateToolsTextures)

	ON_COMMAND(ID_TOOLS_PROJECTCONFIG, &CMainFrame::OnToolsProjectconfig)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_PROJECTCONFIG, &CMainFrame::OnUpdateToolsProjectconfig)

	// Environment
	ON_BN_CLICKED( ID_OPTIONS_CLEARCOLOR,			&CMainFrame::OnClicked_ClearColor)
	ON_BN_CLICKED( ID_OPTIONS_AMBIENTCOLOR,			&CMainFrame::OnClicked_AmbientColor)
	ON_BN_CLICKED( ID_OPTIONS_LIGHTCOLOR,			&CMainFrame::OnClicked_LightColor)

	ON_CONTROL( 0,	ID_OPTIONS_LIGHTINTENSITY,		&CMainFrame::OnChanged_LightIntensity)
	ON_UPDATE_COMMAND_UI( ID_OPTIONS_LIGHTINTENSITY,&CMainFrame::OnUpdate_LightIntensity)

	ON_UPDATE_COMMAND_UI(ID_OPTIONS_LIGHTVERTICAL,	&CMainFrame::OnUpdate_LightVertical)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_LIGHTHORIZON,	&CMainFrame::OnUpdate_LightHorizon)

	ON_BN_CLICKED( ID_OPTIONS_REFLECTMAP,			&CMainFrame::OnClicked_ReflectMap)
	ON_UPDATE_COMMAND_UI( ID_OPTIONS_REFLECTMAP,	&CMainFrame::OnUpdate_ReflectMap)

	ON_CONTROL( 0,		ID_OPTIONS_SHADOWSIZE,		&CMainFrame::OnCBChange_ShadowSize)
	ON_UPDATE_COMMAND_UI( ID_OPTIONS_SHADOWSIZE,	&CMainFrame::OnUpdate_ShadowSize)

	// Panes
	ON_BN_CLICKED( ID_VIEW_AVATAR,					&CMainFrame::OnClicked_ViewAvatar)
	ON_UPDATE_COMMAND_UI( ID_VIEW_AVATAR,			&CMainFrame::OnUpdate_ViewAvatar)

	ON_BN_CLICKED( ID_VIEW_ANIM,					&CMainFrame::OnClicked_ViewAnim)
	ON_UPDATE_COMMAND_UI( ID_VIEW_ANIM,				&CMainFrame::OnUpdate_ViewAnim)

	ON_BN_CLICKED( ID_VIEW_PART,					&CMainFrame::OnClicked_ViewPart)
	ON_UPDATE_COMMAND_UI( ID_VIEW_PART,				&CMainFrame::OnUpdate_ViewPart)

	ON_COMMAND(ID_FILE_SAVE, &CMainFrame::OnFileSave)
	ON_COMMAND(ID_FILE_OPEN, &CMainFrame::OnFileLoad)
	ON_WM_DROPFILES()

	// Team select 
	ON_COMMAND(ID_COLOR_BLUE_TEAM, &CMainFrame::OnSelectBlueTeam)
	ON_UPDATE_COMMAND_UI(ID_COLOR_BLUE_TEAM, &CMainFrame::OnUpdateSelectTeamBlue)
	ON_COMMAND(ID_COLOR_RED_TEAM, &CMainFrame::OnSelectRedTeam)
	ON_UPDATE_COMMAND_UI(ID_COLOR_RED_TEAM, &CMainFrame::OnUpdateSelectTeamRed)
	ON_COMMAND(ID_COLOR_OFF, &CMainFrame::OnSelectTeamColorOff)
	ON_UPDATE_COMMAND_UI(ID_COLOR_OFF, &CMainFrame::OnUpdateSelectTeamColorOff)
	
	// attach weapon
	ON_BN_CLICKED( ID_ATTACHMENT_ATTACHRIGHTHAND,					&CMainFrame::OnClicked_AttachWeaponRightHand)
	ON_UPDATE_COMMAND_UI( ID_ATTACHMENT_ATTACHRIGHTHAND,			&CMainFrame::OnUpdate_AttachWeaponRightHand)
	ON_BN_CLICKED( ID_ATTACHMENT_ATTACHBACKPACK,					&CMainFrame::OnClicked_AttachWeaponBackpack)
	ON_UPDATE_COMMAND_UI( ID_ATTACHMENT_ATTACHBACKPACK,				&CMainFrame::OnUpdate_AttachWeaponBackpack)
	ON_CONTROL( 0,	ID_ATTACHMENT_WEAPONLIST,						&CMainFrame::OnChanged_WeaponSelect)
	ON_UPDATE_COMMAND_UI( ID_ATTACHMENT_WEAPONLIST,					&CMainFrame::OnUpdate_WeaponSelect)
	

	// Make
	// Save Merged texture
	ON_COMMAND( ID_MAKE_SAVEMERGEDTEXTURE, &CMainFrame::OnSaveMergedTexture)
	ON_COMMAND( ID_MAKE_CONVERTSGBYMACROTEXTURE, &CMainFrame::OnConvertSGbyMacroTexture)
	ON_COMMAND( ID_MAKE_CONVERTBLENDINDEX, &CMainFrame::OnConvertBlendIndex)

END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
: m_eTeamType(TEAM_RED), m_bCam1PVMode(false)
{
	// TODO: add member initialization code here
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLACK);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
#ifdef _VC_DEBUG
	//_CrtSetBreakAlloc(31144); //여기에 몇번째 메모리 할당이 일어나며 브레이크 포인터를 설정하게 셋팅한다.
#endif
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	m_wndRibbonBar.Create(this);
	I3PRINTLOG(I3LOG_NOTICE, " InitializeRibbon");
	InitializeRibbon();

	I3PRINTLOG(I3LOG_NOTICE, " Create StatusBar");
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	I3PRINTLOG(I3LOG_NOTICE, " Create Panel");
	CreatePanels();


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
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

	m_wndRibbonBar.SetApplicationButton(&m_MainButton, CSize (45, 45));
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

	bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT);
	ASSERT(bNameValid);
	CMFCRibbonButton* pBtnPrint = new CMFCRibbonButton(ID_FILE_PRINT, strTemp, 6, 6);
	pBtnPrint->SetKeys(_T("p"), _T("w"));
	bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT_LABEL);
	ASSERT(bNameValid);
	pBtnPrint->AddSubItem(new CMFCRibbonLabel(strTemp));
	bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT_QUICK);
	ASSERT(bNameValid);
	pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_DIRECT, strTemp, 7, 7, TRUE));
	bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT_PREVIEW);
	ASSERT(bNameValid);
	pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_PREVIEW, strTemp, 8, 8, TRUE));
	bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT_SETUP);
	ASSERT(bNameValid);
	pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_SETUP, strTemp, 11, 11, TRUE));
	pMainPanel->Add(pBtnPrint);
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

void CMainFrame::CreatePanels(void)
{
	{
		DWORD dwStyle;

		dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_FLOAT_MULTI;

		m_Panel_Part.LoadEquipData();

		I3PRINTLOG(I3LOG_NOTICE, "  Create Avatar Panel");
		m_Panel_Avatar.Create( "Avatar Settings", this, CRect( 0, 0, 200, 100), TRUE, ID_PANEL_AVATAR, dwStyle | CBRS_LEFT);
		m_Panel_Avatar.EnableDocking( CBRS_ALIGN_ANY);

		I3PRINTLOG(I3LOG_NOTICE, "  Create Anim Panel");
		m_Panel_Anim.Create( "Animations", this, CRect( 0, 0, 120, 200), TRUE, ID_PANEL_ANIM, dwStyle | CBRS_LEFT);
		m_Panel_Anim.EnableDocking( CBRS_ALIGN_ANY);

		I3PRINTLOG(I3LOG_NOTICE, "  Create Sg Panel");
		m_Panel_Sg.Create( "SceneGraph", this, CRect( 0, 0, 200, 200), TRUE, ID_PANEL_SG, dwStyle | CBRS_LEFT);
		m_Panel_Sg.EnableDocking(CBRS_ALIGN_ANY);

		I3PRINTLOG(I3LOG_NOTICE, "  Create Part Panel");
		m_Panel_Part.Create( "Part Property", this, CRect( 0, 0, 200, 100), TRUE, ID_PANEL_PART, dwStyle | CBRS_LEFT);
		m_Panel_Part.EnableDocking( CBRS_ALIGN_ANY);

		I3PRINTLOG(I3LOG_NOTICE, "  Create CamSet Panel");
		m_Panel_CamSet.Create( "Camera Setting", this, CRect( 0, 0, 200, 80), TRUE, IDD_PANEL_CAMERASET, dwStyle | CBRS_LEFT);
		m_Panel_CamSet.EnableDocking( CBRS_ALIGN_ANY);

		I3PRINTLOG(I3LOG_NOTICE, "  Dock Panel");
		DockPane( &m_Panel_Avatar);
		DockPane( &m_Panel_Anim);
		DockPane( &m_Panel_Sg);
		DockPane( &m_Panel_Part);
		DockPane( &m_Panel_CamSet);
	}

	DragAcceptFiles( TRUE);
}

// CMainFrame diagnostics

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


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
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

void CMainFrame::OnFileSave()
{
	/*TCHAR szFilters[] = "i3s 파일(*.i3s)|*.i3s|";
	CFileDialog Dlg( FALSE, _T("i3s"), _T(""), OFN_HIDEREADONLY, szFilters);

	if( Dlg.DoModal() == IDCANCEL)
		return;

	g_pFramework->SaveI3S( LPCTSTR( Dlg.GetPathName()));*/
	TCHAR szFilters[] = "avp 파일(*.avp)|*.avp|";
	CFileDialog Dlg( FALSE, _T("avp"), _T(""), OFN_HIDEREADONLY, szFilters);

	if( Dlg.DoModal() == IDCANCEL)
		return;

	g_pFramework->SaveProject( LPCTSTR( Dlg.GetPathName()));
}

void CMainFrame::OnFileLoad()
{
	TCHAR szFilters[] = "avp 파일(*.avp)|*.avp|";
	CFileDialog Dlg( TRUE, _T("avp"), _T(""), OFN_HIDEREADONLY, szFilters);

	if( Dlg.DoModal() == IDCANCEL)
		return;

	g_pFramework->LoadProject( LPCTSTR( Dlg.GetPathName()));
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	int Count, i;
	char szTempName[MAX_PATH], szExt[MAX_PATH];

	Count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	m_Panel_Avatar.UpdateAll();

	for( i = 0; i < Count; i++)
	{
		DragQueryFile( hDropInfo, i, szTempName, sizeof(szTempName) - 1);

		i3::extract_fileext( szTempName, szExt);

		if( i3::generic_is_iequal( szExt, "i3s"))
		{
			g_pFramework->LoadPart( szTempName, m_eTeamType);
		}
		else if( i3::generic_is_iequal( szExt, "i3a"))
		{
			g_pFramework->LoadAnimation(NULL,szTempName);
		}
	}

	DragFinish( hDropInfo);
}

void  CMainFrame::ChangePart( EQUIP::ePART ePart, const char * pszPartName, const char * pszFullPath )
{
	if( NULL != m_Panel_Part.m_hWnd )
	{
		m_Panel_Part.UpdatePart( ePart, pszPartName, pszFullPath, m_eTeamType );
	}
	
}