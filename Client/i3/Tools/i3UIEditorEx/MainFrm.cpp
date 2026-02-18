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

#include "stdafx.h"
#include "i3UIEditor.h"

#include "MainFrm.h"
#include "GlobalVariables.h"
#include "i3UIEditorDoc.h"
#include "i3UIEditorView.h"
#include "SelectModuleDlg.h"
#include "SelectSceneDlg.h"
#include "Dlg_SelectTexture.h"

#include "ScriptPane.h"

#include "ScriptManager.h"

#include	"GameStringTable.h"

#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/compare/generic_is_iequal.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/ext/resolve_path_from_environment_variable.h"
#include "i3Base/string/ext/extract_directoryname.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/make_relative_path.h"

#include "i3Framework/i3UI/i3VTFileFormat.h"

#include "i3Framework/i3UI/i3UIFilePathMgr.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnUpdateApplicationLook)

	// File
	ON_COMMAND( ID_FILE_EXPORT, &CMainFrame::OnExport)
	ON_UPDATE_COMMAND_UI( ID_FILE_EXPORT, &CMainFrame::OnUpdateExportUI)
	ON_COMMAND( ID_FILE_EXPORTLIBRARY, &CMainFrame::OnExportLib)
	ON_UPDATE_COMMAND_UI( ID_FILE_EXPORTLIBRARY, &CMainFrame::OnUpdateExportLIbUI)

	ON_WM_DROPFILES()

	// Edit
	ON_COMMAND( ID_EDIT_COPY, &CMainFrame::OnCopy)
	ON_UPDATE_COMMAND_UI( ID_EDIT_COPY, &CMainFrame::OnUpdateCopyUI)
	ON_COMMAND( ID_EDIT_CUT, &CMainFrame::OnCut)
	ON_UPDATE_COMMAND_UI( ID_EDIT_CUT, &CMainFrame::OnUpdateCutUI)
	ON_COMMAND( ID_EDIT_PASTE, &CMainFrame::OnPaste)
	ON_UPDATE_COMMAND_UI( ID_EDIT_PASTE, &CMainFrame::OnUpdatePasteUI)

	
	
	//resolution
	ON_COMMAND_RANGE(ID_SCR_RES_800X600, ID_SCR_RES_1920X1200, &CMainFrame::OnResolutionChange)
	ON_UPDATE_COMMAND_UI( ID_SCR_RES_800X600, &CMainFrame::OnUpdateMenu_Resolution)
	ON_UPDATE_COMMAND_UI( ID_SCR_RES_1024X768, &CMainFrame::OnUpdateMenu_Resolution)
	ON_UPDATE_COMMAND_UI( ID_SCR_RES_1280X800, &CMainFrame::OnUpdateMenu_Resolution)
	ON_UPDATE_COMMAND_UI( ID_SCR_RES_1280X1024, &CMainFrame::OnUpdateMenu_Resolution)
	ON_UPDATE_COMMAND_UI( ID_SCR_RES_1600X1080, &CMainFrame::OnUpdateMenu_Resolution)
	ON_UPDATE_COMMAND_UI( ID_SCR_RES_1920X1080, &CMainFrame::OnUpdateMenu_Resolution)
	ON_UPDATE_COMMAND_UI( ID_SCR_RES_1920X1200, &CMainFrame::OnUpdateMenu_Resolution)


	ON_COMMAND(ID_PROJECT_WORKINGFOLDER, &CMainFrame::OnProjectWorkingfolder)
	ON_COMMAND(ID_PROJECT_EXPORTTEMPLATELIBRARY, &CMainFrame::OnProjectExporttemplatelibrary)
	ON_COMMAND(ID_PROJECT_IMPORTTEMPLATELIBRARY, &CMainFrame::OnProjectImporttemplatelibrary)
	ON_COMMAND(ID_PROJECT_ADJUSTVIRTUALTEXTURE, &CMainFrame::OnProjectAdjustvirtualtexture)
	ON_COMMAND(ID_PROJECT_ADJUSTVTEXLIST, &CMainFrame::OnProjectAdjustVTexList)
	ON_COMMAND(ID_PROJECT_CREATEVIRTUALTEXTURE, &CMainFrame::OnProjectCreatevirtualtexture)
	ON_COMMAND(ID_PROJECT_CREATEVTEXLIST, &CMainFrame::OnProjectCreateVTexList)
	ON_COMMAND(ID_PROJECT_OPENVIRTUALTEXTURE, &CMainFrame::OnProjectOpenvirtualtexture)
	ON_COMMAND(ID_PROJECT_MODIFYVIRTUALTEXTURE, &CMainFrame::OnProjectModifyVirtualtexture)
	ON_COMMAND(ID_PROJECT_EXTRACTREFTEXTURE, &CMainFrame::OnProjectExtractAllRefTexture)
	ON_COMMAND(ID_TEMPLATE_ADDANEWTEMPLATE, &CMainFrame::OnTemplateAddanewtemplate)
	ON_COMMAND(ID_DEBUG_DUMPVIRTUALTEXTURE, &CMainFrame::OnDebugDumpvirtualtexture)
	ON_COMMAND(ID_DEBUG_EXPORTALL, &CMainFrame::OnDebugExportall)
	ON_COMMAND(ID_PROJECT_RUN, &CMainFrame::OnProjectRun)
	ON_COMMAND(ID_PROJECT_STOP, &CMainFrame::OnProjectStop)
	ON_COMMAND(ID_PROJECT_SAVETEMPLATELIBRARY, &CMainFrame::OnProjectSavetemplatelibrary)
	ON_COMMAND(ID_PROJECT_SAVETEMPLATELIBRARY_NEW, &CMainFrame::OnProjectSavetemplatelibrary_NEW)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_RUN, &CMainFrame::OnUpdateProjectRun)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_STOP, &CMainFrame::OnUpdateProjectStop)
	ON_COMMAND( ID_OPTION_EDITOROPTION, &CMainFrame::OnModuleSelect)

	ON_COMMAND( ID_PROJECT_SELECTSCENEFROMCLIENT, &CMainFrame::OnSelectSceneFromClient)

	// Script
	ON_COMMAND(ID_PROJECT_SAVESCRIPT, &CMainFrame::OnSaveScript)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_SAVESCRIPT, &CMainFrame::OnUpdateSaveScript)
	ON_COMMAND(ID_PROJECT_COMPILESCRIPT, &CMainFrame::OnCompileScript)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_COMPILESCRIPT, &CMainFrame::OnUpdateCompileScript)
	
	// View
	ON_COMMAND( ID_VIEW_SCRIPT_PANE, &CMainFrame::OnViewScriptPane)
	ON_COMMAND( ID_VIEW_SCREEN_PANE, &CMainFrame::OnViewScreenPane)
	ON_COMMAND( ID_VIEW_TEMPLATE_PANE, &CMainFrame::OnViewTemplatePane)
	ON_COMMAND( ID_VIEW_TIMELINE_PANE, &CMainFrame::OnViewTimelinePane)
	ON_COMMAND( ID_VIEW_PROPERTIES_PANE, &CMainFrame::OnViewPropertiesPane)
	ON_COMMAND( ID_VIEW_OUTPUT_PANE, OnViewOutputPane)
	ON_COMMAND( ID_VIEW_TEMPLATE_EXPLORER_PANE, &CMainFrame::OnViewTemplateExplorerPane)
	
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCRIPT_PANE, &CMainFrame::OnUpdateViewScriptPane)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCREEN_PANE, &CMainFrame::OnUpdateViewScreenPane)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TEMPLATE_PANE, &CMainFrame::OnUpdateViewTemplatePane)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TIMELINE_PANE, &CMainFrame::OnUpdateViewTimelinePane)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIES_PANE, &CMainFrame::OnUpdateViewPropertiesPane)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUT_PANE, OnUpdateViewOutputPane)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TEMPLATE_EXPLORER_PANE, &CMainFrame::OnUpdateViewTemplateExplorerPane)

	// Align
	ON_COMMAND( ID_EDIT_ALIGNLEFT, &CMainFrame::OnAlignLeft)
	ON_UPDATE_COMMAND_UI( ID_EDIT_ALIGNLEFT, &CMainFrame::OnUpdateAlignButton)
	ON_COMMAND( ID_EDIT_ALIGNRIGHT, &CMainFrame::OnAlignRight)
	ON_UPDATE_COMMAND_UI( ID_EDIT_ALIGNRIGHT, &CMainFrame::OnUpdateAlignButton)
	ON_COMMAND( ID_EDIT_ALIGNTOP, &CMainFrame::OnAlignTop)
	ON_UPDATE_COMMAND_UI( ID_EDIT_ALIGNTOP, &CMainFrame::OnUpdateAlignButton)
	ON_COMMAND( ID_EDIT_ALIGNBOTTOM, &CMainFrame::OnAlignBottom)
	ON_UPDATE_COMMAND_UI( ID_EDIT_ALIGNBOTTOM, &CMainFrame::OnUpdateAlignButton)

	ON_COMMAND( ID_EDIT_RELOCATIONHORZ, &CMainFrame::OnCascadeHorz)
	ON_UPDATE_COMMAND_UI( ID_EDIT_RELOCATIONHORZ, &CMainFrame::OnUpdateAlignButton)

	ON_COMMAND( ID_EDIT_RELOCATIONVERT, &CMainFrame::OnCascadeVert)
	ON_UPDATE_COMMAND_UI( ID_EDIT_RELOCATIONVERT, &CMainFrame::OnUpdateAlignButton)

	// Undo & Redo
	ON_COMMAND( ID_EDIT_UNDO, &CMainFrame::OnUndo)
	ON_COMMAND( ID_EDIT_REDO, &CMainFrame::OnRedo)
	ON_UPDATE_COMMAND_UI( ID_EDIT_UNDO, &CMainFrame::OnUpdateUndoButton)
	ON_UPDATE_COMMAND_UI( ID_EDIT_REDO, &CMainFrame::OnUpdateRedoButton)

	// Snap
	ON_COMMAND( ID_EDIT_SNAPSIZE, &CMainFrame::OnSnapSize)
/*
	ID_EMPTY		
	ID_BRAZIL       
	ID_CHINA        
	ID_INDONESIA    
	ID_ITALY		
	ID_KOREA		
	ID_LATIN		
	ID_MIDDLEEAST	
	ID_PHILIPPINES	
	ID_RUSSIA		
	ID_TAILAND		
	ID_TURKEY
*/
	ON_COMMAND_RANGE(ID_LANGUAGE_DEV, ID_LANGUAGE_JAPAN, &CMainFrame::OnUpdateNation)

	ON_BN_CLICKED( ID_CLEARCOLOR,		&CMainFrame::OnClicked_ClearColor)
	
	END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2005);

	m_pViewer				= NULL;
	m_pFramework			= NULL;

	m_pUIRenderTarget		= NULL;
	m_pLTexture				= NULL;
	m_DCRef					= 0;

//	m_WorkingDirectory[0]	= 0;

	m_StatusBarPane1 = NULL;
	m_StatusBarPane2 = NULL;

	m_nScriptPaneCnt = 0;
	
	m_bFileWatcher = FALSE;

	m_nIDResolution = 0;

	m_nNationSel = ID_LANGUAGE_DEV;

	m_pEditSnapSize = NULL;

	m_pCOL_ClearColor = NULL;
}

