#include "stdafx.h"
#include "PropertyEditor.h"
#include "AckDialog.h"

#include "GlobalVariable.h"


CAckDlg::CAckDlg(i3::string ack) : CDialog(CAckDlg::IDD)
{
	m_sAck = ack;
}

void CAckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CAckDlg::OnInitDialog()
{
	CStatic* pstatic = (CStatic*)GetDlgItem( IDC_ACK_STATIC1 );
	pstatic->SetWindowText(m_sAck.c_str());
	return true;
}

BEGIN_MESSAGE_MAP(CAckDlg, CDialog)
END_MESSAGE_MAP()
