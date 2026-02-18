#if !defined( __I3_FIXED_POINT_QUAT_H)
#define __I3_FIXED_POINT_QUAT_H

#include "i3Quat.h"

namespace i3Quat
{
	inline
	void	Copy( FIXED32_QUAT * pDest, FIXED32_QUAT * pSrc)
	{
		pDest->x = pSrc->x;
		pDest->y = pSrc->y;
		pDest->z = pSrc->z;
		pDest->w = pSrc->w;
	}

	inline
	void	Copy( FIXED32_QUAT * pDest, QUATERNION * pSrc)
	{
		pDest->x = i3FXD::FromReal( i3Quat::GetA( pSrc));
		pDest->y = i3FXD::FromReal( i3Quat::GetB( pSrc));
		pDest->z = i3FXD::FromReal( i3Quat::GetC( pSrc));
		pDest->w = i3FXD::FromReal( i3Quat::GetD( pSrc));
	}

	inline
	void	Copy( QUATERNION * pDest, FIXED32_QUAT * pSrc)
	{
		i3Quat::Set( pDest, i3FXD::ToReal(pSrc->x), i3FXD::ToReal(pSrc->y), 
							i3FXD::ToReal(pSrc->z), i3FXD::ToReal(pSrc->w));
	}

	inline
	FIXED32	GetA( FIXED32_QUAT * pQuat)
	{
		return pQuat->x;
	}

	inline
	FIXED32 GetB( FIXED32_QUAT * pQuat)
	{
		return pQuat->y;
	}

	inline
	FIXED32 GetC( FIXED32_QUAT * pQuat)
	{
		return pQuat->z;
	}

	inline
	FIXED32 GetD( FIXED32_QUAT * pQuat)
	{
		return pQuat->w;
	}

	inline
	void	Identity( FIXED32_QUAT * pQuat )
	{
		pQuat->x = 0;
		pQuat->y = 0;
		pQuat->z = 0;
		pQuat->w = 65536;
	}

	inline
	void	Set( FIXED32_QUAT * pQuat, FIXED32 a, FIXED32 b, FIXED32 c, FIXED32 d)
	{
		pQuat->x = a;
		pQuat->y = b;
		pQuat->z = c;
		pQuat->w = d;
	}

	inline
	void Normalize( FIXED32_QUAT * pOut, FIXED32_QUAT * pIn)
	{
		FIXED32 len;

		len =	i3FXD::Mul( pIn->x, pIn->x) + 
				i3FXD::Mul( pIn->y, pIn->y) + 
				i3FXD::Mul( pIn->z, pIn->z) + 
				i3FXD::Mul( pIn->w, pIn->w);

		if( len > 0)
			len = i3FXD::Div( 65536, i3Math::Sqrt( len));
		else
			len = 65536;

		pOut->x = i3FXD::Mul( pIn->x, len);
		pOut->y = i3FXD::Mul( pIn->y, len);
		pOut->z = i3FXD::Mul( pIn->z, len);
		pOut->w = i3FXD::Mul( pIn->w, len);
	}

	inline
	void	Lerp( FIXED32_QUAT * pOut, FIXED32_QUAT * pIn1, FIXED32_QUAT * pIn2, FIXED32 t)
	{
		Set( pOut,	GetA(pIn1) + i3FXD::Mul((GetA(pIn2) - GetA(pIn1)), t),
					GetB(pIn1) + i3FXD::Mul((GetB(pIn2) - GetB(pIn1)), t),
					GetC(pIn1) + i3FXD::Mul((GetC(pIn2) - GetC(pIn1)), t),
					GetD(pIn1) + i3FXD::Mul((GetD(pIn2) - GetD(pIn1)), t));
	}

	void Slerp( FIXED32_QUAT * pOut, FIXED32_QUAT * pIn1, FIXED32_QUAT * pIn2, FIXED32 t);
};

#endif
