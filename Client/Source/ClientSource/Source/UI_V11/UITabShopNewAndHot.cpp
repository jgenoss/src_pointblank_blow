#include "pch.h"

#include "UI/UIClanFunction.h"
#include "UI/UIShopFunction.h"

#include "UI_V11/UIToolTipShop.h"
#include "UI_V11/UIPhaseShop_V11.h"
#include "UI_V11/UITabShopNewAndHot.h"

#include "ShopContext.h"

UITabShopNewAndHot::UITabShopNewAndHot(UIPhaseSubBase* phase) 
: UITabShopBase(phase, NULL), m_category(UISHOP_SUBTAB_NEWHOT_ALL), m_currFilter(SISFT_NH_NONE)
{
}

UITabShopNewAndHot::~UITabShopNewAndHot()
{
}

void	UITabShopNewAndHot::OnLoadAllScenes()
{
	m_category = UISHOP_SUBTAB_NEWHOT_ALL;
	m_currFilter = SISFT_NH_NONE;
	m_toolTip = NULL;
}

void	UITabShopNewAndHot::ChangeNewHotCategory( UIShop_SubTab_NewHot  category)
{
	if (m_category == category) return;

	m_category = category;
	m_phaseShop->ChangeSubCategory();
}

void	UITabShopNewAndHot::_MakeGroupList_ByShopCategory(i3::vector<SHOP_GOODS_PACK*>& refGoodsList)
{
	refGoodsList.clear();
	
	switch (m_category)
	{
	case UISHOP_SUBTAB_NEWHOT_ALL:
		_MakeGroupList_ShopGoodsType(refGoodsList, SHOP_WEAPON_GOODS);
		_MakeGroupList_ShopGoodsType(refGoodsList, SHOP_CHARACTER_GOODS);
		_MakeGroupList_ShopGoodsType(refGoodsList, SHOP_CASH_ITEM_GOODS);
		_MakeGroupList_ShopGoodsType(refGoodsList, SHOP_PACKAGE_GOODS);
	break;

	case UISHOP_SUBTAB_NEWHOT_WEAPON:
		_MakeGroupList_ShopGoodsType(refGoodsList, SHOP_WEAPON_GOODS);
	break;
	
	case UISHOP_SUBTAB_NEWHOT_CHARACTER:
		_MakeGroupList_ShopGoodsType(refGoodsList, SHOP_CHARACTER_GOODS);
	break;

	case UISHOP_SUBTAB_NEWHOT_ITEM:
		_MakeGroupList_ShopGoodsType(refGoodsList, SHOP_CASH_ITEM_GOODS);
		_MakeGroupList_ShopGoodsType(refGoodsList, SHOP_PACKAGE_GOODS);
	break;

	default: break;
	}

	// 소팅 처리..	
	switch(m_currFilter)
	{
	case SISFT_NH_NEW: i3::sort(refGoodsList.begin(), refGoodsList.end(), cbSortByNew); break; 
	case SISFT_NH_HOT: i3::sort(refGoodsList.begin(), refGoodsList.end(), cbSortByHot); break;
	case SISFT_NH_NAME: i3::sort(refGoodsList.begin(), refGoodsList.end(), cbSortByName); break;
	case SISFT_NH_POINT: i3::sort(refGoodsList.begin(), refGoodsList.end(), cbSortByPoint); break;
	case SISFT_NH_CASH: i3::sort(refGoodsList.begin(), refGoodsList.end(), cbSortByPoint); break;

	default: break;		// 디폴트..
	}

}

