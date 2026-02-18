#include "pch.h"
#include "UIPhaseGiftShop.h"

#include "UIPhaseShopSlot.h"
#include "UIShopFunction.h"
#include "ItemImageMgr.h"
#include "UIUtil.h"

#include "UIMainFrame.h"
#include "UIPopupBuyItem.h"
#include "PurchaseMgr.h"
#include "BuyUtil.h"

#include "UI/UISideMenu.h"
#include "UISMenu_Ability.h"
#include "UIFloatCharacterView.h"

#include "ShopContext.h"
#include "GiftShopContext.h"
#include "UserInfoContext.h"

#include "i3Base/string/ext/format_string.h"

#include "UISignal.h"
#include "i3Base/itl/bind/bind.h"
#include "i3Base/itl/bind/placeholders.h"

//------------------------------------------------------------------------------//
I3_CLASS_INSTANCE( UIPhaseGiftShop);//, UIPhaseSubBase);

namespace
{
	enum	{ 
		//UITap에 enum 값과 같이 사용한다.
		COMPARE = 6, BUY, CASH };

	UIPhaseGiftShop* g_this = 0;
}

/********************************/
extern "C" 
{
	int gift_shop_glue_click( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1);

		ShopSlot* itemSlot = (ShopSlot*)(g_this->GetItemSlot());
		itemSlot->clicked(idx);
		
		//g_this->clicked_gift_rank_slot(slot);
		return 0;
	}
}

LuaFuncReg giftdouble_clicked_shop_glue[] =
{
	{ "Click",			gift_shop_glue_click },
	{ nullptr,				nullptr}
};

/********************************/
void UIPhaseGiftShop::clicked_menu_impl(INT32 idx)
{
	if (idx <= item_def::CTG_NEWHOT && idx >= item_def::CTG_WEAPON)
	{
		SetSortCombo(idx);
	}

	switch( idx )
	{
	case BUY	: ((ShopSlot*)m_pSlot)->double_clicked(true);	break;
	case CASH	: ((ShopSlot*)m_pSlot)->clicked(4, true);		break;
	}
}

/********************************/
void UIPhaseGiftShop::clicked_gift_rank_slot(INT32 idx)
{
	INT32 real_rank = idx + 1;
	T_ItemID itemID = GiftShopContext::i()->GetRankGiftID( real_rank );

	if(itemID != 0)
	{
		const SHOP_GOODS_PACK * pGoodsPack = CShopContext::i()->GetGoodsPack(itemID);
		if( pGoodsPack == nullptr)
		{
#if defined(_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
			{
				i3::rc_wstring wstr_errMsg;
				i3::sprintf(wstr_errMsg, L"Err! - pGoodsPack = nullptr item(ITEMID=%d) in itemlist of shop..", itemID);
				GameUI::MsgBox(MSG_TYPE_GAME_OK, wstr_errMsg);
			}
#endif
			return;
		}

		SHOP_GOODS * pGoods = pGoodsPack->GetGoods(0);

		if( usf::CheckValidGoods(pGoodsPack, false) == false ) return;
		if( usf::PopupRestriction(pGoods->_ItemID) == false) return;

		buy::PushImmediate(pGoodsPack);
		GetMainFrame()->OpenPopup(UPW_BUYITEM, (void*)buy::PurchaseRoute_Immediate, &UISignal::FinishBuyPopup); // "finish_buy_popup");	//즉시구매
	}
}

void UIPhaseGiftShop::set_gift_shop_display_info(void)
{
	LuaState * L = _CallLuaFunction(m_menu_scn_info.idx, "SetGiftShopDisplay");

	if (nullptr != L)
	{
		i3Lua::PushBoolean(L, UserInfoContext::i()->IsChargeCashEnable() ? true : false);
		_EndLuaFunction(L, 1);
	}
}

