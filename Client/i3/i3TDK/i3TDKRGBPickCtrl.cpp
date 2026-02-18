// i3TDKRGBPickCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKRGBPickCtrl.h"
#include "i3TDKGlobalVariable.h"
#include "resource.h"

// i3TDKRGBPickCtrl

IMPLEMENT_DYNAMIC(i3TDKRGBPickCtrl, CWnd)

i3TDKRGBPickCtrl::i3TDKRGBPickCtrl()
{
	i3mem::FillZero( &m_BMPInfo, sizeof(m_BMPInfo));
}

i3TDKRGBPickCtrl::~i3TDKRGBPickCtrl()
{
	if( m_hBackDC != nullptr)
	{
		::DeleteDC( m_hBackDC);
		m_hBackDC = nullptr;
	}

	if( m_hDrawBitmap != nullptr)
	{
		::DeleteObject( m_hDrawBitmap);
		m_hDrawBitmap = nullptr;
	}
}

void i3TDKRGBPickCtrl::_CreateBitmap(void)
{
	BITMAPINFO bmi;

	HDC dc = ::GetDC( m_hWnd);

	m_hBackDC = CreateCompatibleDC( dc);

	::ReleaseDC( m_hWnd, dc);

	memset( &bmi, 0, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = 256;
	bmi.bmiHeader.biHeight = 256;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;

	m_hDrawBitmap = CreateDIBSection( m_hBackDC, &bmi, DIB_RGB_COLORS, (void **) &m_pBits, nullptr, 0);
	I3ASSERT( m_hDrawBitmap != nullptr);
}

void i3TDKRGBPickCtrl::_BuildBitmap(void)
{
	m_PickX = 0;
	m_PickY = 0;

	// 지정 칼라를 그린다.
	{
		INT32 i, j;
		COLORREAL	baseColor, c, diff;
		UINT32 *	pBuf = (UINT32 *) m_pBits;
		REAL32		step = 1.0f / 255.0f;
		REAL32		rdiff, minDiff = 0.0f;
		bool bFirst = true;

		// BaseColor는 1.0 이상의 값이 있을 수 있기 때문에 정규화를 시킨다.
		baseColor.r = i3Math::Clamp( m_BaseColor.r, 0.0f, 1.0f);
		baseColor.g = i3Math::Clamp( m_BaseColor.g, 0.0f, 1.0f);
		baseColor.b = i3Math::Clamp( m_BaseColor.b, 0.0f, 1.0f);
		baseColor.a = i3Math::Clamp( m_BaseColor.a, 0.0f, 1.0f);

		diff.r = 1.0f - baseColor.r;
		diff.g = 1.0f - baseColor.g;
		diff.b = 1.0f - baseColor.b;
		diff.a = 1.0f - baseColor.a;

		for( i = 0; i < 256; i++)
		{
			REAL32 vFactor = i * step;

			for( j = 255; j >= 0; j--)
			{
				REAL32 hFactor = j * step;

				c.r = (m_BaseColor.r + (hFactor * diff.r)) * vFactor;
				c.g = (m_BaseColor.g + (hFactor * diff.g)) * vFactor;
				c.b = (m_BaseColor.b + (hFactor * diff.b)) * vFactor;

				*pBuf	=	((UINT8)(c.b * 255.0f) << 0) | 
							((UINT8)(c.g * 255.0f) << 8) |
							((UINT8)(c.r * 255.0f) << 16) |
							0xFF000000;

				pBuf ++;

				rdiff =	i3Math::abs( c.r - m_Color.r) +
						i3Math::abs( c.g - m_Color.g) + 
						i3Math::abs( c.b - m_Color.b);

				if( bFirst)
				{
					m_PickX = 255 - j;
					m_PickY = 255 - i;
					minDiff = rdiff;
					bFirst = false;
				}
				else if( rdiff < minDiff)
				{
					m_PickX = 255 - j;
					m_PickY = 255 - i;
					minDiff = rdiff;
				}
			}
		}
	}
}

BOOL i3TDKRGBPickCtrl::Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT id)
{
	LPCTSTR lpszClass;
	HCURSOR hCursor;
	HBRUSH hBrush;
	HICON	hIcon;

	hCursor = LoadCursor(  g_hInstTDK, MAKEINTRESOURCE( IDC_TDK_RGBPICK));

	hBrush = (HBRUSH) ::GetStockObject( NULL_BRUSH);
	hIcon	= ::LoadIcon( g_hInstTDK, IDI_APPLICATION);

	lpszClass = AfxRegisterWndClass( CS_HREDRAW | CS_HREDRAW, hCursor, hBrush, hIcon);
	I3ASSERT( lpszClass != nullptr);

	dwStyle &= ~WS_BORDER;

	return CWnd::CreateEx( WS_EX_STATICEDGE, lpszClass, _T(""), dwStyle, rect, pParentWnd, id);
}