CMainFrame::~CMainFrame()
{
	for (size_t i = 0; i < m_PaneList.size(); i++)
		delete m_PaneList[i];

	_ResetTemplateGallery();

	i3UI::setUILibrary( NULL);

	I3_SAFE_RELEASE( m_pLTexture);
	I3_SAFE_RELEASE( m_pUIRenderTarget);

	TerminateFileWatcher();

	m_LuaWatcher.Terminate();

	ScriptManager::ReleaseLuaMemberTable();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	// get working path
	{
		CString str = theApp.GetProfileString( LPCTSTR("Settings"), LPCTSTR("Working Path"));
		CMainFrame::SetWorkingFolder( (const char*)LPCTSTR( str));
	}

	m_wndRibbonBar.Create(this);
	InitializeRibbon();


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

	m_StatusBarPane1 = new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, "Screen View", TRUE);
	m_wndStatusBar.AddElement( m_StatusBarPane1, strTitlePane1);

	m_StatusBarPane2 = new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE);
	m_wndStatusBar.AddExtendedElement( m_StatusBarPane2, strTitlePane2);

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	
	//create panels
	CreatePanels();
	CreateRenderer();

	// get VTex path
	{
		CString str;

		str = theApp.GetProfileString( LPCTSTR("Settings"), LPCTSTR("VirtualTexture Path"));

		SetVirtualTexturePath( (const char*)LPCTSTR( str));
	}

	checkWorkingFolder();
	//checkVirtualTexture();		// 일단 방치..

	//Build Lua Intellisense
	char szLuaRoot[MAX_PATH];
//	char szEnginepath[MAX_PATH];	
//	char szGamePath[MAX_PATH];		// 포인트 블랭크용 환경변수를 하나 더 씀..11.05.31.수빈.

//	i3System::ResolvePath("$(I3_ROOT)", szEnginepath, MAX_PATH);
//	i3System::ResolvePath("$(PB_ROOT)", szGamePath, MAX_PATH);
	
	i3::stack_string	strEnginePath, strGamePath;

	i3::resolve_path_from_environment_variable( "$(I3_ROOT)", strEnginePath);
	i3::resolve_path_from_environment_variable( "$(PB_ROOT)", strGamePath);

	
	if ( strGamePath[0] != 0)		// PB미디어 폴더용 환경변수를 등록했다면!
	{
		i3::snprintf( szLuaRoot, MAX_PATH, "%s/Script/uiLua", strGamePath);	
	}
	else
	{
		i3::snprintf( szLuaRoot, MAX_PATH, "%s/Script/uiLua", strEnginePath);	
	}

	// Check Directory
	{
		char szTestFile[ MAX_PATH];
		WIN32_FIND_DATA ffd;

		i3::snprintf( szTestFile, MAX_PATH, "%s/intellisense.xml", szLuaRoot);

		HANDLE hFind = ::FindFirstFile( szTestFile, &ffd);
		if( hFind == NULL)
		{
			// no I3_ROOT setting.. so default root directory is current
			i3::string_ncopy_nullpad( szLuaRoot, _T("Script/uiLua"), MAX_PATH);
		}
	}

	ScriptManager::SetLuaRoot( szLuaRoot);

	ScriptManager::BuildIntellisense();

	m_LuaWatcher.Create( szLuaRoot);

	UpdateStatusBar( true);


	return 0;
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	int Count, i;
	char TempName[MAX_PATH << 1], Ext[32];
//	char szTempPath[MAX_PATH], * pPath;
//	char szName[MAX_PATH];

	Count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	for( i = 0; i < Count; i++)
	{
		DragQueryFile( hDropInfo, i, TempName, sizeof(TempName) - 1);
//
//		i3String::SplitFileExt( TempName, Ext, sizeof(Ext) - 1);
		i3::extract_fileext(TempName, Ext);

//		i3::string_ncopy_nullpad( szTempPath, TempName, MAX_PATH);
//		pPath = i3String::SplitFileName( szTempPath, szName, FALSE);
//		*pPath = 0;

		i3::to_upper( Ext);

		if( i3::generic_is_iequal( Ext, I3STR("I3UTX")) )
		{
			INT32 ret = ::MessageBox( NULL, "UI Library에 추가하시겠습니까?", "알림", MB_YESNO|MB_ICONINFORMATION);

			if( ret == IDYES)
			{
				i3UIManager* pUIManager = i3UI::getManager();
				I3ASSERT( pUIManager != NULL);

				i3UILibrary* pLib = pUIManager->getUILibrary();
				I3ASSERT( pLib != NULL);

				i3XMLFile file;

				if( file.Load( TempName) )
				{
					m_TemplateExplorer.OnLoadTemplate( (i3UITemplate*) file.getKeyObject());
					//pLib->AddElement( (i3UITemplate*) file.getKeyObject());
				}
			}
		}
		else if(i3::generic_is_iequal( Ext, I3STR("i3UIe")))
		{
			POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
			CDocTemplate* doc = AfxGetApp()->GetNextDocTemplate(pos);

			doc->OpenDocumentFile(TempName);
		}
		else
		{
			MessageBox( "지원되지 않는 형식의 파일입니다.");
		}
	}

	DragFinish( hDropInfo);
}

