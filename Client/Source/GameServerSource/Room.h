#ifndef	__ROOM__
#define __ROOM__

class CUserSession; 
class i3Packet; 
class CRoomInAnnihilation;
class CRoomInBomb;
class CRoomInEscape;
class CRoomInDeathmatch;
class CRoomInDefence;
class CRoomInDestroy;
class CRoomInSuddenDeath;
class CRoomInHeadHunter;
class CRoomInHeadKiller;
class CRoomInTraining;
class CRoomInTutorial;
class CRoomInDomination;
class CRoomInCrossCount;

#include "RoomBase.h"
#include "VoteRecorder.h"

/*-----------------------------------------------------------------------
Name : CRoom
2007 03 31 : 소스 정리, 버그확인 : 버그없음 : 
2007 04 03 : Notice SlotInfo는 상태가 변경되면 먼저 보내줘야 함
2007 06 19 : 방 생성을 제외한 모든 방에서 배틀에 대한 프로세스는 메인 함수에서 되어야 합니다. 
2007 06 19 : 방에서 데이터 베이스 접근시 주의 할점. 메인 쓰레드 함수 인지, 아니면 외부에서 호출된 함수 인지 정확히 판단해야함. 예 : DeductionExp_DbIdx
2007 07 06 : 메니저를 통과할 필요없는 것들은 바로 유저가 호출하도록 변경 

-----------------------------------------------------------------------*/

struct SLOT_BONUS
{
	UINT16	ui16ExpItem;
	UINT16	ui16ExpPCCafe;
	UINT16	ui16ExpEvent;

	UINT16	ui16PointItem;
	UINT16	ui16PointPCCafe;
	UINT16	ui16PointEvent;
};

// 라운드를 시작한 유저인지
#define IS_USER_START(slot)		(m_Info._ui16RoundStartUser & (0x1 << slot))
// 죽은 유저인지 확인
#define IS_USER_DEATH(slot)		(m_Info._ui16RoundDeathUser & (0x1 << slot ))
// Alive 처리
#define SET_USER_ALIVE(slot)	(m_Info._ui16RoundDeathUser &= ~(0x1 << slot))
// Death 처리
#define SET_USER_DEATH(slot)	(m_Info._ui16RoundDeathUser |= (0x1 << slot))

// 무기 핵 검사 - 서동권
#define HACK_CHECK_WEAPON_COUNT		250
class CRoom : public i3ElementBase	
{
	I3_CLASS_DEFINE( CRoom );

	CRoomBase*				m_pRoomMode;
	CRoomInAnnihilation*	m_pAnnihilation;
	CRoomInBomb*			m_pBomb;
	CRoomInEscape*			m_pEscape;
	CRoomInDeathmatch*		m_pDeathmatch;
	CRoomInDefence*			m_pDefence;
	CRoomInDestroy*			m_pDestroy;
	CRoomInSuddenDeath*		m_pSuddenDeath;
	CRoomInHeadHunter*		m_pHeadHunter;
	CRoomInHeadKiller*		m_pHeadKiller;
	CRoomInTraining*		m_pTraining;
	CRoomInTutorial*		m_pTutorial;
	CRoomInCrossCount*		m_pCrossCount;

	TYPE_ROOM_INFO			m_Info;

	bool					m_abReSpawn[SLOT_MAX_COUNT]; 
	bool					m_abDeath[SLOT_MAX_COUNT]; 
	bool					m_abAlive[SLOT_MAX_COUNT];


public:

	UINT8					GetBattleType() const { return m_ui8BattleType;	}
	//=====================================================================================================================================//
	//LOG-D에서 사용
	INT64					m_i64LogMainUserDbIdx;			// 방장의 DB 인덱스 
	UINT32					m_ui32LogUniqueNumber;			// 방마다 유니크한 값
	UINT32					m_ui32LogBattleCount;			// 방이 만들어지고 했던 게임 카운트
	INT8					m_i8IsClanMatch;				// 클랜 전인지 확인용 로그

	//=====================================================================================================================================//
	//Manager에서 관리 
	BOOL					m_bIsCreate; 					// 가동중인방	
	UINT32					m_ui32LogChannelNum;			// Log용 채널 인덱스

	char					m_strPasswd[NET_ROOM_PW];		// 비밀번호


