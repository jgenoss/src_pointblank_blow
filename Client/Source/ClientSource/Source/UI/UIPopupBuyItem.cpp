#include "pch.h"
#include "UIPopupBuyItem.h"

#include "UIMainFrame.h"
//#include "Shop.h"  //할인 쿠폰
#include "UIShopFunction.h"
#include "UIItemInfoUtil.h"
#include "PurchaseMgr.h"
#include "BuyUtil.h"
#include "InvenDef.h"
#include "InvenList.h"

#include "UIFloatCharacterView.h"
#include "UICharaInfoMgr.h"
#include "GameGUI.h"
#include "GachaContext.h"
#include "UserInfoContext.h"

#include "UI/UIUtil.h"
#include "i3Base/ColorString.h"

#include "UISignal.h"

/**********************//**********************/
I3_CLASS_INSTANCE( UIPopupBuyItem);//, UIPopupBase);

namespace
{
	UIPopupBuyItem* g_this = 0;

	enum { CART_CLEAR = 0, CART_BUY, CART_CANCEL, };

	MultiSceneInfo g_scn_info;

	//Lua 경유해서 ClosePopup 할 경우 LuaStack이 종료되기 전에 날라가기 때문에
	//크래쉬 발생한다. 나쁘지만 간단하게 update 시점에서 ClosePopup 하면 된다
	bool g_close_popup = false;
	bool is_clicked_cancel = false;

	bool invalid_purchase_item = false;

	bool update_paymenu(UINT32 point, UINT32 cash, UINT32 pointPer, UINT32 cashPer, bool is_called_gacha);
	INT32 get_discount_price_cash(INT32 nCash, T_ItemDBIdx nCouponDBIdx);
}

/**********************//**********************/

extern "C" 
{
	int buy_item( LuaState * L)
	{
		INT32 value = i3Lua::GetIntegerArg( L, 1);
		g_this->clicked( value );
		return 0;
	}
}

LuaFuncReg buy_item_glue[] =
{
	{ "Click",			buy_item	},
	{ nullptr,							nullptr}
};

void UIPopupBuyItem::clicked(INT32 idx)
{
	switch(idx)
	{
	case CART_CLEAR:
		{
			buy::Clear_ByPurchaseRoute(m_purchase_route_type);
			mSlot.update_list();
		//	UI::call_cb("update_cart_board");		//상품 지웟으니 cart board를 갱신한다.
			UISignal::UpdateCartBoard();			// //상품 지웟으니 cart board를 갱신한다.
		}
		break;
	case CART_BUY:
		{
			process_buy_item();
		}
		break;
	case CART_CANCEL:
		is_clicked_cancel = true;
		g_close_popup = true;
		break;
	}
}

UIPopupBuyItem::UIPopupBuyItem()
{
	g_this = this;

	mSlot.SetParent( this );

	m_TotalPointPrice = 0;
	m_TotalCashPrice = 0;
}

UIPopupBuyItem::~UIPopupBuyItem()
{
}

