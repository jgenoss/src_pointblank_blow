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
#include "i3GICShapeTool.h"

#include "MainFrm.h"

#include "GlobalVariables.h"

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
	ON_COMMAND(ID_FILE_NEW, &CMainFrame::OnFileNew)
	ON_COMMAND(ID_FILE_SAVE, &CMainFrame::OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, &CMainFrame::OnFileSaveAs)
	ON_COMMAND(ID_FILE_OPEN, &CMainFrame::OnFileOpen)
	ON_COMMAND(ID_FILE_PRINT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CMainFrame::OnFilePrint)

	ON_COMMAND( IDC_CHK_RIBBON_GRID, &CMainFrame::OnView_Grid)
	ON_UPDATE_COMMAND_UI( IDC_CHK_RIBBON_GRID, &CMainFrame::OnUpdateView_Grid)

	ON_COMMAND( IDC_SPIN_GRIDSIZE, &CMainFrame::OnView_GridSize)

	ON_COMMAND( IDC_CHK_LINKSNAP, &CMainFrame::OnView_LinkSnap)
	ON_UPDATE_COMMAND_UI( IDC_CHK_LINKSNAP, &CMainFrame::OnUpdateView_LinkSnap)

	ON_COMMAND( IDC_CHK_GRIDSNAP, &CMainFrame::OnView_GridSnap)
	ON_UPDATE_COMMAND_UI( IDC_CHK_GRIDSNAP, &CMainFrame::OnUpdateView_GridSnap)

	// Font
	ON_COMMAND( IDC_CB_FONT, &CMainFrame::OnFont_Style)
	ON_UPDATE_COMMAND_UI( IDC_CB_FONT, &CMainFrame::OnUpdateFont_Style)
	ON_COMMAND( IDC_CB_FONTSIZE, &CMainFrame::OnFont_Size)
	ON_UPDATE_COMMAND_UI( IDC_CB_FONT, &CMainFrame::OnUpdateFont_Size)
	ON_COMMAND( ID_FONT_UNDERLINE, &CMainFrame::OnFont_Underline)
	ON_UPDATE_COMMAND_UI( ID_FONT_UNDERLINE, &CMainFrame::OnUpdateFont_Underline)
	ON_COMMAND( ID_FONT_ITALIC, &CMainFrame::OnFont_Italic)
	ON_UPDATE_COMMAND_UI( ID_FONT_ITALIC, &CMainFrame::OnUpdateFont_Italic)
	ON_COMMAND( ID_FONT_COLOR, &CMainFrame::OnFont_Color)
	ON_UPDATE_COMMAND_UI( ID_FONT_COLOR, &CMainFrame::OnUpdateFont_Color)
	ON_COMMAND( ID_FONT_BKCOLOR, &CMainFrame::OnFont_BkColor)
	ON_UPDATE_COMMAND_UI( ID_FONT_BKCOLOR, &CMainFrame::OnUpdateFont_BkColor)
	ON_COMMAND( ID_FONT_STRETCH, &CMainFrame::OnFont_Stretch)
	ON_UPDATE_COMMAND_UI( ID_FONT_STRETCH, &CMainFrame::OnUpdateFont_Stretch)
	

	// Align
	ON_COMMAND( ID_BTN_SHAPETOOLS_ALIGN_LEFT, &CMainFrame::OnTools_AlignLeft)
	ON_UPDATE_COMMAND_UI( ID_BTN_SHAPETOOLS_ALIGN_LEFT, &CMainFrame::OnUpdateTools_AlignLeft)
	ON_COMMAND( ID_BTN_SHAPETOOLS_ALIGN_CENTER, &CMainFrame::OnTools_AlignCenter)
	ON_UPDATE_COMMAND_UI( ID_BTN_SHAPETOOLS_ALIGN_CENTER, &CMainFrame::OnUpdateTools_AlignCenter)
	ON_COMMAND( ID_BTN_SHAPETOOLS_ALIGN_RIGHT, &CMainFrame::OnTools_AlignRight)
	ON_UPDATE_COMMAND_UI( ID_BTN_SHAPETOOLS_ALIGN_RIGHT, &CMainFrame::OnUpdateTools_AlignRight)
	ON_COMMAND( ID_BTN_SHAPETOOLS_ALIGN_TOP, &CMainFrame::OnTools_AlignTop)
	ON_UPDATE_COMMAND_UI( ID_BTN_SHAPETOOLS_ALIGN_TOP, &CMainFrame::OnUpdateTools_AlignTop)
	ON_COMMAND( ID_BTN_SHAPETOOLS_ALIGN_MIDDLE, &CMainFrame::OnTools_AlignMiddle)
	ON_UPDATE_COMMAND_UI( ID_BTN_SHAPETOOLS_ALIGN_MIDDLE, &CMainFrame::OnUpdateTools_AlignMiddle)
	ON_COMMAND( ID_BTN_SHAPETOOLS_ALIGN_BOTTOM, &CMainFrame::OnTools_AlignBottom)
	ON_UPDATE_COMMAND_UI( ID_BTN_SHAPETOOLS_ALIGN_BOTTOM, &CMainFrame::OnUpdateTools_AlignBottom)


	// Mode
	ON_COMMAND( IDC_BTN_MODE_SELECT, &CMainFrame::OnMode_Select)
	ON_UPDATE_COMMAND_UI( IDC_BTN_MODE_SELECT, &CMainFrame::OnUpdateMode_Select)
	ON_COMMAND( IDC_BTN_MODE_LINK, &CMainFrame::OnMode_Link)
	ON_UPDATE_COMMAND_UI( IDC_BTN_MODE_LINK, &CMainFrame::OnUpdateMode_Link)

