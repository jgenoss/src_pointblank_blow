#if !defined( __I3_FIXED_POINT_MATRIX_H)
#define __I3_FIXED_POINT_MATRIX_H

#include "i3Matrix.h"
#include "i3FixedPointVector.h"

namespace i3Matrix
{
	inline
	void	Copy( FIXED32_MATRIX * pDest, FIXED32_MATRIX * pSrc)
	{
		register UINT32 * p1, * p2, i;

		p1 = (UINT32 *) pSrc;
		p2 = (UINT32 *) pDest;

		for( i = 0; i < 16; i++)
			p2[i] = p1[i];
	}

	inline
	void	Copy( FIXED32_MATRIX * pDest, MATRIX * pSrc)
	{
		pDest->m00 = i3FXD::FromReal( i3Matrix::Get11( pSrc));
		pDest->m01 = i3FXD::FromReal( i3Matrix::Get12( pSrc));
		pDest->m02 = i3FXD::FromReal( i3Matrix::Get13( pSrc));
		pDest->m03 = i3FXD::FromReal( i3Matrix::Get14( pSrc));
				 
		pDest->m10 = i3FXD::FromReal( i3Matrix::Get21( pSrc));
		pDest->m11 = i3FXD::FromReal( i3Matrix::Get22( pSrc));
		pDest->m12 = i3FXD::FromReal( i3Matrix::Get23( pSrc));
		pDest->m13 = i3FXD::FromReal( i3Matrix::Get24( pSrc));
				 
		pDest->m20 = i3FXD::FromReal( i3Matrix::Get31( pSrc));
		pDest->m21 = i3FXD::FromReal( i3Matrix::Get32( pSrc));
		pDest->m22 = i3FXD::FromReal( i3Matrix::Get33( pSrc));
		pDest->m23 = i3FXD::FromReal( i3Matrix::Get34( pSrc));
				 
		pDest->m30 = i3FXD::FromReal( i3Matrix::Get41( pSrc));
		pDest->m31 = i3FXD::FromReal( i3Matrix::Get42( pSrc));
		pDest->m32 = i3FXD::FromReal( i3Matrix::Get43( pSrc));
		pDest->m33 = i3FXD::FromReal( i3Matrix::Get44( pSrc));
	}

	inline
	void	Copy( MATRIX * pDest, FIXED32_MATRIX * pSrc)
	{
		i3Matrix::SetRight(	pDest,	i3FXD::ToReal( pSrc->m00),
									i3FXD::ToReal( pSrc->m01),
									i3FXD::ToReal( pSrc->m02),
									i3FXD::ToReal( pSrc->m03));
														   
		i3Matrix::SetUp(	pDest,	i3FXD::ToReal( pSrc->m10),
									i3FXD::ToReal( pSrc->m11),
									i3FXD::ToReal( pSrc->m12),
									i3FXD::ToReal( pSrc->m13));
														   
		i3Matrix::SetAt(	pDest,	i3FXD::ToReal( pSrc->m20),
									i3FXD::ToReal( pSrc->m21),
									i3FXD::ToReal( pSrc->m22),
									i3FXD::ToReal( pSrc->m23));
														   
		i3Matrix::SetPos(	pDest,	i3FXD::ToReal( pSrc->m30),
									i3FXD::ToReal( pSrc->m31),
									i3FXD::ToReal( pSrc->m32),
									i3FXD::ToReal( pSrc->m33));
	}

	inline
	void	Copy( PERSIST_MATRIX * pDest, FIXED32_MATRIX * pSrc)
	{
		pDest->mat[0].x = i3FXD::ToReal( pSrc->m00);
		pDest->mat[0].y = i3FXD::ToReal( pSrc->m01);
		pDest->mat[0].z = i3FXD::ToReal( pSrc->m02);
		pDest->mat[0].w = i3FXD::ToReal( pSrc->m03);

		pDest->mat[1].x = i3FXD::ToReal( pSrc->m10);
		pDest->mat[1].y = i3FXD::ToReal( pSrc->m11);
		pDest->mat[1].z = i3FXD::ToReal( pSrc->m12);
		pDest->mat[1].w = i3FXD::ToReal( pSrc->m13);

		pDest->mat[2].x = i3FXD::ToReal( pSrc->m20);
		pDest->mat[2].y = i3FXD::ToReal( pSrc->m21);
		pDest->mat[2].z = i3FXD::ToReal( pSrc->m22);
		pDest->mat[2].w = i3FXD::ToReal( pSrc->m23);

		pDest->mat[3].x = i3FXD::ToReal( pSrc->m30);
		pDest->mat[3].y = i3FXD::ToReal( pSrc->m31);
		pDest->mat[3].z = i3FXD::ToReal( pSrc->m32);
		pDest->mat[3].w = i3FXD::ToReal( pSrc->m33);
	}

