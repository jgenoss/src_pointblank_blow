// i3InterfaceMatrix.h

#ifndef __I3_MATRIX_H
#define __I3_MATRIX_H

#include "../i3Base/i3CommonType.h"
#include "../i3Base/i3Debug.h"
#include "i3MathType.h"
#include "i3Vector.h"

#if defined( I3G_DX)
#endif

#pragma push_macro("new")

#undef new

VECTORALIGN0 union MATRIX
{
	struct
	{
		REAL32	m00, m01, m02, m03;
		REAL32	m10, m11, m12, m13;
		REAL32	m20, m21, m22, m23;
		REAL32	m30, m31, m32, m33;
	};

	struct
	{
		VEC4D vec[4];
	};
	
	REAL32 m[4][4];
	REAL32 v[16];
	
	// VS2015 대입연산자사용시 기본생성자 강제요구됨.
	MATRIX()
	{
		m00 = 1.0f; m01 = 0.0f; m02 = 0.0f; m03 = 0.0f;
		m10 = 0.0f; m11 = 1.0f; m12 = 0.0f; m13 = 0.0f;
		m20 = 0.0f; m21 = 0.0f; m22 = 1.0f; m23 = 0.0f;
		m30 = 0.0f; m31 = 0.0f; m32 = 0.0f; m33 = 1.0f;
	}
	MATRIX(const REAL32* f) { ::memcpy(&m00, f, sizeof(MATRIX)); }
	MATRIX(REAL32 m00, REAL32 m01, REAL32 m02, REAL32 m03,
		REAL32 m10, REAL32 m11, REAL32 m12, REAL32 m13,
		REAL32 m20, REAL32 m21, REAL32 m22, REAL32 m23,
		REAL32 m30, REAL32 m31, REAL32 m32, REAL32 m33)
		: m00(m00), m01(m01), m02(m02), m03(m03),
		m10(m10), m11(m11), m12(m12), m13(m13),
		m20(m20), m21(m21), m22(m22), m23(m23),
		m30(m30), m31(m31), m32(m32), m33(m33) {}		// 컴파일러 호환 편의성을 위해 아래 생성자 대신 이것으로 일단 대체..
																	
	MATRIX& operator=(const MATRIX& rhs) { ::memcpy(&m00, &rhs.m00, sizeof(MATRIX)); return *this; }		// VS2015 C2280 에러에 의해 직접 구현..
																											// new operators
	inline void* operator new  (size_t s)
	{
		void* pMat = ::_aligned_malloc(s, VETORALIGN_SIZE);
		return ::new (pMat)MATRIX;
	}

	inline void* operator new[](size_t s)
	{
		void* pMat = ::_aligned_malloc(s, VETORALIGN_SIZE);
		return ::new (pMat)MATRIX;
	}

	// delete operators
	inline void operator delete   (void* p)   // These are NOT virtual; Do not 
	{
		::_aligned_free(p);
	}

	inline void operator delete[](void* p)   // cast to D3DXMATRIX and delete.
	{
		::_aligned_free(p);
	}

#ifdef _DEBUG
	static void* operator new (std::size_t size, int iBlockState, const char *file, int line);
	static void* operator new[](std::size_t size, int iBlockState, const char* file, int line);

	static void* operator new (std::size_t size, const char *file, int line);
	static void* operator new[](std::size_t size, const char* file, int line);

	static void operator delete (void *ptr, int iBlockState, const char* file, int line);
	static void operator delete[](void *ptr, int iBlockState, const char* file, int line);
#endif	
} VECTORALIGN;

#ifdef _DEBUG
__forceinline
void* MATRIX::operator new (std::size_t size, int, const char *file, int line)
{
	return _aligned_malloc_dbg(size, VETORALIGN_SIZE, file, line);
}

__forceinline
void* MATRIX::operator new[](std::size_t size, int, const char* file, int line)
{
	return _aligned_malloc_dbg(size, VETORALIGN_SIZE, file, line);
}

__forceinline
void* MATRIX::operator new (std::size_t size, const char *file, int line)
{
	return _aligned_malloc_dbg(size, VETORALIGN_SIZE, file, line);
}

__forceinline
void* MATRIX::operator new[](std::size_t size, const char* file, int line)
{
	return _aligned_malloc_dbg(size, VETORALIGN_SIZE, file, line);
}

__forceinline
void  MATRIX::operator delete (void *ptr, int, const char*, int)
{
	::_aligned_free_dbg(ptr);
}

__forceinline
void  MATRIX::operator delete[](void *ptr, int, const char*, int)
{
	::_aligned_free_dbg(ptr);
}

#endif	


#pragma pop_macro("new")

#if defined( I3_FIXED_POINT)
	I3_EXTERN_MATH		FIXED32_MATRIX		I3_IDENTITYMATRIX;
#else
	I3_EXTERN_MATH		MATRIX				I3_IDENTITYMATRIX;
#endif

#define DET2X2(a,b,c,d) (a*d-b*c)
#define DET3X3(a1,a2,a3,b1,b2,b3,c1,c2,c3)  (a1*DET2X2(b2,b3,c2,c3)-b1*DET2X2(a2,a3,c2,c3)+c1*DET2X2(a2,a3,b2,b3))
#define DET4X4( a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4) \
	((a1 * DET3X3( b2, b3, b4, c2, c3, c4, d2, d3, d4)) \
	- (a2 * DET3X3( b1, b3, b4, c1, c3, c4, d1, d3, d4)) \
	+ (a3 * DET3X3( b1, b2, b4, c1, c2, c4, d1, d2, d4)) \
	- (a4 * DET3X3( b1, b2, b3, c1, c2, c3, d1, d2, d3)))

I3_EXPORT_MATH void i3QuaternionFromMatrix( QUATERNION * pQuat, MATRIX * pMtx);
I3_EXPORT_MATH void i3QuaternionSLERP( QUATERNION * pOut, QUATERNION * pIn1, QUATERNION * pIn2, REAL32 t);
I3_EXPORT_MATH MATRIX * i3Matrix_SetRotateAxis( MATRIX * pOut, const VEC3D * pV, REAL32 Angle);

namespace i3Matrix
{
	inline
	VEC3D *	GetRight( MATRIX * pIn)
	{
		return (VEC3D *) &pIn->m00;
	}

	inline
	VEC3D * GetUp( MATRIX * pIn)
	{
		return (VEC3D *) & pIn->m10;
	}

	inline
	VEC3D * GetAt( MATRIX * pIn)
	{
		return (VEC3D *) & pIn->m20;
	}

	inline
	VEC3D * GetPos( MATRIX * pIn)
	{
		return (VEC3D *) & pIn->m30;
	}

	inline
	REAL32	Get11( MATRIX * pIn)
	{
		return pIn->m00;
	}

	inline
	REAL32	Get12( MATRIX * pIn)
	{
		return pIn->m01;
	}

	inline
	REAL32	Get13( MATRIX * pIn)
	{
		return pIn->m02;
	}

	inline
	REAL32	Get14( MATRIX * pIn)
	{
		return pIn->m03;
	}

	inline
	REAL32	Get21( MATRIX * pIn)
	{
		return pIn->m10;
	}

	inline
	REAL32	Get22( MATRIX * pIn)
	{
		return pIn->m11;
	}

	inline
	REAL32	Get23( MATRIX * pIn)
	{
		return pIn->m12;
	}

	inline
	REAL32	Get24( MATRIX * pIn)
	{
		return pIn->m13;
	}

	inline
	REAL32	Get31( MATRIX * pIn)
	{
		return pIn->m20;
	}

	inline
	REAL32	Get32( MATRIX * pIn)
	{
		return pIn->m21;
	}

	inline
	REAL32	Get33( MATRIX * pIn)
	{
		return pIn->m22;
	}

	inline
	REAL32	Get34( MATRIX * pIn)
	{
		return pIn->m23;
	}

	inline
	REAL32	Get41( MATRIX * pIn)
	{
		return pIn->m30;
	}

	inline
	REAL32	Get42( MATRIX * pIn)
	{
		return pIn->m31;
	}

	inline
	REAL32	Get43( MATRIX * pIn)
	{
		return pIn->m32;
	}

	inline
	REAL32	Get44( MATRIX * pIn)
	{
		return pIn->m33;
	}

	inline
	void GetRow( MATRIX * pIn, INT32 idx, VEC4D * pVec)
	{
		i3Vector::Copy( pVec, (VEC4D *) pIn->m[idx]);
	}


	inline
	void GetColumn( MATRIX * pIn, INT32 idx, VEC4D * pVec)
	{
		register REAL32 * pVal = &(pIn->m[0][idx]);
		i3Vector::SetX( pVec, *pVal);	pVal += 4;
		i3Vector::SetY( pVec, *pVal);	pVal += 4;
		i3Vector::SetZ( pVec, *pVal);	pVal += 4;
		i3Vector::SetW( pVec, *pVal);
	}

	inline
	void SetRight( MATRIX * pIn, VEC3D * pRight)
	{
		register REAL32 * pTemp = (REAL32 *) pIn->m[0];

		*pTemp = i3Vector::GetX( pRight);	pTemp++;
		*pTemp = i3Vector::GetY( pRight);	pTemp++;
		*pTemp = i3Vector::GetZ( pRight);	pTemp++;
	}

	inline
	void SetRight( MATRIX * pIn, REAL32 v0, REAL32 v1, REAL32 v2, REAL32 v3)
	{
		register REAL32 * pTemp = (REAL32 *) pIn->m[0];

		*pTemp = v0;	pTemp++;
		*pTemp = v1;	pTemp++;
		*pTemp = v2;	pTemp++;
		*pTemp = v3;
	}

