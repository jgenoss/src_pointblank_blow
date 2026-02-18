#include "pch.h"
#include "UIBattlePopupShop.h"

#include "UIBattleShopSlot.h"

#include "UIBattleFrame.h"
#include "IGShopContext.h"
#include "UIBattlePopupBuyItem.h"
#include "BuyUtil.h"
#include "PurchaseMgr.h"

#include "UIItemInfoUtil.h"
#include "UIBattlePopupDeleteItem.h"
#include "UIShopFunction.h"
#include "UIBattlePopupSelectWeapon.h"
#include "UIHudManager.h"

#include "Useable_Item_Def.h"
#include "UIUtil.h"
#include "UIMath.h"
#include "InvenUtil.h"
#include "hud/UIHUD_Domination_Manager.h"
#include "UIBattleDef.h"

#include "UserInfoContext.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/itl/bind/bind.h"
#include "i3Base/itl/bind/placeholders.h"
#include "UISignal.h"


/********************************/
extern "C"
{
	int battle_shop_tab(LuaState* L)
	{
		INT32 cat = i3Lua::GetIntegerArg(L, 1);
		UIBattlePopupShop * p = static_cast<UIBattlePopupShop*>(GameUI::GetBattlePopup( UBP_INGAME_SHOP ));
		if( p != nullptr)
			p->clicked_tap(cat);
		return 0;
	}
}

LuaFuncReg battle_shop_glue[] = 
{
	{ "Click", battle_shop_tab },
	{ nullptr, nullptr }
};

/********************************/
I3_CLASS_INSTANCE( UIBattlePopupShop);

UIBattlePopupShop::UIBattlePopupShop() : curr_category(-1)
{
#if use_scroll
	m_HitTestCallback.SetObj( this );
	m_tmHitTest.SetCallback( &m_HitTestCallback );
#endif

	m_pSlot = new BattleShopSlot(this);
	MEMDUMP_NEW( m_pSlot, sizeof( BattleShopSlot));

	m_SigConnection += UISignal::FinishBuyItem.connect(i3::bind(&UIBattlePopupShop::cb_finish_buy_battle_shop_item, this, i3::_1, i3::_2));
	m_SigConnection += UISignal::FinishAuthItem.connect(i3::bind(&UIBattlePopupShop::cb_finish_buy_auth_item, this, i3::_1, i3::_2));
	m_SigConnection += UISignal::FinishRemoveItem.connect(i3::bind(&UIBattlePopupShop::cb_finish_remove_item, this, i3::_1, i3::_2));

	//domi_shop_list.list_up();
}

UIBattlePopupShop::~UIBattlePopupShop()
{
	I3_SAFE_DELETE( m_pSlot);
//	UI::remove_cb(this);
}

/********************************/
void UIBattlePopupShop::clicked_tap(INT32 idx)
{
	enum { BTN_CLOSE = 9, };

	if( idx == BTN_CLOSE ) 
		close_shop();
	else 
		select_category(idx);
}



/********************************/
void	UIBattlePopupShop::OnCreate( i3GameNode * pParent)
{
	UIBattlePopupBase::OnCreate(pParent);

	AddScene("Scene/HUD/Ingame_Shop.i3UIs", battle_shop_glue, true, false);

	m_pSlot->OnCreate( pParent );
}

/*virtual*/ bool UIBattlePopupShop::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIBattlePopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	HUD::instance()->SetParameter(HUD::RESPAWN_TIMER, HUD::RT_SEND_BLOCK);

	curr_category = -1;
	max_category = 0;
	m_re_autu_item_goods = 0;

	pf_keyinput = &UIBattlePopupShop::on_keyinput_category;

	GetSlide()->ForceEnd();
	GameUI::SetMouseCenterPosition();

	return true;
}

void	UIBattlePopupShop::OnLoadAllScenes()
{
	UIBattlePopupBase::OnLoadAllScenes();

#if ENABLE_BATTLESHOP_SCROLL
	m_tmHitTest.add( GetScene(0), "frame_main" );
#endif

	m_pSlot->OnLoadAllScenes();
	m_pSlot->MoveBackScene();
}