	inline
	void	Copy( FIXED32_MATRIX * pDest, PERSIST_MATRIX * pSrc)
	{
		pDest->m00 = i3FXD::FromReal( pSrc->mat[0].x);
		pDest->m01 = i3FXD::FromReal( pSrc->mat[0].y);
		pDest->m02 = i3FXD::FromReal( pSrc->mat[0].z);
		pDest->m03 = i3FXD::FromReal( pSrc->mat[0].w);

		pDest->m10 = i3FXD::FromReal( pSrc->mat[1].x);
		pDest->m11 = i3FXD::FromReal( pSrc->mat[1].y);
		pDest->m12 = i3FXD::FromReal( pSrc->mat[1].z);
		pDest->m13 = i3FXD::FromReal( pSrc->mat[1].w);

		pDest->m20 = i3FXD::FromReal( pSrc->mat[2].x);
		pDest->m21 = i3FXD::FromReal( pSrc->mat[2].y);
		pDest->m22 = i3FXD::FromReal( pSrc->mat[2].z);
		pDest->m23 = i3FXD::FromReal( pSrc->mat[2].w);

		pDest->m30 = i3FXD::FromReal( pSrc->mat[3].x);
		pDest->m31 = i3FXD::FromReal( pSrc->mat[3].y);
		pDest->m32 = i3FXD::FromReal( pSrc->mat[3].z);
		pDest->m33 = i3FXD::FromReal( pSrc->mat[3].w);
	}

	inline
	void	Identity( FIXED32_MATRIX * pM)
	{
		pM->m00 = 65536;	pM->m01 = 0;		pM->m02 = 0;		pM->m03 = 0;
		pM->m10 = 0;		pM->m11 = 65536;	pM->m12 = 0;		pM->m13 = 0;
		pM->m20 = 0;		pM->m21 = 0;		pM->m22 = 65536;	pM->m23 = 0;
		pM->m30 = 0;		pM->m31 = 0;		pM->m32 = 0;		pM->m33 = 65536;
	}

	inline
	BOOL	IsIdentity( FIXED32_MATRIX * pM)
	{
		return	(pM->m00 == 65536) && (pM->m01 == 0) && (pM->m02 == 0) && (pM->m03 == 0) &&
				(pM->m10 == 0) && (pM->m11 == 65536) && (pM->m12 == 0) && (pM->m13 == 0) &&
				(pM->m20 == 0) && (pM->m21 == 0) && (pM->m22 == 65536) && (pM->m23 == 0) &&
				(pM->m30 == 0) && (pM->m31 == 0) && (pM->m32 == 0) && (pM->m33 == 65536);
	}

	inline
	BOOL	IsSame( FIXED32_MATRIX * pM1, FIXED32_MATRIX * pM2)
	{
		return	(pM1->m00 == pM2->m00) &&
				(pM1->m01 == pM2->m01) &&
				(pM1->m02 == pM2->m02) &&
				(pM1->m03 == pM2->m03) &&
				(pM1->m10 == pM2->m10) &&
				(pM1->m11 == pM2->m11) &&
				(pM1->m12 == pM2->m12) &&
				(pM1->m13 == pM2->m13) &&
				(pM1->m20 == pM2->m20) &&
				(pM1->m21 == pM2->m21) &&
				(pM1->m22 == pM2->m22) &&
				(pM1->m23 == pM2->m23) &&
				(pM1->m30 == pM2->m30) &&
				(pM1->m31 == pM2->m31) &&
				(pM1->m32 == pM2->m32) &&
				(pM1->m33 == pM2->m33);

	}

	inline
	FIXED32_VEC3D *	GetRight( FIXED32_MATRIX * pIn)
	{
		return (FIXED32_VEC3D *) & pIn->m00;
	}

	inline
	void			SetRight( FIXED32_MATRIX * pOut, FIXED32 x, FIXED32 y, FIXED32 z, FIXED32 w)
	{
		pOut->m00 = x;	pOut->m01 = y;	pOut->m02 = z;	pOut->m03 = w;
	}

	inline
	FIXED32_VEC3D *	GetUp( FIXED32_MATRIX * pIn)
	{
		return (FIXED32_VEC3D *) & pIn->m10;
	}

	inline
	void			SetUp( FIXED32_MATRIX * pOut, FIXED32 x, FIXED32 y, FIXED32 z, FIXED32 w)
	{
		pOut->m10 = x;	pOut->m11 = y;	pOut->m12 = z;	pOut->m13 = w;
	}

	inline
	FIXED32_VEC3D *	GetAt( FIXED32_MATRIX * pIn)
	{
		return (FIXED32_VEC3D *) & pIn->m20;
	}

	inline
	void			SetAt( FIXED32_MATRIX * pOut, FIXED32 x, FIXED32 y, FIXED32 z, FIXED32 w)
	{
		pOut->m20 = x;	pOut->m21 = y;	pOut->m22 = z;	pOut->m23 = w;
	}

	inline	FIXED32	Get11( FIXED32_MATRIX * pM)		{ return pM->m00; }
	inline	FIXED32	Get12( FIXED32_MATRIX * pM)		{ return pM->m01; }
	inline	FIXED32	Get13( FIXED32_MATRIX * pM)		{ return pM->m02; }
	inline	FIXED32	Get14( FIXED32_MATRIX * pM)		{ return pM->m03; }

