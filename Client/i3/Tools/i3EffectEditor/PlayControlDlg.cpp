// PlayControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "PlayControlDlg.h"
#include ".\playcontroldlg.h"
#include "i3EffectEditorDlg.h"

// CPlayControlDlg dialog

IMPLEMENT_DYNAMIC(CPlayControlDlg, CDialog)
CPlayControlDlg::CPlayControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPlayControlDlg::IDD, pParent)
{
}

CPlayControlDlg::~CPlayControlDlg()
{
}

void CPlayControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_PLAY, m_BtnPlay);
	DDX_Control(pDX, IDC_BTN_STOP, m_BtnStop);
	DDX_Control(pDX, IDC_BTN_PAUSE, m_BtnPause);
	DDX_Control(pDX, IDC_FPS_PLACEHOLDER, m_FPSPlaceHolder);
}


BEGIN_MESSAGE_MAP(CPlayControlDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_PLAY, OnBnClickedBtnPlay)
	ON_BN_CLICKED(IDC_BTN_STOP, OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTN_PAUSE, OnBnClickedBtnPause)
END_MESSAGE_MAP()


// CPlayControlDlg message handlers

BOOL CPlayControlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		HICON hIcon;

		hIcon = AfxGetApp()->LoadIcon( MAKEINTRESOURCE( IDI_ICON_PLAY));
		I3ASSERT( hIcon != NULL);
		m_BtnPlay.SetIcon( hIcon);

		hIcon = AfxGetApp()->LoadIcon( MAKEINTRESOURCE( IDI_ICON_STOP));
		I3ASSERT( hIcon != NULL);
		m_BtnStop.SetIcon( hIcon);

		hIcon = AfxGetApp()->LoadIcon( MAKEINTRESOURCE( IDI_ICON_PAUSE));
		I3ASSERT( hIcon != NULL);
		m_BtnPause.SetIcon( hIcon);
	}

	{
		CRect rc;

		m_FPSPlaceHolder.GetWindowRect( &rc);

		ScreenToClient( &rc);
		
		m_FPSCtrl.Create( WS_BORDER | WS_VISIBLE, rc, this, IDC_ED_FPS);
		m_FPSCtrl.SetFloatMode( TRUE);
		m_FPSCtrl.SetFont( (HFONT) GetStockObject(DEFAULT_GUI_FONT) );

		{
			char conv[256];

			sprintf( conv, "%d", g_FPS);

			m_FPSCtrl.SetWindowText( conv);
		}

		m_TooltipCtrl.Create(this);
		m_TooltipCtrl.AddTool(GetDlgItem(IDC_BTN_PLAY), "ShortCut : F5");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPlayControlDlg::OnBnClickedBtnPlay()
{
	Ci3EffectEditorDlg * pDlg = (Ci3EffectEditorDlg *) AfxGetMainWnd();

	pDlg->Play();
}


void CPlayControlDlg::OnBnClickedBtnStop()
{
	Ci3EffectEditorDlg * pDlg = (Ci3EffectEditorDlg *) AfxGetMainWnd();

	pDlg->Stop();
}

void CPlayControlDlg::OnBnClickedBtnPause()
{
	Ci3EffectEditorDlg * pDlg = (Ci3EffectEditorDlg *) AfxGetMainWnd();

	pDlg->Pause();
}

BOOL CPlayControlDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR * pHdr = (NMHDR *) lParam;

	switch( pHdr->code)
	{
		case NM_RETURN :	
			{
				char conv[256];

				m_FPSCtrl.GetWindowText( conv, sizeof(conv) - 1);

				g_FPS = atoi( conv);

				//UpdateEditor( UPDATE_CHANGE_FPS, NULL);
			}
			break;
	}

	return CWnd::OnNotify(wParam, lParam, pResult);
}

void CPlayControlDlg::OnOK()
{
}

void CPlayControlDlg::OnCancel()
{
}

BOOL CPlayControlDlg::PreTranslateMessage(MSG* pMsg)
{
	m_TooltipCtrl.RelayEvent(pMsg);

	if(pMsg->message == WM_KEYDOWN)
	{
		if( (pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN))
		{
			DispatchMessage( pMsg);
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
