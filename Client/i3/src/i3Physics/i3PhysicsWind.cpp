#include "i3PhysicsType.h"
#include "i3PhysicsWind.h"

I3_CLASS_INSTANCE( i3PhysicsWind, i3ElementBase );

i3PhysicsWind::i3PhysicsWind()
{
	i3Vector::Set( &m_Force, 0.0f, 0.0f, 0.0f );
	m_Constant		= 0.0f;
	m_minX = m_maxX	= 0.0f;
	m_minY = m_maxY = 0.0f;
	m_minZ = m_maxZ = 0.0f;
	m_UpdateTime	= 0.0f;

	i3Vector::Set( &m_Accum, 0.0f, 0.0f, 0.0f );
	m_tTime			= 0.0f;
}

i3PhysicsWind::~i3PhysicsWind()
{

}

void i3PhysicsWind::Create( VEC3D * pForce, VEC3D * pFeed )
{
	if( pForce != NULL )
		i3Vector::Copy( &m_Force, pForce );
	
	if( pFeed != NULL )
		i3Vector::Copy( &m_Accum, pFeed );
}

void i3PhysicsWind::CalcRandFactor( REAL32 dTime )
{
	VEC3D Vec;
	REAL32 x, y, z;

	if( m_tTime > m_UpdateTime )
	{
		x = i3Math::Mul( m_Constant, i3Math::Randf2() ) + getX( &m_Force );
		y = i3Math::Mul( m_Constant, i3Math::Randf2() ) + getY( &m_Force );
		z = i3Math::Mul( m_Constant, i3Math::Randf2() ) + getZ( &m_Force );

		if( x < m_minX )
			x = m_minX;
		else if( x > m_maxX )
			x = m_maxX;

		if( y < m_minY )
			y = m_minY;
		else if( y > m_maxY )
			y = m_maxY;

		if( z < m_minZ )
			z = m_minZ;
		else if( z > m_maxZ )
			z = m_maxZ;
		
		i3Vector::Set( &m_Accum, x, y, z );

		m_tTime = 0.0f;
	}
	else
	{
		i3Vector::Sub( &Vec, &m_Force, &m_Accum );
		i3Vector::Scale( &Vec, &Vec, i3Math::div( m_tTime, m_UpdateTime ) );
		i3Vector::Add( &m_Force, &m_Force, &Vec );

		m_tTime += dTime;
	}
}

void i3PhysicsWind::StepSimulation( REAL32 dTime )
{	
	CalcRandFactor( dTime );
}