	inline
	void SetUp( MATRIX * pIn, VEC3D * pUp)
	{
		register REAL32 * pTemp = (REAL32 *) pIn->m[1];

		*pTemp = i3Vector::GetX( pUp);	pTemp++;
		*pTemp = i3Vector::GetY( pUp);	pTemp++;
		*pTemp = i3Vector::GetZ( pUp);	pTemp++;
	}

	inline
	void SetUp( MATRIX * pIn, REAL32 v0, REAL32 v1, REAL32 v2, REAL32 v3)
	{
		register REAL32 * pTemp = (REAL32 *) pIn->m[1];

		*pTemp = v0;	pTemp++;
		*pTemp = v1;	pTemp++;
		*pTemp = v2;	pTemp++;
		*pTemp = v3;
	}

	inline
	void SetAt( MATRIX * pIn, VEC3D * pAt)
	{
		register REAL32 * pTemp = (REAL32 *) pIn->m[2];

		*pTemp = i3Vector::GetX( pAt);	pTemp++;
		*pTemp = i3Vector::GetY( pAt);	pTemp++;
		*pTemp = i3Vector::GetZ( pAt);	pTemp++;
	}

	inline
	void SetAt( MATRIX * pIn, REAL32 v0, REAL32 v1, REAL32 v2, REAL32 v3)
	{
		register REAL32 * pTemp = (REAL32 *) pIn->m[2];

		*pTemp = v0;	pTemp++;
		*pTemp = v1;	pTemp++;
		*pTemp = v2;	pTemp++;
		*pTemp = v3;
	}

	inline
	void SetPos( MATRIX * pIn, REAL32 v0, REAL32 v1, REAL32 v2, REAL32 v3)
	{
		register REAL32 * pTemp = (REAL32 *) pIn->m[3];

		*pTemp = v0;	pTemp++;
		*pTemp = v1;	pTemp++;
		*pTemp = v2;	pTemp++;
		*pTemp = v3;
	}

	inline
	void SetPos( MATRIX * pIn, REAL32 v0, REAL32 v1, REAL32 v2)
	{
		register REAL32 * pTemp = (REAL32 *) pIn->m[3];

		*pTemp = v0;	pTemp++;
		*pTemp = v1;	pTemp++;
		*pTemp = v2;
		
	}

	inline
	void SetPos( MATRIX * pIn, VEC3D * pVec)
	{
		register REAL32 *pTemp = (REAL32 *) pIn->m[3];

		*pTemp = i3Vector::GetX( pVec);		pTemp++;
		*pTemp = i3Vector::GetY( pVec);		pTemp++;
		*pTemp = i3Vector::GetZ( pVec);		pTemp++;
	}

	inline
	void SetPos( MATRIX * pIn, VEC4D * pVec)
	{
		VEC4D * pTemp = (VEC4D *) pIn->m[3];

		*pTemp = *pVec;
	}


	/////////////////////////////////////////////////////////////////
	// 2005.3.3 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 366.407 ms
	// Vfpu unUse(function으로 호출): 1020.018 ms
	// 예외적으로 memcpy를 통한것은 696.409ms, *pDest = *pSrc와 같이 대입을 통한것은 706.037ms
	/////////////////////////////////////////////////////////////////
	inline
	void Copy( MATRIX * pDest, const MATRIX * pSrc)
	{
		#if defined( I3G_PSP)
			__asm__ (
				".set		push\n"
				".set		noreorder\n"
				"lv.q		c100,  0 + %1\n"		// c100 = pm1->x
				"lv.q		c110, 16 + %1\n"		// c110 = pm1->y
				"lv.q		c120, 32 + %1\n"		// c120 = pm1->z
				"lv.q		c130, 48 + %1\n"		// c130 = pm1->w
				"sv.q		c100,	0 + %0\n"
				"sv.q		c110,	16 + %0\n"
				"sv.q		c120,	32 + %0\n"
				"sv.q		c130,	48 + %0\n"
				".set		pop\n"
				: "=m"( *pDest)
				: "m"( *pSrc)
				);					
			//memcpy( pDest, pSrc, sizeof(MATRIX));
			//*pDest = *pSrc;
		#elif defined( I3_WIPI)
			register UINT32 * p1, * p2, i;

			p1 = (UINT32 *) pSrc;
			p2 = (UINT32 *) pDest;

			for( i = 0; i < 16; i++, p1++, p2++)
				*p2 = *p1;
		#elif defined( I3G_DX)
			*pDest = *pSrc;
		#else
			pDest->m[0][0]	=	pSrc->m[0][0];
			pDest->m[0][1]	=	pSrc->m[0][1];
			pDest->m[0][2]	=	pSrc->m[0][2];
			pDest->m[0][3]	=	pSrc->m[0][3];
			
			pDest->m[1][0]	=	pSrc->m[1][0];
			pDest->m[1][1]	=	pSrc->m[1][1];
			pDest->m[1][2]	=	pSrc->m[1][2];
			pDest->m[1][3]	=	pSrc->m[1][3];
			
			pDest->m[2][0]	=	pSrc->m[2][0];
			pDest->m[2][1]	=	pSrc->m[2][1];
			pDest->m[2][2]	=	pSrc->m[2][2];
			pDest->m[2][3]	=	pSrc->m[2][3];
			
			pDest->m[3][0]	=	pSrc->m[3][0];
			pDest->m[3][1]	=	pSrc->m[3][1];
			pDest->m[3][2]	=	pSrc->m[3][2];
			pDest->m[3][3]	=	pSrc->m[3][3];	
		#endif
	}

	inline
	void	Copy( i3::pack::MATRIX * pDest, MATRIX * pSrc)
	{
		pDest->mat[0].x = pSrc->m[0][0];
		pDest->mat[0].y = pSrc->m[0][1];
		pDest->mat[0].z = pSrc->m[0][2];
		pDest->mat[0].w = pSrc->m[0][3];

		pDest->mat[1].x = pSrc->m[1][0];
		pDest->mat[1].y = pSrc->m[1][1];
		pDest->mat[1].z = pSrc->m[1][2];
		pDest->mat[1].w = pSrc->m[1][3];

		pDest->mat[2].x = pSrc->m[2][0];
		pDest->mat[2].y = pSrc->m[2][1];
		pDest->mat[2].z = pSrc->m[2][2];
		pDest->mat[2].w = pSrc->m[2][3];

		pDest->mat[3].x = pSrc->m[3][0];
		pDest->mat[3].y = pSrc->m[3][1];
		pDest->mat[3].z = pSrc->m[3][2];
		pDest->mat[3].w = pSrc->m[3][3];
	}

	inline
	void	Copy( MATRIX * pDest, const i3::pack::MATRIX * pSrc)
	{
		pDest->m[0][0] = pSrc->mat[0].x;
		pDest->m[0][1] = pSrc->mat[0].y;
		pDest->m[0][2] = pSrc->mat[0].z;
		pDest->m[0][3] = pSrc->mat[0].w;

		pDest->m[1][0] = pSrc->mat[1].x;
		pDest->m[1][1] = pSrc->mat[1].y;
		pDest->m[1][2] = pSrc->mat[1].z;
		pDest->m[1][3] = pSrc->mat[1].w;

		pDest->m[2][0] = pSrc->mat[2].x;
		pDest->m[2][1] = pSrc->mat[2].y;
		pDest->m[2][2] = pSrc->mat[2].z;
		pDest->m[2][3] = pSrc->mat[2].w;

		pDest->m[3][0] = pSrc->mat[3].x;
		pDest->m[3][1] = pSrc->mat[3].y;
		pDest->m[3][2] = pSrc->mat[3].z;
		pDest->m[3][3] = pSrc->mat[3].w;
	}

	inline
	bool IsSame( MATRIX * pDest, MATRIX * pSrc)
	{
		return	i3Math::IsSame( pDest->m[0][0], pSrc->m[0][0]) &&
				i3Math::IsSame( pDest->m[0][1], pSrc->m[0][1]) &&
				i3Math::IsSame( pDest->m[0][2], pSrc->m[0][2]) &&
				i3Math::IsSame( pDest->m[0][3], pSrc->m[0][3]) &&

				i3Math::IsSame( pDest->m[1][0], pSrc->m[1][0]) &&
				i3Math::IsSame( pDest->m[1][1], pSrc->m[1][1]) &&
				i3Math::IsSame( pDest->m[1][2], pSrc->m[1][2]) &&
				i3Math::IsSame( pDest->m[1][3], pSrc->m[1][3]) &&

				i3Math::IsSame( pDest->m[2][0], pSrc->m[2][0]) &&
				i3Math::IsSame( pDest->m[2][1], pSrc->m[2][1]) &&
				i3Math::IsSame( pDest->m[2][2], pSrc->m[2][2]) &&
				i3Math::IsSame( pDest->m[2][3], pSrc->m[2][3]) &&

				i3Math::IsSame( pDest->m[3][0], pSrc->m[3][0]) &&
				i3Math::IsSame( pDest->m[3][1], pSrc->m[3][1]) &&
				i3Math::IsSame( pDest->m[3][2], pSrc->m[3][2]) &&
				i3Math::IsSame( pDest->m[3][3], pSrc->m[3][3]);
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.3 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 360.027 ms
	// Vfpu unUse(function으로 호출): 1169.962 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Identity( MATRIX *pOut )
	{
		#if defined( I3G_PSP)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"vmidt.q		e000\n"					// e000 = I
					"sv.q			c000,  0 + %0\n"		// pm->x = c000
					"sv.q			c010, 16 + %0\n"		// pm->y = c010
					"sv.q			c020, 32 + %0\n"		// pm->z = c020
					"sv.q			c030, 48 + %0\n"		// pm->w = c030
					"vnop\n"
					"vnop\n"
					"vnop\n"
					"vnop\n"
					"vnop\n"
					"vnop\n"
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
				);
			#else
				Copy( pOut, &I3_IDENTITYMATRIX);
			#endif
		#else
			//Copy( pOut, &I3_IDENTITYMATRIX);
			SetRight( pOut, 1.0f, 0.0f, 0.0f, 0.0f);
			SetUp(	pOut, 0.0f, 1.0f, 0.0f, 0.0f);
			SetAt(	pOut, 0.0f, 0.0f, 1.0f, 0.0f);
			SetPos(	pOut, 0.0f, 0.0f, 0.0f, 1.0f);
		#endif
	}

