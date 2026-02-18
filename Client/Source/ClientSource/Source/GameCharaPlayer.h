#if !defined( CGAME_CHARA_PLAYER_H)
#define CGAME_CHARA_PLAYER_H


#include "GameCharaBase.h"
#include "GameCharaEquip.h"


// (RADIUS * 2) + TALL <= 1.5
//#define			TALL			0.75f//0.68f	// 
//#define			HALFTALL		0.05f
//#define			RADIUS			0.45f //0.45f


/*
enum DEVIATION_STATE
{
	DEVIATION_FIT = 0,
	DEVIATION_CHANGE,
};

enum BONES
{
	BONE_ROOT = 0,
	BONE_PELVIS,
	BONE_IK,
	BONE_SPINE1,
	BONE_SPINE2,
	BONE_SPINE3,
	BONE_NECK,
	BONE_HEAD,
	BONE_LFOOT,
	BONE_RFOOT,
	BONE_LDARM,
	BONE_RDARM,

	BONE_MAX
};

enum RUNSOUND_MATERIAL
{
	RUNSOUND_THICKWOOD,
	RUNSOUND_CONCRETE,
	RUNSOUND_METAL,
	RUNSOUND_WOOD,
	RUNSOUND_TYPE_COUNT,
};	


enum CHARA_SOUND
{
	CHARA_SOUND_DAMAGE_DEFAULT = 0,
	CHARA_SOUND_DAMAGE_LEFTARM,
	CHARA_SOUND_DAMAGE_RIGHTARM,
	CHARA_SOUND_DAMAGE_CHEST,

	CHARA_SOUND_RUN_COMMON_RIGHT,
	CHARA_SOUND_RUN_COMMON_LEFT,
	CHARA_SOUND_RUN_CONCRETE_RIGHT,
	CHARA_SOUND_RUN_CONCRETE_LEFT,
	CHARA_SOUND_RUN_METAL_RIGHT,
	CHARA_SOUND_RUN_METAL_LEFT,
	CHARA_SOUND_RUN_THICKWOOD_RIGHT,
	CHARA_SOUND_RUN_THICKWOOD_LEFT,
	CHARA_SOUND_RUN_WOOD_RIGHT,
	CHARA_SOUND_RUN_WOOD_LEFT,

	CHARA_SOUND_COUNT
};

enum CHARA_SOUND_TYPE
{
	CHARA_SOUND_TYPE_MALE = 0,
	CHARA_SOUND_TYPE_FEMALE,

	CHARA_SOUND_TYPE_COUNT
};

enum SCR_EFFECT_TYPE
{
	SCR_EFFECT_NONE = 0,
	SCR_EFFECT_DAMAGE,
	SCR_EFFECT_FLASHBANG,

	SCR_EFFECT_TYPE_COUNT
};

enum RECOIL_HORZ_STATE
{
	RECOIL_HORZ_NA = 0,
	RECOIL_HORZ_FIRING,
	RECOIL_HORZ_RESTORE
};

typedef struct _tagCharaPlayer_Recoil
{
	REAL32	m_RecoilTime;
	REAL32	m_RecoilTargetTime;
	REAL32	m_RecoilAngle;		//	수직 반동각
	REAL32	m_RecoilTargetAngle;

	REAL32	m_RecoilHorzAngle;
	REAL32	m_RecoilHorzTime;
	REAL32	m_RecoilHorzTimeTarget;
	REAL32	m_rRecoilHorzTimeTarget;

	RECOIL_HORZ_STATE	m_HorzState;
	REAL32	m_HorzStartAngle;
	REAL32	m_timeHorzRestore;

	void	Reset() {
		m_RecoilTime = m_RecoilTargetTime = m_RecoilAngle = m_RecoilTargetAngle = 0.f;
		m_RecoilHorzAngle = m_RecoilHorzTime = m_RecoilHorzTimeTarget = m_rRecoilHorzTimeTarget = 0.f;
		m_HorzState = RECOIL_HORZ_NA;
		m_timeHorzRestore = 0.0f;
	}

	void	_calcRecoil( REAL32 tm ) {
		if( m_RecoilAngle > 0.f )	{
			m_RecoilTime += tm;
			if( m_RecoilTargetTime > 0.0f)	{
				REAL32 cos_val;
				cos_val = i3Math::vcos( 1.0f + (m_RecoilTime / m_RecoilTargetTime));
				m_RecoilAngle = m_RecoilTargetAngle + ( cos_val * m_RecoilTargetAngle);
				if( m_RecoilAngle < 0.0f)	m_RecoilAngle = 0.0f;
			}
			if( m_RecoilTargetTime < m_RecoilTime)	{
				m_RecoilAngle = 0.0f;	m_RecoilTime = 0.0f;
			}
		}
	}
} CHARAPLAYER_RECOIL;

typedef struct _tagCharaPlayer_Deviation
{
	REAL32			m_DeviationTargetAngle;
	REAL32			m_DeviationStartAngle;
	REAL32			m_DeviationAngle;
	REAL32			m_FireDeviation;
	REAL32			m_DeviationTargetTime;
	REAL32			m_DeviationTime;
	DEVIATION_STATE	m_DeviationState;

	void	Reset() {
		m_DeviationTargetAngle = m_DeviationStartAngle = m_DeviationAngle = 0.f;
		m_DeviationTargetTime = m_DeviationTime = 3.f;
		m_DeviationState = DEVIATION_FIT;
	}

	void	_calcDeviation( REAL32 tm ) {
		if( m_DeviationState == DEVIATION_CHANGE) {
			REAL32 cos_val;
			m_DeviationTime += tm;
			if( m_DeviationTargetTime > I3_EPS) {
				cos_val = i3Math::vsin_libm( (m_DeviationTime / m_DeviationTargetTime));
				m_DeviationAngle = m_DeviationStartAngle + (cos_val * (m_DeviationTargetAngle - m_DeviationStartAngle));
			}

			//I3TRACE( "%f, %f\n", m_DeviationTime, m_DeviationAngle);

			if( m_DeviationTargetTime < m_DeviationTime) {
				m_DeviationAngle = m_DeviationTargetAngle;
				m_DeviationStartAngle = m_DeviationTargetAngle;
				m_DeviationTime = 0.0f;
				m_DeviationState = DEVIATION_FIT;
			}
		}
	}
} CHARAPLAYER_DEVIATION;

typedef struct _tagCharaPlayer_ScopeWave
{
	REAL32	m_timeScopeWaveUpdate;
	REAL32	m_ScopeWaveTargetTheta;
	REAL32	m_ScopeWaveTheta;
	REAL32	m_ScopeWaveThetaVel;
	REAL32	m_ScopeWaveTargetRho;
	REAL32	m_ScopeWaveRho;
	REAL32	m_ScopeWaveRhoVel;
	INT32	m_nCurScopeLevel;	//	현재 줌 단계

	void	Reset() {
		m_timeScopeWaveUpdate = m_ScopeWaveTargetTheta = m_ScopeWaveTheta = m_ScopeWaveThetaVel = 0.f;
		m_ScopeWaveTargetRho = m_ScopeWaveRho = m_ScopeWaveRhoVel = 0.f;
		m_nCurScopeLevel = 0;
	}

	void	_calcScopeWave( REAL32 tm ) {
		if( (m_ScopeWaveTheta - m_ScopeWaveTargetTheta) > 0.0f) m_ScopeWaveThetaVel -= 0.00001f;
		else	m_ScopeWaveThetaVel += 0.00001f;
		if( ( m_ScopeWaveRho - m_ScopeWaveTargetRho) > 0.0f) m_ScopeWaveRhoVel -= 0.001f;
		else	m_ScopeWaveRhoVel += 0.001f;
		m_ScopeWaveTheta += m_ScopeWaveThetaVel;
		m_ScopeWaveRho += m_ScopeWaveRhoVel;
		m_timeScopeWaveUpdate += tm;
		if( m_timeScopeWaveUpdate < 1.0f)	return;
		m_timeScopeWaveUpdate = 0.0f;
		m_ScopeWaveTargetTheta = i3Math::vsin( i3Math::Randf2()) * 0.05f;
		m_ScopeWaveTargetRho = i3Math::vcos( i3Math::Randf2()) * 0.1f;
	}
} CHARAPLAYER_SCOPEWAVE;

*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Class : CGameCharaPlayer
//	Desc: Local과 Network에서 함께 처리가 필요한것을 담당합니다. (Pivot, Theta, Rho, Camera, Weapon, Collision, Camera, Action 처리)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CGameCharaPlayer : public CGameCharaBase
{
	I3_CLASS_DEFINE(  CGameCharaPlayer);

/////////////////////////////////////////////////	Variables	
protected:
	//	Recoil
	//CHARAPLAYER_RECOIL	m_RecoilData;
	//	Deviation
	//CHARAPLAYER_DEVIATION	m_DeviationData;
	//	Scope
	//CHARAPLAYER_SCOPEWAVE	m_ScopeWaveData;

	/*
	//	Pivot
	bool				m_bValidPivot;
	REAL32				m_PivotYaw;		//	상체 Animation을 전방으로 보정하기 위한 각도
	REAL32				m_PivotYawTarget;
	REAL32				m_timeYawBlend;
	REAL32				m_timeTargetYawBlend;
	REAL32				m_PivotRoll;
	VEC3D				m_PivotAxis;

	MATRIX				m_mtxPivot;
	INT32				m_idxBone[BONE_MAX];
	MATRIX 				m_Bone[BONE_MAX];

	//REAL32				m_rIKHeight;		//

	REAL32				m_Theta;			//	캐릭터 좌우 회전 값
	REAL32				m_Rho;				//	캐릭터 위아래 회전 값

	bool				m_bShoveActive, m_bShoveReturn;
	REAL32				m_velShove, m_acclShove;
	REAL32				m_tShove, m_timeShove;
	QUATERNION			m_quatShoveTarget, m_quatShoveStart, m_quatShove;
	MATRIX				m_ShoveMatrix;
	*/

	//UINT16				m_nCollisionGroup;

	//	Collision
	//I3_PHYSIX_HIT_RESULT m_CrossHitResult;			// 크로스헤어 히트 정보
	//i3CollideeLine		m_CrossHitLine;				// 크로스헤어 히트라인
	//REAL32				m_rCrossHitTimer;			// 크로스헤어 체크 타임
	//CGameCharaPlayer *	m_pCrossHitChara;			// 크로스헤어에 컬리젼된 캐릭터, NULL이면 없다.

	// contact
	//i3List *			m_pContactHitList;			// 시물레이션 시 충돌 후 이팩트 처리용
	
	//BOOL				m_bContactBones[ 6 ];
	//VEC3D				m_vBonesForceDir[ 6 ];
	//UINT32				m_nContactReportCount;
	//UINT32				m_nContactReportReadPos;
	//I3_PHYSIX_CONTACT_REPORT m_ContactReportBuf[ PHYSIX_CONTACT_REPORT_COUNT ];

	//BOOL				m_bRightStepSound;
	//REAL32				m_fRunElapseTime;

	// Local Chara용입니다. 차후에 삭제될 예정
	//	Input Key
	//UINT64				m_KeyPressInput;
	//UINT64				m_KeyStrokeInput;

	//INT32				m_nAccessory;				//액세서리 장비의 스치는 소리인덱스( 랜덤으로 하지 않도록 요청받았기때문)

	//REAL32				m_fRunDuration;				//1


