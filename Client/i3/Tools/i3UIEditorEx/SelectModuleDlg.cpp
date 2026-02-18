// SelectModuleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SelectModuleDlg.h"
#include "i3UIEditor.h"


// CSelectModuleDlg dialog

IMPLEMENT_DYNAMIC(CSelectModuleDlg, CDialog)

CSelectModuleDlg::CSelectModuleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectModuleDlg::IDD, pParent)
{
	m_szModuleName[0] = 0;

	CString ExecModuleName = theApp.GetProfileString(LPCTSTR("Settings"), LPCTSTR("ExecModuleName"));
	if(ExecModuleName.GetLength() != 0)
	{
		i3::string_ncopy_nullpad( m_szModuleName, (LPCTSTR)ExecModuleName, MAX_PATH);
	}

	INT32 runExecModule = theApp.GetProfileInt( LPCTSTR("Settings"), LPCTSTR("runExecModule"), 1);
	m_bRunModule = (runExecModule > 0) ? true : false;
}

CSelectModuleDlg::~CSelectModuleDlg()
{
}

void CSelectModuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_MODULERUN_ENABLE, m_chkRunEnable);
	DDX_Control(pDX, IDC_ED_MODULENAME, m_edModuleName);
}


BEGIN_MESSAGE_MAP(CSelectModuleDlg, CDialog)
	ON_BN_CLICKED(IDC_CHK_MODULERUN_ENABLE, &CSelectModuleDlg::OnBnClickedChkModulerunEnable)
	ON_EN_KILLFOCUS(IDC_ED_MODULENAME, &CSelectModuleDlg::OnEnKillfocusEdModulename)
END_MESSAGE_MAP()


// CSelectModuleDlg message handlers

void CSelectModuleDlg::OnBnClickedChkModulerunEnable()
{
	// TODO: Add your control notification handler code here
	if( m_chkRunEnable.GetCheck() == BST_CHECKED)
	{
		theApp.WriteProfileInt(LPCTSTR("Settings"), LPCTSTR("runExecModule"), 1);
	}
	else
	{
		theApp.WriteProfileInt(LPCTSTR("Settings"), LPCTSTR("runExecModule"), 0);
	}
}

void CSelectModuleDlg::OnEnKillfocusEdModulename()
{
	// TODO: Add your control notification handler code here
	if( m_edModuleName.GetModify())
	{
		char conv[MAX_PATH];
		m_edModuleName.GetWindowText( conv, MAX_PATH-1);

		theApp.WriteProfileString(LPCTSTR("Settings"), LPCTSTR("ExecModuleName"), conv);
	}
}

void CSelectModuleDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnCancel();
}

void CSelectModuleDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	OnEnKillfocusEdModulename();

	CDialog::OnOK();
}

BOOL CSelectModuleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	_UpdateInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectModuleDlg::_UpdateInfo(void)
{
	INT32 checked = (m_bRunModule) ? BST_CHECKED: BST_UNCHECKED;

	m_chkRunEnable.SetCheck( checked);

	m_edModuleName.SetWindowText( (LPCTSTR)m_szModuleName);
}