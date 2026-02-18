#ifndef _DediMath_h
#define _DediMath_h

#define BEHIND		0
#define INTERSECTS	1
#define FRONT		2

#define REAL_CHECK_SIGN(r)		(r < 0.f)


BOOL ContainTriToPoint( VEC3D * tri1,VEC3D * tri2,VEC3D * tri3, VEC3D *pos, VEC3D *dir)
{
	VEC3D tempVec, normal;
	
	i3Vector::Sub( &tempVec, tri2, tri1);
	i3Vector::Cross( &normal, &tempVec, dir);
	i3Vector::Sub( &tempVec, pos, tri1);

	if( i3Vector::Dot( &tempVec, &normal ) < 0.0f )
		return FALSE;


	i3Vector::Sub( &tempVec, tri3, tri2);
	i3Vector::Cross( &normal, &tempVec, dir);
	i3Vector::Sub( &tempVec, pos, tri2);

	if( i3Vector::Dot( &tempVec, &normal ) < 0.0f )
		return FALSE;


	i3Vector::Sub( &tempVec, tri1, tri3);
	i3Vector::Cross( &normal, &tempVec, dir);
	i3Vector::Sub( &tempVec, pos, tri3);

	if( i3Vector::Dot( &tempVec, &normal ) < 0.0f )
		return FALSE;


	return TRUE;
}

REAL32 PlaneDistance(VEC3D *pNormal, VEC3D * pPoint)
{	
	REAL32 rDistance = 0;				
	rDistance = -i3Vector::Dot(pNormal,pPoint);
	return rDistance;							
}

double AngleBetweenVectors(VEC3D *pv1, VEC3D *pv2)
{				
	
	REAL32 Dot = acos(i3Vector::Dot(pv1,pv2));				
	if(_isnan(Dot))
		return 0;
	return( Dot );
}



bool InsidePolygon(VEC3D *pIntersection, VEC3D *v1,VEC3D *v2,VEC3D *v3)
{
	const double MATCH_FACTOR = 0.99;	
	double Angle = 0.0;	

	VEC3D vA,vB,vC;




	i3Vector::Sub(&vA, v1, pIntersection);
	i3Vector::Sub(&vB, v2, pIntersection);
	i3Vector::Sub(&vC, v3, pIntersection);

	i3Vector::Normalize(&vA,&vA);
	i3Vector::Normalize(&vB,&vB);
	i3Vector::Normalize(&vC,&vC);

	Angle += AngleBetweenVectors(&vA, &vB);	
	Angle += AngleBetweenVectors(&vB, &vC);	
	Angle += AngleBetweenVectors(&vC, &vA);	



	if(Angle >= (MATCH_FACTOR * (2.0 * PI)) )
	{
		//I3TRACE("InsidePolygon  Angle %f  %f \n",Angle,MATCH_FACTOR * (2.0 * PI));
		return true;	
	}



	return false;						
}

VEC3D *ClosestPointOnLine(VEC3D *pStart,VEC3D *pEnd,VEC3D *pPoint, VEC3D *pResult)
{

	VEC3D vVec1,vVec2,vLen;

	i3Vector::Sub(&vVec1, pPoint,pStart);
	i3Vector::Sub(&vVec2,pEnd,pStart);
	i3Vector::Normalize(&vVec2,&vVec2);


	i3Vector::Sub(&vLen, pEnd , pStart);
	REAL32 d = i3Vector::Length(&vLen);

	REAL32 t = i3Vector::Dot(&vVec2,&vVec1);

	if (t <= 0) 
		return pStart;
	if (t >= d) 
		return pEnd;

	VEC3D vVec3;
	i3Vector::Scale(&vVec3, &vVec2,t);
	i3Vector::Add(pResult,pStart,&vVec3);

	return pResult;
}


bool EdgeSphereCollision(VEC3D *pCenter,VEC3D *v1,VEC3D *v2,VEC3D *v3, REAL32 rRadius)
{
	VEC3D vResult;
	{
		REAL32  rDistance =0.f;
		ClosestPointOnLine(v1,v2, pCenter,&vResult);
		rDistance = (REAL32)sqrt(distance_squared(&vResult, pCenter));
		if(rDistance < rRadius )
			return true;

		ClosestPointOnLine(v2,v3, pCenter,&vResult);
		rDistance = (REAL32)sqrt(distance_squared(&vResult, pCenter));
		if(rDistance < rRadius )
			return true;

		ClosestPointOnLine(v3,v1, pCenter,&vResult);
		rDistance = (REAL32)sqrt(distance_squared(&vResult, pCenter));
		if(rDistance < rRadius) 
			return true;
	}
	return false;
}


void GetCollisionOffset(VEC3D * pNormal, REAL32 rRadius, REAL32 rDistance ,VEC3D *pResult)
{
	VEC3D vOffset;
	i3Vector::Zero(&vOffset);

	REAL32 rDistanceOver =0.f;	

	if(rDistance > 0)
	{

		rDistanceOver = rRadius - rDistance;
		i3Vector::Scale(&vOffset, pNormal,rDistanceOver);
	}
	else 
	{

		rDistanceOver = rRadius + rDistance;
		i3Vector::Scale(&vOffset, pNormal,-rDistance);
	}

	i3Vector::Copy(pResult,&vOffset);
}


