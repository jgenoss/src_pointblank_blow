// DlgDetectOption.cpp : implementation file
//

#include "stdafx.h"
#include "i3OptManager.h"
#include "DlgDetectOption.h"
#include "Warn.h"


// CDlgDetectOption dialog

IMPLEMENT_DYNAMIC(CDlgDetectOption, CDialog)
CDlgDetectOption::CDlgDetectOption(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDetectOption::IDD, pParent)
{
	m_pWarnList = NULL;
}

CDlgDetectOption::~CDlgDetectOption()
{
}

void CDlgDetectOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WARN_LIST, m_LST_Warn);
}

bool CDlgDetectOption::Execute( i3List * pWarnList)
{
	m_pWarnList = pWarnList;

	if( DoModal() == IDCANCEL)
		return false;

	return true;
}


BEGIN_MESSAGE_MAP(CDlgDetectOption, CDialog)
END_MESSAGE_MAP()


// CDlgDetectOption message handlers

BOOL CDlgDetectOption::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_LST_Warn.SetExtendedStyle( LVS_EX_CHECKBOXES  | LVS_EX_FULLROWSELECT);

		m_LST_Warn.InsertColumn( 0, "Name", LVCFMT_LEFT, 200);
	}

	{
		INT32 i, idx;
		Warn * pWarn;

		for( i = 0; i < m_pWarnList->GetCount(); i++)
		{
			pWarn = (Warn *) m_pWarnList->Items[i];

			idx = m_LST_Warn.InsertItem( i, pWarn->GetTitle(), 0);

			m_LST_Warn.SetItemData( idx, (DWORD_PTR) pWarn);
			m_LST_Warn.SetCheck( idx, pWarn->isEnable());
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDetectOption::OnOK()
{
	INT32 i;
	Warn * pWarn;

	for( i = 0; i < m_LST_Warn.GetItemCount(); i++)
	{
		pWarn = (Warn *) m_LST_Warn.GetItemData( i);

		pWarn->setEnable( m_LST_Warn.GetCheck( i) == TRUE);
	}

	CDialog::OnOK();
}
