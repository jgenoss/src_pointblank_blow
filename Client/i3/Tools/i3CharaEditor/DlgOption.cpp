// DlgOption.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "DlgOption.h"
#include ".\dlgoption.h"

// CDlgOption dialog

IMPLEMENT_DYNAMIC(CDlgOption, CDialog)

CDlgOption::CDlgOption(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOption::IDD, pParent)
{
}

CDlgOption::~CDlgOption()
{
}

void CDlgOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_WorkFolderCtrl);
	DDX_Control(pDX, IDC_ED_CHARAS, m_ED_CharaNames);
}


BEGIN_MESSAGE_MAP(CDlgOption, CDialog)
	ON_BN_CLICKED(IDC_BTN_WORKPATH, OnBnClickedBtnWorkpath)
END_MESSAGE_MAP()


// CDlgOption message handlers

BOOL CDlgOption::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_WorkFolderCtrl.SetWindowText( g_pOption->m_szWorkPath);

	m_ED_CharaNames.SetWindowText( g_pOption->m_szCharaNames);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOption::OnOK()
{
	{
		// Working Folder
		m_WorkFolderCtrl.GetWindowText( g_pOption->m_szWorkPath, sizeof(g_pOption->m_szWorkPath) - 1);

		m_ED_CharaNames.GetWindowText( g_pOption->m_szCharaNames, sizeof( g_pOption->m_szCharaNames));

		g_pOption->Save();
	}

	CDialog::OnOK();
}

void CDlgOption::OnBnClickedBtnWorkpath()
{
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd, "Working Folder"))
	{
		m_WorkFolderCtrl.SetWindowText( dlg.GetSelectedFolderPath());
	}
}
