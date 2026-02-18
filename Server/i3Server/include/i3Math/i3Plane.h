#if !defined( __I3_PLANE_H)
#define __I3_PLANE_H

#include "i3MathType.h"
#include "i3Vector.h"
#include "i3Matrix.h"

enum I3_FACE_SIDE
{
	I3_FACE_FRONT = 0,
	I3_FACE_COINCIDENT,
	I3_FACE_BACK,
	I3_FACE_SPAN
};

namespace i3Plane
{
	inline
	void	Set( VEC4D * pPlane, VEC3D * p1, VEC3D * p2, VEC3D * p3)
	{
		VEC3D d1, d2;

		i3Vector::Sub( &d1, p2, p1);
		i3Vector::Sub( &d2, p3, p2);
		i3Vector::Cross( (VEC3D *) pPlane, &d1, &d2);
		i3Vector::Normalize( (VEC3D *) pPlane, (VEC3D *) pPlane);
		pPlane->w = i3Vector::Dot( (VEC3D *) pPlane, p1);
	}

	inline
	void	Set( VEC4D * pPlane, VEC3D * p1, VEC3D * pN)
	{
		i3Vector::Normalize( (VEC3D *) pPlane, pN);
		pPlane->w = i3Vector::Dot( (VEC3D *) pPlane, p1);
	}

	inline
	void	Transform( VEC4D * pDest, VEC4D * pSrc, const MATRIX * pM)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX)
		D3DXPlaneTransform( (D3DXPLANE *) pDest, (D3DXPLANE *) pSrc, (CONST D3DXMATRIX *) pM);
		#else
		#endif
	}

	inline
	void	Normalize( VEC4D * pOut, VEC4D * pIn)
	{
		#if defined( I3G_DX) || defined( I3G_XBOX)
		REAL32 mag;

		mag = i3Math::Sqrt(	(getX(pIn) * getX(pIn)) +
						(getY(pIn) * getY(pIn)) +
						(getZ(pIn) * getZ(pIn)));

		mag = 1.0f / mag;

//		i3Vector::Scale( pOut, pIn, 1.0f / mag);
		i3Vector::SetX( pOut, i3Vector::GetX(pIn) * mag);
		i3Vector::SetY( pOut, i3Vector::GetY(pIn) * mag);
		i3Vector::SetZ( pOut, i3Vector::GetZ(pIn) * mag);
		i3Vector::SetW( pOut, i3Vector::GetW(pIn) * mag);
		#endif
	}

	inline
	REAL32	Dot( VEC4D * pPlane, VEC3D * pPos)
	{
		return	(i3Vector::GetX( pPos) * i3Vector::GetX( pPlane)) +
				(i3Vector::GetY( pPos) * i3Vector::GetY( pPlane)) +
				(i3Vector::GetZ( pPos) * i3Vector::GetZ( pPlane)) + 
				i3Vector::GetW( pPlane);
	}

	inline 
	REAL32 Distance( VEC4D * pPlane, REAL32 x, REAL32 y, REAL32 z)
	{
		return (pPlane->x * x) + (pPlane->y * y) + ( pPlane->z * z) - (pPlane->w);
	}

	inline 
	REAL32 Distance( VEC4D * pPlane, VEC3D * p)
	{
		return Distance( pPlane, p->x, p->y, p->z);
	}

	inline bool isBehindPoint( VEC4D * pPlane, VEC3D * p, REAL32 eps = 10e-5f)
	{
		return Distance( pPlane, p) > eps;
	}

	inline bool isInFrontPoint( VEC4D * pPlane, VEC3D * p, REAL32 eps = 10e-5f)
	{ 
		return Distance( pPlane, p) < -eps;
	}

	inline
	I3_FACE_SIDE	getSide( VEC4D * pPlane, VEC3D * p, REAL32 eps = I3_EPS)
	{
		REAL32 dist = Distance( pPlane, p);

		if( i3Math::abs(dist) <= eps)
			return I3_FACE_COINCIDENT;
		else if( dist < -I3_EPS)
			return I3_FACE_BACK;

		return I3_FACE_FRONT;
	}

	inline
	I3_FACE_SIDE	getSide( VEC4D * pPlane, VEC3D * p1, VEC3D * p2, VEC3D * p3, REAL32 eps = I3_EPS)
	{
		INT32 front, back;
		I3_FACE_SIDE side;

		front = back = 0;

		side = getSide( pPlane, p1, eps);
		if( side == I3_FACE_FRONT)			front++;
		else if( side == I3_FACE_BACK)		back++;

		side = getSide( pPlane, p2, eps);
		if( side == I3_FACE_FRONT)			front++;
		else if( side == I3_FACE_BACK)		back++;

		side = getSide( pPlane, p3, eps);
		if( side == I3_FACE_FRONT)			front++;
		else if( side == I3_FACE_BACK)		back++;

		if((front == 0) && (back > 0))
			return I3_FACE_BACK;

		if((front > 0) && (back == 0))
			return I3_FACE_FRONT;

		if((front == 0) && (back == 0))
			return I3_FACE_COINCIDENT;

		return I3_FACE_SPAN;
	}

	inline
	bool	isInFront( VEC4D * pPlane, VEC3D * p1, VEC3D * p2, VEC3D * p3)
	{
		if( ! isInFrontPoint( pPlane, p1))
			return false;

		if( ! isInFrontPoint( pPlane, p2))
			return false;

		if( ! isInFrontPoint( pPlane, p2))
			return false;

		return true;
	}

	// Line-Plane Intersection
	//
	// http://en.wikipedia.org/wiki/Line-plane_intersection
	//
	inline bool Intersection( VEC4D * pPlane, VEC3D * pStart, VEC3D * pEnd, REAL32 * t, VEC3D * pIntersect, const REAL32 eps = 10e-5f) 
	{
		VEC3D dir;

		i3Vector::Sub( &dir, pEnd, pStart);
		//i3Vector::Normalize( &dir, &dir);

		REAL32 dot = i3Vector::Dot( (VEC3D *) pPlane, &dir);

		if( i3Math::abs(dot) < eps) 
		{ 
			//edge and plane are parallel
			return false;
		}

		*t = (pPlane->w - i3Vector::Dot( (VEC3D *) pPlane, pStart)) / dot;

		//*t = (getD()-getN().dot(l.p))/a;
		//-> plane and line intersect

		i3Vector::Scale( pIntersect, &dir, *t);
		i3Vector::Add( pIntersect, pIntersect, pStart);

		return true;
	}

	inline bool IntersectWithEdge( VEC4D * pPlane, VEC3D * pStart, VEC3D * pEnd, REAL32 * t, VEC3D * pIntersect, REAL32 eps = 10e-5f)
	{
		if( Intersection( pPlane, pStart, pEnd, t, pIntersect, eps)) 
		{
			if( *t < -eps || (1.0f+eps) < *t)
				return false; //intersection is outside of edge
		}
		else {
			return false;
		}

		//-> plane and edge intersect
		return true;
	}

};

#endif
