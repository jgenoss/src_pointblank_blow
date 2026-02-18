#include "pch.h"
#include "UI_V11/UIPopupBasket_V11.h"
#include "UI_V11/UITabShopInform.h"
#include "UI_V11/ShopContextBasket.h"
#include "UI_V11/UIPhaseShop_V11.h"

#include "UI/UIMainFrame_V11.h"
#include "UI/UIUtil.h"
#include "UI/UIShopFunction.h"
#include "UI/PurchaseMgr.h"
#include "UI/BuyUtil.h"
#include "../../ui/CBFuction.h"

#include "Shop.h"
#include "UserInfoContext.h"

#include "i3Base/ColorString.h"

I3_CLASS_INSTANCE( UIPopupBasket_V11);

namespace
{
	enum { UI_BUY_TIMEOUT = 20 };

	//combobox 에 넣는 string 을 만든다.
	//point / cash 으로 들어간다.
	i3::wstring getPriceString( SHOP_GOODS_PACK* packs )
	{
		i3::wstring point, cash, prStr;
		point.clear(); cash.clear(); prStr.clear();

		INT32 size = packs->GetGoodsCount();
		for(INT32 i = 0; i < size; ++i)
		{
			SHOP_GOODS* pack = packs->GetGoods( i );
			if( pack->GetPriceType(usf::g_price_idx) == GOODS_PRICE_ALL )
			{
				prStr.clear();
				usf::GetShopPriceString(pack, prStr, GOODS_PRICE_POINT );
				i3::generic_string_cat(point,prStr);	i3::generic_string_cat(point,L";");

				prStr.clear();
				usf::GetShopPriceString(pack, prStr, GOODS_PRICE_CASH );
				i3::generic_string_cat(cash,prStr);	i3::generic_string_cat(cash,L";");
			}
			else
			{
				prStr.clear();
				usf::GetShopPriceString(pack, prStr, pack->GetPriceType(usf::g_price_idx) );
				if( pack->_SaleType == GOODS_PRICE_CASH )	
				{
					i3::generic_string_cat(point,prStr);	i3::generic_string_cat(point,L";");
				}
				else
				{
					i3::generic_string_cat(cash,prStr);	i3::generic_string_cat(cash,L";");
				}
			}
		}

		//point String 뒤로 cash String 을 넣는다.
		i3::generic_string_cat(point,cash);

		return point;
	}
}

static UIPopupBasket_V11 * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);
	return (UIPopupBasket_V11*) pFrame->GetPopup( UPW_BASKET );
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//

extern "C" {
	int UIPopupBasketInput_Confirm(LuaState * L)	{	GetThis()->Confirm();		return 0;	}
	int UIPopupBasketInput_Cancel(LuaState * L)	{	GetThis()->Cancel();		return 0;	}

	int Click_Slot_X(LuaState* L)	{	
		INT32 inx = i3Lua::GetIntegerArg( L, 1);
		GetThis()->Click_Slot_X( inx );		
		return 0;	
	}

	int Click_Slot_Coupon(LuaState* L)	{

		INT32 nSlotIdx = i3Lua::GetIntegerArg(L , 1);
		i3UIControl * pBtn = (i3UIControl * )i3Lua::GetUserDataArg(L , 2);
		GetThis()->OnClickCouponBtn(pBtn, nSlotIdx);
		return 0;
	}

	int ChangeCombo(LuaState* L){
		INT32 index = i3Lua::GetIntegerArg( L, 1);
		INT32 tab = i3Lua::GetIntegerArg( L, 2);
		GetThis()->Change_Combo( index, tab);
		return 0;
	}
}

LuaFuncReg UIPopupBasketInput_Glue[] = {
	{"Confirm",				UIPopupBasketInput_Confirm	},
	{"Cancel",					UIPopupBasketInput_Cancel	},

	{ "uiClick_Cart_X",		Click_Slot_X	 },
	{"uiChangeCombo", ChangeCombo	},
	{"uiClick_Cart_Coupon",		Click_Slot_Coupon},

	{NULL,						NULL						}
};

//------------------------------------------------------------------------------//


UIPopupBasket_V11::UIPopupBasket_V11()
{
	m_pShopContext = CShopContext::i();
	m_pShopBasket = m_pShopContext->GetBasketPtr();

	m_nCurrentBasketSlotIdx = -1;
	
	m_pParent = NULL;
	m_TotalPointPrice = 0;
	m_TotalCashPrice = 0;
}