UINT32 ClassifySphere(VEC3D *pCenter,VEC3D *pNoraml,VEC3D *pPoint, REAL32 rRadius, REAL32 *pDistance)
{
	REAL32 d = PlaneDistance(pNoraml, pPoint);

	*pDistance = i3Vector::Dot(pNoraml,pCenter) + d;

	//return INTERSECTS;


	if(i3Math::abs(*pDistance) < rRadius)
		return INTERSECTS;
	else if(*pDistance >= rRadius)
		return BEHIND;
	
	return FRONT;
}

bool SphereCollision( VEC3D * v1, VEC3D * v2, VEC3D * v3,					//삼각형 v1,삼각형 v2,삼각형 v3
					  VEC3D * pNormal, VEC3D * pCenter, REAL32 rRadius,		//삼각형 노말,구 센터,구 반지름
					  VEC3D* pc,REAL32 *t)									//교점, 거리
{	

	
	REAL32 rDistance;

	VEC3D vResult;

	//같은 평면상에 충돌되는지 확인
	INT32 classification = ClassifySphere(pCenter, pNormal, v1, rRadius, &rDistance);

	if(classification == INTERSECTS) 
	{
		VEC3D vOffset, vIntersection;
		i3Vector::Scale(&vOffset,pNormal,  rDistance);
		i3Vector::Sub(&vIntersection,pCenter,&vOffset);
		i3Vector::Copy(pc, &vIntersection);


		//폴리곤 내부에 포함되어있는지 확인
		//Edge에 충돌되는지 확인


		/*
		if(ContainTriToPoint(v1,v2,v3,pCenter,&vIntersection))
			return true;
		*/
		
		
		if(InsidePolygon(&vIntersection, v1,v2,v3) ||
			EdgeSphereCollision(pCenter, v1,v2,v3, rRadius / 2))
//			if(InsidePolygon(&vIntersection, v1,v2,v3))
		{

			GetCollisionOffset(pNormal,rRadius, rDistance,&vResult);
			i3Vector::Copy(&vOffset,&vResult);

			i3Vector::Add(pc, pCenter,&vOffset);
			*t = rDistance;
			return true;
		}
		
		
	}


	return false;
}


bool TestTriPoint( VEC3D *nom ,VEC3D *pos, VEC3D *dir, VEC3D * p0)
{

	REAL32 D =  - i3Vector::Dot(p0,nom);


	REAL32 Dot1 = i3Vector::Dot(dir, nom);
	REAL32 Dot2 = i3Vector::Dot( pos, nom);

	if ((Dot1 + D) * (Dot2 + D) >= 0)
	{
		return false;
	}
	return true;
}

bool IntersectsTriangle( VEC3D *v1, VEC3D *v2, VEC3D *v3, VEC3D *pStart, VEC3D *pEnd,  VEC3D* pc,REAL32 *t)
{  

	REAL32 a,u,v;
	REAL32 f =0.f;
	  

	VEC3D e1,e2, nom,vS,vV;


	VEC3D vDir;

	i3Vector::Sub(&vDir, pEnd,pStart);

	i3Vector::Sub(&e1,v2,v1);  
	i3Vector::Sub(&e2,v3,v1);  

	i3Vector::Cross(&nom,&vDir,&e2);

	a = i3Vector::Dot(&e1,&nom);  

	if (a > -I3_EPS&& a < I3_EPS) 
		return FALSE;  

	f = 1/a;  

	i3Vector::Sub(&vS, pStart,v1);
	u = f * i3Vector::Dot(&vS,&nom);  

	if (u < 0.0 || u > 1.0)
		return FALSE;  

	i3Vector::Cross(&vV, &vS,&e1);

	v =  f * i3Vector::Dot(&vDir,&vV);
	if (v < 0.0 || u + v > 1.0)
		return FALSE;  

	// at this stage we can compute t to find out where   
	// the intersection point is on the line 

	*t = f *i3Vector::Dot(&e2,&vV);

	if (*t > 0.00001f && *t < 1.0f) // ray intersection  
	{

		i3Vector::Scale(&vDir,&vDir,*t);
		i3Vector::Add(pc,pStart,&vDir);
		
		I3TRACE("=====================================\n");
		I3TRACE("레이 시작 : %f %f %f \n",pStart->x,pStart->y,pStart->z);
		I3TRACE("레이 끝 : %f %f %f \n",vDir.x,vDir.y,vDir.z);
		I3TRACE("레이 교점  : %f %f %f \n",pc->x,pc->y,pc->z);
		I3TRACE("레이 거리  : %f \n",*t);
		I3TRACE("=====================================\n");
		
		return TRUE;
	}
			return FALSE;  
	
}  




