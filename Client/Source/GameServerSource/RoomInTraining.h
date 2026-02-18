#ifndef __ROOM_IN_TRAINING_H__
#define __ROOM_IN_TRAINING_H__

#include "RoomBase.h"

class CRoomInTraining : public CRoomBase
{
	I3_CLASS_DEFINE( CRoomInTraining );

protected:

public:
	CRoomInTraining();
	virtual ~CRoomInTraining();

	virtual BOOL			OnCreate( TYPE_ROOM_INFO* pRoomInfo );
	void					OnDestroy();

	virtual BOOL			SetRoomOption( UINT32 ui32StageID );
	
	virtual BOOL			CheckRoundEnd( UINT32 ui32ServerTime );
	virtual BOOL			CheckMatchEnd();

	virtual INT32			GetWinTeam();
#ifndef NEW_EXPPOINT
	virtual void			GetBasicExpPoint( INT32 i32Win, TYPE_ROOM_GET_EXPPOINT* pstGetExpPoint );

	virtual REAL32			GetSubPercentOfUnusualEnd( INT32 i32Win );
#endif

	virtual	UINT16			GetMatchEndExp( INT32 i32Win, INT32 i32SlotIdx, UINT16 ui16AccExp, REAL32 r32AdjustPercent, BOOL bNormalEnd, UINT32 ui32RTBattle );

	virtual BOOL			CheckVoteTime( UINT32 ui32ServerTime );
};

#endif