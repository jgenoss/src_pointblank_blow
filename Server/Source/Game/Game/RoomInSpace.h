#pragma once

#include "RoomInDeathmatch.h"

class CRoomInSpace : public CRoomInDeathmatch
{
	I3_CLASS_DEFINE( CRoomInSpace );

public:
	CRoomInSpace();
	virtual ~CRoomInSpace();

	virtual BOOL			OnUpdatePreBattle(UINT32 ui32RTState, UINT32 ui32RTServer);
};