	inline	FIXED32	Get21( FIXED32_MATRIX * pM)		{ return pM->m10; }
	inline	FIXED32	Get22( FIXED32_MATRIX * pM)		{ return pM->m11; }
	inline	FIXED32	Get23( FIXED32_MATRIX * pM)		{ return pM->m12; }
	inline	FIXED32	Get24( FIXED32_MATRIX * pM)		{ return pM->m13; }

	inline	FIXED32	Get31( FIXED32_MATRIX * pM)		{ return pM->m20; }
	inline	FIXED32	Get32( FIXED32_MATRIX * pM)		{ return pM->m21; }
	inline	FIXED32	Get33( FIXED32_MATRIX * pM)		{ return pM->m22; }
	inline	FIXED32	Get34( FIXED32_MATRIX * pM)		{ return pM->m23; }

	inline	FIXED32	Get41( FIXED32_MATRIX * pM)		{ return pM->m30; }
	inline	FIXED32	Get42( FIXED32_MATRIX * pM)		{ return pM->m31; }
	inline	FIXED32	Get43( FIXED32_MATRIX * pM)		{ return pM->m32; }
	inline	FIXED32	Get44( FIXED32_MATRIX * pM)		{ return pM->m33; }

	inline
	FIXED32_VEC3D *	GetPos( FIXED32_MATRIX * pIn)
	{
		return (FIXED32_VEC3D *) & pIn->m30;
	}

	inline
	void			SetPos( FIXED32_MATRIX * pMat, FIXED32_VEC3D * pPos)
	{
		pMat->m30 = pPos->x;	pMat->m31 = pPos->y;	pMat->m32 = pPos->z;
	}

	inline
	void			SetPos( FIXED32_MATRIX * pMat, VEC3D * pPos)
	{
		pMat->m30 = i3FXD::FromReal( i3Vector::GetX( pPos));
		pMat->m31 = i3FXD::FromReal( i3Vector::GetY( pPos));
		pMat->m32 = i3FXD::FromReal( i3Vector::GetZ( pPos));
	}

	inline
	void			SetPos( FIXED32_MATRIX * pOut, FIXED32 x, FIXED32 y, FIXED32 z, FIXED32 w)
	{
		pOut->m30 = x;	pOut->m31 = y;	pOut->m32 = z;	pOut->m33 = w;
	}

	inline
	void			SetPos( FIXED32_MATRIX * pOut, FIXED32 x, FIXED32 y, FIXED32 z)
	{
		pOut->m30 = x;	pOut->m31 = y;	pOut->m32 = z;
	}

	void	GetScale( FIXED32_MATRIX * pMat, FIXED32_VEC3D * pOut);

	void	Mul( FIXED32_MATRIX * pOut, MATRIX * pM1, MATRIX * pM2);
	void	Mul( FIXED32_MATRIX * pOut, FIXED32_MATRIX * pM1, FIXED32_MATRIX * pM2);

	void	MulAffine( FIXED32_MATRIX * pOut, MATRIX * pM1, MATRIX * pM2);
	void	MulAffine( FIXED32_MATRIX * pOut, FIXED32_MATRIX * pM1, FIXED32_MATRIX * pM2);

	inline
	void SetTranslate( FIXED32_MATRIX *pOut, FIXED32 x, FIXED32 y, FIXED32 z )
	{
		Identity( pOut);

		pOut->m30 = x;
		pOut->m31 = y;
		pOut->m32 = z;
	}

	inline
	void SetTranslate( FIXED32_MATRIX *pOut, FIXED32_VEC3D *pVec)
	{
		SetTranslate( pOut, pVec->x, pVec->y, pVec->z);
	}

	inline
	void PreTranslate( FIXED32_MATRIX *pOut, FIXED32 x, FIXED32 y, FIXED32 z )
	{
		pOut->m30 = i3FXD::Mul(x,pOut->m00) + i3FXD::Mul(y,pOut->m10) + i3FXD::Mul(z,pOut->m20) + pOut->m30;
		pOut->m31 = i3FXD::Mul(x,pOut->m01) + i3FXD::Mul(y,pOut->m11) + i3FXD::Mul(z,pOut->m21) + pOut->m31;
		pOut->m32 = i3FXD::Mul(x,pOut->m02) + i3FXD::Mul(y,pOut->m12) + i3FXD::Mul(z,pOut->m22) + pOut->m32;
		pOut->m33 = i3FXD::Mul(x,pOut->m03) + i3FXD::Mul(y,pOut->m13) + i3FXD::Mul(z,pOut->m23) + pOut->m33;
	}

	inline
	void PreTranslate( FIXED32_MATRIX *pOut, FIXED32_VEC3D * pVec)
	{
		PreTranslate( pOut, pVec->x, pVec->y, pVec->z);
	}

	inline
	void PostTranslate( FIXED32_MATRIX *pOut, FIXED32 x, FIXED32 y, FIXED32 z )
	{
		FIXED32_MATRIX temp;

		SetTranslate( &temp, x, y, z);

		Mul( pOut, pOut, &temp);
	}

