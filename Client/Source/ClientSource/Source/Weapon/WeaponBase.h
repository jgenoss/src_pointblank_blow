/**
 * \file	Weapon\WeaponBase.h
 *
 * Declares the weapon base class.
 */


#if !defined( __WEAPON_BASE_H)
#define __WEAPON_BASE_H

#include "Network/ClientP2PDef.h"
#include "./WeaponInfo/WeaponInfo.h"
#include "../GameCharaDef.h"
#include "../Character/GameCharaAnimControlDefine.h"
#include "WeaponClientDef.h"
#include "CharaAIIndexTable.h"
#include "WeaponDamage.h"
#include "MainWeapon.h"

//#define TEST_COLLISION_LINE		// 컬리젼 라인을 그려줍니다.

class CGameCharaBase;

class WeaponBase : public i3Chara
{
	I3_CLASS_DEFINE( WeaponBase, i3Chara);

private:
	friend class MainWeapon_Knife;
	friend class MainWeapon_Flash;
	friend class MainWeapon_CrossFire;
protected:
	virtual MainWeapon*	CreateMainWeapon() ;

public:

	const MainWeapon*	GetMainWeapon() const { return m_mainWeapon.get(); }
	MainWeapon*			GetMainWeapon() { return m_mainWeapon.get(); }
	i3AIContext*		GetAIContext() const { return m_pAICtx; }	

	void				setAIIndex( WEAPON::AI index, INT8 ai) { I3ASSERT( ai < 128); m_AIIndex[ index] = ai; }
	void				PreCreateMainWeaponBeforeCreateCall();
	
protected:

	MainWeaponPtr		m_mainWeapon;

	WEAPON_FLAG			m_WeaponFlag;
	// reference variable
	CWeaponInfo	*		m_pWeaponInfo = nullptr;			//	무기 정보. m_pWeaponInfo 포인터는 Swap Extension에 의해 바뀔수 있다.
	CWeaponInfo	*		m_pAbsoluteWeaponInfo = nullptr;	//  가장 처음 초기화되는 주무기 정보. m_pWeaponInfo 포인터는 Swap Extension에 의해 바뀔수 있다.
	i3Body *			m_pBody = nullptr;

	CWeaponInfo *		m_pExtWeaponInfo = nullptr;		// extension 기능 활성화 시의 무기 정보 (유탄발사기, C5 해당)

	// Damage
	weapon::Damage*		m_pDamage = nullptr;				// normal, random, critical

	// Animation
	i3AIContext *		m_pAICtx = nullptr;				//	AI Context
	INT8				m_AIIndex[ WEAPON::AI_MAX];
	
	// Bone
	INT32				m_idxFXBone = -1;				// 발사 이펙트를 붙일 Bone의 인덱스
	INT32				m_idxFXBullet = -1;				// 탄피 이펙트를 붙일 Bone의 인덱스
	INT32				m_idxFXIdleBone = -1;			// Idle 이펙트가 붙을 GunDummy Bone의 인덱스
	INT32				m_idxFireEffect[MAX_FIRE_EFFECT];			// 발사 이펙트의 인덱스
	INT32				m_idxFireEffect_1PV_Zoom = -1;	// 줌/정조준/스코프시의 발사 이펙트 인덱스
	INT32				m_idxBulletEffect = -1;			// 탄피 이펙트의 인덱스
	INT32				m_idxFXLaserSight = -1;			// LaserSight
	
	i3Transform *		m_pFXTrans = nullptr;				// 본 더미용 Transform이 있는 경우 설정되며 이곳에 이펙트가 붙는다.
	i3Transform *		m_pFXLastTrans = nullptr;			// 파이어 이펙트 offset 사용시 사용하는 transform 입니다.

	//	Material
	i3AttrSet *			m_pAttrSet = nullptr;
	i3MaterialAttr *	m_pMaterialAttr = nullptr;
	i3BlendEnableAttr *	m_pBlendEnable = nullptr;
	REAL32				m_fAlphaVal = 0.0f;			//	모델링 알파 값

	i3Light *			m_pLight = nullptr;
	REAL32				m_timeLight = 0.0f;

#if defined( TEST_COLLISION_LINE )
	i3VertexArray	*	m_pVA = nullptr;
#endif

	// Sound
	i3SoundPlayInfo	 *	m_pFireSoundPlayInfo[WEAPON::FIRE_SOUND_COUNT] = { nullptr, };
	i3SoundPlayState *	m_pFireSoundPlayState = nullptr;

	i3SoundPlayInfo	 *	m_pSwapSoundPlayInfo = nullptr;
	i3SoundPlayState *	m_pSwapSoundPlayState = nullptr;

	i3SoundPlayInfo	 *	m_pIdleSoundPlayInfo = nullptr;
	i3SoundPlayState *	m_pIdleSoundPlayState = nullptr;

	// Weapon information variable
	INT32				m_Idx = -1;					// 웨폰 메니저에서 받은 고유 인덱스
	INT16				m_NetIdx = -1;				// 통신용 인덱스(드랍시에 사용됩니다.)
//#ifdef ALTER_DOMI_INSTALL_OBJ
	DOMI_TYPE			m_objType = DOMI_TYPE_NO;
//#endif
	WEAPON::eSTATE		m_StateWeapon = WEAPON::eSTATE::STATE_READY;			//	State
	WEAPON::STATE_MASK	m_StateMaskWeapon = 0;		//	State Mask
	WEAPON::FIRE_ORDER	m_FireOrder = WEAPON::PRIMARY_FIRE;			//	공격 형태

	//	하나의 무기내에 여러개의 익스텐션을 가질수 있으므로 기존에 m_pWeaponInfo->getExtensionType() 호출대신
	//	무기의 getCurrentExtensionType() 함수 이용하세요. komet
	WEAPON::EXT_TYPE		m_CurrentExtensionType = WEAPON::EXT_TYPE::EXT_NONE;		//	현재 활성화된 무기 익스텐션. (여러개의 익스텐션이 있는 무기가 있으며 변경도 가능하다.중간에 바뀔수 있는 값.)

	// Dragonov만을 위해 추가됨.
	REAL32				m_timeAfterZoom = 0.0f;
	
	// Current attach grip type
	WEAPON_ATTACH_TYPE	m_AttachGrip = WEAPON_ATTACH_NONE;

	WEAPON_DUAL_MAGAZINE_TYPE	m_DualMagazineMode = WEAPON_DUAL_MAGAZINE_NONE;			// 0 듀얼이 아닙니다. 1 레프트에 장전되어 있습니다. 2 라이트에 장전되어 있습니다.
	WEAPON_SNIPER_DUAL_MAGAZINE_TYPE m_SniperDualMagazineMode = WEAPON_SNIPER_DUAL_MAGAZINE_NONE;
	
	REAL32				m_timeAutoFire = 0.0f;			// 연사시의 연사 간격 계산용


//  용량 과다 문제로 ( 1536바이트 ) 링버퍼 대신, i3.list와 static_storage_pool사용으로 변경..(16바이트 + 외부동적할당)
	i3::list<WEAPON_EMPTYBULLET_SOUND*>	m_listEmptyBulletSound;
	i3::list<WEAPON_TERRAIN_SOUND*>		m_listTerrainSound;

