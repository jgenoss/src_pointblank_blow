// i3UIEditorView_Template.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "i3UIEditorView_Template.h"
#include "GlobalVariables.h"

#include "Control/UIGripCtrlRect.h"
#include "MainFrm.h"
#include "UIUVSelectorDlg.h"
#include "UIGlobalRes.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Ci3UIEditorView_Template

IMPLEMENT_DYNCREATE(Ci3UIEditorView_Template, CView)

Ci3UIEditorView_Template::Ci3UIEditorView_Template()
{
	m_ShapeList_Height = 200;

	m_bResizing = false;
}

Ci3UIEditorView_Template::~Ci3UIEditorView_Template()
{
}

BEGIN_MESSAGE_MAP(Ci3UIEditorView_Template, CView)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_DEBUG_DUMPVIRTUALTEXTURE, &Ci3UIEditorView_Template::OnDebugDumpvirtualtexture)
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


int Ci3UIEditorView_Template::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		HCURSOR hCursor = ::LoadCursor( NULL, IDC_ARROW);

		::SetClassLong( m_hWnd, GCL_HCURSOR, (LONG) hCursor);
	}

	{
		CRect rc;
		INT32 top;

		GetClientRect( rc);

		top = rc.top;

		// Editor View
		rc.top = top;
		rc.bottom = rc.bottom - m_ShapeList_Height;
		m_Editor.Create( WS_VISIBLE | WS_CHILD, rc, this, 102);

		// Shape List
		{
			rc.top = rc.bottom - m_ShapeList_Height + 2;

			m_ShapeListCtrl.Create( "Shapes", WS_VISIBLE | WS_CHILD, rc, this, IDC_SHAPE_LIST);
			m_ShapeListCtrl.SetEditor( &m_Editor);
		}

		PlaceControls();
	}

	return 0;
}

void Ci3UIEditorView_Template::SetCurrentItem( i3UITemplate * pTemplate)
{
	m_Editor.setCurrentTemplate( pTemplate);
	m_ShapeListCtrl.setTemplate( pTemplate);
}

// Ci3UIEditorView_Template diagnostics

#ifdef _DEBUG

Ci3UIEditorDoc* Ci3UIEditorView_Template::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Ci3UIEditorDoc)));
	return (Ci3UIEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// Ci3UIEditorView_Template message handlers

void Ci3UIEditorView_Template::OnDraw( CDC * pDC)
{
	CView::OnDraw( pDC);

	CRect rt;
	HPEN hPen, hOldPen;
	HBRUSH hbrBack = g_pTDKGlobalRes->getBrush( I3TDK_COLOR_FORE_3D_FACE);

	GetClientRect( &rt);

	// Background

	::FillRect( pDC->m_hDC, rt, hbrBack);

	// Highlight line
	hPen = g_pTDKGlobalRes->getPen( I3TDK_COLOR_FORE_3D_LIT);
	hOldPen = (HPEN) ::SelectObject( pDC->m_hDC, hPen);

	::MoveToEx( pDC->m_hDC, rt.right, rt.top, NULL);
	::LineTo( pDC->m_hDC, rt.left, rt.top);
	::LineTo( pDC->m_hDC, rt.left, rt.bottom);

	//
	hPen = g_pTDKGlobalRes->getPen( I3TDK_COLOR_FORE_3D_SHADOW);
	::SelectObject( pDC->m_hDC, hPen);

	::LineTo( pDC->m_hDC, rt.right, rt.bottom);
	::LineTo( pDC->m_hDC, rt.right, rt.top);
}

void Ci3UIEditorView_Template::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	if( bActivate && (pActivateView == this))
	{
		g_hActiveView = m_hWnd;//GetSafeHwnd();

		#ifdef _DEBUG
			GetDocument()->SetActiveScene();
		#endif

		m_Editor.AttachScene();
	}
	else if( !bActivate && (pDeactiveView == this))
	{
		if( isPrevMode())
		{
			setPrevMode( false);
		}

		m_Editor.DetachScene();
	}

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void Ci3UIEditorView_Template::OnLButtonDown(UINT nFlags, CPoint point)
{
	::SetCapture( m_hWnd);

	m_bResizing = true;
	m_StartPt = point;
	m_StartPos = m_ShapeList_Height;

	//CView::OnLButtonDown(nFlags, point);
}

void Ci3UIEditorView_Template::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bResizing = false;

	::ReleaseCapture();

	CView::OnLButtonUp(nFlags, point);
}

void Ci3UIEditorView_Template::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bResizing)
	{
		m_ShapeList_Height = m_StartPos + (m_StartPt.y - point.y);

		PlaceControls();
	}

	CView::OnMouseMove(nFlags, point);
}


BOOL Ci3UIEditorView_Template::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}

void Ci3UIEditorView_Template::PlaceControls( INT32 cx, INT32 cy)
{
	if( cx < 0)
	{
		CRect rt;

		GetClientRect( &rt);

		cx = rt.Width();
		cy = rt.Height();
	}

	m_ShapeListCtrl.SetWindowPos( NULL, 0, cy - m_ShapeList_Height + 2, cx, m_ShapeList_Height - 2, SWP_NOACTIVATE | SWP_NOZORDER);
	m_Editor.SetWindowPos( NULL, 0 , 0, cx, cy - m_ShapeList_Height, SWP_NOACTIVATE | SWP_NOZORDER);
}


void Ci3UIEditorView_Template::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if( ::IsWindow( m_Editor.m_hWnd))
	{
		PlaceControls( cx, cy);
	}
}

void Ci3UIEditorView_Template::OnDebugDumpvirtualtexture()
{
	m_Editor.DumpVirtualTexture();
}

BOOL Ci3UIEditorView_Template::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	return CView::OnNotify(wParam, lParam, pResult);
}

void	Ci3UIEditorView_Template::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
}

LRESULT Ci3UIEditorView_Template::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CView::WindowProc(message, wParam, lParam);
}


INT32 Ci3UIEditorView_Template::getSelectedObjCount( void)
{
	return m_Editor.getSelectedObjCount();
}

void Ci3UIEditorView_Template::getSelectedObjList( i3::vector<i3UIControl*>& Out)
{
	m_Editor.getSelectedObjList( Out);
}

void Ci3UIEditorView_Template::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);
}

BOOL Ci3UIEditorView_Template::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	//WM_MOUSEWHEEL 이벤트가 m_Editor로 전달되지 않으므로 여기서 직접 호출.
	m_Editor.OnMouseWheel( nFlags, zDelta, pt);

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void Ci3UIEditorView_Template::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	//WM_KEYDOWN 이벤트가 m_Editor로 전달되지 않으므로 여기서 직접 호출.
	m_Editor.OnKeyDown( nChar, nRepCnt, nFlags);

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void Ci3UIEditorView_Template::UpdateShapeListCtrl()
{
	m_ShapeListCtrl.Invalidate();
}

