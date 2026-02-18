/**
 * \file	GameCharaBase.h
 *
 * Declares the game chara base class.
 */


#if !defined( CGAME_CHARA_BASE_H)
#define CGAME_CHARA_BASE_H

#include "GameCharaDef.h"
#include "GameCharaTextureDef.h"
#include "GameCharaWeaponControlDef.h"
#include "CharaInfo.h"
#include "Character/GameCharaAnimControlDefine.h"

class CGameCharaActionContext;
class CGameCharaAnimContext;
class CGameCharaBoneContext;
class CGameCharaCameraContext;
class CGameCharaCollisionContext;
class CGameCharaEquipContext;
class CGameCharaInputContext;
class CGameCharaMaterialContext;
class CGameCharaMoveContext;
class CGameCharaSoundContext;
class CGameCharaWeaponContext;
class CGameCharaUpdateContext;
class WeaponBase;
class CGameObjectWithWeapon;
class CGameCharaInputControl;
class CAbstractWeaponContext;
class CGameCharaHUDContext;
class CGameCharaMultiWeaponContext;

#define MASTER_RELOAD_SPEED_RATIO 1.25f

typedef UINT32	CHARA_FLAG;
#define CHARA_FLAG_RESETABLE			0x0000FFFF		// 리스폰시 리셋되는 플레그값
#define CHARA_FLAG_DAMAGED_BY_HOWL		0x00000001				// Howl에의한 Damage를 입어 후유증을 가지고있음	(resetable)
#define CHARA_FLAG_DEATHBLOW			0x00000002				// DeathBlow 상태	(resetable)
#define CHARA_FLAG_HELM_PROTECT			0x00000004				// 헬멧 방어
#define CHARA_FLAG_ENABLE_EFFECT		0x00000008				// 이펙트 발생 유무	(resetable)
#define CHARA_FLAG_ON_OBJECT			0x00000020				// Object 탑승 유무 (resetable)
#define CHARA_FLAG_LEFT_HAND			0x80000000				// 왼손잡이
#define CHARA_FLAG_IS_FEMALE			0x40000000				// 여성 캐릭터


#pragma pack(push,4)

struct PHYSIX_SHAPESET_INFO
{
	INT32				_iBoneIndex = -1;
	INT32				_iParentShapeIdx = CHARA_HIT_UNKNOWN;
	REAL32				_rDistanceToParentBone = 0.0f;
	i3PhysixShapeSet *	_pShapeSet = nullptr;
};

