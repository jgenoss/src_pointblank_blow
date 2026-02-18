// GetStringDlg.cpp : implementation file
//

#include "stdafx.h"
#include "i3GuiEditor.h"
#include "GetStringDlg.h"
#include ".\getstringdlg.h"


// CGetStringDlg dialog

IMPLEMENT_DYNAMIC(CGetStringDlg, CDialog)
CGetStringDlg::CGetStringDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGetStringDlg::IDD, pParent)
{
	m_bSearchExactlySame = false;
}

CGetStringDlg::~CGetStringDlg()
{
}

void CGetStringDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_NODENAME, m_edString);
	DDX_Control(pDX, IDC_CHECK_SEARCH_EXACTLYSAME, m_chkSearchExactlySame);
}


BEGIN_MESSAGE_MAP(CGetStringDlg, CDialog)
END_MESSAGE_MAP()


// CGetStringDlg message handlers

void CGetStringDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	m_edString.GetWindowText( m_str, MAX_PATH -1);

	m_bSearchExactlySame = ( m_chkSearchExactlySame.GetCheck() == BST_CHECKED) ? true: false;

	CDialog::OnOK();
}

BOOL CGetStringDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_chkSearchExactlySame.SetCheck( BST_UNCHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
