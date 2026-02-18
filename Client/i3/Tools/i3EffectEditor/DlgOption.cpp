// DlgOption.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "DlgOption.h"
#include "GlobalVariable.h"


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
	DDX_Control(pDX, IDC_ED_WORKFOLDER, m_WorkFolderCtrl);
}


BEGIN_MESSAGE_MAP(CDlgOption, CDialog)
	ON_BN_CLICKED(IDC_BTN_WORKFOLDER, OnBnClickedBtnWorkfolder)
END_MESSAGE_MAP()


// CDlgOption message handlers

BOOL CDlgOption::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_WorkFolderCtrl.SetWindowText( g_pOption->m_szWorkPath);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOption::OnOK()
{
	m_WorkFolderCtrl.GetWindowText( g_pOption->m_szWorkPath, sizeof(g_pOption->m_szWorkPath) - 1);
	i3ResourceFile::SetWorkingFolder( g_pOption->m_szWorkPath);
	i3ResourceFile::setSearchExternRefEnable( true);

	g_pOption->Save();

	CDialog::OnOK();
}

void CDlgOption::OnBnClickedBtnWorkfolder()
{
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd, "Working Folder"))
	{
		m_WorkFolderCtrl.SetWindowText( dlg.GetSelectedFolderPath());
	}
}