	// Collision information
	bool				m_bHitGroup[HIT_CHARA_PART_COUNT] = { false, };	// 한번에 맞을수 있는 캐릭터 Hit Group
	REAL32				m_rPenetrateLength = 0.0f;				// 관통된 벽까지의 거리
	VAR_LR(VEC3D*, 225)	m_StartPos = new VEC3D();
	VAR_LR(VEC3D*, 226)	m_TargetPos = new VEC3D();
	NxShape *			m_pObjectHitShape = nullptr;				// 사용 안함
	NxBox				m_HitBox;						// 나이프의 오버렙테스트에 사용

	UINT8				m_u8EncryptKey = 0;					// Encrypt용

	bool				m_bSetStart = false;					// 나중에 다른 방식으로 수정할것.(13.11.14.수빈)
	friend class MainWeapon_HandGrenade;
	friend class MainWeapon_DualKnife;
private:
	// 중요한 데이터는 Encription합니다.
	// 반드시 Interface를 통해서 변수를 접근합니다.
	VAR_LR(REAL32,227)		m_DamageRate = 0.0f;			// 파괴율 (익스텐션 상태에 따라 변경되어짐)
	VAR_LR(INT32, 228)		m_AutoFireCount = 0;		// 연사시의 발사수
	VAR_LR(REAL32, 229)	m_nLoadedBullet = 0.0f;		// 장전된 총알 수
	VAR_LR(REAL32, 230)	m_nLoadedBulletDual = 0.0f;	// Dual 탄창 총알 수
	VAR_LR(INT32, 231)		m_nMaxBullet = 0;			// 소유 총알 총 수
	INT32					m_AutoPointShotCount = 0;		// 신규 점사 발사수.
	INT32					m_RemainAutoFireCount = 0;		// 익스텐션 전환시 남아있는 오토 파이어 카운터.

	INT32				ObjHitEffIdx = -1;
	INT32				CharaHitEffIdx = -1;
	INT32				CharaHitEff1PVIdx = -1;
	INT32				IdleEffIdx = -1;
	bool				UnDecalHitEffFlag = false;
	bool				UnBlowHitEffFlag = false;
	INT32				DecalEffExtIdx = -1;

	REAL32				m_rLastIdleEffectTime = 0.0f;	//마지막 Idle 이펙트를 출력하고 지난 시간

public:
	INT32				getCharaHitEffIdx(){return CharaHitEffIdx;}
	INT32				getCharaHitEff1PVIdx(){return CharaHitEff1PVIdx;}
protected:

	bool				m_bHitWorld = false;
	I3_PHYSIX_HIT_RESULT *	m_pHitWorld = nullptr;				// 월드 히트 정보
	I3_PHYSIX_HIT_RESULT *	m_pHitWorldPenetrate = nullptr;		// 관통된 월드 히트 정보
	I3_PHYSIX_HIT_RESULT *	m_pHitObject = nullptr;				// 오브젝트 히트 정보
	I3_PHYSIX_HIT_RESULT *	m_pHitObjectPenetrate = nullptr;		// 관통된 오브젝트 히트 정보
	I3_PHYSIX_HIT_RESULT *	m_pHitItem = nullptr;					// 아이템 히트

	I3_PHYSIX_HIT_RESULT	m_LastHitResult;
	bool				m_bTestRoughCollision = false;			// 한번에 여러발 발사되는 총기에 대해서 한번만 체크하도록합니다.

	// Contact report
	i3PhysixShapeSet *	m_pContactHitBox = nullptr;				// 이펙트 히트용 컨텍트 히트 박스
//	UINT32				m_nContactReportCount;			// contact된 정보 버퍼 카운트(계속 증가값이다.)
//	UINT32				m_nContactReportReadPos;		// 버퍼에서 읽은 버퍼 위치 카운트( 계속 증가값이다)
//	I3_PHYSIX_CONTACT_REPORT m_ContactReportBuf[ PHYSIX_CONTACT_REPORT_COUNT ];
	i3::vector<I3_PHYSIX_CONTACT_REPORT_PTR>	m_vecContactReport;

	// attachment
	CGameCharaBase *	m_pOwner = nullptr;				//	무기 주인
	i3GameNode *		m_pParentGame = nullptr;			// 노드

//	i3::vector<i3TimeStamp*>	m_vecEffectStamp;		// 이펙트 스탬프..
	UINT16				m_idxColorForFireEffect = 0;


	CGameCharaBase *	m_pCreater = nullptr;				// 무기 생성자 (재사용 여부를 결정하기 위함)
	INT32				m_RespawnCnt = 0;			// 유저의 Respawn Cnt (재사용 여부에 사용)

	i3Node *			m_pGunDummy = nullptr;			//	총 손잡이 노드
	i3Node *			m_pGunDummyLeftHand = nullptr;	//	총열 덮개잡이 노드
	i3Node *			m_pGunDummyBack = nullptr;	//	총열 덮개잡이 노드
	MATRIX				m_mtxGunDummy;			//  총 손잡이의 matrix
	MATRIX				m_mtxGunDummyLeftHand;	//  총열 덮개의 matrix

	// Commnad buffer C5에 사용
	INT32				m_nCommandBuf[MAX_WEAPON_COMMAND_COUNT] = { 0, };
	INT32				m_nPushCommandCount = 0;
	INT32				m_nPopCommandCount = 0;

	// KEYWORD : WEAPON_DUALHANDGUN
	WeaponBase *		m_pPairWeapon = nullptr;			// 왼손 무기 -> 잘못된 커맨트.. 왼손무기가 이 클래스객체일때 이것은 오른손 무기임...

	i3CollideeLine		m_CollideeLine;

	UINT8				m_ui8Endurance = 0;

	// Drop System variables
	WEAPON_DROPINFO		m_tDropInfo;

	// Fire control time
	// 정확한 시간 측정을 위해 엔진의 animation time을 사용하지 않고 이 타이머로 체크
	VAR_LR(REAL32,232)	m_rFireTime = 0.0f;

	bool			m_bIsDummy = false;		// 듀얼 무기일 경우 더미 무기 설정 Create 함수에서 초기화하면 안됨.
	bool			m_bOnceJump = false;	// 특정 한가지 동작을 고정 리턴(주특기 스킬용)
	bool			m_bOnceSit = false;

protected:
	virtual	void	_OnInitVairable( void);

public:
	void			_ResetGrenadeInfo(void);
protected:

	virtual void	_InitCollideeGroup( void);

	void			_InitParameters(void);
public:
	void			_TransformWorldSpace( REAL32 tm);
protected:
	void			_LoadAllSound(void);
	void			_UpdateSound(REAL32 rDeltaSeconds);

	void			_LoadFireSound();
	void			_LoadSwapSound();
	void			_LoadIdleSound();

public:
	virtual void	_PlayFireSound( WEAPON::FIRE_SOUND_TYPE type = WEAPON::FIRE_SOUND_NORMAL);
	void			SetSwapSoundPlayState(const UINT32 nState);
	void			SetGachaBrightness(void);
protected:

	virtual void	_PlaySwapSound(void);
	virtual void	_PlayIdleSound(void);