public:
	/// debug


public:
	//----------------------------------------------------------------------------------------------------//
	//										 Inline functions
	//----------------------------------------------------------------------------------------------------//

	//Ladder Climbing
//	bool				CalculateLadderEnable( void * pHit);

//	void				setClimbingLadderEnable( bool bEnable)		
//	{
//		if( bEnable == isStateMask( CHARA_STATE_CLIMB))
//			return;

//		if( isStateMask( CHARA_STATE_JUMP) && bEnable)
//		{
//			_ResetJumping();
//		}
		
//		if( bEnable)
//			addStateMask( CHARA_STATE_CLIMB);
//		else
//			removeStateMask( CHARA_STATE_CLIMB);
//	}

//	bool				getClimbingLadderEnable( void)				{	return m_bClimbingLadder;}

//	void				setLadderContactPosition(REAL32 x, REAL32 y, REAL32 z)		{	i3Vector::Set( m_pMoveContext->getLadderContactPosition(), x, y, z);}
//	VEC3D*				getLadderContactPosition(void)				{	return m_pMoveContext->getLadderContactPosition();}

//	i3Transform2 *		getRoughBound( void )						{ return m_pRoughHitTrans; }
	//void				setVelocity2( REAL32 fVal)					{ m_rVelocity = fVal; }
	//REAL32			getVelocity2( void)							{ return m_rVelocity; }
