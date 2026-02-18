#ifndef	__ROOM__
#define __ROOM__

class CUserSession; 
class i3Packet; 
//class CRoomBase;
#include "RoomBase.h"
class CRoomInAnnihilation;
class CRoomInBomb;
class CRoomInEscape;
class CRoomInDeathmatch;
class CRoomInDefence;
class CRoomInDestroy;
class CRoomInSuddenDeath;
class CRoomInHeadHunter;
class CRoomInHeadKiller;
class CRoomInChallenge;
class CRoomInTutorial;
class CRoomInCrossCount;
class CRoomInConvoy;
class CRoomInRunAway;
class CRoomInSpace;

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
	UINT16	ui16Exp[ TYPE_BATTLE_RESULT_EVENT_COUNT ];
	UINT16	ui16Point[ TYPE_BATTLE_RESULT_EVENT_COUNT ];
};

struct TEAM_CLAN_INDEX_LIST
{
	UINT8	ui8ClanIdxCount;
	UINT32	aClanIdx[ SLOT_MAX_COUNT / 2 ];	// 한 팀에 최대 슬롯 수 만큼 배열을 미리 할당 해놓습니다.
};
bool	isContainedClanIndexInList( const TEAM_CLAN_INDEX_LIST & idxList, const UINT32 ui32CheckIndex );

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
	CRoomInChallenge*		m_pChallenge;
	CRoomInTutorial*		m_pTutorial;
	CRoomInCrossCount*		m_pCrossCount;
	CRoomInConvoy*			m_pConvoy;
	CRoomInRunAway*			m_pRunAway;
	CRoomInSpace*			m_pSpace;


	TYPE_ROOM_INFO			m_Info;

	bool					m_bReSpawnFlag[SLOT_MAX_COUNT];
	bool					m_bDeathFlag[SLOT_MAX_COUNT];
	bool					m_bAlive[ SLOT_MAX_COUNT ];
	bool					m_bKickBattle[ SLOT_MAX_COUNT ];	// Dedi 에서 요청한 것으로 Battle에서 유저를 레디룸으로 보냄.


