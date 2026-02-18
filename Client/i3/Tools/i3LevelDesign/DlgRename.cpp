// DlgRename.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "DlgRename.h"


// CDlgRename dialog

IMPLEMENT_DYNAMIC(CDlgRename, CDialog)

CDlgRename::CDlgRename(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRename::IDD, pParent)
{
	m_pszDest = NULL;
	m_Length = 0;
}

CDlgRename::~CDlgRename()
{
}

void CDlgRename::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_NAME, m_ED_Name);
}

bool CDlgRename::Execute( char * pszName, INT32 len)
{
	m_pszDest = pszName;
	m_Length = len;

	INT_PTR rv = DoModal();

	if( rv == IDCANCEL)
		return false;

	return true;
}


BEGIN_MESSAGE_MAP(CDlgRename, CDialog)
END_MESSAGE_MAP()


// CDlgRename message handlers

void CDlgRename::OnOK()
{
	m_ED_Name.GetWindowText( m_pszDest, m_Length);

	CDialog::OnOK();
}