void i3TDKRGBPickCtrl::setColor( COLORREAL * pColor)
{
	i3Color::Set( &m_Color, pColor);

	_BuildBitmap();

	Invalidate();
}

void i3TDKRGBPickCtrl::setBaseColor( COLORREAL * pColor)
{
	i3Color::Set( &m_BaseColor, pColor);

	INT32 x, y;

	x = m_PickX;
	y = m_PickY;

	_BuildBitmap();

	_Pick( x, y);
}

void i3TDKRGBPickCtrl::_Pick( INT32 x, INT32 y)
{
	x = MIN( x, 255);
	x = MAX( x, 0);

	y = MIN( y, 255);
	y = MAX( y, 0);

	m_PickX = x;
	m_PickY = y;

	{
		UINT32 * pColor = (UINT32 *) ((m_pBits + ((255 - y) * 256 * 4)) + (x * 4));

		COLOR col;

		i3Color::ConvertARGB32( *pColor, &col);
		i3Color::Set( &m_Color, &col);
	}

	::PostMessage( GetParent()->m_hWnd, WM_TDK_SELECTCOLOR, 0, 0);

	Invalidate();
}

BEGIN_MESSAGE_MAP(i3TDKRGBPickCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



// i3TDKRGBPickCtrl message handlers



int i3TDKRGBPickCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	_CreateBitmap();

	return 0;
}

void i3TDKRGBPickCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	HDC DC = dc.m_hDC;
	
	//::StretchDIBits( DC, 0, 0, 256, 256, 0, 0, 256, 256, m_pBits, &m_BMPInfo, DIB_RGB_COLORS, SRCCOPY);
	{
		HBITMAP hBitmap;

		hBitmap = (HBITMAP) ::SelectObject( m_hBackDC, m_hDrawBitmap);
		::StretchBlt( DC, 0, 0, 256, 256, m_hBackDC, 0, 0, 256, 256, SRCCOPY);
		::SelectObject( m_hBackDC, hBitmap);
	}

	// Select Color
	{
		HPEN hOldPen;
		HBRUSH hOldBrush;
		int rop, x, y;

		x = m_PickX;
		y = m_PickY;

		hOldPen = (HPEN) ::SelectObject( DC, ::GetStockObject( WHITE_PEN));
		hOldBrush = (HBRUSH) ::SelectObject( DC, ::GetStockObject( NULL_BRUSH));
		rop = ::SetROP2( DC, R2_XORPEN);

		::Ellipse( DC, x - 3, y - 3, x + 4, y + 4);

		::SetROP2( DC, rop);
		::SelectObject( DC, hOldPen);
		::SelectObject( DC, hOldBrush);
	}
}

void i3TDKRGBPickCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bDrag = true;

	_Pick( point.x, point.y);

	SetCapture();

	CWnd::OnLButtonDown(nFlags, point);
}

void i3TDKRGBPickCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_bDrag = false;

	CWnd::OnLButtonUp(nFlags, point);
}

void i3TDKRGBPickCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDrag)
	{
		_Pick( point.x, point.y);
	}

	CWnd::OnMouseMove(nFlags, point);
}
