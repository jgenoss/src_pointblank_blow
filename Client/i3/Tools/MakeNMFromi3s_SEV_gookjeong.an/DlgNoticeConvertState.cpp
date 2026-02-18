#include "stdafx.h"
#include "DlgNoticeConvertState.h"


// CDlgNoticeConvertState dialog

IMPLEMENT_DYNAMIC(CDlgNoticeConvertState, CDialog)

CDlgNoticeConvertState::CDlgNoticeConvertState(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNoticeConvertState::IDD, pParent)
{
	strMsg[0] = '\0';
}

CDlgNoticeConvertState::~CDlgNoticeConvertState()
{
}

void CDlgNoticeConvertState::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MESSAGE, m_stcMessage);
}


BEGIN_MESSAGE_MAP(CDlgNoticeConvertState, CDialog)
END_MESSAGE_MAP()


// CDlgNoticeConvertState message handlers

void CDlgNoticeConvertState::SetMessage( char* msg )
{
	i3String::Concat( strMsg, msg );
	i3String::Concat( strMsg, "\r\n" );
	m_stcMessage.SetWindowText( strMsg );
}
