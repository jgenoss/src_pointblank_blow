/**
 * \file	GameCharaCollisionContext.h
 *
 * Declares the game chara collision context class.
 */


#if !defined( __GAME_CHARA_COLLISION_CONTEXT_H__)
#define __GAME_CHARA_COLLISION_CONTEXT_H__

#include "GameCharaContextBase.h"
#include "GameCharaDef.h"

enum CHARA_CONTACT_BONE
{
	CHARA_CONTACT_BONE_ROOT = 0,
	CHARA_CONTACT_BONE_SPINE3,
	CHARA_CONTACT_BONE_LARM,
	CHARA_CONTACT_BONE_RARM,
	CHARA_CONTACT_BONE_LFOOT,
	CHARA_CONTACT_BONE_RFOOT,

	CHARA_CONTACT_BONE_COUNT
} ;

class CGameCharaCollisionContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaCollisionContext, CGameCharaContextBase);
protected:
	REAL32				m_rColRadius = 0.84f;				//캐릭터 관련 충돌체크에 사용될 구의 반지름
	i3Transform2 *		m_pRoughHitTrans = nullptr;			// 러프히트 컬리젼 쉐입(Sphere)
	INT32				m_DropCount = 0;				// 먼지 이팩트용 카운트

	REAL32				m_rContactTimer = 0.0f;

//	List *			m_pContactShapeSetList;		// 시물레이션 시 충돌 후 이팩트 처리용 ..쓰이지 않아서 관련 사용을 모두 주석처리함(2012.05.30.수빈)
	bool				m_bContactBones[CHARA_CONTACT_BONE_COUNT] = { false, };		// 
	VEC3D				m_vBonesForceDir[ CHARA_CONTACT_BONE_COUNT ];

	i3::vector<I3_PHYSIX_CONTACT_REPORT_PTR> m_vecContactReport;

	//	Crosshair info
	VAR_LR(I3_PHYSIX_HIT_RESULT*, 33)	m_CrossHitResult = new I3_PHYSIX_HIT_RESULT();	// 크로스헤어 히트 정보
	VAR_LR(REAL32, 34)					m_rCrossHitTimer = 0.0f;	// 크로스헤어 체크 타임
	VAR_LR(CGameCharaBase*, 35)		m_pCrossHitChara = nullptr;	// 크로스헤어에 컬리젼된 캐릭터, NULL이면 없다.
	VAR_LR(REAL32, 36)					m_CrossHitDist = 0.0f;		// 크로스헤어에 컬리전된 거리.
	i3CollideeLine						m_CrossHitLine;		// 크로스헤어 히트라인
	
	// Floor hit collision
	VAR_LR(I3_PHYSIX_HIT_RESULT*, 37)	m_FloorHitResult = new I3_PHYSIX_HIT_RESULT();	// 바닥에 대한 컬리젼 정보

	UINT16				m_nCollisionGroup = 0;

protected:
	void			AttachRoughHitCollision( void);
	void			DetachRoughHitCollision( void);

	/** \brief Shape에 대한 부위를 반환합니다.
		\note 사람형		*/
	CHARA_HIT_PART	_GetHitHuman( i3PhysixShapeSet* pDestShapeSet);

	/** \brief Shape에 대한 부위를 반환합니다.
		\note Raptor */
	CHARA_HIT_PART	_GetHitPartRaptor( i3PhysixShapeSet* pDestShapeSet);

	/** \brief Shape에 대한 부위를 반환합니다.
		\note Rex */
	CHARA_HIT_PART	_GetHitPartRex( i3PhysixShapeSet * pDestShapeSet);

	/** \brief Mission의 폭탄을 바라보고 있는지 확인합니다. */
	void			_CheckCrossHitMissionBomb( void);

	void			_CollisionHitMission( WeaponBase * pWeapon);



public:
	/** \brief collision group 을 반환합니다. */
	UINT16				getCollisionGroup( void)				{ return m_nCollisionGroup; }

	/** \brief collision의 반지름값을 반환합니다. */
	REAL32				getCollisionRadius( void)				{ return m_rColRadius; }

	/** \brief collision의 반지름값을 설정합니다. */
	void				setCollisionRadius( REAL32 val)			{ m_rColRadius = val; }

	/** \brief 지면과 충돌될 시의 shape hit count를 반환합니다. */
	INT32				getDropHitCount( void)					{ return m_DropCount; }
	void				setDropHitCount( INT32 val)				{ m_DropCount = val; }

	/** \brief 캐릭터를 둘러싼 Sphere shape을 반환합니다. */
	i3Transform2 *		getRoughBound( void )					{ return m_pRoughHitTrans; }

	/** \brief 충돌된 bone들의 힘의 방향을 반환합니다. */
	VEC3D *				getBonesForceDir( INT32 idx)			{ I3_BOUNDCHK( idx, CHARA_CONTACT_BONE_COUNT); return &m_vBonesForceDir[ idx]; }

	/** \brief 충돌 shape에 대한 list를 반환합니다. */
	// List *			getContactShapeSetList( void)			{ return m_pContactShapeSetList; }

	/** \brief Crosshair의 hit line을 반환합니다. */
	i3CollideeLine *	getCrossHitLine(void)					{ return &m_CrossHitLine;		}

	/** \brief Crosshair의 hit 거리를 반환합니다.
		\note 무기에 따라 변경됩니다. */
	REAL32				getCrossHitDistance(void)				{ return m_CrossHitDist; }

	/** \brief 바닥면에 대한 raycast 결과를 반환합니다. */
	I3_PHYSIX_HIT_RESULT * getFloorHitResult( void)				{ return m_FloorHitResult; }

	/** \brief 바닥면에 대한 raycast 결과를 설정합니다. */
	void				setFloorHitResult( I3_PHYSIX_HIT_RESULT * pResult )	{ I3ASSERT( pResult != nullptr); i3Physix::HitResultCopyTo( m_FloorHitResult, pResult); }

public:
	CGameCharaCollisionContext();
	virtual ~CGameCharaCollisionContext();

	virtual void	OnDestroy( void) override;

	virtual void	OnReset( void) override;

	/** \brief 캐릭터의 컬리젼 그룹을 설정합니다. */
	void			SetCollisionGroup( UINT16 group, UINT32 mask);

	/** \brief Capsule controller를 생성하고 Rough shape을 생성해서 캐릭터에 붙입니다.
		\param[in] pParentNode 붙여질 scene node
		\param[in] nColGroupIndex collision group index */
	void			AttachCollisionNode( i3Node * pParentNode, UINT32 nColGroupIndex);

	/** \brief Capsule controller를 제거하고 Rough shape을 제거한다. */
	void			DetachCollisionNode( void);

	/** \brief 접촉용 Shape 활성화 기능입니다. 
		\note 현재는 사용하지 않음. */
	void			SetContactShapeSetEnable( bool bVal);
	void			QueContactReport( const I3_PHYSIX_CONTACT_REPORT_PTR& spReport );
	void			ParseContactReport( void);
	void			ProcessCollisionEffect( void);
	void			ContactShapeSetUpdate( void);

	/** \brief Shape에 대한 부위를 반환합니다. */
	CHARA_HIT_PART	GetHitPart( i3PhysixShapeSet* pDestShapeSet);

	/** \brief 캐릭터 Capsule에 대한 Overlap여부를 판별합니다. */
	bool			IsOverlappedBy( UINT32 nGroupIdx);

	/** \brief CrossHair에 의한 Nick 표시를 Update합니다.
		\note 초당 20회 raycast를 수행합니다.		*/
	bool			UpdateNickOnTarget( void);

	
};

#endif
