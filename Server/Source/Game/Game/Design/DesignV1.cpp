#include "pch.h"
#include "DesignV1.h"
#include "UserSession.h"

CDesignV1::CDesignV1()
{
 
}

CDesignV1::~CDesignV1()
{
	OnDestroy();
}


BOOL CDesignV1::OnCreate()
{
	return TRUE;
}


void CDesignV1::OnDestroy()
{
	return;
}


BOOL CDesignV1::CheckItemPosition( T_ItemID TItemID, CHAR_EQUIPMENT eSlot )
{
	if( 0 == TItemID )	return TRUE;
	if( eSlot < CHAR_EQUIPMENT_PRIMARY || eSlot >= CHAR_EQUIPMENT_COUNT )	return TRUE;

	ITEM_TYPE eItemType = ITEM_TYPE(GET_ITEM_TYPE( TItemID ));	// TItemID의 ITEM_TYPE

	if( eItemType == ITEM_TYPE_BERET && eItemType == (ITEM_TYPE)(eSlot+13) ) return TRUE;	//Head에 베레모를 낀경우 예외처리
	if( eItemType == ITEM_TYPE_DINO && eItemType == (ITEM_TYPE)(eSlot+10) ) return TRUE;	//Chara에 Dino를 낀경우 예외처리
	if( eItemType != (ITEM_TYPE)(eSlot+1) ) return FALSE;
/*
	switch ( eSlot )
	{
	case CHAR_EQUIPMENT_SKIN:	
		{	
			return FALSE;
		}
		break;
	default:
		{
			
		}
		break;
	}
 */
	return TRUE;
}

 
T_RESULT CDesignV1::CheckBuyChara( UINT8 ui8CreateType, CUserSession* pSession, T_GoodsID TGoodsID ) 
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
		// v1.0은 Trans서버에서 자동 생성해 줍니다.
		case GOODS_BUY_CHAR_FREE:		return EVENT_ERROR_PBV15_NEW_CHARA_CREATE_INVALD;		break;
	}

	// 기본 캐릭터 구매 불가
	T_ItemID TCharItemID = TGoodsID / 100;

	for( INT32 i = 0 ; i < TEAM_COUNT ; ++i )
	{
		if( TCharItemID == g_TCharEquipBasic[ i ] ) return EVENT_ERROR_CHARA_BUY_DEFAULT;
	}

	// 기존에 동일 캐릭터를 영구제로 소지했다면 실패.
	CShop* pShop = g_pContextMain->GetShop();
	UINT32 ui32ItemCount			= pShop->GetItemCount();
	SHOP_ITEM_BASE*		pItemData	= pShop->GetItemAllData();

	for( UINT32 i = 0 ; i < ui32ItemCount ; ++i )
	{
		if( TCharItemID != pItemData[i]._ui32ItemID ) continue;

		if( ITEM_TYPE_ETERNITY != pItemData[i]._ui8AuthType ) continue;

		INVEN_BUFFER *pInven = pSession->m_Inventory.GetItemByItemID( TCharItemID );
		if( !pInven ) break;	// 동일 캐릭터를 인벤토리에 소지하지 않음.
			
		return	EVENT_ERROR_SHOP_ITEM_TYPE_ERROR;
	}

	return EVENT_ERROR_SUCCESS;
}

BOOL CDesignV1::SetTeamSlot(CMultiSlot& MultiSlot, UINT8 ui8TeamSlot, T_ItemID TCharItemID )
{
	switch( GET_ITEM_SUBTYPE(TCharItemID) )
	{
	case CHARACTER_TEAM_RED:
		return MultiSlot.SetTeamSlot( ui8TeamSlot, TEAM_RED );
		break;
	case CHARACTER_TEAM_BLUE:
		return MultiSlot.SetTeamSlot( ui8TeamSlot, TEAM_BLUE );
		break;
	}
	 
	return FALSE;
}

