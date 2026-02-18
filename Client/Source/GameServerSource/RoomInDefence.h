#ifndef __ROOM_IN_DEFENCE_H__
#define __ROOM_IN_DEFENCE_H__

#include "RoomBase.h"

class CRoomInDefence : public CRoomBase
{
	I3_CLASS_DEFINE( CRoomInDefence );

#ifdef NEW_EXPPOINT
	UINT8						m_ui8BonusDamageMax;
	UINT8						m_ui8rBonusDamageScore;
	UINT8						m_ui8BonusDefenceMax;
	UINT8						m_ui8BonusDefenceScore;
	UINT8						m_ui8BonusHPMax;
	UINT8						m_ui8rBonusHPScore;
#endif

	DEFENCE_INFO				m_MiDefenceInfo;						// Defence 미션 오브젝트
	UINT16						m_pui16MiObjectMaxHp[ MAX_OBJECT_COUNT ]; 

	REAL32						m_pr32SubUserCountTwo[SLOT_MAX_COUNT];
	REAL32						m_pr32SubUserCountTwoVS4[SLOT4VS4_MAX_COUNT];

	UINT8						m_ui8AccMissionDefence[SLOT_MAX_COUNT];	// 오브젝트 공격중인 적 킬 횟수
	UINT32						m_ui32AccMissionUserDamage[MAX_OBJECT_COUNT][SLOT_MAX_COUNT];		// 오브젝트에 입힌 데미지 누적
#ifndef NEW_EXPPOINT
	UINT32						m_ui32AccMissionDefenceScore[MAX_OBJECT_COUNT][SLOT_MAX_COUNT];	// 오브젝트 데미지에 대한 보너스 누적
#else
	UINT16						m_ui16AccMissionDefenceRemain[MAX_OBJECT_COUNT];				// 남은 오브젝트 HP
#endif

protected:

public:
	CRoomInDefence();
	virtual ~CRoomInDefence();

	virtual BOOL				OnCreate( TYPE_ROOM_INFO* pRoomInfo );
	void						OnDestroy();

	virtual BOOL				CheckSubType( UINT8 ui8SubType );
	virtual BOOL				SetRoomOption( UINT32	ui32StageID );
	virtual BOOL				UserGiveUpBattle( INT32 i32SlotIdx );
	
	virtual
#ifndef NEW_EXPPOINT
			void
#else
			UINT16
#endif
								UserKillBouns( INT32 i32SlotIdx, UINT32 ui32DeathType, UINT32 ui32DeathMyState, UINT32 ui32DeathEnemyState, UINT32 ui32RTBattle );	// 유저가 죽었을 경우 보너스

	virtual void				SetMatchStart();
	virtual void				SetRoundStart();

	virtual BOOL				CheckRoundEnd( UINT32 ui32ServerTime );
	virtual BOOL				CheckMatchEnd();

	virtual INT32				GetWinTeam();

#ifndef NEW_EXPPOINT
	virtual REAL32				GetSubPercentOfUserCount2( INT32 i32Count );	
	virtual REAL32				GetSubPercentOfUnusualEnd( INT32 i32Win );

	// Calcurate Exp
	virtual UINT16				GetMatchAccExp( INT32 i32SlotIdx, UINT16 ui16AccExp );

	virtual	UINT16				GetMatchEndExp( INT32 i32Win, INT32 i32SlotIdx, UINT16 ui16AccExp, REAL32 r32AdjustPercent, BOOL bNormalEnd, UINT32 ui32RTBattle );
#else
	virtual INT16				GetMissionExp( INT32 i32SlotIdx );	
#endif

	// 패킷에 모드별 정보 추가시 사용
	virtual void				AddStartBattlePacket( i3NetworkPacket* pPacket, CUserSession * pSession, UINT32 ui32RTBattle );

	// Mission Message
	virtual BOOL				MissionObjectDefence( DEFENCE_INFO * pInfo );
};

#endif