	INT32					m_i32MainSlotIdx;				// 방장 Slot Index

	//엔씨 로그 땜시 위로 올림
	INT8	m_i8WinTeam;					// 승리팀

	UINT16					m_ui16BattleUser; 

	INT8					m_i8StartCountdown;

	SLOT_BONUS				m_aSlotBonus[ SLOT_MAX_COUNT ];	// 추후에는 _ui16EventExp 로 병합 바랍니다.
	// 소환핵 체크를 위한 임시 방편
	// 방장이 죽인 유저의 좌표를 기록한다. - 서동권 - 임시 방편입니다.
	REAL32					m_r32Hack_X;
	REAL32					m_r32Hack_Y;
	REAL32					m_r32Hack_Z;

	UINT8					m_ui8Hack_Count; // 몇번 연속으로 같은 자리에서 죽였는지 - 소환핵
	BOOL					m_bIsBattleEnd;					// 정상 종료하였는지 알려주는 플래그 
	
	// 무기 핵 체크 - 서동권
	UINT32					m_aui32RoomWeaponList[ HACK_CHECK_WEAPON_COUNT ];	// 무기 핵 검사를 위해 방에 있는 유저들의 무기를 담아놓는 버퍼 - 서동권
	UINT8					m_ui8RoomWeaponListCount;
	void	AddHackCheckWeapon( CUserSession * pSession );
	BOOL	CheckDeathWeapon( CUserSession* pSession, UINT32 ui32WeaponID );


	void	GMPauseResult_C( INT32 i32Result );

protected: 
	//=====================================================================================================================================//
	//	BATTLE Server Value
	UINT32					m_ui32BattleRoomIdx;	// 그룹 인덱스 -- 확인해 봐야 할듯 
	UINT32					m_ui32BattleEncryptKey;	// 보안용으로 만들어진 비밀키
	BATTLE_ROOM_INFO		m_BattleRoomInfo;		// 배틀 룸 정보 

	UINT8					m_ui8BattleType;		// 배틀 타입  
	bool					m_bIsBattleRoomCreated;	// 릴레이서버에 방이 만들어졌는지 체크 ( 방장 로딩 후에 이 값이 FALSE 면 릴레이 서버가 방을 못 만든겁니다. )
	UINT32					m_ui32BattleServerIP;	// 배틀 서버 아이피 
	UINT16					m_ui16BattleServerPort;	// 배틀 서버 포트 
	
	//=====================================================================================================================================//
	//Common Value 
	UINT32					m_ui32CreateTime;		// 만들어진 시간 	
	UINT32					m_ui32StateStartTime;	// 방 상태에 대한 타임
	UINT32					m_ui32BattleStateTime;
	i3Mutex					m_csSession;			// 방에서 사용하는 크리티컬 섹션 : 고민중.....필요한지에 대해.	

	
	BOOL					m_bIsBattle;			// 배틀중인지를	알려주는 플래그 

	UINT16					m_ui16QuestEndFlag;		// 결과창용 임무카드 완료 플래그

	INT32					m_i32TotalRespawnIdx;	// 클라이언트에서 겹치기 리스폰 방지용입니다.
	UINT32					m_ui32SendPingTime;		// 핑타임

	INT8					m_i8KickUserInsertPoint;
	TUID					m_aKickUserList[ KICK_LIST_IN_ROOM_MAX ];
	//=====================================================================================================================================//
	//Each Slot Value 
	ROOM_USER_INFO			m_aRoomUserInfo[ SLOT_MAX_COUNT ];	// 게임서버에서만 사용하는 유저 정보 여러가지
	
	USER_INFO_EQUIPMENT_CHARA	m_aEquipChara[SLOT_MAX_COUNT];	// 훈련전 AI 장비
	UINT32					m_aaui32WeaponList[SLOT_MAX_COUNT][WEAPON_USAGE_COUNT];
	UINT8					m_aui8Rank[SLOT_MAX_COUNT];
	char					m_astrNickname[SLOT_MAX_COUNT][NET_NICK_NAME_SIZE];

	USED_WEAPON				m_aKilledUserWeapon[SLOT_MAX_COUNT];// 죽은 유저의 무기 누적