//	i3List *			getContactHitList( void)					{ return m_pContactHitList; }
//	void				setRunElapseTime(REAL32 fTime)				{ m_fRunElapseTime = fTime;	}
//	REAL32				getRunElapseTime(void)						{ return m_fRunElapseTime;	}

	//	Chara	information
	//CHARA_INFO *		getCharaInfo(void)					{ return m_pCharaInfo;				}
	//CHARATEAM_TYPE		getCharaTeam( void)					{ if( m_pCharaInfo != NULL ){return m_pCharaInfo->_Team;} return CHARATEAM_TYPE_RED; }
	//INT32				getCharaNetIndex( void)				{ if( m_pCharaInfo != NULL ){return (INT32) m_pCharaInfo->_NetIdx;} return -1;	}
	//void				setCharaNetIndex( INT32 nIndex )	{ if( m_pCharaInfo != NULL ){m_pCharaInfo->_NetIdx = (UINT8) nIndex;}	}

	//INT32				getCurHP( void)						{ if( m_pCharaInfo != NULL ){return m_pCharaInfo->_Hp;}else return 0; }
	// 살아 있는지 체크
	//BOOL				isAlive( void)						{ return (getCurHP() > 0);			}
	// 네트워크 캐릭터 체크
	//BOOL				isNetwork( void)					{ if( m_pCharaInfo != NULL ){return (m_pCharaInfo->_Type == CHARA_TYPE_PLAYER_NET);} return CHARA_TYPE_NA;	}
	// 무적 상태 체크
	//BOOL				isInvincible( void)					{ return (m_rInvincibleElapseTime > 0.0f);		}
	
	//CHARAPLAYER_RECOIL * getRecoilData( void)				{ return &m_RecoilData; }

	//REAL32				getTheta( void)						{ return m_Theta;					}
	//void				setTheta( REAL32 fTheta )			{ m_Theta = fTheta;					}	
	//REAL32				getRho( void)
	//{
	//	return m_Rho + I3_DEG2RAD(m_RecoilData.m_RecoilAngle + m_ScopeWaveData.m_ScopeWaveRho);
	//}
	//REAL32				getViewRho( void)					
	//{ 
	//	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	//	if( pWeapon == NULL )	return 0.f;
	//	return m_Rho + I3_DEG2RAD((m_RecoilData.m_RecoilAngle * pWeapon->getCamSyncRate()) + m_ScopeWaveData.m_ScopeWaveRho);	
	//}
	//REAL32				getRhoValue( void)					{ return m_Rho; }
	//void				setRho( REAL32 fRho )				{ m_Rho = fRho;						}
	//MATRIX *			getShoveMatrix(void)				{ return &m_ShoveMatrix; }

	//MATRIX *			getIKMatrix(void)					{ return &m_Bone[ BONE_IK];			}
	//MATRIX *			getBoneMatrix( INT32 idx)			{ return &m_Bone[ idx];				}
	//MATRIX *			getHeadMatrix(void)					{ return &m_Bone[ BONE_HEAD];		}	
	
	//REAL32				getRecoilAngle(void)				{ return m_RecoilData.m_RecoilAngle; }
	//REAL32				getDeviationAngle(void)				{ return m_DeviationData.m_DeviationAngle; }
	//REAL32				getFireDeviation(void)				{ return m_DeviationData.m_FireDeviation; }
	
	//INT32				getBoneIndex( INT32 idx)			{ BOUNDCHK( idx, BONE_MAX); return m_idxBone[ idx]; }

