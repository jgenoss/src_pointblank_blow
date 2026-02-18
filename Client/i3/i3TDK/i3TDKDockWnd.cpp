// i3TDKDockWnd.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKDockWnd.h"
#include "i3TDKUtilGDI.h"
#include "i3TDKUtilCtrl.h"
#include "resource.h"
#include "i3TDKMeta.h"
#include "i3TDKGlobalVariable.h"
#include "i3TDKDockFrameWnd.h"

#define		FLOAT_STATUS_CY			11
#define		FLOAT_BORDER_CX			3

// i3TDKDockWnd

IMPLEMENT_DYNAMIC(i3TDKDockWnd, CWnd)

i3TDKDockWnd::~i3TDKDockWnd()
{
	if( m_hCaptionFont != nullptr)
	{
		::DeleteObject( m_hCaptionFont);
		m_hCaptionFont = nullptr;
	}
}

void i3TDKDockWnd::GetCaptionArea( RECT * pRect, bool bScreenCoord)
{
	GetWindowRect( pRect);

	if( bScreenCoord)
	{
		pRect->bottom = pRect->top + FOLDER_CAPTION_BACK_CY;
	}
	else
	{
		pRect->right -= pRect->left;
		pRect->left = 0;
		pRect->top = 0;
		pRect->bottom = FOLDER_CAPTION_BACK_CY;
	}
}

void i3TDKDockWnd::GetPaneRect( RECT * pRect)
{
	pRect->left = 0;
	pRect->top = 0;
	pRect->right = m_Width;
	pRect->bottom = m_Height;
}

I3TDK_DOCK_PART i3TDKDockWnd::HitTestRect( POINT pt)
{
	CRect temp, scrRect;

	GetWindowRect( &scrRect);

	// In
	if( PtInRect( &scrRect, pt) == FALSE)
		return I3TDK_DOCK_PART_NONE;

	temp = scrRect;

	// Left side
	{
		temp.right = MIN( scrRect.left + DOCK_SPACE, scrRect.right);

		if( PtInRect( &temp, pt))
			return I3TDK_DOCK_PART_LEFT;
	}

	// Right side
	{
		temp.left = MAX( scrRect.right - DOCK_SPACE, scrRect.left);
		temp.right = scrRect.right;

		if( PtInRect( &temp, pt))
			return I3TDK_DOCK_PART_RIGHT;
	}

	// top side
	{
		temp.left = scrRect.left;
		temp.bottom = MIN( scrRect.top + DOCK_SPACE, scrRect.bottom);

		if( PtInRect( &temp, pt))
			return I3TDK_DOCK_PART_TOP;
	}

	// bottom side
	{
		temp.top = MAX( scrRect.bottom - DOCK_SPACE, scrRect.top);
		temp.bottom = scrRect.bottom;

		if( PtInRect( &temp, pt))
			return I3TDK_DOCK_PART_BOTTOM;
	}

	return I3TDK_DOCK_PART_IN;
}


BEGIN_MESSAGE_MAP(i3TDKDockWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_MOUSEMOVE()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCHITTEST()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()


BOOL i3TDKDockWnd::Create( DWORD dwStyle, INT32 defw, INT32 defh, const RECT &rect, CWnd * pParentWnd, UINT nID)
{
	LPCTSTR pszClass;
	HCURSOR hCursor;
	HBRUSH hBack;
	DWORD dwExStyle;
	BOOL Rv;

	hCursor = LoadCursor( nullptr, IDC_ARROW);
	I3ASSERT( hCursor != nullptr);

	hBack = (HBRUSH) GetStockObject( NULL_BRUSH);
	I3ASSERT( hBack != nullptr);

	pszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, hCursor, hBack, nullptr);
	I3ASSERT( pszClass != nullptr);

	dwStyle |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	m_hCaptionFont = i3TDK::CreateDefFont( 16, _T("Trebuchet MS"), 700);
	I3ASSERT( m_hCaptionFont != nullptr);

	if( dwStyle & WS_CHILD)
	{
		dwExStyle = 0;
	}
	else
	{
		dwExStyle = WS_EX_PALETTEWINDOW;
		dwStyle |= WS_POPUP;
	}

	m_DefWidth = defw;
	m_DefHeight = defh;

	Rv = CWnd::CreateEx( dwExStyle, pszClass, _T("DockWnd"), dwStyle, rect, pParentWnd, 0);
	if( Rv == FALSE)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not create window.");
	}

	return Rv;
}