#pragma pack(pop)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Class : CGameCharaBase
//	Desc: 기본적인 AI 상태, Chara State와 Animation 처리를 합니다.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CGameCharaBase : public i3Chara
{
	I3_CLASS_DEFINE( CGameCharaBase, i3Chara);

	friend class CGameCharaAnimContext;

	//--------------------------------------------------------------------------------------------------------//
	//													Variables
	//--------------------------------------------------------------------------------------------------------//
protected:
	bool				m_bReturnedInstance = false;
	CHARA_VIEW_TYPE		m_ViewType = CHARA_VIEW_3PV;			//	1 or 3인칭 애니메이션
	CHARA_VIEW_TYPE		m_oldViewType = CHARA_VIEW_3PV;

	UINT32				m_uStateMaskKey = 0;
	UINT32				m_EnableStateMask = 0;	// 캐릭터가 할 수 없는 상태들을 저장합니다.
	CHARA_FLAG			m_CharaFlag = 0;

	// Action Variables
	VAR_LR(REAL32, 7)				m_fJumpDelayElapsedTime = 0.0f;		// 점프 딜레이 타임
	VAR_LR(REAL32, 8)				m_rRespawnElapseTime = 0.0f;			// 리스폰 타임 계산용
	VAR_LR(REAL32, 9)				m_rInvincibleElapseTime = 0.0f;		// 무적 타임
	VAR_LR(REAL32, 10)				m_rContactTime = 0.0f;					// Physix를 끄기 위한 타이머입니다. contact가 발생하면 계산을 합니다.
	REAL32				m_rAttackIdleTime = 0.0f;				// Attack 후 Idle까지 시간 - 왼쪽
	REAL32				m_rAttackIdleTimeLeft = 0.0f;			// Attack 후 Idle까지 시간 - 듀얼무기에 필요
	REAL32				m_rPendingActionTime = 0.0f;			// 빠른 키반복으로인한 애니메이션 튀는현상방지용
	
	
	VAR_LR(GLOBAL_CHARA_INFO*, 11)		m_pCharaInfo = nullptr;		//	게임중 변경되는 실제 캐릭터 값
	VAR_LR(CCharaInfo*, 12)		 	m_pDBInfo = nullptr;			//	변경되지 않는 캐릭터 데이터 베이스
	CGameCharaMultiWeaponContext *	m_pMultiWeaponContext = nullptr;		// MultiWeapon Context
	VAR_LR(CGameCharaUpdateContext*, 13)			m_pUpdateContext = nullptr;		// Update Context
	VAR_LR(CGameCharaInputContext*,  14)			m_pInputContext = nullptr;		// Input Context
	VAR_LR(CGameCharaActionContext *, 15)			m_pActionContext = nullptr;		// Action Context
	VAR_LR(CGameCharaAnimContext *, 16)			m_pAnimContext = nullptr;			// Anim Context
	VAR_LR(CGameCharaWeaponContext *, 17)			m_pWeaponContext = nullptr;		// Weapon Context
	VAR_LR(CGameCharaMoveContext *, 18)			m_pMoveContext = nullptr;			// Move Context
	VAR_LR(CGameCharaMaterialContext *, 19)		m_pMaterialContext = nullptr;		// Volume Light context
	VAR_LR(CGameCharaEquipContext *, 20)			m_pEquipContext = nullptr;		// Equip Context
	VAR_LR(CGameCharaCameraContext *, 21)			m_pCameraContext = nullptr;		// Camera Context
	VAR_LR(CGameCharaBoneContext *, 22)			m_pBoneContext = nullptr;			// Bone Context
	VAR_LR(CGameCharaSoundContext *, 23)			m_pSoundContext = nullptr;		// Sound Context
	VAR_LR(CGameCharaCollisionContext *, 24)		m_pCollisionContext = nullptr;	// Collision Context
	VAR_LR(CGameCharaHUDContext *, 25)				m_pHUDContext = nullptr;
	
	REAL32				m_rDamageHowlEffectiveTime = 0.0f;		// Howl에의한 Damage후유증.
	REAL32				m_rElapsedDeathBlow = 0.0f;			// DeathBlow 상태

	// 물리 설정값
	PHYSIX_SHAPESET_INFO	m_PhysixShapeSetInfo[ CHARA_HIT_MAXCOUNT];
 
	// 타고 있는 오브젝트
	i3Object *			m_pAttachedObject = nullptr;

	// 드랍무기에 대한 쿨타임(이시간동안은 발사를 못합니다.)
	REAL32				m_rDropWeaponTimer = 0.0f;

	REAL32				m_rMultiWeaponTimer = 0.0f;

	// 마비 설정값
	REAL32				m_rJumpParalysis = 0.0f;
	REAL32				m_rWalkParalysis = 0.0f;
	REAL32				m_rRunParalysis = 0.0f;
	REAL32				m_rCrouchParalysis = 0.0f;

	bool				m_bLoadMagChg = true;

private:
	VAR_LR(UINT32, 234) m_CharaStateVal[CHARA_STATE_MAX] = { 0 };
	VAR_LR(UINT32, 235) m_CharaStateKey[CHARA_STATE_MAX] = { 0 };


	void _ResetCharaStateMask(void)
	{
		for ( int i = 0 ; i < CHARA_STATE_MAX ; i ++ )
		{
			m_CharaStateKey[i] = ( i3Math::Rand() % (UINT_MAX - 1) ) + 1;
			m_CharaStateVal[i] = 0;
		}
	}

	UINT32	GetCharaStateKey(INT32 Idx) const
	{
		return m_CharaStateKey[Idx];
	}

public:

	/** \brief 캐릭터 상태 마스크에 flag를 추가합니다. */
	void				addCharaStateMask( UINT32 StateIdx)
	{
		m_CharaStateVal[StateIdx] = GetCharaStateKey(StateIdx);
	}

	/** \brief 캐릭터 상태 마스크에서 flag를 지웁니다. */
	void				removeCharaStateMask( UINT32 StateIdx)
	{
		m_CharaStateVal[StateIdx] = 0;
	}

	/** \brief 캐릭터 상태 마스크를 확인합니다. */
	/* 인덱싱 처리로 바뀌었으므로 동시에 두가지 이상의 마스크 처리 불가능합니다 */
	bool				isCharaStateMask( UINT32 StateIdx) const
	{
		I3ASSERT_RETURN(StateIdx < CHARA_STATE_MAX, false);
			
		return m_CharaStateVal[StateIdx] == GetCharaStateKey(StateIdx);
	}
	//------------------------------------------------------------------------------------------------------------//
	//													Functions
	//------------------------------------------------------------------------------------------------------------//
protected:
	// Physix
	/** \brief Actor간에 Pair를 설정합니다. */
	void				_PHYSIX_SetActorPairs( i3PhysixShapeSet * pShapeSet, UINT32 nFlag );

	/** \brief 기본 물리값을 설정합니다.
		\note damp, spring, limit값 */
	void				_PHYSIX_SetDefaultVariable( void );

	/** \brief 맞은 부위에 대한 Linear velocity를 설정합니다. */
	void				_PHYSIX_SetLinearVelocity( CHARA_DAMAGEINFO * pInfo );

	/** \brief Actor pair를 초기화합니다. */
	void				_PHYSIX_InitActorPairs( void);

	/** \brief 피격된 거리를 구합니다. */
	REAL32				_CalcDamageLength( const CHARA_DAMAGEINFO * pDamageInfo);

	/** \brief 거리에 따른 DamageRate를 구합니다. */
	REAL32				_CalcDistanceRate( CHARA_DAMAGEINFO * pDamageInfo, REAL32 rDamagedLength, CGameCharaBase* pKiller);

	/** \brief helmet protection을 검사합니다. */
	void				_CheckHelmetProtection( CHARA_DAMAGEINFO * pDamageInfo, bool &bHelmetDrop, bool &bDamaged);
	
	/** \brief Howl에 대한 Damage를 Update합니다. */
	void				_UpdateDamagedByHowl( REAL32 rDeltaSeconds);

	/** \brief DeathBlow에 대한 Update를 합니다. */
	void				_UpdateDeathBlowState( REAL32 rDeltaSeconds);

	/** \brief 피격 사운드를 Play합니다. */
	void				_PlaySoundForCharaHit( CHARA_DAMAGEINFO * pDamageInfo, VEC3D * pPos, bool bListener);

	/** \brief AIContext를 Update합니다. */
	void				_UpdateOnlyAI( REAL32 tm);

	/** \brief AIContext 외에 Update합니다. */
	void				_UpdateAllExceptAI( REAL32 tm);

	/**\brief CharaResType에 따라 Control을 설정한다. */
	void				_SetControlsByCharaType( void);

	const char *		_GetWeaponBodyUpperName( CHARA_ACTION_BODYUPPER bodyUpper, const CWeaponInfo * pWeaponInfo);


	void				_PrepareCommonAI( void);

	/** \brief 상체 Bone Count */
	virtual INT32		_GetUpperBoneCount( void) const		{ return CHARA_UPPER_BONE_COUNT; }

	/** \brief 하체 Bone Count */
	virtual INT32		_GetLowerBoneCount( void) const		{ return CHARA_LOWER_BONE_COUNT; }

	void				_PresetResName( void);

	/** \brief 낙하 데미지시의 사운드 */
	virtual void		_PlayDownfallSound( void);


public:
	bool				isReturnedInstance( void) const				{ return m_bReturnedInstance; }

	void				addCharaFlag( CHARA_FLAG flag)				{ m_CharaFlag |= flag; }
	void				removeCharaFlag( CHARA_FLAG flag)			{ m_CharaFlag &= ~flag; }
	bool				isCharaFlag( CHARA_FLAG flag) const			{ return (m_CharaFlag & flag) == flag; }

	/** \brief 왼손잡이 모드를 확인합니다. */
	bool				isLeftHand(void) const						{ return isCharaFlag( CHARA_FLAG_LEFT_HAND); }

	/** \brief 여성 캐릭터 유무를 확인합니다. */
	// 해당 플래그로 판별하는 여자 남자는 전부 본, 컬리젼, 애니메이션에 관련된것만 사용해야한다.
	bool				isFemale( void) const						{ return isCharaFlag(CHARA_FLAG_IS_FEMALE); }	

	void				AddOnObjectState();
	void				RemoveOnObjectState();

	/** \brief 캐릭터 가능 상태 마스크를 반환합니다. */
	UINT32				getEnableStateMask( void) const				{ return m_EnableStateMask; }

	/** \brief 캐릭터 가능 상태 마스크에 flag를 추가합니다. */
	void				addEnableStateMask( UINT32 mask);	
	
	/** \brief 캐릭터 가능 상태 마스크에서 flag를 지웁니다. */
	void				removeEnableStateMask( UINT32 mask);
	
	/** \brief 캐릭터 가능 상태 마스크를 확인합니다. */
	bool				isEnableStateMask( UINT32 mask) const		{ return (m_EnableStateMask & mask) != 0; }
	
	// 1인칭 뷰 체크(네트워크 캐릭터도 될 수 있다, 관전)
	// 1인칭 뷰라면 반듯이 FPS모드입니다. 이것을 어긴다면 버그가 됩니다.
	bool				is1PV( void) const							{ return (m_ViewType == CHARA_VIEW_1PV); }
	bool				is3PV( void) const							{ return (m_ViewType == CHARA_VIEW_3PV); }

	/** \brief 왼손잡이 모드를 설정합니다. */
	void				setLeftHand( bool bFlag);

	REAL32				getJumpDelayElapsedTime( void) const		{ return m_fJumpDelayElapsedTime; }
	void				setJumpDelayElapsedTime( REAL32 tm)			{ m_fJumpDelayElapsedTime = tm; }
	
	/** \brief 진행된 무적 시간을 반환합니다. */
	REAL32				getInvincibleTime( void) const				{ return m_rInvincibleElapseTime; }

	/** \brief 무적 시간을 설정합니다. */
	void				setInvincibleTime( REAL32 tm)				{ m_rInvincibleElapseTime = tm; }

	/** \brief 무적 모드인지 확인합니다. */
	bool				isInvincible( void) const					{ return (m_rInvincibleElapseTime > 0.0f);		}

	const CCharaInfo *			getDBInfo( void) const						{ return m_pDBInfo;					}
	const GLOBAL_CHARA_INFO *	getCharaInfo(void) const					{ return m_pCharaInfo;				}
	void						setCharaInfo( GLOBAL_CHARA_INFO * pInfo)	{ m_pCharaInfo = pInfo; }
	GLOBAL_CHARA_INFO *			setCharaInfo( void)							{ return m_pCharaInfo; }

	/** \brief Team을 반환합니다. */
	CHARACTER_TEAM_TYPE		getCharaTeam( void) const				{ I3ASSERT_RETURN( m_pCharaInfo != nullptr, CHARACTER_TEAM_NONE); return m_pCharaInfo->GetTeam(); }

	TEAM_TYPE			getTeam( void) const
	{
		if( getCharaTeam() & CHARACTER_TEAM_RED)	return TEAM_RED;
		return TEAM_BLUE;
	}

	/** \brief Network Index를 반환합니다. */
	INT32				getCharaNetIndex( void) const				{ I3ASSERT_RETURN( m_pCharaInfo != nullptr, -1); return (INT32) m_pCharaInfo->GetNetIdx();}

	/** \brief 현재 HP를 반환합니다. */
	INT32				getCurHP( void) const						{ I3ASSERT_RETURN( m_pCharaInfo != nullptr, 0 ); return (INT32)(m_pCharaInfo->GetHP()); }

	/** \brief 캐쉬 아이템이 적용된 현재 HP를 반환합니다 */
	INT32				getCurHPAppliedCashItem( void) const;

	/** \brief 현재 HP를 반환합니다. */
	REAL32				getCurHP_Real( void) const					{ I3ASSERT_RETURN( m_pCharaInfo != nullptr, 0.f ); return (m_pCharaInfo->GetHP()); }

	/** \brief 현재 HP를 100%기준으로 반환합니다. */
	INT32				getCurHP_Percent( void) const;

	/** \brief 총 HP를 반환합니다. */
	INT32				getFullHP( void) const;
	

	/** \brief 총 HP를 설정합니다. */
	void				setFullHP( void)					{ I3ASSERT_RETURN( m_pCharaInfo != nullptr );	m_pCharaInfo->SetHP((REAL32) getFullHP());	}

	
	// 캐릭터 종류 체크
	bool				isAIToClient( void) const			{ I3ASSERT( m_pCharaInfo != nullptr ); if( m_pCharaInfo != nullptr) return m_pCharaInfo->GetAIToClient(); return false;}
	bool				isAI( void) const					{ I3ASSERT( m_pCharaInfo != nullptr ); if( m_pCharaInfo != nullptr) return (m_pCharaInfo->GetType() == CHARA::TYPE_AI); return false; }
	bool				isLocal( void) const				{ I3ASSERT( m_pCharaInfo != nullptr ); if( m_pCharaInfo != nullptr) 	return (m_pCharaInfo->GetType() == CHARA::TYPE_PLAYER); return false;}
	bool				isNetwork( void) const				{ I3ASSERT( m_pCharaInfo != nullptr ); if( m_pCharaInfo != nullptr) return (m_pCharaInfo->GetType() == CHARA::TYPE_PLAYER_NET) || (m_pCharaInfo->GetType() == CHARA::TYPE_DOMI_FULLBODY_AI); return false;	}
	bool				isNPC( void) const					{ I3ASSERT( m_pCharaInfo != nullptr ); if( m_pCharaInfo != nullptr) return (m_pCharaInfo->GetType() == CHARA::TYPE_NPC); return false;}
	bool				isProfile( void) const				{ I3ASSERT( m_pCharaInfo != nullptr ); if( m_pCharaInfo != nullptr) return (m_pCharaInfo->GetType() == CHARA::TYPE_PROFILE); return false;}
	bool				is1PVProfile( void) const			{ I3ASSERT( m_pCharaInfo != nullptr ); if( m_pCharaInfo != nullptr) return (m_pCharaInfo->GetType() == CHARA::TYPE_FIRST_VIEW_PLAYER); return false; }
	
	/** \brief 물리 충돌 후 시간을 반환합니다.*/
	REAL32				getContactTime( void) const			{ return m_rContactTime; }

	/** \brief 물리 충돌 후 시간을 설정합니다. */
	void				setContactTime( REAL32 tm)			{ m_rContactTime = tm; }

	/** \brief 캐릭터의 물리 설정 정보를 반환합니다. */
	const PHYSIX_SHAPESET_INFO * getPhysixShapeSetInfo( INT32 part) const	{ I3_BOUNDCHK( part, CHARA_HIT_MAXCOUNT); return &m_PhysixShapeSetInfo[ part]; }
	PHYSIX_SHAPESET_INFO * setPhysixShapeSetInfo( INT32 part)				{ I3_BOUNDCHK( part, CHARA_HIT_MAXCOUNT); return &m_PhysixShapeSetInfo[ part]; }

	/** \brief 공격 자세 유지 시간을 반환합니다. */
	REAL32				getAttackIdleTime( void) const			{ return m_rAttackIdleTime; }
	REAL32				getAttackIdleTimeLeft( void) const		{ return m_rAttackIdleTimeLeft; }

	/** \brief 공격 자세 유지 시간을 설정합니다. */
	void				setAttackIdleTime( REAL32 tm)			{ m_rAttackIdleTime = tm; }
	void				setAttackIdleTimeLeft( REAL32 tm)		{ m_rAttackIdleTimeLeft	= tm; }
	
	REAL32				getActionPendingTime(void) const		{ return m_rPendingActionTime;}
	void				setActionPendingTime( REAL32 tm)		{ m_rPendingActionTime = tm;}

	/** \brief 리스폰 시간을 반환합니다. */
	REAL32				getRespawnElapseTime( void) const		{ return m_rRespawnElapseTime; }

	/** \brief 리스폰 시간을 설정합니다. */
	void				setRespawnElapseTime( REAL32 tm)		{ m_rRespawnElapseTime = tm; }

	/** \brief 피격 지속 시간을 반환합니다. */
	REAL32				getDamageElapseTime( void ) const;
	REAL32				getDeathTime(void) const;

	/** \brief character buffer type을 반환 합니다.. */
	CHARA::BUFFER_TYPE	getBufferType( void) const					{ I3ASSERT_RETURN( m_pCharaInfo != nullptr, CHARA::BUFFER_USER); return CHARA::Type2BufferType( m_pCharaInfo->GetType());}

	/** \brief 타고 있는 오브젝트를 반환합니다. */
	i3Object *			getAttachedObject( void) const				{ return m_pAttachedObject; }

	/** \brief 타고 있는 오브젝트를 설정합니다. */
	void				setAttachedObject( i3Object * pObj)			{ m_pAttachedObject = pObj; }

	/** \brief 무기 버리고 지속 시간을 반환합니다.
		\note 해당 시간동안 줍기를 할 수 없습니다. */
	REAL32				getDropWeaponTimer( void) const				{ return m_rDropWeaponTimer; }

	/** \brief 다중무기 HUD를 끄고 지속 시간을 반환합니다.
		\note 해당 시간동안 발사를 할 수 없습니다. */
	REAL32				getMultiWeaponTimer( void) const			{ return m_rMultiWeaponTimer; }
	void				setMultiWeaponTimer( REAL32 time)			{ m_rMultiWeaponTimer = time; }

	/** \brief 무기 버리고 지속 시간을 설정합니다. */
	void				setDropWeaponTimer( REAL32 tm)				{ m_rDropWeaponTimer = tm; }

	bool				getDamagedByHowl(void) const				{ return isCharaFlag( CHARA_FLAG_DAMAGED_BY_HOWL); }
	void				setDamagedByHowl(bool bDamaged)				{ bDamaged?addCharaFlag(CHARA_FLAG_DAMAGED_BY_HOWL):removeCharaFlag(CHARA_FLAG_DAMAGED_BY_HOWL); }
	void				setDamagedByHowlTime( REAL32 time)			{ m_rDamageHowlEffectiveTime = time;}

	void				SetDeathblowState(bool bEnable);
	bool				isDeathblowState(void) const				{ return isCharaFlag( CHARA_FLAG_DEATHBLOW); }

	bool				isAttachedProtectParts(void);
	EQUIP::ePART		getProtectPartsType(void);

	/** \brief 목표 추적 기능 사용을 확인합니다. */
	bool				isEnableTargetChase( void) const;
	REAL32				getTargetChaseRange( void) const;	// 목표 추적 거리

	// revision 53646 HK417 경직기능 추가
	REAL32				getJumpParalysis(void) const				{ return m_rJumpParalysis;	}
	REAL32				getWalkParalysis(void) const				{ return m_rWalkParalysis;	}
	REAL32				getRunParalysis(void) const					{ return m_rRunParalysis;	}
	REAL32				getCrouchParalysis(void) const				{ return m_rCrouchParalysis; }

	CGameCharaInputContext *		getInputContext( void) const	{ return m_pInputContext; }
	CGameCharaWeaponContext*		getWeaponContext( void) const	{ return m_pWeaponContext; }
	CGameCharaUpdateContext *		getUpdateContext( void) const	{ return m_pUpdateContext; }
	CGameCharaActionContext *		getActionContext( void) const	{ return m_pActionContext; }
	CGameCharaAnimContext *			getAnimContext( void) const		{ return m_pAnimContext; }
	CGameCharaMoveContext *			getMoveContext( void) const		{ return m_pMoveContext; }
	CGameCharaMaterialContext *		getMaterialContext( void) const	{ return m_pMaterialContext; }
	CGameCharaEquipContext *		getEquipContext( void) const	{ return m_pEquipContext; }
	CGameCharaCameraContext *		getCameraContext( void) const	{ return m_pCameraContext; }
	CGameCharaBoneContext *			getBoneContext( void) const		{ return m_pBoneContext; }
	CGameCharaSoundContext *		getSoundContext( void) const	{ return m_pSoundContext; }
	CGameCharaCollisionContext *	getCollisionContext( void) const{ return m_pCollisionContext; }
	CGameCharaHUDContext *			getHUDContext( void) const		{ return m_pHUDContext; }
	CGameCharaMultiWeaponContext *	getMultiWeaponContext( void) const { return m_pMultiWeaponContext; }
	
	void ToggleRenderBone() {}

private:
	/** \brief 무기 및 장비를 생성한다.
		\desc 주) MainThread에서만 호출되야 한다. */
	void				__CreateEquipment( bool bInBattle, bool bLoadWeapon);

	/** \brief Resource Instance를 생성합니다.
		\desc PhysX Shape을 이때에 생성한다. 이전에 데이터를 설정해야함.
			주) MainThread에서만 호출되야 한다. */
	void				__Instantiate( void);
	
public:
	CGameCharaBase( void);
	virtual ~CGameCharaBase( void);

	virtual void		Create( GLOBAL_CHARA_INFO * pCharaInfo, const CCharaInfo * pDBInfo);

	/** \brief 로딩 전 생성 함수 */
	virtual void		CreateFirst( const CCharaInfo * pDBInfo, bool bInLobby = false);

	/** \brief 로딩 후 생성 함수(InMain) */
	virtual void		CreateLater( const CCharaInfo * pDBInfo, bool bLoadWeapon = true);

	void				CreateInLobby( const CCharaInfo * pDBInfo);
	void				CreateLaterInLobby( void);
	
	virtual void		Destroy( void);
	virtual void		OnInitVariable();
	virtual void		OnUpdate( REAL32 tm) override;

	/** \brief AIState가 변경되면 호출됩니다.
		\see i3Chara::OnChangeAIState */
	virtual void		OnChangeAIState( i3AIContext * pCtx);
	
	/** \brief 리소스를 bind합니다.
		\note 캐릭터의 Bone에 대한 List를 작성합니다.
		\see i3Chara::OnBindResource */
	virtual void		OnBindResource( void) override;

	/** \brief 캐릭터 Instance를 반환합니다.
		\note 리소스에 관해서 반환이 필요한 경우 이 함수에서 처리되야 함.
			예) 무기, 장비 */
	virtual void		ReturnInstance( void) override;

	/** \brief 캐릭터 정보를 초기화합니다.
		\note Respawn시 호출됩니다. */
	virtual void		ResetChara( void);	

	/** \brief View Type을 반환합니다. */
	CHARA_VIEW_TYPE		getViewType( void)						{ return m_ViewType; }

	/** \brief 이전 View Type을 반환합니다.
		\note Cmd_ChangeView를 호출하면 현재 상태 값이 저장됩니다. */
	CHARA_VIEW_TYPE		getOldViewType( void)					{ return m_oldViewType; }

	/** \brief View Controller를 변경합니다. */
	void				ChangeControl_View( CHARA_VIEW_TYPE viewType );

	/** \brief Loca Controller로 변경합니다. */
	void				ChangeControl_Local( void);

	/** \brief Network Controller로 변경합니다. */
	void				ChangeControl_Network( void);

	void				ChangeControl_Fullbody( void);

	/** \brief NPC Controller로 변경합니다. */
	void				ChangeControl_NPC( void);

	/** \brief Profile Controller로 변경합니다. */
	void				ChangeControl_Profile( void);

	/** \brief AI Controller로 변경합니다. */
	void				ChangeControl_AI( void);

	/** \brief 정지 */
	void				MoveStop( void);

	/** \brief 강제로 위치를 지정합니다 */
	void				forceSetPos( const VEC3D * pPos);
	
	/** \brief Event message를 처리합니다.
		\see i3GameObj::OnEvent */
	virtual bool		OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code) override;

	/** \brief 시간을 계산합니다. */
	void				ProcessTimer( REAL32 rDeltatime);

	/** \brief Profile용 View character를 설정합니다. */
	void				SetViewerChara( bool bEnable = true);

	/** \brief Helmet 관통을 계산합니다. */
	bool				CalcHelmetPenetrate( const CHARA_DAMAGEINFO * pDamageInfo ) const;

	/** \brief 피격 처리입니다. */
	INT32				OnHit( CHARA_DAMAGEINFO * pDamageInfo, CGameCharaBase* pKiller = nullptr);

	/** \brief LOD를 계산합니다. */
	void				calcLOD( VEC3D * pPos, VEC3D * pCamPos, REAL32 fFOVRatio = 1.0f);
	

	// 시뮬레이션 후 Update
	void				PostUpdate(REAL32 rDeltaTime);

	/** \brief Damage를 계산합니다. */
	INT32				CalcDamage( const CHARA_DAMAGEINFO * pDamageInfo, REAL32 rDamagedLength, CGameCharaBase* pKiller);

	/** \brief 부위에 따른 Damage를 계산합니다. */
	void				CalcDamageByPart( CHARA_DAMAGEINFO * pDamageInfo, REAL32& rDamage);

	virtual bool		NET_HitForHost( CHARA_DAMAGEINFO * pDamageInfo, REAL32 rDamage);
	void				NET_HitForClient( CHARA_DAMAGEINFO * pDamageInfo);
	void				NET_HitForClientNoHelmet( CHARA_DAMAGEINFO * pDamageInfo);

	/** \brief HP를 계산합니다. */
	void				CalcHitPoint( REAL32 rDamage, CHARA_DAMAGEINFO* pDamageInfo);


	// Medical Kit 추가  - 2011.06.21
	bool				RecoveryHP(CHARA_DAMAGEINFO* pDamageInfo);

	// 훈련전 AI 캐릭터
	void				BindAI(AI_BIND_DATA* pData);
	
	//--------------------------------------------------------------------------------------------------//
	//																									//
	//										Ability variables											//
	//																									//
	//	캐릭터 능력치에 관한 변수들입니다.
	//--------------------------------------------------------------------------------------------------//

	//-------------------------------------------------------------------------------------------------//
	// 헬멧 보호 무적 시간 - 박명진 2008. 7. 4
