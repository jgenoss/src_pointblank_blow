#ifndef __ROOM_IN_RUN_AWAY_H__
#define __ROOM_IN_RUN_AWAY_H__

#include "RoomBase.h"

class CRoomInRunAway : public CRoomBase
{
	I3_CLASS_DEFINE( CRoomInRunAway );

	UINT8					m_ui8BonusTouchDownMax;				// 터치다운 수행 최대 회수
	UINT8					m_ui8BonusTouchDownExp;				// 터치다운 수행 점수

	UINT8					m_ui8AccTouchDownCount[SLOT_MAX_COUNT];	// 터치다운 설치 횟수

protected:

public:
	CRoomInRunAway();
	virtual ~CRoomInRunAway();

	virtual BOOL			OnCreate( TYPE_ROOM_INFO* pRoomInfo );
	void					OnDestroy();

	virtual BOOL			SetRoomOption( UINT32	ui32StageID );

	virtual BOOL			OnUpdatePreBattle( UINT32 ui32RTState, UINT32 ui32RTServer );
	
	virtual void			SetMatchStart();
	virtual void			SetRoundStart();	

	virtual BOOL			CheckRoundEnd( UINT32 ui32RTServer );
	virtual BOOL			CheckMatchEnd();

	virtual INT32			GetWinTeam();

	virtual void			GetBasicExpPoint( INT32 i32Win, TYPE_ROOM_GET_EXPPOINT_V1* pstGetExpPoint );

	virtual REAL32			GetSubPercentOfUnusualEnd( INT32 i32Win );

	virtual	UINT16			GetMatchEndExp(INT32 i32Win, INT32 i32SlotIdx, UINT16 ui16AccExp, REAL32 r32AdjustPercent, REAL32 r32AsymPercent, BOOL bNormalEnd, UINT32 ui32RTBattle);


	//virtual INT16			GetMissionExp( INT32 i32SlotIdx );

	virtual UINT16			GetMatchAccExp(INT32 i32SlotIdx, UINT16 ui16AccExp);
	virtual UINT16			GetMatchAccPoint(INT32 i32SlotIdx, UINT16 ui16AccPoint);

	virtual void			AddStartBattlePacket( PACKET_BATTLE_STARTBATTLE_ACK* pPacket, CUserSession * pSession, UINT32 ui32RTBattle );

	virtual void			GetRoomNowScore( ROOM_NOW_SCORE* pRoomNowScore, UINT32 ui32RTServer );

	virtual BOOL			MissionTouchDown( CUserSession * pSession, INT32 i32SlotIdx, UINT32 ui32RTBattle );

	virtual void			CheckDefaultWeapon(ITEM_INFO* aWeaponList, INT32& i32CheckSkipFlag, INT32& i32WeaponUseStop, INT32 i32SlotIdx  );
};

#endif