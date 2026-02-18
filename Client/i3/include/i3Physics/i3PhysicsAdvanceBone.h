#if !defined( __I3_PHYSICS_ADVANCEBONE_H )
#define __I3_PHYSICS_ADVANCEBONE_H

class I3_EXPORT_PHYSICS i3PhysicsAdvanceBone : public i3ElementBase
{
	I3_CLASS_DEFINE( i3PhysicsAdvanceBone );
protected:
	// Persistent Members
	i3Transform *					m_pBone;
	i3Transform *					m_pParentBone;

	REAL32	min_rx, max_rx;				// ROTATION X LIMITS
	REAL32	min_ry, max_ry;				// ROTATION Y LIMITS
	REAL32	min_rz, max_rz;				// ROTATION Z LIMITS
	REAL32	damp_width;					// 한번에 영향을 받는 뎀핑값의 한계치
	REAL32	damp_strength;				// 뎀핑값

	REAL32	m_Mass;						// MASS
	REAL32	m_Friction;					// STATIC FRICTION
	REAL32	m_Kfriction;				// KINETIC FRICTION
	REAL32	m_Elast;					// ELASTICITY

	//
	VEC3D	m_vDir;						// BONE LENGTH VECTOR

public:
	REAL32	getMinRotationX( void )			{ return min_rx; }
	void	setMinRotationX( REAL32 minX )	{ min_rx = minX; }

	REAL32	getMaxRotationX( void )			{ return max_rx; }
	void	setMaxRotationX( REAL32 maxX )	{ max_rx = maxX; }

	REAL32	getMinRotationY( void )			{ return min_ry; }
	void	setMinRotationY( REAL32 minY )	{ min_ry = minY; }

	REAL32	getMaxRotationY( void )			{ return max_ry; }
	void	setMaxRotationY( REAL32 maxY )	{ max_ry = maxY; }

	REAL32	getMinRotationZ( void )			{ return min_rz; }
	void	setMinRotationZ( REAL32 minZ )	{ min_rz = minZ; }

	REAL32	getMaxRotationZ( void )			{ return max_rz; }
	void	setMaxRotationZ( REAL32 maxZ )	{ max_rz = maxZ; }

	REAL32	getDampWidth( void )			{ return damp_width; }
	void	setDampWidth( REAL32 dampWidth )	{ damp_width = dampWidth; }

	REAL32	getDampStrength( void )			{ return damp_strength; }
	void	setDampStrength( REAL32 dampStrength )	{ damp_strength = dampStrength; }

	REAL32	getMass( void )					{ return m_Mass; }
	void	setMass( REAL32 mass )			{ m_Mass = mass; }

	REAL32	getFriction( void )				{ return m_Friction; }
	void	setFriction( REAL32 f )			{ m_Friction = f; }

	REAL32	getKfriction( void )			{ return m_Kfriction; }
	void	setKfriction( REAL32 k )		{ m_Kfriction = k; }

	REAL32	getElasticity( void )			{ return m_Elast; }
	void	setElasticity( REAL32 e )		{ m_Elast = e; }

	VEC3D *	getDirection( void )			{ return &m_vDir; }

	void	setBone( i3Transform * pBone )	{ m_pBone = pBone; }
	void	setParentBone( i3Transform * pBone )	{ m_pParentBone = pBone; }
	i3Transform * getBone( void )			{ return m_pBone; }
	i3Transform * getParentBone( void )		{ return m_pParentBone; }

public:
	i3PhysicsAdvanceBone();
	virtual ~i3PhysicsAdvanceBone();

	void Create( i3Transform * pBone, i3Transform * pParentBone );
};

#endif	// __I3_PHYSICS_ADVANCEBONE_H
