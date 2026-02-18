// DlgMd5Key.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "ControlTool.h"
#include "DlgMd5Key.h"


// CDlgMd5Key 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgMd5Key, CDialog)

CDlgMd5Key::CDlgMd5Key(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMd5Key::IDD, pParent)
	, m_strMd5Key00(_T(""))
	, m_strMd5Key01(_T(""))
{

}

CDlgMd5Key::~CDlgMd5Key()
{
}

void CDlgMd5Key::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_MD5_00, m_strMd5Key00);
	DDX_Text(pDX, IDC_ED_MD5_01, m_strMd5Key01);
	DDX_Control(pDX, IDC_ED_MD5_00, m_ctMd5Key00);
	DDX_Control(pDX, IDC_ED_MD5_01, m_ctMd5Key01);
}


BEGIN_MESSAGE_MAP(CDlgMd5Key, CDialog)
END_MESSAGE_MAP()


// CDlgMd5Key 메시지 처리기입니다.

INT_PTR	CDlgMd5Key::DoModalEx( TCHAR strMD5[ 2 ][ NET_MD5_KEY_SIZE+1 ] )
{
	m_strMd5Key00 = strMD5[0];
	m_strMd5Key01 = strMD5[1];

	return DoModal();
}

BOOL CDlgMd5Key::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_ctMd5Key00.SetLimitText( 32 );
	m_ctMd5Key01.SetLimitText( 32 );

	//UpdateData( TRUE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
