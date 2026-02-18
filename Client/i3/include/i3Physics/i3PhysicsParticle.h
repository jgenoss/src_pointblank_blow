#if !defined __I3_PHYSICSPARTICLE_H
#define __I3_PHYSICSPARTICLE_H

class I3_EXPORT_PHYSICS i3PhysicsParticle : public i3ElementBase
{
	I3_CLASS_DEFINE( i3PhysicsParticle );
protected:
	// Persistent Members
	INT32				m_TableStartIdx;
	INT32				m_TableEndIdx;

	UINT32				m_Style;		// 잠금이 되면 힘의 영향을 받지 않는다.
	RT_REAL32			m_Mass;			// 파티클 질량
	RT_REAL32			m_Restitution;	// 반발 계수(충돌)
	RT_REAL32			m_Friction;		// 마찰 계수(충돌)
	RT_REAL32			m_DragFactor;	// 점성 항력 계수(힘을 방해하는 마찰력)

	RT_REAL32			m_CalcTime;		// 계산 간격

	INT32				m_GroupIdx;		// 그룹

	// Volatile Memebers
	RT_REAL32			m_InvMass;		// 적분에 사용(내부 계산용)
	RT_VEC3D			m_Position;		// 벡터 포지션(버텍스 데이터의 위치값이 될 것이다)
	RT_VEC3D			m_Force;		// 힘
	RT_VEC3D			m_Velocity;		// 파티클 속도

	RT_REAL32			m_SquareTime;	// t * t
	RT_REAL32			m_InvSquareTime;
	RT_REAL32			m_AccumTime;// 누적 타임

	RT_VEC3D *			m_pGravity;		// 중력(외부에서 받는다) 시간에 대한 변위량이다
	RT_VEC3D *			m_pWindFactor;	// 항력(외부에서 받는다) 속도를 갖는다

	i3CollisionContext	*m_pCollisionContext;

public:
	i3PhysicsParticle();
	virtual ~i3PhysicsParticle();

	i3CollisionContext * getCollisionContext()	{ return m_pCollisionContext; }
	void		setCollisionContext( i3CollisionContext * pCollisionCXT )	{ m_pCollisionContext = pCollisionCXT; }

	RT_REAL32	getMass(void)					{ return m_Mass; }
	void		setMass( RT_REAL32 m )			{ m_Mass = m; }

	RT_REAL32	getInvMass(void)				{ return m_InvMass; }
	void		setInvMass( RT_REAL32 m )		{ m_InvMass = m; }

	RT_REAL32	getRestitution(void)			{ return m_Restitution; }
	void		setRestitution( RT_REAL32 r )	{ m_Restitution = r; }

	RT_REAL32	getFriction(void)				{ return m_Friction; }
	void		setFriction( RT_REAL32 f)		{ m_Friction = f; }

	RT_REAL32	getDragFactor(void)				{ return m_DragFactor; }
	void		setDragFactor( RT_REAL32 d )	{ m_DragFactor = d; }

	RT_REAL32	getCalcTime(void)				{ return m_CalcTime; }
	void		setCalcTime( RT_REAL32 t )		{ m_CalcTime = t; m_SquareTime = i3Math::Mul( t, t ); if( m_SquareTime > __RT_0 ) m_InvSquareTime = i3Math::div( __RT_1, m_SquareTime ); }

	RT_REAL32	getAccumTime(void)				{ return m_AccumTime; }
	void		addAccumTime( RT_REAL32 t )		{ m_AccumTime += t; }
	void		setAccumTime( RT_REAL32 t )		{ m_AccumTime = t; }

	INT32		getGroupIndex(void)				{ return m_GroupIdx; }
	void		setGroupIndex(INT32 idx)		{ m_GroupIdx = idx; }

	RT_VEC3D *	getPos(void)					{ return &m_Position; }
	void		setPos( RT_VEC3D *pVec )		{ i3Vector::Copy( &m_Position, pVec ); }

	RT_VEC3D *	getForce(void)					{ return &m_Force; }
	void		setForce( RT_VEC3D *pVec )		{ i3Vector::Copy( &m_Force, pVec ); }

	RT_VEC3D *	getVelocity(void)				{ return &m_Velocity; }
	void		setVelocity( RT_VEC3D *pVec )	{ i3Vector::Copy( &m_Velocity, pVec ); }

	RT_VEC3D *	getGravity(void)				{ return m_pGravity; }
	void		setGravity( RT_VEC3D * pVec )	{ m_pGravity = pVec; }

	RT_VEC3D *	getWind(void)					{ return m_pWindFactor; }
	void		setWind( RT_VEC3D * pVec )		{ m_pWindFactor = pVec; }

	UINT32		getStyle()						{ return m_Style; }
	void		setStyle( UINT32 type )			{ m_Style = type; }
	void		addStyle( UINT32 type )			{ m_Style |= type; }
	void		removeStyle( UINT32 type)		{ m_Style &= ~type; }

	INT32		getTableStartIndex(void)		{ return m_TableStartIdx; }
	void		setTableStartIndex( INT32 idx)	{ m_TableStartIdx = idx; }

	INT32		getTableEndIndex(void)			{ return m_TableEndIdx; }
	void		setTableEndIndex( INT32 idx)	{ m_TableEndIdx = idx; }

protected:
	// Vn = (n * V) * n
	void		_calcCollisionNor( RT_VEC3D * pVecOut, RT_VEC3D * pVecNor )
	{
		i3Physics::cptVCollisionNormal( pVecOut, &m_Velocity, pVecNor );
	}
	// Desc : 법선 벡터 계산을 마친 속도로 마찰력 계산
	void		_calcFriction( RT_VEC3D * pVelocityNor )
	{	
		i3Physics::cptFriction( &m_Force, m_Friction, &m_Velocity, pVelocityNor );
	}
	// Desc : 법선 벡터 계산을 마친 속도로 반발력 계산
	void		_calcRestitution( RT_VEC3D * pVelocityNor )
	{
		RT_VEC3D Vec;

		i3Physics::cptRestitution( &Vec, m_Restitution, pVelocityNor );
		i3Vector::Add( &m_Force, &m_Force, &Vec );
	}

public:
	// Gravity
	void		CalcGravity( RT_VEC3D * pGravity );
	// DragFactor
	void		CalcDragFactor(void);
	// WindFactor
	void		CalcWindFactor( RT_VEC3D * pWind );
	// 힘 계산
	void		CalcForce(void);

	// 미분
	void		CalcIntegrate( RT_REAL32 dTime );

	// 컬리젼 체크하여 반발력 계산등을 합니다.
	void		CalcCollisionImpulse( RT_VEC3D * pVecNor );
	void		CheckCollision( void );

	// 시뮬레이션
	void		CalcSimulation( RT_REAL32 dTime );

	// 툴용
	void		ShowGraph();
};

// 시스템 시간에 대한 미분값 계산
extern void i3PhysicsParticle_CalcIntegrateOverTime( i3PhysicsParticle * pInitial, i3PhysicsParticle * pSrc, i3PhysicsParticle * pDest, RT_REAL32 dTime );
extern void i3PhysicsParticle_CalcRungeKutta_G( i3PhysicsParticle * pInitial, i3PhysicsParticle * pSrc, i3PhysicsParticle * pDest, i3PhysicsParticle * pAccum, RT_REAL32 dTime );

#endif // __I3_PHYSICSPARTICLE_H
