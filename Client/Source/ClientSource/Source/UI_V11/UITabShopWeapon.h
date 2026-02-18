#if !defined( __UI_TAB_SHOPWEAPON_H__)
#define __UI_TAB_SHOPWEAPON_H__

#include "UI_V11/UITabShopBase.h"

enum UIShop_SubTab_Weapon
{
	UISHOP_SUBTAB_WEAPON_ALL,
	UISHOP_SUBTAB_WEAPON_ASSAULT,
	UISHOP_SUBTAB_WEAPON_SMG,
	UISHOP_SUBTAB_WEAPON_SNIPER,
	UISHOP_SUBTAB_WEAPON_SHOTGUN,
	UISHOP_SUBTAB_WEAPON_SUBWEAPON,
	UISHOP_SUBTAB_WEAPON_MELEE,
	UISHOP_SUBTAB_WEAPON_THROWN,
	UISHOP_SUBTAB_WEAPON_SPECIAL,
	UISHOP_SUBTAB_WEAPON_MAX,
};

class UIToolTipShop;

class UITabShopWeapon : public UITabShopBase
{
protected:

public:
	UITabShopWeapon(UIPhaseSubBase* phaseShop, UIToolTipShop* pTT);
	virtual ~UITabShopWeapon();

	void			ChangeWeaponCategory( UIShop_SubTab_Weapon  category);	

private:
	virtual void	OnLoadAllScenes();

	virtual	void	OnBuyGoodsSuccess();

	virtual void	_MakeGroupList_ByShopCategory(i3::vector<SHOP_GOODS_PACK*>& refGoodsList);
	
	virtual void	Update_ToolTip_ByShopCategory(SHOP_GOODS_PACK* pGoodsPack, 
		i3::rc_wstring& outItemName, i3::rc_wstring& outDesc, i3::rc_wstring& outCategory, i3::rc_wstring& outStatus, i3::rc_wstring& outRequired);


	virtual void OnClickMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack);
	virtual void OnEnterMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack);
	virtual void OnLeaveMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack);
	virtual void CloseTooltip();
	
	UIShop_SubTab_Weapon  m_category;
	ShopItemSortFilterType_Weapon	m_currFilterType;


	virtual void	_MakeFilterList_ByShopCategory();
	
	ItemStatus		CalcItemStatus(INT32 itemID);
	virtual void	SetFilterType(INT32 filter_idx) { m_currFilterType = ShopItemSortFilterType_Weapon(filter_idx);  }

public:
	static bool	cbSortByDamage(SHOP_GOODS_PACK* pGoodsPack1, SHOP_GOODS_PACK* pGoodsPack2);
	static bool	cbSortByRapid(SHOP_GOODS_PACK* pGoodsPack1, SHOP_GOODS_PACK* pGoodsPack2);
	static bool	cbSortBySpeed(SHOP_GOODS_PACK* pGoodsPack1, SHOP_GOODS_PACK* pGoodsPack2);

	static INT32 GetWeaponDamage(WEAPON_CLASS_TYPE eClassType, CWeaponInfo * pInfo);
};

#endif
