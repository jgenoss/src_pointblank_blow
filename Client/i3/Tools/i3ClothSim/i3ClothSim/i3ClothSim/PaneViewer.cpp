// PaneViewer.cpp : implementation file
//

#include "stdafx.h"
#include "i3ClothSim.h"
#include "PaneViewer.h"
#include ".\paneviewer.h"
#include "GlobalVariable.h"
#include "ClothFramework.h"

// CPaneViewer dialog

IMPLEMENT_DYNAMIC(CPaneViewer, i3TDKDialogBase)
CPaneViewer::CPaneViewer(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPaneViewer::IDD, pParent)
{
	m_pViewer = NULL;
	m_pFramework = NULL;
}

CPaneViewer::~CPaneViewer()
{
	I3_SAFE_RELEASE( m_pViewer);
	I3_SAFE_RELEASE( m_pFramework);
}

void CPaneViewer::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_PLAY, m_PlayCtrl);
	DDX_Control(pDX, IDC_BTN_STOP, m_StopCtrl);
	DDX_Control(pDX, IDC_VIEWER_PLACEHOLDER, m_ViewerPlaceHolderCtrl);
}

void CPaneViewer::ReplaceControls( INT32 cx, INT32 cy)
{
	INT32 btncx, btncy, y;
	CRect btnRc;

	if( cx == -1)
	{
		CRect rc;

		GetClientRect( &rc);
		cx = rc.Width();
		cy = rc.Height();
	}

	if( ::IsWindow( m_ViewerPlaceHolderCtrl.m_hWnd))
	{
		INT32 viewercx, viewercy;

		// Button의 배치
		{
			m_PlayCtrl.GetWindowRect( &btnRc);

			btncx = btnRc.Width();
			btncy = btnRc.Height();

			y = cy - btncy - 3;
			m_PlayCtrl.SetWindowPos( NULL, 3, y, btncx, btncy, SWP_NOACTIVATE | SWP_NOZORDER);

			m_StopCtrl.SetWindowPos( NULL, btncx + 6, y, btncx, btncy, SWP_NOACTIVATE | SWP_NOZORDER);
		}

		viewercx = cx - 6;
		viewercy = cy - btncy - 9;

		m_ViewerPlaceHolderCtrl.SetWindowPos( NULL, 3, 3, viewercx, viewercy, SWP_NOACTIVATE | SWP_NOZORDER);
	}	
}

BEGIN_MESSAGE_MAP(CPaneViewer, i3TDKDialogBase)
//	ON_WM_CREATE()
	ON_WM_SIZE()
//	ON_WM_PAINT()
//ON_WM_SHOWWINDOW()
ON_BN_CLICKED(IDC_BTN_PLAY, OnBnClickedBtnPlay)
ON_BN_CLICKED(IDC_BTN_STOP, OnBnClickedBtnStop)
END_MESSAGE_MAP()

// CPaneViewer message handlers

BOOL CPaneViewer::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	SetWindowText( "View");

	m_pViewer = i3Viewer::NewObject();
	m_pViewer->Create( m_ViewerPlaceHolderCtrl.m_hWnd, 100, 100, FALSE, 0, 0, 0, 1, I3I_DEVICE_MASK_CONTROLLER);

	m_pFramework = CClothFramework::NewObject();
	m_pFramework->Create( m_pViewer);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CPaneViewer::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	ReplaceControls( cx, cy);
}

void CPaneViewer::OnBnClickedBtnPlay()
{
	// TODO: Add your control notification handler code here

	// 시뮬레이션 실행
	m_pFramework->setSimulation( true );
}

void CPaneViewer::OnBnClickedBtnStop()
{
	// TODO: Add your control notification handler code here
	// 시뮬레이션 중지
	m_pFramework->setSimulation( false );
	m_pFramework->OnChangeScene();
}
