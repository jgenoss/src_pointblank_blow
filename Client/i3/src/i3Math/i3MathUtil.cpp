// ISAAC.cpp
#include "i3MathType.h"
#include "i3MathUtil.h"
#include "i3Base.h"
#include "i3Vector.h"

enum Dimension
{
    XDIM = 0,
    YDIM,
    ZDIM
};

I3_EXPORT_MATH
INT32	i3Math::GetNearestPowerOf2( INT32 val)
{
	INT32 temp = 1;

	while( temp < val)
		temp = temp << 1;

	return temp;
}

bool i3Math::BoundBoxContainBoundBox( VEC3D* pMin1, VEC3D* pMax1, VEC3D* pMin2, VEC3D* pMax2)
{
	//REAL32 tag;
	register INT32 itag; 
	INT32 *		pI;
	VEC3D Vec1;

	// 아래의 코드는 VEC3D의 각 X, Y, Z
	i3Vector::Sub( &Vec1, pMax1, pMin2);

	pI = (INT32 *) & Vec1;

	itag = *pI;		pI++;
	itag |= *pI;	pI++;
	itag |= *pI;

	i3Vector::Sub( &Vec1, pMax2, pMin1);

	pI = (INT32 *) & Vec1;

	itag |= *pI;	pI++;
	itag |= *pI;	pI++;
	itag |= *pI;

	return (itag & 0x80000000) == 0;

}

bool i3Math::GetRotFromPos( REAL32 *pTheta, REAL32 *pRho, VEC3D* pPos )
{
	//       target
	//         ★        Y        Z                     
	//          |＼      |       /                      
	//          |  ＼    |      /                       Y
	//          |    ＼  |     /            target      |
	//          +------＼|----+               ★--------+ 
	//         / ↖      |   /                 |＼      |
	//        /    ＼len |  /                  |  ＼    |
	//       /       ＼  | /                   |    ＼  |
	//      /    theta(＼|/                    | rho (＼|
	// X --+-------------+               len --+--------+      
	//
	//  theta = atan(x, z)
	//	len   = sqrt(x*x + z*z)
	//  rho   = atan(len, y)

	// first of all, retrieve interal theta value
	*pTheta = atan2( getX(pPos), getZ(pPos) );

	// calcurate a rho value
	REAL32 fLen;
	fLen = Sqrt( (getX(pPos) * getX(pPos)) + (getZ(pPos) * getZ(pPos)) );
	*pRho = atan2(getY(pPos), fLen);
	return true;
}


// (-) 1
// (+) 0

//#define REAL_CHECK_SIGN(r)		(*((INT32 *)&r) & 0x80000000)
#define REAL_CHECK_SIGN(r)		(r < 0.f)

bool i3Math::GetPerpendicularPoint(VEC3D* pL1,VEC3D* pL2,VEC3D* p0,VEC3D* pPerpendicular,REAL32 *t)
{
	REAL32 num, den;
	VEC3D p21,n,p01;

	i3Vector::Sub( &p21, pL2, pL1 );
	i3Vector::Sub( &p01, p0, pL1 );
	
	i3Vector::Normalize( &n, &p21);

	num = i3Vector::Dot( &n, &p01 );
	den = i3Vector::Dot( &n, &p21);

	if ( den*den < 1E-15f * num * num ) return false;
	*t = num / den;
	
	*pPerpendicular = p21;
	i3Vector::Scale( pPerpendicular, pPerpendicular, *t);
	i3Vector::Add( pPerpendicular, pL1, pPerpendicular);
	
	if ( *t > 0.0f && *t < 1.0f ) return true;
	
	return false;
}

bool i3Math::TestTriPoint( VEC3D *v1, VEC3D *v2, VEC3D *v3, VEC3D *pos, VEC3D *dir)
{
    VEC3D tempVec, normal;
	
	{
        i3Vector::Sub( &tempVec, v2, v1);
		i3Vector::Cross( &normal, dir, &tempVec);
        i3Vector::Sub( &tempVec, pos, v1);

        if( i3Vector::Dot( &tempVec, &normal ) < 0.0f )
        {
            return false;
        }
    }

	{
        i3Vector::Sub( &tempVec, v3, v2);
		i3Vector::Cross( &normal, dir, &tempVec);
        i3Vector::Sub( &tempVec, pos, v2);

        if( i3Vector::Dot( &tempVec, &normal ) < 0.0f )
        {
            return false;
        }
    }
	{
        i3Vector::Sub( &tempVec, v1, v3);
		i3Vector::Cross( &normal, dir, &tempVec);
        i3Vector::Sub( &tempVec, pos, v3);

        if( i3Vector::Dot( &tempVec, &normal ) < 0.0f )
        {
            return false;
        }
    }

    return true;
}

bool i3Math::IsPtInTri( VEC2D *v1, VEC2D *v2, VEC2D *v3, VEC2D *pos)
{
    REAL32 temp1, temp2;
	
	{
		temp1 = (pos->x - v1->x) * (v2->y - v1->y);
		temp2 = (pos->y - v1->y) * (v2->x - v1->x);

        if( temp1 > temp2)
            return false;
    }

	{
		temp1 = (pos->x - v2->x) * (v3->y - v2->y);
		temp2 = (pos->y - v2->y) * (v3->x - v2->x);

        if( temp1 > temp2)
            return false;
    }

	{
		temp1 = (pos->x - v3->x) * (v1->y - v3->y);
		temp2 = (pos->y - v3->y) * (v1->x - v3->x);

        if( temp1 > temp2)
            return false;
    }

    return true;
}

bool i3Math::TestTriPointBothSide( VEC3D *v1, VEC3D *v2, VEC3D *v3, VEC3D *pos, VEC3D *dir)
{
	VEC3D tempVec, normal;

	{
		i3Vector::Sub( &tempVec, v2, v1);
		i3Vector::Cross( &normal, dir, &tempVec);
		i3Vector::Sub( &tempVec, pos, v1);

		if( i3Vector::Dot( &tempVec, &normal ) < 0.0f )
		{
			goto NextSide;
		}
	}

	{
		i3Vector::Sub( &tempVec, v3, v2);
		i3Vector::Cross( &normal, dir, &tempVec);
		i3Vector::Sub( &tempVec, pos, v2);

		if( i3Vector::Dot( &tempVec, &normal ) < 0.0f )
		{
			goto NextSide;
		}
	}
	{
		i3Vector::Sub( &tempVec, v1, v3);
		i3Vector::Cross( &normal, dir, &tempVec);
		i3Vector::Sub( &tempVec, pos, v3);

		if( i3Vector::Dot( &tempVec, &normal ) < 0.0f )
		{
			goto NextSide;
		}
	}

	return true;

NextSide:

	{
		i3Vector::Sub( &tempVec, v1, v2);
		i3Vector::Cross( &normal, dir, &tempVec);
		i3Vector::Sub( &tempVec, pos, v2);

		if( i3Vector::Dot( &tempVec, &normal ) < 0.0f )
		{
			return false;
		}
	}

	{
		i3Vector::Sub( &tempVec, v2, v3);
		i3Vector::Cross( &normal, dir, &tempVec);
		i3Vector::Sub( &tempVec, pos, v3);

		if( i3Vector::Dot( &tempVec, &normal ) < 0.0f )
		{
			return false;
		}
	}
	{
		i3Vector::Sub( &tempVec, v3, v1);
		i3Vector::Cross( &normal, dir, &tempVec);
		i3Vector::Sub( &tempVec, pos, v1);

		if( i3Vector::Dot( &tempVec, &normal ) < 0.0f )
		{
			return false;
		}
	}

	return true;
}

bool i3Math::TestSphereWithTri( VEC3D * v1, VEC3D * v2, VEC3D * v3, VEC3D * pNormal, VEC3D * pCenter, REAL32 r)
{
	VEC3D temp;
	REAL32 dot;

	// 삼각형의 평면과 Sphere 중앙점의 거리를 구한다.
	i3Vector::Sub( &temp, pCenter, v1);
	dot = i3Vector::Dot( &temp, pNormal);

	if( i3Math::abs( dot) < r )
		return false;

	return true;
}