UINT32 CDesignV1::GetExpValue(INT32 i32Rank)
{
	if( i32Rank < 0 ) return 0xFFFFFFFF;
	if( i32Rank >= DEF_RANK_COUNT ) return 0xFFFFFFFF;

	return g_ExpValueV1[i32Rank];
}

INT32 CDesignV1::GetCharaSlotByTeam(INT32 i32Team, CUserSession* pSession )
{
	if( !pSession ) return 0;

	switch( i32Team )
	{
	case TEAM_RED:
		return pSession->m_MultiSlot.GetTeamSlot(TEAM_RED);
		break;
	case TEAM_BLUE:
		return pSession->m_MultiSlot.GetTeamSlot(TEAM_BLUE);
		break;
	}

	return 0;
	 
}

BOOL CDesignV1::CheckItemSlot( ITEM_INFO* aParts, INT32 i32SlotIdx )
{
	if( !aParts ) return TRUE;

	ITEM_INFO*	pItemHead = &aParts[CHAR_EQUIPMENT_HEAD];
	ITEM_INFO*	pItemFace = &aParts[CHAR_EQUIPMENT_FACE];
	ITEM_INFO*	pItemBeret = &aParts[CHAR_EQUIPMENT_BERET];

	// 헤드, 페이스 슬롯 둘중 하나는 반드시 장착 되어있어야함.
	if( pItemHead->m_TItemDBIdx <= 0 && pItemFace->m_TItemDBIdx <= 0 )
	{
		//ui32Check |= ui32Check & ~(0x01 << CHAR_EQUIPMENT_HEAD );
		return FALSE;
	}
	
	if( pItemHead->m_TItemDBIdx > 0 )
	{	// 헤드 슬롯 장착시, 페이스 슬롯에는 기본 페이스기어만 장착되어야함.
		if( !(pItemFace->m_TItemDBIdx & DEFAULT_ITEM_DBIDX_MIN) )
		{
			return FALSE;
		}
	}

	if( pItemFace->m_TItemDBIdx > 0  && !(pItemFace->m_TItemDBIdx & DEFAULT_ITEM_DBIDX_MIN)  )
	{	// 페이스 슬롯 장착시(기본페이스기어 제외), 헤드 슬롯은 장착 되면 안됨( 기본 헤드기어도 안됨 )
		if( pItemHead->m_TItemDBIdx > 0 )
		{
			return FALSE;
		}
	}

	if( pItemBeret->m_TItemDBIdx > 0  )
	{	// 베레모 슬롯에 베레모 장착되어 있을시 헤드슬롯 ItemID 가 701로 시작되어야함.( 기본헤드기어 제외 )

		if( pItemHead->m_TItemDBIdx & DEFAULT_ITEM_DBIDX_MIN )
		{
			return TRUE;
		}

		if( HEAD_PARTS_EQUIP_TYPE_BERET != GET_ITEM_SUBTYPE(pItemHead->m_TItemID))
		{
			return FALSE;
		}
	}

	return TRUE;
}

void CDesignV1::EquipDefaultParts(CUserSession* pSession, INT32 i32SlotIdx, UINT32 ui32Check)
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
		case CHAR_EQUIPMENT_PARTS_BERET:
			pSession->m_MultiSlot.SetCharEquipment(i32SlotIdx, (CHAR_EQUIPMENT_WEAPON_COUNT + i32Equip), 0, 0);
			continue;
		}


		if (0 == (ui32Check & (0x1 << (CHAR_EQUIPMENT_WEAPON_COUNT + i32Equip))))
		{
			pSession->m_MultiSlot.SetCharEquipment(i32SlotIdx, (CHAR_EQUIPMENT_WEAPON_COUNT + i32Equip), pDefaultEquip[i32Equip].m_TItemDBIdx, pDefaultEquip[i32Equip].m_TItemID);
		}
	}

}

