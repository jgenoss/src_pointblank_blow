// i3InterfaceColor.h

#ifndef __I3_COLOR_H
#define __I3_COLOR_H

#include "../i3Base/i3CommonType.h"
#include "i3MathUtil.h"

namespace i3Color
{
	inline
	void Set( COLOR * pDest, COLOR * pSrc)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX)
			pDest->a = pSrc->a;
			pDest->r = pSrc->r;
			pDest->g = pSrc->g;
			pDest->b = pSrc->b;
		#elif defined( I3G_PSP) || defined( I3G_OGLES)
			*pDest = *pSrc;
		#endif
	}

	inline
	void Set( COLOR* color, UINT8 r, UINT8 g, UINT8 b, UINT8 a)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			color->r = r;
			color->g = g;
			color->b = b;
			color->a = a;
		#endif
	}

	inline
	void	Set( COLOR * pColor, REAL32 r, REAL32 g, REAL32 b, REAL32 a)
	{
		pColor->a = (UINT8)(a * 255.0f);
		pColor->r = (UINT8)(r * 255.0f);
		pColor->g = (UINT8)(g * 255.0f);
		pColor->b = (UINT8)(b * 255.0f);
	}

	inline
	void	Set( COLOR * pColor, COLORREAL * pColorReal)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
		pColor->a = (UINT8)(pColorReal->a * 255.0f);
		pColor->r = (UINT8)(pColorReal->r * 255.0f);
		pColor->g = (UINT8)(pColorReal->g * 255.0f);
		pColor->b = (UINT8)(pColorReal->b * 255.0f);
		#endif
	}

	inline
	void	Set( COLORREAL * pColorReal, COLOR * pColor)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
		pColorReal->a  = (REAL32)(pColor->a / 255.0f);
		pColorReal->r  = (REAL32)(pColor->r / 255.0f);
		pColorReal->g  = (REAL32)(pColor->g / 255.0f);
		pColorReal->b  = (REAL32)(pColor->b / 255.0f);
		#endif
	}

	inline
	void	Set( COLORREAL * pDest, COLORREAL * pSrc)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			pDest->a = pSrc->a;
			pDest->r = pSrc->r;
			pDest->g = pSrc->g;
			pDest->b = pSrc->b;
		#endif
	}

	inline
	void	Set( COLORREAL * pDest, REAL32 r, REAL32 g, REAL32 b, REAL32 a)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			pDest->a = a;
			pDest->r = r;
			pDest->g = g;
			pDest->b = b;
		#endif
	}

	inline
	void	Set( COLORREAL * pDest, UINT8 r, UINT8 g, UINT8 b, UINT8 a)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			pDest->a = (REAL32) (a / 255.0f);
			pDest->r = (REAL32) (r / 255.0f);
			pDest->g = (REAL32) (g / 255.0f);
			pDest->b = (REAL32) (b / 255.0f);
		#endif
	}

	inline
	void	SetR( COLOR * pColor, UINT8 r)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			pColor->r = r;
		#endif
	}

	inline
	void	SetG( COLOR * pColor, UINT8 g)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			pColor->g = g;
		#endif
	}

	inline
	void	SetB( COLOR * pColor, UINT8 b)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			pColor->b = b;
		#endif
	}

	inline
	void	SetA( COLOR * pColor, UINT8 a)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			pColor->a = a;
		#endif
	}

	inline
	void	SetR( COLORREAL * pColor, REAL32 r)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			pColor->r = r;
		#endif
	}

	inline
	void	SetG( COLORREAL * pColor, REAL32 g)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			pColor->g = g;
		#endif
	}

	inline
	void	SetB( COLORREAL * pColor, REAL32 b)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			pColor->b = b;
		#endif
	}

	inline
	void	SetA( COLORREAL * pColor, REAL32 a)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			pColor->a = a;
		#endif
	}

	inline
	REAL32 GetR( COLORREAL * color)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			return color->r;
		#endif
	}

	inline
	REAL32 GetG( COLORREAL * color)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			return color->g;
		#endif
	}

	inline
	REAL32 GetB( COLORREAL * color)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			return color->b;
		#endif
	}

	inline
	REAL32 GetA( COLORREAL * color)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			return color->a;
		#endif
	}

	inline
	UINT8 GetR( COLOR * color)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			return color->r;
		#endif
	}

	inline
	UINT8 GetG( COLOR * color)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			return color->g;
		#endif
	}

	inline
	UINT8 GetB( COLOR * color)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			return color->b;
		#endif
	}

	inline
	UINT8 GetA( COLOR * color)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			return color->a;
		#endif
	}

	inline
	BOOL IsSame( COLORREAL * pColor1, COLORREAL * pColor2)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			return ((pColor1->a == pColor2->a) &&
					(pColor1->r == pColor2->r) &&
					(pColor1->g == pColor2->g) &&
					(pColor1->b == pColor2->b));
		#endif
	}

	inline
	BOOL IsAlike( COLORREAL * pColor1, COLORREAL * pColor2)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
		return (	i3Math::isAlike( pColor1->r, pColor2->r) &&
					i3Math::isAlike( pColor1->g, pColor2->g) &&
					i3Math::isAlike( pColor1->b, pColor2->b) &&
					i3Math::isAlike( pColor1->a, pColor2->a));
		#endif
	}

	inline
	BOOL IsSame( COLOR * pColor1, COLOR * pColor2)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			return ((pColor1->a == pColor2->a) &&
					(pColor1->r == pColor2->r) &&
					(pColor1->g == pColor2->g) &&
					(pColor1->b == pColor2->b));
		#endif
	}

	inline
	void		Lerp( COLORREAL * pOut, COLORREAL * pC1, COLORREAL * pC2, REAL32 t)
	{
		pOut->r = (pC1->r + (pC2->r - pC1->r) * t);
		pOut->g = (pC1->g + (pC2->g - pC1->g) * t);
		pOut->b = (pC1->b + (pC2->b - pC1->b) * t);
		pOut->a = (pC1->a + (pC2->a - pC1->a) * t);
	}

	inline
	UINT16		ConvertRGB15( COLOR * pColor)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			return	(((pColor->r >> 3) & 0x1F) << 10) |
					(((pColor->g >> 3) & 0x1F) << 5) |
					((pColor->b >> 3) & 0x1F);
		#endif
	}

	inline
	void		ConvertRGB15( UINT16 val, COLOR * pColor)
	{
		i3Color::Set( pColor,	(UINT8) (((val >> 10) & 0x1F) << 3),
								(UINT8) (((val >> 5) & 0x1F) << 3),
								(UINT8)	((val & 0x1F) << 3),
								255);
	}

	inline
	UINT16		ConvertARGB16( COLOR * pColor)
	{
		UINT16 a;

		if(pColor->a == 0)
			a = 0;
		else
			a = 0x8000;

		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			return 	(((pColor->r >> 3) & 0x1F) << 10) |
					(((pColor->g >> 3) & 0x1F) << 5) |
					((pColor->b >> 3) & 0x1F) | a;
		#endif
	}

	inline
	void		ConvertARGB16( UINT16 val, COLOR * pColor)
	{
		UINT8 a;

		if( val & 0x8000)
			a = 0xFF;
		else
			a = 0;

		i3Color::Set( pColor,	(UINT8) (((val >> 10) & 0x1F) << 3),
								(UINT8) (((val >> 5) & 0x1F) << 3),
								(UINT8) ((val & 0x1F) << 3),
								a);
	}

	inline
	UINT16		ConvertRGBA16( COLOR * pColor)
	{
		UINT16 a;

		if(pColor->a == 0)
			a = 0;
		else
			a = 1;

		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			return 	(((pColor->r >> 3) & 0x1F) << 11) |
					(((pColor->g >> 3) & 0x1F) << 6) |
					(((pColor->b >> 3) & 0x1F) << 1) | a;
		#endif
	}

	inline
	void		ConvertRGBA16( UINT16 val, COLOR * pColor)
	{
		UINT8 a;

		if( val & 1)
			a = 0xFF;
		else
			a = 0;

		i3Color::Set( pColor,	(UINT8) (((val >> 11) & 0x1F) << 3),
								(UINT8) (((val >> 6) & 0x1F) << 3),
								(UINT8) (((val >> 1) & 0x1F) << 3),
								a);
	}

	inline
	UINT32		ConvertXRGB32( COLOR * pColor)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			return	((pColor->r) << 16) |
					((pColor->g) << 8) |
					(pColor->b) |
					0xFF000000;
		#endif
	}

	inline
	void		ConvertXRGB32( UINT32 val, COLOR * pColor)
	{
		i3Color::Set( pColor,	(UINT8) ((val >> 16) & 0xFF),
								(UINT8) ((val >> 8) & 0xFF),
								(UINT8) ((val) & 0xFF),
								(UINT8) 0xFF);
	}

	inline
	UINT32		ConvertRGBX32( COLOR * pColor)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			return	((pColor->r) << 24) |
					((pColor->g) << 16) |
					((pColor->b) << 8) |
					0x000000FF;
		#endif
	}

	inline
	void		ConvertRGBX32( UINT32 val, COLOR * pColor)
	{
		i3Color::Set( pColor,	(UINT8) ((val >> 24) & 0xFF),
								(UINT8) ((val >> 16) & 0xFF),
								(UINT8) ((val >> 8) & 0xFF),
								(UINT8) 0xFF);
	}

	inline
	UINT32		ConvertARGB32( COLOR * pColor)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			return	((pColor->a) << 24) |
					((pColor->r) << 16) |
					((pColor->g) << 8) |
					((pColor->b));
		#endif
	}

	inline
	void		ConvertARGB32( UINT32 val, COLOR * pColor)
	{
		i3Color::Set( pColor,	(UINT8) ((val >> 16) & 0xFF),
								(UINT8) ((val >> 8) & 0xFF),
								(UINT8) ((val) & 0xFF),
								(UINT8) ((val >> 24) & 0xFF));
	}

	inline
	UINT32		ConvertRGBA32( COLOR * pColor)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3G_PSP) || defined( I3G_OGLES) || defined( I3_WINCE )
			return	((pColor->a) << 24) |
					((pColor->b) << 16) |
					((pColor->g) << 8) |
					((pColor->r));
		#endif
	}
	
	inline
	void		ConvertRGBA32( UINT32 val, COLOR * pColor)
	{
		i3Color::Set( pColor,	(UINT8)((val) & 0xFF),
								(UINT8)((val >> 8) & 0xFF),
								(UINT8)((val >> 16) & 0xFF),
								(UINT8)((val >> 24 ) & 0xFF));
	}

	inline
	void		Add( COLORREAL * pOut, COLORREAL * pIn1, COLORREAL * pIn2)
	{
		pOut->r = pIn1->r + pIn2->r;
		pOut->g = pIn1->g + pIn2->g;
		pOut->b = pIn1->b + pIn2->b;
		pOut->a = pIn1->a + pIn2->a;
	}

	inline
	void		Add( COLORREAL * pOut, REAL32 r, REAL32 g, REAL32 b, REAL32 a)
	{
		pOut->r += r;
		pOut->g += g;
		pOut->b += b;
		pOut->a += a;
	}

	inline
	void		Sub( COLORREAL * pOut, COLORREAL * pIn1, COLORREAL * pIn2)
	{
		pOut->r = pIn1->r - pIn2->r;
		pOut->g = pIn1->g - pIn2->g;
		pOut->b = pIn1->b - pIn2->b;
		pOut->a = pIn1->a - pIn2->a;
	}

	inline
	void		Scale( COLORREAL * pOut, COLORREAL * pIn1, REAL32 scale)
	{
		pOut->r = pIn1->r * scale;
		pOut->g = pIn1->g * scale;
		pOut->b = pIn1->b * scale;
		pOut->a = pIn1->a * scale;
	}


	inline
	void		Mul( COLORREAL * pOut, COLORREAL * pIn1, COLORREAL * pIn2)
	{
		pOut->r = pIn1->r * pIn2->r;
		pOut->g = pIn1->g * pIn2->g;
		pOut->b = pIn1->b * pIn2->b;
		pOut->a = pIn1->a * pIn2->a;
	}

	#if defined( I3_DEBUG)
	inline
	void		Dump( const char * pszPrefix, COLORREAL * pColor)
	{
		I3TRACE( "%s : R(%.2f) G(%.2f) B(%.2f) A(%.2f)\n", pszPrefix, 
			GetR( pColor), GetG( pColor), GetB( pColor), GetA( pColor));
	}

	inline
	void		Dump( const char * pszPrefix, COLOR * pColor)
	{
		I3TRACE( "%s : R(%d) G(%d) B(%d) A(%d)\n", pszPrefix, 
			GetR( pColor), GetG( pColor), GetB( pColor), GetA( pColor));
	}
	#endif

	I3_EXPORT_MATH	void	SetToXML( i3XMLElement * pXML, const char * pszName, COLOR * pColor);
	I3_EXPORT_MATH	void	SetToXML( i3XMLElement * pXML, const char * pszName, COLORREAL * pColor);
	I3_EXPORT_MATH	void	GetFromXML( i3XMLElement * pXML, const char * pszName, COLOR * pColor);
	I3_EXPORT_MATH	void	GetFromXML( i3XMLElement * pXML, const char * pszName, COLORREAL * pColor);
};

#endif
/////////////////////////////////////////////////////////////
