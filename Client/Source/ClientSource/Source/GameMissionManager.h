/**
 * \file	GameMissionManager.h
 *
 * Declares the game mission manager class.
 */

#include "CSI_TypeDef.h"

#if !defined( __GAME_MISSION_MANAGER_H__ )
#define __GAME_MISSION_MANAGER_H__


enum GAME_MISSION_STATE
{
	GAME_MISSION_STATE_NONE = 0,				//	설치미션			//
	GAME_MISSION_STATE_PREBATTLE,				//  게임 준비 단계
	GAME_MISSION_STATE_INBATTLE,				//	설치전				//
	GAME_MISSION_STATE_INSTALLED,				//	설치 후				// only Blasting
	GAME_MISSION_STATE_UNINSTALLED,				//	해체 후				// only Blasting
	GAME_MISSION_STATE_ACTIVEOBJECT,			//	폭탄 터짐			// only Blasting
	GAME_MISSION_STATE_ENDBATTLE,				//	라운드종료			//
	GAME_MISSION_STATE_GAMEOVER,				//	미션 종료			//
	GAME_MISSION_STATE_DEACTIVEOBJECT,			//	폭탄 해체	
	GAME_MISSION_STATE_COUNT
};

enum MY_MISSION_OBJECT
{
	MY_MISSOIN_OBJECT_NONE = 0,
	MY_MISSION_OBJECT_OFFENSIVE,
	MY_MISSION_OBJECT_DEFENSIVE,
	MY_MISSION_OBJECT_COUNT
};

enum GAME_RESPAWN_MODE
{	
	GAME_RESPAWN_MODE_1LIFE = 0,
	GAME_RESPAWN_MODE_3LIFE,
	GAME_RESPAWN_MODE_10LIFE,
	GAME_RESPAWN_MODE_NOLIMIT,
};

struct MISSION_DATA_DESTRUCTION
{
	i3Object *				m_pObjectBlue = nullptr;
	i3Object *				m_pObjectRed = nullptr;
	UINT16					m_intervalRateBlue = 0;							// 오브젝트에 대한 1%의 데미지 량
	UINT16					m_intervalRateRed = 0;							// 
	DESTRUCTION_INFO		m_ObjectRecvDamageInfo;						// Recv용
	DESTRUCTION_INFO		m_ObjectSendDamageInfo;						// only Host (Send용)
	INT32					m_ObjectIntervalIdx[SLOT_MAX_COUNT] = { 0 };
	INT32					m_RedObjectIntervalIdx = 0;
	INT32					m_BlueObjectIntervalIdx = 0;

public:
	void		reset( void)
	{
		INT32 i;

		m_ObjectRecvDamageInfo.Reset();
		m_ObjectSendDamageInfo.Reset();

		m_RedObjectIntervalIdx		= 1;
		m_BlueObjectIntervalIdx		= 1;

		if( m_pObjectRed != nullptr )
		{	
			m_ObjectSendDamageInfo._nRedObjectHP = m_ObjectRecvDamageInfo._nRedObjectHP = (UINT16) m_pObjectRed->getMaxHP();
		}

		if( m_pObjectBlue != nullptr )
		{
			m_ObjectSendDamageInfo._nBlueObjectHP = m_ObjectRecvDamageInfo._nBlueObjectHP = (UINT16) m_pObjectBlue->getMaxHP();
		}

		for( i = 0; i < SLOT_MAX_COUNT; i++)
		{
			m_ObjectIntervalIdx[ i]			= 1;
		}
	}

	i3Object * getRedObject(){return m_pObjectRed;}
	i3Object * getBlueObject(){return m_pObjectBlue;}
} ;

struct MISSION_DATA_DEFENCE
{
private :
	i3Object *				m_pDefenceObj[MAX_OBJECT_COUNT] = { nullptr };
	UINT16					m_uIntervalRateObj[MAX_OBJECT_COUNT] = { 0 };
	DEFENCE_INFO			m_DefenceObj_RecvDamageInfo;				// recv
	DEFENCE_INFO			m_DefenceObj_SendDamageInfo;				// send ( only Host)
	
	INT32					m_ObjectIntervalIdx[MAX_OBJECT_COUNT][SLOT_MAX_COUNT] = { 0 };

