// DlgAuth.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "ControlTool.h"
#include "DlgAuth.h"


// CDlgAuth 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgAuth, CDialog)

CDlgAuth::CDlgAuth(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAuth::IDD, pParent)
	, m_strID(_T(""))
	, m_strPass(_T(""))
{

}

CDlgAuth::~CDlgAuth()
{
}

void CDlgAuth::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_ID, m_strID);
	DDX_Text(pDX, IDC_ED_PASS, m_strPass);
}


BEGIN_MESSAGE_MAP(CDlgAuth, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgAuth::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgAuth 메시지 처리기입니다.

void CDlgAuth::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );
	OnOK();
}
