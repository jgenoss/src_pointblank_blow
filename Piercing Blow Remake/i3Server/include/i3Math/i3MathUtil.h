#if !defined( __I3_MATH_UTIL_H)
#define __I3_MATH_UTIL_H

#include "i3MathType.h"
#include "i3MathDefine.h"
#include "i3MathGlobalVariable.h"

enum TOUCH_TYPE
{
	TOUCH_NONE = 0,
	TOUCH_WHOLE,
	TOUCH_PARTIAL,
};

// 점과 사각형의 위치 관계를 나타내는 상수
#define CRS_LEFT		0x00000001	// 점이 사각영역의 왼쪽에 위치하고 있다.
#define CRS_RIGHT		0x00000002	// 점이 사각영역의 오른쪽에 위치하고 있다.
#define CRS_TOP			0x00000004	// 점이 사각영역의 위쪽에 위치하고 있다.
#define CRS_BOTTOM		0x00000008	// 점이 사각영역의 아랫쪽에 위치하고 있다.
#define CRS_HORIZEQ		0x00000100
#define CRS_VERTEQ		0x00000200

namespace i3Math
{
	
	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 333.461 ms
	// Vfpu unUse: 580.919 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32		sin( REAL32 theta)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_WIPI) || defined( I3_WINCE )
			#if defined( USE_TABLE)
				INT32 idx = (INT32)(theta * 41721.51340188181089983337037398f) >> 9;	// (512.0f / I3_2PI)

				return g_SinTable[ idx & 0x1FF];
			#else
				return sinf( theta);
			#endif