	//=====================================================================================================================================//
	//TimeOut 
	UINT32					m_ui32MCTimeOutStartTime;			// 방장을 변경해야합니다
	UINT16					m_ui16MCTimeOutStartUserFlg;		// 타임 아웃 시작할때 같이 게임하는 유저 세팅
	UINT16					m_ui16MCTimeOutFlg;					// 타임 아웃 플래그

	//=====================================================================================================================================//
	//For Mission ALLL
	UINT8					m_ui8MiState;						// 미션 스테이트

	//=====================================================================================================================================//
	// Battle Server
	UINT32					m_aui32BattleClanIdx[TEAM_COUNT];

	//=====================================================================================================================================//
	//휘발성
	UINT32					m_ui32BattleStartTime;
	UINT32					m_ui32OldCountdownTime;
	

	UINT16					m_ui16BattleEndUserFlag;
	// 보급상자
	INT8					m_ai8PresentSupplyBox[SLOT_MAX_COUNT];				// 0 : None, 1 : Normal, 2 : Silver, 3 : Gold
	UINT32					m_ui32SupplyBoxResultReq;

protected: 
	//=====================================================================================================================================//
	//Send Message
	void					_SendPacketAllPlayer(		i3NetworkPacket * pPacket);
	void					_SendPacketAnotherPlayer(	i3NetworkPacket * pPacket, CUserSession * pSession );
	void					_SendPacketBattlePlayer(	i3NetworkPacket * pPacket, INT32 i32TeamIdx = SERVER_VALUE_UNKNOWN);
	void					_SendPacketReadyRoomPlayer(	i3NetworkPacket	* pPacket, INT32 i32TeamIdx = SERVER_VALUE_UNKNOWN);
	void					_SendNickName(	CUserSession * pSession, INT32 i32SlotIdx, BOOL bAll = TRUE);	//닉네임 보내기 
	void					_SendRank(CUserSession * pSession, INT32 i32SlotIdx, BOOL bAll = TRUE);			//계급 보내기
	void					_SendChangeColorNick(CUserSession * pSession, INT32 i32SlotIdx, BOOL bAll);		//컬러 닉네임	
	void					_SendPreStartBattle(CUserSession * pSession, BOOL bSuccess);					//배틀 준비 메시지 
	void					_SendStartBattle(CUserSession * pSession, BOOL bSuccess, bool bNoStartRound = false );		//배틀 시작 메시지
	void					_SendBattleCountdown( INT8 i8StartCountdown );
	void					_SendSlotInfo(	CUserSession * pSession );										//슬롯 정보	pSession == NULL 이면 모두에게 보냄	
	void					_SendEquipmentInfo(	CUserSession * pSession); 
	void					_SendSlotEquipmentInfo(	CUserSession * pSession );
	void					_SendMainChange(void); 
	void					_SendEndBattle_M(UINT8 ui8Win, UINT16 ui16BattleEndUserFlag);					//결과 창 

protected:
	//=====================================================================================================================================//
	//Check
	BOOL					_CheckRespawn( INT32 i32SlotIdx );

	//=====================================================================================================================================//
	//Set State
	void					_SetState_M( UINT8 ui8State, UINT32 ui32ServerTime ); 
	void					_SetMissionState_M(UINT8 ui8State); 	

	//=====================================================================================================================================//
	//Time Out 
	void					_ResetTimeOut(void)	{ m_ui32MCTimeOutStartTime = 0; 		m_ui16MCTimeOutFlg = 0; }
	void					_RemoveTimeOutFlag(INT32 i32SlotIdx);

	//=====================================================================================================================================//
	//
	REAL32					_GetUsePositionWeight( UINT32 ui32ItemID, UINT8 ui8UsedCount );
	void					_SendUsedWeapon( INT32 i32ThreadIdx, CUserSession* pSession );
	INT32					_ChangeMain( SLOT_STATE eSlotState );					//방장변경
	void					_SetMainSlot();											// 방장이 변경된 후 방 방장 및 방정보 업데이트
	UINT16					_ResultUpdate_M( INT32 i32Win, BOOL bNormalEnd);		//점수를 DataBase에 넣습니다.	
	void					_ResultUpdateUser_M( INT32 i32WorkIdx, INT32 i );		//개인 점수를 따로 처리합니다.
	INT32					_GiveUpBattle( INT32 i32WorkIdx, CUserSession * pSession, BOOL bFreeMove, INT32 i32EndType); //배틀에서에서 방으로 가기
	void					_BattleTimeOutClient_M(UINT32 ui32SlotIdx);				//메인에서 도는 함수 
	REAL32					_GetBattleCountEndPercent();
	REAL32					_GetBattleCountEndPercentTeam(INT32 i32Team);
	REAL32					_GetBattleCountEndPercentTeam2(INT32 i32Team);
#ifdef NEW_EXPPOINT
	REAL32					_GetBattleModeInvariable();								// 모드별 상수
#endif

