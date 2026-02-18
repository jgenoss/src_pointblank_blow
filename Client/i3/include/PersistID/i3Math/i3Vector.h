// i3interfaceVector.h

#ifndef __I3_VECTOR_H
#define __I3_VECTOR_H

#include "i3Base.h"
#include "i3MathDefine.h"
#include "i3MathType.h"
#include "i3MathUtil.h"
#include "i3MathGlobalVariable.h"

#if defined( I3_PSP)
//#define		VFPU_ASM_VECTOR
//#define		VFPU_SCE_FOUNCTION		//디버그용으로 사용합니다. 
#endif

namespace i3Vector
{
	//////////////////////////////////////////////////////////////////////////////////////
	// Vector 2
	inline
	void Copy( VEC2D * pDest, VEC2D * pSrc)
	{
		pDest->vec[0] = pSrc->vec[0];
		pDest->vec[1] = pSrc->vec[1];
	}

	inline
	BOOL IsSame( VEC2D * pV1, VEC2D * pV2)
	{
		return	(pV1->vec[0] == pV2->vec[0]) &&
				(pV1->vec[1] == pV2->vec[1]);
	}

	inline
	void	Set( VEC2D * pVec, REAL32 u, REAL32 v)
	{
		pVec->vec[0] = u;
		pVec->vec[1] = v;
	}

	inline
	void	SetU( VEC2D * pVec, REAL32 val)
	{
		pVec->vec[0] = val;
	}

	inline
	void	SetX( VEC2D * pVec, REAL32 val)
	{
		SetU( pVec, val );
	}

	inline
	void	SetV( VEC2D * pVec, REAL32 val)
	{
		pVec->vec[1] = val;
	}

	inline
	void	SetY( VEC2D * pVec, REAL32 val)
	{
		SetV( pVec, val );
	}

	inline
	REAL32 GetU( VEC2D * pVec)
	{
		return pVec->vec[0];
	}

	inline
	REAL32 GetX( VEC2D * pVec)
	{
		return GetU( pVec );
	}

	inline
	REAL32 GetV( VEC2D * pVec)
	{
		return pVec->vec[1];
	}

	inline
	REAL32 GetY( VEC2D * pVec)
	{
		return GetV( pVec );
	}