bool CMainFrame::CreatePanels(void)
{
	BOOL bNameValid;

	// Create WorkSpace Window
	CString strPaneName;
	RECT dummyRect;
	::SetRectEmpty( &dummyRect);

	// Create Screen Window
	bNameValid = strPaneName.LoadString(IDS_PANE_SCREEN);
	ASSERT(bNameValid);
	if (!m_paneScreen.Create(strPaneName, this, CRect(0, 0, 200, 200), TRUE, ID_PANE_SCREEN, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		I3TRACE("Failed to Create Screen Window\n");
		return false; // failed to create
	}

	//
	bNameValid = strPaneName.LoadString(IDS_PANE_TEMPLATE);
	ASSERT(bNameValid);
	if (!m_paneTemplate.Create(strPaneName, this, CRect(0, 0, 200, 200), TRUE, ID_PANE_TEMPLATE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		I3TRACE("Failed to Create Template Window\n");
		return false; // failed to create
	}

	// Create properties line Window
	bNameValid = strPaneName.LoadString(IDS_PANE_PROPERTIES);
	ASSERT(bNameValid);
	if (!m_paneProperties.Create(strPaneName, this, CRect(0, 0, 200, 200), TRUE, ID_PANE_PROPERTIES, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		I3TRACE("Failed to Create properties line Window\n");
		return false; // failed to create
	}

	// Create Time line Window
	bNameValid = strPaneName.LoadString(IDS_PANE_TIMELINE);
	ASSERT(bNameValid);
	if (!m_paneTimeLine.Create(strPaneName, this, CRect(0, 0, 200, 200), TRUE, ID_PANE_TIMELINE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		I3TRACE("Failed to Create Time line Window\n");
		return false; // failed to create
	}

	m_DummyWnd.Create(NULL, NULL, 0, dummyRect, this, ID_PANE_TIMELINE);

	m_paneScreen.EnableDocking( CBRS_ALIGN_ANY);
	DockPane(&m_paneScreen);

	m_paneTemplate.EnableDocking( CBRS_ALIGN_ANY);
	DockPane(&m_paneTemplate);

	m_paneScreen.AttachToTabWnd( &m_paneTemplate, DM_SHOW, TRUE, NULL);
	//CDockablePane * pTabbedBar = NULL;
	//m_paneScreen.AttachToTabWnd( &m_paneTemplate, DM_SHOW, TRUE, &pTabbedBar);

	m_paneProperties.EnableDocking( CBRS_ALIGN_ANY);
	DockPane(&m_paneProperties);
	
	m_paneTimeLine.EnableDocking( CBRS_ALIGN_ANY);
	DockPane(&m_paneTimeLine);

	// Create Script Pane
	bNameValid = strPaneName.LoadString(IDS_PANE_SCRIPT);
	I3ASSERT(bNameValid);
	if( !m_paneScriptList.Create( strPaneName, this, CRect( 0, 0, 200, 200), TRUE, ID_PANE_SCRIPT, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		I3TRACE("Failed to Create Script Pane\n");
		return false;
	}

	m_paneScriptList.EnableDocking( CBRS_ALIGN_ANY);
	DockPane( &m_paneScriptList);
	m_paneScriptList.AttachToTabWnd( &m_paneTemplate, DM_SHOW, TRUE, NULL);

	// Create Output Pane
	bNameValid = strPaneName.LoadString(IDS_PANE_OUTPUT);
	I3ASSERT(bNameValid);
	if( !m_paneOutput.Create( strPaneName, this, CRect( 0, 0, 50, 50), TRUE, ID_PANE_OUTPUT, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		I3TRACE("Failed to Create Output Pane\n");
		return false;
	}

	m_paneOutput.EnableDocking( CBRS_ALIGN_ANY);
	DockPane( &m_paneOutput);
	
	
	// Create Template Explorer Pane
	bNameValid = strPaneName.LoadString(IDS_PANE_TEMPLATEEXPLORER);
	I3ASSERT(bNameValid);
	if( !m_TemplateExplorer.Create( strPaneName, this, CRect( 0, 0, 400, 400), TRUE, 
						ID_PANE_TEMPLATEEXPLORER, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		I3TRACE("Failed to Create Output Pane\n");
		return false;
	}

	m_TemplateExplorer.EnableDocking( CBRS_ALIGN_ANY);
	DockPane( &m_TemplateExplorer);
	
	return true;
}

namespace
{
	void CALLBACK CallbackReviveProc(void)
	{
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		if (pMainFrame == NULL)
			return;
		
		pMainFrame->_ReviveUIRenderTarget();
	}
	
}

void	CMainFrame::_ReviveUIRenderTarget()
{
	i3RenderContext * pGfx = m_pViewer->GetRenderContext();
	if (m_pUIRenderTarget)
		pGfx->SetDefaultRenderTarget(m_pUIRenderTarget);
}


bool CMainFrame::_CreateRenderTargets( INT32 width, INT32 height)
{
	I3_SAFE_RELEASE( m_pUIRenderTarget);
	I3_SAFE_RELEASE( m_pLTexture);

	m_pUIRenderTarget = i3RenderTarget::new_object();
	m_pUIRenderTarget->Create( width, height, I3G_IMAGE_FORMAT_RGBX_8888);

	i3RenderContext * pGfx = m_pViewer->GetRenderContext();
	pGfx->setPresentState( false);
	pGfx->SetDefaultRenderTarget(m_pUIRenderTarget);

	m_pViewer->setExternalRenderReviveHandler(CallbackReviveProc);
	
	m_pLTexture = i3Texture::new_object();
	m_pLTexture->Create( width, height, 1, I3G_IMAGE_FORMAT_RGBX_8888, I3G_USAGE_FREQUENT_UPDATE);

	return true;
}

bool CMainFrame::SetResolution( INT32 width, INT32 height)
{
	_SetResolutionID( width, height);

	if( width <= 0 || height <= 0)
		return false;

	//if((width == m_pViewer->GetViewWidth()) && (height == m_pViewer->GetViewHeight()))	
	//	return true;

	theApp.WriteProfileInt( LPCTSTR("Settings"), LPCTSTR("Screen Resolution Width"), width);
	theApp.WriteProfileInt( LPCTSTR("Settings"), LPCTSTR("Screen Resolution Height"), height);

	g_rtTarget.right = width;
	g_rtTarget.bottom = height;

	// app의 크기에 따라서 viewer가 자동으로 resizing되는 것을 막기위해 fixed resolution을 true로 설정하고 진행 합니다.
	m_pViewer->setFixedResolution( false);
	m_pViewer->Resize( width, height);
	m_pViewer->setFixedResolution( true);

	{
		i3UIManager * pUI = i3UI::getManager();

		pUI->SetUIResolution( width, height);
	}

	{
		Ci3UIEditorDoc * pDoc = ( Ci3UIEditorDoc*)GetActiveDocument();

		if( pDoc != NULL)
			pDoc->SetResolution( width, height);
	}

	_CreateRenderTargets( width, height);

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	//Update Status Bar
	CView* pView = GetActiveView();

	if( pView != NULL)
	{
		if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
		{
			Ci3UIEditorView* pEView = (Ci3UIEditorView*)pView;
			pEView->SetupScroll();

			UpdateStatusBar( true);
		}
		else if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView_Template)))
		{
			Ci3UIEditorView_Template* pTView = (Ci3UIEditorView_Template*)pView;
			pTView->SetupScroll();
			UpdateStatusBar( false);
		}
	}
	
	return true;
}

void CMainFrame::OnResolutionChange(UINT id)
{
	if( m_pViewer == NULL)
		return;

	INT32 width = g_rtTarget.right;
	INT32 height = g_rtTarget.bottom;
	switch( id)
	{
		case ID_SCR_RES_800X600:	width = 800;	height = 600;	break;
		case ID_SCR_RES_1024X768:	width = 1024;	height = 768;	break;
		case ID_SCR_RES_1280X800:	width = 1280;	height = 800;	break;
		case ID_SCR_RES_1280X1024:	width = 1280;	height = 1024;	break;
		case ID_SCR_RES_1600X1080:	width = 1600;	height = 1080;	break;
		case ID_SCR_RES_1920X1080:	width = 1920;	height = 1080;	break;
		case ID_SCR_RES_1920X1200:	width = 1920;	height = 1200;	break;
		default:	
			break;
	}

	SetResolution( width, height);
}


void CMainFrame::OnUpdateMenu_Resolution( CCmdUI * pCmdUI)
{
	pCmdUI->SetCheck( m_nIDResolution == pCmdUI->m_nID ? BST_CHECKED : BST_UNCHECKED);
}

void CMainFrame::_SetResolutionID( INT32 width, INT32 height)
{
	switch( width)
	{
	case 800:	m_nIDResolution = ID_SCR_RES_800X600;	break;
	case 1024:	m_nIDResolution = ID_SCR_RES_1024X768;	break;
	case 1280:
		if( height == 800)
			m_nIDResolution = ID_SCR_RES_1280X800;
		else
			m_nIDResolution = ID_SCR_RES_1280X1024;	
		break;
	case 1600:
		m_nIDResolution = ID_SCR_RES_1600X1080;	break;
	case 1920:
		if( height == 1080)
			m_nIDResolution = ID_SCR_RES_1920X1080;	
		else
			m_nIDResolution = ID_SCR_RES_1920X1200;	
		break;
	default:	
		break;
	}
}


//
//
//
//void CMainFrame::RegisterScriptModule(lua_State * L)
//{
//	m_pInterface = TryLoadGame( "PointBlank_DEBUG.i3Exec");
//
//	if( m_pInterface != NULL)
//	{
//		m_pInterface->RegisterScript(L);
//	}
//}
//
//void CMainFrame::UnRegisterScriptModule()
//{
//	if( m_hModule != NULL)
//	{
//		delete m_pInterface;
//		m_pInterface = NULL;
//
//		FreeLibrary( m_hModule);
//
//		m_hModule = NULL;
//	}
//}

bool CMainFrame::CreateRenderer(void)
{
	INT32 width, height;

	width	= theApp.GetProfileInt( LPCTSTR("Settings"), LPCTSTR("Screen Resolution Width"), 1280);
	height	= theApp.GetProfileInt( LPCTSTR("Settings"), LPCTSTR("Screen Resolution Height"), 800);

	i3VideoInfo video;

	video.m_Width					= width;
	video.m_Height					= height;
	video.m_bFullScreen				= FALSE;
	video.m_bVSync					= TRUE;
	video.m_MultiSampling			= 0;
	video.m_MultiSamplingQuality	= 0;

	video.m_RenderTargetFormat = I3G_IMAGE_FORMAT_RGBX_8888;

	g_rtTarget.left		= g_rtTarget.top = 0;
	g_rtTarget.right		= width;
	g_rtTarget.bottom		= height;

	g_hActiveView = GetSafeHwnd();
	
	m_pViewer = i3Viewer::new_object();
	m_pViewer->setFixedResolution( true);
	m_pViewer->Create( m_DummyWnd.m_hWnd, &video, I3I_DEVICE_MASK_KEYBOARD | I3I_DEVICE_MASK_MOUSE);
	//m_pViewer->SetIgnoreCursorAction( true);

	g_pViewer = m_pViewer;

	if( g_pInterface == NULL || (theApp.getRunModuleEnable() == false))
	{
		m_pFramework = i3UIEFramework::new_object();
		m_pFramework->Create( m_pViewer, true);//false );
	}
	else
	{
		if( g_pInterface->OnInitInstance( theApp.m_hInstance, g_pViewer->GetWindowHandle()) == false)
		{
			MessageBox( "인터페이스의 초기화에 실패했습니다.(g_pInterface)\r\n종료합니다.", "Error", MB_OK);

			return FALSE;
		}

		m_pFramework = g_pInterface->CreateFramework( m_pViewer);

		char szCmd[MAX_PATH];
		szCmd[0] = 0;
		if( g_pInterface->OnInitFramework( m_pFramework, szCmd) == false)
		{
			I3TRACE("failed OnInitFramework\n");
		}
	}

	_CreateRenderTargets( g_rtTarget.right, g_rtTarget.bottom);

	{
		i3UIManager * pUI = i3UI::getManager();

		pUI->SetUIResolution( width, height);
	}

	m_pViewer->SetPendingUpdateEnable( true);

	{
		//Background Brush
		HBITMAP hBitmap = ::LoadBitmap( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDB_PANEL_BACKGROUND));
		g_hbrBackground	= i3TDK::CreateBitmapBrush( hBitmap);
		
		I3ASSERT( g_hbrBackground != NULL);
	}

	_LoadClearColor();
	_SetClearColor();
	
//	m_pViewer->SetPendingUpdateEnable(false);

	return true;
}

void CMainFrame::EnableExtraRenderTarget(bool bEnable)
{
	i3RenderContext * pCtx = m_pViewer->GetRenderContext();

	if( bEnable)
		pCtx->SetCurrentRenderTarget(m_pUIRenderTarget);
	else
		pCtx->SetCurrentRenderTarget(NULL);
}

void CMainFrame::ReleaseUIDC( HDC dc)
{
	if( m_pLTexture == NULL)
		return;

	IDirect3DSurface9	* pRT;

	pRT = m_pLTexture->getSurface( 0)->getSurface();
	pRT->ReleaseDC( dc);

	m_DCRef--;
	I3ASSERT(m_DCRef == 0);
}

HDC CMainFrame::GetUIDC(void)
{
	if( m_pLTexture == NULL)
		return NULL;

	HRESULT hr;
	i3RenderContext		* pCtx;
	IDirect3DSurface9	* pRT;
		
	pCtx = m_pViewer->GetRenderContext();
	if( pCtx->IsReady() == false)
		return NULL;

	pCtx->CopyRenderTarget( m_pLTexture, m_pUIRenderTarget->GetColorTexture());

	pRT = m_pLTexture->getSurface( 0)->getSurface();

	HDC dc;
	hr = pRT->GetDC( &dc);
	if( SUCCEEDED( hr))
	{
		m_DCRef++;
		I3ASSERT(m_DCRef == 1);
		return dc;		
	}

	return NULL;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;

	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

void CMainFrame::CreateRibbon_MainCategory(void)
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
}


void CMainFrame::CreateRibbon_Debug(void)
{
	CMFCRibbonPanel * pPanel;
	CMFCRibbonButton * pBtn;

	// Add "Project" category
	CMFCRibbonCategory* pCategoryHome = m_wndRibbonBar.AddCategory( LPCTSTR("Debug"), IDB_TB_SM, IDB_TB_LA);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Template Library Panel
	{
		pPanel = pCategoryHome->AddPanel( "Virtual Textures");

		pBtn = new CMFCRibbonButton( ID_DEBUG_DUMPVIRTUALTEXTURE, "Dump", -1, -1, TRUE);
		pPanel->Add( pBtn);

		pBtn = new CMFCRibbonButton( ID_DEBUG_EXPORTALL, "Export All", -1, -1, TRUE );
		pPanel->Add( pBtn );
	}
}

// View Category
void CMainFrame::CreateRibbon_View( void)
{
	CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory( LPCTSTR("View"), IDB_TB_SM, IDB_TB_LA); 
	CMFCRibbonPanel* pPanel = pCategory->AddPanel( "Panes");
	
	CMFCRibbonCheckBox* pCheckBox = new CMFCRibbonCheckBox( ID_VIEW_SCRIPT_PANE, "View Script");
	pPanel->Add( pCheckBox);

	pCheckBox = new CMFCRibbonCheckBox( ID_VIEW_SCREEN_PANE, "View Screen");
	pPanel->Add( pCheckBox);

	pCheckBox = new CMFCRibbonCheckBox( ID_VIEW_TEMPLATE_PANE, "View Template");
	pPanel->Add( pCheckBox);

	pCheckBox = new CMFCRibbonCheckBox( ID_VIEW_TIMELINE_PANE, "View Timeline");
	pPanel->Add( pCheckBox);

	pCheckBox = new CMFCRibbonCheckBox( ID_VIEW_PROPERTIES_PANE, "View Properties");
	pPanel->Add( pCheckBox);

	pCheckBox = new CMFCRibbonCheckBox( ID_VIEW_OUTPUT_PANE, "View Output");
	pPanel->Add( pCheckBox);

	pCheckBox = new CMFCRibbonCheckBox( ID_VIEW_TEMPLATE_EXPLORER_PANE, "View Template Explorer");
	pPanel->Add( pCheckBox);
}

void CMainFrame::CreateRibbon_VisualStyle(void)
{
	BOOL bNameValid;
	CString strTemp;

	// Add elements to the right side of tabs:
	bNameValid = strTemp.LoadString(IDS_RIBBON_STYLE);
	ASSERT(bNameValid);
	CMFCRibbonButton* pVisualStyleButton = new CMFCRibbonButton((UINT)-1, strTemp, -1, -1);

	pVisualStyleButton->SetMenu(IDR_THEME_MENU, FALSE /* No default command */, TRUE /* Right align */);

	bNameValid = strTemp.LoadString(IDS_RIBBON_STYLE_TIP);
	ASSERT(bNameValid);
	pVisualStyleButton->SetToolTipText(strTemp);
	bNameValid = strTemp.LoadString(IDS_RIBBON_STYLE_DESC);
	ASSERT(bNameValid);
	pVisualStyleButton->SetDescription(strTemp);
	m_wndRibbonBar.AddToTabs(pVisualStyleButton);
}

void CMainFrame::CreateRibbon_QuickAccess(void)
{
	// Add quick access toolbar commands:
	CList<UINT, UINT> lstQATCmds;

	lstQATCmds.AddTail(ID_FILE_NEW);
	lstQATCmds.AddTail(ID_FILE_OPEN);
	lstQATCmds.AddTail(ID_FILE_SAVE);
	lstQATCmds.AddTail(ID_FILE_PRINT_DIRECT);

	m_wndRibbonBar.SetQuickAccessCommands(lstQATCmds);
	m_wndRibbonBar.AddToTabs(new CMFCRibbonButton(ID_APP_ABOUT, _T("\na"), m_PanelImages.ExtractIcon (0)));
}

void CMainFrame::InitializeRibbon()
{
	CreateRibbon_MainCategory();
	CreateRibbon_Home();
	CreateRibbon_ProjectCategory();
	CreateRibbon_Template();
	CreateRibbon_VisualStyle();

	CreateRibbon_View();
#if defined( _DEBUG)
	CreateRibbon_Debug();
#endif

	// 해상도 설정 메뉴 추가
	CreateRibbon_QuickAccess();
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

void CMainFrame::SetCurrentLibrary( i3UILibrary * pUILib)
{
}

void CMainFrame::SetCurrentScene( i3UIScene * pScene)
{
}

bool CMainFrame::_IsInside( CWnd * pWnd, POINT pt)
{
	CRect rt;

	pWnd->GetWindowRect( &rt);

	if( rt.PtInRect( pt))
		return true;

	return false;
}


bool CMainFrame::DragMoveObjectTemplate( POINT scrPt, bool bInsideFrame /*= false*/, i3ClassMeta * pInsideFrameMeta /*= NULL*/)
{
	Ci3UIEditorView * pView = (Ci3UIEditorView*)theApp.getScreenView();

	if( _IsInside( pView, scrPt))
	{
		theApp.SwitchView( EVIEW_SCREEN);

		pView->ScreenToClient( &scrPt);
		
		return pView->OnDragMoveObjectTemplate( scrPt, bInsideFrame, pInsideFrameMeta);
	}

	return false;
}

bool CMainFrame::DropObject( i3UITemplate * pTemplate, POINT scrPt)
{
	Ci3UIEditorView * pView = (Ci3UIEditorView*)theApp.getScreenView();

	if( _IsInside( pView, scrPt))
	{
		theApp.SwitchView( EVIEW_SCREEN);

		pView->ScreenToClient( &scrPt);
		
		return pView->DropObject( pTemplate, scrPt);
	}

	return false;
}

void CMainFrame::OnProjectExporttemplatelibrary()
{
	Ci3UIEditorDoc * pDoc = ( Ci3UIEditorDoc*)GetActiveDocument();

	pDoc->CreateTemplateLibrary();
}

void CMainFrame::OnProjectImporttemplatelibrary()
{
	Ci3UIEditorDoc * pDoc = ( Ci3UIEditorDoc*)GetActiveDocument();

	pDoc->ImportTemplateLibrary();
}

void CMainFrame::OnProjectAdjustvirtualtexture()
{
	if( i3UI::getVirtualTexture() != NULL)
		i3UI::getVirtualTexture()->DeleteWrongTexture();
	i3UI::AdjustVirtualTexture(false);

	{
		AfxMessageBox( "Virtual Texture의 변경으로 인해, Template를 반드시 저장해야 합니다. 저장합니다");

		OnProjectSavetemplatelibrary();
	}
}

void CMainFrame::OnProjectAdjustVTexList()
{
	if (i3UI::getVirtualTexture() != NULL)
		i3UI::getVirtualTexture()->DeleteWrongTexture();
	i3UI::AdjustVirtualTexture(true);

	{
		AfxMessageBox("신규 버추얼텍스쳐 포맷은, Template을 저장하지 않아요.");

	//	OnProjectSavetemplatelibrary();
	}
}

void CMainFrame::OnProjectCreatevirtualtexture()
{
	CreateVirtualTexture(false);
}

void CMainFrame::OnProjectCreateVTexList()
{
	CreateVirtualTexture(true);
}

void CMainFrame::OnProjectOpenvirtualtexture()
{
	OpenVirtualTexture();
}

void CMainFrame::OnProjectModifyVirtualtexture()
{
	CDlg_SelectTexture dlg;

	i3VirtualTexture * pVTex = i3UI::getVirtualTexture();
	if( pVTex == NULL)
	{
		checkVirtualTexture();
	}

	pVTex = i3UI::getVirtualTexture();

	dlg.Execute( pVTex);
}

void CMainFrame::OnProjectExtractAllRefTexture(void)
{
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd, "Extract all contained textures to...") == FALSE)
		return;

	i3VirtualTexture * pVTex = i3UI::getVirtualTexture();
	if( pVTex == NULL)
		return;

	pVTex->ExtractAllContainedTextures( dlg.GetSelectedFolderPath());

	AfxMessageBox( "All contained textures are extracted.");
}

void CMainFrame::OnDebugDumpvirtualtexture()
{
	CView * pView = theApp.getTemplateView();

	if( pView != NULL)
	{
		if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView_Template)))
		{
			Ci3UIEditorView_Template * pTemplateView = (Ci3UIEditorView_Template *) pView;

			pTemplateView->OnDebugDumpvirtualtexture();
		}
	}
}