UIPopupBasket_V11::~UIPopupBasket_V11()
{
}

/*virtual*/ void UIPopupBasket_V11::OnCreate( i3GameNode * pParent)
{
	Base::OnCreate( pParent);

	AddScene("UIRe1/Scene/Popup/PBRe_PopUp_Shop_Cart.i3UIs",UIPopupBasketInput_Glue);
}

/*virtual*/ bool UIPopupBasket_V11::OnEntranceStart(void * pArg1 , void * pArg2 )
{
	if( Base::OnEntranceStart(pArg1,pArg2) == false)
		return false;

	m_pParent = static_cast< UITabShopInform* >(pArg1);

	_ClearUseCoupon();

	return true;
}

/*virtual*/ void UIPopupBasket_V11::_InitializeAtLoad(i3UIScene * pScene)
{
	Base::_InitializeAtLoad(pScene);

	size_t size = m_pShopBasket->size();
	for(size_t i = 0; i < size; ++i)
	{
		m_pShopBasket->set_pack_inx(i, 0);
	}

	_updateList();
}

/*virtual*/ void UIPopupBasket_V11::ProcessGameEvent(GAME_EVENT event, INT32 Arg, const i3::user_data& UserData)
{
	Base::ProcessGameEvent(event,Arg,UserData);
	switch(event)
	{
		case EVENT_BUY_GOODS :
			if( EV_SUCCESSED( Arg ) )
			{
				NotifySuccess();
			}
			else
			{
				NotifyFail(Arg);				
			}
		break;
	}
}

void	UIPopupBasket_V11::OnUpdate( REAL32 rDeltaSeconds)
{
	Base::OnUpdate(rDeltaSeconds);
}

void UIPopupBasket_V11::Confirm()
{
	//장바구니에 아이템에 없는 경우엔 밖으러~~~
	if( m_pShopBasket->size() == 0 )
	{
		GetMainFrame()->TogglePopup(UPW_BASKET);
		return;
	}

	//구매 할 돈 량을 본다.
	if( UserInfoContext::i()->GetMyPoint() < m_TotalPointPrice ||  
		UserInfoContext::i()->GetMyCash() < m_TotalCashPrice )
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STBL_IDX_EP_SHOP_DEFICIT_MONEY"), &GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_FAIL_CAPTION_E"));
		return;
	}
	
	for(size_t i = 0; i < m_pShopBasket->size() ; i++)
	{
		buy::op::push_cart(  (*m_pShopBasket)[i], usf::g_price_idx, m_pShopBasket->get_pack_inx(i));
	}
	buy::purchase purchase;
	purchase.type = buy::Cart;
	buy::op::copy_buy_item_in_purchase(purchase.type, purchase.list);
	//((UIPhaseShop_V11*)GetMainFrame()->GetSubPhase(USP_SHOP))->Do_Process_Buy_V2(buy::Cart, purchase);
	//PurchaseMgr::i()->buy(this, purchase);

	SetInputDisable(TRUE);
}

void UIPopupBasket_V11::NotifyFail(INT32 Arg)
{
	SetInputDisable(FALSE);
	_ClearUseCoupon();
	GetMainFrame()->TogglePopup(UPW_BASKET);
}

void UIPopupBasket_V11::NotifySuccess()
{
#if defined(USE_SALE_COUPON) && defined(ENABLE_UI2)
	// 사용된 쿠폰을 인벤에서 제거
	CInvenList::i()->DeleteAllCouponBuffer();
#endif 

	m_pShopBasket->clear();

	m_pParent->UpdateBasketList();

	UIPhaseShop_V11* phase = static_cast<UIPhaseShop_V11*>(GetMainFrame()->GetSubPhase(USP_SHOP));
	phase->UpdateGoods_IfPurchased();
	
	SetInputDisable(FALSE);
	GetMainFrame()->TogglePopup(UPW_BASKET);
}

void UIPopupBasket_V11::Cancel()
{
	_ClearUseCoupon();
	GetMainFrame()->TogglePopup(UPW_BASKET);
}


