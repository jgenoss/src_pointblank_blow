// i3InterfaceColor.h

#ifndef __I3_COLOR_H
#define __I3_COLOR_H

#include "../i3Base/i3CommonType.h"
#include "i3MathUtil.h"

struct I3COLOR
{
	UINT8	r, g, b, a;

	I3COLOR(void) : r(0), g(0), b(0), a(0) {}
	I3COLOR( UINT8 _r, UINT8 _g, UINT8 _b, UINT8 _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	UINT32	GetARGB(void) const
	{
		return  ((a << 24) & 0xFF000000) |
				((r << 16) & 0x00FF0000) |
				((g << 8)  & 0x0000FF00) |
				((b)       & 0x000000FF);
	}

	UINT32	GetRGBA(void) const
	{
		return  ((a << 24) & 0xFF000000) |
				((b << 16) & 0x00FF0000) |
				((g <<  8) & 0x0000FF00) |
				((r      ) & 0x000000FF);
	}

	void	SetARGB( UINT32 val)
	{
		a = (UINT8)((val >> 24) & 0xFF);
		r = (UINT8)((val >> 16) & 0xFF);
		g = (UINT8)((val >> 8)  & 0xFF);
		b = (UINT8)((val)		& 0xFF);
	}

	void	SetRGBA( UINT32 val)
	{
		a = (UINT8)((val >> 24) & 0xFF);
		b = (UINT8)((val >> 16) & 0xFF);
		g = (UINT8)((val >> 8)  & 0xFF);
		r = (UINT8)((val)		& 0xFF);
	}

	I3COLOR & operator = (const I3COLOR & color)
	{
		a = color.a;
		b = color.b;
		g = color.g;
		r = color.r;

		return *this;
	}
};

union I3UNIONCOLOR
{
	struct
	{
		UINT8 r, g, b, a;
	} rgba;

	UINT32 value;

};

typedef I3COLOR					COLOR;

struct COLORREAL
{
	REAL32 r, g, b, a;

	COLORREAL(void) : r(0.f), g(0.f), b(0.f), a(0.f) {}

	COLORREAL( REAL32 _r, REAL32 _g, REAL32 _b, REAL32 _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	COLORREAL & operator = (const COLORREAL & p)
	{
		r = p.r;
		g = p.g;
		b = p.b;
		a = p.a;

		return *this;
	}

	bool operator < (const COLORREAL & p) const
	{
		if( i3Math::isAlike( r, p.r))
		{
			if( i3Math::isAlike( g, p.g))
			{
				if( i3Math::isAlike( b, p.b))
				{
					if( i3Math::isAlike( a, p.a))
						return false;
					else if( a > p.a)
						return false;
				}
				else if( b > p.b)
					return false;
			}
			else if( g > p.g)
			{
				return false;
			}
		}
		else if( r > p.r)
		{
			return false;
		}
		
		return true;
	}

	bool operator == (const COLORREAL & p) const
	{
		return i3Math::isAlike( r, p.r) && i3Math::isAlike( g, p.g) && i3Math::isAlike( b, p.b) && i3Math::isAlike( a, p.a);
	}

	bool operator != (const COLORREAL & p) const
	{
		return ! (*this == p);
	}

	bool operator > (const COLORREAL & p) const
	{
		return !( (*this) < p || (*this)==p );
	}

	bool operator <= (const COLORREAL & p) const
	{
		return ((*this) < p || (*this == p));
	}

	bool operator >= (const COLORREAL & p) const
	{
		return !((*this) < p);
	}
} VECTORALIGN;

namespace i3Color
{
	inline
	void Set( COLOR * pDest, const COLOR * pSrc)
	{
		*pDest = *pSrc;
	}
	inline void Set(i3::pack::COLORREAL * pDest, const COLOR * pSrc)
	{
		pDest->r = pSrc->r * 0.003921568627451f;
		pDest->g = pSrc->g * 0.003921568627451f;
		pDest->b = pSrc->b * 0.003921568627451f;
		pDest->a = pSrc->a * 0.003921568627451f;
	}

	inline
	void Set( COLOR* color, UINT8 r, UINT8 g, UINT8 b, UINT8 a)
	{
		color->r = r;
		color->g = g;
		color->b = b;
		color->a = a;
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
		pColor->a = (UINT8)(pColorReal->a * 255.0f);
		pColor->r = (UINT8)(pColorReal->r * 255.0f);
		pColor->g = (UINT8)(pColorReal->g * 255.0f);
		pColor->b = (UINT8)(pColorReal->b * 255.0f);
	}

	inline
	void	Set( COLORREAL * pColorReal, COLOR * pColor)
	{
		pColorReal->a  = (REAL32)(pColor->a * 0.003921568627451f);
		pColorReal->r  = (REAL32)(pColor->r * 0.003921568627451f);
		pColorReal->g  = (REAL32)(pColor->g * 0.003921568627451f);
		pColorReal->b  = (REAL32)(pColor->b * 0.003921568627451f);
	}

