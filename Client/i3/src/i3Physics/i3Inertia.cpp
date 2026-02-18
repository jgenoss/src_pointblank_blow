#include "i3PhysicsType.h"
#include "i3Math.h"

RT_REAL32 i3Physics_CalcCubeInertia( RT_REAL32 mass, RT_VEC3D * pCube, RT_VEC3D * pAxis )
{
	RT_REAL32 moment;
	RT_REAL32 h = pCube->y;
	RT_REAL32 w = pCube->x;
	RT_REAL32 l = pCube->z;
	RT_REAL32 tempw;
	RT_REAL32 templ;
	RT_REAL32 k = i3Math::Mul( __RT(1.0f / 12.0f),  mass );

	h = i3Math::Mul( h, h );
	w = i3Math::Mul( w, w );
	l = i3Math::Mul( l, l );
	
	tempw = i3Math::Mul( k, w + h );
	templ = i3Math::Mul( k, l + h );

	moment = i3Math::Mul( templ, pAxis->x );
	moment += i3Math::Mul( tempw, pAxis->z );

	return moment;
}

RT_REAL32 i3Physics_CalcSphereInertia( RT_REAL32 mass, RT_REAL32 r )
{
	RT_REAL32 k = i3Math::Mul( __RT(0.4f), mass );

	return i3Math::Mul( k, i3Math::Mul( r , r ) );
}

RT_REAL32 i3Physics_CalcCylinderInertia( RT_REAL32 mass, RT_REAL32 r, RT_REAL32 l, RT_VEC3D * pAxis )
{
	RT_REAL32 moment = __RT_0;
	RT_REAL32 r2 = i3Math::Mul( r, r );

	RT_REAL32 tempz = i3Math::Mul( __RT(0.5f), mass );
	RT_REAL32 tempx = i3Math::Mul( __RT(0.25f), mass );
	RT_REAL32 templ = i3Math::Mul( __RT(1.0f / 3.0f), mass );

	moment = i3Math::Mul( i3Math::Mul( tempz, r2 ), pAxis->z );

	moment += i3Math::Mul( i3Math::Mul( tempx, r2 ) + i3Math::Mul( templ, i3Math::Mul(l, l) ), pAxis->x );

	return moment;
}

//
RT_REAL32 i3Physics_CalcPlaneInertia( RT_REAL32 mass, RT_REAL32 width, RT_REAL32 height )
{	
	RT_REAL32 k = i3Math::div( __RT(1.0f), __RT(12.0f) );

	k = i3Math::Mul( k, mass );

	return i3Math::Mul( k, i3Math::Mul( width, width ) + i3Math::Mul( height, height ) );
}
