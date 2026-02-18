// i3TDKWndFoldCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKWndFoldCtrl.h"
#include "i3TDKGlobalRes.h"
#include "i3TDKGlobalVariable.h"
#include "i3TDKUtilGDI.h"

enum DRAG_MODE
{
	DRAG_NONE = 0,
	DRAG_RESIZE,
	DRAG_REORDER,
	DRAG_SCROLL
};

// i3TDKWndFoldCtrl

IMPLEMENT_DYNAMIC(i3TDKWndFoldCtrl, CWnd)

i3TDKWndFoldCtrl::~i3TDKWndFoldCtrl()
{
	

	if( m_hBrush != nullptr)
	{
		::DeleteObject( m_hBrush);
		m_hBrush = nullptr;
	}

	if( m_hFont != nullptr)
	{
		::DeleteObject( m_hFont);
		m_hFont = nullptr;
	}

	RemoveAllPane();
}

void i3TDKWndFoldCtrl::AddPane( CWnd * pWnd, const TCHAR * pszCaption)
{
	I3WNDFOLDINFO * pNew;
	CRect rect;

	pNew = new I3WNDFOLDINFO;

	pNew->m_pWnd = pWnd;

	pWnd->GetWindowRect( &rect);

	pNew->m_Width = rect.Width();
	pNew->m_Height = rect.Height();
	m_WndList.push_back( pNew);

	pWnd->SetParent( this);

	{
		SetClassLongPtr( pWnd->m_hWnd, GCLP_HBRBACKGROUND, (LONG) g_pTDKGlobalRes->getBrush( I3TDK_COLOR_BACK_APP_BKGND));
	}

	// Caption
	if( pszCaption != nullptr)
	{
		pNew->m_strCaption = pszCaption;
	}

	CalcPos();
	RecalcScrollInfo();
	AdjustPanePos();
}

INT32 i3TDKWndFoldCtrl::FindPane( CWnd * pWnd)
{
	
	I3WNDFOLDINFO * pInfo;

	for(size_t i = 0; i < m_WndList.size(); i++)
	{
		pInfo = m_WndList[i];

		if( pInfo->m_pWnd == pWnd)
			return i;
	}

	return -1;
}

void i3TDKWndFoldCtrl::RemovePane( CWnd * pWnd)
{
	INT32 idx;
	I3WNDFOLDINFO * pInfo;

	idx = FindPane( pWnd);
	if( idx == -1)
		return;

	pInfo = m_WndList[idx];

	delete pInfo;

	m_WndList.erase(m_WndList.begin() + idx);

	CalcPos();
	AdjustPanePos();
}

void i3TDKWndFoldCtrl::RemoveAllPane(void)
{
	
	I3WNDFOLDINFO * pInfo;

	for( SIZE_T i = 0; i < m_WndList.size(); i++)
	{
		pInfo = m_WndList[i];

		delete pInfo;
	}

	m_WndList.clear();
}

BOOL i3TDKWndFoldCtrl::Create( const TCHAR* pszCaption, DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID)
{
	LPCTSTR lpszClass;
	HCURSOR hCursor;
	HBRUSH hbrBack;

	hCursor = ::LoadCursor( nullptr, IDC_ARROW);
	I3ASSERT( hCursor);

	hbrBack = ::GetSysColorBrush( COLOR_WINDOW);

	lpszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_DROPSHADOW, hCursor, hbrBack, nullptr);
	I3ASSERT( lpszClass != nullptr);

	dwStyle |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VSCROLL;

	if( pszCaption != nullptr)
	{
		m_strCaption = pszCaption;
	}

	m_hBrush = CreateSolidBrush( RGB( 126, 126, 126));

	{
		LOGFONT	log;

		memset( &log, 0, sizeof(log));
		log.lfHeight = 15;
		log.lfWidth = 0;
		log.lfWeight = 700;
		log.lfCharSet = DEFAULT_CHARSET;
		log.lfOutPrecision = OUT_DEFAULT_PRECIS;
		log.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		log.lfQuality = DRAFT_QUALITY;
		log.lfPitchAndFamily = DEFAULT_PITCH;
		i3::generic_string_copy( log.lfFaceName, _T("Arial") );

		m_hFont = CreateFontIndirect( &log);
	}

	return CWnd::Create( lpszClass, pszCaption, dwStyle, rect, pParentWnd, nID, nullptr);
}

