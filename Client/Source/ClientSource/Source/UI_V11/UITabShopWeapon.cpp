#include "pch.h"
#include "UI_V11/UITabShopWeapon.h"
#include "UI_V11/UIToolTipShop.h"

#include "UI/UIShopFunction.h"
#include "UI/UIClanFunction.h"

#include "Shop.h"
#include "ShopContext.h"
#include "Designation/DesigUtil.h"

#include "i3Base/string/ext/contain_string.h"
//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//

UITabShopWeapon::UITabShopWeapon(UIPhaseSubBase* phaseShop, UIToolTipShop* pTT) : 
	UITabShopBase(phaseShop, pTT), m_category(UISHOP_SUBTAB_WEAPON_ALL), m_currFilterType(SISFT_W_NEW)
{									  

}

UITabShopWeapon::~UITabShopWeapon()
{

}






void	UITabShopWeapon::OnLoadAllScenes()
{
	m_category = UISHOP_SUBTAB_WEAPON_ALL;
	m_currFilterType = SISFT_W_NEW;
}


void	UITabShopWeapon::_MakeGroupList_ByShopCategory(i3::vector<SHOP_GOODS_PACK*>& refGoodsList)
{
	//	필터를 거친 상품만 리스트에 넣는다.
	refGoodsList.clear();

	CShopContext * pShopContext = CShopContext::i();
	INT32 goodsPackCount = pShopContext->GetGoodsPackCount(SHOP_WEAPON_GOODS);
	i3::rc_wstring keyword = m_phaseShop->GetKeywordString();

	for(INT32 i = 0 ; i < goodsPackCount ; i++)
	{
		SHOP_GOODS_PACK * pGoodsPack = const_cast<SHOP_GOODS_PACK*>(pShopContext->GetGoodsPack(SHOP_WEAPON_GOODS, i));
		if( pGoodsPack == NULL)
			continue;

		SHOP_GOODS * pGoods = pGoodsPack->GetGoods(0);
		if( pGoods == NULL)
			continue;

		INT32  WeaponID = pGoods->_ItemID;
		if (WeaponID <= -1) continue;

		WEAPON_CLASS_TYPE weapon_class = WEAPON::ItemID2Class( pGoods->_ItemID);
		switch(m_category)
		{
		case UISHOP_SUBTAB_WEAPON_ALL:		// 디폴트..
		break;
		case UISHOP_SUBTAB_WEAPON_ASSAULT:
			if (weapon_class != WEAPON_CLASS_ASSAULT && 
				weapon_class != WEAPON_CLASS_MG)
				continue;
		break;
		case UISHOP_SUBTAB_WEAPON_SMG:
			if (weapon_class != WEAPON_CLASS_SMG &&
				weapon_class != WEAPON_CLASS_DUALSMG)
				continue;
		break;
		case UISHOP_SUBTAB_WEAPON_SNIPER:
			if (weapon_class != WEAPON_CLASS_SNIPER)
				continue;
		break;
		case UISHOP_SUBTAB_WEAPON_SHOTGUN:
			if (weapon_class != WEAPON_CLASS_SHOTGUN&&
				weapon_class != WEAPON_CLASS_DUALSHOTGUN)
				continue;
		break;
		case UISHOP_SUBTAB_WEAPON_SUBWEAPON:
			if (weapon_class != WEAPON_CLASS_HANDGUN&&
				weapon_class != WEAPON_SUBCLASS_ASSAULT&&
				weapon_class != WEAPON_SUBCLASS_SHOTGUN&&
				weapon_class != WEAPON_SUBCLASS_SNIPER&&
				weapon_class != WEAPON_SUBCLASS_SMG&&
				weapon_class != WEAPON_SUBCLASS_G_LAUNCHER&&
				weapon_class != WEAPON_SUBCLASS_BOW&&
				weapon_class != WEAPON_CLASS_BOW&&
				weapon_class != WEAPON_CLASS_DUALHANDGUN&&
				weapon_class != WEAPON_CLASS_CIC)
				continue;
		break;
		case UISHOP_SUBTAB_WEAPON_MELEE:
			if(weapon_class != WEAPON_CLASS_KNIFE&&
			   weapon_class != WEAPON_CLASS_DUALKNIFE&&
			   weapon_class != WEAPON_CLASS_KNUCKLE)
				continue;
		break;
		case UISHOP_SUBTAB_WEAPON_THROWN:
			if (weapon_class != WEAPON_CLASS_THROWING_GRENADE&&
				weapon_class != WEAPON_CLASS_THROWING_ATTACK)
				continue;
		break;
		case UISHOP_SUBTAB_WEAPON_SPECIAL:
			if(weapon_class != WEAPON_CLASS_THROWING_CONTAIN&&
			   weapon_class != WEAPON_CLASS_THROWING_HEAL)
				continue;
		break;
		default:
		break;
		}
		
		if( pGoods->_SaleType == GOODS_SALE_EVENT )
			continue;

		if ( keyword.size() != 0 )
		{
			i3::rc_wstring strItemName;
			usf::GetShopItemName(strItemName, pGoods);
			i3::to_lower_copy(strItemName, strItemName.c_str()); i3::to_lower_copy(keyword, keyword.c_str());
			if( i3::contain_string(strItemName,keyword) < 0 )		
				continue;
		}
		
		refGoodsList.push_back(pGoodsPack);
	}

	switch (m_currFilterType)
	{
	case SISFT_W_NAME:	i3::sort(refGoodsList.begin(), refGoodsList.end(), cbSortByName); break;
	case SISFT_W_DAMAGE: i3::sort(refGoodsList.begin(), refGoodsList.end(), cbSortByDamage); break;
	case SISFT_W_RAPID:	i3::sort(refGoodsList.begin(), refGoodsList.end(), cbSortByRapid); break;
	case SISFT_W_SPEED: i3::sort(refGoodsList.begin(), refGoodsList.end(), cbSortBySpeed); break;
	case SISFT_W_POINT:	i3::sort(refGoodsList.begin(), refGoodsList.end(), cbSortByPoint); break;
	case SISFT_W_CASH:	i3::sort(refGoodsList.begin(), refGoodsList.end(), cbSortByCash); break;
	default: break;
	}
}

