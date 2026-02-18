#ifndef __ROOM_IN_DEATHMATCH_H__
#define __ROOM_IN_DEATHMATCH_H__

#include "RoomBase.h"

class CRoomInDeathmatch : public CRoomBase
{
	I3_CLASS_DEFINE( CRoomInDeathmatch );

protected:
	TYPE_ROOM_GET_EXPPOINT_VALUES		m_stExpPointValues4VS4;

public:
	CRoomInDeathmatch();
	virtual ~CRoomInDeathmatch();

	virtual BOOL			OnCreate( TYPE_ROOM_INFO* pRoomInfo );
	void					OnDestroy();

	virtual BOOL			SetRoomOption( UINT32	ui32StageID );

	virtual BOOL			CheckRoundEnd( UINT32 ui32RTServer );
	virtual BOOL			CheckMatchEnd();

	virtual INT32			GetWinTeam();
	virtual void			GetBasicExpPoint( INT32 i32Win, TYPE_ROOM_GET_EXPPOINT_V1* pstGetExpPoint );

	virtual REAL32			GetSubPercentOfUnusualEnd( INT32 i32Win );

	virtual BOOL			CheckVoteTime( UINT32 ui32ServerTime );

	virtual void			AddStartBattlePacket( PACKET_BATTLE_STARTBATTLE_ACK* pPacket, CUserSession * pSession, UINT32 ui32RTBattle );
	virtual void			GetRoomNowScore( ROOM_NOW_SCORE* pRoomNowScore, UINT32 ui32RTServer );
};

#endif