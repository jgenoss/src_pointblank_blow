#include "pch.h"
#include "UIBattleShopSlot.h"

#include "UIBattlePopupShop.h"
#include "UIBattleFrame.h"
#include "PurchaseMgr.h"
#include "ItemImageMgr.h"
#include "BuyUtil.h"
#include "UIShopFunction.h"

#include "UIUtil.h"
#include "UIItemInfo_Def.h"
#include "ui/UIMath.h"

namespace
{
	BattleShopSlot* g_this = 0;

#if ENABLE_BATTLESHOP_SCROLL 
	static const REAL32 target_time = 1.f;

	bool enable_ice_cream_scroll = false;
	bool act_ice_cream_scroll = false;
	REAL32 elapse_time = 0.f;
#endif
}

extern "C" 
{
	int battle_shop_item_slot_clicked( LuaState * L)
	{
		g_this->clicked();
		return 0;
	}
	int battle_shop_item_slot_clicked2( LuaState * L)
	{
		g_this->double_clicked();
		return 0;
	}
}

LuaFuncReg battle_shop_slot_glue[] =
{
	{ "Click", battle_shop_item_slot_clicked	},
	{ "Click2", battle_shop_item_slot_clicked2	},
	{ nullptr, nullptr}
};


BattleShopSlot::BattleShopSlot(UIBattlePopupShop* p) : mParent(p)
{
	g_this = this;

#if ENABLE_BATTLESHOP_SCROLL 
	enable_ice_cream_scroll = false;
	act_ice_cream_scroll = false;
#endif
}

BattleShopSlot::~BattleShopSlot()
{
}

///************************/
void BattleShopSlot::clicked()
{
	selected_slot_idx = iSlot::get_now_slot();
	mSlot.UpdateList( m_item_list.m_list.size() );
}

void BattleShopSlot::double_clicked()
{
	INT32 slot_idx = iSlot::apply_select();
	if( slot_idx == -1 ) return;

	selected_slot_idx = slot_idx;

	SelectSlot();
}


//************************/
void BattleShopSlot::OnCreate( i3GameNode * pParent)
{
	mSlot.EnableScroll( false );
	mSlot.OnCreate( (UIBase*)mParent, BsS::ROW, BsS::COL);
	mSlot.RegisterSlotScene("Scene/HUD/IngameShop_itemSlot.i3UIs", battle_shop_slot_glue, true);

	m_ppImgSet = new UIImgSetCtrl*[BsS::SLOT];
	MEMDUMP_NEW( m_ppImgSet, sizeof( UIImgSetCtrl*) * BsS::SLOT);

	::memset( m_ppImgSet, 0, sizeof(UIImgSetCtrl*) * BsS::SLOT);
}

void BattleShopSlot::OnLoadAllScenes()
{
	VEC3D* main_pos = mParent->get_left_top();

	mSlot.OnLoadAllScenes( (INT32)main_pos->x, (INT32)main_pos->y,0,0);
	mSlot.SetEnableMouseOver(true);
	mSlot.SetVisible(false);
}

void BattleShopSlot::OnUnloadAllScenes()
{
#if ENABLE_BATTLESHOP_SCROLL 
	enable_ice_cream_scroll = false;
#endif

	mSlot.OnUnloadAllScenes();
}

void BattleShopSlot::OnUpdate( REAL32 rDeltaSeconds)
{
	mSlot.OnUpdate();

#if ENABLE_BATTLESHOP_SCROLL 
	if( enable_ice_cream_scroll == true && act_ice_cream_scroll == true )
	{
		elapse_time -= rDeltaSeconds;

		REAL32 factor = uiMath::remap_ranges(0.f, 1.f, elapse_time, 0.f, target_time);
		factor = uiMath::limited(factor, 0.f, 1.f);
		mSlot.GetScrollCtrl().SetAlpha(factor);

		if(factor == 0.f) act_ice_cream_scroll = false;
	}
#endif
}

#if ENABLE_BATTLESHOP_SCROLL 
void BattleShopSlot::OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code)
{
	mSlot.OnEvent(event, pObj, param2, code);
}

/************************/
void BattleShopSlot::hit_test_enter()
{
	mSlot.GetScrollCtrl().SetAlpha(1.f);
	mSlot.SetScrollFocus();
}

