// UITimeLine.cpp : implementation file
//

#include "stdafx.h"
#include "PaneLogView.h"
#include "MainFrm.h"


// CUIPaneTimeLine

IMPLEMENT_DYNAMIC(CPaneLogView, CDockablePane)

CPaneLogView::CPaneLogView()
{
	
}

CPaneLogView::~CPaneLogView()
{
}

BEGIN_MESSAGE_MAP(CPaneLogView, CDockablePane)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CUIPaneTimeLine message handlers


void CPaneLogView::OnPaint()
{	
	// TODO: Add your message handler code here
	// Do not call CDockablePane::OnPaint() for painting messages

	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDockablePane::OnPaint() for painting messages
	RECT rt;
	::GetClientRect( m_hWnd, &rt);
	
	::FillRect( dc.m_ps.hdc,&rt, (HBRUSH)COLOR_WINDOW);
}

void CPaneLogView::OnLButtonUp( UINT nFlags, CPoint point)
{
	CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();

	pMainWnd->LButtonUp( nFlags, point, false);

	CDockablePane::OnLButtonUp( nFlags, point);
}

