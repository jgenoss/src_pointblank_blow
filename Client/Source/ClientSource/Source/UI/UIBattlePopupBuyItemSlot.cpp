#include "pch.h"
#include "UIBattlePopupBuyItemSlot.h"

#include "UIBattlePopupBuyItem.h"

#include "UIShopFunction.h"
#include "BuyUtil.h"
#include "UIItemInfoUtil.h"

#include "i3Base/ColorString.h"

#include "UISignal.h"

namespace
{
	UIBattlePopupBuyItemSlot* g_this = 0;
	UINT32 g_curr_slot_idx = 0;
	enum	{	ROW = 8, COL = 1, SLOT = ROW * COL,	};
}

extern "C" 
{
	int battle_buy_item_slot( LuaState * L)
	{
		g_this->clicked();
		return 0;
	}
	int battle_buy_item_slot2( LuaState * L)
	{
		INT32 tab = i3Lua::GetIntegerArg( L, 1);
		g_this->clicked_combo(tab);
		return 0;
	}
}

LuaFuncReg battle_buy_item_slot_glue[] =
{
	{ "Click", battle_buy_item_slot	},
	{ "ClickCombo", battle_buy_item_slot2	},
	{ nullptr, nullptr}
};

UIBattlePopupBuyItemSlot::UIBattlePopupBuyItemSlot() : mParent(0)
{
	g_this = this;
	mSlot.EnableScroll(false);
}

void UIBattlePopupBuyItemSlot::clicked()
{
	buy::RemoveByPurchaseRoute(mParent->getPurchaseRoute(), get_now_slot());

	update_list();

//	UI::call_cb("update_cart_board");	//상품 하나 지웟으니 cart board를 갱신한다.
	UISignal::UpdateCartBoard();		//상품 하나 지웟으니 cart board를 갱신한다.

}

void UIBattlePopupBuyItemSlot::clicked_combo(INT32 tab)
{
	//combo box에 unfolder 된 menu에 항목 선택시에 now slot idx 가 변경 될 수 있기에 unfolder 될 때 slot idx를 얻는다.
	if( tab == -1 ) { g_curr_slot_idx = get_now_slot(); return; }

	buy::DisplayedGoods* pDisplayedGoods = buy::GetDisplayedGoods_ByPurchaseRoute(mParent->getPurchaseRoute(), g_curr_slot_idx);

	I3ASSERT_RETURN(pDisplayedGoods);

	pDisplayedGoods->SetCurrentDisplayIndex(tab);

	update_list();
}

void UIBattlePopupBuyItemSlot::update_list()
{
	mParent->update_item_paymenu();
	mSlot.UpdateList( buy::GetCount_ByPurchaseRouteType( mParent->getPurchaseRoute() ) );
}


void UIBattlePopupBuyItemSlot::OnCreate( i3GameNode * pParent)
{
	//Slot 만들고
	mSlot.EnableScroll( false );
	mSlot.OnCreate( (UIBase*)mParent, ROW, COL);
	mSlot.RegisterSlotScene("Scene/Popup/PBRe_PopUp_ShoppingCart_Slot.i3UIs", battle_buy_item_slot_glue);
}

void UIBattlePopupBuyItemSlot::OnLoadAllScenes()
{
	mSlot.OnLoadAllScenes(227, 186, 0,  0);

	update_list();

	//combo 박스 때문에 slot depth 변경
	mSlot.ReverseScene();

	mSlot.SetScrollFocus();
}

void UIBattlePopupBuyItemSlot::OnUnloadAllScenes()
{
	mSlot.OnUnloadAllScenes();
}

void UIBattlePopupBuyItemSlot::OnUpdate( REAL32 rDeltaSeconds)
{
	mSlot.OnUpdate();
}

void UIBattlePopupBuyItemSlot::OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code)
{
	mSlot.OnEvent(event, pObj, param2, code);
}

void	UIBattlePopupBuyItemSlot::cb_Enabled_impl( INT32 slot_idx, i3UIScene* slot_scn)
{
	const buy::DisplayedGoods* displayedGoods = buy::GetDisplayedGoods_ByPurchaseRoute( mParent->getPurchaseRoute(), slot_idx );
	const SHOP_GOODS* pGoods = displayedGoods->GetCurrentGoods();

	i3::rc_wstring wstr_name;
	usf::GetShopItemName(wstr_name, pGoods);

	bool is_red_name = false;

	if( item_def::is_chara_category(pGoods->_ItemID) == true ) //캐릭터를 구입 할 수 없는 경우
		is_red_name = !is_buyable_chara;
	else
		is_red_name = (usf::IsValidBuyGoods(displayedGoods->GetGoodsPack() ) == usf::vbt_Ok) ? false : true;

	if( is_red_name == true ) 
	{
		::ColorString(wstr_name, 255, 0, 0);
	}

	LuaState* L = 0;
	L = UILua::CallLuaFunction( slot_scn, "SetSlot");
	i3Lua::PushStringUTF16To8(L, wstr_name.c_str(), wstr_name.size() );
	UILua::EndLuaFunction(L , 1);

	//아이템 종류 (1/3/7/9 일 등..)
	i3::wstring wstr_nameList = usf::payment::get_price_combo_string(displayedGoods->GetGoodsPack());

	L = UILua::CallLuaFunction( slot_scn, "SetFilterList");
	i3Lua::PushStringUTF16To8(L, wstr_nameList);
	i3Lua::PushInteger(L, displayedGoods->GetCurrentDisplayIndex() );
	UILua::EndLuaFunction(L , 2);
}

void UIBattlePopupBuyItemSlot::cb_Disabled_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	slot_scn->EnableScene(false);
}