void UIPhaseGiftShop::set_weekly_best_item(INT32 idx)
{
	//INT32 real_rank = idx + 1;

	//T_ItemID itemID = GiftShopContext::i()->GetRankGiftID( real_rank );

	//set_weekly_best_item_image(idx, itemID);

	// // 2위부터는 스트링 데이터를 설정합니다.
	//if(real_rank > 1)
	//	set_weekly_best_item_string(idx, real_rank, itemID);	
}

void UIPhaseGiftShop::set_weekly_best_item_image(INT32 idx, T_ItemID itemID)
{
	/*if(itemID > 0) 
	{
		m_pImgSet[idx]->SetEnable(true);
		GetItemImageMgr()->SelectImage(m_pImgSet[idx], itemID);
	}
	else
	{
		m_pImgSet[idx]->SetEnable(false);
	}*/
}

void UIPhaseGiftShop::set_weekly_best_item_string(INT32 idx, INT32 rank, T_ItemID itemID)
{
	/*INT32 i32Type = -1;
	INT32 i32Amount = 0;

	i3::rc_wstring wstrItemName;
	i3::stack_wstring wstrRankingAmount;
	COLOR color;
	i3Color::Set(&color, (UINT8)255, 255, 255, 255);

	if(itemID > 0)
	{
		i32Type = GiftShopContext::i()->GetRankType(rank);
		i32Amount = GiftShopContext::i()->GetRankAmount(rank);

		usf::GetShopItemName(wstrItemName, itemID);
		get_item_ranking_amount_text(wstrRankingAmount, i32Type, i32Amount);
		get_item_ranking_amount_text_color(color, i32Type);
	}

	LuaState * L = _CallLuaFunction(m_gift_scn_info.idx, "SetWeeklyBestInfo");

	if(nullptr != L)
	{
		i3Lua::PushInteger(L, idx);
		i3Lua::PushStringUTF16To8(L, wstrItemName.c_str(), wstrItemName.size());
		i3Lua::PushStringUTF16To8(L, wstrRankingAmount);
		i3Lua::PushColor(L, &color);
		i3Lua::PushInteger(L, i32Type);
		_EndLuaFunction(L , 8);
	}*/
}

void UIPhaseGiftShop::get_item_ranking_amount_text(i3::stack_wstring & out_wstr, const INT32 i32Type, const INT32 i32Amount)
{
	
	switch(i32Type)
	{
	case GiftShopContext::GRT_UP:
	case GiftShopContext::GRT_DOWN:
		{
			i3::sprintf(out_wstr, L"%d", i32Amount);
		}
		break;
	default:
		out_wstr.clear();
		break;
	}
}

void UIPhaseGiftShop::get_item_ranking_amount_text_color(COLOR & out_color, const INT32 i32Type)
{
	i3Color::Set(&out_color, (UINT8)255, 255, 255, 255);

	switch(i32Type)
	{
	case GiftShopContext::GRT_UP:
		{
			i3Color::Set(&out_color, (UINT8)244, 17, 0, 255);
		}
		break;
	case GiftShopContext::GRT_DOWN:
		{
			i3Color::Set(&out_color, (UINT8)34, 127, 255, 255);
		}
		break;
	}
}

void UIPhaseGiftShop::cb_finish_buy_popup(bool arg1, bool arg2)
{
	m_pSlot->Entrance();
}

/********************************/
//상점 구매 패킷 후에 서버 응답
void UIPhaseGiftShop::cb_finish_buy_gift(bool arg1)
{
	UIPopupBuyItem * buy_popup = (UIPopupBuyItem*)GetMainFrame()->GetPopup(UPW_BUYITEM);

	bool result_buy_item = arg1; // ((BOOL)arg1 == TRUE) ? true : false;
	
	if (result_buy_item == true)
	{
		buy::Clear_ByPurchaseRoute(buy_popup->getPurchaseRoute());
	}

	buy_popup->ClosePopup();
	GetMainFrame()->ClosePopup(UPW_GIFT_SEND);
}