private:
	REAL32				m_rEndHelmProtectionTime = 0.0f;

public:
	REAL32				getHelmetProtectionTime( void)		{ return m_rEndHelmProtectionTime; }
	void				setHelmetProtectionTime( REAL32 tm)	{ m_rEndHelmProtectionTime = tm; }

	void				SetHelmProtectionFlag(bool bFlag)	{ bFlag?addCharaFlag(CHARA_FLAG_HELM_PROTECT):removeCharaFlag(CHARA_FLAG_HELM_PROTECT); }
	bool				IsHelmProtection()					{ return isCharaFlag(CHARA_FLAG_HELM_PROTECT); }
	void				SetHelmetProtection( bool bEnable);

	void				DropEquip( VEC3D * pDir = nullptr);
	void				HelmetEffect( CHARA_DAMAGEINFO * pInfo);

private:
	REAL32				m_rTotalAbility[ CHARA::eABILITY_COUNT];	// 아이템 + 장비 + 스킬로 인한 능력치 값	// 무기는 추가적으로 계산이 이루어 저야한다.
	REAL32				m_rMoveOffset = 0.0f;								// m_arMoveOffset은 제대로 쓰이지 않아서..그냥 값하나로 변경..호칭전용으로 해석한다..
		
	INT32				m_iFullHP = 0;						// 총 HP
	REAL32				m_rMovementSpeedRate[MST_COUNT][MDT_COUNT];	// 이동 속도량 % 
	REAL32				m_rDefenceRate[ CHARA::eDAMAGEPART_COUNT];	// 방어력 %

	REAL32				m_rAccelerationTimeOffset = 0.0f;
	REAL32				m_rAccelerationTimeOffset_RunSkill = 0.0f;
	
	REAL32				m_rFallDamageMinOffset = 0.0f;