public:

	STAGE_UID				m_StageUID;					// 스테이지 고유 인덱스. 빠른입장시 사용.	

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
	UINT32					m_i32LogChannelNum;				// Log용 채널 인덱스

	char					m_strPasswd[NET_ROOM_PW];		// 비밀번호
	
	INT32					m_i32MainSlotIdx;				// 방장 Slot Index

	//엔씨 로그 땜시 위로 올림
	INT32					m_i32WinTeam;					// 승리팀

	UINT16					m_ui16BattleUser; 

	INT8					m_i8StartCountdown;

	SLOT_BONUS				m_aSlotBonus[ SLOT_MAX_COUNT ];	// 추후에는 _ui16EventExp 로 병합 바랍니다.
	
	BOOL					m_bIsBattleEnd;					// 정상 종료하였는지 알려주는 플래그 
	
	// 데디가 요청한 유저 킥( 유저를 배틀에서 방으로 내보냄 )
	void					SetKickBattle_Dedi( UINT32 ui32SlotIdx ) { if( ui32SlotIdx < SLOT_MAX_COUNT ) m_bKickBattle[ ui32SlotIdx ] = true; }

	// GM Pause
	void					GMPauseResult_C( INT32 i32Result );

	// Round Control by Dedi
	void					SetRoundStart();
	void					SetRoundEnd( UINT8 ui8RoundEndType, UINT8 ui8RoundWinTeam );

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
	UINT32					m_ui32ATRoomState;				// 방 스테이트가 변경되었을때의 시간
	UINT32					m_ui32RTMissionRoundState;		// 배틀중에 미션라운드 스테이트가 변경되었을때의 시간
	i3Mutex					m_csSession;					// 방에서 사용하는 크리티컬 섹션 : 고민중.....필요한지에 대해.	


	
	BOOL					m_bIsBattle;					// 배틀중인지를	알려주는 플래그 

	UINT16					m_ui16QuestEndFlag;				// 결과창용 임무카드 완료 플래그

	INT32					m_i32TotalRespawnIdx;			// 클라이언트에서 겹치기 리스폰 방지용입니다.
	UINT32					m_ui32ATSendPing;				// 핑타임

	INT8					m_i8KickUserInsertPoint;
	T_UID					m_aKickUserList[ KICK_LIST_IN_ROOM_MAX ];
	//=====================================================================================================================================//
	//Each Slot Value 
	BOOL					m_pbAlive[SLOT_MAX_COUNT];			// 살아있는 유저인지 죽은 유저인지 체크
	ROOM_USER_INFO			m_stRoomUserInfo[ SLOT_MAX_COUNT ];	// 게임서버에서만 사용하는 유저 정보 여러가지
	
	USER_INFO_EQUIPMENT_CHARA	m_aiEquipChara[SLOT_MAX_COUNT];						// 챌린지모드 AI 장비
	UINT8					m_aui8Rank[SLOT_MAX_COUNT];
	char					m_aiNickname[SLOT_MAX_COUNT][NET_NICK_NAME_SIZE];

	USED_WEAPON				m_stKilledUserWeapon[SLOT_MAX_COUNT];		// 죽은 유저의 무기 누적

	//=====================================================================================================================================//
	//TimeOut 
	UINT32					m_ui32ATMCTimeOutStart;				// 방장을 변경해야합니다
	UINT16					m_ui16MCTimeOutStartUserFlg;		// 타임 아웃 시작할때 같이 게임하는 유저 세팅
	UINT16					m_ui16MCTimeOutFlg;					// 타임 아웃 플래그

	//=====================================================================================================================================//
	//For Mission ALLL
	UINT8					m_ui8MiState;						// 미션 스테이트

	//=====================================================================================================================================//
	//휘발성
	UINT32					m_ui32ATCountDownR;		//대기룸에서 START 버튼 클릭시 카운트 다운.
	UINT32					m_ui32ATCountDownB;		//배틀시작 직전 카운트 다운. 종료시간 기록.
	UINT16					m_ui16BattleEndUserFlag;

	UINT8					m_ui8MultiKillCount[SLOT_MAX_COUNT];	// 유저별 연속킬 횟수. m_ui8A_KillCount는 4킬 이상 증가가 안되기떄문에 따로 만듭니다.
	MULTI_KILL_INFO			m_stMultiKill;

	T_MatchingID			m_TMatchingID;		// Clan Match ID

	UINT8					m_ui8AtkDefChange;

protected: 
	//=====================================================================================================================================//
	//Send Message
	void					_SendPacketAllPlayer(		i3NetworkPacket * pPacket, SLOT_STATE eLimitState = SLOT_STATE_CLOSE );
	void					_SendS2PacketAllPlayer(		S2MOPacketBase * pPacket, SLOT_STATE eLimitState = SLOT_STATE_CLOSE );
	void					_SendPacketAnotherPlayer(	i3NetworkPacket * pPacket, CUserSession * pSession, SLOT_STATE eLimitState = SLOT_STATE_CLOSE );
	void					_SendS2PacketAnotherPlayer(	S2MOPacketBase * pPacket, CUserSession * pSession, SLOT_STATE eLimitState = SLOT_STATE_CLOSE );

	void					_SendPacketStartGame(		CUserSession * pSession );
	void					_SendPacketBattlePlayer(	i3NetworkPacket * pPacket, INT32 i32TeamIdx = SERVER_VALUE_UNKNOWN);
	void					_SendPacketReadyRoomPlayer(	i3NetworkPacket	* pPacket, INT32 i32TeamIdx = SERVER_VALUE_UNKNOWN);

	void					_SendNickName(	CUserSession * pSession, INT32 i32SlotIdx, BOOL bAll = TRUE);	//닉네임 보내기 
	void					_SendRank(CUserSession * pSession, INT32 i32SlotIdx, BOOL bAll = TRUE);			//계급 보내기
	void					_SendChangeColorNick(CUserSession * pSession, INT32 i32SlotIdx, BOOL bAll);		//컬러 닉네임
	void					_SendChangeColorMuzzleFlash( INT32 i32SlotIdx, BOOL bAll);						//컬러 총구 화염	
	void					_SendPreStartBattle(CUserSession * pSession, BOOL bSuccess);					//배틀 준비 메시지 
	void					_SendStartBattle( CUserSession * pSession, INT32 i32Result, bool bNoStartRound = false );	//배틀 시작 메시지
	void					_SendBattleCountdown( INT8 i8StartCountdown );
	void					_SendSlotInfo(	CUserSession * pSession );										//슬롯 정보	pSession == NULL 이면 모두에게 보냄	
	void					_SendSlotEquipmentInfo(	CUserSession * pSession );
	void					_SendMainChange(void); 
	void					_SendEndBattle_M(UINT8 ui8Win, UINT16 ui16BattleEndUserFlag);					//결과 창 				
