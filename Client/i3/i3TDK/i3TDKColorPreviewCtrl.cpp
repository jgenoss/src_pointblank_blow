// i3TDKColorPreviewCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKColorPreviewCtrl.h"

// i3TDKColorPreviewCtrl

IMPLEMENT_DYNAMIC(i3TDKColorPreviewCtrl, CWnd)

i3TDKColorPreviewCtrl::~i3TDKColorPreviewCtrl()
{
	if( m_hDrawDC != nullptr)
	{
		::DeleteDC( m_hDrawDC);
		m_hDrawDC = nullptr;
	}

	if( m_hDrawBitmap != nullptr)
	{
		::DeleteObject( m_hDrawBitmap);
		m_hDrawBitmap = nullptr;
	}
}

void i3TDKColorPreviewCtrl::setColor( COLORREAL * pColor)
{
	COLOR c;
	REAL32	alpha, back;
	INT32 i, j;
	UINT32 * pBits, data;

	c.r = (UINT8) i3TDK::intColor( pColor->r);
	c.g = (UINT8) i3TDK::intColor( pColor->g);
	c.b = (UINT8) i3TDK::intColor( pColor->b);
	c.a = (UINT8) i3TDK::intColor( pColor->a);

	alpha = pColor->a;

	pBits = m_pBits;

	for( i = 0; i < m_Height; i++)
	{
		for( j = 0; j < m_Width; j++, pBits++)
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

	Invalidate();
}


void i3TDKColorPreviewCtrl::_ResizeDrawBitmap( INT32 cx, INT32 cy)
{
	if( m_hDrawBitmap != nullptr)
	{
		::DeleteObject( m_hDrawBitmap);
	}

	m_Width = cx;
	m_Height = cy;

	BITMAPINFO bmi;

	memset( &bmi, 0, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = cx;
	bmi.bmiHeader.biHeight = cy;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;

	m_hDrawBitmap = CreateDIBSection( m_hDrawDC, &bmi, DIB_RGB_COLORS, (void **) &m_pBits, nullptr, 0);
	I3ASSERT( m_hDrawBitmap != nullptr);
}


BEGIN_MESSAGE_MAP(i3TDKColorPreviewCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL i3TDKColorPreviewCtrl::Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT id)
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


// i3TDKColorPreviewCtrl message handlers


int i3TDKColorPreviewCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		HDC dc = ::GetDC( m_hWnd);

		m_hDrawDC = CreateCompatibleDC( dc);

		::ReleaseDC( m_hWnd, dc);
	}

	return 0;
}

void i3TDKColorPreviewCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	_ResizeDrawBitmap( cx, cy);
}

void i3TDKColorPreviewCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	HBITMAP hOld;

	hOld = (HBITMAP) ::SelectObject( m_hDrawDC, m_hDrawBitmap);
	::StretchBlt( dc.m_hDC, 0, 0, m_Width, m_Height, m_hDrawDC, 0, 0, m_Width, m_Height, SRCCOPY);
	::SelectObject( m_hDrawDC, hOld);
}
