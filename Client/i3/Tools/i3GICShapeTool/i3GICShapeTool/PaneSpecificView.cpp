//

#include "stdafx.h"
#include "PaneSpecificView.h"
#include "MainFrm.h"
#include "CommonDef.h"

// CUIPaneTimeLine

IMPLEMENT_DYNAMIC(CPaneSpecificView, CDockablePane)

CPaneSpecificView::CPaneSpecificView()
{
	
}

CPaneSpecificView::~CPaneSpecificView()
{
}

BEGIN_MESSAGE_MAP(CPaneSpecificView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONUP()
	ON_COMMAND( WM_GIC_SELECTSHAPE, &CPaneSpecificView::OnSelectObject)
	ON_COMMAND( WM_GIC_UNSELECTSHAPE, &CPaneSpecificView::OnUnselectObject)
END_MESSAGE_MAP()


// CUIPaneTimeLine message handlers

int CPaneSpecificView::OnCreate( LPCREATESTRUCT lpCreateStruct)
{
	int rv = CDockablePane::OnCreate( lpCreateStruct);

	CRect rect;
	GetClientRect( &rect);
	m_GridCtrl.Create( WS_CHILD | WS_VISIBLE, rect, this, IDC_LIST_SPECIFIC);

	m_GridCtrl.EnableDescriptionArea();
	m_GridCtrl.SetShowDragContext();

	return rv;
}


void CPaneSpecificView::OnPaint()
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

void CPaneSpecificView::OnSize( UINT nType, int cx, int cy)
{
	CDockablePane::OnSize( nType, cx, cy);

	RECT rect;

	GetClientRect( &rect);

	m_GridCtrl.SetWindowPos( NULL, rect.left, rect.top, cx, cy, 0);
}

void CPaneSpecificView::OnLButtonUp( UINT nFlags, CPoint point)
{
	CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();

	pMainWnd->LButtonUp( nFlags, point, false);

	CDockablePane::OnLButtonUp( nFlags, point);
}

void CPaneSpecificView::OnSelectObject( void)
{
	i3List list;

	CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();

	pMainWnd->getDiagram()->GetSelectedShapes( &list);

	if( list.GetCount() > 0)
	{
		m_GridCtrl.RemoveAll();

		i3GICShape * pShape = (i3GICShape*) list.Items[ 0];

		pShape->OnProperty( &m_GridCtrl);
	}
}

void CPaneSpecificView::OnUnselectObject()
{
	m_GridCtrl.RemoveAll();
	m_GridCtrl.RedrawWindow();
}


