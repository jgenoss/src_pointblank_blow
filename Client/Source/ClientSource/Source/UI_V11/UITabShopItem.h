#if !defined( __UI_TAB_SHOPITEM_H__)
#define __UI_TAB_SHOPITEM_H__

#include "UI_V11/UITabShopBase.h"

enum UIShop_SubTab_Item
{
	UISHOP_SUBTAB_ITEM_ALL,
	UISHOP_SUBTAB_ITEM_ENHANCED,
	UISHOP_SUBTAB_ITEM_FUNCTIONAL,
	UISHOP_SUBTAB_ITEM_COMPENSATION,
	UISHOP_SUBTAB_ITEM_BOX,
	UISHOP_SUBTAB_ITEM_MAX,
};


class UIToolTipShop;

class UITabShopItem : public UITabShopBase
{
public:
	UITabShopItem(UIPhaseSubBase* phase, UIToolTipShop* pTT);
	virtual ~UITabShopItem();

	void			ChangeItemCategory( UIShop_SubTab_Item  category);

private:
	virtual void	OnLoadAllScenes();

	void			_MakeGroupList_ByShopCategory(i3::vector<SHOP_GOODS_PACK*>& refGoodsList);
	virtual void	Update_ToolTip_ByShopCategory(SHOP_GOODS_PACK* pGoodsPack, 
		i3::rc_wstring& outItemName, i3::rc_wstring& outDesc, i3::rc_wstring& outCategory, i3::rc_wstring& outStatus, i3::rc_wstring& outRequired);

	virtual void OnClickMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack);
	virtual void OnEnterMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack);
	virtual void OnLeaveMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack);
	virtual void	CloseTooltip();


	UIShop_SubTab_Item	m_category;
	ShopItemSortFilterType		m_currFilterType;

	virtual void	_MakeFilterList_ByShopCategory();
	ItemStatus		CalcItemStatus(INT32 itemID);
	virtual void	SetFilterType(INT32 filter_idx) { m_currFilterType = ShopItemSortFilterType(filter_idx);  }

	

};

#endif