void BattleShopSlot::hit_test_leave()
{
	elapse_time = target_time;
	act_ice_cream_scroll = true;
}
#endif

/************************/
void BattleShopSlot::Entrance(UINT32 tap)
{
	mSlot.SetVisible(true);

	m_item_list.list_up(tap);

	selected_slot_idx = -1;

	INT32 size = m_item_list.m_list.size();
	iSlot::reset_select( size );
	mSlot.UpdateList( size );

#if ENABLE_BATTLESHOP_SCROLL 
	mSlot.SetScrollFocus();
	enable_ice_cream_scroll = (size > SLOT) ? true : false;
#endif
}

void BattleShopSlot::MoveBackScene()
{
	mSlot.MoveBackScene();
}

void BattleShopSlot::SelectSlot()
{
	if( selected_slot_idx == -1 ) return;

	const SHOP_GOODS_PACK* pack = m_item_list.m_list[selected_slot_idx].info;
	if( usf::CheckValidGoods(pack, true) == false ) return;

	buy::PushImmediate(pack);

	UIBattleFrame::i()->OpenPopup(UBP_BUYITEM, (void*)buy::PurchaseRoute_Immediate);
	mParent->is_using_normal = true;
}

void BattleShopSlot::SelectSlot(INT32 sel_idx)
{
	if (selected_slot_idx == sel_idx ) return;
	selected_slot_idx = sel_idx;
	mSlot.UpdateList( m_item_list.m_list.size() );
}

/************************/
void BattleShopSlot::cb_Load_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	m_ppImgSet[slot_idx] = GetItemImageMgr()->CreateImgSetCtrl(slot_scn, "i3UIButtonImageSet_item_IIS");
}

void BattleShopSlot::cb_Unload_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	if( m_ppImgSet[slot_idx] )
		m_ppImgSet[slot_idx]->RestoreSizePos();
	I3_SAFE_RELEASE(m_ppImgSet[slot_idx]);
}

void	BattleShopSlot::cb_Enabled_impl( INT32 slot_idx, i3UIScene* slot_scn)
{
	slot_scn->EnableScene(true);

	INT32 index = mSlot.GetStartInx() + slot_idx;

	const SHOP_GOODS_PACK* shop_info = m_item_list.m_list[index].info;
	SHOP_GOODS* goods = shop_info->GetGoods(0);

	GetItemImageMgr()->SelectImage(m_ppImgSet[slot_idx], goods);

	i3::rc_wstring wstr_name; usf::GetShopItemName(wstr_name, goods);
	i3::wstring wstr_perido; usf::GetShopPeriodCountString(goods, wstr_perido);
	i3::wstring wstr_price; usf::GetShopPriceString(goods, wstr_price);

	{
		LuaState * L = UILua::CallLuaFunction(slot_scn, "InitSlot");
		i3Lua::PushStringUTF16To8(L, wstr_name);
		i3Lua::PushStringUTF16To8(L, wstr_perido);
		i3Lua::PushStringUTF16To8(L, wstr_price);
		UILua::EndLuaFunction(L, 3);
	}

	if( index == selected_slot_idx )
	{
		LuaState * L = UILua::CallLuaFunction( slot_scn, "Enter");
		UILua::EndLuaFunction(L , 0);
	}
	else
	{
		LuaState * L = UILua::CallLuaFunction( slot_scn,  "Leave");
		UILua::EndLuaFunction(L , 0);
	}
}

void BattleShopSlot::cb_Disabled_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	slot_scn->EnableScene(true);

	m_ppImgSet[slot_idx]->SetEnable(false);
	LuaState * L = UILua::CallLuaFunction( slot_scn, "InitSlot");
	i3Lua::PushStringUTF16To8( L, L"" );
	i3Lua::PushStringUTF16To8( L, L"" );
	i3Lua::PushStringUTF16To8( L, L"" );
	UILua::EndLuaFunction(L , 3);

	UILua::CallLuaFunction( slot_scn,  "Leave");
	UILua::EndLuaFunction(L , 0);
}

void BattleShopSlot::cb_Enter_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
//	LuaState * L = UILua::CallLuaFunction( slot_scn, "Enter");
//	UILua::EndLuaFunction(L , 0);
}
void BattleShopSlot::cb_Leave_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
//	LuaState * L = UILua::CallLuaFunction( slot_scn,  "Leave");
//	UILua::EndLuaFunction(L , 0);
}
