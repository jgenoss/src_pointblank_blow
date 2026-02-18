#ifndef __ROOM_IN_CROSSCOUNT_H__
#define __ROOM_IN_CROSSCOUNT_H__

#include "RoomBase.h"

class CRoomInCrossCount : public CRoomBase
{
	I3_CLASS_DEFINE( CRoomInCrossCount );

	UINT8						m_ui8ScoreKill;										// 킬 점수

	UINT16						m_pi16MatAssist[ SLOT_MAX_COUNT ];
	
	UINT8						m_ui8DinoTeam;

protected:
	void						_InitUserState();									// 라운드 시작 전 유저 상태 설정

public:
	CRoomInCrossCount();
	virtual ~CRoomInCrossCount();

	virtual BOOL				OnCreate( TYPE_ROOM_INFO* pRoomInfo );
	void						OnDestroy();

	virtual BOOL				SetRoomOption( UINT32	ui32StageID );

	virtual void				UserKillBounsV1( INT32 i32SlotIdx, UINT32 ui32DeathType, UINT32 ui32DeathMyState, UINT32 ui32DeathEnemyState, UINT32 ui32RTBattle );	// 유저가 죽었을 경우 보너스

	virtual UINT16				UserKillBounsV2( INT32 i32SlotIdx, UINT32 ui32DeathType, UINT32 ui32DeathMyState, UINT32 ui32DeathEnemyState, UINT32 ui32RTBattle );	// 유저가 죽었을 경우 보너스


	virtual BOOL				OnUpdatePreBattle( UINT32 ui32RTState, UINT32 ui32RTServer );

	virtual void				SetMatchStart();
	virtual void				SetRoundStart();
	
	virtual BOOL				CheckRoundEnd( UINT32 ui32RTServer );
	virtual BOOL				CheckMatchEnd();

	virtual INT32				GetWinTeam();

	virtual void				GetBasicExpPoint( INT32 i32Win, TYPE_ROOM_GET_EXPPOINT_V1* pstGetExpPoint );							// 정상 종료시 얻어가는 Exp, Point 수치

	virtual REAL32				GetSubPercentOfUserCountTeam( INT32 i32Team, INT32 i32AllCount, INT32 i32Count );	
	virtual REAL32				GetSubPercentOfUnusualEnd( INT32 i32Win );


	virtual bool				ExistRecord( INT32 i32SlotIdx )						{ return !((m_pInfo->_aui8AccMissionCount[i32SlotIdx] == 0) && (m_pInfo->_aM_UserScore[i32SlotIdx].m_KillCount == 0) && (m_pInfo->_aM_UserScore[i32SlotIdx].m_DeathCount == 0) && (m_pi16MatAssist[i32SlotIdx] == 0)); }

	virtual void				AddStartBattlePacket( PACKET_BATTLE_STARTBATTLE_ACK* pPacket, CUserSession * pSession, UINT32 ui32RTBattle );
	virtual void				AddRespawnPacket( i3NetworkPacket* pPacket, CUserSession * pSession );

	virtual void				UpdateUserState( INT32 i32Idx );
	virtual bool				CheckTrex( void );

	virtual BOOL				CheckVoteTime( UINT32 ui32ServerTime );
	virtual void				GetRoomNowScore( ROOM_NOW_SCORE* pRoomNowScore, UINT32 ui32RTServer );

	virtual void				CommandIncreaseKillCount( INT32 i32SlotIdx, INT32 i32KillCount );

	virtual bool				IsDinoUser(INT32 i32SlotIdx)
	{
		INT32 i32AtkDefChange = (ATK_DEF_CHANGED == GetAtkDefChange()) ? 1 : 0;

		if ((i32SlotIdx % 2) == i32AtkDefChange)	return true;

		return false;
	}
};

#endif