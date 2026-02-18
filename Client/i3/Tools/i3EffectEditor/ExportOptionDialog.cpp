// ExportOptionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "ExportOptionDialog.h"
#include ".\exportoptiondialog.h"


// CExportOptionDialog dialog

IMPLEMENT_DYNAMIC(CExportOptionDialog, CDialog)
CExportOptionDialog::CExportOptionDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CExportOptionDialog::IDD, pParent)
{
}

CExportOptionDialog::~CExportOptionDialog()
{
}

void CExportOptionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_EXPORT_TEX, m_ExportTextureCtrl);
	DDX_Control(pDX, IDC_ED_PATH, m_PathCtrl);
}

BOOL CExportOptionDialog::Execute(void)
{
	if( DoModal() == IDCANCEL)
		return FALSE;

	return TRUE;
}

BEGIN_MESSAGE_MAP(CExportOptionDialog, CDialog)
END_MESSAGE_MAP()


// CExportOptionDialog message handlers

void CExportOptionDialog::OnOK()
{
	if( m_ExportTextureCtrl.GetCheck() == BST_CHECKED)
		m_bExportTex = TRUE;
	else
		m_bExportTex = FALSE;

	m_PathCtrl.GetWindowText( m_szPath, sizeof(m_szPath) - 1);

	CDialog::OnOK();
}