	inline
	bool IsIdentity( const MATRIX *pM )
	{
		return IsSame( (MATRIX *) pM, (MATRIX *) &I3_IDENTITYMATRIX);
	}

	// 행렬식 전개값
	inline
	REAL32 Determinant( const MATRIX *pM )
	{
		return    pM->m00 * DET3X3(pM->m11,pM->m12,pM->m13,pM->m21,pM->m22,pM->m23,pM->m31,pM->m32,pM->m33)
				- pM->m10 * DET3X3(pM->m01,pM->m02,pM->m03,pM->m21,pM->m22,pM->m23,pM->m31,pM->m32,pM->m33)
				+ pM->m20 * DET3X3(pM->m01,pM->m02,pM->m03,pM->m11,pM->m12,pM->m13,pM->m31,pM->m32,pM->m33)
				- pM->m30 * DET3X3(pM->m01,pM->m02,pM->m03,pM->m11,pM->m12,pM->m13,pM->m21,pM->m22,pM->m23);
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.3 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 516.027 ms
	// Vfpu unUse(function으로 호출): 5550.110 ms
	/////////////////////////////////////////////////////////////////
	inline
	void Mul( MATRIX *pOut, const MATRIX *pM1, const MATRIX *pM2 )
	{
		#if defined( I3G_PSP)
			__asm__ (
				".set			push\n"					// save assember option
				".set			noreorder\n"			// suppress reordering
				"lv.q			c100,  0 + %1\n"		// c100 = pm1->x
				"lv.q			c110, 16 + %1\n"		// c110 = pm1->y
				"lv.q			c120, 32 + %1\n"		// c120 = pm1->z
				"lv.q			c130, 48 + %1\n"		// c130 = pm1->w
				"lv.q			c200,  0 + %2\n"		// c200 = pm2->x
				"lv.q			c210, 16 + %2\n"		// c210 = pm2->y
				"lv.q			c220, 32 + %2\n"		// c220 = pm2->z
				"lv.q			c230, 48 + %2\n"		// c230 = pm2->w
				"vrmmul.q		e000, e100, e200\n"		// e000 = e100 * e200
				"sv.q			c000,  0 + %0\n"		// pm0->x = c000
				"sv.q			c010, 16 + %0\n"		// pm0->y = c010
				"sv.q			c020, 32 + %0\n"		// pm0->z = c020
				"sv.q			c030, 48 + %0\n"		// pm0->w = c030
				".set			pop\n"					// restore assember option
				: "=m"(*pOut)
				: "m"(*pM1), "m"(*pM2)
			);
		#elif defined( I3G_DX)
			D3DXMatrixMultiply( (D3DXMATRIX *) pOut, (const D3DXMATRIX *) pM1, (const D3DXMATRIX *) pM2);
		#else
			MATRIX mat;

			mat.m00 = pM2->m00 * pM1->m00 + pM2->m10 * pM1->m01 + pM2->m20 * pM1->m02 + pM2->m30 * pM1->m03;
			mat.m01 = pM2->m01 * pM1->m00 + pM2->m11 * pM1->m01 + pM2->m21 * pM1->m02 + pM2->m31 * pM1->m03;
			mat.m02 = pM2->m02 * pM1->m00 + pM2->m12 * pM1->m01 + pM2->m22 * pM1->m02 + pM2->m32 * pM1->m03;
			mat.m03 = pM2->m03 * pM1->m00 + pM2->m13 * pM1->m01 + pM2->m23 * pM1->m02 + pM2->m33 * pM1->m03;
			mat.m10 = pM2->m00 * pM1->m10 + pM2->m10 * pM1->m11 + pM2->m20 * pM1->m12 + pM2->m30 * pM1->m13;
			mat.m11 = pM2->m01 * pM1->m10 + pM2->m11 * pM1->m11 + pM2->m21 * pM1->m12 + pM2->m31 * pM1->m13;
			mat.m12 = pM2->m02 * pM1->m10 + pM2->m12 * pM1->m11 + pM2->m22 * pM1->m12 + pM2->m32 * pM1->m13;
			mat.m13 = pM2->m03 * pM1->m10 + pM2->m13 * pM1->m11 + pM2->m23 * pM1->m12 + pM2->m33 * pM1->m13;
			mat.m20 = pM2->m00 * pM1->m20 + pM2->m10 * pM1->m21 + pM2->m20 * pM1->m22 + pM2->m30 * pM1->m23;
			mat.m21 = pM2->m01 * pM1->m20 + pM2->m11 * pM1->m21 + pM2->m21 * pM1->m22 + pM2->m31 * pM1->m23;
			mat.m22 = pM2->m02 * pM1->m20 + pM2->m12 * pM1->m21 + pM2->m22 * pM1->m22 + pM2->m32 * pM1->m23;
			mat.m23 = pM2->m03 * pM1->m20 + pM2->m13 * pM1->m21 + pM2->m23 * pM1->m22 + pM2->m33 * pM1->m23;
			mat.m30 = pM2->m00 * pM1->m30 + pM2->m10 * pM1->m31 + pM2->m20 * pM1->m32 + pM2->m30 * pM1->m33;
			mat.m31 = pM2->m01 * pM1->m30 + pM2->m11 * pM1->m31 + pM2->m21 * pM1->m32 + pM2->m31 * pM1->m33;
			mat.m32 = pM2->m02 * pM1->m30 + pM2->m12 * pM1->m31 + pM2->m22 * pM1->m32 + pM2->m32 * pM1->m33;
			mat.m33 = pM2->m03 * pM1->m30 + pM2->m13 * pM1->m31 + pM2->m23 * pM1->m32 + pM2->m33 * pM1->m33;
			Copy( pOut, &mat);
		#endif
	}

	inline
	void MulAffine( MATRIX *pOut, const MATRIX *pM1, const MATRIX *pM2 )
	{
		#if defined( I3G_PSP)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"lv.q			c100,  0 + %1\n"		// c100 = pm1->x
					"lv.q			c110, 16 + %1\n"		// c110 = pm1->y
					"lv.q			c120, 32 + %1\n"		// c120 = pm1->z
					"lv.q			c130, 48 + %1\n"		// c130 = pm1->w
					"lv.q			c200,  0 + %2\n"		// c200 = pm2->x
					"lv.q			c210, 16 + %2\n"		// c210 = pm2->y
					"lv.q			c220, 32 + %2\n"		// c220 = pm2->z
					"lv.q			c230, 48 + %2\n"		// c230 = pm2->w
					"vrmmul.q		e000, e100, e200\n"		// e000 = e100 * e200
					"sv.q			c000,  0 + %0\n"		// pm0->x = c000
					"sv.q			c010, 16 + %0\n"		// pm0->y = c010
					"sv.q			c020, 32 + %0\n"		// pm0->z = c020
					"sv.q			c030, 48 + %0\n"		// pm0->w = c030
					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "m"(*pM1), "m"(*pM2)
				);
			#else
				MATRIX mat;

				mat.m00 = pM2->m00 * pM1->m00 + pM2->m10 * pM1->m01 + pM2->m20 * pM1->m02 + pM2->m30 * pM1->m03;
				mat.m01 = pM2->m01 * pM1->m00 + pM2->m11 * pM1->m01 + pM2->m21 * pM1->m02 + pM2->m31 * pM1->m03;
				mat.m02 = pM2->m02 * pM1->m00 + pM2->m12 * pM1->m01 + pM2->m22 * pM1->m02 + pM2->m32 * pM1->m03;
				mat.m03 = pM2->m03 * pM1->m00 + pM2->m13 * pM1->m01 + pM2->m23 * pM1->m02 + pM2->m33 * pM1->m03;
				mat.m10 = pM2->m00 * pM1->m10 + pM2->m10 * pM1->m11 + pM2->m20 * pM1->m12 + pM2->m30 * pM1->m13;
				mat.m11 = pM2->m01 * pM1->m10 + pM2->m11 * pM1->m11 + pM2->m21 * pM1->m12 + pM2->m31 * pM1->m13;
				mat.m12 = pM2->m02 * pM1->m10 + pM2->m12 * pM1->m11 + pM2->m22 * pM1->m12 + pM2->m32 * pM1->m13;
				mat.m13 = pM2->m03 * pM1->m10 + pM2->m13 * pM1->m11 + pM2->m23 * pM1->m12 + pM2->m33 * pM1->m13;
				mat.m20 = pM2->m00 * pM1->m20 + pM2->m10 * pM1->m21 + pM2->m20 * pM1->m22 + pM2->m30 * pM1->m23;
				mat.m21 = pM2->m01 * pM1->m20 + pM2->m11 * pM1->m21 + pM2->m21 * pM1->m22 + pM2->m31 * pM1->m23;
				mat.m22 = pM2->m02 * pM1->m20 + pM2->m12 * pM1->m21 + pM2->m22 * pM1->m22 + pM2->m32 * pM1->m23;
				mat.m23 = pM2->m03 * pM1->m20 + pM2->m13 * pM1->m21 + pM2->m23 * pM1->m22 + pM2->m33 * pM1->m23;
				mat.m30 = pM2->m00 * pM1->m30 + pM2->m10 * pM1->m31 + pM2->m20 * pM1->m32 + pM2->m30 * pM1->m33;
				mat.m31 = pM2->m01 * pM1->m30 + pM2->m11 * pM1->m31 + pM2->m21 * pM1->m32 + pM2->m31 * pM1->m33;
				mat.m32 = pM2->m02 * pM1->m30 + pM2->m12 * pM1->m31 + pM2->m22 * pM1->m32 + pM2->m32 * pM1->m33;
				mat.m33 = pM2->m03 * pM1->m30 + pM2->m13 * pM1->m31 + pM2->m23 * pM1->m32 + pM2->m33 * pM1->m33;
				Copy( pOut, &mat);
			#endif

		#else
			MATRIX mat;

			mat.m00 = pM2->m00 * pM1->m00 + pM2->m10 * pM1->m01 + pM2->m20 * pM1->m02;
			mat.m01 = pM2->m01 * pM1->m00 + pM2->m11 * pM1->m01 + pM2->m21 * pM1->m02;
			mat.m02 = pM2->m02 * pM1->m00 + pM2->m12 * pM1->m01 + pM2->m22 * pM1->m02;
			mat.m03 = 0.0f;
			mat.m10 = pM2->m00 * pM1->m10 + pM2->m10 * pM1->m11 + pM2->m20 * pM1->m12;
			mat.m11 = pM2->m01 * pM1->m10 + pM2->m11 * pM1->m11 + pM2->m21 * pM1->m12;
			mat.m12 = pM2->m02 * pM1->m10 + pM2->m12 * pM1->m11 + pM2->m22 * pM1->m12;
			mat.m13 = 0.0f;
			mat.m20 = pM2->m00 * pM1->m20 + pM2->m10 * pM1->m21 + pM2->m20 * pM1->m22;
			mat.m21 = pM2->m01 * pM1->m20 + pM2->m11 * pM1->m21 + pM2->m21 * pM1->m22;
			mat.m22 = pM2->m02 * pM1->m20 + pM2->m12 * pM1->m21 + pM2->m22 * pM1->m22;
			mat.m23 = 0.0f;
			mat.m30 = pM2->m00 * pM1->m30 + pM2->m10 * pM1->m31 + pM2->m20 * pM1->m32 + pM2->m30;
			mat.m31 = pM2->m01 * pM1->m30 + pM2->m11 * pM1->m31 + pM2->m21 * pM1->m32 + pM2->m31;
			mat.m32 = pM2->m02 * pM1->m30 + pM2->m12 * pM1->m31 + pM2->m22 * pM1->m32 + pM2->m32;
			mat.m33 = 1.0f;
			Copy( pOut, &mat);
		#endif
	}


