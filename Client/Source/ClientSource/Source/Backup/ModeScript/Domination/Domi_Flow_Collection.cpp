#include "pch.h"
#include "Domi_Flow_Collection.h"

#include "./GameMode_Domination.h"
#include "../../Source/ui/hud/UIHUD_Domination_Manager.h"

CDomi_Flow_Collection::CDomi_Flow_Collection( gmode_domination* pmode) : CDominationFlowBase( pmode)
{
}

CDomi_Flow_Collection::~CDomi_Flow_Collection(void)
{
}

void CDomi_Flow_Collection::ProcessModeData( INT32 nIndex, INT32 nData)
{
	//switch( nIndex)
	//{
	//case 110:
	//	UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_MAX_COUNT, nData );
	//	break;

	//case 111:
	//	UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_CLEAR_COUNT, nData );
	//	break;

	//case 114:	//모든 오브젝트 회수 후 도착지 표시.
	//	{
	//		i3::vector<i3RespawnObj*>& respawnObjectList = m_pmode->GetRespawnObjectList();

	//		INT32 size = static_cast<INT32>(respawnObjectList.size());
	//		I3_BOUNDCHK( nData, size);
	//		if( 0 <= nData && nData < size)
	//		{
	//			i3RespawnObj* pRespawnObj = respawnObjectList[nData];
	//			CUIHudDomi_Indicator* pHudIndicate = static_cast<CUIHudDomi_Indicator*>(UIHUDDominationMgr::i()->GetHUD( HUD_DMN_INDICATOR));

	//			if( pHudIndicate->InsertIndication( static_cast<i3GameObj*>(pRespawnObj), WAVE_MISSION_RECOVERY2, UI_Util::CIndicateObject::RESPAWN_OBJECT) == false)
	//				I3TRACE("  -- Can not Insert Indicate Respawn object(%s)\n", pRespawnObj->GetName());
	//		}
	//	}
	//	break;
	//}
}