void CMainFrame::OnSelectSceneFromClient()
{
	i3UIManager * pUI = i3UI::getManager();
	I3ASSERT( pUI != NULL);

	i3::vector<I3UI_SCENE_INFO*>& List = pUI->getSceneInfoList();

	if( List.size() < 1)
	{
		MessageBox( "UIManager doesn't have any Selectable scene", "알림", MB_OK);
		return;
	}

	CSelectSceneDlg Dlg;
	if( Dlg.DoModal() == IDOK)
	{
		
	}
}

void CMainFrame::OnProjectRun()
{
	// TODO: Add your command handler code here
	
	CView* pView = GetActiveView();

	if( pView != NULL)
	{
		if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView_Template)))
		{
			Ci3UIEditorView_Template * pTemplateView = (Ci3UIEditorView_Template *) pView;

			pTemplateView->setPrevMode( true);
		}
		else if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
		{
			Ci3UIEditorView * pScreenView = (Ci3UIEditorView *) pView;

			pScreenView->SetPrevMode( true);
		}
	}
}

void CMainFrame::OnProjectStop()
{
	// TODO: Add your command handler code here
	CView * pView = GetActiveView();
	
	if( pView != NULL)
	{
		if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView_Template)))
		{
			Ci3UIEditorView_Template * pTemplateView = (Ci3UIEditorView_Template *) pView;

			pTemplateView->setPrevMode( false);
		}
		else if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
		{
			Ci3UIEditorView * pScreenView = (Ci3UIEditorView *) pView;

			pScreenView->SetPrevMode( false);
		}
	}
}

void CMainFrame::OnProjectSavetemplatelibrary()
{
	// TODO: Add your command handler code here
	Ci3UIEditorDoc * pDoc = ( Ci3UIEditorDoc*)GetActiveDocument();

	pDoc->SaveTemplateLibrary(false);
}

void CMainFrame::OnProjectSavetemplatelibrary_NEW()
{
	Ci3UIEditorDoc * pDoc = (Ci3UIEditorDoc*)GetActiveDocument();

	pDoc->SaveTemplateLibrary(true);
}



void CMainFrame::OnUpdateProjectRun(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	CView * pView = GetActiveView();

	if( pView != NULL)
	{
		if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView_Template)))
		{
			Ci3UIEditorView_Template * pTemplateView = (Ci3UIEditorView_Template *) pView;

			pCmdUI->SetCheck( (pTemplateView->isPrevMode())? BST_CHECKED : BST_UNCHECKED);
		}
		else if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
		{
			Ci3UIEditorView * pScreenView = (Ci3UIEditorView *) pView;

			pCmdUI->SetCheck( (pScreenView->isPrevMode())? BST_CHECKED : BST_UNCHECKED);
		}
	}
}

void CMainFrame::OnModuleSelect()
{
	CSelectModuleDlg Dlg;
	
	Dlg.DoModal();
}

void CMainFrame::OnUpdateProjectStop(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	CView * pView = GetActiveView();
	
	if( pView != NULL)
	{
		if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView_Template)))
		{
			Ci3UIEditorView_Template * pTemplateView = (Ci3UIEditorView_Template *) pView;

			pCmdUI->SetCheck( (pTemplateView->isPrevMode())? BST_UNCHECKED : BST_CHECKED);
		}
		else if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
		{
			Ci3UIEditorView * pScreenView = (Ci3UIEditorView *) pView;

			pCmdUI->SetCheck( (pScreenView->isPrevMode())? BST_UNCHECKED : BST_CHECKED);
		}
		
		//pCmdUI->Enable( FALSE);	//Disable 시키기ㅋㅋ
	}
}

void CMainFrame::OnSaveScript()
{
	CScriptPane* pScriptPane = _getFocusedPane();
	
	if( pScriptPane != NULL)
	{
		pScriptPane->Save();	
	}
}

void CMainFrame::setEnableOtherScene(bool bEnable)
{
	i3Framework * pFramework = i3Framework::getCurrentFramework();
	I3ASSERT( pFramework != NULL);

#if defined( I3_ENGINE3)
	if(bEnable)
		pFramework->setEditMode( i3Framework::MODE_PLAY);
	else
		pFramework->setEditMode( i3Framework::MODE_EDIT_2D);

	i3Node* p3DRoot = pFramework->get3DMainRoot();
#else
	i3Node * p3DRoot = pFramework->Get3DRoot();
#endif
	
	I3ASSERT( p3DRoot != NULL);

	i3Node * p2DRoot = pFramework->Get2DRoot();
	I3ASSERT( p2DRoot != NULL);

	INT32 i;
	for( i = 1; i<p2DRoot->GetChildCount(); ++i)
	{
		i3Node * pChild = (i3Node*)p2DRoot->GetChild(i);
		if( pChild != NULL)
		{
			if( bEnable)
			{
				pChild->RemoveFlag( I3_NODEFLAG_DISABLE);
			}
			else
			{
				pChild->AddFlag( I3_NODEFLAG_DISABLE);
			}
		}
	}

	if( bEnable)
	{
		p3DRoot->RemoveFlag( I3_NODEFLAG_DISABLE);
	}
	else
	{
		p3DRoot->AddFlag( I3_NODEFLAG_DISABLE);
	}
}

void CMainFrame::returnToEditState()
{
	m_paneScreen.returnToEditState();
}

void CMainFrame::ClearNationControl()
{
	m_pNationControl->RemoveAllSubItems();
}

void CMainFrame::AddSubItem(CMFCRibbonBaseElement* pSubItem, int nIndex)
{
	m_pNationControl->AddSubItem(pSubItem, nIndex);
}

void CMainFrame::OnUpdateSaveScript(CCmdUI *pCmdUI)
{
	CScriptPane* pPane = _getFocusedPane();
	
	if( pPane != NULL)
		pCmdUI->Enable( TRUE);
	else
		pCmdUI->Enable( FALSE);
}