		#elif defined( I3_PSP)
				float d;
			//#if defined( VFPU_ASM_MATH)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,  %1\n"				// t0   = s
					"mtv			$8,  s000\n"			// s000 = t0
					"vcst.s			s001, 0x05\n"			// s001 = 2/PI
					"vmul.s			s000, s000, s001\n"		// s000 = s000 * s001
					"vsin.s			s002, s000\n"			// s000 = sin(s000)
					"mfv			$8,  s002\n"			// t0   = s000
					"mtc1			$8,  %0\n"				// d    = t0
					".set			pop\n"					// restore assember option
					: "=f"(d)
					: "f"(theta)
					: "$8"
				);
			#else
				d = sinf(theta);
			#endif
				return (d);			
		#endif
	}

	inline
	FIXED32	sin( FIXED32 theta)
	{
		#if defined( USE_TABLE)
			INT32 idx = (i3FXD::Div( theta, 51472) >> 10);	// (511.0f / (I3_2PI * 65536))

			return g_FXDSinTable[ idx & 0x1FF];
		#else
			return i3FXD::FromReal( sinf( i3FXD::ToReal( theta)));
		#endif
	}

	inline
	REAL32	sin_libm( REAL32 rad)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_PS2)
			return sinf( rad);
		#elif defined( I3_WIPI) || defined( I3_WINCE )
			#if defined( I3_WIPI_GIGA)
				return OEMC_gMU_SinR( i3FXD::FromReal( rad));
			#else
				return sinf( rad);
			#endif
		#endif
	}

	inline
	FIXED32	sin_libm( FIXED32 rad)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_PS2)
			return i3FXD::FromReal( sinf( i3FXD::ToReal( rad)));
		#elif defined( I3_WIPI) || defined( I3_WINCE )
			#if defined( I3_WIPI_GIGA)
				return OEMC_gMU_SinR( rad);
			#else
				return i3FXD::FromReal( sinf( i3FXD::ToReal( rad)));
			#endif
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 306.741 ms
	// Vfpu unUse: 639.433 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32	vsin( REAL32 t)	
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_WIPI) || defined( I3_WINCE )
			#if defined( USE_TABLE)
				INT32 idx = (INT32)( i3FXD::FromReal(t) >> 9);

				return g_SinTable[ idx & 0x1FF];
			#else
				return i3Math::sin( t * I3_PI2);
			#endif

		#elif defined( I3_PSP)
			float d;

			//#if defined( VFPU_ASM_MATH)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,  %1\n"				// t0   = s
					"mtv			$8,  s000\n"			// s000 = t0
					"vsin.s			s000, s000\n"			// s000 = sin(s000)
					"mfv			$8,  s000\n"			// t0   = s000
					"mtc1			$8,  %0\n"				// d    = t0
					".set			pop\n"					// restore assember option
					: "=f"(d)
					: "f"(t)
					: "$8"
				);
			#else
				d = i3Math::sin( t * I3_PI2);
			#endif

			return (d);		
		#endif
	}

	inline
	REAL32		vsin_libm( REAL32 t)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_PS2)
			return sinf( t * I3_PI2);
		#elif defined( I3_WIPI) || defined( I3_WINCE )
			#if defined( I3_WIPI_GIGA)
				return i3FXD::ToReal( OEMC_gMU_SinR( i3FXD::FromReal( t * I3_PI2)));
			#else
				return sinf( t * I3_PI2);
			#endif
		#endif
	}

	inline
	FIXED32		vsin( FIXED32 t)
	{
		#if defined( USE_TABLE)
			INT32 idx = t >> 9;

			return g_FXDSinTable[ idx & 0x1FF];
		#else
			return i3FXD::FromReal( vsin( i3FXD::ToReal( t)));
		#endif
	}

	inline
	FIXED32		vsin_libm( FIXED32 t)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_PS2)
			return i3FXD::FromReal( sinf( i3FXD::ToReal( i3FXD::Mul( t, 102944))));
		#elif defined( I3_WIPI) || defined( I3_WINCE )
			#if defined( I3_WIPI_GIGA)
				return OEMC_gMU_SinR( i3FXD::Mul( t, 102944));
			#else
				return i3FXD::FromReal( sinf( i3FXD::ToReal( i3FXD::Mul( t, 102944))));
			#endif
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 334.912 ms
	// Vfpu unUse: 654.291 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32		cos( REAL32 theta)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_WIPI) || defined( I3_WINCE )
			#if defined( USE_TABLE)
				INT32 idx = ((INT32)(theta * 41721.51340188181089983337037398f) >> 9) + 128;	// (512.0f / I3_2PI)

				return g_SinTable[ idx & 0x1FF];
			#else
				return cosf( theta);
			#endif

		#elif defined( I3_PSP)
				float d;
			//#if defined( VFPU_ASM_MATH)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,  %1\n"				// t0   = s
					"mtv			$8,  s000\n"			// s000 = t0
					"vcst.s			s001, 0x05\n"			// s001 = 2/PI
					"vmul.s			s000, s000, s001\n"		// s000 = s000 * s001
					"vcos.s			s000, s000\n"			// s000 = cos(s000)
					"mfv			$8,  s000\n"			// t0   = s000
					"mtc1			$8,  %0\n"				// d    = t0
					".set			pop\n"					// restore assember option
					: "=f"(d)
					: "f"(theta)
					: "$8"
				);
			#else
				d = cosf(theta);
			#endif
				return (d);
		#endif
	}

	inline
	FIXED32	cos( FIXED32 theta)
	{
		#if defined( USE_TABLE)
			INT32 idx = (i3FXD::Div( theta, 51472) >> 10) + 128;	// (511.0f / (I3_2PI * 65536))

			return g_FXDSinTable[ idx & 0x1FF];
		#else
			return i3FXD::FromReal( cosf( i3FXD::ToReal( theta)));
		#endif
	}

	inline
	REAL32	cos_libm( REAL32 rad)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_PS2)
			return cosf( rad);
		#elif defined( I3_WIPI) || defined( I3_WINCE )
			#if defined( I3_WIPI_GIGA)
				return i3FXD::ToReal( OEMC_gMU_CosR( i3FXD::FromReal( rad)));
			#else
				return cosf( rad);
			#endif
		#endif
	}

	inline
	FIXED32	cos_libm( FIXED32 rad)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_PS2)
			return i3FXD::FromReal( cosf( i3FXD::ToReal( rad)));
		#elif defined( I3_WIPI) || defined( I3_WINCE )
			#if defined( I3_WIPI_GIGA)
				return OEMC_gMU_CosR( rad);
			#else
				return i3FXD::FromReal( cosf( i3FXD::ToReal( rad)));
			#endif
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 307.594 ms
	// Vfpu unUse: 710.092 ms
	/////////////////////////////////////////////////////////////////
	// sin() 함수와 유사한 기능을 하나,
	// Radian 값이 아닌, 0~90 Degree 값을 0~1로 표현한 값으로
	// 인자를 받아들인다.
	inline
	REAL32		vcos( REAL32 t)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_WIPI)  || defined( I3_WINCE )
			#if defined( USE_TABLE)
				INT32 idx = (INT32)(t * 127.0f) + 128;

				return g_SinTable[ idx & 0x1FF];
			#else
				return cosf( t * I3_PI2);
			#endif

		#elif defined( I3_PSP)
				float d;
			//#if defined( VFPU_ASM_MATH)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,  %1\n"				// t0   = s
					"mtv			$8,  s000\n"			// s000 = t0
					"vcos.s			s000, s000\n"			// s000 = cos(s000)
					"mfv			$8,  s000\n"			// t0   = s000
					"mtc1			$8,  %0\n"				// d    = t0
					".set			pop\n"					// restore assember option
					: "=f"(d)
					: "f"(t)
					: "$8"
				);
			#else
				d = cosf( t * (I3_PI * 0.5f));
			#endif
				return (d);
		#endif
	}

	inline
	FIXED32 vcos( FIXED32 t)
	{
		#if defined( USE_TABLE)
			INT32 idx = (t >> 9) + 128;

			return g_FXDSinTable[ idx & 0x1FF];
		#else
			return i3FXD::FromReal( vcos( i3FXD::ToReal( t)));
		#endif
	}

	inline
	REAL32	vcos_libm( REAL32 t)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_PS2)
			return cosf( t * I3_PI2);
		#elif defined( I3_WIPI) || defined( I3_WINCE )
			#if defined( I3_WIPI_GIGA)
				return i3FXD::ToReal( OEMC_gMU_CosR( i3FXD::FromReal( t * I3_PI2)));
			#else
				return cosf( t * I3_PI2);
			#endif
		#endif
	}

	inline
	FIXED32	vcos_libm( FIXED32 t)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_PS2)
			return i3FXD::FromReal( cosf( i3FXD::ToReal( t) * I3_PI2));
		#elif defined( I3_WIPI) || defined( I3_WINCE )
			#if defined( I3_WIPI_GIGA)
				return OEMC_gMU_CosR( i3FXD::Mul( t, 102944));
			#else
				return i3FXD::FromReal( cosf( i3FXD::ToReal( t) * I3_PI2));
			#endif
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 416.699 ms
	// Vfpu unUse: 804.779 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32		tan( REAL32 theta)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_WIPI) || defined( I3_WINCE )
			return tanf( theta);

		#elif defined( I3_PSP)
				float d;
			#if defined( VFPU_ASM_MATH)
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,  %1\n"				// t0   = s
					"mtv			$8,  s000\n"			// s000 = t0
					"vcst.s			s001, 0x05\n"			// s001 = 2/PI
					"vmul.s			s000, s000, s001\n"		// s000 = s000 * s001
					"vrot.p			c002, s000, [c, s]\n"	// s002 = cos(s) , s003 = sin(s)
					"vdiv.s			s000, s003, s002\n"		// s000 = s003 / s002 = tan(s)
					"mfv			$8,  s000\n"			// t0   = s000
					"mtc1			$8,  %0\n"				// d    = t0
					".set			pop\n"					// restore assember option
					: "=f"(d)
					: "f"(theta)
					: "$8"
				);
			#else
				d = tanf(theta);
			#endif
				return (d);	
		#endif
	}
	/////////////////////////////////////////////////////////////////
	// 2005.6.20 by KOMET (Math에 없어서 추가함) 
	/////////////////////////////////////////////////////////////////
	inline
	REAL32		atan2( REAL32 x, REAL32 y)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_WIPI) || defined( I3_WINCE )
			return atan2f( x, y);

		#elif defined( I3_PSP)
			return atan2f( x, y);

		#endif
	}
	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 348.903 ms
	// Vfpu unUse: 1081.037 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32		acos( REAL32 radian)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_WIPI) || defined( I3_WINCE )
			return acosf( radian);

		#elif defined( I3_PSP)
				float d;
			//#if defined( VFPU_ASM_MATH)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,  %1\n"				// t0   = s
					"mtv			$8,  s000\n"			// s000 = t0
					"vasin.s		s000, s000\n"			// s000 = asin(s000)
					"vcst.s			s001, 0x08\n"			// s001 = PI/2
					"vocp.s			s000, s000\n"			// s000 = 1 - s000 = 1 - asin(s) = acos
					"vmul.s			s000, s000, s001\n"		// s000 = s000 * s001
					"mfv			$8,  s000\n"			// t0   = s000
					"mtc1			$8,  %0\n"				// d    = t0
					".set			pop\n"					// restore assember option
					: "=f"(d)
					: "f"(radian)
					: "$8"
				);
			#else
				d = acosf(radian);
			#endif
				return (d);
		#endif
	}

	inline
	FIXED32		acos( FIXED32 radian)
	{
		return i3FXD::FromReal( acosf( i3FXD::ToReal(radian)));
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 305.899 ms
	// Vfpu unUse: 1862.623 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32		Sqrt( REAL32 val)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_WIPI) || defined( I3_WINCE )
			return sqrtf( val);

		#elif defined( I3_PSP)
				float d;
			//#if defined( VFPU_ASM_MATH)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,  %1\n"				// t0   = s
					"mtv			$8,  s000\n"			// s000 = t0
					"vsqrt.s		s000, s000\n"			// s000 = sqrt(s000)
					"mfv			$8,  s000\n"			// t0   = s000
					"mtc1			$8,  %0\n"				// d    = t0
					".set			pop\n"					// restore assember option
					: "=f"(d)
					: "f"(val)
					: "$8"
				);
			#else
				d = sqrtf(val);
			#endif
				return (d);
		#endif
	}

	inline
	FIXED32		Sqrt( FIXED32 val)
	{
		return i3FXD::FromReal( sqrtf( i3FXD::ToReal( val)));
	}

	I3_EXPORT_MATH BOOL BoundBoxContainBoundBox( VEC3D* pMin1, VEC3D* pMax1, VEC3D* pMin2, VEC3D* pMax2);

	I3_EXPORT_MATH 
	BOOL GetRotFromPos( REAL32 *pTheta, REAL32 *pRho, VEC3D* pPos );

	/// 직선(p1, p2)와 점(p2)가 직각을 이루는 점접을 반환한다.
	///
	/// Return
	///     직선과 점이 직각을 이루는 교점이 직선 내(t:0~1)에 존재한다면 True,
	///		그렇지 않다면 False를 반환한다.
	///
	I3_EXPORT_MATH 
	BOOL GetPerpendicularPoint( VEC3D* pL1, VEC3D* pL2,VEC3D* p0, VEC3D* pPerpendicular, REAL32* t);

	I3_EXPORT_MATH 
	BOOL	TestTriPoint( VEC3D *v1, VEC3D *v2, VEC3D *v3, VEC3D *pos, VEC3D *dir);

	I3_EXPORT_MATH
	bool	IsPtInTri( VEC2D * v1, VEC2D *v2, VEC2D *v3, VEC2D *pos);

	I3_EXPORT_MATH 
	BOOL	TestTriPointBothSide( VEC3D *v1, VEC3D *v2, VEC3D *v3, VEC3D *pos, VEC3D *dir);

	I3_EXPORT_MATH 
	BOOL	TestSphereWithTri( VEC3D * v1, VEC3D * v2, VEC3D * v3, VEC3D * pNormal, VEC3D * pCenter, REAL32 r);

	// Line Functions
	I3_EXPORT_MATH 
	BOOL	IntersectLineToLine();

	I3_EXPORT_MATH 
	BOOL	IntersectLineToSphere( VEC3D* pL1,VEC3D* pL2,VEC3D* pCenter,REAL32 sqr_radius,VEC3D *pIntersect,REAL32* t);

	I3_EXPORT_MATH 
	BOOL	IntersectLineToCylinder();

	I3_EXPORT_MATH 
	BOOL	IntersectLineToTri();	
	
	// Sphere Functions
	I3_EXPORT_MATH 
	BOOL	IntersectSphereToSphere( VEC3D * pC1, REAL32 Radius1, VEC3D * pC2, REAL32 Radius2, VEC3D * pIntersect, VEC3D * pNorm, REAL32 * t);

	I3_EXPORT_MATH 
	BOOL	IntersectSphereToCylinder();

	I3_EXPORT_MATH 
	BOOL	IntersectSphereToTri();

	// Cylinder Functions
	I3_EXPORT_MATH 
	BOOL	IntersectCylinderToCylinder();

	I3_EXPORT_MATH 
	BOOL	IntersectCylinderToTri();

	// Capsule Functions added by 정순구 2006. 3. 18, (revision by 백성민 2006.7.19)
	// pC1 : 캡슐의 시작중심점	pC2 : 캡슐의 끝중심점
	// pL1 : 라인의 시작점		pL2 : 라인의 끝점
	I3_EXPORT_MATH
	BOOL	IntersectCapsuleToLine( VEC3D * pCStart, VEC3D * pCDir, REAL32 fCLength, REAL32 CRadius, VEC3D * pL1, VEC3D * pL2,
									VEC3D * pIntersect = NULL, VEC3D * pNorm = NULL, REAL32 * t = NULL);

	I3_EXPORT_MATH
	BOOL	IntersectCapsuleToSphere( VEC3D * pCStart, VEC3D * pCDir, REAL32 fCLength, REAL32 CRadius,
									  VEC3D * pSCenter,REAL32 SRadius, VEC3D * pIntersect = NULL, VEC3D * pNormal = NULL, REAL32 * t = NULL );

	I3_EXPORT_MATH
	BOOL	IntersectCapsuleToCylinder( void);

	I3_EXPORT_MATH
	BOOL	IntersectCapsuleToCapsule( VEC3D * pCStart1, VEC3D * pCDir1, REAL32 fCLength1, REAL32 CRadius1, 
									   VEC3D * pCStart2, VEC3D * pCDir2, REAL32 fCLength2, REAL32 CRadius2,
									   VEC3D * pIntersect1 = NULL, VEC3D * pIntersect2 = NULL, VEC3D * pNorm = NULL, REAL32 * t1 = NULL, REAL32 * t2 = NULL);

	// Ellipsoid Functions	(2006. 7. 19 added by 백성민)
	I3_EXPORT_MATH
	BOOL	IntersectLineToEllipsoid( VEC3D * p1, VEC3D * p2, VEC3D * pEllipCenter, MATRIX * pEllipA);

	// Triangle Functions
	I3_EXPORT_MATH 
	BOOL	IntersectTriToTri();


	///////////////////////////////////////////////////////
	I3_EXPORT_MATH
	bool		PtInRect( I3RECT * pRect, VEC2D * pPt);

	I3_EXPORT_MATH
	void NormalizeRect( I3RECT * pRect);

	I3_EXPORT_MATH 
	TOUCH_TYPE RectInRect( I3RECT * dr, I3RECT * sr);

	I3_EXPORT_MATH 
	int GetPointOnRect( REAL32 x1, REAL32 y1, I3RECT * rc);

	//	Add new 백성민 2006.7.19
	I3_EXPORT_MATH		//	두 직선 사이의 최단 거리를 반환한다.
	REAL32	GetSqrDistanceLineToLine( VEC3D * pOrig1, VEC3D * pDir1, VEC3D * pOrig2, VEC3D * pDir2, 
									REAL32 * f1 = NULL, REAL32 * f2 = NULL);

	I3_EXPORT_MATH
	REAL32	GetDistanceLineToLine( VEC3D * pOrig1, VEC3D * pDir1, VEC3D * pOrig2, VEC3D * pDir2, 
									REAL32 * f1 = NULL, REAL32 * f2 = NULL);


	I3_EXPORT_MATH 
	bool IsTouchLineRect( VEC2D *Pt1, VEC2D *Pt2, I3RECT * pRect);

	///////////////////////////////////////////////////////////////////////////////////////////////
	// 사각영역과 직선의 교점을 구한다.
	// 
	// Pt1 : 직선의 시작점
	// Pt2 : 직선의 끝점
	// Rect : 사각 영역
	I3_EXPORT_MATH 
	bool GetIntersectLineFromRect( VEC2D *Pt1, VEC2D *Pt2, I3RECT * pRect);

	I3_EXPORT_MATH
	void CalcTangentBasis( VEC3D * pPos1, VEC3D * pPos2, VEC3D * pPos3, VEC2D * pUV1, VEC2D * pUV2, VEC2D * pUV3,
							VEC3D * pTangent, VEC3D * pBinorm);

	I3_EXPORT_MATH
	bool IsSharedEdge( VEC3D * pSrc0, VEC3D * pSrc1, VEC3D * pDest0, VEC3D * pDest1);

	I3_EXPORT_MATH
	bool CheckSharedEdge( VEC3D	pSrcTri[3], VEC3D pDestTri[3]);

	I3_EXPORT_MATH
	REAL32	ComputeGaussian( REAL32 deviation, REAL32 n);

	I3_EXPORT_MATH
	void	SetupGaussianBlurParameters( REAL32 fX, REAL32 fY, INT32 nTapCount, REAL32 dev, VEC4D * pDest);
};

