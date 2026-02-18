// PaneLog.cpp : implementation file
//

#include "stdafx.h"
#include "i3PackFileEditor.h"
#include "PaneLog.h"
#include ".\panelog.h"


// CPaneLog dialog

IMPLEMENT_DYNAMIC(CPaneLog, CDialog)
CPaneLog::CPaneLog(CWnd* pParent /*=NULL*/)
	: CDialog(CPaneLog::IDD, pParent)
{
}

CPaneLog::~CPaneLog()
{
}

void CPaneLog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGLIST, m_LogListCtrl);
}

void CPaneLog::Log( const char * pszFile, const char * pszMessage)
{
	INT32 idx;

	idx = m_LogListCtrl.InsertItem( m_LogListCtrl.GetItemCount(), pszFile, 0);

	m_LogListCtrl.SetItemText( idx, 1, pszMessage);

	m_LogListCtrl.EnsureVisible( idx, FALSE);
}

void CPaneLog::ClearLog(void)
{
	m_LogListCtrl.DeleteAllItems();
}

BEGIN_MESSAGE_MAP(CPaneLog, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPaneLog message handlers

BOOL CPaneLog::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_LogListCtrl.InsertColumn( 0, "File", LVCFMT_LEFT, 160);
		m_LogListCtrl.InsertColumn( 1, "Message", LVCFMT_LEFT, 400);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CPaneLog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_LogListCtrl.m_hWnd))
	{
		m_LogListCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

