#ifndef __ROOM_BASE_H__
#define __ROOM_BASE_H__

#include "UserSession.h"

#define RED_TEAM_MASK		0x5555
#define BLUE_TEAM_MASK		0xAAAA

// Acc 가 붙은 변수는 게임 종료시까지 초기화 되지 않습니다.
// Rou 가 붙은 변수는 라운드에만 사용 됩니다.
struct TYPE_ROOM_INFO
{
	UINT32				_ui32ChannelNum;

	INT32				_i32StageType;								//킬 포인트, 피노를 세팅하기 위한값, 게임 모드가 들어감 모드 인덱스 //내부에서 사용하기 위해서 사용 이중변수임..쩝.
	INT32				_i32MiStateType;							// 미션 스테이트

	INT32				_i32MainSlotIdx;

	ROOM_INFO_BASIC		_InfoBasic;									// 기본 방 정보
	ROOM_INFO_ADD		_InfoAdd;									// 추가 방 정보

	UINT32				_ui32GameStartDate;							// 게임 최초 시작 날짜
	UINT32				_ui32GameStartTime;							// 게임 최초 시작 타임

	UINT32				_ui32BattleStartTime;						// Battle 시작 타임
	UINT32				_ui32BattleEndTime;							// Battle 최종 종료 타임

	UINT16				_aui16TotalRoundCount[ TEAM_COUNT ];		// 방에서 표시되는 카운트

	UINT16				_aui16AccExp[SLOT_MAX_COUNT];				// 배틀 종료후 점수
	UINT16				_aui16AccPoint[SLOT_MAX_COUNT];				// 배틀 종료후 점수
	
	
	UINT16				_aui16EventExp[SLOT_MAX_COUNT];				// 배틀 종료후 점수
	UINT16				_aui16EventPoint[SLOT_MAX_COUNT];			// 배틀 종료후 점수

	UINT8				_aui8ResultIcon[SLOT_MAX_COUNT];			// 결과창 아이콘

	SCORE_INFO			_aTeamScoreTotal[TEAM_COUNT];				// Total 전체 스코어 

	// Match 개인 스코어
	SCORE_INFO			_aM_UserScore[SLOT_MAX_COUNT];				// Kill, Death
	UINT16				_aui16M_Headshot[SLOT_MAX_COUNT];			// 해드샷 카운트
	UINT16				_aui16M_PiercingShot[ SLOT_MAX_COUNT ];
	UINT16				_aui16M_MassKill[ SLOT_MAX_COUNT ];
	UINT16				_aui16M_ChainStoppper[ SLOT_MAX_COUNT ];
	UINT16				_aui16M_ChainHeadShot[ SLOT_MAX_COUNT ];
	UINT16				_aui16M_ChainSlugger[ SLOT_MAX_COUNT ];
	UINT16				_aui16M_AccKill_1[ SLOT_MAX_COUNT ];
	UINT16				_aui16M_AccKill_2[ SLOT_MAX_COUNT ];
	UINT16				_aui16M_AccKill_3[ SLOT_MAX_COUNT ];
	UINT16				_aui16M_Intercept[ SLOT_MAX_COUNT ];		// 탈출미션 인터셉트
	UINT16				_aui16M_ChainTouchdown[ SLOT_MAX_COUNT ];
	UINT16				_aui16M_KillHuman[ SLOT_MAX_COUNT ];
	// 계산용 연속적으로 행하는 일.
	UINT8				m_aui8A_KillCount[SLOT_MAX_COUNT];			// 계산용으로 사용		// 연속킬에서 사용
	UINT8				m_aui8A_HeadShot[SLOT_MAX_COUNT];			// 계산용으로 사용 
	UINT8				m_aui8A_SlugShot[SLOT_MAX_COUNT];			// 계산용으로 사용 


	SCORE_INFO			_aUserScoreRound[SLOT_MAX_COUNT];			// Round 개인 스코어
	SCORE_INFO			_aTeamScoreRound[TEAM_COUNT];				// Round 전체 스코어

#ifdef NEW_EXPPOINT
	UINT16				_aui16Acc_KillExp[SLOT_MAX_COUNT];			// 라운드당 최대 킬 수 까지만 포함하여 쌓은 킬 수
#endif

