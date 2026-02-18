// i3TDKAlphaPickCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKAlphaPickCtrl.h"

// i3TDKAlphaPickCtrl

IMPLEMENT_DYNAMIC(i3TDKAlphaPickCtrl, CWnd)

i3TDKAlphaPickCtrl::~i3TDKAlphaPickCtrl()
{
	if( m_hBackDC != nullptr)
	{
		::DeleteDC( m_hBackDC);
		m_hBackDC = nullptr;
	}

	if( m_hDrawDC != nullptr)
	{
		::DeleteDC( m_hDrawDC);
		m_hDrawDC = nullptr;
	}

	if( m_hAlphaTableBitmap != nullptr)
	{
		::DeleteObject( m_hAlphaTableBitmap);
		m_hAlphaTableBitmap = nullptr;
	}

	if( m_hPickBitmap != nullptr)
	{
		::DeleteObject( m_hPickBitmap);
		m_hPickBitmap = nullptr;
	}

	if( m_hPickMaskBitmap != nullptr)
	{
		::DeleteObject( m_hPickMaskBitmap);
		m_hPickMaskBitmap = nullptr;
	}

	if( m_hDrawBitmap != nullptr)
	{
		::DeleteObject( m_hDrawBitmap);
		m_hDrawBitmap = nullptr;
	}
}

#define	STEP		6
#define	CT_WIDTH	20
#define	CT_HEIGHT	258

#define	PICK_WIDTH	9
#define	PICK_HEIGHT	13

void i3TDKAlphaPickCtrl::setColor( COLORREAL * pColor)
{
	COLOR c;
	REAL32	alpha, back;
	INT32 i, j;
	UINT32 * pBits, data;

	c.r = (UINT8) i3TDK::intColor( pColor->r);
	c.g = (UINT8) i3TDK::intColor( pColor->g);
	c.b = (UINT8) i3TDK::intColor( pColor->b);
	c.a = (UINT8) i3TDK::intColor( pColor->a);

	pBits = m_pBits;

	m_SelectIndex = (255 - c.a);

	for( i = 0; i < CT_HEIGHT; i++)
	{
		alpha = (i * (1.0f / 255.0f));
		if( alpha > 1.0f)
			alpha = 1.0f;

		for( j = 0; j < CT_WIDTH; j++, pBits++)
		{
			back = ((((i >> 3) & 1) + ((j >> 3) & 1) & 1) * 0.5f) + 0.2f;
			back *= (255.0f * (1.0f - alpha));

			data =	(((UINT32)((c.r * alpha) + back) & 0xFF) << 16) | 
					(((UINT32)((c.g * alpha) + back) & 0xFF) << 8) | 
					(((UINT32)((c.b * alpha) + back) & 0xFF) << 0) | 
					0xFF000000;
		
			
			*pBits = data;
		}
	}

	_Draw();

	Invalidate();
}

void i3TDKAlphaPickCtrl::_CreateAlphaTableBitmap(void)
{
	BITMAPINFO bmi;

	memset( &bmi, 0, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = CT_WIDTH;
	bmi.bmiHeader.biHeight = CT_HEIGHT;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;

	m_hAlphaTableBitmap = CreateDIBSection( m_hBackDC, &bmi, DIB_RGB_COLORS, (void **) &m_pBits, nullptr, 0);
	I3ASSERT( m_hAlphaTableBitmap != nullptr);
}

void i3TDKAlphaPickCtrl::_ResizeDrawBitmap( INT32 cx, INT32 cy)
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

void i3TDKAlphaPickCtrl::_getColorTableRect( CRect * pRect)
{
	pRect->left = (m_Width >> 1) - (CT_WIDTH >> 1);
	pRect->top  = (m_Height >> 1) - (CT_HEIGHT >> 1);
	pRect->right = pRect->left + CT_WIDTH;
	pRect->bottom = pRect->top + CT_HEIGHT;
}

void i3TDKAlphaPickCtrl::_Draw(void)
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

		::SelectObject( m_hBackDC, m_hAlphaTableBitmap);
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

void i3TDKAlphaPickCtrl::_Select( INT32 y)
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

	::PostMessage( GetParent()->m_hWnd, WM_TDK_UPDATEALPHA, 0, 0);
}

BEGIN_MESSAGE_MAP(i3TDKAlphaPickCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

BOOL i3TDKAlphaPickCtrl::Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT id)
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


// i3TDKAlphaPickCtrl message handlers


int i3TDKAlphaPickCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

		_CreateAlphaTableBitmap();
	}

	return 0;
}

void i3TDKAlphaPickCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	m_Width = cx;
	m_Height = cy;

	_ResizeDrawBitmap( cx, cy);
	_Draw();
}

void i3TDKAlphaPickCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	::StretchBlt( dc.m_hDC, 0, 0, m_Width, m_Height, m_hDrawDC, 0, 0, m_Width, m_Height, SRCCOPY);
}

void i3TDKAlphaPickCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bDrag = true;

	SetCapture();

	_Select( point.y);

	CWnd::OnLButtonDown(nFlags, point);
}

void i3TDKAlphaPickCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bDrag = false;

	ReleaseCapture();

	CWnd::OnLButtonUp(nFlags, point);
}

void i3TDKAlphaPickCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDrag)
	{
		_Select( point.y);
	}

	CWnd::OnMouseMove(nFlags, point);
}
