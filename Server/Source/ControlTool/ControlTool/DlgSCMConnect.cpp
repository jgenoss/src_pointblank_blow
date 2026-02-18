// DlgSCMConnect.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "ControlTool.h"
#include "DlgSCMConnect.h"


// CDlgSCMConnect 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSCMConnect, CDialog)

CDlgSCMConnect::CDlgSCMConnect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSCMConnect::IDD, pParent)
	, m_ui32Port(0)
{

}

CDlgSCMConnect::~CDlgSCMConnect()
{
}

void CDlgSCMConnect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS, m_ctIPAddress);
	DDX_Text(pDX, IDC_ED_PORT, m_ui32Port);
}


BEGIN_MESSAGE_MAP(CDlgSCMConnect, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgSCMConnect::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSCMConnect 메시지 처리기입니다.

BOOL CDlgSCMConnect::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_ctIPAddress.SetAddress( m_dwIP );
	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


INT_PTR	CDlgSCMConnect::DoModalEx(  DWORD dwIp, UINT ui32Port )
{
	m_dwIP		= dwIp;
	m_ui32Port	= ui32Port;

	return CDialog::DoModal();
}


void CDlgSCMConnect::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData( TRUE );

	m_ctIPAddress.GetAddress( m_dwIP );

	OnOK();
}
