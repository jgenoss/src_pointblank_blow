#include "i3PhysicsType.h"
#include "i3PhysicsParticle.h"

I3_CLASS_INSTANCE( i3PhysicsParticle, i3ElementBase);

i3PhysicsParticle::i3PhysicsParticle()
{
	m_pCollisionContext	= NULL;
	m_pGravity = NULL;
	m_pWindFactor = NULL;
    
	m_Mass = __RT(1.0f);
	m_InvMass = __RT(1.0f);
	m_Restitution = __RT(0.1f);
	m_Friction = __RT_0;
	m_DragFactor = __RT(0.5f);
	m_CalcTime = __RT_1;
	m_SquareTime = __RT_1;
	m_InvSquareTime = __RT_1;

	m_AccumTime = __RT_0;

	m_GroupIdx = 0;

	i3Vector::Set( &m_Position, __RT_0, __RT_0, __RT_0 );
	i3Vector::Set( &m_Force, __RT_0, __RT_0, __RT_0 );
	i3Vector::Set( &m_Velocity, __RT_0, __RT_0, __RT_0 );
	m_Style = I3_PHYSICS_INTEGRATE_EULER;
	m_TableStartIdx = -1;
	m_TableEndIdx = -1;
}

i3PhysicsParticle::~i3PhysicsParticle()
{

}

// 중력 계산
void i3PhysicsParticle::CalcGravity( RT_VEC3D * pGravity)
{	
	RT_VEC3D Vec;

	if( m_Style & I3_PHYSICS_PARAM_GRAVITY )
	{
		if( pGravity == NULL )
			i3Vector::Set( &Vec, __RT_0, i3Math::Mul( I3_PHYSICS_GRAVITY, m_Mass ), __RT_0 );
		else
			i3Vector::Scale( &Vec, pGravity, m_Mass );

		i3Vector::Add( &m_Force, &m_Force, &Vec );
	}
}

// 점성항력 계산
void i3PhysicsParticle::CalcDragFactor(void)
{	
	RT_VEC3D Vec;

	if( m_Style & I3_PHYSICS_DRAG_AIR )
	{	
		i3Vector::Scale( &Vec, &m_Velocity, m_DragFactor );
		i3Vector::Sub( &m_Force, &m_Force, &Vec );
	}
}

// 항력 계산
void i3PhysicsParticle::CalcWindFactor( RT_VEC3D * pWind )
{
	if( m_Style & I3_PHYSICS_PARAM_WINDFACTOR )
	{
		if( pWind != NULL )
			i3Vector::Add( &m_Force, &m_Force, pWind );
	}
}

void i3PhysicsParticle::CalcForce( void )
{
	i3Vector::Set( &m_Force, __RT_0, __RT_0, __RT_0 );

	if( m_Style & I3_PHYSICS_PARAM_LOCKED )
		return ;

	// 중력 계산
	CalcGravity( m_pGravity );

	// 점성 항력 계산
	CalcDragFactor();

	// 항력 계산
	CalcWindFactor( m_pWindFactor );
}

void i3PhysicsParticle::CalcIntegrate( RT_REAL32 dTime )
{
	// Euler
	if( m_Style & I3_PHYSICS_INTEGRATE_EULER )
	{
		i3Physics::CalcIntegrateOverTime( &m_Velocity, &m_Position,
			&m_Force, &m_Velocity, &m_Velocity, &m_Position, dTime, m_InvMass );
	}
	else if( m_Style & I3_PHYSICS_INTEGRATE_MIDP )
	{
		RT_REAL32 f = i3Math::Mul( dTime, __RT(0.5f) );
		RT_VEC3D InitialV, InitialP;

		i3Vector::Copy( &InitialV, &m_Velocity );
		i3Vector::Copy( &InitialP, &m_Position );

		i3Physics::CalcIntegrateOverTime( &InitialV, &InitialP, &m_Force, &m_Velocity,
			&m_Velocity, &m_Position, f, m_InvMass );

		CalcForce();

		i3Physics::CalcIntegrateOverTime( &InitialV, &InitialP, &m_Force, &m_Velocity,
			&m_Velocity, &m_Position, dTime, m_InvMass );
	}
	else if( m_Style & I3_PHYSICS_INTEGRATE_RK4 )
	{
		RT_REAL32 f = i3Math::Mul( dTime, __RT(0.5f) );
		RT_REAL32 h = i3Math::Mul( dTime, __RT(0.16666666666666666666666666666667f) );
		RT_VEC3D InitialV, InitialP;
		RT_VEC3D Accum0F, Accum0V;
		RT_VEC3D Accum1F, Accum1V;
		RT_VEC3D Accum2F, Accum2V;
		RT_VEC3D Accum3F, Accum3V;
		RT_VEC3D Vec0, Vec1, Vec2;

		i3Vector::Copy( &InitialV, &m_Velocity );
		i3Vector::Copy( &InitialP, &m_Position );

		i3Physics::CalcRungeKutta_G( &InitialV, &InitialP, &m_Force, &m_Velocity,
			&m_Velocity, &m_Position, &Accum0F, &Accum0V, f, m_InvMass );

		CalcForce();

		i3Physics::CalcRungeKutta_G( &InitialV, &InitialP, &m_Force, &m_Velocity,
			&m_Velocity, &m_Position, &Accum1F, &Accum1V, f, m_InvMass );

		CalcForce();

		i3Physics::CalcRungeKutta_G( &InitialV, &InitialP, &m_Force, &m_Velocity,
			&m_Velocity, &m_Position, &Accum2F, &Accum2V, dTime, m_InvMass );

		CalcForce();

		f = i3Math::Mul( dTime, m_InvMass );
		i3Vector::Scale( &Accum3F, &m_Force, f );
		i3Vector::Scale( &Accum3V, &m_Velocity, dTime );

		i3Vector::Add( &Vec0, &Accum1F, &Accum2F );
		i3Vector::Scale( &Vec0, &Vec0, __RT(2.0f) );

		i3Vector::Add( &Vec1, &Accum0F, &Accum3F );
		i3Vector::Add( &Vec1, &Vec0, &Vec1);

		i3Vector::Scale( &Vec2, &Vec1, h );
		i3Vector::Add( &m_Velocity, &InitialV, &Vec2 );
			
		// DETERMINE THE NEW POSITION FOR THE PARTICLE USING RK4 FORMULA
		i3Vector::Add( &Vec0, &Accum1V, &Accum2V );
		i3Vector::Scale( &Vec0, &Vec0, __RT(2.0f) );
		i3Vector::Add( &Vec1, &Accum0V, &Accum3V );
		i3Vector::Add( &Vec1, &Vec0, &Vec1 );
		i3Vector::Scale( &Vec2, &Vec1, h );
		i3Vector::Add( &m_Position, &InitialP, &Vec2 );
	}
}