	//=====================================================================================================================================//
	// ClanMatch 관련
	void	_EnterTypeClan( INT32 & i32Start, INT32 & i32Incre, CUserSession * pSession, ROOM_INFO_JOIN* pInfoJoin  );
	INT32	_ReadyBattleClan( );
	void	_UserPenalty( INT32 i32WorkIdx, CUserSession * pSession, BOOL bFreeMove ); // 비정상종료 유저 페널티 적용

	void	_OnUpdateRoom_Ready_M( UINT32 ui32ServerTime );
	void	_OnUpdateRoom_Countdown_M( UINT32 ui32ServerTime );
	void	_OnUpdateRoom_Loading_M( UINT32 ui32ServerTime );
	void	_OnUpdateRoom_PreBattle_M( UINT32 ui32ServerTime );
	void	_OnUpdateRoom_Battle_M( UINT32 ui32ServerTime );
#ifdef USE_GM_PAUSE
	BOOL	_OnUpdateRoundMany_M();
#else
	BOOL	_OnUpdateRoundMany_M( UINT32 ui32ServerTime );
#endif

	void					_SendPing_M(void); 

	// 보급상자
	void					_PresentSupplyBox_M();

	bool					isDedicationMode(void)
	{
		switch(m_ui8BattleType)
		{
		case BATTLE_SERVER_TYPE_DEDICATION:
			if( STAGE_MASK_TYPE_TRAINING != m_Info._InfoBasic._StageMask )
			{
				return true; 
			}
			break; 
		default: 	
			break; 
		}

		return false; 
	}

	bool isUseBattleServer(void)
	{
		switch(m_ui8BattleType)
		{
		case BATTLE_SERVER_TYPE_RELAY		: 
		case BATTLE_SERVER_TYPE_DEDICATION  :
				return true; 
		}
		return false; 
	}

public: 
	CRoom( void);
	virtual ~CRoom();
	BOOL	OnCreate( UINT32 ui32ChannelNum, UINT32 ui32RoomNum );
	void	OnDestroy();

	BOOL OnUpdateRoom_M(UINT32 ui32ServerTime); 
	
	SLOT_STATE GetStateSub( GAME_TASK_SUB state )
	{
		switch( state )
		{
			case GAME_TASK_SUB_NONE:				return SLOT_STATE_NORMAL;
			case GAME_TASK_SUB_INVENTORY:			return SLOT_STATE_INVENTORY;
			case GAME_TASK_SUB_SHOP:				return SLOT_STATE_SHOP;
			case GAME_TASK_SUB_INFO:				return SLOT_STATE_INFO;
			case GAME_TASK_SUB_CLAN:				return SLOT_STATE_CLAN;
			case GAME_TASK_SUB_RS:					return SLOT_STATE_RS;
		}
		return SLOT_STATE_EMPTY;
	}

	ROOM_INFO_BASIC *	GetInfo(void)				{	return &m_Info._InfoBasic;				}
	SLOT_INFO *			GetSlotInfo(void)			{	return m_Info._SlotInfo;				}
	SLOT_INFO *			GetSlotInfo(INT32 i32Idx)	{	return &m_Info._SlotInfo[i32Idx];		}
	CUserSession	*	GetSession(INT32 i32Idx)	{	return m_Info._pSession[i32Idx];		}

	//=====================================================================================================================================//
	// Send Change Room Info Only
	void	_SendRoomInfo(	CUserSession * pSession );										//방 정보 변경 배틀시작 전에만 가능	pSession == NULL 이면 모두에게 보냄		
	void	_SendRoomInfoOption();															//방 정보 변경 ROOM_INFO_ADD 만 바뀔때 사용