	/////////////////////////////////////////////////////////////////
	// 2005.3.3 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 370.006 ms
	// Vfpu unUse(function으로 호출): 1479.989 ms
	/////////////////////////////////////////////////////////////////
	// 전치 행렬
	inline
	void Transpose( MATRIX *pOut, const MATRIX *pM )
	{
		#if defined( I3G_PSP)
			__asm__ (
				".set			push\n"					// save assember option
				".set			noreorder\n"			// suppress reordering
				"lv.q			c000,  0 + %1\n"		// c100 = pm1->x
				"lv.q			c010, 16 + %1\n"		// c110 = pm1->y
				"lv.q			c020, 32 + %1\n"		// c120 = pm1->z
				"lv.q			c030, 48 + %1\n"		// c130 = pm1->w
				"sv.q			r000,  0 + %0\n"		// pm0->x = r000
				"sv.q			r001, 16 + %0\n"		// pm0->y = r010
				"sv.q			r002, 32 + %0\n"		// pm0->z = r020
				"sv.q			r003, 48 + %0\n"		// pm0->w = r030
				".set			pop\n"					// restore assember option
				: "=m"(*pOut)
				: "m"(*pM)
			);
		#elif defined( I3G_DX)
			D3DXMatrixTranspose( (D3DXMATRIX *)pOut, (const D3DXMATRIX *)pM);
		#else
			MATRIX m;
			m.m00 = pM->m00;	m.m01 = pM->m10;	m.m02 = pM->m20;	m.m03 = pM->m30;
			m.m10 = pM->m01;	m.m11 = pM->m11;	m.m12 = pM->m21;	m.m13 = pM->m31;
			m.m20 = pM->m02;	m.m21 = pM->m12;	m.m22 = pM->m22;	m.m23 = pM->m32;
			m.m30 = pM->m03;	m.m31 = pM->m13;	m.m32 = pM->m23;	m.m33 = pM->m33;
			memcpy( pOut, &m, sizeof(MATRIX));
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.3 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 396.606 ms
	// Vfpu unUse(function으로 호출): 3420.033 ms
	/////////////////////////////////////////////////////////////////
	// 전치 행렬
	inline
	void Transpose3x3( MATRIX *pOut, const MATRIX *pIn )
	{
		#if defined( I3G_PSP)
			#if 1
				// 2005.2.21 Lene3	
				__asm__ (
					".set			push\n"
					".set			noreorder\n"
					"lv.q			c000,  0 + %1\n"		// c000 = pIn->x
					"lv.q			c010, 16 + %1\n"		// c010 = pIn->y
					"lv.q			c020, 32 + %1\n"		// c020 = pIn->z
					"lv.q			c030, 48 + %1\n"		// c030 = pIn->w
					
					"vmmov.t		e100, m000\n"			// 
					"vmov.q			r103, r003\n"
					"vmov.t			c130, c030\n"
					
					"sv.q			c100,  0 + %0\n"		// pOut->x = r000
					"sv.q			c110, 16 + %0\n"		// pOut->y = r001
					"sv.q			c120, 32 + %0\n"		// pOut->z = r002
					"sv.q			c130, 48 + %0\n"		// pOut->w = r003
					
					".set			pop\n"
					:"=m"(*pOut)
					:"m"(*pIn)
				);
			#else
				MATRIX m, * pM;

				pM = (MATRIX *) pIn;

				SetRight(	&m, Get11(pM), Get21(pM), Get31(pM), Get14(pM));
				SetUp(		&m, Get12(pM), Get22(pM), Get32(pM), Get24(pM));
				SetAt(		&m, Get13(pM), Get23(pM), Get33(pM), Get34(pM));
				SetPos(		&m, Get41(pM), Get42(pM), Get43(pM), Get44(pM));

				memcpy( pOut, &m, sizeof(MATRIX));
			#endif
		#else
			MATRIX m, * pM;

			pM = (MATRIX *) pIn;

			SetRight(	&m, Get11(pM), Get21(pM), Get31(pM), Get14(pM));
			SetUp(		&m, Get12(pM), Get22(pM), Get32(pM), Get24(pM));
			SetAt(		&m, Get13(pM), Get23(pM), Get33(pM), Get34(pM));
			SetPos(		&m, Get41(pM), Get42(pM), Get43(pM), Get44(pM));

			memcpy( pOut, &m, sizeof(MATRIX));
		#endif
	}

	I3_EXPORT_MATH MATRIX * Inverse( MATRIX *pOut, REAL32 *pDeterminant, const MATRIX *pM );

	inline
	void		InverseAffine( MATRIX * pOut, const MATRIX * pM)
	{
		Transpose3x3( pOut, pM);
		i3Vector::Scale( GetPos( pOut), GetPos( (MATRIX *) pM), -1.0f);
	}

	inline
	void SetScale( MATRIX *pOut, REAL32 sx, REAL32 sy, REAL32 sz )
	{
		Identity( pOut);

		pOut->m00 = sx;
		pOut->m11 = sy;
		pOut->m22 = sz;
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.3 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 506.163 ms
	// Vfpu unUse(function으로 호출): 1210.004 ms
	/////////////////////////////////////////////////////////////////
	inline
	void PostScale( MATRIX *pOut, REAL32 sx, REAL32 sy, REAL32 sz)
	{
		#if defined( I3G_PSP)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,   %1\n"				// t0   = s
					"mtv			$8,   s200\n"			// s010 = t0

					"mfc1			$8,   %2\n"				// t0   = s
					"mtv			$8,   s201\n"			// s010 = t0

					"mfc1			$8,   %3\n"				// t0   = s
					"mtv			$8,   s202\n"			// s010 = t0

					"lv.q			c100,  0 + %0\n"		// c100 = pm0->x
					"lv.q			c110, 16 + %0\n"		// c110 = pm0->y
					"lv.q			c120, 32 + %0\n"		// c120 = pm0->z
					"lv.q			c130, 48 + %0\n"		// c130 = pm0->w

					"vscl.q			r100, r100,	s200\n"		//
					"vscl.q			r101, r101,	s201\n"		//
					"vscl.q			r102, r102,	s202\n"		//

					"sv.q			c100,	0 + %0\n"
					"sv.q			c110,	16 + %0\n"
					"sv.q			c120,	32 + %0\n"
					"sv.q			c130,	48 + %0\n"

					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "f"(sx), "f"(sy), "f"(sz)
				);
			#else
				pOut->m00 *= sx;
				pOut->m10 *= sx;
				pOut->m20 *= sx;
				pOut->m30 *= sx;

				pOut->m01 *= sy;
				pOut->m11 *= sy;
				pOut->m21 *= sy;
				pOut->m31 *= sy;

				pOut->m02 *= sz;
				pOut->m12 *= sz;
				pOut->m22 *= sz;
				pOut->m32 *= sz;
			#endif

		#else
			pOut->m00 *= sx;
			pOut->m10 *= sx;
			pOut->m20 *= sx;
			pOut->m30 *= sx;

			pOut->m01 *= sy;
			pOut->m11 *= sy;
			pOut->m21 *= sy;
			pOut->m31 *= sy;

			pOut->m02 *= sz;
			pOut->m12 *= sz;
			pOut->m22 *= sz;
			pOut->m32 *= sz;
		#endif
	}

	inline
	void PostScale( MATRIX *pOut, VEC3D * pVec)
	{
		PostScale( pOut, i3Vector::GetX(pVec), i3Vector::GetY(pVec), i3Vector::GetZ(pVec));
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.3 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 466.627 ms
	// Vfpu unUse(function으로 호출): 1220.010 ms
	/////////////////////////////////////////////////////////////////
	inline
	void PreScale( MATRIX *pOut, REAL32 sx, REAL32 sy, REAL32 sz)
	{
		#if defined( I3G_PSP)
			#if 1
			__asm__ (
				".set			push\n"					// save assember option
				".set			noreorder\n"			// suppress reordering
				"mfc1			$8,   %1\n"				// t0   = s
				"mtv			$8,   s200\n"			// s010 = t0

				"mfc1			$8,   %2\n"				// t0   = s
				"mtv			$8,   s201\n"			// s010 = t0

				"mfc1			$8,   %3\n"				// t0   = s
				"mtv			$8,   s202\n"			// s010 = t0

				"lv.q			c100,  0 + %0\n"		// c100 = pm0->x
				"lv.q			c110, 16 + %0\n"		// c110 = pm0->y
				"lv.q			c120, 32 + %0\n"		// c120 = pm0->z
				"lv.q			c130, 48 + %0\n"		// c130 = pm0->w

				"vscl.q			c100, c100,	s200\n"		//
				"vscl.q			c110, c110,	s201\n"		//
				"vscl.q			c120, c120,	s202\n"		//

				"sv.q			c100,	0 + %0\n"
				"sv.q			c110,	16 + %0\n"
				"sv.q			c120,	32 + %0\n"
				"sv.q			c130,	48 + %0\n"

				".set			pop\n"					// restore assember option
				: "=m"(*pOut)
				: "f"(sx), "f"(sy), "f"(sz)
				);
			#else
				pOut->m00 *= sx;
				pOut->m01 *= sx;
				pOut->m02 *= sx;
				pOut->m03 *= sx;

				pOut->m10 *= sy;
				pOut->m11 *= sy;
				pOut->m12 *= sy;
				pOut->m13 *= sy;

				pOut->m20 *= sz;
				pOut->m21 *= sz;
				pOut->m22 *= sz;
				pOut->m23 *= sz;
			#endif

		#else
			pOut->m00 *= sx;
			pOut->m01 *= sx;
			pOut->m02 *= sx;
			pOut->m03 *= sx;

			pOut->m10 *= sy;
			pOut->m11 *= sy;
			pOut->m12 *= sy;
			pOut->m13 *= sy;

			pOut->m20 *= sz;
			pOut->m21 *= sz;
			pOut->m22 *= sz;
			pOut->m23 *= sz;
		#endif
	}

	inline
	void GetScale( MATRIX * pIn, VEC3D * pOut)
	{
		VEC3D * pRight, *pUp, *pAt;

		pRight = GetRight( pIn);
		pUp = GetUp( pIn);
		pAt = GetAt( pIn);

		i3Vector::Set( pOut, i3Vector::Length( pRight), i3Vector::Length( pUp), i3Vector::Length( pAt));
	}

	inline
	void SetTranslate( MATRIX *pOut, REAL32 x, REAL32 y, REAL32 z )
	{
		Identity( pOut);

		pOut->m30 = x;
		pOut->m31 = y;
		pOut->m32 = z;
	}

	inline
	void SetTranslate( MATRIX *pOut, VEC3D *pVec)
	{
		SetTranslate( pOut, getX(pVec), getY(pVec), getZ(pVec));
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.3 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 426.652 ms
	// Vfpu unUse(function으로 호출): 1179.945 ms
	/////////////////////////////////////////////////////////////////
	inline
	void PreTranslate( MATRIX *pOut, REAL32 x, REAL32 y, REAL32 z )
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_MATRIX)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,   %1\n"				// t0   = s
					"mtv			$8,   s200\n"			// s010 = t0

					"mfc1			$8,   %2\n"				// t0   = s
					"mtv			$8,   s201\n"			// s010 = t0

					"mfc1			$8,   %3\n"				// t0   = s
					"mtv			$8,   s202\n"			// s010 = t0

					"vone.s			s203\n"

					"lv.q			c100,  0 + %0\n"		// r100 = pm0->x
					"lv.q			c110, 16 + %0\n"		// r101 = pm0->y
					"lv.q			c120, 32 + %0\n"		// r102 = pm0->z
					"lv.q			c130, 48 + %0\n"		// r103 = pm0->z

					"vtfm4.q		c000, e100,	c200\n"

					"sv.q			c000,	48 + %0\n"

					".set			pop\n"					// restore assember option
					: "=m"(*pOut)
					: "f"(x), "f"(y), "f"(z)
					: "$8"
				);
			#else
				pOut->m30 = (x * pOut->m00) + (y * pOut->m10) + (z * pOut->m20) + pOut->m30;
				pOut->m31 = (x * pOut->m01) + (y * pOut->m11) + (z * pOut->m21) + pOut->m31;
				pOut->m32 = (x * pOut->m02) + (y * pOut->m12) + (z * pOut->m22) + pOut->m32;
				pOut->m33 = (x * pOut->m03) + (y * pOut->m13) + (z * pOut->m23) + pOut->m33;
			#endif

		#else
			pOut->m30 = (x * pOut->m00) + (y * pOut->m10) + (z * pOut->m20) + pOut->m30;
			pOut->m31 = (x * pOut->m01) + (y * pOut->m11) + (z * pOut->m21) + pOut->m31;
			pOut->m32 = (x * pOut->m02) + (y * pOut->m12) + (z * pOut->m22) + pOut->m32;
			pOut->m33 = (x * pOut->m03) + (y * pOut->m13) + (z * pOut->m23) + pOut->m33;
		#endif
	}

