#if !defined( __I3_FIXED_POINT_VECTOR_H)
#define __I3_FIXED_POINT_VECTOR_H

#include "i3Vector.h"

namespace i3Vector
{
	//////////////////////////////////////////////////////////////////////////////
	// FIXED32_VEC2D
	inline	FIXED32	GetX( FIXED32_VEC2D * pVec)						{ return pVec->x; }
	inline	FIXED32	GetY( FIXED32_VEC2D * pVec)						{ return pVec->y; }

	inline	FIXED32	GetU( FIXED32_VEC2D * pVec)						{ return pVec->x; }
	inline	FIXED32	GetV( FIXED32_VEC2D * pVec)						{ return pVec->y; }

	inline	void	SetX( FIXED32_VEC2D * pVec, FIXED32 val)		{ pVec->x = val; }
	inline	void	SetY( FIXED32_VEC2D * pVec, FIXED32 val)		{ pVec->y = val; }

	inline	void	SetU( FIXED32_VEC2D * pVec, FIXED32 val)		{ pVec->x = val; }
	inline	void	SetV( FIXED32_VEC2D * pVec, FIXED32 val)		{ pVec->y = val; }

	inline	void	Set( FIXED32_VEC2D * pVec, FIXED32 x, FIXED32 y)
	{
		pVec->x = x;
		pVec->y = y;
	}

	inline	void	Copy( FIXED32_VEC2D * pDest, FIXED32_VEC2D * pIn)
	{
		pDest->x = pIn->x;
		pDest->y = pIn->y;
	}

	inline	void	Copy( FIXED32_VEC2D * pDest, VEC2D * pIn)
	{
		pDest->x = i3FXD::FromReal( i3Vector::GetX( pIn));
		pDest->y = i3FXD::FromReal( i3Vector::GetY( pIn));
	}

	inline	void	Copy( VEC2D * pDest, FIXED32_VEC2D * pSrc)
	{
		i3Vector::Set( pDest, i3FXD::ToReal(pSrc->x), i3FXD::ToReal(pSrc->y) );
	}

	inline	void	Zero( FIXED32_VEC2D * pVec)
	{
		pVec->x = pVec->y = 0;
	}

	inline	void	Add( FIXED32_VEC2D * pOut, FIXED32_VEC2D * v1, FIXED32_VEC2D * v2)
	{
		pOut->x = v1->x + v2->x;
		pOut->y = v1->y + v2->y;
	}

	inline	void	Add( FIXED32_VEC2D * pOut, FIXED32 x, FIXED32 y )
	{
		pOut->x += x;	pOut->y += y;
	}

	inline	void	AddX( FIXED32_VEC2D * pOut, FIXED32 val)				{	pOut->x += val; }

	inline	void	AddY( FIXED32_VEC2D * pOut, FIXED32 val)				{	pOut->y += val; }

	inline	void	Sub( FIXED32_VEC2D * pOut, FIXED32_VEC2D * v1, FIXED32_VEC2D * v2)
	{
		pOut->x = v1->x - v2->x;
		pOut->y = v1->y - v2->y;
	}

	inline	void	Mul( FIXED32_VEC2D * pOut, FIXED32_VEC2D * v1, FIXED32_VEC2D * v2)
	{
		pOut->x = i3Math::Mul( v1->x, v2->x);
		pOut->y = i3Math::Mul( v1->y, v2->y);
	}
	inline void		Mul( FIXED32_VEC2D * pOut, FIXED32 f, FIXED32_VEC2D * pV)
	{
		pOut->x = i3Math::Mul( f, pV->x );
		pOut->y = i3Math::Mul( f, pV->y );
	}

	inline	void	Scale( FIXED32_VEC2D * pOut, FIXED32_VEC2D * pIn, FIXED32 s)
	{
		pOut->x = i3FXD::Mul( pIn->x, s);
		pOut->y = i3FXD::Mul( pIn->y, s);
	}

	inline	FIXED32_VEC2D* ScaleAdd( FIXED32_VEC2D *pOut, const FIXED32_VEC2D *pV, FIXED32 s)
	{
		pOut->x += i3Math::Mul( pV->x, s);
		pOut->y += i3Math::Mul( pV->y, s);

		return pOut;
	}

