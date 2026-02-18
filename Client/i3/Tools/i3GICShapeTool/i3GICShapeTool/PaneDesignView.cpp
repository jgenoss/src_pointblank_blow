// UITimeLine.cpp : implementation file
//

#include "stdafx.h"
#include "PaneDesignView.h"
#include "GlobalVariables.h"
#include "CommonDef.h"
#include "MainFrm.h"

// CUIPaneTimeLine

IMPLEMENT_DYNAMIC(CPaneDesignView, CDockablePane)

CPaneDesignView::CPaneDesignView()
{
	m_pDiagramView = NULL;
}

CPaneDesignView::~CPaneDesignView()
{
	if( m_pDiagramView)
	{
		delete m_pDiagramView;
	}
}


BEGIN_MESSAGE_MAP(CPaneDesignView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_CONTEXTMENU()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CUIPaneTimeLine message handlers
int CPaneDesignView::OnCreate( LPCREATESTRUCT lpCreateStruct)
{
	int rv = CDockablePane::OnCreate( lpCreateStruct);

	CRect rect;
	GetClientRect( &rect);

	m_pDiagramView = new CGICDiagramView;
	if( m_pDiagramView->Create( I3STR(""), WS_CHILD | WS_VISIBLE, rect, this, IDD_DLG_DIAGRAMVIEW) == FALSE)
	{
		I3FATAL( I3STR("can't create Diagram view window."));
	}
	
	m_pDiagramView->setSnapEps( 10);

	return rv;
}

void CPaneDesignView::OnSize( UINT nType, int cx, int cy)
{
	CDockablePane::OnSize( nType, cx, cy);

	m_pDiagramView->SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
}


void CPaneDesignView::OnMouseMove( UINT nFlags, CPoint point)
{
	CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();

	pMainWnd->MouseMove( point);
	
	CDockablePane::OnMouseMove( nFlags, point);
}

void CPaneDesignView::OnLButtonUp( UINT nFlags, CPoint point)
{
	CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();

	pMainWnd->LButtonUp( nFlags, point, true);

	SetFocus();

	CDockablePane::OnLButtonUp( nFlags, point);
}

void CPaneDesignView::OnRButtonUp( UINT nFlags, CPoint point)
{
	CDockablePane::OnRButtonUp( nFlags, point);

	CMenu menu, * pPopup;

	menu.LoadMenu( IDR_POPUP_EDIT);

	pPopup = menu.GetSubMenu( 0);

	int cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
			point.x, point.y, (CWnd *) this, NULL);

	switch( cmd)
	{
		case ID_EDIT_CUT:			break;
		case ID_EDIT_COPY:			break;
		case ID_EDIT_PASTE:			break;
		case ID_EDIT_DELETE:		break;

		default:
			break;
	}
}

void CPaneDesignView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	if (CMFCPopupMenu::GetActiveMenu() != NULL)
	{
		return;
	}

	// default Popup 창을 사용하지 않도록 합니다.
}


void CPaneDesignView::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDockablePane::OnKeyDown( nChar, nRepCnt, nFlags);

	I3TRACE( "--- KeyDown : %d\n", nChar);
}