void	UIBattlePopupShop::OnEntranceEnd()
{
	enum { MAX_CaTeGory = 9, };			// 이게 뭔데?? 

	UIBattlePopupBase::OnEntranceEnd();

	IGShopContext* cont = g_pFramework->GetIGShopContext();
	max_category = cont->GetNumCategory();

	if (max_category > MAX_FIELDSHOP_CATEGORYCOUNT)
	{
		I3_BOUNDCHK(max_category, MAX_FIELDSHOP_CATEGORYCOUNT + 1 );
		max_category = MAX_FIELDSHOP_CATEGORYCOUNT;
	}
		
	//catrgory 이름 지정
	LuaState * L = _CallLuaFunction("SetTab"); 
	for(INT32 i=0;i<MAX_CaTeGory;i++)
	{
		if( i < max_category ) 
		{
		//	I3_BOUNDCHK(i, max_category);		바운드체크는 필요하지만, 이 값이 필요한게 아니다.
			i3Lua::PushStringUTF16To8(L, cont->GetCategoryString(i) );		// 접근 함수는 MAX_FIELDSHOP_CATEGORYCOUNT 영향받음..
		}
		else 
			i3Lua::PushStringUTF16To8( L, L"");
	}
	UILua::EndLuaFunction(L , MAX_CaTeGory);

	//cash 지정
	update_my_cash();

	//0번째 category 선택
	if( max_category != 0 )
		to_lua_select_category(0);
}

void	UIBattlePopupShop::OnUnloadAllScenes()
{
#if ENABLE_BATTLESHOP_SCROLL 
	m_tmHitTest.clear();
#endif

	m_pSlot->OnUnloadAllScenes();

	UIBattlePopupBase::OnUnloadAllScenes();
}

void	UIBattlePopupShop::OnExitEnd(void)
{
	UIBattlePopupBase::OnExitEnd();

	g_pFramework->GetIGShopContext()->LeaveServer();

	HUD::instance()->SetParameter(HUD::RESPAWN_TIMER, HUD::RT_SEND_UNBLOCK);
}

void UIBattlePopupShop::OnUpdate(REAL32 rDeltaSeconds)
{
	UIBattlePopupBase::OnUpdate(rDeltaSeconds);

	if( max_category == 0 ) return;

	m_pSlot->OnUpdate(rDeltaSeconds);

#if ENABLE_BATTLESHOP_SCROLL 
	m_tmHitTest.OnUpdate(rDeltaSeconds);
#endif
}

#if ENABLE_BATTLESHOP_SCROLL 
bool UIBattlePopupShop::OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code)
{
	m_pSlot->OnEvent(event, pObj, param2, code);

	return UIBattlePopupBase::OnEvent(event, pObj, param2, code);
}
#endif

void UIBattlePopupShop::on_keyinput_category(i3InputKeyboard* keyboard)
{
	for(INT32 i = 0; i<=max_category;++i )
	{
		if( keyboard->GetStrokeState( I3I_KEY(I3I_KEY_1+i) ) || keyboard->GetStrokeState( I3I_KEY(I3I_KEY_NUMPAD1+i) ) )
		{
			to_lua_select_category(i); return;
		}
	}

	if( keyboard->GetStrokeState( I3I_KEY_UP) )
	{
		INT32 next_category = uiMath::move_number( (INT32)curr_category, (INT32)0, (INT32)max_category-1, false);
		to_lua_select_category(next_category);
	}
	else if( keyboard->GetStrokeState( I3I_KEY_DOWN) )
	{
		INT32 next_category = uiMath::move_number( (INT32)curr_category, (INT32)0, (INT32)max_category-1, true);
		to_lua_select_category(next_category);
	}
	else if( keyboard->GetStrokeState( I3I_KEY_RIGHT ) )
	{
		pf_keyinput = &UIBattlePopupShop::on_keyinput_itemlist;
		m_pSlot->SelectSlot(0);
	}
	else if( keyboard->GetStrokeState( I3I_KEY_0 ) || keyboard->GetStrokeState( I3I_KEY_ESC ) )
	{
		close_shop();
	}
}