	inline void	Set( COLORREAL * pDest, COLORREAL * pSrc)
	{
		pDest->a = pSrc->a;
		pDest->r = pSrc->r;
		pDest->g = pSrc->g;
		pDest->b = pSrc->b;
	}
	inline void Set(i3::pack::COLORREAL * pDest, const COLORREAL * pSrc)
	{
		pDest->a = pSrc->a;
		pDest->r = pSrc->r;
		pDest->g = pSrc->g;
		pDest->b = pSrc->b;
	}

	inline
	void	Set( COLORREAL * pDest, REAL32 r, REAL32 g, REAL32 b, REAL32 a)
	{
		pDest->a = a;
		pDest->r = r;
		pDest->g = g;
		pDest->b = b;
	}

	inline
	void	Set( COLORREAL * pDest, UINT8 r, UINT8 g, UINT8 b, UINT8 a)
	{
		pDest->a = (REAL32) (a * 0.003921568627451f);
		pDest->r = (REAL32) (r * 0.003921568627451f);
		pDest->g = (REAL32) (g * 0.003921568627451f);
		pDest->b = (REAL32) (b * 0.003921568627451f);
	}

	inline
	void	SetR( COLOR * pColor, UINT8 r)
	{
		pColor->r = r;
	}

	inline
	void	SetG( COLOR * pColor, UINT8 g)
	{
		pColor->g = g;
	}

	inline
	void	SetB( COLOR * pColor, UINT8 b)
	{
		pColor->b = b;
	}

	inline
	void	SetA( COLOR * pColor, UINT8 a)
	{
		pColor->a = a;
	}

	inline
	void	SetR( COLORREAL * pColor, REAL32 r)
	{
		pColor->r = r;
	}

	inline
	void	SetG( COLORREAL * pColor, REAL32 g)
	{
		pColor->g = g;
	}

	inline
	void	SetB( COLORREAL * pColor, REAL32 b)
	{
		pColor->b = b;
	}

	inline
	void	SetA( COLORREAL * pColor, REAL32 a)
	{
		pColor->a = a;
	}

	inline
	REAL32 GetR( COLORREAL * color)
	{
		return color->r;
	}

	inline
	REAL32 GetG( COLORREAL * color)
	{
		return color->g;
	}

	inline
	REAL32 GetB( COLORREAL * color)
	{
		return color->b;
	}

	inline
	REAL32 GetA( COLORREAL * color)
	{
		return color->a;
	}

	inline
	UINT8 GetR( COLOR * color)
	{
		return color->r;
	}

	inline
	UINT8 GetG( COLOR * color)
	{
		return color->g;
	}

	inline
	UINT8 GetB( COLOR * color)
	{
		return color->b;
	}

	inline
	UINT8 GetA( COLOR * color)
	{
		return color->a;
	}

	inline
	void	Copy( COLOR * pOut, const COLOR * pC)
	{
		*pOut = *pC;
	}

	inline
	bool IsSame( COLORREAL * pColor1, COLORREAL * pColor2)
	{
		return ((pColor1->a == pColor2->a) &&
				(pColor1->r == pColor2->r) &&
				(pColor1->g == pColor2->g) &&
				(pColor1->b == pColor2->b));
	}

	inline
	bool IsAlike( COLORREAL * pColor1, COLORREAL * pColor2)
	{
		return (	i3Math::isAlike( pColor1->r, pColor2->r) &&
					i3Math::isAlike( pColor1->g, pColor2->g) &&
					i3Math::isAlike( pColor1->b, pColor2->b) &&
					i3Math::isAlike( pColor1->a, pColor2->a));
	}

	inline
	bool IsSame( COLOR * pColor1, COLOR * pColor2)
	{
			return ((pColor1->a == pColor2->a) &&
					(pColor1->r == pColor2->r) &&
					(pColor1->g == pColor2->g) &&
					(pColor1->b == pColor2->b));
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
		return	(((pColor->r >> 3) & 0x1F) << 10) |
				(((pColor->g >> 3) & 0x1F) << 5) |
				((pColor->b >> 3) & 0x1F);
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

		return 	(((pColor->r >> 3) & 0x1F) << 10) |
				(((pColor->g >> 3) & 0x1F) << 5) |
				((pColor->b >> 3) & 0x1F) | a;
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

		return 	(((pColor->r >> 3) & 0x1F) << 11) |
				(((pColor->g >> 3) & 0x1F) << 6) |
				(((pColor->b >> 3) & 0x1F) << 1) | a;
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
		return	((pColor->r) << 16) |
				((pColor->g) << 8) |
				(pColor->b) |
				0xFF000000;
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
		return	((pColor->r) << 24) |
				((pColor->g) << 16) |
				((pColor->b) << 8) |
				0x000000FF;
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
		return	((pColor->a) << 24) |
				((pColor->r) << 16) |
				((pColor->g) << 8) |
				((pColor->b));
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
		return	((pColor->a) << 24) |
				((pColor->b) << 16) |
				((pColor->g) << 8) |
				((pColor->r));
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