void i3TDKWndFoldCtrl::CalcPos(void)
{
	INT32 x, y, cy;
	I3WNDFOLDINFO * pInfo = nullptr;
	CRect Rect;

	x = 0;
	y = 0;

	GetClientRect( &Rect);

	for(size_t i = 0; i < m_WndList.size(); i++)
	{
		pInfo = m_WndList[i];

		if( pInfo->m_State & I3TDK_FOLD_STATE_COLLAPSED)
		{
			cy = FOLDER_CAPTION_BACK_CY;
		}
		else
		{
			cy = pInfo->m_Height + FOLDER_CAPTION_BACK_CY;

			pInfo->m_Rect.left = x;
			pInfo->m_Rect.top = y + FOLDER_CAPTION_BACK_CY;
			pInfo->m_Rect.right = Rect.Width();
			pInfo->m_Rect.bottom = y + cy;
		}

		pInfo->m_CaptionRect.left = x;
		pInfo->m_CaptionRect.top = y;
		pInfo->m_CaptionRect.right = Rect.Width();
		pInfo->m_CaptionRect.bottom = y + FOLDER_CAPTION_BACK_CY;

		y += cy;
	}

	if((pInfo != nullptr) && (y < Rect.Height()))
	{
		if( (pInfo->m_State & I3TDK_FOLD_STATE_COLLAPSED) == 0)
		{
			pInfo->m_Rect.bottom = Rect.Height();
		}
	}

	RecalcScrollInfo();
}

BEGIN_MESSAGE_MAP(i3TDKWndFoldCtrl, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()



// i3TDKWndFoldCtrl message handlers

int i3TDKWndFoldCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


BOOL i3TDKWndFoldCtrl::OnEraseBkgnd(CDC* pDC)
{
	return 0;
}

void i3TDKWndFoldCtrl::DrawCaption( HDC dc, I3WNDFOLDINFO * pInfo)
{
	CRect rc;
	INT32 div, mod, i, x;
	HBITMAP hbmpBack, hbmpBackLeft, hbmpBackRight, hbmpInd, hbmpClose, hbmpGrip;

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
	{
		hbmpBack =		g_pTDKGlobalRes->m_hbmpCaptionBack;
		hbmpBackLeft =	g_pTDKGlobalRes->m_hbmpCaptionBackLeft;
		hbmpBackRight = g_pTDKGlobalRes->m_hbmpCaptionBackRight;
		hbmpClose =		g_pTDKGlobalRes->m_hbmpCaptionClose;
		hbmpGrip =		g_pTDKGlobalRes->m_hbmpCaptionGrip;

		if( pInfo->m_State & I3TDK_FOLD_STATE_COLLAPSED)
			hbmpInd = g_pTDKGlobalRes->m_hbmpCaptionCollapse;
		else
			hbmpInd = g_pTDKGlobalRes->m_hbmpCaptionExpand;
	}

	// Caption Bar
	rc = pInfo->m_CaptionRect;
	rc.MoveToY( rc.top - m_ScrollPos);

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

	// Indicator Icon
	rc.left += FOLDER_CAPTION_GRIP_CX + 2;
	i3TDK::AlphaBlend( dc, rc.left, rc.top + 4, FOLDER_CAPTION_IND_CX, FOLDER_CAPTION_IND_CY, 
		hbmpInd, 0, 0, FOLDER_CAPTION_IND_CX, FOLDER_CAPTION_IND_CY, true);

	// Close
	i3TDK::BitBlt( dc, rc.right - 4 - FOLDER_CAPTION_CLOSE_CX, rc.top + 3,
		FOLDER_CAPTION_CLOSE_CX, FOLDER_CAPTION_CLOSE_CY, hbmpClose);

	rc.left += FOLDER_CAPTION_IND_CX + 5;
	::DrawText( dc, pInfo->m_strCaption.c_str(), -1, &rc, DT_END_ELLIPSIS | DT_LEFT | DT_SINGLELINE | DT_VCENTER);
}

void i3TDKWndFoldCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	HBRUSH hOldBrush;
	HFONT hOldFont;
	I3WNDFOLDINFO * pInfo;
	CRect rc, ClientRect;

	GetClientRect( &ClientRect);

	hOldBrush = (HBRUSH) ::SelectObject( dc.m_hDC, m_hBrush);
	hOldFont = (HFONT) ::SelectObject( dc.m_hDC, m_hFont);

	SetTextColor( dc.m_hDC, GetSysColor( COLOR_WINDOWTEXT));
	SetBkMode( dc.m_hDC, TRANSPARENT);

	for(size_t i = 0; i < m_WndList.size(); i++)
	{
		pInfo = m_WndList[i];

		DrawCaption( dc.m_hDC, pInfo);
	}

	GetAllRect( &rc);
	if( rc.bottom < ClientRect.bottom)
	{
		rc.left = 0;
		rc.top = rc.bottom;
		rc.right = ClientRect.right;
		rc.bottom = ClientRect.bottom;

		::FillRect( dc.m_hDC, &rc, (HBRUSH) GetSysColorBrush( COLOR_BTNFACE));
	}

	::SelectObject( dc.m_hDC, hOldBrush);
	::SelectObject( dc.m_hDC, hOldFont);
}

