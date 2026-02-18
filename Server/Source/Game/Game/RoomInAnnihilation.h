#ifndef __ROOM_IN_ANNIHILATION_H__
#define __ROOM_IN_ANNIHILATION_H__

#include "RoomBase.h"

class CRoomInAnnihilation : public CRoomBase
{
	I3_CLASS_DEFINE( CRoomInAnnihilation );

	UINT8						m_ui8BonusKillTargetCount;				// 킬 목표 달성 킬 수
	UINT8						m_ui8BonusKillTargetCount4VS4;			// 킬 목표 달성 킬 수 (4vS4)
	UINT8						m_ui8BonusKillExp;						// 킬 목표 달성 보너스 경험치
	UINT8						m_ui8BonusKillExp4VS4;					// 킬 목표 달성 보너스 경험치

	UINT8						m_ui8AccMissionKill[SLOT_MAX_COUNT];	// 킬 목표 달성 횟수

protected:

public:
	CRoomInAnnihilation();
	virtual ~CRoomInAnnihilation();

	virtual BOOL				OnCreate( TYPE_ROOM_INFO* pRoomInfo );
	void						OnDestroy();

	virtual BOOL				SetRoomOption( UINT32 ui32StageID );

	virtual void				SetMatchStart();

	virtual void				UserKillBounsV1( INT32 i32SlotIdx, UINT32 ui32DeathType, UINT32 ui32DeathMyState, UINT32 ui32DeathEnemyState, UINT32 ui32RTBattle );

	virtual BOOL				CheckRoundEnd( UINT32 ui32RTServer );
	virtual BOOL				CheckMatchEnd();

	virtual INT32				GetWinTeam();

	virtual REAL32				GetSubPercentOfUnusualEnd( INT32 i32Win );

	virtual UINT16				GetMatchAccExp( INT32 i32SlotIdx, UINT16 ui16AccExp );

	virtual void				AddStartBattlePacket( PACKET_BATTLE_STARTBATTLE_ACK* pPacket, CUserSession * pSession, UINT32 ui32RTBattle );
	virtual void				GetRoomNowScore( ROOM_NOW_SCORE* pRoomNowScore, UINT32 ui32RTServer );

	virtual STAGE_ID			GetRandMap( STAGE_ID eStageID);
};

#endif