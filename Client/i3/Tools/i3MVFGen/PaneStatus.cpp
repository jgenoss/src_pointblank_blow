// PaneStatus.cpp : implementation file
//

#include "stdafx.h"
#include "i3MVFGen.h"
#include "PaneStatus.h"
#include ".\panestatus.h"


// CPaneStatus dialog

IMPLEMENT_DYNAMIC(CPaneStatus, CDialog)
CPaneStatus::CPaneStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CPaneStatus::IDD, pParent)
{
}

CPaneStatus::~CPaneStatus()
{
}

void CPaneStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_FILE, m_FileCtrl);
	DDX_Control(pDX, IDC_PROGRESS, m_ProgCtrl);
	DDX_Control(pDX, IDC_LOGLIST, m_LogListCtrl);
}


BEGIN_MESSAGE_MAP(CPaneStatus, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPaneStatus message handlers

BOOL CPaneStatus::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_LogListCtrl.InsertColumn( 0, "Type", LVCFMT_LEFT, 60);
		m_LogListCtrl.InsertColumn( 1, "File", LVCFMT_LEFT, 130);
		m_LogListCtrl.InsertColumn( 2, "Message", LVCFMT_LEFT, 320);
		m_LogListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPaneStatus::ClearLog(void)
{
	m_LogListCtrl.DeleteAllItems();
}

void CPaneStatus::Log( INT32 type, MVFNode * pNode, char * pszMsg)
{
	INT32 idx;

	idx = m_LogListCtrl.InsertItem( m_LogListCtrl.GetItemCount(), "-", 0);

	m_LogListCtrl.SetItemData( idx, (DWORD_PTR) pNode);

	switch( type)
	{
		case 0 :		// Error
			m_LogListCtrl.SetItemText( idx, 0, "Error");
			break;
	}

	if( pNode != NULL)
		m_LogListCtrl.SetItemText( idx, 1, pNode->GetName());
	else
		m_LogListCtrl.SetItemText( idx, 1, "-----------------");

	m_LogListCtrl.SetItemText( idx, 2, pszMsg);

	m_LogListCtrl.EnsureVisible( idx, FALSE);
}

void CPaneStatus::SetProgress( INT32 vmax)
{
	if( vmax == 0)
	{
		m_FileCtrl.EnableWindow( FALSE);
		m_ProgCtrl.EnableWindow( FALSE);
	}
	else
	{
		m_FileCtrl.EnableWindow( TRUE);
		m_ProgCtrl.EnableWindow( TRUE);
	}

	m_ProgCtrl.SetRange32( 0, vmax);
	m_ProgCtrl.SetPos( 0);

	ClearLog();
}

void CPaneStatus::AddProgress( char * pszFile)
{
	m_ProgCtrl.SetPos( m_ProgCtrl.GetPos() + 1);

	m_FileCtrl.SetWindowText( pszFile);
}

void CPaneStatus::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_LogListCtrl.m_hWnd))
	{
		CRect rt;

		m_LogListCtrl.GetWindowRect( &rt);
		ScreenToClient( &rt);

		m_LogListCtrl.SetWindowPos( NULL, 0, 0, cx - rt.left - 3, cy - rt.top - 3, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);

		m_ProgCtrl.GetClientRect( &rt);

		m_ProgCtrl.SetWindowPos( NULL, 0, 0, cx - 42, rt.Height(), SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
	}
}