void i3TDKWndFoldCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( point.x < FOLDER_CAPTION_IND_CX)
	{
		m_DragMode = DRAG_REORDER;
	}
	else if( point.x < m_Width - FOLDER_CAPTION_GRIP_CX)
	{
		m_DragMode = DRAG_RESIZE;

		// 제일 첫번째 Folder는 Resize가 불가능하다.
		m_DragFoldIndex = FindWndByPos( point);
		if((m_DragFoldIndex < 1) && (m_DragMode == DRAG_RESIZE))
		{
			m_DragMode = DRAG_NONE;
		}
		else
		{
			I3WNDFOLDINFO * pTemp = m_WndList[ m_DragFoldIndex - 1];

			m_DragStartSize.cx = pTemp->m_Width;
			m_DragStartSize.cy = pTemp->m_Height;
		}
	}
	else
	{
		m_DragMode = DRAG_SCROLL;
	}

	SetCapture();

	m_DragStartPt = point;
	m_DragStartScrollPos = m_ScrollPos;

	CWnd::OnLButtonDown(nFlags, point);
}

void i3TDKWndFoldCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_DragMode = DRAG_NONE;

	ReleaseCapture();

	CWnd::OnLButtonUp(nFlags, point);
}

INT32 i3TDKWndFoldCtrl::FindWndByPos( CPoint pt)
{
	I3WNDFOLDINFO * pInfo;
	CRect rect;

	if( m_WndList.size() <= 0)
		return -1;

	for(size_t i = 0; i < m_WndList.size(); i++)
	{
		pInfo = m_WndList[i];

		rect = pInfo->m_CaptionRect;
		rect.MoveToY( rect.top - m_ScrollPos);

		if( PtInRect( &rect, pt))
			return i;
	}

	return -1;
}