	inline
	void PostTranslate( FIXED32_MATRIX *pOut, FIXED32_VEC3D * pVec)
	{
		FIXED32_MATRIX temp;

		SetTranslate( &temp, pVec);

		Mul( pOut, pOut, &temp);
	}

	inline
	void SetRotateAxis( FIXED32_MATRIX *pOut, const FIXED32_VEC3D *pV, FIXED32 Angle )
	{
		FIXED32 c, s, _c, x, y, z;
		FIXED32 _yc, _xc, zs, xs, ys;	// ,_zc;
		FIXED32 _xyc, _xzc, _yzc;

		c = i3Math::cos( Angle);
		s = i3Math::sin( Angle);
		_c = 65536 - c;
		x = pV->x;
		y = pV->y;
		z = pV->z;

		_yc = i3FXD::Mul( y, _c);
		_xc = i3FXD::Mul( x, _c);
		zs = i3FXD::Mul( z, s);
		ys = i3FXD::Mul( y, s);
		xs = i3FXD::Mul( x, s);

		_xyc = i3FXD::Mul( y, _xc);
		_xzc = i3FXD::Mul( z, _xc);
		_yzc = i3FXD::Mul( z, _yc);

		pOut->m00 = c + i3FXD::Mul( x, _xc);
		pOut->m10 = _xyc - zs;
		pOut->m20 = _xzc + ys;
		pOut->m30 = 0;

		pOut->m01 = _xyc + zs;
		pOut->m11 = c + i3FXD::Mul( y, _yc);
		pOut->m21 = _yzc - xs;
		pOut->m31 = 0;

		pOut->m02 = _xzc - ys;
		pOut->m12 = -(_yzc - xs);
		pOut->m22 = c + ( i3FXD::Mul( i3FXD::Mul( z, z), _c));
		pOut->m32 = 0;

		pOut->m03 = 0;
		pOut->m13 = 0;
		pOut->m23 = 0;
		pOut->m33 = 65536;
	}

	inline
	void SetRotateAxisDeg( FIXED32_MATRIX *pOut, const FIXED32_VEC3D *pV, FIXED32 Degree )
	{
		REAL32 rad = I3_DEG2RAD( i3FXD::ToReal( Degree));
		SetRotateAxis( pOut, pV, i3FXD::FromReal( rad));
	}

	inline
	void	PreRotateAxis( FIXED32_MATRIX * pOut, const FIXED32_VEC3D * pV, FIXED32 Angle)
	{
		FIXED32_MATRIX tmp;

		Identity( &tmp);

		SetRotateAxis( &tmp, pV, Angle);

		//return Mul( pOut, &tmp, pOut);
		Mul( pOut, &tmp, pOut);
	}

	inline
	void	PreRotateAxisDeg( FIXED32_MATRIX * pOut, const FIXED32_VEC3D * pV, FIXED32 Degree)
	{
		REAL32 rad = I3_DEG2RAD( i3FXD::ToReal( Degree));
		PreRotateAxis( pOut, pV, i3FXD::FromReal( rad));
	}

	inline
	void	PostRotateAxis( FIXED32_MATRIX * pOut, const FIXED32_VEC3D * pV, FIXED32 Angle)
	{
		FIXED32_MATRIX tmp;

		Identity( &tmp);

		SetRotateAxis( &tmp, pV, Angle);
		Mul( pOut, pOut, &tmp);
	}

	inline
	void	PostRotateAxisDeg( FIXED32_MATRIX * pOut, const FIXED32_VEC3D * pV, FIXED32 Degree)
	{
		REAL32 rad = I3_DEG2RAD( i3FXD::ToReal( Degree));

		//return PostRotateAxis( pOut, pV, i3FXD::FromReal( rad));
		PostRotateAxis( pOut, pV, i3FXD::FromReal( rad));
		
	}

	inline
	void SetRotateX( FIXED32_MATRIX *pOut, FIXED32 Angle )
	{
		FIXED32 s = i3Math::sin( Angle);
		FIXED32 c = i3Math::cos( Angle);

		Identity( pOut);

		pOut->m11 = c;
		pOut->m12 = s;
		pOut->m21 = -s;
		pOut->m22 = c;
	}

	inline
	void SetRotateXDeg( FIXED32_MATRIX *pOut, FIXED32 Degree )
	{
		REAL32 rad = I3_DEG2RAD( i3FXD::ToReal( Degree));
		SetRotateX( pOut, i3FXD::FromReal( rad));
	}

	inline
	void SetRotateY( FIXED32_MATRIX *pOut, FIXED32 Angle )
	{
		FIXED32 s = i3Math::sin( Angle);
		FIXED32 c = i3Math::cos( Angle);

		Identity( pOut);

		pOut->m00 = c;
		pOut->m02 = -s;
		pOut->m20 = s;
		pOut->m22 = c;
	}

	inline
	void SetRotateYDeg( FIXED32_MATRIX *pOut, FIXED32 Degree )
	{
		FIXED32 rad = i3FXD::FromReal( I3_DEG2RAD( i3FXD::ToReal( Degree)));
		SetRotateY( pOut, rad);
	}