BOOL CDesignV1::CheckReleaseEquip( CHAR_EQUIPMENT eSlot )
{
	switch( eSlot )
	{
	case CHAR_EQUIPMENT_HEAD:
	case CHAR_EQUIPMENT_BERET:
	//case CHAR_EQUIPMENT_SKIN:
		return TRUE;
	}
	return FALSE;
}

BOOL CDesignV1::CheckBuyRestrictionMedal( void )
{
	return FALSE;
}

BOOL CDesignV1::CheckBuyRestrictionTitle( void )
{
	return TRUE;
}


BOOL CDesignV1::GetCharaBonus(INT32 i32TeamIdx, CMultiSlot* pMultiSlot, REAL32& r32AddExpItem, REAL32& r32AddPotItem)
{
	if (!pMultiSlot) return FALSE;

	// 현재 팀에 해당하는 캐릭터로 보너스 적용
	ITEM_INFO	CharaInfo;
	INT32		i32CharSlot = pMultiSlot->GetTeamSlot( (TEAM_TYPE)i32TeamIdx );
	pMultiSlot->GetCharEquipment(i32CharSlot, CHAR_EQUIPMENT_CHARA, &CharaInfo);

	if (i32TeamIdx == TEAM_RED)
	{
		switch (CharaInfo.m_TItemID)
		{
			// 디폭스	 // MAKE_ITEM_ID(ITEM_TYPE_CHARA, CHARACTER_TEAM_RED, CHARA_RES_ID_D_FOX ):
		case 601012:	r32AddExpItem += 0.20001f;	return TRUE;

		// 바이퍼레드	// MAKE_ITEM_ID(ITEM_TYPE_CHARA, CHARACTER_TEAM_RED, CHARA_RES_ID_VIPERRED ):
		case 601015:	r32AddPotItem += 0.30001f;	return TRUE;
		
		// 강화 디폭스	//MAKE_ITEM_FLAG(ITEM_TYPE_CHARA, CHARACTER_TEAM_RED, CHARA_RES_ID_RFC_D_FOX):
		case 601026:	r32AddExpItem += 0.20001f;	return TRUE;
		
		// 강화 바이퍼레드	//MAKE_ITEM_FLAG(ITEM_TYPE_CHARA, CHARACTER_TEAM_RED, CHARA_RES_ID_RFC_VIPERRED):
		case 601028:	r32AddPotItem += 0.30001f;	return TRUE;
		
		//// 타란튤라 GRS	//MAKE_ITEM_FLAG(ITEM_TYPE_CHARA, CHARACTER_TEAM_RED, CHARACTER_RES_ID_FEMALE_RED_GRS):
		case 601032:	r32AddExpItem += 0.20001f;	return TRUE;

		//// 레드불스 GRS	//MAKE_ITEM_FLAG(ITEM_TYPE_CHARA, CHARACTER_TEAM_RED, CHARA_RES_ID_GRS_RED_BULLS):
		case 601030:	r32AddPotItem += 0.20001f;	return TRUE;
		
		}
	}
	else
	{
		switch (CharaInfo.m_TItemID)
		{
		// 레오파트			 // MAKE_ITEM_ID(ITEM_TYPE_CHARA, CHARACTER_TEAM_BLUE, CHARA_RES_ID_LEOPARD ):
		case 602013:	r32AddExpItem += 0.20001f;	return TRUE;

		// 하이드			// MAKE_ITEM_ID(ITEM_TYPE_CHARA, CHARACTER_TEAM_BLUE, CHARA_RES_ID_HIDE ):
		case 602014:	r32AddPotItem += 0.30001f;	return TRUE;
		
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
	}
	return FALSE;
}

BOOL CDesignV1::SetMainCharBaseInfo(CMultiSlot* pMultiSlot, S2MO_CHAR_BASE_INFO* pCharBaseInfo, INT32 i32Team)
{
	if (!pMultiSlot)			return FALSE;
	if (!pCharBaseInfo)		return FALSE;
	if (i32Team <= TEAM_NONE)	return FALSE;
	if (i32Team >= TEAM_COUNT)	return FALSE;

	return pMultiSlot->SetCharBaseInfo(pMultiSlot->GetTeamSlot((TEAM_TYPE)i32Team), pCharBaseInfo);
}

BOOL CDesignV1::GetMainCharBaseInfo(CMultiSlot* pMultiSlot, S2MO_CHAR_BASE_INFO* pCharBaseInfo, INT32 i32Team )
{
	if (!pMultiSlot)			return FALSE;
	if ( !pCharBaseInfo )		return FALSE;
	if (i32Team <= TEAM_NONE)	return FALSE;
	if (i32Team >= TEAM_COUNT)	return FALSE;

	return pMultiSlot->GetCharBaseInfo(pMultiSlot->GetTeamSlot((TEAM_TYPE)i32Team), pCharBaseInfo);			
}

BOOL CDesignV1::GetMainCharEquipInfo(CMultiSlot* pMultiSlot, S2MO_CHAR_EQUIP_INFO* pCharEquipInfo, INT32 i32Team  )
{
	if (!pMultiSlot)			return FALSE;
	if (!pCharEquipInfo)		return FALSE;
	if (i32Team <= TEAM_NONE)	return FALSE;
	if (i32Team >= TEAM_COUNT)	return FALSE;

	return pMultiSlot->GetCharEquipInfo(pMultiSlot->GetTeamSlot((TEAM_TYPE)i32Team), pCharEquipInfo);
}

BOOL CDesignV1::GetMainCharSkillInfo(CMultiSlot* pMultiSlot, S2MO_CHAR_SKILL_INFO* pCharSkillInfo, INT32 i32Team  )
{
	if (!pMultiSlot)			return FALSE;
	if (!pCharSkillInfo)		return FALSE;
	if (i32Team <= TEAM_NONE)	return FALSE;
	if (i32Team >= TEAM_COUNT)	return FALSE;

	return pMultiSlot->GetCharSkillInfo(pMultiSlot->GetTeamSlot((TEAM_TYPE)i32Team), pCharSkillInfo);
}

BOOL CDesignV1::GetMainCharEquipmentAll(CMultiSlot* pMultiSlot, ITEM_INFO* pItemInfo, INT32 i32Team  )
{
	if (!pMultiSlot)			return FALSE;
	if (!pItemInfo)				return FALSE;
	if (i32Team <= TEAM_NONE)	return FALSE;
	if (i32Team >= TEAM_COUNT)	return FALSE;

	return pMultiSlot->GetCharEquipmentAll(pMultiSlot->GetTeamSlot((TEAM_TYPE)i32Team), pItemInfo);
}

BOOL CDesignV1::GetMainCharEquipment(CMultiSlot* pMultiSlot, INT32 i32Equipment, ITEM_INFO* pItemInfo, INT32 i32Team  )
{
	if (!pMultiSlot)			return FALSE;
	if (!pItemInfo)		return FALSE;
	if (i32Team <= TEAM_NONE)	return FALSE;
	if (i32Team >= TEAM_COUNT)	return FALSE;

	return pMultiSlot->GetCharEquipment(pMultiSlot->GetTeamSlot((TEAM_TYPE)i32Team), i32Equipment, pItemInfo);
}

BOOL CDesignV1::DeleteChara(CMultiSlot& MultiSlot, INT32 i32DeleteSlotIdx, INT32& i32MainSlotIdx)
{
	return MultiSlot.DeleteCharInfoTeam(i32DeleteSlotIdx, i32MainSlotIdx);
}

BOOL CDesignV1::GetSkinBonus(CMultiSlot* pMultiSlot, REAL32& r32AddExpItem, REAL32& r32AddPotItem)
{
	// 스킨 파츠 보너스 적용하지 않음. 기획의도
	return FALSE;
}