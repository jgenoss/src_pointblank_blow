// ErrorDlg.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "ControlTool.h"
#include "ErrorDlg.h"


// CErrorDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CErrorDlg, CDialog)

CErrorDlg::CErrorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CErrorDlg::IDD, pParent)
{

}

CErrorDlg::~CErrorDlg()
{
}

void CErrorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, m_editServerIdx);
	DDX_Control(pDX, IDC_EDIT3, m_editErrorNum);
}


BEGIN_MESSAGE_MAP(CErrorDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CErrorDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CErrorDlg 메시지 처리기입니다.

void CErrorDlg::OnBnClickedOk()
{
	//ShowWindow(SW_HIDE);

	//OnOK();
}
