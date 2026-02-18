#include "i3MathType.h"
#include "i3Frustum.h"
#include "i3Plane.h"
#include "i3BoundBox.h"

I3_CLASS_INSTANCE( i3Frustum);

#define DW_AS_FLT(DW) (*(FLOAT*)&(DW))
#define FLT_AS_DW(F) (*(DWORD*)&(F))
#define FLT_SIGN(F) ((FLT_AS_DW(F) & 0x80000000L))
#define ALMOST_ZERO(F) ((FLT_AS_DW(F) & 0x7f800000L)==0)
#define IS_SPECIAL(F)  ((FLT_AS_DW(F) & 0x7f800000L)==0x7f800000L)

///////////////////////////////////////////////////////////////////////////
//  PlaneIntersection
//    computes the point where three planes intersect
//    returns whether or not the point exists.
static inline bool PlaneIntersection( VEC3D * intersectPt, const VEC4D * p0, const VEC4D * p1, const VEC4D * p2 )
{
    VEC3D n0, n1, n2;
    VEC3D n1_n2, n2_n0, n0_n1;  

	i3Vector::Set( &n0, p0->x, p0->y, p0->z);
	i3Vector::Set( &n1, p1->x, p1->y, p1->z);
	i3Vector::Set( &n2, p2->x, p2->y, p2->z);

	i3Vector::Cross( &n1_n2, &n1, &n2 );
	i3Vector::Cross( &n2_n0, &n2, &n0 );
	i3Vector::Cross( &n0_n1, &n0, &n1 );

	REAL32 cosTheta = i3Vector::Dot( &n0, &n1_n2 );
    
    if ( ALMOST_ZERO(cosTheta) || IS_SPECIAL(cosTheta) )
        return false;

    float secTheta = 1.f / cosTheta;

	i3Vector::Scale( &n1_n2, &n1_n2, p0->w);
	i3Vector::Scale( &n2_n0, &n2_n0, p1->w);
	i3Vector::Scale( &n0_n1, &n0_n1, p2->w);

	i3Vector::Add( intersectPt,		&n1_n2,			&n2_n0);
	i3Vector::Add( intersectPt,		intersectPt,	&n0_n1);
	i3Vector::Scale( intersectPt,	intersectPt,	-secTheta);

    return true;
}


void i3Frustum::Set( const MATRIX * matrix)
{
	//  build a view frustum based on the current view & projection matrices...
    VEC4D column4, column1, column2, column3;

	i3Vector::Set( &column4, matrix->m03, matrix->m13, matrix->m23, matrix->m33 );
    i3Vector::Set( &column1, matrix->m00, matrix->m10, matrix->m20, matrix->m30 );
    i3Vector::Set( &column2, matrix->m01, matrix->m11, matrix->m21, matrix->m31 );
    i3Vector::Set( &column3, matrix->m02, matrix->m12, matrix->m22, matrix->m32 );

	i3Vector::Sub( &vPlanes[0], &column4, &column1);		// left
    i3Vector::Add( &vPlanes[1], &column4, &column1);		// right
    i3Vector::Sub( &vPlanes[2], &column4, &column2);		// bottom
	i3Vector::Add( &vPlanes[3], &column4, &column2);		// top
    i3Vector::Sub( &vPlanes[4], &column4, &column3);		// near
	i3Vector::Add( &vPlanes[5], &column4, &column3);		// far
    // ignore near & far plane
    
    INT32 i;

	for( i = 0; i < 6; i++)
	{
		i3Plane::Normalize( &vPlanes[i], &vPlanes[i]);

		//  build a bit-field that will tell us the indices for the nearest and farthest vertices from each plane...
        nVertexLUT[i] = ((vPlanes[i].x < 0.f) ? 1:0) | 
						((vPlanes[i].y < 0.f) ? 2:0) | 
						((vPlanes[i].z < 0.f) ? 4:0);
	}

    for( i = 0; i < 8; i++)  // compute extrema
    {
        VEC4D * p0 = (i & 1) ? &vPlanes[4] : &vPlanes[5];
        VEC4D * p1 = (i & 2) ? &vPlanes[3] : &vPlanes[2];
        VEC4D * p2 = (i & 4) ? &vPlanes[0] : &vPlanes[1];

        PlaneIntersection( &pntList[i], p0, p1, p2 );
    }
}

