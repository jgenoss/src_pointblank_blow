#include "pch.h"
#include "DinoModeContext.h"
#include "BattleSlotContext.h"
#include "../DinoInfo.h"

CHARA_RES_ID DinoModeContext::GetDinoResType( INT32 idx)
{
	if( DinoSlot._uiTRexSlotIdx == idx)
		return CHARA_RES_ID_DINO_TREX;

	for(INT32 i=0; i<RAPTOR_TOTAL_CNT; i++)
	{
		if( DinoSlot._uiRaptorSlotIdx[i] == idx)
		{	
			const COMMON_PARTS_INFO * pInfo = BattleSlotContext::i()->getNetCharaInfo(idx)->getCommonParts();
			// vv3 - check
			T_ItemID dino_id = pInfo->m_Info[CHAR_EQUIPMENT_COMMON_DINO].m_TItemID;
			const CCharaInfo * pCharaDBInfo = g_pCharaInfoDataBase->GetCharaDBInfo( dino_id);
			if( pCharaDBInfo != nullptr)
			{
				I3ASSERT( i3::same_of<CDinoInfo*>(pCharaDBInfo));
				return (CHARA_RES_ID)pCharaDBInfo->GetResID();
			}
		}
	}
	return CHARA_RES_ID_UNKNOWN;
}
