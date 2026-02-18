// UITimeLine.cpp : implementation file
//

#include "stdafx.h"
#include "../i3UIEditor.h"
#include "UIPaneTimeLine.h"


// CUIPaneTimeLine

IMPLEMENT_DYNAMIC(CUIPaneTimeLine, CUIPaneBase)

CUIPaneTimeLine::CUIPaneTimeLine()
{

}

CUIPaneTimeLine::~CUIPaneTimeLine()
{
}


BEGIN_MESSAGE_MAP(CUIPaneTimeLine, CUIPaneBase)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CUIPaneTimeLine message handlers

int CUIPaneTimeLine::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		CRect rc;

		GetClientRect( &rc);

		m_TimeEventCtrl.Create( WS_CHILD | WS_VISIBLE, rc, this, 101);
	}

	return 0;
}

void CUIPaneTimeLine::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDockablePane::OnPaint() for painting messages
	RECT rt;
	::GetClientRect( m_hWnd, &rt);

	::FillRect( dc.m_ps.hdc,&rt, (HBRUSH)COLOR_WINDOW);
}

void CUIPaneTimeLine::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if( ::IsWindow( m_TimeEventCtrl.m_hWnd))
	{
		m_TimeEventCtrl.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}
