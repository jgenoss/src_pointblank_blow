#if !defined( __I3_CLOTHANIMATION_H )
#define __I3_CLOTHANIMATION_H

#include "i3Base.h"
#include "i3Gfx.h"
#include "i3Scene.h"

#include "i3PhysicsParticle.h"
#include "i3Spring.h"
#include "i3PHSParticleIdx.h"

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

// Persist Particle
typedef struct ALIGN4 _tagi3PersistClothParticle
{
	// Persistent Members
	UINT32				m_Style;

	INT32				m_VIndexStart;
	INT32				m_VIndexEnd;

	REAL32				m_Mass;			// 파티클 질량(전체 천의 질량의 상대량으로 계산을 한다.) 파티클의 모든 질량을 합한값이 오브젝트의 질량이 된다.
	REAL32				m_Restitution;	// 반발 계수
	REAL32				m_Friction;		// 마찰 계수
} I3PSQ_PERSIST_CLOTH_PARTICLE;

// SpringIndex info
typedef struct ALIGN4 _tagi3ClothSpringIndexInfo
{
	INT32	m_Index0;
	INT32	m_Index1;
} I3PSQ_CLOTH_SPRING_INDEX_INFO;

// Persist Spring
typedef struct ALIGN4 _tagi3PersistClothSpring
{
	UINT32				m_Type;

	I3PSQ_CLOTH_SPRING_INDEX_INFO	m_Idx;

	REAL32				m_DampConstant;	// 완충력( 완충력 만큼 힘이 감소된다 )
	REAL32				m_Constant;		// 용수철 상수
	REAL32				m_RestLength;	// 원래 길이
	INT8				m_Reserved[4];
} I3PSQ_PERSIST_CLOTH_SPRING;

// Persist Clothobject
typedef struct ALIGN4 _tagi3PersistClothObject
{
	UINT8				m_ID[4];

	UINT32				m_Style;
	
	INT32				m_ParticleCount;
	INT32				m_SpringCount;
	OBJREF				m_pGeoAttr;
	OBJREF				m_pIndexTable;

	REAL32				m_Mass;				// 전체 질량
	PERSIST_VEC3D		m_WindFactor;		// 풍력(벡터값)
	PERSIST_VEC3D		m_Gravity;			// 중력(벡터값)
	REAL32				m_DragFactor;		// 점성 항력값(운동을 방해하는 저항력)
	INT8				pad[32];
} I3PSQ_PERSIST_CLOTH_OBJECT;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

class I3_EXPORT_PHYSICS i3ClothObject : public i3NamedElement
{
	I3_CLASS_DEFINE( i3ClothObject );
protected:
	i3GeometryAttr *	m_pGeoAttr;

	// 파티클 구조가 필요하다
	i3List				m_ParticleList;
	i3List				m_CurParticleList;
	//i3List				m_AcumParticleList0;
	//i3List				m_AcumParticleList1;
	//i3List				m_AcumParticleList2;
	//i3List				m_AcumParticleList3;
	
	// 스프링 구조가 필요하다.
	i3List				m_SpringList;

	// Collision
	i3CollisionContext *m_pCollisionContext;

	// Type
	UINT32				m_Style;			// 오브젝트 스타일

	// Physics varialble
	RT_REAL32			m_Mass;				// 전체 질량
	RT_VEC3D			m_WindFactor;		// 풍력(벡터값)
	RT_VEC3D			m_Gravity;			// 중력(벡터값)
	RT_REAL32			m_DragFactor;		// 점성 항력값(운동을 방해하는 저항력)

	INT32				m_ParticleGroupCount;

	RT_REAL32			m_Time;

	i3PHSParticleIdxTable *		m_pIndexTable;
	
public:
	i3ClothObject();
	virtual ~i3ClothObject();

	UINT32		getStyle()							{ return m_Style; }
	void		setStyle( UINT32 st )				{ m_Style = st; }
	void		addStyle( UINT32 st )				{ m_Style |= st; }
	void		removeStyle( UINT32 st )			{ m_Style &= ~st; }

	RT_REAL32	getMass()							{ return m_Mass; }
	void		setMass( RT_REAL32 m)				{ m_Mass = m; }

	RT_VEC3D *	getWind()							{ return &m_WindFactor; }
	void		setWind( RT_VEC3D * pVec )			{ i3Vector::Copy( &m_WindFactor, pVec); }

	RT_VEC3D *	getGravity()						{ return &m_Gravity; }
	void		setGravity( RT_VEC3D *pfactor )		{ i3Vector::Copy( &m_Gravity, pfactor); }

	RT_REAL32	getDrag()							{ return m_DragFactor; }
	void		setDrag( RT_REAL32 factor )			{ m_DragFactor = factor; }

	INT32		getGroupCount(void)					{ return m_ParticleGroupCount; }
	void		setGroupCount(INT32 c)				{ m_ParticleGroupCount = c; }

	INT32				getCurParticleCount(void)	{ return m_CurParticleList.GetCount(); }
	i3PhysicsParticle * getCurParticle( INT32 idx ) { return (i3PhysicsParticle *) m_CurParticleList.Items[idx]; }

	INT32				getParticleCount(void)		{ return m_ParticleList.GetCount(); }
	i3PhysicsParticle *	getParticle( INT32 idx)		{ return (i3PhysicsParticle *) m_ParticleList.Items[idx]; }
	void				RemoveAllParticles(void);

	INT32				getSpringCount(void)		{ return m_SpringList.GetCount(); }
	i3Spring *			getSpring( INT32 idx)		{ return (i3Spring *) m_SpringList.Items[idx]; }
	void				RemoveAllSprings(void);

	i3PHSParticleIdxTable *	getIndexTable(void)		{ return m_pIndexTable; }
	void					SetIndexTable( i3PHSParticleIdxTable *);

protected:
	// 파티클에 대한 모든 힘 계산
	void		_CalcForces(void);

	void		_RemoveAllAcumParticles(void);		// 계산용
	
	i3PhysicsParticle *	_FindParticleByVertexIndex( INT32 idx);

	i3PhysicsParticle * _FindParticleByGroupIndex( INT32 idx );

public:
	void		SetGeometryAttr( i3GeometryAttr * pGeoAttr );

	void		BuildParticle( I3PSQ_PERSIST_CLOTH_PARTICLE * pParticle, INT32 Count );
	void		BuildSpring( I3PSQ_PERSIST_CLOTH_SPRING * pInfo, INT32 Count );

	void		AddAcumParticle( INT32 num, i3PhysicsParticle * pParticle );
	void		AddParticle( i3PhysicsParticle * pParticle);
	void		AddSpring( i3Spring * pSpring);
	
	void		ResetParticle(void);

	// 시뮬레이션을 합니다.
	void		StepSimulation( RT_REAL32 delta );

	// 버텍스를 업데이트합니다.
	void		UpdateClothGeometry( void );

	i3CollisionContext * GetCollisionContext()		{ return m_pCollisionContext; }
	void		BindCollisionContext( i3CollisionContext * pC )	{ m_pCollisionContext = pC; }

	virtual		void	OnBuildObjectList( i3List * pList);
	virtual		UINT32	OnSave( i3ResourceFile * pResFile);
	virtual		UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif	// __I3_CLOTHANIMATION_H