	void			_StopFireSound(void);
	void			_StopSwapSound(void);
	void			_StopIdleSound(void);

public:
	bool			_PlayWeaponAnimWithApplyTimeScale( INT8 idx);		//	idx에 따라 아이템, 베레모 등 애니메이션 시간이 적용되어 플레이된다.
protected:

public:
	virtual void	_OnPlayWeaponIdleAnim(void);		// 파생가상함수들은 이 기본클래스함수 호출을 요구하지 않고 있다...(일관성 문제가 좀 있다..)
protected:

public:
	virtual void	_OnPlayWeaponFireAnim(void);
protected:

	virtual void	_OnPlayWeaponSecondFireAnim( void);

	// 피격에 대한 처리
	// must call resetRaycastResult() after process
	UINT32			_OnHitBloodMark( I3_PHYSIX_HIT_RESULT ** pResultOut, VEC3D * pStartPos, VEC3D * pDir);
	virtual void	_OnHitCharaByExplosion( CGameCharaBase * pChara, CHARA_HIT_PART part, VEC3D * pDir,VEC3D * pStart);
	bool			_OnHit( I3_PHYSIX_HIT_RESULT * pResult, i3CollideeLine * pLine, bool bWarnOwner, bool bDamage = true);
	void			_OnHitExplosionChara( CGameCharaBase * pChara, CHARA_HIT_PART part, VEC3D * pDir);

	// 폭발에 대한 컬리젼 처리
public:
	virtual void	_OnCheckCollisionByExplosion( void);
	virtual bool	_OnCheckCharaCollisionByExplosion( CGameCharaBase * pChara, UINT32 nColGroup, REAL32 rRange);

protected:

	// Wallshot check : return true -> penetrated wall
	bool			_CheckPenetration( I3_PHYSIX_HIT_RESULT * pResult, REAL32 rFireRange, REAL32 rWeaponRange );
	// 히트에 대한 거리 계산
	// rLength == return : no penetrate
	// rLength > return : penetrate hit
	REAL32			_CheckWorldHitLength( I3_PHYSIX_HIT_RESULT * pResult, REAL32 rLength );
	bool			_CheckHitWorld( i3CollideeLine& ColLine, REAL32 rLength );
	bool			_CheckHit( i3CollideeLine& ColLine, REAL32 rLength, INT32 HitType );

	void			_QueContactReport( const I3_PHYSIX_CONTACT_REPORT_PTR& spReport );
	void			_ParseContactReport( void );
	void			_ProcessContactHitBox( void);
	void			_ProcessDrop( void);

	void			_CalcHitWorld( I3_PHYSIX_HIT_RESULT * pResult, VEC3D * pDir);
public:
	void			_CalcHitObject( I3_PHYSIX_HIT_RESULT * pResult, i3CollideeLine * pLine, VEC3D * pDir, REAL32 rLength, bool bDamage = true);
private:
	void			_CalcHitObjectDamage(I3_PHYSIX_HIT_RESULT * pResult, i3CollideeLine * pLine, i3Object * pObject, i3PhysixShapeSet * pShapeSet, REAL32 rDamage);
	void			_CalcHitObjectDamage( I3_PHYSIX_HIT_RESULT * pResult,i3CollideeLine * pLine, REAL32 rDamage);
public:
	void			_CalcHitChara( I3_PHYSIX_HIT_RESULT * pResult, i3CollideeLine * pLine, VEC3D * pDir, REAL32 rLength);
private:
	void			_CalcHitItem( I3_PHYSIX_HIT_RESULT * pResult,  VEC3D * pDir);

	
public:
	REAL32			_CheckExplosionObjectCollision( REAL32 rRange, REAL32 rRadian = I3_PI);		// return : 컬리젼된 오브젝트의 최소 거리
	void			_CheckExplosionWeapon( REAL32 rRange, REAL32 rRadian = I3_PI );
	virtual	bool	IsValidExplosionArea(VEC3D * pTargetPos, REAL32 rRange, REAL32 rRadian, bool bChkWorld = true)		{ return true; }
protected:

	// KEYWORD : WEAPON_KNIFE
	I3_PHYSIX_HIT_RESULT * _CollisionTestByHitBox( i3CollideeLine * pline, REAL32 rMinLength, VEC3D* pVec3 = nullptr);
public:
	I3_PHYSIX_HIT_RESULT * _DoFireCollisionKnife( i3CollideeLine * pline, INT32 idxGroup, bool *bOwnerWarn);
protected:
	// KEYWORD : PHYSIX
	void			_CalcPhysixForce( i3PhysixShapeSet * pShapeSet, VEC3D * pHitDir, VEC3D * pHitPos, REAL32 rForce, REAL32 rLength);
	void			_CalcPhysixForce( I3_PHYSIX_HIT_RESULT * pResult, VEC3D * pHitDir, REAL32 rDamage );
public:
	void			_FireEffect( MATRIX * pStart, INT32 idxEffect, bool bAttachWorld = false);		//	발사 이펙트
	void			SetColorFireEffect(i3TimeStamp * pEffect);
	void			_RemoveLight(void);

private:
	void			_IdleEffect(REAL32 rDeltaSeconds);												//	Idle 이펙트
	void			_RemoveIdleEffect();

protected:
	void			_EmptyCartridgeEffect( INT32 mtxIdx, INT32 effectIdx);						//	탄피 이펙트
	void			_PlayEmptyCartridgeSound( I3_TERRAIN_TYPE nTerrainType, VEC3D * pPos);						// 탄피 사운드
	void			_AddEmptyCartridgeSound( I3_TERRAIN_TYPE nTerrainType, REAL32 delay, VEC3D * pPos );
	virtual void	_OnPlayTerrainHitSound( WEAPON::FIRE_ORDER fireOrder, UINT32 type, VEC3D * pPos);

	WEAPON_CLASS_TYPE	_GetDinoTerrainHitWeaponClass( bool & bAddSound);

public:
	void			_AddTerrainEffectSound( I3_TERRAIN_TYPE nTerrainType, REAL32 delay, VEC3D * pPos);
protected:

	// 발사 이팩트를 뿌립니다.
	virtual void	_OnEmitFireEffect();
	// 발사 사운드를 플레이 합니다.
public:
	virtual void	_OnPlayFireSound();
protected:

	// 발사에 따른 총알 카운트를 계산합니다.
	virtual INT32	_OnProcessFireBulletCount( void);

	virtual void	_OnAddTerrainEffect( WEAPON_TERRAIN_EFFECT_INFO * pInfo);

	WEAPON_BULLET_HIT_STATE		_Raycast( i3CollideeLine * pLine, REAL32 & rMinLength, UINT32 & nWorldHitCount, UINT32 & nPenetrateCount);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		AnimCallback function
	virtual void	_OnProcessFireFirstAnim( void);
	virtual void	_OnProcessFireMiddleAnim( REAL32 rDelay);
public:
	virtual void	_OnProcessFireEndAnim( REAL32 rDelay);

protected:
	REAL32			_CalcFireDelayForDesignation(REAL32 rDelay);
	REAL32			_CalcFireDelayForSkill(REAL32 rDelay);
	REAL32			_CalcFireDelayForEquip(REAL32 rDelay);

public:	// getAIIndex를 public으로 변경..
	INT8			getAIIndex( WEAPON::AI index)				{ return m_AIIndex[ index]; }


public:
	bool			getSkillOnce_Jump()									{ return m_bOnceJump;		}
	void			setSkillOnce_Jump(bool Sig)							{ m_bOnceJump = Sig;			}
	bool			getSkillOnce_Sit()									{ return m_bOnceSit;		}
	void			setSkillOnce_Sit(bool Sig)							{ m_bOnceSit = Sig;			}


