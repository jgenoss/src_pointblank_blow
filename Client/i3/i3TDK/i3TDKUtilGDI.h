#if !defined( __I3_TDK_UTIL_GDI_H)
#define __I3_TDK_UTIL_GDI_H

#define DOT_TYPE_RECT		0
#define DOT_TYPE_CIRCLE		1
#define DOT_TYPE_TRIANGLE	2

#define DITHER_MONO			1
#define DITHER_GRAY			2

struct I3TDK_BMPINFO
{
	HBITMAP		m_hBitmap = nullptr;
	INT32		m_Width = 0;
	INT32		m_Height = 0;
};

namespace i3TDK
{
	I3_EXPORT_TDK	HBRUSH		CreateHalftoneBrush(void);
	I3_EXPORT_TDK	HPEN		CreateHalftonePen(int Width, COLORREF Color);
	I3_EXPORT_TDK	HFONT		CreateDefFont( INT32 height, const TCHAR* pszName, INT32 weight = 400, INT32 quality = ANTIALIASED_QUALITY);

	I3_EXPORT_TDK	HBRUSH		CreateBitmapBrush( HBITMAP);
	I3_EXPORT_TDK	void		BitBlt( HDC destDC, INT32 x, INT32 y, INT32 w, INT32 h, HBITMAP hBitmap, INT32 sx = 0, INT32 sy = 0, DWORD mode = SRCCOPY);
	I3_EXPORT_TDK	void		StretchBlt( HDC destDC, INT32 x, INT32 y, INT32 w, INT32 h, HBITMAP hBitmap, INT32 sx, INT32 sy, INT32 cx, INT32 cy, DWORD mode = SRCCOPY);
	I3_EXPORT_TDK	void		FillBlt( HDC destDC, INT32 x, INT32 y, INT32 w, INT32 h, HBITMAP hBitmap, INT32 sx, INT32 sy, INT32 cx, INT32 cy, DWORD mode = SRCCOPY);
	I3_EXPORT_TDK	void		AlphaBlend( HDC destDC, INT32 x, INT32 y, INT32 w, INT32 h, 
									HBITMAP hBitmap, INT32 sx, INT32 sy, INT32 cx, INT32 cy, 
									bool bAlphaChn = true, UINT8 FixedAlpha = 255);

	I3_EXPORT_TDK	void		DrawButton( HDC dc, RECT * pRect, HDC tempDC, I3TDK_BMPINFO * pLeft, I3TDK_BMPINFO * pBody, I3TDK_BMPINFO * pRight);

	I3_EXPORT_TDK	void		DumpGDIObjects(void);

	inline REAL32	normalizeColor( REAL32 f)
	{
		REAL32 rv = i3Math::fmod( f, 1.0f);

		if( rv == 0.0f)
		{
			if( f > 0)
				rv = 1.0f;
		}

		return rv;
	}

	inline UINT32	intColor( REAL32 f, bool bNormalize = true)
	{
		if( bNormalize == false)
			return (UINT32) ( f * 255.0f);

		return (UINT32)(normalizeColor( f) * 255.0f);
	}
};


#endif