	INT32				_ai32AcidBombingCount[SLOT_MAX_COUNT];		// 애시드 자폭 횟수 ( 자폭시 DeathCount 가 증가하지 않아 리스폰 해킹 검사를 위해 추가, _CheckRespawn() ) - 서동권
	UINT32				_aui32RespawnCount[SLOT_MAX_COUNT];			// 리스폰 카운트

	UINT8				_aui8AccMissionCount[SLOT_MAX_COUNT];		// 미션 카운트

	// 계산용 연속적으로 행하는 일.
	UINT8				_aui8A_Mission[ SLOT_MAX_COUNT ];			// 미션 연속 달성 카운트 (리스폰시 초기화)

	CUserSession	*	_pSession[SLOT_MAX_COUNT];					// 유저에 대한 포인터
	SLOT_INFO			_SlotInfo[SLOT_MAX_COUNT];					// 슬롯 정보

	UINT32				_aui32BattleClanIdx[TEAM_COUNT];			// 클랜 인덱스 정보

	UINT16				_aui16AccTrainingScore[SLOT_MAX_COUNT];		// AI 모드 점수

	UINT32				_ui32FirstBattleStartUser;					// 처음 게임을 시작한 유저 (Ready를 해서 같이 시작한 유저)
	UINT16				_ui16BattleUser;							// 게임중인 유저( 옵저버는 제외 )
	UINT16				_ui16RoundStartUser;						// 라운드 시작 유저
	UINT16				_ui16RoundDeathUser;						// 라운드 죽은 유저

	

	UINT8				_ui8MiEndType;								// 미션 엔트 타입
	UINT8				_ui8RoundWinTeam;							// 승리팀

	DINO_ROOM_INFO		_MiDinoInfo;
	bool				_abDinoStateUpdate[SLOT_MAX_COUNT];				
	UINT32				_aui32DInoCharInfo[SLOT_MAX_COUNT];

	UINT32				m_aui32AILevel[ SLOT_MAX_COUNT ];			// AI 레벨.. 혹시나 추후에 1:15 같은게 나올지 몰라 16으로 가겠습니다.
	// 훈련전
	INT8				m_i8enemyAiCount;							// 훈련전 NPC 수
	INT8				m_i8difficultyLevel;						// 사격장 난이도
	INT8				m_i8currentDifficultyLevel;
};

enum ROOM_READY_OPTION
{
	ROOM_READY_SOLO			= 0,		// 방장 혼자 게임을 시작 할 수 있음
	ROOM_READY_TOGETHER,				// 상대방이 있어야 게임을 시작 할 수 있음
};

enum ROOM_START_OPTION
{
	ROOM_START_DIRECT		= 0,		// 방장 레뒤 시 바로 시작됨
	ROOM_START_COUNTDOWN,				// 방장 레뒤 시 카운트 다운
};

enum ROOM_PRESTART_OPTION
{
	ROOM_PRESTART_DIRECT	= 0,		// 방장 로딩 후 혼자 시작
	ROOM_PRESTART_ONE_WAIT,				// 방장 로딩 후 한명의 유저라도 있으면 바로 시작
	ROOM_PRESTART_ALL_WAIT,				// 방장 로딩 후 다른 유저 기다림
};

enum ROOM_RESPAWN_OPTION
{
	ROOM_RESPAWN_NONE		= 0,		// 맨 처음 한번만 리스폰 할 수 있습니다.
	ROOM_RESPAWN_LIMIT,					// 일정 횟수만큼만 리스폰 된다..
	ROOM_RESPAWN_CONTINUE,				// 계속 리스폰 된다.
};

enum ROOM_CHATTING_OPTION
{
	ROOM_CHATTING_ALL		= 0,		// 모든 유저에게 채팅을 보낼 수 있습니다.
	ROOM_CHATTING_GHOST,				// 살아있는 유저에게만 채팅을 보냅니다.
};