	//User Session Call MainUser Only
	BOOL					CreateRoom(			CUserSession * pSession, ROOM_INFO_CREATE* pInfoCreate );
	INT32					ChangeRoomInfo(		CUserSession * pSession, ROOM_INFO_BASIC * pInfoBasic, INT8 i8enemyAiCount, INT8 i8difficultyLevel);
	INT32					ChangeRoomInfoOption(CUserSession * pSession, ROOM_INFO_ADD * pInfoAdd); 
	INT32					ChangePasswd(		CUserSession * pSession, char * pPasswd);
	INT32					ChangeSlot_U(		CUserSession * pSession, INT32 i32SlotIdx, BOOL bOpen); 
	INT32					ChangePing_CS(		CUserSession * pSession, UINT8 * pui8Ping);
	void					MissionTouchDown( 	CUserSession * pSession );
	void					MissionDeathBlow( 	CUserSession * pSession, DEATHBLOW_INFO * pInfo );
	void					RequestTeamChange(	CUserSession * pSession);

	INT32					GMPause( CUserSession * pSession );
	INT32					GMResume( INT32 i32WorkIdx, INT32 i32SlotIdx );
	void					SendGMResume();

	void					SendSlotEquipmentInfo( INT32 i32ThreadIdx, INT32 i32Slot, UINT16 ui16ChangeFlag );

	BOOL					CheckGameTime(CUserSession * pSession, INT32 i32GameTime, UINT8 ui8Ping, UINT8 ui8RoundCount );
	BOOL					CheckLoadingStart_U( char* pstrMapName );											// 로딩시 정상적인 값들이 세팅되었는지 검사합니다.

	INT32					ChangePing_U(	CUserSession * pSession, UINT8 ui8Ping );

	//=====================================================================================================================================//
	//User Session Call EveryBody 
	INT32	Enter(				CUserSession * pSession, ROOM_INFO_JOIN* pInfoJoin );	//들어가기
	INT32	LeaveLobby_MU(		CUserSession * pSession, INT32 i32WorkThread, INT32 i32EndType );				//나가기//나가기 메인쓰레드, 워킹쓰레드에서 호출 가능
	INT32	ReadyBattle_U(		CUserSession * pSession);								// Ready 상태입니다.( 방장은 게임 시작을 하며 로딩을 합니다.)
	INT32	PreStartBattle(		CUserSession * pSession);								// NetInfo를 보내줍니다. 각 Client간에 홀펀칭을 합니다.
	INT32	StartBattle_U(		CUserSession * pSession);								// 홀펀칭이 완료되고 게임이 시작됩니다.	
	INT32	GiveUpBattle_U(		INT32 i32WorkThreadIdx, CUserSession * pSession, BOOL bFreeMove=FALSE);			// 게임에서 방으로 가기	//외부에서 호출 ( 전장 이동권이 있는지를 체크 )
	INT32	ReSpawnBattle_U(	CUserSession * pSession ) ;										//각 유저는 리스폰 패킷을 보냄 
	BOOL	ReSpawnBattleForAi_U( CUserSession * pSession, INT32 i32SlotIdx);
	void	ReSpawnReq(	INT32 i32SlotIdx )			{ m_abReSpawn[i32SlotIdx] = TRUE; }

	INT32	ChangeTeam(			CUserSession * pSession, INT32 i32NewTeam );

	//
	INT32	DeathBattle( UINT32 ui32ThreadIdx, CUserSession * pSession, DEATH_INFO_HEADER * pHeader, DEATH_INFO_CHARA * pData, UINT8 ui8RoundNum );	//방장이 보내는 패킷.. 서버는 받아서 모든 배틀 유저에게 보냄 
	void	MissionBombInstall(CUserSession * pSession,	INT32 i32SlotIdx, UINT8 ui8BombPosition, REAL32 * pr32BombVecPos);			// 2009.06.22 정순구 C4 설치 위치 추가
	void	MissionBombUnInstall(CUserSession * pSession, INT32 i32SlotIdx);	
	void	MissionObjectDestroy(CUserSession * pSession, DESTRUCTION_INFO * pInfo); 	
	void	MissionObjectDefence(CUserSession * pSession, DEFENCE_INFO * pInfo); 	
	void	BattleSlotEquipment( CUserSession* pSession, UINT8 ui8SlotIdx, UINT16 ui16ChangeFlag, UINT32 ui32EquipItem );	// 야전상점

#ifdef DEF_OBSERVER_MODE
	INT32   ChangeObserverSlot(CUserSession * pSession, INT32 Slotflag);
	INT32	ChangeHiddenSlot(CUserSession * pSession, INT32 Slotflag);
#endif
	void	MoveInventory(	CUserSession * pSession , BOOL bEnter);
	void	MoveShop(		CUserSession * pSession , BOOL bEnter);
	void	MoveInfo(		CUserSession * pSession , BOOL bEnter);
	void	MoveClan(		CUserSession * pSession , BOOL bEnter);
	void	MoveRS(			CUserSession * pSession , BOOL bEnter );
	void	Chatting_U(		INT32 i32ThreadIdx, CUserSession * pSession, char * pstrString, UINT16 ui16Type);	//채팅	
	void	SendPlayerInfo(	CUserSession * pSession, INT32 i32Slotidx );
	void	HoleCheck(		CUserSession * pSession );

