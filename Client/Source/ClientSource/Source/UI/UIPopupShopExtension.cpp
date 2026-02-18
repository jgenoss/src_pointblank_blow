#include "pch.h"
#include "UIPopupShopExtension.h"

#include "UIMainFrame.h"
#include "UIShopFunction.h"
#include "UIClanFunction.h"
#include "ItemImageMgr.h"
#include "BuyUtil.h"
#include "UIUtil.h"
#include "GameGUI.h"
#include "PurchaseMgr.h"
#include "UIFloatCharacterView.h"
#include "InvenUtil.h"
#include "UICharaInfoMgr.h"
#include "UserInfoContext.h"
#include "UISideMenu.h"
#include "UISMenu_Ability.h"
#include "UISMenu_Equip.h"
#include "MainWeapon_Dual.h"
#include "MainWeapon_Knuckle.h"
#include "MainWeapon_DualGrenade.h"
#include "MainWeapon_C5.h"
#include "MainWeapon_DualCIC.h"
#include "MainWeapon_DualUzi.h"
#include "i3Base/string/ext/sprintf.h"
#include "i3Base/string/ext/erase_string.h"
#include "i3Base/string/ext/insert_string.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "../../../../Common/CommonSource/CommonDef.h"

#include "UISignal.h"
#include "i3Base/itl/bind/bind.h"
#include "i3Base/itl/bind/placeholders.h"


I3_CLASS_INSTANCE( UIPopupShopExtension);//, UIPopupBase);

namespace
{
	enum	{	BUY=0,	CANCEL, ESC, };
	enum	{	MAX_PRICE = 4, };
	enum    {   D_DAMAGE, D_RAPID, D_HITRATE, D_SPEED, D_VCOIL, D_HCOIL, D_BULLET, D_RANGE, };

	UINT32 g_curr_slot_idx = 0;
	UIPopupShopExtension* g_this = 0;

	//Lua 경유해서 ClosePopup 할 경우 LuaStack이 종료되기 전에 날라가기 때문에
	//크래쉬 발생한다. 나쁘지만 간단하게 update 시점에서 ClosePopup 하면 된다
	bool g_close_popup = false;

	void set_close()
	{
		UISignal::FinishBuyPopup(false, false);		//상점 slot을 업데이트 한다.
		//	UI::call_cb( "finish_buy_popup"); //상점 slot을 업데이트 한다.
		g_close_popup = true;
	}

	/************************************************************************/
	/*							3D Viewer namespace							*/
	/************************************************************************/
	// magic number
	const VEC2D		BG_SPRITE_SIZE	= VEC2D(130.0f, 75.0f);
	const REAL32	BG_SPRITE_POS_Z	= -90.0f;

	const VEC3D		EFFECT_POS		= VEC3D(-2.5f, 0.0f, -1.0f);
	const REAL32	EFFECT_ROT_Z	= -(I3_PI2);
	//////////////////////////////////////////////////////////////////////////
}

UIPopupShopExtension::UIPopupShopExtension() : 
				view_frame_wnd(nullptr),
				m_pChara(nullptr), 
				isOpenInventory(false),
				m_pWeapon3DView(nullptr)
{
	g_this = this;
}
UIPopupShopExtension::~UIPopupShopExtension()
{

}

extern "C" 
{
	int click( LuaState* L )
	{
		INT32 value = i3Lua::GetIntegerArg( L, 1);
		g_this->clicked(value);
		return 0;
	}
	int click_combo( LuaState* L )
	{
		INT32 value = i3Lua::GetIntegerArg( L, 1);
		g_this->clicked_combo(value);
		return 0;
	}
}

LuaFuncReg UIPopupShopExtension_Glue[] =
{
	{ "Click", click },
	{ "ClickCombo", click_combo },
	{nullptr,			nullptr}
};