// 평면( p0를 통과하고 방향이 n인)과 두점(p1,p2)와의 교점
I3_EXPORT_MATH BOOL IntersectTriLine( VEC3D* p1, VEC3D* p2, VEC3D* n, VEC3D* p0, VEC3D* pc,REAL32* t);

// 삼각형( tri[3])과 두점(p1,p2)와의 포함여부
I3_EXPORT_MATH BOOL ContainTriPoint( VEC3D tri[3], VEC3D *pos, VEC3D *dir);
I3_EXPORT_MATH BOOL ContainTriPoint( VEC3D* tri[3], VEC3D *pos, VEC3D *dir);
I3_EXPORT_MATH BOOL ContainTriPoint( VEC3D* v1, VEC3D *v2, VEC3D *v3, VEC3D *pos, VEC3D *dir);

// 두 삼각형( p0[3],n0 / p[3])이 교차 하는지 여부
I3_EXPORT_MATH BOOL IntersectTriTri(VEC3D p0[3],VEC3D* n0,VEC3D p[3],VEC3D* pc);

// 평면( p0를 통과하고 방향이 n0인 )과 한점(p)의 가장 가까운 위치(x) 구하기
I3_EXPORT_MATH REAL32 NearestPlanePoint(VEC3D* p0,VEC3D* n0,VEC3D* p,VEC3D* pc);