void CMainFrame::OnCompileScript()
{
	CScriptPane* pPane = _getFocusedPane();

	if( pPane != NULL)
	{
		char pBuf[MAX_PATH];

		i3UIManager* pManager = i3UI::getManager();
		I3ASSERT( pManager != NULL);
		
		pPane->Save();	//saveifunsure..
		pManager->Compile( pPane->GetFilename(), pBuf, sizeof( pBuf));

		CUIPaneOutput* pOutput = getOutputPane();
		I3ASSERT( pOutput != NULL);
		
		pOutput->OutputCompileResult( pBuf);
	}
}

void CMainFrame::OnUpdateCompileScript(CCmdUI *pCmdUI)
{
	CScriptPane* pPane = _getFocusedPane();
	
	if( pPane != NULL)
		pCmdUI->Enable( TRUE);
	else
		pCmdUI->Enable( FALSE);
}

void CMainFrame::UpdatePropertyPane( void)
{
	if( m_paneProperties.IsWindowEnabled() )
	{
		m_paneProperties.Update();	
	}
}

void CMainFrame::UpdateStatusBar( bool bScreen)
{
	char szText[MAX_PATH];

	// text on the left side
	if (m_StatusBarPane1 != NULL)
	{
		const char* pszWorkingDir = CMainFrame::getWorkingFolder();

		i3::rc_string strRelLocaleDir = i3UIFilePathMgr::i()->Retrieve_LocaleUIRootDirectory(false);

		i3::rc_string strVTexDir = i3UIFilePathMgr::i()->Retrieve_Current_VTexDirectory(false);

		i3::stack_string relVTexDir;

		i3::make_relative_path(strRelLocaleDir, strVTexDir, relVTexDir);

		i3::stack_string strMsg = "Working Folder = ";
		strMsg += '\"';
		strMsg += pszWorkingDir;
		strMsg += '\"';

		if (!strRelLocaleDir.empty())
		{
			strMsg += "       Locale Directory = ";
			strMsg += '\"';
			strMsg += strRelLocaleDir;
			strMsg += '\"';

			if (strVTexDir.empty())
			{
				strMsg += "       No VirtualTexture Folder";
			}
			else
			{
				strMsg += "       VirtualTexture Folder = ";
				strMsg += '\"';
				strMsg += relVTexDir;
				strMsg += '\"';
			}
		}

		i3UILibrary* pUILib = i3UI::getUILibrary();

		if (pUILib && pUILib->hasName())
		{
			strMsg += "       UITemplate Filename = ";
			strMsg += '\"';
			strMsg += pUILib->GetNameString();
			strMsg += '\"';
		}
		
		m_StatusBarPane1->SetText( strMsg.c_str() );
	}

	// text on the right side
	if( m_StatusBarPane2 != NULL )
	{
		INT32 nWidth = 0;
		INT32 nHeight = 0;
		INT32 nZoom = 100;

		i3UIManager * pUI = i3UI::getManager();
		if( pUI != NULL)
		{
			nWidth = pUI->getWidth();
			nHeight = pUI->getHeight();
		}

		//Update Resolution Menu ID
		//프로그램 시작시의 해상도를 ID의 초기값으로 설정해두기 위해 호출해준다.
		_SetResolutionID( nWidth, nHeight);
		
		if( bScreen)
		{
			Ci3UIEditorView* pView = (Ci3UIEditorView*)theApp.getScreenView();
			if( pView != NULL)
				nZoom = (INT32)(pView->getZoomRatio() * 100);
		}
		else
		{
			Ci3UIEditorView_Template* pView = (Ci3UIEditorView_Template*)theApp.getTemplateView();
			if( pView != NULL)
				nZoom = (INT32)(pView->getZoomRatio() * 100);
		}

		i3::snprintf( szText, MAX_PATH, "%dx%d(%d%%)", nWidth, nHeight, nZoom);

		m_StatusBarPane2->SetText( szText);
	}

	m_wndStatusBar.RecalcLayout();
	m_wndStatusBar.Invalidate();
}

// *********** View Category *************
// OnView
void CMainFrame::OnViewScriptPane( void)
{
	//CDockablePane
	//virtual BOOL IsVisible() const;
	//virtual void ShowPane(    BOOL bShow,    BOOL bDelay,    BOOL bActivate);
	m_paneScriptList.ShowPane( !m_paneScriptList.IsVisible(), FALSE, TRUE);
}

void CMainFrame::OnViewScreenPane( void)
{
	m_paneScreen.ShowPane( !m_paneScreen.IsVisible(), FALSE, TRUE);
}

void CMainFrame::OnViewTemplatePane( void)
{
	m_paneTemplate.ShowPane( !m_paneTemplate.IsVisible(), FALSE, TRUE);
}

void CMainFrame::OnViewTimelinePane( void)
{
	m_paneTimeLine.ShowPane( !m_paneTimeLine.IsVisible(), FALSE, TRUE);
}

void CMainFrame::OnViewPropertiesPane( void)
{
	m_paneProperties.ShowPane( !m_paneProperties.IsVisible(), FALSE, TRUE);
}

void CMainFrame::OnViewOutputPane( void)
{
	m_paneOutput.ShowPane( !m_paneOutput.IsVisible(), FALSE, TRUE);
}

void CMainFrame::OnViewTemplateExplorerPane( void)
{
	m_TemplateExplorer.ShowPane( !m_TemplateExplorer.IsVisible(), FALSE, TRUE);
}

//////////////////////////////////////////////////////////////////////
// OnUpdateView
void CMainFrame::OnUpdateViewScriptPane(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_paneScriptList.IsVisible());
}

void CMainFrame::OnUpdateViewScreenPane(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_paneScreen.IsVisible());
}

void CMainFrame::OnUpdateViewTemplatePane(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_paneTemplate.IsVisible());
}

void CMainFrame::OnUpdateViewTimelinePane(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_paneTimeLine.IsVisible());
}

void CMainFrame::OnUpdateViewPropertiesPane(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_paneProperties.IsVisible());
}

void CMainFrame::OnUpdateViewOutputPane(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( m_paneOutput.IsVisible());
}

void CMainFrame::OnUpdateViewTemplateExplorerPane(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_TemplateExplorer.IsVisible());
}

//==============================================================
//	Script
//==============================================================
CScriptPane* CMainFrame::_getFocusedPane( void)
{
	CWnd* pWnd = GetFocus();

	if( pWnd != NULL)
	{
		//CScriptPane* pScriptPane = dynamic_cast<CScriptPane*>(pWnd);
		if( pWnd->IsKindOf( RUNTIME_CLASS( CScriptPane)))
		{
			CScriptPane* pScriptPane = (CScriptPane*)pWnd;
			return pScriptPane;
		}
		else if( pWnd->IsKindOf( RUNTIME_CLASS( CScriptWnd)))
		{
			CScriptWnd* pScriptWnd = (CScriptWnd*)pWnd;
			CWnd* pParentWnd = pScriptWnd->GetParent();
			if( pParentWnd != NULL && pParentWnd->IsKindOf( RUNTIME_CLASS( CScriptPane)))
			{
				CScriptPane* pScriptPane = (CScriptPane*)pParentWnd;
				return pScriptPane;
			}
		}
	}

	return NULL;
}

CScriptPane* CMainFrame::OnEditScript( const char* pszName, bool bFile, bool bScreen)
{
	if( pszName == NULL)
	{
		::MessageBox( NULL, _T("Cannot Proceed: Script Name == NULL"), _T("Error"), MB_OK);
		return NULL;
	}

	char szTitle[MAX_PATH];
	bool bExist = false;
	bool bAnyPane = false;

	CScriptPane* pPane = NULL;

	for( size_t i=0; i< m_PaneList.size(); i++)
	{
		bAnyPane = true;

		pPane = m_PaneList[i];
		CScriptWnd* pWnd = pPane->getScriptWnd();
		pWnd->getWindowName( szTitle, sizeof( szTitle));

		if(i3::generic_is_iequal( szTitle, pszName) )		
		{
			pPane->ShowPane( TRUE, FALSE, TRUE);
			pPane->SetFocus();
			bExist = true;
			break;
		}
	}

	if( !bExist)
	{
		pPane = new CScriptPane();
		
		i3::string_ncopy_nullpad( szTitle, pszName, sizeof( szTitle));

		CRect rt;
		GetActiveView()->GetClientRect( rt);

		UINT32 nID = ID_INITIAL_SCRIPT_PANE + m_nScriptPaneCnt;		//ID를 지정해주지 않으면 DockingManager에 등록되지 않는다! 
		pPane->Create( szTitle, this, CRect(0,0,rt.right/2,rt.bottom/2), TRUE, nID, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI);	
		m_nScriptPaneCnt++;
		
		pPane->EnableDocking( CBRS_ALIGN_ANY);

		CScriptWnd* pWnd = pPane->getScriptWnd();
		I3ASSERT( pWnd != NULL);
		pWnd->setWindowName( szTitle);

		bool bDockToFrameWindow = true;

		if( bAnyPane)
		{
			for( size_t i=0; i< m_PaneList.size(); i++)
			{
				CScriptPane* pExistPane = m_PaneList[i];
				if( !pExistPane->IsAutoHideMode() && pExistPane->IsWindowVisible())
				{
					pPane->AttachToTabWnd( pExistPane, DM_SHOW, TRUE, NULL);
					bDockToFrameWindow = false;
					break;
				}
			}
		}
		
		if( bDockToFrameWindow)
			pPane->DockToFrameWindow( CBRS_ALIGN_RIGHT);	

		m_PaneList.push_back( pPane);

		//Set
		const char* szScriptFileName = NULL;
		i3::stack_string strScriptFilePath;

		if (bScreen)
		{
			pPane->setBelongToScreen();

			Ci3UIEditorDoc* pDoc = (Ci3UIEditorDoc*)GetActiveDocument();
			I3ASSERT(pDoc != NULL);

			i3UIScene* pScene = pDoc->getCurrentScene();
			I3ASSERT(pScene != NULL);

			szScriptFileName = pScene->getScriptFile();
		}
		else
		{
			i3UIManager* pUIManager = i3UI::getManager();
			I3ASSERT(pUIManager != NULL);

			i3UILibrary* pLib = pUIManager->getUILibrary();
			I3ASSERT(pLib != NULL);

			szScriptFileName = pLib->getScriptFile();
		}

		i3::rc_string uiRoot = i3UIFilePathMgr::i()->Retrieve_CommonUIRootDirectory(true);			// Media/UI_XXX 정도까지를 의미함..

		strScriptFilePath = uiRoot;
		strScriptFilePath += '/';
		strScriptFilePath += szScriptFileName;

		/*
		i3::sprintf(strScriptFilePath, "%s/%s/%s", i3ResourceFile::GetWorkingFolder(), i3UI::getManager()->GetUIVersionName()->c_str(), szScriptFileName);


			if(i3UI::getManager()->GetLocale()->length() )
				i3::snprintf( szScriptFile, sizeof( szScriptFile), "%s/%s/%s", i3ResourceFile::GetWorkingFolder(), i3UI::getManager()->GetUIVersionName()->c_str(), pScene->getScriptFile());
			else
			{
				char szRoot[MAX_PATH];
				i3::snprintf(szScriptFile, MAX_PATH, "%s", i3ResourceFile::GetWorkingFolder());
				
				i3::extract_filename(szScriptFile, szRoot);
				if (i3::generic_is_iequal(szRoot, "VirtualTexture"))
				{
					i3::extract_directoryname(szScriptFile, szRoot);
					i3::snprintf(szScriptFile, MAX_PATH, "%s/%s", szRoot, pScene->getScriptFile());
				}
				else
					i3::snprintf(szScriptFile, sizeof(szScriptFile), "%s/%s", i3ResourceFile::GetWorkingFolder(), pScene->getScriptFile());
			}
		}
		else
		{
			i3UIManager* pUIManager = i3UI::getManager();
			I3ASSERT( pUIManager != NULL);

			i3UILibrary* pLib = pUIManager->getUILibrary();
			I3ASSERT( pLib != NULL);

			if (i3UI::getManager()->GetLocale()->length())
				i3::snprintf(szScriptFile, sizeof(szScriptFile), "%s/%s/%s", i3ResourceFile::GetWorkingFolder(), i3UI::getManager()->GetUIVersionName()->c_str(), pLib->getScriptFile());
			else
			{
				char szRoot[MAX_PATH];
				i3::snprintf(szScriptFile, MAX_PATH, "%s", i3ResourceFile::GetWorkingFolder());

				i3::extract_filename(szScriptFile, szRoot);
				if (i3::generic_is_iequal(szRoot, "VirtualTexture"))
				{
					i3::extract_directoryname(szScriptFile, szRoot);
					i3::snprintf(szScriptFile, MAX_PATH, "%s/%s", szRoot, pLib->getScriptFile());
				}
				else
					i3::snprintf(szScriptFile, sizeof(szScriptFile), "%s/%s", i3ResourceFile::GetWorkingFolder(), pLib->getScriptFile());
			}
		}
		*/
		
		pPane->SetFilename(strScriptFilePath.c_str());

		//Open
		if( bFile)
		{
			pPane->OpenFile();
		}
		else
		{
			pPane->OpenBlock( szTitle);
		}
	}


	// 이게 여기 있는것보다 다른데 있는게 더 좋을것 같은데.. 
	//Begin file watching thread if none exist
	if( !m_bFileWatcher)
	{
		CreateFileWatcher();
	}

	return pPane;
}