void UIPopupShopExtension::clicked(INT32 idx)
{
	switch( idx ) 
	{
	case BUY:
		{
		
			SHOP_GOODS* goods = m_buyable_weapon_item->GetGoodsPack()->GetGoods(0);
			if( usf::PopupRestriction(goods->_ItemID) == false) return;

			buy::PushImmediate(m_buyable_weapon_item->GetGoodsPack(), m_buyable_weapon_item->GetCurrentDisplayIndex());		// 포인터가 아니라 값복사를 행한다..

			GetMainFrame()->OpenPopup(UPW_BUYITEM, (void*)buy::PurchaseRoute_Immediate, &UISignal::FinishExtensionBuyItem); //"finish_extension_buy_item");	//즉시구매
		}
		break;
	case ESC:
	case CANCEL:
	{
		set_close();	
		if(m_pWeapon3DView)
			m_pWeapon3DView->ReleaseWeaponObj();
	}
		break;
	}
}
void UIPopupShopExtension::clicked_combo(INT32 tab)
{
	//combo box에 unfolder 된 menu에 항목 선택시에 now slot idx 가 변경 될 수 있기에 unfolder 될 때 slot idx를 얻는다.
	if( tab == -1 ) { return; }

	m_buyable_weapon_item->SetCurrentDisplayIndex(tab);			// 이렇게 간단해야했다..

	update_payment();
}

//------------------------------------------------------------------------------//

void UIPopupShopExtension::cb_finish_buy_item(bool arg1, bool arg2)
{
	bool result_buy_item = arg1; // ((BOOL)arg1 == TRUE) ? true : false;

	if(result_buy_item == true)
	{
		const SHOP_GOODS* pGoods = m_buyable_weapon_item->GetCurrentGoods(); //현재 선택된 goods
			
		PurchaseMgr::i()->Auth_Shop(this, pGoods, true); //  "finish_extension_buy_auth_item", pGoods);

		buy::Clear_ByPurchaseRoute(buy::PurchaseRoute_Immediate);
	}

	if(arg2 == false)    //(BOOL)arg2 == FALSE ) //buy button 클릭 상태
		set_close();
}
void UIPopupShopExtension::cb_finish_buy_auth_item(bool arg1, const SHOP_GOODS* arg2)
{
//	if( (BOOL)arg1 == FALSE ) return;
	if (arg1 == false)
		return;

	const SHOP_GOODS* buy_goods = arg2; // static_cast<const SHOP_GOODS*>(arg2);
	ItemInfoConvert item_info( buy_goods->_ItemID );

	//주무기만 변경하자.
	CharaInfoMgr::CurrentState & state = CharaInfoMgr::instance()->m_current_state;

	state.ctg = item_info.get_side_category();
	state.info.m_TItemID = buy_goods->_ItemID;
	state.info.m_TItemDBIdx = PurchaseMgr::i()->GetTargetItemDBIndex();
	state.slot_idx = static_cast<UINT8>(item_info.get_char_slot_idx());

	//장비 변경 교체 한다.
	//g_pFramework->OnEvent( UIEVENT_SMENU_CHANGE_ITEM, nullptr, 0, I3_EVT_CODE_ACTIVATE);
	if( UISideMenu::instance() != nullptr)
	{
		//아이템 교환
		SMenuEquip * pSMenuEquip = static_cast<SMenuEquip*>(UISideMenu::instance()->getSubSidemenu(SIDEMENU_BTN_EQUIP));
		if( pSMenuEquip != nullptr)
			pSMenuEquip->UIEvent_ChangedCharaEquip();	// 현재는 이녀석이 메니져 역할이라.. 직접 호출.. 따로 메니징할 수 있는 중간 함수로 대체해야 한다.

		//장비 변경 교체 한다.
		UISideMenu::instance()->send_change_equip();
	}
}

//------------------------------------------------------------------------------//

