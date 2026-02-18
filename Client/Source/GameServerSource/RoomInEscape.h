#ifndef __ROOM_IN_ESCAPE_H__
#define __ROOM_IN_ESCAPE_H__

#include "RoomBase.h"

#define TOUCHDOWN_BLOCK_BONUS_TIME	30
class CRoomInEscape : public CRoomBase
{
	I3_CLASS_DEFINE( CRoomInEscape );

#ifndef NEW_EXPPOINT
	UINT8						m_ui8BonusEscapePoint;								// 탈출 수행 보너스 포인트
	UINT8						m_ui8BonusEscapeExp;								// 탈출 수행 보너스 경험치
	UINT8						m_ui8BonusChainEscapePoint;							// 연속 탈출 수행 보너스 포인트
	UINT8						m_ui8BonusKillPoint;								// 킬 보너스 포인트
	UINT8						m_ui8BonusKillExp;									// 킬 보너스 경험치
	UINT8						m_ui8BonusInterceptPoint;							// 인터셉트 보너스 포인트

	UINT8						m_ui8BonusMaxEscapePoint;							// 탈출 보너스 포인트를 받을 수 있는 최대 횟수
	UINT8						m_ui8BonusMaxEscapeExp;								// 탈출 보너스 경험치를 받을 수 있는 최대 횟수
	UINT8						m_ui8BonusMaxChainEscapePoint;						// 연속 탈출 보너스 포인트를 받을 수 있는 최대 횟수
	UINT8						m_ui8BonusMaxKillPoint;								// 킬 보너스 포인트를 받을 수 있는 최대 횟수
	UINT8						m_ui8BonusMaxKillExp;								// 킬 보너스 경험치를 받을 수 있는 최대 횟수
	UINT8						m_ui8BonusMaxInterceptPoint;						// 인터셉트 보너스 포인트를 받을 수 있는 최대 횟수
#else
	UINT8						m_ui8BonusEscapeMax;								// 탈출 횟수 최대 제한
	UINT8						m_ui8BonusEscapeScore;								// 탈출 점수
	UINT8						m_ui8BonusInterceptMax;								// 인터셉트 횟수 최대 제한
	UINT8						m_ui8BonusInterceptScore;							// 인터셉트 점수
	UINT8						m_ui8BonusDiffMax;									// 상대팀과의 점수 차이 보너스 최대값
	UINT8						m_ui8BonusDiffStandard;								// 상대팀과의 점수 차이 기준
	UINT8						m_ui8BonusDiffScore;								// 상대팀과의 점수 차이 기준 별 점수
#endif

	UINT8						m_ui8ScoreKill;										// 킬 점수
	UINT8						m_ui8ScoreIntercept;								// 인터셉트 점수
	UINT8						m_ui8ScoreBlock;									// 블록 점수
	UINT8						m_ui8ScoreEscape;									// 탈출 점수
	UINT8						m_ui8ScoreAceattacker;								// 2연속 탈출 점수
	UINT8						m_ui8ScoreHattrick;									// 3연속 탈출 점수

	UINT16						m_aui16RouTouchDownCount[ SLOT_MAX_COUNT ];
	UINT16						m_aui16MatAssist[ SLOT_MAX_COUNT ];

	UINT16						m_i16TouchDownFlag;

	UINT8						m_ui8A_TouchDownChainCount[SLOT_MAX_COUNT];			// 연속 터치다운 회수 (임무카드를 위해 필요)

	UINT32						m_ui32BlockBonusTime;								// 블록 보너스를 지급하는 시간
	UINT8						m_ui8LastTouchDownState;							// 마지막 터치다운 상태

protected:
	void						_InitUserState();									// 라운드 시작 전 유저 상태 설정

public:
	CRoomInEscape();
	virtual ~CRoomInEscape();

	virtual BOOL				OnCreate( TYPE_ROOM_INFO* pRoomInfo );
	void						OnDestroy();

	virtual BOOL				CheckSubType( UINT8 ui8SubType );
	virtual BOOL				SetRoomOption( UINT32	ui32StageID );