	//------------------------------------------------------------------------------------------------------------------------------//
	//														Inline functions
	//------------------------------------------------------------------------------------------------------------------------------//
public:

	void			setIdx( INT32 idx)							{ m_Idx = idx; }
	INT32			getIdx( void)								{ return m_Idx; }

	void			setNetworkIdx( INT16 idx )					{ m_NetIdx = idx; }
	INT16			getNetworkIdx( void)						{ return m_NetIdx; }
//#ifdef ALTER_DOMI_INSTALL_OBJ
	void			setObjectType( DOMI_TYPE objType)			{ m_objType = objType; }
	DOMI_TYPE		getObjectType() const						{ return m_objType;	}
//#endif

	bool			isWeaponFlag( WEAPON_FLAG flag)				{ return (m_WeaponFlag & flag) == flag; }
	void			addWeaponFlag( WEAPON_FLAG flag)			{ m_WeaponFlag |= flag; }
	void			removeWeaponFlag( WEAPON_FLAG flag)			{ m_WeaponFlag &= ~flag; }

	bool			isAttached( void)							{ return isWeaponFlag( WEAPON_FLAG_ATTACHED);	}
	bool			isTakeChara( void)							{ return isWeaponFlag( WEAPON_FLAG_TAKE_CHARA);	}

	virtual void	setExtensionActivate( bool bVal )			{ if( bVal) addWeaponStateMask( WSM_ACTIVATE_EXTENSION); else removeWeaponStateMask( WSM_ACTIVATE_EXTENSION); }
	bool			isExtensionActivate( void)					{ return isWeaponStateMask( WSM_ACTIVATE_EXTENSION);	}

	//	두가지 Extension 기능 가지고 있는 여부
	bool			isMultipleExtension(void)					{ return isWeaponFlag( WEAPON_FLAG_MULTIPLE_EXTENSION);	}
	
	void			setZoom( bool bFlag);

	CGameCharaBase *	getOwner(void)							{ return m_pOwner; }
	void				setOwner( CGameCharaBase * pOwner)		{ m_pOwner = pOwner; }
	virtual void	SetOwner( CGameCharaBase * pOwner);

	CGameCharaBase *	getCreater( void)						{ return m_pCreater; }
	void				setCreater( CGameCharaBase * pChara)	{ m_pCreater = pChara; }
	void			setUserRespawnCnt( INT32 val)				{ m_RespawnCnt = val; }
	INT32			getUserRespawnCnt( void)					{ return m_RespawnCnt; }

	void			setParentGame(i3GameNode * pNode )			{ m_pParentGame = pNode;		}
	i3GameNode *	getParentGame( void)						{ return m_pParentGame;			}

	i3Node *		getParentScene( void)						{ return getSceneObject()->GetParent(); }	
	

	void			setWeaponInfo(CWeaponInfo* info)			{  m_pWeaponInfo = info; }
	CWeaponInfo *	getWeaponInfo(void)							{ return m_pWeaponInfo; }
	CWeaponInfo *	getAbsoluteWeaponInfo(void)					{ return m_pAbsoluteWeaponInfo; }
	CWeaponInfo *	getExtensionWeaponInfo( void)				{ return m_pExtWeaponInfo; }
	CWeaponInfo *	GetCurrentWeaponInfo(void);

	WEAPON::eSTATE	getWeaponState(void)						{ return m_StateWeapon; }
	void			setWeaponState( WEAPON::eSTATE state);
	
	bool			isWeaponStateMask( WEAPON::STATE_MASK mask)		{ return (m_StateMaskWeapon & mask) != 0;	}
	void			addWeaponStateMask( WEAPON::STATE_MASK mask)		{ m_StateMaskWeapon |= mask; }
	void			removeWeaponStateMask( WEAPON::STATE_MASK mask)	{ m_StateMaskWeapon &= ~mask; }
	void			setWeaponStateMask(WEAPON::STATE_MASK mask) { m_StateMaskWeapon = mask; }

	INT32			getCurLOD( void)							{ if(m_pBody!=nullptr){return m_pBody->getCurrentLevel();}return 0;}
	void			setCurLOD( INT32 lv)						{ if(m_pBody!=nullptr){m_pBody->setCurrentLevel( lv);}}

	bool			isFireEnabled(void)							{ return (isWeaponStateMask( WSM_FIRE_DISABLE) == false); }
	bool			isLoadMagazineEnabled(void)					{ return (isWeaponStateMask( WSM_LOADMAG_DISABLE) == false); }
	bool			isLoadBulletEnabled(void)					{ return (isWeaponStateMask( WSM_LOADBULLET_DISABLE) == false); }


	WEAPON::FIRE_ORDER	getFireOrder( void)						{ return m_FireOrder;	}
	void				SetFireOrder(WEAPON::FIRE_ORDER order) { m_FireOrder = order; }					// 불가피하게 생겨난 함수...


	WEAPON_DUAL_MAGAZINE_TYPE getMagazineType( void )			{ return m_DualMagazineMode; }

	WEAPON_ATTACH_TYPE getAttachGripType(void)					{ return m_AttachGrip; }

	bool			isPossibleTurnOffWithDotLaser(void);
	
	REAL32			getCrosshairSpeedFactor(void);
	REAL32			getCrossHairDeviationTargetTime(CWeaponInfo * pInfo);
	bool			getScopeCrosshairEnable(void);

	INT32			getAutoPointShotCount( void);
	void			setAutoPointShotCount( INT32 count);

	INT32			getAutoFireCount(void);
	void			setAutoFireCount(INT32 count);

	INT32			getRemainAutoFireCount(void);
	void			setRemainAutoFireCount(INT32 count);

	virtual INT32	getLoadedBulletDualCount( void);
	virtual void	setLoadedBulletDualCount( INT32 nCount );
	INT32			getLoadedBulletCount(void);
	void			setLoadedBulletCount( INT32 nCount );

	INT32			getTotalBulletCount(void);
	void			setTotalBulletCount( INT32 nCount );

	virtual REAL32	getLoadedBulletDualCountF( void);
	virtual void	setLoadedBulletDualCountF( REAL32 nCount );
	REAL32			getLoadedBulletCountF(void);
	void			setLoadedBulletCountF( REAL32 nCount );

	REAL32			getDamageRate( void);
	void			setDamageRate( REAL32 rDamage);

	REAL32			getCamSyncRate(void)						{ return m_pWeaponInfo->GetCamSyncRate(); }