	inline
	void SetRotateZ( FIXED32_MATRIX *pOut, FIXED32 Angle )
	{
		FIXED32 s = i3Math::sin( Angle);
		FIXED32 c = i3Math::cos( Angle);

		Identity( pOut);

		pOut->m00 = c;
		pOut->m01 = s;
		pOut->m10 = -s;
		pOut->m11 = c;
	}

	inline
	void SetRotateZDeg( FIXED32_MATRIX *pOut, FIXED32 Degree)
	{
		FIXED32 rad = i3FXD::FromReal( I3_DEG2RAD( i3FXD::ToReal( Degree)));
		SetRotateZ( pOut, rad);
	}


	// PreRotate
	inline
	void PreRotateX( FIXED32_MATRIX *pOut, FIXED32 Angle )
	{
		FIXED32 s = i3Math::sin( Angle);
		FIXED32 c = i3Math::cos( Angle);
		FIXED32 m10, m11, m12, m13;

		m10 = pOut->m10;
		m11 = pOut->m11;
		m12 = pOut->m12;
		m13 = pOut->m13;

		pOut->m10 = i3FXD::Mul(c, m10)  + i3FXD::Mul(s, pOut->m20);
		pOut->m11 = i3FXD::Mul(c, m11)  + i3FXD::Mul(s, pOut->m21);
		pOut->m12 = i3FXD::Mul(c, m12)  + i3FXD::Mul(s, pOut->m22);
		pOut->m13 = i3FXD::Mul(c, m13)  + i3FXD::Mul(s, pOut->m23);

		pOut->m20 = i3FXD::Mul(-s, m10)  + i3FXD::Mul(c, pOut->m20);
		pOut->m21 = i3FXD::Mul(-s, m11)  + i3FXD::Mul(c, pOut->m21);
		pOut->m22 = i3FXD::Mul(-s, m12)  + i3FXD::Mul(c, pOut->m22);
		pOut->m23 = i3FXD::Mul(-s, m13)  + i3FXD::Mul(c, pOut->m23);
	}

	inline
	void PreRotateY( FIXED32_MATRIX *pOut, FIXED32 Angle )
	{
		FIXED32 s = i3Math::sin( Angle);
		FIXED32 c = i3Math::cos( Angle);
		FIXED32 m00, m01, m02, m03;
		
		m00 = pOut->m00;
		m01 = pOut->m01;
		m02 = pOut->m02;
		m03 = pOut->m03;

		pOut->m00 = i3FXD::Mul(c, m00)  + i3FXD::Mul(-s, pOut->m20);
		pOut->m01 = i3FXD::Mul(c, m01)  + i3FXD::Mul(-s, pOut->m21);
		pOut->m02 = i3FXD::Mul(c, m02)  + i3FXD::Mul(-s, pOut->m22);
		pOut->m03 = i3FXD::Mul(c, m03)  + i3FXD::Mul(-s, pOut->m23);

		pOut->m20 = i3FXD::Mul(s, m00)  + i3FXD::Mul(c, pOut->m20);
		pOut->m21 = i3FXD::Mul(s, m01)  + i3FXD::Mul(c, pOut->m21);
		pOut->m22 = i3FXD::Mul(s, m02)  + i3FXD::Mul(c, pOut->m22);
		pOut->m23 = i3FXD::Mul(s, m03)  + i3FXD::Mul(c, pOut->m23);
	}

	inline
	void PreRotateZ( FIXED32_MATRIX *pOut, FIXED32 Angle )
	{
		FIXED32 s = i3Math::sin( Angle);
		FIXED32 c = i3Math::cos( Angle);
		FIXED32 m00, m01, m02, m03;
		
		m00 = pOut->m00;
		m01 = pOut->m01;
		m02 = pOut->m02;
		m03 = pOut->m03;

		pOut->m00 = i3FXD::Mul(c, m00)  + i3FXD::Mul(s, pOut->m10);
		pOut->m01 = i3FXD::Mul(c, m01)  + i3FXD::Mul(s, pOut->m11);
		pOut->m02 = i3FXD::Mul(c, m02)  + i3FXD::Mul(s, pOut->m12);
		pOut->m03 = i3FXD::Mul(c, m03)  + i3FXD::Mul(s, pOut->m13);

		pOut->m10 = i3FXD::Mul(-s, m00)  + i3FXD::Mul(c, pOut->m10);
		pOut->m11 = i3FXD::Mul(-s, m01)  + i3FXD::Mul(c, pOut->m11);
		pOut->m12 = i3FXD::Mul(-s, m02)  + i3FXD::Mul(c, pOut->m12);
		pOut->m13 = i3FXD::Mul(-s, m03)  + i3FXD::Mul(c, pOut->m13);
	}

	inline
	void PreRotateXDeg( FIXED32_MATRIX *pOut, FIXED32 Angle )
	{
		REAL32 rad =  I3_DEG2RAD( i3FXD::ToReal( Angle) );
		//return PreRotateX( pOut, i3FXD::FromReal(rad) );
		PreRotateX( pOut, i3FXD::FromReal(rad) );
	}

