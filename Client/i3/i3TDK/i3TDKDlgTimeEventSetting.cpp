// i3TDKDlgTimeEventSetting.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKDlgTimeEventSetting.h"
#include ".\i3tdkdlgtimeeventsetting.h"


// i3TDKDlgTimeEventSetting dialog

IMPLEMENT_DYNAMIC(i3TDKDlgTimeEventSetting, CDialog)

void i3TDKDlgTimeEventSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_CHK_REPEAT, m_CHK_Repeat);
}

bool i3TDKDlgTimeEventSetting::Execute( i3TimeEventGen * pTimeEvt)
{
	m_pTimeEvent = pTimeEvt;

	i3TDK::SetResInstance();

	INT32 rv = DoModal();

	i3TDK::RestoreResInstance();

	if( rv == IDCANCEL)
		return false;

	return true;
}

BEGIN_MESSAGE_MAP(i3TDKDlgTimeEventSetting, CDialog)
END_MESSAGE_MAP()


// i3TDKDlgTimeEventSetting message handlers
BOOL i3TDKDlgTimeEventSetting::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		I3ASSERT( m_pTimeEvent != nullptr);

		if( m_pTimeEvent->isStyle( I3_TIMEEVENT_REPEAT))
			m_CHK_Repeat.SetCheck( BST_CHECKED);
		else
			m_CHK_Repeat.SetCheck( BST_UNCHECKED);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void i3TDKDlgTimeEventSetting::OnOK()
{
	if( m_CHK_Repeat.GetCheck() == BST_CHECKED)
		m_pTimeEvent->setStyle( m_pTimeEvent->getStyle() | I3_TIMEEVENT_REPEAT);
	else
		m_pTimeEvent->setStyle( m_pTimeEvent->getStyle() & ~I3_TIMEEVENT_REPEAT);

	CDialog::OnOK();
}