void CMainFrame::OnCloseScript( CScriptPane* pPaneToBeClosed)
{
	INT32 idx = -1;

	for( size_t i=0; i< m_PaneList.size(); i++)
	{
		CScriptPane* pPane =m_PaneList[i];
		if( pPane == pPaneToBeClosed)
		{
			idx = i;
			break;
		}
	}

	I3ASSERT( idx != -1);

	i3::vu::erase_index(m_PaneList, idx);
}

void CMainFrame::CloseAllScript()
{
	for (size_t i = 0; i< m_PaneList.size(); i++)
	{
		m_PaneList[i]->OnDestroy();
		
		delete m_PaneList[i];
	}

	m_PaneList.clear();

	RecalcLayout();
}

void CMainFrame::OnModifyScript( CScriptWnd* pWnd, bool bModified)
{
	for( size_t i=0; i< m_PaneList.size(); i++)
	{
		CScriptPane* pPane = m_PaneList[i];
		CScriptWnd* pChild = pPane->getScriptWnd();

		if( pChild == pWnd)
		{
			char buf[MAX_PATH];
			pChild->getWindowName( buf, sizeof( buf));
			
			if( bModified)
			{
				char szNew[MAX_PATH];
				i3::snprintf( szNew, sizeof( szNew), "%s *", buf);	
				pPane->SetWindowText( szNew);
			}
			else
			{
				pPane->SetWindowText( buf);
			}
			
		}
	}
}

void CMainFrame::OnSavingScript( CScriptPane* pPane)
{
	//작업중인 스크립트를 저장할 때 호출
	//이것은 OnFileChanged에 의해 처리되면 안된다. 
	bool bFile = pPane->isFileOpened();
	bool bScreen = pPane->isBelongToScreen();

	for( size_t i=0; i< m_PaneList.size(); i++)
	{
		CScriptPane* pCur = m_PaneList[i];
		if( pCur->isBelongToScreen() == bScreen && pCur != pPane)
		{
			if( bFile)
			{
				// File인 경우 모든 Block에 Reload Flag를 설정한다. 
				pCur->setNeedToReload();
			}
			else if( pCur->isFileOpened())
			{
				// Block인 경우 File에만 Reload Flag를 설정한다. 
				pCur->setNeedToReload();
			}
		}
	}
}

void CMainFrame::OnFileChanged( bool bScreen)
{
	//외부에서 파일이 변경되는 것을 감지하기 위한 용도
	for( size_t i=0; i< m_PaneList.size(); i++)
	{
		CScriptPane* pPane = m_PaneList[i];
		if( pPane->isBelongToScreen() == bScreen)
		{
			pPane->setNeedToReload();
		}
	}
}

// DockablePane이 Floating되어 있는 상태에서 닫히는 경우
// 이상하게도 꼭 2번씩 들어온다. (2번째 들어오는 경우를 대비하여 ::IsWindow 체크)
BOOL CMainFrame::OnCloseMiniFrame( CPaneFrameWnd* pWnd)
{ 
	//http://stackoverflow.com/questions/1333801/how-to-close-dynamically-created-cdockablepane-windows
	// Panes are placed inside a mini frame when they have the "floating" status. 
    // Since I didn't find a way to iterate over the panes of a mini frame 
    // (CMultiPaneFrameWnd can have several panes), we iterate over all panes 
    // and close those whose parent frame is pWnd. 
	
	if(0 == pWnd->GetPaneCount()) { return TRUE; } // No panes.. allow closing 

	BOOL bAllowClosing = TRUE;

	CObList allPanes; 
	
	CDockingManager* pDockMan = GetDockingManager(); 
	I3ASSERT( pDockMan != NULL);
	
	pDockMan->GetPaneList(allPanes, TRUE, NULL, TRUE); 

	for(POSITION pos = allPanes.GetHeadPosition(); pos != NULL;) 
	{
		CDockablePane* pPane = dynamic_cast<CDockablePane*>(allPanes.GetNext(pos)); 
		if(NULL == pPane) { continue; }

		if( pPane->GetParentMiniFrame() == pWnd)	//miniframe에 속한 pane들에 대하여
		{
			if( ::IsWindow( pPane->m_hWnd))	
			{
				if( pPane->IsKindOf( RUNTIME_CLASS( CScriptPane)))
				{
					CScriptPane* pScriptPane = (CScriptPane*)pPane;
					bool bClose = pScriptPane->SaveIfModified();
					
					if( bClose)
						pPane->PostMessage( WM_CLOSE);	 // Note: Post instead of Send 
					else
						bAllowClosing = FALSE;

				}
				else
				{
					pPane->ShowPane( FALSE, FALSE, FALSE);	// ScriptPane이 아니면 그냥 숨긴다.
				}
			}
		}
	}
	
	return bAllowClosing;	// return true to allow closing
} 

BOOL CMainFrame::OnCloseDockingPane(CDockablePane* pWnd) 
{ 
	//http://stackoverflow.com/questions/1333801/how-to-close-dynamically-created-cdockablepane-windows
    CObList paneList; 
 
    // We can get CDockablePanes and CTabbedPanes here. 
    // The tabbed panes contain dockable panes. 
    CTabbedPane* pTabbed = dynamic_cast<CTabbedPane*>(pWnd); 
    CDockablePane* pDockable = dynamic_cast<CDockablePane*>(pWnd); 
    if(NULL != pTabbed) 
    { 
        pTabbed->GetPaneList(paneList); 
    } 
    else if(NULL != pDockable) 
    { 
        paneList.InsertAfter(paneList.GetHeadPosition(), pDockable); 
    } 
 
    // Whatever it was, we now have a list of dockable panes, which we will close. 
    for(POSITION pos = paneList.GetHeadPosition(); NULL != pos;) 
    { 
        CDockablePane* pPane = dynamic_cast<CDockablePane*>(paneList.GetNext(pos)); 
        ASSERT(NULL != pPane); 
		
        // Let the window disappear and then recalculate the layout. 
        // Not doing this causes problems with panes grouped together in a tabbed pane. 
 
        // Really close the window so the docking manager also doesn't know of it anymore. 
		if( pPane->IsKindOf( RUNTIME_CLASS( CScriptPane)))
		{
			if( pPane->IsAutoHideMode())
			{
				//AutoHide인 상태로 Close 해버리면 크래시하므로 일단은 이렇게..
				pPane->ToggleAutoHide();
			}
			else
			{
				if( pPane->IsWindowVisible())
				{
					CScriptPane* pScriptPane = (CScriptPane*)pPane;
					bool bClose = pScriptPane->SaveIfModified();
					if( bClose)
						pPane->PostMessage(WM_CLOSE); // Note: Post instead of Send 
					else
						return FALSE;
				}
			}
			//if( pPane->IsAutoHideMode())
			//{
			//	pPane->SetAutoHideMode( FALSE, pPane->GetCurrentAlignment());
			//	
			//	/*CTabbedPane* pTabbed = dynamic_cast<CTabbedPane*>(pPane); 
			//	if( pTabbed != NULL)
			//	{
			//		CObList list;
			//		pTabbed->GetPaneList( list);
			//	}*/

			//	

			//	//CMFCAutoHideBar* GetAutoHideToolBar() const;
			//	//pPane->PostMessage(WM_CLOSE);
			//	//CMFCAutoHideBar* pBar = pPane->GetAutoHideToolBar();
			//	//pBar->PostMessage( WM_CLOSE);
			//	//pPane->EnableGripper( FALSE);
			//	//pPane->SetAutoHideMode( FALSE, CBRS_ALIGN_RIGHT);
			//	//pPane->GetParent()->PostMessage( WM_CLOSE);
			//	//CRect rt;
			//	//pPane->GetClientRect( &rt);
			//	//pPane->FloatPane( rt);
			//}
			////else
			////if( GetFocus() == pPane)
			//	pPane->PostMessage(WM_CLOSE); // Note: Post instead of Send 
		}
    } 
 
    return TRUE; // Allow closing 
	//return FALSE;
} 


void CMainFrame::CreateFileWatcher( void)
{
	// Screen Script
	Ci3UIEditorDoc* pDoc = (Ci3UIEditorDoc*)GetActiveDocument();
	I3ASSERT( pDoc != NULL);
	i3UIScene* pScene = pDoc->getCurrentScene();
	I3ASSERT( pScene != NULL);
	const char* pszScrScript = pScene->getScriptFile();
	

	//Library Script
	i3UIManager* pManager = i3UI::getManager();
	I3ASSERT( pManager != NULL);
	i3UILibrary* pLib = pManager->getUILibrary();
	I3ASSERT( pLib != NULL);
	const char* pszLibScript = pLib->getScriptFile();


	m_bFileWatcher = m_FileWatcher.Create(	i3ResourceFile::GetWorkingFolder(),
							pszScrScript == NULL ? "" : pszScrScript, 
							pszLibScript == NULL ? "" : pszLibScript	);

}

void CMainFrame::SuspendFileWatcher( void)
{
	m_FileWatcher.Suspend();
}

