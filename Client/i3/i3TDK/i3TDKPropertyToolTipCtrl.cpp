// PropertyToolTip.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKPropertyToolTipCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// i3TDKPropertyToolTipCtrl

BOOL i3TDKPropertyToolTipCtrl::Create( CWnd * pParentWnd, UINT Style)
{
	BOOL Rv;
	RECT Rect;
	LOGFONT logFont;

	Rect.left = Rect.top = 0;
	Rect.bottom = Rect.right = 0;

	Rv = CToolTipCtrl::Create( pParentWnd, Style | TTS_ALWAYSTIP);
	ASSERT( Rv != 0);

	memset( &logFont, 0, sizeof(LOGFONT));

	logFont.lfHeight			= 12;
	logFont.lfWeight			= FW_DONTCARE;
	logFont.lfItalic			= FALSE;
	logFont.lfCharSet			= DEFAULT_CHARSET;
	logFont.lfOutPrecision		= OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
	logFont.lfQuality			= DEFAULT_QUALITY;
	logFont.lfPitchAndFamily	= DEFAULT_PITCH;
	i3::generic_string_copy( logFont.lfFaceName, _T("±¼¸²") );

	SetTextFont( &logFont);

	return Rv;
}

void i3TDKPropertyToolTipCtrl::SetTextFont( const LOGFONT *logFont)
{
	Font.CreateFontIndirect( logFont);
}

void i3TDKPropertyToolTipCtrl::SetText( CWnd *pParent, const TCHAR* Buf)
{
	const TCHAR *LineStart, *LineEnd;
	HDC dc;
	HFONT OldFont;
	SIZE Sz;

	DelTool( pParent, (UINT)pParent->m_hWnd);

	if( Buf != nullptr)
		i3::generic_string_copy( m_Buffer, Buf);
	else
		m_Buffer[0] = 0;

	pOwner = pParent;

	dc = ::GetDC( m_hWnd);

	OldFont = (HFONT) SelectObject( dc, Font.m_hObject);

	LineStart = LineEnd = m_Buffer;
	Width = 0;
	Height = 0;

	while( *LineEnd) {
		switch( *LineEnd) {
			case '\r' :
				GetTextExtentPoint32( dc, LineStart, LineEnd - LineStart, &Sz);

				Width = max( Width, Sz.cx);
				Height += Sz.cy + 2;

				LineStart = LineEnd + 1;

				while((*LineStart != 0) && (*LineStart == '\n')) 
					LineStart++;

				LineEnd = LineStart;
				break;

			default :
				LineEnd++;
				break;
		}
	}

	if( LineEnd > LineStart) {
		GetTextExtentPoint32( dc, LineStart, LineEnd - LineStart, &Sz);

		Width = max( Width, Sz.cx);
		Height += Sz.cy + 2;
	}

	if( Width != 0)
		Width += 6;

	if( Height != 0)
		Height += 6;

	SelectObject( dc, OldFont);
	::ReleaseDC( m_hWnd, dc);
}

void i3TDKPropertyToolTipCtrl::Show( int x, int y)
{
	POINT MousePt;
	int ScreenCX, ScreenCY;
	RECT Rect;
	CToolInfo Info;

	if((Width == 0) || (Height == 0))
		return;

	GetCursorPos( &MousePt);

	pOwner->ScreenToClient( &MousePt);

	ScreenCY = GetSystemMetrics( SM_CYFULLSCREEN);
	ScreenCX = GetSystemMetrics( SM_CXFULLSCREEN);
	
	if( x == -1) {
		x = MousePt.x + 20;
	}

	if( y == -1)
		y = MousePt.y + 20;

	if( x + Width > ScreenCX) 
		x -= (x + Width) - ScreenCX;

	if( y + Height > ScreenCY)
		y -= (y + Height) - ScreenCY;

	Rect.left = x; Rect.top = y;
	Rect.right = x + Width - 1;
	Rect.bottom = y + Height - 1;

	memset( &Info, 0, sizeof(TOOLINFO));
	Info.cbSize = sizeof(TOOLINFO);
	Info.hwnd = pOwner->m_hWnd;
	Info.uFlags = TTF_IDISHWND;
	Info.uId = (UINT) pOwner->m_hWnd;
	if( GetToolInfo( Info, pOwner, (UINT) pOwner->m_hWnd) == FALSE) {
		ASSERT( AddTool( pOwner, m_Buffer, &Rect, (UINT)pOwner->m_hWnd) != 0);
	}

	SetToolRect( pOwner,(UINT) pOwner->m_hWnd, &Rect);

	Activate(TRUE);
	SendMessage(TTM_TRACKACTIVATE, TRUE, (LPARAM)&Info);

	::SetWindowPos( m_hWnd, HWND_TOP, 0, 0, Width, Height, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOOWNERZORDER);

	Showed = TRUE;
}

void i3TDKPropertyToolTipCtrl::Hide(void)
{
	Activate( FALSE);

	Showed = FALSE;
}

BEGIN_MESSAGE_MAP(i3TDKPropertyToolTipCtrl, CWnd)
	//{{AFX_MSG_MAP(i3TDKPropertyToolTipCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// i3TDKPropertyToolTipCtrl message handlers

void i3TDKPropertyToolTipCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	HDC DC;
	const TCHAR *LineStart, *LineEnd;
	HFONT OldFont;
	SIZE Sz;
	int X, Y;

	DC = dc.m_hDC;

	SetTextColor( DC, TextColor);
	SetBkColor( DC, BackColor);

	OldFont = (HFONT) SelectObject( DC, Font.m_hObject);

	GetTextExtentPoint32( dc, _T("O"), 1, &Sz);
	
	LineStart = LineEnd = m_Buffer;
	X = 3;
	Y = 3;

	while( *LineEnd) {
		switch( *LineEnd) {
			case '\r' :
				TextOut( dc, X, Y, LineStart, LineEnd - LineStart);

				LineStart = LineEnd + 1;
				Y += Sz.cy + 2;

				while((*LineStart != 0) && (*LineStart == '\n')) 
					LineStart++;

				LineEnd = LineStart;
				break;

			default :
				LineEnd++;
				break;
		}
	}

	if( LineEnd > LineStart) {
		TextOut( dc, X, Y, LineStart, LineEnd - LineStart);
	}
	
	SelectObject( DC, OldFont);
}

BOOL i3TDKPropertyToolTipCtrl::OnEraseBkgnd(CDC* pDC) 
{
	HBRUSH Brush;
	RECT Rect;
	HDC DC;

	DC = ::GetDC( m_hWnd);

	Brush = CreateSolidBrush( BackColor);

	GetClientRect( &Rect);

	FillRect( DC, &Rect, Brush);
	
	DeleteObject( Brush);

	::ReleaseDC( m_hWnd, DC);

	return 1;
}


LRESULT i3TDKPropertyToolTipCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{/*
	switch( message) {
	}*/

	return CWnd::WindowProc(message, wParam, lParam);
}
