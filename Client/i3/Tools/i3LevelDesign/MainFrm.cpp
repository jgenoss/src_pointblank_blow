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
#include "i3LevelDesign.h"
#include "MainFrm.h"
#include "PathExporter.h"
#include "OptionDlg.h"
#include "Dlg_Debug_Sg.h"
#include "i3LevelDesignDoc.h"
#include "i3LevelDesignView.h"
#include "GlobalVariable.h"
#include "DlgHitMapTimeSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define _CRTDBG_MAP_ALLOC
//#include "crtdbg.h"

static BOOL		s_bFirstShow = TRUE;

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnUpdateApplicationLook)
	
	ON_UPDATE_COMMAND_UI_RANGE(ID_RIBBON_SELECTION, ID_RIBBON_PATHEDIT, &CMainFrame::OnUpdateRibbonCtrlMode)	
	
	ON_COMMAND(ID_FILE_PRINT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnUpdateFilePrintPreview)
	ON_WM_DROPFILES()
	ON_COMMAND(ID_TOOLS_EXPORTPATH, &CMainFrame::OnToolsExportpath)
	ON_COMMAND(ID_EDIT_ADDLIGHT, &CMainFrame::OnEditAddlight)
	ON_COMMAND(ID_TOOLS_OPTIONS, &CMainFrame::OnToolsOptions)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_DEBUG_SCENE, &CMainFrame::OnDebugScene)
	ON_COMMAND(ID_RIBBON_MODE, &CMainFrame::OnBtnChangeMode)
	ON_COMMAND(ID_RIBBON_SELECTION, &CMainFrame::OnBtnElmSelection)
	ON_COMMAND(ID_RIBBON_TRANSLATION, &CMainFrame::OnBtnElmTranslation)
	ON_COMMAND(ID_RIBBON_ROTATION, &CMainFrame::OnBtnElmRotation)
	ON_COMMAND(ID_RIBBON_SCALE, &CMainFrame::OnBtnElmScale)
	ON_COMMAND(ID_RIBBON_PATHEDIT, &CMainFrame::OnBtnElmPathedit)
	ON_COMMAND(ID_RIBBON_OBJBRUSH, &CMainFrame::OnBtnElmObjbrush)
	ON_COMMAND(IDC_BTN_ELM_LIST, &CMainFrame::OnBtnElmList)
	ON_COMMAND(IDC_BTN_ELM_EXPORT, &CMainFrame::OnBtnElmExport)
	ON_COMMAND(IDC_BTN_COLLISION_EDIT, &CMainFrame::OnBtnCollisionEdit)
	ON_COMMAND(IDC_BTN_LAYER, &CMainFrame::OnBtnLayer)
	ON_COMMAND(IDC_BTN_OPTION, &CMainFrame::OnBtnOption)
	ON_COMMAND(ID_EDIT_UNDO, &CMainFrame::OnBtnElmUndo)
	ON_COMMAND(ID_EDIT_REDO, &CMainFrame::OnBtnElmRedo)
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_EDIT_COPY, &CMainFrame::OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, &CMainFrame::OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, &CMainFrame::OnEditPaste)
	ON_COMMAND(ID_EDIT_REASSIGN_RES_NAME, &CMainFrame::OnEditReassignResName)
	ON_COMMAND(ID_EDIT_ESCAPE, &CMainFrame::OnBtnEditEscape)

	ON_COMMAND(IDC_BTN_ELM_UNDO, &CMainFrame::OnBtnElmUndo)
	ON_COMMAND(IDC_BTN_ELM_REDO, &CMainFrame::OnBtnElmRedo)
	ON_COMMAND( IDC_BTN_ELM_REDO_CLEAR, &CMainFrame::OnBtnElmUndoListClear)

	ON_UPDATE_COMMAND_UI( IDC_BTN_ELM_UNDO, &CMainFrame::OnUpdateRibbonUndo)
	ON_UPDATE_COMMAND_UI( IDC_BTN_ELM_REDO, &CMainFrame::OnUpdateRibbonRedo)
	ON_UPDATE_COMMAND_UI( IDC_BTN_ELM_REDO_CLEAR, &CMainFrame::OnUpdateRibbonUndoListClear)

	ON_CONTROL(0, ID_SLIDER_BLOOMINTENSITY,			&CMainFrame::OnChanged_BloomIntensity)
	ON_UPDATE_COMMAND_UI(ID_SLIDER_BLOOMINTENSITY,	&CMainFrame::OnUpdate_BloomIntensity)

	ON_CONTROL(0, ID_SLIDER_BLOOMTHRESHOLD,			&CMainFrame::OnChanged_BloomThreshold)
	ON_UPDATE_COMMAND_UI(ID_SLIDER_BLOOMTHRESHOLD,	&CMainFrame::OnUpdate_BloomThreshold)

	ON_BN_CLICKED(ID_CHECK_BLOOM,					&CMainFrame::OnClicked_BloomEnable)
	ON_UPDATE_COMMAND_UI(ID_CHECK_BLOOM,			&CMainFrame::OnUpdate_BloomEnable)

	ON_CONTROL(0, ID_EDIT_BLURITERATION,			&CMainFrame::OnChanged_BlurIteration)
	ON_UPDATE_COMMAND_UI(ID_EDIT_BLURITERATION,		&CMainFrame::OnUpdate_BlurIteration)

	ON_CONTROL( 0,	ID_SLIDER_GAUSSIANTHETA,		&CMainFrame::OnChanged_BlurTheta)
	ON_UPDATE_COMMAND_UI(ID_SLIDER_GAUSSIANTHETA,	&CMainFrame::OnUpdate_BlurTheta)

	ON_COMMAND( ID_SPACE_MODE,	&CMainFrame::OnSpaceMode)
	ON_UPDATE_COMMAND_UI( ID_SPACE_MODE, &CMainFrame::OnUpdate_SpaceMode)
	ON_COMMAND(IDC_BTN_HITMAP_TIMESET, &CMainFrame::OnBtn_LoadHitMap)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLACK);
	m_pFramework = NULL;
	i3UI::setToolDevMode(true);
}

