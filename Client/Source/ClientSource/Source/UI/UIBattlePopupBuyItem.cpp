#include "pch.h"
#include "UIBattlePopupBuyItem.h"


#include "UIBattleFrame.h"
#include "UIShopFunction.h"
#include "UIItemInfoUtil.h"
#include "PurchaseMgr.h"
#include "BuyUtil.h"
#include "GameGUI.h"
#include "UserInfoContext.h"
#include "UI/UIUtil.h"
#include "i3Base/ColorString.h"
#include "UISignal.h"
/**********************//**********************/
I3_CLASS_INSTANCE( UIBattlePopupBuyItem);
namespace
{
	UIBattlePopupBuyItem* g_this = 0;

	enum { CART_CLEAR = 0, CART_BUY, CART_CANCEL, };

	MultiSceneInfo g_scn_info;

	//Lua 경유해서 ClosePopup 할 경우 LuaStack이 종료되기 전에 날라가기 때문에
	//크래쉬 발생한다. 나쁘지만 간단하게 update 시점에서 ClosePopup 하면 된다
	bool g_close_popup = false;

	bool invalid_purchase_item = false;

	bool update_paymenu(UINT32 point, UINT32 cash, UINT32 pointPer, UINT32 cashPer);
	INT32 get_discount_price_cash(INT32 nCash, INT64 nCouponDBIdx);
}

/**********************//**********************/

extern "C" 
{
	int battle_buy_item( LuaState * L)
	{
		INT32 value = i3Lua::GetIntegerArg( L, 1);
		g_this->clicked( value );
		return 0;
	}
}

LuaFuncReg battle_buy_item_glue[] =
{
	{ "Click", battle_buy_item	},
	{ nullptr, nullptr}
};

void UIBattlePopupBuyItem::clicked(INT32 idx)
{
	switch(idx)
	{
	case CART_CLEAR:
		{
			buy::Clear_ByPurchaseRoute(m_purchase_route_type);
			mSlot.update_list();
		//	UI::call_cb("update_cart_board");		//상품 지웟으니 cart board를 갱신한다.
			UISignal::UpdateCartBoard();			//상품 지웟으니 cart board를 갱신한다.
		}
		break;
	case CART_BUY:
		{
			buy::DisplayedGoodsList displayedGoodsList;
			buy::GatherDisplayedGoods_ByPurchaseRoute(m_purchase_route_type, displayedGoodsList);
			PurchaseMgr::i()->Buy(this, m_purchase_route_type, displayedGoodsList);
		}
		break;
	case CART_CANCEL:
		g_close_popup = true;
		break;
	}
}

/**********************//**********************/
UIBattlePopupBuyItem::UIBattlePopupBuyItem()
{
	g_this = this;

	mSlot.SetParent( this );

	m_TotalPointPrice = 0;
	m_TotalCashPrice = 0;
}

bool UIBattlePopupBuyItem::check_enable_buy(i3::rc_wstring& out_fail_reason)
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
		buy::DisplayedGoods* displayedGood = buy::GetDisplayedGoods_ByPurchaseRoute(m_purchase_route_type, i);
		
		pGoods = displayedGood->GetCurrentGoods();

		if( item_def::get_category(pGoods->_ItemID) == item_def::CTG_CHARA )
		{
			if( item_def::get_type(pGoods->_ItemID) != ITEM_TYPE_DINO )
			{
				is_chara_goods = true;
			}
		}
		else
		{
			if( usf::IsValidBuyGoods(displayedGood->GetGoodsPack() ) != usf::vbt_Ok )
				enable_buy = false;
		}

	//	T_ItemDBIdx nCouponDBIdx = ShopPurchaseList::instance()->GetCouponIndex(i);

		GOODS_PRICE_INFO info;		
		usf::GetShopGoodInfo(pGoods, info);

		if( displayedGood->GetCurrentPriceType() == GOODS_PRICE_CASH )
		{
			m_TotalCashPrice += info._Cash;
			//nDiscountCash += get_discount_price_cash(cash, nCouponDBIdx);
		}
		else
		{
			m_TotalPointPrice += info._Point;
		}

		size_t nCouponEnable	= 0;

