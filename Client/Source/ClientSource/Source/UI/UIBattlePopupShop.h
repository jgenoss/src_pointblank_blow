#pragma once

#include "UIBattlePopupBase.h"
#include "UILBScrollSlot.h"
#include "UIShopInfoMgr.h"

#include "i3Base/itl/signal/signal_common.h"

enum DomiShopItem;

//namespace domi_util
//{
//	bool is_buyable_domi_item(CASHITEM_GROUP_TYPE buy_group_type);
//	bool is_buyable_domi_cash_item(CASHITEM_GROUP_TYPE buy_group_type);
//}
namespace buy { typedef i3::shared_ptr<class DisplayedGoods> DisplayedGoodsPtr;  }

struct ITEM_INFO;

class UIBattlePopupShop : public UIBattlePopupBase
{
	I3_CLASS_DEFINE( UIBattlePopupShop, UIBattlePopupBase);

	friend class BattleShopSlot;
	//friend bool domi_util::is_buyable_domi_item(CASHITEM_GROUP_TYPE buy_group_type);
	//friend bool domi_util::is_buyable_domi_cash_item(CASHITEM_GROUP_TYPE buy_group_type);

public:	
	UIBattlePopupShop();
	virtual ~UIBattlePopupShop();
	
public:
	void clicked_tap(INT32 idx);

public:
	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnLoadAllScenes() override;
	virtual void	OnEntranceEnd() override;

	virtual void	OnUnloadAllScenes() override;
	virtual void	OnExitEnd(void) override;

	virtual bool	OnKey_Enter( void) override { return false; } //OnKeyInput이 함수에서 처리한다.
	virtual bool	OnKey_Escape( void) override { return false; } //OnKeyInput이 함수에서 처리한다.
	virtual bool	OnKeyInput( i3InputDeviceManager * pInputMng) override;
	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;

#if ENABLE_BATTLESHOP_SCROLL 
	virtual bool OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code) override;
#endif
	

private:
	void on_keyinput_category(i3InputKeyboard* keyboard);
	void on_keyinput_itemlist(i3InputKeyboard* keyboard);
	void (UIBattlePopupShop::*pf_keyinput)(i3InputKeyboard* keyboard);

public:
	VEC3D*  get_left_top();

private:
	void finish_buy_item_normal_imp(bool);
	void finish_buy_item_domi_imp(bool);

	void cb_finish_buy_battle_shop_item(bool, const i3::vector<buy::DisplayedGoodsPtr>&);
	void cb_finish_buy_auth_item(bool, const SHOP_GOODS*);
	void cb_finish_remove_item(bool, const ITEM_INFO*);

public:
	void update_my_cash();
	void select_category(INT32 ctg);
	void to_lua_select_category(INT32 ctg);
	void close_shop();
	void try_remove_item( T_ItemID item_id);

public:
	//아래 함수는 UIBattlePopupShop 클래스 비활성화 상태에서 호출된다.
	//void buy_domi_item(CASHITEM_GROUP_TYPE buy_group_type); 
	//void buy_domi_cash_item(CASHITEM_GROUP_TYPE buy_group_type);

private:
	class BattleShopSlot* m_pSlot;

	INT32 max_category, curr_category;
	INT32 m_sel_idx;
	const SHOP_GOODS* m_re_autu_item_goods; //중복 사용할 경우 기존 아이템 삭제후에 사용할 아이템 인증을 해야된다. (0 은 없다는 뜻)

	i3::scoped_connection_set m_SigConnection;

	//ig_domi_ShopInfoList domi_shop_list;
	bool is_using_normal;

#if ENABLE_BATTLESHOP_SCROLL 
private:
	void hit_test_enter( UINT32 inx, i3UIControl* pWnd );
	void hit_test_leave( UINT32 inx, i3UIControl* pWnd );
	UI::tmiHover_Callback< mine, &mine::hit_test_enter, &mine::hit_test_leave	> m_HitTestCallback;
	UI::tmHover m_tmHitTest;
#endif
};