END_MESSAGE_MAP()

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	m_pFramework = NULL;
	m_pRenderTarget = NULL;
	m_pViewer = NULL;
	m_pRenderBuffer = NULL;
	m_szFilePath[0] = '\0';

	m_bChangeTemplate = false;

	m_pCBFont = NULL;

	m_bUnderline = FALSE;
	m_bItalic = FALSE;
	m_bStretch = FALSE;
}

CMainFrame::~CMainFrame()
{
	I3_SAFE_RELEASE( m_pRenderTarget);
	I3_SAFE_RELEASE( m_pFramework);
	I3_SAFE_RELEASE( m_pViewer);
	I3_SAFE_RELEASE( m_pRenderBuffer);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	// set the visual style to be used the by the visual manager
	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);

	// set the visual manager used to draw all user interface elements
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));

	m_wndRibbonBar.Create(this);
	InitializeRibbon();

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
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

	if( g_pTDKGlobalRes == NULL)
		i3TDKGlobalRes::Init();
	
	_CreateFramework();
	_CreatePane();

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

void CMainFrame::_CreateRibbonView( CMFCRibbonCategory * pCategory)
{
	CMFCRibbonPanel* pPanelView = pCategory->AddPanel( _T("View"), m_PanelImages.ExtractIcon (7));

	CMFCRibbonButton * pBtnGrid = new CMFCRibbonCheckBox( IDC_CHK_RIBBON_GRID, _T("Grid") );
	pPanelView->Add( pBtnGrid);

	CMFCRibbonEdit * pRibbonEdGridSize = new CMFCRibbonEdit( IDC_SPIN_GRIDSIZE, 50, _T("GridSize"));
	pRibbonEdGridSize->EnableSpinButtons( 20, 120);
	pRibbonEdGridSize->SetEditText( _T( "30"));
	pPanelView->Add( pRibbonEdGridSize);

	CMFCRibbonButton * pBtnSnap = new CMFCRibbonCheckBox( IDC_CHK_LINKSNAP, _T("Linker Editable"));
	pBtnSnap->SetToolTipText( _T("Shape의 Link를 수정할 수 있는 모드"));
	pPanelView->Add( pBtnSnap);

	CMFCRibbonButton * pBtnGridSnap = new CMFCRibbonCheckBox( IDC_CHK_GRIDSNAP, _T("SnapToGrid"));
	pPanelView->Add( pBtnGridSnap);

	/*
	bNameValid = strTemp.LoadString(IDS_RIBBON_STATUSBAR);
	ASSERT(bNameValid);
	CMFCRibbonButton* pBtnStatusBar = new CMFCRibbonCheckBox(ID_VIEW_STATUS_BAR, strTemp);
	pPanelView->Add(pBtnStatusBar);
	bNameValid = strTemp.LoadString(IDS_RIBBON_CAPTIONBAR);
	ASSERT(bNameValid);
	CMFCRibbonButton* pBtnCaptionBar = new CMFCRibbonCheckBox(ID_VIEW_CAPTION_BAR, strTemp);
	pPanelView->Add(pBtnCaptionBar);
	*/
}