	virtual MATRIX *		getFXMatrix(void)
	{
		if( m_pFXLastTrans != nullptr && m_pOwner != nullptr && m_pOwner->is1PV() && m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM)) return m_pFXLastTrans->GetCacheMatrix();
		if( m_pFXTrans != nullptr) return m_pFXTrans->GetCacheMatrix();
		if( m_idxFXBone == -1)	return nullptr;
		return getSceneObject()->GetBody()->getCacheMatrix( m_idxFXBone);
	}

	void			setDroped( bool bVal )						{ m_tDropInfo._bDroped = bVal; }
	bool			IsDroped( void )							{ return m_tDropInfo._bDroped; }

	VEC3D *			getDropDir( void )							{ return &m_tDropInfo._vDropDir; }
	void			setDropDir( VEC3D * pDir )					{ i3Vector::Copy( &m_tDropInfo._vDropDir, pDir ); }

	void			updateAnim( i3SceneTracer * pTracer)		{ if(m_pBody!=nullptr){m_pBody->UpdateAnim( pTracer);}}
	void			setCurrentLOD( INT32 lv)					{ if(m_pBody!=nullptr){m_pBody->setCurrentLevel( lv);}}
	void			calcLOD( VEC3D * pPos, VEC3D * pCamPos, REAL32 fFOVRatio)		{ if(m_pBody!=nullptr){m_pBody->CalcLOD( pPos, pCamPos, fFOVRatio);}}
	void			MoveNodeForBow();

	/** \brief 공격력 반환 
		\desc 소음기 등의 추가 장착 장비에 따른 모든 요소를 감안한 값을 반환한다. */
	REAL32			getAP( bool bRandomDamage = true)			{ return GetDamage( bRandomDamage) * m_DamageRate; }

	/** \brief 무기 공격에 따른 Physics Force 값을 반환한다. */
	virtual REAL32	GetPhysixForce( WEAPON_CLASS_TYPE classType);

	i3PhysixShapeSet * getContactHitBox( void)					{ return m_pContactHitBox; }

	void			ResetCommand( void);
	INT32			CommandCount( void);
	void			PushCommnad( INT32 nCmd);
	INT32			PopCommand( );

	void			setPairWeapon( WeaponBase * pWeapon)		{ m_pPairWeapon = pWeapon; }
	WeaponBase *	getPairWeapon( void)						{ return m_pPairWeapon; }

	CWeaponInfo *	getBaseWeaponInfo( void )	
	{ 
		return (m_pPairWeapon != nullptr ) ? m_pPairWeapon->getBaseWeaponInfo() : m_pAbsoluteWeaponInfo ;
	}

	VEC3D*			GetStartPos() { return m_StartPos; }
	VEC3D*			GetTargetPos() { return m_TargetPos; }

	void			StopFireSound(void)							{ _StopFireSound(); }
	void			StopSwapSound(void)							{ _StopSwapSound(); }

	INT32			getFireEffectIndex( INT32 idx=0) const 
	{ 
		if (idx < 0 || idx >= MAX_FIRE_EFFECT)
		{
			I3_BOUNDCHK(idx, MAX_FIRE_EFFECT);
			return -1;
		}
		return m_idxFireEffect[idx];	
	}
	INT32			getFireEffectIndex1PVZoom(void)				{ return m_idxFireEffect_1PV_Zoom;		}

	// 익스텐션 상태에 따른 현재의 진짜 무기
	virtual WeaponBase * getUsedWeapon( void)					{ return this; }

	// 내구도 함수
	void			SetEndurance( UINT8 ui8Val )				{ m_ui8Endurance	= ui8Val;	}
	UINT8			GetEndurance(void) const					{ return m_ui8Endurance;		}
	bool			IsEnduranceZero(void) const					{ return m_ui8Endurance == 0;	}


	WEAPON_DROPINFO	* getDropInfo( void	)						{ return &m_tDropInfo; }

	// 탄환 발사 형태의 무기
	virtual bool	isBulletFireWeapon( void)					{ return true; }
	virtual bool	isReadyToLoadMagazine()						{ return isWeaponStateMask( WSM_READY_TO_LOADMAG); }


	// 무기 정보
	virtual CWeaponInfo *	getRealWeaponInfo( void)			{ return m_pWeaponInfo; }
	static bool		GenerateWeaponSndName(char* buf, INT32 bufSiz, CWeaponInfo* winfo,
		WEAPON::FIRE_SOUND_TYPE type);

	void			setEnablePlayFireSound( bool bEnable)		{ if(bEnable)addWeaponFlag(WEAPON_FLAG_ENABLE_FIRE_SOUND); else removeWeaponFlag( WEAPON_FLAG_ENABLE_FIRE_SOUND);	}

	// World에서 습득하는 무기 (RPG7, K201)
	bool			isWorldWeapon( void);

	bool			isDamageToMissionObject( void);