bool UIPopupBuyItem::check_enable_buy(i3::rc_wstring& out_wstr_fail_reason)
{
	m_TotalPointPrice = 0; 
	m_TotalCashPrice = 0;

	UINT32 nDiscountCash = 0;

		bool is_chara_goods = false;
	bool enable_buy = true;

	const SHOP_GOODS* pGoods = 0;
	INT32 size = buy::GetCount_ByPurchaseRouteType(m_purchase_route_type);

	for(INT32 i = 0; i < size; ++i)
	{
		buy::DisplayedGoods* displayedGoods = buy::GetDisplayedGoods_ByPurchaseRoute(m_purchase_route_type, i);
		
		if (displayedGoods == nullptr)
		{
			enable_buy = false;
			continue;
		}

		pGoods = displayedGoods->GetCurrentGoods(); //현재 선택된 goods

		if( pGoods == nullptr)
		{
			enable_buy = false;
			continue;
		}
		
		if( item_def::get_category_goodsID(pGoods->_GoodsID) == item_def::CTG_CHARA )
		{
			if( item_def::get_type(pGoods->_ItemID) != ITEM_TYPE_DINO )
			{
				is_chara_goods = true;
			}
		}

		GOODS_PRICE_INFO info;		
		usf::GetShopGoodInfo(pGoods, info);

		if(displayedGoods->GetCurrentPriceType() == GOODS_PRICE_CASH )
		{
			m_TotalCashPrice += info._Cash;
		}
		else
		{
			m_TotalPointPrice += info._Point;
		}

		size_t nCouponEnable	= 0;

#if defined(USE_SALE_COUPON)
		// 버튼 활성화 조건 : 캐쉬템& (버튼이 활성화 상태 or 할인쿠폰 소유)  
		if(displayedGoods->GetCurrentPriceType() == GOODS_PRICE_CASH)
		{
			if(CInvenList::i()->GetDiscountCouponCount() > 0 )
			{
				nCouponEnable = 1;
			}
		}
		else
		{
			// 구매 타입이 Cash/Point를 선택하는 국가에 대한 예외처리
			RevertUseCoupon(i);
		}
#endif
	}

	//최종 가격
	if(enable_buy == false)
	{
		out_wstr_fail_reason = GAME_RCSTRING("STR2_SHOP_DISABLE_BUY_PARTS_ANY_MORE");
		::ColorString(out_wstr_fail_reason, 255, 0, 0);
	}

	bool have_enough_money = update_paymenu( m_TotalPointPrice, m_TotalCashPrice, 0, nDiscountCash, false);
	if( have_enough_money == false )
	{
		out_wstr_fail_reason = GAME_RCSTRING("STBL_IDX_EP_SHOP_DEFICIT_MONEY");
		::ColorString(out_wstr_fail_reason, 255, 0, 0);

		enable_buy = false;
	}

	mSlot.is_buyable_chara = true;

	if (is_chara_goods && CCharaInfoContext::i()->GetEmptyCharacterIdx() == -1)
	{
		mSlot.is_buyable_chara = false;
		enable_buy = false;

		out_wstr_fail_reason = GAME_RCSTRING("STR2_SHOP_BUY_LIMIT_MSG");	/*더 이상 구입 할 수 없습니다.*/
		::ColorString(out_wstr_fail_reason, 255, 0, 0);
	}

	if( size == 0 ) enable_buy = false;	//cart에 물건이 없다.

	return enable_buy;
}

bool UIPopupBuyItem::check_enable_buy_Gacha(i3::rc_wstring & out_wstr_fail_reason)
{
	m_TotalPointPrice = 0; 
	m_TotalCashPrice = 0;
//	UINT32 point = 0; UINT32 cash = 0;

	UINT32 nDiscountCash = 0;

	static const bool is_chara_goods = false;
	bool enable_buy = true;

	size_t size = buy::GetCount_ByPurchaseRouteType(m_purchase_route_type);

	if( GachaContext::i()->GetGachaBuyType() == GOODS_BUY_CASH)
	{
		m_TotalCashPrice = GachaContext::i()->GetGachaBuyCashPrice();
	}
	else
	{
		m_TotalPointPrice = GachaContext::i()->GetGachaBuyPointPrice();
	}

//최종 가격 : 실행 안되는 코드라 일단 주석으로 걸어둠.  (2016.07.29.수빈)
//	if(enable_buy == false)
//	{
//		out_wstr_fail_reason = GAME_RCSTRING("STR2_SHOP_DISABLE_BUY_PARTS_ANY_MORE");
//		::ColorString(out_wstr_fail_reason, 255, 0, 0);
//	}

	bool have_enough_money = update_paymenu( m_TotalPointPrice, m_TotalCashPrice, 0, nDiscountCash, false);
	if( have_enough_money == false )
	{
		out_wstr_fail_reason = GAME_RCSTRING("STBL_IDX_EP_SHOP_DEFICIT_MONEY");
		::ColorString(out_wstr_fail_reason, 255, 0, 0);

		enable_buy = false;
	}

	mSlot.is_buyable_chara = true;

	if (is_chara_goods && CCharaInfoContext::i()->GetEmptyCharacterIdx() == -1 )
	{
		mSlot.is_buyable_chara = false;
		enable_buy = false;

		out_wstr_fail_reason = GAME_RCSTRING("STR2_SHOP_BUY_LIMIT_MSG");	/*더 이상 구입 할 수 없습니다.*/
		::ColorString(out_wstr_fail_reason, 255, 0, 0);
	}

	if( size == 0 ) enable_buy = false;	//cart에 물건이 없다.

	return enable_buy;
}

