#ifndef __ROOM_IN_SUDDENDEATH_H__
#define __ROOM_IN_SUDDENDEATH_H__

#include "RoomBase.h"

class CRoomInSuddenDeath : public CRoomBase
{
	I3_CLASS_DEFINE( CRoomInSuddenDeath );

protected:

public:
	CRoomInSuddenDeath();
	virtual ~CRoomInSuddenDeath();

	virtual BOOL			OnCreate( TYPE_ROOM_INFO* pRoomInfo );
	void					OnDestroy();

	virtual BOOL			SetRoomOption( UINT32	ui32StageID );

	virtual BOOL			CheckRoundEnd( UINT32 ui32ServerTime );
	virtual BOOL			CheckMatchEnd();

	virtual INT32			GetWinTeam();

#ifndef NEW_EXPPOINT
	virtual REAL32			GetSubPercentOfUnusualEnd( INT32 i32Win );
#endif

	virtual void			AddStartBattlePacket( i3NetworkPacket* pPacket, CUserSession * pSession, UINT32 ui32RTBattle );
};

#endif