void UIBattlePopupShop::on_keyinput_itemlist(i3InputKeyboard* keyboard)
{
	if( keyboard->GetStrokeState( I3I_KEY_UP) )
	{
		INT32 row_pos = m_pSlot->selected_slot_idx / BsS::COL;
		if( row_pos > 0 )
		{
			m_pSlot->SelectSlot(m_pSlot->selected_slot_idx - BsS::COL);
		}
	}
	else if( keyboard->GetStrokeState( I3I_KEY_DOWN) )
	{
		INT32 row_pos = m_pSlot->selected_slot_idx / BsS::COL;
		if( row_pos < BsS::ROW-1 )
		{
			if ( m_sel_idx + BsS::COL < m_pSlot->get_able_select_slot_size())
			{
				m_pSlot->SelectSlot(m_pSlot->selected_slot_idx + BsS::COL);
			}
		}
	}
	else if( keyboard->GetStrokeState( I3I_KEY_LEFT ) )
	{
		INT32 col_pos = m_pSlot->selected_slot_idx % BsS::COL;
		if( col_pos == 0 )
		{
			pf_keyinput = &UIBattlePopupShop::on_keyinput_category;
			m_pSlot->SelectSlot(-1);
		}
		else
		{
			m_pSlot->SelectSlot(m_pSlot->selected_slot_idx-1);
		}
	}
	else if( keyboard->GetStrokeState( I3I_KEY_RIGHT ) )
	{
		INT32 col_pos = m_pSlot->selected_slot_idx % BsS::COL;
		if (col_pos < BsS::COL - 1)
		{
			if( m_pSlot->selected_slot_idx + 1 < m_pSlot->get_able_select_slot_size() )
				m_pSlot->SelectSlot(m_pSlot->selected_slot_idx + 1);
		}
	}
	else if( keyboard->GetStrokeState( I3I_KEY_0 ) || keyboard->GetStrokeState( I3I_KEY_NUMPAD0 ) || keyboard->GetStrokeState( I3I_KEY_ESC ) )
	{
		pf_keyinput = &UIBattlePopupShop::on_keyinput_category;
		m_pSlot->SelectSlot(-1);
	}
	else if( keyboard->GetStrokeState( I3I_KEY_ENTER ))
	{
		m_pSlot->SelectSlot();
	}

	for(INT32 i = 0; i<=max_category;++i )
	{
		if( keyboard->GetStrokeState( I3I_KEY(I3I_KEY_1+i) ) || keyboard->GetStrokeState( I3I_KEY(I3I_KEY_NUMPAD1+i) ) )
		{
			m_pSlot->SelectSlot(i);
			return;
		}
	}
}

bool	UIBattlePopupShop::OnKeyInput( i3InputDeviceManager * pInputMng)
{
	if (UIBattlePopupBase::OnKeyInput(pInputMng) == true) 
		return true;

	if( max_category == 0 ) return true;

	(this->*pf_keyinput)(pInputMng->GetKeyboard());

	return true;
}

VEC3D*  UIBattlePopupShop::get_left_top()
{
	i3UIFrameWnd* main_wnd = (i3UIFrameWnd*)GetScene(0)->FindChildByName("i3UIFrameWnd");
	return main_wnd->getAccumulatedPos();
}

#if ENABLE_BATTLESHOP_SCROLL 
void UIBattlePopupShop::hit_test_enter( UINT32 inx, i3UIControl* pWnd )
{
	m_pSlot->hit_test_enter();
}

void UIBattlePopupShop::hit_test_leave( UINT32 inx, i3UIControl* pWnd )
{
	m_pSlot->hit_test_leave();
}
#endif