	inline	FIXED32 Dot( const FIXED32_VEC2D *pV1, const FIXED32_VEC2D *pV2 )
	{
		return	i3FXD::FromReal( (i3FXD::ToReal(pV1->x) * i3FXD::ToReal( pV2->x)) + 
			(i3FXD::ToReal(pV1->y) * i3FXD::ToReal( pV2->y)) );
	}

	// 2D ÁÂÇ¥°è·Î ¸¸µé¾î ÁÜ.
	// Á¤¼öÈ­ÇÑ ÈÄ 0.5f¸¦ »©ÁÜ.
	inline void Make2DCoord( FIXED32_VEC2D *pVec )
	{
		pVec->x = (pVec->x & 0xFFFF0000) - 0x7FFF;
		pVec->y = (pVec->y & 0xFFFF0000) - 0x7FFF;
	}

	//////////////////////////////////////////////////////////////////////////////
	// FIXED32_VEC3D
	inline
	void	Copy( FIXED32_VEC3D * pDest, FIXED32_VEC3D * pSrc)
	{
		pDest->x = pSrc->x;
		pDest->y = pSrc->y;
		pDest->z = pSrc->z;
	}

	inline
	void	Copy( FIXED32_VEC3D * pDest, VEC3D * pSrc)
	{
		pDest->x = i3FXD::FromReal( i3Vector::GetX( pSrc));
		pDest->y = i3FXD::FromReal( i3Vector::GetY( pSrc));
		pDest->z = i3FXD::FromReal( i3Vector::GetZ( pSrc));
	}

	inline
	void	Copy( VEC3D * pDest, FIXED32_VEC3D * pSrc)
	{
		i3Vector::Set( pDest, i3FXD::ToReal(pSrc->x), i3FXD::ToReal(pSrc->y), i3FXD::ToReal(pSrc->z));
	}

	inline	FIXED32	GetX( FIXED32_VEC3D * pVec)						{ return pVec->x; }
	inline	FIXED32	GetY( FIXED32_VEC3D * pVec)						{ return pVec->y; }
	inline	FIXED32	GetZ( FIXED32_VEC3D * pVec)						{ return pVec->z; }

	inline	void	SetX( FIXED32_VEC3D * pVec, FIXED32 val)		{ pVec->x = val; }
	inline	void	SetY( FIXED32_VEC3D * pVec, FIXED32 val)		{ pVec->y = val; }
	inline	void	SetZ( FIXED32_VEC3D * pVec, FIXED32 val)		{ pVec->z = val; }

	inline
	void	Set( FIXED32_VEC3D * pVec, FIXED32 x, FIXED32 y, FIXED32 z)
	{
		pVec->x = x;	pVec->y = y;	pVec->z = z;
	}

	inline
	void	Zero( FIXED32_VEC3D * pVec)
	{
		pVec->x = pVec->y = pVec->z = 0;
	}

	inline
	void	Add( FIXED32_VEC3D * pOut, FIXED32_VEC3D * v1, FIXED32_VEC3D * v2)
	{
		pOut->x = v1->x + v2->x;
		pOut->y = v1->y + v2->y;
		pOut->z = v1->z + v2->z;
	}

	inline
	void	Add( FIXED32_VEC3D * pOut, FIXED32_VEC3D * v1, VEC3D * v2)
	{
		pOut->x = v1->x + i3FXD::FromReal( i3Vector::GetX( v2));
		pOut->y = v1->y + i3FXD::FromReal( i3Vector::GetY( v2));
		pOut->z = v1->z + i3FXD::FromReal( i3Vector::GetZ( v2));
	}

	inline
	void	Add( FIXED32_VEC3D * pOut, FIXED32 x, FIXED32 y, FIXED32 z)
	{
		pOut->x += x;	pOut->y += y;	pOut->z += z;
	}

	inline
	void	AddX( FIXED32_VEC3D * pOut, FIXED32 val)				{	pOut->x += val; }

	inline
	void	AddY( FIXED32_VEC3D * pOut, FIXED32 val)				{	pOut->y += val; }

	inline
	void	AddZ( FIXED32_VEC3D * pOut, FIXED32 val)				{	pOut->z += val; }

