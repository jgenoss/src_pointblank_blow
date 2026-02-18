#if !defined( __I3_QUATERNION_H)
#define __I3_QUATERNION_H

#include "../i3Base/i3CommonType.h"
#include "../i3Base/i3Debug.h"
#include "i3MathType.h"
#include "i3Matrix.h"

I3_EXPORT_MATH void i3QuaternionFromMatrix( QUATERNION * pQuat, MATRIX * pMtx);
I3_EXPORT_MATH void i3QuaternionSLERP( QUATERNION * pOut, QUATERNION * pIn1, QUATERNION * pIn2, REAL32 t);

#define VFPU_ASM_QUAT
//#define		VFPU_SCE_FOUNCTION		//디버그용으로 사용합니다. 

namespace i3Quat
{
	inline
	void Copy( QUATERNION * pDest, QUATERNION * pSrc)
	{
		i3Vector::Copy( pDest, pSrc);
	}

	//	Q =		( x, y, z, w)
	//
	//			| (1 - 2y^2 - 2z^2),	(2xy - 2wz),		(2xz + 2wy)			|
	//	R =		| (2xy + 2wz),			(1 - 2x^2 - 2z^2),	(2yz - 2wx)			|
	//			| (2xz - 2wy),			(2yz + 2wx),		(1 - 2x^2 - 2y^2)	|
	inline
	void Set( QUATERNION * pDest, MATRIX * pMatrix)
	{
		i3QuaternionFromMatrix( pDest, pMatrix);
	}

	inline
	void Set( QUATERNION * pDest, REAL32 a, REAL32 b, REAL32 c, REAL32 d)
	{
		pDest->vec[0] = a;
		pDest->vec[1] = b;
		pDest->vec[2] = c;
		pDest->vec[3] = d;
	}

	inline
	REAL32	GetA( QUATERNION * pIn)
	{
		return pIn->vec[0];
	}

	inline
	REAL32	GetB( QUATERNION * pIn)
	{
		return pIn->vec[1];
	}

	inline
	REAL32	GetC( QUATERNION * pIn)
	{
		return pIn->vec[2];
	}

	inline
	REAL32	GetD( QUATERNION * pIn)
	{
		return pIn->vec[3];
	}

	inline
	void Identity( QUATERNION * pOut)
	{
		#if defined( I3G_PSP)
			#if defined( VFPU_ASM_QUAT)
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"vidt.q			c030\n"					// c030 = (0.0, 0.0, 0.0, 1.0)
					"sv.q			c030, %0\n"				// *pq  = c030
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
				);
			#else
				pOut->vec[0] = 0.0f;
				pOut->vec[1] = 0.0f;
				pOut->vec[2] = 0.0f;
				pOut->vec[3] = 1.0f;
			#endif