CMainFrame::~CMainFrame()
{
	delete m_pHitMapMng;

	if( i3Level::GetScene() != NULL)
	{
		i3Level::SetScene( NULL);				// 풀었다가 다시 막는다..애초에 호출되지 않았을때도 Scene쪽 릭이 심각하진 않았음.
	}
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	{
		m_DlgLayer.Create( CDlgLayer::IDD, this);

		m_DlgLayer.ShowWindow( SW_HIDE);
	}

	m_wndRibbonBar.Create(this);
	InitializeRibbon();

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	{
		HICON hIcon;
		HINSTANCE hInst = AfxGetApp()->m_hInstance;

		hIcon = (HICON) ::LoadImage( hInst, MAKEINTRESOURCE( IDR_MAINFRAME), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
		::SetClassLong( m_hWnd, GCL_HICONSM, (LONG) hIcon);

		hIcon = (HICON) ::LoadImage( hInst, MAKEINTRESOURCE( IDR_MAINFRAME), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
		::SetClassLong( m_hWnd, GCL_HICON, (LONG) hIcon);
	}

	CreateDockingWindows();
	DragAcceptFiles( TRUE);
	

	m_pFramework = i3LevelViewport::GetCurrentFocusViewport()->getFramework();

	i3Level::Create( g_pProfile->m_szResPath);

	m_PanelRes.BuildResTree();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;

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
	m_MainButton.SetImage( IDR_MAINFRAME);
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
	CMFCRibbonButton* pVisualStyleButton = new CMFCRibbonButton( 0xFFFFFFFF, strTemp, -1, -1);

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

	//InitStageInfo();
}

void CMainFrame::CreateDockingWindows(void)
{
	{
		DWORD dwStyle;

		dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_FLOAT_MULTI;

		// Resource
		{
			m_DockRes.Create( "Resouces", this, CRect( 0, 0, 200, 200), TRUE, ID_PANEL_RES, dwStyle | CBRS_LEFT);
			m_PanelRes.Create( CPanelResource::IDD, &m_DockRes);
		}

		// Properties
		{
			m_DockProperty.Create( "Properties", this, CRect( 0, 0, 200, 200), TRUE, ID_PANEL_PROPERTY, dwStyle | CBRS_RIGHT);
			m_PanelProperty.Create( CPanelProperty::IDD, &m_DockProperty);
		}

		// HitMap Generator
		{
			m_DockHitMap.Create("HitMap", this, CRect(0, 0, 200, 200), TRUE, ID_PANEL_HITMAP, dwStyle | CBRS_RIGHT);
			m_PanelHitMap.Create(CPanelHitMap::IDD, &m_DockHitMap);
		}

		// Transform
		{
			m_DockTransform.Create( "Transform", this, CRect( 0, 0, 200, 60), TRUE, ID_PANEL_TRANSFORM, dwStyle | CBRS_BOTTOM);
			m_PanelTransform.Create( CPanelTransform::IDD, &m_DockTransform);
		}

		// Navigation Mesh
		{
			m_DockNavMesh.Create( "Navigation Mesh", this, CRect( 0, 0, 200, 200), TRUE, ID_PANEL_NAVMESH, dwStyle | CBRS_RIGHT );
			m_PanelNavMesh.Create( CPanelNavMesh::IDD, &m_DockNavMesh );
		}

		m_DockRes.EnableDocking( CBRS_ALIGN_ANY);
		m_DockProperty.EnableDocking( CBRS_ALIGN_ANY);
		m_DockHitMap.EnableDocking(CBRS_ALIGN_ANY);
		m_DockTransform.EnableDocking( CBRS_ALIGN_ANY);
		m_DockNavMesh.EnableDocking( CBRS_ALIGN_ANY);
		
		DockPane( &m_DockRes);
		DockPane( &m_DockNavMesh);
		DockPane( &m_DockTransform);		

		CDockablePane* ptemp = NULL;
		m_DockProperty.AttachToTabWnd( &m_DockNavMesh, DM_SHOW, TRUE, &ptemp );
		m_DockHitMap.AttachToTabWnd(&m_DockNavMesh, DM_SHOW, TRUE, &ptemp);
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

void CMainFrame::OnFilePrint()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_PRINT);
	}
}

void CMainFrame::OnFilePrintPreview()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);  // force Print Preview mode closed
	}
}

void CMainFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(IsPrintPreview());
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	int Count, i;
	char TempName[MAX_PATH << 1];

	Count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	for( i = 0; i < Count; i++)
	{
		DragQueryFile( hDropInfo, i, TempName, sizeof(TempName) - 1);

		DropRes( TempName);
	}

	g_pResDB->FlushDB();

	DragFinish( hDropInfo);
}



void CMainFrame::OnToolsExportpath()
{
	CFileDialog	Dlg( FALSE, "pth", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"Path Export File(*.pth)|*.pth||", this);

	if( Dlg.DoModal() != IDOK )
	{
		//Message 
		return; 
	}

	CPathExporter exp;

	exp.Export( LPCTSTR( Dlg.GetPathName()));
}

void CMainFrame::OnEditAddlight()
{
	i3LevelLight * pLight = i3LevelLight::new_object();
	i3LevelScene * pScene = i3Level::GetScene();

	pScene->AddElement( pLight, "Light");
}

void CMainFrame::OnToolsOptions()
{
	COptionDlg Dlg;
	if( Dlg.DoModal() == IDOK)
	{

	}
}


bool CMainFrame::_IsInside( CWnd * pWnd, POINT pt)
{
	CRect rt;

	pWnd->GetWindowRect( &rt);

	if( rt.PtInRect( pt))
		return true;

	return false;
}

