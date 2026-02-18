// OptionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PropertyEditor.h"
#include "OptionDialog.h"

#include "GlobalVariable.h"

// COptionDialog dialog

IMPLEMENT_DYNAMIC(COptionDialog, CDialog)

COptionDialog::COptionDialog(CWnd* pParent /*=NULL*/)
	: CDialog(COptionDialog::IDD, pParent)
{

}

COptionDialog::~COptionDialog()
{
}

void COptionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_EditCtrl_WorkMediaPath);
}


BEGIN_MESSAGE_MAP(COptionDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &COptionDialog::OnBnClickedPath)
	ON_BN_CLICKED(IDOK, &COptionDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// COptionDialog message handlers
BOOL COptionDialog::PreTranslateMessage(MSG* pMsg)   
{   
	if(pMsg->message == WM_KEYDOWN)   
	{   
		if(pMsg->wParam == VK_RETURN)
		{   
			return TRUE;   
		}   
	}   

	return CDialog::PreTranslateMessage(pMsg);   
}   


void COptionDialog::OnBnClickedPath()
{
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd ) == FALSE)
		return ;

	const char* WorkDir = dlg.GetSelectedFolderPath();

	if( _tcscmp(WorkDir, "") != 0 )
	{
		g_pProfile->m_WorkDirectoryPath = WorkDir;
		m_EditCtrl_WorkMediaPath.SetWindowText( WorkDir );
	}
}

BOOL COptionDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_EditCtrl_WorkMediaPath.SetWindowText( g_pProfile->m_WorkDirectoryPath );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COptionDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if( g_pProfile->m_WorkDirectoryPath.IsEmpty() )
		return OnCancel();

	g_pProfile->Save();

	OnOK();
}