/********************************/
//상점 구매 패킷 후에 서버 응답
void UIBattlePopupShop::finish_buy_item_normal_imp(bool arg1)
{
	UIBattlePopupBuyItem* popup = static_cast<UIBattlePopupBuyItem*>(GameUI::GetBattlePopup(UBP_BUYITEM));
	if( popup != nullptr)
		popup->ClosePopup();

	if (arg1 == false) 
		return;

	buy::DisplayedGoods* item = buy::GetDisplayedGoods_Immediate(); 	

	const SHOP_GOODS* pGoods = item->GetCurrentGoods(); //buy::item_util::get_selected_goods(item); //현재 선택된 goods

	i3::rc_wstring wstr_ItemName;
	usf::GetShopItemName(wstr_ItemName, pGoods->_ItemID);		

	i3::wstring wstrMsg = wstr_ItemName;	
	wstrMsg.append( L" : " );
	wstrMsg += GAME_RCSTRING("STR_POPUP_NOTICE_DESIGNATION_GET_WITH_ITEM"); /*인벤토리에서 확인하실 수 있습니다.*/
	g_pFramework->PutSystemChatting(wstrMsg);

	bool need_auth = true;
	item_def::Category ctg = item_def::get_category(pGoods->_ItemID);
	if( ctg == item_def::CTG_ITEM )
	{
		usf::ValidUseType result = usf::IsUseableItem(pGoods->_ItemID);
		if( result == usf::vut_Duplicate )
		{
			m_re_autu_item_goods = pGoods;
			usf::Message_UseResert(result, true);
			try_remove_item(pGoods->_ItemID);
			need_auth = false;
		}

#ifndef JuuL_Test_Code
		need_auth = false;
#endif
	}

	if(need_auth == true)
	{
		PurchaseMgr::i()->Auth_InGame(this, pGoods);    // "finish_buy_auth_item",
	}

	UIBattlePopupSelectWeapon * pPopup = static_cast<UIBattlePopupSelectWeapon*>(GameUI::GetBattlePopup( UBP_SELECT_WEAPON));
	if( pPopup != nullptr)
		pPopup->SetInitList(); //무기교체 창 db를 다시 생성한다.

	buy::PurchaseRouteType route = popup->getPurchaseRoute();
	buy::Clear_ByPurchaseRoute(route);
	
	update_my_cash();
}

void UIBattlePopupShop::finish_buy_item_domi_imp(bool arg1)
{
	UIBattlePopupBuyItem* popup = static_cast<UIBattlePopupBuyItem*>(GameUI::GetBattlePopup(UBP_BUYITEM));
	if( popup != nullptr)
		popup->ClosePopup();

	if (arg1 == false)
		return;
//	if((BOOL)arg1 == FALSE) return;
	
	buy::DisplayedGoods* displayedGoods = buy::GetDisplayedGoods_Immediate();		

	const SHOP_GOODS* pGoods = displayedGoods->GetCurrentGoods();  //현재 선택된 goods

	i3::rc_wstring wstr_ItemName;
	usf::GetShopItemName(wstr_ItemName, pGoods->_ItemID);
	
	i3::wstring wstr_msg = wstr_ItemName;		
	wstr_msg.append( L" : " );
	wstr_msg += GAME_RCSTRING("STR_POPUP_NOTICE_DESIGNATION_GET_WITH_ITEM"); /*인벤토리에서 확인하실 수 있습니다.*/
	g_pFramework->PutSystemChatting(wstr_msg);

	buy::PurchaseRouteType route = popup->getPurchaseRoute();
	buy::Clear_ByPurchaseRoute(route);
	
	UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_REFRESH, dui::annihilation);
}

void UIBattlePopupShop::cb_finish_buy_battle_shop_item(bool arg1, const i3::vector<buy::DisplayedGoodsPtr>& )
{
	if( is_using_normal == true )
	{
		finish_buy_item_normal_imp(arg1);
	}
	else
	{
		finish_buy_item_domi_imp(arg1);
	}
}