void UIPhaseGiftShop::cb_is_ignore_shared(bool& out_is_ignore_shared)
{
//	bool * ignore_shard = reinterpret_cast<bool*>(arg1);
//	*ignore_shard = true;

	out_is_ignore_shared = true;					// 아..이게뭐야...함수명 그지 같음...
}

void UIPhaseGiftShop::cb_enable_compare_display_attr(bool bEnable)
{
	LuaState *L = _CallLuaFunction(m_menu_scn_info.idx, "EnableWeaponInfo2");
	i3Lua::PushBoolean(L, bEnable);
	UILua::EndLuaFunction(L, 1);
}

/********************************/
UIPhaseGiftShop::UIPhaseGiftShop() : iMultiTap()
{
	g_this = this;
	m_phase_type = USP_GIFTSHOP;

	m_pSlot = new ShopSlot(this);
	MEMDUMP_NEW( m_pSlot, sizeof(ShopSlot));

	m_slide_offset_x = SLIDE_OFFSET;
	m_slide_offset_y = 0;
	slot_scene_name = "Scene/Main/PointBlankRe_Shop_Slot_V3.i3UIs";

	/*for(INT32 idx = 0; idx < MAX_RANKING_COUNT; ++idx)
		m_pImgSet[idx] = nullptr;*/
}

UIPhaseGiftShop::~UIPhaseGiftShop()
{
	I3_SAFE_DELETE( m_pSlot);

	/*for(INT32 idx = 0; idx < MAX_RANKING_COUNT; ++idx)
	{	
		if(m_pImgSet[idx] != nullptr)
		{
			m_pImgSet[idx]->RestoreSizePos();
			I3_SAFE_RELEASE(m_pImgSet[idx]);
		}
	}*/
}

//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//
/*virtual*/ void UIPhaseGiftShop::OnCreate( i3GameNode * pParent)
{
	iMultiTap::OnCreate( pParent);

	m_deco_scn_info.idx = GetSceneCount();
	m_deco_scn_info.scn = AddScene("Scene/Main/PointBlankRe_Shop_Gift_Decoration.i3UIs", nullptr, true, false);

	//m_gift_scn_info.idx = GetSceneCount();
	//m_gift_scn_info.scn = AddScene("Scene/Main/PointBlankRe_Shop_Giftshop.i3UIs", gift_shop_glue, true, false);
}

/*virtual*/ void UIPhaseGiftShop::OnLoadAllScenes()
{
	usf::SetPriceIdx(PRICE_TYPE_GIFT_REAL);
	//usf::SetPriceIdx(PRICE_TYPE_SHOP_REAL);

	//m_tmSlide2.clear();
	iMultiTap::OnLoadAllScenes();
	//m_tmSlide2.move_target();

	m_SigConnection += UISignal::FinishBuyPopup.connect(i3::bind(&UIPhaseGiftShop::cb_finish_buy_popup, this, i3::_1, i3::_2));
	m_SigConnection += UISignal::EnableCompareDisplayAttr.connect(i3::bind(&UIPhaseGiftShop::cb_enable_compare_display_attr, this, i3::_1));
	m_SigConnection += UISignal::IsIgnoreShared.connect(i3::bind(&UIPhaseGiftShop::cb_is_ignore_shared, this, i3::_1));
	m_SigConnection += UISignal::FinishBuyGift.connect(i3::bind(&UIPhaseGiftShop::cb_finish_buy_gift, this, i3::_1));

	/*m_pImgSet[0] = GetItemImageMgr()->CreateImgSetCtrl(m_gift_scn_info.scn , "BtnImgSet_TB_Weapon");
	I3ASSERT(m_pImgSet[0] != nullptr);

	for(INT32 idx = 1 ; idx < MAX_RANKING_COUNT ; ++idx)
	{
		i3::string strCtrlName = i3::format_string("BtnImgSet_WB_Weapon%d" ,idx);
		m_pImgSet[idx] = GetItemImageMgr()->CreateImgSetCtrl(m_gift_scn_info.scn, strCtrlName.c_str());
		I3ASSERT(m_pImgSet[idx] != nullptr);
	}
	*/
	LuaState * L = nullptr;

	
	L = _CallLuaFunction(m_menu_scn_info.idx, "EnableCash");	//캐쉬 충전 가능 여부
	i3Lua::PushBoolean(L, UserInfoContext::i()->IsChargeCashEnable() ? true : false);
	UILua::EndLuaFunction(L, 1);

	L = _CallLuaFunction(m_menu_scn_info.idx, "TabEnable");
	i3Lua::PushInteger(L, 5);
	UILua::EndLuaFunction(L, 1);

	set_gift_shop_display_info();

	//무기를 보여준다.
	//clicked_tap(item_def::CTG_WEAPON, -1); // -1 for 전체무기
	//clicked_menu_impl(item_def::CTG_WEAPON);
	//open_menu(item_def::CTG_WEAPON);

	clicked_tap(item_def::CTG_NEWHOT, item_def::MASKNEWHOT_ALL);
	clicked_menu_impl(item_def::CTG_NEWHOT);

	OnRevive(0);

	open_menu(item_def::CTG_NEWHOT);
}

