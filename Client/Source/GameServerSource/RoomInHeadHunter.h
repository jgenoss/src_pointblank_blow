#ifndef __ROOM_IN_HEADHUNTER_H__
#define __ROOM_IN_HEADHUNTER_H__

#include "RoomBase.h"

class CRoomInHeadHunter : public CRoomBase
{
	I3_CLASS_DEFINE( CRoomInHeadHunter );

protected:

public:
	CRoomInHeadHunter();
	virtual ~CRoomInHeadHunter();

	virtual BOOL			OnCreate( TYPE_ROOM_INFO* pRoomInfo );
	void					OnDestroy();

	virtual BOOL			SetRoomOption( UINT32	ui32StageID );
	virtual BOOL			CheckReady();
	
	virtual BOOL			CheckRoundEnd( UINT32 ui32ServerTime );
	virtual BOOL			CheckMatchEnd();

	virtual INT32			GetWinTeam();

#ifndef NEW_EXPPOINT
	virtual REAL32			GetSubPercentOfUnusualEnd( INT32 i32Win );
#endif

	virtual BOOL			CheckVoteTime( UINT32 ui32ServerTime );

};

#endif