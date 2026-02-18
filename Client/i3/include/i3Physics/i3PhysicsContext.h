#if !defined( __I3_PHYSICS_CONTEXT_H )
#define __I3_PHYSICS_CONTEXT_H

#define I3_PHYSICS_PLAY_PARTICLE		0x00000001
#define I3_PHYSICS_PLAY_SPRING			0x00000002
#define I3_PHYSICS_PLAY_CLOTH			0x00000004
#define I3_PHYSICS_PLAY_HAIR			0x00000008
#define I3_PHYSICS_PLAY_RIGIDBODY		0x00000010
#define I3_PHYSICS_PLAY_IK				0x00000020
#define I3_PHYSICS_PLAY_ALL				0x10000000

enum _I3_PHYSICS_OBJTYPE
{
	I3_PHYSICS_OBJTYPE_PARTICLE,
	I3_PHYSICS_OBJTYPE_SPRING,
	I3_PHYSICS_OBJTYPE_CLOTH,
	I3_PHYSICS_OBJTYPE_HAIR,
	I3_PHYSICS_OBJTYPE_RIGIDBODY,
	I3_PHYSICS_OBJTYPE_IK
};

class I3_EXPORT_PHYSICS i3PhysicsContext : public i3ElementBase
{
	I3_CLASS_DEFINE( i3PhysicsContext );
protected:
	i3List		m_ParticleObjList;
	i3List		m_SpringObjList;
	i3List		m_ClothObjList;
	i3List		m_HairObjList;
	i3List		m_RigidbodyList;
	i3List		m_IKList;

	RT_REAL32	m_Time;

	UINT32		m_Style;

	void		_RemoveAllParticleObj(void);
	void		_RemoveAllSpringObj(void);
	void		_RemoveAllClothObj(void);
	void		_RemoveAllHairObj(void);
	void		_RemoveAllRigidbodyObj(void);
	void		_RemoveAllIK(void);

public:
	RT_REAL32	getTime(void)			{ return m_Time; }
	void		setTime(RT_REAL32 t)	{ m_Time = t; }

	INT32		getParticleCount(void)		{ return m_ParticleObjList.GetCount(); }
	i3PhysicsParticle * getParticle( INT32 idx)	{ return (i3PhysicsParticle *) m_ParticleObjList.Items[idx]; }

	INT32		getSpringCount(void)		{ return m_SpringObjList.GetCount(); }
	i3Spring *	getSpring( INT32 idx )	{ return (i3Spring *) m_SpringObjList.Items[idx]; }

	INT32		getClothCount(void)			{ return m_ClothObjList.GetCount(); }
	i3ClothObject * getCloth( INT32 idx )	{ return (i3ClothObject *) m_ClothObjList.Items[idx]; }

	INT32		getHairCount(void)			{ return m_HairObjList.GetCount(); }
	i3HairObject *	getHair( INT32 idx )	{ return (i3HairObject *) m_HairObjList.Items[idx]; }

	INT32		getRigidbodyCount(void)		{ return m_RigidbodyList.GetCount(); }
	i3Rigidbody * getRigidbody( INT32 idx )	{ return (i3Rigidbody *) m_RigidbodyList.Items[idx]; }

	INT32		getIKCount(void)			{ return m_IKList.GetCount(); }
	i3PhysicsIK *	getIK( INT32 idx )		{ return (i3PhysicsIK *) m_IKList.Items[idx]; }

	void		setSimulation( UINT32 flag ){ m_Style = flag; }
	void		addSimulation( UINT32 flag ){ m_Style |= flag; }
	void		removeSimulation( UINT32 flag ) { m_Style &= ~flag; }

public:
	i3PhysicsContext();
	virtual ~i3PhysicsContext();
	
	void		AddPhysicsObj( void * pObj, _I3_PHYSICS_OBJTYPE type );
	void		RemovePhysicsObj( void * pObj );
	void		RemoveAllPhysicsObj();

	void		Reset(void);

	void		CalcParticle( RT_REAL32 dTime );
	void		CalcSpring( RT_REAL32 dTime );
	void		CalcCloth( RT_REAL32 dTime );
	void		CalcHair( RT_REAL32 dTime );
	void		CalcRigidbody( RT_REAL32 dTime );
	void		CalcIK( RT_REAL32 dTime );

	void		OnUpdate( RT_REAL32 dTime );
};

#endif // __I3_PHYSICS_CONTEXT_H