bool	UITabShopWeapon::cbSortByDamage(SHOP_GOODS_PACK* pGoodsPack1, SHOP_GOODS_PACK* pGoodsPack2)
{
	SHOP_GOODS* pGoods1 = pGoodsPack1->GetGoods(0);	SHOP_GOODS* pGoods2 = pGoodsPack2->GetGoods(0);
	
	INT32 itemID1 = pGoods1->_ItemID;	INT32 itemID2 = pGoods2->_ItemID;

	WEAPON_CLASS_TYPE weapon_class1 = WEAPON::ItemID2Class( itemID1 );
	WEAPON_CLASS_TYPE weapon_class2 = WEAPON::ItemID2Class( itemID2 );

	CWeaponInfo* pInfo1 = g_pWeaponInfoDataBase->getWeaponInfo( itemID1);
	CWeaponInfo* pInfo2 = g_pWeaponInfoDataBase->getWeaponInfo( itemID2);

	INT32 damage1 =  GetWeaponDamage(weapon_class1, pInfo1);
	INT32 damage2 =  GetWeaponDamage(weapon_class2, pInfo2);

	return damage2 < damage1;
}

bool	UITabShopWeapon::cbSortByRapid(SHOP_GOODS_PACK* pGoodsPack1, SHOP_GOODS_PACK* pGoodsPack2)
{
	
	SHOP_GOODS* pGoods1 = pGoodsPack1->GetGoods(0);	SHOP_GOODS* pGoods2 = pGoodsPack2->GetGoods(0);

	INT32 itemID1 = pGoods1->_ItemID;	INT32 itemID2 = pGoods2->_ItemID;

	CWeaponInfo* pInfo1 = g_pWeaponInfoDataBase->getWeaponInfo( itemID1);
	CWeaponInfo* pInfo2 = g_pWeaponInfoDataBase->getWeaponInfo( itemID2);

	INT32 rapid1 = (pInfo1) ? pInfo1->GetPercentage(WEAPON::INFO_SPEED) : 0;
	INT32 rapid2 = (pInfo2) ? pInfo2->GetPercentage(WEAPON::INFO_SPEED) : 0;

	return rapid2 < rapid1;

}