	inline
	void	Zero( VEC2D * pVec)
	{
		pVec->vec[0] = 0.0f;
		pVec->vec[1] = 0.0f;
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 348.477 ms
	// Vfpu unUse: 1914.200 ms - 안에서 Sqrt가 쓰였음(1862.623 ms)
	// Vfpu unUse(function으로 호출): 2043.208 ms - 안에서 Sqrt가 쓰였음(1862.623 ms)
	/////////////////////////////////////////////////////////////////
	inline
	REAL32 Length( VEC2D *pV )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 1
				REAL32 f;

				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.s			s000, 0 + %1\n"			// s000 = pfv->x
					"lv.s			s001, 4 + %1\n"			// s001 = pfv->y
					"vmul.p			c000, c000, c000\n"		// c000 = c000 * c000
					"vadd.s			s000, s000, s001\n"		// s000 = s000 + s001
					"vsqrt.s		s000, s000\n"			// s000 = sqrtf(s000)
					"mfv			$8,   s000\n"			// t0 = s000
					"mtc1			$8,   %0\n"				// fd = t0
					".set			pop\n"					// restore assember option
					: "=f"(f)
					: "m"(*pV)
					: "$8"
				);
				return (f);
			#else
				return i3Math::Sqrt( (pV->vec[0] * pV->vec[0]) + (pV->vec[1] * pV->vec[1]));
			#endif
		#else
			return i3Math::Sqrt( (pV->vec[0] * pV->vec[0]) + (pV->vec[1] * pV->vec[1]));
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 316.247 ms
	// Vfpu unUse: 181.839 ms
	// Vfpu unUse(function으로 호출): 292.456 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32 LengthSq( const VEC2D *pV )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 0
				REAL32 f;
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.s			s000, 0 + %1\n"			// s000 = pfv->x
					"lv.s			s001, 4 + %1\n"			// s001 = pfv->y
					"vmul.p			c000, c000, c000\n"		// c000 = c000 * c000
					"vadd.s			s000, s000, s001\n"		// s000 = s000 + s001
					"mfv			$8,   s000\n"			// t0 = s000
					"mtc1			$8,   %0\n"				// fd = t0
					".set			pop\n"					// restore assember option
					: "=f"(f)
					: "m"(*pV)
					: "$8"
				);
				return (f);
			#else
				return (pV->vec[0] * pV->vec[0]) + (pV->vec[1] * pV->vec[1]);
			#endif

		#else
			return (pV->vec[0] * pV->vec[0]) + (pV->vec[1] * pV->vec[1]);
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 334.236 ms
	// Vfpu unUse: 181.778 ms  - VFPU를 사용했을때와 사용하지 않았을때 0.00...001정도의 오차가 발생
	// Vfpu unUse(function으로 호출): 316.399 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32 Dot( const VEC2D *pV1, const VEC2D *pV2 )
	{
		#if defined( I3G_PSP)
			#if defined( VFPU_ASM_VECTOR)
				REAL32 v;
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.s			s010, 0 + %1\n"			// s010 = pv0->x
					"lv.s			s011, 4 + %1\n"			// s011 = pv0->y
					"lv.s			s020, 0 + %2\n"			// s020 = pv1->x
					"lv.s			s021, 4 + %2\n"			// s021 = pv1->y
					"vdot.p			s000, c010, c020\n"		// s000 = c010 dot c020
					"mfv			$8, s000\n"				// t0 = s000
					"mtc1			$8, %0\n"				// v  = t0
					".set			pop\n"					// restore assember option
					: "=f"(v)
					: "m"(*pV1), "m"(*pV2)
					: "$8"
				);
				return v;
			#else
				return (pV1->vec[0] * pV2->vec[0]) + (pV1->vec[1] * pV2->vec[1]);
			#endif

		#else
			return (pV1->vec[0] * pV2->vec[0]) + (pV1->vec[1] * pV2->vec[1]);
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 325.559 ms
	// Vfpu unUse: 200.048 ms
	// Vfpu unUse(function으로 호출): 357.306 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Add( VEC2D *pOut, const VEC2D *pV1, const VEC2D *pV2 )
	{
		#if defined( I3G_PSP)
			#if defined( VFPU_ASM_VECTOR)
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.s 			s010, 0 + %1\n"			// s010 = pv1->x
					"lv.s 			s011, 4 + %1\n"			// s011 = pv1->y
					"lv.s			s020, 0 + %2\n"			// s020 = pv2->x
					"lv.s			s021, 4 + %2\n"			// s021 = pv2->y
					"vadd.p			c000, c010, c020\n"		// c000 = c010 + c020
					"sv.s			s000, 0 + %0\n"			// pv0->x = s000
					"sv.s			s001, 4 + %0\n"			// pv0->y = s001
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pV1), "m"(*pV2)
				);
			#else
				//2005.02.16 raja
				pOut->vec[0] = pV1->vec[0] + pV2->vec[0];
				pOut->vec[1] = pV1->vec[1] + pV2->vec[1];
			#endif

		#else
			pOut->vec[0] = pV1->vec[0] + pV2->vec[0];
			pOut->vec[1] = pV1->vec[1] + pV2->vec[1];
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 325.283 ms
	// Vfpu unUse: 201.054 ms
	// Vfpu unUse(function으로 호출): 355.139 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Sub( VEC2D *pOut, const VEC2D *pV1, const VEC2D *pV2 )
	{
		#if defined( I3G_PSP)
			#if defined( VFPU_ASM_VECTOR)
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.s 			s010, 0 + %1\n"			// s010 = pv1->x
					"lv.s 			s011, 4 + %1\n"			// s011 = pv1->y
					"lv.s			s020, 0 + %2\n"			// s020 = pv2->x
					"lv.s			s021, 4 + %2\n"			// s021 = pv2->y
					"vsub.p			c000, c010, c020\n"		// c000 = c010 - c020
					"sv.s			s000, 0 + %0\n"			// pv0->x = s000
					"sv.s			s001, 4 + %0\n"			// pv0->y = s001
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pV1), "m"(*pV2)
				);
			#else
				pOut->vec[0] = pV1->vec[0] - pV2->vec[0];
				pOut->vec[1] = pV1->vec[1] - pV2->vec[1];
			#endif

		#else
			pOut->vec[0] = pV1->vec[0] - pV2->vec[0];
			pOut->vec[1] = pV1->vec[1] - pV2->vec[1];
		#endif
	}


	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 325.424 ms
	// Vfpu unUse: 320.537 ms
	// Vfpu unUse(function으로 호출): 530.152 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Minimize( VEC2D *pOut, const VEC2D *pV1, const VEC2D *pV2 )
	{
		#if defined( I3G_PSP)
			#if defined( VFPU_ASM_VECTOR)
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.s			s010, 0 + %1\n"			// s010 = pv1->x
					"lv.s			s011, 4 + %1\n"			// s011 = pv1->y
					"lv.s			s020, 0 + %2\n"			// s020 = pv2->x
					"lv.s			s021, 4 + %2\n"			// s021 = pv2->y
					"vmin.p			c000, c010, c020\n"		// c000 = min(c010, c020)
					"sv.s			s000, 0 + %0\n"			// pv0->x = s000
					"sv.s			s001, 4 + %0\n"			// pv0->y = s001
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pV1), "m"(*pV2)
				);
			#else
				pOut->vec[0] = MIN( pV1->vec[0], pV2->vec[0]);
				pOut->vec[1] = MIN( pV1->vec[1], pV2->vec[1]);
			#endif

		#else
			pOut->vec[0] = MIN( pV1->vec[0], pV2->vec[0]);
			pOut->vec[1] = MIN( pV1->vec[1], pV2->vec[1]);
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 325.424 ms
	// Vfpu unUse: 320.537 ms
	// Vfpu unUse(function으로 호출): 530.152 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Maximize( VEC2D *pOut, const VEC2D *pV1, const VEC2D *pV2 )
	{
		#if defined( I3G_PSP)
			#if defined( VFPU_ASM_VECTOR)
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.s			s010, 0 + %1\n"			// s010 = pv1->x
					"lv.s			s011, 4 + %1\n"			// s011 = pv1->y
					"lv.s			s020, 0 + %2\n"			// s020 = pv2->x
					"lv.s			s021, 4 + %2\n"			// s021 = pv2->y
					"vmax.p			c000, c010, c020\n"		// c000 = max(c010, c020)
					"sv.s			s000, 0 + %0\n"			// pv0->x = s000
					"sv.s			s001, 4 + %0\n"			// pv0->y = s001
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pV1), "m"(*pV2)
				);
			#else
				pOut->vec[0] = MAX( pV1->vec[0], pV2->vec[0]);
				pOut->vec[1] = MAX( pV1->vec[1], pV2->vec[1]);
			#endif

		#else
			pOut->vec[0] = MAX( pV1->vec[0], pV2->vec[0]);
			pOut->vec[1] = MAX( pV1->vec[1], pV2->vec[1]);
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 339.248 ms
	// Vfpu unUse: 201.063 ms
	// Vfpu unUse(function으로 호출): 355.200 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Scale( VEC2D *pOut, const VEC2D *pV, REAL32 s )
	{
		#if defined( I3G_PSP)
			#if defined( VFPU_ASM_VECTOR)
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,   %2\n"				// t0   = s
					"mtv			$8,   s010\n"			// s010 = t0
					"lv.s			s000, 0 + %1\n"			// s000 = pv1->x
					"lv.s			s001, 4 + %1\n"			// s001 = pv1->y
					"vscl.p			c000, c000, s010\n"		// c000 = c000 * s010
					"sv.s			s000, 0 + %0\n"			// pv0->x = s000
					"sv.s			s001, 4 + %0\n"			// pv0->y = s001
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pV), "f"(s)
					: "$8"
				);
			#else
				pOut->vec[0] = pV->vec[0] * s;
				pOut->vec[1] = pV->vec[1] * s;
			#endif

		#else
			pOut->vec[0] = pV->vec[0] * s;
			pOut->vec[1] = pV->vec[1] * s;
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 385.938 ms
	// Vfpu unUse: 245.519 ms
	// Vfpu unUse(function으로 호출): 411.835 ms
	/////////////////////////////////////////////////////////////////
	inline
	void ScaleAdd( VEC2D *pOut, const VEC2D *pV, REAL32 s )
	{
		pOut->vec[0] += pV->vec[0] * s;
		pOut->vec[1] += pV->vec[1] * s;
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 416.556 ms
	// Vfpu unUse: 291.360 ms
	// Vfpu unUse(function으로 호출): 475.815 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Lerp( VEC2D *pOut, const VEC2D *pV1, const VEC2D *pV2, REAL32 s )
	{
		#if defined( I3G_PSP)
			#if defined( VFPU_ASM_VECTOR)
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,   %3\n"				// t0   = t
					"mtv			$8,   s020\n"			// s010 = t0
					"lv.s			s000, 0 + %1\n"			// s000 = pV1->x
					"lv.s			s001, 4 + %1\n"			// s001 = pV1->y
					"lv.s			s010, 0 + %2\n"			// s010 = pV2->x
					"lv.s			s011, 4 + %2\n"			// s011 = pV2->y
					"vsub.p			c010, c010, c000\n"		// c010 = c010 - c000
					"vscl.p			c010, c010, s020\n"		// c000 = c000 * s010
					"vadd.p			c000, c000, c010\n"		// c000 = c000 + c010
					"sv.s			s000, 0 + %0\n"			// pv0->x = s000
					"sv.s			s001, 4 + %0\n"			// pv0->y = s001
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pV1), "m"(*pV2), "f"(s)
					: "$8"
				);
			#else
				pOut->vec[0] = pV1->vec[0] + ((pV2->vec[0] - pV1->vec[0]) * s);
				pOut->vec[1] = pV1->vec[1] + ((pV2->vec[1] - pV1->vec[1]) * s);
			#endif

		#else
			pOut->vec[0] = pV1->vec[0] + ((pV2->vec[0] - pV1->vec[0]) * s);
			pOut->vec[1] = pV1->vec[1] + ((pV2->vec[1] - pV1->vec[1]) * s);
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 371.635 ms
	// Vfpu unUse: 2395.573 ms - 안에서 Sqrt가 쓰였음(1862.623 ms)
	// Vfpu unUse(function으로 호출): 2579.760 ms - 안에서 Sqrt가 쓰였음(1862.623 ms)
	/////////////////////////////////////////////////////////////////
	inline
	REAL32 Normalize( VEC2D *pOut, const VEC2D *pV )
	{
		REAL32 len;

		#if defined( I3G_PSP)
			#if defined( VFPU_ASM_VECTOR)
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.s			s000, 0 + %2\n"			// s000 = pv1->x
					"lv.s			s001, 4 + %2\n"			// s001 = pv1->y
					"vmul.p			c010, c000, c000\n"		// c010 = c000 * c000
					"vadd.s			s010, s010, s011\n"		// s010 = s010 + s011
					"vsqrt.s		s011, s010\n"			// s010 = 1 / sqrtf(s010)
					"sv.s			s011, %1\n"
					"vrcp.s			s010, s011\n"
					"vscl.p			c000, c000, s010\n"		// c000 = c000 * s010
					"sv.s			s000, 0 + %0\n"			// pv0->x = s000
					"sv.s			s001, 4 + %0\n"			// pv0->y = s001
					".set			pop\n"					// restore assember option
					: "=m"(*pOut), "=f"(len)
					: "m"(*pV)
				);
			#else
				len = Length( (VEC2D *) pV);
				REAL32 rlen = i3Math::Reciprocal( len);

				pOut->vec[0] = pV->vec[0] * rlen;
				pOut->vec[1] = pV->vec[1] * rlen;
			#endif
		#else
			len = Length( (VEC2D *) pV);
			REAL32 rlen = i3Math::Reciprocal( len);

			pOut->vec[0] = pV->vec[0] * rlen;
			pOut->vec[1] = pV->vec[1] * rlen;
		#endif

		return len;
	}

	inline
	void Transform( VEC4D *pOut, const VEC2D *pV, const MATRIX *pM )
	{
		VEC2D temp;
		Copy( &temp, (VEC2D *) pV);			
		pOut->vec[0] = (temp.vec[0] * pM->m[0][0]) + (temp.vec[1] * pM->m[0][1]) + pM->m[0][2] + pM->m[0][3];
		pOut->vec[1] = (temp.vec[0] * pM->m[1][0]) + (temp.vec[1] * pM->m[1][1]) + pM->m[1][2] + pM->m[1][3];
		pOut->vec[2] = (temp.vec[0] * pM->m[2][0]) + (temp.vec[1] * pM->m[2][1]) + pM->m[2][2] + pM->m[2][3];
		pOut->vec[3] = (temp.vec[0] * pM->m[3][0]) + (temp.vec[1] * pM->m[3][1]) + pM->m[3][2] + pM->m[3][3];
	}

	inline
	void TransformCoord( VEC2D *pOut, const VEC2D *pV, const MATRIX *pM )
	{
		VEC2D temp;

		Copy( &temp, (VEC2D *) pV);
		pOut->vec[0] = (temp.vec[0] * pM->m[0][0]) + (temp.vec[1] * pM->m[0][1]);
		pOut->vec[1] = (temp.vec[0] * pM->m[1][0]) + (temp.vec[1] * pM->m[1][1]);
	}

	inline
	void TransformNormal( VEC2D *pOut, const VEC2D *pV, const MATRIX *pM )
	{
		VEC2D temp;
		Copy( &temp, (VEC2D *)pV);
		pOut->vec[0] = (pV->vec[0] * pM->m[0][0]) + (pV->vec[1] * pM->m[0][1]);
		pOut->vec[1] = (pV->vec[0] * pM->m[1][0]) + (pV->vec[1] * pM->m[1][1]);
	}

	///////////////////////////////////////////////////////////////////////////////////////
	// Vector 3
	inline
	void	Copy( VEC3D * pDest, VEC3D * pSrc)
	{
		pDest->vec[0] = pSrc->vec[0];
		pDest->vec[1] = pSrc->vec[1];
		pDest->vec[2] = pSrc->vec[2];
	}

	inline
	BOOL IsSame( VEC3D * pV1, VEC3D * pV2)
	{
		return	(pV1->vec[0] == pV2->vec[0]) &&
				(pV1->vec[1] == pV2->vec[1]) &&
				(pV1->vec[2] == pV2->vec[2]);
	}

	inline
	bool	isAlike( VEC3D * pV1, VEC3D * pV2, REAL32 eps = I3_EPS)
	{
		return	i3Math::isAlike( pV1->x, pV2->x, eps) &&
				i3Math::isAlike( pV1->y, pV2->y, eps) &&
				i3Math::isAlike( pV1->z, pV2->z, eps);
	}

	inline
	void	Set( VEC3D * pVec, REAL32 x, REAL32 y, REAL32 z)
	{
		pVec->vec[0] = x;
		pVec->vec[1] = y;
		pVec->vec[2] = z;
	}

	inline
	void	SetX( VEC3D * pVec, REAL32 x)
	{
		pVec->vec[0] = x;
	}

	inline
	void	SetY( VEC3D * pVec, REAL32 y)
	{
		pVec->vec[1] = y;
	}

	inline
	void	SetZ( VEC3D * pVec, REAL32 z)
	{
		pVec->vec[2] = z;
	}

	inline
	void	Zero( VEC3D * pVec)
	{
		pVec->vec[0] = pVec->vec[1] = pVec->vec[2] = 0.0f;
	}

	inline
	REAL32	GetX( VEC3D * pVec)
	{
		return pVec->vec[0];
	}

	inline
	REAL32 GetY( VEC3D * pVec)
	{
		return pVec->vec[1];
	}

	inline
	REAL32 GetZ( VEC3D * pVec)
	{
		return pVec->vec[2];
	}

	inline
	void AddX( VEC3D * pVec, REAL32 r)
	{
		pVec->vec[0] += r;
	}

	inline
	void AddY( VEC3D * pVec, REAL32 r)
	{
		pVec->vec[1] += r;
	}

	inline
	void AddZ( VEC3D * pVec, REAL32 r)
	{
		pVec->vec[2] += r;
	}

	inline
	void Add( VEC2D * pVec, REAL32 x, REAL32 y)
	{
		pVec->vec[0] += x;
		pVec->vec[1] += y;
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 340.252 ms
	// Vfpu unUse: 264.777 ms
	// Vfpu unUse(function으로 호출): 443.815 ms
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	// 2005.10.7 Lene3 
	// PC Test(Release Build)
	// Test Result (1000*1000 loop)
	// DX  : 3.674433 ms
	// CPU : 4.273655 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Add( VEC3D * pVec, REAL32 x, REAL32 y, REAL32 z)
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 0
			__asm__ (
				".set			push\n"					// save assember option
				".set			noreorder\n"			// suppress reordering
				"lv.q			c000, %0\n"				// s000 = pv1->x
				"mfc1			$8,   %1\n"				// t0   = t
				"mtv			$8,   s010\n"			// s020 = t0
				"mfc1			$8,   %2\n"				// t0   = t
				"mtv			$8,   s011\n"			// s020 = t0
				"mfc1			$8,   %3\n"				// t0   = t
				"mtv			$8,   s012\n"			// s020 = t0
				"vadd.t			c000, c000, c010\n"		// c000 = c000 + c010
				"sv.s			s000, 0 + %0\n"			// pv0->x = s000
				"sv.s			s001, 4 + %0\n"				// pv0->x = s000
				"sv.s			s002, 8 + %0\n"				// pv0->x = s000
				".set			pop\n"					// restore assember option
				: "=m"(*pVec)
				: "f"(x), "f"(y), "f"(z)
				: "$8"
				);
			#else
				pVec->vec[0] += x;
				pVec->vec[1] += y;
				pVec->vec[2] += z;
			#endif

		#else
			pVec->vec[0] += x;
			pVec->vec[1] += y;
			pVec->vec[2] += z;
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 275.160 ms
	// Vfpu unUse: 266.567 ms
	// Vfpu unUse(function으로 호출): 436.460 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Mul( VEC3D * pOut, VEC3D * pVec1, VEC3D * pVec2)
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 0
			__asm__ (
				".set			push\n"					// save assember option
				".set			noreorder\n"			// suppress reordering
				"lv.q			c000, 0 + %1\n"			// s000 = pVec1
				"lv.q			c010, 0 + %2\n"			// s010 = pVec2
				"vmul.t			c000, c000, c010\n"		// s000 = s000 + s010
				"sv.s			s000, 0 + %0\n"			// pv0->x = s000
				"sv.s			s001, 4 + %0\n"			// pv0->x = s000
				"sv.s			s002, 8 + %0\n"			// pv0->x = s000
				".set			pop\n"					// restore assember option
				: "=m"(*pOut)
				: "m"(*pVec1), "m"(*pVec2)
				);
			#else
				pOut->vec[0] = pVec1->vec[0] * pVec2->vec[0];
				pOut->vec[1] = pVec1->vec[1] * pVec2->vec[1];
				pOut->vec[2] = pVec1->vec[2] * pVec2->vec[2];
			#endif
		#else
			pOut->vec[0] = pVec1->vec[0] * pVec2->vec[0];
			pOut->vec[1] = pVec1->vec[1] * pVec2->vec[1];
			pOut->vec[2] = pVec1->vec[2] * pVec2->vec[2];
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 320.111 ms
	// Vfpu unUse: 236.085 ms
	// Vfpu unUse(function으로 호출): 356.347 ms
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	// 2005.10.7 Lene3 
	// PC Test(Release Build)
	// Test Result (1000*1000 loop)
	// DX  : 1.351228 ms
	// CPU : 1.019180 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32 LengthSq( const VEC3D *pV )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 0
				REAL32 f;
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c000, %1\n"			// s000 = pfv->x
					"vmul.t			c010, c000, c000\n"		// c000 = c000 * c000
					"vfad.t			s020, c010\n"			// s000 = s000 + s001 + s002
					"mfv			$8,   s020\n"			// t0 = s000
					"mtc1			$8,   %0\n"				// fd = t0
					".set			pop\n"					// restore assember option
					: "=f"(f)
					: "m"(*pV)
					: "$8"
				);
				return f;
			#else
				return (pV->vec[0] * pV->vec[0]) + (pV->vec[1] * pV->vec[1]) + (pV->vec[2] * pV->vec[2]);
			#endif

		#else
			return (pV->vec[0]*pV->vec[0]) + (pV->vec[1]*pV->vec[1]) + (pV->vec[2]*pV->vec[2]);
		#endif
	}


	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 360.144 ms - VFPU사용시 -0.000004의 오차가 발생하였음
	// Vfpu unUse: 2123.970 ms - 안에서 Sqrt가 쓰였음(1862.623 ms)
	// Vfpu unUse(function으로 호출): 2259.585 ms - 안에서 Sqrt가 쓰였음(1862.623 ms)
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	// 2005.10.7 Lene3 
	// PC Test(Release Build)
	// Test Result (1000*1000 loop)
	// DX  : 1.653147 ms
	// CPU : 2.964136 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32 Length( const VEC3D *pV )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 1
				REAL32 f;
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c000, %1\n"				// s000 = pfv->x
					"vmul.t			c000, c000, c000\n"		// c000 = c000 * c000
					"vfad.t			s000, c000\n"			// s000 = s000 + s001 + s002
					"vsqrt.s		s000, s000\n"			// s000 = sqrt(s000)
					"mfv			$8,   s000\n"			// t0 = s000
					"mtc1			$8,   %0\n"				// fd = t0
					".set			pop\n"					// restore assember option
					: "=f"(f)
					: "m"(*pV)
					: "$8"
				);
				return f;
			#else
				return i3Math::Sqrt( LengthSq( pV));				
				
			#endif

		#else
			return i3Math::Sqrt( LengthSq( pV));
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 344.010 ms
	// Vfpu unUse: 237.501 ms
	// Vfpu unUse(function으로 호출): 379.878 ms
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	// 2005.10.7 Lene3 
	// PC Test(Release Build)
	// Test Result (1000*1000 loop)
	// DX  : 2.310689 ms
	// CPU : 2.921010 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32 Dot( const VEC3D *pV1, const VEC3D *pV2 )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 0
				REAL32 v;

				// 1.5.0 Final
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c000, %1\n"				// s000 = pv1->x
					"lv.q			c010, %2\n"				// s010 = pv2->x
					"vdot.t			s000, c000, c010\n"		// s000 = c000 dot c010
					"sv.s			s000, %0\n"				// v    = s000
					".set			pop\n"					// restore assember option
					: "=m"(v)
					: "m"(*pV1), "m"(*pV2)
					);

				return v;
			#else
				return	(pV1->vec[0] * pV2->vec[0]) + 
						(pV1->vec[1] * pV2->vec[1]) +
						(pV1->vec[2] * pV2->vec[2]);
			#endif

		#else
			return	(pV1->vec[0] * pV2->vec[0]) + 
					(pV1->vec[1] * pV2->vec[1]) +
					(pV1->vec[2] * pV2->vec[2]);
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 380.035 ms
	// Vfpu unUse: 417.112 ms
	// Vfpu unUse(function으로 호출): 626.474 ms
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	// 2005.10.7 Lene3 
	// PC Test(Release Build)
	// Test Result (1000*1000 loop)
	// DX  : 1.382440 ms
	// CPU : 1.064541 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Cross( VEC3D *pOut, const VEC3D *pV1, const VEC3D *pV2 )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 1
				// 1.5.0 Final
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c010, %1\n"				// s010 = pv1->x
					"lv.q			c020, %2\n"				// s020 = pv2->x
					"vcrsp.t		c000, c010, c020\n"		// c000 = c010 x c020
					"sv.s			s000, 0 + %0\n"			// pv0->x = s000
					"sv.s			s001, 4 + %0\n"			// pv0->x = s000
					"sv.s			s002, 8 + %0\n"			// pv0->x = s000
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pV1), "m"(*pV2)
					);
				
			#else
				pOut->vec[0] = (pV1->vec[1] * pV2->vec[2]) - (pV1->vec[2] * pV2->vec[1]);
				pOut->vec[1] = (pV1->vec[2] * pV2->vec[0]) - (pV1->vec[0] * pV2->vec[2]);
				pOut->vec[2] = (pV1->vec[0] * pV2->vec[1]) - (pV1->vec[1] * pV2->vec[0]);
			#endif

		#else
			I3ASSERT( pOut != pV1);
			I3ASSERT( pOut != pV2);
			pOut->vec[0] = (pV1->vec[1] * pV2->vec[2]) - (pV1->vec[2] * pV2->vec[1]);
			pOut->vec[1] = (pV1->vec[2] * pV2->vec[0]) - (pV1->vec[0] * pV2->vec[2]);
			pOut->vec[2] = (pV1->vec[0] * pV2->vec[1]) - (pV1->vec[1] * pV2->vec[0]);
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 366.410 ms
	// Vfpu unUse: 264.578 ms
	// Vfpu unUse(function으로 호출): 439.015 ms
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	// 2005.10.7 Lene3 
	// PC Test(Release Build)
	// Test Result (1000*1000 loop)
	// DX  : 1.998061 ms
	// CPU : 3.261294 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Add( VEC3D *pOut, const VEC3D *pV1, const VEC3D *pV2 )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 0
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c000, %1\n"			// s000 = pv1->x
					"lv.q			c010, %2\n"			// s010 = pv2->x
					"vadd.t			c000, c000, c010\n"		// c000 = c000 + c010
					"sv.s			s000, 0 + %0\n"			// pv0->x = s000
					"sv.s			s001, 4 + %0\n"			// pv0->x = s000
					"sv.s			s002, 8 + %0\n"			// pv0->x = s000
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pV1), "m"(*pV2)
				);
			#else
				pOut->vec[0] = pV1->vec[0] + pV2->vec[0];
				pOut->vec[1] = pV1->vec[1] + pV2->vec[1];
				pOut->vec[2] = pV1->vec[2] + pV2->vec[2];
			#endif

		#else
			pOut->vec[0] = pV1->vec[0] + pV2->vec[0];
			pOut->vec[1] = pV1->vec[1] + pV2->vec[1];
			pOut->vec[2] = pV1->vec[2] + pV2->vec[2];
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 366.922 ms
	// Vfpu unUse: 264.539 ms
	// Vfpu unUse(function으로 호출): 446.166 ms
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	// 2005.10.7 Lene3 
	// PC Test(Release Build)
	// Test Result (1000*1000 loop)
	// DX  : 2.000613 ms
	// CPU : 3.167380 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Sub( VEC3D *pOut, const VEC3D *pV1, const VEC3D *pV2 )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 0
			__asm__ (
				".set			push\n"					// save assember option
				".set			noreorder\n"			// suppress reordering
				"lv.q			c000, %1\n"				// c000 = pv1->x.y.z
				"lv.q			c010, %2\n"				// c010 = pv2->x.y.z
				"vsub.t			c000, c000, c010\n"		// c000 = c000 - c010
				"sv.s			s000, 0 + %0\n"			// pv0->x.y.z = c000
				"sv.s			s001, 4 + %0\n"			// pv0->x.y.z = c000
				"sv.s			s002, 8 + %0\n"			// pv0->x.y.z = c000
				".set			pop\n"					// restore assember option
				: "=m"(*pOut)
				: "m"(*pV1), "m"(*pV2)
				);
			#else
				pOut->vec[0] = pV1->vec[0] - pV2->vec[0];
				pOut->vec[1] = pV1->vec[1] - pV2->vec[1];
				pOut->vec[2] = pV1->vec[2] - pV2->vec[2];
			#endif

		#else
			pOut->vec[0] = pV1->vec[0] - pV2->vec[0];
			pOut->vec[1] = pV1->vec[1] - pV2->vec[1];
			pOut->vec[2] = pV1->vec[2] - pV2->vec[2];
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 289.964 ms
	// Vfpu unUse(function으로 호출): 706.478 ms
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	// 2005.10.7 Lene3 
	// PC Test(Release Build)
	// Test Result (1000*1000 loop)
	// DX  : 2.000613 ms
	// CPU : 3.167380 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Minimize( VEC3D *pOut, const VEC3D *pV1, const VEC3D *pV2 )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c000, %1\n"				// c000 = pv1->x.y.z
					"lv.q			c010, %2\n"				// c010 = pv2->x.y.z
					"vmin.t			c000, c000, c010\n"		// c000 = min(c000, c010)
					"sv.s			s000, 0 + %0\n"			// pv0->x.y.z = c000
					"sv.s			s001, 4 + %0\n"			// pv0->x.y.z = c000
					"sv.s			s002, 8 + %0\n"			// pv0->x.y.z = c000
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pV1), "m"(*pV2)
				);				
			#else
				pOut->vec[0] = MIN( pV1->vec[0], pV2->vec[0]);
				pOut->vec[1] = MIN( pV1->vec[1], pV2->vec[1]);
				pOut->vec[2] = MIN( pV1->vec[2], pV2->vec[2]);
			#endif
			
		#else
			pOut->vec[0] = MIN( pV1->vec[0], pV2->vec[0]);
			pOut->vec[1] = MIN( pV1->vec[1], pV2->vec[1]);
			pOut->vec[2] = MIN( pV1->vec[2], pV2->vec[2]);
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 284.750 ms
	// Vfpu unUse(function으로 호출): 700.314 ms
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	// 2005.10.7 Lene3 
	// PC Test(Release Build)
	// Test Result (1000*1000 loop)
	// DX  : 1.743525 ms
	// CPU : 3.248562 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Maximize( VEC3D *pOut, const VEC3D *pV1, const VEC3D *pV2 )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c000, %1\n"				// c000 = pv1->x.y.z
					"lv.q			c010, %2\n"				// c010 = pv2->x.y.z
					"vmax.t			c000, c000, c010\n"		// c000 = max(c000, c010)
					"sv.s			s000, 0 + %0\n"			// pv0->x.y.z = c000
					"sv.s			s001, 4 + %0\n"			// pv0->x.y.z = c000
					"sv.s			s002, 8 + %0\n"			// pv0->x.y.z = c000
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pV1), "m"(*pV2)
				);
			#else
				pOut->vec[0] = MAX( pV1->vec[0], pV2->vec[0]);
				pOut->vec[1] = MAX( pV1->vec[1], pV2->vec[1]);
				pOut->vec[2] = MAX( pV1->vec[2], pV2->vec[2]);
			#endif

		#else
			pOut->vec[0] = MAX( pV1->vec[0], pV2->vec[0]);
			pOut->vec[1] = MAX( pV1->vec[1], pV2->vec[1]);
			pOut->vec[2] = MAX( pV1->vec[2], pV2->vec[2]);
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 약간의 문제가 있는듯함!
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 307.046 ms
	// Vfpu unUse(function으로 호출): 440.100 ms
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	// 2005.10.7 Lene3 
	// PC Test(Release Build)
	// Test Result (1000*1000 loop)
	// DX  : 2.962741 ms
	// CPU : 2.197615 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Scale( VEC3D *pOut, const VEC3D *pV, REAL32 s)
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,   %2\n"				// t0   = t
					"mtv			$8,   s010\n"			// s010 = t0
					"lv.q			c000, %1\n"				// s000 = pv1->x
					"vscl.t			c000, c000, s010\n"		// c000 = c000 * s010
					"sv.s			s000, 0 + %0\n"			// pv0->x.y.z = c000
					"sv.s			s001, 4 + %0\n"			// pv0->x.y.z = c000
					"sv.s			s002, 8 + %0\n"			// pv0->x.y.z = c000
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pV), "f"(s)
					: "$8"
				);
			#else
				pOut->vec[0] = pV->vec[0] * s;
				pOut->vec[1] = pV->vec[1] * s;
				pOut->vec[2] = pV->vec[2] * s;
			#endif

		#else
			pOut->vec[0] = pV->vec[0] * s;
			pOut->vec[1] = pV->vec[1] * s;
			pOut->vec[2] = pV->vec[2] * s;
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// 약간의 문제가 있는듯함!
	// Test Result (1000*1000 loop)
	// Vfpu Use: 330.556 ms
	// Vfpu unUse(function으로 호출): 521.598 ms
	/////////////////////////////////////////////////////////////////
	inline
	void ScaleAdd( VEC3D *pOut, const VEC3D *pV, REAL32 s)
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,   %2\n"				// t0   = t
					"mtv			$8,   s030\n"			// s020 = t0
					"lv.q			c000, %0\n"				// s000 = pOut->x					
					"lv.q			c010, %1\n"				// s010 = pV->x
					"vscl.t			c010, c010, s030\n"		// c010 = c010 * s020
					"vadd.t			c000, c000, c010\n"		// c000 = c000 + c010
					"sv.s			s000, 0 + %0\n"			// pv0->x.y.z = c000
					"sv.s			s001, 4 + %0\n"			// pv0->x.y.z = c000
					"sv.s			s002, 8 + %0\n"			// pv0->x.y.z = c000
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pV), "f"(s)
					: "$8"
				);
			#else
				pOut->vec[0] += pV->vec[0] * s;
				pOut->vec[1] += pV->vec[1] * s;
				pOut->vec[2] += pV->vec[2] * s;
			#endif

		#else
			pOut->vec[0] += pV->vec[0] * s;
			pOut->vec[1] += pV->vec[1] * s;
			pOut->vec[2] += pV->vec[2] * s;
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 307.606 ms
	// Vfpu unUse(function으로 호출): 521.212 ms
	/////////////////////////////////////////////////////////////////
	inline
	void MulAdd( VEC3D *pOut, const VEC3D *pV1, VEC3D * pV2)
	{
		#if defined( I3G_PSP)
			#if 1 //defined( VFPU_ASM_VECTOR)
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c000, %1\n"				// s000 = pOut->x
					"lv.q			c010, %2\n"				// s001 = pOut->y
					"lv.q			c020, %0\n"				// s002 = pOut->z
					"vmul.t			c000, c000, c010\n"
					"vadd.t			c000, c020, c000\n"		// c000 = c000 + c010
					"sv.s			s000, 0 + %0\n"			// pv0->x = s000
					"sv.s			s001, 4 + %0\n"			// pv0->x = s000
					"sv.s			s002, 8 + %0\n"			// pv0->x = s000
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pV1), "m"(*pV2)
				);
			#else
				pOut->vec[0] += pV1->vec[0] * pV2->vec[0];
				pOut->vec[1] += pV1->vec[1] * pV2->vec[1];
				pOut->vec[2] += pV1->vec[2] * pV2->vec[2];
			#endif

		#else
			pOut->vec[0] += pV1->vec[0] * pV2->vec[0];
			pOut->vec[1] += pV1->vec[1] * pV2->vec[1];
			pOut->vec[2] += pV1->vec[2] * pV2->vec[2];
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 379.694 ms
	// Vfpu unUse(function으로 호출): 607.463 ms
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	// 2005.10.7 Lene3 
	// PC Test(Release Build)
	// Test Result (1000*1000 loop)
	// DX  : 1.280222 ms
	// CPU : 3.059048 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Lerp( VEC3D *pOut, const VEC3D *pV1, const VEC3D *pV2, REAL32 s )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 1
				// 1.5.0b버젼
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,   %3\n"				// t0   = t
					"mtv			$8,   s030\n"			// s030 = t0
					"lv.q			c010, %1\n"				// c010 = *pv1
					"lv.q			c020, %2\n"				// c020 = *pv2
					"vsub.t			c000, c020, c010\n"		// c000 = c020 - c010 = (v2 - v1)
					"vscl.t			c000, c000, s030\n"		// c000 = c000 * s030 = (v2 - v1) * t
					"vadd.t			c010, c010, c000\n"		// c010 = c010 + c000 = v1 + t * (v2 - v1)
					"sv.s			s010, 0 + %0\n"			// *pv0 = c010
					"sv.s			s011, 4 + %0\n"			// *pv0 = c010
					"sv.s			s012, 8 + %0\n"			// *pv0 = c010
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pV1), "m"(*pV2), "f"(s)
					: "$8"
				);				
			#else
				pOut->vec[0] = pV1->vec[0] + ((pV2->vec[0] - pV1->vec[0]) * s);
				pOut->vec[1] = pV1->vec[1] + ((pV2->vec[1] - pV1->vec[1]) * s);
				pOut->vec[2] = pV1->vec[2] + ((pV2->vec[2] - pV1->vec[2]) * s);
			#endif

		#else
			pOut->vec[0] = pV1->vec[0] + ((pV2->vec[0] - pV1->vec[0]) * s);
			pOut->vec[1] = pV1->vec[1] + ((pV2->vec[1] - pV1->vec[1]) * s);
			pOut->vec[2] = pV1->vec[2] + ((pV2->vec[2] - pV1->vec[2]) * s);
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 344.675 ms
	// Vfpu unUse(function으로 호출): 3025.476 ms
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	// 2005.10.7 Lene3 
	// PC Test(Release Build)
	// Test Result (1000*1000 loop)
	// DX  : 30.062818 ms
	// CPU : 10.765868 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32 Normalize( VEC3D *pOut, const VEC3D *pV )
	{
		REAL32 len;

		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c000, %2\n"				// s000 = pv1->x
					"vdot.t			s010, c000, c000\n"		// s010 = s000*s000 + s001*s001 + s002*s002
					"vzero.s		s011\n"					// s011 = 0
					"vcmp.s			EZ, s010\n"				// CC[0] = (s010==0.0f)
					"vsqrt.s		s020, s010\n"			// s010 = 1.0 / sqrt(s010)
					"vrcp.s			s010, s020\n"
					"sv.s			s020, %1\n"
					"vcmovt.s		s010, s011, 0\n"		// if (CC[0]) s010 = s011
					"vscl.t			c000[-1:1,-1:1,-1:1], c000, s010\n"	// c000 = c000 * s010
					"sv.s			s000, 0 + %0\n"			// pv0->x = s000			
					"sv.s			s001, 4 + %0\n"			// pv0->x = s000			
					"sv.s			s002, 8 + %0\n"			// pv0->x = s000			
					".set			pop\n"					// restore assember option
					: "=m"(*pOut), "=f"(len)
					: "m"(*pV)
					);

				return pOut;
			#else
				len = Length( pV);

				if( len > 0.00001f)
					Scale( pOut, pV, i3Math::Reciprocal( len));
			#endif
		#else
			len = Length( pV);

			if( len > 0.00001f)
				Scale( pOut, pV, i3Math::Reciprocal( len));
			else
				Copy( (VEC3D *) pOut, (VEC3D *) pV);
		#endif

		return len;
	}

	inline
	void Normal( VEC3D *pOut, const VEC3D *pV1, const VEC3D *pV2 , const VEC3D *pV3 )
	{
		VEC3D d1;
		VEC3D d2;
		Sub( &d1, pV1, pV2);
		Sub( &d2, pV3, pV2);
		Cross(pOut, &d2, &d1);
		Normalize( pOut, pOut);
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 322.215 ms
	// Vfpu unUse(function으로 호출): 1387.693 ms
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	// 2005.10.7 Lene3 
	// PC Test(Release Build)
	// Test Result (1000*1000 loop)
	// DX  : 35.638717 ms
	// CPU : 1.127387 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Transform( VEC4D *pOut, const VEC3D *pV, const MATRIX *pM )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c100,  0 + %1\n"		// c100 = pm0->x
					"lv.q			c110, 16 + %1\n"		// c110 = pm0->y
					"lv.q			c120, 32 + %1\n"		// c120 = pm0->z
					"lv.q			c130, 48 + %1\n"		// c130 = pm0->w
					"lv.q			c200, %2\n"				// c200 = *pv1
					"vone.s			s203\n"					// s203 = 0.0f;
					"vtfm4.q		c000, e100, c200\n"		// c000 = e100 * c200
					"sv.q			c000, %0\n"				// *pv0 = c000
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pM), "m"(*pV)
				);
			#else
				VEC3D temp;
				Copy( &temp, (VEC3D *)pV);
				pOut->vec[0] = (pM->m[0][0] * temp.vec[0]) + (pM->m[1][0] * temp.vec[1]) + (pM->m[2][0] * temp.vec[2]) + pM->m[3][0];
				pOut->vec[1] = (pM->m[0][1] * temp.vec[0]) + (pM->m[1][1] * temp.vec[1]) + (pM->m[2][1] * temp.vec[2]) + pM->m[3][1];
				pOut->vec[2] = (pM->m[0][2] * temp.vec[0]) + (pM->m[1][2] * temp.vec[1]) + (pM->m[2][2] * temp.vec[2]) + pM->m[3][2];
				pOut->vec[3] = (pM->m[0][3] * temp.vec[0]) + (pM->m[1][3] * temp.vec[1]) + (pM->m[2][3] * temp.vec[2]) + pM->m[3][3];
			#endif

		#else
			VEC3D temp;
			Copy( &temp, (VEC3D *)pV);
			pOut->vec[0] = (pM->m[0][0] * temp.vec[0]) + (pM->m[1][0] * temp.vec[1]) + (pM->m[2][0] * temp.vec[2]) + pM->m[3][0];
			pOut->vec[1] = (pM->m[0][1] * temp.vec[0]) + (pM->m[1][1] * temp.vec[1]) + (pM->m[2][1] * temp.vec[2]) + pM->m[3][1];
			pOut->vec[2] = (pM->m[0][2] * temp.vec[0]) + (pM->m[1][2] * temp.vec[1]) + (pM->m[2][2] * temp.vec[2]) + pM->m[3][2];
			pOut->vec[3] = (pM->m[0][3] * temp.vec[0]) + (pM->m[1][3] * temp.vec[1]) + (pM->m[2][3] * temp.vec[2]) + pM->m[3][3];
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 322.074 ms
	// Vfpu unUse(function으로 호출): 1736.063 ms
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	// 2005.10.7 Lene3 
	// PC Test(Release Build)
	// Test Result (1000*1000 loop)
	// DX  : 38.349116 ms
	// CPU : 2.771796 ms
	// 주의: 두개의 결과값에 미묘한 차이가 있음(아주 약간의 오차발생!) 
	//      ex) DX  : -9.999999, 1.000000, 2.100000
	//			CPU : -10.000000, 1.000000, 2.100000
	/////////////////////////////////////////////////////////////////
	inline
	void TransformCoord( VEC3D *pOut, const VEC3D *pV, const MATRIX *pM )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c100,  0 + %1\n"		// c100 = pm0->x
					"lv.q			c110, 16 + %1\n"		// c110 = pm0->y
					"lv.q			c120, 32 + %1\n"		// c120 = pm0->z
					"lv.q			c130, 48 + %1\n"		// c120 = pm0->z
					"lv.q			c200, %2\n"				// c200 = *pv1
					"vone.s			s203\n"					// s003 = s203
					"vtfm4.q		c000, e100, c200\n"		// c000 = e100 * c200
					
					"vrcp.s			s003,	s003\n"
					"vmul.s			s000, s000, s003\n"
					"vmul.s			s001, s001, s003\n"
					"vmul.s			s002, s002, s003\n"
					
					"sv.s			s000, 0 + %0\n"			// *pv0 = c000
					"sv.s			s001, 4 + %0\n"			// *pv0 = c000
					"sv.s			s002, 8 + %0\n"			// *pv0 = c000
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pM), "m"(*pV)
				);
			#else
				REAL32 x,y,z,w,rw;
				x = (pM->m[0][0] * pV->vec[0]) + (pM->m[1][0] * pV->vec[1]) + (pM->m[2][0] * pV->vec[2]) + pM->m[3][0];
				y = (pM->m[0][1] * pV->vec[0]) + (pM->m[1][1] * pV->vec[1]) + (pM->m[2][1] * pV->vec[2]) + pM->m[3][1];
				z = (pM->m[0][2] * pV->vec[0]) + (pM->m[1][2] * pV->vec[1]) + (pM->m[2][2] * pV->vec[2]) + pM->m[3][2];
				w = (pM->m[0][3] * pV->vec[0]) + (pM->m[1][3] * pV->vec[1]) + (pM->m[2][3] * pV->vec[2]) + pM->m[3][3];
				rw = 1.0f / w;
				pOut->vec[0] = x * rw;
				pOut->vec[1] = y * rw;
				pOut->vec[2] = z * rw;
			#endif
		#elif defined( I3G_DX)
			D3DXVec3TransformCoord( (D3DXVECTOR3 *) pOut, (const D3DXVECTOR3 *) pV, (const D3DXMATRIX *) pM);
		#else
			REAL32 x,y,z,w,rw;
			x = (pM->m[0][0] * pV->vec[0]) + (pM->m[1][0] * pV->vec[1]) + (pM->m[2][0] * pV->vec[2]) + pM->m[3][0];
			y = (pM->m[0][1] * pV->vec[0]) + (pM->m[1][1] * pV->vec[1]) + (pM->m[2][1] * pV->vec[2]) + pM->m[3][1];
			z = (pM->m[0][2] * pV->vec[0]) + (pM->m[1][2] * pV->vec[1]) + (pM->m[2][2] * pV->vec[2]) + pM->m[3][2];
			w = (pM->m[0][3] * pV->vec[0]) + (pM->m[1][3] * pV->vec[1]) + (pM->m[2][3] * pV->vec[2]) + pM->m[3][3];
			rw = 1.0f / w;
			pOut->vec[0] = x * rw;
			pOut->vec[1] = y * rw;
			pOut->vec[2] = z * rw;
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 313.039 ms
	// Vfpu unUse(function으로 호출): 1025.317 ms
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	// 2005.10.7 Lene3 
	// PC Test(Release Build)
	// Test Result (1000*1000 loop)
	// DX  : 27.161207 ms
	// CPU : 1.002268 ms
	/////////////////////////////////////////////////////////////////
	inline
	void TransformNormal( VEC3D *pOut, const VEC3D *pV, const MATRIX *pM )
	{
		#if defined( I3G_PSP)
			#if defined( VFPU_ASM_VECTOR)
			//#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c100,  0 + %1\n"		// c100 = pm0->x
					"lv.q			c110, 16 + %1\n"		// c110 = pm0->y
					"lv.q			c120, 32 + %1\n"		// c120 = pm0->z
					"lv.q			c200, %2\n"				// c200 = *pv1
					//"vmov.s			s003, s203\n"			// s003 = s203
					"vzero.s		s203\n"
					"vtfm3.t		c000, e100, c200\n"		// c000 = e100 * c200
					"sv.s			s000, 0 + %0\n"			// *pv0 = c000
					"sv.s			s001, 4 + %0\n"			// *pv0 = c000
					"sv.s			s002, 8 + %0\n"			// *pv0 = c000
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pM), "m"(*pV)
				);
			#else
				VEC3D temp;
				Copy( &temp, (VEC3D *)pV);
				pOut->vec[0] = (pM->m[0][0] * temp.vec[0]) + (pM->m[1][0] * temp.vec[1]) + (pM->m[2][0] * temp.vec[2]);
				pOut->vec[1] = (pM->m[0][1] * temp.vec[0]) + (pM->m[1][1] * temp.vec[1]) + (pM->m[2][1] * temp.vec[2]);
				pOut->vec[2] = (pM->m[0][2] * temp.vec[0]) + (pM->m[1][2] * temp.vec[1]) + (pM->m[2][2] * temp.vec[2]);
			#endif

		#else
			VEC3D temp;
			Copy( &temp, (VEC3D *)pV);
			pOut->vec[0] = (pM->m[0][0] * temp.vec[0]) + (pM->m[1][0] * temp.vec[1]) + (pM->m[2][0] * temp.vec[2]);
			pOut->vec[1] = (pM->m[0][1] * temp.vec[0]) + (pM->m[1][1] * temp.vec[1]) + (pM->m[2][1] * temp.vec[2]);
			pOut->vec[2] = (pM->m[0][2] * temp.vec[0]) + (pM->m[1][2] * temp.vec[1]) + (pM->m[2][2] * temp.vec[2]);
		#endif
	}

	#if defined( I3_DEBUG)
	inline
	void	Dump( VEC3D * pVec)
	{
		I3TRACE( "%f, %f, %f\n", i3Vector::GetX( pVec), i3Vector::GetY(pVec), i3Vector::GetZ(pVec));
	}
	inline
	void	Dump( VEC2D * pVec)
	{
		I3TRACE( "%f, %f\n", i3Vector::GetX( pVec), i3Vector::GetY(pVec) );
	}
	#endif

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// Vector 4
	inline
	void	Copy( VEC4D * pDest, VEC4D * pSrc)
	{
		*pDest = *pSrc;
	}

	inline
	void	Set( VEC4D * pVec, REAL32 x, REAL32 y, REAL32 z, REAL32 w)
	{
		pVec->vec[0] = x;
		pVec->vec[1] = y;
		pVec->vec[2] = z;
		pVec->vec[3] = w;
	}

	inline
	void	SetX( VEC4D * pVec, REAL32 x)
	{
		pVec->vec[0] = x;
	}

	inline
	void	SetY( VEC4D * pVec, REAL32 y)
	{
		pVec->vec[1] = y;
	}

	inline
	void	SetZ( VEC4D * pVec, REAL32 z)
	{
		pVec->vec[2] = z;
	}

	inline
	void	SetW( VEC4D * pVec, REAL32 w)
	{
		pVec->vec[3] = w;
	}

	inline
	void	Zero( VEC4D * pVec)
	{
		pVec->vec[0] = 0.0f;
		pVec->vec[1] = 0.0f;
		pVec->vec[2] = 0.0f;
		pVec->vec[3] = 0.0f;
	}

	inline
	REAL32	GetX( VEC4D * pVec)
	{
		return pVec->vec[0];
	}

	inline
	REAL32 GetY( VEC4D * pVec)
	{
		return pVec->vec[1];
	}

	inline
	REAL32 GetZ( VEC4D * pVec)
	{
		return pVec->vec[2];
	}

	inline
	REAL32 GetW( VEC4D * pVec)
	{
		return pVec->vec[3];
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 320.647 ms
	// Vfpu unUse(function으로 호출): 319.656 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32 LengthSq( const VEC4D *pV )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 1
				REAL32 f;

				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c000, %1\n"				// c000 = *pv1
					"vmul.q			c010, c000, c000\n"		// c010 = c000 * c000
					"vfad.q			s010, c010\n"			// s010 = s010 + s011 + s012 + s013
					"mfv			$8,   s010\n"			// t0   = s010
					"mtc1			$8,   %0\n"				// f    = t0
					".set			pop\n"					// restore assember option
					: "=f"(f)
					: "m"(*pV)
					: "$8"
				);
				return f;
			#else
				return	(pV->vec[0] * pV->vec[0]) +
						(pV->vec[1] * pV->vec[1]) +
						(pV->vec[2] * pV->vec[2]) +
						(pV->vec[3] * pV->vec[3]);
			#endif

		#else
			return	(pV->vec[0] * pV->vec[0]) +
					(pV->vec[1] * pV->vec[1]) +
					(pV->vec[2] * pV->vec[2]) +
					(pV->vec[3] * pV->vec[3]);
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 348.910 ms - VFPU를 쓰지 않을때와 -0.000004의 차이가 생김
	// Vfpu unUse(function으로 호출): 2405.087 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32 Length( const VEC4D *pV )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 1
				REAL32 f;

				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c000, %1\n"				// c000 = *pv1
					"vmul.q			c010, c000, c000\n"		// c010 = c000 * c000
					"vfad.q			s010, c010\n"			// s010 = s010 + s011 + s012 + s013
					"vsqrt.s		s010, s010\n"			// s010 = sqrt(s010)
					"mfv			$8,   s010\n"			// t0   = s010
					"mtc1			$8,   %0\n"				// f    = t0
					".set			pop\n"					// restore assember option
					: "=f"(f)
					: "m"(*pV)
					: "$8"
				);
				return f;
			#else
				return i3Math::Sqrt( LengthSq( pV));
			#endif

		#else
			return i3Math::Sqrt( LengthSq( pV));
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 323.684 ms - VFPU를 쓰지 않을때와 -0.000002의 차이가 생김
	// Vfpu unUse(function으로 호출): 451.138 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32 Dot( const VEC4D *pV1, const VEC4D *pV2 )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 1
				REAL32 v;

				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c010, %1\n"				// c010 = *pv1
					"lv.q			c020, %2\n"				// c020 = *pv2
					"vdot.q			s000, c010, c020\n"		// s000 = c010 dot c020
					"mfv			$8,  s000\n"			// t0   = s000
					"mtc1			$8,  %0\n"				// v    = t0
					".set			pop\n"					// restore assember option
					: "=f"(v)
					: "m"(*pV1), "m"(*pV2)
					: "$8"
				);

				return v;
			#else
				return	(pV1->vec[0] * pV2->vec[0]) + 
						(pV1->vec[1] * pV2->vec[1]) +
						(pV1->vec[2] * pV2->vec[2]) +
						(pV1->vec[3] * pV2->vec[3]);
			#endif

		#else
			return	(pV1->vec[0] * pV2->vec[0]) + 
					(pV1->vec[1] * pV2->vec[1]) +
					(pV1->vec[2] * pV2->vec[2]) +
					(pV1->vec[3] * pV2->vec[3]);
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 290.000 ms
	// Vfpu unUse(function으로 호출): 525.983 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Add( VEC4D *pOut, const VEC4D *pV1, const VEC4D *pV2 )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q 			c010, %1\n"				// c010 = *pv1
					"lv.q			c020, %2\n"				// c020 = *pv2
					"vadd.q			c000, c010, c020\n"		// c000 = c010 + c020
					"sv.q			c000, %0\n"				// *pv0 = c000
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pV1), "m"(*pV2)
				);
			#else
				pOut->vec[0] = pV1->vec[0] + pV2->vec[0];
				pOut->vec[1] = pV1->vec[1] + pV2->vec[1];
				pOut->vec[2] = pV1->vec[2] + pV2->vec[2];
				pOut->vec[3] = pV1->vec[3] + pV2->vec[3];

			#endif
		#else
			pOut->vec[0] = pV1->vec[0] + pV2->vec[0];
			pOut->vec[1] = pV1->vec[1] + pV2->vec[1];
			pOut->vec[2] = pV1->vec[2] + pV2->vec[2];
			pOut->vec[3] = pV1->vec[3] + pV2->vec[3];
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 289.982 ms
	// Vfpu unUse(function으로 호출): 526.982 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Sub( VEC4D *pOut, const VEC4D *pV1, const VEC4D *pV2 )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q 			c010, %1\n"				// c010 = *pv1
					"lv.q			c020, %2\n"				// c020 = *pv2
					"vsub.q			c000, c010, c020\n"		// c000 = c010 - c020
					"sv.q			c000, %0\n"				// *pv0 = c000
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pV1), "m"(*pV2)
				);
			#else
				pOut->vec[0] = pV1->vec[0] - pV2->vec[0];
				pOut->vec[1] = pV1->vec[1] - pV2->vec[1];
				pOut->vec[2] = pV1->vec[2] - pV2->vec[2];
				pOut->vec[3] = pV1->vec[3] - pV2->vec[3];
			#endif

		#else
			pOut->vec[0] = pV1->vec[0] - pV2->vec[0];
			pOut->vec[1] = pV1->vec[1] - pV2->vec[1];
			pOut->vec[2] = pV1->vec[2] - pV2->vec[2];
			pOut->vec[3] = pV1->vec[3] - pV2->vec[3];
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 283.135 ms
	// Vfpu unUse(function으로 호출): 869.920 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Minimize( VEC4D *pOut, const VEC4D *pV1, const VEC4D *pV2 )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c010, %1\n"				// c010 = *pv1
					"lv.q			c020, %2\n"				// c020 = *pv2
					"vmin.q			c000, c010, c020\n"		// c000 = min(c010, c020)
					"sv.q			c000, %0\n"				// *pv0 = c000
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pV1), "m"(*pV2)
				);
			#else
				pOut->vec[0] = MIN( pV1->vec[0], pV2->vec[0]);
				pOut->vec[1] = MIN( pV1->vec[1], pV2->vec[1]);
				pOut->vec[2] = MIN( pV1->vec[2], pV2->vec[2]);
				pOut->vec[3] = MIN( pV1->vec[3], pV2->vec[3]);
			#endif

		#else
			pOut->vec[0] = MIN( pV1->vec[0], pV2->vec[0]);
			pOut->vec[1] = MIN( pV1->vec[1], pV2->vec[1]);
			pOut->vec[2] = MIN( pV1->vec[2], pV2->vec[2]);
			pOut->vec[3] = MIN( pV1->vec[3], pV2->vec[3]);
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 283.881 ms
	// Vfpu unUse(function으로 호출): 864.441 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Maximize( VEC4D *pOut, const VEC4D *pV1, const VEC4D *pV2 )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c000, %1\n"				// c000 = *pv1
					"lv.q			c010, %2\n"				// c010 = *pv2
					"vmax.q			c000, c000, c010\n"		// c000 = max(c000, c010)
					"sv.q			c000, %0\n"				// *pv0 = c000
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pV1), "m"(*pV2)
				);

				return pOut;
			#else
				pOut->vec[0] = MAX( pV1->vec[0], pV2->vec[0]);
				pOut->vec[1] = MAX( pV1->vec[1], pV2->vec[1]);
				pOut->vec[2] = MAX( pV1->vec[2], pV2->vec[2]);
				pOut->vec[3] = MAX( pV1->vec[3], pV2->vec[3]);
			#endif

		#else
			pOut->vec[0] = MAX( pV1->vec[0], pV2->vec[0]);
			pOut->vec[1] = MAX( pV1->vec[1], pV2->vec[1]);
			pOut->vec[2] = MAX( pV1->vec[2], pV2->vec[2]);
			pOut->vec[3] = MAX( pV1->vec[3], pV2->vec[3]);
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 309.977 ms
	// Vfpu unUse(function으로 호출): 526.689 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Scale( VEC4D *pOut, const VEC4D *pV, REAL32 s)
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,   %2\n"				// t0   = t
					"mtv			$8,   s010\n"			// s010 = t0
					"lv.q			c000, %1\n"				// c000 = *pv1
					"vscl.q			c000, c000, s010\n"		// c000 = c000 * s010
					"sv.q			c000, %0\n"				// *pv0 = c000
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pV), "f"(s)
					: "$8"
				);
			#else
				pOut->vec[0] = pV->vec[0] * s;
				pOut->vec[1] = pV->vec[1] * s;
				pOut->vec[2] = pV->vec[2] * s;
				pOut->vec[3] = pV->vec[3] * s;
			#endif

		#else
			pOut->vec[0] = pV->vec[0] * s;
			pOut->vec[1] = pV->vec[1] * s;
			pOut->vec[2] = pV->vec[2] * s;
			pOut->vec[3] = pV->vec[3] * s;
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 375.246 ms
	// Vfpu unUse(function으로 호출): 741.473 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Lerp( VEC4D *pOut, const VEC4D *pV1, const VEC4D *pV2, REAL32 s )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 1
				// 1.5.0b 버젼
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,   %3\n"				// t0   = t
					"mtv			$8,   s030\n"			// s030 = t0
					"lv.q			c010, %1\n"				// c010 = *pv1
					"lv.q			c020, %2\n"				// c020 = *pv2
					"vsub.q			c000, c020, c010\n"		// c000 = c020 - c010 = (v2 - v1)
					"vscl.q			c000, c000, s030\n"		// c000 = c000 * s030 = (v2 - v1) * t
					"vadd.q			c010, c010, c000\n"		// c010 = c010 + c000 = v1 + t * (v2 - v1)
					"sv.q			c010, %0\n"				// *pv0 = c010
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pV1), "m"(*pV2), "f"(s)
					: "$8"
				);
			#else
				pOut->vec[0] = pV1->vec[0] + ((pV2->vec[0] - pV1->vec[0]) * s);
				pOut->vec[1] = pV1->vec[1] + ((pV2->vec[1] - pV1->vec[1]) * s);
				pOut->vec[2] = pV1->vec[2] + ((pV2->vec[2] - pV1->vec[2]) * s);
				pOut->vec[3] = pV1->vec[3] + ((pV2->vec[3] - pV1->vec[3]) * s);
			#endif

		#else
			pOut->vec[0] = pV1->vec[0] + ((pV2->vec[0] - pV1->vec[0]) * s);
			pOut->vec[1] = pV1->vec[1] + ((pV2->vec[1] - pV1->vec[1]) * s);
			pOut->vec[2] = pV1->vec[2] + ((pV2->vec[2] - pV1->vec[2]) * s);
			pOut->vec[3] = pV1->vec[3] + ((pV2->vec[3] - pV1->vec[3]) * s);
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 344.917 ms
	// Vfpu unUse(function으로 호출): 3075.259 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32 Normalize( VEC4D *pOut, const VEC4D *pV )
	{
		REAL32 len;

		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c000, %2\n"				// c000 = *pv1
					"vmul.q			c010, c000, c000\n"		// c010 = c000 * c000
					"vfad.q			s010, c010\n"			// s010 = s010 + s011 + s012 + s013
					"vsqrt.s		s020, s010\n"			// s011 = 1.0 / sqrt(s010)
					"sv.s			s020, %1\n"
					"vrcp.s			s010, s020\n"
					"vscl.q			c000, c000, s010\n"		// c000 = c000 * s010
					"sv.q			c000, %0\n"				// *pv0 = c000
					".set			pop\n"					// restore assember option
					: "=m"(*pOut), "=f"(len)
					: "m"(*pV)
				);
			#else
				len = Length( pV);
				Scale( pOut, pV, i3Math::Reciprocal( len));
			#endif

		#else
			len = Length( pV);
			Scale( pOut, pV, i3Math::Reciprocal( len));
		#endif

		return len;
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 317.846 ms
	// Vfpu unUse(function으로 호출): 1497.093 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Transform( VEC4D *pOut, const VEC4D *pV, const MATRIX *pM )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_VECTOR)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c100,  0 + %1\n"		// c100 = pm0->x
					"lv.q			c110, 16 + %1\n"		// c110 = pm0->y
					"lv.q			c120, 32 + %1\n"		// c120 = pm0->z
					"lv.q			c130, 48 + %1\n"		// c130 = pm0->w
					"lv.q			c200, %2\n"				// c200 = *pv1
					"vtfm4.q		c000, e100, c200\n"		// c000 = e100 * c200
					"sv.q			c000, %0\n"				// *pv0 = c000
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pM), "m"(*pV)
				);
			#else
				VEC4D temp;
				Copy( &temp, (VEC4D *) pV);
				pOut->vec[0] = (pM->m[0][0] * temp.vec[0]) + (pM->m[1][0] * temp.vec[1]) + (pM->m[2][0] * temp.vec[2]) + (pM->m[3][0] * temp.vec[3]);
				pOut->vec[1] = (pM->m[0][1] * temp.vec[0]) + (pM->m[1][1] * temp.vec[1]) + (pM->m[2][1] * temp.vec[2]) + (pM->m[3][1] * temp.vec[3]);
				pOut->vec[2] = (pM->m[0][2] * temp.vec[0]) + (pM->m[1][2] * temp.vec[1]) + (pM->m[2][2] * temp.vec[2]) + (pM->m[3][2] * temp.vec[3]);
				pOut->vec[3] = (pM->m[0][3] * temp.vec[0]) + (pM->m[1][3] * temp.vec[1]) + (pM->m[2][3] * temp.vec[2]) + (pM->m[3][3] * temp.vec[3]);
			#endif

		#else
			VEC4D temp;
			Copy( &temp, (VEC4D *) pV);
			pOut->vec[0] = (pM->m[0][0] * temp.vec[0]) + (pM->m[1][0] * temp.vec[1]) + (pM->m[2][0] * temp.vec[2]) + (pM->m[3][0] * temp.vec[3]);
			pOut->vec[1] = (pM->m[0][1] * temp.vec[0]) + (pM->m[1][1] * temp.vec[1]) + (pM->m[2][1] * temp.vec[2]) + (pM->m[3][1] * temp.vec[3]);
			pOut->vec[2] = (pM->m[0][2] * temp.vec[0]) + (pM->m[1][2] * temp.vec[1]) + (pM->m[2][2] * temp.vec[2]) + (pM->m[3][2] * temp.vec[3]);
			pOut->vec[3] = (pM->m[0][3] * temp.vec[0]) + (pM->m[1][3] * temp.vec[1]) + (pM->m[2][3] * temp.vec[2]) + (pM->m[3][3] * temp.vec[3]);
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 272.504 ms
	/////////////////////////////////////////////////////////////////
	inline
	void GetRandVector(VEC3D* pOut)
	{
		#if defined( I3_PSP)
			#if defined( VFPU_ASM_VECTOR)
			//#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"vrndf1.t		s000\n"
					"viim.s			s004,	1\n"
					"viim.s			s005,	1\n"
					"viim.s			s006,	1\n"
					"vsub.t			s000,	s004\n"
					"sv.q			s000,	%0\n"			// *pv0 = c000
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					:
				);
			#else
				sceVfpuRandVector( (ScePspFVector4 *) pOut);
			#endif
		//	05.06.15 by KOMET
		#else
			PERSIST_VEC3D* pIn = &_random_vector_table[ i3Math::Rand() & 0x00ff];
			Set( pOut, pIn->x, pIn->y, pIn->z);
		#endif
	}

	// 2D 좌표계로 만들어 줌.
	// 정수화한 후 0.5f를 빼줌.
	inline void Make2DCoord( VEC2D *pVec )
	{
		pVec->vec[0] = INT32( pVec->vec[0] ) - 0.5f;
		pVec->vec[1] = INT32( pVec->vec[1] ) - 0.5f;
	}

	// 2D 좌표계로 만들어 줌.
	// 정수화한 후 0.5f를 빼줌.
	inline void Make2DCoord( VEC3D *pVec )
	{
		pVec->vec[0] = INT32( pVec->vec[0] ) - 0.5f;
		pVec->vec[1] = INT32( pVec->vec[1] ) - 0.5f;
		pVec->vec[2] = INT32( pVec->vec[2] ) - 0.5f;
	}

	enum CONVERT_MODE
	{
		CONVERT_MODE_XY,
		CONVERT_MODE_XYZ = CONVERT_MODE_XY,
		CONVERT_MODE_XZ,
		CONVERT_MODE_XZY = CONVERT_MODE_XZ,
		CONVERT_MODE_MINUS_X_Z,
		CONVERT_MODE_MINUS_X_Z_Y = CONVERT_MODE_MINUS_X_Z,
	};

	// 2D 좌표를 3D로 변환
	inline
	VEC3D *	Convert( VEC3D * pDest, VEC2D * pSrc, INT32 nMode )
	{
		switch( nMode )
		{
		case CONVERT_MODE_XYZ:
			{
				Set( pDest, GetX( pSrc ), GetY( pSrc ), 0.0f );
			}
			break;
		case CONVERT_MODE_XZY:
			{
				Set( pDest, GetX( pSrc ), 0.0f, GetY( pSrc ) );
			}
			break;
		case CONVERT_MODE_MINUS_X_Z_Y:
			{
				Set( pDest, -1.0f * GetX( pSrc ), 0.0f, GetY( pSrc ) );
			}
			break;
		default:
			{
				Set( pDest, 0.0f, 0.0f, 0.0f );
				I3ASSERT_0;
			}
			break;
		}

		return pDest;
	}

	// 3D 좌표를 3D로 좌표계 변환
	inline
	VEC3D *	Convert( VEC3D * pDest, VEC3D * pSrc, INT32 nMode )
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
				Set( pDest, -1.0f * GetX( pSrc ), GetZ( pSrc ), GetY( pSrc ) );
			}
			break;
		default:
			{
				Set( pDest, 0.0f, 0.0f, 0.0f );
				I3ASSERT_0;
			}
			break;
		}

		return pDest;
	}

	I3_EXPORT_MATH	bool	SetToXML( i3XMLElement * pXML, const char * pszName, VEC2D * pVec);
	I3_EXPORT_MATH	bool	SetToXML( i3XMLElement * pXML, const char * pszName, VEC3D * pVec);
	I3_EXPORT_MATH	bool	SetToXML( i3XMLElement * pXML, const char * pszName, VEC4D * pVec);

	I3_EXPORT_MATH	bool	GetFromXML( i3XMLElement * pXML, const char * pszName, VEC2D * pVec);
	I3_EXPORT_MATH	bool	GetFromXML( i3XMLElement * pXML, const char * pszName, VEC3D * pVec);
	I3_EXPORT_MATH	bool	GetFromXML( i3XMLElement * pXML, const char * pszName, VEC4D * pVec);
};