	inline
	void PreTranslate( MATRIX *pOut, VEC3D * pVec)
	{
		PreTranslate( pOut, getX( pVec), getY(pVec), getZ(pVec));
	}

	inline
	void PostTranslate( MATRIX *pOut, REAL32 x, REAL32 y, REAL32 z )
	{
		MATRIX temp;

		SetTranslate( &temp, x, y, z);
		Mul( pOut, pOut, &temp);
	}

	inline
	void PostTranslate( MATRIX *pOut, VEC3D * pVec)
	{
		MATRIX temp;

		SetTranslate( &temp, pVec);
		Mul( pOut, pOut, &temp);
	}

	//
	// V ( x, y, z)
	// C : cos
	// S : sin
	//
	// m00 : C + x^2 * (1 - C)
	// m01 : xy(1 - C) - zS
	// m02 : xz(1 - C) + yS
	//	
	// m10 : xy(1 - C) + zS
	// m11 : C + y^2(1 - C)
	// m12 : yz(1 - C) - xS
	//
	// m20 : xz(1 - C) - yS
	// m21 : yz(1 - C) + xS
	// m22 : C + z^2(1 - C)
	//
	inline
	void SetRotateAxis( MATRIX *pOut, const VEC3D *pV, REAL32 Angle )
	{
		i3Matrix_SetRotateAxis( pOut, pV, Angle);
	}

	inline
	void SetRotateAxisDeg( MATRIX *pOut, const VEC3D *pV, REAL32 Degree )
	{
		SetRotateAxis( pOut, pV, I3_DEG2RAD( Degree));
	}

	inline
	void PreRotateAxis( MATRIX * pOut, const VEC3D * pV, REAL32 Angle)
	{
		MATRIX tmp;

		SetRotateAxis( &tmp, pV, Angle);
		Mul( pOut, &tmp, pOut);;
	}

	inline
	void	PreRotateAxisDeg( MATRIX * pOut, const VEC3D * pV, REAL32 Degree)
	{
		PreRotateAxis( pOut, pV, I3_DEG2RAD( Degree));
	}

	inline
	void	PostRotateAxis( MATRIX * pOut, const VEC3D * pV, REAL32 Angle)
	{
		MATRIX tmp;

		SetRotateAxis( &tmp, pV, Angle);
		Mul( pOut, pOut, &tmp);
	}

	inline
	void	PostRotateAxisDeg( MATRIX * pOut, const VEC3D * pV, REAL32 Degree)
	{
		PostRotateAxis( pOut, pV, I3_DEG2RAD( Degree));
	}

	inline
	void SetRotateX( MATRIX *pOut, REAL32 Angle )
	{
		REAL32 s = sinf( Angle);
		REAL32 c = cosf( Angle);

		Identity( pOut);

		pOut->m11 = c;
		pOut->m12 = s;
		pOut->m21 = -s;
		pOut->m22 = c;
	}

	inline
	void SetRotateXDeg( MATRIX *pOut, REAL32 Degree )
	{
		SetRotateX( pOut, I3_DEG2RAD( Degree));
	}

	inline
	void SetRotateY( MATRIX *pOut, REAL32 Angle )
	{
		REAL32 s = sinf( Angle);
		REAL32 c = cosf( Angle);

		Identity( pOut);

		pOut->m00 = c;
		pOut->m02 = -s;
		pOut->m20 = s;
		pOut->m22 = c;
	}

	inline
	void SetRotateYDeg( MATRIX *pOut, REAL32 Degree )
	{
		SetRotateY( pOut, I3_DEG2RAD( Degree));
	}

	inline
	void SetRotateZ( MATRIX *pOut, REAL32 Angle )
	{
		REAL32 s = sinf( Angle);
		REAL32 c = cosf( Angle);

		Identity( pOut);

		pOut->m00 = c;
		pOut->m01 = s;
		pOut->m10 = -s;
		pOut->m11 = c;
	}

	inline
	void SetRotateZDeg( MATRIX *pOut, REAL32 Degree)
	{
		SetRotateZ( pOut, I3_DEG2RAD( Degree));
	}