enum ROOM_END_OPTION
{
	ROOM_END_KILLCOUNT		= 0,
	ROOM_END_ROUND,
	ROOM_END_MOD,
};

#ifndef NEW_EXPPOINT

struct TYPE_ROOM_GET_EXPPOINT
{
	INT16					_i16WinExp;		// 승리시 추가 경험치
	INT16					_i16WinPoint;	// 승리시 추가 포인트
	INT16					_i16LoseExp;	// 패배시 추가 경험치
	INT16					_i16LosePoint;	// 패배시 추가 포인트
	INT16					_i16DrawExp;	// 무승부시 추가 경험치
	INT16					_i16DrawPoint;	// 무승부시 추가 포인트
};

struct TYPE_ROOM_KILL_EXPPOINT
{
	INT16					_i16KillExp;		// 킬 보너스 경험치
	INT16					_i16KillPoint;		// 킬 보너스 포인트
	INT16					_i16MultiPt;		// 연속킬 보너스
	INT16					_i16MultiPt_Head;	// 연속 헤드샷 보너스
	INT16					_i16MultiPt_Slug;	// 연속 근접킬 보너스
	INT16					_i16MultiPt_Stop;	// 연속킬 저지 보너스
	INT16					_i16MultiPt_Mass;	// 대량킬 보너스
	INT16					_i16MultiPt_Pier;	// 피어싱샷 보너스
};

#else

struct TYPE_ROOM_GET_ROUND_EXP
{
	INT16					_i16Round;					// 라운드제 게임
 	INT16					_i16KillPerRound1Round;		// 라운드 방식 게임 모드에서 1 라운드 당 최대 킬 수 제한
 	INT16					_i16rLowKillFactor;		// 최대 킬 수 범위 내에서 낮은  범위( 1 ~ 30 % )에서 1 킬 당 점수 ( float*10 )
 	INT16					_i16rMidKillFactor;		// 최대 킬 수 범위 내 중간 범위( 31 ~ 70 % )에서 1 킬 당 점수 ( float*10 )
 	INT16					_i16rHighKillFactor;	// 최대 킬 수 범위 내 높은 범위(71 ~ 100 % )에서 1 킬 당 점수 ( float*10 )
 	INT16				 	_i16HeadshotScore;		// 모든 모드 공통의 보너스 조건 - 헤드샷 1 회당 점수
 	INT16				 	_i16ChainKillScore;		// 모든 모드 공통의 보너스 조건 - 연속 킬 1 회당 점수
 	INT16				 	_i16ChainMeleeScore;	// 모든 모드 공통의 보너스 조건 - 연속 근접 킬 1 회당 점수
	INT16				 	_i16HeadshotMax;		// 모든 모드 공통의 보너스 조건 - 헤드샷 최대 회수
 	INT16				 	_i16ChainKillMax;		// 모든 모드 공통의 보너스 조건 - 연속 킬 최대 회수
 	INT16				 	_i16ChainMeleeMax;		// 모든 모드 공통의 보너스 조건 - 연속 근접 킬 최대 회수
	INT16					_ai16rUserCountValue[SLOT_MAX_COUNT];	// 인원 점수 (float*10)
};

struct TYPE_ROOM_GET_ROUND_POINT
{
	INT16					_ai16r2RankPoint[SLOT_MAX_COUNT];		// 등수 점수 (float*100)
	INT16					_ai16rUserCountValue[SLOT_MAX_COUNT];	// 인원 점수 (float*10)
 	INT16				 	_i16HeadshotScore;						// 모든 모드 공통의 보너스 조건 - 헤드샷 1 회당 점수
 	INT16				 	_i16ChainKillScore;						// 모든 모드 공통의 보너스 조건 - 연속 킬 1 회당 점수
 	INT16				 	_i16ChainMeleeScore;					// 모든 모드 공통의 보너스 조건 - 연속 근접 킬 1 회당 점수
	INT16				 	_i16HeadshotMax;						// 모든 모드 공통의 보너스 조건 - 헤드샷 최대 회수
 	INT16				 	_i16ChainKillMax;						// 모든 모드 공통의 보너스 조건 - 연속 킬 최대 회수
 	INT16				 	_i16ChainMeleeMax;						// 모든 모드 공통의 보너스 조건 - 연속 근접 킬 최대 회수
	INT16					_i16BonusScore;							// 미션 포인트
	INT16					_ai16BonusMaxCount[MISSION_POINT_MAX_COUNT];		// 미션 포인트 최대 회수
};

