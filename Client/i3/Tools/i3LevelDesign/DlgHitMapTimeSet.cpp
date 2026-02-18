#include "pch.h"
#include "resource.h"
#include "DlgHitMapTimeSet.h"

IMPLEMENT_DYNAMIC(CDlgHitMapTimeSet, CDialog)
BEGIN_MESSAGE_MAP(CDlgHitMapTimeSet, CDialog)

	ON_NOTIFY(MCN_SELCHANGE, IDC_MONTHCALENDAR_START, &CDlgHitMapTimeSet::OnMcnSelchangeMonthcalendarStart)
	ON_NOTIFY(MCN_SELCHANGE, IDC_MONTHCALENDAR_END, &CDlgHitMapTimeSet::OnMcnSelchangeMonthcalendarEnd)
END_MESSAGE_MAP()
CDlgHitMapTimeSet::CDlgHitMapTimeSet(CWnd* pParent) : CDialog(CDlgHitMapTimeSet::IDD, pParent)
{
	m_tmStart = COleDateTime::GetCurrentTime();
	m_tmEnd = COleDateTime::GetCurrentTime();
	m_bState = false;
}


CDlgHitMapTimeSet::~CDlgHitMapTimeSet()
{
}

void CDlgHitMapTimeSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_MonthCalCtrl(pDX, IDC_MONTHCALENDAR_START, m_tmStart);
	DDX_MonthCalCtrl(pDX, IDC_MONTHCALENDAR_END, m_tmEnd);
}

// void CDlgHitMapTimeSet::Init()
// {
// 
// 
// 	Build();
// }

// bool CDlgHitMapTimeSet::Build()
// {
// 	if (m_pRoomList)
// 	{
// 		delete[] m_pRoomList;
// 		m_pRoomList = NULL;
// 	}
// 
// 
// 	m_tmStart.SetDateTime(2016, 2, 1, 1, 1, 1);
// 
// 	m_pMapInfo->GetRoomList(&m_tmStart, &m_tmEnd, &m_i32RoomListCount, &m_pRoomList);
// 
// 	return true;
// }

BOOL CDlgHitMapTimeSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;
}
void CDlgHitMapTimeSet::OnOK()
{
	m_bState = true;
	CDialog::OnOK();
}

void CDlgHitMapTimeSet::OnMcnSelchangeMonthcalendarStart(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMSELCHANGE pSelChange = reinterpret_cast<LPNMSELCHANGE>(pNMHDR);

	m_tmStart = pSelChange->stSelStart;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CDlgHitMapTimeSet::OnMcnSelchangeMonthcalendarEnd(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMSELCHANGE pSelChange = reinterpret_cast<LPNMSELCHANGE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_tmEnd = pSelChange->stSelStart;
	*pResult = 0;
}
