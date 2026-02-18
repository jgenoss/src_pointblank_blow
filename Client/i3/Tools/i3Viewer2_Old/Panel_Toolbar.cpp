// Panel_Toolbar.cpp : implementation file
//

#include "stdafx.h"
#include "i3Viewer2.h"
#include "Panel_Toolbar.h"
#include "OptionInfo.h"
#include "GlobalVar.h"
#include ".\panel_toolbar.h"

// CPanel_Toolbar dialog

IMPLEMENT_DYNAMIC(CPanel_Toolbar, CDialog)
CPanel_Toolbar::CPanel_Toolbar(CWnd* pParent /*=NULL*/)
	: CDialog(CPanel_Toolbar::IDD, pParent)
{
}

CPanel_Toolbar::~CPanel_Toolbar()
{
}

void CPanel_Toolbar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_NORMALVIEW, m_BTN_ModeNormal);
	DDX_Control(pDX, IDC_BTN_MESHVIEW, m_BTN_ModeMesh);
	DDX_Control(pDX, IDC_BTN_COLLVIEW, m_BTN_ModeColl);
	DDX_Control(pDX, IDC_BTN_WIREVIEW, m_BTN_ModeWire);
	DDX_Control(pDX, IDC_BTN_SHADOWENABLE, m_BTN_ShadowEnable);
	DDX_Control(pDX, IDC_BTN_GRID, m_BTN_Grid);
	DDX_Control(pDX, IDC_SLIDER_TIMESCALE, m_SL_TimeScale);
	DDX_Control(pDX, IDC_BTN_VIEWONLYSEL, m_BTN_ModeSelOnly);
}


BEGIN_MESSAGE_MAP(CPanel_Toolbar, CDialog)
	ON_BN_CLICKED(IDC_BTN_GRID, OnBnClickedBtnGrid)
	ON_BN_CLICKED(IDC_BTN_SHADOWENABLE, OnBnClickedBtnShadowenable)
	ON_BN_CLICKED(IDC_BTN_NORMALVIEW, OnBnClickedBtnNormalview)
	ON_BN_CLICKED(IDC_BTN_MESHVIEW, OnBnClickedBtnMeshview)
	ON_BN_CLICKED(IDC_BTN_COLLVIEW, OnBnClickedBtnCollview)
	ON_BN_CLICKED(IDC_BTN_WIREVIEW, OnBnClickedBtnWireview)
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnUpdateToolTip )
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_DYNAMIC, OnBnClickedBtnDynamic)
	ON_BN_CLICKED(IDC_BTN_KINEMATIC, OnBnClickedBtnKinematic)
	ON_BN_CLICKED(IDC_BTN_VIEWONLYSEL, OnBnClickedBtnViewonlysel)
END_MESSAGE_MAP()


// CPanel_Toolbar message handlers