void i3TDKDockWnd::AddPane( CWnd * pWnd)
{
	m_TabCtrl.AddPane( pWnd);

	OnNcPaint();
}

void i3TDKDockWnd::RemovePane( CWnd * pWnd)
{
	m_TabCtrl.RemovePane( pWnd);

	OnNcPaint();
}

void i3TDKDockWnd::RemoveAllPane(void)
{
	m_TabCtrl.RemoveAllPane();

	OnNcPaint();
}

// i3TDKDockWnd message handlers

int i3TDKDockWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		if( lpCreateStruct->style & WS_CHILD)
			m_State |= I3TDK_DOCK_STATE_DOCKED;
	}

	{
		CRect rect;

		GetPaneRect( &rect);

		m_TabCtrl.Create( WS_VISIBLE | WS_CHILD, rect, this, 100);
		m_TabCtrl.SetDockMode( true);
	}

	return 0;
}

void i3TDKDockWnd::DrawCaption( HDC dc)
{
	CRect rc;
	INT32 div, mod, i, x;
	HBITMAP hbmpBack, hbmpBackLeft, hbmpBackRight, hbmpClose, hbmpGrip;
	HFONT hOldFont;

	/*
	if( pInfo->m_State & I3TDK_FOLD_STATE_FOCUSED)
	{
		hbmpBack =		g_pTDKGlobalRes->m_hbmpSelCaptionBack;
		hbmpBackLeft =	g_pTDKGlobalRes->m_hbmpSelCaptionBackLeft;
		hbmpBackRight = g_pTDKGlobalRes->m_hbmpSelCaptionBackRight;
		hbmpClose =		g_pTDKGlobalRes->m_hbmpSelCaptionClose;
		hbmpGrip =		g_pTDKGlobalRes->m_hbmpSelCaptionGrip;

		if( pInfo->m_State & I3TDK_FOLD_STATE_COLLAPSED)
			hbmpInd = g_pTDKGlobalRes->m_hbmpSelCaptionCollapse;
		else
			hbmpInd = g_pTDKGlobalRes->m_hbmpSelCaptionExpand;
	}
	else
	*/
	{
		hbmpBack =		g_pTDKGlobalRes->m_hbmpCaptionBack;
		hbmpBackLeft =	g_pTDKGlobalRes->m_hbmpCaptionBackLeft;
		hbmpBackRight = g_pTDKGlobalRes->m_hbmpCaptionBackRight;
		hbmpClose =		g_pTDKGlobalRes->m_hbmpCaptionClose;
		hbmpGrip =		g_pTDKGlobalRes->m_hbmpCaptionGrip;
	}

	// Caption Bar
	GetCaptionArea( &rc);

	// Background
	div = ((rc.right - rc.left) / FOLDER_CAPTION_BACK_CX);
	mod = (rc.right - rc.left) % FOLDER_CAPTION_BACK_CX;

	x = rc.left;

	for( i = 0; i < div; i++)
	{
		i3TDK::BitBlt( dc, x, rc.top, FOLDER_CAPTION_BACK_CX, FOLDER_CAPTION_BACK_CY, hbmpBack);

		x += FOLDER_CAPTION_BACK_CX;
	}

	if( mod != 0)
	{
		i3TDK::BitBlt( dc, rc.right - mod, rc.top, mod, FOLDER_CAPTION_BACK_CY, hbmpBack);
	}

	// Left Side
	i3TDK::BitBlt( dc, rc.left, rc.top, FOLDER_CAPTION_BACK_LEFT_CX, FOLDER_CAPTION_BACK_CY, 
		hbmpBackLeft);

	// Right Side
	i3TDK::BitBlt( dc, rc.right - FOLDER_CAPTION_BACK_RIGHT_CX, rc.top, 
		FOLDER_CAPTION_BACK_RIGHT_CX, FOLDER_CAPTION_BACK_CY, hbmpBackRight);

	// Grip
	rc.left += 2;
	i3TDK::BitBlt( dc, rc.left, rc.top + 2, 
		FOLDER_CAPTION_GRIP_CX, FOLDER_CAPTION_GRIP_CY, hbmpGrip);

	// Close
	i3TDK::BitBlt( dc, rc.right - 4 - FOLDER_CAPTION_CLOSE_CX, rc.top + 3,
		FOLDER_CAPTION_CLOSE_CX, FOLDER_CAPTION_CLOSE_CY, hbmpClose);

	// Text
	hOldFont = (HFONT) ::SelectObject( dc, m_hCaptionFont);

	rc.left += FOLDER_CAPTION_IND_CX + 5;
	rc.right -= FOLDER_CAPTION_CLOSE_CX - 5;

	{
		TCHAR szText[256];

		if( m_TabCtrl.GetSelectedPane() != nullptr)
			m_TabCtrl.GetSelectedPane()->GetWindowText( szText, _countof( szText));
		else
			szText[0] = 0;

		::DrawText( dc, szText, -1, &rc, DT_END_ELLIPSIS | DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}

	::SelectObject( dc, hOldFont);
}

void i3TDKDockWnd::DrawBorder( HDC dc)
{
	HBRUSH hOldBrush;
	HPEN hOldPen;
	INT32 x1, y1, x2, y2;

	x1 = 0;
	y1 = 0;
	x2 = m_WindowRect.Width();
	y2 = m_WindowRect.Height();

	// 가장 외곽 선 (검정)
	{
		hOldBrush = (HBRUSH) ::SelectObject( dc, GetStockObject( NULL_BRUSH));
		hOldPen = (HPEN) ::SelectObject( dc, g_pTDKGlobalRes->getPen( I3TDK_COLOR_FLOAT_BORDER));

		::Rectangle( dc, x1, y1, x2, y2);

		::Rectangle( dc,	x1 + FLOAT_BORDER_CX - 1, y1 + FOLDER_CAPTION_BACK_CY - 1, 
							x2 - FLOAT_BORDER_CX + 1, y2 - FLOAT_STATUS_CY + 1);
	}

	// 테두리 면
	{
		HBRUSH hBrush;
		CRect rc;

		hBrush = g_pTDKGlobalRes->getBrush( I3TDK_COLOR_FLOAT_BORDER_FACE);

		// 왼쪽 테두리 면
		rc.left =  1;
		rc.top = m_LeftBorderRect.top - m_WindowRect.top;
		rc.right = m_LeftBorderRect.Width() - 1;
		rc.bottom = m_LeftBorderRect.bottom - m_WindowRect.top + 1;
		::FillRect( dc, &rc, hBrush);

		// 오른쪽 테두리 면
		rc.left = m_RightBorderRect.left - m_WindowRect.left + 1;
		rc.right = m_WindowRect.Width() - 1;
		::FillRect( dc, &rc, hBrush);

		// 아래쪽 테두리 면
		rc.left = 1;
		rc.right = m_RightBorderRect.right - m_WindowRect.left - 1;
		rc.top = m_BottomBorderRect.top - m_WindowRect.top + 1;
		rc.bottom = m_BottomBorderRect.bottom - m_WindowRect.top - 1;
		::FillRect( dc, &rc, hBrush);
	}

	// Size Grip
	{
		i3TDK::BitBlt( dc, x2 - FLOAT_GRIP_CX - 2, y2 - FLOAT_GRIP_CY - 3, FLOAT_GRIP_CX, FLOAT_GRIP_CY,
			g_pTDKGlobalRes->m_hbmpFloatGrip);
	}

	// Shading Line
	{
		::SelectObject( dc, g_pTDKGlobalRes->getPen( I3TDK_COLOR_FORE_3D_FACE));
		MoveToEx( dc, x1 + 1, y2 - 2, nullptr);
		LineTo( dc, x2 - 1, y2 - 2);
	}

	::SelectObject( dc, hOldBrush);
	::SelectObject( dc, hOldPen);
}

void i3TDKDockWnd::OnNcPaint()
{
	HDC dc;

	dc = ::GetWindowDC( m_hWnd);

	SetBkMode( dc, TRANSPARENT);

	// Caption
	DrawCaption( dc);

	if( IsDocked() == false)
	{
		DrawBorder( dc);
	}

	::ReleaseDC( m_hWnd, dc);
}

void i3TDKDockWnd::MovePaneTo( i3TDKDockWnd * pDest)
{
	m_TabCtrl.MovePaneTo( &pDest->m_TabCtrl);
}


void i3TDKDockWnd::UpdateSize(void)
{
	CRect rc;

	{
		GetClientRect( &rc);

		m_Width = rc.Width();
		m_Height = rc.Height();
	}

	GetWindowRect( &m_WindowRect);

	m_LeftBorderRect = m_WindowRect;
	m_LeftBorderRect.right = m_LeftBorderRect.left + FLOAT_BORDER_CX;
	m_LeftBorderRect.top += FOLDER_CAPTION_BACK_CY;
	m_LeftBorderRect.bottom -= FLOAT_STATUS_CY;

	m_RightBorderRect = m_WindowRect;
	m_RightBorderRect.left = m_RightBorderRect.right - FLOAT_BORDER_CX;
	m_RightBorderRect.top += FOLDER_CAPTION_BACK_CY;
	m_RightBorderRect.bottom -= FLOAT_STATUS_CY;

	m_BottomBorderRect.left = m_WindowRect.left;
	m_BottomBorderRect.right = m_WindowRect.right - FLOAT_STATUS_CY;
	m_BottomBorderRect.top = m_WindowRect.bottom - FLOAT_STATUS_CY;
	m_BottomBorderRect.bottom = m_WindowRect.bottom;

	m_SizeGripRect.left = m_WindowRect.right - FLOAT_STATUS_CY;
	m_SizeGripRect.top = m_WindowRect.bottom - FLOAT_STATUS_CY;
	m_SizeGripRect.right = m_WindowRect.right;
	m_SizeGripRect.bottom = m_WindowRect.bottom;

	if( ::IsWindow( m_TabCtrl.m_hWnd))
	{
		GetPaneRect( &rc);
		m_TabCtrl.SetWindowPos( nullptr, rc.left, rc.top , rc.Width(), rc.Height(), 
			SWP_NOACTIVATE | SWP_NOZORDER);
	}

	OnNcPaint();
}

void i3TDKDockWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	{
		CRect rt;

		GetWindowRect( &rt);

		I3TRACE( "DOCK : %d %d - %d %d\n", rt.left, rt.top, rt.Width(), rt.Height());
	}

	UpdateSize();
}

