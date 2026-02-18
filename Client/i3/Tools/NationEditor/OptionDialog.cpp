// OptionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "NationEditor.h"
#include "OptionDialog.h"

#include "FileUtil.h"

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
	CStreamDialog dlg;

	g_pProfile->m_WorkDirectoryPath = dlg.OpenFolder( this->m_hWnd );

	m_EditCtrl_WorkMediaPath.SetWindowText( 
		g_pProfile->m_WorkDirectoryPath );
}

//CString COptionDialog::GetPath() const
//{
//	CString Ret;
//
//	m_EditCtrl_WorkMediaPath.GetWindowText( Ret );
//
//	return Ret;
//}
BOOL COptionDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_EditCtrl_WorkMediaPath.SetWindowText( 
		g_pProfile->m_WorkDirectoryPath );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COptionDialog::OnBnClickedOk()
{
	if( g_pProfile->m_WorkDirectoryPath.IsEmpty() )
		return OnCancel();

	OnOK();
}
