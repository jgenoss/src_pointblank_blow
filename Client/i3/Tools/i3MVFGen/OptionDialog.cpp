// OptionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3MVFGen.h"
#include "OptionDialog.h"
#include ".\optiondialog.h"
#include "GlobVar.h"

// COptionDialog dialog

IMPLEMENT_DYNAMIC(COptionDialog, CDialog)
COptionDialog::COptionDialog(CWnd* pParent /*=NULL*/)
	: CDialog(COptionDialog::IDD, pParent)
	, m_nDefaultAlign(1)
{
}

COptionDialog::~COptionDialog()
{
}

void COptionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_WORKDIR, m_WorkDirCtrl);
	DDX_Control(pDX, IDC_EDIT2, m_ExceptRullCtrl);
	DDX_Text(pDX, IDC_EDIT1, m_nDefaultAlign);
}


BEGIN_MESSAGE_MAP(COptionDialog, CDialog)
	ON_BN_CLICKED(IDC_BTN_WORKDIR, OnBnClickedBtnWorkdir)
END_MESSAGE_MAP()


// COptionDialog message handlers

BOOL COptionDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_WorkDirCtrl.SetWindowText( g_pProfile->m_szWorkDir);

		m_ExceptRullCtrl.SetWindowText( g_pProfile->m_szExceptRull);

		m_nDefaultAlign = g_pProfile->m_nAlign;
	}

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COptionDialog::OnOK()
{
	UpdateData( TRUE );

	{
		m_WorkDirCtrl.GetWindowText( g_pProfile->m_szWorkDir, sizeof(g_pProfile->m_szWorkDir) - 1);

		m_ExceptRullCtrl.GetWindowText( g_pProfile->m_szExceptRull, sizeof( g_pProfile->m_szExceptRull) - 1);

		g_pProfile->m_nAlign = m_nDefaultAlign;
	}

	CDialog::OnOK();
}

void COptionDialog::OnBnClickedBtnWorkdir()
{
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd) == FALSE)
		return;

	m_WorkDirCtrl.SetWindowText( dlg.GetSelectedFolderPath());
}
