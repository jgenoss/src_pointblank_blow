#include "pch.h"
#include "DesignV2.h"
#include "UserSession.h"

CDesignV2::CDesignV2()
{
 
}

CDesignV2::~CDesignV2()
{
	OnDestroy();
}


BOOL CDesignV2::OnCreate()
{
	return TRUE;
}


void CDesignV2::OnDestroy()
{
	return;
}


BOOL CDesignV2::CheckItemPosition( T_ItemID TItemID, CHAR_EQUIPMENT eSlot )
{
	if( 0 == TItemID )	return TRUE;
	if( eSlot < CHAR_EQUIPMENT_PRIMARY || eSlot >= CHAR_EQUIPMENT_COUNT )	return TRUE;

	ITEM_TYPE eItemType = ITEM_TYPE(GET_ITEM_TYPE( TItemID ));	// TItemID의 ITEM_TYPE

	switch ( eSlot )
	{
	case CHAR_EQUIPMENT_HEAD:
		{
			if( eItemType != ITEM_TYPE_HEADGEAR && ITEM_TYPE_BERET != eItemType ) return FALSE;
		}
		break;
	case CHAR_EQUIPMENT_BERET:
		{	// 베레모 슬롯에 장착이 불가능 합니다. TItemID가 0이어야 됨. 위에서 리턴되야함.
			return FALSE;
		}
		break;
	case CHAR_EQUIPMENT_CHARA:
		{
			if( ITEM_TYPE_DINO	!= eItemType &&	ITEM_TYPE_CHARA	!= eItemType ) return FALSE;
		}
		break;
	default:
		{
			if( eItemType != (ITEM_TYPE)(eSlot+1) ) return FALSE;
		}
		break;
	}
 
	return TRUE;
}

T_RESULT CDesignV2::CheckBuyChara( UINT8 ui8CreateType, CUserSession* pSession, T_GoodsID TGoodsID ) 
{
	INT32 i32CharCount = pSession->m_MultiSlot.GetCharSlotCount()+1;
	if( i32CharCount > S2MO_MULTI_SLOT_COUNT )
	{	// 멀티슬롯 갯수 초과
		return EVENT_ERROR_CHARA_FULL_SLOT;
	}

	if (pSession->m_MultiSlot.GetInsertSlot() == CHAR_SLOT_NONE)
	{	// PCCafe 캐릭터용 슬롯
		return EVENT_ERROR_CHARA_FULL_SLOT;
	}

	switch( ui8CreateType )
	{
	case GOODS_BUY_CHAR_FREE:
		{	// Create 는 최초 생성시에만 가능
			if( 1 != i32CharCount )											return EVENT_ERROR_PBV15_NEW_CHARA_CREATE_INVALD;
		}
		break;
	
		// 랭크 체크 하지 않습니다. 기획의도. 2015.09.14 어깨깡패
	case GOODS_BUY_CASH:
	case GOODS_BUY_POINT:
		break;
	default:
		return EVENT_ERROR_PBV15_CHARA_BUY_INVALD;
	}


	return EVENT_ERROR_SUCCESS;
}



BOOL CDesignV2::SetTeamSlot( CMultiSlot& MultiSlot, UINT8 ui8TeamSlot, T_ItemID TCharItemID )
{
	return MultiSlot.SetTeamSlot( ui8TeamSlot, TEAM_RED );
}

UINT32 CDesignV2::GetExpValue(INT32 i32Rank)
{
	if( i32Rank < 0 ) return 0xFFFFFFFF;
	if( i32Rank >= DEF_RANK_COUNT ) return 0xFFFFFFFF;

	return g_ExpValueV2[i32Rank];
}

INT32 CDesignV2::GetCharaSlotByTeam(INT32 i32Team, CUserSession* pSession )
{
	if( !pSession ) return 0;

	return pSession->m_MultiSlot.GetTeamSlot(TEAM_RED); 
}

BOOL CDesignV2::CheckItemSlot( ITEM_INFO* aParts, INT32 i32SlotIdx )
{
	if( !aParts ) return TRUE;

	ITEM_INFO*	pItemHead = &aParts[CHAR_EQUIPMENT_HEAD];
	ITEM_INFO*	pItemFace = &aParts[CHAR_EQUIPMENT_FACE];
	

	// 헤드,페이스 슬롯에 아이템이 장착되어 있어야 합니다.
	if( pItemHead->m_TItemDBIdx <= 0  )
	{
		return FALSE;
	}
	if( pItemHead->m_TItemID <= 0 )
	{	
		return FALSE;
	}
	if( pItemFace->m_TItemDBIdx <= 0 )
	{
		return FALSE;
	}
	if( pItemFace->m_TItemID <= 0 )
	{	
		return FALSE;
	}

	return TRUE;
}