//Immediate 구입 일 경우만 호출 된다.
void UIBattlePopupShop::cb_finish_buy_auth_item(bool arg1, const SHOP_GOODS* arg2) 
{
//	if( (BOOL)arg1 == FALSE ) return;
	if (arg1 == false)
		return;

	const SHOP_GOODS* buy_goods = arg2; // static_cast<const SHOP_GOODS*>(arg2);

	item_def::Category ctg = item_def::get_category(buy_goods->_ItemID);
	switch( ctg )
	{
	case item_def::CTG_WEAPON:
	case item_def::CTG_PARTS:
	case item_def::CTG_SKIN :
		{
			i3::rc_wstring wstr_ItemName;
			usf::GetShopItemName(wstr_ItemName, buy_goods->_ItemID);


			i3::wstring wstr_msg = wstr_ItemName; 
			wstr_msg.append( L" : " );
			wstr_msg += GAME_RCSTRING("STR_INVEN_DO_EQUIP") ; /*장비하기*/
			g_pFramework->PutSystemChatting(wstr_msg);
		}
		break;
	case item_def::CTG_CHARA:	
		if( item_def::is_chara_category(buy_goods->_ItemID) )
		{
		}
		else
		{
		}
		break;
	case item_def::CTG_ITEM:
		{
			ShopItemInfo* info = g_pShopItemInfoDataBase->getInfo(buy_goods->_ItemID);
			if(info != 0)
			{
				g_pFramework->PutSystemChatting(info->GetDescription());
				g_pFramework->PutSystemChatting(info->GetResultNotice());
			}
		}
		break;
	}

	ITEM_INFO item_info;
	item_info.m_TItemID = buy_goods->_ItemID;
	item_info.m_TItemDBIdx = PurchaseMgr::i()->GetTargetItemDBIndex();
		
	g_pFramework->GetIGShopContext()->SetEvent_EquipItem(item_info);
}

void UIBattlePopupShop::cb_finish_remove_item(bool arg1, const ITEM_INFO* arg2)
{
//	if( (BOOL)arg1 == FALSE ) return;
	if (arg1 == false) 
		return;

	ITEM_INFO item_info = *arg2; //		*((ITEM_INFO*)arg2);

	item_def::Category ctg = item_def::get_category(item_info.m_TItemID);

	i3::rc_wstring wstr_ItemName;
	usf::GetShopItemName(wstr_ItemName, item_info.m_TItemID);
	i3::wstring wstr_msg = wstr_ItemName;	
	wstr_msg.append( L" : " );
	wstr_msg += GAME_RCSTRING("STR_INVEN_DELETE_ITEM_SUCCESS"); /*아이템이 삭제되었습니다.*/
	g_pFramework->PutSystemChatting(wstr_msg);

	switch( ctg )
	{
	case item_def::CTG_WEAPON:
	case item_def::CTG_PARTS:
	case item_def::CTG_SKIN :
	case item_def::CTG_CHARA:		break;
	case item_def::CTG_ITEM:
		{
			g_pFramework->PutSystemChatting(GAME_RCSTRING("STR_INVEN_DELETE_ITEM_SUCCESS")); /*아이템이 삭제되었습니다.*/

			if( m_re_autu_item_goods != 0 )
			{
				PurchaseMgr::i()->Auth_InGame(this, m_re_autu_item_goods);		//"finish_buy_auth_item",
				m_re_autu_item_goods = 0;
			}
		}
		break;
	}
}



/********************************/
void UIBattlePopupShop::close_shop()
{
	UIBattleFrame::i()->ClosePopup(UBP_INGAME_SHOP);
}

void UIBattlePopupShop::update_my_cash()
{
	UINT32 my_cash = UserInfoContext::i()->GetMyCash();
	i3::stack_wstring cash_str;
	i3_locale::itoa_comma(my_cash, cash_str);

	LuaState* L = _CallLuaFunction("SetMyCash");
	i3Lua::PushStringUTF16To8( L, cash_str);
	UILua::EndLuaFunction(L , 1);
}

void UIBattlePopupShop::select_category(INT32 ctg)
{
	if( curr_category == ctg ) return;
	curr_category = ctg;

	pf_keyinput = &UIBattlePopupShop::on_keyinput_category;

	m_pSlot->Entrance(ctg);
	m_pSlot->SelectSlot(-1);
}

void UIBattlePopupShop::to_lua_select_category(INT32 ctg)
{
	if( curr_category == ctg ) return;

	LuaState* L = _CallLuaFunction("ClickedTab");
	i3Lua::PushInteger(L, ctg);
	UILua::EndLuaFunction(L , 1);
}

void UIBattlePopupShop::try_remove_item( T_ItemID item_id)
{
	UINT32 pair_item_id = usf::GetDisableDuplicateUseItemPair(item_id);
	ITEM_QUERY_STYLE style = IQS_REMOVE;
	UIBattleFrame::i()->OpenPopup(UBP_DELETEITEM, (void *)&pair_item_id, &style);
}