private:

	void				__ResetDesigOffsetDatas(void);

	/** \brief 리스폰시 무적 시간 계산 */
	void				__ResetInvincibleTime( void);

	/** \brief 리스폰시 점프 높이력 계산 */
	void				__ResetJumpHeightRate( void);

	/** \brief 리스폰시 던지기 속력 계산 */
	void				__ResetThrowSpeed( void);

	/** \brief 리스폰시 총 HP량 계산 */
	void				__ResetFullHP( void);

	/** \brief 리스폰시 이동속도 계산 */
	void				__ResetMovementSpeed( void);

	/** \brief 리스폰시 방어력 계산*/
	void				__ResetDefenceRate( void);
	
	/** \brief 리스폰시 무기 교체 속도율 계산 */
	void				__ResetWeaponSwapTimeRate( void);

public:
	void				SetMoveSpeedAddOffset(REAL32 rMoveOffset);

	REAL32				getAbility( CHARA::eABILITY type)							{ return m_rTotalAbility[ type];	}

	void				SetAccelerationTimeOffset(REAL32 rAccelerationTimeOffset)	{ m_rAccelerationTimeOffset = rAccelerationTimeOffset; }
	
	REAL32				GetMoveSpeedAddOffset(EMoveSpeedType eMST, EMoveDirectionType eMDT) const;
	REAL32				GetJumpDelay() const						{ I3ASSERT_RETURN(m_pDBInfo != nullptr, 0.0f ); return m_pDBInfo->GetJumpDelay(); }
	REAL32				GetAccelerationTimeAddOffset() const;

	REAL32				GetDamageRateByDefence(CHARA_HIT_PART hitpart = CHARA_HIT_UNKNOWN) const;
	REAL32				GetHelmetProtectionRate() const;
	REAL32				GetWeaponSwapTimeScale( void) const;

	virtual void				HP_Recovery(REAL32 rRecoveryHP = 0.0f) { I3ASSERT_0; }
	REAL32						GetDinoDamage(REAL32 rDamage);
	REAL32						GetDinoFullHP();

	virtual REAL32				GetDinoSkillDuration(INT32 idx) const		{ return 0.0f; }
	virtual REAL32				GetDinoSkillCoolTime(INT32 idx) const		{ return 0.0f; }
	virtual REAL32				GetDinoSkillMovementRate(INT32 idx) const	{ return 0.0f; }

	// 낙하 데미지 데이터 관련.
	INT32				GetFallMinDamage() const;
	REAL32				GetFallDamageMinHeight() const;

	void				SetFallDamageMinOffset(REAL32 rOffset) { m_rFallDamageMinOffset = rOffset; }
	REAL32				GetFallDamageMinOffset() const { return m_rFallDamageMinOffset; }

	REAL32				GetFallDamageSlope() const;

	/** \brief 투척무기 속도 */
	REAL32		GetThrowSpeed( WEAPON_SLOT_TYPE nSlot );
	REAL32		GetExThrowSpeed(WEAPON_SLOT_TYPE nSlot);

	//크리티컬 데미지

	INT32				m_hitCount;
	INT32				GetHitCount()				{ return m_hitCount; }
	void				SetHitCount(INT32 val)		{ m_hitCount = val;  }
	