void	UIPhaseGiftShop::OnUnloadAllScenes()
{
	usf::SetPriceIdx(PRICE_TYPE_SHOP_REAL);

	//UI::remove_cb(this);
	m_SigConnection.clear();

	//m_tmSlide2.move_origin();
	//m_tmSlide2.clear();

	/*for(INT32 idx = 0; idx < MAX_RANKING_COUNT; ++idx)
	{
		if(m_pImgSet[idx] != nullptr)
		{
			m_pImgSet[idx]->RestoreSizePos();
			I3_SAFE_RELEASE(m_pImgSet[idx]);
		}
	}*/

	iMultiTap::OnUnloadAllScenes();
}

void	UIPhaseGiftShop::OnEntranceEnd()
{
	iMultiTap::OnEntranceEnd();

	int m_StartTab = 5;
	// 입장해야하는 탭이 무기 탭이 아닐 경우
	if (m_StartTab != 5)
	{
		iMultiTap::clicked_menu(m_StartTab);

		g_this->clicked_tap(m_StartTab, -1);
		SelectedEnteranceTab(m_StartTab);
		set_current_menu_idx(m_StartTab);
		m_StartTab = 5;
	}
}

/*virtual*/ bool UIPhaseGiftShop::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/)
{
	iMultiTap::OnEvent(event, pObj, param2, code);

	if( event == I3_EVT_UI_NOTIFY)
	{
		I3UI_CONTROL_NOTIFY * pNotify = (I3UI_CONTROL_NOTIFY*) param2;

		i3UIButtonComposed3 * pButton = i3::same_cast<i3UIButtonComposed3*>(pNotify->m_pCtrl);

		if( pButton != nullptr)
		{
			i3UIStaticText * pTextCtrl = i3::same_cast<i3UIStaticText*>(pButton->FindChildByType(i3UIStaticText::static_meta()));
			if( pTextCtrl != nullptr)
			{
				COLOR color;

				if( pButton->isSelected() )					i3Color::Set(&color, (UINT8)0, 51, 96, 255);
				if( pButton->isState( I3UI_STATE_ONMOUSE) )	i3Color::Set(&color, (UINT8)6, 82, 149, 255);
				else										i3Color::Set(&color, (UINT8)184, 216, 255, 255);

				pTextCtrl->SetTextColor(&color);
			}
		}
	}

	return UIBase::OnEvent( event, pObj, param2, code);
}