void CMainFrame::_CreateRibbonFont( CMFCRibbonCategory * pCategory)
{
	CMFCRibbonPanel * pPanelFont = pCategory->AddPanel( _T("Font"));

	CMFCRibbonButtonsGroup* pFontGroup = new CMFCRibbonButtonsGroup;

	CMFCRibbonFontComboBox::m_bDrawUsingFont = TRUE;
	m_pCBFont = new CMFCRibbonFontComboBox( IDC_CB_FONT);
	m_pCBFont->SelectItem(_T("Arial"));
	m_pCBFont->SetWidth(55, TRUE);
	pFontGroup->AddButton(m_pCBFont);

	m_pCBFontSize = new CMFCRibbonComboBox( IDC_CB_FONTSIZE, TRUE, 39);
	m_pCBFontSize->AddItem(_T("8"));
	m_pCBFontSize->AddItem(_T("9"));
	m_pCBFontSize->AddItem(_T("10"));
	m_pCBFontSize->AddItem(_T("11"));
	m_pCBFontSize->AddItem(_T("12"));
	m_pCBFontSize->AddItem(_T("14"));
	m_pCBFontSize->AddItem(_T("16"));
	m_pCBFontSize->AddItem(_T("18"));
	m_pCBFontSize->AddItem(_T("20"));
	m_pCBFontSize->AddItem(_T("22"));
	m_pCBFontSize->AddItem(_T("24"));
	m_pCBFontSize->AddItem(_T("26"));
	m_pCBFontSize->AddItem(_T("28"));
	m_pCBFontSize->AddItem(_T("36"));
	m_pCBFontSize->AddItem(_T("48"));
	m_pCBFontSize->AddItem(_T("72"));
	m_pCBFontSize->SetWidth(20, TRUE); // Width in "floaty" mode
	m_pCBFontSize->SelectItem(2);
	pFontGroup->AddButton(m_pCBFontSize);

	pPanelFont->Add(pFontGroup);

	// Add toolbar(all toolbar buttons will be automatically
	// converted to ribbon elements:
	pPanelFont->AddToolBar(IDR_FONT);

	// Replace ID_FONT_COLOR and ID_FONT_TEXTHIGHLIGHT elements
	// by color pickers:
	m_pBtnFontColor = new CMFCRibbonColorButton();
	m_pBtnFontColor->EnableAutomaticButton(_T("&Automatic"), RGB(0, 0, 0));
	m_pBtnFontColor->EnableOtherButton(_T("&More Colors..."), _T("More Colors"));
	m_pBtnFontColor->SetColumns(10);
	m_pBtnFontColor->SetColor(RGB(255, 0, 0));
	m_pBtnFontColor->SetColorBoxSize(CSize(17, 17));

	m_pBtnFontColor->AddColorsGroup(_T("Theme Colors"), m_lstMainColors);
	m_pBtnFontColor->AddColorsGroup(_T(""), m_lstAdditionalColors, TRUE /* Contiguous Columns*/);
	m_pBtnFontColor->AddColorsGroup(_T("Standard Colors"), m_lstStandardColors);

	pPanelFont->ReplaceByID( ID_FONT_COLOR, m_pBtnFontColor);

	m_pBtnFontBkColor = new CMFCRibbonColorButton();
	m_pBtnFontBkColor->EnableAutomaticButton(_T("&Automatic"), RGB(255, 255, 255));
	m_pBtnFontBkColor->EnableOtherButton(_T("&More Colors..."), _T("More Colors"));
	m_pBtnFontBkColor->SetColumns(10);
	m_pBtnFontBkColor->SetColor(RGB(0, 0, 0));
	m_pBtnFontBkColor->SetColorBoxSize(CSize(17, 17));

	m_pBtnFontBkColor->AddColorsGroup(_T("Theme Colors"), m_lstMainColors);
	m_pBtnFontBkColor->AddColorsGroup(_T(""), m_lstAdditionalColors, TRUE /* Contiguous Columns*/);
	m_pBtnFontBkColor->AddColorsGroup(_T("Standard Colors"), m_lstStandardColors);

	pPanelFont->ReplaceByID( ID_FONT_BKCOLOR, m_pBtnFontBkColor);

	/*
	CMFCRibbonColorButton* pFontColorHighlightBtn = new CMFCRibbonColorButton();
	pFontColorHighlightBtn->SetColor(RGB(255, 255, 255));
	pFontColorHighlightBtn->EnableAutomaticButton(_T("&No Color"), RGB(240, 240, 240), TRUE, NULL, FALSE , TRUE );

	pPanelFont->ReplaceByID( ID_FONT_HIGHLIGHT, pFontColorHighlightBtn);

	pFontColorHighlightBtn->SetColorBoxSize(CSize(26, 26));
	pFontColorHighlightBtn->AddSubItem(new CMFCRibbonButton( ID_FONT_STOP_HIGHLIGHT, _T("&Stop Highlighting")));
	*/

	// Add the launch button at the bottom of the panel:
	pPanelFont->EnableLaunchButton( ID_WRITE_FONT);
}