private:
	REAL32				m_rJumpHeightOffset = 0.0f;

public:
	void				SetJumpHeightOffset( REAL32 offset)				{ m_rJumpHeightOffset = offset; }
	REAL32				GetJumpHeightOffset() const						{ return m_rJumpHeightOffset; }

	//--------------------------------------------------------------------------------------------------//
	//				오브젝트 무기 관련
	//--------------------------------------------------------------------------------------------------//
public:
	VEC3D m_vSavedPos;
	CGameObjectWithWeapon* m_pObjectForControlWeapon = nullptr;

public:
	void				OnObject(CGameObjectWithWeapon* pObj);
	void				OffObject(); 
	bool				IsOnObject() { return isCharaFlag( CHARA_FLAG_ON_OBJECT); }

	CGameObjectWithWeapon*			GetOnObject() { return m_pObjectForControlWeapon; }
	void				SaveNowPos();
	VEC3D*				LoadSavedPos();


	//--------------------------------------------------------------------------------------------------//
	//		클랜 마크 관련
	//--------------------------------------------------------------------------------------------------//
private:
	INT32				m_iClanMark = 0;
	i3Texture*			m_pClanMarkTexture = nullptr;
	i3Texture*			m_pPBMarkTexture = nullptr;
	i3TextureBindAttr*  m_arClanMarkTexBindAttr[AVT_LOD_COUNT];
	i3AttrSet*			m_arClanMarkAttrSet[AVT_LOD_COUNT];
	