	inline
	void PreRotateYDeg( FIXED32_MATRIX *pOut, FIXED32 Angle )
	{
		REAL32 rad = I3_DEG2RAD(  i3FXD::ToReal( Angle) );
		//return PreRotateY( pOut, i3FXD::FromReal( rad) );
		PreRotateY( pOut, i3FXD::FromReal( rad) );
	}

	inline
	void PreRotateZDeg( FIXED32_MATRIX *pOut, FIXED32 Angle )
	{
		REAL32 rad = I3_DEG2RAD( i3FXD::ToReal( Angle) );
		//return PreRotateZ( pOut, i3FXD::FromReal( rad) );
		PreRotateZ( pOut, i3FXD::FromReal( rad) );
	}

	// PostRotate
	inline
	void PostRotateX( FIXED32_MATRIX *pOut, FIXED32 Angle )
	{
		FIXED32 s = i3Math::sin( Angle);
		FIXED32 c = i3Math::cos( Angle);
		FIXED32 tmp;

		tmp = pOut->m01;
		pOut->m01 = i3FXD::Mul(tmp, c) + i3FXD::Mul(pOut->m02, -s);
		pOut->m02 = i3FXD::Mul(tmp, s) + i3FXD::Mul(pOut->m02, c);
						
		tmp = pOut->m11;
		pOut->m11 = i3FXD::Mul(tmp, c) + i3FXD::Mul(pOut->m12, -s);
		pOut->m12 = i3FXD::Mul(tmp, s) + i3FXD::Mul(pOut->m12, c);
						
		tmp = pOut->m21;
		pOut->m21 = i3FXD::Mul(tmp, c) + i3FXD::Mul(pOut->m22, -s);
		pOut->m22 = i3FXD::Mul(tmp, s) + i3FXD::Mul(pOut->m22, c);
					
		tmp = pOut->m31;
		pOut->m31 = i3FXD::Mul(tmp, c) + i3FXD::Mul(pOut->m32, -s);
		pOut->m32 = i3FXD::Mul(tmp, s) + i3FXD::Mul(pOut->m32, c);
	}

	inline
	void PostRotateY( FIXED32_MATRIX *pOut, FIXED32 Angle )
	{
		FIXED32 s = i3Math::sin( Angle);
		FIXED32 c = i3Math::cos( Angle);
		FIXED32 tmp;

		tmp = pOut->m00;
		pOut->m00 = i3FXD::Mul(tmp, c)  + i3FXD::Mul(pOut->m02, s);
		pOut->m02 = i3FXD::Mul(tmp, -s) + i3FXD::Mul(pOut->m02, c);
								
		tmp = pOut->m10;
		pOut->m10 = i3FXD::Mul(tmp, c)  + i3FXD::Mul(pOut->m12, s);
		pOut->m12 = i3FXD::Mul(tmp, -s) + i3FXD::Mul(pOut->m12, c);
					
		tmp = pOut->m20;
		pOut->m20 = i3FXD::Mul(tmp, c)  + i3FXD::Mul(pOut->m22, s);
		pOut->m22 = i3FXD::Mul(tmp, -s) + i3FXD::Mul(pOut->m22, c);
					
		tmp = pOut->m30;
		pOut->m30 = i3FXD::Mul(tmp, c)  + i3FXD::Mul(pOut->m32, s);
		pOut->m32 = i3FXD::Mul(tmp, -s) + i3FXD::Mul(pOut->m32, c);
	}

	inline
	void PostRotateZ( FIXED32_MATRIX *pOut, FIXED32 Angle )
	{
		FIXED32 s = i3Math::sin( Angle);
		FIXED32 c = i3Math::cos( Angle);
		FIXED32 tmp;

		tmp = pOut->m00;
		pOut->m00 = i3FXD::Mul(tmp, c)  + i3FXD::Mul(pOut->m01, -s);
		pOut->m01 = i3FXD::Mul(tmp, s)  + i3FXD::Mul(pOut->m01, c);
								
		tmp = pOut->m10;
		pOut->m10 = i3FXD::Mul(tmp, c)  + i3FXD::Mul(pOut->m11, -s);
		pOut->m11 = i3FXD::Mul(tmp, s)  + i3FXD::Mul(pOut->m11, c);
					
		tmp = pOut->m20;
		pOut->m20 = i3FXD::Mul(tmp, c)  + i3FXD::Mul(pOut->m21, -s);
		pOut->m21 = i3FXD::Mul(tmp, s)  + i3FXD::Mul(pOut->m21, c);
					
		tmp = pOut->m30;
		pOut->m30 = i3FXD::Mul(tmp, c)  + i3FXD::Mul(pOut->m31, -s);
		pOut->m31 = i3FXD::Mul(tmp, s)  + i3FXD::Mul(pOut->m31, c);
	}