	// PreRotate
	inline
	void PreRotateX( MATRIX *pOut, REAL32 Angle )
	{
		REAL32 s = sinf( Angle);
		REAL32 c = cosf( Angle);
		REAL32 m10, m11, m12, m13;

		m10 = pOut->m10;
		m11 = pOut->m11;
		m12 = pOut->m12;
		m13 = pOut->m13;

		pOut->m10 = (c * m10)  + (s * pOut->m20);
		pOut->m11 = (c * m11)  + (s * pOut->m21);
		pOut->m12 = (c * m12)  + (s * pOut->m22);
		pOut->m13 = (c * m13)  + (s * pOut->m23);

		pOut->m20 = (-s * m10)  + (c * pOut->m20);
		pOut->m21 = (-s * m11)  + (c * pOut->m21);
		pOut->m22 = (-s * m12)  + (c * pOut->m22);
		pOut->m23 = (-s * m13)  + (c * pOut->m23);
	}

	inline
	void PreRotateY( MATRIX *pOut, REAL32 Angle )
	{
		REAL32 s = sinf( Angle);
		REAL32 c = cosf( Angle);
		REAL32 m00, m01, m02, m03;

		m00 = pOut->m00;
		m01 = pOut->m01;
		m02 = pOut->m02;
		m03 = pOut->m03;

		pOut->m00 = (c * m00)  + (-s * pOut->m20);
		pOut->m01 = (c * m01)  + (-s * pOut->m21);
		pOut->m02 = (c * m02)  + (-s * pOut->m22);
		pOut->m03 = (c * m03)  + (-s * pOut->m23);

		pOut->m20 = (s * m00)  + (c * pOut->m20);
		pOut->m21 = (s * m01)  + (c * pOut->m21);
		pOut->m22 = (s * m02)  + (c * pOut->m22);
		pOut->m23 = (s * m03)  + (c * pOut->m23);
	}

	inline
	void PreRotateZ( MATRIX *pOut, REAL32 Angle )
	{
		REAL32 s = sinf( Angle);
		REAL32 c = cosf( Angle);
		REAL32 m00, m01, m02, m03;

		m00 = pOut->m00;
		m01 = pOut->m01;
		m02 = pOut->m02;
		m03 = pOut->m03;

		pOut->m00 = (c * m00)  + (s * pOut->m10);
		pOut->m01 = (c * m01)  + (s * pOut->m11);
		pOut->m02 = (c * m02)  + (s * pOut->m12);
		pOut->m03 = (c * m03)  + (s * pOut->m13);

		pOut->m10 = (-s * m00)  + (c * pOut->m10);
		pOut->m11 = (-s * m01)  + (c * pOut->m11);
		pOut->m12 = (-s * m02)  + (c * pOut->m12);
		pOut->m13 = (-s * m03)  + (c * pOut->m13);
	}

	inline
	void PreRotateXDeg( MATRIX *pOut, REAL32 Angle )
	{
		PreRotateX( pOut, I3_DEG2RAD(Angle) );
	}

	inline
	void PreRotateYDeg( MATRIX *pOut, REAL32 Angle )
	{
		PreRotateY( pOut, I3_DEG2RAD(Angle) );
	}

	inline
	void PreRotateZDeg( MATRIX *pOut, REAL32 Angle )
	{
		PreRotateZ( pOut, I3_DEG2RAD(Angle) );
	}

	// PostRotate
	inline
	void PostRotateX( MATRIX *pOut, REAL32 Angle )
	{
		REAL32 s = sinf( Angle);
		REAL32 c = cosf( Angle);
		REAL32 tmp;

		tmp = pOut->m01;
		pOut->m01 = (tmp * c) + (pOut->m02 * -s);
		pOut->m02 = (tmp * s) + (pOut->m02 * c);

		tmp = pOut->m11;
		pOut->m11 = (tmp * c) + (pOut->m12 * -s);
		pOut->m12 = (tmp * s) + (pOut->m12 * c);

		tmp = pOut->m21;
		pOut->m21 = (tmp * c) + (pOut->m22 * -s);
		pOut->m22 = (tmp * s) + (pOut->m22 * c);

		tmp = pOut->m31;
		pOut->m31 = (tmp * c) + (pOut->m32 * -s);
		pOut->m32 = (tmp * s) + (pOut->m32 * c);
	}

	inline
	void PostRotateY( MATRIX *pOut, REAL32 Angle )
	{
		REAL32 s = sinf( Angle);
		REAL32 c = cosf( Angle);
		REAL32 tmp;

		tmp = pOut->m00;
		pOut->m00 = (tmp * c)  + (pOut->m02 * s);
		pOut->m02 = (tmp * -s)  + (pOut->m02 * c);

		tmp = pOut->m10;
		pOut->m10 = (tmp * c)  + (pOut->m12 * s);
		pOut->m12 = (tmp * -s)  + (pOut->m12 * c);

		tmp = pOut->m20;
		pOut->m20 = (tmp * c)  + (pOut->m22 * s);
		pOut->m22 = (tmp * -s)  + (pOut->m22 * c);

		tmp = pOut->m30;
		pOut->m30 = (tmp * c)  + (pOut->m32 * s);
		pOut->m32 = (tmp * -s)  + (pOut->m32 * c);
	}

	inline
	void PostRotateZ( MATRIX *pOut, REAL32 Angle )
	{
		REAL32 s = sinf( Angle);
		REAL32 c = cosf( Angle);
		REAL32 tmp;

		tmp = pOut->m00;
		pOut->m00 = (tmp * c)  + (pOut->m01 * -s);
		pOut->m01 = (tmp * s)  + (pOut->m01 * c);

		tmp = pOut->m10;
		pOut->m10 = (tmp * c)  + (pOut->m11 * -s);
		pOut->m11 = (tmp * s)  + (pOut->m11 * c);

		tmp = pOut->m20;
		pOut->m20 = (tmp * c)  + (pOut->m21 * -s);
		pOut->m21 = (tmp * s)  + (pOut->m21 * c);

		tmp = pOut->m30;
		pOut->m30 = (tmp * c)  + (pOut->m31 * -s);
		pOut->m31 = (tmp * s)  + (pOut->m31 * c);
	}

	inline
	void PostRotateXDeg( MATRIX *pOut, REAL32 Angle )
	{
		PostRotateX( pOut, I3_DEG2RAD(Angle) );
	}

	inline
	void PostRotateYDeg( MATRIX *pOut, REAL32 Angle )
	{
		PostRotateY( pOut, I3_DEG2RAD(Angle) );
	}

	inline
	void PostRotateZDeg( MATRIX *pOut, REAL32 Angle )
	{
		PostRotateZ( pOut, I3_DEG2RAD(Angle) );
	}


	/////////////////////////////////////////////////////////////////
	// 2005.3.3 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 536.439 ms
	// Vfpu unUse(function으로 호출): 2230.020 ms
	/////////////////////////////////////////////////////////////////
	//	Q =		( x, y, z, w)
	//
	//			| (1 - 2y^2 - 2z^2),	(2xy - 2wz),		(2xz + 2wy)			|
	//	R =		| (2xy + 2wz),			(1 - 2x^2 - 2z^2),	(2yz - 2wx)			|
	//			| (2xz - 2wy),			(2yz + 2wx),		(1 - 2x^2 - 2y^2)	|
	inline
	void SetRotateQuat( MATRIX *pOut, const QUATERNION *pQ)
	{
		#if defined( I3G_PSP)
			//#if defined( VFPU_ASM_MATRIX)
			#if 1
				__asm__ (
					".set			push\n"							// save assember option
					".set			noreorder\n"					// suppress reordering
					"lv.q			c130,  %1\n"					// c130 = *pq = (x, y, z, w)
					"vmov.q			c100, c130[ W,  Z, -Y, -X]\n"	// c100 = ( w,  z, -y, -x)
					"vmov.q			c110, c130[-Z,  W,  X, -Y]\n"	// c110 = (-z,  w,  x, -y)
					"vmov.q			c120, c130[ Y, -X,  W, -Z]\n"	// c120 = ( y, -x,  w, -z)
					"vmov.q			c200, c130[ W,  Z, -Y,  X]\n"	// c200 = ( w,  z, -y,  x)
					"vmov.q			c210, c130[-Z,  W,  X,  Y]\n"	// c210 = (-z,  w,  x,  y)
					"vmov.q			c220, c130[ Y, -X,  W,  Z]\n"	// c220 = ( y, -x,  w,  z)
					"vmov.q			c230, c130[-X, -Y, -Z,  W]\n"	// c230 = (-x, -y, -z,  w)
					"vrmmul.q		e000, e100, e200\n"				// e000 = e100 * e200
					"sv.q			c000,  0 + %0\n"				// pm->x = c000
					"sv.q			c010, 16 + %0\n"				// pm->y = c010
					"sv.q			c020, 32 + %0\n"				// pm->z = c020
					"sv.q			c030, 48 + %0\n"				// pm->w = c030
					".set			pop\n"							// restore assember option
					: "=m"(*pOut)
					: "m"(*pQ)
					);
			#else
				REAL32 wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

				x2 = pQ->vec[0] + pQ->vec[0];
				y2 = pQ->vec[1] + pQ->vec[1];
				z2 = pQ->vec[2] + pQ->vec[2];

				xx = pQ->vec[0] * x2;   xy = pQ->vec[0] * y2;   xz = pQ->vec[0] * z2;
				yy = pQ->vec[1] * y2;   yz = pQ->vec[1] * z2;   zz = pQ->vec[2] * z2;
				wx = pQ->vec[3] * x2;   wy = pQ->vec[3] * y2;   wz = pQ->vec[3] * z2;

				pOut->m00 = (1.0f - yy - zz);
				pOut->m10 = xy - wz;
				pOut->m20 = xz + wy;
				pOut->m30 = 0.0f;
				               
				pOut->m01 = xy + wz;
				pOut->m11 = 1.0f - xx - zz;
				pOut->m21 = yz - wx;
				pOut->m31 = 0.0f;
				               
				pOut->m02 = xz - wy;
				pOut->m12 = yz + wx;
				pOut->m22 = 1.0f - xx - yy;
				pOut->m32 = 0.0f;
				               
				pOut->m03 = 0.0f;
				pOut->m13 = 0.0f;
				pOut->m23 = 0.0f;
				pOut->m33 = 1.0f;
			#endif //VFPU_ASM

		#else
			REAL32 wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

			x2 = pQ->vec[0] + pQ->vec[0];
			y2 = pQ->vec[1] + pQ->vec[1];
			z2 = pQ->vec[2] + pQ->vec[2];

			xx = pQ->vec[0] * x2;   xy = pQ->vec[0] * y2;   xz = pQ->vec[0] * z2;
			yy = pQ->vec[1] * y2;   yz = pQ->vec[1] * z2;   zz = pQ->vec[2] * z2;
			wx = pQ->vec[3] * x2;   wy = pQ->vec[3] * y2;   wz = pQ->vec[3] * z2;

			pOut->m00 = (1.0f - yy - zz);
			pOut->m10 = xy - wz;
			pOut->m20 = xz + wy;
			pOut->m30 = 0.0f;

			pOut->m01 = xy + wz;
			pOut->m11 = 1.0f - xx - zz;
			pOut->m21 = yz - wx;
			pOut->m31 = 0.0f;

			pOut->m02 = xz - wy;
			pOut->m12 = yz + wx;
			pOut->m22 = 1.0f - xx - yy;
			pOut->m32 = 0.0f;

			pOut->m03 = 0.0f;
			pOut->m13 = 0.0f;
			pOut->m23 = 0.0f;
			pOut->m33 = 1.0f;
		#endif
	}

