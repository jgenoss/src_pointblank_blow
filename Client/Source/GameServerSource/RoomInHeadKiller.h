#ifndef __ROOM_IN_HEADKILLER_H__
#define __ROOM_IN_HEADKILLER_H__

#include "RoomBase.h"

class CRoomInHeadKiller : public CRoomBase
{
	I3_CLASS_DEFINE( CRoomInHeadKiller );

#ifndef NEW_EXPPOINT
	UINT8						m_ui8BonusKillTargetCount;			// 킬 목표 달성 킬 수
	UINT8						m_ui8BonusKillTargetCount4VS4;		// 킬 목표 달성 킬 수 (4vS4)
	UINT8						m_ui8BonusKillExp;					// 킬 목표 달성 보너스 경험치
	UINT8						m_ui8BonusKillExp4VS4;				// 킬 목표 달성 보너스 경험치
#endif
	UINT8						m_ui8MissionKill[SLOT_MAX_COUNT];	// 킬 목표 달성 횟수

protected:

public:
	CRoomInHeadKiller();
	virtual ~CRoomInHeadKiller();

	virtual BOOL				OnCreate( TYPE_ROOM_INFO* pRoomInfo );
	void						OnDestroy();

	virtual BOOL				SetRoomOption( UINT32	ui32StageID );	

	virtual void				SetMatchStart();

#ifndef NEW_EXPPOINT
	virtual void				UserKillBouns( INT32 i32SlotIdx, UINT32 ui32DeathType, UINT32 ui32DeathMyState, UINT32 ui32DeathEnemyState, UINT32 ui32RTBattle );
#endif

	virtual BOOL				CheckReady();
	virtual BOOL				CheckRoundEnd( UINT32 ui32ServerTime );
	virtual BOOL				CheckMatchEnd();

	virtual INT32				GetWinTeam();

#ifndef NEW_EXPPOINT
	virtual REAL32				GetSubPercentOfUnusualEnd( INT32 i32Win );

	virtual UINT16				GetMatchAccExp( INT32 i32SlotIdx, UINT16 ui16AccExp );
#endif

	virtual void				AddStartBattlePacket( i3NetworkPacket* pPacket, CUserSession * pSession, UINT32 ui32RTBattle );
};

#endif