#if defined(USE_SALE_COUPON)
		// 버튼 활성화 조건 : 캐쉬템& (버튼이 활성화 상태 or 할인쿠폰 소유)  
		if(displayedGood->GetCurrentPriceType() == GOODS_PRICE_CASH)
		{
			if(CInvenList::i()->GetDiscountCouponCount() > 0 )/*||
				ShopPurchaseList::instance()->GetCouponIndex(i) > 0)*/
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
		out_fail_reason = GAME_RCSTRING("STR2_SHOP_DISABLE_BUY_PARTS_ANY_MORE");
		::ColorString(out_fail_reason, 255, 0, 0);
	}

	bool have_enough_money = update_paymenu( m_TotalPointPrice, m_TotalCashPrice, 0, nDiscountCash);
	if( have_enough_money == false )
	{
		out_fail_reason = GAME_RCSTRING("STBL_IDX_EP_SHOP_DEFICIT_MONEY");
		::ColorString(out_fail_reason, 255, 0, 0);

		enable_buy = false;
	}

	mSlot.is_buyable_chara = true;

	if (is_chara_goods && CCharaInfoContext::i()->GetEmptyCharacterIdx() == -1)
	{
		mSlot.is_buyable_chara = false;
		enable_buy = false;

		out_fail_reason = GAME_RCSTRING("STR2_SHOP_BUY_LIMIT_MSG");	/*더 이상 구입 할 수 없습니다.*/
		::ColorString(out_fail_reason, 255, 0, 0);
	}

	if( size == 0 ) enable_buy = false;	//cart에 물건이 없다.

	return enable_buy;
}

void UIBattlePopupBuyItem::update_item_paymenu()
{
	buy::RemoveInvalidDisplayedGoods_CartPreview(m_purchase_route_type, invalid_purchase_item);

	
	i3::rc_wstring reason;
	bool enable_buy = check_enable_buy(reason);

	LuaState * L = UILua::CallLuaFunction(g_scn_info.scn, "SetCharaNotice");
	i3Lua::PushStringUTF16To8( L, reason );
	UILua::EndLuaFunction(L , 1);

	//shopping cart 수에 따른 구입 버튼 활성화
	i3UIButtonComposed3* btn = (i3UIButtonComposed3*)g_scn_info.scn->FindChildByName( "i3UIButtonComposed3" );
	if(enable_buy == false)
	{
		btn->disable_btn();
	}
	else
	{
		btn->enable_btn();
	}
}


void UIBattlePopupBuyItem::RevertUseCoupon(INT32 nSlotIdx)
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

void UIBattlePopupBuyItem::ChangeCouponBtnTemplate(INT32 slot, bool bUseCoupon)
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
void UIBattlePopupBuyItem::ClosePopup() const
{
	g_close_popup = true;
}

void UIBattlePopupBuyItem::cb_close_duplicate_popup(void* pThis,INT32 nParam)
{
	((UIBattlePopupBuyItem*)pThis)->update_item_paymenu();
}

/*virtual*/ void UIBattlePopupBuyItem::OnCreate( i3GameNode * pParent)
{
	UIBattlePopupBase::OnCreate( pParent);

	g_scn_info.idx = GetSceneCount();
	AddScene("Scene/Popup/PBRe_PopUp_ShoppingCart.i3UIs", battle_buy_item_glue, true, false);

	mSlot.OnCreate( pParent );
}

/*virtual*/ void UIBattlePopupBuyItem::_InitializeAtLoad( i3UIScene * pScene)
{
	UIBattlePopupBase::_InitializeAtLoad(pScene);

	i3UIButtonComposed3* btn = (i3UIButtonComposed3*)GetScene(0)->FindChildByName( "i3UIButtonComposed3" );
	btn->set_disable_text_color(*GameGUI::GetColor(GCT_INVALID_C) );
}

/*virtual*/ void UIBattlePopupBuyItem::_ClearAtUnload( i3UIScene * pScene)
{
	UIBattlePopupBase::_ClearAtUnload( pScene);

	i3UIButtonComposed3* btn = (i3UIButtonComposed3*)GetScene(0)->FindChildByName( "i3UIButtonComposed3" );
	btn->enable_btn();
}

bool UIBattlePopupBuyItem::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	m_purchase_route_type = (buy::PurchaseRouteType)(UINT32)pArg1;

	m_pSigClose = reinterpret_cast<i3::unique_signal<void(bool, bool)>*>(pArg2);

//	m_cb_close = pArg2 ? (const char*)pArg2 : "";

	g_close_popup = false;
	invalid_purchase_item = false;

	return UIBattlePopupBase::OnEntranceStart(pArg1, pArg2);
}

void UIBattlePopupBuyItem::OnEntranceEnd(void)
{
	UIBattlePopupBase::OnEntranceEnd();

	update_item_paymenu();

	if(invalid_purchase_item)
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_SHOP_CART_OVERLAP_ERROR"), nullptr, MAKE_CALLBACK(cb_close_duplicate_popup), this) ;
}