public:
	WeaponBase( void);
	virtual ~WeaponBase( void);

	virtual void	Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon);
	virtual void	PostCreate( void);
	virtual void	Reset( void);
	virtual void	OnBindResource(void);
	virtual void	OnUpdate( REAL32 rDeltaSeconds);

	/** \brief 엔진 이벤트를 처리합니다.
		\note Physics에 의한 이벤트 처리.
			월드에 접촉 여부를 판별하여 드랍 상태를 처리합니다. */
	virtual bool	OnEvent( UINT32 event, i3ElementBase *pObj, UINT32 param, I3_EVT_CODE code);

	/** \brief 무기 인스턴스를 반환합니다.
		\note 메모리 해제는 여기서 실행됩니다. */
	virtual void	ReturnInstance();

	/************************************************************************/
	/*							Material Functions							*/
	/************************************************************************/
	virtual void	OnSetBlendAlpha( REAL32 val);
	virtual void	OnSetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular);
	void			CheckAlphaUsed( void);

	/************************************************************************/
	/*							View Process Functions						*/
	/************************************************************************/
	/** \brief 1인칭 뷰에 대한 처리를 합니다.
		\note Shadow를 끄고 LOD를 설정합니다. */
	virtual void	OnChangeFPSView( void);

	/** \brief 3인칭 뷰에 대한 처리를 합니다. */
	virtual void	OnChangeObserverView( void);

	/** \brief Gun Dummy의 위치를 설정합니다. 
		\note 손으로 잡는 부위에 따라 무기의 Matrix를 설정합니다. */
	void			ChangeGunDummy( WEAPON_ATTACH_TYPE nAttachType);

	/** \brief 캐릭터가 무기의 Gun Dummy의 위치를 설정합니다. 
		\note 손으로 잡는 부위에 따라 무기의 Matrix를 설정합니다. */
	void			ChangeAttachHand( CHARA_WEAPON_ATTACH_HAND nAttachHand, WEAPON_ATTACH_TYPE nWeaponAttach);

	/** \brief 무기를 오른손에 잡는다. 
		\note  */
	void			SetCurrentAttachHand(void);

	/** \brief 무기를 왼손에 잡는다. 
		\note  */
	void			SetCurrentAttachLeftHand(void);

	/** \brief 장전시 총열덮개를 잡는 무기인지 확인합니다. 
		\note true로 반환되는 무기는 장전시 왼손에 무기 더미가 붙는 무기를 의미한다.	*/
	bool			IsLoadMagazineLeftBarrel( void);

	/** \brief 노리쇠 전후퇴시 총열덮개를 잡는 무기인지 확인합니다. */
	bool			IsLoadBulletLeftBarrel( void);

	/** \brief Extension 무기 여부를 확인합니다. */
	bool			IsExtensionWeapon( void);

	/** \brief 발사후 볼트액션 동작이 있는지 확인합니다. */
	bool			IsFireBulletWeapon(void);

	/** \brief 현재 설정된 Extension Type을 반환합니다. */
	WEAPON::EXT_TYPE	GetCurrentExtensionType(void)		{ return m_CurrentExtensionType; }
	WEAPON::EXT_TYPE	GetNextExtensionType(void);
	WEAPON::EXT_TYPE	setExtensionType(WEAPON::EXT_TYPE type);

	/** \brief 2가지 이상의 Extension 기능을 교체합니다. */
	virtual void	SwapExtensionType(void);

	/** \brief Extension 상태에 따라 파츠가 보이거나 안보이도록 설정합니다.
		\note true : 파츠가 보이는 상태 (소음기) */
	virtual void	OnSetVisibleExtension( bool bVal )	{}

	/** \brief 화면의 가운데(조준점)이 향한 지점을 Collision Test를 하여 구한다.
		\note 1PV, 3PV 모두 사용가능. 해당 캐릭터가 바라보는 시점을 기준으로 계산된다.
		\param[out] pDest Collision 된 결과 위치
		\param[in] pStart 시작 위치 */
	virtual void	GetShotTargetPos( VEC3D * pDest, VEC3D * pStart = nullptr);

	/** \brief 탄피 사운드 업데이트 */
	void			ProcessSound( REAL32 tm );

	/** \brief 탄피 사운드 없애기 */
	void			ResetSound();

	/** \brief Contact 용 Hit Box를 생성합니다.
		\note Convex Shape은 느리므로 Hit Box를 사용 */
	void			CreateContactHitBox( INT32 nUserIndex);

	// 총알이 발사된 것에 대한 모든 처리...
	// 실제 총알이 날아간 괘적에 따라 충돌처리 및 캐릭터에 대한
	// Effect 등을 뿌린다.
	/** \brief Line Collision을 합니다. */
	void			DoLineCollision( VEC3D * pStart, VEC3D * pEnd, INT32 idxGroup);
	I3_PHYSIX_HIT_RESULT * DoLineCollision2( i3CollideeLine * pline, INT32 idxGroup, bool *bOwnerWarn);
	I3_PHYSIX_HIT_RESULT * DoLineCollision3( i3CollideeLine * pline, INT32 idxGroup, bool *bOwnerWarn);

	/** \brief 탄 궤적을 추가합니다. */
	void			AddBulletTrace(I3_PHYSIX_HIT_RESULT* pBulletHit);

	/** \brief 탄 궤적의 시작 위치를 반환합니다.
		\note 기본적으로 총구*/
	virtual VEC3D*	GetBulletTraceStartPos();
	virtual VEC3D*	GetSinoBulletTraceStartPos();

	/************************************************************************/
	/*						Character Hit Functions							*/
	/************************************************************************/
	/** \brief Hit Shape 이름으로 Hit 여부를 판별합니다.
		\note m_bHitGroup array에 기록되며 이미 맞은 경우에 대한 처리를 합니다. */
	virtual bool	CheckHitPart( const char * pszPart);

	/** \brief m_bHitGroup를 일괄 설정합니다. */
	void			SetHitGroup( bool bFlag);
	
	/** \brief Hit Direction을 구합니다.
		\param[in] pHitResult Hit된 Shape 정보
		\param[in] pPos 발사 위치
		\param[out] pOutDir 방향 */
	void			CalcHitDir( I3_PHYSIX_HIT_RESULT * pHitResult, VEC3D * pPos, VEC3D * pOutDir);

	/****************************************************************************/
	/*							Weapon Order Functions							*/
	/****************************************************************************/
	/** \brief 무기 교체 process를 실행합니다. */
	virtual void	WeaponChange( bool bPlaySound = true);

	/** \brief 무기 발사 */
	virtual bool	Fire( WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/);

	/** \brief 무기 발사 준비 (for only grenade) */
	virtual void	FireReady( void);

	/** \brief 무기 발사 종료 */
	virtual void	FireEnd(void);
	
	/** \brief Ready 상태로 변경합니다. */
	virtual void	ReturnToReady( void);

	/** \brief 무기 컬리젼을 실행합니다. */
	virtual void	FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos);							// 실제 컬리젼 계산

	/** \brief 장전을 실행합니다. */
	virtual void	LoadMagazine( void);

	/** \brief 장전 준비를 실행합니다. */
	virtual void	LoadMagazine_Ready( void) {}

	/** \brief 노리쇠 전후퇴를 합니다. */
	void			LoadBullet(void);

	/** \brief 무기를 해당 노드에 붙입니다.
		\note 월드에 무기를 버리는 경우*/
	virtual void	Transmission( i3Node * pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pPos = nullptr, bool bToWorld = false);

	/** \brief 무기를 해당 노드에 붙입니다. */
	virtual void	AttachNode( i3Node * pSceneNode, i3GameNode * pOwner);

	/** \brief 무기가 붙은 노드에서 띄어냅니다. */
	virtual void	DetachNode( void );

	virtual void	OnDrop( void);
	virtual void	InstallToWorld(i3Node*, i3GameNode*, VEC3D*, bool, MATRIX*, REAL32 theta = 0.0f)	{}

	void			GetDamageInfo( CHARA_DAMAGEINFO * pOut);
	WEAPON::ATTACK_TYPE	GetAttackType( void);


	/** \brief 히트 컬리전 그룹으로 설정 */
	virtual void	SetHitCollideeGroup(void)		{}

	/** \brief 컬리전 그룹 초기화 */
	void			ResetColideeGroup(void);

	/****************************************************************************/
	/*							Animation Play Functions						*/
	/****************************************************************************/


	/** \brief 공격준비상태를 Play한다. */
	virtual void			PlayAnimation_AttackReady( void);
	/** \brief 착지 애니메이션을 Play한다. */
	void			PlayAnimation_JumpEnd( void);

	/** \brief Damage 애니메이션을 Play한다. */
	void			PlayAnimation_Damage( void);
	
	/** \brief 장전 애니메이션을 Play한다. */
	virtual void	OnPlayLoadMagAnimation( void);

	/** \brief 장전 준비 애니메이션을 Play한다. */
	virtual void	OnPlayLoadMagReadyAnimation( void) {}

	/** \brief 노리쇠 전후퇴를 실행한다. */
	virtual void	OnPlayLoadBulletAnimation();

	/** \breif 무기 교체 애니메이션을 플레이합니다. */
	// virtual void	OnPlayChangeAnimation( REAL32 rTimeScale = 1.f);

	void			OnPlayChangeAni( REAL32 rTimeScale = 1.f);			// 비가상함수로 만들고 구현함수를 가상으로 처리..

	virtual void	OnPlayChangeAniImpl(REAL32 rTimeScale);				// public으로 두면 안되지만, 우선 놔둔다..
	void			OnPlayChangeAniImpl_Default( REAL32 rTimeScale );	// 이것도 일단 비가상으로 따로 둔다..

	// UI weapon animation
	virtual void	PlayAnim_UI_IdleA( void)	{}
	virtual void	PlayAnim_UI_IdleB( void)	{}

	/****************************************************************************/
	/*						Weapon Extension Functions							*/
	/****************************************************************************/
	/** \brief Extension 기능 비/활성화 한다. */
	bool			ExtensionActivate( bool bVal, bool bAnimPlay = true);

	/** \brief Extension 가능 여부를 판별합니다. */
	virtual bool	OnCheckExtensionPossible( bool bVal) 
	{ 
		if( !IsExtensionWeapon())			return false;
		if( isExtensionActivate() == bVal)	return false;
		return true;
	}

	/** \brief Extension animation을 play합니다. */
	virtual void	OnPlayExtensionAnimation( bool bVal, bool bAnimPlay = true );

	/** \brief Extension이 가능하면 처리하는 함수입니다. */
	virtual void	OnProcessExtension( bool bVal) {}

	/****************************************************************************/
	/*								Network Functions							*/
	/****************************************************************************/
	/** \brief 발사를 합니다.*/
	virtual void	NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE );

	/** \brief 네트워크 인자값을 설정합니다.
		\note 발사정보를 네트워크 패킷에 설정합니다.
			네트워크로 보내거나 받은 발사 총알에 대한 랜덤 세팅( 이때에 네트워크로 보내주게 됩니다. ) */
	void			NET_FireSync( VEC3D * pTargetPos, WEAPON::FIRE_ORDER fireOrder = WEAPON::PRIMARY_FIRE );