void i3TDKWndFoldCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	INT32 idx;
	I3WNDFOLDINFO * pInfo;

	idx = FindWndByPos( point);
	if( idx == -1)
		return;

	pInfo = m_WndList[idx];

	if( pInfo->m_State & I3TDK_FOLD_STATE_COLLAPSED)
		pInfo->m_State &= ~I3TDK_FOLD_STATE_COLLAPSED;
	else
		pInfo->m_State |= I3TDK_FOLD_STATE_COLLAPSED;

	CalcPos();
	RecalcScrollInfo();
	AdjustPanePos();

	Invalidate();

	CWnd::OnLButtonDblClk(nFlags, point);
}

void i3TDKWndFoldCtrl::SetMouseCursor( HCURSOR hCursor)
{
	SetCursor( hCursor);

	SetClassLong( m_hWnd, GCL_HCURSOR, (LONG) hCursor);
}

void i3TDKWndFoldCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	switch( m_DragMode)
	{
		case DRAG_RESIZE :
			{
				I3WNDFOLDINFO * pDrag = m_WndList[ m_DragFoldIndex - 1];

				pDrag->m_Height = m_DragStartSize.cy + (point.y - m_DragStartPt.y);
				if( pDrag->m_Height < 32)
					pDrag->m_Height = 32;

				CalcPos();
				RecalcScrollInfo();
				AdjustPanePos();
			}
			break;

		case DRAG_REORDER :
			break;

		case DRAG_SCROLL :
			{
				SCROLLINFO si;
				
				GetScrollInfo( SB_VERT, &si);

				m_ScrollPos = m_DragStartScrollPos - (point.y - m_DragStartPt.y);
				if( m_ScrollPos < 0)
					m_ScrollPos = 0;

				if((m_ScrollPos + si.nPage) > (UINT)si.nMax)
					m_ScrollPos = si.nMax - si.nPage;
				
				UpdateScrollInfo();
			}
			break;

		default :
			{
				INT32 idx = FindWndByPos( point);

				if( m_bSetTrackMouse ==false)
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

				FocusFold( idx);

				if( idx == -1)
					SetMouseCursor( g_pTDKGlobalRes->m_hCursorArrow);
				else
				{
					// Focus 설정
					if( point.x > (m_Width - FOLDER_CAPTION_GRIP_CX))
					{
						// Grip 위치임 (Drag & Drop)
						SetMouseCursor( g_pTDKGlobalRes->m_hCursorHand);
					}
					else if( point.x > FOLDER_CAPTION_IND_CX)
					{
						// Resize 위치
						SetMouseCursor( g_pTDKGlobalRes->m_hCursorSizeNS);
					}
					else
					{
						SetMouseCursor( g_pTDKGlobalRes->m_hCursorArrow);
					}
				}
			}
			break;
	}


	CWnd::OnMouseMove(nFlags, point);
}

BOOL i3TDKWndFoldCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void i3TDKWndFoldCtrl::FocusFold( INT32 idx)
{
	if( m_OldFocusFold != idx)
	{
		I3WNDFOLDINFO * pTemp;

		if( m_OldFocusFold != -1)
		{
			// Focus 해제
			pTemp = m_WndList[m_OldFocusFold];

			pTemp->m_State &= ~I3TDK_FOLD_STATE_FOCUSED;

			InvalidateRect( &pTemp->m_CaptionRect);
		}

		m_OldFocusFold = idx;

		if( idx != -1)
		{
			pTemp = m_WndList[idx];

			pTemp->m_State |= I3TDK_FOLD_STATE_FOCUSED;

			InvalidateRect( &pTemp->m_CaptionRect);
		}
	}
}

void i3TDKWndFoldCtrl::GetAllRect( RECT * pRect)
{
	I3WNDFOLDINFO * pInfo;

	pRect->left = 0;
	pRect->right = 0;
	pRect->top = 0;

	if( m_WndList.size() <= 0)
	{
		pRect->bottom = FOLDER_CAPTION_BACK_CY;
		return;
	}

	pInfo = m_WndList[ m_WndList.size() - 1];

	if( pInfo->m_State & I3TDK_FOLD_STATE_COLLAPSED)
		pRect->bottom = pInfo->m_CaptionRect.bottom;
	else
		pRect->bottom = pInfo->m_Rect.bottom;
}

