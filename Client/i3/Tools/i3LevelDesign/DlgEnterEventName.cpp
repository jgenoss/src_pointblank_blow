// DlgEnterEventName.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "DlgEnterEventName.h"

// CDlgEnterEventName dialog

IMPLEMENT_DYNAMIC(CDlgEnterEventName, CDialog)
CDlgEnterEventName::CDlgEnterEventName(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEnterEventName::IDD, pParent)
{
	szEventName[0] = 0;
}

CDlgEnterEventName::~CDlgEnterEventName()
{
}

void CDlgEnterEventName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ENTEREVENTNAME, m_editEnterEventName);
}


BEGIN_MESSAGE_MAP(CDlgEnterEventName, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgEnterEventName message handlers

void CDlgEnterEventName::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if( m_editEnterEventName.GetModify())
	{
		m_editEnterEventName.GetWindowText( szEventName, MAX_PATH - 1);
	}

	OnOK();
}