bool CMainFrame::DropObject( i3LevelRes * pRes, POINT scrPt)
{
	/// 위치를 기준으로 어느 Panel에 Drop된 것인지를 확인한다.

	Ci3LevelDesignView * pView = (Ci3LevelDesignView *) GetActiveView();

	if( _IsInside( pView, scrPt))
	{
		pView->ScreenToClient( &scrPt);
		pView->DropObject( pRes, scrPt);
	}

	return true;
}

bool CMainFrame::DropRes( const char * pszResPath)
{
	i3LevelRes * pRes = g_pResDB->FindResByPath( pszResPath);
	CPoint pt;

	GetCursorPos( &pt);

	if( pRes == NULL)
	{
		// 새로운 Resource.
		pRes = g_pResDB->AddRes( pszResPath);
	}

	if( pRes == NULL)
	{
		NotifyBox( m_hWnd, "%s 파일을 추가할 수 없습니다.", pszResPath);
		return false;
	}
		

	return DropObject( pRes, pt);
}

void CMainFrame::OnDestroy()
{	
	CFrameWndEx::OnDestroy();
}

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFrameWndEx::OnShowWindow(bShow, nStatus);

	/*
	if( bShow && s_bFirstShow)
	{
		s_bFirstShow = FALSE;

		{
			char szPath[MAX_PATH];

			sprintf( szPath, "%s/%s", g_pProfile->m_szResPath, RESDB_FILENAME);

			if( i3Level::LoadDB( szPath) == false)
			{
				if( i3System::IsDirectory( g_pProfile->m_szResPath) == FALSE)
				{
					AfxMessageBox( "Resource Path가 올바르지 않습니다. (-_-) 우선 Option에서 폴더 설정을 해줘어....");
				}
				else
				{
					int rv = AfxMessageBox( "Resource DB 파일을 찾을 수 없습니다. 작업 폴더에서 자동 검색하시겠습니까?", MB_YESNO);
					
					if( rv == IDYES)
					{
						m_PanelRes.Scan( szPath);
					}
				}
			}
			else
			{
				m_PanelRes.BuildResTree();
			}
		}
	}
	*/
}

void CMainFrame::OnDebugScene()
{
	CDlg_Debug_Sg dlg;

	dlg.DoModal();
}
/*
void CMainFrame::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	//CFrameWndEx::PostNcDestroy();			//
}
*/
void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CFrameWndEx::OnActivate(nState, pWndOther, bMinimized);
}

void CMainFrame::OnEditCopy()
{
	i3LevelFramework * pFramework = (i3LevelFramework *) i3Framework::getCurrentFramework();

	if( pFramework != NULL)
	{
		pFramework->Copy();
	}
}

void CMainFrame::OnEditCut()
{
	i3LevelFramework * pFramework = (i3LevelFramework *) i3Framework::getCurrentFramework();

	if( pFramework != NULL)
	{
		pFramework->Cut();
	}
}

void CMainFrame::OnEditPaste()
{
	i3LevelFramework * pFramework = (i3LevelFramework *) i3Framework::getCurrentFramework();

	if( pFramework != NULL)
	{
		pFramework->Paste();
	}
}

//void	CMainFrame::LevelEditModeChanged( LEVEL_EDIT_MODE editMode)
//{
//	i3LevelFramework* pFramework = (i3LevelFramework *) i3Framework::getCurrentFramework();
//
//	if( pFramework )
//	{
//		pFramework->SetLevelEditMode( editMode );
//	}
//
//	/*if( pFramework )
//		m_PanelNavMesh.LevelEditModeChanged( pFramework->GetLevelEditMode() );
//	else
//		m_PanelNavMesh.LevelEditModeChanged( LEM_NORMAL );*/
//}