BOOL CPanel_Toolbar::OnInitDialog()
{
	HICON hIcon;

	CDialog::OnInitDialog();

	{
		HINSTANCE hInst = AfxGetApp()->m_hInstance;

		hIcon = (HICON) ::LoadImage( hInst, MAKEINTRESOURCE( IDI_MODE_NORMAL), IMAGE_ICON, 24, 24, LR_VGACOLOR);
		m_BTN_ModeNormal.SetIcon( hIcon);

		hIcon = (HICON) ::LoadImage( hInst, MAKEINTRESOURCE( IDI_MODE_MESH), IMAGE_ICON, 24, 24, LR_VGACOLOR);
		m_BTN_ModeMesh.SetIcon( hIcon);

		hIcon = (HICON) ::LoadImage( hInst, MAKEINTRESOURCE( IDI_MODE_COLL), IMAGE_ICON, 24, 24, LR_VGACOLOR);
		m_BTN_ModeColl.SetIcon( hIcon);

		hIcon = (HICON) ::LoadImage( hInst, MAKEINTRESOURCE( IDI_MODE_WIRE), IMAGE_ICON, 24, 24, LR_VGACOLOR);
		m_BTN_ModeWire.SetIcon( hIcon);

		hIcon = (HICON) ::LoadImage( hInst, MAKEINTRESOURCE( IDI_SHADOW), IMAGE_ICON, 24, 24, LR_VGACOLOR);
		m_BTN_ShadowEnable.SetIcon( hIcon);

		hIcon = (HICON) ::LoadImage( hInst, MAKEINTRESOURCE( IDI_GRID), IMAGE_ICON, 24, 24, LR_VGACOLOR);
		m_BTN_Grid.SetIcon( hIcon);
	}

	{
		m_SL_TimeScale.SetLineSize( 10);
		m_SL_TimeScale.SetRange( -20, 20, TRUE);
		m_SL_TimeScale.SetTic( 1);
		m_SL_TimeScale.SetTicFreq( 5);
		m_SL_TimeScale.SetPos( 20);
	}

	EnableToolTips();

	_Update();

	i3TDK::RegisterUpdate( m_hWnd, COptionInfo::GetClassMeta(), I3_TDK_UPDATE_EDIT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanel_Toolbar::OnOK()
{
}

void CPanel_Toolbar::OnCancel()
{
}

void CPanel_Toolbar::_Update(void)
{
	// Normal View
	switch( g_pOption->m_ViewerMode)
	{
		case VIEWER_MODE_NORMAL :
			m_BTN_ModeNormal.SetState( TRUE);
			m_BTN_ModeMesh.SetState( FALSE);
			m_BTN_ModeColl.SetState( FALSE);
			m_BTN_ModeWire.SetState( FALSE);
			m_BTN_ModeSelOnly.SetState( FALSE);
			break;

		case VIEWER_MODE_MESH :
			m_BTN_ModeNormal.SetState( FALSE);
			m_BTN_ModeMesh.SetState( TRUE);
			m_BTN_ModeColl.SetState( FALSE);
			m_BTN_ModeWire.SetState( FALSE);
			m_BTN_ModeSelOnly.SetState( FALSE);
			break;

		case VIEWER_MODE_SELONLY :
			m_BTN_ModeNormal.SetState( FALSE);
			m_BTN_ModeMesh.SetState( FALSE);
			m_BTN_ModeColl.SetState( FALSE);
			m_BTN_ModeWire.SetState( FALSE);
			m_BTN_ModeSelOnly.SetState( TRUE);
			break;

		case VIEWER_MODE_COLL :
			m_BTN_ModeNormal.SetState( FALSE);
			m_BTN_ModeMesh.SetState( FALSE);
			m_BTN_ModeColl.SetState( TRUE);
			m_BTN_ModeWire.SetState( FALSE);
			m_BTN_ModeSelOnly.SetState( FALSE);
			break;

		case VIEWER_MODE_WIRE :
			m_BTN_ModeNormal.SetState( FALSE);
			m_BTN_ModeMesh.SetState( FALSE);
			m_BTN_ModeColl.SetState( FALSE);
			m_BTN_ModeWire.SetState( TRUE);
			m_BTN_ModeSelOnly.SetState( FALSE);
			break;
	}

	// Grid Show
	m_BTN_Grid.SetState( g_pOption->m_bGridShow);

	// Shadow
	m_BTN_ShadowEnable.SetState( g_pOption->m_bShadowEnable);
}

void	CPanel_Toolbar::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_EDIT)
	{
		if( pInfo->m_pMeta->IsExactTypeOf( COptionInfo::GetClassMeta()))
		{
			_Update();
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_UI)
	{
		_Update();
	}
}

LRESULT CPanel_Toolbar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CPanel_Toolbar::OnBnClickedBtnGrid()
{
	g_pOption->m_bGridShow = !g_pOption->m_bGridShow;

	g_pFramework->SetGridShow( g_pOption->m_bGridShow == TRUE);

	_Update();

	g_pRenderPanel->Invalidate();
}

void CPanel_Toolbar::OnBnClickedBtnShadowenable()
{
	g_pOption->m_bShadowEnable = !g_pOption->m_bShadowEnable;

	g_pFramework->setShadowEnable( g_pOption->m_bShadowEnable == TRUE);

	_Update();

	if( g_pRenderPanel != NULL)
		g_pRenderPanel->Invalidate();
}

void CPanel_Toolbar::OnBnClickedBtnNormalview()
{
	if( g_pFramework != NULL)
	{
		g_pFramework->setViewMode( VIEWER_MODE_NORMAL);

		if( g_pRenderPanel != NULL)
			g_pRenderPanel->Invalidate();
	}
}

void CPanel_Toolbar::OnBnClickedBtnViewonlysel()
{
	if( g_pFramework != NULL)
	{
		g_pFramework->setViewMode( VIEWER_MODE_SELONLY);
		
		if( g_pRenderPanel != NULL)
			g_pRenderPanel->Invalidate();
	}
}


void CPanel_Toolbar::OnBnClickedBtnMeshview()
{
	if( g_pFramework != NULL)
	{
		g_pFramework->setViewMode( VIEWER_MODE_MESH);
		
		if( g_pRenderPanel != NULL)
			g_pRenderPanel->Invalidate();
	}
}

void CPanel_Toolbar::OnBnClickedBtnCollview()
{
	if( g_pFramework != NULL)
	{
		g_pFramework->setViewMode( VIEWER_MODE_COLL);
		
		if( g_pRenderPanel != NULL)
			g_pRenderPanel->Invalidate();
	}
}

void CPanel_Toolbar::OnBnClickedBtnWireview()
{
	if( g_pFramework != NULL)
	{
		g_pFramework->setViewMode( VIEWER_MODE_WIRE);
		
		if( g_pRenderPanel != NULL)
			g_pRenderPanel->Invalidate();
	}
}

void CPanel_Toolbar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( pScrollBar == (CScrollBar *) &m_SL_TimeScale)
	{
		INT32 pos = m_SL_TimeScale.GetPos();

		g_pViewer->SetTimeScale( pos / 20.0f);
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL	CPanel_Toolbar::OnUpdateToolTip( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;

	pTTT->hinst = AfxGetApp()->m_hInstance;
	pTTT->lpszText = (LPSTR) ::GetDlgCtrlID( (HWND) pTTTStruct->idFrom);

	return TRUE;
}

void CPanel_Toolbar::OnBnClickedBtnDynamic()
{
	g_pFramework->SetDynamicState( true);
}

void CPanel_Toolbar::OnBnClickedBtnKinematic()
{
	g_pFramework->SetDynamicState( false);
}