struct TYPE_ROOM_GET_ROUND_EXPPOINT_RATE
{
	INT16					_i16rWinExp;			// (float*10)
	INT16					_i16rDrawExp;			// (float*10)
	INT16					_i16rLoseExp;			// (float*10)
	INT16					_i16r2WinPoint;			// (float*100)
	INT16					_i16r2DrawPoint;		// (float*100)
	INT16					_i16r2LosePoint;		// (float*100)
};

struct TYPE_ROOM_GET_EXPPOINT
{
	INT16					_i16rExpPerRound;		// 라운드 요소가 경험치에 미치는 영향을 조절하는 가중치 값 (float*10)
 	INT16					_i16rExpPerKill;		// 킬 요소가 경험치에 미치는 영향을 조절하는 가중치 값 (float*10)
 	INT16					_i16rExpPerMission;		// 임무 요소가 경험치에 미치는 영향을 조절하는 가중치 값 (float*10)
};

struct TYPE_ROOM_GET_MEMBER_RATE
{
	BOOL					_bUseMemberRate;		// 인원 비율 적용 여부
	INT16					_i16MemberRangeEnd;		// 각 모드 게임 중간인원 구간 종료 값 (float*10)
	INT16					_i16rMemberRangeRate;	// 각 모드 게임 중간인원 가중치 (float*10)
	INT16					_i16rmemberMinRate;		// 각 모드 최소 인원 구간 가중치 (float*10)
};

struct TYPE_ROOM_GET_EXPPOINT_VALUES
{
	TYPE_ROOM_GET_ROUND_EXP				_stRoundExp;
	TYPE_ROOM_GET_ROUND_POINT			_stRoundPoint;
	TYPE_ROOM_GET_ROUND_EXPPOINT_RATE	_stRoundExpPointRate;
	TYPE_ROOM_GET_EXPPOINT				_stExpPoint;
	TYPE_ROOM_GET_MEMBER_RATE			_stMemberRate;
};

#define POINT_PER_SECOND	2
#endif

class CRoomBase : public i3ElementBase
{
	I3_CLASS_DEFINE( CRoomBase ); 

protected:
	ROOM_READY_OPTION			m_eReadyOption;
	ROOM_START_OPTION			m_eStartOption;
	ROOM_PRESTART_OPTION		m_ePreStartOption;
	ROOM_RESPAWN_OPTION			m_eRespawnOption;
	ROOM_CHATTING_OPTION		m_eChattingOption;
	ROOM_END_OPTION				m_eEndOption;

#ifndef NEW_EXPPOINT
	TYPE_ROOM_GET_EXPPOINT		m_stGetExpPoint;
	TYPE_ROOM_KILL_EXPPOINT		m_stKillExpPoint;

	REAL32						m_pr32SubUserCount[SLOT_MAX_COUNT];
	REAL32						m_pr32SubUserCountVS4[SLOT4VS4_MAX_COUNT];
#else
	TYPE_ROOM_GET_EXPPOINT_VALUES		m_stExpPointValues;
	TYPE_ROOM_GET_EXPPOINT_VALUES *		m_pstExtPointValues;
#endif
	
	TYPE_ROOM_INFO*				m_pInfo;

	UINT32						m_ui32RoundCount;
	UINT32						m_ui32RespawnCount;						// 허용하는 리스폰 수

	bool						m_bNewTrex; 

protected:
	void						_SendPacketBattlePlayer( i3NetworkPacket * pPacket, INT32 i32TeamIdx = SERVER_VALUE_UNKNOWN );

public:
	CRoomBase();
	virtual ~CRoomBase();

	TYPE_ROOM_INFO*				GetTypeRoomInfo()									{	return m_pInfo;				}

