#include "stdafx.h"
#include "CalenderDlg.h"

BEGIN_MESSAGE_MAP(CCalenderDlg, CDialog)

END_MESSAGE_MAP()
IMPLEMENT_DYNAMIC(CCalenderDlg, CDialog)

CCalenderDlg::CCalenderDlg(COleDateTime tmBegin, COleDateTime tmEnd, CWnd* pParent /*=NULL*/)
	: CDialog(CCalenderDlg::IDD, pParent)
{
	m_tmBegin = tmBegin;
	m_tmEnd = tmEnd;
}


CCalenderDlg::~CCalenderDlg()
{
}

void CCalenderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_MonthCalCtrl(pDX, IDC_MONTHCALENDAR_BEGIN, m_tmBegin);
	DDX_MonthCalCtrl(pDX, IDC_MONTHCALENDAR_END, m_tmEnd);
}

void CCalenderDlg::OnOK()
{
	CDialog::OnOK();
}