bool UIPhaseGiftShop::OnRevive( i3RenderContext * pCtx)
{
	if( pCtx && iMultiTap::OnRevive( pCtx) == false) //OnLoadAllScenes에서 OnRevive 이 함수를 호출할때 매개인자로 0을 넣는다. 
		return false;

	if( isEnable() == false ) return false;

	/*m_pSlot->OnLoadAllScenes();
	VEC3D pos = *m_SlotFrm[1]->getPos();
	i3UIControl* item_ctrl = static_cast<i3UIControl*>(m_slot_scn_info.scn->getFirstChild());
	item_ctrl->setPos(&pos);*/

	return true;
}

void UIPhaseGiftShop::init_slide_impl()
{
	UI::tmSlideAttContainer list(m_menu_scn_info.scn);
	list.add("i3UIFrameWnd");
	m_tmSlide.add(list);
	//UI::tmSlideAttContainer list(m_deco_scn_info.scn);
	//list.add("FrameWnd_Deco");
	//m_tmSlide.add(list);

	//UI::tmSlideAttContainer list2(m_gift_scn_info.scn);
	//list2.add("FrameWnd_GiftShop");
	//m_tmSlide2.add(list2);
}

void UIPhaseGiftShop::entrance_end_impl()
{
	//m_deco_scn_info.scn->Attach( false );

	//for(INT32 idx = 0 ; idx < MAX_RANKING_COUNT ; ++idx)
//		set_weekly_best_item(idx);
}

void UIPhaseGiftShop::update_impl( REAL32 rDeltaSeconds)
{
	//m_tmSlide2.OnUpdate(rDeltaSeconds);
}

/*virtual*/ void UIPhaseGiftShop::Enter(void)
{
}

/*virtual*/ void UIPhaseGiftShop::Leave(void)
{
	GameEventSender::i()->SetEventOnce( EVENT_LEAVE_GIFTSHOP);
}

void UIPhaseGiftShop::ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData)
{
	iMultiTap::ProcessGameEvent(event, Arg, UserData);
}

void UIPhaseGiftShop::open_menu(INT32 idx) const
{
	iMultiTap::open_menu(idx);

	if (idx == item_def::CTG_WEAPON)
	{
		LuaState * L = UILua::CallLuaFunction(m_menu_scn_info.scn, "EnableCompare");
		i3Lua::PushBoolean(L, false);
		UILua::EndLuaFunction(L, 1);
	}
	LuaState * L = UILua::CallLuaFunction(m_menu_scn_info.scn, "Subopen");
	i3Lua::PushInteger(L, idx);
	i3Lua::PushBoolean(L, false);
	UILua::EndLuaFunction(L, 2);
}

void UIPhaseGiftShop::clicked_tap_impl(INT32 idx1, INT32 idx2)
{
	m_pSlot->SortType((item_def::Category)idx1, GetSortType());

	SMenuAbility* sidemenu = static_cast<SMenuAbility*>(UISideMenu::instance()->getSubSidemenu(SIDEMENU_BTN_ABILITY));
	sidemenu->EnableCtrlsByShopCategory(static_cast<item_def::Category>(idx1), true);

	if (idx1 == item_def::CTG_PARTS)
	{
		// parts탭 내에서는 무기만 리셋
		INT32 revert = (idx2 > -1) ? REVERT_TYPE_WEAPON : REVERT_TYPE_ALL;
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_REVERT_PARTS, revert);
	}
	else
	{
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_REVERT_CHARA, REVERT_TYPE_ALL);
	}

	UISideMenu::instance()->setAbilityButton(false);	// SideMenu의 Ability 버튼을 원위치로 돌려놓는다
	sidemenu->EnableCtrlsByShopCategory(static_cast<item_def::Category>(idx1), false);
}

void UIPhaseGiftShop::enable_ability_main_info(bool enable)
{
	SMenuAbility* ability =
		static_cast<SMenuAbility*>(UISideMenu::instance()->getSubSidemenu(SIDEMENU_BTN_ABILITY));

	if (ability == nullptr)
		return;

	ability->EnableMainDefaultInfoOnly();
	ability->EnableMainInfo(enable);
}