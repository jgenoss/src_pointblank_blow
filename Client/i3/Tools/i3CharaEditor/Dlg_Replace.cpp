// Dlg_Replace.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "Dlg_Replace.h"


// CDlg_Replace dialog

IMPLEMENT_DYNAMIC(CDlg_Replace, CDialog)

CDlg_Replace::CDlg_Replace(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_Replace::IDD, pParent)
{
	m_szSrc[0] = 0;
	m_szDest[0] = 0;
}

CDlg_Replace::~CDlg_Replace()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Exchanges data to/from the controls in this dialog. </summary>
///
/// <remarks>	¿Ã¡¯±’, 2010-11-30. </remarks>
///
/// <param name="pDX">	[in,out] If non-null, an object that manages the data exchange operation. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

void CDlg_Replace::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_SRC, m_ED_Src);
	DDX_Control(pDX, IDC_ED_DEST, m_ED_Dest);
}

bool CDlg_Replace::Execute( const char * pszSrc, const char * pszDest)
{
	if( pszSrc != NULL)
		i3::safe_string_copy( m_szSrc, pszSrc, 512 );

	if( pszDest != NULL)
		i3::safe_string_copy( m_szDest, pszDest, 512 );

	return DoModal() == IDOK;
}


BEGIN_MESSAGE_MAP(CDlg_Replace, CDialog)
END_MESSAGE_MAP()


// CDlg_Replace message handlers

void CDlg_Replace::OnOK()
{
	m_ED_Src.GetWindowText( m_szSrc, sizeof( m_szSrc));
	m_ED_Dest.GetWindowText( m_szDest, sizeof( m_szDest));

	CDialog::OnOK();
}

BOOL CDlg_Replace::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ED_Src.SetWindowText( m_szSrc);
	m_ED_Dest.SetWindowText( m_szDest);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