	inline
	void GetRotateQuat( QUATERNION * pOut, const MATRIX * pMatrix)
	{
		i3QuaternionFromMatrix( pOut, (MATRIX *) pMatrix);
	}

	// Builds a right-handed look-at matrix.
	//
	// This function uses the following formula to compute the returned matrix.
	//
	// zaxis = normal(Eye - At)
	// xaxis = normal(cross(Up, zaxis))
	// yaxis = cross(zaxis, xaxis)
	// 
	// xaxis.x           yaxis.x           zaxis.x          0
	// xaxis.y           yaxis.y           zaxis.y          0
	// xaxis.z           yaxis.z           zaxis.z          0
	// -dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  1
	inline
	void MakeLookAtRH( MATRIX *pOut, const VEC3D *pEye, const VEC3D *pAt, const VEC3D *pUp )
	{
		VEC3D zaxis, xaxis, yaxis;

		i3Vector::Sub( &zaxis, pEye, pAt);
		i3Vector::Normalize( &zaxis, &zaxis);

		i3Vector::Cross( &xaxis, pUp, &zaxis);
		i3Vector::Normalize( &xaxis, &xaxis);

		i3Vector::Cross( &yaxis, &zaxis, &xaxis);

		pOut->m00 = i3Vector::GetX( &xaxis);
		pOut->m01 = i3Vector::GetX( &yaxis);
		pOut->m02 = i3Vector::GetX( &zaxis);
		pOut->m03 = 0.0f;

		pOut->m10 = i3Vector::GetY( &xaxis);
		pOut->m11 = i3Vector::GetY( &yaxis);
		pOut->m12 = i3Vector::GetY( &zaxis);
		pOut->m13 = 0.0f;

		pOut->m20 = i3Vector::GetZ( &xaxis);
		pOut->m21 = i3Vector::GetZ( &yaxis);
		pOut->m22 = i3Vector::GetZ( &zaxis);
		pOut->m23 = 0.0f;

		pOut->m30 = -i3Vector::Dot( &xaxis, pEye);
		pOut->m31 = -i3Vector::Dot( &yaxis, pEye);
		pOut->m32 = -i3Vector::Dot( &zaxis, pEye);
		pOut->m33 = 1.0f;
	}

	// Builds a left-handed look-at matrix.
	//
	// This function uses the following formula to compute the returned matrix.
	//
	// zaxis = normal(At - Eye)
	// xaxis = normal(cross(Up, zaxis))
	// yaxis = cross(zaxis, xaxis)
	// 
	// xaxis.x           yaxis.x           zaxis.x          0
	// xaxis.y           yaxis.y           zaxis.y          0
	// xaxis.z           yaxis.z           zaxis.z          0
	// -dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  1
	inline
	void MakeLookAtLH( MATRIX *pOut, const VEC3D *pEye, const VEC3D *pAt, const VEC3D *pUp )
	{
		VEC3D zaxis, xaxis, yaxis;

		i3Vector::Sub( &zaxis, pAt, pEye);
		i3Vector::Normalize( &zaxis, &zaxis);

		i3Vector::Cross( &xaxis, pUp, &zaxis);
		i3Vector::Normalize( &xaxis, &xaxis);

		i3Vector::Cross( &yaxis, &zaxis, &xaxis);

		pOut->m00 = i3Vector::GetX( &xaxis);
		pOut->m01 = i3Vector::GetX( &yaxis);
		pOut->m02 = i3Vector::GetX( &zaxis);
		pOut->m03 = 0.0f;

		pOut->m10 = i3Vector::GetY( &xaxis);
		pOut->m11 = i3Vector::GetY( &yaxis);
		pOut->m12 = i3Vector::GetY( &zaxis);
		pOut->m13 = 0.0f;

		pOut->m20 = i3Vector::GetZ( &xaxis);
		pOut->m21 = i3Vector::GetZ( &yaxis);
		pOut->m22 = i3Vector::GetZ( &zaxis);
		pOut->m23 = 0.0f;

		pOut->m30 = -i3Vector::Dot( &xaxis, pEye);
		pOut->m31 = -i3Vector::Dot( &yaxis, pEye);
		pOut->m32 = -i3Vector::Dot( &zaxis, pEye);
		pOut->m33 = 1.0f;
	}

	// Builds a right-handed perspective projection matrix.
	//
	// All the parameters of the D3DXMatrixPerspectiveRH function are distances in camera space.
	// The parameters describe the dimensions of the view volume.
	//
	// This function uses the following formula to compute the returned matrix.
	// 
	// 2*zn/w  0       0              0
	// 0       2*zn/h  0              0
	// 0       0       zf/(zn-zf)    -1
	// 0       0       zn*zf/(zn-zf)  0
	inline
	void PerspectiveRH ( MATRIX *pOut, REAL32 w, REAL32 h, REAL32 zn, REAL32 zf )
	{
		REAL32 r_zdepth = 1.0f / (zn - zf);

		pOut->m00 = zn * 2.0f / w;
		pOut->m01 = 0.0f;
		pOut->m02 = 0.0f;
		pOut->m03 = 0.0f;

		pOut->m10 = 0.0f;
		pOut->m11 = zn * 2.0f / h;
		pOut->m12 = 0.0f;
		pOut->m13 = 0.0f;

		pOut->m20 = 0.0f;
		pOut->m21 = 0.0f;
		pOut->m22 = zf * r_zdepth;
		pOut->m23 = -1.0f;

		pOut->m30 = 0.0f;
		pOut->m31 = 0.0f;
		pOut->m32 = zn * pOut->m22;
		pOut->m33 = 0.0f;
	}

	// Builds a left-handed perspective projection matrix.
	//
	// All the parameters of the D3DXMatrixPerspectiveLH function are distances in camera space.
	// The parameters describe the dimensions of the view volume.
	//
	// This function uses the following formula to compute the returned matrix.
	//
	// 2*zn/w  0       0              0
	// 0       2*zn/h  0              0
	// 0       0       zf/(zf-zn)     1
	// 0       0       zn*zf/(zn-zf)  0
	inline
	void PerspectiveLH ( MATRIX *pOut, REAL32 w, REAL32 h, REAL32 zn, REAL32 zf )
	{
		REAL32 r_zdepth = 1.0f / (zf - zn);

		pOut->m00 = zn * 2.0f / w;
		pOut->m01 = 0.0f;
		pOut->m02 = 0.0f;
		pOut->m03 = 0.0f;

		pOut->m10 = 0.0f;
		pOut->m11 = zn * 2.0f / h;
		pOut->m12 = 0.0f;
		pOut->m13 = 0.0f;

		pOut->m20 = 0.0f;
		pOut->m21 = 0.0f;
		pOut->m22 = zf * r_zdepth;
		pOut->m23 = -1.0f;

		pOut->m30 = 0.0f;
		pOut->m31 = 0.0f;
		pOut->m32 = zn * zf / (zn - zf);
		pOut->m33 = 0.0f;
	}

	// Builds a right-handed perspective projection matrix based on a field of view (FOV).
	//
	// This function uses the following formula to compute the returned matrix.
	//
	// h = cos(fov/2) / sin(fov/2);
	// w = h / aspect;
	// 
	//	2*zn/w  0       0,             0
	//	0       2*zn/h  0,             0
	//	0       0       zf/(zn-zf)    -1
	//	0       0       zn*zf/(zn-zf)  0
	inline
	void PerspectiveFOVRH( MATRIX *pOut, REAL32 fovy, REAL32 Aspect, REAL32 zn, REAL32 zf )
	{
		REAL32 h, w;

		//h = i3Math::cos( fovy * 0.5f) / i3Math::sin( fovy * 0.5f);
		h = cosf( fovy * 0.5f) / sinf( fovy * 0.5f);
		w = h / Aspect;

		pOut->m00 = w;
		pOut->m01 = 0.0f;
		pOut->m02 = 0.0f;
		pOut->m03 = 0.0f;

		pOut->m10 = 0.0f;
		pOut->m11 = h;
		pOut->m12 = 0.0f;
		pOut->m13 = 0.0f;

		pOut->m20 = 0.0f;
		pOut->m21 = 0.0f;
		pOut->m22 = zf / (zn - zf);
		pOut->m23 = -1.0f;

		pOut->m30 = 0.0f;
		pOut->m31 = 0.0f;
		pOut->m32 = zn * zf / (zn - zf);
		pOut->m33 = 0.0f;
	}

