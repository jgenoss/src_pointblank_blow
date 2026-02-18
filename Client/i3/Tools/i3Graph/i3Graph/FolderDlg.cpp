// FolderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "FolderDlg.h"
#include "i3GraphDlg.h"
#include ".\folderdlg.h"


// CFolderDlg dialog

IMPLEMENT_DYNAMIC(CFolderDlg, CDialog)
CFolderDlg::CFolderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFolderDlg::IDD, pParent)
	, m_Dir(_T(""))
{
}

CFolderDlg::~CFolderDlg()
{
}

void CFolderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FOLDER, m_Dir);
}

BOOL CFolderDlg::OnInitDialog()
{
	SetWindowText( LPCTSTR( m_Title ) );
	return CDialog::OnInitDialog();
}


BEGIN_MESSAGE_MAP(CFolderDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SET_FOLDER, OnBnClickedButtonSetFolder)
	ON_BN_CLICKED(IDC_BTN_AMBIENT, OnBnClickedBtnAmbient)
END_MESSAGE_MAP()


// CFolderDlg message handlers

void CFolderDlg::OnBnClickedButtonSetFolder()
{
	i3TDKFolderSelectDialog dlg;
	if( dlg.Execute(g_pMainWnd->m_hWnd) )
	{
		char* test = dlg.GetSelectedFolderPath();
		m_Dir.SetString( dlg.GetSelectedFolderPath() );
		UpdateData(FALSE);
	}
}

void CFolderDlg::OnBnClickedBtnAmbient()
{
	//i3TDKColorPick Dlg;
}
