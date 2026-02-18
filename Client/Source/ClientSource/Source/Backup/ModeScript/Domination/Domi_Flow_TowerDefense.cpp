#include "pch.h"
#include "Domi_Flow_TowerDefense.h"

#include "./GameMode_Domination.h"
#include "../../Source/ui/hud/UIHUD_Domination_Manager.h"

CDomi_Flow_TowerDefense::CDomi_Flow_TowerDefense( gmode_domination* pmode) : CDominationFlowBase( pmode)
{
}

CDomi_Flow_TowerDefense::~CDomi_Flow_TowerDefense(void)
{
}

void CDomi_Flow_TowerDefense::ProcessModeData( INT32 nIndex, INT32 nData)
{
	//switch( nIndex)
	//{
	//case 110:
	//	{
	//		m_vecTriggerAI.clear();
	//		m_nCurrentTriggerAI = -1;

	//		m_iLimit = (UINT16)nData;
	//		char szRemain[WAVE_MISSION_LIMIT_TEXT_BUFFER] = {0,};	

	//		sprintf_s( szRemain, WAVE_MISSION_LIMIT_TEXT_BUFFER, "%d", m_iLimit);

	//		UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_REMAIN, reinterpret_cast< INT32 >(szRemain));
	//	}
	//	break;

	//case  111:
	//	{
	//		UINT16 iRemain = m_iLimit - (UINT16)nData;
	//		if( iRemain <= 0) iRemain = 0;

	//		char szRemain[WAVE_MISSION_LIMIT_TEXT_BUFFER] = {0,};
	//		sprintf_s( szRemain, WAVE_MISSION_LIMIT_TEXT_BUFFER, "%d", iRemain);

	//		if( iRemain <= 10)
	//		{
	//			ZeroMemory( szRemain, sizeof(szRemain));

	//			REAL32 fRatio = uiMath::interpolate( (REAL32)iRemain, 0.f, 10.f);
	//			sprintf_s( szRemain, WAVE_MISSION_LIMIT_TEXT_BUFFER, "{col:255, %d, %d, 255}%d{/col}", (UINT8)(255*fRatio), (UINT8)(255*fRatio), iRemain);
	//		}

	//		UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_REMAIN, reinterpret_cast< INT32 >(szRemain));
	//	}
	//	break;

	//case 112:	//AI 리스폰 오브젝트 인덱스
	//	{
	//		i3::vector<i3RespawnObj*>& respawnObjectList = m_pmode->GetRespawnObjectList();

	//		INT32 size = static_cast<INT32>(respawnObjectList.size());
	//		I3_BOUNDCHK( nData, size);
	//		if( 0 <= nData && nData < size)
	//		{
	//			m_pAiRespawnObj = respawnObjectList[nData];
	//		}
	//	}
	//	break;

	//case 113:	//트리거 건드린 AI idx
	//	{
	//		m_vecTriggerAI.push_back( nData);
	//	}
	//	break;

	//case 114:	//AI 도착 지점 리스폰 오브젝트 인덱스
	//	{
	//		i3::vector<i3RespawnObj*>& respawnObjectList = m_pmode->GetRespawnObjectList();

	//		INT32 size = static_cast<INT32>(respawnObjectList.size());
	//		I3_BOUNDCHK( nData, size);
	//		if( 0 <= nData && nData < size)
	//		{
	//			m_pAiDestObj = respawnObjectList[nData];

	//			//Indicate 추가.
	//			CUIHudDomi_Indicator* pHudIndicate = static_cast<CUIHudDomi_Indicator*>(UIHUDDominationMgr::i()->GetHUD( HUD_DMN_INDICATOR));
	//			if( pHudIndicate->InsertIndication( static_cast<i3GameObj*>(m_pAiDestObj), WAVE_MISSION_BLOCK, UI_Util::CIndicateObject::RESPAWN_OBJECT) == false)
	//				I3TRACE("  -- Can not Insert Indicate Respawn object(%s)\n", m_pAiDestObj->GetName());
	//		}
	//	}
	//	break;

	//case 115:	//도착한 AI Idx
	//	{
	//		std::vector<INT32>::iterator itr = m_vecTriggerAI.begin();
	//		while( itr != m_vecTriggerAI.end())
	//		{
	//			if( *itr == nData) 
	//			{
	//				m_vecTriggerAI.erase( itr);

	//				UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_INDICATOR, HUD_DMN_EVT_DELETE_CHARACTER_OBJECT, *itr);

	//				break;
	//			}
	//		}
	//	}
	//	break;
	//}
}

/*virtual*/ void CDomi_Flow_TowerDefense::ProcessUpdate( REAL32 rDeltaTime)
{
	std::vector<INT32>::iterator itr = m_vecTriggerAI.begin();

	while( itr != m_vecTriggerAI.end())
	{
		CGameCharaBase* pAi = g_pCharaManager->getCharaByNetIdx( *itr, CHARA_INFO_AI);

		if( pAi == NULL || pAi->isCharaStateMask( CHARA_STATE_DEATH))
		{
			UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_INDICATOR, HUD_DMN_EVT_DELETE_CHARACTER_OBJECT, *itr);
			
			itr = m_vecTriggerAI.erase( itr);
			continue;
		}
		++itr;
	}

	itr = m_vecTriggerAI.begin();
	if( !m_vecTriggerAI.empty() && m_nCurrentTriggerAI != *itr)
	{
		CGameCharaBase* pNewAi = g_pCharaManager->getCharaByNetIdx( *itr, CHARA_INFO_AI);

		CUIHudDomi_Indicator* pHudIndicate = static_cast<CUIHudDomi_Indicator*>(UIHUDDominationMgr::i()->GetHUD( HUD_DOMI_INDICATOR));
		if( pHudIndicate->InsertIndication( static_cast<i3GameObj*>(pNewAi), WAVE_MISSION_ELIMINATE, UI_Util::CHARACTER_OBJECT) == false)
			I3TRACE("  -- Can not Insert Indicate Respawn object(%s)\n", m_pAiRespawnObj->GetName());

		m_nCurrentTriggerAI = *itr;
	}
}