	bool					m_bIsAttackingObject[SLOT_MAX_COUNT] = { false };		///< Object 공격자 체크
	REAL32					m_rDetectedAttackerTime[SLOT_MAX_COUNT] = { 0.0f };

public:
	void		reset( void)
	{
		INT32 i, j;

		m_DefenceObj_RecvDamageInfo.Reset();
		m_DefenceObj_SendDamageInfo.Reset();

		for( i = 0; i < SLOT_MAX_COUNT; i++)
		{
			m_bIsAttackingObject[i] = false;
			m_rDetectedAttackerTime[i] = 3.f;
		}

		for( i = 0; i < MAX_OBJECT_COUNT; i++)
		{
			for( j = 0; j < SLOT_MAX_COUNT; j++)
			{
				m_ObjectIntervalIdx[i][j]		= 1;
			}

			if( m_pDefenceObj[ i] != nullptr)
			{
				UINT16 hp = (UINT16) m_pDefenceObj[ i]->getMaxHP();

				m_DefenceObj_SendDamageInfo.setObjectHP( i, hp);
				m_DefenceObj_RecvDamageInfo.setObjectHP( i, hp);
			}
		}
	}

	i3Object *			getDefenceObj( INT32 idx)
	{
		I3_BOUNDCHK( idx, MAX_OBJECT_COUNT);

		return m_pDefenceObj[ idx];
	}
	void				setDefenceObj( INT32 idx, i3Object * pObj)
	{
		I3_BOUNDCHK( idx, MAX_OBJECT_COUNT);

		m_pDefenceObj[ idx] = pObj;
	}

	UINT16				getIntervalRateObject( INT32 idx)
	{
		I3_BOUNDCHK( idx, MAX_OBJECT_COUNT);

		return m_uIntervalRateObj[ idx];
	}

	void				setIntervalRateObject( INT32 idx, UINT16 rate)
	{
		I3_BOUNDCHK( idx, MAX_OBJECT_COUNT);

		m_uIntervalRateObj[ idx] = rate;
	}


	DEFENCE_INFO *		getRecvDamageInfo( void)		{ return &m_DefenceObj_RecvDamageInfo; }

	DEFENCE_INFO *		getSendDamageInfo( void)		{ return &m_DefenceObj_SendDamageInfo; }

	INT32				getObjIntervalIndex( INT32 iObjectIdx, INT32 idx)
	{
		I3_BOUNDCHK( iObjectIdx, MAX_OBJECT_COUNT);
		I3_BOUNDCHK( idx, SLOT_MAX_COUNT);

		return m_ObjectIntervalIdx[ iObjectIdx][ idx];
	}

	void				setObjIntervalIndex( INT32 iObjectIdx, INT32 idx, INT32 intervalIdx)
	{
		I3_BOUNDCHK( iObjectIdx, MAX_OBJECT_COUNT);
		I3_BOUNDCHK( idx, SLOT_MAX_COUNT);

		m_ObjectIntervalIdx[ iObjectIdx][ idx] = intervalIdx;
	}

	bool	isAttackingObject( INT32 idx)
	{
		I3_BOUNDCHK( idx, SLOT_MAX_COUNT);
		return m_bIsAttackingObject[ idx];
	}

	void	setAttackingObjectUser( INT32 idx, bool bAttack)
	{
		I3_BOUNDCHK( idx, SLOT_MAX_COUNT);
		m_bIsAttackingObject[ idx] = bAttack;

		m_rDetectedAttackerTime[ idx] = 3.f;	///< BattleHUD_TargetHP.cpp DETECT_ENEMY_UPDATE_TIME 3초로 설정
	}

	REAL32	getAttackerTime( INT32 idx)
	{
		I3_BOUNDCHK( idx, SLOT_MAX_COUNT);
		return m_rDetectedAttackerTime[ idx];
	}

	void	setAttackerTime( INT32 idx, REAL32 tm)
	{
		I3_BOUNDCHK( idx, SLOT_MAX_COUNT);
		m_rDetectedAttackerTime[ idx] = tm;
	}

} ;

struct MISSION_DATA_USURPATION {
	i3Object *			_MissionObj = nullptr;

	UINT8				m_iUsurpationIdx = 0xFF;
	bool				m_bMyUsurpationState = false;
	
	void	Reset( void) {
		_MissionObj = nullptr;
		m_iUsurpationIdx = 0xFF;
		m_bMyUsurpationState = false;
	}
};

class CGameObjectManager;

