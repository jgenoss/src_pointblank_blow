#if !defined( __I3_FIXED_POINT_COLOR_H)
#define __I3_FIXED_POINT_COLOR_H

#include "i3Base.h"
#include "i3Color.h"

namespace i3Color
{
	inline
	void Copy( FIXED32_COLORREAL * pDest, COLORREAL * pSrc)
	{
		pDest->r = i3FXD::FromReal( i3Color::GetR( pSrc));
		pDest->g = i3FXD::FromReal( i3Color::GetG( pSrc));
		pDest->b = i3FXD::FromReal( i3Color::GetB( pSrc));
		pDest->a = i3FXD::FromReal( i3Color::GetA( pSrc));
	}

	inline
	void Copy( FIXED32_COLORREAL * pDest, COLOR * pSrc)
	{
		pDest->r = (i3Color::GetR( pSrc) << 8) + 255;
		pDest->g = (i3Color::GetG( pSrc) << 8) + 255;
		pDest->b = (i3Color::GetB( pSrc) << 8) + 255;
		pDest->a = (i3Color::GetA( pSrc) << 8) + 255;
	}

	inline
	void Copy( COLORREAL * pDest, FIXED32_COLORREAL * pSrc)
	{
		i3Color::Set( pDest,	i3FXD::ToReal( pSrc->r),
								i3FXD::ToReal( pSrc->g),
								i3FXD::ToReal( pSrc->b),
								i3FXD::ToReal( pSrc->a));
	}

	inline
	void Copy( COLOR * pDest, FIXED32_COLORREAL * pSrc)
	{
		i3Color::Set( pDest,	(UINT8) (pSrc->r >> 8),
								(UINT8) (pSrc->g >> 8),
								(UINT8) (pSrc->b >> 8),
								(UINT8) (pSrc->a >> 8));
	}

	inline
	void Copy( COLOR * pDest, COLOR * pSrc)
	{
		i3Color::Set( pDest,	i3Color::GetR(pSrc),
								i3Color::GetG(pSrc),
								i3Color::GetB(pSrc),
								i3Color::GetA(pSrc));
	}
};

#endif

