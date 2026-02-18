#ifndef __ROOM_IN_CHALLENGE_H__
#define __ROOM_IN_CHALLENGE_H__

#include "RoomBase.h"

class CRoomInChallenge : public CRoomBase
{
	I3_CLASS_DEFINE( CRoomInChallenge );

protected:

public:
	CRoomInChallenge();
	virtual ~CRoomInChallenge();

	virtual BOOL			OnCreate( TYPE_ROOM_INFO* pRoomInfo );
	void					OnDestroy();

	virtual BOOL			SetRoomOption( UINT32 ui32StageID );
	
	virtual BOOL			CheckRoundEnd( UINT32 ui32RTServer );
	virtual BOOL			CheckMatchEnd();

	virtual INT32			GetWinTeam();

	virtual void			GetBasicExpPoint( INT32 i32Win, TYPE_ROOM_GET_EXPPOINT_V1* pstGetExpPoint );

	virtual REAL32			GetSubPercentOfUnusualEnd( INT32 i32Win );

	virtual	UINT16			GetMatchEndExp(INT32 i32Win, INT32 i32SlotIdx, UINT16 ui16AccExp, REAL32 r32AdjustPercent, REAL32 r32AsymPercent, BOOL bNormalEnd, UINT32 ui32RTBattle);

	virtual BOOL			CheckVoteTime( UINT32 ui32ServerTime );

	virtual void			AddStartBattlePacket( PACKET_BATTLE_STARTBATTLE_ACK* pPacket, CUserSession * pSession, UINT32 ui32RTBattle );
};

#endif