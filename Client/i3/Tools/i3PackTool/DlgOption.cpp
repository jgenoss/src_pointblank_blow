// DlgOption.cpp : implementation file
//

#include "stdafx.h"
#include "i3PackTool.h"
#include "DlgOption.h"
#include "GlobalVar.h"
#include ".\dlgoption.h"
#include "i3Base/i3StringListBuffer.h"

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
	DDX_Control(pDX, IDC_ED_WORKDIR, m_ED_WorkDir);
	DDX_Control(pDX, IDC_ED_EXCEPT, m_ED_Except);
}


BEGIN_MESSAGE_MAP(CDlgOption, CDialog)
	ON_BN_CLICKED(IDC_BTN_WORKDIR, OnBnClickedBtnWorkdir)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgOption message handlers

BOOL CDlgOption::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_ED_WorkDir.SetWindowText( g_pOption->m_szWorkPath);

		char temp[ 4096] = {};
	//	g_pOption->m_ExceptList.CopyToBuffer( temp, sizeof(temp));
		CopyFromStringListToBuffer(g_pOption->m_ExceptList, temp, sizeof(temp));
		m_ED_Except.SetWindowText( temp);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOption::OnBnClickedBtnWorkdir()
{
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd, "Working Directory", BIF_RETURNONLYFSDIRS, NULL, NULL, g_pOption->m_szWorkPath) == TRUE)
	{
		m_ED_WorkDir.SetWindowText( dlg.GetSelectedFolderPath());
	}
}

void CDlgOption::OnBnClickedOk()
{
	{
		m_ED_WorkDir.GetWindowText( g_pOption->m_szWorkPath, sizeof(g_pOption->m_szWorkPath) - 1);

		i3ResourceFile::SetWorkingFolder( g_pOption->m_szWorkPath);

		SetCurrentDirectory( g_pOption->m_szWorkPath);
	}

	{
		char temp[ 4096];
		m_ED_Except.GetWindowText( temp, sizeof(temp));

	//	g_pOption->m_ExceptList.CopyFromBuffer( temp);
		CopyFromBufferToStringList(g_pOption->m_ExceptList, temp);	
	}

	OnOK();
}