protected:
	//=====================================================================================================================================//
	//Check
	BOOL					_CheckRespawn( INT32 i32SlotIdx );

	//=====================================================================================================================================//
	//Set State
	void					_SetState_M( UINT8 ui8State, UINT32 ui32ATServer ); 
	void					_SetMissionState_M(UINT8 ui8State);

	void					_GetRandMap();
	void					_SetUsersScopeType();
	//=====================================================================================================================================//
	//Time Out 
	void					_ResetTimeOut(void)	{ m_ui32ATMCTimeOutStart = 0; 		m_ui16MCTimeOutFlg = 0; }
	void					_RemoveTimeOutFlag(INT32 i32SlotIdx);

	//=====================================================================================================================================//
	//
	REAL32					_GetUsePositionWeight( UINT32 ui32ItemID, UINT8 ui8UsedCount );
	void					_SendUsedWeapon( INT32 i32ThreadIdx, CUserSession* pSession );
	INT32					_ChangeMain( SLOT_STATE eSlotState );						//방장변경
	void					_SetMainSlot();												// 방장이 변경된 후 방 방장 및 방정보 업데이트
	UINT16					_ResultUpdate_M( INT32 i32Win, BOOL bNormalEnd);			//점수를 DataBase에 넣습니다.	
	void					_ResultUpdateUser_M( INT32 i32WorkIdx, INT32 i );			//개인 점수를 따로 처리합니다.
	INT32					_GiveUpBattle( INT32 i32WorkIdx, CUserSession * pSession, BOOL bFreeMove, INT32 i32EndType); //배틀에서에서 방으로 가기
	void					_BattleTimeOutClient_M(UINT32 ui32SlotIdx);					//메인에서 도는 함수 
	REAL32					_GetBattleCountEndPercent();
	REAL32					_GetBattleCountEndPercentTeam(INT32 i32Team);
	REAL32					_GetBattleCountEndPercentTeam2(INT32 i32Team);
	REAL32					_GetBattleCountEndPercentAsymmetryPoint(INT32 i32Team);
	REAL32					_GetBattleCountEndPercentAsymmetryExp(INT32 i32Team);

	REAL32					_GetBattleModeInvariable();								// 모드별 상수

	void					_GetBattleUserCountInfo( INT32 i32InTeam, INT32 & i32OutAllUser, INT32 & i32OutTeamUser);
	void					_GetBattleUserCountAllTeam(INT32 & i32OutAllUser, INT32 & i32OutRedTeamUser, INT32 & i32OutBlueTeamUser);
	//=====================================================================================================================================//
	// ClanMatch 관련
	void					_UserPenalty( INT32 WorkIdx, CUserSession * pSession, INT32 i32EndType, BOOL bFreeMove ); // 비정상종료 유저 페널티 적용

	void	_OnUpdateRoom_Ready_M( UINT32 ui32ATServer );
	void	_OnUpdateRoom_CountDownR_M( UINT32 ui32ATServer );	//ReadyRoom에서 START 버튼 클릭시 카운트 다운.	
	void	_OnUpdateRoom_Loading_M( UINT32 ui32ATServer );
	void	_OnUpdateRoom_PreBattle_M( UINT32 ui32ATServer );
	void	_OnUpdateRoom_CountDownB_M( UINT32 ui32ATServer );	//배틀시작 직전 카운트 다운. 
	void	_OnUpdateRoom_Battle_M( UINT32 ui32ATServer );
	void	_OnUpdateRoom_Battle_End_M( UINT32 ui32ATServer );
	BOOL	_OnUpdateRoundMany_M();

	void					_SendPing_M(void); 

	bool					isDedicationMode(void)
	{
		switch(m_ui8BattleType)
		{
		case BATTLE_SERVER_TYPE_DEDICATION:
			if( !IsChallengeMode() )
			{
				return true; 
			}
			break;
		default:
			break;
		}

		return false; 
	}

	bool	isUseBattleServer(void)
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
			case GAME_TASK_SUB_GACHA:				return SLOT_STATE_GACHA;
			case GAME_TASK_SUB_GIFTSHOP:			return SLOT_STATE_GIFTSHOP;
		}
		return SLOT_STATE_EMPTY;
	}

	ROOM_STATE 				GetRoomState(void)			{	return (ROOM_STATE)m_Info._InfoBasic._State;	}
	ROOM_INFO_BASIC *		GetInfo(void)				{	return &m_Info._InfoBasic;				}
	SLOT_INFO *				GetSlotInfo(void)			{	return m_Info._aSlotInfo;				}
	SLOT_INFO *				GetSlotInfo(INT32 i32Idx)	{	return &m_Info._aSlotInfo[i32Idx];		}
	CUserSession	*		GetSession(INT32 i32nIdx)	{	return m_Info._pSession[ i32nIdx ];		}

	//=====================================================================================================================================//
	// Send Change Room Info Only
	void					_SendRoomInfo(	CUserSession * pSession );										//방 정보 변경 배틀시작 전에만 가능	pSession == NULL 이면 모두에게 보냄		
	void					_SendRoomInfoOption();															//방 정보 변경 ROOM_INFO_ADD 만 바뀔때 사용

	//User Session Call MainUser Only
	BOOL					CreateRoom(			CUserSession * pSession, ROOM_INFO_CREATE* pInfoCreate);
	INT32					ChangeRoomInfo(		CUserSession * pSession, ROOM_INFO_BASIC * pInfoBasic, INT8 i8enemyAiCount, INT8 i8difficultyLevel, BOOL bIsRoomCreate );
	INT32					ChangeRoomInfoOption(CUserSession * pSession, ROOM_INFO_ADD * pInfoAdd, BOOL bIsRoomCreate ); 
	INT32					ChangePasswd(		CUserSession * pSession, char * pPasswd);
	INT32					ChangeSlot_U(		CUserSession * pSession, INT32 SlotIdx, BOOL bOpen); 
	INT32					ChangePing_CS(		CUserSession * pSession, UINT8 * pui8Ping);
	void					MissionTouchDown(	CUserSession * pSession );
	void					MissionDeathBlow(	CUserSession * pSession, DEATHBLOW_INFO * pInfo);
	void					RequestTeamChange(	CUserSession * pSession);
	
	INT32					GMPause( CUserSession * pSession );
	INT32					GMResume( INT32 i32WorkIdx, INT32 i32SlotIdx );
	void					SendGMResume();

	bool					CheckMatchStart();
	void					EnableSoloPlay();
	INT32					CheckUserReady();
	INT32					_AllocateSlot( UINT32 ui32MatchTeam, UINT32 ui32ClanIdx, UINT8 ui8SlotState		);

	void					SendSlotEquipmentInfo( INT32 i32ThreadIdx, INT32 i32Slot, UINT16 ui16ChangeFlag );

	BOOL					CheckGameTime(CUserSession * pSession, INT32 i32GameTime, UINT8 ui8Ping, UINT16 ui16PingTime, UINT8 ui8RoundCount );

	INT32					ChangePing_U(	CUserSession * pSession, UINT8 ui8Ping );

	BOOL					ChangeMainSlotIdx( CUserSession * pSession );
	//=====================================================================================================================================//
	//User Session Call EveryBody 
	INT32					Enter(	INT32 i32ThreadIdx,	CUserSession * pSession, ROOM_INFO_JOIN* pInfoJoin );	//들어가기
	INT32					LeaveLobby_MU(		CUserSession * pSession, INT32 i32WorkThread, INT32 i32EndType );				//나가기//나가기 메인쓰레드, 워킹쓰레드에서 호출 가능
	INT32					ReadyBattle_U(		CUserSession * pSession);								// Ready 상태입니다.( 방장은 게임 시작을 하며 로딩을 합니다.)
	INT32					PreStartBattle(		CUserSession * pSession);								// NetInfo를 보내줍니다. 각 Client간에 홀펀칭을 합니다.
	INT32					StartBattle_U(		CUserSession * pSession);								// 홀펀칭이 완료되고 게임이 시작됩니다.	
	INT32					GiveUpBattle_U(		INT32 i32WorkThreadIdx, CUserSession * pSession, BOOL bFreeMove=FALSE);			// 게임에서 방으로 가기	//외부에서 호출 ( 전장 이동권이 있는지를 체크 )
	INT32					ReSpawnBattle_U(	CUserSession * pSession ) ;										//각 유저는 리스폰 패킷을 보냄 
	INT32					ReSpawnBattleForAi_U( CUserSession * pSession, INT32 i32SlotIdx);
	void					ReSpawnReq(	INT32 i32SlotIdx )			{ m_bReSpawnFlag[i32SlotIdx] = TRUE; }
	INT32					PickupWeapon_U(		CUserSession * pSession, UINT32 ui32SlotNum, UINT32 ui32Weapon);				//차후 적용.

	INT32					ChangeTeam(			CUserSession * pSession, INT32 i32NewTeam );

	INT32					DeathBattle( INT32 iThreadIdx, CUserSession * pSession, DEATH_INFO_HEADER * pHeader, DEATH_INFO_CHARA * pData, UINT8 ui8RoundNum );					//방장이 보내는 패킷.. 서버는 받아서 모든 배틀 유저에게 보냄 
	INT32					ProcessMissionCardAndMedal(INT32 iThreadIdx, DEATH_INFO_HEADER * pDeathHeader, DEATH_INFO_CHARA* pDeathInfos);
	void					MissionBombInstall(  INT32 i32ThreadIdx, CUserSession * pSession,		INT32 i32SlotIdx, UINT8 ui8BombPosition, REAL32 * pr32BombVecPos);			// 2009.06.22 정순구 C4 설치 위치 추가
	void					MissionBombUnInstall(INT32 i32ThreadIdx, CUserSession * pSession,	INT32 i32SlotIdx);
	void					MissionObjectDestroy(INT32 i32ThreadIdx,CUserSession * pSession, DESTRUCTION_INFO * pInfo); 	
	void					MissionObjectDefence(CUserSession * pSession, DEFENCE_INFO * pInfo); 	
	void					BattleSlotEquipment( CUserSession* pSession, UINT8 ui8SlotIdx, UINT16 ui16ChangeFlag, UINT32 ui32EquipItem );	// 야전상점
	void					KickUser_Dedi( CUserSession * pSession );

	void					MoveInventory(	CUserSession * pSession , BOOL bEnter);
	void					MoveShop(		CUserSession * pSession , BOOL bEnter);
	void					MoveInfo(		CUserSession * pSession , BOOL bEnter);
	void					MoveClan(		CUserSession * pSession , BOOL bEnter);
	void					MoveRS(			CUserSession * pSession , BOOL bEnter );
	void					MoveGiftShop(	CUserSession * pSession , BOOL bEnter );
	void					Chatting_U(		INT32 i32ThreadIdx, CUserSession * pSession, TTCHAR * pString, UINT16 ui16Type);	//채팅	
	void					SendPlayerInfo(	CUserSession * pSession, INT32 i32Slotidx );
	void					HoleCheck(		CUserSession * pSession );

	void					UnreadySlot(void);
	BOOL					IsAllowJoin(CUserSession * pSession);
	void					ChangeTeamForBalance();												// 방 밸런스 맞추기 전체 - ChangeTeam() 이용
	void					ChangeTeamForBalance2();
	void					ChangeTeamForBalance(CUserSession * pSession, UINT8 ui8Option);							// 방 밸런스 맞추기 개인 - ChangeTeam() 이용

	void					SendChangeNickname(INT32 i32slotIdx);
	void					SendChangeRank(INT32 i32slotIdx);
	void					SendChangeColorNick(INT32 i32slotIdx);
	void					SendChangeColorMuzzleFlash(INT32 i32slotIdx);

	INT32					GetNowTeam(INT32 i32SlotIdx);

	//////////////////////////////////////////////////////////////////////////
	// 난입유저 스코어출력
	void					GetRoomNowScore(ROOM_NOW_SCORE* pRoomNowScore);
	//////////////////////////////////////////////////////////////////////////
	//=====================================================================================================================================//
	//Time Out
	void					CheckMainClient_U(CUserSession * pSession); 
	void					GameKickNormalClient_U(CUserSession * pSession, UINT32 ui32SlotIdx); 

	//=====================================================================================================================================//

	//=====================================================================================================================================//
	//AdminGateWay
	void					DestroyRoom(); 
	void					SendNoticeMessage(char * pMessage, INT32 i32len); 

	//=====================================================================================================================================//
	//방장 변경 요청
	INT32					CheckMainChage(CUserSession * pSession);						// 최적의 방장을 찾는다 ( 방장의 방장검색버튼 )
	void					RequestMain(CUserSession * pSession);							// 유저가 방장을 찾아달라고 때쓴다.	
	void					RequestMainChange(CUserSession * pSession); 					// 방장 위임
	void					RequestMainChangeSlot(CUserSession * pSession, UINT32 ui32Slotidx); // 특정 유저에게 방장 위임

	INT32					_CheckMainBest( SLOT_STATE eSlotState );			// 방장 변경시 최적의 방장을 찾는다.

	//=====================================================================================================================================//
	// GM 용 커맨더 - 특정 유저 팅겨내기
	void					GM_KickUser_U( CUserSession * pSession, INT8 i8Slot );
	void					GM_ExitUser_U( CUserSession * pSession, INT8 i8Slot );
	void					GM_BlockUser_U( CUserSession * pSession, INT8 i8Slot, const TTCHAR *strComment );
	void					GM_DestroyRoom_U( CUserSession * pSession );
	BOOL					GetUserItemInfo( CUserSession * pSession, INT8 i8Slot );

	//======================================================================================================================================//
	// QA 자동화 커맨드
	void					IncreaseKillCount(	INT32 i32WorkIdx, INT32 i32UserIdx, INT32 i32IncKillCount, UINT32 ui32WeaponID );
	void					TeleportCharacter(	INT32 i32WorkIdx, INT32 i32UserIdx, INT32 i32TargetIdx, REAL32* pr32Pos );
	void					DamageToObject(		INT32 i32WorkIdx, INT32 i32UserIdx, INT32 i32Team, INT32 i32Damage );
	void					TimerGMPause(		INT32 i32WorkIdx, UINT32 ui32Second );

	void					SetQuestEndFlag( INT32 i32SlotIdx )
	{
		m_ui16QuestEndFlag = m_ui16QuestEndFlag | (UINT16)(1 << i32SlotIdx);
		return;
	}

	// Battle Server Module
	void					SetBattleGroupInfo( P_BATTLE_ROOM_CREATE_ACK * pAckInfo );
	void					ResetBattleGroupInfo();

	void					ChangeDifficultyLevel(CUserSession * pSession);
	void					SendAnnounceMessage( i3NetworkPacket* pPacket );

	// 리스폰 해킹 체크
	BOOL					CheckRespawnHacking( CUserSession* pSession );

	// 튜토리얼 미션관련
	void					MissionTutorialRoundEnd(  CUserSession* pSession );

	void					SendNotifyHackUser( INT32 i32SlotIdx, UINT8 ui8Type, INT32 i32ThreadIdx, UINT32 iHackType,  UINT8 ui8LocaleCode );		// 배틀중 핵 사용 유저 발견시 룸에 있는 모든 유저에게 알림 패킷을 보냅니다.

	bool					IsDinoUser(INT32 i32SlotIdx)	{ return m_pRoomMode->IsDinoUser(i32SlotIdx); }

	void					SendUserScopeTypeInfo(UINT8 ui8ScopeType, UINT32 ui32SlotIdx, CUserSession* pUserSession);
