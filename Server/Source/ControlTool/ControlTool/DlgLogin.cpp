// DlgLogin.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "ControlTool.h"
#include "DlgLogin.h"


// CDlgLogin 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgLogin, CDialog)

CDlgLogin::CDlgLogin(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLogin::IDD, pParent)
	, m_strID(_T(""))
	, m_strPass(_T(""))
	, m_ui32Port(0)
{
	m_dwIP	= 0;
	m_i32WarningSwitch = 0;
}

CDlgLogin::~CDlgLogin()
{
}

void CDlgLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_ID, m_strID);
	DDX_Text(pDX, IDC_ED_PASS, m_strPass);
	DDX_Control(pDX, IDC_IPADDRESS, m_ctIPAddress);
	DDX_Text(pDX, IDC_ED_PORT, m_ui32Port);
	DDX_Control(pDX, IDC_ED_PASS, m_ctPass);
	DDX_Control(pDX, IDC_CH_CHECK_WARNING, m_ctCheckWarning);
}


BEGIN_MESSAGE_MAP(CDlgLogin, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgLogin::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgLogin 메시지 처리기입니다.
INT_PTR CDlgLogin::DoModalEx( TCHAR* strID, DWORD dwIP, UINT ui32Port )
{
	m_strID		= strID;
	m_dwIP		= dwIP;	
	m_ui32Port	= ui32Port;

	return CDialog::DoModal();
}

void CDlgLogin::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	m_ctIPAddress.GetAddress( m_dwIP );
	// 워닝 체크 버전인지 확인
	if( m_ctCheckWarning.GetCheck() )	m_i32WarningSwitch = 1;	
	OnOK();
}

BOOL CDlgLogin::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_ctIPAddress.SetAddress( m_dwIP );
	UpdateData( FALSE );

	if( FALSE == m_strID.IsEmpty() )
	{
		m_ctPass.SetFocus();
		return FALSE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