public:
	static i3BlendEnableAttr*	m_spBlendEnableAttr;
	static i3BlendModeAttr*		m_spBlendModeAttr;

	void				CharaChangeClanMark(UINT32 iClanMark);
	bool				IsPBMark(UINT32 uiMark);
	void				InitClanMark();
	void				CreateClanMarkTex();

	//--------------------------------------------------------------------------------------------------//
	//		공룡 관련
	//--------------------------------------------------------------------------------------------------//
	bool				IsDino() const					{ if(m_pCharaInfo != nullptr) return (m_pCharaInfo->IsDino() || m_pCharaInfo->IsDomiDino()); else return false; }

	 //렙터 기반 공룡들 (스팅,랩터)
	bool				IsRaptorBase() const			{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsRaptorBase();	else return false; }
	bool				IsRaptor() const				{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsRaptor();	else return false; }
	bool				IsTank() const					{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsTank();	else return false; }
	bool				IsAcid() const					{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsAcid();	else return false; }
	bool				IsElite() const					{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsElite();	else return false; }
	bool				IsSting() const					{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsSting();	else return false; }
	bool				IsRex() const					{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsRex();	else return false; }
	bool				IsCCRaptor() const				{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsCCRaptor(); else return false; }
	bool				IsCCSting() const				{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsCCSting(); else return false; }
	bool				IsCCAcid() const				{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsCCAcid(); else return false; }
	bool				IsRaptorMercury() const			{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsRaptorMercury(); else return false; }
	bool				IsStingMars() const				{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsStingMars(); else return false; }
	bool				IsAcidVulcan() const			{ if(m_pCharaInfo != nullptr) return m_pCharaInfo->IsAcidVulcan(); else return false; }
#ifdef DOMI_DINO
	bool				IsDomiDino() const				{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiDino(); else return false;			}
	bool				IsDomiRaptor() const			{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiRaptor(); else return false;			}
	bool				IsDomiRaptorGreen() const		{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiRaptorGreen(); else return false;	}
	bool				IsDomiRaptorBlue() const		{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiRaptorBlue(); else return false;		}
	bool				IsDomiAcid() const				{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiAcid(); else return false;			}
	bool				IsDomiAcidNotBomb() const		{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiAcidNotBomb(); else return false;	}
	bool				IsDomiAcidItem() const			{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiAcidItem(); else return false;	}
	bool				IsDomiSting() const				{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiSting(); else return false;			}
	bool				IsDomiMutantRex() const			{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiMutantRex(); else return false;		}
	bool				IsDomiTank() const				{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiTank(); else return false;			}
	bool				IsDomiRaptorBoneBase() const	{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiRaptorBoneBase(); else return false; }			
	bool				IsDomiRaptorMercury() const 	{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiRaptorMercury(); else return false;	}
	bool				IsDomiStingMars() const 		{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiStingMars(); else return false;		}
	bool				IsDomiAcidVulcan() const		{ if (m_pCharaInfo) return m_pCharaInfo->IsDomiAcidVulcan(); else return false;		}