void CDesignV2::EquipDefaultParts(CUserSession* pSession, INT32 i32SlotIdx, UINT32 ui32Check)
{
	// 3. 파츠
	ITEM_INFO*	pDefaultEquip = g_pContextMain->GetDefaultCharEquipSetting();
	if (NULL == pDefaultEquip)	return;

	for (INT32 i32Equip = CHAR_EQUIPMENT_PARTS_HEAD; i32Equip < CHAR_EQUIPMENT_PARTS_COUNT; i32Equip++)
	{
		switch (i32Equip)
		{
		case CHAR_EQUIPMENT_PARTS_HEAD:
		case CHAR_EQUIPMENT_PARTS_FACE:
			pSession->m_MultiSlot.SetCharEquipment(i32SlotIdx, (CHAR_EQUIPMENT_WEAPON_COUNT + i32Equip), pDefaultEquip[i32Equip].m_TItemDBIdx, pDefaultEquip[i32Equip].m_TItemID);
			continue;
		}

		if (0 == (ui32Check & (0x1 << (CHAR_EQUIPMENT_WEAPON_COUNT + i32Equip))))
		{
			pSession->m_MultiSlot.SetCharEquipment(i32SlotIdx, (CHAR_EQUIPMENT_WEAPON_COUNT + i32Equip), pDefaultEquip[i32Equip].m_TItemDBIdx, pDefaultEquip[i32Equip].m_TItemID);
		}
	}

}

BOOL CDesignV2::CheckReleaseEquip( CHAR_EQUIPMENT eSlot )
{
	switch( eSlot )
	{
	case CHAR_EQUIPMENT_BERET:
	//case CHAR_EQUIPMENT_SKIN:
		return TRUE;
	}
	return FALSE;
}

BOOL CDesignV2::CheckBuyRestrictionMedal( void )
{
	return TRUE;
}

BOOL CDesignV2::CheckBuyRestrictionTitle( void )
{
	return FALSE;
}


BOOL CDesignV2::GetCharaBonus(INT32 i32TeamIdx, CMultiSlot* pMultiSlot, REAL32& r32AddExpItem, REAL32& r32AddPotItem)
{
	if (!pMultiSlot) return FALSE;

	// 레드슬롯(메인슬롯)으로 적용합니다.
	ITEM_INFO	CharaInfo;
	INT32		i32CharSlot = pMultiSlot->GetTeamSlot(TEAM_RED);
	pMultiSlot->GetCharEquipment(i32CharSlot, CHAR_EQUIPMENT_CHARA, &CharaInfo);

	switch (CharaInfo.m_TItemID)
	{
		// 강화 디폭스	//MAKE_ITEM_FLAG(ITEM_TYPE_CHARA, CHARACTER_TEAM_RED, CHARA_RES_ID_RFC_D_FOX):
	case 601026:	r32AddExpItem += 0.20001f;	return TRUE;

		// 강화 바이퍼레드	//MAKE_ITEM_FLAG(ITEM_TYPE_CHARA, CHARACTER_TEAM_RED, CHARA_RES_ID_RFC_VIPERRED):
	case 601028:	r32AddPotItem += 0.30001f;	return TRUE;

		//// 타란튤라 GRS	//MAKE_ITEM_FLAG(ITEM_TYPE_CHARA, CHARACTER_TEAM_RED, CHARACTER_RES_ID_FEMALE_RED_GRS):
	case 601032:	r32AddExpItem += 0.20001f;	return TRUE;

		//// 레드불스 GRS	//MAKE_ITEM_FLAG(ITEM_TYPE_CHARA, CHARACTER_TEAM_RED, CHARA_RES_ID_GRS_RED_BULLS):
	case 601030:	r32AddPotItem += 0.20001f;	return TRUE;


		// 강화 레오파트	// MAKE_ITEM_ID(ITEM_TYPE_CHARA, CHARACTER_TEAM_BLUE, CHARA_RES_ID_RFC_LEOPARD ):
	case 602027:	r32AddExpItem += 0.20001f;	return TRUE;

		// 강화 하이드		// MAKE_ITEM_ID(ITEM_TYPE_CHARA, CHARACTER_TEAM_BLUE, CHARA_RES_ID_RFC_HIDE ):
	case 602029:	r32AddPotItem += 0.30001f;	return TRUE;

		// 킨아이즈 GRS		// MAKE_ITEM_ID(ITEM_TYPE_CHARA, CHARACTER_TEAM_BLUE, CHARA_RES_ID_GRS_SWAT_FEMALE ):
	case 602033:	r32AddExpItem += 0.20001f;	return TRUE;

		// 액시드폴 GRS		// MAKE_ITEM_ID(ITEM_TYPE_CHARA, CHARACTER_TEAM_BLUE, CHARA_RES_ID_GRS_ACID_POLE ):
	case 602031:	r32AddPotItem += 0.20001f;	return TRUE;

		// 보페 레오파드	// MAKE_ITEM_ID(ITEM_TYPE_CHARA, CHARACTER_TEAM_BLUE, CHARA_RES_ID_BOPE_REOPARD ):
	case 602035:	r32AddExpItem += 0.15001f; r32AddPotItem += 0.15001f;	return TRUE;


	}
	return FALSE;
}