//	두 직선의 최단거리를 구할때 기준이 되는 직선을 pOrig2와 pDir2 인자로 넣어야 제대로된 Distance가 나온다 (유의!)
REAL32 i3Math::GetSqrDistanceLineToLine( VEC3D * pOrig1, VEC3D * pDir1, VEC3D * pOrig2, VEC3D * pDir2, REAL32 * f1, REAL32 * f2)
{//	3D Game Engine Design (David H. Eberly) : 2.6.2 LINEAR COMPONENT TO LINEAR COMPONENT (p.42) 참조.
	const REAL32 gs_fTolerance = 1e-05f;

	/*	
	//	끝점이 없는 무한한 반직선일 경우
	VEC3D fDiff;
	i3Vector::Sub( &fDiff, pOrig1, pOrig2);

	REAL32 fA00 = i3Vector::LengthSq( pDir1);
	REAL32 fA01 = - (i3Vector::Dot( pDir1, pDir2));
	REAL32 fA11 = i3Vector::LengthSq( pDir2);
	REAL32 fB0 = i3Vector::Dot( &fDiff, pDir1);
	REAL32 fC = i3Vector::LengthSq( &fDiff);
	REAL32 fDet = i3Math::abs( fA00 * fA11 - fA01 * fA01);
	REAL32 fB1, fS, fT, fSqrDist;
	
	// 두 직선이 평행하지 않다.
	if( fDet >= gs_fTolerance)
	{
		REAL32 fInvDet = 1.0f / fDet;

		fB1 = -( i3Vector::Dot( &fDiff, pDir2));
		fS = ( fA01 * fB1 - fA11 * fB0) * fInvDet;
		fT = ( fA01 * fB0 - fA00 * fB1) * fInvDet;

        fSqrDist = fS * (fA00 * fS + fA01 * fT + 2.0f * fB0) + fT * ( fA01 * fS + fA11 * fT + 2.0f * fB1) + fC;
	}
	//	두 직선이 평행하다.
	else
	{
		fS = -fB0 / fA00;
		fT = 0.0f;
		fSqrDist = fB0 * fS + fC;
	}*/

	//	시작과 끝이 있는 직선일 경우
	VEC3D fDiff;
	i3Vector::Sub( &fDiff, pOrig1, pOrig2);

	REAL32 fA00 = i3Vector::LengthSq( pDir1);
	REAL32 fA01 = - (i3Vector::Dot( pDir1, pDir2));
	REAL32 fA11 = i3Vector::LengthSq( pDir2);
	REAL32 fB0 = i3Vector::Dot( &fDiff, pDir1);
	REAL32 fC = i3Vector::LengthSq( &fDiff);
	REAL32 fDet = i3Math::abs( fA00 * fA11 - fA01 * fA01);
	REAL32 fB1, fS, fT, fSqrDist;
	
	// 두 직선이 평행하지 않다.
	if( fDet >= gs_fTolerance)
	{
		fB1 = -( i3Vector::Dot( &fDiff, pDir2));
		fT = fA01 * fB0 - fA00 * fB1;

		if( fT >= 0.f)
		{
			if( fT <= fDet)
			{
				// two interior points are closest, one on line and one on
				REAL32 fInvDet = 1.0f / fDet;
				fS = ( fA01 * fB1 - fA11 * fB0) * fInvDet;
				fT *= fInvDet;
				
				 fSqrDist = fS * (fA00 * fS + fA01 * fT + 2.0f * fB0) + fT * ( fA01 * fS + fA11 * fT + 2.0f * fB1) + fC;
			}
			else
			{
				// end point e1 of segment and interior point of line are
				REAL32 fTmp = fA01 + fB0;
				fS = -fTmp / fA00;
				fT = 1.0f;
				
				fSqrDist = fTmp * fS + fA11 + 2.0f * fB1 + fC;
			}
		}
		else
		{
			// end point e0 of segment and interior point of line are closest
			fS = -fB0 / fA00;
			fT = 0.0f;

			fSqrDist = fB0 * fS + fC;
		}
	}
	//	두 직선이 평행하다.
	else
	{
		if( -I3_EPS < fA00 && fA00 < I3_EPS)
			fS = 0.f;
		else
			fS = -fB0 / fA00;

		fT = 0.0f;
		fSqrDist = fB0 * fS + fC;
	}

	if( f1 != nullptr)		*f1 = fS;
	if( f2 != nullptr)		*f2 = fT;

	return i3Math::abs( fSqrDist);
}

REAL32 i3Math::GetDistanceLineToLine( VEC3D * pOrig1, VEC3D * pDir1, VEC3D * pOrig2, VEC3D * pDir2, REAL32 * f1, REAL32 * f2)
{
	REAL32 fSqrDist = GetSqrDistanceLineToLine( pOrig1, pDir1, pOrig2, pDir2, f1, f2);
	return i3Math::Sqrt( fSqrDist);
}


bool i3Math::IntersectLineToLine( void)
{
	return false;
}

bool i3Math::IntersectLineToSphere( VEC3D* pL1,VEC3D* pL2,VEC3D* pCenter,REAL32 sqr_radius,VEC3D *pIntersect,REAL32* t)
{
	VEC3D len;

	if( GetPerpendicularPoint( pL1, pL2, pCenter, pIntersect, t) )
	{
		i3Vector::Sub( &len, pIntersect, pCenter);

		if( i3Vector::LengthSq( &len) < sqr_radius )
			return true;// 당첨 : 선분과의 거리가 radius 보다 작다

		return false;// 꽝 : 선분과의 거리가 radius 보다 크다
	}

	i3Vector::Sub( &len, pL2, pCenter);
	if( i3Vector::LengthSq( &len) < sqr_radius )
	{
		*pIntersect = *pL2;
		*t = 1.f;
		return true;// 당첨 : pL1와의 거리가 radius 보다 작다
	}

	i3Vector::Sub( &len, pL1, pCenter);
	if( i3Vector::LengthSq( &len) < sqr_radius )
	{
		*pIntersect = *pL1;
		*t = 0.f;
		return true;// 당첨 : pCenter와의 거리가 radius 보다 작다
	}

	return false;// 꽝 : 아무 상관 없다...
}

bool i3Math::IntersectSphereToSphere( VEC3D * pC1, REAL32 Radius1, VEC3D * pC2, REAL32 Radius2, VEC3D * pIntersect, VEC3D * pNorm, REAL32 * t)
{
	VEC3D diff;
	REAL32 len;

	i3Vector::Sub( &diff, pC2, pC1);

	len = i3Vector::Length( &diff);

	if( len <= (Radius1 + Radius2))
	{
		i3Vector::Normalize( &diff, &diff);

		if( pNorm != nullptr)
			i3Vector::Copy( pNorm, &diff);

		len = (Radius1 + Radius2) - len;

		if( t != nullptr)
			*t = Radius1 - len;

		if( pIntersect != nullptr)
			i3Vector::Scale( pIntersect, &diff, Radius1 - len);

		return true;
	}

	return false;
}