	ROOM_READY_OPTION			GetReadyOption()									{	return m_eReadyOption;		}
	ROOM_START_OPTION			GetStartOption()									{	return m_eStartOption;		}
	ROOM_PRESTART_OPTION		GetPreStartOption()									{	return m_ePreStartOption;	}	
	ROOM_RESPAWN_OPTION			GetRespawnOption()									{	return m_eRespawnOption;	}
	ROOM_CHATTING_OPTION		GetChattingOption()									{	return m_eChattingOption;	}
		
	virtual BOOL				OnCreate( TYPE_ROOM_INFO* pRoomInfo );
	void						OnDestroy();

	virtual BOOL				CheckSubType( UINT8 ui8SubType );
	virtual BOOL				SetRoomOption( UINT32 ui32StageID )					{	return FALSE;		}						// 최초 Room Option 저장 ( RoomCreate, RoomOptionChange )

	virtual BOOL				CheckReady()										{	return TRUE;		}						// 레디 버튼 자격 검사	
	virtual bool				CheckRespawn( INT32 i32SlotIdx );
	virtual bool				CheckRespawnAi( INT32 i32SlotIdx )					{	return TRUE;		}
	virtual bool				CheckLoadingStart();																				// 로딩 시작 조건 검사

	virtual bool				CheckMatchStart();																					// 게임 시작 조건 검사	
	virtual BOOL				CheckMatchEnd()										{	return FALSE;		}						// 게임 종료 조건 검사
	virtual BOOL				CheckRoundStart()									{	return TRUE;		}						// 라운드 시작 조건 검사
	virtual BOOL				CheckRoundEnd( UINT32 ui32ServerTime )				{	return FALSE;		}						// 라운드 종료 조건 검사

	virtual BOOL				UserGiveUpBattle( INT32 i32SlotIdx )				{	return FALSE;		}

#ifndef NEW_EXPPOINT
	virtual void				UserKillBouns( INT32 i32SlotIdx, UINT32 ui32DeathType, UINT32 ui32DeathMyState, UINT32 ui32DeathEnemyState, UINT32 ui32RTBattle )	{}	// 유저가 죽었을 경우 보너스
#else
	virtual UINT16				UserKillBouns( INT32 i32SlotIdx, UINT32 ui32DeathType, UINT32 ui32DeathMyState, UINT32 ui32DeathEnemyState, UINT32 ui32RTBattle );	// 유저가 죽었을 경우 보너스
#endif
	virtual void				UserMatchEndBouns( INT32 i32SlotIdx );																// 게임 종료시 보너스

	virtual BOOL				OnUpdatePreBattle( UINT32 ui32StateTime, UINT32 ui32ServerTime )	{	return TRUE;		}						// 홀펀칭 후 게임시작전 까지 Update

	virtual void				SetMatchStart()										{}												// Match 시작시
	virtual void				SetRoundStart()										{}												// Round 시작시
	virtual bool				RoundStart( UINT32 ui32RTBattle)					{ return false; }								// Round 시작시

	virtual INT32				GetWinTeam()										{	return TEAM_DRAW;	}
#ifndef NEW_EXPPOINT
	virtual void				GetBasicExpPoint( INT32 i32Win, TYPE_ROOM_GET_EXPPOINT* pstGetExpPoint );							// 정상 종료시 얻어가는 Exp, Point 수치
	TYPE_ROOM_KILL_EXPPOINT*	GetKillExpPoint()									{	return &m_stKillExpPoint;	}
#else
	virtual INT16				GetLimitKillCount();
	virtual INT16				GetKillExp( INT32 i32SlotIdx );
	virtual INT16				GetMissionExp( INT32 i32SlotIdx );
	virtual INT16				GetRoundExp( INT32 i32SlotIdx, INT32 i32Win, BOOL bNormalEnd );
	virtual INT16				GetMissionPoint( INT32 i32SlotIdx );

