// i3TDKPrimaryColorPickCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKPrimaryColorPickCtrl.h"
#include ".\i3tdkprimarycolorpickctrl.h"


// i3TDKPrimaryColorPickCtrl

IMPLEMENT_DYNAMIC(i3TDKPrimaryColorPickCtrl, CWnd)

i3TDKPrimaryColorPickCtrl::~i3TDKPrimaryColorPickCtrl()
{
	if( m_hPrimaryColorTableBitmap != nullptr)
	{
		::DeleteObject( m_hPrimaryColorTableBitmap);
		m_hPrimaryColorTableBitmap = nullptr;
	}

	if( m_hPickMaskBitmap != nullptr)
	{
		::DeleteObject( m_hPickMaskBitmap);
		m_hPickMaskBitmap = nullptr;
	}

	if( m_hPickBitmap != nullptr)
	{
		::DeleteObject( m_hPickBitmap);
		m_hPickBitmap = nullptr;
	}

	if( m_hDrawDC != nullptr)
	{
		::DeleteDC( m_hDrawDC);
		::DeleteDC( m_hBackDC);
		m_hDrawDC = nullptr;
		m_hBackDC = nullptr;
	}

	if( m_hDrawBitmap != nullptr)
	{
		::DeleteObject( m_hDrawBitmap);
		m_hDrawBitmap = nullptr;
	}
}

INT32		s_RGBTable[][3] =
{
	{	255,	0,		0},				// 0
	{	255,	0,		255},			// 43
	{	0,		0,		255},			// 86
	{	0,		255,	255},			// 129
	{	0,		255,	0},				// 172
	{	255,	255,	0},				// 215
	{	255,	0,		0},				// 258
};

#define	STEP		6
#define	CT_WIDTH	20
#define	CT_HEIGHT	258

#define	PICK_WIDTH	9
#define	PICK_HEIGHT	13

void i3TDKPrimaryColorPickCtrl::setColor( COLORREAL * pColor)
{
	COLOR c;
	INT32	diff, minDiff = 0;
	INT32 i;

	c.r = (UINT8) i3TDK::intColor( pColor->r);
	c.g = (UINT8) i3TDK::intColor( pColor->g);
	c.b = (UINT8) i3TDK::intColor( pColor->b);

	for( i = 0; i < CT_HEIGHT; i++)
	{
		diff =	abs((INT32)c.r - m_Color[i][0]) + 
				abs((INT32)c.g - m_Color[i][1]) +
				abs((INT32)c.b - m_Color[i][2]);

		if( i == 0)
		{
			minDiff = diff;
			m_SelectIndex = i;
		}
		else if( diff < minDiff)
		{
			minDiff = diff;
			m_SelectIndex = i;
		}
	}

	_Draw();
}

void i3TDKPrimaryColorPickCtrl::getPrimaryColor( COLORREAL * pColor)
{
	if( m_SelectIndex == -1)
	{
		i3Color::Set( pColor, 1.0f, 0.0f, 0.0f, 1.0f);
		return;
	}

	pColor->r = m_Color[ m_SelectIndex][0] * (1.0f / 255.0f);
	pColor->g = m_Color[ m_SelectIndex][1] * (1.0f / 255.0f);
	pColor->b = m_Color[ m_SelectIndex][2] * (1.0f / 255.0f);
}

