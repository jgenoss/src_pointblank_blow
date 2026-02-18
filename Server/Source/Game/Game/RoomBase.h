#ifndef __ROOM_BASE_H__
#define __ROOM_BASE_H__

#include "UserSession.h"

#define RED_TEAM_MASK		0x5555
#define BLUE_TEAM_MASK		0xAAAA

enum ROOM_READY_OPTION
{
	ROOM_READY_ONE_TEAM		= 0,		// 한팀에 대해 체크(enum값0 변경되어선 안됩니다. CheckUserReady()에서 사용)
	ROOM_READY_BOTH_TEAM,				// 양팀에 대해 체크(enum값1 변경되어선 안됩니다. CheckUserReady()에서 사용)
	ROOM_READY_BOTH_TEAM_CLAN,			// 양팀에 대해 체크하되, 클랜도 체크
	ROOM_READY_ALL_READY,				// 모두 레디해야 시작할 수 있음
};

enum ROOM_JOIN_OPTION
{
	ROOM_JOIN_SEQUENCE		= 0,		// 빈슬롯중 제일 적은 번호로 
	ROOM_JOIN_SEQUENCE_USER_FOLLOW,		// 빈슬롯중 제일 적은 번호로 가되 기존 유저가 있는 팀으로
	ROOM_JOIN_CLANMATCH,				// 비클랜메치메이킹 클랜전 룰대로
	ROOM_JOIN_CLANMATCH_MAKING,			// 클랜매치메이킹 클랜전 룰대로
	ROOM_JOIN_IMPOSSIBLE,				// 슬롯을 할당받을수 없음
};

enum ROOM_PRESTART_OPTION
{
	ROOM_PRESTART_DIRECT	= 0,		// 방장 로딩 후 혼자 시작
	ROOM_PRESTART_ONE_WAIT,				// 방장 로딩 후 한명의 유저라도 있으면 바로 시작
	ROOM_PRESTART_ALL_WAIT,				// 방장 로딩 후 다른 유저 기다림
};

// Acc 가 붙은 변수는 게임 종료시까지 초기화 되지 않습니다.
// Rou 가 붙은 변수는 라운드에만 사용 됩니다.
struct TYPE_ROOM_INFO
{
	UINT32				_ui32ChannelNum;

	UINT64				m_ui64BattleUID;					// 배틀 고유 인덱스. ZLog 에 사용

	INT32				_i32StageType;						//킬 포인트, 피노를 세팅하기 위한값, 게임 모드가 들어감 모드 인덱스 //내부에서 사용하기 위해서 사용 이중변수임..쩝.
	INT32				_i32MiStateType;					// 미션 스테이트

	INT32				_i32MainSlotIdx;

	ROOM_INFO_BASIC		_InfoBasic;							// 기본 방 정보
	ROOM_INFO_ADD		_InfoAdd;							// 추가 방 정보

	UINT32				_ui32GameStartDate;					// 게임 최초 시작 날짜

	UINT32				_ui32RTRoundStart;					// Battle 시작 타임

	UINT16				_aui16TotalRoundCount[ TEAM_COUNT ];	// 방에서 표시되는 카운트

	UINT16				_aui16AccExp[SLOT_MAX_COUNT];		// 배틀 종료후 점수
	UINT16				_aui16AccPoint[SLOT_MAX_COUNT];		// 배틀 종료후 점수
	
	
	UINT16				_aui16EventExp[SLOT_MAX_COUNT];		// 배틀 종료후 점수
	UINT16				_aui16EventPoint[SLOT_MAX_COUNT];	// 배틀 종료후 점수

	UINT8				_aui8ResultIcon[SLOT_MAX_COUNT];	// 결과창 아이콘

	SCORE_INFO			_aTeamScoreTotal[TEAM_COUNT];		// Total 전체 스코어 

