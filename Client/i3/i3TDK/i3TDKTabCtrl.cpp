// i3TDKTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKTabCtrl.h"
#include "i3TDKUtilGDI.h"
#include "i3TDKUtilCtrl.h"
#include "resource.h"
#include "i3TDKMeta.h"
#include "i3TDKGlobalVariable.h"
#include "i3TDKDockFrameWnd.h"
#include ".\i3tdktabctrl.h"

// i3TDKTabCtrl

#define		TAB_SPACE_UP			0
#define		TAB_CY					12

#define		TAB_LEFT_CX				12
#define		TAB_MID_CX				12
#define		TAB_RIGHT_CX			20

IMPLEMENT_DYNAMIC(i3TDKTabCtrl, CWnd)

i3TDKTabCtrl::~i3TDKTabCtrl()
{
	for( size_t i = 0;i < m_WndList.size(); ++i)
	{
		I3TDK_TAB_INFO * pInfo = m_WndList[i];
		
		delete pInfo;
	}

	if( m_hTabFont != nullptr)
	{
		::DeleteObject( m_hTabFont);
		m_hTabFont = nullptr;
	}
}

void i3TDKTabCtrl::GetTabArea( RECT * pRect)
{
	pRect->left = 0;
	pRect->right = m_Width;
	pRect->top = 0;
	pRect->bottom = pRect->top + (TAB_SPACE_UP + TAB_CY);
}

void i3TDKTabCtrl::CalcTabSize( INT32 idx, RECT * pRect)
{
	HDC dc = ::GetDC( m_hWnd);
	HFONT hOldFont;
	SIZE Sz;
	I3TDK_TAB_INFO * pInfo = m_WndList[idx];
	TCHAR szCaption[256];

	pInfo->m_pWnd->GetWindowText( szCaption, _countof(szCaption) - 1);

	hOldFont = (HFONT) ::SelectObject( dc, m_hTabFont);
	GetTextExtentPoint32( dc, szCaption, i3::generic_string_size( szCaption), &Sz);

	::SelectObject( dc, hOldFont);
	::ReleaseDC( m_hWnd, dc);

	pRect->left = 0;
	pRect->top = 0;
	pRect->right = Sz.cx + 8 + TAB_RIGHT_CX;
	pRect->bottom = TAB_CY;
}

void i3TDKTabCtrl::CalcAllTabPos(void)
{
	INT32 width, x, y;
	I3TDK_TAB_INFO * pInfo;
	CRect rc;

	// 전체 Tab의 크기를 더한다.
	width = 0;
	for(INT32 i = 0 ; i < GetPaneCount(); i++)
	{
		pInfo = m_WndList[i];

		CalcTabSize( i, &pInfo->m_TabRect);

		width += (pInfo->m_TabRect.Width() + 1);
	}

	GetTabArea( &rc);

	x = rc.left + 2;
	y = rc.top + (TAB_SPACE_UP + 2);

	if( width > m_Width)
	{
		// 모든 Tab의 크기를 더한 크기가 Client Area 보다 작다면
		// 각 Tab의 크기를 개별적으로 조절하여 밀어넣는다.
		INT32 divcx;

		divcx = m_Width / (INT32)m_WndList.size();

		for( size_t i = 0; i < m_WndList.size(); i++)
		{
			pInfo = m_WndList[i];

			pInfo->m_TabRect.left = x;
			pInfo->m_TabRect.top = y;
			pInfo->m_TabRect.right = x + divcx - 1;
			pInfo->m_TabRect.bottom = y + TAB_CY;

			x += divcx;
		}
	}
	else
	{
		// 모든 Tab이 Client 영역 안에 들어갈 수 있다면
		// 원래의 Text 크기대로 배치한다.
		for(size_t i = 0; i < m_WndList.size(); i++)
		{
			pInfo = m_WndList[i];

			pInfo->m_TabRect.OffsetRect( x, y);

			x += pInfo->m_TabRect.Width() + 1;
		}
	}
}

void i3TDKTabCtrl::GetTabRect( INT32 idx, RECT * pRect)
{
	I3TDK_TAB_INFO * pInfo = m_WndList[idx];
	* pRect = pInfo->m_TabRect;
}

void i3TDKTabCtrl::GetPaneRect( RECT * pRect)
{
	GetTabArea( pRect);

	pRect->left += 2;
	pRect->top = pRect->bottom + 2;
	pRect->right -= 2;
	pRect->bottom = m_Height - 2;
}


