#include "pch.h"
#include "UIMultiTabSlot.h"

#include "UIPhaseShop.h"
#include "ItemImageMgr.h"
#include "UIShopFunction.h"

#include "UIUtil.h"
#include "UICharaInfoMgr.h"

#include "UIMainFrame.h"
#include "UISideMenu.h"

#include "PurchaseMgr.h"
#include "UIPopupBuyItem.h"

#include "UISignal.h"
#include "i3Base/itl/bind/bind.h"
#include "i3Base/itl/bind/placeholders.h"


/********************************/
iMultiTabSlot::iMultiTabSlot(iMultiTap *parent) : mParent(parent), mImageMgr( GetItemImageMgr() )
{
	mDpImgSetCtrl = nullptr;
	mDpScene = nullptr;
	mDpAttScene = nullptr;
	mCategory = item_def::CTG_NA;
	mType = 0;
	use_combo_type = false;
}

void iMultiTabSlot::update_slot( bool b1, bool b2) 
{
	//arg1에 따라서 기존 그대로 갱신 / 완전 갱신
	if( b1 )
	{
		if(use_combo_type)
			Entrance((item_def::WeaponComboType)mType);
		else 
			Entrance(mCategory, mType);

		if( b2 )
			mParent->hide_display_info();
	}
	else
	{
		Entrance();
	}
}

/********************************/
void iMultiTabSlot::OnLoadAllScenes()
{
	UINT32 iAddsize = 0;
	if (g_pEnvSet->IsV2Version() == false)
		iAddsize = 8;

//	UI::insert_cb("update_slot9", this, &iMultiTabSlot::update_slot );
	m_SigConnection += UISignal::UpdateSlot9.connect(i3::bind(&iMultiTabSlot::update_slot, this, i3::_1, i3::_2));

	VEC3D pos = *(mParent->GetSlotPos());
	mSlot.OnLoadAllScenes((INT32)pos.x, (INT32)pos.y, 2,  2, iAddsize);
	mSlot.SetEnableMouseOver(true);

	UI::tmSlideAtt * pSlide = new UI::tmSlideAtt( mSlot.GetScrollCtrl().GetFrameWnd() );
	MEMDUMP_NEW( pSlide, sizeof( UI::tmSlideAtt));
	mParent->add_slide( pSlide);

	Entrance();
}

void iMultiTabSlot::OnRevive()
{
}

void iMultiTabSlot::OnUnloadAllScenes()
{
	m_SigConnection.clear();
//	UI::remove_cb("update_slot9", this );
	
	mSlot.OnUnloadAllScenes();
}

void iMultiTabSlot::OnUpdate( REAL32 rDeltaSeconds)
{
	mSlot.OnUpdate();
	update_impl(rDeltaSeconds);
}

void iMultiTabSlot::OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code)
{
	mSlot.OnEvent(event, pObj, param2, code);
}

/********************************/
void iMultiTabSlot::cb_Load_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	cb_Unload_impl(slot_idx, slot_scn);
	m_ppImgSet[slot_idx] = mImageMgr->CreateImgSetCtrl(slot_scn, "i3UIButtonImageSet");

	i3UIFrameWnd* pFrm = (i3UIFrameWnd*)slot_scn->FindChildByName("i3UIFrameWnd0");
	UI::tmSlideAtt * pSlide = new UI::tmSlideAtt(pFrm);
	MEMDUMP_NEW( pSlide, sizeof( UI::tmSlideAtt));
	mParent->add_slide( pSlide);
}

void iMultiTabSlot::cb_Unload_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	if( m_ppImgSet[slot_idx] )
		m_ppImgSet[slot_idx]->RestoreSizePos();
	I3_SAFE_RELEASE(m_ppImgSet[slot_idx]);
}

void	iMultiTabSlot::cb_Enabled_impl( INT32 slot_idx, i3UIScene* slot_scn)
{
	INT32 index = mSlot.GetStartInx() + slot_idx;
	update_item_info(m_ppImgSet[slot_idx], index, slot_scn );
}

void iMultiTabSlot::cb_Disabled_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	slot_scn->EnableScene(false);
}
void iMultiTabSlot::cb_Enter_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	LuaState * L = UILua::CallLuaFunction( slot_scn, "Enter");
	UILua::EndLuaFunction(L , 0);
}
void iMultiTabSlot::cb_Leave_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	LuaState * L = UILua::CallLuaFunction( slot_scn,  "Leave");
	UILua::EndLuaFunction(L , 0);
}


/********************************/
void iMultiTabSlot::set_display_item(UIImgSetCtrl* pSetCtrl, i3UIScene* dp_scn, 	i3UIScene* att_scn)
{
	mDpImgSetCtrl = pSetCtrl;
	mDpScene = dp_scn;
	mDpAttScene = att_scn;
}

/********************************/
void iMultiTabSlot::update_empty_item_attribute()
{
	switch( mCategory )
	{
	case item_def::CTG_WEAPON:	update_weapon_attribute_impl( (T_ItemID)-1, "SetWeaponInfo");	break;
	case item_def::CTG_CHARA:	update_chara_attribute_impl((T_ItemID)-1);	break;
	case item_def::CTG_PARTS:	update_parts_attribute_impl((T_ItemID)-1);	break;
	case item_def::CTG_SKIN:	update_parts_attribute_impl((T_ItemID)-1);	break;
	case item_def::CTG_ITEM:	update_item_attribute_impl((T_ItemID)-1);	break;
	}
}