	// Match 개인 스코어
	SCORE_INFO			_aM_UserScore[SLOT_MAX_COUNT];			// Kill, Death
	UINT16				_aui16M_Headshot[SLOT_MAX_COUNT];		// 해드샷 카운트
	UINT16				_aui16M_PiercingShot[ SLOT_MAX_COUNT ];
	UINT16				_aui16M_MassKill[ SLOT_MAX_COUNT ];
	UINT16				_aui16M_ChainStoppper[ SLOT_MAX_COUNT ];
	UINT16				_aui16M_ChainHeadShot[ SLOT_MAX_COUNT ];
	UINT16				_aui16M_ChainSlugger[ SLOT_MAX_COUNT ];
	UINT16				_aui16M_AccKill_1[ SLOT_MAX_COUNT ];
	UINT16				_aui16M_AccKill_2[ SLOT_MAX_COUNT ];
	UINT16				_aui16M_AccKill_3[ SLOT_MAX_COUNT ];
	UINT16				_aui16M_Intercept[ SLOT_MAX_COUNT ];	// 탈출미션 인터셉트
	UINT16				_aui16M_ChainTouchdown[ SLOT_MAX_COUNT ];
	UINT16				_aui16M_KillHuman[ SLOT_MAX_COUNT ];
	// 계산용 연속적으로 행하는 일.
	UINT8				m_aui8A_KillCount[SLOT_MAX_COUNT];		// 계산용으로 사용		// 연속킬에서 사용
	UINT8				m_aui8A_HeadShot[SLOT_MAX_COUNT];		// 계산용으로 사용 
	UINT8				m_aui8A_SlugShot[SLOT_MAX_COUNT];		// 계산용으로 사용 


	SCORE_INFO			_aUserScoreRound[SLOT_MAX_COUNT];		// Round 개인 스코어
	SCORE_INFO			_aTeamScoreRound[TEAM_COUNT];			// Round 전체 스코어

	UINT16				_aui16Acc_KillExp[SLOT_MAX_COUNT];		// 라운드당 최대 킬 수 까지만 포함하여 쌓은 킬 수
	UINT16				_aui16Acc_AssistExp[SLOT_MAX_COUNT];	// 라운드당 최대 어시 수 까지만 포함한 어시스트수

	INT32				_ai32AcidBombingCount[SLOT_MAX_COUNT];	// 애시드 자폭 횟수 ( 자폭시 DeathCount 가 증가하지 않아 리스폰 해킹 검사를 위해 추가, _CheckRespawn() ) - 서동권
	UINT32				_aui32RespawnCount[SLOT_MAX_COUNT];		// 리스폰 카운트

	UINT8				_aui8AccMissionCount[SLOT_MAX_COUNT];	// 미션 카운트

	// 계산용 연속적으로 행하는 일.
	UINT8				_aui8A_Mission[ SLOT_MAX_COUNT ];		// 미션 연속 달성 카운트 (리스폰시 초기화)

	CUserSession	*	_pSession[SLOT_MAX_COUNT];				// 유저에 대한 포인터
	SLOT_INFO			_aSlotInfo[SLOT_MAX_COUNT];				// 슬롯 정보

	UINT64				_ui64Temp1;	// 확인용 - 추후 제거
	UINT64				_ui64Temp2; // 확인용 - 추후 제거


	UINT16				_aui16AccChallengeScore[SLOT_MAX_COUNT];// 챌린지 모드 점수

	UINT32				_ui32FirstBattleStartUser;			// 처음 게임을 시작한 유저 (Ready를 해서 같이 시작한 유저)
	UINT16				_ui16BattleUser;					// 게임중인 유저( 옵저버는 제외 )
	UINT16				_ui16RoundStartUser;				// 라운드 시작 유저
	UINT16				_ui16RoundDeathUser;				// 라운드 죽은 유저
	UINT32				_ui32HackOffUser;					// 핵체크 예외 유저

	UINT8				_ui8MiEndType;						// 미션 엔트 타입
	UINT8				_ui8RoundWinTeam;					// 승리팀

