#include "pch.h"
#include "UIPopupBuyItemSlot.h"

#include "UIPopupBuyItem.h"
#include "UIShopFunction.h"
#include "BuyUtil.h"
#include "UIItemInfoUtil.h"
#include "UIDefine.h"
#include "i3Base/ColorString.h"
#include "UIMainFrame.h"
#include "GachaContext.h"
#include "UIUtil.h"

#include "UISignal.h"

namespace
{
	ShopPopupBuyItemSlot* g_this = 0;
	UINT32 g_curr_slot_idx = 0;
}

extern "C" 
{
	int buy_item_slot( LuaState * L)
	{
		g_this->clicked();
		return 0;
	}
	int buy_item_slot2( LuaState * L)
	{
		INT32 tab = i3Lua::GetIntegerArg( L, 1);
		g_this->clicked_combo(tab);
		return 0;
	}
}

LuaFuncReg buy_item_slot_glue[] =
{
	{ "Click",			buy_item_slot	},
	{ "ClickCombo",		buy_item_slot2	},
	{ nullptr,							nullptr}
};

ShopPopupBuyItemSlot::ShopPopupBuyItemSlot() : mParent(0), m_nRow(8), m_nCol(1)
{
	m_nSlot = m_nRow * m_nCol;
	g_this = this;
	mSlot.EnableScroll(false);
}

void ShopPopupBuyItemSlot::clicked()
{
	buy::RemoveByPurchaseRoute( mParent->getPurchaseRoute(), get_now_slot() );

	update_list();

//	UI::call_cb("update_cart_board");	//상품 하나 지웟으니 cart board를 갱신한다.
	UISignal::UpdateCartBoard();		//상품 지웟으니 cart board를 갱신한다.
}

void ShopPopupBuyItemSlot::clicked_combo(INT32 tab)
{
	//combo box에 unfolder 된 menu에 항목 선택시에 now slot idx 가 변경 될 수 있기에 unfolder 될 때 slot idx를 얻는다.
	if( tab == -1 ) { g_curr_slot_idx = get_now_slot(); return; }

	buy::DisplayedGoods* pDisplayedGoods = buy::GetDisplayedGoods_ByPurchaseRoute(mParent->getPurchaseRoute(), g_curr_slot_idx);

//	I3ASSERT_RETURN(pDisplayedGoods);
	pDisplayedGoods->SetCurrentDisplayIndex(tab);


	/*if(g_pFramework->GetUIMainframe()->GetCurrentSubPhaseType() == USP_GACHA)
	{
		if( tab == -1 )
			tab = 0;

		GachaContext::i()->SetGachaBuyType((UINT8)tab);
	}*/

	update_list();
}

void ShopPopupBuyItemSlot::update_list()
{
	mParent->update_item_paymenu();
	mSlot.UpdateList( buy::GetCount_ByPurchaseRouteType( mParent->getPurchaseRoute() ) );
}


void ShopPopupBuyItemSlot::OnCreate( i3GameNode * pParent)
{
	m_nSlot = m_nRow * m_nCol;

	//Slot 만들고
	mSlot.EnableScroll( false );
	mSlot.OnCreate( (UIBase*)mParent, m_nRow, m_nCol);
//	char fullPath[MAX_PATH];
//	i3::snprintf(fullPath, sizeof(fullPath), "Scene/Popup/PBRe_PopUp_ShoppingCart_Slot.i3UIs", i3UI::getManager()->GetCommonLocaleUIFolder()->c_str());
	mSlot.RegisterSlotScene("Scene/Popup/PBRe_PopUp_ShoppingCart_Slot.i3UIs", buy_item_slot_glue);
}

void ShopPopupBuyItemSlot::OnLoadAllScenes()
{
	i3UIFrameWnd* temp_child = (i3UIFrameWnd*)( mParent->GetScene(0)->FindChildByName( "i3UIFrameWnd" ) );
	if( temp_child != nullptr )
	{
		VEC3D* temp_pos = temp_child->getAccumulatedPos();
		mSlot.OnLoadAllScenes((INT32)temp_pos->x, (INT32)temp_pos->y, 0,  0);
	}else
		mSlot.OnLoadAllScenes(227, 80, 0,  0);
	

	update_list();

	//combo 박스 때문에 slot depth 변경
	mSlot.ReverseScene();

	mSlot.SetScrollFocus();
}

void ShopPopupBuyItemSlot::OnUnloadAllScenes()
{
	mSlot.OnUnloadAllScenes();
}

void ShopPopupBuyItemSlot::OnUpdate( REAL32 rDeltaSeconds)
{
	mSlot.OnUpdate();
}

void ShopPopupBuyItemSlot::OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code)
{
	mSlot.OnEvent(event, pObj, param2, code);
}

void	ShopPopupBuyItemSlot::cb_Enabled_impl( INT32 slot_idx, i3UIScene* slot_scn)
{
	const buy::DisplayedGoods* pDisplayedGoods = buy::GetDisplayedGoods_ByPurchaseRoute(mParent->getPurchaseRoute(), slot_idx);
	i3::rc_wstring wstr_name;

	const SHOP_GOODS* pCurrGoods = pDisplayedGoods->GetCurrentGoods();

	usf::GetShopItemName(wstr_name, pCurrGoods);

	bool is_red_name = false;

	if( item_def::is_chara_category(pCurrGoods->_ItemID) == true ) //캐릭터를 구입 할 수 없는 경우
		is_red_name = !is_buyable_chara;
	else
		is_red_name = (usf::IsValidBuyGoods( pDisplayedGoods->GetGoodsPack() ) == usf::vbt_Ok) ? false : true;

	if( is_red_name == true ) 
	{
		::ColorString(wstr_name, 255, 0, 0);
	}

	LuaState* L = 0;
	L = UILua::CallLuaFunction( slot_scn, "SetSlot");
	i3Lua::PushStringUTF16To8(L, wstr_name.c_str(), wstr_name.size() );
	UILua::EndLuaFunction(L , 1);

	i3::wstring wstr_nameList;	
	if(g_pFramework->GetUIMainframe()->GetCurrentSubPhaseType() == USP_GACHA )
	{
		if(GachaContext::i()->GetGachaBuyType() == GOODS_BUY_CASH)
		{
			i3::sprintf(wstr_nameList, L"%d CASH", GachaContext::i()->GetGachaBuyCashPrice());
		}
		else if(GachaContext::i()->GetGachaBuyType() == GOODS_BUY_POINT)
		{
			i3::sprintf(wstr_nameList, L"%d POINT", GachaContext::i()->GetGachaBuyPointPrice());
		}
		else
		{		
			if(GachaContext::i()->GetGachaBuyCoupon() > 0)
			{ // Coin으로 살 수 있을경우 coin 추가
				i3::sprintf(wstr_nameList, L"%d COIN", GachaContext::i()->GetGachaBuyCoupon());
			}
		}
	}
	else
		wstr_nameList = usf::payment::get_price_combo_string( pDisplayedGoods->GetGoodsPack() );

	L = UILua::CallLuaFunction( slot_scn, "SetFilterList");
	i3Lua::PushStringUTF16To8(L, wstr_nameList);
	i3Lua::PushInteger(L, pDisplayedGoods->GetCurrentDisplayIndex()); //  item.display_inx );
	UILua::EndLuaFunction(L , 2);
}

void ShopPopupBuyItemSlot::cb_Disabled_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	slot_scn->EnableScene(false);
}