bool UIPopupBuyItem::check_enable_buy_gift(i3::rc_wstring & out_wstr_fail_reason)
{
	m_TotalPointPrice = 0; 
	m_TotalCashPrice = 0;

	bool enable_buy = true;

	const SHOP_GOODS* pGoods = 0;
	size_t size = buy::GetCount_ByPurchaseRouteType(m_purchase_route_type);

	for(size_t i = 0; i < size; ++i)
	{
		buy::DisplayedGoods* displayedGoods = buy::GetDisplayedGoods_ByPurchaseRoute(m_purchase_route_type, i);		

		pGoods = displayedGoods->GetCurrentGoods(); ////현재 선택된 goods
		
		if( pGoods == nullptr)
		{
			enable_buy = false;
			continue;
		}

		GOODS_PRICE_INFO info;		
		usf::GetShopGoodInfo(pGoods, info);

		if(displayedGoods->GetCurrentPriceType() == GOODS_PRICE_CASH )
		{
			m_TotalCashPrice += info._Cash;
		}
		else
		{
			m_TotalPointPrice += info._Point;
		}
	}

	bool have_enough_money = update_paymenu( m_TotalPointPrice, m_TotalCashPrice, 0, 0, false);
	if( have_enough_money == false )
	{
		out_wstr_fail_reason = GAME_RCSTRING("STBL_IDX_EP_SHOP_DEFICIT_MONEY");
		::ColorString(out_wstr_fail_reason, 255, 0, 0);
		enable_buy = false;
	}

	if( size == 0 ) enable_buy = false;	//cart에 물건이 없다.

	return enable_buy;
}


void UIPopupBuyItem::update_item_paymenu()
{
	buy::RemoveInvalidDisplayedGoods_CartPreview(m_purchase_route_type, invalid_purchase_item);
	

	i3::rc_wstring wstr_reason;
	bool enable_buy = false;

	switch( GetMainFrame()->GetCurrentSubPhaseType() )
	{
	case USP_SHOP		: enable_buy = check_enable_buy(wstr_reason); break;
	case USP_GACHA		: enable_buy = check_enable_buy_Gacha(wstr_reason); break;
	case USP_GIFTSHOP	: enable_buy = check_enable_buy_gift(wstr_reason); break;
	default				: enable_buy = check_enable_buy(wstr_reason); break;
	}

	LuaState * L = UILua::CallLuaFunction(g_scn_info.scn, "SetCharaNotice");
	i3Lua::PushStringUTF16To8( L, wstr_reason );
	UILua::EndLuaFunction(L , 1);

	//shopping cart 수에 따른 구입 버튼 활성화
	if(enable_buy == false)
	{
		buyBtn->disable_btn();
	}
	else
	{
		buyBtn->enable_btn();
	}
}



void UIPopupBuyItem::process_buy_item(void)
{
	switch( GetMainFrame()->GetCurrentSubPhaseType() )
	{
	case USP_GACHA :
		{
			GameEventReceiver::i()->PushReceivedGameEvent(EVENT_BUY_GOODS);
		}
		break;
	case USP_GIFTSHOP:
		{
			GetMainFrame()->OpenPopup(UPW_GIFT_SEND);
		}
		break;
	default:
		{
			i3::rc_wstring wstrShopItemName;

			buy::DisplayedGoodsList		checkPurchase, purchase;
					
			buy::GatherDisplayedGoods_ByPurchaseRoute(m_purchase_route_type, checkPurchase);
						
			const size_t numCheckGoods = checkPurchase.size();

			for ( size_t i = 0 ; i < numCheckGoods ; ++i )	
			{
				const buy::DisplayedGoodsPtr& pDisplayedGoods = checkPurchase[i];

				UINT32 buySlotIdx = pDisplayedGoods->GetCurrentPackIndex();	
				const SHOP_GOODS_PACK* pGoodsPack = pDisplayedGoods->GetGoodsPack();
				
				if (false == usf::CheckBuyItemPeriod(pGoodsPack, buySlotIdx))
				{
					// 최대 기간/갯수 제한팝업, 하나라도 제한에 걸리는게 있다면 리턴(구매X)
					GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_INVEN_PERIOD_LIMIT_POPUP"), &GAME_RCSTRING("STR_TBL_GLOBALDEF_GAME_ERROR_TEXT2"));
					return;
				}
				
				// 구매되는 아이템이름
				SHOP_GOODS* goods = pGoodsPack->GetGoods(buySlotIdx);	//	iter->pack->GetGoods(buySlotIdx);
				usf::GetShopItemName(wstrShopItemName, goods->_ItemID);
				I3TRACE("ItemName = %s \n", wstrShopItemName);

				purchase.push_back(pDisplayedGoods); 
			}
			
			if (purchase.size() < 1)
			{
				I3TRACE( "purchase.list.size() < 1 \n" );
				return;
			}

			PurchaseMgr::i()->Buy(this, m_purchase_route_type, purchase);
		}
		break;
	}

	is_clicked_cancel = false;
}