/*virtual*/ void UIBattlePopupBuyItem::OnLoadAllScenes()
{
	UIBattlePopupBase::OnLoadAllScenes();

	g_scn_info.scn= GetScene( g_scn_info.idx );
	LuaState * L = _CallLuaFunction(g_scn_info.idx, "LoadAllScenes");
	UILua::EndLuaFunction(L , 0);

	mSlot.OnLoadAllScenes();
}

void	UIBattlePopupBuyItem::OnUnloadAllScenes()
{
	
	mSlot.OnUnloadAllScenes();
	UIBattlePopupBase::OnUnloadAllScenes();

	g_close_popup = false;
}

void UIBattlePopupBuyItem::OnUpdate( REAL32 rDeltaSeconds)
{
	UIBattlePopupBase::OnUpdate(rDeltaSeconds);

	mSlot.OnUpdate(rDeltaSeconds);

	if(g_close_popup)
	{
		if (m_pSigClose)
			(*m_pSigClose)(false, false);

		//UI::call_cb(m_cb_close);
		UIBattleFrame::i()->ClosePopup(UBP_BUYITEM);
	}
}

bool UIBattlePopupBuyItem::OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code)
{
	mSlot.OnEvent( event, pObj, param2, code );

	return UIBattlePopupBase::OnEvent( event, pObj, param2, code);
}

/*virtual*/ bool UIBattlePopupBuyItem::OnKey_Enter( void)
{
	i3::rc_wstring reason;
	bool enable_buy = check_enable_buy(reason);			// 낭비 루틴으로 보인다. (2014.07.18.수빈)

	if( enable_buy == true )
		clicked(CART_BUY);

	return false;
}



namespace
{
	bool update_paymenu(UINT32 point, UINT32 cash, UINT32 pointPer, UINT32 cashPer)
	{
		//pay_point, pay_cash, my_point, my_cash, result_point, result_cash
	//	i3::string discount_point, discount_cash;

		UINT32 nHaveCash, nHavePoint;

		nHaveCash = UserInfoContext::i()->GetMyCash();
		i3::stack_wstring my_cash;
		i3_locale::itoa_comma(nHaveCash, my_cash);

		nHavePoint = UserInfoContext::i()->GetMyPoint();

		i3::stack_wstring my_point;
		i3_locale::itoa_comma(nHavePoint, my_point);
		
		// 결제 금액(할인 금액 포함) 설정

		i3::stack_wstring	pay_point;
		i3_locale::itoa_comma(point, pay_point);

		i3::stack_wstring pay_cash;

			if(cashPer > 0)
			{
			//	i3::string frontCash, backCash;

//				frontCash = StringUtil::ToString( (int)(cashPer * -1) );	
//				StringUtil::ToComma(frontCash);
//				backCash = StringUtil::ToString( (int)(cash - cashPer) );
//				StringUtil::ToComma(backCash);

				//"(frontCash) backCash"
//				pay_cash = backCash;
				i3_locale::itoa_comma((int)(cash - cashPer), pay_cash);
			}
			else
			{
				i3_locale::itoa_comma(cash, pay_cash);
			}
		

		//할인률이 100이 넘으면 안 된다.
		//할인된 가격 = 원금*(100-할인률) * 0.01
		INT32 totPoint = INT32(nHavePoint) - INT32(point);//point * (100 - pointPer) * 0.01;
		INT32 totCash = INT32(nHaveCash) - (INT32(cash) - INT32(cashPer)); //cash * (100 - cashPer) * 0.01;


	
		// 최종 구매 후 잔액 설정
		
		i3::stack_wstring	result_point;
		i3_locale::itoa_comma(totPoint, result_point);
		
		//if( totPoint < 0 )
		//	StringUtil::ColorString(str1, 255, 0, 0);

		i3::stack_wstring	result_cash;
		i3_locale::itoa_comma(totCash, result_cash);

		//if( totCash < 0 )
		//	StringUtil::ColorString(str2, 255, 0, 0);
		

		//pay_point, pay_cash, my_point, my_cash, result_point, result_cash
		LuaState * L = UILua::CallLuaFunction(g_scn_info.scn, "SetPrice");
		i3Lua::PushStringUTF16To8( L, pay_point);
		i3Lua::PushStringUTF16To8( L, pay_cash );
		i3Lua::PushStringUTF16To8( L, my_point); 
		i3Lua::PushStringUTF16To8( L, my_cash );
		i3Lua::PushStringUTF16To8( L, result_point );
		i3Lua::PushStringUTF16To8( L, result_cash );
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