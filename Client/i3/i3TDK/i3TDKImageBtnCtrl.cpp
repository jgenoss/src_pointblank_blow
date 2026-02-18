// ImageButton.cpp : implementation file
//
#include "stdafx.h"
#include "i3TDKImageBtnCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// i3TDKImageBtnCtrl

i3TDKImageBtnCtrl::i3TDKImageBtnCtrl()
{
	Image.m_hObject = nullptr;
}

void i3TDKImageBtnCtrl::DrawButtonAsUp( HDC dc, LPRECT Rect)
{
	HPEN WhitePen, DkPen, BlackPen, OldPen;
	int Width, Height;
	int x, y;
	HDC  MemDC;
	HBITMAP OldBitmap;

	FillRect( dc, Rect, (HBRUSH) GetSysColorBrush( COLOR_BTNFACE));

	if( Image.m_hObject != nullptr) {

		Width = Rect->right - Rect->left + 1;
		Height = Rect->bottom - Rect->top + 1;

		x = (Width >> 1) - (ImageSize.cx >> 1) - 1;
		y = (Height >> 1) - (ImageSize.cy >> 1) - 1;

		MemDC = CreateCompatibleDC( nullptr);
		OldBitmap = (HBITMAP) SelectObject( MemDC, Image.m_hObject);

		::BitBlt( dc, x, y, ImageSize.cx, ImageSize.cy, MemDC, 0, 0, SRCCOPY);

		SelectObject( MemDC, OldBitmap);
		DeleteDC( MemDC);
	}

	WhitePen = CreatePen( PS_SOLID, 1, RGB( 255, 255, 255));
	ASSERT( WhitePen != nullptr);

	DkPen = CreatePen( PS_SOLID, 1, RGB( 128, 128, 128));
	ASSERT( DkPen != nullptr);

	BlackPen = CreatePen( PS_SOLID, 1, RGB( 0, 0, 0));
	ASSERT( BlackPen != nullptr);

	OldPen = (HPEN) SelectObject( dc, WhitePen);

	MoveToEx( dc, Rect->left, Rect->bottom - 1, nullptr);
	LineTo( dc, Rect->left, Rect->top);
	LineTo( dc, Rect->right - 1, Rect->top);

	SelectObject( dc, BlackPen);
	LineTo( dc, Rect->right - 1, Rect->bottom - 1);
	LineTo( dc, Rect->left, Rect->bottom - 1);

	SelectObject( dc, DkPen);
	MoveToEx( dc, Rect->left + 1, Rect->bottom - 2, nullptr);
	LineTo( dc, Rect->right - 2, Rect->bottom - 2);
	LineTo( dc, Rect->right - 2, Rect->top + 1);

	SelectObject( dc, OldPen);
	DeleteObject( WhitePen);
	DeleteObject( DkPen);
	DeleteObject( BlackPen);
}

void i3TDKImageBtnCtrl::DrawButtonAsDown( HDC dc, LPRECT Rect)
{
	HPEN WhitePen, DkPen, BlackPen, OldPen;
	int Width, Height;
	int x, y;
	HDC  MemDC;
	HBITMAP OldBitmap;

	FillRect( dc, Rect, (HBRUSH) GetSysColorBrush( COLOR_BTNFACE));

	if( Image.m_hObject != nullptr) {

		Width = Rect->right - Rect->left + 1;
		Height = Rect->bottom - Rect->top + 1;

		x = (Width >> 1) - (ImageSize.cx >> 1) - 1;
		y = (Height >> 1) - (ImageSize.cy >> 1) - 1;

		MemDC = CreateCompatibleDC( nullptr);
		OldBitmap = (HBITMAP) SelectObject( MemDC, Image.m_hObject);

		::BitBlt( dc, x + 1, y + 1, ImageSize.cx, ImageSize.cy, MemDC, 0, 0, SRCCOPY);

		SelectObject( MemDC, OldBitmap);
		DeleteDC( MemDC);
	}

	WhitePen = CreatePen( PS_SOLID, 1, RGB( 255, 255, 255));
	ASSERT( WhitePen != nullptr);

	DkPen = CreatePen( PS_SOLID, 1, RGB( 128, 128, 128));
	ASSERT( DkPen != nullptr);

	BlackPen = CreatePen( PS_SOLID, 1, RGB( 0, 0, 0));
	ASSERT( BlackPen != nullptr);

	OldPen = (HPEN) SelectObject( dc, BlackPen);

	MoveToEx( dc, Rect->left, Rect->bottom - 1, nullptr);
	LineTo( dc, Rect->left, Rect->top);
	LineTo( dc, Rect->right - 1, Rect->top);

	SelectObject( dc, WhitePen);
	LineTo( dc, Rect->right - 1, Rect->bottom - 1);
	LineTo( dc, Rect->left, Rect->bottom - 1);

	SelectObject( dc, DkPen);
	MoveToEx( dc, Rect->left + 1, Rect->bottom - 2, nullptr);
	LineTo( dc, Rect->left + 1, Rect->top + 1);
	LineTo( dc, Rect->right - 2, Rect->top + 1);

	SelectObject( dc, OldPen);
	DeleteObject( WhitePen);
	DeleteObject( DkPen);
	DeleteObject( BlackPen);
}

void i3TDKImageBtnCtrl::DrawItem( LPDRAWITEMSTRUCT lpDraw)
{
	RECT Rect;

	GetClientRect( &Rect);

	if( lpDraw->itemState & ODS_SELECTED) {
		DrawButtonAsDown( lpDraw->hDC, &Rect);
	}
	else {
		DrawButtonAsUp( lpDraw->hDC, &Rect);
	}
}

BOOL i3TDKImageBtnCtrl::LoadBitmap( UINT BitmapID, int CX, int CY)
{
	BOOL Rv;

	ImageSize.cx = 0;
	ImageSize.cy = 0;

	Rv = Image.LoadBitmap( BitmapID);
	if( Rv == FALSE)
		return FALSE;

	ImageSize.cx = CX;
	ImageSize.cy = CY;

	return TRUE;
}

BEGIN_MESSAGE_MAP(i3TDKImageBtnCtrl, CButton)
	//{{AFX_MSG_MAP(i3TDKImageBtnCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// i3TDKImageBtnCtrl message handlers

void i3TDKImageBtnCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( m_bTimeEvent == TRUE)
		SetTimer( 100, 200, nullptr);
	
	CButton::OnLButtonDown(nFlags, point);
}

void i3TDKImageBtnCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( m_bTimeEvent == TRUE)
		KillTimer( 100);
	
	CButton::OnLButtonUp(nFlags, point);
}

void i3TDKImageBtnCtrl::OnTimer(UINT nIDEvent) 
{
	POINT Pt;
	RECT Rect;
	WORD ID, Code;

	if( nIDEvent == 100) {
		GetCursorPos( &Pt);

		GetWindowRect( &Rect);

		if( PtInRect( &Rect, Pt)) {
			ID = (WORD) GetWindowLong( m_hWnd, GWL_ID);
			Code = BN_CLICKED;

			::PostMessage( GetParent()->m_hWnd, WM_COMMAND, MAKELONG( ID, Code), (LPARAM)m_hWnd);
		}
	}

	CButton::OnTimer(nIDEvent);
}