	virtual void				SetRoundDataCount();								// 라운드 마다의 기록이 필요한 데이터들 처리
#endif

#ifndef NEW_EXPPOINT
	virtual REAL32				GetSubPercentOfUserCount( INT32 i32Count );	
	virtual REAL32				GetSubPercentOfUserCount2( INT32 i32Count );	
	virtual REAL32				GetSubPercentOfUnusualEnd( INT32 i32Win )			{	return 0.f;			}
#endif
	virtual REAL32				GetSubPercentOfUserCountTeam( INT32 i32Team, INT32 i32AllCount, INT32 i32Count );	

	virtual UINT16				GetMatchAccExp( INT32 i32SlotIdx, UINT16 ui16AccExp );
	virtual UINT16				GetMatchAccPoint( INT32 i32SlotIdx, UINT16 ui16AccPoint );

	virtual	UINT16				GetMatchEndExp( INT32 i32Win, INT32 i32SlotIdx, UINT16 ui16AccExp, REAL32 r32AdjustPercent, BOOL bNormalEnd, UINT32 ui32RTBattle );
	virtual	UINT16				GetMatchEndPoint( INT32 i32Win, INT32 i32SlotIdx, UINT16 ui16AccPoint, REAL32 r32AdjustPercent, BOOL bNormalEnd,
#ifdef NEW_EXPPOINT
																																			INT8 * pi8Raning, INT8 i8UserCount, UINT32 ui32RTBattle );
#else
																																			UINT32 ui32RTBattle );
#endif

	virtual bool				ExistRecord( INT32 i32SlotIdx )						{ return !((m_pInfo->_aui16AccExp[i32SlotIdx] == 0) && (m_pInfo->_aM_UserScore[i32SlotIdx].m_KillCount == 0) && (m_pInfo->_aM_UserScore[i32SlotIdx].m_DeathCount == 0)); }

	// 패킷에 모드별 정보 추가시 사용
	virtual void				AddStartBattlePacket( i3NetworkPacket* pPacket, CUserSession * pSession, UINT32 ui32RTBattle ) {}
	virtual void				AddGiveUpBattlePacket( i3NetworkPacket* pPacket, CUserSession * pSession )			{}
	virtual void				AddDeathBattlePacket( i3NetworkPacket* pPacket )									{}
	virtual void				AddRespawnPacket( i3NetworkPacket* pPacket, CUserSession * pSession )				{}
	virtual void				AddRespawnAiPacket( i3NetworkPacket* pPacket, INT32 i32SlotIdx )					{}

	virtual void				UpdateUserState( INT32 i32Idx )						{}
	virtual bool				CheckTrex( void )			{ return TRUE; }

	virtual void				periodicUpdateRoomState( UINT32 ui32ServerTime )	{}				// 메인스레드에서만 부를것


	// Mission Message
	// Bomb
	virtual BOOL			MissionBombInstall( INT32 i32SlotIdx, UINT8 ui8BombPosition, REAL32 * pr32BombVecPos, UINT32 ui32RTBattle ) {	return FALSE;	}
	virtual BOOL			MissionBombUnInstall( INT32 i32SlotIdx )			{	return FALSE;	}
	// Destroy
	virtual BOOL			MissionObjectDestroy( DESTRUCTION_INFO* pInfo )		{	return FALSE;	}
	// Defance
	virtual BOOL			MissionObjectDefence( DEFENCE_INFO * pInfo )		{	return FALSE;	}
	// Boss
	virtual BOOL			MissionTouchDown( CUserSession * pSession, INT32 i32SlotIdx, UINT32 ui32RTBattle )	{	return FALSE;	}
	virtual BOOL			GetTouchDownChainCount( INT32 i32SlotIdx )									{	return 0;		}
	// TUTORIAL
	virtual BOOL			MissionTutorialRoundEnd( CUserSession* pSession ) {	return FALSE;	}

	virtual void			GetRanking( INT8 *i8Ranking, INT8 i8BattleUserCount, GET_RANKING_TYPE rankingType = GET_RANKING_KILLDEATH );

	// 2011.05.30 삽입. 2011.07.04 수정. 안국정
	virtual BOOL			CheckVoteTime( UINT32 ui32ServerTime );
};

#endif