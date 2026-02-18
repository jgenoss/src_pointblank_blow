#include "pch.h"
#include "RoomInSpace.h"

I3_CLASS_INSTANCE(CRoomInSpace, CRoomInDeathmatch);

CRoomInSpace::CRoomInSpace()
{
}

CRoomInSpace::~CRoomInSpace()
{
}

BOOL CRoomInSpace::OnUpdatePreBattle(UINT32 ui32RTState, UINT32 ui32RTServer)
{
	if (0 == m_pInfo->_InfoAdd._NowRoundCount)
	{	// 1라운드는 탈출모드와 동일하게 HUD 팝업 대기 시간.
		if ((ui32RTState + MISSION_PREBATTLE_TIME) > ui32RTServer)
		{
			return FALSE;
		}
	}
	else
	{	// 2라운드 부터는 폭파모드와 동일하게 자동 공수교대 시간 체크
		return CRoomBase::OnUpdatePreBattle(ui32RTState, ui32RTServer);
	}
	return TRUE;
}