void UIPopupBasket_V11::_toLua(const char* lua_name)
{
	LuaState* L = UILua::CallLuaFunction(GetScene(0), lua_name);
	I3ASSERT( L != NULL );

	//Int 형 매겨변수로 넘긴다.
	size_t sizeInt = m_LuaIntAtt.size();
	for(size_t i = 0; i < sizeInt; ++i)	{
		i3Lua::PushInteger( L , m_LuaIntAtt[i] );
	}
	m_LuaIntAtt.clear();

	//String 형 매겨변수로 넘긴다.
	size_t sizeString = m_LuaStringAtt.size();
	for(size_t i = 0; i < sizeString; ++i)	{
		i3Lua::PushStringUTF16To8( L , m_LuaStringAtt.at(i).c_str() );
	}
	m_LuaStringAtt.clear();

	//BOOL 형 매겨변수로 넘긴다.
	size_t sizeBool = m_LuaBoolAtt.size();
	for(size_t i = 0; i < sizeBool; ++i)	{
		i3Lua::PushBoolean( L , m_LuaBoolAtt[i] );
	}
	m_LuaBoolAtt.clear();

	UILua::EndLuaFunction(L , sizeInt + sizeString + sizeBool);
}

void UIPopupBasket_V11::_updateCoupon(UINT32 coupon)
{
	/*현재 사용 가능한 쿠폰이 {col:255,127,39,255}총 %d개{/col} 가 있습니다.*/
	i3::rc_wstring str;
	i3::sprintf(str, GAME_RCSTRING("STR_TBL_DISCOUNT_COUPON_NOTICE"), coupon); 

	_addLua(str);
	_addLua(coupon > 0 ? true : false);
	_toLua( "set_coupon" );
}

void UIPopupBasket_V11::_updatePrice(UINT32 point, UINT32 cash, 
								 UINT32 pointPer, UINT32 cashPer)
{
	i3::stack_wstring str1, str2;

	INT64 nHaveCash, nHavePoint;

	nHaveCash  = UserInfoContext::i()->GetMyCash();
	nHavePoint = UserInfoContext::i()->GetMyPoint();

	i3_locale::itoa_comma(nHavePoint, str1);
	i3_locale::itoa_comma(nHaveCash, str2);
	_addLua(str1);	_addLua(str2);
	_toLua("set_price");
	
	// 결제 금액(할인 금액 포함) 설정
	{
		i3_locale::itoa_comma(point, str1);
		if(cashPer > 0)
		{
			i3::stack_wstring frontCash, backCash;
			i3_locale::itoa_comma(((int)(cashPer * -1)), frontCash);
			i3_locale::itoa_comma(((int)(cash - cashPer)), backCash);
			str2.assign( L"(" );		str2.append( frontCash );		str2.append( L")" );
			str2.append( L" " );		str2.append( backCash );
		}
		else
		{
			i3_locale::itoa_comma(cash, str2);
		}
		_addLua(str1);	_addLua(str2);
		_addLua(cashPer > 0 ? TRUE : FALSE);
		_toLua("set_discount");
	}

	// 최종 구매 후 잔액 설정
	{
		//할인률이 100이 넘으면 안 된다.
		//할인된 가격 = 원금*(100-할인률) * 0.01
		INT64 totPoint = nHavePoint - point;//point * (100 - pointPer) * 0.01;
		INT64 totCash = nHaveCash - (cash - cashPer); //cash * (100 - cashPer) * 0.01;

		i3_locale::itoa_comma(totPoint, str1);
		if( totPoint < 0 )
			::ColorString(str1, 255, 0, 0);

		i3_locale::itoa_comma(totCash, str2);
		if( totCash < 0 )
			::ColorString(str2, 255, 0, 0);

		_addLua(str1);	_addLua(str2);
		_toLua("set_tot_price");
	}
}