bool	UITabShopWeapon::cbSortBySpeed(SHOP_GOODS_PACK* pGoodsPack1, SHOP_GOODS_PACK* pGoodsPack2)
{

	SHOP_GOODS* pGoods1 = pGoodsPack1->GetGoods(0);	SHOP_GOODS* pGoods2 = pGoodsPack2->GetGoods(0);

	INT32 itemID1 = pGoods1->_ItemID;	INT32 itemID2 = pGoods2->_ItemID;

	CWeaponInfo* pInfo1 = g_pWeaponInfoDataBase->getWeaponInfo( itemID1);
	CWeaponInfo* pInfo2 = g_pWeaponInfoDataBase->getWeaponInfo( itemID2);

	INT32 speed1 = (pInfo1) ? pInfo1->GetPercentage(WEAPON::INFO_SPEED) : 0;
	INT32 speed2 = (pInfo2) ? pInfo2->GetPercentage(WEAPON::INFO_SPEED) : 0;

	return speed2 < speed1;
}

INT32 UITabShopWeapon::GetWeaponDamage(WEAPON_CLASS_TYPE eClassType, CWeaponInfo * pInfo)
{
	if( pInfo == NULL)
		return 0;

	INT32 Damage = 0;

	if(eClassType == WEAPON_CLASS_SHOTGUN)
		Damage = pInfo->GetDamage() * pInfo->GetFireBulletCount();
	else 
		Damage = pInfo->GetDamage();

	if(Damage > 100)
		Damage = 100;
	else if(Damage < 0)
		Damage = 0;

	return Damage;
}

void	UITabShopWeapon::ChangeWeaponCategory(UIShop_SubTab_Weapon category)
{
	if (m_category == category)
		return;
	// 상품 목록을 다시 만든다.

	m_category = category;
	m_phaseShop->ChangeSubCategory();

}
/*virtual*/	void	UITabShopWeapon::OnBuyGoodsSuccess()
{
#if defined (LOCALE_THAILAND)
	
	m_phaseShop->ProcessBuyGoodsSuccess_OnThailand();
	
#endif
}

void	UITabShopWeapon::Update_ToolTip_ByShopCategory(SHOP_GOODS_PACK* pGoodsPack, 
													 i3::rc_wstring& outItemName, i3::rc_wstring& outDesc, i3::rc_wstring& outCategory, i3::rc_wstring& outStatus, i3::rc_wstring& outRequired)
{
	SHOP_GOODS* pGoods = pGoodsPack->GetGoods(0);
	INT32 itemID = pGoods->_ItemID;

	UINT8 Type = 0; 	UINT32 Arg = 0;
	bool  bRepairItem = false;

	bRepairItem = (CShopContext::i()->IsDurabilityItem(pGoods->_ItemID) != 0);

	if ( CInvenList::i()->GetItemTypeByItemID( INVEN::DB_WEAPON, itemID, &Type, &Arg))
	{
		i3::wstring temp_outstatus;
		bool Expendable = (pGoods->_AuthType == ITEM_TYPE_COUNT) ;
		usf::GetGoodsUseDay( CalcItemStatus(itemID), bRepairItem, Expendable, Type, Arg, temp_outstatus);
		outStatus = temp_outstatus;
	}
	else
	{
		//	미보유라도 갯수제,기간제 구분하는 상세설명을 넣는다..
		i3::stack_wstring temp_outstatus;
		bool IsCounted = (pGoods->_AuthType == ITEM_TYPE_COUNT) ;
		usf::GetGoodsUseDay_Zero(bRepairItem, IsCounted, temp_outstatus);
		outStatus = temp_outstatus;
	}

	CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo( itemID );
	
	if (pInfo)
	{
		pInfo->GetWeaponName( outItemName);

		if( outItemName.size() == 0)
			i3::sprintf(outItemName, L"NO NAME\nGoods:%d\nItem:%d", INVALID_TEXT, pGoods->_GoodsID, pGoods->_ItemID); 
		
		pInfo->GetDescription(outDesc);

		// 슬롯에서 표시되었던 것을 이제 툴팁으로 옮김..
		// 1. 무기 카테고리..
		g_pWeaponInfoDataBase->GetWeaponCategoryEx(pInfo, outCategory);
		
		const i3::vector<UINT8>* pvecNeedDesigTypes = NULL;

		if (DesigUtil::CheckPurchasable_MyItemByDesig(itemID, &pvecNeedDesigTypes))
		{	// 기존코드는 구입가능시 표시가 없었으나 이 경우엔 필요할지 모른다.
			if (pvecNeedDesigTypes == NULL || pvecNeedDesigTypes->empty() )
			{
				i3::sprintf(outRequired,L"%s",GAME_STRING("STR_SHOP_CAN_BUYING"));
			}
			else
			{
				i3::sprintf(outRequired, L"%s (%s)", GAME_RCSTRING("STR_SHOP_CAN_BUYING"), GAME_RCSTRING("STR_TBL_DESIGNATION_TEXT") ); 
			}
		}
		else
		{
			#if defined(BUY_WEAPON_BY_CASH)	
				#if defined( ENABLE_BUY_CASH_WEAPON_IF_NEED_USER_TITLE) 
				UINT32 weaponBuyType = (UINT32)g_pGameContext->GetShop()->GetGoodsPrice(pGoods->_GoodsID, NULL, NULL);
				if (weaponBuyType == GOODS_PRICE_CASH)
				{
					i3::sprintf(outRequired, L"%s (%s)", GAME_RCSTRING("STR_SHOP_CAN_BUYING"), GAME_RCSTRING("STR_TBL_DESIGNATION_TEXT") ); 
				}
				else
				#endif
				{
					i3::sprintf(outRequired, L"{col:255,0,0,255}%s (%s){/col}", GAME_RCSTRING("STR_SHOP_CANNOT_ITEM_BUYING"), GAME_RCSTRING("STR_SHOP_CANNOT_BUYING")); 
				}
			#else
				i3::sprintf(outRequired, L"{col:255,0,0,255}%s (%s){/col}", GAME_RCSTRING("STR_SHOP_CANNOT_ITEM_BUYING"), GAME_RCSTRING("STR_SHOP_CANNOT_BUYING")); 
			#endif
		}
		
	}
	else
	{
		i3::sprintf(outItemName,L"%s\nGoods:%d\nItem:%d",INVALID_TEXT, pGoods->_GoodsID, pGoods->_ItemID);

		// 여기서 컨티뉴를 건드리면 안되고, 나머지 정보는 리셋시켜서 함수전송..
		outDesc.clear();
		outCategory.clear();
		outRequired.clear();
	}
}

