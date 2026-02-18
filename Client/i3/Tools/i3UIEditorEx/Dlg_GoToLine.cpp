// Dlg_GoToLine.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "Dlg_GoToLine.h"


// Dlg_GoToLine dialog

IMPLEMENT_DYNAMIC(Dlg_GoToLine, CDialog)

Dlg_GoToLine::Dlg_GoToLine(CWnd* pParent /*=NULL*/)
	: CDialog(Dlg_GoToLine::IDD, pParent)
{
	pszStatic[0] = 0;
	pszEdit[0] = 0;
}

Dlg_GoToLine::~Dlg_GoToLine()
{
}

void Dlg_GoToLine::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ST_GOTOLINE, m_st_GoToLine);
	DDX_Control(pDX, IDC_ED_GOTOLINE, m_ed_GoToLine);
}


BEGIN_MESSAGE_MAP(Dlg_GoToLine, CDialog)
END_MESSAGE_MAP()


// Dlg_GoToLine message handlers


void Dlg_GoToLine::SetStaticText( const char* pszText)
{
	i3::string_ncopy_nullpad( pszStatic, pszText, sizeof( pszStatic));
}

void Dlg_GoToLine::SetEditText( const char* pszText)
{
	i3::string_ncopy_nullpad( pszEdit, pszText, sizeof( pszEdit));
}

const char* Dlg_GoToLine::GetEditText( void)
{
	return (const char*)pszEdit;
}

BOOL Dlg_GoToLine::OnInitDialog()
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

void Dlg_GoToLine::OnOK()
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