void i3TDKTabCtrl::AddPane( CWnd * pWnd)
{
	INT32 idx;
	I3TDK_TAB_INFO * pInfo;

	idx = (INT32)m_WndList.size();

	pInfo = new I3TDK_TAB_INFO;
	pInfo->m_pWnd = pWnd;

	m_WndList.push_back( pInfo);

	pWnd->SetParent( this);

	CalcAllTabPos();

	SelectTab( idx);

	Invalidate();
}

void i3TDKTabCtrl::RemovePane( CWnd * pWnd)
{
	INT32 idx;
	I3TDK_TAB_INFO * pInfo;

	idx = FindTab( pWnd);

	pInfo = m_WndList[idx];

	m_WndList.erase(m_WndList.begin() + idx);

	delete pInfo;

	if( idx == m_OldSelectTab)
	{
		m_OldSelectTab = -1;

		if( m_WndList.size() > 0)
		{
			SelectTab( 0);
		}
	}

	CalcAllTabPos();
	Invalidate();
}

void i3TDKTabCtrl::RemoveAllPane(void)
{
	
	I3TDK_TAB_INFO * pTab;

	for(size_t i = 0; i < m_WndList.size(); i++)
	{
		pTab = m_WndList[i];

		delete pTab;
	}

	m_WndList.clear();

	m_OldSelectTab = -1;
	m_OldFocusTab = -1;

	CalcAllTabPos();
}

INT32 i3TDKTabCtrl::FindTab( CWnd * pWnd)
{
	I3TDK_TAB_INFO * pInfo;

	for(size_t i = 0; i < m_WndList.size(); i++)
	{
		pInfo = m_WndList[i];

		if( pInfo->m_pWnd == pWnd)
			return i;
	}

	return -1;
}

INT32 i3TDKTabCtrl::FindTabByPos( POINT pt)
{
	
	I3TDK_TAB_INFO * pInfo;

	for(size_t i = 0; i < m_WndList.size(); i++)
	{
		pInfo = m_WndList[i];

		if( PtInRect( &pInfo->m_TabRect, pt))
			return i;
	}

	return -1;
}


void i3TDKTabCtrl::SelectTab( INT32 idx)
{
	I3TDK_TAB_INFO * pTab;

	// 이전 Selected Tab을 Invisible 상태로 바꾼다.
	if( m_OldSelectTab != -1)
	{
		pTab = m_WndList[ m_OldSelectTab];

		pTab->m_State &= ~I3TDK_TAB_STATE_SELECTED;
		pTab->m_pWnd->ShowWindow( SW_HIDE);
	}

	m_OldSelectTab = idx;

	if( m_OldSelectTab != -1)
	{
		CRect rc;

		pTab = m_WndList[ m_OldSelectTab];

		pTab->m_State |= I3TDK_TAB_STATE_SELECTED;

		GetPaneRect( &rc);

		pTab->m_pWnd->SetWindowPos( nullptr, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
		pTab->m_pWnd->ShowWindow( SW_SHOW);
	}

	Invalidate();
}

void i3TDKTabCtrl::FocusTab( INT32 idx)
{
	I3TDK_TAB_INFO * pTab;

	if( m_OldFocusTab == idx)
		return;

	// 이전 Selected Tab을 Invisible 상태로 바꾼다.
	if( m_OldFocusTab != -1)
	{
		pTab = m_WndList[ m_OldFocusTab];

		pTab->m_State &= ~I3TDK_TAB_STATE_FOCUSED;

		InvalidateRect( & pTab->m_TabRect, FALSE);
	}

	m_OldFocusTab = idx;

	if( m_OldFocusTab != -1)
	{
		CRect rc;

		pTab = m_WndList[ m_OldFocusTab];

		pTab->m_State |= I3TDK_TAB_STATE_FOCUSED;
		InvalidateRect( & pTab->m_TabRect, FALSE);
	}
}

void i3TDKTabCtrl::MovePaneTo( i3TDKTabCtrl * pDest)
{
	INT32 i;
	I3TDK_TAB_INFO * pInfo;

	for( i = 0; i < GetPaneCount(); i++)
	{
		pInfo = m_WndList[i];

		pDest->AddPane( pInfo->m_pWnd);
	}

	RemoveAllPane();
	Invalidate();
}

BEGIN_MESSAGE_MAP(i3TDKTabCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


BOOL i3TDKTabCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT nID)
{
	LPCTSTR pszClass;
	HCURSOR hCursor;
	HBRUSH hBack;

	hCursor = LoadCursor( nullptr, IDC_ARROW);
	I3ASSERT( hCursor != nullptr);

	hBack = (HBRUSH) GetStockObject( NULL_BRUSH);
	I3ASSERT( hBack != nullptr);

	pszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, hCursor, hBack, nullptr);
	I3ASSERT( pszClass != nullptr);

	dwStyle |= WS_CLIPCHILDREN;

	m_hTabFont = i3TDK::CreateDefFont( 11, _T("Tahoma"));
	I3ASSERT( m_hTabFont);

	return CWnd::Create( pszClass, _T("TabCtrl"), dwStyle, rect, pParentWnd, nID, nullptr);
}

