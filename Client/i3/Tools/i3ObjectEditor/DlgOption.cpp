// DlgOption.cpp : implementation file
//

#include "stdafx.h"
#include "i3ObjectEditor.h"
#include "DlgOption.h"
#include "GlobalVar.h"
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
	DDX_Control(pDX, IDC_ED_WORKDIR, m_WorkDirCtrl);
}


BEGIN_MESSAGE_MAP(CDlgOption, CDialog)
	ON_BN_CLICKED(IDC_BTN_WORKDIR, OnBnClickedBtnWorkdir)
END_MESSAGE_MAP()


// CDlgOption message handlers

BOOL CDlgOption::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_WorkDirCtrl.SetWindowText( g_pOption->m_szWorkPath);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgOption::OnBnClickedBtnWorkdir()
{
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd, "Working Folder", BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_STATUSTEXT | BIF_USENEWUI, NULL, NULL, g_pOption->m_szWorkPath) == FALSE)
		return;

	m_WorkDirCtrl.SetWindowText( dlg.GetSelectedFolderPath());
}

void CDlgOption::OnOK()
{
	char conv[MAX_PATH];

	m_WorkDirCtrl.GetWindowText( conv, sizeof(conv) - 1);

	strcpy( g_pOption->m_szWorkPath, conv);
	i3ResourceFile::SetWorkingFolder( g_pOption->m_szWorkPath);
	i3ResourceFile::setSearchExternRefEnable( true);

	CDialog::OnOK();
}
