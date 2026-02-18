#if !defined( __I3_RIGIDBODY_H)
#define __I3_RIGIDBODY_H

// m_Type
enum _I3_PHYSICS_RIGIDBOTY_TYPE
{	
	RIGIDBODYTYPE_PLANE,
	RIGIDBODYTYPE_CUBE,
	RIGIDBODYTYPE_SPHERE,
	RIGIDBODYTYPE_CONE,
	RIGIDBODYTYPE_MESH
};

typedef struct ALIGN4 _tagi3RigidbodyElement
{	
	REAL32			m_Mass;
	PERSIST_VEC3D	m_CenterPos;
	PERSIST_VEC3D	m_Inertia;

} I3PSQ_PERSIST_RIGIDBODYELEMENT;

typedef struct ALIGN4 _tagi3PersistRigidbody
{
	UINT8			m_ID[4];
	UINT32			m_Type;
	UINT32			m_Style;

	INT32			m_ElementCount;
	OBJREF			m_pGeoAttr;
	
	PERSIST_MATRIX	m_Inertia;
	PERSIST_VEC3D	m_Gravity;
	PERSIST_VEC3D	m_Wind;

	REAL32			m_Mass;
	REAL32			m_Drag;

} I3PSQ_PERSIST_RIGIDBODY;

class i3RigidbodyElement;

class I3_EXPORT_PHYSICS i3Rigidbody : public i3NamedElement
{
	I3_CLASS_DEFINE( i3Rigidbody );
protected:
// Persistent members
	UINT32				m_Type;			// 강체 타입
	UINT32				m_Style;		// 강체 속성

	i3List				m_ElementList;	// 강체 구성 요소

	// 강체 속성
	REAL32				m_Mass;			// body mass
	MATRIX				m_Inertia;		// moment of inertia
	
	VEC3D				m_Gravity;
	REAL32				m_DragFactor;	// 점성 항력
	VEC3D				m_Wind;

// Volatile members
	REAL32				m_InvMass;		// inverse mass
	MATRIX				m_InvInertia;	// inverse inertia(tensor)

	VEC3D				m_Position;			// 월드 좌표
	QUATERNION			m_Orientation;		// 월드 방향
	VEC3D				m_Velocity;			// 물체 선속도
	VEC3D				m_AngularVelocity;	// 물체 각속도
	
	// 계산된 물리량
	VEC3D				m_Force;			// 힘(선 운동략)
	VEC3D				m_Moment;			// 모멘트(각 운동량)

	// 외부 힘과 토크
	VEC3D				m_externalForce;
	VEC3D				m_externalMoment;

	REAL32				m_dTime;			// delta time

	i3GeometryAttr *	m_pGeoAttr;

	void		_AddElement( i3RigidbodyElement * pObj );
	void		_RemoveAllElement( void );

	// 무게량 계산
	void		_CalcMassProperties( void );
public:
	i3Rigidbody();
	virtual ~i3Rigidbody();

	UINT32		getType( void )				{ return m_Type; }
	void		setType( UINT32 type )		{ m_Type = type; }

	UINT32		getStyle( void )			{ return m_Style; }
	void		setStyle( UINT32 s )		{ m_Style = s; }
	void		addStyle( UINT32 s )		{ m_Style |= s; }
	void		removeStyle( UINT32 s )		{ m_Style &= ~s; }

	REAL32		getMass( void )				{ return m_Mass; }
	void		setMass( REAL32 mass )		{ m_Mass = mass; }

	REAL32		getInvMass( void )			{ return m_InvMass; }
	void		setInvMass( REAL32 invM )	{ m_InvMass = invM; }

	MATRIX *	getInertia( void )			{ return &m_Inertia; }
	void		setInertia( MATRIX * m )	{ i3Matrix::Copy( &m_Inertia, m ); i3Matrix::Inverse( &m_InvInertia, NULL, &m_Inertia ); }

	MATRIX *	getInvInertia( void )		{ return &m_InvInertia; }
	void		setInvInertia( MATRIX * m )	{ i3Matrix::Copy( &m_InvInertia, m ); }

	VEC3D *		getGravity( void )			{ return &m_Gravity; }
	void		setGravity( VEC3D * pVec )	{ i3Vector::Copy( &m_Gravity, pVec ); }

	REAL32		getDrag( void )				{ return m_DragFactor; }
	void		setDrag( REAL32 d )			{ m_DragFactor = d; }

	VEC3D *		getWind( void )				{ return &m_Wind; }
	void		setWind( VEC3D * pVec )		{ i3Vector::Copy( &m_Wind, pVec ); }

	VEC3D *		getPos( void )				{ return &m_Position; }
	void		setPos( VEC3D * pVec )		{ i3Vector::Copy( &m_Position, pVec ); }

	VEC3D *		getVelocity( void )			{ return &m_Velocity; }
	void		setVelocity( VEC3D * pVec )	{ i3Vector::Copy( &m_Velocity, pVec ); }
	
	VEC3D *		getAngular( void )			{ return &m_AngularVelocity; }
	void		setAngular( VEC3D * pVec )	{ i3Vector::Copy( &m_AngularVelocity, pVec ); }

	QUATERNION * getOrientation( void )		{ return &m_Orientation; }
	void		setOrientation( QUATERNION * q )	{ i3Quat::Copy( &m_Orientation, q ); }

	VEC3D *		getForce( void )			{ return &m_Force; }
	void		setForce( VEC3D * pVec )	{ i3Vector::Copy( &m_Force, pVec ); }

	VEC3D *		getMoment( void )			{ return &m_Moment; }
	void		setMoment( VEC3D * pVec )	{ i3Vector::Copy( &m_Moment, pVec ); }

	VEC3D *		getExternalForce( void )	{ return &m_externalForce; }
	void		setExternalForce( VEC3D * pVec )	{ i3Vector::Copy( &m_externalForce, pVec ); }

	VEC3D *		getExternalMoment( void )	{ return &m_externalMoment; }
	void		setExternalMoment( VEC3D * pVec )	{ i3Vector::Copy( &m_externalMoment, pVec ); }

	REAL32		getDelta( void )			{ return m_dTime; }
	void		setDelta( REAL32 dTime )	{ m_dTime = dTime; }

	INT32		getElementCount( void )		{ return m_ElementList.GetCount(); }
	i3RigidbodyElement * getElement( INT32 idx )	{ return (i3RigidbodyElement *) m_ElementList.Items[idx]; }
	
public:
	void		Create( void );
	void		BuildElement( I3PSQ_PERSIST_RIGIDBODYELEMENT * pElement, INT32 nCount );

	void		CalcForce( void );
	// 미분
	void		CalcIntegrate( void );
	void		StepSimulation( REAL32 dTime );

	void		OnBuildObjectList( i3List *pList );
	UINT32		OnSave( i3ResourceFile * pResFile );
	UINT32		OnLoad( i3ResourceFile * pResFile );
};


// 2006. 1. 19. by dfly79
// 컬리젼 충돌에 대한 외력 계산
// pCollPoint : 충돌점
// pNor : 충돌점의 법선벡터
I3_EXTERN_PHYSICS void i3Rigidbody_ProcessCollidingContact( i3Rigidbody * pBodyA, i3Rigidbody * pBodyB , RT_VEC3D * pCollPoint, RT_VEC3D * pNor, RT_REAL32 elast );

#endif	// __I3_RIGIDBODY_H