void CMainFrame::_CreateRibbonShapeTool( CMFCRibbonCategory * pCategory)
{
	I3ASSERT( pCategory != NULL);

	CMFCRibbonPanel * pPanelTools = pCategory->AddPanel( I3STR("Tools"));
	
	m_Pane_Images.Load(IDR_SHAPETOOLS);

	CMFCRibbonButtonsGroup* pToolsGroup = new CMFCRibbonButtonsGroup;

	CMFCRibbonButton * pBtnAlignLeft = new CMFCRibbonButton( ID_BTN_SHAPETOOLS_ALIGN_LEFT, I3STR("Align Left"), m_Pane_Images.ExtractIcon(0) );
	pToolsGroup->AddButton( pBtnAlignLeft);

	CMFCRibbonButton * pBtnAlignCenter = new CMFCRibbonButton( ID_BTN_SHAPETOOLS_ALIGN_CENTER, I3STR("Align Center"), m_Pane_Images.ExtractIcon(1));
	pToolsGroup->AddButton( pBtnAlignCenter);

	CMFCRibbonButton * pBtnAlignRight = new CMFCRibbonButton( ID_BTN_SHAPETOOLS_ALIGN_RIGHT, I3STR("Align Right"), m_Pane_Images.ExtractIcon(2));
	pToolsGroup->AddButton( pBtnAlignRight);

	pPanelTools->Add( pToolsGroup);

	pToolsGroup = new CMFCRibbonButtonsGroup;

	CMFCRibbonButton * pBtnAlignTop = new CMFCRibbonButton( ID_BTN_SHAPETOOLS_ALIGN_TOP, I3STR("Align Top"), m_Pane_Images.ExtractIcon(3));
	pToolsGroup->AddButton( pBtnAlignTop);

	CMFCRibbonButton * pBtnAlignMiddle = new CMFCRibbonButton( ID_BTN_SHAPETOOLS_ALIGN_MIDDLE, I3STR("Align Middle"), m_Pane_Images.ExtractIcon(4));
	pToolsGroup->AddButton( pBtnAlignMiddle);

	CMFCRibbonButton * pBtnAlignBottom = new CMFCRibbonButton( ID_BTN_SHAPETOOLS_ALIGN_BOTTOM, I3STR("Align Bottom"), m_Pane_Images.ExtractIcon(5));
	pToolsGroup->AddButton( pBtnAlignBottom);

	pPanelTools->Add( pToolsGroup);

}

