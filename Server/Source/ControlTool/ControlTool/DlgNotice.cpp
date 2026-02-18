// DlgNotice.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "ControlTool.h"
#include "DlgNotice.h"


// CDlgNotice 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgNotice, CDialog)

CDlgNotice::CDlgNotice(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNotice::IDD, pParent)
	, m_strNotice(_T(""))
{

}

CDlgNotice::~CDlgNotice()
{
}

void CDlgNotice::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_NOTICE, m_strNotice);
}


BEGIN_MESSAGE_MAP(CDlgNotice, CDialog)
END_MESSAGE_MAP()


// CDlgNotice 메시지 처리기입니다.