//	VEC3D *				getBonesForceDir( INT32 idx)		{ BOUNDCHK( idx, 6); return &m_vBonesForceDir[ idx]; }

//	REAL32				getDamageElapseTime( void )			{ return m_rDamageElapseTime;		}
//	REAL32				getRespawnElapseTime( void )		{ return m_rRespawnElapseTime; }
//	void				setRespawnElapseTime( REAL32 tm)	{ m_rRespawnElapseTime = tm; }
	
	//	Input 차후에 삭제될 예정
	//UINT64			getKeyPressInput( void)					{ return m_KeyPressInput;		}
	//void			setKeyPressInput( UINT64 key)			{ m_KeyPressInput = key; }
	//UINT64			getKeyStrokeInput( void)				{ return m_KeyStrokeInput;		}
	//void			setKeyStrokeInput( UINT64 key)			{ m_KeyStrokeInput = key; }

	//	Collision stand motion
	//bool				_IsOverlappedBy( UINT32 nGroupIdx);

protected:
	//	Calcurate Processing
	//void				_AdjustPivot( REAL32 tm, MATRIX * pInvMtx);
	//void				_InitBones(void);	
	//void				_calcSpineBones(void);
	//void				_calcTheta( REAL32 tm);			//	캐릭터 좌우 회전 연산
	//void				_calcRho( REAL32 tm);			//	캐릭터 상하 회전 연산

	// 반동 계산
	//void				_calcRecoil( REAL32 tm);			//	사격 반동 연산 
	//void				_calcDeviation( REAL32 tm);
	//void				_calcScopeWave( REAL32 tm);
	    	
	//void				_TransformWorldSpace( REAL32 tm);

	//	Camera
	//void				_CAMERA_SetIdle( void);
	//void				_CAMERA_SetZoom( void);
	//BOOL				_CAMERA_SetScope( INT32 nScopeLevel);
	//void				_CAMERA_UpdateState(void);	

	//	Process
	//void				_ProcessRespawn( REAL32 tm);	
	//void				_ProcessChangePose( RT_REAL32 rDeltaSeconds);
	//void				_ProcessMaterial( REAL32 tm);
	//void				_ProcessScreenEffect( REAL32 tm);

	//void				_UpdateAnim( REAL32 tm);	
	//void				_setStandOn1PV( CHARA_STATE_FOR_CAM nState, REAL32 rTime);

	//void				_DriveLightVolume( REAL32 tm);

	//	HUD
	//BOOL				_CalcProjectionPos( REAL32 * pOutX, REAL32 * pOutY, VEC3D * pIn );	

	//	Sound