#define setU( pVec, u)					i3Vector::SetU( (pVec), (u))
#define setV( pVec, v)					i3Vector::SetV( (pVec), (v))
#define setUV( pVec, u, v)				i3Vector::Set( (pVec), (u), (v))
#define getU( pVec)						i3Vector::GetU( (pVec))
#define getV( pVec)						i3Vector::GetV( (pVec))

#define setX( pVec, x)					i3Vector::SetX( (pVec), x)
#define setY( pVec, y)					i3Vector::SetY( (pVec), y)
#define setZ( pVec, z)					i3Vector::SetZ( (pVec), z)
#define setW( pVec, w)					i3Vector::SetW( (pVec), w)

#define setXYZ( pVec, x, y, z)			i3Vector::Set( (pVec), (x), (y), (z))
#define getX( pVec)						i3Vector::GetX( (pVec))
#define getY( pVec)						i3Vector::GetY( (pVec))
#define getZ( pVec)						i3Vector::GetZ( (pVec))
#define getW( pVec)						i3Vector::GetW( (pVec))

#define addX( pVec, r)					i3Vector::AddX( (pVec), r)
#define addY( pVec, r)					i3Vector::AddY( (pVec), r)
#define addZ( pVec, r)					i3Vector::AddZ( (pVec), r)
#define addXYZ( pVec, x, y, z)			i3Vector::Add( (pVec), x, y, z)


#endif
/////////////                                                  