void i3TDKDockWnd::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CWnd::OnWindowPosChanged(lpwndpos);

	UpdateSize();
}

void i3TDKDockWnd::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CWnd::OnWindowPosChanging(lpwndpos);

	UpdateSize();
}



HBRUSH i3TDKDockWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch( nCtlColor)
	{
		case CTLCOLOR_DLG :		return g_pTDKGlobalRes->getBrush( I3TDK_COLOR_BACK_APP_BKGND);
	}

	return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}

void i3TDKDockWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	{
		//INT32 idx = FindTabByPos( point);

		if( m_bSetTrackMouse == false)
		{
			m_bSetTrackMouse = true;

			// Track Mouse Event
			{
				TRACKMOUSEEVENT event;

				event.cbSize = sizeof( TRACKMOUSEEVENT);
				event.dwFlags = TME_LEAVE;
				event.hwndTrack = m_hWnd;

				TrackMouseEvent( &event);
			}
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

void i3TDKDockWnd::OnMouseLeave(void)
{
	m_bSetTrackMouse = false;
}

LRESULT i3TDKDockWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message)
	{
		case WM_MOUSELEAVE :		OnMouseLeave();			break;
	}

	return CWnd::WindowProc(message, wParam, lParam);
}

void i3TDKDockWnd::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	CRect rc;

	// Caption과의 Collision Test
	{
		GetCaptionArea( &rc, true);

		if( PtInRect( &rc, point))
		{
			// 전체 탭의 Dragging
			if( g_pDockFrameWnd != nullptr)
			{
				g_pDockFrameWnd->BeginDock( this, I3TDK_DOCK_DROP_WHOLE, 0, point, m_DefWidth, m_DefHeight);
			}
		}
	}

	CWnd::OnNcLButtonDown(nHitTest, point);
}