void CMainFrame::ResumeFileWatcher( void)
{
	m_FileWatcher.Resume();
}

void CMainFrame::TerminateFileWatcher( void)
{
	m_FileWatcher.Terminate();
}

bool CMainFrame::IsScriptChanged( bool bScreen)
{
	if( bScreen)
		return m_FileWatcher.isModified_Screen();
	else
		return m_FileWatcher.isModified_Library();
}

void CMainFrame::SetScriptChanged( bool bScreen, bool bChanged)
{
	if( bScreen)
		m_FileWatcher.setModifed_Screen( bChanged);
	else
		m_FileWatcher.setModifed_Library( bChanged);
}


INT32 CMainFrame::getSelectedObjCount( void)
{
	CView * pView = GetActiveView();

	if( pView != NULL)
	{
		if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView_Template)))
		{
			Ci3UIEditorView_Template * pTemplateView = (Ci3UIEditorView_Template *) pView;

			return pTemplateView->getSelectedObjCount();
		}
		else if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
		{
			Ci3UIEditorView * pScreenView = (Ci3UIEditorView *) pView;

			return pScreenView->getSelectedObjCount();
		}
	}

	return -1;
}

void CMainFrame::getSelectedObjList( i3::vector<i3UIControl*>& Out)
{
	CView * pView = GetActiveView();

	if( pView != NULL)
	{
		if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView_Template)))
		{
			Ci3UIEditorView_Template * pTemplateView = (Ci3UIEditorView_Template *) pView;

			pTemplateView->getSelectedObjList( Out);
		}
		else if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
		{
			Ci3UIEditorView * pScreenView = (Ci3UIEditorView *) pView;

			pScreenView->getSelectedObjList( Out);
		}
	}
}

void CMainFrame::GotoViewTemplateEdit( i3UITemplate * pTemplate)
{
	theApp.SwitchView( EVIEW_TEMPLATE);

	m_TemplateExplorer.EditTemplate( pTemplate);
	
	CView * pActiveView = GetActiveView();

	if( pActiveView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView_Template)) == FALSE)
		return;

	Ci3UIEditorView_Template * pView = (Ci3UIEditorView_Template*)pActiveView;
	
	if( pTemplate == NULL)
	{
		pView->SetCurrentItem( NULL);
		m_paneTemplate.ShowTemplateTree( NULL);
	}
	else
	{
		//Template View에 실제 템플릿 그리기
		pView->SetCurrentItem( pTemplate);

		//Template Pane에 트리 그리기
		m_paneTemplate.ShowTemplateTree( pTemplate);
		m_paneTemplate.SetFocus();
	}
}

void CMainFrame::ImportUIScene( void)
{
	Ci3UIEditorDoc * pDoc = ( Ci3UIEditorDoc*)GetActiveDocument();

	i3UIScene * pScene = pDoc->ImportScene();

	m_paneScreen.SetScene( pScene);
}

void CMainFrame::ExportUIScene( i3UIControl * pRoot)
{
	Ci3UIEditorDoc * pDoc = ( Ci3UIEditorDoc*)GetActiveDocument();

	pDoc->ExportScene( pRoot);
}




void CMainFrame::OnUpdateMenu_Nation(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_nNationSel == pCmdUI->m_nID ? BST_CHECKED : BST_UNCHECKED);
}

// TODO : 160414 수빈.
////////////////// "VirtualTexture" 문자열 검색하여 관련된 경로 코드를 CMainFrame::getWorkingFolder와 i3UIFilePathMgr API등으로 변경해야한다.

namespace
{

	// 아래 작성해놓은 함수는 씬에 전혀 링크가 되어있지 않은 콘트롤의 경우 템플릿 교체가 안되는 큰 단점이 있는데, 일단은 그냥 놔둬본다...
	/*
	void RefreshTemplateForTool_Rec(i3UIControl* pCtrl)
	{

		UINT128 hashCode = pCtrl->getTemplateHashCode();
		UINT128 emptyCode; emptyCode = 0;

		if (hashCode != emptyCode)
		{
			i3UITemplate* pNewTemplate = i3UI::FindTemplateByHash(hashCode);

			if (pNewTemplate)
				pCtrl->SetTemplate(pNewTemplate);
		}

		i3UIControl* pChild = (i3UIControl*)pCtrl->getFirstChild();

		while (pChild)
		{
			RefreshTemplateForTool_Rec(pChild);
			pChild = (i3UIControl*)pChild->getNext();
		}
	}

	void RefreshTemplateForTool(CMainFrame* pFrame)
	{
		Ci3UIEditorDoc * pDoc = (Ci3UIEditorDoc*)pFrame->GetActiveDocument();

		if (pDoc->getCurrentScene())
		{
			i3UIControl* pCtrl = (i3UIControl*)pDoc->getCurrentScene()->getFirstChild();
			while (pCtrl)
			{
				RefreshTemplateForTool_Rec(pCtrl);
				pCtrl = (i3UIControl*)pCtrl->getNext();
			}
		}

	}
	*/

}



void CMainFrame::OnUpdateNation(UINT id)
{
	if( m_nNationSel != id )
	{
		// Default Image Info
		if(!m_ImgChecker.IsEnableCheck())
			m_ImgChecker.LoadDefaultImageInfo();

		i3::rc_string strNationName = i3UIFilePathMgr::i()->FindLanguageString(id); //	GetNationString(id);

		CMFCRibbonButton * pBtn = (CMFCRibbonButton*)GetRibbonBar()->GetCategory(2)->GetPanel(7)->GetElement(1);

		pBtn->SetText(strNationName.c_str());

 		if( !m_ImgChecker.CheckNationImageInfo(strNationName.c_str()) )
 			return;

		m_nNationSel = id;

		i3UIManager* pUI = i3UI::getManager();
		if (pUI)
		{
			
			if(strNationName != "Dev")
			{
			//	pUI->SetLocale(strNationName.c_str());
				i3UIFilePathMgr::i()->SetCurrentLanguageID(id);
			}
			else
			{
			//	pUI->SetLocale("");
				i3UIFilePathMgr::i()->SetCurrentLanguageID(-1);
			}
			
//			char szLocalePath[MAX_PATH];
//			i3::snprintf(szLocalePath, sizeof(szLocalePath), "Locale/%s", szNationName);
			
//			pUI->SetUIResourceFolder(szLocalePath);

			i3UILibrary* pLib = pUI->getUILibrary();
			i3::rc_string strOldLibFilePath	= pLib->GetNameString();			// 참조를 붙이면 안됨..
			
			pUI->CloseTemplateLibrary();
			pUI->CloseVirtualTexture();
			/*
			const char * pszWorkDir = i3ResourceFile::GetWorkingFolder();
//			std::string _filename = pszBasePath;
//			i3::transform(_filename.begin(), _filename.end(), _filename.begin(), ::tolower);

			std::string uiversion = pUI->GetUIVersionName()->c_str();
//			i3::transform(uiversion.begin(), uiversion.end(), uiversion.begin(), ::tolower);
			int pos = _filename.find(uiversion.c_str());

			if( pos > 0 )			// WorkDir가 UI버전이 포함된 경로인 경우?? ..아예 이런 경우가 존재하면 안됨..
			{

				char szDir[MAX_PATH];
				i3::extract_directoryname(_filename, szDir);		//vriutalTexture경로제거

				char szRoot[MAX_PATH];
				i3::extract_filename(_filename, szRoot);
				if (i3::generic_is_iequal(szRoot, "VirtualTexture"))
				{
					i3::extract_directoryname(szDir, szDir);			//버전경로제거

					std::string version = uiversion + "\\VirtualTexture";
					pUI->SetUIVersionName(version.c_str());
				}
				
//				i3ResourceFile::SetWorkingFolder(szDir);			// 워킹 폴더를 수시 변경하지 말것..
			}
			*/
// 			if( pUI->GetUIVersionName()->length() <= 0 )
// 			{
// 				if( pos > 0 )
// 				{
// 					std::string viersion = &_filename[pos+1];
// 					pUI->SetUIVersionName(viersion.c_str());
// 				}
// 				_filename[pos] = '\0';
// 				i3ResourceFile::SetWorkingFolder((char*)_filename.c_str());
//  			}
//   			else
//   			{
//   				_filename += "\\";
//   				_filename += pUI->GetUIVersionName()->c_str();
//   				i3ResourceFile::SetWorkingFolder((char*)_filename.c_str());
//   			}

			//selectimage같은파일들 삭제
			i3NamedResourceManager * pManager = i3ResourceFile::GetResourceManager( i3Texture::static_meta());
			if(pManager->GetResourceCount() >0 )
				pManager->DeleteResource(UITEXTURE_EMPTY);
			UIGlobalRes::DeleteAllTexture();

		
			pUI->LoadTemplateLibrary(strOldLibFilePath.c_str()); // "UIRe\\PointBlankRe.i3Uil");	//strOldLibFilePath.c_str());  // 이거 문제있음......
						
//			pUI->getUILibrary()->Defragment();

//			RefreshTemplateForTool(this);			// UIL라이브러리 교체시 템플릿 전체 변경되므로, 기존 존재 콘트롤에 새 템플릿을 반영해야한다...
													// i3UIScene에 연결되지 않은 콘트롤의 경우 반영되지 않을 가능성이 있는데..
													// 일단은 놔둬본다.. // 문제가 되면 생성된 콘트롤을 모두 모아놓은뒤, 일일이 교체하는 쪽으로 가닥을 잡아야한다...
													// SetTemplate 선별 후 컨테이너 검색쪽보다는 루프를 돌면서 처리하는게 편할 것이다..
													// SetTemplate은 내부에 서브템플릿등으로 재귀될수 있기 때문에
													// Template 포인터 변수가 기존것과 새것이 동일한지 여부도 따지는게 좋다...
													// 게임에서는 쓸일이 없기 때문에 엔진으로 이 모듈을 떼야할지를 아직 잘 모르는 상태이다..
			{
				Ci3UIEditorDoc * pDoc = (Ci3UIEditorDoc*)GetActiveDocument();
				i3UILibrary* pUILib = pUI->getUILibrary();
				if (pDoc != NULL && pUILib != NULL)
				{

					pDoc->setUILibrary(pUILib);
				}
			}


			theApp.SwitchView(EVIEW_TEMPLATE);
			theApp.SwitchView(EVIEW_SCREEN);
		}
	}
}	

void	CMainFrame::OnDebugExportall()
{
	char dir[ MAX_PATH ];
	i3::snprintf( dir, MAX_PATH, "%s\\UIRe", getWorkingFolder() );

	char msg[ 1024 ];
	i3::snprintf( msg, 1024, "경로의 모든 i3UIe 파일을 export 하시겠습니까?\n경로 : %s\n", dir );

	int nRes = AfxMessageBox( msg, MB_YESNO );

	if( IDNO == nRes )
	{
		return;
	}

	_ExportScene_Directory( dir );

	AfxMessageBox( "완료 되었습니다." );


	/*char   pszPathname[MAX_PATH];
	BROWSEINFO  bInfo;
	::memset(&bInfo, 0, sizeof(bInfo));

	bInfo.hwndOwner		= GetSafeHwnd();
	bInfo.pidlRoot		= NULL;	
	bInfo.lpszTitle		= _T("익스포트 할 디렉토리를 설정 하세요");
	bInfo.ulFlags		= BIF_RETURNONLYFSDIRS;

	ITEMIDLIST* pildBrowse = ::SHBrowseForFolder( &bInfo );

	if( NULL != pildBrowse )	
	{
		if( FALSE == ::SHGetPathFromIDList( pildBrowse, pszPathname ) )
		{
			return;
		}

		_ExportScene_Directory( pszPathname );
	}*/
}