/*virtual*/ void UIPopupShopExtension::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad( pScene);

	i3UIButtonComposed3* btn = (i3UIButtonComposed3*)GetScene(0)->FindChildByName( "i3UIButtonComposedApply" );
	btn->set_disable_text_color(*GameGUI::GetColor(GCT_INVALID_C) );

	view_frame_wnd = (i3UIFrameWnd*)pScene->FindChildByName( "i3UI3DViewWnd" );
	I3ASSERT(view_frame_wnd != nullptr);
}
/*virtual*/ void UIPopupShopExtension::_ClearAtUnload( i3UIScene * pScene)
{
	UIPopupBase::_ClearAtUnload( pScene);

	i3UIButtonComposed3* btn = (i3UIButtonComposed3*)GetScene(0)->FindChildByName( "i3UIButtonComposedApply" );
	btn->enable_btn();
}

//------------------------------------------------------------------------------//

/*virtual*/ void UIPopupShopExtension::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	// Load Scene
	AddScene("Scene/Main/PBRe_Weapon_Detail_New.i3UIs", UIPopupShopExtension_Glue, true, false);
}
/*virtual*/ bool UIPopupShopExtension::OnEntranceStart( void * pArg1/* = nullptr*/, void * pArg2/* = nullptr*/)
{
	g_close_popup = false;

	m_SigConnection += UISignal::FinishExtensionBuyItem.connect(i3::bind(&UIPopupShopExtension::cb_finish_buy_item, this, i3::_1, i3::_2));
	m_SigConnection += UISignal::FinishExtensionBuyAuthItem.connect(i3::bind(&UIPopupShopExtension::cb_finish_buy_auth_item, this, i3::_1, i3::_2));

//	UI::insert_cb( "finish_extension_buy_item", this, &UIPopupShopExtension::cb_finish_buy_item);
//	UI::insert_cb( "finish_extension_buy_auth_item", this, &UIPopupShopExtension::cb_finish_buy_auth_item);
	
	m_HitTestCallback.SetObj(this);
	m_tmHitTest.SetCallback(&m_HitTestCallback);
	m_tmHitTest.add(view_frame_wnd);

	m_pWeapon3DView = new WeaponDetail3DView();

	pArg2 ? isOpenInventory = (*(bool*)pArg2) : isOpenInventory = false;
	if( pArg1 )
	{
		if( !isOpenInventory )
		{
			m_buyable_weapon_item = buy::MakeDisplayedGoodsPtr(reinterpret_cast<const SHOP_GOODS_PACK*>(pArg1));
		//	buy::item_util::set(m_buyable_weapon_item, (SHOP_GOODS_PACK*)pArg1, usf::g_price_idx);

			m_nItemID = m_buyable_weapon_item->GetCurrentGoods()->_ItemID; // 	buy::item_util::get_item_id(m_buyable_weapon_item);
		}
		else
			m_nItemID = *static_cast<T_ItemID*>(pArg1);
	
		m_pWeapon3DView->SetAttachFrameWnd(view_frame_wnd);
		m_pWeapon3DView->Weapon3DViewCreate(m_nItemID);
	}

	is_mouse_pt_in_frame_wnd = false;
	is_mouse_leftbtn_down = false;

	GameUI::ResetMousePressedUp();
	GetSlide()->ForceEnd();
	
	SetEnable( true );

	return true;
}
/*virtual*/ void UIPopupShopExtension::OnLoadAllScenes()
{
	UIPopupBase::OnLoadAllScenes();

	char templet_name[MAX_PATH] = {};
	for (int i = 1; i < 9; i++)
	{
		i3::snprintf(templet_name, sizeof(templet_name), "i3UITextRate%d", i);
		text_rate[i - 1] = (i3UIStaticText*)GetScene(0)->FindChildByName(templet_name);
		i3::snprintf(templet_name, sizeof(templet_name), "i3UIProgressBar%d", i);
		progress_rate[i - 1] = (i3UIProgressBar*)GetScene(0)->FindChildByName(templet_name);
	}

	update_weapon_attribute();
	update_weapon_attribute_detail();
	update_weapon_class_used(m_nItemID);
	update_payment();

	if( isOpenInventory )
	{
		i3UIFrameWnd*		PointFrame  = (i3UIFrameWnd*)GetScene(0)->FindChildByName("i3UIFrame_BT");
		PointFrame->EnableCtrl(false, true);
	}

#if defined(_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)

	output_ui = (i3UIStaticText*)GetScene(0)->FindChildByName("TEST_TEXT");
	output_ui->SetVisible(false);

#endif

} 
bool UIPopupShopExtension::OnExitStart( void)
{
//	UI::remove_cb(this);
	m_SigConnection.clear();

	g_pFramework->setShadowQuality( g_pEnvSet->m_nShadowQualityType, false );

	SetEnable( false );

	//m_pWeapon3DView->ReleaseWeaponObj();

	return true;
}

