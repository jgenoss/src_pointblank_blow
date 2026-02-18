// Dlg_GoToLine.cpp : implementation file
//

#include "stdafx.h"
#include "GoToLineDlg.h"


// Dlg_GoToLine dialog

IMPLEMENT_DYNAMIC(CGoToLineDlg, CDialog)

CGoToLineDlg::CGoToLineDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGoToLineDlg::IDD, pParent)
{
	pszStatic[0] = 0;
	pszEdit[0] = 0;
}

CGoToLineDlg::~CGoToLineDlg()
{
}

void CGoToLineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_GOTOLINE, m_st_GoToLine);
	DDX_Control(pDX, IDC_EDIT_GOTOLINE, m_ed_GoToLine);
}


BEGIN_MESSAGE_MAP(CGoToLineDlg, CDialog)
END_MESSAGE_MAP()


// Dlg_GoToLine message handlers


void CGoToLineDlg::SetStaticText( const char* pszText)
{
	i3::string_ncopy_nullpad( pszStatic, pszText, sizeof( pszStatic));
}

void CGoToLineDlg::SetEditText( const char* pszText)
{
	i3::string_ncopy_nullpad( pszEdit, pszText, sizeof( pszEdit));
}

const char* CGoToLineDlg::GetEditText( void)
{
	return (const char*)pszEdit;
}

BOOL CGoToLineDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_st_GoToLine.SetWindowTextA( pszStatic);
	m_ed_GoToLine.SetWindowTextA( pszEdit);
	m_ed_GoToLine.SetSel( 0, i3::generic_string_size(pszEdit));

	::SetFocus( m_ed_GoToLine.m_hWnd);

	return FALSE;  // return TRUE unless you set the focus to a control !!!!!!!!!!!!!!!!!!!!!!!!!!!
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CGoToLineDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	if( ::IsWindow(m_ed_GoToLine.m_hWnd))
	{
		char buf[64];
		m_ed_GoToLine.GetWindowTextA(buf, sizeof(buf));

		i3::string_ncopy_nullpad( pszEdit, buf, sizeof( pszEdit));
	}

	CDialog::OnOK();
}
