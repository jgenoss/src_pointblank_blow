// OptionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "i3RSCGen.h"
#include "OptionDlg.h"
#include "GlobalVar.h"

// OptionDlg dialog

IMPLEMENT_DYNAMIC(OptionDlg, CDialog)
OptionDlg::OptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(OptionDlg::IDD, pParent)
{
}

OptionDlg::~OptionDlg()
{
}

void OptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_WORKDIR, m_ctrlWorkDir);
	DDX_Control(pDX, IDC_EDIT2, m_CtrlExceptRule);
}


BEGIN_MESSAGE_MAP(OptionDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_SELDIR, OnBnClickedBtnSeldir)
END_MESSAGE_MAP()


// OptionDlg message handlers

BOOL OptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	{
		m_ctrlWorkDir.SetWindowText( g_pProfile->m_strWorkDir.c_str());

		m_CtrlExceptRule.SetWindowText( g_pProfile->m_szExceptRull);
	}

	UpdateData( FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void OptionDlg::OnBnClickedBtnSeldir()
{
	// TODO: Add your control notification handler code here
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd) == FALSE)
		return ;

	m_ctrlWorkDir.SetWindowText( dlg.GetSelectedFolderPath());
}

void OptionDlg::OnOK()
{
	UpdateData( TRUE);

	// TODO: Add your specialized code here and/or call the base class
	{
		CString strVal;

		m_ctrlWorkDir.GetWindowText(strVal);
		
		g_pProfile->m_strWorkDir.assign(LPCSTR(strVal), strVal.GetLength());

		m_CtrlExceptRule.GetWindowText( g_pProfile->m_szExceptRull, sizeof( g_pProfile->m_szExceptRull) - 1);
	}

	CDialog::OnOK();
}