	DINO_ROOM_INFO		_MiDinoInfo;
	bool				_abDinoStateUpdate[SLOT_MAX_COUNT];
	UINT32				_aui32DInoCharInfo[SLOT_MAX_COUNT];

	UINT32				m_aui32AILevel[ SLOT_MAX_COUNT ];	// AI 레벨.. 혹시나 추후에 1:15 같은게 나올지 몰라 16으로 가겠습니다.
	// 챌린지모드
	INT8				m_i8enemyAiCount;					// 챌린지모드 NPC 수
	INT8				m_i8difficultyLevel;				// 사격장 난이도
	INT8				m_i8currentDifficultyLevel;			//

	UINT8				m_ui8CountDownSec;					// 배틀시작전 카운트 다운 시간. ( Sec )

	UINT32				_aui32BattleClanIdx[TEAM_COUNT];		// 클랜 인덱스 정보
	T_TeamID			m_TMatchTeamID[TEAM_COUNT];				// 클랜 매치팀 ID.

	// 유저슬롯체크 구조개선	2014.03.28
	ROOM_READY_OPTION	m_eReadyOption;			// 게임 시작 체크 조건 1. 한팀에 대해 체크 2. 양팀에 대해 체크 3. 모두 레디해야 시작 가능
	INT8				m_i8PersonAtLeast;		// m_eReadyOption 조건이 1 혹은 2일때 체크하는 최소요구 인원수
	ROOM_JOIN_OPTION	m_eJoinOption;			// 방 조인시 배정받는 슬롯번호 조건 1. 빈슬롯중 제일 적은 번호로 2. 빈슬롯중 제일 적은 번호로 가되 기존 유저가 있는 팀으로
	ROOM_PRESTART_OPTION	m_ePreStartOption;

	UINT8				m_ui8PlayRoundCound[SLOT_MAX_COUNT];

	UINT8				m_aui8UsersScopeType[SLOT_MAX_COUNT];
};

void	SetRoomInfoNonMatchMaking( TYPE_ROOM_INFO & destInfo );
void	SetRoomInfoMatchMaking( TYPE_ROOM_INFO & destInfo );