ItemStatus	UITabShopWeapon::CalcItemStatus(INT32 itemID)
{
	T_ItemDBIdx ware_idx = CInvenList::i()->FindWareDBIdx_ByItemID(itemID);
	return usf::GetItemStatus(itemID, ware_idx);
}

void UITabShopWeapon::OnClickMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack)
{
	m_toolTip->EnableCurrentSlotToolTip(SlotIndex, pGoodsPack);
}

void UITabShopWeapon::OnEnterMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack)
{
	m_toolTip->EnableCurrentSlotToolTip(SlotIndex, pGoodsPack);
}

void UITabShopWeapon::OnLeaveMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack)
{
	m_toolTip->DisableToolTip(SlotIndex, pGoodsPack);
}

void UITabShopWeapon::CloseTooltip()
{
	m_toolTip->DisableToolTip_Forced();
}


void	UITabShopWeapon::_MakeFilterList_ByShopCategory()
{
	ucf::ComboString str_combo;
	str_combo	<< GAME_RCSTRING("STR_SHOP_FILTER_NEW") 
		<< GAME_RCSTRING("STR_SHOP_FILTER_NAME") 
		<< GAME_RCSTRING("STR_SHOP_FILTER_DAMAGE")
		<< GAME_RCSTRING("STR_SHOP_FILTER_RAPIDFIRE")
		<< GAME_RCSTRING("STR_SHOP_FILTER_SPEED")
		<< GAME_RCSTRING("STR_SHOP_FILTER_POINT") 
		<< GAME_RCSTRING("STR_SHOP_FILTER_CASH");

	LuaState* L = UILua::CallLuaFunction(m_phaseShop->GetMenuScene(), "SetFilterList");
	const i3::wstring cs_wstr = str_combo;
	i3Lua::PushStringUTF16To8(L, cs_wstr.c_str(), cs_wstr.size());
	i3Lua::PushInteger(L, m_currFilterType);
	UILua::EndLuaFunction(L, 2);
}
