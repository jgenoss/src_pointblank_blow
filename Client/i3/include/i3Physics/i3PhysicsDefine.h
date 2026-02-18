#if !defined( __I3_PHYSICS_DEFINE_H)
#define __I3_PHYSICS_DEFINE_H

#include "i3Math.h"

/************************************************************************/
/*                 Physics 정의                                         */
// 단위																	//
//		Gal 중력가속도 1Gal = 1c㎨										//
//		m 미터,															//
//		Sca 스칼라값													//
//		Vec 벡터값														//
// Desc : 기본 단위는 meter로 표기 중량은 kg으로 한다.					//
/************************************************************************/

// 물리 계산 여부
#define I3_PHYSICS_PARAM_LOCKED			0x00000001		// 물리 계산을 하지 않습니다.
#define I3_PHYSICS_PARAM_GRAVITY		0x00000002		// 중력의 영향을 받습니다.
#define I3_PHYSICS_PARAM_VELOCITY		0x00000004		// 속도 계산을 합니다.
#define I3_PHYSICS_PARAM_WINDFACTOR		0x00000008		//

// 계산량의 균등 여부(정밀계산이 아니라면 세팅해줄 필요가 있다)
#define I3_PHYSICS_UNIFORMED_MASS		0x00000100		// 균일 질량
#define I3_PHYSICS_UNIFORMED_FRICTION	0x00000200		// 균일 마찰력
#define I3_PHYSICS_UNIFORMED_DRAG		0x00000400		// 균일 항력

// 컬리젼 실행 여부
#define I3_PHYSICS_COLLISION_SELF		0x10000000		// 자신과 합니다.
#define I3_PHYSICS_COLLISION_OBJECT		0x20000000		// 다른 오브젝트와 합니다.
#define I3_PHYSICS_COLLISION_WORLD		0x40000000		// 월드와 합니다.

// 충돌에 대한 반발력등 물리 계산을 한다.
#define I3_PHYSICS_COLLISION_IMPULSE	0x01000000		// 충돌시 반발력을 계산합니다
#define I3_PHYSICS_COLLISION_FRICTION	0x02000000		// 마찰력을 계산합니다
#define I3_PHYSICS_DRAG_AIR				0x04000000		// 공기항력을 계산합니다.

// 미분법 알고리즘 선택
#define I3_PHYSICS_INTEGRATE_EULER		0x00100000		// 율러 미분
#define I3_PHYSICS_INTEGRATE_MIDP		0x00200000		// 중간점 미분
#define I3_PHYSICS_INTEGRATE_RK4		0x00400000		// Runge-Kutta 4th 미분

#define I3_PHYSICS_EPSILON				__RT(0.000001f)	// 이하값은 에러로 간주 최소값으로 맞춰줍니다.

#define I3_PHYSICS_GRAVITY				984.2064f		// 중력가속도 984 Gal ( cm/s*s )
#define I3_PHYSICS_INVGRAVITY			0.0010160470405394640798921852164343f
#define I3_PHYSICS_UGC					6.67259e-11f	// 만유인력 상수

//////////////////////////////////////////////////////////////////////////
// 단위 환산
// Desc : 정확한 계산을 위해 실수연산
#define cvtKMETER2MILE(km)			(REAL32)(km) * 0.62137119f
#define	cvtMETER2YARD(m)			(REAL32)(m) * 1.0936133f
#define	cvtMETER2FIT(m)				(REAL32)(m) * 3.2808399f
#define cvtCENT2INCH(cm)			(REAL32)(cm) * 0.39370079f

#define cvtMETER2NMILE(m)			(REAL32)(m) * 5.3995680345572354211663066954644e-4

#define CHECK_PHYSICS_EPSILON(e)	if(e<I3_PHYSICS_EPSILON){I3WARN("Warnning value : %f\n",e);e=I3_PHYSICS_EPSILON;}

namespace i3Physics
{
	// F = ma
	inline void cptForce( RT_VEC3D * pVecOut, RT_REAL32 Mass, RT_VEC3D * pVecA)
	{
		i3Vector::Scale( pVecOut, pVecA, Mass );
	}