	void	UnreadySlotFor4vs4(void);
	BOOL	IsAllowJoinFor4vs4(CUserSession * pSession);
	void	ChangeTeamForBalance();												// 방 밸런스 맞추기 전체 - ChangeTeam() 이용
	void	ChangeTeamForBalance2();
	void	ChangeTeamForBalance(CUserSession * pSession, UINT8 ui8Option);							// 방 밸런스 맞추기 개인 - ChangeTeam() 이용


	void	SendChangeNickname(INT32 i32slotIdx);
	void	SendChangeRank(INT32 i32slotIdx);
	void	SendChangeColorNick(INT32 i32slotIdx);

	void	SetUserMissionCard( CUserSession* pSession, ACTION_ID eAttionID, void* pPointer=NULL );
	BOOL	CheckMissionCardComplete( QUEST_CTX* pQuest, ACTION_ID eAttionID, void* pPointer=NULL );
	//=====================================================================================================================================//
	//Time Out
	void	CheckMainClient_U(CUserSession * pSession); 
	void	GameKickNormalClient_U(CUserSession * pSession, UINT32 ui32SlotIdx); 

	//=====================================================================================================================================//
	//Log연동 
	void	GetRoomInfoLogD( INT32* pi32RedCount, INT32* pi32BlueCount, char * pRedNick, char * pBlueNick); 
#ifdef DEF_OBSERVER_MODE
	void	GetObsRoomInfoLogD( INT32* pi32ObsCount, INT32* pi32HiddenCount, char * pObsNick, char * pHiddenNick );
#endif
	void	GetRoomMissionLogD( INT32* pi32RedCount, INT32* pi32BlueCount );

	//=====================================================================================================================================//
	//AdminGateWay
	void	DestroyRoom(); 
	void	SendNoticeMessage(char * pstrMessage, INT32 i32len); 

	//=====================================================================================================================================//
	//방장 변경 요청
	INT32	CheckMainChage(CUserSession * pSession);						// 최적의 방장을 찾는다 ( 방장의 방장검색버튼 )
	void	RequestMain(CUserSession * pSession);							// 유저가 방장을 찾아달라고 때쓴다.	
	void	RequestMainChange(CUserSession * pSession); 					// 방장 위임
	void	RequestMainChangeSlot(CUserSession * pSession, UINT32 ui32Slotidx); // 특정 유저에게 방장 위임

	INT32	_CheckMainBest( SLOT_STATE eSlotState );			// 방장 변경시 최적의 방장을 찾는다.

	//=====================================================================================================================================//
	// GM 용 커맨더 - 특정 유저 팅겨내기
	void	GM_KickUser_U( CUserSession * pSession, INT8 i8Slot );
	void	GM_ExitUser_U( CUserSession * pSession, INT8 i8Slot );
	void	GM_BlockUser_U( CUserSession * pSession, INT8 i8Slot, const char *pstrComment );
	void	GM_DestroyRoom_U( CUserSession * pSession );
	BOOL	GetUserItemInfo( CUserSession * pSession, INT8 i8Slot );

	void	SetQuestEndFlag( INT32 i32SlotIdx )
	{
		m_ui16QuestEndFlag = m_ui16QuestEndFlag | (UINT16)(1 << i32SlotIdx); 
		return; 
	}