void i3TDKDockWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	lpncsp->rgrc[0].top    += FOLDER_CAPTION_BACK_CY;

	if( IsDocked() == false)
	{
		// Docking되지 않은 경우에는 크기 변경할 수 있는 테두리가
		// 필요하기 때문에...
		//lpncsp->rgrc[0].top;
		lpncsp->rgrc[0].left += FLOAT_BORDER_CX;
		lpncsp->rgrc[0].right -= FLOAT_BORDER_CX;
		lpncsp->rgrc[0].bottom -= FLOAT_STATUS_CY;
	}

	CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
}

#if _MSC_VER >= 1400
	LRESULT i3TDKDockWnd::OnNcHitTest(CPoint point)
#else
	UINT i3TDKDockWnd::OnNcHitTest(CPoint point)
#endif
{
	CRect rc;

	GetCaptionArea( &rc, true);

	if( PtInRect( &rc, point))
	{
		// Caption Hit~
		return HTMENU;
	}

	// Left Border
	if( PtInRect( &m_LeftBorderRect, point))
	{
		return HTLEFT;
	}

	// Right Border
	if( PtInRect( &m_RightBorderRect, point))
	{
		return HTRIGHT;
	}

	// Sizing Grip
	if( PtInRect( &m_SizeGripRect, point))
	{
		return HTBOTTOMRIGHT;
	}

	if( PtInRect( &m_BottomBorderRect, point))
	{
		return HTBOTTOM;
	}

	return CWnd::OnNcHitTest(point);
}

void i3TDKDockWnd::SaveContext( char * pszStr)
{
	CRect rc;

	GetWindowRect( &rc);

	sprintf( pszStr, "%d %d %d %d %d %d",
		rc.left, rc.top, rc.Width(), rc.Height(),
		m_DefWidth, m_DefHeight);
}

void i3TDKDockWnd::LoadContext( char * pszStr)
{
	i3LineLexer lex;
	INT32 x, y, cx, cy;
	char szToken[128], sep;

	lex.SetMasterString( pszStr);

	// Left
	lex.GetNextToken( szToken, &sep);
	x = atoi( szToken);

	// Top
	lex.GetNextToken( szToken, &sep);
	y = atoi( szToken);

	// Right
	lex.GetNextToken( szToken, &sep);
	cx = atoi( szToken);

	// Bottom
	lex.GetNextToken( szToken, &sep);
	cy = atoi( szToken);

	SetWindowPos( nullptr, x, y, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);

	// default Width
	lex.GetNextToken( szToken, &sep);
	m_DefWidth = atoi( szToken);
	
	// Default Height
	lex.GetNextToken( szToken, &sep);
	m_DefHeight = atoi( szToken);
}