	inline
	void PostRotateXDeg( FIXED32_MATRIX *pOut, FIXED32 Angle )
	{
		FIXED32 rad = i3FXD::FromReal( I3_DEG2RAD( i3FXD::ToReal( Angle)));
		PostRotateX( pOut, rad);
	}

	inline
	void PostRotateYDeg( FIXED32_MATRIX *pOut, FIXED32 Angle )
	{
		FIXED32 rad = i3FXD::FromReal( I3_DEG2RAD( i3FXD::ToReal( Angle)));
		PostRotateY( pOut, rad);
	}

	inline
	void PostRotateZDeg( FIXED32_MATRIX *pOut, FIXED32 Angle )
	{
		FIXED32 rad = i3FXD::FromReal( I3_DEG2RAD( i3FXD::ToReal( Angle)));
		PostRotateZ( pOut, rad);
	}

	inline
	void	SetScale( FIXED32_MATRIX * pOut, FIXED32 sx, FIXED32 sy, FIXED32 sz)
	{
		Identity( pOut);

		pOut->m00 = sx;
		pOut->m11 = sy;
		pOut->m22 = sz;
	}

	inline
	void	PreScale( FIXED32_MATRIX * pOut, FIXED32 sx, FIXED32 sy, FIXED32 sz)
	{
		pOut->m00 = i3FXD::Mul( pOut->m00, sx);
		pOut->m01 = i3FXD::Mul( pOut->m01, sx);
		pOut->m02 = i3FXD::Mul( pOut->m02, sx);
		pOut->m03 = i3FXD::Mul( pOut->m03, sx);
								   
		pOut->m10 = i3FXD::Mul( pOut->m10, sy);
		pOut->m11 = i3FXD::Mul( pOut->m11, sy);
		pOut->m12 = i3FXD::Mul( pOut->m12, sy);
		pOut->m13 = i3FXD::Mul( pOut->m13, sy);
								   
		pOut->m20 = i3FXD::Mul( pOut->m20, sz);
		pOut->m21 = i3FXD::Mul( pOut->m21, sz);
		pOut->m22 = i3FXD::Mul( pOut->m22, sz);
		pOut->m23 = i3FXD::Mul( pOut->m23, sz);
	}

	inline
	void	PostScale( FIXED32_MATRIX *pOut, FIXED32 sx, FIXED32 sy, FIXED32 sz)
	{
		pOut->m00 = i3FXD::Mul( pOut->m00, sx);
		pOut->m10 = i3FXD::Mul( pOut->m10, sx);
		pOut->m20 = i3FXD::Mul( pOut->m20, sx);
		pOut->m30 = i3FXD::Mul( pOut->m30, sx);
					
		pOut->m01 = i3FXD::Mul( pOut->m01, sy);
		pOut->m11 = i3FXD::Mul( pOut->m11, sy);
		pOut->m21 = i3FXD::Mul( pOut->m21, sy);
		pOut->m31 = i3FXD::Mul( pOut->m31, sy);
					
		pOut->m02 = i3FXD::Mul( pOut->m02, sz);
		pOut->m12 = i3FXD::Mul( pOut->m12, sz);
		pOut->m22 = i3FXD::Mul( pOut->m22, sz);
		pOut->m32 = i3FXD::Mul( pOut->m32, sz);
	}

	inline
	void	PostScale( FIXED32_MATRIX *pOut, FIXED32_VEC3D * pVec)
	{
		PostScale( pOut, pVec->x, pVec->y, pVec->z);
	}

	inline
	void	Transpose( FIXED32_MATRIX * pOut, const FIXED32_MATRIX * pIn)
	{
		FIXED32_MATRIX m;

		SetRight(	&m, pIn->m00, pIn->m10, pIn->m20, pIn->m30);
		SetUp(		&m, pIn->m01, pIn->m11, pIn->m21, pIn->m31);
		SetAt(		&m, pIn->m02, pIn->m12, pIn->m22, pIn->m32);
		SetPos(		&m, pIn->m03, pIn->m13, pIn->m23, pIn->m33);
		
		Copy( pOut, &m);
	}

	inline
	void	Transpose3x3( FIXED32_MATRIX * pOut, const FIXED32_MATRIX * pIn)
	{
		FIXED32_MATRIX m;

		SetRight(	&m, pIn->m00, pIn->m10, pIn->m20, pIn->m03);
		SetUp(		&m, pIn->m01, pIn->m11, pIn->m21, pIn->m13);
		SetAt(		&m, pIn->m02, pIn->m12, pIn->m22, pIn->m23);
		SetPos(		&m, pIn->m30, pIn->m31, pIn->m32, pIn->m33);
		
		Copy( pOut, &m);
	}

	FIXED32_MATRIX *	Inverse( FIXED32_MATRIX * pOut, FIXED32 * pDeterminant, FIXED32_MATRIX * pIn);

	void	GetRotateQuat( FIXED32_QUAT * pOut, const FIXED32_MATRIX * pMatrix);
	void	SetRotateQuat( FIXED32_MATRIX * pOut, const FIXED32_QUAT * pQ);

