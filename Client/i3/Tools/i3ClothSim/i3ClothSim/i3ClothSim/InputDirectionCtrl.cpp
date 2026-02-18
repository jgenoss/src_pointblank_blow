// InputDirectionCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3ClothSim.h"
#include "InputDirectionCtrl.h"
#include ".\inputdirectionctrl.h"


// CInputDirectionCtrl

IMPLEMENT_DYNAMIC(CInputDirectionCtrl, CWnd)
CInputDirectionCtrl::CInputDirectionCtrl()
{
	m_StartPoint.x = 0;
	m_StartPoint.y = 0;
	m_EndPoint.x = 0;
	m_EndPoint.y = 0;

	m_bDrag = false;

	i3Vector::Set( &m_Direction, 1.0f, 0.0f);
}

CInputDirectionCtrl::~CInputDirectionCtrl()
{
}

BOOL CInputDirectionCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT nID)
{
	LPCTSTR pszClass;
	HCURSOR hCursor;
	HBRUSH hBack;

	hCursor = ::LoadCursor( NULL, MAKEINTRESOURCE( IDC_CROSS));
	hBack = g_pTDKGlobalRes->getBrush( I3TDK_COLOR_BACK_APP_BKGND);

	pszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, hCursor, hBack, NULL);

	return CWnd::Create( pszClass, "", dwStyle, rect, pParentWnd, nID, NULL);
}

BEGIN_MESSAGE_MAP(CInputDirectionCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



// CInputDirectionCtrl message handlers


void CInputDirectionCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	HPEN oldPen;

	oldPen = (HPEN) ::SelectObject( dc.m_hDC, GetStockObject( WHITE_PEN));

	dc.MoveTo( m_StartPoint);
	dc.LineTo( m_EndPoint);
	
	::SelectObject( dc.m_hDC, oldPen);
}

void CInputDirectionCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);

	SetCapture();

	m_EndPoint = m_StartPoint = point;
	m_bDrag = true;

	Invalidate();
}

void CInputDirectionCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	m_bDrag = false;

	m_EndPoint = point;

	if((m_StartPoint.x != m_EndPoint.x) || (m_StartPoint.y != m_EndPoint.y))
	{
		i3Vector::Set( &m_Direction, (REAL32)(m_EndPoint.x - m_StartPoint.x),
									(REAL32)(m_EndPoint.y - m_StartPoint.y));

		i3Vector::Normalize( &m_Direction, &m_Direction);
	}

	Invalidate();
	
	CWnd::OnLButtonUp(nFlags, point);
}

void CInputDirectionCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDrag)
	{
		m_EndPoint = point;

		Invalidate();
	}

	CWnd::OnMouseMove(nFlags, point);
}