void iMultiTabSlot::update_weapon_attribute_impl(T_ItemID itemId, const char* lua_name)
{
	i3::stack_wstring wstrRange;
	i3::stack_wstring wstrBulletCount;

	INT32 damage = 0, rapid = 0, accuracy = 0, recoil_v = 0, recoil_h = 0, speed = 0;

 	if( itemId != -1 )
	{
		LuaState* L = UILua::CallLuaFunction(mDpAttScene, "EnableWeaponInfo");
		i3Lua::PushBoolean(L, true);
		UILua::EndLuaFunction(L, 1);

		if (::strcmp(lua_name, "SetWeaponInfo2") == 0)
		{
			L = UILua::CallLuaFunction(mDpAttScene, "EnableWeaponInfo2");
			i3Lua::PushBoolean(L, true);
			UILua::EndLuaFunction(L, 1);
		}

		GameUI::GetNormalizedWeaponInfo( itemId, 
			damage, rapid, accuracy, 
			recoil_v, recoil_h, speed, 
			wstrRange, wstrBulletCount);
	}
	else
	{
		if (::strcmp(lua_name, "SetWeaponInfo2") == 0)
		{
			LuaState* L = UILua::CallLuaFunction(mDpAttScene, "EnableWeaponInfo2");
			i3Lua::PushBoolean(L, false);
			UILua::EndLuaFunction(L, 1);
		}
		else
		{
			LuaState* L = UILua::CallLuaFunction(mDpAttScene, "EnableWeaponInfo");
			i3Lua::PushBoolean(L, false);
			UILua::EndLuaFunction(L, 1);
		}
	}

	LuaState * L = UILua::CallLuaFunction( mDpAttScene, lua_name );

	i3Lua::PushNumber(L, float(damage) / 100.f);
	i3Lua::PushNumber(L, float(rapid) / 100.f);
	i3Lua::PushNumber(L, float(accuracy) / 100.f);
	i3Lua::PushNumber(L, float(recoil_v) / 100.f);
	i3Lua::PushNumber(L, float(recoil_h) / 100.f);
	i3Lua::PushNumber(L, float(speed) / 100.f);

	i3::stack_wstring wstr_temp;
	
	GameUI::GetGaugeString(damage, wstr_temp);
	i3Lua::PushStringUTF16To8(L, wstr_temp);
	GameUI::GetGaugeString(rapid, wstr_temp);
	i3Lua::PushStringUTF16To8(L, wstr_temp);
	GameUI::GetGaugeString(accuracy, wstr_temp);
	i3Lua::PushStringUTF16To8(L, wstr_temp);
	GameUI::GetGaugeString(recoil_v, wstr_temp);
	i3Lua::PushStringUTF16To8(L, wstr_temp);
	GameUI::GetGaugeString(recoil_h, wstr_temp);
	i3Lua::PushStringUTF16To8(L, wstr_temp);
	GameUI::GetGaugeString(speed, wstr_temp);
	i3Lua::PushStringUTF16To8(L, wstr_temp);

	i3Lua::PushStringUTF16To8(L, wstrRange );
	i3Lua::PushStringUTF16To8(L, wstrBulletCount );

	UILua::EndLuaFunction(L, 14);
}

void iMultiTabSlot::update_chara_attribute_impl(T_ItemID itemId)
{
	i3::rc_wstring wstr_buf1, wstr_buf2;
	usf::GetShopItemName( wstr_buf1, itemId);
	usf::GetShopItemDescription( wstr_buf2, itemId);

	i3UIEditBox * pEditBox = (i3UIEditBox *)mDpAttScene->FindChildByName("i3UIEditBox");
	pEditBox->getScrollBar()->setScrollPos(0);
	
	LuaState * L = UILua::CallLuaFunction( mDpAttScene, "SetCharaInfo" );
	i3Lua::PushStringUTF16To8(L,wstr_buf1.c_str(), wstr_buf1.size());
	i3Lua::PushStringUTF16To8(L,wstr_buf2.c_str(), wstr_buf2.size());
	UILua::EndLuaFunction(L,2);

	pEditBox->setScrollState(i3UIEditBox::UBSS_INIT);

}

void iMultiTabSlot::update_parts_attribute_impl(T_ItemID itemId)
{
	i3::rc_wstring wstr_temp;
	usf::GetShopItemDescription(wstr_temp, itemId);

	i3UIEditBox * pEditBox = (i3UIEditBox *)mDpAttScene->FindChildByName("i3UIEditBox0");
	//pEditBox->getScrollBar()->EnableCtrl(true);
	pEditBox->getScrollBar()->setScrollPos(0);

	LuaState * L = UILua::CallLuaFunction( mDpAttScene, "SetItemInfo" );
	i3Lua::PushStringUTF16To8(L, wstr_temp.c_str(), wstr_temp.size());
	UILua::EndLuaFunction(L, 1);

	//pEditBox->setScrollState(i3UIEditBox::UBSS_INIT);

}

void iMultiTabSlot::update_item_attribute_impl(T_ItemID itemId)
{
	update_parts_attribute_impl(itemId);	//동일한 기능
}

void iMultiTabSlot::update_item_attribute_impl(SHOP_GOODS* goods)
{
	i3::rc_wstring wstr_temp;
	usf::GetShopItemDescription(wstr_temp, goods);

	i3UIEditBox * pEditBox = (i3UIEditBox *)mDpAttScene->FindChildByName("i3UIEditBox0");
	//pEditBox->getScrollBar()->EnableCtrl(true);
	pEditBox->getScrollBar()->setScrollPos(0);

	LuaState * L2 = UILua::CallLuaFunction( mDpAttScene, "SetItemInfo" );
	i3Lua::PushStringUTF16To8(L2, wstr_temp.c_str(), wstr_temp.size());
	UILua::EndLuaFunction(L2, 1);

	//pEditBox->setScrollState(i3UIEditBox::UBSS_INIT);
}