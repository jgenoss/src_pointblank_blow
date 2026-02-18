#if !defined( __UI_TAB_SHOPBASE_H__)
#define __UI_TAB_SHOPBASE_H__

#include "UI/UITabBase.h"

#include "UI_V11/UIPhaseShop_V11.h"
#include "Shop.h"

//#define MAX_ITEM_SLOT		12

// 기존코드와 달리 새리소스에서 라디오버튼이 보이지 않는다.

class UIPhaseShop_V11;
class UIToolTipShop;



enum ShopItemSortFilterType_Weapon
{
	SISFT_W_NEW,				// 정렬없음..ShopContext의 디폴트리스트가 이미 이 정렬로 이루어짐..
	SISFT_W_NAME,
	SISFT_W_DAMAGE,
	SISFT_W_RAPID,
	SISFT_W_SPEED,
	SISFT_W_POINT,
	SISFT_W_CASH,
	SISFT_W_MAX,
};

enum ShopItemSortFilterType
{
	SISFT_NEW,
	SISFT_NAME,
	SISFT_POINT,
	SISFT_CASH,
	SISFT_MAX,
};


enum ShopItemSortFilterType_NH
{
	SISFT_NH_NONE,
	SISFT_NH_NEW,
	SISFT_NH_HOT,
	SISFT_NH_NAME,
	SISFT_NH_POINT,
	SISFT_NH_CASH,
	SISFT_NH_MAX,
};


class UITabShopBase 
{

protected:
	UIPhaseSubBase*		m_phaseShop;
	UIToolTipShop*			m_toolTip;
public:

	UITabShopBase(UIPhaseSubBase* phaseShop, UIToolTipShop* toolTip );
	virtual ~UITabShopBase();
	
	UIToolTipShop*			GetToolTip() const { return m_toolTip; }
	
	virtual	void	OnBuyGoodsSuccess()		{}
	
	virtual void	OnLoadAllScenes() {}

	virtual void	OnClickMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack) {}
	virtual void	OnEnterMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack) {}
	virtual void	OnLeaveMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack) {}
	virtual void	CloseTooltip() {}


	void			MakeGroupList(i3::vector<SHOP_GOODS_PACK*>& refGoodsList);
	void			MakeFilterList();
	virtual void	SetFilterType(INT32 filter_idx) {} //{ m_currFilterType = filter_idx;  }

	virtual void	Update_ToolTip_ByShopCategory(SHOP_GOODS_PACK* pGoodsPack, 
		i3::rc_wstring& outItemName, i3::rc_wstring& outDesc, i3::rc_wstring& outCategory, i3::rc_wstring& outStatus, i3::rc_wstring& outRequired) {}



protected:
	


private:

	virtual void	_MakeGroupList_ByShopCategory(i3::vector<SHOP_GOODS_PACK*>& refGoodsList); 
	virtual void	_MakeFilterList_ByShopCategory();
public:
	
	static bool	cbSortByName(SHOP_GOODS_PACK* pGoodsPack1, SHOP_GOODS_PACK* pGoodsPack2);
	static bool	cbSortByPoint(SHOP_GOODS_PACK* pGoodsPack1, SHOP_GOODS_PACK* pGoodsPack2);
	static bool	cbSortByCash(SHOP_GOODS_PACK* pGoodsPack1, SHOP_GOODS_PACK* pGoodsPack2);

};


#endif