void CMainFrame::_CreateRibbonMode( CMFCRibbonCategory * pCategory)
{
	I3ASSERT( pCategory != NULL);

	CMFCRibbonPanel * pPanelMode = pCategory->AddPanel( I3STR("Mode"));
	
	CMFCToolBarImages image;
	
	image.Load(IDB_BTN_MODE);

	CMFCRibbonButton * pBtnSelect = new CMFCRibbonButton( IDC_BTN_MODE_SELECT, I3STR("Select Mode"), image.ExtractIcon(0) );
	pPanelMode->Add( pBtnSelect);

	CMFCRibbonButton * pBtnLink = new CMFCRibbonButton( IDC_BTN_MODE_LINK, I3STR("Link Mode"), image.ExtractIcon(1) );
	pPanelMode->Add( pBtnLink);

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

	// Add "Home" category with "Clipboard" panel:
	bNameValid = strTemp.LoadString(IDS_RIBBON_HOME);
	ASSERT(bNameValid);
	CMFCRibbonCategory* pCategoryHome = m_wndRibbonBar.AddCategory(strTemp, IDB_WRITESMALL, IDB_WRITELARGE);

	// Create "Clipboard" panel:
	bNameValid = strTemp.LoadString(IDS_RIBBON_CLIPBOARD);
	ASSERT(bNameValid);
	CMFCRibbonPanel* pPanelClipboard = pCategoryHome->AddPanel(strTemp, m_PanelImages.ExtractIcon(27));

	bNameValid = strTemp.LoadString(IDS_RIBBON_PASTE);
	ASSERT(bNameValid);
	CMFCRibbonButton* pBtnPaste = new CMFCRibbonButton(ID_EDIT_PASTE, strTemp, 0, 0);
	pPanelClipboard->Add(pBtnPaste);

	bNameValid = strTemp.LoadString(IDS_RIBBON_CUT);
	ASSERT(bNameValid);
	pPanelClipboard->Add(new CMFCRibbonButton(ID_EDIT_CUT, strTemp, 1));
	bNameValid = strTemp.LoadString(IDS_RIBBON_COPY);
	ASSERT(bNameValid);
	pPanelClipboard->Add(new CMFCRibbonButton(ID_EDIT_COPY, strTemp, 2));
	bNameValid = strTemp.LoadString(IDS_RIBBON_SELECTALL);
	ASSERT(bNameValid);
	pPanelClipboard->Add(new CMFCRibbonButton(ID_EDIT_SELECT_ALL, strTemp, -1));

	// Create and add a "View" panel:
	_CreateRibbonView( pCategoryHome);

	// Create Font property
	_CreateRibbonFont( pCategoryHome);

	_CreateRibbonShapeTool( pCategoryHome);

	_CreateRibbonMode( pCategoryHome);

	// Add quick access toolbar commands:
	CList<UINT, UINT> lstQATCmds;

	lstQATCmds.AddTail(ID_FILE_NEW);
	lstQATCmds.AddTail(ID_FILE_OPEN);
	lstQATCmds.AddTail(ID_FILE_SAVE);
	lstQATCmds.AddTail(ID_FILE_PRINT_DIRECT);

	m_wndRibbonBar.SetQuickAccessCommands(lstQATCmds);
	m_wndRibbonBar.AddToTabs(new CMFCRibbonButton(ID_APP_ABOUT, _T("\na"), m_PanelImages.ExtractIcon (0)));
}

