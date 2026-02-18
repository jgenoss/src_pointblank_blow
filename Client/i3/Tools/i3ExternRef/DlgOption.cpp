// DlgOption.cpp : implementation file
//

#include "stdafx.h"
#include "i3ExternRef.h"
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
	DDX_Control(pDX, IDC_EDIT2, m_ExpResFolderCtrl);
}


BEGIN_MESSAGE_MAP(CDlgOption, CDialog)
	ON_BN_CLICKED(IDC_BTN_PATH, OnBnClickedBtnPath)
	ON_BN_CLICKED(IDC_BTN_PATH2, OnBnClickedBtnPath2)
END_MESSAGE_MAP()


// CDlgOption message handlers
BOOL CDlgOption::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_WorkFolderCtrl.SetWindowText( g_pOption->m_szWorkPath);
	}

	{
		m_ExpResFolderCtrl.SetWindowText( g_pOption->m_szExportResFolder);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgOption::OnBnClickedBtnPath()
{
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd, "Working Folder"))
	{
		m_WorkFolderCtrl.SetWindowText( dlg.GetSelectedFolderPath());
	}
}

void CDlgOption::OnBnClickedBtnPath2()
{
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd, "Working Folder"))
	{
		m_ExpResFolderCtrl.SetWindowText( dlg.GetSelectedFolderPath());
	}
}

void CDlgOption::OnOK()
{
	{
		m_WorkFolderCtrl.GetWindowText( g_pOption->m_szWorkPath, sizeof( g_pOption->m_szWorkPath) - 1);
		m_ExpResFolderCtrl.GetWindowText( g_pOption->m_szExportResFolder, sizeof( g_pOption->m_szExportResFolder) - 1);
	}

	CDialog::OnOK();
}