// i3TDKTabCtrl message handlers

int i3TDKTabCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}

void i3TDKTabCtrl::DrawTab( HDC dc, I3TDK_TAB_INFO * pTab)
{
	HBITMAP hLeft, hMid, hRight;
	HFONT	hOldFont;
	INT32 x1, y1, x2, y2;
	TCHAR szCaption[256];
	RECT * pRect = &pTab->m_TabRect;
	CRect rc;
	bool bSel = ((pTab->m_State & I3TDK_TAB_STATE_SELECTED) != 0);
	bool bFocus = ((pTab->m_State & I3TDK_TAB_STATE_FOCUSED) != 0);

	hOldFont = (HFONT) ::SelectObject( dc, m_hTabFont);

	if( bSel)
	{
		hLeft = g_pTDKGlobalRes->m_hbmpTabForeLeft;
		hMid = g_pTDKGlobalRes->m_hbmpTabForeMid;
		hRight = g_pTDKGlobalRes->m_hbmpTabForeRight;

		SetTextColor( dc, g_pTDKGlobalRes->getColor( I3TDK_COLOR_FORE_CAPTION_TEXT));
	}
	else
	{
		hLeft = g_pTDKGlobalRes->m_hbmpTabBackLeft;
		hMid = g_pTDKGlobalRes->m_hbmpTabBackMid;
		hRight = g_pTDKGlobalRes->m_hbmpTabBackRight;

		SetTextColor( dc, g_pTDKGlobalRes->getColor( I3TDK_COLOR_BACK_CAPTION_TEXT));
	}

	if( bFocus)
		SetTextColor( dc, g_pTDKGlobalRes->getColor( I3TDK_COLOR_TEXT_HILIGHT));

	x1 = pRect->left;
	y1 = pRect->top;
	x2 = pRect->right;
	y2 = pRect->bottom;

	// Body
	i3TDK::FillBlt( dc, x1 + TAB_LEFT_CX, y1, x2 - x1 - TAB_LEFT_CX - TAB_RIGHT_CX, TAB_CY, 
		hMid, 0, 0, TAB_LEFT_CX, TAB_CY);

	// Left
	i3TDK::BitBlt( dc, x1, y1, TAB_LEFT_CX, TAB_CY, hLeft);

	// Right
	i3TDK::AlphaBlend( dc, x2 - TAB_RIGHT_CX, y1, TAB_RIGHT_CX, TAB_CY, 
		hRight, 0, 0, TAB_RIGHT_CX, TAB_CY);

	// Text
	pTab->m_pWnd->GetWindowText( szCaption, _countof(szCaption) - 1);

	rc.SetRect( x1 + 7, y1 + 2, x2, y2);

	::DrawText( dc, szCaption, -1, &rc, DT_LEFT | DT_END_ELLIPSIS | DT_SINGLELINE);

	::SelectObject( dc, hOldFont);
}

