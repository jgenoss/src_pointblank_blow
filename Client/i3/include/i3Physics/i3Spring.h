#if !defined( __I3_SPRING_H )
#define __I3_SPRING_H

//		┌───┐①
//		*--*--*--*
//		|＼|  |  |
//		*--*--*--*
//		|  |  |  |
//		*--*--*--*
//		①		Bend spring
//		＼		Shear spring
//		*--*	Structural spring
enum I3_PHS_SPRING_TYPE
{
	MANUAL_SPRING,			// 직접 세팅한 스프링
	STRUCTURAL_SPRING,		// 기본적으로 세팅된 스프링
	SHEAR_SPRING,			// 대각선의 스프링
	BEND_SPRING				// 끝점 간의 스프링
};

//////////////////////////////////////////////////////////////////////////
// Name : class i3Spring
class I3_EXPORT_PHYSICS i3Spring : public i3ElementBase
{
	I3_CLASS_DEFINE( i3Spring );
protected:
	UINT32			m_Type;			// 스프링 타입

	i3PhysicsParticle * m_pLinkP1;	// 연결 점 1
	i3PhysicsParticle * m_pLinkP2;	// 연결 점 2

	// 상수
	RT_REAL32		m_DampConstant;	// 완충력( 완충력 만큼 힘이 감소된다 )
	RT_REAL32		m_Constant;		// 용수철 상수

	RT_REAL32		m_RestLength;	// 원래 길이
	RT_REAL32		m_InvRest;
	
	// 상태 변수
	RT_REAL32		m_CurrentLength;// 현재 길이
	RT_VEC3D		m_VecDir;		// 방향 벡터
	RT_VEC3D		m_VecSub;		// 방향 벡터에 대한 길이 포함

protected:
	void		_CalcCurrentDir();
	// 호출하기전에 파티클이 설정되어야 된다.
	void		_CalcForces( RT_VEC3D * pVecOut, RT_VEC3D * pVelocity );

public:
	i3Spring();
	virtual ~i3Spring();

	UINT32		getType(void)				{ return m_Type; }
	void		setType( UINT32 type )		{ m_Type = type; }

	RT_REAL32	getDamp(void)				{ return m_DampConstant;}
	void		setDamp( RT_REAL32 d )		{ m_DampConstant = d; }

	RT_REAL32	getConstant(void)			{ return m_Constant;}
	void		setConstant( RT_REAL32 c )	{ m_Constant = c; }

	RT_REAL32	getLength(void)				{ return m_RestLength; }
	void		setLength( RT_REAL32 l )	{ m_RestLength = l; }

	// 스프링 세팅
	void		setSpring( UINT32 type, RT_REAL32 damper, RT_REAL32 constant, RT_REAL32 restLength ) { setType(type); setDamp( damper ); setConstant( constant ); setLength( restLength ); }

	i3PhysicsParticle * getParticle1(void)	{ return m_pLinkP1; }
	i3PhysicsParticle * getParticle2(void)	{ return m_pLinkP2; }

	// 파티클 세팅
	void		setParticles( i3PhysicsParticle * p1, i3PhysicsParticle * p2 )
	{
		I3ASSERT( p1 != NULL );
		I3ASSERT( p2 != NULL );
		m_pLinkP1 = p1; m_pLinkP2 = p2;
		i3Vector::Sub( &m_VecDir, m_pLinkP1->getPos(), m_pLinkP2->getPos() );
		m_RestLength = i3Vector::Normalize( &m_VecDir, &m_VecDir );
		m_InvRest = i3Math::div( __RT(1.0f), m_RestLength );
	}
	i3PhysicsParticle *	getStartParticle(void)		{ return m_pLinkP1; }
	i3PhysicsParticle *	getEndParticle(void)		{ return m_pLinkP2; }

public:
	// 외부에서 호출하여 시뮬레이션한다.
	void		CalcSimulation();
};

#endif	// __I3_SPRING_H