void UIPopupBuyItem::set_notice_message(void)
{
	i3::rc_wstring wstr_notice;

	if( GetMainFrame()->GetCurrentSubPhaseType() == USP_GIFTSHOP )
		wstr_notice = GAME_RCSTRING("STR_SHOP_GIFT_NOTICE");
	else
		wstr_notice = GAME_RCSTRING("STR2_SHOP_NOTICE");

	LuaState * L = UILua::CallLuaFunction(g_scn_info.scn, "SetNotice");
	i3Lua::PushStringUTF16To8( L, wstr_notice );
	UILua::EndLuaFunction(L, 1);
}

void UIPopupBuyItem::RevertUseCoupon(INT32 nSlotIdx)
{
	//기능을 몰라서 일단 막자
	return;

#if defined(USE_SALE_COUPON)
	// [쿠폰 취소] 버튼 -> 취소후 사용 가능한 상태로 변경
	//T_ItemDBIdx nCouponDBIdx = ShopPurchaseList::instance()->GetCouponIndex(nSlotIdx);
	//if(nCouponDBIdx >= 0)
	//{
	//	CInvenList::i()->SetUsedCoupon(nCouponDBIdx , false);
	//	ShopPurchaseList::instance()->SetCouponIndex(nSlotIdx, -1);
	//	ChangeCouponBtnTemplate(nSlotIdx, false);
	//}
#endif
}

void UIPopupBuyItem::ChangeCouponBtnTemplate(INT32 slot, bool bUseCoupon)
{
	LuaState * L = _CallLuaFunction("GetCouponBtnCtrl");

	if(L != nullptr)
	{
		i3Lua::PushInteger(L , slot);
		i3UIControl * pCtrl = reinterpret_cast<i3UIControl*>(_ReturnLuaPointer(L , 1));
		if(bUseCoupon == true)
			pCtrl->SetTemplateByName("BtC3_List_sR");
		else
			pCtrl->SetTemplateByName("BtC3_List_sB");
	}
}


/**********************//**********************/
void UIPopupBuyItem::ClosePopup() const
{
	g_close_popup = true;
}

void UIPopupBuyItem::cb_close_duplicate_popup(void* pThis,INT32 nParam)
{
	((UIPopupBuyItem*)pThis)->update_item_paymenu();
}

/*virtual*/ void UIPopupBuyItem::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	g_scn_info.idx = GetSceneCount();

	AddScene("Scene/Popup/PBRe_PopUp_ShoppingCart.i3UIs", buy_item_glue, true, false);

	mSlot.OnCreate( pParent );

	
}

/*virtual*/ void UIPopupBuyItem::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad(pScene);

	buyBtn = (i3UIButtonComposed3*)GetScene(0)->FindChildByName( "i3UIButtonComposed3" );
	buyBtn->set_disable_text_color(*GameGUI::GetColor(GCT_INVALID_C) );

}

/*virtual*/ void UIPopupBuyItem::_ClearAtUnload( i3UIScene * pScene)
{
	UIPopupBase::_ClearAtUnload( pScene);

	i3UIButtonComposed3* btn = (i3UIButtonComposed3*)GetScene(0)->FindChildByName( "i3UIButtonComposed3" );
	btn->enable_btn();
}

bool UIPopupBuyItem::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	m_purchase_route_type = (buy::PurchaseRouteType)(UINT32)pArg1;
	m_pSigClose = reinterpret_cast<i3::unique_signal<void(bool, bool)>*>(pArg2);
//	m_cb_close = pArg2 ? (const char*)pArg2 : "";

	g_close_popup = false;
	invalid_purchase_item = false;

	return UIPopupBase::OnEntranceStart(pArg1, pArg2);
}

