#ifndef __ROOM_IN_CROSSCOUNT_H__
#define __ROOM_IN_CROSSCOUNT_H__

#include "RoomBase.h"

#define TOUCHDOWN_BLOCK_BONUS_TIME	30
class CRoomInCrossCount : public CRoomBase
{
	I3_CLASS_DEFINE( CRoomInCrossCount );

	UINT8						m_ui8ScoreKill;										// 킬 점수

	UINT16						m_pi16MatAssist[ SLOT_MAX_COUNT ];

protected:
	void						_InitUserState();									// 라운드 시작 전 유저 상태 설정

public:
	CRoomInCrossCount();
	virtual ~CRoomInCrossCount();

	virtual BOOL				OnCreate( TYPE_ROOM_INFO* pRoomInfo );
	void						OnDestroy();

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
	
	virtual BOOL				CheckRoundEnd( UINT32 ui32ServerTime );
	virtual BOOL				CheckMatchEnd();

	virtual INT32				GetWinTeam();
#ifndef NEW_EXPPOINT
	virtual void				GetBasicExpPoint( INT32 i32Win, TYPE_ROOM_GET_EXPPOINT* pstGetExpPoint );							// 정상 종료시 얻어가는 Exp, Point 수치

	virtual REAL32				GetSubPercentOfUserCountTeam( INT32 i32Team, INT32 i32AllCount, INT32 i32Count );	
	virtual REAL32				GetSubPercentOfUnusualEnd( INT32 i32Win );
#endif

	virtual bool				ExistRecord( INT32 i32SlotIdx )						{ return !((m_pInfo->_aui8AccMissionCount[i32SlotIdx] == 0) && (m_pInfo->_aM_UserScore[i32SlotIdx].m_KillCount == 0) && (m_pInfo->_aM_UserScore[i32SlotIdx].m_DeathCount == 0) && (m_pi16MatAssist[i32SlotIdx] == 0)); }

	virtual void				AddStartBattlePacket( i3NetworkPacket* pPacket, CUserSession * pSession, UINT32 ui32RTBattle );
	virtual void				AddRespawnPacket( i3NetworkPacket* pPacket, CUserSession * pSession );
	virtual void				AddDeathBattlePacket( i3NetworkPacket* pPacket );

	virtual void				UpdateUserState( INT32 i32Idx );
	virtual bool				CheckTrex( void );

	virtual BOOL				CheckVoteTime( UINT32 ui32ServerTime );
};

#endif