BOOL CDesignV2::SetMainCharBaseInfo(CMultiSlot* pMultiSlot, S2MO_CHAR_BASE_INFO* pCharBaseInfo, INT32 i32Team)
{
	if (!pMultiSlot)			return FALSE;
	if (!pCharBaseInfo)		return FALSE;
	
	return pMultiSlot->SetCharBaseInfo(pMultiSlot->GetTeamSlot(TEAM_RED), pCharBaseInfo);
}

BOOL CDesignV2::GetMainCharBaseInfo(CMultiSlot* pMultiSlot, S2MO_CHAR_BASE_INFO* pCharBaseInfo, INT32 i32Team)
{
	if (!pMultiSlot)			return FALSE;
	if (!pCharBaseInfo)		return FALSE;
	
	return pMultiSlot->GetCharBaseInfo(pMultiSlot->GetTeamSlot(TEAM_RED), pCharBaseInfo);
}

BOOL CDesignV2::GetMainCharEquipInfo(CMultiSlot* pMultiSlot, S2MO_CHAR_EQUIP_INFO* pCharEquipInfo, INT32 i32Team)
{
	if (!pMultiSlot)			return FALSE;
	if (!pCharEquipInfo)		return FALSE;
	
	return pMultiSlot->GetCharEquipInfo(pMultiSlot->GetTeamSlot(TEAM_RED), pCharEquipInfo);
}

BOOL CDesignV2::GetMainCharSkillInfo(CMultiSlot* pMultiSlot, S2MO_CHAR_SKILL_INFO* pCharSkillInfo, INT32 i32Team)
{
	if (!pMultiSlot)			return FALSE;
	if (!pCharSkillInfo)		return FALSE;

	return pMultiSlot->GetCharSkillInfo(pMultiSlot->GetTeamSlot(TEAM_RED), pCharSkillInfo);
}

BOOL CDesignV2::GetMainCharEquipmentAll(CMultiSlot* pMultiSlot, ITEM_INFO* pItemInfo, INT32 i32Team)
{
	if (!pMultiSlot)			return FALSE;
	if (!pItemInfo)				return FALSE;

	return pMultiSlot->GetCharEquipmentAll(pMultiSlot->GetTeamSlot(TEAM_RED), pItemInfo);
}

BOOL CDesignV2::GetMainCharEquipment(CMultiSlot* pMultiSlot, INT32 i32Equipment, ITEM_INFO* pItemInfo, INT32 i32Team)
{
	if (!pMultiSlot)			return FALSE;
	if (!pItemInfo)		return FALSE;
	
	return pMultiSlot->GetCharEquipment(pMultiSlot->GetTeamSlot(TEAM_RED), i32Equipment, pItemInfo);
}

BOOL CDesignV2::GetSkinBonus(CMultiSlot* pMultiSlot, REAL32& r32AddExpItem, REAL32& r32AddPotItem)
{
	if (!pMultiSlot) return FALSE;

	// v1.5 스킨 파츠 장착시 Exp +10%, Point + 10%
	ITEM_INFO				SkinItemInfo;

	if ( FALSE == pMultiSlot->GetCharEquipment(pMultiSlot->GetTeamSlot(TEAM_RED), CHAR_EQUIPMENT_SKIN, &SkinItemInfo) )return FALSE;

	if (SkinItemInfo.m_TItemDBIdx & DEFAULT_ITEM_DBIDX_MIN || SkinItemInfo.m_TItemID > DEFAULT_PARTS_ITEM_ID_MIN) return FALSE;

	if (ITEM_TYPE_SKIN != GET_ITEM_TYPE(SkinItemInfo.m_TItemID)) return FALSE;
		
	if (1403019 == SkinItemInfo.m_TItemID) //ItemID 1403019 스킨만 20% 적용 추후 수정 예정
	{
		r32AddExpItem += 0.20001f;
		r32AddPotItem += 0.20001f; 		
	}
	else
	{
		r32AddExpItem += 0.10001f;
		r32AddPotItem += 0.10001f;
	} 

	return TRUE;
}

BOOL CDesignV2::DeleteChara(CMultiSlot& MultiSlot, INT32 i32DeleteSlotIdx, INT32& i32MainSlotIdx)
{
	return MultiSlot.DeleteCharInfo(i32DeleteSlotIdx, i32MainSlotIdx);
}