// OptionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3GameEditor.h"
#include "OptionDialog.h"
#include ".\optiondialog.h"


// OptionDialog dialog

IMPLEMENT_DYNAMIC(OptionDialog, CDialog)
OptionDialog::OptionDialog(CWnd* pParent /*=NULL*/)
	: CDialog(OptionDialog::IDD, pParent)
{
}

OptionDialog::~OptionDialog()
{
}

void OptionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OPTION_EDIT_WORK, m_WorkDirEdit);
	DDX_Control(pDX, IDC_OPTION_EDIT_RSC, m_EditRSC);
}


BEGIN_MESSAGE_MAP(OptionDialog, CDialog)
	ON_BN_CLICKED(ID_OPTION_BTN_WORKDIR, OnBnClickedOptionBtnFolder)
	ON_EN_CHANGE(IDC_OPTION_EDIT_WORK, OnEnChangeOptionEditWork)
	ON_BN_CLICKED(ID_OPTION_BTN_RSC, OnBnClickedOptionBtnRsc)
END_MESSAGE_MAP()


// OptionDialog message handlers
char szTemp[256];

BOOL OptionDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	//	Edit 초기화
	m_WorkDirEdit.SetWindowText( g_Profile.GetWorkDir());
	m_EditRSC.SetWindowText( g_Profile.GetRSCFile());

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void OptionDialog::OnBnClickedOptionBtnFolder()
{
	i3TDKFolderSelectDialog FolderDlg;

	//	작업폴더 선택
	if( FolderDlg.Execute( m_hWnd, "Working Folder Selection") == FALSE)	return;

	const char * pszLabel = FolderDlg.GetSelectedFolderPath();
	m_WorkDirEdit.SetWindowText( pszLabel);
}

void OptionDialog::OnBnClickedOptionBtnRsc()
{
	i3TDKFolderSelectDialog FolderDlg;

	//	작업폴더 선택
	char szBuffer[256] = "";
	
	OPENFILENAME ofn;
	memset( &ofn, 0, sizeof( OPENFILENAME));
	ofn.hwndOwner = this->GetSafeHwnd();
	ofn.lStructSize = sizeof( OPENFILENAME);
	ofn.lpstrFile = szBuffer;
	ofn.lpstrInitialDir = NULL;
	ofn.nMaxFile = 256;
	ofn.lpstrTitle = "RSC Genarator File";
	ofn.Flags = OFN_HIDEREADONLY;

	if( GetOpenFileName( &ofn) == 0)	return;

	m_EditRSC.SetWindowText( szBuffer);
}

void OptionDialog::OnOK()
{
	//	Working Directory 설정
	m_WorkDirEdit.GetWindowText( szTemp, sizeof( szTemp) );
	g_Profile.SetWorkDir( szTemp),

	//	RSC Genarator 설정
	m_EditRSC.GetWindowText( szTemp, sizeof( szTemp) );
	g_Profile.SetRSCFile( szTemp),	

	CDialog::OnOK();
}

void OptionDialog::OnEnChangeOptionEditWork()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
}
