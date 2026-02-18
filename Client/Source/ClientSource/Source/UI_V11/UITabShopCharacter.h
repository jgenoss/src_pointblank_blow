#if !defined( __UI_TAB_SHOPCHARACTER_H__)
#define __UI_TAB_SHOPCHARACTER_H__

#include "UI_V11/UITabShopBase.h"

enum UIShop_SubTab_Character
{
	UISHOP_SUBTAB_CHARACTER_ALL,
	UISHOP_SUBTAB_CHARACTER_MAINITEM,
	UISHOP_SUBTAB_CHARACTER_HAIR,
	UISHOP_SUBTAB_CHARACTER_MASK,
	UISHOP_SUBTAB_CHARACTER_BODY,
	UISHOP_SUBTAB_CHARACTER_LOWER,
	UISHOP_SUBTAB_CHARACTER_MAX,
};

class UIToolTipShop;

class UITabShopCharacter : public UITabShopBase
{

public:
	UITabShopCharacter(UIPhaseSubBase* phaseShop, UIToolTipShop* pTT);
	virtual ~UITabShopCharacter();


	void			ChangeCharacterCategory( UIShop_SubTab_Character  category);

private:
	virtual void	OnLoadAllScenes();

	virtual void	_MakeGroupList_ByShopCategory(i3::vector<SHOP_GOODS_PACK*>& refGoodsList);
	virtual void	Update_ToolTip_ByShopCategory(SHOP_GOODS_PACK* pGoodsPack, 
		i3::rc_wstring& outItemName, i3::rc_wstring& outDesc, i3::rc_wstring& outCategory, i3::rc_wstring& outStatus, i3::rc_wstring& outRequired);
	
	virtual void OnClickMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack);
	virtual void OnEnterMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack);
	virtual void OnLeaveMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack);
	virtual void	CloseTooltip();
	
	

	UIShop_SubTab_Character		m_category;
	ShopItemSortFilterType		m_currFilterType;

	virtual void	_MakeFilterList_ByShopCategory();
	virtual void	SetFilterType(INT32 filter_idx) { m_currFilterType = ShopItemSortFilterType(filter_idx);  }

	ItemStatus		CalcItemStatus(INT32 itemID);
};

#endif
