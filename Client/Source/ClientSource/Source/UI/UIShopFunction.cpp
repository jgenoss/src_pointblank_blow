#include "pch.h"
#include "UIShopFunction.h"

#include "UIUtil.h"
#include "Shop.h"
#include "UIItemInfoUtil.h"
#include "UISideMenu.h"
#include "UICharaInfoMgr.h"

#include "InvenUtil.h"
#include "UIInvenInfoMgr.h"
#include "BuyUtil.h"

#include "Batmanager.h"
#include "UserInfoContext.h"
#include "ShopContext.h"

#include "i3Base/string/ext/format_string.h"
#include "i3Base/ColorString.h"
#include "i3Base/string/ext/mb_to_utf16.h"

namespace
{
/************************************************/
// basic function
ShopItemCategory	_GetShopItemCategory( T_ItemID itemID)
{
	switch( item_def::get_category_tab(itemID) )
	{
	case item_def::CTG_WEAPON:		return SHOP_ITEM_CATEGORY_WEAPON;
	case item_def::CTG_CHARA:
	case item_def::CTG_PARTS:
	case item_def::CTG_SKIN:		return SHOP_ITEM_CATEGORY_CHARACTER;
	case item_def::CTG_ITEM: 
		if( item_def::get_type(itemID) == ITEM_TYPE_DINO)
			return SHOP_ITEM_CATEGORY_CHARACTER;

		return SHOP_ITEM_CATEGORY_ITEM;
	}	

	return NUM_SHOP_ITEM_CATEGORY;
}

/************************************************/
// _FormatAuthTypeText
bool	_FormatAuthTypeText(UINT8 type, UINT32 arg, i3::wstring& out )
{
	switch ( type )
	{
	case ITEM_TYPE_COUNT:
		{
			i3::sprintf(out, L"%d%s", arg, GAME_RCSTRING("STBL_IDX_ITEM_COUNT_UNIT"));
		}
		break;
	case ITEM_TYPE_TIME:
		{
			INT32 hour_term = ConvertSecToUseHour(arg);
			//	사용 기간
			if (hour_term < 24)
			{
				i3::sprintf(out, L"%d%s", hour_term, GAME_RCSTRING("STR_UI_HOUR"));
			}	
			else
			{
				INT32 day_term = hour_term / 24;
				i3::sprintf(out, L"%d%s", day_term, GAME_RCSTRING("STBL_IDX_ITEM_TERM_UNIT"));
			}
		}
		break;
	case ITEM_TYPE_ETERNITY:
		out = GAME_RCSTRING("STR_POPUP_USABLE_FREE");
		break;
	default:
		I3ASSERT_0;
		return false;
		break;
	}
	return true;
}


/************************************************/
// _GetShopPeriodCountString_SetItem
void _GetShopPeriodCountString_SetItem(const SHOP_GOODS* pGoods, i3::wstring& out)
{
	bool isPeriod = false;
	INT32 periodItemIdx = 0;
	INT32 notPointItemIdx = 0;

	SHOP_GOODS_LIST * pGoodsList = CShop::i()->GetGoodsListData(pGoods->_GoodsID);

	INT32 ItemCount = pGoodsList->_ui8ItemCount;
	for(INT32 j = 0; j < ItemCount ; j++)
	{
		// 기간제라면.
		if (ITEM_TYPE_TIME == pGoodsList->_ppItemData[j]->_ui8AuthType)
		{
			isPeriod = true;
			periodItemIdx = j;
			break;
		}
		else if (ITEM_TYPE_POINT != GET_ITEM_TYPE(pGoodsList->_ppItemData[j]->_ui32ItemID))
		{
			notPointItemIdx = j;
		}
	}

	if (isPeriod)
	{
		UINT32 seconds = pGoodsList->_pui32ItemArg[periodItemIdx];
		UINT32 hour = ConvertSecToUseHour(seconds);
		if (hour < 24)
		{
			i3::sprintf(out, L"%d%s", hour, GAME_RCSTRING("STR_UI_HOUR"));
		}
		else
		{
			UINT32 day = hour / 24;
			i3::sprintf(out, L"%d%s", day, GAME_RCSTRING("STBL_IDX_ITEM_TERM_UNIT"));
		}
	}
	else
	{
		T_ItemID ItemID = pGoodsList->_ppItemData[notPointItemIdx]->_ui32ItemID;

		//	유료 아이템 Goods에는 pGoods->_AuthArg에 모두 갯수가 있기 때문에 예외 처리가 필요 komet
		switch( item_def::get_item_type(ItemID) )
		{
			//	지속성
		case ITEM_TYPE_WRAP_PERIOD:
			{
				UINT32 hour = GetMaintenanceItemHour(pGoods->_ItemID, pGoods->_AuthType, pGoods->_AuthArg);
				if (hour < 24)
				{
					i3::sprintf(out, L"%d%s", hour, GAME_RCSTRING("STR_UI_HOUR"));
				}
				else
				{
					UINT32 day = hour / 24;
					i3::sprintf(out, L"%d%s", day, GAME_RCSTRING("STBL_IDX_ITEM_TERM_UNIT"));
				}
			}
			break;
			//	소모성
		case ITEM_TYPE_MAINTENANCE:
		case ITEM_TYPE_WRAP_COUNT:	
			{
				UINT32 count = pGoodsList->_pui32ItemArg[notPointItemIdx];
				i3::sprintf(out, L"%d%s", count, GAME_RCSTRING("STBL_IDX_ITEM_COUNT_UNIT"));
			}
			break;
		default:
			break;
		}
	}
}


/************************************************/
// _GetShopPeriodCountString_NonSetItem
void _GetShopPeriodCountString_NonSetItem( T_ItemID itemID, UINT8 authType, UINT32 arg, i3::wstring& out)
{
	switch( _GetShopItemCategory(itemID) )
	{
	case SHOP_ITEM_CATEGORY_WEAPON:
	case SHOP_ITEM_CATEGORY_CHARACTER:
		if ( LocaleValue::Enable("EnableRepairWeapon") && CShopContext::i()->IsDurabilityItem(itemID) )
		{
			out = GAME_RCSTRING("STR_SHOP_PERPETUITY_BUY");
			break;
		}

		_FormatAuthTypeText(authType, arg, out);
		break;

	case SHOP_ITEM_CATEGORY_ITEM:
		switch( item_def::get_item_type(itemID) )
		{
		case ITEM_TYPE_MAINTENANCE:
		case ITEM_TYPE_WRAP_COUNT:	
		case ITEM_TYPE_INGAMEITEM:
		case ITEM_TYPE_DOMIITEM:
			i3::sprintf(out, L"%d%s", arg, GAME_RCSTRING("STBL_IDX_ITEM_COUNT_UNIT"));
			break;

		case ITEM_TYPE_WRAP_PERIOD:
			{
				UINT32 hour = GetMaintenanceItemHour(itemID, authType, arg);

				if (hour < 24)
				{
					i3::sprintf(out, L"%d%s", hour, GAME_RCSTRING("STR_UI_HOUR"));
				}
				else
				{
					UINT32 day = hour/24;
					i3::sprintf(out, L"%d%s", day, GAME_RCSTRING("STBL_IDX_ITEM_TERM_UNIT"));
				}
			}
			break;
		case ITEM_TYPE_COUPON:
			{
				UINT32 hour = ConvertSecToUseHour(arg);

				if (hour < 24)
				{
					i3::sprintf(out, L"%d%s", hour, GAME_RCSTRING("STR_UI_HOUR"));
				}
				else
				{
					UINT32 day = hour / 24;
					i3::sprintf(out, L"%d%s", day, GAME_RCSTRING("STBL_IDX_ITEM_TERM_UNIT"));
				}
			}
			break;
		}	//end of switch(type)
		break;	//end of case shop_util::CTG_ITEM:
	}
}


/************************************************/
// GetShopItemName_Weapon
bool GetShopItemName_Weapon(i3::rc_wstring& outName, T_ItemID itemID)
{
	CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo( itemID );

	if ( pInfo == nullptr )
	{
		i3::sprintf(outName, L"%s (%d)", INVALID_TEXTW, itemID);
		::ColorString(outName, 255, 0, 0);
		return false;
	}
	else
	{
#if defined( I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
		if( g_pWeaponManager->IsExistResource( WEAPON::ItemID2Class(itemID), WEAPON::ItemID2ResID(itemID)) == false)
		{ 
			i3::sprintf(outName, L"No I3CHR (%d)", itemID );
			::ColorString(outName, 255,0,0);
			return false;
		}
#endif

		pInfo->GetWeaponName(outName);

		if( outName.empty() )
		{
			i3::sprintf(outName, L"No Name (%d)", itemID);
			::ColorString(outName, 255, 0, 0);
			return false;
		}
	}

	return true;
}

void GetShopItemName_Weapon( i3::rc_wstring& outName, const SHOP_GOODS* goods)
{
	if( GetShopItemName_Weapon(outName, goods->_ItemID) == false)
	{
		i3::wstring str;
		i3::sprintf( str, L"\nGoods:%d", goods->_GoodsID);
		i3::generic_string_cat( outName, str);
		::ColorString(outName, 255, 0, 0);
	}
}

bool	CheckEquipShapeSet( i3Node * pRoot)
{
	i3::vector<i3Node*> list;
	i3Scene::FindNodeByType( pRoot, i3Transform2::static_meta(), list);
	i3Scene::FindNodeByType( pRoot, i3ShapeSetContainer::static_meta(), list);
	i3Scene::FindNodeByType( pRoot, i3Body::static_meta(), list);

	for( UINT32 i = 0; i < list.size(); i++)
	{
		if( i3::same_of<i3Transform2*>( list[i]))
		{
			i3Transform2 * pNode = (i3Transform2 *) list[i];

			if( pNode->getShapeSet() != nullptr)
				return true;
		}
		else if( i3::same_of<i3ShapeSetContainer* >( list[i]))
		{
			i3ShapeSetContainer * pNode = (i3ShapeSetContainer *) list[i];

			if( pNode->getShapeSet() != nullptr)
				return true;
		}
		else if( i3::same_of<i3Body* >( list[i]))
		{
			i3Body * pBody = (i3Body *) list[i];

			i3LOD * pLOD = pBody->getFrameLOD();
			if( pLOD != nullptr)
			{
				i3Skeleton * pSkel = pLOD->getSkeleton();

				if( pSkel != nullptr)
				{
					for( INT32 j = 0; j < pSkel->getBoneCount(); j++)
					{
						if( pSkel->isShapeSet( j))
						{
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

/************************************************/
// GetShopItemName_Char
void	GetShopItemName_Char(i3::rc_wstring& outName, T_ItemID itemID)
{
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	// 장비 검사
	CHARACTER_CLASS_TYPE charaClass = EQUIP::ItemID2Class( itemID);
	if( charaClass == CHARACTER_CLASS_HEADGEAR || charaClass == CHARACTER_CLASS_FACEGEAR ||
		charaClass == CHARACTER_CLASS_BELT || charaClass == CHARACTER_CLASS_HOLSTER || charaClass == CHARACTER_CLASS_BERET )
	{
		EQUIP::ePART part = EQUIP::ItemID2PartsType( itemID);
		EQUIP::RES_ID res_num = EQUIP::ItemID2ResID(itemID);

		if( g_pCharaManager->IsExistEquipResource( part, res_num, TEAM_RED) == false)
		{
			i3::sprintf(outName, L"No I3S Red(%d)", itemID);
			::ColorString(outName, 255, 0, 0);
			return;
		}

		if( g_pCharaManager->IsExistEquipResource( part, res_num, TEAM_BLUE) == false)
		{
			i3::sprintf(outName, L"No I3S Blue(%d)", itemID);
			::ColorString(outName, 255, 0, 0);
			return;
		}

		if( part == EQUIP::ePART_HEADGEAR || part == EQUIP::ePART_BERET )
		{
			for(INT32 idx = 0 ; idx < TEAM_COUNT ; ++idx)
			{
				const i3GameResSceneGraph * pRes = g_pCharaManager->QueryEquipResource( itemID, static_cast<TEAM_TYPE>(idx) );				
				if(pRes == nullptr)
				{
					i3::sprintf(outName, L"No Resource %s(%d)", idx == TEAM_RED ? L"Red" : L"Blue", itemID);
					::ColorString(outName, 255, 0, 0);
					return;
				}

				i3SceneGraphInfo * pSgInfo = const_cast<i3GameResSceneGraph*>(pRes)->getSceneGraphInfo();
				if (pSgInfo == nullptr)
				{
					i3::sprintf(outName, L"No SgInfo %s(%d)", idx == TEAM_RED ? L"Red" : L"Blue", itemID);
					::ColorString(outName, 255, 0, 0);
					return;
				}

				if( CheckEquipShapeSet( pSgInfo->getInstanceSg() ) == false)
				{
					i3::sprintf(outName, L"No PhysXShape %s(%d)", idx == TEAM_RED ? L"Red" : L"Blue", itemID);
					::ColorString(outName, 255, 0, 0);
					return;
				}
			}
		}
	}
#endif

	g_pCharaInfoDataBase->GetCharacterName(itemID, outName);

	if( outName.empty() )
	{
		i3::sprintf(outName, L"No Name (%d)", itemID);
		::ColorString(outName, 255, 0, 0);
	}
}
void	GetShopItemName_Char(i3::rc_wstring& outName, const SHOP_GOODS* goods)
{
	GetShopItemName_Char(outName, goods->_ItemID);
}

/************************************************/
// GetShopItemName_Char
void	GetMultiSlotName_Char(i3::rc_wstring& outName, ::ITEM_INFO item_info)
{
	const TTCHAR* name = CCharaInfoContext::i()->GetNickName(item_info);

	outName = name;	

#ifdef I3_DEBUG	//테스트용 코드
	if( GameUI::IsKeyboardPressed(I3I_KEY_LCTRL) ) 
	{
		i3::sprintf( outName, L"%s\nId %d, Wdb %d", outName, item_info.m_TItemID, CInvenList::i()->FindWareDBIdx_ByItemID( item_info.m_TItemID) );
	}
	else if( GameUI::IsKeyboardPressed(I3I_KEY_LALT) )
	{
		INT32 idx = CCharaInfoContext::i()->FindCharaInfoIndexByItemID(item_info.m_TItemID);
		if( idx == -1)	return;

		const CHARA_INFO* pChara = CCharaInfoContext::i()->GetCharaInfo(idx);
		i3::sprintf( outName, L"%s\nslot %d", outName, pChara->m_CharaBasicInfo.m_ui8CharaSlotIdx );
	}
#endif
}


/************************************************/
// GetShopItemName_Item
void	GetShopItemName_Item(i3::rc_wstring& outName, T_ItemID itemID)
{
	g_pShopItemInfoDataBase->GetItemName(itemID, outName);
}
void	GetShopItemName_Item(i3::rc_wstring& outName, const SHOP_GOODS* goods)
{
	if ( usf::IsSetItem(goods) )
	{
		INT32 GoodsID = goods->_GoodsID;
		ShopPackageGoodsInfo* pInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(GoodsID);
		if (pInfo == nullptr)
		{
			i3::sprintf(outName, L"%s (%d, %d)", INVALID_TEXTW, goods->_GoodsID, goods->_ItemID); 
			::ColorString(outName, 255, 0, 0);
		}
		else
		{
			outName = pInfo->GetScriptName();
		}
	}
	else
	{
		GetShopItemName_Item(outName, goods->_ItemID);
	}
}

/************************************************/
// GetItemClassName_Weapon
i3::rc_wstring GetItemClassName_Weapon( T_ItemID itemID)
{
	switch( item_def::get_weapon_slot_type(itemID) )
	{
	case WEAPON_SLOT_PRIMARY: return GAME_RCSTRING("STR_INVEN_PRIMARY_WEAPON");	break;
	case WEAPON_SLOT_SECONDARY: return GAME_RCSTRING("STR_INVEN_SECONDARY_WEAPON"); break;
	case WEAPON_SLOT_MELEE:	 return GAME_RCSTRING("STR_INVEN_MELEE_WEAPON"); break;
	case WEAPON_SLOT_THROWING1:	return GAME_RCSTRING("STR_INVEN_THROWING_WEAPON"); break;
	case WEAPON_SLOT_THROWING2: return GAME_RCSTRING("STR_INVEN_SPECIAL_WEAPON"); break;
	}

	return i3::rc_wstring();
}

/************************************************/
// GetItemClassName_Parts
i3::rc_wstring GetItemClassName_Parts( T_ItemID itemID)
{
	switch( item_def::get_parts_type(itemID) )
	{
	case ITEM_TYPE_HEADGEAR:	return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_HEADGEAR");	break;
	case ITEM_TYPE_FACEGEAR:	return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_FACEGEAR"); break;
	case ITEM_TYPE_UPPER:		return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_UPPER"); break;
	case ITEM_TYPE_LOWER:		return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_LOWER"); break;
	case ITEM_TYPE_GLOVE:		return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_GLOVE"); break;
	case ITEM_TYPE_BELT:		return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_BELT"); break;
	case ITEM_TYPE_HOLSTER:		return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_HOLSTER"); break;
	case ITEM_TYPE_BERET :		return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_BERET"); break;
	}

	return i3::rc_wstring();
}

/************************************************/
// GetShopItemDescription_Weapon
void	GetShopItemDescription_Weapon(i3::rc_wstring& outName, T_ItemID itemID)
{
	CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo( itemID );

	if ( pInfo == nullptr )
	{
		outName = i3::format_string(L"%s (%d)", INVALID_TEXTW, itemID);
	}
	else
	{
		pInfo->GetDescription(outName);	

		if( outName.empty())
			outName = i3::format_string(L"No Description (%d)", itemID);
	}
}

void	GetShopItemDescription_Weapon(i3::rc_wstring& outName, SHOP_GOODS* goods)
{
	GetShopItemDescription_Weapon(outName, goods->_ItemID);
}

/************************************************/
// GetShopItemDescription_Char
void	GetShopItemDescription_Char(i3::rc_wstring& outName, T_ItemID itemID)
{
	g_pCharaInfoDataBase->GetDescription(itemID, outName);
}
void	GetShopItemDescription_Char(i3::rc_wstring& outName, SHOP_GOODS* goods)
{
	GetShopItemDescription_Char(outName, goods->_ItemID);
}

/************************************************/
// GetShopItemDescription_Item
void	GetShopItemDescription_Item(i3::rc_wstring& outName, T_ItemID itemID)
{
	g_pShopItemInfoDataBase->GetItemDescription(itemID, outName);
}

void	GetShopItemDescription_Item(i3::rc_wstring& outName, const SHOP_GOODS* goods)
{
	if ( usf::IsSetItem(goods) )
	{
		INT32 GoodsID = goods->_GoodsID;
		ShopPackageGoodsInfo* pInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(GoodsID);
		if (pInfo == nullptr)
			i3::sprintf(outName, L"%s (%d, %d)", INVALID_TEXTW, goods->_GoodsID, goods->_ItemID); 
		else
			outName = pInfo->GetDescription() ;
	}
	else
	{
		GetShopItemDescription_Item(outName, goods->_ItemID);
	}
}

}	//end of namespace


namespace usf 
{
	UINT8 g_price_idx = PRICE_TYPE_SHOP_REAL;
	void SetPriceIdx(UINT8 price_idx)
	{
		g_price_idx = price_idx;
	}
	/***********************/
	i3::rc_wstring GetShopItemStatusString(ItemStatus status)
	{
		switch (status)
		{
		case ITEM_STATUS_NOHAVE:		return GAME_RCSTRING("STBL_IDX_ITEM_NO_HAVE");
		case ITEM_STATUS_HAVE:			return GAME_RCSTRING("STBL_IDX_ITEM_HAVE");
		case ITEM_STATUS_USED:			return GAME_RCSTRING("STBL_IDX_ITEM_USED");
		case ITEM_STATUS_NOTUSED: return GAME_RCSTRING("STR_INVEN_NOTUSED");
		case ITEM_STATUS_EQUIPPED:		return GAME_RCSTRING("STBL_IDX_ITEM_EQUIPPED");
		default: break;
		}
		return i3::rc_wstring();
	}
	bool IsSetItem(const SHOP_GOODS* pGoods)
	{
		SHOP_GOODS_LIST * pGoodsList = CShop::i()->GetGoodsListData(pGoods->_GoodsID);
		return (pGoodsList && pGoodsList->_ui8ItemCount > 1);
	}

	/***********************/
	//GetShopItemDescription
	void	GetShopItemDescription(i3::rc_wstring& outName, T_ItemID itemID)
	{
		ShopItemCategory cat = usf::GetShopItemCategory(itemID);

		switch (cat)
		{
		case SHOP_ITEM_CATEGORY_WEAPON:		GetShopItemDescription_Weapon(outName, itemID);	break;
		case SHOP_ITEM_CATEGORY_CHARACTER:	GetShopItemDescription_Char(outName, itemID); break;
		case SHOP_ITEM_CATEGORY_ITEM:		GetShopItemDescription_Item(outName, itemID); break;
		default: outName.clear(); break;
		}
	}
	void	GetShopItemDescription(i3::rc_wstring& outName, const SHOP_GOODS* goods)
	{
		ShopItemCategory cat = usf::GetShopItemCategory(goods);

		switch (cat)
		{
		case SHOP_ITEM_CATEGORY_WEAPON:		GetShopItemDescription_Weapon(outName, goods->_ItemID);	break;
		case SHOP_ITEM_CATEGORY_CHARACTER:	GetShopItemDescription_Char(outName, goods->_ItemID); break;
		case SHOP_ITEM_CATEGORY_ITEM:		GetShopItemDescription_Item(outName, goods); break;
		default: outName.clear(); break;
		}
	}
	void	GetShopItemDescription(i3::rc_wstring& outName, const SHOP_GOODS_PACK* pack)
	{
		GetShopItemDescription(outName, pack->GetGoods(0));
	}

	/***********************/
	//GetShopItemName
	void	GetShopItemName(i3::rc_wstring& outName, T_ItemID itemID)
	{
		ShopItemCategory cat = usf::GetShopItemCategory(itemID);

		switch (cat)
		{
		case SHOP_ITEM_CATEGORY_WEAPON:		GetShopItemName_Weapon(outName, itemID);	break;
		case SHOP_ITEM_CATEGORY_CHARACTER:	GetShopItemName_Char(outName, itemID); break;
		case SHOP_ITEM_CATEGORY_ITEM:		GetShopItemName_Item(outName, itemID); break;
		default: outName.clear(); break;
		}

#ifdef I3_DEBUG	//테스트용 코드
		if (GameUI::IsKeyboardPressed(I3I_KEY_LCTRL) || BaTMaN::enable_debug_item_name)
		{
			switch (item_def::get_category(itemID))
			{
			case item_def::CTG_WEAPON:
			case item_def::CTG_ITEM:
			case item_def::CTG_CHARA:
			{
				//아래 주석된 코드가 문제가 있는 코드이기 때문에 이런식으로 표현한다.
				wchar_t buf[MAX_STRING_COUNT];
				i3::snprintf(buf, MAX_STRING_COUNT, L"%s\nId %d, Wdb %d", outName, itemID, CInvenList::i()->FindWareDBIdx_ByItemID(itemID));
				outName = buf;

				//i3::sprintf( outName, "%s\nId %d, Wdb %d", outName, itemID, inven::Get_WareDBIndex(itemID)  ); //현재 시점에서 이 코드는 문제가 있다. [2013년 3월 21일]
			}
			break;
			case item_def::CTG_PARTS:
			case item_def::CTG_SKIN:
			{
				//아래 주석된 코드가 문제가 있는 코드이기 때문에 이런식으로 표현한다.
				wchar_t buf[MAX_STRING_COUNT];
				i3::snprintf(buf, MAX_STRING_COUNT, L"%s\nId %d", outName, itemID);
				outName = buf;

				//i3::sprintf( outName, "%s\nId %d", outName, itemID); //파츠는 ware_db_index가 정확하지 않다. //현재 시점에서 이 코드는 문제가 있다. [2013년 3월 21일]
			}
			break;
			}
		}
#endif
	}

	void	GetShopItemName(i3::rc_wstring& outName, const SHOP_GOODS* goods)
	{
		if (goods == nullptr)
		{
			outName = L"invalid";
			return;
		}

		ShopItemCategory cat = usf::GetShopItemCategory(goods);

		switch (cat)
		{
		case SHOP_ITEM_CATEGORY_WEAPON:		GetShopItemName_Weapon(outName, goods);	break;
		case SHOP_ITEM_CATEGORY_CHARACTER:	GetShopItemName_Char(outName, goods); break;
		case SHOP_ITEM_CATEGORY_ITEM:		GetShopItemName_Item(outName, goods); break;
		default: outName.clear(); break;
		}

#ifdef I3_DEBUG	//테스트용 코드
		if (GameUI::IsKeyboardPressed(I3I_KEY_LCTRL) || BaTMaN::enable_debug_item_name)
		{
			//아래 주석된 코드가 문제가 있는 코드이기 때문에 이런식으로 표현한다.
			wchar_t buf[MAX_STRING_COUNT];
			i3::snprintf(buf, MAX_STRING_COUNT, L"%s\nId %d, gID %d", outName, goods->_ItemID, goods->_GoodsID);
			outName = buf;

			//		i3::sprintf( outName, "%s\nId %d, gID %d", outName, goods->_ItemID, goods->_GoodsID ); //현재 시점에서 이 코드는 문제가 있다. [2013년 3월 21일]
		}
#endif
	}

	void	GetShopItemName(i3::rc_wstring& outName, const SHOP_GOODS_PACK* pack)
	{
		GetShopItemName(outName, pack->GetGoods(0));
	}

	void	GetShopItemName(i3::rc_wstring& outName, ITEM_INFO item_info)
	{
		GetShopItemName(outName, item_info.m_TItemID);
	}

	void	 GetShopItemMultiCharaName(i3::rc_wstring& outName, ITEM_INFO item_info)
	{
		GetMultiSlotName_Char(outName, item_info);
	}


	/***********************/
	//GetItemClassName
	i3::rc_wstring GetItemClassName(T_ItemID itemID)
	{
		switch (item_def::get_category(itemID))
		{
		case item_def::CTG_WEAPON: return GetItemClassName_Weapon(itemID);	break;
		case item_def::CTG_PARTS: return GetItemClassName_Parts(itemID);	break;
		}

		return i3::rc_wstring();
	}

	i3::rc_wstring GetItemUsageName_Only_PrimaryWeapon(T_ItemID itemID)
	{
		if (item_def::get_category(itemID) != item_def::CTG_WEAPON) return i3::rc_wstring();
		if (item_def::get_weapon_type(itemID) != ITEM_TYPE_PRIMARY) return i3::rc_wstring();

		switch (item_def::get_weapon_usage(itemID))
		{
		case WEAPON_CLASS_ASSAULT: return GAME_RCSTRING("STR_SHOP_MENU_WEAPON_ASSAULT"); break;
		case WEAPON_CLASS_SMG:
		case WEAPON_CLASS_DUALSMG: return GAME_RCSTRING("STR_SHOP_MENU_WEAPON_SMG"); break;
		case WEAPON_CLASS_SNIPER: return GAME_RCSTRING("STR_SHOP_MENU_WEAPON_SNIPER"); break;
		case WEAPON_CLASS_MG: return GAME_RCSTRING("STR_SHOP_MENU_WEAPON_MACHINE"); break;
		case WEAPON_CLASS_SHOTGUN:
		case WEAPON_CLASS_DUALSHOTGUN: return GAME_RCSTRING("STR_SHOP_MENU_WEAPON_SHOTGUN"); break;
		}

		return i3::rc_wstring();
	}

	i3::rc_wstring GetItemLineUpName_Only_Parts(item_def::PartsType_ShopPreview line_up)
	{
		switch (line_up)
		{
		case item_def::PartsType_ShopPreview_Headgear: return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_HEADGEAR"); break;
		case item_def::PartsType_ShopPreview_Facegear: return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_FACEGEAR"); break;
		case item_def::PartsType_ShopPreview_Upper: return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_UPPER"); break;
		case item_def::PartsType_ShopPreview_Lower: return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_LOWER"); break;
		case item_def::PartsType_ShopPreview_Glove: return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_GLOVE"); break;
		case item_def::PartsType_ShopPreview_Belt: return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_BELT"); break;
		case item_def::PartsType_ShopPreview_Holster: return GAME_RCSTRING("STBL_IDX_SHOP_FILTER_HOLSTER"); break;
		}
		return i3::rc_wstring();
	}

	i3::rc_wstring GetItemLineUpName_Only_Parts(T_ItemID itemID)
	{
		item_def::PartsType_ShopPreview line_up = item_def::get_shop_preview_parts_type(itemID);
		return GetItemLineUpName_Only_Parts(line_up);
	}

	/***********************/
	//GetShopPeriodCountString
	void	GetShopPeriodCountString(T_ItemID itemID, i3::wstring& outstr)
	{
		UINT8 authType = CShop::i()->GetAuthType(itemID);

		UINT8 type; UINT32 arg;
		inven::GetItemTypeArg(itemID, &type, &arg);

		_GetShopPeriodCountString_NonSetItem(itemID, authType, arg, outstr);
	}

	void	GetShopPeriodCountString(T_ItemID itemID, UINT8 authType, UINT32 authArg, i3::wstring& outStr)
	{
		_GetShopPeriodCountString_NonSetItem(itemID, authType, authArg, outStr);
	}

	void GetShopPeriodCountString(const SHOP_GOODS* pGoods, i3::wstring& outstr)
	{
		if (IsSetItem(pGoods))
			_GetShopPeriodCountString_SetItem(pGoods, outstr);
		else
			_GetShopPeriodCountString_NonSetItem(pGoods->_ItemID, pGoods->_AuthType, pGoods->_AuthArg, outstr);
	}

	void GetShopPeriodCountStringByGoodsID(const T_GoodsID goodsID, i3::wstring& outstr)
	{
		const UINT32 itemID = CShopContext::i()->ConvertGoodsID(goodsID);
		const SHOP_GOODS* pGoods = CShopContext::i()->GetGoods(itemID);
		if (!pGoods) return;

		if (IsSetItem(pGoods))
			_GetShopPeriodCountString_SetItem(const_cast<SHOP_GOODS*>(pGoods), outstr);
		else
		{
			INT32 periodOrCount = CShopContext::i()->FindItemCountOrPeriod(goodsID);
			if (periodOrCount == -1)
				periodOrCount = pGoods->_AuthArg;

			_GetShopPeriodCountString_NonSetItem(pGoods->_ItemID, pGoods->_AuthType, periodOrCount, outstr);
		}
	}

	void GetShopPeriodCountString(SHOP_GOODS_PACK* pack, i3::wstring& outstr, const wchar_t* word)
	{
		INT32 size = pack->GetGoodsCount();
		i3::wstring wstrtemp;
		outstr.clear();

		for (INT32 i = 0; i < size; ++i)
		{
			GetShopPeriodCountString(pack->GetGoods(i), wstrtemp);

			wstrtemp = i3::format_string(L"%s%s", wstrtemp, word);
			outstr += wstrtemp;
		}
	}

	/***********************/
	//GetShopPeriodCountString_only_Item
	void GetShopPeriodCountString_only_Item(T_ItemID itemID, i3::wstring& outName)
	{
		if (item_def::get_category(itemID) != item_def::CTG_ITEM) return;

		ITEM_TYPE itemUsage = item_def::get_item_usage(itemID);
		switch (itemUsage)
		{
		case ITEM_TYPE_WRAP_PERIOD:
		{
			UINT8 authType = CShop::i()->GetAuthType(itemID);

			UINT8 type; UINT32 arg;
			inven::GetItemTypeArg(itemID, &type, &arg);

			UINT32 hour = GetMaintenanceItemHour(itemID, authType, arg);
			i3::sprintf(outName, GAME_RCSTRING("STR_POPUP_USABLE_TERM"), hour / 24);/*%d일권*/
		}
		break;

		case ITEM_TYPE_MAINTENANCE:
		case ITEM_TYPE_WRAP_COUNT:
		case ITEM_TYPE_POINT:
		{
			outName = GAME_RCSTRING("STR_POPUP_DISPOSABLE");/*1회용*/
		}
		break;
		}
	}



	/****************************************/
	//ShopItemCategory
	ShopItemCategory	GetShopItemCategory(T_ItemID itemID)
	{
		return _GetShopItemCategory(itemID);
	}
	ShopItemCategory  GetShopItemCategory(const SHOP_GOODS* goods)
	{
		if (usf::IsSetItem(goods))
			return SHOP_ITEM_CATEGORY_ITEM;

		return _GetShopItemCategory(goods->_ItemID);
	}


	/****************************************/
	//namespace - GetItemStatus
	namespace is	//item_state
	{
		//ITEM_STATUS_NOHAVE : 인벤에 itemID 물품이 없는 경우(인벤/상점 동일)
		//ITEM_STATUS_EQUIPPED : 현재 캐릭터가 작용중인 경우(인벤/상점 동일)
		//ITEM_STATUS_HAVE : 다른 캐릭터가 작용중인 경우(인벤/상점 동일)
		//ITEM_STATUS_NOTUSED : 

		// Function Declaration
		ItemStatus get_weapon_item_status(T_ItemID itemID, bool use_shop);
		ItemStatus get_weapon_item_status(T_ItemID itemID, T_ItemDBIdx ware_idx, bool use_shop);
		ItemStatus get_chara_item_status(T_ItemID itemID, T_ItemDBIdx ware_idx, bool use_shop);
		ItemStatus get_dino_item_status(T_ItemID itemID, T_ItemDBIdx ware_idx, bool use_shop);
		ItemStatus get_parts_item_status(T_ItemID itemID);
		ItemStatus get_parts_item_status(T_ItemID itemID, T_ItemDBIdx ware_idx);
		ItemStatus get_skin_item_status(T_ItemID itemID);
		ItemStatus get_skin_item_status(T_ItemID itemID, T_ItemDBIdx ware_idx);
		ItemStatus get_item_item_status(T_ItemID itemID);

		// Function Definition
		ItemStatus get_weapon_item_status(T_ItemID itemID, bool use_shop)
		{
			//상점 status : no_have, have
			//인벤 status : no_have, equpped, have, not_used
			UINT8 Type = 0; UINT32 Arg = 0;
			if (CInvenList::i()->GetItemTypeByItemID(INVEN::DB_WEAPON, itemID, &Type, &Arg) == false)
				return ITEM_STATUS_NOHAVE;

			if (CharaInfoMgr::instance()->is_chara_equipped(itemID, CCharaInfoContext::i()->GetMainCharaSlotIdx()))
			{
				if (use_shop == false)
					return ITEM_STATUS_EQUIPPED;
				else
					return ITEM_STATUS_HAVE;
			}

			if (use_shop == false)
			{
				UINT8 authType = CShop::i()->GetAuthType(itemID);
				if (ITEM_ATTR_TYPE_BUY == Type && ITEM_TYPE_TIME == authType)
					return ITEM_STATUS_NOTUSED;
			}

			return ITEM_STATUS_HAVE;
		}

		// Function Definition
		ItemStatus get_weapon_item_status(T_ItemID itemID, T_ItemDBIdx ware_idx, bool use_shop)
		{
			//상점 status : no_have, have
			//인벤 status : no_have, equpped, have, not_used
			UINT8 Type = 0; UINT32 Arg = 0;
			if (CInvenList::i()->GetItemTypeByItemID(INVEN::DB_WEAPON, itemID, &Type, &Arg) == false)
				return ITEM_STATUS_NOHAVE;

			if (CharaInfoMgr::instance()->is_chara_equipped(itemID, ware_idx, CCharaInfoContext::i()->GetMainCharaSlotIdx()))
				if (use_shop == false)
					return ITEM_STATUS_EQUIPPED;
				else
					return ITEM_STATUS_HAVE;

			if (use_shop == false)
			{
				UINT8 authType = CShop::i()->GetAuthType(itemID);
				if (ITEM_ATTR_TYPE_BUY == Type && ITEM_TYPE_TIME == authType)
					return ITEM_STATUS_NOTUSED;
			}

			return ITEM_STATUS_HAVE;
		}

		ItemStatus get_chara_item_status(T_ItemID itemID, T_ItemDBIdx ware_idx, bool use_shop)
		{
			//상점 status : have
			//인벤 status : equpped, not_used

			// 캐릭터가 공룡인 경우, 공룡 상태 반환.
			if (item_def::get_type(itemID) == ITEM_TYPE_DINO)
				return is::get_dino_item_status(itemID, ware_idx, use_shop);

			if (use_shop == false) //인벤일 경우
			{
				if (item_def::is_chara_category(itemID) == true) //캐릭터는 무조건 착용중
				{
					if (ware_idx == 0)
					{
						ware_idx = CInvenList::i()->FindWareDBIdx_ByItemID(itemID);
					}

					if (((CCharaInfoContext::i()->GetMainCharaPartsInfo(TEAM_BLUE)->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA) == itemID) &&
						(CCharaInfoContext::i()->GetMainCharaPartsInfo(TEAM_BLUE)->GetPartsIdx(CHAR_EQUIPMENT_PARTS_CHARA) == ware_idx)) ||
						((CCharaInfoContext::i()->GetMainCharaPartsInfo(TEAM_RED)->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA) == itemID) &&
						(CCharaInfoContext::i()->GetMainCharaPartsInfo(TEAM_RED)->GetPartsIdx(CHAR_EQUIPMENT_PARTS_CHARA) == ware_idx)))
						return ITEM_STATUS_EQUIPPED;
					else if (CCharaInfoContext::i()->FindCharaInfoIndexByItemID(itemID) != -1)
					{
						if (g_pEnvSet->IsV2Version() == false)
						{
							UINT8 Type = 0; UINT32 Arg = 0;
							if (CInvenList::i()->GetItemTypeByItemID(INVEN::DB_CHARA, itemID, &Type, &Arg) == false)
								return ITEM_STATUS_NOHAVE;

							if (CharaInfoMgr::instance()->is_chara_equipped(itemID, ware_idx, CCharaInfoContext::i()->GetMainCharaSlotIdx()))
								if (use_shop == false)
									return ITEM_STATUS_EQUIPPED;
								else
									return ITEM_STATUS_HAVE;

							if (use_shop == false)
							{
								UINT8 authType = CShop::i()->GetAuthType(itemID);
								if (ITEM_ATTR_TYPE_BUY == Type && ITEM_TYPE_TIME == authType)
									return ITEM_STATUS_NOTUSED;
							}
						}

						return ITEM_STATUS_HAVE;
					}
				}
			}
			else
			{
				if (CCharaInfoContext::i()->FindCharaInfoIndexByItemID(itemID) != -1) //캐릭터 장착 유무
				{
					return ITEM_STATUS_HAVE;
				}

				if ((CCharaInfoContext::i()->GetMainCharaPartsInfo(TEAM_RED)->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA) == itemID) ||
					(CCharaInfoContext::i()->GetMainCharaPartsInfo(TEAM_BLUE)->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA) == itemID))
					return ITEM_STATUS_HAVE;
			}

			return ITEM_STATUS_NOHAVE;
		}

		ItemStatus get_dino_item_status(T_ItemID itemID, T_ItemDBIdx ware_idx, bool use_shop)
		{
			//상점 status : no_have, have, equpped
			//인벤 status : equpped, not_used, have

			UINT8 Type = 0;
			UINT32 Arg = 0;

			// 유저 인벤에 공룡이 있는지 확인.
			if (CInvenList::i()->GetItemTypeByItemID(INVEN::DB_CASHITEM, itemID, &Type, &Arg))
			{
				// 인벤토리에서 해당 아이템이 미사용 상태이면 NOT_USED 반환
				if (use_shop == false && Type == ITEM_ATTR_TYPE_BUY)
					return ITEM_STATUS_NOTUSED;

				if (CharaInfoMgr::instance()->is_chara_equipped(itemID, CCharaInfoContext::i()->GetMainCharaSlotIdx()))
					return ITEM_STATUS_EQUIPPED;
				else
					return ITEM_STATUS_HAVE;
			}

			return ITEM_STATUS_NOHAVE;
		}

		ItemStatus get_parts_item_status(T_ItemID itemID)
		{
			//상점 status : no_have, have
			UINT8 Type = 0; UINT32 Arg = 0;
			if (CInvenList::i()->GetItemTypeByItemID(INVEN::DB_PARTS, itemID, &Type, &Arg) == false)
				return ITEM_STATUS_NOHAVE;

			return ITEM_STATUS_HAVE;
		}

		ItemStatus get_parts_item_status(T_ItemID itemID, T_ItemDBIdx ware_idx)
		{
			//인벤 status : no_have, equpped, have, not_used
			UINT8 Type = 0; UINT32 Arg = 0;

			const CEquipInfo * pEquip = g_pCharaInfoDataBase->GetInfo(itemID);

			if (pEquip != nullptr && pEquip->IsDefaultEquipment())
			{
				itemID = static_cast<T_ItemID>(MAKE_DEFAULT_PARTS_ITEMID(GET_ITEM_TYPE(itemID)));
				ware_idx = static_cast<T_ItemDBIdx>(MAKE_DEFAULT_EQUIP_DBIDX(GET_ITEM_TYPE(itemID)));
			}

			if (CharaInfoMgr::instance()->is_chara_equipped(itemID, ware_idx, CCharaInfoContext::i()->GetMainCharaSlotIdx()))
				return ITEM_STATUS_EQUIPPED;

			INVEN::DB_TYPE partType = INVEN::DB_PARTS;
			if (g_pEnvSet->IsV2Version() == false)
				partType = INVEN::DB_CHARA;

			if (CInvenList::i()->GetItemTypeByDBIdx(partType, ware_idx, &Type, &Arg) == false)
				return ITEM_STATUS_NOHAVE;

			UINT8 authType = CShop::i()->GetAuthType(itemID);
			if (ITEM_ATTR_TYPE_BUY == Type && ITEM_TYPE_TIME == authType)
				return ITEM_STATUS_NOTUSED;

			return ITEM_STATUS_HAVE;
		}

		ItemStatus get_skin_item_status(T_ItemID itemID)
		{
			//상점 status : no_have, have
			UINT8 Type = 0; UINT32 Arg = 0;
			if (CInvenList::i()->GetItemTypeByItemID(INVEN::DB_SKIN, itemID, &Type, &Arg) == false)
				return ITEM_STATUS_NOHAVE;

			return ITEM_STATUS_HAVE;
		}

		ItemStatus get_skin_item_status(T_ItemID itemID, T_ItemDBIdx ware_idx)
		{
			UINT8 Type = 0; UINT32 Arg = 0;
			if (CInvenList::i()->GetItemTypeByDBIdx(INVEN::DB_SKIN, ware_idx, &Type, &Arg) == false)
				return ITEM_STATUS_NOHAVE;

			if (CharaInfoMgr::instance()->is_chara_equipped(itemID, ware_idx, CCharaInfoContext::i()->GetMainCharaSlotIdx()))
				return ITEM_STATUS_EQUIPPED;

			UINT8 authType = CShop::i()->GetAuthType(itemID);
			if (ITEM_ATTR_TYPE_BUY == Type && ITEM_TYPE_TIME == authType)
				return ITEM_STATUS_NOTUSED;

			return ITEM_STATUS_HAVE;
		}

		ItemStatus get_item_item_status(T_ItemID itemID)
		{
			//인벤 status : no_have, use, not_used
			UINT8 Type = 0; UINT32 Arg = 0;
			if (CInvenList::i()->GetItemTypeByItemID(INVEN::DB_CASHITEM, itemID, &Type, &Arg) == false)
				return ITEM_STATUS_NOHAVE;

			switch (Type)
			{
			case ITEM_ATTR_TYPE_AUTH: return ITEM_STATUS_USED;
			case ITEM_ATTR_TYPE_PC_1:
			case ITEM_ATTR_TYPE_PC_2:
			{
				if (GET_ITEM_TYPE(itemID) == ITEM_TYPE_MAINTENANCE)
					return ITEM_STATUS_USED;
			}
			break;
			}

			return ITEM_STATUS_NOTUSED;
		}
	}

	/****************************************/
	//GetItemStatus
	//인벤용이다.
	ItemStatus	 GetItemStatus(T_ItemID itemID, T_ItemDBIdx ware_idx)
	{
		switch (item_def::get_category(itemID))
		{
		case item_def::CTG_WEAPON: return is::get_weapon_item_status(itemID, ware_idx, false);
		case item_def::CTG_PARTS:	return is::get_parts_item_status(itemID, ware_idx);
		case item_def::CTG_SKIN:	return is::get_skin_item_status(itemID, ware_idx);
		case item_def::CTG_CHARA:	return is::get_chara_item_status(itemID, ware_idx, false);
		case item_def::CTG_ITEM:	return is::get_item_item_status(itemID);
		}

		return ITEM_STATUS_NOHAVE;
	}

	//상점용이다.
	ItemStatus	 GetItemStatus(const SHOP_GOODS_PACK* goods_pack)
	{
		SHOP_GOODS* goods0 = goods_pack->GetGoods(0);
		T_ItemID itemID = goods0->_ItemID;

		switch (item_def::get_category_goodsID(goods0->_GoodsID))
		{
		case item_def::CTG_WEAPON:	return is::get_weapon_item_status(itemID, g_pEnvSet->IsV2Version() ? true : false);
		case item_def::CTG_PARTS:	return is::get_parts_item_status(itemID);
		case item_def::CTG_SKIN:	return is::get_skin_item_status(itemID);
		case item_def::CTG_CHARA:	return is::get_chara_item_status(itemID, 0, g_pEnvSet->IsV2Version() ? true : false);
		case item_def::CTG_ITEM:
		{
			if (usf::IsSetItem(goods0))	return ITEM_STATUS_NOHAVE;			// 세트아이템은 인벤에 존재하지 않는다..

			INT32 type = GET_ITEM_TYPE(itemID);
			UINT32 item_day_term = 0;
			UINT8 itemType = 0;		UINT32 itemArg = 0;
			switch (type)
			{
			case ITEM_TYPE_WRAP_COUNT:
			case ITEM_TYPE_MAINTENANCE:
			{
				// 지속성 일경우 소유한 같은 계열 아이템의 날짜를 합한다. (보유기간 합 = (1일 * 1일권 갯수) + (7일 * 7일권 갯수) + (30일 * 30일권 갯수))
				for (INT32 i = 0; i < goods_pack->GetGoodsCount(); i++)
				{
					SHOP_GOODS * pGoods = goods_pack->GetGoods(i);

					if (CInvenList::i()->GetItemTypeByItemID(INVEN::DB_CASHITEM, pGoods->_ItemID, &itemType, &itemArg))
					{
						INT32 Day = GET_ITEM_NUMBER(pGoods->_ItemID);
						//	갯수에 따른 사용기간 누적
						item_day_term += Day * itemArg;
					}
				}

				INT32 ItemKeyID = GetCashItemByGroup(goods0->_ItemID);
				if (ItemKeyID != 0)
				{
					if (CInvenList::i()->GetItemTypeByItemID(INVEN::DB_CASHITEM, ItemKeyID, &itemType, &itemArg))
					{
						//사용중인 아이템을 OWNED로 표시하기로 함(PBBUG-795 문서(20161209) 참고)
						//if (itemType == ITEM_ATTR_TYPE_AUTH)	return ITEM_STATUS_USED;

						return ITEM_STATUS_HAVE;
					}
				}

				if (item_day_term > 0) return ITEM_STATUS_HAVE;
				return ITEM_STATUS_NOHAVE;
			}
			break;

			case ITEM_TYPE_POINT:
			case ITEM_TYPE_WRAP_PERIOD:
			{
				//사용중인 아이템 검사
				INT32 ItemKeyID = GetCashItemByGroup(goods0->_ItemID);
				if (ItemKeyID != 0)
				{
					CInvenList::i()->GetItemTypeByItemID(INVEN::DB_CASHITEM, ItemKeyID, &itemType, &itemArg);
					if (itemArg > 0)
					{
						return ITEM_STATUS_HAVE;
					}
				}

				//소지하고 있는 아이템 검사
				if (!CInvenList::i()->GetItemTypeByItemID(INVEN::DB_CASHITEM, itemID, &itemType, &itemArg))
					return ITEM_STATUS_NOHAVE;

				return ITEM_STATUS_HAVE;
			}
			break;
			}
		}
		break;
		}

		return ITEM_STATUS_NOHAVE;
	}


	/****************************************/
	//IsExtensionItem
	bool IsExtensionItem(T_ItemID itemID)
	{
		if (usf::GetShopItemCategory(itemID) != SHOP_ITEM_CATEGORY_WEAPON) return false;

		CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo(itemID);		// 이것이 널값일수 있다..
		if (pInfo && g_pWeaponInfoDataBase->IsExtensionWeapon(pInfo))
			return true;

		return false;
	}
	bool IsExtensionItem(const SHOP_GOODS* pGoods)
	{
		return IsExtensionItem(pGoods->_ItemID);
	}

	/****************************************/
	//IsExtensionItem
	bool HasExtensionDescImage(T_ItemID itemID)
	{
		if (usf::GetShopItemCategory(itemID) != SHOP_ITEM_CATEGORY_WEAPON) return false;

		CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo(itemID);		// 이것이 널값일수 있다..
		if (pInfo && pInfo->isDescriptionImage())
			return true;

		return false;
	}
	bool HasExtensionDescImage(SHOP_GOODS* pGoods)
	{
		// 세트아이템은 상세정보 표시 안함.
		if (IsSetItem(pGoods) == false)
			return HasExtensionDescImage(pGoods->_ItemID);

		return false;
	}

	/****************************************/
	//GetItemItemCategory
	UIShop_Item_Category GetItemItemCategory(SHOP_GOODS* goods)
	{
		if (IsSetItem(goods))
		{
			ShopPackageGoodsInfo* pInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(goods->_GoodsID);
			return (pInfo) ? UIShop_Item_Category(pInfo->GetCatagory()) : UISHOP_ITEM_INVALID;
		}

		ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(goods->_ItemID);
		return (pInfo) ? UIShop_Item_Category(pInfo->GetCatagory()) : UISHOP_ITEM_INVALID;
	}

	UIShop_Item_Category GetItemItemCategory(T_ItemID itemID)
	{
		ShopItemInfo* pInfo = g_pShopItemInfoDataBase->getInfo(itemID);
		return (pInfo) ? UIShop_Item_Category(pInfo->GetCatagory()) : UISHOP_ITEM_INVALID;
	}



	/***********************/
	void	FormatHaveItemSecTermText(UINT32 SecTerm, i3::wstring& outDay)
	{
		//기존 공식이 이상해서 바꿈...
		//기존은 날짜갖고 하는데 그러면 1시간이랑 1일이랑 구분이 가질 않음...
		UINT32 HourTerm = ConvertSecToUseHour(SecTerm);

		if (HourTerm >= 24)//24시간이 넘으면 1일
		{
			UINT32 DayTerm = ConvertSecToUseDay(SecTerm);//초를 1일단위로 변환.
			i3::sprintf(outDay, L"{col:255,255,255,255}%d%s{/col}", DayTerm, GAME_RCSTRING("STBL_IDX_ITEM_TERM_UNIT"));	//	%d일
		}
		else if (HourTerm == 0)	// 날짜수 컨버팅이 올림처리이기 때문에 0초가 아니라면 이 조건은 오지 않음...
			i3::sprintf(outDay, L"{col:210,0,0,255}0%s{/col}", GAME_RCSTRING("STR_UI_HOUR"));			// 0시간..
		else
		{
			if (HourTerm > 1)
				i3::sprintf(outDay, L"{col:255,255,255,255}%d%s{/col}", HourTerm, GAME_RCSTRING("STR_UI_HOUR"));
			else
				i3::sprintf(outDay, L"{col:210,0,0,255}1%s %s{/col}", GAME_RCSTRING("STR_UI_HOUR"), GAME_RCSTRING("STBL_IDX_ITEM_LESS_THEN"));
		}
	}

	namespace inner
	{
		void GetGoodsUseDay_include_Remain(ItemStatus status, bool IsCountItem, UINT8 Type, UINT32 Arg,
			i3::wstring& outStatus, bool includeState)
		{
			if (includeState)
				i3::sprintf(outStatus, L"(%s)", GetShopItemStatusString(status));

			i3::wstring wstrTermCount;

			switch (Type)
			{
			case ITEM_ATTR_TYPE_AUTH:		//인증한 아이템
			{
				if (!IsCountItem)
				{
					// 기간제 아이템이면.
					UINT32 sec_term = 0;
					if (Arg > 0)
					{
						sec_term = UserInfoContext::i()->GetInventorySecTime(Arg);
						sec_term -= ITEM_TERM_ADJUST_VALUE;		//	김성백 실장님 부탁으로 코드 추가합니다. 기간 보정 - komet
						if (0 > (INT32)sec_term) sec_term = 0;
					}

					FormatHaveItemSecTermText(sec_term, wstrTermCount);

					outStatus = i3::format_string(L"%s %s: %s", outStatus, GAME_RCSTRING("STR_SHOP_REMAIN_DAY"), wstrTermCount);
				}
				else
				{
					// 개수제라면
					outStatus = i3::format_string(L"%s %s: %d%s", outStatus, GAME_RCSTRING("STR_SHOP_REMAIN_COUNT"), Arg, GAME_RCSTRING("STBL_IDX_ITEM_COUNT_UNIT"));
				}
			}
			break;
			case ITEM_ATTR_TYPE_BUY:		//구입만한 아이템(기간제)
			{
				if (!IsCountItem)
				{
					//기간제
					UINT32 term = ConvertSecToUseHour(Arg);
					if (term < 24)
					{
						//	보유중 : %d시간
						outStatus = i3::format_string(L"%s %s: %d%s", outStatus, GAME_RCSTRING("STR_SHOP_REMAIN_DAY"), term, GAME_RCSTRING("STR_UI_HOUR"));
					}
					else
					{
						term = term / 24;
						//	보유중 : %d일
						outStatus = i3::format_string(L"%s %s: %d%s", outStatus, GAME_RCSTRING("STR_SHOP_REMAIN_DAY"), term, GAME_RCSTRING("STBL_IDX_ITEM_TERM_UNIT"));
					}
				}
				else
				{
					// 개수제라면
					outStatus = i3::format_string(L"%s %s: %d%s", outStatus, GAME_RCSTRING("STR_SHOP_REMAIN_COUNT"), Arg, GAME_RCSTRING("STBL_IDX_ITEM_COUNT_UNIT"));
				}
			}
			break;
			default:
			{
#if defined (ENABLE_INVEN_DISPLAY_OBT)
				//	보유중 : 무제한
				outStatus = i3::format_string(L"%s %s: %s", outStatus, GAME_RCSTRING("STR_SHOP_REMAIN_DAY"), GAME_RCSTRING("STBL_IDX_ITEM_UNLIMITED"));
#else
				outStatus.clear();
#endif
			}
			break;
			}
		}

		void GetGoodsUseDay_non_Remain(ItemStatus status, bool IsCountItem, UINT8 Type, UINT32 Arg,
			i3::wstring& outStatus, bool includeState)
		{
			if (includeState)
				i3::sprintf(outStatus, L"(%s)", GetShopItemStatusString(status));

			i3::wstring wstrTermCount;

			switch (Type)
			{
			case ITEM_ATTR_TYPE_AUTH:		//인증한 아이템
			{
				if (!IsCountItem)
				{
					// 기간제 아이템이면.
					UINT32 sec_term = 0;
					if (Arg > 0)
					{
						sec_term = UserInfoContext::i()->GetInventorySecTime(Arg);
						sec_term -= ITEM_TERM_ADJUST_VALUE;		//	김성백 실장님 부탁으로 코드 추가합니다. 기간 보정 - komet
						if (0 > (INT32)sec_term) sec_term = 0;
					}

					FormatHaveItemSecTermText(sec_term, wstrTermCount);

					outStatus = i3::format_string(L"%s %s", outStatus, wstrTermCount);
				}
				else
				{
					// 개수제라면
					outStatus = i3::format_string(L"%s %d%s", outStatus, Arg, GAME_RCSTRING("STBL_IDX_ITEM_COUNT_UNIT"));
				}
			}
			break;
			case ITEM_ATTR_TYPE_BUY:		//구입만한 아이템(기간제)
			{
				if (!IsCountItem)
				{
					//기간제
					UINT32 term = ConvertSecToUseHour(Arg);
					if (term < 24)
					{
						//	보유중 : %d시간
						outStatus = i3::format_string(L"%s %d%s", outStatus, term, GAME_RCSTRING("STR_UI_HOUR"));
					}
					else
					{
						term = term / 24;
						//	보유중 : %d일
						outStatus = i3::format_string(L"%s %d%s", outStatus, term, GAME_RCSTRING("STBL_IDX_ITEM_TERM_UNIT"));
					}
				}
				else
				{
					// 개수제라면
					outStatus = i3::format_string(L"%s %d%s", outStatus, Arg, GAME_RCSTRING("STBL_IDX_ITEM_COUNT_UNIT"));
				}
			}
			break;
			default:
			{
#if defined (ENABLE_INVEN_DISPLAY_OBT)
				//	보유중 : 무제한
				outStatus = i3::format_string(L"%s %s", outStatus, GAME_RCSTRING("STBL_IDX_ITEM_UNLIMITED"));
#else
				outStatus.clear();
#endif
			}
			break;
			}
		}
	} //end of namespace::inner


	void GetGoodsUseDay(ItemStatus status, bool IsRepairItem, bool IsCountItem, UINT8 Type, UINT32 Arg,
		i3::wstring& outStatus, bool includeState)
	{
		if (IsRepairItem)
		{
			outStatus = i3::format_string(L"%s %s", outStatus, GAME_RCSTRING("STR_SHOP_DURABILITY_ITEM"));
			return;
		}

		if (status == ITEM_STATUS_NOTUSED)
			inner::GetGoodsUseDay_non_Remain(status, IsCountItem, Type, Arg, outStatus, includeState);
		else
			inner::GetGoodsUseDay_include_Remain(status, IsCountItem, Type, Arg, outStatus, includeState);
	}


	void GetItemUseDay(ItemStatus status, bool IsRepairItem, T_ItemID itemID, T_ItemDBIdx ware_idx, i3::wstring& outStatus, bool includeState)
	{
		if (IsRepairItem) return;

		i3::stack_wstring wstr_not_used;

		if (includeState == true)
		{
			if (status == ITEM_STATUS_NOTUSED)
				i3::sprintf(wstr_not_used, L"(%s)", GetShopItemStatusString(status));
		}

		UINT8 type; UINT32 arg;
		inven::GetItemTypeArg(itemID, ware_idx, &type, &arg);

		UINT8 authType = CShop::i()->GetAuthType(itemID);
		i3::wstring wstr_count;

		GetGoodsUseDay(status, IsRepairItem, (authType == AUTH_COUNT) ? true : false, type, arg, wstr_count, false);

		i3::sprintf(outStatus, L"%s%s", wstr_not_used, wstr_count);

	}
	/***********************/
	// 미보유인 경우 , 0개 혹은 0일을 표시하도록 처리..
	void	GetGoodsUseDay_Zero(bool IsRepairItem, bool IsCountItem, i3::stack_wstring& outStatus)
	{
		// 미보유인 경우 전체를 노란색으로 입힘...
		if (IsRepairItem)			// 수리아이템은 내구도 아이템으로 표시..
		{
			i3::sprintf(outStatus, L"({col:255,255,0,255}%s{/col}) {col:255,255,0,255}%s{/col}",
				GAME_RCSTRING("STBL_IDX_ITEM_NO_HAVE"), GAME_RCSTRING("STR_SHOP_DURABILITY_ITEM"));
			return;
		}

		if (IsCountItem != true)
		{
			i3::sprintf(outStatus, L"({col:255,255,0,255}%s{/col}) %s : {col:255,255,0,255}%d%s{/col}",
				GAME_RCSTRING("STBL_IDX_ITEM_NO_HAVE"), GAME_RCSTRING("STR_SHOP_REMAIN_DAY"), 0, GAME_RCSTRING("STBL_IDX_ITEM_TERM_UNIT"));
		}
		else
		{
			i3::sprintf(outStatus, L"({col:255,255,0,255}%s{/col}) %s : {col:255,255,0,255}%d%s{/col}",
				GAME_RCSTRING("STBL_IDX_ITEM_NO_HAVE"), GAME_RCSTRING("STR_SHOP_REMAIN_COUNT"), 0, GAME_RCSTRING("STBL_IDX_ITEM_COUNT_UNIT"));
		}
	}

	/***********************/
	i3::rc_wstring GetCharacterItemCategoryString(INT32 characID)
	{
		switch (CHARA::ItemID2Type(characID))
		{
		case ITEM_TYPE_CHARA: return GAME_RCSTRING("STR_SHOP_CHARACTER_MAINITEM");
		case ITEM_TYPE_DINO: return GAME_RCSTRING("STR_SHOP_CHARACTER_DINO");
		case ITEM_TYPE_HEADGEAR: return GAME_RCSTRING("STR_SHOP_CHARACTER_HEAD");
		case ITEM_TYPE_FACEGEAR: return GAME_RCSTRING("STR_SHOP_CHARACTER_MASK");
		case ITEM_TYPE_BERET: return GAME_RCSTRING("STR_SHOP_CHARACTER_BERET");
			//case CHARACTER_CLASS_EVENT: return GAME_RCSTRING("STR_SHOP_CHARACTER_EVENT");
			//case CHARACTER_CLASS_ITEM: return GAME_RCSTRING("STR_SHOP_CHARACTER_BERET");
		default: break;
		}
		return GAME_RCSTRING("STR_SHOP_UNKNOWNITEM");
	}

	void	GetItemItemCategoryString(i3::wstring& out_wstr, UIShop_Item_Category cat, bool is_set_item)
	{
		i3::rc_wstring wstr_cat1;
		switch (cat)
		{
		case UISHOP_ITEM_ENHANCED: wstr_cat1 = GAME_RCSTRING("STR_SHOP_MENU_ITEM_ENHANCED"); break;
		case UISHOP_ITEM_FUNCTIONAL: wstr_cat1 = GAME_RCSTRING("STR_SHOP_MENU_ITEM_FUNCTIONAL"); break;
		case UISHOP_ITEM_COMPENSATION: wstr_cat1 = GAME_RCSTRING("STR_SHOP_MENU_ITEM_COMPENSATION"); break;
		case UISHOP_ITEM_BOX: wstr_cat1 = GAME_RCSTRING("STR_SHOP_MENU_ITEM_BOX"); break;
		default: break;
		}

		if (is_set_item)
			i3::sprintf(out_wstr, L"%s (%s)", wstr_cat1, GAME_RCSTRING("STR_SHOP_MENU_ITEM_SET"));
		else
			out_wstr = wstr_cat1;
	}

	void GetShopGoodInfo(const SHOP_GOODS* pGoods, GOODS_PRICE_INFO & info)
	{
		i3mem::FillZero(&info, sizeof(GOODS_PRICE_INFO));

		CShop * shop = CShop::i();
		shop->GetGoodsPrice(pGoods->_GoodsID, info, usf::g_price_idx);
	}

	bool GetShopPriceString(SHOP_GOODS* pGoods, i3::wstring& outResult, GOODS_PRICE_TYPE t)
	{
		GOODS_PRICE_INFO info;
		GetShopGoodInfo(pGoods, info);

		if (t == GOODS_PRICE_ALL)
		{
			if (info._Type & GOODS_PRICE_POINT)	info._Type = GOODS_PRICE_POINT;
		}
		else
		{
			if (info._Type & t)		info._Type = t;
			else					info._Type = GOODS_PRICE_UNKNOWN;
		}

		if (info._Type == GOODS_PRICE_CASH)
		{
			i3::sprintf(outResult, L"%d %s", info._Cash, GAME_RCSTRING("STR_SHOP_CASH"));
			return true;
		}
		else
		{
			if (info._Type == GOODS_PRICE_POINT)
			{
				i3::sprintf(outResult, L"%d %s", info._Point, GAME_RCSTRING("STBL_IDX_SHOP_MONEY"));
				return true;
			}
		}

		return false;
	}
	bool GetShopPriceStringComma(SHOP_GOODS* pGoods, i3::wstring& inoutwstrResult, GOODS_PRICE_TYPE t)
	{
		GOODS_PRICE_INFO info;
		GetShopGoodInfo(pGoods, info);

		if (info.CheckPrice() == false)
			return false;

		if (t == GOODS_PRICE_ALL)
		{
			if (info._Type & GOODS_PRICE_POINT)	info._Type = GOODS_PRICE_POINT;
		}
		else
		{
			if (info._Type & t)	info._Type = t;
			else info._Type = GOODS_PRICE_UNKNOWN;
		}

		i3::wstring wstr;
		if (info._Type == GOODS_PRICE_CASH)
		{
			i3_locale::itoa_comma(info._Cash, wstr);

			wstr.append(GAME_RCSTRING("STR_SHOP_CASH"));
			inoutwstrResult = wstr;
			return true;
		}
		else if (info._Type == GOODS_PRICE_POINT)
		{
			i3_locale::itoa_comma(info._Point, wstr);

			wstr.append(GAME_RCSTRING("STBL_IDX_SHOP_MONEY"));
			inoutwstrResult = wstr;
			return true;
		}

		return false;
	}

	void	GetShopPriceAndMyMoney(SHOP_GOODS* pGoods, UINT32& price, UINT32& mymoney, GOODS_PRICE_TYPE t)
	{
		price = 0;	mymoney = 0;

		GOODS_PRICE_INFO info;
		GetShopGoodInfo(pGoods, info);

		if (t == GOODS_PRICE_ALL)
		{
			if (info._Type & GOODS_PRICE_POINT) info._Type = GOODS_PRICE_POINT;
		}
		else
		{
			if (info._Type & t) info._Type = t;
			else info._Type = GOODS_PRICE_UNKNOWN;
		}

		if (info._Type == GOODS_PRICE_CASH)
		{
			price = info._Cash;
			USER_INFO_BASIC basic;
			UserInfoContext::i()->GetMyUserInfoBasic(&basic);
			mymoney = basic.m_ui32Cash;
		}
		else if (info._Type == GOODS_PRICE_POINT)
		{
			price = info._Point;
			USER_INFO_BASIC basic;
			UserInfoContext::i()->GetMyUserInfoBasic(&basic);

			mymoney = basic.m_ui32Point;
		}
	}


	bool	CheckGoodsPackBothPrices(SHOP_GOODS_PACK* pack)
	{
		GOODS_PRICE_TYPE t = pack->GetPriceTypeAll(usf::g_price_idx);
		return ((t & GOODS_PRICE_CASH) && (t & GOODS_PRICE_POINT));
	}

	void	CalcGoodsPackMapByPriceType(SHOP_GOODS_PACK* pack, INT32& out_num, INT32(&out_array)[4], GOODS_PRICE_TYPE t)
	{
		const INT32 num_goods = pack->GetGoodsCount();
		out_num = 0;
		for (INT32 i = 0; i < num_goods; ++i)
		{
			SHOP_GOODS* goods = pack->GetGoods(i);

			if (goods->_SaleType != GOODS_SALE_ALL && goods->_SaleType != GOODS_SALE_BUY)
				continue;

			if (goods->GetPriceType(usf::g_price_idx) & t)
			{
				out_array[out_num] = i;
				++out_num;
			}
		}
	}

	bool	CalcGoodsPackFirstIdxByPriceType(SHOP_GOODS_PACK* pack, INT32& out_idx, GOODS_PRICE_TYPE t)
	{
		const INT32 num_goods = pack->GetGoodsCount();
		out_idx = -1;
		for (INT32 i = 0; i < num_goods; ++i)
		{
			SHOP_GOODS* goods = pack->GetGoods(i);
			if (goods->GetPriceType(usf::g_price_idx) & t)
			{
				out_idx = i;
				return true;
			}
		}
		return false;
	}


	INT32		GetGoodsCount(SHOP_GOODS_PACK* pack)
	{
		return pack->GetGoodsCount();
	}

	SHOP_GOODS*	GetGoodsFromPack(SHOP_GOODS_PACK* pack, INT32 goods_idx)
	{
		return pack->GetGoods(goods_idx);
	}



	/*
	void GetWeaponSpecificValues( T_ItemID itemID,
		INT32& Damage, INT32& Rapid, INT32& Accuracy, INT32& Recoil_V, INT32& Recoil_H, INT32& Speed,
		char* strRange, const size_t sizeRangeMax, char* strBulletCount, const size_t sizeBulletCountMax )
	{
		GameUI::GetNormalizedWeaponInfo( itemID,
			Damage, Rapid, Accuracy, Recoil_V, Recoil_H, Speed,
			strRange, sizeRangeMax, strBulletCount, sizeBulletCountMax);
	}

	void GetWeaponSpecificValues( SHOP_GOODS_PACK* pack,
		INT32& Damage, INT32& Rapid, INT32& Accuracy, INT32& Recoil_V, INT32& Recoil_H, INT32& Speed,
		char* strRange, const size_t sizeRangeMax, char* strBulletCount, const size_t sizeBulletCountMax )
	{
		GetWeaponSpecificValues( pack->GetGoods(0)->_ItemID,Damage, Rapid, Accuracy,
			Recoil_V, Recoil_H, Speed, strRange, sizeRangeMax, strBulletCount, sizeBulletCountMax );
	}
	*/


	/****************************************/
	//SetShopPointInfo

	// 현재 경고에러발생하고 제대로 기능하지 않을듯 하여 주석처리...지우기엔 참고용으로 쓰일듯하여 주석만 칩니다. (2015.09.07.수빈)
	/*
	void SetShopPointInfo(SHOP_GOODS * pGoods, GOODS_PRICE_TYPE price_type,
						  i3::rc_wstring& out_wstrSubjectPointCash,
						  char* szHavePointCash, size_t sizeHavePointCash,
						  char* szBuyPointCash, size_t sizeBuyPointCash,
						  char* szRemainPointCash, size_t sizeRemainPointCash,
						  const char* szMoneySuffix ,
						  const INT32 nCouponIdx)
	{
		if(pGoods == nullptr) return;
		if ( !( price_type & pGoods->GetPriceType(usf::g_price_idx) ) ) return;

		if ( price_type == GOODS_PRICE_ALL )
		{
			price_type = pGoods->GetPriceType(usf::g_price_idx);
		}

		switch( price_type)
		{
		case GOODS_PRICE_CASH:	//	캐시
			{
				out_wstrSubjectPointCash = GAME_RCSTRING("STR_TBL_GUI_ETC_MENUBAR_MONEY");
				INT32 haveCash  = UserInfoContext::i()->GetMyCash();
				sprintf_s(szHavePointCash, sizeHavePointCash, "%d %s%s", haveCash,  GAME_STRING("STR_SHOP_CASH"), szMoneySuffix);

				INT32 nPriceCash = pGoods->GetPriceCash(usf::g_price_idx);

	#if defined(USE_SALE_COUPON)

				INT32 nDiscountCash = 0;
				if(nCouponIdx >= 0)
				{
					INT32 nCouponID = CInvenList::i()->GetItemID( INVEN::DB_COUPON, nCouponIdx);

					SALE_COUPON * pCoupon = CShop::i()->GetCouponData(nCouponID);

					if(pCoupon->m_ui8SaleType == SALE_TYPE_PER)
						nDiscountCash = static_cast<INT32>(pGoods->GetPriceCash(usf::g_price_idx) * (pCoupon->m_ui32DisCount * 0.01));
					else
						nDiscountCash = pCoupon->m_ui32DisCount;

					nPriceCash -= nDiscountCash;
					sprintf_s(szBuyPointCash, sizeBuyPointCash, "(-%d %s) %d %s%s", nDiscountCash, GAME_STRING("STR_SHOP_CASH"), nPriceCash , GAME_STRING("STR_SHOP_CASH"), szMoneySuffix );

				}
				else
	#endif
				{
					sprintf_s(szBuyPointCash, sizeBuyPointCash, "%d %s%s",  nPriceCash ,  GAME_STRING("STR_SHOP_CASH"), szMoneySuffix );
				}

				INT32 remain = haveCash - pGoods->GetPriceCash(usf::g_price_idx);

				if (remain >= 0)
					sprintf_s(szRemainPointCash, sizeRemainPointCash, "%d %s%s", haveCash - nPriceCash, GAME_STRING("STR_SHOP_CASH"), szMoneySuffix );
				else
					sprintf_s(szRemainPointCash, sizeRemainPointCash, "{col:255,0,0,255}%d %s%s{/col}", haveCash - nPriceCash, GAME_STRING("STR_SHOP_CASH"), szMoneySuffix );
			}
			break;

		case GOODS_PRICE_POINT:	//	게임 머니
		default:
			{
				out_wstrSubjectPointCash = GAME_RCSTRING("STR_TBL_GUI_ETC_MENUBAR_POINT");
				INT32 havePoint = UserInfoContext::i()->GetMyPoint();
				sprintf_s(szHavePointCash, sizeHavePointCash, "%d %s%s", havePoint,  GAME_STRING("STBL_IDX_SHOP_MONEY"), szMoneySuffix);
				sprintf_s(szBuyPointCash, sizeBuyPointCash, "%d %s%s", pGoods->GetPricePoint(usf::g_price_idx), GAME_STRING("STBL_IDX_SHOP_MONEY"), szMoneySuffix);
				INT32 remain = havePoint - pGoods->GetPricePoint(usf::g_price_idx);
				if (remain >= 0)
					sprintf_s(szRemainPointCash, sizeRemainPointCash, "%d %s%s", havePoint - pGoods->GetPricePoint(usf::g_price_idx), GAME_STRING("STBL_IDX_SHOP_MONEY"), szMoneySuffix);
				else
					sprintf_s(szRemainPointCash, sizeRemainPointCash, "{col:255,0,0,255}%d %s%s{/col}", havePoint - pGoods->GetPricePoint(usf::g_price_idx), GAME_STRING("STBL_IDX_SHOP_MONEY"), szMoneySuffix);
			}
			break;
		}
	}
	*/

	bool GetDurability(T_ItemID itemID, T_ItemDBIdx ware_idx, REAL32& rate)
	{
		bool durable = CShopContext::i()->IsDurabilityItem(itemID);
		if (durable)
		{
			durable = CShopContext::i()->GetDurability(itemID, ware_idx, rate);
		}

		return durable;
	}

	bool CheckValidGoods_in_Pef(T_ItemID itemID)
	{
		switch (GetShopItemCategory(itemID))
		{
		case SHOP_ITEM_CATEGORY_WEAPON:
		{
			CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo(itemID);
			if (pInfo == nullptr)
			{
				I3TRACE("CheckValidGoods_in_Pef::Unknown Weapon %d\n", itemID);
				return false;
			}
		}
		break;
		case SHOP_ITEM_CATEGORY_CHARACTER:
		{
			const CEquipInfo * pInfo = g_pCharaInfoDataBase->GetInfo(itemID);
			if (pInfo == nullptr)
			{
				I3TRACE("CheckValidGoods_in_Pef::Unknown Character %d\n", itemID);
				return false;
			}
		}
		break;

		case SHOP_ITEM_CATEGORY_ITEM:
		{
			//아이템은 세트/일반으로 구분된다.
			ShopPackageGoodsInfo* pPackInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(itemID);
			if (pPackInfo == nullptr)
			{
				ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(itemID);
				if (pInfo == nullptr)
				{
					I3TRACE("CheckValidGoods_in_Pef::Unknown Item %d\n", itemID);
					return false;
				}
			}
		}
		break;
		case NUM_SHOP_ITEM_CATEGORY:
			return false;
			break;
		}

		return true;
	}

	bool CheckValidGoods_in_Pef(const SHOP_GOODS_PACK* pack)
	{
		if (pack->GetGoodsCount() < 1)
			return false;

		SHOP_GOODS* pGoods = pack->GetGoods(0);

		// 세트 아이템일 경우 - 셋트 아이템 제대로 Goods.pef에 등록 안되어있는 인도네시아 라이브 때문에 셋트 아이템 Vaild 체크는 예외처리.
		//usf::IsSetItem 로 판단하기 보다 CheckValidGoods 이 함수로 판단하는게 더 좋을 듯 하다. 
		//if (usf::IsSetItem(pGoods))	return true;  //일단 주석 처리한다.

		return CheckValidGoods_in_Pef(pGoods->_ItemID);
	}

	bool CheckBuyItemPeriod(const SHOP_GOODS_PACK* pack, UINT32 buySlot)
	{
		if (pack->GetGoodsCount() < 1)						// 상품갯수가 0일 경우 리턴 false
		{
			I3TRACE(" Pack->GetGoodsCount < 1");
			return false;
		}

		if (buySlot < 0 || buySlot >= (UINT32)pack->GetGoodsCount())	// 상품 구매슬롯 값이 0보다작거나 상품갯수보다 클경우 리턴 false
		{
			I3TRACE(" buySlot <= Pack->GetGoodsCount");
			return false;
		}

		SHOP_GOODS* pGoods = pack->GetGoods(buySlot);

		UINT8 GoodsItemType = pGoods->_AuthType;
		T_ItemID GoodsItemID = pGoods->_ItemID;
		UINT32 GoodsItemArg = pGoods->_AuthArg;

		//셋트아이템인지 체크 셋트 아이템이면 return true (구매가능)
		if ( g_pShopItemInfoDataBase->CheckPackageGoodsGroupInfo(pGoods->_GoodsID) )
		{
			return true;
		}

		INVEN::DB_TYPE dbType = INVEN::ItemID2DBType(GoodsItemID);
		
		UINT8 Type = 0; UINT32 Arg = 0;
		if (false == CInvenList::i()->GetItemTypeByItemID(dbType, GoodsItemID, &Type, &Arg))
		{
			//인벤토리에 해당 상품이 없음
			return true;
		}

		INT32 GoodsTerm = 0;			//사는 아이템 기간
		INT32 InvenItemTerm = 0;		//소지한 아이템 기간
		INT32 MAX_Term = 0;				//최대 기간 42년12월31일23시59분

		switch (Type)
		{
		case ITEM_ATTR_TYPE_AUTH:	//인증한 아이템
		{
			if (GoodsItemType == ITEM_TYPE_COUNT)
			{
				if (GoodsItemArg + Arg > LIMIT_ITEM_COUNT)
				{
					return false;
				}
			}
			else if (GoodsItemType == ITEM_TYPE_TIME)
			{
				//인증한 아이템은 기간이 00년00월00일00시간00분 으로나타남.
				MAX_Term = UserInfoContext::i()->GetInventorySecTime(LIMIT_ITEM_DATE);
				InvenItemTerm = UserInfoContext::i()->GetInventorySecTime(Arg);

				GoodsTerm = ConvertSecToUseHour(GoodsItemArg);
				InvenItemTerm = ConvertSecToUseHour(InvenItemTerm);
				MAX_Term = ConvertSecToUseHour((UINT32)MAX_Term);
				if (GoodsTerm + InvenItemTerm > MAX_Term)
				{
					return false;
				}
			}
		}
			break;

		case ITEM_ATTR_TYPE_BUY:	//미인증한 아이템
		{
			if (GoodsItemType == ITEM_TYPE_COUNT)
			{
				if (GoodsItemArg + Arg > LIMIT_ITEM_COUNT)
				{
					return false;
				}
			}
			else if (GoodsItemType == ITEM_TYPE_TIME)
			{
				MAX_Term = UserInfoContext::i()->GetInventorySecTime(LIMIT_ITEM_DATE);

				GoodsTerm = ConvertSecToUseHour(GoodsItemArg);
				InvenItemTerm = ConvertSecToUseHour(Arg);
				MAX_Term = ConvertSecToUseHour((UINT32)MAX_Term);
				if (GoodsTerm + InvenItemTerm > MAX_Term)
				{
					return false;
				}
			}
		}
			break;
		default:
			break;
		}
	

		return true;
	}




	//- buy --------------//
	bool CheckValidGoods_InCart(const SHOP_GOODS_PACK * pack)
	{
		if (usf::CheckValidGoods_in_Pef(pack) == false)
		{
			i3::rc_wstring wstrCaption = L"out of work";
			GameUI::MsgBox(MSG_TYPE_ERROR, L"Invalid Item", &wstrCaption);
			return false;
		}

		usf::ValidBuyType result = usf::IsValidBuyGoods_InCart(pack);

		if (result != usf::vbt_Ok)
		{
			usf::Message_BuyResert(result);
			return false;
		}

		return true;
	}

	bool CheckValidGoods(const SHOP_GOODS_PACK * pack, bool chat_msg)
	{
		if (usf::CheckValidGoods_in_Pef(pack) == false)
		{
			i3::rc_wstring wstrCaption = L"out of work";
			GameUI::MsgBox(MSG_TYPE_ERROR, L"Invalid Item", &wstrCaption);
			return false;
		}

		usf::ValidBuyType result = usf::IsValidBuyGoods(pack);
		if (result != usf::vbt_Ok)
		{
			usf::Message_BuyResert(result, chat_msg);
			return false;
		}

		return true;
	}


}	//end of namespace usf