class CGameMissionManager : public i3::shared_ginst<CGameMissionManager>
{
private:
	GAME_MISSION_STATE	m_MissionState;
	MY_MISSION_OBJECT	m_MyMissionObject;
	GAME_RESPAWN_MODE	m_RespawnMode;

	VEC3D				m_rMissionObjPos;

	MISSION_DATA_USURPATION	m_UsurpationData;

	i3::vector<VEC3D>	m_RunAwayObjPos;

	// Test-Automation을 위해 추가된 멤버
	// 이건 Round의 종료 상태를 저장해 둡니다. (어떤 이유로 Round가 종료되었는지, 누가 이겼는지 정보를 확인하기 위함)
	MISSION_END_TYPE	m_LastRoundWinType;
	TEAM_RESULT_TYPE	m_LastRoundWinTeam;

	/////////////////////////////////////////////////////////////////////////////////////////////
	// variable at Blasting mission mode
	bool			m_bEnableMyArea;				// 설치 지역 가능,
	bool			m_bEnableMyCrossHit;			// 해체 방향 가능
	
	bool			m_bSendMsgBombInstall;			// 설치된 메세지 Send 여부
	bool			m_bSendMsgBombUninstall;		// 해체된 메세지 Send 여부
	
	REAL32			m_fExplosionTimer;				// Bomb Explosion timer;		// 폭탄 타이머
	INT32			m_nWhoInstalledIdx;				// 설치자 0 ~ 15 (-1이면 설치하지 않은 상태)

	REAL32			m_fBombElapsedKeyTime;
	REAL32			m_fBombInstallingTimer[ SLOT_MAX_COUNT ];
	REAL32			m_fBombUninstallingTimer[ SLOT_MAX_COUNT ];

	//-----------------------------------------------------------------------------------------------
	//같은 변수 입니다. 한셋은 삭제 가능합니다. 
	bool			m_bIsInstalling		[ SLOT_MAX_COUNT ];
	bool			m_bIsUninstalling	[ SLOT_MAX_COUNT ];
	bool			m_bWhoInstall		[ SLOT_MAX_COUNT ];	// Battle ( Round )
	bool			m_bWhoUninstall		[ SLOT_MAX_COUNT ];

	REAL32			m_rWhoInstallCheckTime	[ SLOT_MAX_COUNT ];	// for only CS
	REAL32			m_rWhoUninstallCheckTime[ SLOT_MAX_COUNT ];	// for only CS

	bool			m_bInstalledArea	[ MAX_BOMB_AREA_COUNT ];
	bool			m_bInstallingArea	[SLOT_MAX_COUNT][ MAX_BOMB_AREA_COUNT ];

	i3SoundPlayState	*	m_pUnInstallSound;
	INT32					m_nCurrentUninstallUserIndex;

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// variable at Destruction Mission mode

	bool					m_bUH60DustEffectReset;
	MISSION_DATA_DESTRUCTION *	m_pDestructionData;
	

	bool					m_bObjectSend;
	REAL32					m_rSendTimerForObject;						// Send Timer
	bool					m_bOpenedLocationOfBomb;					//미션전에서 블루팀의 누군가가 폭탄이 설치되어있는 위치를 발견하였는지 여부( false : 발견안됨) 필요없는 변수임.

	UINT32					m_nRespawnTotalCount;
	UINT32					m_nRespawnCurrentCount[ SLOT_MAX_COUNT];
	

	// 스테이지별 리스폰 타임
	REAL32					m_rRespawnTime;
	REAL32					m_rRespawnTime4vs4;
	REAL32					m_rInvincibleTime;
	REAL32					m_rInvincibleTime4vs4;

	//-----------------------------------------------------------------------------------------//
	//	Defence Mission Mode
	MISSION_DATA_DEFENCE *	m_pDefenceData;

	//	난입 체크 (난입시 Round Start Method로 들어가는것 방지)
	bool					m_bResetObj;
	bool					m_bMyMissionState;	//현재 미션중인지를 체크함(미션설치변수가 취소가 되었지만 각종 플레그들은 취소가 아니라고 나와서 변수를 두고 이를

public:
	GAME_MISSION_STATE	getMissionState( void )			{ return m_MissionState; }
	void				setMissionState( GAME_MISSION_STATE state )		{ I3_BOUNDCHK( state, GAME_MISSION_STATE_COUNT); m_MissionState = state; }

	MY_MISSION_OBJECT	getMyMissionObject(void)		{ return m_MyMissionObject; }
	void				setMyMissionObject( MY_MISSION_OBJECT objectType )	{ I3_BOUNDCHK( objectType, MY_MISSION_OBJECT_COUNT); m_MyMissionObject = objectType; }