	void	ForceSetKillCount(UINT16 ui16Red, UINT16 ui16Blue)
	{
		if( m_Info._InfoBasic._State != ROOM_STATE_BATTLE)return; 
		m_Info._aTeamScoreTotal[0].m_KillCount = ui16Red; 
		m_Info._aTeamScoreTotal[1].m_KillCount = ui16Blue; 
		return; 
	}

	void	ForceBattleEndlessSet(void)
	{
		if( m_Info._InfoBasic._State != ROOM_STATE_BATTLE)return; 

		switch( m_Info._i32StageType )
		{
			case STAGE_TYPE_DEATHMATCH		:	m_Info._InfoAdd._SubType = 0x96;	break;	//TEAMBATTLE_TIME_COUNT //TEAMBATTLE_KILL_COUNT 9//6/
			default							: 
			case STAGE_TYPE_ESCAPE			: 
			case STAGE_TYPE_DESTROY			: 
			case STAGE_TYPE_DEFENCE			: 
			case STAGE_TYPE_ANNIHILATION	: 
			case STAGE_TYPE_BOMB			:	m_Info._InfoAdd._SubType = 0x95;	break;	// TEAMBATTLE_TIME_COUNT // BATTLE_ROUND_TYPE_9 9/5
		}
		return; 
	}

	// Battle Server Module
	void	SetBattleGroupInfo( INT32 i32Result, P_BATTLE_ROOM_CREATE_ACK * pAckInfo );
	void	ResetBattleGroupInfo();

	void ChangeDifficultyLevel(CUserSession * pSession);
	//BOOL	IsUDPHolePunching( CUserSession* pSession );

	// 보스 미션전 관련..

	// 보급상자
	void	SetPresentSupplyBox( UINT32 ui32RoomStateCheckTime, INT8 i8BattleMemberCount, USER_PRESENTED_INFO *pUserPresentInfo );
	void	SendAnnounceMessage( i3NetworkPacket* pPacket );

	// 리스폰 해킹 체크
	BOOL	CheckRespawnHacking( CUserSession* pSession );

	// 튜토리얼 미션관련
	void	MissionTutorialRoundEnd(  CUserSession* pSession );

	void	SendNotifyHackUser( INT32 i32SlotIdx, UINT8 ui8Type, INT32 i32ThreadIdx, UINT32 ui32HackType,  UINT8 ui8LocaleCode );		// 배틀중 핵 사용 유저 발견시 룸에 있는 모든 유저에게 알림 패킷을 보냅니다.

	void		InsertSlotItem( CUserSession * pSession, SLOT_INFO* pSlotInfo );				// Slot 에 아이템 정보 담기

private:
	UINT16		m_ui16MiRoundBossStartHumanUser;
	UINT16		_SetRaptorIdx(INT32 i32RaptorCnt, INT32 i32DinoIdx, INT32 i32BattleUserCount, INT32* pi32BattleUserSlot);
	void		_SetMissionStatePreBattle();

	// 어뷰징 체크
	void	_CheckAbusing( INT32 i32SlotIdx, UINT32 ui32PlayTime );

	// 어뷰징 페널티 적용
	void	_AbusingPenalty( INT32 i32SlotIdx,  UINT32 ui32PlayTime );

	// 강제탈퇴투표
public:
	
	INT32		DecideVote( CUserSession* pUser, UINT8 ui8TargetSlot, UINT8 ui8KickReason );
	void		GetVote( INT32 i32Slot, UINT8 ui8Vote, INT32 i32WorkIdx );			// 투표시작 후 표 집계
	BOOL		ChkBattleUser3Over();		// 2011.07.22 배틀중인 유저가 3명 이상인지 체크합니다.

private:
	void		CloseVote();	// 시간경과 또는 유저의 아웃으로 투표 종료

	UINT8		GetVoteState( UINT8* pui8Good );

	CVoteRecorder	m_VoteRecorder;

	BOOL		m_bVoting;					// 투표중인지 아닌지 여부
	INT8		m_i8SuggestLocaleCode;		// 투표 발의한 클라의 로케일 코드
	UINT32		m_ui32BeginTime;			// 투표 시작 시간 ( 유권자가 투표 하지 않더라도 일정 시간 후 강제투표 종료를 하기 위해 )
	INT8		m_i8GoodFromAlly;			// 아군의 찬성표
	INT8		m_i8GoodFromEnemy;			// 적군의 찬성표
	UINT8		m_ui8HalfCount;				// 투표 시작시의 인원의 '반수', 이 수를 초과해야 과반수가 성립됩니다.

