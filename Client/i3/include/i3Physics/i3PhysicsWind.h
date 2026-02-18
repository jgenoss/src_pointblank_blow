#if !defined( __I3_PHYSICS_WIND_H )
#define __I3_PHYSICS_WIND_H

class I3_EXPORT_PHYSICS i3PhysicsWind : public i3ElementBase
{
	I3_CLASS_DEFINE( i3PhysicsWind );
protected:
	// Persistant member
	VEC3D			m_Force;
	REAL32			m_Constant;
	REAL32			m_minX, m_maxX;
	REAL32			m_minY, m_maxY;
	REAL32			m_minZ, m_maxZ;
	REAL32			m_UpdateTime;

	//
	VEC3D			m_Accum;
	REAL32			m_tTime;		// √— Ω√∞£ »Â∏ß ( 0 ~ 1√ )

public:
	VEC3D *		getForce( void )			{ return &m_Force; }
	void		setForce( VEC3D * pVec )	{ i3Vector::Copy( &m_Force, pVec ); }

	REAL32		getConstant( void )			{ return m_Constant; }
	void		setConstant( REAL32	k )		{ m_Constant = k; }

	REAL32		getMinX( void )				{ return m_minX; }
	void		setMinX( REAL32 x )			{ m_minX = x; }
	REAL32		getMaxX( void )				{ return m_maxX; }
	void		setMaxX( REAL32 x )			{ m_maxX = x; }
	REAL32		getMinY( void )				{ return m_minY; }
	void		setMinY( REAL32 y )			{ m_minY = y; }
	REAL32		getMaxY( void )				{ return m_maxY; }
	void		setMaxY( REAL32 y )			{ m_maxY = y; }
	REAL32		getMinZ( void )				{ return m_minZ; }
	void		setMinZ( REAL32 z )			{ m_minZ = z; }
	REAL32		getMaxZ( void )				{ return m_maxZ; }
	void		setMaxZ( REAL32 z )			{ m_maxZ = z; }

	REAL32		getUpdateTime( void )		{ return m_UpdateTime; }
	void		setUpdateTime( REAL32 t )	{ m_UpdateTime = t; }

public:
	i3PhysicsWind();
	virtual ~i3PhysicsWind();

	void		Create( VEC3D * pForce, VEC3D * pFeed );

	void		CalcRandFactor( REAL32 dTime );

	void		StepSimulation( REAL32 dTime );
};

#endif // __I3_PHYSICS_WIND_H