	bool				IsPossibleDisarmBomb();

	GAME_RESPAWN_MODE	getRespawnMode( void)			{ return m_RespawnMode; }
	void			setRespawnMode( GAME_RESPAWN_MODE mode)
	{
		m_RespawnMode = mode;
		switch( mode)
		{
		case GAME_RESPAWN_MODE_1LIFE :		m_nRespawnTotalCount = 1;		break;
		case GAME_RESPAWN_MODE_3LIFE :		m_nRespawnTotalCount = 3;		break;
		case GAME_RESPAWN_MODE_10LIFE :		m_nRespawnTotalCount = 10;		break;
		case GAME_RESPAWN_MODE_NOLIMIT :	m_nRespawnTotalCount = 0;		break;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// Blasting mode function
	bool			IsEnableMy( void )							{ return m_bEnableMyArea; }
	bool			IsEnableCrossHit( void )					{ return m_bEnableMyCrossHit; }
	void			setEnableCrossHit( bool bEnable );			// 오브젝트에 LayCast 된 경우

	bool			IsInstalling( INT32 idx )					{ I3_BOUNDCHK( idx, SLOT_MAX_COUNT); return m_bIsInstalling[ idx ]; }
	bool			IsUninstalling( INT32 idx )					{ I3_BOUNDCHK( idx, SLOT_MAX_COUNT); return m_bIsUninstalling[ idx ]; }
	bool			WhoInstall( INT32 idx )						{ I3_BOUNDCHK( idx, SLOT_MAX_COUNT); return m_bWhoInstall[idx]; }
	bool			WhoUninstall( INT32 idx )					{ I3_BOUNDCHK( idx, SLOT_MAX_COUNT); return m_bWhoUninstall[idx]; }
	BOMB_AREA		WhoInstallingArea( INT32 idx)				
	{
		I3_BOUNDCHK( idx, SLOT_MAX_COUNT);
		for( INT32 i = 0; i < MAX_BOMB_AREA_COUNT; i++)
		{
			if( m_bInstallingArea[ idx][ i] )
				return (BOMB_AREA) i;
		}
		return BOMB_AREA_UNKNOWN;
	}

	REAL32			getInstallingTimer( INT32 idx )				{ I3_BOUNDCHK( idx, SLOT_MAX_COUNT); return m_fBombInstallingTimer[ idx ]; }
	REAL32			getUninstallingTimer( INT32 idx )			{ I3_BOUNDCHK( idx, SLOT_MAX_COUNT); return m_fBombUninstallingTimer[ idx ]; }

	REAL32			getExplosionTimer( void )					{ return m_fExplosionTimer; }
	void			setExplosionTimer( REAL32 rTime )			{ m_fExplosionTimer = rTime; }

	bool			IsInstalledArea( INT32 idx )				{ I3_BOUNDCHK( idx, MAX_BOMB_AREA_COUNT); return m_bInstalledArea[idx]; }
	void			setInstalledArea( UINT8 idx )				{ I3ASSERT_RETURN( idx < MAX_BOMB_AREA_COUNT); m_bInstalledArea[idx] = true; }

	INT32			getWhoInstalled( void )						{ return m_nWhoInstalledIdx; }

	bool			isOpenedLocationOfBomb(void)				{ return m_bOpenedLocationOfBomb; }
	void			setOpenLocationOfBomb( bool bOpen = true)	{ m_bOpenedLocationOfBomb = bOpen; }		//로칼 왜 만들었나요? ....필요없는 변수임.

	UINT8			getUsurpationIdx( void )					{ return m_UsurpationData.m_iUsurpationIdx; }
	void			setUsurpationIdx( UINT8 idx )				
	{
		if(m_UsurpationData.m_iUsurpationIdx != idx)
		{
			m_UsurpationData.m_iUsurpationIdx = idx; 
			MsgHud( idx);
		}
		
	}
	void MsgHud(INT32 idx);
	bool			isMyUsurpatio()								{ return m_UsurpationData.m_bMyUsurpationState; }

	const VEC3D* 	getMissionObjPos( void )					{ return &m_rMissionObjPos; }
	VEC3D*			setMissionObjPos(void)						{ return &m_rMissionObjPos; }
	void			CopyMissionObjPos( VEC3D * pos)
	{
		i3Vector::Copy( &m_rMissionObjPos, pos);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// Destruction Mode function
	i3Object			*	getBlueObject( void)
	{
		if( m_pDestructionData != nullptr)
			return m_pDestructionData->m_pObjectBlue;

		return nullptr;
	}
	void					setBlueObject( i3Object * pObj);
	i3Object			*	getRedObject( void)
	{
		if( m_pDestructionData != nullptr)
			return m_pDestructionData->m_pObjectRed;

		return nullptr;
	}
	void					setRedObject( i3Object * pObj);

	void					setRunAwayObject( i3Object * pObj);

	MISSION_DATA_DESTRUCTION * getDestructionData( void)	{ return m_pDestructionData; }

	DESTRUCTION_INFO	*	getDestructionInfo( void)
	{
		I3ASSERT( m_pDestructionData != nullptr);
		return &m_pDestructionData->m_ObjectRecvDamageInfo;
	}

	/** \brief 파괴미션 정보를 설정합니다.
		\note HUD표시를 합니다. 점수 표시 */
	void					setDestructionInfo( DESTRUCTION_INFO * pInfo);

	/** \brief HUD 표시 없이 정보를 설정합니다..
		\note 난입시 호출됩니다.		*/
	void					setDestructionInfoNoEffect( DESTRUCTION_INFO * pInfo);

	/** \brief 총 Respawn count를 반환합니다. */
	UINT32					getTotalRespawnCount( void)				{ return m_nRespawnTotalCount; }

	/** \brief 해당 User의 Respawn count를 반환합니다. */
	UINT32					getRespawnCount( INT32 idx)				{ return m_nRespawnCurrentCount[ idx]; }

	/** \brief 해당 User의 Respawn Count를 설정합니다. */
	void					setRespawnCount( INT32 idx, UINT32 count)	{ m_nRespawnCurrentCount[ idx] = count; }

	/****************************************************************************/
	/*								Defence Mission mode						*/

	/** \brief 방어 오브젝트 정보를 반환합니다. */
	i3Object *				getDefenceObj( INT32 idx)
	{
		if( m_pDefenceData != nullptr)
		{
			I3_BOUNDCHK( idx, MAX_OBJECT_COUNT);
			return m_pDefenceData->getDefenceObj( idx);
		}

		return nullptr;
	}

	/** \brief 방어 오브젝트 정보를 설정합니다. */
	void					setDefenceObj( INT32 idx, i3Object * pObj);

	/** \brief 방어 미션 정보를 반환합니다. */
	MISSION_DATA_DEFENCE * getDefenceData( void)	{ return m_pDefenceData; }

	/** \brief 방어 오브젝트 정보를 설정하고 HUD 점수를 표시합니다. */
	void					setDefenceInfo( DEFENCE_INFO * pInfo);

	/** \brief 방어 오브젝트 정보를 반환합니다.
		\note 네트워크로 받은 정보입니다.		*/
	DEFENCE_INFO	*		getDefenceInfo( void)
	{
		I3ASSERT( m_pDefenceData != nullptr);
		return m_pDefenceData->getRecvDamageInfo();
	}

	/** \brief 방어 오브젝트 정보를 설정합니다.
		\note HUD 점수를 표시하지 않습니다. 난입시 사용 */
	void					setDefenceInfoNoEffect( DEFENCE_INFO * pInfo);

	// revision 43552
	void					setDefenceInfoIntrude( DEFENCE_INFO * pInfo, UINT32 SlotIndex );	// 난입자 데이터 리셋

	/** \brief 유저가 Object를 공격중인지 확인합니다. */
	bool			isAttackingMissionObj( INT32 idx)
	{
		if( m_pDefenceData != nullptr)
			return m_pDefenceData->isAttackingObject( idx);

		return false;
	}

	// 난입 체크 설정
	void			SetResetObjFlag(bool bFlag)	{ m_bResetObj = bFlag;}
	bool			IsResetobj() { return m_bResetObj;}

protected:
	void			_ProcessInBattleForBlastMode( REAL32 rDeltatime);
	void			_ProcessInstalledForBlastMode( REAL32 rDeltatime);
	void			_ProcessInBattleForDestructionMode( REAL32 rDeltatime);
	void			_ProcessInBattleForConvoyEventMode( REAL32 rDeltatime);
	void			_ProcessInBattleForConvoyMode( REAL32 rDeltatime);
	void			_ProcessInBattleForRunAwayMode( REAL32 rDeltatime);

public:
	CGameMissionManager();
	virtual ~CGameMissionManager();

	void			InitializeObject( STAGE_MODE type);

	void			WhoInstallReset();
	void			WhoUninstallReset();
	void			Reset( void );

	// C4 설치를 시작 또는 취소 네트워크 상태를 세팅합니다.
	void			Install( INT32 idx, BOMB_AREA area, bool active );

	// C4 해체를 시작 또는 취소 네트워크 상태를 세팅합니다.
	void			Uninstall( INT32 idx, bool active );

	void			OnUpdate( REAL32 rDeltatime);

	void			StartMission( CSI_STAGE* stage);
	void			EndMission( void);

	// MyCharacter Mission commande
	void			SendMissionInstall( bool active );
	void			SendMissionUninstall( bool active);
	void			SendInstallComplete( INT32 idx );
	void			SendUninstallComplete( INT32 idx );


	//Key:EscapeMission
	void			SendTouchDown(INT32 idx);
	//
	// 미션 시작 준비중인 경우 호출합니다.
	void			ProcessMissionPreStart( void);

	// 미션 시작시 호출해줍니다.
	void			ProcessMissionStart( void );

	// 설치완료되면 호출해줍니다.
	void			ProcessInstalled( void );

	// 해체완료되면 호출해줍니다.
	void			ProcessUninstalled( void );
	
	/** \brief Mission Object에 대한 Damage를 계산합니다.
		\note 해당 유저가 준 Damage를 계산하여 GameServer에 보냅니다. */
	void			DamageObjectForUser( INT32 idx, REAL32 damage, i3Object * pObj, bool bSend = false);

	// User가 나가면 호출합니다.
	// User가 설정했던 Mission Data를 초기화합니다.
	void			GiveupBattle( INT32 idx);

	/** \brief Respawn 처리입니다. */
	bool			ProcessCharaRespawn( INT32 idx);

	void			ProcessUninstallingSound( REAL32 rDeltaSeconds);		//내부

	void			ClearUninstallingSound( void);

	/** \brief 무적시간을 반환합니다.
		\note Stage Mode에 따라 반환값이 다릅니다. 		*/
	REAL32			GetInvincibleTime( CSI_STAGE* stage ) const;

	/** \brief 일반 모드에서의 무적시간을 설정합니다. */
	void			setInvincibleTime( REAL32 rInvincibleTime)		{ m_rInvincibleTime = rInvincibleTime; }

	/** \brief 4대4 모드에서의 무적시간을 설정합니다. */
	void			setInvincibleTime4vs4( REAL32 rInvincibleTime)	{ m_rInvincibleTime4vs4 = rInvincibleTime; }

	/** \brief Respawn Time을 반환합니다.
		\note Stage Mode에 따라 반환값이 다릅니다.
		\param[in] iStageID StageMode
		\param[in] userIdx User Slot Idx */
	REAL32			GetRespawnTime( CSI_STAGE* stage, INT32 userIdx) const;

	/** \brief 일반 모드에서의 Respawn time을 설정합니다. */
	void			setRespawnTime( REAL32 rRespawnTime)			{ m_rRespawnTime = rRespawnTime; }

	/** \brief 4대4 모드에서의 Respawn Time을 설정합니다. */
	void			setRespawnTime4vs4( REAL32 rRespawnTime )		{ m_rRespawnTime4vs4 = rRespawnTime; }

	/** \brief Respawn Time Reset */
	void			LoadPropertiesByStage();

	// 연합 미션 관련..
public:
	void			AddUserDamage(INT32 iKiller);

	/** \brief Mission Object인지 확인합니다. */
	bool			isMissionObject( i3Object * pObj);

	bool			CheckCrownMinimap(void);
	bool			isMyMission(){ return m_bMyMissionState; }

	//
	// Test-Automation을 위해 추가된 멤버
	void			setLastRoundResult(MISSION_END_TYPE winType, TEAM_RESULT_TYPE winTeam)
	{
		m_LastRoundWinType = winType;
		m_LastRoundWinTeam = winTeam;
	}
	MISSION_END_TYPE	getLastRoundWinType(void) const { return m_LastRoundWinType; }
	TEAM_RESULT_TYPE	getLastRoundWinTeam(void) const { return m_LastRoundWinTeam; }
	// Test-Automation을 위해 추가된 멤버
	// 
	
};

#endif