void	UITabShopNewAndHot::_MakeGroupList_ShopGoodsType(i3::vector<SHOP_GOODS_PACK*>& refGoodsList, SHOP_GOODS_TYPE t)
{
	CShopContext * pShopContext = CShopContext::i();
	INT32 goodsPackCount = pShopContext->GetGoodsPackCount(t);
	
	i3::rc_wstring keyword = m_phaseShop->GetKeywordString();

	for(INT32 i = 0 ; i < goodsPackCount ; i++)
	{
		SHOP_GOODS_PACK * pGoodsPack = const_cast<SHOP_GOODS_PACK*>(pShopContext->GetGoodsPack(t, i));
		if( pGoodsPack == NULL)
			continue;
		SHOP_GOODS * pGoods = pGoodsPack->GetGoods(0);
		if( pGoods == NULL)
			continue;
		INT32  itemID = pGoods->_ItemID;
		if (itemID <= -1) continue;
		
		if ( pGoods->GetSaleMark(PRICE_TYPE_SHOP_REAL) == GOODS_MARK_SALE)			// 세일마크가 있는것만 걸러낸다..
			continue;

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
}

bool	UITabShopNewAndHot::cbSortByNew(SHOP_GOODS_PACK* pGoodsPack1, SHOP_GOODS_PACK* pGoodsPack2)
{

	SHOP_GOODS* pGoods1 = pGoodsPack1->GetGoods(0);	SHOP_GOODS* pGoods2 = pGoodsPack2->GetGoods(0);

	INT32 sale_mark1 = pGoods1->GetSaleMark(PRICE_TYPE_SHOP_REAL);
	INT32 sale_mark2 = pGoods2->GetSaleMark(PRICE_TYPE_SHOP_REAL);
	
	return sale_mark1 < sale_mark2;
}

bool	UITabShopNewAndHot::cbSortByHot(SHOP_GOODS_PACK* pGoodsPack1, SHOP_GOODS_PACK* pGoodsPack2)
{

	SHOP_GOODS* pGoods1 = pGoodsPack1->GetGoods(0);	SHOP_GOODS* pGoods2 = pGoodsPack2->GetGoods(0);
	
	INT32 sale_mark1 = pGoods1->GetSaleMark(PRICE_TYPE_SHOP_REAL);
	INT32 sale_mark2 = pGoods2->GetSaleMark(PRICE_TYPE_SHOP_REAL);
	
	if ( sale_mark1 == 2) sale_mark1 = 0;
	if ( sale_mark2 == 2) sale_mark2 = 0;
	
	return sale_mark1 < sale_mark2;			
}


void	UITabShopNewAndHot::Update_ToolTip_ByShopCategory(SHOP_GOODS_PACK* pGoodsPack, 
											 i3::rc_wstring& outItemName, i3::rc_wstring& outDesc, i3::rc_wstring& outCategory, i3::rc_wstring& outStatus, i3::rc_wstring& outRequired)
{
	SHOP_GOODS* pGoods = pGoodsPack->GetGoods(0);
	UITabShopBase* concrete = m_phaseShop->FindShopTab_ByGoods(pGoods);

	concrete->Update_ToolTip_ByShopCategory(pGoodsPack, outItemName, outDesc, outCategory, outStatus, outRequired);
}

void UITabShopNewAndHot::OnClickMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack)
{
	SHOP_GOODS* pGoods = pGoodsPack->GetGoods(0);
	UITabShopBase* concrete = m_phaseShop->FindShopTab_ByGoods(pGoods);
	
	if( concrete == NULL ) return;

	if ( m_toolTip != concrete->GetToolTip() )
	{
		if (m_toolTip)
			m_toolTip->DisableToolTip_Forced();
		m_toolTip = concrete->GetToolTip();
	}

	m_toolTip->EnableCurrentSlotToolTip(SlotIndex, pGoodsPack);
}


void UITabShopNewAndHot::OnEnterMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack)
{
	SHOP_GOODS* pGoods = pGoodsPack->GetGoods(0);
	UITabShopBase* concrete = m_phaseShop->FindShopTab_ByGoods(pGoods);
	
	if( concrete == NULL ) return;

	if ( m_toolTip != concrete->GetToolTip() )
	{
		if (m_toolTip)
			m_toolTip->DisableToolTip_Forced();
		m_toolTip = concrete->GetToolTip();
	}

	m_toolTip->EnableCurrentSlotToolTip(SlotIndex, pGoodsPack);
	
}

void UITabShopNewAndHot::OnLeaveMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack)
{
	SHOP_GOODS* pGoods = pGoodsPack->GetGoods(0);
	if( pGoods == NULL)
		return;
	UITabShopBase* concrete = m_phaseShop->FindShopTab_ByGoods(pGoods);
	
	if( concrete == NULL ) return;

	UIToolTipShop* pTT = concrete->GetToolTip();
	pTT->DisableToolTip(SlotIndex, pGoodsPack);

	if ( pTT->IsToolTipDisable() && m_toolTip == pTT )
	{
		m_toolTip = NULL;
	}
	
}

void UITabShopNewAndHot::CloseTooltip()
{
	if (m_toolTip)
		m_toolTip->DisableToolTip_Forced();
}

void	UITabShopNewAndHot::_MakeFilterList_ByShopCategory()
{
	ucf::ComboString str_combo;

	str_combo	<< GAME_RCSTRING("STR_SHOP_FILTER_DEFAULT")
				<< GAME_RCSTRING("STR_SHOP_FILTER_NEW")
				<< GAME_RCSTRING("STR_SHOP_FILTER_HOT")
				<< GAME_RCSTRING("STR_SHOP_FILTER_NAME")
				<< GAME_RCSTRING("STR_SHOP_FILTER_POINT")
				<< GAME_RCSTRING("STR_SHOP_FILTER_CASH");

	LuaState* L = UILua::CallLuaFunction(m_phaseShop->GetMenuScene(), "SetFilterList");
	const i3::wstring cs_wstr = str_combo;
	i3Lua::PushStringUTF16To8(L, cs_wstr.c_str(), cs_wstr.size());
	i3Lua::PushInteger(L, m_currFilter);
	UILua::EndLuaFunction(L, 2);
	
}