/*virtual*/ void UIPopupShopExtension::OnUnloadAllScenes()
{
	if (m_pWeapon3DView)
	{
		I3_SAFE_DELETE(m_pWeapon3DView);
	}
}

/***********************/
bool UIPopupShopExtension::enable_buy()
{
	const SHOP_GOODS* pGoods = m_buyable_weapon_item->GetCurrentGoods(); //현재 선택된 goods
	
	GOODS_PRICE_INFO info;		
	usf::GetShopGoodInfo(pGoods, info);

	if( info._Point > 0 )
		return UserInfoContext::i()->GetMyPoint() >= info._Point;

	return UserInfoContext::i()->GetMyCash() >= info._Cash;
}
void UIPopupShopExtension::update_payment()
{
	if( isOpenInventory )
		return;

	i3::wstring wstr_nameList = usf::payment::get_price_combo_string( m_buyable_weapon_item->GetGoodsPack() );

	LuaState* L = _CallLuaFunction( "SetFilterList");
	i3Lua::PushStringUTF16To8(L, wstr_nameList);
	i3Lua::PushInteger(L, m_buyable_weapon_item->GetCurrentDisplayIndex() );
	UILua::EndLuaFunction(L , 2);

	i3UIButtonComposed3* btn = (i3UIButtonComposed3*)GetScene(0)->FindChildByName( "i3UIButtonComposedApply" );
	if(enable_buy() == false)
		btn->disable_btn();
	else
		btn->enable_btn();
}
void UIPopupShopExtension::update_weapon_attribute()
{	
	i3UIStaticText* weapon_title = (i3UIStaticText*)GetScene(0)->FindChildByName("i3UIWeapon_Title2");
	i3UIStaticText* weapon_desc = (i3UIStaticText*)GetScene(0)->FindChildByName("i3UIStaticText_Item_02");
	i3UIStaticText* weapon_desc_feature = (i3UIStaticText*)GetScene(0)->FindChildByName("i3UIStaticText_Scope_02");

	SHOP_GOODS* pGoods;
	CWeaponInfo* pInfo;
	
	if( !isOpenInventory )
	{
		pGoods = m_buyable_weapon_item->GetGoodsPack()->GetGoods(0);
		if( pGoods == nullptr) return;

		pInfo = g_pWeaponInfoDataBase->getWeaponInfo( pGoods->_ItemID );
		if( pInfo == nullptr) return;
	}
	else
	{
		pInfo = g_pWeaponInfoDataBase->getWeaponInfo( m_nItemID );
		if( pInfo == nullptr) return;
	}

	i3::rc_wstring wstrWeaponName;
	pInfo->GetWeaponName(wstrWeaponName);
	weapon_title->SetText(wstrWeaponName);

	i3::rc_wstring wstrDescription;
	pInfo->GetDescription(wstrDescription);
	weapon_desc->SetText(wstrDescription);

	i3::rc_wstring wstrDescFeature;
	pInfo->GetExtensionFeatures(wstrDescFeature);
	weapon_desc_feature->SetText(wstrDescFeature);
}
void UIPopupShopExtension::update_weapon_attribute_detail()
{
	i3::stack_wstring wstrRange;
	i3::stack_wstring wstrBulletCount;
	INT32 damage = 0, rapid = 0, accuracy = 0, recoil_v = 0, recoil_h = 0, speed = 0;

	if( !isOpenInventory )
	{
		SHOP_GOODS* pGoods = m_buyable_weapon_item->GetGoodsPack()->GetGoods(0);

		if( pGoods)
		{
			GameUI::GetNormalizedWeaponInfo( pGoods->_ItemID, 
				damage, rapid, accuracy, 
				recoil_v, recoil_h, speed, 
				wstrRange, wstrBulletCount);
		}
	}
	else
	{
		GameUI::GetNormalizedWeaponInfo( m_nItemID, 
			damage, rapid, accuracy, 
			recoil_v, recoil_h, speed, 
			wstrRange, wstrBulletCount);
	}

	set_detail_data(D_DAMAGE,	damage,		1.3f);
	set_detail_data(D_RAPID,	rapid,		1.3f);
	set_detail_data(D_HITRATE,	accuracy,	1.3f);
	set_detail_data(D_VCOIL,	recoil_v,	1.3f);
	set_detail_data(D_HCOIL,	recoil_h,	1.3f);
	set_detail_data(D_SPEED,	speed,		1.3f);

	text_rate[D_RANGE]->SetText(wstrRange);
	text_rate[D_BULLET]->SetText(wstrBulletCount);
}
void UIPopupShopExtension::update_weapon_class_used(UINT32 itemid)
{
	i3UIButtonImageSet* btn = (i3UIButtonImageSet*)GetScene(0)->FindChildByName("i3UIButtonImageSet_Icon");
	i3UIStaticText*		title = (i3UIStaticText*)GetScene(0)->FindChildByName("i3UIWeapon_Title");

	WEAPON_CLASS_TYPE weapon_class = WEAPON::ItemID2Class(itemid);
	switch (weapon_class)
	{
	case WEAPON_CLASS_ASSAULT:
		btn->SetShapeIdx(0);
		title->SetText(GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_ASSAULT"));
		break;
	case WEAPON_CLASS_SMG:
	case WEAPON_CLASS_DUALSMG:
		btn->SetShapeIdx(1);
		title->SetText(GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_SMG"));
		break;
	case WEAPON_CLASS_SNIPER:
		btn->SetShapeIdx(2);
		title->SetText(GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_SNIPER"));
		break;
	case WEAPON_CLASS_MG:
		btn->SetShapeIdx(3);
		title->SetText(GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_MG"));
		break;
	case WEAPON_CLASS_SHOTGUN:
	case WEAPON_CLASS_DUALSHOTGUN:
		btn->SetShapeIdx(4);
		title->SetText(GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_SHOTGUN"));
		break;
	case WEAPON_CLASS_HANDGUN:
	case WEAPON_SUBCLASS_ASSAULT:
	case WEAPON_SUBCLASS_SHOTGUN:
	case WEAPON_SUBCLASS_SNIPER:
	case WEAPON_SUBCLASS_SMG:
	case WEAPON_SUBCLASS_G_LAUNCHER:
	case WEAPON_SUBCLASS_BOW:
	case WEAPON_CLASS_BOW:
	case WEAPON_CLASS_DUALHANDGUN:
	case WEAPON_CLASS_CIC:
		btn->SetShapeIdx(5);
		title->SetText(GAME_RCSTRING("STBL_IDX_WEAPON_SLOT_TITLE2"));
		break;
	case WEAPON_CLASS_KNIFE:
	case WEAPON_CLASS_DUALKNIFE:
	case WEAPON_CLASS_KNUCKLE:
		btn->SetShapeIdx(6);
		title->SetText(GAME_RCSTRING("STBL_IDX_WEAPON_SLOT_TITLE3"));
		break;
	case WEAPON_CLASS_THROWING_GRENADE:
	case WEAPON_CLASS_THROWING_ATTACK:
		btn->SetShapeIdx(7);
		title->SetText(GAME_RCSTRING("STBL_IDX_WEAPON_SLOT_TITLE4"));
		break;
	case WEAPON_CLASS_THROWING_CONTAIN:
	case WEAPON_CLASS_THROWING_HEAL:
		btn->SetShapeIdx(8);
		title->SetText(GAME_RCSTRING("STBL_IDX_WEAPON_SLOT_TITLE5"));
		break;
	}
}

void UIPopupShopExtension::set_detail_data(int progress_number, INT32 data, float animation_Speed)
{
	i3::stack_wstring wstr_temp;
	GameUI::GetGaugeString(data, wstr_temp);
	text_rate[progress_number]->SetText(wstr_temp);

	if (data > 0.0f)
	{
		progress_rate[progress_number]->setAnimate(true);
		progress_rate[progress_number]->setAnimateSlop(animation_Speed);
		progress_rate[progress_number]->setValue(float(data) / 100.f);
	}
	else
	{
		progress_rate[progress_number]->setAnimate(false);
		progress_rate[progress_number]->setValue(0.0f);
		progress_rate[progress_number]->setProgress(0.0f);
	}
}

void UIPopupShopExtension::OnUpdate( REAL32 rDeltaSeconds)
{
	UIPopupBase::OnUpdate(rDeltaSeconds);

	//해당 팝업이 포커스 되어 있을 때만 작동
	if( GetMainFrame()->IsFocusedTopPopup(this) )
	{
		m_tmHitTest.OnUpdate(rDeltaSeconds);
		m_pWeapon3DView->Weapon3DViewUpdate(is_mouse_leftbtn_down, is_mouse_pt_in_frame_wnd);
	}

#if defined(_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)

	update_output();
	
#endif

	if(g_close_popup)
		GetMainFrame()->ClosePopup(UPW_SHOPEXTENSION);

}
bool UIPopupShopExtension::OnKey_Escape(void)
{
	set_close();
	return true;
}
bool UIPopupShopExtension::OnLostDevice( bool bLostDevice)
{
	bool bRv = UIPopupBase::OnLostDevice( bLostDevice);

	return bRv;
}
bool UIPopupShopExtension::OnRevive( i3RenderContext * pCtx)
{
	bool bRv = UIPopupBase::OnRevive( pCtx);
	if( m_pWeapon3DView != nullptr) m_pWeapon3DView->Weapon3DViewOnRevie();
	return bRv;
}

#if defined(_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)

void UIPopupShopExtension::update_output()
{
	//나도 이런처리 되게 싫어하지만 우리 엔진에서 실시간 visible on/off가 안먹힘! 귀찮아서 일단 이렇게... 짜증...
	if (!output_ui->isVisible())
	{
		output_ui->SetText(" ");
	}
	else
	{
		i3::rc_wstring outputmesage;
		outputmesage += m_pWeapon3DView->GetTest_Data();
		output_ui->SetText(outputmesage);
	}
}
void UIPopupShopExtension::switch_visible()
{
	bool bswitch = false;
	output_ui->isVisible() ? bswitch = false : bswitch = true;
	output_ui->SetVisible(bswitch);
}
/*virtual*/ bool UIPopupShopExtension::OnKeyInput(i3InputDeviceManager * pInputMng)
{
	i3InputKeyboard * pKeyboard = pInputMng->GetKeyboard();
	
	if (pKeyboard->GetStrokeState(I3I_KEY_ESC))
		set_close();

	if (pKeyboard->GetStrokeState(I3I_KEY_END))
		switch_visible();

	if (output_ui->isVisible())
	{
		if (pKeyboard->GetStrokeState(I3I_KEY_UP))
			m_pWeapon3DView->Set_ChangeDist_RealTime(true);

		if (pKeyboard->GetStrokeState(I3I_KEY_DOWN))
			m_pWeapon3DView->Set_ChangeDist_RealTime(false);

		if (pKeyboard->GetStrokeState(I3I_KEY_LEFT))
			m_pWeapon3DView->Set_ChangeDist_Offset(false);

		if (pKeyboard->GetStrokeState(I3I_KEY_RIGHT))
			m_pWeapon3DView->Set_ChangeDist_Offset(true);

		if (pKeyboard->GetStrokeState(I3I_KEY_DEL))
			m_pWeapon3DView->Set_ChangeDist_Reset();
	}

	return true;
}

#endif

