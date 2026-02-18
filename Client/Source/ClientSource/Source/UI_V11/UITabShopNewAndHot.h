#if !defined( __UI_TAB_SHOPNEWANDHOT_H__)
#define __UI_TAB_SHOPNEWANDHOT_H__

#include "UI_V11/UITabShopBase.h"

enum UIShop_SubTab_NewHot
{
	UISHOP_SUBTAB_NEWHOT_ALL,
	UISHOP_SUBTAB_NEWHOT_WEAPON,
	UISHOP_SUBTAB_NEWHOT_CHARACTER,
	UISHOP_SUBTAB_NEWHOT_ITEM,					// 신규 기획 확충에 의해 이 enum값은 더 추가될수 있다...
	UISHOP_SUBTAB_NEWHOT_MAX,			
};

class UITabShopNewAndHot : public UITabShopBase
{

public:
	UITabShopNewAndHot(UIPhaseSubBase* phase);
	virtual ~UITabShopNewAndHot();

	void			ChangeNewHotCategory( UIShop_SubTab_NewHot  category);	
private:
	virtual void	OnLoadAllScenes();
	virtual void	_MakeGroupList_ByShopCategory(i3::vector<SHOP_GOODS_PACK*>& refGoodsList);
	void			_MakeGroupList_ShopGoodsType(i3::vector<SHOP_GOODS_PACK*>& refGoodsList, SHOP_GOODS_TYPE t);
	
	virtual void	Update_ToolTip_ByShopCategory(SHOP_GOODS_PACK* pGoodsPack, 
		i3::rc_wstring& outItemName, i3::rc_wstring& outDesc,i3::rc_wstring& outCategory, i3::rc_wstring& outStatus, i3::rc_wstring& outRequired);

	virtual void OnClickMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack);
	virtual void OnEnterMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack);
	virtual void OnLeaveMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack);
	virtual void CloseTooltip();


	UIShop_SubTab_NewHot m_category;
	ShopItemSortFilterType_NH m_currFilter;

	virtual void	_MakeFilterList_ByShopCategory();

	virtual void	SetFilterType(INT32 filter_idx) { m_currFilter = ShopItemSortFilterType_NH(filter_idx);  }

public:

	static bool			cbSortByNew(SHOP_GOODS_PACK* pGoodsPack1, SHOP_GOODS_PACK* pGoodsPack2);
	static bool			cbSortByHot(SHOP_GOODS_PACK* pGoodsPack1, SHOP_GOODS_PACK* pGoodsPack2);

};

#endif