// 2006. 3. 18 추가 by 정순구
// 2006. 7. 19 revision by 백성민
bool i3Math::IntersectCapsuleToLine( VEC3D * pCStart, VEC3D * pCDir, REAL32 fCLength, REAL32 CRadius, VEC3D * pL1, VEC3D * pL2,
									VEC3D * pIntersect, VEC3D * pNorm, REAL32 * t)
{
	REAL32 fSqrDist = 0.f;
	REAL32	fS = 0.f, fT = 0.f;
	VEC3D	vLineDir;
	VEC3D	vCapDir, vCapEnd;
	VEC3D	vIntersect, vNormal;
	
	i3Vector::Sub( &vLineDir, pL2, pL1 );			//	길이가 포함된 라인의 방향	
	i3Vector::Scale( &vCapDir, pCDir, fCLength);	//	길이가 포함된 캡슐의 방향
	i3Vector::Add( &vCapEnd, pCStart, &vCapDir);	//	캡슐의 End

	i3Vector::Set( &vNormal, 0.0f, 1.0f, 0.0f);

	REAL32 fSqrRadius = CRadius * CRadius;

	REAL32 rLineLength = i3Math::abs( i3Vector::Length( &vLineDir));
	REAL32 rCapLength = i3Math::abs( i3Vector::Length( &vCapDir));

	//	캡슐과 라인의 최단 거리를 구한다.
	if( rCapLength < rLineLength)		//	라인이 캡슐보다 더 길다면 첫번때 인자들은 라인으로 두번째 인자들은 캡슐로
	{
		fSqrDist = GetSqrDistanceLineToLine( pL1, &vLineDir, pCStart, &vCapDir, &fT, &fS);
	}
	else								//	캡슐이 라인보다 더 길다면 첫번째 인자들은 캡슐로 두번째 인자들은 라인으로
	{
		fSqrDist = GetSqrDistanceLineToLine( pCStart, &vCapDir, pL1, &vLineDir, &fS, &fT);
	}
		
	//if( (0.f < fS && fS < 1.0f) && (0.f < fT && fT < 1.0f))	
	if( ( -I3_EPS < fS && fS < 1.f+I3_EPS) && (-I3_EPS < fT && fT < 1.f+I3_EPS))
	{
		if( fSqrDist <= fSqrRadius) 
		{	
			//i3Vector::Lerp( &vIntersect, pCStart, &vCapEnd, fS);	//	캡슐의 교점
			i3Vector::Lerp( &vIntersect, pL1, pL2, fT);		//	라인의 교점

			goto LABEL_TRUE;
		}
	}
	//	캡슐의 안쪽에 라인을 포함할 경우
	else if( -I3_EPS < fS && fS < 1.f+I3_EPS)		
	{
		REAL32 fDist = 0.f; 
		VEC3D vPoint, vDiff;

		//	임시로 함수호출. 2번 연산하는 결과가 되므로 속도에 문제될수 있지만 GetSqrDistanceLineToLine 함수의 인자 순서에 따라
		//	라인의 기준이 달라져서 반환값들이 달라지는 것으로 생각됩니다. 반환값으로 보면 두번째 인자의 라인을 기준으로 계산하기 때문에
		//  이렇게 하지 않으면 위에서 GetSqrDistanceLineToLine 연산은 캡슐이 기준으로 true가 되어야 할 특정조건에선 라인과의 교점이 true가 될수 없습니다.
		//	속도가 문제가 된다면 아래 GetSqrDistanceLineToLine를 두번 호출해야 하는 결과를 개선해야 합니다.
		//  2006.8.2 by komet

		//	현재 상황에선 반환 값이 달라야 하기(기준을 바꿔야 하므로) 때문에 인자 순서를 바꿔서 다시 호출합니다. 
		//GetSqrDistanceLineToLine( pCStart, &vCapDir, pL1, &vLineDir, &fS, &fT);		

		i3Vector::Lerp( &vPoint, pCStart, &vCapEnd, fS);  

		if( -I3_EPS >= fT || fT == 0.f)			//	라인의 Start를 교점으로
		{
			i3Vector::Sub( &vDiff, &vPoint, pL1);
			i3Vector::Copy( &vIntersect, pL1);

			fT = 0.f;
		}
		else if( fT >= 1.f+I3_EPS || fT == 1.f)	//	라인의 End를 교점으로
		{
			i3Vector::Sub( &vDiff, &vPoint, pL2 );
			i3Vector::Copy( &vIntersect, pL2);

			fT = 1.f;
		}	
		else
		{
			I3PRINTLOG(I3LOG_WARN,  "Invalid Result" );
			return false;
		}
		
		fDist = i3Vector::Length( &vDiff);
		fDist = i3Math::abs( fDist);

		if( fDist <= CRadius)		//	라인과의 거리가 캡슐의 반지름에 포함되면
		{
			goto LABEL_TRUE;
		}
	}
	//	라인의 안쪽에 캡슐을 포함할 경우
	else if(-I3_EPS < fT && fT < 1.f+I3_EPS)		
	{
		REAL32 fDist = 0.f; 
		VEC3D vPoint, vDiff;

		i3Vector::Lerp( &vPoint, pL1, pL2, fT);

		if( -I3_EPS >= fS || fS == 0.f)			//	캡슐의 Start를 교점으로
		{
			i3Vector::Sub( &vDiff, pCStart, &vPoint);
			//i3Vector::Copy( &vIntersect, pCStart);

			fS = 0.f;
		}
		else if( fS >= 1.f+I3_EPS || fS == 1.f)	//	캡슐의 End를 교점으로
		{
			i3Vector::Sub( &vDiff, &vCapEnd, &vPoint);
			//i3Vector::Copy( &vIntersect, &vCapEnd);

			fS = 1.f;
		}	
		else
		{
			//I3PRINTLOG(I3LOG_WARN,  "Invalid Result\n" );
			return false;
		}
		
		fDist = i3Vector::Length( &vDiff);
		fDist = i3Math::abs( fDist);

		if( fDist <= CRadius)		//	라인과의 거리가 캡슐의 반지름에 포함되면
		{
			i3Vector::Lerp( &vIntersect, pL1, pL2, fT);				//	라인의 교점
	
			goto LABEL_TRUE;
		}
	}

	return false;

LABEL_TRUE:
	if( t != nullptr)		*t = fT;
	if( pIntersect != nullptr)		i3Vector::Copy( pIntersect, &vIntersect);
	if( pNorm != nullptr)		
	{
		VEC3D vCapPoint;
		i3Vector::Lerp( &vCapPoint, pCStart, &vCapEnd, fS);  
		i3Vector::Sub( &vNormal, &vIntersect, &vCapPoint);
		i3Vector::Normalize( &vNormal, &vNormal);
		i3Vector::Copy( pNorm, &vNormal);
	}

	return true;
}

// 2006. 8.1 add by 백성민
bool i3Math::IntersectCapsuleToCapsule( VEC3D * pCStart1, VEC3D * pCDir1, REAL32 fCLength1, REAL32 CRadius1, 
									   VEC3D * pCStart2, VEC3D * pCDir2, REAL32 fCLength2, REAL32 CRadius2,
									   VEC3D * pIntersect1, VEC3D * pIntersect2, VEC3D * pNorm, REAL32 * t1, REAL32 * t2)
{	
	VEC3D	CStart[2], CDir[2];
	REAL32	CLength[2];

	//	GetSqrDistanceLineToLine함수에서 2번째 라인의 인자들은 1번째 라인 인자보다 짧아야 한다.
	//	2번째 라인인자를 짧은 쪽으로 기준을 해야 결과거리가 정확하게 나오기 때문에 양인자를 교체한다.
	if( fCLength1 < fCLength2)		//	2번째 캡슐이 1번째 캡슐보다 길어서 교체
	{
		i3Vector::Copy( &CStart[0], pCStart2);
		i3Vector::Copy( &CStart[1], pCStart1);
		i3Vector::Copy( &CDir[0], pCDir2);
		i3Vector::Copy( &CDir[1], pCDir1);
		CLength[0] = fCLength2;
		CLength[1] = fCLength1;		
	}
	else							//	2번째 캡슐이 1번째 캡슐보다 짧다.
	{
		i3Vector::Copy( &CStart[0], pCStart1);
		i3Vector::Copy( &CStart[1], pCStart2);
		i3Vector::Copy( &CDir[0], pCDir1);
		i3Vector::Copy( &CDir[1], pCDir2);
		CLength[0] = fCLength1;
		CLength[1] = fCLength2;		
	}

	REAL32	fS = 0.f, fT = 0.f;
	VEC3D	vCapDir[2], vCapEnd[2];
	VEC3D	vIntersect[2];

	i3Vector::Scale( &vCapDir[0], &CDir[0], CLength[0]);		//	길이가 포함된 캡슐1의 방향
	i3Vector::Add( &vCapEnd[0], &CStart[0], &vCapDir[0]);		//	캡슐1의 end
	i3Vector::Scale( &vCapDir[1], &CDir[1], CLength[1]);		//	길이가 포함된 캡슐2의 방향
	i3Vector::Add( &vCapEnd[1], &CStart[1], &vCapDir[1]);		//	캡슐2의 end

	//	캡슐1과 캡슐2의 최단 거리를 구한다. (2번째 라인인자들은 1번째 라인인자들보다 짧아야 옳은 결과가 반환된다.)
	REAL32 fSqrDist = GetSqrDistanceLineToLine( &CStart[0], &vCapDir[0], &CStart[1], &vCapDir[1], &fS, &fT);

	if( ( -I3_EPS < fS && fS < 1.f+I3_EPS) && (-I3_EPS < fT && fT < 1.f+I3_EPS))
	{
		if( i3Math::abs( i3Math::Sqrt( fSqrDist)) <= CRadius1 + CRadius2)
		{		
			i3Vector::Lerp( &vIntersect[0], &CStart[0], &vCapEnd[0], fS);	
			i3Vector::Lerp( &vIntersect[1], &CStart[1], &vCapEnd[1], fT);	

			goto LABEL_TRUE;
		}
	}
	else if( -I3_EPS < fS && fS < 1.f+I3_EPS)	//	캡슐1의 안쪽에 포함
	{
		REAL32 fDist = 0.f;
		VEC3D vPoint, vDiff;		
		i3Vector::Lerp( &vPoint, &CStart[0], &vCapEnd[0], fS);		//	캡슐1의 교점

		if(-I3_EPS >= fT || fT == 0.f)				//	캡슐2 start 외곽 반지름에 포함
		{
			i3Vector::Sub( &vDiff, &vPoint, &CStart[1]);			
			i3Vector::Copy( &vIntersect[1], &CStart[1]);	

			fT = 0.f;
		}
		else if( fT >= 1.f+I3_EPS || fT == 1.f)		//	캡슐2 end 외곽 반지름에 포함
		{
			i3Vector::Sub( &vDiff, &vPoint, &vCapEnd[1]);			
			i3Vector::Copy( &vIntersect[1], &vCapEnd[1]);

			fT = 1.f;
		}
		else I3ASSERT_0;

		fDist = i3Vector::Length( &vDiff);
		fDist = i3Math::abs( fDist);

		if( fDist <= CRadius1 + CRadius2)
		{
			i3Vector::Copy( &vIntersect[0], &vPoint);	

			goto LABEL_TRUE;
		}
	}
	else if(-I3_EPS < fT && fT < 1.f+I3_EPS)	//	캡슐2의 안쪽에 포함
	{
		REAL32 fDist = 0.f;
		VEC3D vPoint, vDiff;
		i3Vector::Lerp( &vPoint, &CStart[1], &vCapEnd[1], fT);	//	캡슐2의 교점

		if( -I3_EPS >= fS || fS == 0.f)				//	캡슐1 start 외곽 반지름에 포함
		{
			i3Vector::Sub( &vDiff, &CStart[0], &vPoint);	
			i3Vector::Copy( &vIntersect[0], &CStart[0]);

			fS = 0.f;
		}
		else if( fS >= 1.f+I3_EPS || fS == 1.f)		//	캡슐1 end 외곽 반지름에 포함
		{
			i3Vector::Sub( &vDiff, &vCapEnd[0], &vPoint);	
			i3Vector::Copy( &vIntersect[0], &vCapEnd[0]);

			fS = 1.f;
		}
		else I3ASSERT_0;

		fDist = i3Vector::Length( &vDiff);
		fDist = i3Math::abs( fDist);

		if( fDist <= CRadius1 + CRadius2) 
		{		
			i3Vector::Copy( &vIntersect[1], &vPoint);

			goto LABEL_TRUE;
		}
	}	

	return false;

LABEL_TRUE:
	if( fCLength1 < fCLength2)
	{
		if( t1 != nullptr)		*t1 = fT;
		if( t2 != nullptr)		*t2 = fS;		
	}
	else
	{
		if( t1 != nullptr)		*t1 = fS;
		if( t2 != nullptr)		*t2 = fT;
	}

	if( fCLength1 < fCLength2)
	{
		if( pIntersect1 != nullptr)		i3Vector::Copy( pIntersect1, &vIntersect[1]);		//	캡슐1의 교점
		if( pIntersect2 != nullptr)		i3Vector::Copy( pIntersect2, &vIntersect[0]);		//	캡슐2의 교점		
	}
	else
	{
		if( pIntersect1 != nullptr)		i3Vector::Copy( pIntersect1, &vIntersect[0]);		//	캡슐1의 교점
		if( pIntersect2 != nullptr)		i3Vector::Copy( pIntersect2, &vIntersect[1]);		//	캡슐2의 교점
	}

	if( pNorm != nullptr)				
	{		
		if( fCLength1 < fCLength2)
			i3Vector::Sub( pNorm, &vIntersect[0], &vIntersect[1]);			
		else
			i3Vector::Sub( pNorm, &vIntersect[1], &vIntersect[0]);

		i3Vector::Normalize( pNorm, pNorm);
	}

	return true;
}

