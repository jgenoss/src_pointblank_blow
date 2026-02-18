#ifndef __ROOM_IN_DEATHMATCH_H__
#define __ROOM_IN_DEATHMATCH_H__

#include "RoomBase.h"

class CRoomInDeathmatch : public CRoomBase
{
	I3_CLASS_DEFINE( CRoomInDeathmatch );

protected:
#ifdef NEW_EXPPOINT
	TYPE_ROOM_GET_EXPPOINT_VALUES		m_stExpPointValues4VS4;
#endif

public:
	CRoomInDeathmatch();
	virtual ~CRoomInDeathmatch();

	virtual BOOL			OnCreate( TYPE_ROOM_INFO* pRoomInfo );
	void					OnDestroy();

	virtual BOOL			SetRoomOption( UINT32	ui32StageID );

#ifdef NEW_EXPPOINT
	virtual bool			CheckMatchStart();
#endif
	virtual BOOL			CheckRoundEnd( UINT32 ui32ServerTime );
	virtual BOOL			CheckMatchEnd();

	virtual INT32			GetWinTeam();
#ifndef NEW_EXPPOINT
	virtual void			GetBasicExpPoint( INT32 i32Win, TYPE_ROOM_GET_EXPPOINT* pstGetExpPoint );

	virtual REAL32			GetSubPercentOfUnusualEnd( INT32 i32Win );
#endif

	virtual BOOL				CheckVoteTime( UINT32 ui32ServerTime );
};

#endif