enum ROOM_START_OPTION
{
	ROOM_START_DIRECT		= 0,		// 방장 레뒤 시 바로 시작됨
	ROOM_START_COUNTDOWN,				// 방장 레뒤 시 카운트 다운
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

enum ROOM_ATKDEF_CHANGE_OPTION
{
	ROOM_ATKDEF_NOT_CHANGE	= 0,		// 공수교대 사용하지 않습니다.
	ROOM_ATKDEF_CHANGE,					// 공수교대 사용합니다.
};

typedef struct __tag_RoomGetExpPoint_V1
{
	INT16					_i16WinExp;		// 승리시 추가 경험치
	INT16					_i16WinPoint;	// 승리시 추가 포인트
	INT16					_i16LoseExp;	// 패배시 추가 경험치
	INT16					_i16LosePoint;	// 패배시 추가 포인트
	INT16					_i16DrawExp;	// 무승부시 추가 경험치
	INT16					_i16DrawPoint;	// 무승부시 추가 포인트
}TYPE_ROOM_GET_EXPPOINT_V1;

typedef struct __tag_RoomKillExpPoint
{
	INT16					_i16KillExp;		// 킬 보너스 경험치
	INT16					_i16KillPoint;		// 킬 보너스 포인트
	INT16					_i16MultiPt;		// 연속킬 보너스
	INT16					_i16MultiPt_Head;	// 연속 헤드샷 보너스
	INT16					_i16MultiPt_Slug;	// 연속 근접킬 보너스
	INT16					_i16MultiPt_Stop;	// 연속킬 저지 보너스
	INT16					_i16MultiPt_Mass;	// 대량킬 보너스
	INT16					_i16MultiPt_Pier;	// 피어싱샷 보너스
	INT16					_i16AssistExp;		// 어시스트 보너스 경험치
}TYPE_ROOM_KILL_EXPPOINT;

typedef struct __tagRoomGetRoundExp
{
	INT16					_i16Round;					// 라운드제 게임
 	INT16					_i16KillPerRound1Round;		// 라운드 방식 게임 모드에서 1 라운드 당 최대 킬 수 제한
	INT16					_i16AssistPerRound1Round;	// 라운드 방식 게임 모드에서 1 라운드 당 최대 어시스트 수 제한.
 	INT16					_i16rLowKillFactor;		// 최대 킬 수 범위 내에서 낮은  범위( 1 ~ 30 % )에서 1 킬 당 점수 ( float*10 )
 	INT16					_i16rMidKillFactor;		// 최대 킬 수 범위 내 중간 범위( 31 ~ 70 % )에서 1 킬 당 점수 ( float*10 )
 	INT16					_i16rHighKillFactor;	// 최대 킬 수 범위 내 높은 범위(71 ~ 100 % )에서 1 킬 당 점수 ( float*10 )
 	INT16				 	_i16HeadshotScore;		// 모든 모드 공통의 보너스 조건 - 헤드샷 1 회당 점수
 	INT16				 	_i16ChainKillScore;		// 모든 모드 공통의 보너스 조건 - 연속 킬 1 회당 점수
 	INT16				 	_i16ChainMeleeScore;	// 모든 모드 공통의 보너스 조건 - 연속 근접 킬 1 회당 점수
	INT16				 	_i16HeadshotMax;		// 모든 모드 공통의 보너스 조건 - 헤드샷 최대 회수
 	INT16				 	_i16ChainKillMax;		// 모든 모드 공통의 보너스 조건 - 연속 킬 최대 회수
 	INT16				 	_i16ChainMeleeMax;		// 모든 모드 공통의 보너스 조건 - 연속 근접 킬 최대 회수
	INT16					_i16rUserCountValue[SLOT_MAX_COUNT];	// 인원 점수 (float*10)
}TYPE_ROOM_GET_ROUND_EXP;

typedef struct __tagRoomGetRoundPoint
{
	INT16					_i16r2RankPoint[SLOT_MAX_COUNT];		// 등수 점수 (float*100)
	INT16					_i16rUserCountValue[SLOT_MAX_COUNT];	// 인원 점수 (float*10)
 	INT16				 	_i16HeadshotScore;						// 모든 모드 공통의 보너스 조건 - 헤드샷 1 회당 점수
 	INT16				 	_i16ChainKillScore;						// 모든 모드 공통의 보너스 조건 - 연속 킬 1 회당 점수
 	INT16				 	_i16ChainMeleeScore;					// 모든 모드 공통의 보너스 조건 - 연속 근접 킬 1 회당 점수
	INT16				 	_i16HeadshotMax;						// 모든 모드 공통의 보너스 조건 - 헤드샷 최대 회수
 	INT16				 	_i16ChainKillMax;						// 모든 모드 공통의 보너스 조건 - 연속 킬 최대 회수
 	INT16				 	_i16ChainMeleeMax;						// 모든 모드 공통의 보너스 조건 - 연속 근접 킬 최대 회수
	INT16					_i16BonusScore;							// 미션 포인트
	INT16					_i16BonusMaxCount[MISSION_POINT_MAX_COUNT];		// 미션 포인트 최대 회수
}TYPE_ROOM_GET_ROUND_POINT;

typedef struct __tagRoomGetRoundExpPointRate
{
	INT16					_i16rWinExp;			// (float*10)
	INT16					_i16rDrawExp;			// (float*10)
	INT16					_i16rLoseExp;			// (float*10)
	INT16					_i16r2WinPoint;			// (float*100)
	INT16					_i16r2DrawPoint;		// (float*100)
	INT16					_i16r2LosePoint;		// (float*100)
}TYPE_ROOM_GET_ROUND_EXPPOINT_RATE;

typedef struct __tagRoomGetExpPointV2
{
	INT16					_i16rExpPerRound;		// 라운드 요소가 경험치에 미치는 영향을 조절하는 가중치 값 (float*10)
 	INT16					_i16rExpPerKill;		// 킬 요소가 경험치에 미치는 영향을 조절하는 가중치 값 (float*10)
 	INT16					_i16rExpPerMission;		// 임무 요소가 경험치에 미치는 영향을 조절하는 가중치 값 (float*10)
	INT16					_i16rExpPerAssist;		// 어시스트 가중치값.
}TYPE_ROOM_GET_EXPPOINT_V2;

typedef struct __tagRoomGetMemberRate
{
	BOOL					_bUseMemberRate;		// 인원 비율 적용 여부
	INT16					_i16MemberRangeEnd;		// 각 모드 게임 중간인원 구간 종료 값 (float*10)
	INT16					_i16rMemberRangeRate;	// 각 모드 게임 중간인원 가중치 (float*10)
	INT16					_i16rmemberMinRate;		// 각 모드 최소 인원 구간 가중치 (float*10)
}TYPE_ROOM_GET_MEMBER_RATE;

typedef struct __tagRoomGetExpPointValues
{
	TYPE_ROOM_GET_ROUND_EXP				_stRoundExp;
	TYPE_ROOM_GET_ROUND_POINT			_stRoundPoint;
	TYPE_ROOM_GET_ROUND_EXPPOINT_RATE	_stRoundExpPointRate;
	TYPE_ROOM_GET_EXPPOINT_V2			_stExpPoint;
	TYPE_ROOM_GET_MEMBER_RATE			_stMemberRate;
}TYPE_ROOM_GET_EXPPOINT_VALUES;

#define POINT_PER_SECOND	2

class CRoomBase : public i3ElementBase
{
	I3_CLASS_DEFINE( CRoomBase ); 

protected:
	ROOM_START_OPTION			m_eStartOption;
	ROOM_RESPAWN_OPTION			m_eRespawnOption;
	ROOM_CHATTING_OPTION		m_eChattingOption;
	ROOM_END_OPTION				m_eEndOption;

