// i3TDKColorCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKColorCtrl.h"
#include ".\i3tdkcolorctrl.h"

// i3TDKColorCtrl

IMPLEMENT_DYNAMIC(i3TDKColorCtrl, CWnd)

i3TDKColorCtrl::~i3TDKColorCtrl()
{
	if( m_hBrush != nullptr)
	{
		DeleteObject( m_hBrush);
		m_hBrush = nullptr;
	}
}

void i3TDKColorCtrl::setColor( COLORREAL * pColor)
{
	i3Color::Set( &m_Color, pColor);

	COLOR col;

	i3Color::Set( &col, pColor);

	HBRUSH hbr = CreateSolidBrush( RGB( col.r, col.g, col.b));

	::SetClassLong( m_hWnd, GCL_HBRBACKGROUND, (LONG)hbr);

	if( m_hBrush != nullptr)
	{
		DeleteObject( m_hBrush);
	}

	m_hBrush = hbr;

	Invalidate( TRUE);
}

void i3TDKColorCtrl::setColor( COLOR * pCol)
{
	COLORREAL rcol;

	i3Color::Set( &rcol, pCol);

	setColor( &rcol);
}


BEGIN_MESSAGE_MAP(i3TDKColorCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

BOOL i3TDKColorCtrl::Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT id)
{
	LPCTSTR lpszClass;
	HCURSOR hCursor;
	HBRUSH hBrush;
	HICON	hIcon;

	hCursor = LoadCursor( nullptr, IDC_ARROW);

	hBrush = (HBRUSH) ::GetStockObject( NULL_BRUSH);
	hIcon	= ::LoadIcon( g_hInstTDK, IDI_APPLICATION);

	lpszClass = AfxRegisterWndClass( CS_HREDRAW | CS_HREDRAW | CS_DBLCLKS, hCursor, hBrush, hIcon);
	I3ASSERT( lpszClass != nullptr);

	dwStyle &= ~WS_BORDER;

	return CWnd::CreateEx( WS_EX_STATICEDGE, lpszClass, _T(""), dwStyle, rect, pParentWnd, id);
}


// i3TDKColorCtrl message handlers


int i3TDKColorCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void i3TDKColorCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
}

void i3TDKColorCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	::FillRect( dc.m_hDC, & dc.m_ps.rcPaint, m_hBrush);
}

void i3TDKColorCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	i3TDKColorPicker dlg;

	if( dlg.Execute( &m_Color))
	{
		setColor( dlg.getSelectedColor());

		UINT32 id = ::GetWindowLong( m_hWnd, GWL_ID);

		GetParent()->PostMessage( WM_TDK_SETCOLOR, id, (LPARAM) m_hWnd);
	}

	CWnd::OnLButtonDblClk(nFlags, point);
}