	// Builds a left-handed perspective projection matrix based on a field of view (FOV).
	//
	// This function uses the following formula to compute the returned matrix.
	// 
	// h = cos(fov/2) / sin(fov/2);
	// w = h / aspect;
	// 
	// 2*zn/w  0       0              0
	// 0       2*zn/h  0              0
	// 0       0       zf/(zf-zn)     1
	// 0       0       zn*zf/(zn-zf)  0
	inline
	void PerspectiveFOVLH( MATRIX *pOut, REAL32 fovy, REAL32 Aspect, REAL32 zn, REAL32 zf )
	{
		REAL32 h, w;

		h = i3Math::cos( fovy * 0.5f) / i3Math::sin( fovy * 0.5f);
		w = h / Aspect;

		pOut->m00 = w;
		pOut->m01 = 0.0f;
		pOut->m02 = 0.0f;
		pOut->m03 = 0.0f;

		pOut->m10 = 0.0f;
		pOut->m11 = h;
		pOut->m12 = 0.0f;
		pOut->m13 = 0.0f;

		pOut->m20 = 0.0f;
		pOut->m21 = 0.0f;
		pOut->m22 = zf / (zf - zn);
		pOut->m23 = -1.0f;

		pOut->m30 = 0.0f;
		pOut->m31 = 0.0f;
		pOut->m32 = zn * zf / (zn - zf);
		pOut->m33 = 0.0f;
	}

	// Builds a right-handed orthogonal projection matrix.
	//
	// An orthogonal matrix is an invertible matrix for which the inverse of the matrix
	// is equal to the transpose of the matrix. 
	//
	// All the parameters of the D3DXMatrixOrthoRH function are distances in camera space.
	// The parameters describe the dimensions of the view volume.
	// 
	// This function uses the following formula to compute the returned matrix.
	// 
	// 2/w  0    0           0
	// 0    2/h  0           0
	// 0    0    1/(zn-zf)   0
	// 0    0    zn/(zn-zf)  1
	inline
	void OrthoRH( MATRIX *pOut, REAL32 w, REAL32 h, REAL32 zn, REAL32 zf )
	{
		REAL32 r_zd = 1.0f / (zn - zf);

		pOut->m00 = 2.0f / w;
		pOut->m01 = 0.0f;
		pOut->m02 = 0.0f;
		pOut->m03 = 0.0f;

		pOut->m10 = 0.0f;
		pOut->m11 = 2.0f / h;
		pOut->m12 = 0.0f;
		pOut->m13 = 0.0f;

		pOut->m20 = 0.0f;
		pOut->m21 = 0.0f;
		pOut->m22 = r_zd;
		pOut->m23 = 0.0f;

		pOut->m30 = 0.0f;
		pOut->m31 = 0.0f;
		pOut->m32 = zn * r_zd;
		pOut->m33 = 1.0f;
	}

	// Builds a left-handed orthogonal projection matrix.
	//
	// An orthogonal matrix is an invertible matrix for which the inverse of the matrix
	// is equal to the transpose of the matrix. 
	// 
	// All the parameters of the D3DXMatrixOrthoLH function are distances in camera space.
	// The parameters describe the dimensions of the view volume.
	//
	// This function uses the following formula to compute the returned matrix.
	//
	// 2/w  0    0           0
	// 0    2/h  0           0
	// 0    0    1/(zf-zn)   0
	// 0    0    zn/(zn-zf)  1
	//
	inline
	void OrthoLH( MATRIX *pOut, REAL32 w, REAL32 h, REAL32 zn, REAL32 zf )
	{
		REAL32 r_zd = 1.0f / (zn - zf);

		pOut->m00 = 2.0f / w;
		pOut->m01 = 0.0f;
		pOut->m02 = 0.0f;
		pOut->m03 = 0.0f;

		pOut->m10 = 0.0f;
		pOut->m11 = 2.0f / h;
		pOut->m12 = 0.0f;
		pOut->m13 = 0.0f;

		pOut->m20 = 0.0f;
		pOut->m21 = 0.0f;
		pOut->m22 = 1.0f / (zf - zn);
		pOut->m23 = 0.0f;

		pOut->m30 = 0.0f;
		pOut->m31 = 0.0f;
		pOut->m32 = zn * r_zd;
		pOut->m33 = 1.0f;
	}


	inline
	void TransformPoints( VEC3D * pOutVA, VEC3D * pInVA, INT32 count, MATRIX * pMatrix)
	{
		#if defined( I3G_DX)
			D3DXVec3TransformCoordArray( (D3DXVECTOR3 *) pOutVA, sizeof(VEC3D), (const D3DXVECTOR3 *) pInVA, sizeof(VEC3D), 
				(const D3DXMATRIX *) pMatrix, count);
			
		#else
			INT32 i;

			for( i = 0; i < count; i++)
			{
				i3Vector::TransformCoord( pOutVA, pInVA, pMatrix);

				pInVA ++;
				pOutVA ++;
			}
		#endif
	}

	inline
	void TransformVectors( VEC3D * pOutVA, VEC3D * pInVA, INT32 count, MATRIX * pMatrix)
	{
		#if defined( I3G_DX)
			D3DXVec3TransformNormalArray( (D3DXVECTOR3 *) pOutVA, sizeof(VEC3D), (const D3DXVECTOR3 *) pInVA, sizeof(VEC3D), 
				(const D3DXMATRIX *) pMatrix, count);
			
		#else
			INT32 i;

			for( i = 0; i < count; i++)
			{
				i3Vector::TransformNormal( pOutVA, pInVA, pMatrix);
				pInVA ++;
				pOutVA ++;
			}
		#endif
	}

	I3_EXPORT_MATH void     DecomposeEulerAngleXYZ( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     DecomposeEulerAngleXZY( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     DecomposeEulerAngleYXZ( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     DecomposeEulerAngleYZX( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     DecomposeEulerAngleZXY( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     DecomposeEulerAngleZYX( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     DecomposeEulerAngleXYX( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     DecomposeEulerAngleXZX( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     DecomposeEulerAngleYXY( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     DecomposeEulerAngleYZY( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     DecomposeEulerAngleZXZ( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     DecomposeEulerAngleZYZ( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     ComposeEulerAngleXYZ( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     ComposeEulerAngleXZY( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     ComposeEulerAngleYXZ( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     ComposeEulerAngleYZX( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     ComposeEulerAngleZXY( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     ComposeEulerAngleZYX( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     ComposeEulerAngleXYX( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     ComposeEulerAngleXZX( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     ComposeEulerAngleYXY( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     ComposeEulerAngleYZY( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     ComposeEulerAngleZXZ( MATRIX * pMat, REAL32 angles[3]);
	I3_EXPORT_MATH void     ComposeEulerAngleZYZ( MATRIX * pMat, REAL32 angles[3]);

	I3_EXPORT_MATH bool	isValid( const char * pszName, MATRIX * pMat);

	I3_EXPORT_MATH void	MakeWithUp( MATRIX * pOut, VEC3D * pUp, VEC3D * pAt);
	I3_EXPORT_MATH void	MakeWithAt( MATRIX * pOut, VEC3D * pAt, VEC3D * pUp);

	I3_EXPORT_MATH	void	SetToXML( i3XMLElement * pXML, const char * pszName, MATRIX * pMtx);
	I3_EXPORT_MATH	void	GetFromXML( i3XMLElement * pXML, const char * pszName, MATRIX * pMtx);

	#if defined( I3_DEBUG)
	inline
	void Dump( MATRIX * pMtx)
	{
		#if defined( I3G_PSP) || defined( I3G_DX) || defined( I3G_XBOX) || defined( I3_WINCE )
			I3TRACE( "%.3f, %.3f, %.3f, %.3f\n%.3f, %.3f, %.3f, %.3f\n%.3f, %.3f, %.3f, %.3f\n%.3f, %.3f, %.3f, %.3f\n",
				pMtx->m00, pMtx->m01, pMtx->m02, pMtx->m03,
				pMtx->m10, pMtx->m11, pMtx->m12, pMtx->m13,
				pMtx->m20, pMtx->m21, pMtx->m22, pMtx->m23,
				pMtx->m30, pMtx->m31, pMtx->m32, pMtx->m33);
		#elif defined( I3G_OGLES)	//	by KOMET 임시
			I3TRACE4( "%d, %d, %d, %d\n", (INT32)(pMtx->m00 * 1000.0f), (INT32)(pMtx->m01 * 1000.0f), (INT32)(pMtx->m02 * 1000.0f), (INT32)(pMtx->m03 * 1000.0f));
			I3TRACE4( "%d, %d, %d, %d\n", (INT32)(pMtx->m10 * 1000.0f), (INT32)(pMtx->m11 * 1000.0f), (INT32)(pMtx->m12 * 1000.0f), (INT32)(pMtx->m13 * 1000.0f));
			I3TRACE4( "%d, %d, %d, %d\n", (INT32)(pMtx->m20 * 1000.0f), (INT32)(pMtx->m21 * 1000.0f), (INT32)(pMtx->m22 * 1000.0f), (INT32)(pMtx->m23 * 1000.0f));
			I3TRACE4( "%d, %d, %d, %d\n", (INT32)(pMtx->m30 * 1000.0f), (INT32)(pMtx->m31 * 1000.0f), (INT32)(pMtx->m32 * 1000.0f), (INT32)(pMtx->m33 * 1000.0f));
		#endif

	}
	#endif
};

#endif
/////////////////////////////////////////////////////////////