private:
	UINT16					m_MiRoundBossStartHumanUser;
	UINT16					_SetRaptorIdx(INT32 i32RaptorCnt, INT32 i32DinoIdx, INT32 i32BattleUserCount, INT32* pi32BattleUserSlot);
	void					_SetMissionStatePreBattle();

	// 어뷰징 체크
	void					_CheckAbusing( INT32 i32SlotIdx, UINT32 ui32PlayTime );

	// 어뷰징 페널티 적용	// _ResultUpdateUser_M 함수에 합쳐 버렸습니다. 2013-11-13 김현우
	//void					_AbusingPenalty( INT32 i32SlotIdx,  UINT32 ui32PlayTime );

	// 강제탈퇴투표
public:
	
	INT32					DecideVote( CUserSession* pUser, UINT8 ui8TargetSlot, UINT8 ui8KickReason );
	void					GetVote( INT32 i32Slot, UINT8 ui8Vote );			// 투표시작 후 표 집계
	BOOL					ChkBattleUser3Over();								// 2011.07.22 배틀중인 유저가 3명 이상인지 체크합니다.

private:
	void					CloseVote();										// 시간경과 또는 유저의 아웃으로 투표 종료

	UINT8					GetVoteState( UINT8* pi8Good );

	T_ItemID				GetCharItemId(INT32 i32SlotIdx);

	UINT8					m_ui8ChannelType;

	BOOL					m_bVoting;				// 투표중인지 아닌지 여부
	INT8					m_i8SuggestLocaleCode;	// 투표 발의한 클라의 로케일 코드
	UINT32					m_ui32RTVoteBegin;		// 투표 시작 시간 ( 유권자가 투표 하지 않더라도 일정 시간 후 강제투표 종료를 하기 위해 )
	INT8					m_i8GoodFromAlly;		// 아군의 찬성표
	INT8					m_i8GoodFromEnemy;		// 적군의 찬성표
	UINT8					m_ui8HalfCount;			// 투표 시작시의 인원의 '반수', 이 수를 초과해야 과반수가 성립됩니다.

	UINT8					m_aui8VoteData[SLOT_MAX_COUNT];		// 투표표 : 0-미투표	, 1-찬성, 2-반대
	UINT8					m_aui8VoteRight[SLOT_MAX_COUNT];	// 투표권리가 있는 유저는 1, 없는 유저는 0, 투표를 하거나 기브업시 1->0으로 변경되며, 모두 0이면 투표 종료

	UINT8					m_ui8Reason;			// 진행중인 투표의 투표 제기 이유 ( enum FORCEREMOVE_CAUSE )
	INT32					m_i32CallSlot;			// 진행중인 투표의 신고자 슬롯번호
	INT32					m_i32TargetSlot;		// 진행중인 투표의 피신고자 슬롯번호
	INT32					m_i32CallTeam;			// 진행중인 투표의 신고자 팀
	INT32					m_i32TargetTeam;		// 진행중인 투표의 피신고자 팀
	T_UID					m_i64TargetUID;			// 진행중인 투표의 피신고자 UID

	i3LinearRand*			m_pRandMap;

	// GM 정지		2013.07.22
	BOOL					m_bGMPause;				// 게임이 정지중이면 true
	INT32					m_i32PauseUserIdx;		// 정지명령을 내린 슬롯번호
	UINT32					m_ui32ATPauseBegin;		// 다음 정지 가능한 시간

	UINT32					m_ui32TimerToPause;
	UINT32					m_ui32RandomState[SLOT_MAX_COUNT];
	UINT8					m_ui8RandomIndex;