void	CMainFrame::OnUpdateDebugExportall(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( TRUE );
}

void CMainFrame::_LoadClearColor(void)
{
	INT32 R,G,B,A;

	R = theApp.GetProfileInt( LPCTSTR("Settings"), LPCTSTR("Clear Red"), 0) & 0xFF;
	G = theApp.GetProfileInt( LPCTSTR("Settings"), LPCTSTR("Clear Green"), 0) & 0xFF;
	B = theApp.GetProfileInt( LPCTSTR("Settings"), LPCTSTR("Clear Blue"), 0) & 0xFF;
	A = theApp.GetProfileInt( LPCTSTR("Settings"), LPCTSTR("Clear Alpha"), 0) & 0xFF ;

	i3Color::Set( &m_ClearColor, (UINT8) R, (UINT8) G, (UINT8) B, (UINT8) A);
}

void CMainFrame::_SaveClearColor(void)
{
	INT32 R,G,B,A;

	R = (INT32)( i3Color::GetR( &m_ClearColor) * 255.0f);
	G = (INT32)( i3Color::GetG( &m_ClearColor) * 255.0f);
	B = (INT32)( i3Color::GetB( &m_ClearColor) * 255.0f);
	A = (INT32)( i3Color::GetA( &m_ClearColor) * 255.0f);

	theApp.WriteProfileInt( LPCTSTR("Settings"), LPCTSTR("Clear Red"), R);
	theApp.WriteProfileInt( LPCTSTR("Settings"), LPCTSTR("Clear Green"), G);
	theApp.WriteProfileInt( LPCTSTR("Settings"), LPCTSTR("Clear Blue"), B);
	theApp.WriteProfileInt( LPCTSTR("Settings"), LPCTSTR("Clear Alpha"), A);
}

void CMainFrame::_SetClearColor(void)
{
	i3Viewer* pViewer = i3UI::getViewer();

	if( pViewer )
	{
		i3RenderContext * pCtx = pViewer->GetRenderContext();
		if( pCtx )
		{
			COLOR col;
			i3Color::Set( &col, & m_ClearColor);

			pCtx->SetClearColor( &col);
		}
	}
}

void CMainFrame::OnClicked_ClearColor()
{
	i3Color::Set( & m_ClearColor, m_pCOL_ClearColor->getColor());

	_SaveClearColor();

	_SetClearColor();
}

void	CMainFrame::_ExportScene_Directory( const char* dir )
{
	i3UIManager * pUI = i3UI::getManager();

	if( NULL == pUI || NULL == dir )
	{
		return;
	}

	std::string path;
	std::string name;

	TCHAR prevPath[ MAX_PATH ];	

	::GetCurrentDirectory( MAX_PATH, prevPath );
	::SetCurrentDirectory( dir );

	HANDLE hf = NULL; 
	WIN32_FIND_DATA findData;

	hf = ::FindFirstFile( _T("*.i3UIe"), &findData );

	if( INVALID_HANDLE_VALUE == hf )
	{
		return;
	}

	do
	{
		i3UIScene * pScene = pUI->LoadUIScene( findData.cFileName );

		if( pScene )
		{
			name = findData.cFileName;
			
			int _pos = name.rfind( _T('.') );
			name = name.substr( 0, _pos );

			/*path = dir;
			path += "\\";
			path += name;
			path += ".i3UIe";

			i3XMLFile xmlFile;
			xmlFile.setKeyObject( pScene );
			xmlFile.Save( path.c_str() );*/

			path = dir;
			path += "\\";
			path += name;
			path += ".i3UIs";

			i3ResourceFile resFile;

			resFile.setKeyObject( pScene );
			resFile.Save( path.c_str() );

			pUI->RemoveScene( pScene );			
		}

	} while( TRUE == ::FindNextFile( hf, &findData ) );

	::FindClose( hf );
	::SetCurrentDirectory( prevPath );
}

const char*		CMainFrame::GetNationString( int id ) const
{
//	if( i3::generic_is_iequal( i3UI::getManager()->GetUIVersionName()->c_str(), "uire") )
	if (i3UIFilePathMgr::i()->IsV3UI() == false )
	{
		switch( id )
		{
		case ID_LANGUAGE_MIDDLEEAST:		return "MiddleEast";	break;
		case ID_LANGUAGE_THAILAND:			return "Thai";		break;
		case ID_LANGUAGE_ENGLISH:			return "English";		break;
		case ID_LANGUAGE_PHILIPPINES:		return "English/Philippines";	break;
		case ID_LANGUAGE_SINGMAL:			return "English/Singmal";		break;
		case ID_LANGUAGE_NORTHAMERICA:		return "English/USA";	break;
		case ID_LANGUAGE_INDONESIA:			return "Indonesia";		break;
		case ID_LANGUAGE_ITALY:				return "Italian";			break;
		case ID_LANGUAGE_KOREAN:			return "Korean";			break;
		case ID_LANGUAGE_BRAZIL:			return "Brazil";		break;
		case ID_LANGUAGE_RUSSIA:			return "Russian";		break;
		case ID_LANGUAGE_LATINAMERICA:		return "Spanish/Spanish_Chile";	break;
		case ID_LANGUAGE_THAIWAN:			return "Thaiwan";		break;
		case ID_LANGUAGE_TURKY:				return "Turkish";			break;
		case ID_LANGUAGE_VIETNAM:			return "Vietnamese";		break;
		case ID_LANGUAGE_JAPAN:				return "Japanese";		break;
		}
	}
 	else
 	{
 		switch( id )
 		{
 		case ID_LANGUAGE_THAILAND:			return "Thai";		break;
 		case ID_LANGUAGE_PHILIPPINES:		return "Philippines";	break;
 		case ID_LANGUAGE_SINGMAL:			return "Singmal";		break;
 		case ID_LANGUAGE_NORTHAMERICA:		return "NorthAmerica";	break;
 		case ID_LANGUAGE_INDONESIA:			return "Indonesia";		break;
 		case ID_LANGUAGE_ITALY:				return "Italy";			break;
 		case ID_LANGUAGE_KOREAN:			return "Korea";			break;
 		case ID_LANGUAGE_BRAZIL:			return "Brazil";		break;
 		case ID_LANGUAGE_RUSSIA:			return "Russia";		break;
 		case ID_LANGUAGE_LATINAMERICA:		return "LatinAmerica";	break;
 		case ID_LANGUAGE_THAIWAN:			return "Thaiwan";		break;
 		case ID_LANGUAGE_TURKY:				return "Turky";			break;
  		case ID_LANGUAGE_VIETNAM:			return "Vietnam";		break;
  		case ID_LANGUAGE_SPANISH:			return "Spanish";		break;
  		case ID_LANGUAGE_CHINA:				return "China";			break;
  		case ID_LANGUAGE_ARABIC:			return "Arabic";		break;
		case ID_LANGUAGE_JAPAN:				return "Japan";		break;
		case ID_LANGUAGE_ENGLISH:				return "English";		break;
 		}
 	}

	return "Dev";
}


void CMainFrame::UpdateVirtualTextureFiles()
{
	/*
	//
	// 국가폴더안에 VTex 관련 파일을 지우고, UIRe폴더에 있는 파일을 복사한다.
	if( m_nNationSel != ID_LANGUAGE_DEV )
	{
		const char * pszNation = GetNationString( m_nNationSel );
		//m_toolFileMgr.RemoveNationFolderFile( pszNation );
		//m_toolFileMgr.MoveToNatioFolder( pszNation );
	}
	*/
}

void CMainFrame::UpdateNationFromVTex()
{
	char pszNation[MAX_PATH] = {""};
	if(m_ImgChecker.GetNationFromVirtualTexture( pszNation ))
	{
		i3::iterator_range<const char*> strNation = i3::as_literal(pszNation);


		if( i3::generic_is_iequal( strNation, "MiddleEast") )
			m_nNationSel = ID_LANGUAGE_MIDDLEEAST;
		else if( i3::generic_is_iequal( strNation, "Thai") )
			m_nNationSel = ID_LANGUAGE_THAILAND;
		else if( i3::generic_is_iequal( strNation, "English") )
			m_nNationSel = ID_LANGUAGE_ENGLISH;
		else if( i3::generic_is_iequal( strNation, "English/Philippines") )
			m_nNationSel = ID_LANGUAGE_PHILIPPINES;
		else if( i3::generic_is_iequal( strNation, "English/Singmal") )
			m_nNationSel = ID_LANGUAGE_SINGMAL;
		else if( i3::generic_is_iequal( strNation, "English/USA") )
			m_nNationSel = ID_LANGUAGE_NORTHAMERICA;
		else if( i3::generic_is_iequal( strNation, "Indonesia") )
			m_nNationSel = ID_LANGUAGE_INDONESIA;
		else if( i3::generic_is_iequal( strNation, "Italian") )
			m_nNationSel = ID_LANGUAGE_ITALY;
		else if( i3::generic_is_iequal( strNation, "Korean") )
			m_nNationSel = ID_LANGUAGE_KOREAN;
		else if( i3::generic_is_iequal( strNation, "Brazil") )
			m_nNationSel = ID_LANGUAGE_BRAZIL;
		else if( i3::generic_is_iequal( strNation, "Russia") )
			m_nNationSel = ID_LANGUAGE_RUSSIA;
		else if( i3::generic_is_iequal( strNation, "LatinAmerica") )
			m_nNationSel = ID_LANGUAGE_LATINAMERICA;
		else if( i3::generic_is_iequal( strNation, "Thaiwan") )
			m_nNationSel = ID_LANGUAGE_THAIWAN;
		else if( i3::generic_is_iequal( strNation, "Turky") )
			m_nNationSel = ID_LANGUAGE_TURKY;
		else if (i3::generic_is_iequal(strNation, "Vietnam"))
			m_nNationSel = ID_LANGUAGE_VIETNAM;
		else if (i3::generic_is_iequal(strNation, "Japan"))
			m_nNationSel = ID_LANGUAGE_JAPAN;
		else
			m_nNationSel = ID_LANGUAGE_DEV;
	}

	if( m_nNationSel != ID_LANGUAGE_DEV )
	{
		if( ::AfxMessageBox( "사용중인 Virtual Texture가 기본국가가 아닙니다.\n 변경하시겠습니까?", MB_YESNO) == IDYES)
		{
			i3VirtualTexture* pVTex = i3UI::getVirtualTexture();
			if( pVTex )
			{
				i3VTFileFormat* pFF = pVTex->GetFileFormat();

				pFF->LoadRefTextureInfo();
				pFF->Defragment();
				pFF->Flush();
				::AfxMessageBox( "기본 국가로 변경되었습니다." );
				m_nNationSel = ID_LANGUAGE_DEV;
			}
		}
	}
}

void CMainFrame::UpdateDefaultImageInfo()
{
	m_ImgChecker.LoadDefaultImageInfo();
}