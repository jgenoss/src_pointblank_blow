#if !defined( __I3_INERTIA_H )
#define __I3_INERTIA_H

/************************************************************************/
/* 2005. 12. 16. dfly79                                                 */
/* Calculate moment of inertia											*/
/* IntertiaCube() - 직육면체의 관성모멘트 계산							*/
/* IntertiaSphere() - 구의 관성모멘트 계산								*/
/* IntertiaCylinder() - 원기둥의 관성모멘트 계산						*/
/* IntertiaPlane() - 판자의 관성모멘트 계산								*/
/*																		*/
/* E = 1/2 * (I * w * w)
/* l = m * r * r
/************************************************************************/

namespace i3Inertia
{
	inline
	RT_REAL32 GetCude( RT_REAL32 mass, RT_VEC3D * pCube, RT_VEC3D * pAxis )
	{
		return i3Physics_CalcCubeInertia( mass, pCube, pAxis );
	}

	inline
	RT_REAL32 GetSphere( RT_REAL32 mass, RT_REAL32 r )
	{
		return i3Physics_CalcSphereInertia( mass, r );
	}

	inline
	RT_REAL32 GetCylinder( RT_REAL32 mass, RT_REAL32 r, RT_REAL32 l )
	{
		return i3Physics_CalcCylinderInertia( mass, r, l );
	}

	inline
	RT_REAL32 GetCone( RT_REAL32 mass )
	{
	}
}

#endif	// __I3_INERTIA_H