	// F = 1/2 * g * dt*dt * m
	inline void cptGravity( RT_VEC3D * pVecOut, RT_VEC3D * pVecG, RT_REAL32 dTime, RT_REAL32 Mass )
	{
		RT_REAL32 f = i3Math::Mul( dTime, dTime );
		RT_REAL32 m = i3Math::Mul( __RT(0.5f), Mass );
		RT_REAL32 g = i3Math::Mul( f, m );

		i3Vector::Scale( pVecOut, pVecG, g );
	}

	// v = s / dt
	inline void cptVelocity( RT_VEC3D * pVecOut, RT_VEC3D * pVecS, RT_REAL32 dTime )
	{
		RT_REAL32 f;
		f = i3Math::div( __RT(1.0f), dTime );
		i3Vector::Scale( pVecOut, pVecS, f );
	}

	// v = F * dt / m
	inline void cptVelocity( RT_VEC3D * pVecOut, RT_VEC3D * pVecForce, RT_REAL32 dTime, RT_REAL32 InvMass )
	{
		RT_REAL32 f;
		f = i3Math::Mul( InvMass, dTime );
		i3Vector::Scale( pVecOut, pVecForce, f );
	}

	// a = v / dt
	inline void cptAccel( RT_VEC3D * pVecOut, RT_VEC3D * pVec, RT_REAL32 dTime)
	{
		RT_REAL32 f;
		f = i3Math::div( __RT(1.0f), dTime );
		i3Vector::Scale( pVecOut, pVec, f );
	}

	// s = v * dt
	inline void cptDistance( RT_VEC3D * pVecOut, RT_VEC3D * pVecS, RT_REAL32 dTime )
	{
		i3Vector::Scale( pVecOut, pVecS, dTime );
	}

	// Vn = (n * V) * n
	inline
	void cptVCollisionNormal( RT_VEC3D * pVecOut, RT_VEC3D * pVecVelocity, RT_VEC3D * pVecNormal )
	{
		RT_VEC3D Vec;
		i3Vector::Mul( &Vec, pVecNormal, pVecVelocity );
		i3Vector::Mul( pVecOut, &Vec, pVecNormal );
	}

	// Friction Vf = c * ( V - Vn )
	inline
	void cptFriction( RT_VEC3D * pVecOut, RT_REAL32 Friction, RT_VEC3D * pVelocity, RT_VEC3D * pVelocityNormal )
	{
		RT_VEC3D Vec;
		i3Vector::Sub( &Vec, pVelocity, pVelocityNormal );
		i3Vector::Scale( pVecOut, &Vec, Friction );
	}

	// Restitution Vr = -( c + 1 ) * Vn
	inline
	void cptRestitution( RT_VEC3D * pVecOut, RT_REAL32 Restitution, RT_VEC3D * pVelocityNormal )
	{
		RT_REAL32 f;
		f = Restitution + __RT_1;
		i3Vector::Scale( pVecOut, pVelocityNormal, -f );
	}

	// Center of pos

	// Center of gravity
	// Vector w를 질량으로 계산합니다.
	inline
	void CalcCenterGravityLinear( RT_VEC3D * pVecOut, RT_VEC4D * pVec1, RT_VEC4D * pVec2)
	{	
		if( pVec1->w == pVec2->w)
		{
			pVecOut->x = i3Math::div( pVec1->x + pVec2->x, __RT( 2.0f ) );
			pVecOut->y = i3Math::div( pVec1->y + pVec2->y, __RT( 2.0f ) );
			pVecOut->z = i3Math::div( pVec1->z + pVec2->z, __RT( 2.0f ) );
		}
		else
		{
			RT_VEC3D x1, x2;
			RT_REAL32 w1, w2;

			w1 = i3Math::div( pVec1->w, pVec1->w + pVec2->w );
			w2 = i3Math::div( pVec2->w, pVec1->w + pVec2->w );

			x1.x = i3Math::Mul( w1, pVec1->x );
			x1.y = i3Math::Mul( w1, pVec1->y );
			x1.z = i3Math::Mul( w1, pVec1->z );

			x2.x = i3Math::Mul( w2, pVec2->x );
			x2.y = i3Math::Mul( w2, pVec2->y );
			x2.z = i3Math::Mul( w2, pVec2->z );

			i3Vector::Add( pVecOut, &x1, &x2 );
		}
	}