	TYPE_ROOM_GET_EXPPOINT_V1	m_stGetExpPointV1;
	TYPE_ROOM_KILL_EXPPOINT		m_stKillExpPoint;

	REAL32**					m_pr32SubUserCount;

	//REAL32						m_pr32SubUserCountVS5[SLOT_MAX_COUNT];
	//REAL32						m_pr32SubUserCountVS4[SLOT_MAX_COUNT];
//#else
	TYPE_ROOM_GET_EXPPOINT_VALUES		m_stExpPointValues;
//	TYPE_ROOM_GET_EXPPOINT_VALUES		m_stExpPointValues4VS4;
	TYPE_ROOM_GET_EXPPOINT_VALUES *		m_pstExtPointValues;
//#endif
	
	TYPE_ROOM_INFO*				m_pInfo;

	UINT32						m_ui32RoundCount;
	//UINT32					m_ui32RoundTime;
	UINT32						m_ui32RespawnCount;						// 허용하는 리스폰 수

	bool						m_bNewTrex; 

protected:
	void						_SendPacketBattlePlayer( i3NetworkPacket * pPacket, INT32 i32TeamIdx = SERVER_VALUE_UNKNOWN );

	//void						_ResultUpdateUser_M( INT32 i32ThreadIdx, INT32 i32SlotIdx );

public:
	CRoomBase();
	virtual ~CRoomBase();

	TYPE_ROOM_INFO*				GetTypeRoomInfo()									{	return m_pInfo;				}

	ROOM_START_OPTION			GetStartOption()									{	return m_eStartOption;		}
	ROOM_RESPAWN_OPTION			GetRespawnOption()									{	return m_eRespawnOption;	}
	ROOM_CHATTING_OPTION		GetChattingOption()									{	return m_eChattingOption;	}
	UINT32						GetRoundCount()										{ return m_ui32RoundCount;		}			
		
	virtual BOOL				OnCreate( TYPE_ROOM_INFO* pRoomInfo );
	void						OnDestroy();