	virtual
#ifndef NEW_EXPPOINT
			void
#else
			UINT16
#endif
								UserKillBouns( INT32 i32SlotIdx, UINT32 ui32DeathType, UINT32 ui32DeathMyState, UINT32 ui32DeathEnemyState, UINT32 ui32RTBattle );	// 유저가 죽었을 경우 보너스

	virtual BOOL				OnUpdatePreBattle( UINT32 ui32StateTime, UINT32 ui32ServerTime );	

	virtual void				SetMatchStart();
	virtual void				SetRoundStart();
	virtual bool				RoundStart( UINT32 ui32RTBattle );
	
	virtual BOOL				CheckRoundEnd( UINT32 ui32ServerTime );
	virtual BOOL				CheckMatchEnd();

	virtual INT32				GetWinTeam();
#ifndef NEW_EXPPOINT
	virtual void				GetBasicExpPoint( INT32 i32Win, TYPE_ROOM_GET_EXPPOINT* pstGetExpPoint );							// 정상 종료시 얻어가는 Exp, Point 수치

	virtual REAL32				GetSubPercentOfUserCountTeam( INT32 i32Team, INT32 i32AllCount, INT32 i32Count );	
	virtual REAL32				GetSubPercentOfUnusualEnd( INT32 i32Win );

	virtual UINT16				GetMatchAccExp( INT32 i32SlotIdx, UINT16 ui16AccExp );
	virtual UINT16				GetMatchAccPoint( INT32 i32SlotIdx, UINT16 ui16AccPoint );

	virtual	UINT16				GetMatchEndPoint( INT32 i32Win, INT32 i32SlotIdx, UINT16 ui16AccPoint, REAL32 r32AdjustPercent, BOOL bNormalEnd, UINT32 ui32RTBattle );
#else
	virtual INT16				GetMissionExp( INT32 i32SlotIdx );
#endif

	virtual	UINT16				GetMatchEndExp( INT32 i32Win, INT32 i32SlotIdx, UINT16 ui16AccExp, REAL32 r32AdjustPercent, BOOL bNormalEnd, UINT32 ui32RTBattle );

	virtual bool				ExistRecord( INT32 i32SlotIdx )						{ return !((m_pInfo->_aui8AccMissionCount[i32SlotIdx] == 0) && (m_pInfo->_aM_UserScore[i32SlotIdx].m_KillCount == 0) && (m_pInfo->_aM_UserScore[i32SlotIdx].m_DeathCount == 0) && (m_aui16MatAssist[i32SlotIdx] == 0)); }

	virtual void				AddStartBattlePacket( i3NetworkPacket* pPacket, CUserSession * pSession, UINT32 ui32RTBattle );
	virtual void				AddGiveUpBattlePacket( i3NetworkPacket* pPacket, CUserSession * pSession );
	virtual void				AddRespawnPacket( i3NetworkPacket* pPacket, CUserSession * pSession );
	virtual void				AddDeathBattlePacket( i3NetworkPacket* pPacket );

	virtual void				periodicUpdateRoomState( UINT32 ui32ServerTime );							// 절대 메인스레드에서만 부를것

	virtual void				UpdateUserState( INT32 i32Idx );
	virtual bool				CheckTrex( void );

	// Mission Message
	virtual BOOL				MissionTouchDown( CUserSession * pSession, INT32 i32SlotIdx, UINT32 ui32RTBattle );
	virtual BOOL				GetTouchDownChainCount( INT32 i32SlotIdx )									{	return m_ui8A_TouchDownChainCount[i32SlotIdx];		}

	virtual BOOL				CheckVoteTime( UINT32 ui32ServerTime );

#ifdef USE_GM_PAUSE
	void						TouchdownBlockTimeReset( UINT32 ui32RTBattle )		
	{
		m_ui32BlockBonusTime	= ui32RTBattle + TOUCHDOWN_BLOCK_BONUS_TIME;
		m_ui8LastTouchDownState	= TOUCHDOWN_BLOCK_STATE_INIT;
	}
#else
	void						TouchdownBlockTimeReset( UINT32 ui32ServerTime )
	{
		m_ui32BlockBonusTime	= ui32ServerTime + TOUCHDOWN_BLOCK_BONUS_TIME;
		m_ui8LastTouchDownState = TOUCHDOWN_BLOCK_STATE_INIT;
	}
#endif
};

#endif