/*
bool ContainTriToPoint( VEC3D tri[3], VEC3D *pos, VEC3D *dir)
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
			return FALSE;
		}
	}

	return TRUE;
}
*/
bool IntersectTriToLine(VEC3D* p1, VEC3D* p2,// 시작점, 끝점
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
	if ( den*den <= 1E-15f * num * num ) return FALSE;

	*t = num / den;

	i3Vector::Lerp( pc, p1, p2, *t);

	if ( *t > 0.0f && *t < 1.0f ) return TRUE;
	else return FALSE;
}

bool IntersectTriToTri( VEC3D p0[3], VEC3D * n0, VEC3D p[3], VEC3D* pc)
{
	VEC3D dir;
	REAL32 temp;

	for(INT32 i=0;i<3;i++)
	{
		i3Vector::Sub( &dir, &p[i], &p[(i+1)%3]);

		if(i3Vector::Length( &dir) < 0.000001f )
		{
			return FALSE;
		}
		i3Vector::Normalize(&dir,&dir);
		if( ContainTriToPoint( &p0[0],&p0[1],&p0[2], &p[i], &dir) )
		{
			if( IntersectTriToLine( &p[i], &p[(i+1)%3], n0, &p0[0], pc, &temp) )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}




bool ContainAABBoxToPoint2D(REAL32 u,REAL32 min_u, REAL32 max_u,
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

	if( tag & 0x80000000 )
		return TRUE;

	return FALSE;
}

bool ContainAABBoxToPoint(VEC3D* p,VEC3D* pMin,VEC3D* pMax)
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

	if( tag & 0x80000000 )
		return TRUE;

	return FALSE;
}


bool IntersectAABBoxToLine(VEC3D* p1,VEC3D* p2,VEC3D* pMin,VEC3D* pMax,VEC3D* pc,REAL32* t)
{
	if( ContainAABBoxToPoint(p1,pMin,pMax) )
	{
		*t = 0.f;
		*pc = *p1;
		return TRUE;
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
			if (*t > -I3_EPS && *t < 1.f)
			{
				*pc = p21;
				i3Vector::Scale( pc, pc, *t);
				i3Vector::Add( pc, pc, p1);

				if( ContainAABBoxToPoint2D(	i3Vector::GetY(pc), i3Vector::GetY(pMin), i3Vector::GetY(pMax),
					i3Vector::GetZ(pc), i3Vector::GetZ(pMin), i3Vector::GetZ(pMax)) )
				{
					return TRUE;
				}
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
			if (*t > -I3_EPS && *t < 1.f)
			{
				*pc = p21;
				i3Vector::Scale( pc, pc, *t);
				i3Vector::Add( pc, pc, p1);

				if( ContainAABBoxToPoint2D(	i3Vector::GetY(pc), i3Vector::GetY(pMin), i3Vector::GetY(pMax),
					i3Vector::GetZ(pc), i3Vector::GetZ(pMin), i3Vector::GetZ(pMax)) )
				{
					return TRUE;
				}
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
			if (*t > -I3_EPS && *t < 1.f)
			{
				*pc = p21;
				i3Vector::Scale( pc, pc, *t);
				i3Vector::Add( pc, pc, p1);

				if( ContainAABBoxToPoint2D(	i3Vector::GetX(pc), i3Vector::GetX(pMin), i3Vector::GetX(pMax),
					i3Vector::GetZ(pc), i3Vector::GetZ(pMin), i3Vector::GetZ(pMax)) )
				{
					return TRUE;
				}
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
			if (*t > -I3_EPS && *t < 1.f)
			{
				*pc = p21;
				i3Vector::Scale( pc, pc, *t);
				i3Vector::Add( pc, pc, p1);

				if( ContainAABBoxToPoint2D(	i3Vector::GetX(pc), i3Vector::GetX(pMin), i3Vector::GetX(pMax),
					i3Vector::GetZ(pc), i3Vector::GetZ(pMin), i3Vector::GetZ(pMax)) )
				{
					return TRUE;
				}
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
			if (*t > -I3_EPS && *t < 1.f)
			{
				*pc = p21;
				i3Vector::Scale( pc, pc, *t);
				i3Vector::Add( pc, pc, p1);

				if( ContainAABBoxToPoint2D(	i3Vector::GetX(pc), i3Vector::GetX(pMin), i3Vector::GetX(pMax),
					i3Vector::GetY(pc), i3Vector::GetY(pMin), i3Vector::GetY(pMax)) )
				{
					return TRUE;
				}
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
			if (*t > -I3_EPS && *t < 1.f)
			{
				*pc = p21;
				i3Vector::Scale( pc, pc, *t);
				i3Vector::Add( pc, pc, p1);

				if( ContainAABBoxToPoint2D(	i3Vector::GetX(pc), i3Vector::GetX(pMin), i3Vector::GetX(pMax),
					i3Vector::GetY(pc), i3Vector::GetY(pMin), i3Vector::GetY(pMax)) )
				{
					return TRUE;
				}
			}
		}
	}

	*t = -1.f;

	return FALSE;
}


#endif