	UINT32		m_ui32Agree;				// 찬성 상태 비트플래그, 투표 시작시 0으로 설정되고 해당 슬롯이 찬성표를 던질때 비트가 1로 설정됩니다.
	UINT32		m_ui32Disagree;				// 반대 상태 비트플래그
	UINT32		m_ui32VoteState;			// 투표상태 비트플래그, 투표 시작시 값이 설정되는데, 00000000 00000000 00000010 00010001로 설정되면, 0번, 4번, 9번 슬롯의 유저가 투표를 해야 한다는 뜻입니다.
											// m_bitVoteState == 0 의 조건이 성립되면 투표는 종료됩니다. 투표를 하거나, 배틀룸을 나가면 1->0으로 변경됩니다. 

	UINT8		m_ui8Reason;				// 진행중인 투표의 투표 제기 이유 ( enum FORCEREMOVE_CAUSE )
	INT32		m_i32CallSlot;				// 진행중인 투표의 신고자 슬롯번호
	INT32		m_i32TargetSlot;			// 진행중인 투표의 피신고자 슬롯번호
	INT32		m_i32CallTeam;				// 진행중인 투표의 신고자 팀
	INT32		m_i32TargetTeam;			// 진행중인 투표의 피신고자 팀
	TUID		m_i64CallUID;				// 진행중인 투표의 신고자 UID ( 이 정보는 처음에 입력받은 후에 아무 사용 안하다가 투표 종료후  m_VoteRecord 구조체에 기록됩니다.)
	TUID		m_i64TargetUID;				// 진행중인 투표의 피신고자 UID

	// 중력핵 좌표	2011.09.08
	REAL32		m_ar32Boundry[ MAX_BOUNDRY_NAME ];	// x1,y1,z1,x2,y2,z2

	// GM 정지		2013.08.13
	BOOL				m_bGMPause;				// 게임이 정지중이면 true
	INT32				m_i32PauseUserIdx;		// 정지명령을 내린 슬롯번호
	UINT32				m_ui32ATPauseBegin;		// 다음 정지 가능한 시간

public:
	BOOL		IsGMPause()		{ return m_bGMPause; }

private:
	BOOL		CheckPlayableStage( UINT32 ui32StageID, UINT8 ui8WeaponFlag, UINT8 ui8StageMask );	// 플레이 가능한 맵인지 확인
	INT32		SetRoomMode( UINT32 ui32StageID, UINT8 ui8StageMask );	// 룸 모드 설정

public:
	void		SendRespawnToDedi( INT32 i32ThreadIdx, INT32 i32SlotIdx, UINT32 * pui32WeaponList, INT32 i32RespawnIdx, INT32 i32RespawnCount );

	void		SetObserverMode( INT32 i32SlotIdx, BOOL bIsGmObserverMode  ) 
	{ 
		if( 0 > i32SlotIdx || SLOT_MAX_COUNT <= i32SlotIdx  )	return;
		m_Info._SlotInfo[i32SlotIdx]._bIsGmObserverMode = bIsGmObserverMode; 
	}

	UINT32		m_aui32RecordRoundTime[64];	// 라운드 오차나서 오탐되는 버그 찾기 위한 임시멤버

public:
	UINT32		GetRoomBattleTime()
#ifdef USE_GM_PAUSE
										{ return m_ui32RoomBattleTime; }
#else
										{ return 0; }
#endif
private:
	i3Timer				m_Timer;
	REAL32				m_r32RoomBattleTime;	// 배틀시작시 0.0f부터 1초당 1.0f씩 증가
	UINT32				m_ui32RoomBattleTime;	// m_r32RoomBattleTime의 소수점 자른 값

	void		_InitBattleTime()
	{
		m_Timer.GetDeltaSec();
		m_r32RoomBattleTime		= 0.0f;
		m_ui32RoomBattleTime	= 0;
	}
	void		_UpdateBattleTime()
	{
		m_bGMPause ? m_Timer.GetDeltaSec() : m_r32RoomBattleTime += m_Timer.GetDeltaSec();
		m_ui32RoomBattleTime = (UINT32)m_r32RoomBattleTime;
	}
};

#endif