//////////////////////////////////////////////////////////////////////
// Name :	CalcCollisionImpulse
// Parm :	pVecNor - 충돌점의 노말벡터 포인터
// Desc :	2005. 12. 19
//			충돌 후의 반발력을 계산하여 파티클의 위치를 조정한다.
void i3PhysicsParticle::CalcCollisionImpulse( RT_VEC3D * pVecNor )
{	
	RT_VEC3D VecNor;

	// 충돌 후의 법선에 대한 노말벡터 계산
	_calcCollisionNor( &VecNor, pVecNor );

	// 반발력 계산
	_calcRestitution( &VecNor );
	
	// 마찰계수 계산
	if( m_Style & I3_PHYSICS_COLLISION_FRICTION )
		_calcFriction( &VecNor );
}

void i3PhysicsParticle::CheckCollision( void )
{
	UINT32 CollisionType = 0;
	i3CollideeLine line;
	I3_COLLISION_RESULT * pResult;

#ifdef I3_FIXED_POINT
	VEC3D Vec;
#endif
	RT_VEC3D endV;

	if( m_Style & I3_PHYSICS_COLLISION_SELF )	// 컬리젼 그룹 설정
		CollisionType = 1;
	else if( m_Style & I3_PHYSICS_COLLISION_OBJECT )
		CollisionType = 2;
	else if( m_Style & I3_PHYSICS_COLLISION_WORLD )
		CollisionType = 3;
	else
		return;

	if( i3Vector::Length( &m_Force) == __RT_0 )
		return;

#ifdef I3_FIXED_POINT
	i3Vector::Copy( &Vec, &m_Position );
	line.SetStart( &Vec );
#else
	line.SetStart( &m_Position );
#endif
	i3Vector::Add( &endV,  &m_Position, &m_Force );
#ifdef I3_FIXED_POINT
	i3Vector::Copy( &Vec, &endV );
	line.SetEnd( &Vec );
#else
	line.SetEnd( &endV );
#endif

	// 충돌 검사
	if( m_pCollisionContext->CollisionTest( &line, 0, I3_COLLISION_STYLE_NEAREST_ONE) )
	{
		pResult = m_pCollisionContext->GetResult();
		// 충돌량 계산
		if( m_Style & I3_PHYSICS_COLLISION_IMPULSE )
		{
#ifdef I3_FIXED_POINT
			RT_VEC3D VecNor;
			i3Vector::Copy( &VecNor, &pResult->m_Normal );
			CalcCollisionImpulse( &VecNor );
#else
			CalcCollisionImpulse( &pResult->m_Normal );
#endif
			// 영향을 받은 힘으로 위치를 설정한다.
			CalcIntegrate( m_CalcTime );
		}
	}
}

void i3PhysicsParticle::CalcSimulation( RT_REAL32 dTime )
{	
	// 힘
	CalcForce();

	// 미분
	CalcIntegrate( dTime );

	// 충돌 처리
	CheckCollision();
}

// 1차 미분
void i3PhysicsParticle_CalcIntegrateOverTime( i3PhysicsParticle * pInitial, i3PhysicsParticle * pSrc, i3PhysicsParticle * pDest, RT_REAL32 dTime )
{
	i3Physics::CalcIntegrateOverTime( pInitial->getVelocity(), pInitial->getPos(),
		pSrc->getForce(), pSrc->getVelocity(), pDest->getVelocity(), pDest->getPos(), dTime, pInitial->getInvMass() );
}

void i3PhysicsParticle_CalcRungeKutta_G( i3PhysicsParticle * pInitial, i3PhysicsParticle * pSrc, i3PhysicsParticle * pDest, i3PhysicsParticle * pAccum, RT_REAL32 dTime )
{	
	i3Physics::CalcRungeKutta_G( pInitial->getVelocity(), pInitial->getPos(), pSrc->getForce(), pSrc->getVelocity(),
		pDest->getVelocity(), pDest->getPos(), pAccum->getForce(), pAccum->getVelocity(), dTime, pInitial->getInvMass() );
}