bool i3Math::IntersectCapsuleToSphere( VEC3D * pCStart, VEC3D * pCDir, REAL32 fCLength, REAL32 CRadius,
									  VEC3D * pSCenter,REAL32 SRadius, VEC3D * pIntersect, VEC3D * pNormal, REAL32 * t )
{
	VEC3D len, vCapEnd;

	i3Vector::Scale( &vCapEnd, pCDir, fCLength);	//	길이가 포함된 캡슐의 방향
	i3Vector::Add( &vCapEnd, &vCapEnd, pCStart);

	//	캡슙의 중심 라인과 Sphere와의 충돌을 체크
	if( GetPerpendicularPoint( pCStart, &vCapEnd, pSCenter, pIntersect, t) )
	{
		//i3Vector::Sub( &len, pIntersect, pSCenter);
		i3Vector::Sub( &len, pSCenter, pIntersect);
		if( pNormal != nullptr) i3Vector::Copy( pNormal, &len);

		if( i3Vector::Length( &len) < SRadius + CRadius)	//	캡슐의 반지름과 Sphere의 반지름의 크기를 합친 것보다 작으면
			return true;// 당첨 : 선분과의 거리가 radius 보다 작다

		return false;// 꽝 : 선분과의 거리가 radius 보다 크다
	}

	i3Vector::Sub( &len, &vCapEnd, pSCenter);
	if( i3Vector::Length( &len) < SRadius + CRadius)
	{
		*pIntersect = vCapEnd;
		*t = 1.f;

		if( pNormal != nullptr) i3Vector::Copy( pNormal, &len);

		return true;// 당첨 : pL1와의 거리가 radius 보다 작다
	}

	i3Vector::Sub( &len, pCStart, pSCenter);
	if( i3Vector::Length( &len) < SRadius + CRadius)
	{
		*pIntersect = *pCStart;
		*t = 0.f;

		if( pNormal != nullptr) i3Vector::Copy( pNormal, &len);

		return true;// 당첨 : pCenter와의 거리가 radius 보다 작다
	}

	return false;// 꽝 : 아무 상관 없다...
}

#include "i3Matrix.h"

// 타원과 라인의 충돌 체크 2006. 7. 19 added by 백성민
bool i3Math::IntersectLineToEllipsoid( VEC3D * p1, VEC3D * p2, VEC3D * pEllipCenter, MATRIX * pEllipA)
{	//	From MgcIntrLin3Elp3.cpp of 3D Game Engine Design file in Sample Disc
	VEC3D fDiff;
	VEC3D fLineDir, fMatDir, fMatDiff;

	 // set up quadratic Q(t) = a*t^2 + 2*b*t + c
	i3Vector::Sub( &fLineDir, p2, p1);
	i3Vector::Normalize( &fLineDir, &fLineDir);

	i3Vector::Sub( &fDiff, p1, pEllipCenter);
	i3Matrix::TransformVectors( &fMatDir, &fLineDir, 1, pEllipA);
	i3Matrix::TransformVectors( &fMatDiff, &fDiff, 1, pEllipA);

	REAL32 fA = i3Vector::Dot( &fLineDir, &fMatDir);
	REAL32 fB = i3Vector::Dot( &fLineDir, &fMatDiff);
	REAL32 fC = i3Vector::Dot( &fDiff, &fMatDiff) - 1.0f;

	// intersection occurs if Q(t) has real roots
	REAL32 fDiscr = fB * fB - fA * fC;

	return fDiscr >= 0.0f;
}

bool IntersectCapsuleToCylinder( void)
{

	return false;
}


bool IntersectTriLine(VEC3D* p1, VEC3D* p2,// 시작점, 끝점
                         VEC3D* n, VEC3D* p0,//삼각형의 Normal, 삼각형(평면)의 임의의 점
						 VEC3D* pc,REAL32 *t)// 받아갈 교점
{
	REAL32 num, den;
	VEC3D p21;
	//
	// Compute line vector
	// 
	//for (i=0; i<3; i++) p21[i] = p2[i] - p1[i];
	i3Vector::Sub( &p21, (const VEC3D *)p2, (const VEC3D *) p1 );


	//
	// Compute denominator.  If ~0, line and plane are parallel.
	// 
	// num = DotProduct(n[0],n[1],n[2],p0[0],p0[1],p0[2]) - ( n[0]*p1[0] + n[1]*p1[1] + n[2]*p1[2] ) ;
	// den = n[0]*p21[0] + n[1]*p21[1] + n[2]*p21[2];

	num = i3Vector::Dot( n, p0 ) - i3Vector::Dot( n, p1);
	den = i3Vector::Dot( n, &p21);

	//
	// If denominator with respect to numerator is "zero", then the line and
	// plane are considered parallel. 
	//
	if ( den*den <= 1E-15f * num * num ) return false;

	*t = num / den;

	i3Vector::Lerp( pc, p1, p2, *t);

	if ( *t > 0.0f && *t < 1.0f ) return true;
	else return false;
}

bool ContainTriPoint( VEC3D tri[3], VEC3D *pos, VEC3D *dir)
{
    VEC3D tempVec, normal;
    INT32 i;
    for(i=0; i<3; i++)
    {
        i3Vector::Sub( &tempVec, &tri[(i+1)%3], &tri[i]);
		i3Vector::Cross( &normal, &tempVec, dir);
        i3Vector::Sub( &tempVec, pos, &tri[i]);

        if( i3Vector::Dot( &tempVec, &normal ) > 0.0f )
        {
            return false;
        }
    }

    return true;
}

bool ContainTriPoint( VEC3D * tri[3], VEC3D *pos, VEC3D *dir)
{
    VEC3D tempVec, normal;
    INT32 i;
    
    for(i = 0; i < 3; i++)
    {
        i3Vector::Sub( &tempVec, tri[(i+1)%3], tri[i]);
		i3Vector::Cross( &normal, &tempVec, dir);
        i3Vector::Sub( &tempVec, pos, tri[i]);

        if( i3Vector::Dot( &tempVec, &normal ) < 0.0f )
        {
            return false;
        }
    }

    return true;
}


bool IntersectTriTri( VEC3D p0[3], VEC3D * n0, VEC3D p[3], VEC3D* pc)
{
	VEC3D dir;
	REAL32 temp;
	
	for(INT32 i=0;i<3;i++)
	{
		i3Vector::Sub( &dir, &p[i], &p[(i+1)%3]);

		if(i3Vector::Length( &dir) < 0.000001f )
		{
			return false;
		}
		i3Vector::Normalize(&dir,&dir);
		if( ContainTriPoint( p0, &p[i], &dir) )
		{
			if( IntersectTriLine( &p[i], &p[(i+1)%3], n0, &p0[0], pc, &temp) )
			{
				return true;
			}
		}
	}

	return false;
}

// 평면( p0를 통과하고 방향이 n0인 )과 한점(p)의 가장 가까운 위치(pc) 구하기
REAL32 NearestPlanePoint(VEC3D* p0,VEC3D* n0,VEC3D* p,VEC3D* pc)
{
	VEC3D p12;
	
	i3Vector::Sub( &p12, p, p0 );

	REAL32 dot = i3Vector::Dot( &p12, n0);

	*pc = *n0;
	i3Vector::Scale( pc, pc, dot);
	i3Vector::Sub( pc, p, pc);
	
	return dot;
}