#endif
	bool				IsControlledDino() const	{ return ( IsDino() && (isLocal() || isAI()) ); }

	//
	virtual void		setHpTagMaxHp( REAL32 max) {}
	virtual REAL32		getHpTagMaxHp() const { return 0.f;}


	virtual WeaponBase*			GetCharaWeapon( WEAPON_SLOT_TYPE slot) const;
	virtual WeaponBase*			GetCurrentCharaWeapon() const;
	virtual void				SetDinoWeaponType( WEAPON::FIRE_ORDER eType) { I3ASSERT_0; }

	
	//--------------------------------------------------------------------------------------------------//
	//		Battle Use Item Use 관련
	//--------------------------------------------------------------------------------------------------//
	void				SetGravityRecovery(bool Enable);
	

	//////////////////////////////////////////////////////////////////////////////////////
	//									Action	Interface								//
	//////////////////////////////////////////////////////////////////////////////////////

public:
	// 죽음 처리 및 패킷 전송 - 박명진 2008. 7. 23
	void				DeathCharaAndWritePacket( const CHARA_DAMAGEINFO * pDamageInfo);
	void				DeathCharaByFastObject(VEC3D* pDir);
public:
	bool			isLoadMagChg()							{ return m_bLoadMagChg;}
	void			SetLoadMagChg(bool bMagChg)				{ m_bLoadMagChg = bMagChg;}
public:
	/** \brief Crosshair를 설정합니다. */
	bool		Cmd_ApplyCrosshair( void);

	/* \brief 공격 동작 실행
	\param[in] fireOrder 공격 타입
	\param[in] pPos 현재 위치
	\param[in] pTargetPos 목표 위치
	\return bool	*/
	bool		Cmd_Attack( WEAPON::FIRE_ORDER fireOrder = WEAPON::PRIMARY_FIRE,
		VEC3D * pPos = nullptr, VEC3D * pTarget = nullptr, bool bOnlyAction = false, INT32 iSlot = WEAPON_SLOT_PRIMARY);

	/** \brief 1/3인칭 뷰를 변경한다.
	\param[in] type CHARA_VIEW_TYPE
	*/
	void		Cmd_ChangeView( CHARA_VIEW_TYPE type, bool bPlayAnimSync = true);

	/** \brief 무기 슬롯 변경
		\param[in] slot 무기 슬롯 번호	*/
	bool		Cmd_ChangeWeapon( WEAPON_SLOT_TYPE slot, bool bDirect = false);
	
	/** \brief 장비를 생성한다. */
	void		Cmd_CreateEquip( CHARA_RES_ID charaResType, T_ItemID itemID, T_ItemID subitemID = 0);

	/** \brief 장비를 제거한다. */
	void		Cmd_DeleteEquip( EQUIP::ePART part);

	/** \brief 무기를 생성한다. */
	bool		Cmd_CreateWeapon( WEAPON_SLOT_TYPE slot, T_ItemID Itemid);

	// 무기 생성(무기변경 없음)
	bool		Cmd_CreateWeapon_Only( WEAPON_SLOT_TYPE slot, T_ItemID Itemid);

	// 앉기 액션 실행
	bool		Cmd_Crouch( void);

	// Damage 동작 실행
	void		Cmd_Damage( CHARA_DAMAGEINFO * pInfo);

	/** \brief 자신에게 Damage를 받는 경우
		\note falling damage
		\param[in] nDamage Damage 양
		\param[in] iObjectIdx 오브젝트에 의한 Damage이면 오브젝트 Idx 0xFFFF이면 오브젝트가 아님
		\param[in] bFalling 낙하 유무		*/
	void		Cmd_DamageSelf( INT32 nDamage, UINT16 iObjectIdx = 0xFFFF, bool bFalling = false);


	/** \brief Death 상태로 변경한다. */
	void		Cmd_Death( INT32 iShooterIdx, CHARA_DEATH_TYPE type, VEC3D * pHitDir);

	/** \brief 해당 슬롯의 무기를 삭제한다. */
	void		Cmd_DeleteWeapon( WEAPON_SLOT_TYPE slot);

	/** \brief 해당 슬롯의 무기를 띄어낸다. */
	void		Cmd_DetachWeapon( WEAPON_SLOT_TYPE slot);

	/** \brief 무기를 캐릭터에서 띄어냅니다. 
		\note 등이나 권총집에 있는 경우 자동으로 띄어낸다. 
	*/
	void		Cmd_DetachWeapon( WeaponBase * pWeapon);

	/** \brief 장비를 월드로 버린다. */
	bool		Cmd_DropEquip( EQUIP::ePART equipType, VEC3D * pPos);

	/** \brief 무기를 월드로 버린다.
		\param[in] iSlot 슬롯 번호
		\param[in] pPos 위치vector
		\param[in] bNextWeapon 다음 무기로 자동 교체 여부
				false 이면 이전 무기로 교체된다. */
	bool		Cmd_DropWeapon( WEAPON_SLOT_TYPE iSlot, VEC3D * pPos, bool bNextWeapon = true, bool bDirectPickup = false);

	// TeamTag 활성 유무를 설정한다.
	void		Cmd_Enable_TeamTag( bool bEnable);

	// Buff Tag 활성 유무를 설정한다.
	void		Cmd_Enable_Buff( INT32 bAttack, INT32 bSpeed);

	/** \brief 무기 Extension을 활성화한다.	*/
	bool		Cmd_Extension( bool bEnable);

	/** \brief Hit Effect를 뿌립니다. */
	bool		Cmd_HitEffect( CHARA_DAMAGEINFO * pDamageInfo);

	/** \brief 캐릭터 상태 및 Weapon을 Idle로 변경한다. */
	void		Cmd_Idle( void);

	/** \brief 캐릭터 하체만 Idle로 변경 */
	void		Cmd_Idle_Lower( void);

	/** \brief 캐릭터 상체만 idle로 변경 */
	void		Cmd_Idle_Upper( void);

	// revision 29547
	/// 상체가 Move일때만 쓰임 idle로 변경. 듀얼핸드건의 싱크때문에 쓰인다
	void		Cmd_Idle_Upper_Move( void);

	/** \brief Camera를 Idle로 변경 */
	void		Cmd_Idle_Camera( void);

	/** \brief 폭탄 설치
		\param[in] bEnable 설치 여부 false이면 취소한다.	*/
	bool		Cmd_Install( bool bEnable = true);

	/** 겨울 모드 미션무기 설치
		\param[in] bEnable 설치 여부 false이면 취소한다.	*/
	bool		Cmd_RunAwayInstall( bool bEnable = true);

	/** \brief 캐릭터 무적 설정
		\param[in] bEnable true : 무적상태 false : 일반상태 */
	void		Cmd_Invincible( bool bEnable, REAL32 InvincibleElapseTime = 0.f);

	// 착지 액션 실행
	void		Cmd_JumpEnd( void);

	/** \brief 점프 동작 */
	void		Cmd_JumpStart( void);

	/** \brief 동작 없이 무기만 공격한다. */
	void		Cmd_Only_AttackWeapon( WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE, VEC3D * pPos = nullptr, VEC3D * pTarget = nullptr);

	/** \brief 캐릭터의 무기 교환 액션만 취합니다. */
	void		Cmd_Only_ChangeWeaponAction( void);

	/** \brief 캐릭터의 Damage 액션만 취합니다. */
	void		Cmd_Only_DamageAction( CHARA_HIT_PART part, VEC3D * pDir);

	/** \brief Observer로 변경시 호출*/
	void		Cmd_Observer( bool bEnable = true);

	/** \brief Observer Mode에서 Zoom 기능 활성화 */
	void		Cmd_Observer_Zoom( void);

	/** \brief 무기 줍기 */
	void		Cmd_PickupWeapon( WeaponBase * pWeapon, UINT8 nSyncNumber = 0, bool bCheckDropList = true, bool bPushWeaponList = true);

	/** \brief Profile 동작 Ceremony를 한다. */
	void		Cmd_Profile_Ceremony( void);

	/** \brief Profile 동작 A를 실행한다.
		\note 1.0 버전 동작	*/
	void		Cmd_Profile_Idle_A( void);

	/** \brief Profile 동작 B를 실행한다.
		\note 1.0 버전 동작 */
	void		Cmd_Profile_Idle_B( void);

	/** \brief Profile 동작 Idle_Change를 실행한다. */
	void		Cmd_Profile_Idle_Change( bool bIsSameWeapon);

	/** \brief Profile 동작 Idle_Stand를 실행한다. */
	void		Cmd_Profile_Idle_Stand( void);

	/** \brief 재장전 동작을 한다. */
	bool		Cmd_Reload( ACTION_LOAD_MAG_TYPE type = ACTION_LOAD_MAG);

	// Camera, Weapon, Action을 Idle로 초기화한다.
	void		Cmd_Reset( void);

	/** \brief 캐릭터를 Respawn 시킨다.
		\param[in] pPos Respawn 위치
		\param[in] rTheta Theta
	*/
	void		Cmd_Respawn( VEC3D * pPos, REAL32 rTheta, bool bInvincible = true);

	/** \brief 상체의 이전 상태로 변경한다. */
	bool		Cmd_Return_Upper( void);

	/** \brief 하체의 이전 상태로 변경한다. */
	void		Cmd_Return_Lower( void);

	/** \brief 달리기 액션 실행
		\note 상.하체 모두 설정됩니다. 	*/
	void		Cmd_Run( VEC3D * pDir, bool bUpper = true, bool bLower = true);

	/** \brief 닉네임을 표시합니다. */
	void		Cmd_SetCharaNick( bool bFriendly);

	/** \brief clan Texture를 설정합니다. */
	bool		Cmd_SetClanTexture( INT32 nClanTexIdx);

	/** \brief 캐릭터 색상을 변경합니다.	*/
	void		Cmd_SetColor( COLORREAL * pDiff = nullptr, COLORREAL * pSpec = nullptr, REAL32 alpha = -1.f);

	/** \brief 캐릭터 물리를 설정합니다. */
	void		Cmd_SetPhysics( void);

	/** \brief 속도 감속 효과 */
	void		Cmd_SpeedReduction( CHARA_HIT_PART part, INT32 rCurHP, INT32 nAfterHP);

	// 서기 액션 실행
	bool		Cmd_StandUp( void);

	/** \brief 무기 싱크를 맞춥니다.
		\note 다음 바뀌는 무기가 있으면 바꿔준다.
			액션을 취하기 전에 호출하여 무기를 변경합니다.
			네트워크의 경우 무기 교체와 액션이 동시에 이루어질 수 있다.		*/
	void		Cmd_SyncWeapon( void);

	/** \brief */
	void		Cmd_ToggleBodyState( void);

	/** \brief Extension 상태를 Toggle한다. */
	bool		Cmd_ToggleExtensionWeapon( void);

	/** \brief 줌기능을 Toggle한다. */
	void		Cmd_ToggleZoom( void);


	/** \brief 줌기능을 Toggle한다. */
	bool		Cmd_ToggleZoomForDSR( void);

	/** \brief 캐릭터를 회전시킨다. */
	void		Cmd_Turn( REAL32 deg = 180.f);

	// Character의 Visible 및 Controller Shape의 활성화 유무를 설정한다.
	void		Cmd_Visible( bool bVisible);
	

	/** \brief 폭탄 해체
		\param[in] bEnable 해체 여부. false이면 취소한다.*/
	bool		Cmd_Uninstall( bool bEnable = true);

	/** \brief Domination Skill Use
		\param[in] bEnable 해체 여부. false이면 취소한다.*/
	bool		Cmd_UseDomiSkill();
	bool		Cmd_CancelDomiSkill();

	/** \brief 걷기 동작 */
	void		Cmd_Walk( VEC3D * pDir);

	/** \brief 하체만 걷기 동작을 한다. */
	void		Cmd_WalkLower( VEC3D * pDir);

	/** \brief 고함 동작을 한다. */
	void		Cmd_Roar( void);

	/** \brief 뻘짓 동작을 한다. */
	void		Cmd_Dino_Idle( void);

	/** \brief 스턴 동작을 한다. */
	void		Cmd_Dino_Stun( void);

	
	// 튜토리얼 관련
