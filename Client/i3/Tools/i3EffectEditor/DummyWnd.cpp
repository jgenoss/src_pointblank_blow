// DummyWnd.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "DummyWnd.h"
#include ".\dummywnd.h"
#include "GlobalVariable.h"
#include "i3EffectEditorDlg.h"

// CDummyWnd

IMPLEMENT_DYNAMIC(CDummyWnd, CWnd)
CDummyWnd::CDummyWnd()
{
	m_pViewer = NULL;
}

CDummyWnd::~CDummyWnd() 
{
}


BEGIN_MESSAGE_MAP(CDummyWnd, CWnd)
	//ON_WM_ERASEBKGND()
	//ON_WM_SIZE()
	ON_WM_PAINT()
	//ON_WM_LBUTTONDOWN()
	//ON_WM_LBUTTONUP()
	//ON_WM_MBUTTONDOWN()
	//ON_WM_MBUTTONUP()
	//ON_WM_RBUTTONDOWN()
	//ON_WM_RBUTTONUP()
	//ON_WM_MOUSEMOVE()
	//ON_WM_MOUSEWHEEL()
	//ON_WM_KEYDOWN()
	//ON_WM_KEYUP()
END_MESSAGE_MAP()

BOOL CDummyWnd::Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID)
{
	LPCTSTR lpszClass;
	HCURSOR hCursor;
	HBRUSH hbrBack;

	hCursor = ::LoadCursor( NULL, IDC_ARROW);

	hbrBack = (HBRUSH) GetSysColorBrush( COLOR_3DFACE);

	lpszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, hCursor, hbrBack, NULL);
	ASSERT( lpszClass);

	return CWnd::Create( lpszClass, NULL, dwStyle, rect, pParentWnd, nID, NULL);
}

void CDummyWnd::SetViewer( i3Viewer * pViewer)
{
	CRect rc;

	m_pViewer = pViewer;

	GetClientRect( &rc);

	m_pViewer->Resize( rc.Width(), rc.Height());
}

// CDummyWnd message handlers


/*
BOOL CDummyWnd::OnEraseBkgnd(CDC* pDC)
{
	return 0;
}

void CDummyWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if((m_pViewer != NULL) && (cx > 0))
	{
		m_pViewer->Resize( cx, cy);
	}
}
*/

void CDummyWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if( m_pViewer != NULL)
	{
		EnterRenderCS();

		if( g_bNeedToCreate)
		{
			g_pMainWnd->Reset();
			g_pMainWnd->GoRender();
			g_bNeedToCreate = FALSE;
		}


		m_pViewer->Render();

		LeaveRenderCS();
	}
}

/*
void CDummyWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();

	if( m_pViewer != NULL)
	{
		m_pViewer->OnLButtonDown( nFlags, point);
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CDummyWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_pViewer != NULL)
	{
		m_pViewer->OnLButtonUp( nFlags, point);
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CDummyWnd::OnMButtonDown(UINT nFlags, CPoint point)
{
	if( m_pViewer != NULL)
	{
		m_pViewer->OnMButtonDown( nFlags, point);
	}

	CWnd::OnMButtonDown(nFlags, point);
}

void CDummyWnd::OnMButtonUp(UINT nFlags, CPoint point)
{
	if( m_pViewer != NULL)
	{
		m_pViewer->OnMButtonUp( nFlags, point);
	}

	CWnd::OnMButtonUp(nFlags, point);
}

void CDummyWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	if( m_pViewer != NULL)
	{
		m_pViewer->OnRButtonDown( nFlags, point);
	}

	CWnd::OnRButtonDown(nFlags, point);
}

void CDummyWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	if( m_pViewer != NULL)
	{
		m_pViewer->OnRButtonUp( nFlags, point);
	}

	CWnd::OnRButtonUp(nFlags, point);
}

void CDummyWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_pViewer != NULL)
	{
		m_pViewer->OnMouseMove( nFlags, point);
	}

	CWnd::OnMouseMove(nFlags, point);
}

BOOL CDummyWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if( m_pViewer != NULL)
	{
		m_pViewer->OnMouseWheel( nFlags, zDelta, pt);
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CDummyWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( m_pViewer != NULL)
	{
		m_pViewer->OnKeyDown( nChar);

		if( nChar == 'D')
		{
			//I3_SG_DUMP( m_pViewer->GetSceneGraph());
		}
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CDummyWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}
*/