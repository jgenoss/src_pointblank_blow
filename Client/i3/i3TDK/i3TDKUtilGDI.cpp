#include "stdafx.h"
#include "i3TDKUtilGDI.h"

static HDC		s_hMemDC = nullptr;

I3_EXPORT_TDK
HBRUSH i3TDK::CreateHalftoneBrush(void)
{
	WORD grayPattern[8];
	HBITMAP grayBitmap;
	HBRUSH Brush;

	for(int i = 0; i < 8; i++)
		grayPattern[i] = (WORD)(0x5555 << (i & 1));

	grayBitmap = CreateBitmap(8, 8, 1, 1, &grayPattern);
	//ASSERT( grayBitmap != nullptr);

	Brush = ::CreatePatternBrush( grayBitmap);
	DeleteObject( grayBitmap);

	return Brush;
}

I3_EXPORT_TDK
HPEN i3TDK::CreateHalftonePen(int Width, COLORREF Color)
{
	LOGBRUSH logBrush;
	DWORD Style[2] = {1, 3};

	logBrush.lbStyle = BS_SOLID;
	logBrush.lbColor = Color;
	logBrush.lbHatch = 0;

	return ExtCreatePen( PS_USERSTYLE | PS_GEOMETRIC, Width, &logBrush, 2, Style);
}

I3_EXPORT_TDK
HFONT i3TDK::CreateDefFont( INT32 height, const TCHAR * pszName, INT32 weight, INT32 quality)
{
	LOGFONT log;
	HFONT hFont;

	memset( &log, 0, sizeof(LOGFONT));

	log.lfHeight = height;
	log.lfCharSet = DEFAULT_CHARSET;
	log.lfOutPrecision = OUT_DEFAULT_PRECIS;
	log.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	log.lfQuality  = (BYTE) quality;
	log.lfPitchAndFamily = DEFAULT_PITCH;
	log.lfWeight = weight;
	i3::safe_string_copy( log.lfFaceName, pszName, _countof(log.lfFaceName) );

	hFont = CreateFontIndirect( &log);
	ASSERT( hFont);

	return hFont;
}

I3_EXPORT_TDK
void i3TDK::BitBlt( HDC destDC, INT32 x, INT32 y, INT32 w, INT32 h, HBITMAP hBitmap, INT32 sx, INT32 sy, DWORD mode)
{
	HBITMAP hOld; 

	if( s_hMemDC == nullptr)
	{
		HDC dc = ::GetDC( 0);

		s_hMemDC = CreateCompatibleDC( dc);
		I3ASSERT( s_hMemDC != nullptr);

		::ReleaseDC( 0, dc);
	}

	hOld = (HBITMAP) SelectObject( s_hMemDC, hBitmap);

	::BitBlt( destDC, x, y, w, h, s_hMemDC, sx, sy, mode);

	::SelectObject( s_hMemDC, hOld);
}

I3_EXPORT_TDK	
void i3TDK::StretchBlt( HDC destDC, INT32 x, INT32 y, INT32 w, INT32 h, HBITMAP hBitmap, INT32 sx, INT32 sy, INT32 cx, INT32 cy, DWORD mode)
{
	HBITMAP hOld; 

	if( s_hMemDC == nullptr)
	{
		HDC dc = ::GetDC( 0);

		s_hMemDC = CreateCompatibleDC( dc);
		I3ASSERT( s_hMemDC != nullptr);

		::ReleaseDC( 0, dc);
	}

	hOld = (HBITMAP) SelectObject( s_hMemDC, hBitmap);

	::StretchBlt( destDC, x, y, w, h, s_hMemDC, sx, sy, cx, cy, mode);

	::SelectObject( s_hMemDC, hOld);
}

I3_EXPORT_TDK
void i3TDK::AlphaBlend( HDC destDC, INT32 x, INT32 y, INT32 w, INT32 h, 
						HBITMAP hBitmap, INT32 sx, INT32 sy, INT32 cx, INT32 cy, 
						bool bAlphaChn, UINT8 FixedAlpha)
{
	HBITMAP hOld; 
	BLENDFUNCTION blendop;

	if( s_hMemDC == nullptr)
	{
		HDC dc = ::GetDC( 0);

		s_hMemDC = CreateCompatibleDC( dc);
		I3ASSERT( s_hMemDC != nullptr);

		::ReleaseDC( 0, dc);
	}

	hOld = (HBITMAP) SelectObject( s_hMemDC, hBitmap);

	blendop.BlendOp = AC_SRC_OVER;
	blendop.BlendFlags = 0;
	blendop.SourceConstantAlpha = FixedAlpha;
	if( bAlphaChn)
		blendop.AlphaFormat = AC_SRC_ALPHA;
	else
		blendop.AlphaFormat = 0;

	::AlphaBlend( destDC, x, y, w, h, s_hMemDC, sx, sy, cx, cy, blendop);

	::SelectObject( s_hMemDC, hOld);
}