	UINT8						GetAtkDefChange();
	UINT8						GetWinTeamCurRound( UINT8 ui8WinTeam );

	virtual BOOL				CheckSubType( UINT8 ui8SubType );
	virtual BOOL				SetRoomOption( UINT32 ui32StageID )					{	return FALSE;		}						// 최초 Room Option 저장 ( RoomCreate, RoomOptionChange )

	virtual bool				CheckRespawn( INT32 i32SlotIdx );
	virtual bool				CheckRespawnAi( INT32 i32SlotIdx )					{	return TRUE;		}

	virtual BOOL				CheckMatchEnd()										{	return FALSE;		}						// 게임 종료 조건 검사
	virtual BOOL				CheckRoundStart()									{	return TRUE;		}						// 라운드 시작 조건 검사
	virtual BOOL				CheckRoundEnd( UINT32 ui32RTServer )				{	return FALSE;		}						// 라운드 종료 조건 검사

	virtual BOOL				UserGiveUpBattle( INT32 i32SlotIdx )				{	return FALSE;		}
	virtual void				UserKillBounsV1( INT32 i32SlotIdx, UINT32 ui32DeathType, UINT32 ui32DeathMyState, UINT32 ui32DeathEnemyState, UINT32 ui32RTBattle )	{}	// 유저가 죽었을 경우 보너스
	virtual UINT16				UserKillBounsV2( INT32 i32SlotIdx, UINT32 ui32DeathType, UINT32 ui32DeathMyState, UINT32 ui32DeathEnemyState, UINT32 ui32RTBattle );	// 유저가 죽었을 경우 보너스
	virtual void				UserMatchEndBouns( INT32 i32SlotIdx );																// 게임 종료시 보너스

	virtual BOOL				OnUpdatePreBattle( UINT32 ui32RTState, UINT32 ui32RTServer );						// 홀펀칭 후 게임시작전 까지 Update

	virtual void				SetMatchStart()										{}												// Match 시작시
	virtual void				SetRoundStart()										{}												// Round 시작시
	virtual void				RoundStart( UINT32 ui32RTBattle)					{}												// Round 시작시
	virtual void				RoundStart( REAL32 r32RTBattle)					{}												// Round 시작시

	virtual INT32				GetWinTeam()										{	return TEAM_DRAW;	}

	virtual void				GetRoomNowScore( ROOM_NOW_SCORE* pRoomNowScore, UINT32 ui32RTServer );

	virtual void				GetBasicExpPoint( INT32 i32Win, TYPE_ROOM_GET_EXPPOINT_V1* pstGetExpPoint );							// 정상 종료시 얻어가는 Exp, Point 수치
	TYPE_ROOM_KILL_EXPPOINT*	GetKillExpPoint()									{	return &m_stKillExpPoint;	}

	virtual INT16				GetLimitKillCount();
	virtual INT16				GetLimitAssistCount();
	virtual INT16				GetKillExp( INT32 i32SlotIdx );
	virtual INT16				GetAssistExp(INT32 i32SlotIdx);
	virtual INT16				GetMissionExp( INT32 i32SlotIdx );
	virtual INT16				GetRoundExp( INT32 i32SlotIdx, INT32 i32Win, BOOL bNormalEnd );
	virtual INT16				GetMissionPoint( INT32 i32SlotIdx );

	virtual void				SetRoundDataCount();								// 라운드 마다의 기록이 필요한 데이터들 처리

	virtual REAL32				GetSubPercentOfUserCount( INT32 i32Count );	
	virtual REAL32				GetSubPercentOfUserCount2( INT32 i32Count );	
	virtual REAL32				GetSubPercentOfUnusualEnd( INT32 i32Win )			{	return 0.f;			}
	virtual REAL32				GetSubPercentOfUserCountTeam( INT32 i32Team, INT32 i32AllCount, INT32 i32Count );	