//	void				_InitCharaSound( void);
//	void				_InitRunSound( void);
//	void				_InitDamageSound( void);	
//	BOOL				_LoadCharaSound( CHARA_SOUND idx, const char * pszSoundName, const char * pszSoundPath);

	//void				_UpdateCharaSound( REAL32 tm);	
	
	//사다리 타기 계산
	//void				_CalcClimbingLadder( VEC3D * vMoveDir);
	// 오브젝트 타기 계산
	//void				_CalcTakeObject( VEC3D * pFirstPos );

	//	Collision stand motion
	//bool				_IsOverlappedBy( UINT32 nGroupIdx);

	//INT32				_CalcHit( CHARA_HIT_PART part, REAL32 nDamage, REAL32 rValidDamageRate = 1.0f);		
	//REAL32				_CalcDamageLength( CHARA_DAMAGEINFO * pDamageInfo);
	//REAL32				_CalcDamageRate( CHARA_DAMAGEINFO * pDamageInfo, REAL32 rDamagedLength);
	
	//void				_AttachWorldCollision();
	//void				_DetachWorldCollision();
	//void				_CheckWorldCollision( REAL32 tm);
	//void				_CheckWallCollision(REAL32 tm);
	
	//void				_AttachRoughHitCollision();
	//void				_DetachRoughHitCollision();

	//void				_ChangeControllerHeight( REAL32 rTall );

	//void				_UpdateJumping(REAL32 tm);
	//void				_ResetJumping();

	// Contact report
	//void				_QueContactReport( I3_PHYSIX_CONTACT_REPORT * pReport );
	//void				_ParseContactReport( void );

	//void				_PHYSIX_SetLinearVelocity( CHARA_DAMAGEINFO * pInfo );

	//void				_ProcessDownEffect( void);

	//void				_ProcessOldWeapon( void);