	inline
	void	Sub( FIXED32_VEC3D * pOut, FIXED32_VEC3D * v1, FIXED32_VEC3D * v2)
	{
		pOut->x = v1->x - v2->x;
		pOut->y = v1->y - v2->y;
		pOut->z = v1->z - v2->z;
	}

	inline
	void	Mul( FIXED32_VEC3D * pOut, FIXED32_VEC3D * v1, FIXED32_VEC3D * v2)
	{
		pOut->x = i3Math::Mul( v1->x, v2->x);
		pOut->y = i3Math::Mul( v1->y, v2->y);
		pOut->z = i3Math::Mul( v1->z, v2->z);
	}

	inline
	void	Mul( FIXED32_VEC3D * pOut, FIXED32 f, FIXED32_VEC3D * pVec )
	{
		pOut->x = i3Math::Mul( f, pVec->x );
		pOut->y = i3Math::Mul( f, pVec->y );
		pOut->z = i3Math::Mul( f, pVec->z );
	}

	inline
	void	Scale( FIXED32_VEC3D * pOut, FIXED32_VEC3D * pIn, FIXED32 s)
	{
		pOut->x = i3FXD::Mul( pIn->x, s);
		pOut->y = i3FXD::Mul( pIn->y, s);
		pOut->z = i3FXD::Mul( pIn->z, s);
	}

	inline
	FIXED32_VEC3D* ScaleAdd( FIXED32_VEC3D *pOut, const FIXED32_VEC3D *pV, FIXED32 s)
	{
		pOut->x += i3Math::Mul( pV->x, s);
		pOut->y += i3Math::Mul( pV->y, s);
		pOut->z += i3Math::Mul( pV->z, s);

		return pOut;
	}

	inline
	FIXED32 Dot( const FIXED32_VEC3D *pV1, const FIXED32_VEC3D *pV2 )
	{
		return	i3FXD::FromReal( (i3FXD::ToReal(pV1->x) * i3FXD::ToReal( pV2->x)) + 
				(i3FXD::ToReal(pV1->y) * i3FXD::ToReal( pV2->y)) +
				(i3FXD::ToReal(pV1->z) * i3FXD::ToReal( pV2->z)));
	}

	// 2D ÁÂÇ¥¸¦ 3D·Î º¯È¯
	inline
	FIXED32_VEC3D *	Convert( FIXED32_VEC3D * pDest, FIXED32_VEC2D * pSrc, INT32 nMode )
	{
		switch( nMode )
		{
		case CONVERT_MODE_XYZ:
			{
				Set( pDest, GetX( pSrc ), GetY( pSrc ), 0);
			}
			break;
		case CONVERT_MODE_XZY:
			{
				Set( pDest, GetX( pSrc ), 0, GetY( pSrc ) );
			}
			break;
		case CONVERT_MODE_MINUS_X_Z_Y:
			{
				Set( pDest, -GetX( pSrc ), 0, GetY( pSrc ) );
			}
			break;
		default:
			{
				Set( pDest, 0, 0, 0);
				I3ASSERT_0;
			}
			break;
		}

		return pDest;
	}

	// 3D ÁÂÇ¥¸¦ 3D·Î ÁÂÇ¥°è º¯È¯
	inline
	FIXED32_VEC3D *	Convert( FIXED32_VEC3D * pDest, FIXED32_VEC3D * pSrc, INT32 nMode )
	{
		switch( nMode )
		{
		case CONVERT_MODE_XYZ:
			{
				Set( pDest, GetX( pSrc ), GetY( pSrc ), GetZ( pSrc ) );
			}
			break;
		case CONVERT_MODE_XZY:
			{
				Set( pDest, GetX( pSrc ), GetZ( pSrc ), GetY( pSrc ) );
			}
			break;
		case CONVERT_MODE_MINUS_X_Z_Y:
			{
				Set( pDest, -GetX( pSrc ), GetZ( pSrc ), GetY( pSrc ) );
			}
			break;
		default:
			{
				Set( pDest, 0, 0, 0);
				I3ASSERT_0;
			}
			break;
		}

		return pDest;
	}

