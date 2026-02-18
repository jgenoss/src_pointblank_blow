#include "i3PhysicsType.h"
#include "i3PhysicsParticle.h"
#include "i3Spring.h"

I3_CLASS_INSTANCE( i3Spring, i3ElementBase );

i3Spring::i3Spring()
{
	m_pLinkP1 = NULL;
	m_pLinkP2 = NULL;
	
	// test
	m_DampConstant = __RT(1.5f);
	m_Constant = __RT(0.9f);
	m_RestLength = __RT_1;
	m_InvRest	=	__RT_1;

//	m_DampConstant = __RT_0;
//	m_Constant = __RT_0;
//	m_RestLength = __RT_0;

	m_CurrentLength = __RT_0;
	i3Vector::Set( &m_VecDir, __RT_0, __RT_0, __RT_0 );
}

i3Spring::~i3Spring()
{

}

void i3Spring::_CalcCurrentDir()
{
	I3ASSERT( m_pLinkP1 != NULL );
	I3ASSERT( m_pLinkP2 != NULL );

	i3Vector::Sub( &m_VecSub, m_pLinkP1->getPos(), m_pLinkP2->getPos() );
	m_CurrentLength = i3Vector::Normalize( &m_VecDir, &m_VecSub );
}

void i3Spring::_CalcForces( RT_VEC3D * pVecOut, RT_VEC3D * pVelocity )
{	
	RT_REAL32 distOff;
	RT_REAL32 c;
	RT_REAL32 DTerm = __RT_0;
	RT_REAL32 HTerm = __RT_0;

	// 방향 벡터와 길이를 구한다.
	_CalcCurrentDir();

	// Hooke's term
	// 오차 수정 필요
	distOff = m_CurrentLength - m_RestLength;
	c = i3Math::Mul( m_Constant, __RT(984.0f));
	HTerm = i3Math::Mul( distOff, c );	// c * (dist - rest)

	// Damping term
	DTerm = i3Math::div( i3Math::Mul( i3Vector::Dot( pVelocity, &m_VecSub ), m_DampConstant ), m_CurrentLength );

	i3Vector::Scale( pVecOut, &m_VecDir, -(HTerm + DTerm) );
}

void i3Spring::CalcSimulation()
{
	I3ASSERT( m_pLinkP1 != NULL );
	I3ASSERT( m_pLinkP2 != NULL );

	RT_VEC3D springForce;
	RT_VEC3D deltaV;

	i3Vector::Sub( &deltaV, m_pLinkP1->getVelocity(), m_pLinkP2->getVelocity() );
	_CalcForces( &springForce, &deltaV );

	// Update
	if( !( m_pLinkP1->getStyle() & I3_PHYSICS_PARAM_LOCKED ) )
		i3Vector::Add( m_pLinkP1->getForce(), m_pLinkP1->getForce(), &springForce );
	if( !(m_pLinkP2->getStyle() & I3_PHYSICS_PARAM_LOCKED ) )
		i3Vector::Sub( m_pLinkP2->getForce(), m_pLinkP2->getForce(), &springForce );
}