void UIPopupBasket_V11::_updateList()
{
	// 상품 목록을 다시 만든다.
	i3::rc_wstring name;
	i3::wstring price;
	
	m_TotalPointPrice = 0; 
	m_TotalCashPrice = 0;

	UINT32 nDiscountCash = 0;
	GOODS_PRICE_TYPE PriceType = 0;

	for(size_t i = 0; i < USE_BASKET_COUNT; ++i)
	{
		//현재 선택된 goods
		SHOP_GOODS* pGoods = m_pShopBasket->get_sel( i );
		if( pGoods != NULL )
		{
			INT64 nCouponDBIdx = m_pShopBasket->GetCouponIndex(i);
			GOODS_PRICE_INFO price_info;
			usf::GetShopGoodInfo(pGoods, price_info);
			PriceType = price_info._Type;
			if( PriceType == GOODS_PRICE_CASH )
			{
				m_TotalCashPrice += price_info._Cash;
				nDiscountCash += _GetDiscountPriceCash(price_info._Cash, nCouponDBIdx);
			}
			else
			{
				m_TotalPointPrice += price_info._Point;
			}

			usf::GetShopItemName(name, pGoods);
			usf::GetShopPriceStringComma(pGoods, price, PriceType);

			size_t nCouponEnable	= 0;
#if defined(USE_SALE_COUPON) && defined(ENABLE_UI2)
			// 버튼 활성화 조건 : 캐쉬템& (버튼이 활성화 상태 or 할인쿠폰 소유)  
			if(PriceType == GOODS_PRICE_CASH)
			{
				if(CInvenList::i()->GetDiscountCouponCount() > 0 ||
					m_pShopBasket->GetCouponIndex(i) > 0)
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

			//현재 아이템 설정
			_addLua(i);		_addLua(nCouponEnable);		_addLua(name);		_addLua(price);		
			_toLua("set_data");

			//아이템 종류 (1/3/7/9 일 등..)
			i3::wstring nameList = getPriceString( (*m_pShopBasket)[i] );
			_addLua(i);			_addLua((size_t)m_pShopBasket->get_pack_dpinx(i));			_addLua(nameList);
			_toLua("set_combo");
		}
		else
		{
			//빈 Slot
			_addLua(i);
			_toLua( "blank_data" );
		}
	}

	//최종 가격
	_updatePrice( m_TotalPointPrice, m_TotalCashPrice, 0, nDiscountCash);
	
	INT32 nCouponCnt = 0; 
#if defined(USE_SALE_COUPON) && defined(ENABLE_UI2)
	nCouponCnt =  CInvenList::i()->GetDiscountCouponCount();
#endif
	_updateCoupon( nCouponCnt );	//현재 쿠폰 수

	_addLua( m_pShopBasket->size() );
	_toLua( "ClearList" );
}

void UIPopupBasket_V11::Click_Slot_X(INT32 index)
{
	m_pShopBasket->pop( index );

#if defined(USE_SALE_COUPON) && defined(ENABLE_UI2)
	UINT64 n64CouponDBIdx = m_pShopBasket->GetCouponIndex(index);
	CInvenList::i()->SetUsedCoupon(n64CouponDBIdx, false);

	INT32 slot = 0;

	for(slot = index + 1 ; slot < USE_BASKET_COUNT ; slot++)
	{
		n64CouponDBIdx = m_pShopBasket->GetCouponIndex(slot);

		INT32 nPrevSlot = slot - 1;

		m_pShopBasket->SetCouponIndex(nPrevSlot, n64CouponDBIdx);

		if(m_pShopBasket->GetCouponIndex(nPrevSlot) > 0)
			ChangeCouponBtnTemplate(nPrevSlot , true);
		else
			ChangeCouponBtnTemplate(nPrevSlot , false);	
	}

	slot = USE_BASKET_COUNT - 1;

	m_pShopBasket->SetCouponIndex(slot, -1);
	ChangeCouponBtnTemplate(slot , false);	
#endif


	_updateList();

	m_pParent->UpdateBasketList();
}

void UIPopupBasket_V11::Change_Combo(INT32 index, INT32 tab)
{
	INT32 packInx = tab;

	//선택된 goods 가  point/cash 로 된 경우에 다른 처리를 해야 된다.
	if( m_pShopBasket->is_pack_price_dual(index) == true )
	{
		INT32 cnt = (*m_pShopBasket)[index]->GetGoodsCount();
		INT32 type = GOODS_PRICE_POINT;
		//comboBox에서 선택된 index가 goods가 가지고 있는 값보다 크면
		//cash 형식으로 변경된다.
		if( cnt <= tab )	
			type = GOODS_PRICE_CASH;

		m_pShopBasket->set_pack_type(index, type);
		
		//cash 형식이라도 packInx는 같다.  즉, 1,2,3(point) - 1,2,3(cash)
		packInx = tab % cnt;
	}

	m_pShopBasket->set_pack_inx(index, packInx);
	m_pShopBasket->set_pack_dpinx(index, tab);

	_updateList();
}


void UIPopupBasket_V11::OnClickCouponBtn(i3UIControl * pCtrl, INT32 nSlotIdx)
{
	// Blue Btn Template (Use Enable)
	i3::rc_wstring szTemplateName;
	szTemplateName.clear(); szTemplateName += L"BtC3_List_sB";

	// [쿠폰 사용 버튼] 할인쿠폰 Popup 활성화
	if( i3::generic_find_first_not_of(szTemplateName, pCtrl->getTemplate()->GetName()) == NULL)
	{
		GetMainFrame()->OpenPopup(UPW_DISCOUNT_COUPON, pCtrl);
		m_nCurrentBasketSlotIdx = nSlotIdx;
		return;
	}
	
	RevertUseCoupon(nSlotIdx);
	_updateList();
	m_nCurrentBasketSlotIdx = -1;
}

void UIPopupBasket_V11::UseDiscountCoupon(i3UIControl * pCtrl, INT32 nCouponIdx)
{
	// 여기에서 할인쿠폰 적용을 시킨 후, 버튼 템플릿 변경
	pCtrl->SetTemplateByName("BtC3_List_sR");

#if defined(USE_SALE_COUPON) && defined(ENABLE_UI2)
	INT64 nCouponDBIdx = CInvenList::i()->GetItem(INVEN::DB_COUPON, (nCouponIdx))->_i64ItemWareDBIdx;
	CInvenList::i()->SetUsedCoupon(nCouponDBIdx, true);
	m_pShopBasket->SetCouponIndex(m_nCurrentBasketSlotIdx, nCouponDBIdx);
#endif

	_updateList();
	m_nCurrentBasketSlotIdx = -1;
}

void UIPopupBasket_V11::RevertUseCoupon(INT32 nSlotIdx)
{
#if defined(USE_SALE_COUPON) && defined(ENABLE_UI2)
	// [쿠폰 취소] 버튼 -> 취소후 사용 가능한 상태로 변경
	INT64 nCouponDBIdx = m_pShopBasket->GetCouponIndex(nSlotIdx);
	if(nCouponDBIdx >= 0)
	{
		CInvenList::i()->SetUsedCoupon(nCouponDBIdx , false);
		m_pShopBasket->SetCouponIndex(nSlotIdx, -1);
		ChangeCouponBtnTemplate(nSlotIdx, false);
	}
#endif
}

SHOP_GOODS * UIPopupBasket_V11::GetTargetGoods()
{
	SHOP_GOODS * pGoods = NULL;

	if(m_nCurrentBasketSlotIdx >= 0)
		pGoods = m_pShopBasket->get(m_nCurrentBasketSlotIdx);

	return pGoods;
}

void UIPopupBasket_V11::_ClearUseCoupon()
{
#if defined(USE_SALE_COUPON) && defined(ENABLE_UI2)
	CInvenList * pInvenList = CInvenList::i();

	for(INT32 slot = 0 ; slot < USE_BASKET_COUNT ; slot++)
	{
		INT64 nCouponDBIdx = m_pShopBasket->GetCouponIndex(slot);

		if(nCouponDBIdx > 0)
		{
			pInvenList->SetUsedCoupon(nCouponDBIdx, false);
		}

		m_pShopBasket->SetCouponIndex(slot, -1);
	}
#endif
}

INT32 UIPopupBasket_V11::_GetDiscountPriceCash(INT32 nCash, INT64 nCouponDBIdx)
{
	INT32 nDiscountCash = 0;

#if defined(USE_SALE_COUPON) && defined(ENABLE_UI2)
	if(nCouponDBIdx > 0)
	{
		CInvenList * pInvenList = CInvenList::i();

		INT32 nCouponID = pInvenList->FindItemIndexByDBIdx(INVEN::DB_COUPON, nCouponDBIdx);

		if(nCouponID > 0)
		{
			SALE_COUPON * pCoupon = CShop::i()->GetCouponData(nCouponID);

			if(pCoupon->m_ui8SaleType == SALE_TYPE_PER)
			{
				//서버가 할인 된 금액을 소수점 버림으로 처리하기 때문에 클라는 할인 할 금액을 소수점 올림으로 처리한다.
				nDiscountCash = static_cast<INT32>( ceilf( nCash * (REAL32)(pCoupon->m_ui32DisCount * 0.01f) ) );
			}
			else
			{
				nDiscountCash = pCoupon->m_ui32DisCount;
			}
		}
	}
#endif

	return nDiscountCash;
}

void UIPopupBasket_V11::ChangeCouponBtnTemplate(INT32 slot, bool bUseCoupon)
{
	LuaState * L = _CallLuaFunction("GetCouponBtnCtrl");

	if(L != NULL)
	{
		i3Lua::PushInteger(L , slot);
		i3UIControl * pCtrl = reinterpret_cast<i3UIControl*>(_ReturnLuaPointer(L , 1));
		if(bUseCoupon == true)
			pCtrl->SetTemplateByName("BtC3_List_sR");
		else
			pCtrl->SetTemplateByName("BtC3_List_sB");
	}
}
