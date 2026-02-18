#include "stdafx.h"
#include "PanePreView.h"
#include "MainFrm.h"

IMPLEMENT_DYNAMIC(CPanePreView, CDockablePane)

CPanePreView::CPanePreView()
{
	
}

CPanePreView::~CPanePreView()
{
}

BEGIN_MESSAGE_MAP(CPanePreView, CDockablePane)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CUIPaneTimeLine message handlers

void CPanePreView::OnPaint()
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

void CPanePreView::OnLButtonUp( UINT nFlags, CPoint point)
{
	CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();

	pMainWnd->LButtonUp( nFlags, point, false);

	CDockablePane::OnLButtonUp( nFlags, point);
}