// 직선(p1-p2)과 직선(q1-q2)의 가장 가까운 거리
bool NearestLineLine(VEC3D* /*p1*/,VEC3D* /*p2*/,VEC3D* /*q1*/,VEC3D* /*q2*/,REAL32* /*t1*/,REAL32* /*t2*/,REAL32* /*dist*/)
{
    
	return true;
}

bool ContainAABBoxPoint(VEC3D* p,VEC3D* pMin,VEC3D* pMax)
{
	//    .p                  .p                   . p   
	//      +-----+        +-----+        +-----+        
	//      |     |        |     |        |     |        
	//      |     |        |     |        |     |        
	//      +-----+        +-----+        +-----+        
	//                                                   
	//      +-----+        +-----+        +-----+        
	//   .  |     |        |  .  |        |     | .      
	//    p |     |        |   p |        |     |  p     
	//      +-----+        +-----+        +-----+        
	//                                                   
	//      +-----+        +-----+        +-----+        
	//      |     |        |     |        |     |        
	//      |     |        |     |        |     |        
	//      +-----+        +-----+        +-----+        
	//   .p                    .p                    .p  
	//
	//   시작점이 BoundBox안에 있을 경우 당첨
	//

	REAL32 min_tag_x;
	REAL32 max_tag_x;
	REAL32 min_tag_y;
	REAL32 max_tag_y;
	REAL32 min_tag_z;
	REAL32 max_tag_z;

	INT32 imin_tag_x;
	INT32 imax_tag_x;
	INT32 imin_tag_y;
	INT32 imax_tag_y;
	INT32 imin_tag_z;
	INT32 imax_tag_z;
	INT32 tag;
	
	// 1번          p : min : max
	// 2번          min : p : max
	// 3번          min : max : p
	//
	// ....에서  2번일 경우만 당첨
	//
	// p : min = (+)          (0 ???????...?????)
	// min : p = (-) <-- 당첨 (1 ???????...?????)
	// p : max = (-) <-- 당첨 (1 ???????...?????)
	// max : p = (+)          (0 ???????...?????)
	VEC3D Min, Max;

	i3Vector::Sub( &Min, pMin, p);
	i3Vector::Sub( &Max, p, pMax);

	min_tag_x  = i3Vector::GetX( &Min);
	max_tag_x  = i3Vector::GetX( &Max);
	min_tag_y  = i3Vector::GetY( &Min);
	max_tag_y  = i3Vector::GetY( &Max);
	min_tag_z  = i3Vector::GetZ( &Min);
	max_tag_z  = i3Vector::GetZ( &Max);
	
	imin_tag_x = *((INT32 *) & min_tag_x);
	imax_tag_x = *((INT32 *) & max_tag_x);

	imin_tag_y = *((INT32 *) & min_tag_y);
	imax_tag_y = *((INT32 *) & max_tag_y);
	
	imin_tag_z = *((INT32 *) & min_tag_z);
	imax_tag_z = *((INT32 *) & max_tag_z);
	
	// 
	// 3개의 면에 대해서 Min, Max의 모두의 조건을 만족하면 당첨
	// 
	
	tag = imin_tag_x & imax_tag_x & imin_tag_y & imax_tag_y & imin_tag_z & imax_tag_z;

	return (tag & 0x80000000) != 0;
}

/////////////////////////////////////////////////////////////////////////////
//    .p                  .p                   . p   
//      +-----+        +-----+        +-----+        
//      |     |        |     |        |     |        
//      |     |        |     |        |     |        
//      +-----+        +-----+        +-----+        
//                                                   
//      +-----+        +-----+        +-----+        
//   .  |     |        |  .  |        |     | .      
//    p |     |        |   p |        |     |  p     
//      +-----+        +-----+        +-----+        
//                                                   
//      +-----+        +-----+        +-----+        
//      |     |        |     |        |     |        
//      |     |        |     |        |     |        
//      +-----+        +-----+        +-----+        
//   .p                    .p                    .p  
//
//   시작점이 BoundBox안에 있을 경우 당첨
//
// 1번          p : min : max
// 2번          min : p : max
// 3번          min : max : p
//
// ....에서  2번일 경우만 당첨
//
// p : min = (+)          (0 ???????...?????)
// min : p = (-) <-- 당첨 (1 ???????...?????)
// p : max = (-) <-- 당첨 (1 ???????...?????)
// max : p = (+)          (0 ???????...?????)


void __BoundBoxContainPoint( bool* pResult, VEC3D* p, VEC3D* pMin, VEC3D* pMax)
{
	REAL32 min_tag_x;
	REAL32 max_tag_x;
	REAL32 min_tag_y;
	REAL32 max_tag_y;
	REAL32 min_tag_z;
	REAL32 max_tag_z;
	INT32 * imin_tag_x;
	INT32 * imax_tag_x;
	INT32 * imin_tag_y;
	INT32 * imax_tag_y;
	INT32 * imin_tag_z;
	INT32 * imax_tag_z;
	VEC3D Min, Max;

	i3Vector::Sub( &Min, pMin, p);
	i3Vector::Sub( &Max, p, pMax);

	min_tag_x  = i3Vector::GetX( &Min);
	max_tag_x  = i3Vector::GetX( &Max);
	min_tag_y  = i3Vector::GetY( &Min);
	max_tag_y  = i3Vector::GetY( &Max);
	min_tag_z  = i3Vector::GetZ( &Min);
	max_tag_z  = i3Vector::GetZ( &Max);

	imin_tag_x = ((INT32 *) & min_tag_x);
	imax_tag_x = ((INT32 *) & max_tag_x);

	imin_tag_y = ((INT32 *) & min_tag_y);
	imax_tag_y = ((INT32 *) & max_tag_y);
	
	imin_tag_z = ((INT32 *) & min_tag_z);
	imax_tag_z = ((INT32 *) & max_tag_z);
	
	*pResult = ((*imin_tag_x & *imax_tag_x & *imin_tag_y & *imax_tag_y & *imin_tag_z & *imax_tag_z) & 0x80000000) != 0;
}

bool ContainAABBoxSphere(VEC3D* p,REAL32 r,VEC3D* pMin,VEC3D* pMax)
{
	VEC3D min;
	VEC3D max;
	
	min = *pMin;
	max = *pMax;
	
	i3Vector::Add( &min, -r, -r, -r);
	i3Vector::Add( &max, r, r, r);
	
	return ContainAABBoxPoint(p,&min,&max);
}

bool ContainAABoxAABox(VEC3D* pMin1,VEC3D* pMax1,VEC3D* pMin2,VEC3D* pMax2)
{
	REAL32 tag;
	register INT32 itag;
	VEC3D Vec1;
	
	i3Vector::Sub( &Vec1, pMax1, pMin2);
	
	tag = i3Vector::GetX( &Vec1);
	itag = *((INT32 *) & tag);
	if( itag & 0x80000000)
		return false;
	
	tag = i3Vector::GetY( &Vec1);	
	itag = *((INT32 *) & tag);
	if( itag & 0x80000000)
		return false;
		
	tag = i3Vector::GetZ( &Vec1);
	itag = *((INT32 *) & tag);
	if( itag & 0x80000000)
		return false;
		
	i3Vector::Sub( &Vec1, pMax2, pMin1);
	
	tag = i3Vector::GetX( &Vec1);
	itag = *((INT32 *) & tag);
	if( itag & 0x80000000)
		return false;
		
	tag = i3Vector::GetY( &Vec1);
	itag = *((INT32 *) & tag);
	if( itag & 0x80000000)
		return false;
		
	tag = i3Vector::GetZ( &Vec1);
	itag = *((INT32 *) & tag);
	if( itag & 0x80000000)
		return false;

	return true;

}

bool ContainAABBoxPoint2D(REAL32 u,REAL32 min_u, REAL32 max_u,
							 REAL32 v,REAL32 min_v, REAL32 max_v)
{
	REAL32 min_tag_u;
	REAL32 max_tag_u;
	REAL32 min_tag_v;
	REAL32 max_tag_v;

	INT32 imin_tag_u;
	INT32 imax_tag_u;
	INT32 imin_tag_v;
	INT32 imax_tag_v;
	INT32 tag;

	min_tag_u = min_u - u;
	max_tag_u = u - max_u;
	min_tag_v = min_v - v;
	max_tag_v = v - max_v;

	imin_tag_u = *((INT32 *) &min_tag_u);
	imax_tag_u = *((INT32 *) &max_tag_u);
	
	imin_tag_v = *((INT32 *) &min_tag_v);
	imax_tag_v = *((INT32 *) &max_tag_v);
	
	tag = imin_tag_u & imax_tag_u & imin_tag_v & imax_tag_v;

	return (tag & 0x80000000) != 0;
}