public:
	CGameCharaPlayer( void);
	virtual ~CGameCharaPlayer( void);
	//virtual void		Create( i3GameResChara * pRes, CHARA_INFO * pCharaInfo, const CCharaInfo * pDBInfo);	
	//void				Destroy( void);
	//virtual void		OnUpdate( REAL32 tm);
	//virtual void		OnChangeAIState( i3AIContext * pCtx);
	
	//virtual void		ReturnInstance( void);
	//virtual void		ResetChara( void);	
	//virtual void		DumpChara( void);
	//virtual void		ProcessFirst( REAL32 tm);
	//virtual void		ChangeAnimSet( CHARA_ANIMSET_TYPE type, BOOL bAnimSync, BOOL isFixed3PV);
	//virtual void		ChangeAnimSetLowerSync( CHARA_ANIMSET_TYPE type, BOOL isFixed3PV);

	//virtual REAL32		GetSyncTimeLower( void);
	//virtual REAL32		GetSyncTimeUpper( void);

	//void				CreateInLobby( i3GameResChara * pRes, CHARA_INFO * pCharaInfo, const CCharaInfo * pDBInfo);

	//void				ResetDefaultAnim( BOOL bIs3PV);
	//void				ResetDefaultInfo( void);
	//void				ResetLobbyAnim(void);

	//void				SetCollisionGroup( UINT16 nGroup, UINT32 nMask);
	//void				EnableCollisionGroup( BOOL bEnable = FALSE);

	//void				AttachCollisionNode( i3Node * pParentNode, UINT32 nColGroupIndex);
	//void				DetachCollisionNode();

	// check Hit part from character shapeset
	//CHARA_HIT_PART		GetHitPart( i3PhysixShapeSet* pDestShapeSet);
	//void				calcLOD( VEC3D * pPos, VEC3D * pCamPos, REAL32 fFOVRatio = 1.0f);

	//void				Shove( VEC3D * pDir, REAL32 force);

	//void				ProcessMissionEnd( void );

	//BOOL				CharaHit( WeaponBase * pShooterWeapon);
	//BOOL				CharaHit( CHARA_DAMAGEINFO * pDamageInfo);
	//void				SyncWeapon( void);

	//void				ProcessMovement( REAL32 tm );

	//	Action
	//BOOL				ACTION_Respawn( VEC3D * pStartPos);

	//void				ACTION_TurnBack( void);
	// 자세에 관한 동작
	//BOOL				ACTION_Stand( void);
	//BOOL				ACTION_Crouch( void);
	//BOOL				ACTION_ToggleIdle( void);	
	//BOOL				ACTION_MoveLower( VEC3D * pMoveDir = NULL);
	//BOOL				ACTION_MoveUpper( VEC3D * pMoveDir = NULL);
	//BOOL				ACTION_WalkLower( VEC3D * pMoveDir = NULL);
	//BOOL				ACTION_IdleLower( void);
	//BOOL				ACTION_IdleUpper( void);
	//BOOL				ACTION_Jump( void);
	//BOOL				ACTION_JumpEndUpper( void );			// 네트워크를 위해 추가 by 정순구 2006. 11. 13
	//BOOL				ACTION_JumpEndLower( void );

	// 강제 동작
	//BOOL				ACTION_Damage( CHARA_HIT_PART part, VEC3D * pvHitDir);
	//BOOL				ACTION_Death( VEC3D * pHitDir, CHARA_DEATH_TYPE type, INT32 nKillIdx);	

	// 무기에 관한 동작
	//BOOL				ACTION_Fire( VEC3D * pTargetPos = NULL, WEAPON_FIRE_ORDER type = WEAPON_PRIMARY_FIRE);
	//void				ACTION_GhostFire( WEAPON_FIRE_ORDER type = WEAPON_PRIMARY_FIRE);	// 모션만 취합니다.
	//BOOL				ACTION_FireReady( void);
	//BOOL				ACTION_ExtensionWeapon( BOOL bActivate);
	//BOOL				ACTION_ToggleExtensionWeapon( void);
	//BOOL				ACTION_ActiveZoom( void);
	//BOOL				ACTION_CancelZoom( void);
	//BOOL				ACTION_ToggleZoom( void);
	//void				ACTION_SetObserveZoom(void);
	//BOOL				ACTION_SwapWeapon( INT32 slot);
	//BOOL				ACTION_SwapNextWeapon( void);
	//BOOL				ACTION_SwapPrevWeapon( void);
	//BOOL				ACTION_SwapOldWeapon( void);
	//void				ACTION_SyncWeapon( void);
	// 장전 관련 동작
	//BOOL				ACTION_LoadMagazine( void);
	//BOOL				ACTION_LoadMagazineDual( void );
	//BOOL				ACTION_LoadBullet( void);	

	// 폭파 미션
	//BOOL	ACTION_ActiveMission( WeaponBase * pWeapon);
	//BOOL	ACTION_ActiveDisarm( void);
	//BOOL	ACTION_CancelMission( void);
	//BOOL	ACTION_CancelDisarm( void);

	// 히트 관련
	//void				ACTION_FallDamage( void);
	//BOOL				ACTION_Hit( WeaponBase * pShooterWeapon);
	//BOOL				ACTION_Hit( CHARA_DAMAGEINFO * pDamageInfo);	

	//BOOL				ACTION_DropEquip( EQUIP_CLASS_TYPE type, i3Node * pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pPos = NULL );
	//BOOL				ACTION_DropWeapon( INT32 nSlot, VEC3D * pDropPos);
	//BOOL				ACTION_ReturnToReady( BOOL bWithCurrentWeapon);	// 기본 상태로 세팅합니다.
	//BOOL				ACTION_Invincible( BOOL bVal );					// 무적 타임 세팅

	// UI Action	Lobby에서만 사용합니다.
	//BOOL				ACTION_NoneBattleIdle( void);
	//BOOL				ACTION_NoneBattleWeaponAction( void);
	
    
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	Weapon
	//BOOL				WEAPON_Add( INT32 nSlot, WEAPON_CLASS_TYPE classType, INT32 number);
	//void				WEAPON_Delete( INT32 nSlot);
	//void				WEAPON_Attach( INT32 nSlot, i3Node * pSceneNode, i3GameNode * pGameNode );
	//void				WEAPON_Detach( INT32 nSlot);
	//BOOL				WEAPON_Select( INT32 nSlot);
	//BOOL				WEAPON_IsSame( WeaponBase * pSrc, WeaponBase * pDst);

	//WeaponBase*			WEAPON_getBackpack( void )						{ return m_pPurchasePrimWeapon; }
	//BOOL				WEAPON_AttachToBackpack( INT32 nSlot);
	//void				WEAPON_DetachFromBackpack( void);

	//WeaponBase*			WEAPON_getSidepack( void )						{ return m_pPurchaseSideWeapon; }
	//BOOL				WEAPON_AttachToSidepack( INT32 nSlot);
	//void				WEAPON_DetachFromSidepack( void);
	
	//BOOL				WEAPON_SetBlendAlpha( INT32 nSlot, REAL32 fVal);
	//void				WEAPON_SetBlendAlphaAll( REAL32 fVal);	
	//BOOL				WEAPON_SetMaterialColor( INT32 nSlot, COLORREAL * pfDiffuse, COLORREAL * pfSpecular);
	//void				WEAPON_SetMaterialColorAll( COLORREAL * pfDiffuse, COLORREAL * pfSpecular);

	//BOOL				WEAPON_DropToWorld( INT32 nSlot, VEC3D * pPos = NULL);	//	월드에 붙는다.
	//void				WEAPON_Take( WeaponBase * pWeapon );					//	총을 줍는다.
	//void				WEAPON_CheckTaking( REAL32 rDeltaTime );
	//void				WEAPON_validCheck( WEAPON_CLASS_TYPE nClassType, INT32 WeaponNum );
	
	//	Equip
	//BOOL				EQUIP_DropToWorld( EQUIP_CLASS_TYPE equipType, VEC3D * pHitDir );
	//BOOL				EQUIP_Add( EQUIP_CLASS_TYPE type, INT32 number);
	//BOOL				EQUIP_Set( CGameCharaEquip * pEquip);	
	//BOOL				EQUIP_AttachChara( EQUIP_CLASS_TYPE type, BOOL bReset = FALSE);
	//BOOL				EQUIP_Attach( CGameCharaEquip * pEquip);
	//CGameCharaEquip *	EQUIP_Detach( EQUIP_CLASS_TYPE type);
	//CGameCharaEquip *	EQUIP_Get( EQUIP_CLASS_TYPE type)										{ I3ASSERT(type<EQUIP_CLASS_TYPE_COUNT); return m_pCharaEquip[ type];}
	//BOOL				EQUIP_Delete( EQUIP_CLASS_TYPE type);
	//void				EQUIP_SetVisibleAll( BOOL bEnable, BOOL bAttachedOnChara);
	//BOOL				EQUIP_IsAttachedOnChara( EQUIP_CLASS_TYPE type);
	//BOOL				EQUIP_IsAttachedOnChara( CGameCharaEquip * pEquip);
	//BOOL				EQUIP_SetBlendAlpha( EQUIP_CLASS_TYPE type, REAL32 fVal, BOOL bAttachedOnChara);
	//void				EQUIP_SetBlendAlphaAll( REAL32 fVal, BOOL bAttachedOnChara);
	//BOOL				EQUIP_SetMaterialColor( EQUIP_CLASS_TYPE type, COLORREAL * pfDiffuse, COLORREAL * pfSpecular, BOOL bAttachedOnChara);
	//void				EQUIP_SetMaterialColorAll( COLORREAL * pfDiffuse, COLORREAL * pfSpecular, BOOL bAttachedOnChara);
	//void				EQUIP_CheckVisible( void);
	// Equip Info
	//void				EQUIP_setInfo( EQUIP_CLASS_TYPE equipType, CEquipInfo * pInfo);
	//CEquipInfo *		EQUIP_getInfo( EQUIP_CLASS_TYPE equipType);
	
	//	Control
	//void				EnableControl( BOOL bVal = TRUE);
	//void				EnableCollision( BOOL bVal = TRUE );
	//void				Move( REAL32 tm );
	//void				MoveStop( void);
	//virtual void		forceSetPos( VEC3D * pPos);

	//	Camera
	//void				ChangeFPSView(BOOL bPlayAnimSync = TRUE);
	//void				ChangeObserverView(BOOL bPlayAnimSync = TRUE);
	//void				SetViewerChara( BOOL bEnable = TRUE);	//	뷰어 또는 GUI용 3인칭 캐릭터로 설정
	
	//	Collision
	//BOOL				CalcHelmetPenetrate( CHARA_DAMAGEINFO * pDamageInfo );
	//INT32				OnHit( CHARA_DAMAGEINFO * pDamageInfo);
	//BOOL				NET_Hit( CHARA_HIT_PART part, CHARA_DAMAGEINFO * pDamageInfo, REAL32 rValidDamageRate );
	//void				CheckFloorTerrain();					// 바닥면의 제질 체크

	//void				setSoundOwner( void);
	//void				DeleteBoundBox( void);					//	컬링을 제거한다. (복구 불가)
	

	//	HUD
	//void				SetNickOnTarget();
	//void				CheckCrossHit( );
	//void				SetCharaNick(BOOL bFriendly);
	//BOOL				ApplyCrosshair( void);

	//	Sound
	//bool				PlayFootStep( I3_TERRAIN_TYPE nType, bool bLeftfoot, BOOL bListener);

	//BOOL				PlayCharaSound( CHARA_SOUND idx, BOOL bImmediately = FALSE);
	//BOOL				PlayCharaDamageSound( CHARA_HIT_PART part, BOOL bImmediately);
