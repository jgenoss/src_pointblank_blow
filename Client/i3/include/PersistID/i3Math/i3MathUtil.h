#if !defined( __I3_MATH_UTIL_H)
#define __I3_MATH_UTIL_H

#include "i3MathType.h"
#include "i3MathDefine.h"

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
	inline
	REAL32 relativeEPS( REAL32 a, REAL32 epsilon) 
	{
		return max( i3Math::abs(a * epsilon), epsilon);
	}

	inline
	bool isAlike( REAL32 a, REAL32 b, REAL32 epsilon = I3_EPS) 
	{
		if(a == b) 
			return true;

		REAL32 relEps = relativeEPS( a, epsilon);
		return (a-relEps <= b) && (b <= a+relEps);
	}

	inline
	bool isValid( REAL32 val)
	{
		bool valid = (0 == ((_FPCLASS_SNAN | _FPCLASS_QNAN | _FPCLASS_NINF | _FPCLASS_PINF) & _fpclass(val) ));

		valid = valid && ( *((UINT32 *) &val) != 0xCDCDCDCD);

		return valid;
	}

	// 입력된 값을 -PI ~ +PI 사의의 값으로 바꾸어 준다.
	inline
	REAL32 NormalizeAngle( REAL32 rRadian )
	{
		if( rRadian <= -I3_PI )
		{
			while( rRadian <= -I3_PI )
			{
				rRadian += I3_2PI;
			}
		}
		else if( rRadian > I3_PI )
		{
			while( rRadian > I3_PI )
			{
				rRadian -= I3_2PI;
			}
		}

		return rRadian;
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
