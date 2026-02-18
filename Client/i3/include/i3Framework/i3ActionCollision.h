#if !defined( __I3_ACTION_COLLISION_H__)
#define __I3_ACTION_COLLISION_H__

struct I3_ACTION_COLLISION_INFO_PLANE
{
	VEC3D	_v[4];
} ;

struct I3_ACTION_COLLISION_INFO_BOX
{
	VEC3D	_v[8];
} ;

struct I3_ACTION_COLLISION_INFO_SPHERE
{
	VEC3D	_vCenter;
	REAL32	_r = 0.0f;
} ;

struct I3_ACTION_COLLISION_INFO_CONE
{
	VEC3D	_vDir;
	REAL32	_rAngle = 0.0f;
	REAL32	_rHeight = 0.0f;
} ;

struct I3_ACTION_COLLISION_INFO_LINE
{
	VEC3D	_vOffset;
	VEC3D	_vDir;
	REAL32	_Length = 0.0f;
} ;

struct I3_ACTION_COLLISION_INFO_CONLINE	// line을 div마다 raycast
{
	VEC3D	_vOffset;
	VEC3D	_vDir;
	REAL32	_Length = 0.0f;
	REAL32	_angle = 0.0f;
};

class I3_EXPORT_FRAMEWORK i3ActionCollision : public i3Action
{
	I3_EXPORT_CLASS_DEFINE( i3ActionCollision, i3Action);

public:
	enum	SHAPE_TYPE
	{
		SHAPE_TYPE_NONE = 0,
		SHAPE_TYPE_PLANE,
		SHAPE_TYPE_BOX,
		SHAPE_TYPE_SPHERE,
		SHAPE_TYPE_CONE,
		SHAPE_TYPE_LINE,
		SHAPE_TYPE_CONLINE,
	};

private:
	// persist member
	SHAPE_TYPE		m_ShapeType = SHAPE_TYPE_NONE;
	UINT32			m_CollisionGroup = 0;

	bool			m_bAttachedBone = false;
	char *			m_pAttachedBoneName = nullptr;

	REAL32			m_StartTime = 0.0f;
	REAL32			m_Duration = 0.0f;
	bool			m_bMultiHit = false;
	bool			m_bDirectionalHit = false;

	void *			m_pShapeInfo = nullptr;

	//volatile member
	INT32			m_BoneIndex = -1;



protected:
	void			_ReleaseShapeInfo( void);

public:
	SHAPE_TYPE		getShapeType( void)					{ return m_ShapeType; }
	void			setShapeType( SHAPE_TYPE type)		{ m_ShapeType = type; }
	
	UINT32			getCollisionGroup( void)			{ return m_CollisionGroup; }
	void			setCollisionGroup( UINT32 group)	{ m_CollisionGroup = group; }

	REAL32			getStartTime( void)					{ return m_StartTime; }
	void			setStartTime( REAL32 tm)			{ m_StartTime = tm; }
	
	REAL32			getDuration( void)					{ return m_Duration; }
	void			setDuration( REAL32 tm)				{ m_Duration = tm; }

	void *			getShapeInfo( void)					{ return m_pShapeInfo; }

	void			setMultiHit( bool bEnable)			{	m_bMultiHit = bEnable;	}
	bool			getMultiHit( void)					{	return m_bMultiHit;		}

	void			setDirectionalHit( bool bEnable)	{	m_bDirectionalHit = bEnable;	}
	bool			getDirectionalHit( void)			{	return m_bDirectionalHit;		}

public:
	virtual ~i3ActionCollision();

	I3_ACTION_COLLISION_INFO_PLANE *	getPlane( void)	{ return (I3_ACTION_COLLISION_INFO_PLANE*) m_pShapeInfo; }
	void			SetPlane( VEC3D ** pVArray);

	I3_ACTION_COLLISION_INFO_BOX *		getBox( void)	{ return (I3_ACTION_COLLISION_INFO_BOX*) m_pShapeInfo; }
	void			SetBox( VEC3D ** pVArray);

	I3_ACTION_COLLISION_INFO_SPHERE *	getSphere( void)	{ return (I3_ACTION_COLLISION_INFO_SPHERE*) m_pShapeInfo; }
	void			SetSphere( VEC3D * pCenter, REAL32 r);

	I3_ACTION_COLLISION_INFO_CONE *	getCone( void)		{ return (I3_ACTION_COLLISION_INFO_CONE*) m_pShapeInfo; }
	void			SetCone( VEC3D * pDir, REAL32 angle, REAL32 height);

	I3_ACTION_COLLISION_INFO_LINE *	getLine( void)		{ return (I3_ACTION_COLLISION_INFO_LINE*) m_pShapeInfo; }
	void			SetLine( VEC3D * pOffset, VEC3D * pDir, REAL32 length);

	I3_ACTION_COLLISION_INFO_CONLINE *	getConLine(void)	{	return (I3_ACTION_COLLISION_INFO_CONLINE*)m_pShapeInfo;}
	void			SetConLine( VEC3D * pOffset, VEC3D * pDir, REAL32 length, REAL32 angle);

	bool			isAttached(void)					{ return m_bAttachedBone;	}
	
	char *			getAttachBoneName(void)				{ return m_pAttachedBoneName; }

	void			SetAttachBoneName( const char * pszName);

	virtual void	GetInfoString( char * pszStr, bool bShort, INT32 len = -1) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	/** \brief 인스턴스 정보를 저장합니다.
		\sa i3PersistantElement::OnSaveXML	*/
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	/** \brief 인스턴스 정보을 로드합니다.
		\sa i3PersistantElement::OnLoadXML	*/
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

#if defined( I3_DEBUG)
	virtual void	GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List) override;
	virtual void	OnTDKPropertyChanged( const char * szName) override;
#endif
};

#endif
