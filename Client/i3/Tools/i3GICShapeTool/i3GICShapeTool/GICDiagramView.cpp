#include "stdafx.h"
#include "GICDiagramView.h"
#include "MainFrm.h"

IMPLEMENT_DYNAMIC( CGICDiagramView, i3GICDiagramView);

CGICDiagramView::CGICDiagramView()
{
	enableGrid();
}

CGICDiagramView::~CGICDiagramView()
{
}

/*virtual*/ void CGICDiagramView::OnSelectShape( i3GICShape * pShape, GIC_MOUSEMODE mode)
{
	CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();

	::SendMessage( pMainWnd->getSpecificView()->m_hWnd, WM_COMMAND, WM_GIC_SELECTSHAPE, 0);
}

/*virtual*/ void CGICDiagramView::OnUnselectShape( void)
{
	CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();

	::SendMessage( pMainWnd->getSpecificView()->m_hWnd, WM_COMMAND, WM_GIC_UNSELECTSHAPE, 0);

}

BEGIN_MESSAGE_MAP(CGICDiagramView, i3GICDiagramView)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CGICDiagramView::OnPaint()
{
	i3GICDiagramView::OnPaint();


}

void CGICDiagramView::OnSize( UINT nType, int cx, int cy)
{
	i3GICDiagramView::OnSize( nType, cx, cy);
}

void CGICDiagramView::OnMouseMove( UINT nFlags, CPoint point)
{
	i3GICDiagramView::OnMouseMove( nFlags, point);

	CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();

	pMainWnd->MouseMove( point);
}


void CGICDiagramView::OnLButtonUp( UINT nFlags, CPoint point)
{
	i3GICDiagramView::OnLButtonUp( nFlags, point);

	CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();
	pMainWnd->LButtonUp( nFlags, point, true);
}