	inline
	void CalcCenterGravityTri( RT_VEC3D *pVecOut, RT_VEC4D * pVec1, RT_VEC4D * pVec2, RT_VEC4D * pVec3 )
	{
		RT_REAL32 w1, w2, w3;

		if( (pVec1->w == pVec2->w) && ( pVec1->w == pVec3->w ) )
		{
			pVecOut->x = i3Math::div( pVec1->x + pVec2->x + pVec3->x, __RT( 3.0f ) );
			pVecOut->y = i3Math::div( pVec1->y + pVec2->y + pVec3->y, __RT( 3.0f ) );
			pVecOut->z = i3Math::div( pVec1->z + pVec2->z + pVec3->z, __RT( 3.0f ) );
		}
		else
		{
			RT_VEC3D x1, x2, x3;
			RT_REAL32 f = pVec1->w + pVec2->w + pVec3->w;

			w1 = i3Math::div( pVec1->w, f );
			w2 = i3Math::div( pVec2->w, f );
			w3 = i3Math::div( pVec3->w, f );

			x1.x = i3Math::Mul( w1, pVec1->x );
			x1.y = i3Math::Mul( w1, pVec1->y );
			x1.z = i3Math::Mul( w1, pVec1->z );

			x2.x = i3Math::Mul( w2, pVec2->x );
			x2.y = i3Math::Mul( w2, pVec2->y );
			x2.z = i3Math::Mul( w2, pVec2->z );
			
			x3.x = i3Math::Mul( w3, pVec3->x );
			x3.y = i3Math::Mul( w3, pVec3->y );
			x3.z = i3Math::Mul( w3, pVec3->z );

			i3Vector::Add( &x1, &x1, &x2 );
			i3Vector::Add( pVecOut, &x1, &x3 );
		}
	}

	// Integrate over Time
	inline
	void CalcIntegrateOverTime( RT_VEC3D * pInitialV, RT_VEC3D * pInitialPos, RT_VEC3D * pSrcForce, RT_VEC3D * pSrcV,
		RT_VEC3D * pDestV, RT_VEC3D * pDestPos, RT_REAL32 dTime, RT_REAL32 InvMass )
	{	
		RT_REAL32 deltaTimeMass;
		RT_VEC3D Vec;
		
		if( dTime > __RT(0.01f) )
			dTime = 0.01f;
		deltaTimeMass = i3Math::Mul( dTime, InvMass );
		// DETERMINE THE NEW VELOCITY FOR THE PARTICLE
		i3Vector::Scale( &Vec, pSrcForce, deltaTimeMass );
		i3Vector::Add( pDestV, pInitialV, &Vec );

		// SET THE NEW POSITION
		i3Vector::Scale( &Vec, pSrcV, dTime );
		i3Vector::Add( pDestPos, pInitialPos, &Vec );
	}

	inline
	void CalcRungeKutta_G( RT_VEC3D * pInitialV, RT_VEC3D * pInitialP,
		RT_VEC3D * pSrcF, RT_VEC3D * pSrcV, RT_VEC3D * pDestV, RT_VEC3D * pDestP,
		RT_VEC3D * pAccumF, RT_VEC3D * pAccumV, RT_REAL32 dTime, RT_REAL32 InvMass )
	{	
		RT_REAL32 DTMass;
		DTMass = i3Math::Mul( dTime, InvMass );

		i3Vector::Scale( pAccumF, pSrcF, DTMass );
		i3Vector::Scale( pAccumV, pSrcV, dTime );

		// DETERMINE THE NEW VELOCITY FOR THE PARTICLE OVER 1/2 TIME
		i3Vector::Add( pDestV, pInitialV, pAccumF );

		// SET THE NEW POSITION
		i3Vector::Add( pDestP, pInitialP, pAccumV );
	}
}

#endif	// __I3_PHYSICS_DEFINE_H