void UIPopupBuyItem::OnEntranceEnd(void)
{
	UIPopupBase::OnEntranceEnd();

	update_item_paymenu();
	set_notice_message();

	// Key : User Guide System
	{
		UserGuideMgr::i()->OnLoadGuideTargetInfo(this);
	}

	if(invalid_purchase_item)
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_SHOP_CART_OVERLAP_ERROR"), nullptr, MAKE_CALLBACK(cb_close_duplicate_popup), this) ;
}

/*virtual*/ void UIPopupBuyItem::OnLoadAllScenes()
{
	UIPopupBase::OnLoadAllScenes();

	g_scn_info.scn= GetScene( g_scn_info.idx );
	LuaState * L = _CallLuaFunction(g_scn_info.idx, "LoadAllScenes");
	UILua::EndLuaFunction(L , 0);

	mSlot.OnLoadAllScenes();
}

void	UIPopupBuyItem::OnUnloadAllScenes()
{
	if( g_pEnvSet->IsV2Version() )
	{
		UIFloatCharacterView * pCharaViewWin = static_cast<UIFloatCharacterView*>(GameUI::GetFloating(UFW_CHARACTER));
		if( pCharaViewWin != nullptr && pCharaViewWin->isEnable() )
		{
			if(m_purchase_route_type == buy::PurchaseRoute_Immediate )
			{
				const buy::DisplayedGoods* pDisplayedGoods = buy::GetDisplayedGoods_Immediate();	
				if(pDisplayedGoods == nullptr ) // 확인 버튼 클릭한 경우
				{
				}
				else //취소 버튼 클릭한 경우
				{
					const SHOP_GOODS* pSelectedGoods = pDisplayedGoods->GetCurrentGoods();
					item_def::Category category = item_def::get_category_goodsID(pSelectedGoods->_GoodsID );
					if( category == item_def::CTG_PARTS || category == item_def::CTG_SKIN)
						pCharaViewWin->Update_CharaView_Parts_Preview();
					else
						pCharaViewWin->RevertChara();			// finish이후이므로 preview 리스트만 지워지고, enable은 건드리지 않음..
				}
			}
			else if(m_purchase_route_type == buy::PurchaseRoute_Preview )
			{
				pCharaViewWin->Update_CharaView_Parts_Preview();
			}
		}
	}

	UISignal::UpdateCartBoard();
	UISignal::UpdatePreviewSlot();

	mSlot.OnUnloadAllScenes();
	UIPopupBase::OnUnloadAllScenes();
}

void UIPopupBuyItem::OnUpdate( REAL32 rDeltaSeconds)
{
	UIPopupBase::OnUpdate(rDeltaSeconds);

	mSlot.OnUpdate(rDeltaSeconds);

	size_t size = buy::GetCount_ByPurchaseRouteType(m_purchase_route_type); 

	if(size == 0)
	{
		buyBtn->disable_btn();
	}

	if(g_close_popup)
	{
	//	UI::call_cb(m_cb_close, 0, (void*)(is_clicked_cancel ? 1 : 0));
		if (m_pSigClose)
			(*m_pSigClose)(false, is_clicked_cancel);

		GetMainFrame()->ClosePopup(UPW_BUYITEM);
	}
}

bool UIPopupBuyItem::OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code)
{
	mSlot.OnEvent( event, pObj, param2, code );

	return UIPopupBase::OnEvent( event, pObj, param2, code);
}

/*virtual*/ bool UIPopupBuyItem::OnKey_Enter( void)
{
	i3::rc_wstring wstr_reason;
	bool enable_buy = false;

	if(g_pFramework->GetUIMainframe()->GetCurrentSubPhaseType() == USP_GACHA)
		enable_buy = check_enable_buy_Gacha(wstr_reason);									// 루틴이 다소 낭비 스럽다..(2014.07.17.수빈)
	else
		enable_buy = check_enable_buy(wstr_reason);											// 루틴이 다소 낭비 스럽다..(2014.07.17.수빈)

	if( enable_buy == true )
		clicked(CART_BUY);

	return false;
}



namespace
{
	UINT32 get_my_cash_case_about_gacha()
	{
		UINT32 nGoodid = GachaContext::i()->GetGachaBuyGoodId();

		UINT32 itemCnt = CInvenList::i()->GetItemCount( INVEN::DB_CASHITEM);
		for( size_t i = 0; i < itemCnt; ++i)
		{
			const INVEN_BUFFER* item = CInvenList::i()->GetItem( INVEN::DB_CASHITEM, i);
			if( item->_ui32ItemID == nGoodid)
			{
				return item->_ui32ItemArg;
			}
		}
		return 0;
	}