void i3TDKTabCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	HPEN hOldPen;
	HBRUSH hOldBrush;
	INT32 i;
	CRect TabRect;
	INT32 x1, y1, x2, y2;

	SetBkMode( dc, TRANSPARENT);

	// Tab의 밑바탕
	{
		GetTabArea( &TabRect);
		::FillRect( dc, &TabRect, g_pTDKGlobalRes->getBrush( I3TDK_COLOR_BACK_APP_BKGND));
	}

	x1 = TabRect.left;
	y1 = TabRect.bottom;
	x2 = TabRect.right;
	y2 = m_Height;

	// 테두리1
	{
		hOldPen = (HPEN) ::SelectObject( dc.m_hDC, g_pTDKGlobalRes->getPen( I3TDK_COLOR_BACK_BORDER));
		hOldBrush = (HBRUSH) ::SelectObject( dc.m_hDC, GetStockObject( NULL_BRUSH));

		::Rectangle( dc.m_hDC, x1, y1, x2, y2);
	}

	// 테두리2
	{
		SelectObject( dc.m_hDC, g_pTDKGlobalRes->getPen( I3TDK_COLOR_FORE_3D_LIT_DARK));
		::MoveToEx( dc.m_hDC, x1 + 1, y2 - 2, nullptr);
		::LineTo( dc.m_hDC, x1 + 1, y1 + 1);
		SelectObject( dc.m_hDC, g_pTDKGlobalRes->getPen( I3TDK_COLOR_FORE_3D_LIT));
		::LineTo( dc.m_hDC, x2 - 2, y1 + 1);

		SelectObject( dc.m_hDC, g_pTDKGlobalRes->getPen( I3TDK_COLOR_FORE_3D_SHADOW));
		::LineTo( dc.m_hDC, x2 - 2, y2 - 2);
		SelectObject( dc.m_hDC, g_pTDKGlobalRes->getPen( I3TDK_COLOR_FORE_3D_SHADOW_DARK));
		::LineTo( dc.m_hDC, x1 + 1, y2 - 2);
	}

	// Pane들이 배치되는 영역은 그리지 않는다.

	// Tab Drawing
	// 역순으로 그린다.
	{
		for( i = (INT32)m_WndList.size() - 1; i >= 0; i--)
		{
			I3TDK_TAB_INFO * pTab = m_WndList[i];

			DrawTab( dc.m_hDC, pTab);
		}
	}

	// Restore
	SelectObject( dc.m_hDC, hOldPen);
	SelectObject( dc.m_hDC, hOldBrush);
}

CWnd * i3TDKTabCtrl::GetSelectedPane(void)
{
	if( m_OldSelectTab == -1)
		return nullptr;

	I3TDK_TAB_INFO * pInfo = m_WndList[ m_OldSelectTab] ;

	return pInfo->m_pWnd;
}

void i3TDKTabCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	m_Width = cx;
	m_Height = cy;

	{
		CWnd * pWnd = GetSelectedPane();
		CRect rc;

		if( pWnd != nullptr)
		{
			GetPaneRect( &rc);
			pWnd->SetWindowPos( nullptr, rc.left, rc.top , rc.Width(), rc.Height(), 
				SWP_NOACTIVATE | SWP_NOZORDER);
		}
	}

	CalcAllTabPos();
}

HBRUSH i3TDKTabCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch( nCtlColor)
	{
		case CTLCOLOR_DLG :		return g_pTDKGlobalRes->getBrush( I3TDK_COLOR_BACK_APP_BKGND);
	}

	return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}

void i3TDKTabCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDrag )
	{
		// Window 영역 바깥으로 나가는지를 확인하여,
		// 만약 나간다면, Docking Window에 대한 Drag으로 간주한다.
		if( m_bDockingMode)
		{
			if((point.x < 0) || (point.y < 0) || (point.x > m_Width) || (point.y > m_Height))
			{
				ClientToScreen( &m_DragStartPt);
				g_pDockFrameWnd->BeginDock( this, I3TDK_DOCK_DROP_PANE, m_OldSelectTab, m_DragStartPt, m_Width, m_Height);
			}
		}
	}
	else
	{
		INT32 idx = FindTabByPos( point);

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

		FocusTab( idx);
	}

	CWnd::OnMouseMove(nFlags, point);
}

void i3TDKTabCtrl::OnMouseLeave(void)
{
	m_bSetTrackMouse = false;
	m_bDrag = false;

	FocusTab( -1);
}

LRESULT i3TDKTabCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message)
	{
		case WM_MOUSELEAVE :		OnMouseLeave();			break;
	}

	return CWnd::WindowProc(message, wParam, lParam);
}

void i3TDKTabCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	INT32 idx;
	CRect rc;

	idx = FindTabByPos( point);
	if( idx != -1)
	{
		SelectTab( idx);

		m_bDrag = true;

		::SetCapture( m_hWnd);
	}

	m_DragStartPt = point;

	CWnd::OnLButtonDown(nFlags, point);
}

void i3TDKTabCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_bDrag)
	{
		m_bDrag = false;

		::ReleaseCapture();
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void i3TDKTabCtrl::OnDestroy()
{
	RemoveAllPane();

	CWnd::OnDestroy();
}