	// 2D ÁÂÇ¥¸¦ 3D·Î º¯È¯
	inline
	FIXED32_VEC3D *	Convert( FIXED32_VEC3D * pDest, VEC2D * pSrc, INT32 nMode )
	{
		switch( nMode )
		{
		case CONVERT_MODE_XYZ:			Set( pDest, i3FXD::FromReal( GetX( pSrc )), i3FXD::FromReal(GetY( pSrc )), 0);		break;
		case CONVERT_MODE_XZY:			Set( pDest, i3FXD::FromReal( GetX( pSrc )), 0, i3FXD::FromReal( GetY( pSrc )));		break;
		case CONVERT_MODE_MINUS_X_Z_Y:	Set( pDest, -i3FXD::FromReal(GetX( pSrc )), 0, i3FXD::FromReal( GetY( pSrc )));		break;
		default:		
			Set( pDest, 0, 0, 0);
			I3ASSERT_0;
			break;
		}

		return pDest;
	}

	// 3D ÁÂÇ¥¸¦ 3D·Î ÁÂÇ¥°è º¯È¯
	inline
	FIXED32_VEC3D *	Convert( FIXED32_VEC3D * pDest, VEC3D * pSrc, INT32 nMode )
	{
		switch( nMode )
		{
		case CONVERT_MODE_XYZ:
			Set( pDest, i3FXD::FromReal( GetX( pSrc)), i3FXD::FromReal( GetY( pSrc)), i3FXD::FromReal( GetZ( pSrc)) );
			break;
		case CONVERT_MODE_XZY:
			Set( pDest, i3FXD::FromReal( GetX( pSrc)), i3FXD::FromReal( GetZ( pSrc)), i3FXD::FromReal( GetY( pSrc)) );
			break;
		case CONVERT_MODE_MINUS_X_Z_Y:
			Set( pDest, -i3FXD::FromReal( GetX( pSrc)), i3FXD::FromReal( GetZ( pSrc)), i3FXD::FromReal( GetY( pSrc)) );
			break;
		default:
			Set( pDest, 0, 0, 0);
			I3ASSERT_0;
			break;
		}

		return pDest;
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 380.035 ms
	// Vfpu unUse: 417.112 ms
	// Vfpu unUse(functionÀ¸·Î È£Ãâ): 626.474 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Cross( FIXED32_VEC3D *pOut, const FIXED32_VEC3D *pV1, const FIXED32_VEC3D *pV2 )
	{
		REAL32 x1, y1, z1, x2, y2, z2;

		x1 = i3FXD::ToReal( pV1->x);
		y1 = i3FXD::ToReal( pV1->y);
		z1 = i3FXD::ToReal( pV1->z);

		x2 = i3FXD::ToReal( pV2->x);
		y2 = i3FXD::ToReal( pV2->y);
		z2 = i3FXD::ToReal( pV2->z);

		pOut->x = i3FXD::FromReal( (y1 * z2) - (z1 * y2));
		pOut->y = i3FXD::FromReal( (z1 * x2) - (x1 * z2));
		pOut->z = i3FXD::FromReal( (x1 * y2) - (y1 * x2));
	}

	inline
	FIXED32	LengthSq( FIXED32_VEC3D * pVec)
	{
		REAL32 x, y, z;

		x = i3FXD::ToReal( pVec->x);
		y = i3FXD::ToReal( pVec->y);
		z = i3FXD::ToReal( pVec->z);

		return i3FXD::FromReal((x * x) + (y * y) + (z * z));
	}

	inline
	FIXED32	Length( FIXED32_VEC3D * pVec)
	{
		REAL32 x, y, z, len;

		x = i3FXD::ToReal( pVec->x);
		y = i3FXD::ToReal( pVec->y);
		z = i3FXD::ToReal( pVec->z);

		len = (x * x) + (y * y) + (z * z);

		return i3FXD::FromReal( i3Math::Sqrt( len));
	}

	inline
	FIXED32	Normalize( FIXED32_VEC3D * pOut, FIXED32_VEC3D * pV)
	{
		FIXED32 len = Length( pV);
		FIXED32 rlen = i3FXD::Div( 65536, len);

		pOut->x = i3FXD::Mul( pV->x, rlen);
		pOut->y = i3FXD::Mul( pV->y, rlen);
		pOut->z = i3FXD::Mul( pV->z, rlen);

		return len;
	}

	inline
	void Normal( FIXED32_VEC3D *pOut, const FIXED32_VEC3D *pV1, const FIXED32_VEC3D *pV2 , const FIXED32_VEC3D *pV3 )
	{
		FIXED32_VEC3D d1;
		FIXED32_VEC3D d2;
		Sub( &d1, (FIXED32_VEC3D *) pV1, (FIXED32_VEC3D *) pV2);
		Sub( &d2, (FIXED32_VEC3D *) pV3, (FIXED32_VEC3D *) pV2);
		Cross(pOut, &d2, &d1);
		Normalize( pOut, pOut);
	}

	inline
	void	Lerp( FIXED32_VEC3D * pOut, FIXED32_VEC3D * pV1, FIXED32_VEC3D *pV2, FIXED32 s)
	{
		pOut->x = pV1->x + ( i3FXD::Mul( (pV2->x - pV1->x), s));
		pOut->y = pV1->y + ( i3FXD::Mul( (pV2->y - pV1->y), s));
		pOut->z = pV1->z + ( i3FXD::Mul( (pV2->z - pV1->z), s));
	}

	inline
	void	Minimize( FIXED32_VEC3D * pOut, FIXED32_VEC3D * pV1, FIXED32_VEC3D * pV2)
	{
		pOut->x = MIN( pV1->x, pV2->x);
		pOut->y = MIN( pV1->y, pV2->y);
		pOut->z = MIN( pV1->z, pV2->z);
	}

	inline
	void	Maximize( FIXED32_VEC3D * pOut, FIXED32_VEC3D * pV1, FIXED32_VEC3D * pV2)
	{
		pOut->x = MAX( pV1->x, pV2->x);
		pOut->y = MAX( pV1->y, pV2->y);
		pOut->z = MAX( pV1->z, pV2->z);
	}

	inline
	void TransformCoord( FIXED32_VEC3D *pOut, const FIXED32_VEC3D *pV, const FIXED32_MATRIX *pM )
	{
		FIXED32 x, y, z, w;

		x = i3FXD::Mul(pM->m00,pV->x) + i3FXD::Mul(pM->m10,pV->y) + i3FXD::Mul(pM->m20,pV->z) + pM->m30;
		y = i3FXD::Mul(pM->m01,pV->x) + i3FXD::Mul(pM->m11,pV->y) + i3FXD::Mul(pM->m21,pV->z) + pM->m31;
		z = i3FXD::Mul(pM->m02,pV->x) + i3FXD::Mul(pM->m12,pV->y) + i3FXD::Mul(pM->m22,pV->z) + pM->m32;
		w = i3FXD::Mul(pM->m03,pV->x) + i3FXD::Mul(pM->m13,pV->y) + i3FXD::Mul(pM->m23,pV->z) + pM->m33;
		pOut->x = i3FXD::Div( x, w);
		pOut->y = i3FXD::Div( y, w);
		pOut->z = i3FXD::Div( z, w);
	}

	inline
	void TransformNormal( FIXED32_VEC3D *pOut, const FIXED32_VEC3D *pV, const FIXED32_MATRIX *pM )
	{
		FIXED32 x, y, z;

		x= pV->x;	y= pV->y;	z= pV->z;

		pOut->x = i3FXD::Mul(pM->m00,x) + i3FXD::Mul(pM->m10,y) + i3FXD::Mul(pM->m20,z);
		pOut->y = i3FXD::Mul(pM->m01,x) + i3FXD::Mul(pM->m11,y) + i3FXD::Mul(pM->m21,z);
		pOut->z = i3FXD::Mul(pM->m02,x) + i3FXD::Mul(pM->m12,y) + i3FXD::Mul(pM->m22,z);
	}

	inline
	void TransformCoord( VEC3D *pOut, const VEC3D *pV, const FIXED32_MATRIX *pM )
	{
		FIXED32 x, y, z, w;
		FIXED32 vx, vy, vz;

		vx = i3FXD::FromReal( i3Vector::GetX( (VEC3D *)pV));
		vy = i3FXD::FromReal( i3Vector::GetY( (VEC3D *)pV));
		vz = i3FXD::FromReal( i3Vector::GetZ( (VEC3D *)pV));

		x = i3FXD::Mul(pM->m00,vx) + i3FXD::Mul(pM->m10,vy) + i3FXD::Mul(pM->m20,vz) + pM->m30;
		y = i3FXD::Mul(pM->m01,vx) + i3FXD::Mul(pM->m11,vy) + i3FXD::Mul(pM->m21,vz) + pM->m31;
		z = i3FXD::Mul(pM->m02,vx) + i3FXD::Mul(pM->m12,vy) + i3FXD::Mul(pM->m22,vz) + pM->m32;
		w = i3FXD::Mul(pM->m03,vx) + i3FXD::Mul(pM->m13,vy) + i3FXD::Mul(pM->m23,vz) + pM->m33;
		i3Vector::Set( pOut, i3FXD::ToReal( i3FXD::Div( x, w)),
							i3FXD::ToReal( i3FXD::Div( y, w)),
							i3FXD::ToReal( i3FXD::Div( z, w)));
	}

	inline
	void TransformNormal( VEC3D *pOut, const VEC3D *pV, const FIXED32_MATRIX *pM )
	{
		FIXED32 x, y, z;
		FIXED32 vx, vy, vz;

		vx = i3FXD::FromReal( i3Vector::GetX( (VEC3D *)pV));
		vy = i3FXD::FromReal( i3Vector::GetY( (VEC3D *)pV));
		vz = i3FXD::FromReal( i3Vector::GetZ( (VEC3D *)pV));

		x = i3FXD::Mul(pM->m00,vx) + i3FXD::Mul(pM->m10,vy) + i3FXD::Mul(pM->m20,vz);
		y = i3FXD::Mul(pM->m01,vx) + i3FXD::Mul(pM->m11,vy) + i3FXD::Mul(pM->m21,vz);
		z = i3FXD::Mul(pM->m02,vx) + i3FXD::Mul(pM->m12,vy) + i3FXD::Mul(pM->m22,vz);

		i3Vector::Set( pOut, i3FXD::ToReal(x), i3FXD::ToReal(y), i3FXD::ToReal(z));
	}

	// 2D ÁÂÇ¥°è·Î ¸¸µé¾î ÁÜ.
	// Á¤¼öÈ­ÇÑ ÈÄ 0.5f¸¦ »©ÁÜ.
	inline void Make2DCoord( FIXED32_VEC3D *pVec )
	{
		pVec->x = (pVec->x & 0xFFFF0000) - 0x7FFF;
		pVec->y = (pVec->y & 0xFFFF0000) - 0x7FFF;
		pVec->z = (pVec->z & 0xFFFF0000) - 0x7FFF;
	}

	inline
	void Dump( FIXED32_VEC3D * pVec)
	{
		I3TRACE3( "%d, %d, %d\n",	(INT32)(i3FXD::ToReal(pVec->x) * 1000.0f),
									(INT32)(i3FXD::ToReal(pVec->y) * 1000.0f),
									(INT32)(i3FXD::ToReal(pVec->z) * 1000.0f));
	}

	//////////////////////////////////////////////////////////////////////////////
	// FIXED32_VEC4D
	inline	FIXED32	GetX( FIXED32_VEC4D * pVec)						{ return pVec->x; }
	inline	FIXED32	GetY( FIXED32_VEC4D * pVec)						{ return pVec->y; }
	inline	FIXED32	GetZ( FIXED32_VEC4D * pVec)						{ return pVec->z; }
	inline	FIXED32	GetW( FIXED32_VEC4D * pVec)						{ return pVec->w; }

	inline	void	SetX( FIXED32_VEC4D * pVec, FIXED32 val)	{	pVec->x = val; }
	inline	void	SetY( FIXED32_VEC4D * pVec, FIXED32 val)	{	pVec->y = val; }
	inline	void	SetZ( FIXED32_VEC4D * pVec, FIXED32 val)	{	pVec->z = val; }
	inline	void	SetW( FIXED32_VEC4D * pVec, FIXED32 val)	{	pVec->w = val; }
};

#endif