		#else
			pOut->vec[0] = 0.0f;
			pOut->vec[1] = 0.0f;
			pOut->vec[2] = 0.0f;
			pOut->vec[3] = 1.0f;
		#endif
	}

	inline
	bool IsIdentity( QUATERNION * pIn)
	{
		return	i3Math::IsSame( pIn->vec[0], 0.0f) &&
				i3Math::IsSame(	pIn->vec[1], 0.0f) &&
				i3Math::IsSame( pIn->vec[2], 0.0f) &&
				i3Math::IsSame( pIn->vec[3], 1.0f);
	}

	// Conjugate
	//
	// Conjugate, Norm and Absolute value
	//
	// q = (a, b, c, d)
	// 
	// conj(q) = (a, -b, -c, -d)
	inline
	void Conjugate( QUATERNION * pOut, QUATERNION * pIn)
	{
		pOut->vec[0] = -pIn->vec[0];
		pOut->vec[1] = -pIn->vec[1];
		pOut->vec[2] = -pIn->vec[2];
		pOut->vec[3] = pIn->vec[3];
	}

	// // norm(q) = a*a + b*b + c*c + d*d
	inline
	REAL32 LengthSq( QUATERNION * pIn)
	{
		return	(pIn->vec[0] * pIn->vec[0]) + 
				(pIn->vec[1] * pIn->vec[1]) +
				(pIn->vec[2] * pIn->vec[2]) +
				(pIn->vec[3] * pIn->vec[3]);
	}

	// abs(q)  = sqrt(norm(q))
	inline
	REAL32 Length( QUATERNION * pIn)
	{
		return i3Math::Sqrt( LengthSq( pIn));
	}

	// inv(q) = conj(q)/norm(q)
	inline
	void Inverse( QUATERNION * pOut, QUATERNION * pIn)
	{
		REAL32 lensq = 1.f / LengthSq( pIn);

		Conjugate( pOut, pIn);

		pOut->vec[0] = pOut->vec[0] * lensq;
		pOut->vec[1] = pOut->vec[1] * lensq;
		pOut->vec[2] = pOut->vec[2] * lensq;
		pOut->vec[3] = pOut->vec[3] * lensq;
	}

	// 2005.2.22 Lene3 수정하였습니다. (VFPU코드 수정 및 libvfpu함수 추가)
	/////////////////////////////////////////////////////////////////
	// 2005.3.4 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 313.624 ms
	// Vfpu unUse: 1229.493 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Normalize( QUATERNION * pOut, QUATERNION * pIn)
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_QUAT_)				
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c000, %1\n"				// c000 = *pq1
					"vdot.q			s010, c000, c000\n"		// s010 = s000*s000 + s001*s001 + s002*s002 + s003*s003
					"vrsq.s			s010, s010\n"			// s011 = 1.0 / sqrt(s010)
					"vscl.q			c000, c000, s010\n"		// c000 = c000 * s010
					"sv.q			c000, %0\n"				// *pq0 = c000
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pIn)
					);
			#else
				REAL32 len;

				len = (pIn->vec[0] * pIn->vec[0]) + (pIn->vec[1] * pIn->vec[1]) + (pIn->vec[2] * pIn->vec[2]) + (pIn->vec[3] * pIn->vec[3]);

				if( len > 0.0f)
					len = 1.0f / i3Math::Sqrt( len);
				else
					len = 1.0f;

				pOut->vec[0] = pIn->vec[0] * len;
				pOut->vec[1] = pIn->vec[1] * len;
				pOut->vec[2] = pIn->vec[2] * len;
				pOut->vec[3] = pIn->vec[3] * len;
			#endif


		#else
			REAL32 len;

			len = (pIn->vec[0] * pIn->vec[0]) + (pIn->vec[1] * pIn->vec[1]) + (pIn->vec[2] * pIn->vec[2]) + (pIn->vec[3] * pIn->vec[3]);

			if( len > 0.0f)
				len = 1.0f / i3Math::Sqrt( len);
			else
				len = 1.0f;

			pOut->vec[0] = pIn->vec[0] * len;
			pOut->vec[1] = pIn->vec[1] * len;
			pOut->vec[2] = pIn->vec[2] * len;
			pOut->vec[3] = pIn->vec[3] * len;
		#endif
	}

	// Multiplication
	// 
	// q = (a, b, c, d), p = (x, y, z, w)
	// 
	// q * p = (a + b*i + c*j + d*k) * (x + y*i + z*j + w*k)
	// 
	//	=   a   * (x + y*i + z*j + w*k)
    //       + b*i * (x + y*i + z*j + w*k)
    //       + c*j * (x + y*i + z*j + w*k)
    //       + d*k * (x + y*i + z*j + w*k)
	//
    //     =   a*x   + a*y*i + a*z*j + a*w*k
    //       + b*x*i - b*y   + b*z*k - b*w*j
    //       + c*x*j - c*y*k - c*z   + c*w*i
    //       + d*x*k + d*y*j - d*z*i - d*w
	//
    //     = (a*x - b*y - c*z - d*w,
    //        a*y + b*x + c*w - d*z,
    //        a*z - b*w + c*x + d*y,
    //        a*w + b*z - c*y + d*x)
	//
	// One can rewrite this if we define two 3-dimensional vectors:
	// 
	// qV = (b, c, d)      q = (a, qV)
	// pV = (y, z, w)      p = (x, pV)
	//
	// q * p = (a*x - dotprod(qV, pV),
    //        a*pV + x*qV + crossprod(qV, pV))
	// 
	// Now it is easy to see that multiplication of quaternion is not commutative,
	// since the cross product of the vectors is not commutative. One must take
	// this in consideration while working with quaternion algebra.
	// 
	// If we square a quaternion, the cross product is zero:
	// 
	//	q * q = (a*a - dotprod(qV, qV), 2*a*qV)
	// 
	// In fact, One do not have to consider the multiplication order at all when
	// raising a quaternion to any positive integer power.
	// 
	// Fast multiplication
	// 
	// There are some schemes available that reduces the number of internal
	// multiplications when doing quaternion multiplication. Here is one:
	// 
	// q = (a, b, c, d), p = (x, y, z, w)
	// 
	// tmp_00 = (d - c) * (z - w)
	// tmp_01 = (a + b) * (x + y)
	// tmp_02 = (a - b) * (z + w)
	// tmp_03 = (c + d) * (x - y)
	// tmp_04 = (d - b) * (y - z)
	// tmp_05 = (d + b) * (y + z)
	// tmp_06 = (a + c) * (x - w)
	// tmp_07 = (a - c) * (x + w)
	// tmp_08 = tmp_05 + tmp_06 + tmp_07
	// tmp_09 = 0.5 * (tmp_04 + tmp_08)
	// 
	// q * p = (tmp_00 + tmp_09 - tmp_05,
    //        tmp_01 + tmp_09 - tmp_08,
    //        tmp_02 + tmp_09 - tmp_07,
    //        tmp_03 + tmp_09 - tmp_06)
	//
	// With this method You get 7 less multiplications, but 15 more
	// additions/subtractions. Generally, this is still an improvement.
	/////////////////////////////////////////////////////////////////
	// 2005.3.4 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 306.490 ms
	// Vfpu unUse: 1987.210 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Mul( QUATERNION * pOut, QUATERNION * pIn1, QUATERNION * pIn2)
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_QUAT_)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c010, %1\n"				// c010 = *pq1
					"lv.q			c020, %2\n"				// c020 = *pq2
					"vqmul.q		c000, c010, c020\n"		// c000 = c010 * c020
					"sv.q			c000, %0\n"				// *pq0 = c000
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pIn1), "m"(*pIn2)
				);
			#else
				register REAL32 a, b, c, d, x, y, z, w;

				a = pIn1->vec[0];
				b = pIn1->vec[1];
				c = pIn1->vec[2];
				d = pIn1->vec[3];

				x = pIn2->vec[0];
				y = pIn2->vec[1];
				z = pIn2->vec[2];
				w = pIn2->vec[3];

				pOut->vec[0] = (d*x) + (a*w) + (b*z) - (c*y);
				pOut->vec[1] = (d*y) + (b*w) + (c*x) - (a*z);
				pOut->vec[2] = (d*z) + (c*w) + (a*y) - (b*x);
				pOut->vec[3] = (d*w) - (a*x) - (b*y) - (c*z);

				Normalize( pOut, pOut);
			#endif
		#else
			register REAL32 a, b, c, d, x, y, z, w;

			a = pIn1->vec[0];
			b = pIn1->vec[1];
			c = pIn1->vec[2];
			d = pIn1->vec[3];

			x = pIn2->vec[0];
			y = pIn2->vec[1];
			z = pIn2->vec[2];
			w = pIn2->vec[3];

			pOut->vec[0] = (d*x) + (a*w) + (b*z) - (c*y);
			pOut->vec[1] = (d*y) + (b*w) + (c*x) - (a*z);
			pOut->vec[2] = (d*z) + (c*w) + (a*y) - (b*x);
			pOut->vec[3] = (d*w) - (a*x) - (b*y) - (c*z);

			Normalize( pOut, pOut);
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.4 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 406.645 ms
	// Vfpu unUse: 2141.134 ms
	/////////////////////////////////////////////////////////////////
	inline
	void RotateAxis( QUATERNION * pOut, VEC3D * pAxis, REAL32 Rad)
	{
		#if defined( I3G_PSP)
			//#if defined(VFPU_ASM_QUAT)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c000, %2\n"				// c000 = *pv
					"mfc1			$8,   %1\n"				// t0   = angle
					"vdot.q			s010, c000, c000\n"		// s010 = s000*s000 + s001*s001 + s002*s002 + s003*s003
					"vcst.s			s020, 0x06\n"			// s020 = MFLOAT_CONST_1_PI = 1/PI
					"mtv			$8,   s021\n"			// s021 = t0 = angle
					"vmul.s			s020, s020, s021\n"		// s020 = s020 * s021 = angle * 0.5 * (2/PI)
					"vcos.s			s003, s020\n"			// s003 = cos(s020)
					"vsin.s			s020, s020\n"			// s020 = sin(s020)
					"vrsq.s			s010, s010\n"			// s010 = 1.0 / sqrt(s010)
					"vmul.s			s010, s010, s020\n"		// s010 = s010 * s020
					"vscl.t			c000, c000, s010\n"		// c000 = c000 * s010
					"sv.q			c000, %0\n"				// *pq  = c000
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "f"(Rad), "m"(*pAxis)
					: "$8"
					);
			#else
				REAL32 dist;
				REAL32 tmpf;

				//Normalize
				tmpf = i3Vector::Length( pAxis);

				if( tmpf != 0.0f)
				{
					dist = (REAL32) i3Math::sin( Rad * 0.5f) / tmpf;

					pOut->vec[0] = i3Vector::GetX( pAxis) * dist;
					pOut->vec[1] = i3Vector::GetY( pAxis) * dist;
					pOut->vec[2] = i3Vector::GetZ( pAxis) * dist;
				}
				else
				{
					pOut->vec[0] = i3Vector::GetX( pAxis);
					pOut->vec[1] = i3Vector::GetY( pAxis);
					pOut->vec[2] = i3Vector::GetZ( pAxis);
				}

				pOut->vec[3] = (REAL32) i3Math::cos( Rad * 0.5f);
			#endif
		#else
			REAL32 dist;
			REAL32 tmpf;

			//Normalize
			tmpf = i3Vector::Length( pAxis);

			if( tmpf != 0.0f)
			{	// 오차가 있어 table은 사용하지 않습니다. swoongoo.jung
				dist = (REAL32) i3Math::sin_libm( Rad * 0.5f) / tmpf;

				pOut->vec[0] = i3Vector::GetX( pAxis) * dist;
				pOut->vec[1] = i3Vector::GetY( pAxis) * dist;
				pOut->vec[2] = i3Vector::GetZ( pAxis) * dist;
			}
			else
			{
				pOut->vec[0] = i3Vector::GetX( pAxis);
				pOut->vec[1] = i3Vector::GetY( pAxis);
				pOut->vec[2] = i3Vector::GetZ( pAxis);
			}

			pOut->vec[3] = (REAL32) i3Math::cos_libm( Rad * 0.5f);
		#endif
	}

	inline
	void Lerp( QUATERNION * pOut, QUATERNION * pIn1, QUATERNION * pIn2, REAL32 t)
	{
		Set( pOut,	GetA(pIn1) + ((GetA(pIn2) - GetA(pIn1)) * t),
					GetB(pIn1) + ((GetB(pIn2) - GetB(pIn1)) * t),
					GetC(pIn1) + ((GetC(pIn2) - GetC(pIn1)) * t),
					GetD(pIn1) + ((GetD(pIn2) - GetD(pIn1)) * t));
	}

	// 2005.2.22 Lene3 수정하였습니다. (VFPU코드 수정 및 libvfpu함수 추가)
	/////////////////////////////////////////////////////////////////
	// 2005.3.4 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 508.204 ms
	// Vfpu unUse: 1545.828 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Slerp( QUATERNION * pOut, QUATERNION * pIn1, QUATERNION * pIn2, REAL32 t)
	{
		#if defined( I3G_DX)
			D3DXQuaternionSlerp( (D3DXQUATERNION *) pOut, (const D3DXQUATERNION *) pIn1, (const D3DXQUATERNION *) pIn2, t);
		#else
			i3QuaternionSLERP( pOut, pIn1, pIn2, t);	
		#endif
	}

	inline
	bool	isValid( QUATERNION * pSrc)
	{
		return i3Math::isValid( GetA( pSrc)) && i3Math::isValid( GetB( pSrc)) && i3Math::isValid( GetC( pSrc)) && i3Math::isValid( GetD( pSrc));
	}

#if defined( I3_DEBUG)
	inline void Dump( char * pszPrefix, QUATERNION * pQ)
	{
		if( pszPrefix == nullptr)
		{
			I3TRACE( "%f, %f, %f, %f\n", i3Quat::GetA( pQ), i3Quat::GetB( pQ), i3Quat::GetC( pQ), i3Quat::GetD( pQ));
		}
		else
		{
			I3TRACE( "%s : %f, %f, %f, %f\n", pszPrefix, i3Quat::GetA( pQ), i3Quat::GetB( pQ), i3Quat::GetC( pQ), i3Quat::GetD( pQ));
		}
	}
#endif
};

#endif