public:
	void		WarpChara(VEC3D* pPos);
	
	// Animation Load for background system
	void		PreloadAnimationByWeapon( T_ItemID itemID);

	// Network Attack 수정 관련
private:
	REAL32				m_iNetworkAttackCount = 0.0f;

public:
	void				ResetNetworkAttackCount() { m_iNetworkAttackCount = 0.f; }
	void				NetworkAttackCountPlus(REAL32 tm) { m_iNetworkAttackCount += tm; }
	bool				IsNetworkAttackReturnUpper();

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
public:
	INT64				m_fDebugtimeStart = 0;
	INT64				m_fDebugtimeEnd = 0;
	
	REAL32				m_rDebugFireTimer = 0.0f;
	REAL32				m_rDebugMoveTime = 0.0f;
	REAL32				m_rDebugElp = 0.0f;
	VEC3D				m_vDebugPos;
	VEC3D				m_vDebugOriginalPos;
	VEC3D				m_vDebugTargetPos;

	virtual void		DumpChara( void);
#endif

public:
	/** \brief Eye Enable */
	virtual void	SetEyeEnable( bool enable) { }

// To Version
protected:
	REAL32 _CalcDamageToVersion(const CHARA_DAMAGEINFO * pDamageInfo, REAL32 rValidDamageRate);
};

#endif