public:
	BOOL					IsGMPause()		{ return m_bGMPause; }


private:	
	INT32					SetRoomMode( UINT32 ui32StageID );	// 룸 모드 설정
	UINT32					WELL512_Random(UINT32 ui32State[], UINT8& ui8Index); // 랜덤 값 생성
	void					SetRandomMapStartEndIdx(UINT16& ui16StartIdx, UINT16& ui16EndIdx); // 랜덤맵 인덱스 범위 설정.
public:

	void					SendRespawnToDedi( INT32 i32ThreadIdx, INT32 i32SlotIdx, UINT32 * pui32WeaponList, INT32 i32RespawnIdx, INT32 i32RespawnCount );

	CUserSession*			GetUserSession( INT32 i32SlotIdx );
	void					SetObserverMode( INT32 i32SlotIdx, bool bIsGmObserverMode  ) 
	{ 
		if( 0 > i32SlotIdx || SLOT_MAX_COUNT <= i32SlotIdx  )	return;
		m_Info._aSlotInfo[i32SlotIdx]._bIsGmObserverMode = bIsGmObserverMode;
	}

	// 서버타임 개선. 배틀중의 시간은 반드시 GetRoomBattleTime()함수로 얻어와야 합니다.	2013.08.06 안국정
public:
	UINT32					GetRoomBattleTime()			{ return m_ui32RoomBattleTime; }
	REAL32					GetRoomRealBattleTime()		{ return m_r32RoomBattleTime; }

	void					MedalMissionCheck( INT32 i32ThreadIdx, CUserSession * pUser, MEDAL_MC_ACTION_ID eActionID , UINT32 ui32UseItemID = 0 , CUserSession * pTargetUser = NULL );  
	BOOL					CheckMissionActionID_Category(  MMC_INFO* pMMCInfo, CUserSession * pUser, CUserSession * pTargetUser ) ;	// 액션 아이디별 미션 완료 체크 - 여러개 카테고리 중복가능.

	void					SetUserMissionCard( CUserSession * pUser, ACTION_ID eAttionID, void* pPointer = NULL);
	BOOL					CheckMissionCardComplete( QUEST_CTX * pQuest, ACTION_ID eAttionID, void* pPointer = NULL);

	BOOL					IsConvoyMode()		const { return (m_Info._i32StageType == STAGE_MODE_CONVOY);					}
	BOOL					IsTutorialMode()	const { return (m_Info._i32StageType == STAGE_MODE_TUTORIAL);					}
	BOOL					IsChallengeMode()	const { return ( AI_USE_NO != StageID::GetAIMode(m_Info._InfoBasic._StageID));	}
	BOOL					IsHeadHunterMode()	const { return ( true == StageID::IsHeadShotOnly(m_Info._InfoBasic._StageID)); }
	BOOL					IsChaosMode()		const { return ( true == StageID::IsHudOff(m_Info._InfoBasic._StageID)) && (true == StageID::IsOneShotOneKill(m_Info._InfoBasic._StageID)); }
	bool					IsMatchMaking()		const { return m_Info._InfoBasic._IsClanMatch; }
	BOOL					IsSpaceMode()		const { return (true == StageID::IsSpaceMode(m_Info._InfoBasic._StageID)); }
	
	bool					IsClanChannel()		const;

	void					DaliyPlayTime( INT32 i32WorkIdx, INT32 i32SlotIdx, BOOL bFreeMove );

	void					ChangePCCafeValue( INT32 m_i32SlotIdx, UINT8 ui8PCCafe );

	void					CheckWeaponFlag(ITEM_INFO* aWeaponList, INT32& i32CheckSkipFlag, INT32& i32WeaponUseStop );
	T_MatchingID			GetClanMatchingID(void)		{ return m_TMatchingID; }		// Clan Match ID

private:
	i3Timer					m_Timer;
	REAL32					m_r32RoomBattleTime;	// 배틀시작시 0.0f부터 1초당 1.0f씩 증가
	UINT32					m_ui32RoomBattleTime;	// m_r32RoomBattleTime의 소수점 자른 값

	void					_InitBattleTime()
	{
		m_Timer.GetDeltaSec();
		m_r32RoomBattleTime		= 0.0f;
		m_ui32RoomBattleTime	= 0;
	}
	void					_UpdateBattleTime()
	{
		m_bGMPause ? m_Timer.GetDeltaSec() : m_r32RoomBattleTime += m_Timer.GetDeltaSec();
		m_ui32RoomBattleTime = (UINT32)m_r32RoomBattleTime;
	}


};

#endif