I3_EXPORT_TDK
void i3TDK::FillBlt( HDC destDC, INT32 x, INT32 y, INT32 w, INT32 h, 
						HBITMAP hBitmap, INT32 sx, INT32 sy, INT32 cx, INT32 cy, DWORD mode)
{
	HBITMAP hOld;
	INT32 i, j, basex;
	INT32 divx, divy;

	if( s_hMemDC == nullptr)
	{
		HDC dc = ::GetDC( 0);

		s_hMemDC = CreateCompatibleDC( dc);
		I3ASSERT( s_hMemDC != nullptr);

		::ReleaseDC( 0, dc);
	}

	hOld = (HBITMAP) SelectObject( s_hMemDC, hBitmap);

	divx = (w + cx - 1) / cx;
	divy = (h + cy - 1) / cy;

	basex = x;

	for( i = 0; i < divy; i++)
	{
		x = basex;

		for( j = 0; j < divx; j++)
		{
			::BitBlt( destDC, x, y, cx, cy, s_hMemDC, sx, sy, mode);

			x += cx;
		}

		y += cy;
	}

	::SelectObject( s_hMemDC, hOld);
}

I3_EXPORT_TDK
HBRUSH i3TDK::CreateBitmapBrush( HBITMAP hBitmap)
{
	LOGBRUSH log;

	log.lbStyle = BS_PATTERN;
	log.lbHatch = (LONG) hBitmap;

	return CreateBrushIndirect( &log);
}



I3_EXPORT_TDK	
void i3TDK::DrawButton( HDC dc, RECT * pRect, HDC tempDC, I3TDK_BMPINFO * pLeft, I3TDK_BMPINFO * pBody, I3TDK_BMPINFO * pRight)
{
	CRect bodyRect;
	INT32 x;

	bodyRect = *pRect;

	// Left
	if( pLeft != nullptr)
	{
		bodyRect.left += pLeft->m_Width;

		::SelectObject( tempDC, pLeft->m_hBitmap);
		::BitBlt( dc,pRect->left, pRect->top, pLeft->m_Width, pLeft->m_Height, tempDC, 0, 0, SRCCOPY);
	}

	// Right
	if( pRight != nullptr)
	{
		bodyRect.right -= pRight->m_Width;
	}

	if( bodyRect.left < 0)
		bodyRect.left = 0;

	// Body
	for( x = bodyRect.left; x < bodyRect.right; x += pBody->m_Width)
	{
		::SelectObject( tempDC, pBody->m_hBitmap);
		::BitBlt( dc, x, pRect->top, pBody->m_Width, pBody->m_Height, tempDC, 0, 0, SRCCOPY);
	}
	
	if( pRight != nullptr)
	{
		::SelectObject( tempDC, pRight->m_hBitmap);
		::BitBlt( dc, pRect->right - pRight->m_Width, pRect->top, pRight->m_Width, pRight->m_Height, tempDC, 0, 0, SRCCOPY);
	}
}

I3_EXPORT_TDK
void i3TDK::DumpGDIObjects(void)
{
	DWORD dwObjectType;
	DWORD hGdi;
	INT32	counts[GDI_OBJ_LAST];
	static const char *	s_GDIObjNames[GDI_OBJ_LAST + 1] =
	{
		"Unknown(0)",
		"Pen",
		"Brush",
		"DC",
		"MetaDC",
		"Palette",
		"Font",
		"Bitmap",
		"Region",
		"MetaFile",
		"MemDC",
		"ExtPen",
		"Enhanced MetaDC",
		"Enhanced MetaFile",
		"Color Space"
	};

	i3mem::FillZero( counts, sizeof(counts));

	for (hGdi = 0; hGdi < 0xFFFFFFFF; hGdi++)
	{
		dwObjectType = ::GetObjectType((HANDLE)hGdi);

		if( dwObjectType != 0)
		{
			I3ASSERT( dwObjectType <= GDI_OBJ_LAST);

			counts[ dwObjectType - 1] ++;
		}
	}

	INT32 i;

	for( i = 1; i <= GDI_OBJ_LAST; i++)
	{
		I3TRACE( "%s : %d\n", s_GDIObjNames[i], counts[i - 1]);
	}

	I3TRACE( "-------------------\n");
}