	virtual REAL32				GetPercentOfAsymmetryUserExp( INT32 i32Team, INT32 i32AllCount, INT32 i32TeamCount );
	virtual REAL32				GetPercentOfAsymmetryUserPoint( INT32 i32Team, INT32 i32AllCount, INT32 i32TeamCount );

	virtual UINT16				GetMatchAccExp( INT32 i32SlotIdx, UINT16 ui16AccExp );
	virtual UINT16				GetMatchAccPoint( INT32 i32SlotIdx, UINT16 ui16AccPoint );

	virtual	UINT16				GetMatchEndExp( INT32 i32Win, INT32 i32SlotIdx, UINT16 ui16AccExp, REAL32 r32AdjustPercent, REAL32 r32AsymPercent, BOOL bNormalEnd, UINT32 ui32RTBattle );
	virtual	UINT16				GetMatchEndPoint( INT32 i32Win, INT32 i32SlotIdx, UINT16 ui16AccPoint, REAL32 r32AdjustPercent, REAL32 r32AsymPercent, BOOL bNormalEnd, UINT32 ui32RTBattle );
	virtual	UINT16				GetMatchEndPoint( INT32 i32Win, INT32 i32SlotIdx, UINT16 ui16AccPoint, REAL32 r32AdjustPercent, REAL32 r32AsymPercent, BOOL bNormalEnd, INT8 * pi8Raning, INT8 i8UserCount, UINT32 ui32RTBattle );

	virtual bool				ExistRecord( INT32 i32SlotIdx )						{ return !((m_pInfo->_aui16AccExp[i32SlotIdx] == 0) && (m_pInfo->_aM_UserScore[i32SlotIdx].m_KillCount == 0) && (m_pInfo->_aM_UserScore[i32SlotIdx].m_DeathCount == 0)); }

	//virtual UINT16			ResultUpdate_M( INT32 i32Win, BOOL bNormalEnd );

	// 패킷에 모드별 정보 추가시 사용
	virtual void				AddStartBattlePacket( PACKET_BATTLE_STARTBATTLE_ACK* pPacket, CUserSession * pSession, UINT32 ui32RTBattle ) {}
	virtual void				AddGiveUpBattlePacket( i3NetworkPacket* pPacket, CUserSession * pSession )			{}
	virtual void				AddRespawnPacket( i3NetworkPacket* pPacket, CUserSession * pSession )				{}
	virtual void				AddRespawnAiPacket( i3NetworkPacket* pPacket, INT32 i32SlotIdx )					{}

	virtual void				UpdateUserState( INT32 i32Idx )						{}
	virtual bool				CheckTrex( void )			{ return TRUE; }

	virtual void				periodicUpdateRoomState( REAL32 r32RTServer )	{}				// 메인스레드에서만 부를것


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
	virtual UINT8			GetTouchDownChainCount( INT32 i32SlotIdx )											{	return 0;		}
	// TUTORIAL
	virtual BOOL			MissionTutorialRoundEnd( CUserSession* pSession ) {	return FALSE;	}

	virtual void			GetRanking( INT8 *i8Ranking, INT8 i8BattleUserCount, GET_RANKING_TYPE rankingType = GET_RANKING_KILLDEATH );

	// 2011.05.30 삽입. 2011.07.04 수정. 안국정
	virtual BOOL			CheckVoteTime( UINT32 ui32ServerTime );

	virtual UINT8			GetCountDownTime() {	return m_pInfo->m_ui8CountDownSec;	}

	virtual void			CommandIncreaseKillCount( INT32 i32SlotIdx, INT32 i32KillCount )	{}

	virtual void			CheckDefaultWeapon(ITEM_INFO* aWeaponList, INT32& i32CheckSkipFlag, INT32& i32WeaponUseStop, INT32 i32SlotIdx  )	{ return ; }

	virtual STAGE_ID		GetRandMap( STAGE_ID eStageID)	{ return eStageID; }

	virtual bool			IsDinoUser(INT32 i32SlotIdx)	{ return false; }
};

#endif