void CMainFrame::_CreatePane( void)
{
	RECT rect;

	GetClientRect( &rect);

	{
		if( !m_Pane_TemplateView.Create( I3STR("Template"), this, CRect( &rect), TRUE, IDD_PANE_TEMPLATE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP | CBRS_FLOAT_MULTI))
		{
			I3FATAL( I3STR("Failed to Create TemplateView Window\n"));
			return;
		}
		
		m_Pane_TemplateView.SetBorders( 1, 1);
		m_Pane_TemplateView.EnableDocking( CBRS_ALIGN_ANY);

		m_Pane_TemplateView.DockToWindow( &m_Pane_PreView, CBRS_ALIGN_BOTTOM);
	}

	{
		if( !m_Pane_ShapesView.Create( I3STR("Shapes"), this, CRect( &rect), TRUE, IDD_PANE_SHAPES, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP | CBRS_FLOAT_MULTI))
		{
			I3FATAL( I3STR("Failed to Create ShapesView Window\n"));
			return;
		}

		m_Pane_ShapesView.SetBorders( 1, 1);
		m_Pane_ShapesView.EnableDocking( CBRS_ALIGN_ANY);

		DockPane( &m_Pane_ShapesView, AFX_IDW_DOCKBAR_LEFT);
	}
	
	{
		if( !m_Pane_SpecificView.Create( I3STR("Specific"), this, CRect( &rect), TRUE, IDD_PANE_SPECIFIC, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP | CBRS_FLOAT_MULTI))
		{
			I3FATAL( I3STR("Failed to Create TemplateView Window\n"));
			return;
		}

		m_Pane_SpecificView.SetBorders( 1, 1);
		m_Pane_SpecificView.EnableDocking( CBRS_ALIGN_ANY);

		m_Pane_SpecificView.DockToWindow( &m_Pane_ShapesView, CBRS_ALIGN_BOTTOM);
	}

	{
		if( !m_Pane_DesignView.Create( I3STR("Design"), this, CRect( &rect), TRUE, IDD_PANE_DESIGN, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP | CBRS_FLOAT_MULTI))
		{
			I3FATAL( I3STR("Failed to Create TemplateView Window\n"));
			return;
		}

		m_Pane_DesignView.SetBorders( 1, 1);
		m_Pane_DesignView.EnableDocking( CBRS_ALIGN_ANY);

		DockPane( &m_Pane_DesignView);
	}

	{
		if( !m_Pane_LogView.Create( I3STR("Log"), this, CRect( &rect), TRUE, IDD_PANE_LOG, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP | CBRS_FLOAT_MULTI))
		{
			I3FATAL( I3STR("Failed to Create TemplateView Window\n"));
			return;
		}

		m_Pane_LogView.SetBorders( 1, 1);
		m_Pane_LogView.EnableDocking( CBRS_ALIGN_ANY);

		DockPane( &m_Pane_LogView);
	}
	
}

void CMainFrame::_CreateFramework( void)
{
	m_pViewer = i3Viewer::NewObject();

	i3VideoInfo video;

	video.m_Width		= 1280;
	video.m_Height		= 900;
	video.m_bFullScreen = FALSE;
	video.m_bVSync		= TRUE;
	video.m_MultiSampling			= 0;
	video.m_MultiSamplingQuality	= 0;

	video.m_RenderTargetFormat = I3G_IMAGE_FORMAT_RGBX_8888;

	m_Pane_PreView.Create( I3STR("PreView"), this, CRect(0, 0, 200, 200), TRUE, IDD_PANE_PREVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP | CBRS_FLOAT_MULTI);
	m_Pane_PreView.SetBorders( 1, 1);
	m_Pane_PreView.EnableDocking( CBRS_ALIGN_ANY);

	DockPane( &m_Pane_PreView, AFX_IDW_DOCKBAR_LEFT);

	m_pViewer->Create( m_Pane_PreView.m_hWnd, &video, I3I_DEVICE_MASK_CONTROLLER);
	m_pViewer->SetPendingUpdateEnable( true);

	m_pFramework = CFramework::NewObject();
	m_pFramework->Create( m_pViewer);

	g_pFramework = m_pFramework;

	m_pRenderTarget = i3Texture::NewObject();

	
	m_rectRenderView.left		= m_rectRenderView.top = 0;
	m_rectRenderView.right	= 1280;
	m_rectRenderView.bottom	= 900;
	m_pRenderTarget->Create( m_rectRenderView.right, m_rectRenderView.bottom, 1, I3G_IMAGE_FORMAT_RGBX_8888, I3G_USAGE_RENDERTARGET);

	i3RenderContext * pCtx = m_pViewer->GetRenderContext();
	pCtx->SetColorRenderTarget( m_pRenderTarget);

#if defined( I3_DEBUG_DUMP)
	m_pFramework->DumpObjects();
#endif
}