void i3TDKWndFoldCtrl::GetCaptionRect( INT32 idx, RECT * pRect)
{
	I3WNDFOLDINFO * pInfo = m_WndList[idx];

	*pRect  = pInfo->m_CaptionRect;
}

void i3TDKWndFoldCtrl::RecalcScrollInfo(void)
{
	CRect Rect, ClientRect;
	SCROLLINFO si;

	GetAllRect( &Rect);
	GetClientRect( &ClientRect);

	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_RANGE;
	si.nPage = ClientRect.Height();;
	si.nMin = 0;
	si.nMax = MAX( ClientRect.Height(), Rect.Height());

	SetScrollInfo( SB_VERT, &si);
}

void i3TDKWndFoldCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CRect Rect, ClientRect;
	SCROLLINFO si;
	bool bFlag = false;
	INT32 iPos = (INT32) nPos;

	si.cbSize = sizeof(si);
	GetScrollInfo( SB_VERT, &si);

	switch( nSBCode)
	{
		case SB_TOP :
			iPos = 0;
			bFlag = true;
			break;

		case SB_BOTTOM :
			iPos = si.nMax - si.nPage;
			bFlag = true;
			break;

		case SB_ENDSCROLL :
			break;

		case SB_LINEDOWN :
			iPos = si.nPos + 1;
			if( (iPos + si.nPage) > (UINT)si.nMax)
				iPos = si.nMax - si.nPage;
			bFlag = true;
			break;

		case SB_LINEUP :
			iPos --;
			if( iPos < 0)
				iPos = 0;
			bFlag = true;
			break;

		case SB_PAGEDOWN :
			iPos += si.nPage;
			if( (iPos + si.nPage) > (UINT)si.nMax)
				iPos = si.nMax - si.nPage;
			bFlag = true;
			break;

		case SB_PAGEUP :
			iPos -= si.nPage;
			if( iPos < 0)
				iPos = 0;
			bFlag = true;
			break;

		case SB_THUMBPOSITION :
			bFlag = true;
			break;

		case SB_THUMBTRACK :
			bFlag = true;
			break;
	}

	if( bFlag)
	{
		m_ScrollPos = iPos;

		UpdateScrollInfo();
	}

	CWnd::OnVScroll(nSBCode, iPos, pScrollBar);
}

void i3TDKWndFoldCtrl::UpdateScrollInfo(void)
{
	SetScrollPos( SB_VERT, m_ScrollPos);

	AdjustPanePos();
}

void i3TDKWndFoldCtrl::AdjustPanePos(void)
{

	I3WNDFOLDINFO * pInfo;

	for(size_t i = 0; i < m_WndList.size(); i++)
	{
		pInfo = m_WndList[i];

		if( pInfo->m_State & I3TDK_FOLD_STATE_COLLAPSED)
		{
			pInfo->m_pWnd->ShowWindow( SW_HIDE);
		}
		else
		{
			pInfo->m_pWnd->SetWindowPos( nullptr, pInfo->m_Rect.left, 
							pInfo->m_Rect.top - m_ScrollPos, 
							pInfo->m_Rect.Width(), pInfo->m_Rect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
			pInfo->m_pWnd->ShowWindow( SW_SHOW);
		}
	}

	Invalidate();
}

void i3TDKWndFoldCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	m_Width = cx;
	m_Height = cy;

	CalcPos();
	RecalcScrollInfo();
	AdjustPanePos();
}

void i3TDKWndFoldCtrl::OnMouseLeave(void)
{
	m_bSetTrackMouse = false;

	FocusFold( -1);
}

LRESULT i3TDKWndFoldCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message)
	{
		case WM_MOUSELEAVE :
			OnMouseLeave();
			break;
	}

	return CWnd::WindowProc(message, wParam, lParam);
}