// 직선(p1-p2)과 직선(q1-q2)의 가장 가까운 거리
I3_EXPORT_MATH BOOL NearestLineLine(VEC3D* p1,VEC3D* p2,VEC3D* q1,VEC3D* q2,REAL32* t1,REAL32* t2,REAL32* dist);


// 직선(p1-p2)과 BoundBox( min - max )의 교차점 구하기
I3_EXPORT_MATH BOOL IntersectAABBoxLine(VEC3D* p1,VEC3D* p2,VEC3D* pMin,VEC3D* pMax,VEC3D* pc,REAL32* t);

// 직선(p1-p2)과 BoundBox( min - max )의 포함 여부
I3_EXPORT_MATH BOOL ContainAABBoxLine(VEC3D* p1,VEC3D* p2,VEC3D* pMin,VEC3D* pMax,REAL32 last_t=1.f);

// BoundBox( min - max ) 간의 포함 여부
I3_EXPORT_MATH BOOL ContainAABoxAABox(VEC3D* pMin1,VEC3D* pMax1,VEC3D* pMin2,VEC3D* pMax2);

// 점(p)과 BoundBox( min - max )의 포함 여부
I3_EXPORT_MATH BOOL ContainAABBoxPoint(VEC3D* p,VEC3D* pMin,VEC3D* pMax);

// Sphere와 BoundBox( min - max )의 포함 여부
I3_EXPORT_MATH BOOL ContainAABBoxSphere(VEC3D* p,REAL32 r,VEC3D* pMin,VEC3D* pMax);

// u,v가 각각의 min,max에 포함 여부를 검사
I3_EXPORT_MATH BOOL ContainAABBoxPoint2D(REAL32 u, REAL32 min_u, REAL32 max_u, REAL32 v, REAL32 min_v, REAL32 max_v);


#endif