bool	i3Frustum::TestSphere( VEC3D * pCenter, REAL32 r )
{
	bool bInside = true;
	INT32 i;

    for( i = 0; ( i < 6) && bInside; i++)
	{
		bInside &= (( i3Plane::Dot( &vPlanes[i], pCenter) + r) >= 0.f);
	}

    return bInside;
}

INT32		i3Frustum::TestBox( i3BoundBox * box )
{
	bool intersect = false;
	INT32 i;
	VEC3D vmin, vmax;

	i3Vector::Copy( &vmin, box->GetMin());
	i3Vector::Copy( &vmax, box->GetMax());

    for( i = 0; i < 6; i++)
    {
        int nV = nVertexLUT[i];

        // pVertex is diagonally opposed to nVertex
		VEC3D nVertex, pVertex;

		i3Vector::Set( &nVertex,	(nV & 1) ? vmin.x : vmax.x, 
									(nV & 2) ? vmin.y : vmax.y, 
									(nV & 4) ? vmin.z : vmax.z );

		i3Vector::Set( &pVertex,	(nV & 1) ? vmax.x : vmin.x, 
									(nV & 2) ? vmax.y : vmin.y, 
									(nV & 4) ? vmax.z : vmin.z );

		if( i3Plane::Dot( &vPlanes[i], &nVertex) < 0.f )
            return 0;

		if( i3Plane::Dot( &vPlanes[i], &pVertex) < 0.f )
            intersect = true;
    }

    return (intersect) ? 2 : 1;
}

//  this function tests if the projection of a bounding sphere along the light direction intersects
//  the view frustum 

bool SweptSpherePlaneIntersect( REAL32& t0, REAL32& t1, VEC4D * plane, VEC3D * pCenter, REAL32 radius, VEC3D * sweepDir)
{
	REAL32 b_dot_n = i3Plane::Dot( plane, pCenter);
	REAL32 d_dot_n = i3Vector::Dot( (VEC3D *) plane, sweepDir);

    if (d_dot_n == 0.f)
    {
        if (b_dot_n <= radius)
        {
            //  effectively infinity
            t0 = 0.f;
            t1 = 1e32f;
            return true;
        }
        else
            return false;
    }
    else
    {
        float tmp0 = ( radius - b_dot_n) / d_dot_n;
        float tmp1 = (-radius - b_dot_n) / d_dot_n;
		t0 = min( tmp0, tmp1);
        t1 = max( tmp0, tmp1);
        return true;
    }
}

bool	i3Frustum::TestSweptSphere( VEC3D * pCenter, REAL32 r, VEC3D * sweepDir )
{
	//  algorithm -- get all 12 intersection points of the swept sphere with the view frustum
    //  for all points >0, displace sphere along the sweep driection.  if the displaced sphere
    //  is inside the frustum, return true.  else, return false
    REAL32 displacements[12];
    INT32 cnt = 0, i;
    REAL32 a, b;
    bool inFrustum = false;

    for( i = 0; i < 6; i++)
    {
        if( SweptSpherePlaneIntersect( a, b, &vPlanes[i], pCenter, r, sweepDir))
        {
            if (a >= 0.f)
                displacements[cnt++] = a;

            if ( b >= 0.f)
                displacements[cnt++] = b;
        }
    }

    for( i = 0; i < cnt; i++)
    {
		VEC3D c;

		i3Vector::Scale( &c, sweepDir, displacements[i]);
		i3Vector::Add( &c, &c, pCenter);

        inFrustum |= TestSphere( &c, r * 1.1f);
    }

    return inFrustum;
}
