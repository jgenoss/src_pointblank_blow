#include "pch.h"
#include "TabMinimap_RoomInfo.h"
#include "MyRoomInfoContext.h"

INT32 TabMinimap_RoomInfo::_GetLimitKillCount(void)
{
	INT32 idxKillCount = LONIBBLE( MyRoomInfoContext::i()->GetMyRoomSubType());

	UINT32 StageID = MyRoomInfoContext::i()->GetMyRoomStageID();

	switch( StageID::GetStageMode( StageID) )
	{
	case STAGE_MODE_ESCAPE : 
	case STAGE_MODE_ANNIHILATION	: 
	case STAGE_MODE_BOMB	:
	case STAGE_MODE_DESTROY :
	case STAGE_MODE_DEFENCE	:
	case STAGE_MODE_CONVOY:
	case STAGE_MODE_RUN_AWAY:
		return 0;
	}

	return g_TeamKillCountTable[ idxKillCount];
}

// Round 제의 Stage인 경우, 전체 Round 회수를 반환한다.
INT32 TabMinimap_RoomInfo::_GetTotalRoundCount(void)
{
	UINT32 StageID = MyRoomInfoContext::i()->GetMyRoomStageID();
	switch( StageID::GetStageMode( StageID) )
	{
	case STAGE_MODE_ESCAPE : 
	case STAGE_MODE_ANNIHILATION : 
	case STAGE_MODE_BOMB :
	case STAGE_MODE_DESTROY :
	case STAGE_MODE_DEFENCE	:
	case STAGE_MODE_CONVOY :
	case STAGE_MODE_RUN_AWAY :
		{
			INT32 round = LONIBBLE( MyRoomInfoContext::i()->GetMyRoomSubType() );
			round = MINMAX( BATTLE_ROUND_TYPE_1, round, BATTLE_ROUND_TYPE_9);
			return g_MissionRoundCountTable[round];
		}
		break;
	}
	return 0;
}