void _ContainAABBoxPoint2D( bool* pResult,REAL32 u,REAL32 min_u,REAL32 max_u,REAL32 v,REAL32 min_v,REAL32 max_v)
{
	REAL32 min_tag_u;
	REAL32 max_tag_u;
	REAL32 min_tag_v;
	REAL32 max_tag_v;

	INT32 * imin_tag_u;
	INT32 * imax_tag_u;
	INT32 * imin_tag_v;
	INT32 * imax_tag_v;

	min_tag_u = min_u - u;
	max_tag_u = u - max_u;
	min_tag_v = min_v - v;
	max_tag_v = v - max_v;

	imin_tag_u = ((INT32 *) &min_tag_u);
	imax_tag_u = ((INT32 *) &max_tag_u);

	imin_tag_v = ((INT32 *) &min_tag_v);
	imax_tag_v = ((INT32 *) &max_tag_v);

	*pResult = ((*imin_tag_u & *imax_tag_u & *imin_tag_v & *imax_tag_v) & 0x80000000) != 0;
}

bool ContainAABBoxLine(VEC3D* p1,VEC3D* p2,VEC3D* pMin,VEC3D* pMax,REAL32 last_t)
{
	REAL32 t;
	bool Result;
	
	__BoundBoxContainPoint( &Result, p1,pMin,pMax);

	// 첫점이 포함되어 있다면 당첨
	if( Result)
	{
		return true;
	}

	VEC3D p21;
	VEC3D pc;
	VEC3D numMin;
	VEC3D numMax;
	VEC3D sign;

	i3Vector::Sub( &p21, p2, p1);

	i3Vector::Sub( &numMin, pMin, p1);
	i3Vector::Sub( &numMax, pMax, p1);
	i3Vector::Mul( &sign, &numMin, &numMax);
	
	{
		register REAL32 dd;
		
		dd = i3Vector::GetX( &p21);
		
		if( i3Math::abs(dd) > I3_EPS)
		{
			if( REAL_CHECK_SIGN(dd))
			{
				// p21의 방향이 음(-)이라는 뜻은 바운드박스의 Max과 접촉한다는 뜻이다.
				t = i3Vector::GetX( &numMax ) / i3Vector::GetX( &p21);
			}
			else
			{
				// p21의 방향이 양(+)이라는 뜻은 바운드박스의 Min와 접촉한다는 뜻이다.
				t = i3Vector::GetX( &numMin ) / i3Vector::GetX( &p21);		
			}

			if( (t > 0.0f) && ( t < last_t) )
			{
				i3Vector::Lerp( &pc, p1, p2, t);

				_ContainAABBoxPoint2D(	&Result, i3Vector::GetY(&pc), i3Vector::GetY(pMin), i3Vector::GetY(pMax),
													i3Vector::GetZ(&pc), i3Vector::GetZ(pMin), i3Vector::GetZ(pMax));

				if( Result)
				{
					return true;
				}
			}
		}
	}

	{
		register REAL32 dd;
		
		dd = i3Vector::GetY( &p21);
		
		if( i3Math::abs(dd) > I3_EPS)
		{
			if( REAL_CHECK_SIGN(dd))
			{
				// p21의 방향이 음(-)이라는 뜻은 바운드박스의 Max과 접촉한다는 뜻이다.
				t = i3Vector::GetY( &numMax ) / i3Vector::GetY( &p21);
			}
			else
			{
				// p21의 방향이 양(+)이라는 뜻은 바운드박스의 Min와 접촉한다는 뜻이다.
				t = i3Vector::GetY( &numMin ) / i3Vector::GetY( &p21);		
			}
			
			if( (t > 0.0f) && ( t < last_t) )
			{
				i3Vector::Lerp( &pc, p1, p2, t);
				
				_ContainAABBoxPoint2D( &Result, i3Vector::GetX(&pc), i3Vector::GetX(pMin), i3Vector::GetX(pMax),
													i3Vector::GetZ(&pc), i3Vector::GetZ(pMin), i3Vector::GetZ(pMax));

				if( Result )
				{
					return true;

				}
			}
		}
	}

	{
		register REAL32 dd;
		
		dd = i3Vector::GetZ( &p21);
		
		if( i3Math::abs(dd) > I3_EPS)
		{
			if( REAL_CHECK_SIGN(dd))	
			{
				// p21의 방향이 음(-)이라는 뜻은 바운드박스의 Max과 접촉한다는 뜻이다.
				t = i3Vector::GetZ( &numMax ) / i3Vector::GetZ( &p21);
			}
			else
			{
				// p21의 방향이 양(+)이라는 뜻은 바운드박스의 Min와 접촉한다는 뜻이다.
				t = i3Vector::GetZ( &numMin ) / i3Vector::GetZ( &p21);		
			}
			
			if( (t > 0.0f) && ( t < last_t) )
			{
				i3Vector::Lerp( &pc, p1, p2, t);

				_ContainAABBoxPoint2D( &Result, i3Vector::GetX(&pc), i3Vector::GetX(pMin), i3Vector::GetX(pMax),
											 		i3Vector::GetY(&pc), i3Vector::GetY(pMin), i3Vector::GetY(pMax));
				if( Result )
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool IntersectAABBoxLine(VEC3D* p1,VEC3D* p2,VEC3D* pMin,VEC3D* pMax,VEC3D* pc,REAL32* t)
{
	if( ContainAABBoxPoint(p1,pMin,pMax) )
	{
		*t = 0.f;
		*pc = *p1;
		return true;
	}

	VEC3D p21;

	REAL32 num;
	REAL32 den;

	i3Vector::Sub( &p21, p2, p1);

	if( i3Vector::GetX( &p21) > 0.f )
	{
		// p21의 방향이 양(+)이라는 뜻은 바운드박스의 Min과 접촉한다는 뜻이다.
		num = i3Vector::GetX( pMin) - i3Vector::GetX( p1);
		den = i3Vector::GetX(   p2) - i3Vector::GetX( p1);
		if ( den*den > 1E-15f * num * num )
		{
			(*t) = num / den;

			*pc = p21;
			i3Vector::Scale( pc, pc, *t);
			i3Vector::Add( pc, pc, p1);

			if( ContainAABBoxPoint2D(	i3Vector::GetY(pc), i3Vector::GetY(pMin), i3Vector::GetY(pMax),
											i3Vector::GetZ(pc), i3Vector::GetZ(pMin), i3Vector::GetZ(pMax)) )
			{
				return true;
			}
		}
	}
	else
	{
		// p21의 방향이 음(+)이라는 뜻은 바운드박스의 Max와 접촉한다는 뜻이다.
		num = i3Vector::GetX( pMax) - i3Vector::GetX( p1);
		den = i3Vector::GetX(   p2) - i3Vector::GetX( p1);
		if ( den*den > 1E-15f * num * num )
		{
			(*t) = num / den;

			*pc = p21;
			i3Vector::Scale( pc, pc, *t);
			i3Vector::Add( pc, pc, p1);

			if( ContainAABBoxPoint2D(	i3Vector::GetY(pc), i3Vector::GetY(pMin), i3Vector::GetY(pMax),
											i3Vector::GetZ(pc), i3Vector::GetZ(pMin), i3Vector::GetZ(pMax)) )
			{
				return true;
			}
		}
	}

	if( i3Vector::GetY( &p21) > 0.f )
	{
		// p21의 방향이 양(+)이라는 뜻은 바운드박스의 Min과 접촉한다는 뜻이다.
		num = i3Vector::GetY( pMin) - i3Vector::GetY( p1);
		den = i3Vector::GetY(   p2) - i3Vector::GetY( p1);
		if ( den*den > 1E-15f * num *num )
		{
			(*t) = num / den;

			*pc = p21;
			i3Vector::Scale( pc, pc, *t);
			i3Vector::Add( pc, pc, p1);

			if( ContainAABBoxPoint2D(	i3Vector::GetX(pc), i3Vector::GetX(pMin), i3Vector::GetX(pMax),
											i3Vector::GetZ(pc), i3Vector::GetZ(pMin), i3Vector::GetZ(pMax)) )
			{
				return true;
			}
		}
	}
	else
	{
		// p21의 방향이 음(+)이라는 뜻은 바운드박스의 Max와 접촉한다는 뜻이다.
		num = i3Vector::GetY( pMax) - i3Vector::GetY( p1);
		den = i3Vector::GetY(   p2) - i3Vector::GetY( p1);
		if ( den*den > 1E-15f * num * num )
		{
			(*t) = num / den;

			*pc = p21;
			i3Vector::Scale( pc, pc, *t);
			i3Vector::Add( pc, pc, p1);

			if( ContainAABBoxPoint2D(	i3Vector::GetX(pc), i3Vector::GetX(pMin), i3Vector::GetX(pMax),
											i3Vector::GetZ(pc), i3Vector::GetZ(pMin), i3Vector::GetZ(pMax)) )
			{
				return true;
			}
		}
	}

	if( i3Vector::GetZ( &p21) > 0.f )
	{
		// p21의 방향이 양(+)이라는 뜻은 바운드박스의 Min과 접촉한다는 뜻이다.
		num = i3Vector::GetZ( pMin) - i3Vector::GetZ( p1);
		den = i3Vector::GetZ(   p2) - i3Vector::GetZ( p1);
		if ( den*den > 1E-15f * num*num )
		{
			(*t) = num / den;

			*pc = p21;
			i3Vector::Scale( pc, pc, *t);
			i3Vector::Add( pc, pc, p1);

			if( ContainAABBoxPoint2D(	i3Vector::GetX(pc), i3Vector::GetX(pMin), i3Vector::GetX(pMax),
											i3Vector::GetY(pc), i3Vector::GetY(pMin), i3Vector::GetY(pMax)) )
			{
				return true;
			}
		}
	}
	else
	{
		// p21의 방향이 음(+)이라는 뜻은 바운드박스의 Max와 접촉한다는 뜻이다.
		num = i3Vector::GetZ( pMax) - i3Vector::GetZ( p1);
		den = i3Vector::GetZ(   p2) - i3Vector::GetZ( p1);
		if ( den*den > 1E-15f * num*num )
		{
			(*t) = num / den;

			*pc = p21;
			i3Vector::Scale( pc, pc, *t);
			i3Vector::Add( pc, pc, p1);

			if( ContainAABBoxPoint2D(	i3Vector::GetX(pc), i3Vector::GetX(pMin), i3Vector::GetX(pMax),
											i3Vector::GetY(pc), i3Vector::GetY(pMin), i3Vector::GetY(pMax)) )
			{
				return true;
			}
		}
	}

	*t = -1.f;

	return false;
}

void i3Math::NormalizeRect( i3::pack::RECT * pRect)
{
	REAL32 temp;

	if( pRect->left > pRect->right)
	{
		temp = pRect->right;
		pRect->right = pRect->left;
		pRect->left = temp;
	}

	if( pRect->top > pRect->bottom)
	{
		temp = pRect->bottom;
		pRect->bottom = pRect->top;
		pRect->top = temp;
	}
}

bool i3Math::PtInRect( i3::pack::RECT * pRect, VEC2D * pPt)
{
	if((pRect->left > pPt->x) || (pRect->right < pPt->x))
		return false;

	if((pPt->y < pRect->top) || (pPt->y > pRect->bottom))
		return false;

	return true;
}

TOUCH_TYPE i3Math::RectInRect( i3::pack::RECT * dr, i3::pack::RECT * sr)
{
	 if(( dr->left < sr->left) && (dr->right < sr->left))
		return TOUCH_NONE;
	 if(( dr->left > sr->right) && ( dr->right > sr->right))
		return TOUCH_NONE;
	 if(( dr->top < sr->top) && ( dr->bottom < sr->top))
		return TOUCH_NONE;
	 if(( dr->top > sr->bottom) && (dr->bottom > sr->bottom))
		return TOUCH_NONE;

	 if((dr->left >= sr->left) && (dr->right <= sr->right) &&
		(dr->top >= sr->top) && (dr->bottom <= sr->bottom))
		return TOUCH_WHOLE;

	 return TOUCH_PARTIAL;
}

int i3Math::GetPointOnRect( REAL32 x1, REAL32 y1, i3::pack::RECT * rc)
{
	 int rv;

	 rv = 0;

	 if( x1 < rc->left)
		rv |= CRS_LEFT;

	 if( x1 > rc->right)
		rv |= CRS_RIGHT;

	 if( y1 < rc->top)
		rv |= CRS_TOP;

	 if( y1 > rc->bottom)
		rv |= CRS_BOTTOM;

	 return rv;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// 사각영역과 직선의 교차 여부를 구한다.
// 
// Pt1 : 직선의 시작점
// Pt2 : 직선의 끝점
// Rect : 사각 영역
bool i3Math::IsTouchLineRect( VEC2D *Pt1, VEC2D *Pt2, i3::pack::RECT * pRect)
{
	int rv, lf, rb;
	REAL32 x = 0.0f, y = 0.0f;
	REAL32 div;
	int f1, f2, f3, f4;

	if( Pt1->x == Pt2->x) 
	{
		// 수직선이면...
		rv = GetPointOnRect( Pt1->x, Pt1->y, pRect);

		if( (rv & CRS_LEFT) || (rv & CRS_RIGHT))
			return false;

		return true;
	}

	if( Pt1->y == Pt2->y) 
	{
		// 수평선이면...
		rv = GetPointOnRect( Pt1->x, Pt1->y, pRect);
		if( (rv & CRS_TOP) || (rv & CRS_BOTTOM))
			return false;

		return true;
	}

	VEC2D p1, p2;

	i3Vector::Copy( &p1, Pt1);
	i3Vector::Copy( &p2, Pt2);

	for ( ; ; )			// while(1) => warning C4127: conditional expression is constant
	{
		lf = GetPointOnRect( p1.x, p1.y, pRect);
		rb = GetPointOnRect( p2.x, p2.y, pRect);
		if( (lf | rb) == 0)
			return true;

		f1 = lf & CRS_LEFT;
		f2 = lf & CRS_RIGHT;
		f3 = lf & CRS_TOP;
		f4 = lf & CRS_BOTTOM;

		f1 = rb & f1;
		f2 = rb & f2;
		f3 = rb & f3;
		f4 = rb & f4;

		if( f1 | f2 | f3 | f4)
			return false;

		if( lf & (CRS_LEFT|CRS_RIGHT|CRS_TOP|CRS_BOTTOM))
			rv = lf;
		else
			rv = rb;

		if( rv & CRS_LEFT) 
		{
			div = (p2.x - p1.x);
			I3ASSERT( div != 0.0f);

			y = (p1.y + ((p2.y - p1.y) * (pRect->left - p1.x)) / div);
			x = pRect->left;
		}

		if( rv & CRS_RIGHT) 
		{
			div = (p2.x - p1.x);
			I3ASSERT( div != 0);

			y = (p1.y + ((p2.y - p1.y) * (pRect->right - p1.x)) / div);
			x = pRect->right;
		}

		if( rv & CRS_TOP) 
		{
			div = (p2.y - p1.y);
			I3ASSERT( div != 0);

			x = (p1.x + ((p2.x - p1.x) *(pRect->top - p1.y)) / div);
			y = pRect->top;
		}

		if( rv & CRS_BOTTOM) 
		{
			div = (p2.y - p1.y);
			I3ASSERT( div != 0);

			x = (p1.x + ((p2.x - p1.x) *(pRect->bottom - p1.y)) / div);
			y = pRect->bottom;
		}

		if( rv == lf) 
		{
			p1.x = x;
			p1.y = y;
		}
		else 
		{
			p2.x = x;
			p2.y = y;
		}
	}

//	return true;		// warning C4702: unreachable code
}

///////////////////////////////////////////////////////////////////////////////////////////////
// 사각영역과 직선의 교점을 구한다.
// 
// Pt1 : 직선의 시작점
// Pt2 : 직선의 끝점
// Rect : 사각 영역
bool i3Math::GetIntersectLineFromRect( VEC2D *Pt1, VEC2D *Pt2, i3::pack::RECT * pRect)
{
	int rv, lf, rb;
	REAL32 x = 0.0f, y = 0.0f;
	REAL32 div;
	int f1, f2, f3, f4;

	if( Pt1->x == Pt2->x) 
	{
		// 수직선이면...
		rv = GetPointOnRect( Pt1->x, Pt1->y, pRect);

		if( (rv & CRS_LEFT) || (rv & CRS_RIGHT))
			return false;

		if( rv & CRS_TOP)
			Pt1->y = pRect->top;
		else if( rv & CRS_BOTTOM)
			Pt1->y = pRect->bottom;

		rv = GetPointOnRect( Pt2->x, Pt2->y, pRect);
		if( rv & CRS_TOP)
			Pt2->y = pRect->top;
		else if( rv & CRS_BOTTOM)
			Pt2->y = pRect->bottom;

		return true;
	}

	if( Pt1->y == Pt2->y) 
	{
		// 수평선이면...
		rv = GetPointOnRect( Pt1->x, Pt1->y, pRect);
		if( (rv & CRS_TOP) || (rv & CRS_BOTTOM))
			return false;

		if( rv & CRS_LEFT)
			Pt1->x = pRect->left;
		else if( rv & CRS_RIGHT)
			Pt1->x = pRect->right;

		rv = GetPointOnRect( Pt2->x, Pt2->y, pRect);
		if( rv & CRS_LEFT)
			Pt2->x = pRect->left;
		else if( rv & CRS_RIGHT)
			Pt2->x = pRect->right;

		return true;
	}

	//while(1) 
	{
		lf = GetPointOnRect( Pt1->x, Pt1->y, pRect);
		rb = GetPointOnRect( Pt2->x, Pt2->y, pRect);
		if( (lf | rb) == 0)
			return true;

		f1 = lf & CRS_LEFT;
		f2 = lf & CRS_RIGHT;
		f3 = lf & CRS_TOP;
		f4 = lf & CRS_BOTTOM;

		f1 = rb & f1;
		f2 = rb & f2;
		f3 = rb & f3;
		f4 = rb & f4;

		if( f1 | f2 | f3 | f4)
			return false;

		if( lf & (CRS_LEFT|CRS_RIGHT|CRS_TOP|CRS_BOTTOM))
			rv = lf;
		else
			rv = rb;

		if( rv & CRS_LEFT) 
		{
			div = (Pt2->x - Pt1->x);
			I3ASSERT( div != 0.0f);

			y = (Pt1->y + ((Pt2->y - Pt1->y) * (pRect->left - Pt1->x)) / div);
			x = pRect->left;
		}

		if( rv & CRS_RIGHT) 
		{
			div = (Pt2->x - Pt1->x);
			I3ASSERT( div != 0);

			y = (Pt1->y + ((Pt2->y - Pt1->y) * (pRect->right - Pt1->x)) / div);
			x = pRect->right;
		}

		if( rv & CRS_TOP) 
		{
			div = (Pt2->y - Pt1->y);
			I3ASSERT( div != 0);

			x = (Pt1->x + ((Pt2->x - Pt1->x) *(pRect->top - Pt1->y)) / div);
			y = pRect->top;
		}

		if( rv & CRS_BOTTOM) 
		{
			div = (Pt2->y - Pt1->y);
			I3ASSERT( div != 0);

			x = (Pt1->x + ((Pt2->x - Pt1->x) *(pRect->bottom - Pt1->y)) / div);
			y = pRect->bottom;
		}

		if( rv == lf) 
		{
			Pt1->x = x;
			Pt1->y = y;
		}
		else 
		{
			Pt2->x = x;
			Pt2->y = y;
		}
	}

	return true;
}

void i3Math::CalcTangentBasis( VEC3D * pPos1, VEC3D * pPos2, VEC3D * pPos3, VEC2D * pUV1, VEC2D * pUV2, VEC2D * pUV3,
							VEC3D * pTangent, VEC3D * pBinorm)
{
	VEC3D edge1, edge2, tmp1, tmp2;
	VEC2D uv1, uv2;

	i3Vector::Sub( &edge1, pPos2, pPos1);
	i3Vector::Sub( &edge2, pPos3, pPos1);
	i3Vector::Sub( &uv1, pUV2, pUV1);
	i3Vector::Sub( &uv2, pUV3, pUV1);

	REAL32 cp = (uv1.y * uv2.x) - (uv1.x * uv2.y);

	if( cp != 0.0f)
	{
		REAL32 mul = 1.0f / cp;

		// tangent   = (Edge1 * -Edge2uv.y + Edge2 * Edge1uv.y) * mul;
		// tangent.Normalize();
		i3Vector::Scale( &tmp1, &edge1, -uv2.y);
		i3Vector::Scale( &tmp2, &edge2, uv1.y);
		i3Vector::Add( &tmp1, &tmp1, &tmp2);
		i3Vector::Scale( &tmp1, &tmp1, mul);
		i3Vector::Normalize( pTangent, &tmp1);

		// bitangent = (Edge1 * -Edge2uv.x + Edge2 * Edge1uv.x) * mul;
		// bitangent.Normalize();
		i3Vector::Scale( &tmp1, &edge1, -uv2.x);
		i3Vector::Scale( &tmp2, &edge2, uv1.x);
		i3Vector::Add( &tmp1, &tmp1, &tmp2);
		i3Vector::Scale( &tmp1, &tmp1, mul);
		i3Vector::Normalize( pBinorm, &tmp1);
	}
}

bool i3Math::IsSharedEdge( VEC3D * pSrc0, VEC3D * pSrc1, VEC3D * pDest0, VEC3D * pDest1)
{
	VEC3D dir0, dir1, diff, baseLine;
	REAL32 dot, t1, t2, den;

	// 동일 Vertex를 공유하는 경우 항상 true로 반환.
	if( i3Vector::isAlike( pSrc0, pDest0) || i3Vector::isAlike( pSrc0, pDest1))
		return true;

	if( i3Vector::isAlike( pSrc1, pDest0) || i3Vector::isAlike( pSrc1, pDest1))
		return true;

	i3Vector::Sub( &baseLine, pSrc0, pSrc1);
	den = i3Vector::Normalize( &dir0, &baseLine);

	if( den < I3_EPS)
		return false;

	// Line0
	{
		i3Vector::Sub( &diff, pSrc0, pDest0);
		i3Vector::Normalize( &dir1, &diff);

		dot = i3Math::abs( i3Vector::Dot( &dir0, &dir1));	// cos
		t1	= i3Vector::Dot( &dir0, &diff);					// distance
		t1  = t1 / den;

		// 0도 180도 허용.
		if( i3Math::abs( 1.0f - dot) > I3_EPS)
			return false;
	}

	// Line1
	{
		i3Vector::Sub( &diff, pSrc0, pDest1);
		i3Vector::Normalize( &dir1, &diff);

		dot = i3Math::abs( i3Vector::Dot( &dir0, &dir1));
		t2	= i3Vector::Dot( &dir0, &diff);
		t2  = t2 / den;

		if( i3Math::abs( 1.0f - dot) > I3_EPS)
			return false;
	}

	// 위의 테스트를 통과하였다면
	// pDest0와 pDest1 두 점 모두 pSrc0->pSrc1 Line 선 상에 있다는 뜻.
	// 그러나, t1과 t2 중의 어느 하나라도 0~1 사이에 있어야 선분이 겹친 것이라
	// 확신할 수 있다.
	if( ((t1 >= 0.0f) && (t1 <= (1.0f + I3_EPS))) || (t2 >= 0.0f) && (t2 <= (1.0f + I3_EPS)))
		return true;

	return false;
}

bool i3Math::CheckSharedEdge( VEC3D	pSrcTri[3], VEC3D pDestTri[3])
{
	INT32 i, j;
	INT32 idxSrc0, idxSrc1, idxDest0, idxDest1;
	static INT32 s_SideIndex[3][2] =
	{
		{	0, 1},
		{	1, 2 },
		{	2, 0 }
	};

	for( i = 0; i < 3; i++)
	{
		idxSrc0 = s_SideIndex[i][0];
		idxSrc1 = s_SideIndex[i][1];

		#if defined( VLOG_EDGE)
			i3VLog::Begin();

			i3VLog::SetColor( 0.0f, 1.0f, 0.0f, 1.0f);
			i3VLog::Line( &pSrcTri[ idxSrc0], &pSrcTri[ idxSrc1]);
		#endif

		for( j = 0; j < 3; j++)
		{
			idxDest0 = s_SideIndex[j][0];
			idxDest1 = s_SideIndex[j][1];

			#if defined( VLOG_EDGE)
				i3VLog::SetColor( 0.0f, 0.0f, 1.0f, 1.0f);
				i3VLog::Line( &pDestTri[ idxDest0], &pDestTri[ idxDest1]);
			#endif

			if( IsSharedEdge( &pSrcTri[ idxSrc0], &pSrcTri[ idxSrc1], &pDestTri[ idxDest0], &pDestTri[ idxDest1]))
				return true;
		}

		#if defined( VLOG_EDGE)
			i3VLog::End();
		#endif
	}

	return false;
}

REAL32 i3Math::ComputeGaussian( REAL32 deviation, REAL32 n)
{
	REAL32 b, e;
	REAL32 dev = deviation * deviation;

	e = expf( -(n * n) / (2.0f * dev));
	b = i3Math::Sqrt( I3_2PI * deviation);

	return  1.0f / b * e;
}


void i3Math::SetupGaussianBlurParameters( REAL32 fX, REAL32 fY, INT32 nPixelCount, REAL32 dev, VEC4D * pDest)
{
	INT32 i, cnt;

    // Maintain a sum of all the weighting values.
	REAL32 totalWeights = ComputeGaussian( dev, 0);
	
	i3Vector::Set( &pDest[0], 0.0f, 0.0f, totalWeights, 0.0f);

	cnt = (nPixelCount >> 1);

    for( i = 0; i < cnt; i++)
    {
        // Store weights for the positive and negative taps.
        REAL32 weight = ComputeGaussian( dev, (REAL32)(i + 1));

		pDest[ i + 1].z = weight;

		totalWeights += weight * 2.0f;

        // To get the maximum amount of blurring from a limited number of
        // pixel shader samples, we take advantage of the bilinear filtering
        // hardware inside the texture fetch unit. If we position our texture
        // coordinates exactly halfway between two texels, the filtering unit
        // will average them for us, giving two samples for the price of one.
        // This allows us to step in units of two texels per sample, rather
        // than just one at a time. The 1.5 offset kicks things off by
        // positioning us nicely in between two texels.
        REAL32 sampleOffset = (i * 2) + 1.5f;

        // Store texture coordinate offsets for the positive and negative taps.
		pDest[ i + 1].x = sampleOffset * fX;
		pDest[ i + 1].y = sampleOffset * fY;
    }

    // Normalize the list of sample weightings, so they will always sum to one.
	{
		for( i = 0; i < cnt; i++)
		{
			pDest[i].z = pDest[i].z / totalWeights;
		}
	}
}