	inline
	void	TransformPoints( FIXED32_VEC3D * pOutVA, FIXED32_VEC3D * pInVA, INT32 count, FIXED32_MATRIX * pMatrix)
	{
		INT32 i;

		for( i = 0; i < count; i++)
		{
			i3Vector::TransformCoord( pOutVA, pInVA, pMatrix);

			pInVA ++;
			pOutVA ++;
		}
	}

	inline
	void TransformVectors( FIXED32_VEC3D * pOutVA, FIXED32_VEC3D * pInVA, INT32 count, FIXED32_MATRIX * pMatrix)
	{
		INT32 i;

		for( i = 0; i < count; i++)
		{
			i3Vector::TransformNormal( pOutVA, pInVA, pMatrix);
			pInVA ++;
			pOutVA ++;
		}
	}

	inline
	void	TransformPoints( VEC3D * pOutVA, VEC3D * pInVA, INT32 count, FIXED32_MATRIX * pMatrix)
	{
		INT32 i;

		for( i = 0; i < count; i++)
		{
			i3Vector::TransformCoord( pOutVA, pInVA, pMatrix);

			pInVA ++;
			pOutVA ++;
		}
	}

	inline
	void TransformVectors( VEC3D * pOutVA, VEC3D * pInVA, INT32 count, FIXED32_MATRIX * pMatrix)
	{
		INT32 i;

		for( i = 0; i < count; i++)
		{
			i3Vector::TransformNormal( pOutVA, pInVA, pMatrix);
			pInVA ++;
			pOutVA ++;
		}
	}

	inline
	void PerspectiveFOVRH( FIXED32_MATRIX *pOut, FIXED32 fovy, FIXED32 Aspect, FIXED32 zn, FIXED32 zf )
	{
		FIXED32 h, w;
		REAL32 fzn, fzf, r;

		fzn = i3FXD::ToReal( zn);
		fzf = i3FXD::ToReal( zf);
		r = i3Math::Reciprocal( fzn - fzf);

		h = i3FXD::Div( i3Math::cos( fovy >> 1), i3Math::sin( fovy >> 1));
		w = i3FXD::Div( h, Aspect);

		pOut->m00 = w;
		pOut->m01 = 0;
		pOut->m02 = 0;
		pOut->m03 = 0;

		pOut->m10 = 0;
		pOut->m11 = h;
		pOut->m12 = 0;
		pOut->m13 = 0;

		pOut->m20 = 0;
		pOut->m21 = 0;
		pOut->m22 = i3FXD::FromReal( fzf * r);
		pOut->m23 = i3FXD::FromReal( -1.0f);

		pOut->m30 = 0;
		pOut->m31 = 0;
		pOut->m32 = i3FXD::FromReal( (fzn * fzf) * r);
		pOut->m33 = 0;
	}

	inline
	void OrthoRH( FIXED32_MATRIX *pOut, FIXED32 w, FIXED32 h, FIXED32 zn, FIXED32 zf )
	{
		FIXED32 r_zd = i3FXD::Div( 65536, (zn - zf));

		pOut->m00 = i3FXD::Div( i3FXD::FromReal( 2.0f), w);
		pOut->m01 = 0;
		pOut->m02 = 0;
		pOut->m03 = 0;

		pOut->m10 = 0;
		pOut->m11 = i3FXD::Div( i3FXD::FromReal( 2.0f), h);
		pOut->m12 = 0;
		pOut->m13 = 0;

		pOut->m20 = 0;
		pOut->m21 = 0;
		pOut->m22 = r_zd;
		pOut->m23 = 0;

		pOut->m30 = 0;
		pOut->m31 = 0;
		pOut->m32 = i3FXD::Mul( zn, r_zd);
		pOut->m33 = 65536;
	}

	inline
	void OrthoLH( FIXED32_MATRIX *pOut, FIXED32 w, FIXED32 h, FIXED32 zn, FIXED32 zf )
	{
		FIXED32 r_zd = i3FXD::Div( 65536, (zn - zf));

		pOut->m00 = i3FXD::Div( i3FXD::FromReal( 2.0f), w);
		pOut->m01 = 0;
		pOut->m02 = 0;
		pOut->m03 = 0;

		pOut->m10 = 0;
		pOut->m11 = i3FXD::Div( i3FXD::FromReal( 2.0f), h);
		pOut->m12 = 0;
		pOut->m13 = 0;

		pOut->m20 = 0;
		pOut->m21 = 0;
		pOut->m22 = i3FXD::Div( i3FXD::FromReal( 1.0f), (zf - zn));
		pOut->m23 = 0;

		pOut->m30 = 0;
		pOut->m31 = 0;
		pOut->m32 = i3FXD::Mul( zn, r_zd);
		pOut->m33 = 65536;
	}

	void	MakeWithUp( FIXED32_MATRIX * pOut, FIXED32_VEC3D * pUp, FIXED32_VEC3D * pAt);
	void	MakeWithAt( FIXED32_MATRIX * pOut, FIXED32_VEC3D * pAt, FIXED32_VEC3D * pUp);

	void	Dump( FIXED32_MATRIX * pMtx);
};

#endif