void i3TDKPrimaryColorPickCtrl::_CreatePrimaryColorTableBitmap(void)
{
	UINT32 *	pBits;
	BITMAPINFO bmi;
	INT32 i, r, g, b, j, target;

	memset( &bmi, 0, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = CT_WIDTH;
	bmi.bmiHeader.biHeight = CT_HEIGHT;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;

	m_hPrimaryColorTableBitmap = CreateDIBSection( m_hBackDC, &bmi, DIB_RGB_COLORS, (void **) &pBits, nullptr, 0);
	I3ASSERT( m_hPrimaryColorTableBitmap != nullptr);

	r = s_RGBTable[0][0];
	g = s_RGBTable[0][1];
	b = s_RGBTable[0][2];

	for( i = 0; i < CT_HEIGHT; i++)
	{
		m_Color[i][0] = (UINT8) r;
		m_Color[i][1] = (UINT8) g;
		m_Color[i][2] = (UINT8) b;
		
		for( j = 0; j < CT_WIDTH; j++)
		{
			*pBits =	((r & 0xFF) << 16) |
						((g & 0xFF) << 0) |
						((b & 0xFF) << 8) |
						0x00000000;

			pBits++;
		}

		target = (i / (CT_HEIGHT / 6)) + 1;

		if( r < s_RGBTable[ target][0])
			r += STEP;
		else if( r > s_RGBTable[ target][0])
			r -= STEP;

		r = MIN( r, 255);
		r = MAX( r, 0);

		if( g < s_RGBTable[ target][1])
			g += STEP;
		else if( g > s_RGBTable[ target][1])
			g -= STEP;

		g = MIN( g, 255);
		g = MAX( g, 0);

		if( b < s_RGBTable[ target][2])
			b += STEP;
		else if( b > s_RGBTable[ target][2])
			b -= STEP;

		b = MIN( b, 255);
		b = MAX( b, 0);
	}
}

void i3TDKPrimaryColorPickCtrl::_ResizeDrawBitmap( INT32 cx, INT32 cy)
{
	if( m_hDrawBitmap != nullptr)
	{
		::SelectObject( m_hDrawDC, m_hOldDrawBitmap);
		::DeleteObject( m_hDrawBitmap);
	}

	m_hDrawBitmap = CreateBitmap( cx, cy, 
						GetDeviceCaps( m_hDrawDC, PLANES),
						GetDeviceCaps( m_hDrawDC, BITSPIXEL),
						nullptr);

	::SelectObject( m_hDrawDC, m_hDrawBitmap);
}

void i3TDKPrimaryColorPickCtrl::_getColorTableRect( CRect * pRect)
{
	pRect->left = (m_Width >> 1) - (CT_WIDTH >> 1);
	pRect->top  = (m_Height >> 1) - (CT_HEIGHT >> 1);
	pRect->right = pRect->left + CT_WIDTH;
	pRect->bottom = pRect->top + CT_HEIGHT;
}

void i3TDKPrimaryColorPickCtrl::_Draw(void)
{
	HBRUSH hbrBack;
	HPEN hLitPen, hDarkPen;
	CRect rc;

	hbrBack = g_pTDKGlobalRes->getBrush( I3TDK_COLOR_FORE_3D_FACE);
	hLitPen = g_pTDKGlobalRes->getPen( I3TDK_COLOR_FORE_3D_LIT);
	hDarkPen = g_pTDKGlobalRes->getPen( I3TDK_COLOR_FORE_3D_SHADOW);

	// Draw Background
	rc.SetRect( 0, 0, m_Width, m_Height);
	::FillRect( m_hDrawDC, &rc, hbrBack);

	// Draw Color Table
	{
		_getColorTableRect( &rc);

		// border (lit)
		::SelectObject( m_hDrawDC, hLitPen);
		::MoveToEx( m_hDrawDC, rc.right + 1, rc.top, nullptr);
		::LineTo( m_hDrawDC, rc.right + 1, rc.bottom + 1);
		::LineTo( m_hDrawDC, rc.left, rc.bottom + 1);

		// border (shadow)
		::SelectObject( m_hDrawDC, hDarkPen);
		::MoveToEx( m_hDrawDC, rc.left - 1, rc.bottom + 1, nullptr);
		::LineTo( m_hDrawDC, rc.left - 1, rc.top - 1);
		::LineTo( m_hDrawDC, rc.right, rc.top - 1);

		::SelectObject( m_hBackDC, m_hPrimaryColorTableBitmap);
		::StretchBlt( m_hDrawDC, rc.left, rc.top, CT_WIDTH, CT_HEIGHT, m_hBackDC, 0, 0, CT_WIDTH, CT_HEIGHT, SRCCOPY);
	}

	// Selected Color
	if( m_SelectIndex != -1)
	{
		INT32 x1, x2, y;

		x1 = rc.left - PICK_WIDTH;
		x2 = rc.right + PICK_WIDTH;
		y = rc.top + m_SelectIndex - (PICK_HEIGHT >> 1);

		::SelectObject( m_hBackDC, m_hPickMaskBitmap);
		::BitBlt( m_hDrawDC, x1, y, PICK_WIDTH, PICK_HEIGHT,	m_hBackDC, 0, 0, SRCAND);
		::StretchBlt( m_hDrawDC, x2, y, -PICK_WIDTH, PICK_HEIGHT,	m_hBackDC, 0, 0, PICK_WIDTH, PICK_HEIGHT, SRCAND);

		::SelectObject( m_hBackDC, m_hPickBitmap);
		::BitBlt( m_hDrawDC, x1, y, PICK_WIDTH, PICK_HEIGHT,	m_hBackDC, 0, 0, SRCPAINT);
		::StretchBlt( m_hDrawDC, x2, y, -PICK_WIDTH, PICK_HEIGHT, m_hBackDC, 0, 0, PICK_WIDTH, PICK_HEIGHT, SRCPAINT);
	}
}

void i3TDKPrimaryColorPickCtrl::_Select( INT32 y)
{
	CRect rect;

	_getColorTableRect( &rect);

	m_SelectIndex = y - rect.top;

	if( m_SelectIndex < 0)
		m_SelectIndex = 0;

	if( m_SelectIndex >= CT_HEIGHT)
		m_SelectIndex = CT_HEIGHT - 1;

	_Draw();
	Invalidate();

	::PostMessage( GetParent()->m_hWnd, WM_TDK_UPDATEPRIMARYCOLOR, 0, 0);
}

BEGIN_MESSAGE_MAP(i3TDKPrimaryColorPickCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

BOOL i3TDKPrimaryColorPickCtrl::Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT id)
{
	LPCTSTR lpszClass;
	HCURSOR hCursor;
	HBRUSH hBrush;
	HICON	hIcon;

	hCursor = LoadCursor( nullptr, IDC_ARROW);

	hBrush = (HBRUSH) ::GetStockObject( NULL_BRUSH);
	hIcon	= ::LoadIcon( g_hInstTDK, IDI_APPLICATION);

	lpszClass = AfxRegisterWndClass( CS_HREDRAW | CS_HREDRAW, hCursor, hBrush, hIcon);
	I3ASSERT( lpszClass != nullptr);

	dwStyle &= ~WS_BORDER;

	return CWnd::CreateEx( WS_EX_STATICEDGE, lpszClass, _T(""), dwStyle, rect, pParentWnd, id);
}


// i3TDKPrimaryColorPickCtrl message handlers


int i3TDKPrimaryColorPickCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		HDC dc = ::GetDC( m_hWnd);

		m_hBackDC = CreateCompatibleDC( dc);
		m_hDrawDC = CreateCompatibleDC( dc);

		::ReleaseDC( m_hWnd, dc);

		m_hPickBitmap = ::LoadBitmap( g_hInstTDK, MAKEINTRESOURCE( IDB_BMP_PRIMARYCOLOR_PICK));
		I3ASSERT( m_hPickBitmap != nullptr);

		m_hPickMaskBitmap = ::LoadBitmap( g_hInstTDK, MAKEINTRESOURCE( IDB_BMP_PRIMARYCOLOR_PICK_MASK));
		I3ASSERT( m_hPickMaskBitmap != nullptr);

		m_hOldBackBitmap = (HBITMAP) ::SelectObject( m_hBackDC, m_hPickBitmap);
		m_hOldDrawBitmap = (HBITMAP) ::SelectObject( m_hDrawDC, m_hPickBitmap);

		_CreatePrimaryColorTableBitmap();
	}

	return 0;
}

void i3TDKPrimaryColorPickCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	m_Width = cx;
	m_Height = cy;

	_ResizeDrawBitmap( cx, cy);
	_Draw();
}

void i3TDKPrimaryColorPickCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	::StretchBlt( dc.m_hDC, 0, 0, m_Width, m_Height, m_hDrawDC, 0, 0, m_Width, m_Height, SRCCOPY);
}

void i3TDKPrimaryColorPickCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bDrag = true;

	SetCapture();

	_Select( point.y);

	CWnd::OnLButtonDown(nFlags, point);
}

void i3TDKPrimaryColorPickCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bDrag = false;

	ReleaseCapture();

	CWnd::OnLButtonUp(nFlags, point);
}

void i3TDKPrimaryColorPickCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDrag)
	{
		_Select( point.y);
	}

	CWnd::OnMouseMove(nFlags, point);
}