//	void				StopCharaSoundAll( void);
//	void				StopCharaSound( CHARA_SOUND idx);

	//virtual void		OnInitVariable( void);

	//	Anim (CallBack)
	//virtual void		OnMoveRun( i3AIContext * pCtx, REAL32 tm);
	//virtual void		OnAttackRun( i3AIContext *pCtx, REAL32 tm);
	//virtual void		OnAttackReadyFinish( i3AIContext * pCtx, REAL32 tm);
	//virtual void		OnJumpEndFinish( i3AIContext * pCtx, REAL32 tm);
	//virtual void		OnLoadBulletRun( i3AIContext * pCtx, REAL32 tm);
	//virtual void		OnDamageRun( i3AIContext * pCtx, REAL32 tm);
	//virtual void		OnDeathFinish( i3AIContext * pCtx, REAL32 tm);
	//virtual void		OnExtensionActivateRun( i3AIContext * pCtx, REAL32 tm);
	//virtual void		OnExtensionDeactivateRun( i3AIContext * pCtx, REAL32 tm);
	//virtual void		OnUI_IdleAFinish( i3AIContext * pCtx, REAL32 tm);
	//virtual void		OnUI_IdleBRun( i3AIContext * pCtx, REAL32 tm);
	//virtual void		OnUI_IdleBFinish( i3AIContext * pCtx, REAL32 tm);

	// Event
	//virtual BOOL	OnEvent( UINT32 event, i3ElementBase *pObj, UINT32 param );

	//REAL32				AnimUpperScale( ID_UPPER_AI ai, CWeaponInfo * pWeaponInfo, REAL32 time);

	
	//void				ProcessTimer( REAL32 tm);	
	//void	SetMoveDir( UINT64 key);

	//INT32				CalcFallDamage( VEC3D * pTakeOffPos);
};

#endif

