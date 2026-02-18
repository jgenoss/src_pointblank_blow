#include "pch.h"
#include "Domi_Flow_Boss.h"

#include "../../Source/ui/hud/UIHUD_Domination_Manager.h"

CDomi_Flow_Boss::CDomi_Flow_Boss( gmode_domination* pmode) : CDominationFlowBase( pmode)
{
}

CDomi_Flow_Boss::~CDomi_Flow_Boss(void)
{
}

void CDomi_Flow_Boss::ProcessModeData( INT32 nIndex, INT32 nData)
{
	//switch( nIndex)
	//{
	//case 110:
	//	UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_MAX_COUNT, nData );
	//	break;

	//case 111:
	//	UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_CLEAR_COUNT, nData );
	//	break;
	//}
}