void CMainFrame::MouseMove( CPoint point)
{
	m_Pane_ShapesView.ProcessMouseMove( point);
}

void CMainFrame::LButtonUp( UINT nFlags, CPoint point, bool bInsertShape)
{
	m_Pane_ShapesView.ProcessLButtonUp( nFlags, point, bInsertShape);
}

BOOL s_TreeNodeFlagToInstancing( INT32 depth, i3TreeNode * pNode, void * pUserData)
{
	bool bFlag = *(bool*)pUserData;
	if( !pNode->IsExactTypeOf( i3GICControlPoint::GetClassMeta() ) )
	{
		if( !bFlag)
			pNode->removeNodeStyle( I3_TREENODE_STYLE_VOLATILE);
		else
			pNode->addNodeStyle( I3_TREENODE_STYLE_VOLATILE);
	}

	return TRUE;
}

void CMainFrame::DropShape( i3GICShape * pShape, CPoint point)
{
	// Shape을 생성해서 Diagram View Root에 붙인다.
	if( getDiagram()->getTemplateCount() == 0)
	{
		MessageBox( I3STR("편집할 Template를 생성하지 않았거나, 선택하지 않았습니다."));
		return;
	}

	bool bFlag = true;
	i3GICShape * pNewShape = NULL;

	i3TreeNode::Traverse( pShape, (I3_TREE_TRAVERSE_PROC) s_TreeNodeFlagToInstancing, &bFlag);

	pNewShape = (i3GICShape*) i3GICShape::CreateClone( pShape, I3_COPY_INSTANCE);

	bFlag = false;
	i3TreeNode::Traverse( pShape, (I3_TREE_TRAVERSE_PROC) s_TreeNodeFlagToInstancing, &bFlag);

	I3ASSERT( pNewShape != NULL);

	pNewShape->OnCreate();
	pNewShape->setPos( (REAL32)point.x, (REAL32)point.y);
	
	m_Pane_DesignView.addShape( pNewShape);
	m_Pane_TemplateView.AttachShapeToTemplate( pNewShape);

	m_bChangeTemplate = true;
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

void CMainFrame::OnFileNew()
{
	if( m_bChangeTemplate)
	{
		int rv = MessageBox( I3STR("Template가 수정되었습니다. 저장하지 않고 계속하시겠습니까?"), NULL, MB_YESNO);

		if( rv == IDNO)
			return;
	}

	m_Pane_TemplateView.RemoveAllTemplate();
}

void CMainFrame::OnFileSave()
{
	if( m_szFilePath[0] == 0)
	{
		OnFileSaveAs();
		return;
	}

	saveTemplate( m_szFilePath);
}

void CMainFrame::OnFileSaveAs()
{
	CFileDialog	Dlg( FALSE, I3STR("XML"), NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		I3STR("i3GICShape Template File(*.XML)|*.XML||"), this);

	if( Dlg.DoModal() != IDOK)return;

	saveTemplate( (const char *) Dlg.GetPathName().GetString() );
}

void CMainFrame::OnFileOpen()
{
	CFileDialog	Dlg( TRUE, I3STR("XML"), NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		I3STR("i3GICShape Template File(*.XML)|*.XML||"), this);

	if( Dlg.DoModal() != IDOK)return;

	if( m_bChangeTemplate)
	{
		int rv = MessageBox( I3STR("Template가 수정되었습니다. 저장하지 않고 계속하시겠습니까?"), NULL, MB_YESNO);

		if( rv == IDNO)
			return;
	}

	m_Pane_TemplateView.RemoveAllTemplate();

	loadTemplate( (const char *) Dlg.GetPathName().GetString() );
	m_Pane_TemplateView.UpdateListCtrl();
}

void CMainFrame::OnFilePrint()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_PRINT);
	}
}





