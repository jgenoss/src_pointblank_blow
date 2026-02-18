#include "stdafx.h"
#include "TimeLinePane.h"
IMPLEMENT_DYNAMIC(CTimeLinePane, CPaneBase)

BEGIN_MESSAGE_MAP(CTimeLinePane, CPaneBase)
	ON_WM_CREATE()
	ON_WM_SIZE()

END_MESSAGE_MAP()

#define ID_WND_TIMELINE		1

CTimeLinePane::CTimeLinePane()
{
}


CTimeLinePane::~CTimeLinePane()
{
}

int CTimeLinePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPaneBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rc;

	GetClientRect(&rc);

	m_TimeCtrl.Create(WS_CHILD | WS_VISIBLE, rc, this, ID_WND_TIMELINE);

	AdjustLayout();

	return 0;
}


void CTimeLinePane::OnSize(UINT nType, int cx, int cy)
{
	CPaneBase::OnSize(nType, cx, cy);

	AdjustLayout();
}

void CTimeLinePane::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	if (::IsWindow(m_TimeCtrl.m_hWnd))
	{
		m_TimeCtrl.SetWindowPos(NULL, 0, 0, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CTimeLinePane::AddSequence(i3UIAnimation2DSprite* pAnimation)
{
	m_TimeCtrl.SetTimeSequence(pAnimation);
}