#ifdef DOMI_STING_POISON_GRENADE
	void			NET_FireSync_DomiDinoGrenade(VEC3D* pTargetPos, WEAPON::FIRE_ORDER fireOrder);
#endif

	/** \brief 수류탄을 던집니다. (for only grenade)	*/
	virtual void	NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nThrowNetIdx ) {}

	/** \brief 수류탄의 바운드를 처리합니다. (for only grenade)*/
	virtual void	NET_Bound( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType );

	/** \brief 수류탄을 던집니다. (for only grenade)
		\note Manager에서 인스턴스를 받아와 월드에 던집니다. */
	virtual void	NET_FirstThrow( VEC3D * pStartPos, VEC3D * pDir, INT32 nNetSlotIdx ) {}
	
	virtual void	GetNetBullet( GAMEINFO_BULLET* pBullet );
	virtual void	SetNetBullet( GAMEINFO_BULLET* pBullet );
	///////////////////////////////////////////////////////////////////////////////////////////

	virtual REAL32	GetMoveSpeed( WEAPON_MOVE_SPEED_TYPE type);
	virtual REAL32	GetFireDelay();
	virtual REAL32	GetFireDelay2();
	REAL32			GetReloadTime();
		// WeaponBase::GetLoadBulletTime() 함수...쓰지도 않는 함수라서 바로 삭제함..(안쓰면 지웁니다. 2015.05.22.수빈)
	REAL32			GetRecoilHorzAngle();
	REAL32			GetRecoilVertAngle();
	REAL32			GetDeviation();
	virtual REAL32	GetRange();
	virtual REAL32	GetDPD_Rate();

	weapon::Damage* GetWeaponDamage() const { return m_pDamage;}

	// bRandomDamage 는 Character에만 적용되야 합니다.
	// 오브젝트는 반듯이 False
	virtual REAL32	GetDamage( bool bRandomDamage = true);
	REAL32			GetScopeFOV();
	REAL32			GetScopeFOV2();
	virtual REAL32	GetThrowSpeed();
	REAL32			GetExThrowSpeed();
	REAL32			GetExplosionTime();
	REAL32			GetExplosionRange();
	REAL32			GetExplosionNearRange();
	REAL32			GetDetectedRange();

	INT32			GetMaxBulletWithCashItem(void);			//	캐시템등 탄약 증가 처리위해
	INT32			GetLoadBulletWithCashItem(void);		//	캐시템등 탄약 증가 처리위해
	INT32			GetMaxBulletWithSkill(void);			//	스킬 탄창 증가처리

	void			AddPumpBullet(REAL32 tm);
	void			AddNomalBullet(void);

	// xgkick_thai
	void	setTeamTag( INT32 team);

	// 듀얼 무기일 경우 더미 무기로 설정
	void			SetDummy( bool bVal)		{ m_bIsDummy = bVal;		}
	bool			IsDummy( void)				{ return m_bIsDummy;		}

	// 수류탄인지 체크
	virtual bool	IsGrenade(void)const		{ return false; } 
	virtual void	Explosion(void)				{};

	virtual bool	isBombState( UINT mask )	{ return false; }

	///////////////////////////////////////////////////////////////////////////////////////////

	virtual REAL32	GetAnimFireScaleTime( CHARA_ACTION_UPPER upperState, ID_UPPER_AI ai )		{ return 1.f;}
	virtual REAL32	GetAnimLoadBulletScaleTime( CHARA_VIEW_TYPE eCAT)						{ return 1.f; }
	virtual REAL32	GetAnimFireBulletScaleTime( CHARA_VIEW_TYPE eCAT)						{ return 1.f; }


	void			DumpState( void);

	virtual void	UpdateAfterAnim(REAL32 rDeltaSeconds)										{}
	virtual void	PlayHitOwnSound( CGameCharaBase * pChara, bool bChangeHP = false)			{}
	
	void			PlayFireSoundByOrder( WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE);
	void			PlayFireSoundByScope( void);
	void			PlaySwapSound( void)		{ _PlaySwapSound();	}

	virtual void	OnPlayChangeSound( void);
	i3SoundPlayInfo* GetFireSoundPlayInfo(WEAPON::FIRE_SOUND_TYPE type) 
	{ return m_pFireSoundPlayInfo[type]; }

	// 마스터 관련

	bool			EnablePlayMasterAnim();
	bool			IsFastReloadAnimWeapon();
	void			ReloadAnimation(CHARA_VIEW_TYPE eCAT);
	void			ChangeDualAnimation(CHARA_VIEW_TYPE eCAT, REAL32 rTimeScale);
	virtual bool	NotMasterLoadMagAnim(INT32 idx);

	bool			isReloadLoopAnimation();
	virtual bool	isDualGun( void)			{ return false;}

	virtual bool	hasLoadedBullet()			{ return (( getMagazineType() != WEAPON_DUAL_MAGAZINE_RIGHT && getLoadedBulletCount() > 0 )
														|| (getMagazineType() == WEAPON_DUAL_MAGAZINE_RIGHT && getLoadedBulletDualCount() > 0 )); }

	virtual void	SetBulletCountForNetwork( INT32 iLoadedCount, INT32 iDualLoadedCount, INT32 iMaxCount);

	virtual void	ApplyBulletCountWithCashItem();
	virtual void	ApplyBulletCountWithSkill();
	virtual void    ApplyBulletWithDropItem(REAL32 rRate);


	virtual void	SetEnable( bool bTrue = true, REAL32 tm = 0.f);

	void			Instantiate( void);

	virtual void	SetExtWeaponInfo();

	void			PlayLoadMagazine( CHARA::eSEX_TYPE nArg, bool b1PV = true);
	void			PlayLoadMagazineExt( CHARA::eSEX_TYPE nArg, bool b1PV = true);
	void			PlayLoadMagazineDualMaz( CHARA::eSEX_TYPE nArg, bool b1PV = true);

	// Load Mag이 Loop와 Non Loop의 함수 구별
	virtual bool	_OnNormalReload(i3AIContext * pCtx, REAL32 tm);
	virtual bool	_OnLoopAnimationReload(i3AIContext * pCtx, REAL32 tm);
	REAL32			m_rReloadTime;

	REAL32			GetWeaponPenetrate(bool IsStrengthen);
	REAL32			GetCrossHairMin();
	INT32			GetRecoilHorzCount(CWeaponInfo * pInfo, bool bApplyExt);
	bool			GetOneHorzEnable(CWeaponInfo * pInfo, bool bApplyExt);
	INT32			GetRecoilVertCount(CWeaponInfo * pInfo, bool bApplyExt);
	bool			GetOneVertEnable(CWeaponInfo * pInfo, bool bApplyExt);