	bool update_paymenu(UINT32 point, UINT32 cash, UINT32 pointPer, UINT32 cashPer, bool is_called_gacha)
	{
		//pay_point, pay_cash, my_point, my_cash, result_point, result_cash
	
	//	i3::string discount_point, discount_cash;
	
		INT64 nHaveCash, nHavePoint;

		nHaveCash = UserInfoContext::i()->GetMyCash();

		if( is_called_gacha == false )
			nHavePoint = UserInfoContext::i()->GetMyPoint();
		else
			nHavePoint = get_my_cash_case_about_gacha();

		i3::stack_wstring my_cash;		i3_locale::itoa_comma(nHaveCash, my_cash);

		i3::stack_wstring my_point;		i3_locale::itoa_comma(nHavePoint, my_point);

		// 결제 금액(할인 금액 포함) 설정
		
		i3::stack_wstring	pay_point;	i3_locale::itoa_comma(point, pay_point);

		i3::stack_wstring pay_cash;

		if(cashPer > 0)
		{
			//i3::string frontCash, backCash;

			//frontCash = StringUtil::ToString( (int)(cashPer * -1) );	
			//StringUtil::ToComma(frontCash);
			//backCash = StringUtil::ToString( (int)(cash - cashPer) );
			//StringUtil::ToComma(backCash);

			//"(frontCash) backCash"
			//pay_cash = backCash;
			
			i3_locale::itoa_comma((int)(cash - cashPer), pay_cash);
		}
		else
		{
			i3_locale::itoa_comma( cash, pay_cash);
		}


		//할인률이 100이 넘으면 안 된다.
		//할인된 가격 = 원금*(100-할인률) * 0.01
		INT64 totPoint = nHavePoint - INT64(point);//point * (100 - pointPer) * 0.01;
		INT64 totCash = nHaveCash - (INT64(cash) - INT64(cashPer)); //cash * (100 - cashPer) * 0.01;

		// 최종 구매 후 잔액 설정
		
		i3::wstring	result_point;	i3_locale::itoa_comma(totPoint, result_point);

		if( totPoint < 0 )
			::ColorString(result_point, 255, 0, 0);

		i3::wstring	result_cash;	i3_locale::itoa_comma(totCash, result_cash);

		if( totCash < 0 )
			::ColorString(result_cash, 255, 0, 0);

		if(pay_cash.length() <= 0)
			i3::generic_string_cat(pay_cash, L"0");

		if(pay_point.length() <= 0)
			i3::generic_string_cat(pay_point, L"0");

		//pay_point, pay_cash, my_point, my_cash, result_point, result_cash
		LuaState * L = UILua::CallLuaFunction(g_scn_info.scn, (is_called_gacha == false) ? "SetPrice" : "SetGachaPrice" );
		i3Lua::PushStringUTF16To8( L,pay_point );
		i3Lua::PushStringUTF16To8( L,pay_cash );
		i3Lua::PushStringUTF16To8( L,my_point ); 
		i3Lua::PushStringUTF16To8( L,my_cash );
		i3Lua::PushStringUTF16To8( L,result_point );
		i3Lua::PushStringUTF16To8( L,result_cash );
		UILua::EndLuaFunction(L , 6);

		if( point == 0 ) return totCash >= 0;
		if( cash == 0 ) return totPoint >= 0;

		return (totPoint >= 0 && totCash >= 0);
	}

	INT32 get_discount_price_cash(INT32 nCash, T_ItemDBIdx nCouponDBIdx)
	{
		INT32 nDiscountCash = 0;

#if defined(USE_SALE_COUPON)
		//if(nCouponDBIdx > 0)
		//{
		//	CInvenList * pInvenList = CInvenList::i();

		//	INT32 nCouponID = pInvenList->FindItemIDByDBIdx( INVEN::DB_COUPON, nCouponDBIdx);

		//	if(nCouponID > 0)
		//	{
		//		SALE_COUPON * pCoupon = CShop::i()->GetCouponData(nCouponID);

		//		if(pCoupon->m_ui8SaleType == SALE_TYPE_PER)
		//			nDiscountCash = static_cast<INT32>(nCash * (pCoupon->m_ui32DisCount * 0.01));
		//		else
		//			nDiscountCash = pCoupon->m_ui32DisCount;
		//	}
		//}
#endif

		return nDiscountCash;
	}
}