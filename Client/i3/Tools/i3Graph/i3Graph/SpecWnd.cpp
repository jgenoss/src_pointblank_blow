// SpecWnd.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "SpecWnd.h"
#include ".\specwnd.h"


// CSpecWnd dialog

IMPLEMENT_DYNAMIC(CSpecWnd, CDialog)
CSpecWnd::CSpecWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CSpecWnd::IDD, pParent)
{
	m_pDialog = NULL; 
}

CSpecWnd::~CSpecWnd()
{

}

BOOL CSpecWnd::AttachDialog( CBaseSpecDialog * pDialog)
{
	if( ! pDialog->Create( TRUE, this))
	{
		//생성할수 없습니다. 
		return FALSE;
	}

	pDialog->SetParent( this);

	CRect Rect;

	GetClientRect( &Rect);

	pDialog->SetWindowPos( NULL, 0, 0, Rect.Width(), Rect.Height(), SWP_NOZORDER | SWP_NOACTIVATE);

	m_pDialog = pDialog;

	return TRUE;
}


void CSpecWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSpecWnd, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CSpecWnd message handlers

void CSpecWnd::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//OnOK();
}

void CSpecWnd::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	//OnCancel();
}

void CSpecWnd::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( m_pDialog != NULL)
	{
		m_pDialog->MoveWindow( 0, 0, cx, cy);
	}
}