#if defined( AI_MOVE_TO_WEAPON)
	////////////////////////////////////////////////////////
	// 
	// Weapon이 가지고 있는 Chara-AI_State에 대한 Pointer
protected:
	i3AIContext *			m_pCharaAINode;
	CCharaAIIndexTable		m_AIIndexTable;

protected:
	void			_MakeAnimPath( char * pszDest, INT32 len, const char * pszSrc, bool bCommon);
	void			_Rec_BindWeaponAnim( i3AIContext * pAICtx);

public:
	void			AttachCharaAIToChara( i3Chara * pOwner);
	void			DetachCharaAIFromChara(void);

	CCharaAIIndexTable *	getCharaAIIndexTable(void)			{ return &m_AIIndexTable; }

	INT32			getCharaAIIndex( CHARA_ANIMSET_TYPE animSet, CHARA_ACTION_BODYUPPER body2, ID_UPPER_AI ai)
	{
		return m_AIIndexTable.getIndex( animSet, body2, ai);
	}

	INT32			getCharaDualAIIndex( CHARA_ANIMSET_TYPE animSet, ID_UPPER_DUAL_AI ai)
	{
		return m_AIIndexTable.getDualIndex( animSet, ai);
	}
#endif

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	/** \brief Idle State를 업데이트 합니다. */
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm);

	/** \brief Idle State 종료시 호출됩니다. */
	virtual void	OnEndIdle( i3AIContext * pCtx, REAL32 tm);

	/** \brief Fire State를 업데이트 합니다. */
	virtual void	OnFire( i3AIContext * pCtx, REAL32 tm);

	/** \brief Secondary fire state를 업데이트 합니다. */
	virtual void	OnSecondaryFire( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief Fire State 종료시 호출됩니다. */
	virtual void	OnEndFire( i3AIContext * pCtx, REAL32 tm);

	/** \brief LoadMagazine state를 업데이트 합니다.
		\note 장전 완료시 true 반환		*/
	virtual bool	OnLoadMag( i3AIContext * pCtx, REAL32 tm);

	/** \brief LoadMagazine state 종료시 호출됩니다. */
	virtual void	OnEndLoadMag( i3AIContext * pCtx, REAL32 tm);

	/** \brief Load Bullet state를 업데이트 합니다. */
	virtual void	OnLoadBullet( i3AIContext * pCtx, REAL32 tm);

	/** \brief Load Bullet state 종료시 호출됩니다. */
	virtual void	OnEndLoadBullet( i3AIContext * pCtx, REAL32 tm);

	/** \brief Extension Activate state를 업데이트합니다. */
	virtual void	OnExtensionActivate( i3AIContext * pCtx, REAL32 tm);

	/** \brief Extension Deactivate state를 업데이트합니다. */
	virtual void	OnExtensionDeactivate( i3AIContext * pCtx, REAL32 tm);

	/** \brief Quick Load magazine state를 업데이트 합니다. */
	virtual void	OnQuickLoadMag( i3AIContext * pCtx, REAL32 tm ) {}

	/** \brief Quick load magazine state를 종료시 호출됩니다. */
	virtual void	OnEndQuickLoadMag( i3AIContext * pCtx, REAL32 tm ) {}

	/** \brief Shell Fire state를 업데이트 합니다.
		\date 2008.04.11 insert GrenadeShellFire */
	virtual void	OnShellFire( i3AIContext * pCtx, REAL32 tm){}

	/** \brief Shell Fire state 종료시 호출됩니다. */
	virtual void	OnEndShellFire( i3AIContext * pCtx, REAL32 tm){}

	/** \brief Shell Reload state를 업데이트합니다. */
	virtual void	OnShellReload( i3AIContext * pCtx, REAL32 tm){}

	/** \brief Shell Reload state 종료시 호출됩니다. */
	virtual void	OnEndShellReload( i3AIContext * pCtx, REAL32 tm){}

	/** \brief Shell Remove State를 업데이트 합니다. */
	virtual void	OnShellRemove( i3AIContext * pCtx, REAL32 tm){}

	/** \brief Swaping state를 업데이트 합니다. */
	virtual void	OnSwaping( i3AIContext * pCtx, REAL32 tm);

	/** \brief UI에서의 Show weapon state를 업데이트 합니다. */
	virtual void	OnUI_ShowWeapon( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief Load magazine ready state를 업데이트 합니다. */
	virtual void	OnLoadMag_Ready( i3AIContext * pCtx, REAL32 tm);

	/** \brief Fire Bullet state를 업데이트 합니다. */
	virtual void	OnFireBullet( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief Load Bullet state 종료시 호출됩니다. */
	virtual void	OnEndFireBullet( i3AIContext * pCtx, REAL32 tm);

	virtual bool	IsDropObj() override { return IsDroped() || m_tDropInfo._bDropping; }

	/****************************************************************************/

	bool IsThereWallBetween(VEC3D& v3StartPos, VEC3D& v3EndPos, REAL32 rLength);

	// Dedicated server 유무에 따라 Host 설정을 바꾼다.
	bool			IsDedicatedHost( void);
	bool			IsLocalOrAI();


	/** \brief 신규 점사 방식 루틴 */
	void	ProcessAutoPoint(REAL32 tm);


protected:
	REAL32			_CalcReloadTimeToVersion(REAL32 rReloadTime);
	REAL32			_CalcReloadTimeScaleToVersion(i3AIState * pAIState = nullptr, REAL32 rReloadTime = 1.0f, REAL32 rSpeedRatio = 0.0f);
	REAL32			_CalcSwapTimeScaleToVersion(void);
	REAL32			_CalcFireDelayToVersion(REAL32 rFireDelay);
	REAL32			_CalcCrossHairMinToVersion(REAL32 rCrossHairMin);
	REAL32			_CalcDeviationToVersion(REAL32 rDeviation);

	bool			_IsEnableMasterAnimToVersion(void);
};

//
// chunjong.song 14.1.1
//
// 이펙트 중간에 제거처리(제거시점에 시간을 이펙트의 Duration 이상으로 강제세팅해서 종료)
// 이펙트관련 엔진부분 수정없이 처리하기위한(한숨) 매크로
#define WEAPON_EFFECT_FORCE_DELETE(effInsti3TimeStampOnly) { \
	if(effInsti3TimeStampOnly) { \
	i3Node* _eff1stChild = effInsti3TimeStampOnly->GetChild(0); \
	if(_eff1stChild && i3::same_of<i3TimeSequence*>(_eff1stChild)) { \
	i3TimeSequence* _effTimeSeq = (i3TimeSequence*)_eff1stChild; \
	_effTimeSeq->SetLocalTime(_effTimeSeq->getDuration()+1.0f); } \
	effInsti3TimeStampOnly = nullptr; \
	} }

#endif // __WEAPON_BASE_H

