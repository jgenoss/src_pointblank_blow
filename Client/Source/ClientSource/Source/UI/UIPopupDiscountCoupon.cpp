#include "pch.h"
#include "UIPopupDiscountCoupon.h"
#include "UIPopupBuyItem.h"
#include "UIMainFrame.h"
#include "UIPopupBuyItem.h"
#include "Shop.h"

#include "UIUtil.h"
#include "../TextSet.h"
#include "i3Base/string/ext/generic_string_cat.h"

#include "UserInfoContext.h"


I3_CLASS_INSTANCE( UIPopupDiscountCoupon);//, UIPopupBase);

#if defined(USE_SALE_COUPON)

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//

extern "C" {

	int UIPopupDiscountCoupon_OnClickButton( LuaState * L)
	{
		UIPopupDiscountCoupon * pThis = static_cast<UIPopupDiscountCoupon*>(GameUI::GetPopup(UPW_DISCOUNT_COUPON));
		if( pThis != nullptr)
			pThis->OnClickButton();
		return 0;
	}

	int UIPopupDiscountCoupon_UseCoupon( LuaState * L)
	{
		//UIPopupDiscountCoupon * pThis = static_cast<UIPopupDiscountCoupon*>(GameUI::GetPopup(UPW_DISCOUNT_COUPON));
		//if( pThis != nullptr)
		//pThis->OnClickButton();
		return 0;
	}

	int UIPopupDiscountCoupon_SelectCoupon( LuaState * L)
	{
		INT32 slot = i3Lua::GetIntegerArg(L , 1);
		UIPopupDiscountCoupon * pThis = static_cast<UIPopupDiscountCoupon*>(GameUI::GetPopup(UPW_DISCOUNT_COUPON));
		if( pThis != nullptr)
			pThis->ChangeCurrnetSlot(slot);
		return 0;
	}
}

LuaFuncReg UIPopupDiscountCoupon_Glue[] = {
	{"OnClickButton",		UIPopupDiscountCoupon_OnClickButton},
	{"UseCoupon",			UIPopupDiscountCoupon_UseCoupon}	,
	{"SelectCoupon",		UIPopupDiscountCoupon_SelectCoupon}	,
	{nullptr,					nullptr}
};

//------------------------------------------------------------------------------//

#endif

UIPopupDiscountCoupon::UIPopupDiscountCoupon()
{
#if defined(USE_SALE_COUPON)
	m_bCouponUseWnd = false;
	m_nCurrentSlot = -1;

	m_pBtnCtrl	= nullptr;
#endif
}

UIPopupDiscountCoupon::~UIPopupDiscountCoupon()
{
}

#if defined(USE_SALE_COUPON)
/*virtual*/ void UIPopupDiscountCoupon::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);
	//Load Scene
	AddScene("Scene/Popup/PBRe_PopUp_CouponList.i3UIs", UIPopupDiscountCoupon_Glue);
}

/*virtual*/ bool UIPopupDiscountCoupon::OnEntranceStart(void * pArg1 , void * pArg2 )
{
	
	I3TRACE("UIPopupDiscountCoupon::OnEntranceStart\n");
	
	bool resPopup = UIPopupBase::OnEntranceStart(pArg1,pArg2);
	I3ASSERT(resPopup);

	m_pBtnCtrl = static_cast<i3UIControl*>(pArg1);

	return true;
}

/*virtual*/ void UIPopupDiscountCoupon::OnEntranceEnd()
{
	UIPopupBase::OnEntranceEnd();

	_InitCouponList();
	I3TRACE("UIPopupDiscountCoupon::OnEntranceEnd\n");
}

/*virtual*/ void UIPopupDiscountCoupon::OnExitEnd()
{
	m_pBtnCtrl = nullptr;

	UIPopupBase::OnExitEnd();
}


/*virtual*/ void UIPopupDiscountCoupon::_InitializeAtLoad(i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad(pScene);

}


void UIPopupDiscountCoupon::OnClickButton()
{
	if(m_bCouponUseWnd)
		Confirm();
	else
		Cancel();

}

void UIPopupDiscountCoupon::Confirm()
{
//	SHOP_GOODS* pGoods = nullptr;

//	if(GetMainFrame()->IsOpenedPopup(UPW_BUYITEM))
//	{
//		UIPopupBuyItem * pFrame = (UIPopupBuyItem *) GetMainFrame()->GetPopup(UPW_BUYITEM);
//		pGoods = pFrame->GetTargetGoods();
//	}
//	else
//	{
//		UIPopupBasket * pFrame = (UIPopupBasket *) GetMainFrame()->GetPopup(UPW_BASKET);
//		pGoods = pFrame->GetTargetGoods();
//	}

//	if(CheckDiscountCoupon(pGoods))
//	{
//		if(GetMainFrame()->IsOpenedPopup(UPW_BUYITEM))
//		{
//			UIPopupBuyItem * pFrame = (UIPopupBuyItem *) GetMainFrame()->GetPopup(UPW_BUYITEM);
//			pFrame->UseDiscountCoupon(m_pBtnCtrl, m_nCurrentSlot);
//		}
//		else
//		{
//			UIPopupBasket * pFrame = (UIPopupBasket *) GetMainFrame()->GetPopup(UPW_BASKET);
//			pFrame->UseDiscountCoupon(m_pBtnCtrl, m_nCurrentSlot);
//		}
//	}

	GetMainFrame()->ClosePopup(UPW_DISCOUNT_COUPON);
}

void UIPopupDiscountCoupon::Cancel()
{
	GetMainFrame()->ClosePopup(UPW_DISCOUNT_COUPON);
}

bool UIPopupDiscountCoupon::CheckDiscountCoupon(SHOP_GOODS * pGoods)
{
	if(m_nCurrentSlot < 0)
	{
		/*쿠폰을 선택하여야 합니다.*/
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_COUPON_NONE_SELECT"));
		return false;
	}

	INT32	gameItemId	= pGoods->_ItemID;
	ITEM_TYPE goodsClass = (ITEM_TYPE)GET_ITEM_TYPE(gameItemId);

	CInvenList * pInvenList = CInvenList::i();
	CShop * pShop = CShop::i();

	T_ItemDBIdx n64CouponDBIdx = pInvenList->GetDBIdxByIndex( INVEN::DB_COUPON, m_nCurrentSlot);
	
	SALE_COUPON * pCoupon = pShop->GetCouponData(pInvenList->FindItemIDByDBIdx( INVEN::DB_COUPON, n64CouponDBIdx));

//	char szErrorMsg[MAX_STRING_COUNT] = {0};
	i3::stack_wstring wstrErrorMsg;

	// 구매 타입이 캐시/포인트 선택일 경우를 위해 And연산으로 처리
	if((pGoods->GetPriceType(PRICE_TYPE_SHOP_REAL) & GOODS_PRICE_CASH) == false)
	{
		/* 캐시 아이템에만 사용할 수 있습니다.*/
		i3::sprintf(wstrErrorMsg, L"%s\n(%s)", GAME_RCSTRING("STBL_IDX_COUPON_USE_FAIL"), GAME_RCSTRING("STBL_IDX_COUPON_USE_FAIL_REASON_MISMATH_CASH"));
		GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrErrorMsg, &GAME_RCSTRING("STR_HEADER_COUPON_USE_FAIL"));
		return false;
	}

	// 카테고리 체크
	{
		if(pCoupon->m_ui8ItemClass != ITEM_TYPE_UNKNOWN && 
			goodsClass != pCoupon->m_ui8ItemClass)
		{
			/*사용 가능한 카테고리가 맞지 않습니다.*/
			i3::sprintf(wstrErrorMsg, L"%s\n(%s)",
				GAME_RCSTRING("STBL_IDX_COUPON_USE_FAIL"), GAME_RCSTRING("STBL_IDX_COUPON_USE_FAIL_REASON_MISMATH_TYPE"));
			GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrErrorMsg, &GAME_RCSTRING("STR_HEADER_COUPON_USE_FAIL"));
			return false;
		}
	}

	// 사용 기간 체크
	{
		/*사용 기간이 올바르지 않습니다.*/

		UINT32 nServerTime = UserInfoContext::i()->GetInvenServerTime();

		UINT32 nStartDate = 0;
		UINT32 nEndDate = 0;
		if(pCoupon->m_ui8CouponType == DAY_TYPE_DATE)
		{
			// 절대기간
			nStartDate = pCoupon->m_ui32StartDate;
			nEndDate = pCoupon->m_ui32EndDate;
		}
		else
		{
			// 상대기간
			nEndDate = pInvenList->GetItemArg( INVEN::DB_COUPON, m_nCurrentSlot);
		}

		if(nServerTime < nStartDate || nServerTime > nEndDate)
		{
			i3::sprintf(wstrErrorMsg, L"%s\n(%s)",
				GAME_RCSTRING("STBL_IDX_COUPON_USE_FAIL"), GAME_RCSTRING("STBL_IDX_COUPON_USE_FAIL_REASON_LIMIT_TIME"));
			GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrErrorMsg, &GAME_RCSTRING("STR_HEADER_COUPON_USE_FAIL"));
			return false;
		}
	}

	{
		// 쿠폰 금액 제한 체크	
		bool bLimitError = false;

		// LimitType = LIMIT_TYPE_NONE (제한 없음), LimitType = LIMIT_TYPE_UP (이상), LimitType = LIMIT_TYPE_DOWN (이하)

		switch(pCoupon->m_ui8LimitType)
		{
		case LIMIT_TYPE_UP:		
			if(pCoupon->m_ui32Limit > pGoods->GetPriceCash(PRICE_TYPE_SHOP_REAL))
				bLimitError = true;
			break;
		case LIMIT_TYPE_DOWN:
			if(pCoupon->m_ui32Limit < pGoods->GetPriceCash(PRICE_TYPE_SHOP_REAL))
				bLimitError = true;
			break;
		default: break;
		}

		if(bLimitError)
		{
			/*금액 제한 조건이 맞질 않습니다.*/
			i3::sprintf(wstrErrorMsg, L"%s\n(%s)",
				GAME_RCSTRING("STBL_IDX_COUPON_USE_FAIL"), GAME_RCSTRING("STBL_IDX_COUPON_USE_FAIL_REASON_LIMIT_MONEY"));
			GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrErrorMsg, &GAME_RCSTRING("STR_HEADER_COUPON_USE_FAIL"));
			return false;
		}
	}

	// 사용 여부에 관한 체크
	{
		if(pInvenList->IsUsedCoupon(n64CouponDBIdx) == true)
		{	
			/*이미 사용된 쿠폰입니다*/
			i3::sprintf(wstrErrorMsg, L"%s\n(%s)",
				GAME_RCSTRING("STBL_IDX_COUPON_USE_FAIL"), GAME_RCSTRING("STBL_IDX_COUPON_USE_FAIL_REASON_USED_COUPON"));
			GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrErrorMsg, &GAME_RCSTRING("STR_HEADER_COUPON_USE_FAIL"));
			return false;
		}
	}

	return true;

}

void UIPopupDiscountCoupon::ChangeCurrnetSlot(INT32 slot)
{
	m_nCurrentSlot = slot;	
}


void UIPopupDiscountCoupon::_InitCouponList()
{
	LuaState * L = _CallLuaFunction("EnableControls");
	i3Lua::PushBoolean(L , true);
	_EndLuaFunction(L, 1);

	INT32 nCouponCount = CInvenList::i()->GetDiscountCouponCount();
	
	INT32 nCouponIdx = 0;

	CInvenList * pInvenList = CInvenList::i();

	for(INT32 slot = 0 ; slot < nCouponCount ; slot ++)
	{
		// 사용된 쿠폰은 리스트에 추가하지 않음.
		if(pInvenList->IsUsedCoupon( pInvenList->GetDBIdxByIndex( INVEN::DB_COUPON, nCouponIdx, 1)) == false)
			_AddCouponItem(slot, nCouponIdx);
		else
			slot--;

		nCouponIdx++;
	}

	_SetNoticeMsg();
	_SetButtonText();

	m_nCurrentSlot = -1;
}

void UIPopupDiscountCoupon::_SetNoticeMsg()
{
	i3::stack_wstring wstrText;

	INT32 CouponNum = CInvenList::i()->GetDiscountCouponCount();

	i3::sprintf(wstrText, GAME_RCSTRING("STR_TBL_DISCOUNT_COUPON_NOTICE"), CouponNum); 

	LuaState * L = _CallLuaFunction("SetNoticeMessage");

	i3Lua::PushStringUTF16To8(L , wstrText);
	_EndLuaFunction(L ,1);
}

void UIPopupDiscountCoupon::_SetButtonText()
{

//	if(GetMainFrame()->IsOpenedPopup(UPW_BUYITEM) )
//		m_bCouponUseWnd = true;
//	else 
		m_bCouponUseWnd = false;

	LuaState * L = _CallLuaFunction("SetButtonText");

	i3Lua::PushBoolean(L , m_bCouponUseWnd);
	_EndLuaFunction(L, 1);
}


void UIPopupDiscountCoupon::_AddCouponItem(INT32 slot, INT32 nCouponIdx)
{
	CShop * pShop = CShop::i();
	CInvenList * pInvenList = CInvenList::i();

	SALE_COUPON * pCoupon = pShop->GetCouponData(pInvenList->GetItemID( INVEN::DB_COUPON, nCouponIdx));

	

	UINT32 nItemTerm = pInvenList->GetItemArg( INVEN::DB_COUPON, nCouponIdx, 1);	// 1 모든 쿠폰리스트에서 찾는다.
	
	// 할인율
	i3::stack_wstring wstrRate;
	_SetDiscountRate(wstrRate, pCoupon);
	// 할인 아이템 타입
	i3::stack_wstring wstrType;
	_SetItemTypeLimit(wstrType, pCoupon);
	// 가격 제한
	i3::stack_wstring wstrLimit;
	_SetCashLimit(wstrLimit, pCoupon);
	// 쿠폰 남은 시간
	i3::stack_wstring wstrTerm;
	_SetTerm(wstrTerm, pCoupon, nItemTerm);

	i3::stack_wstring wstrTemp;
	i3::sprintf(wstrTemp, GAME_RCSTRING("STR_TBL_DISCOUNT_COUPON_MESSAGE"), wstrRate, wstrType, wstrLimit);

	LuaState * L = _CallLuaFunction("SetSlotInfo");
	i3Lua::PushInteger(L, slot);
	i3Lua::PushA2UTF8String(L, pCoupon->m_strCouponName);		// TODO : 일단 방치..서버 협업후 변경.. (2014.07.14.수빈)
	i3Lua::PushStringUTF16To8(L, wstrTemp);
	i3Lua::PushStringUTF16To8(L, wstrTerm);
	_EndLuaFunction(L, 4);
}

void UIPopupDiscountCoupon::_SetDiscountRate(i3::stack_wstring& wstrDest, SALE_COUPON* pCoupon)
{
	switch(pCoupon->m_ui8SaleType)
	{
		case SALE_TYPE_PER: 
			i3::sprintf(wstrDest, GAME_RCSTRING("STR_TBL_DISCOUNT_COUPON_MSG_RATE_RELATIVE"), pCoupon->m_ui32DisCount);
			break;
		case SALE_TYPE_CASH :
			i3::sprintf(wstrDest, GAME_RCSTRING("STR_TBL_DISCOUNT_COUPON_MSG_RATE_ABSOULTE"),  pCoupon->m_ui32DisCount);
			break;
		default:	break;
	}
}

void UIPopupDiscountCoupon::_SetItemTypeLimit(i3::stack_wstring& wstrDest, SALE_COUPON * pCoupon)
{
	if(pCoupon->m_ui8ItemClass == ITEM_TYPE_UNKNOWN)
		wstrDest = GAME_RCSTRING("STR_TBL_DISCOUNT_COUPON_MSG_USEABLE_ALL");
	else
		i3::sprintf(wstrDest, GAME_RCSTRING("STR_TBL_DISCOUNT_COUPON_MSG_USEABLE_LIMIT"), 
			TextSet::ItemType(pCoupon->m_ui8ItemClass));
}

void UIPopupDiscountCoupon::_SetCashLimit(i3::stack_wstring& wstrDest, SALE_COUPON * pCoupon)
{
	switch(pCoupon->m_ui8LimitType)
	{
		case LIMIT_TYPE_NONE:
			wstrDest = GAME_RCSTRING("STR_TBL_DISCOUNT_COUPON_MSG_CASH_LIMIT_NONE");
			break;
		case LIMIT_TYPE_UP:
			i3::sprintf(wstrDest, GAME_RCSTRING("STR_TBL_DISCOUNT_COUPON_MSG_CASH_LIMIT_ABOVE"), pCoupon->m_ui32Limit);
			break;
		case LIMIT_TYPE_DOWN:
			i3::sprintf(wstrDest, GAME_RCSTRING("STR_TBL_DISCOUNT_COUPON_MSG_CASH_LIMIT_BELOW"), pCoupon->m_ui32Limit);
			break;
		default:
			break;
	}
}

void UIPopupDiscountCoupon::_SetTerm(i3::stack_wstring& wstrDest, SALE_COUPON* pCoupon, UINT32 ItemTerm)
{
	//STR_POPUP_WHAT_MONTH				// ~ 월
	//STR_POPUP_WHAT_DAY				// ~ 일
	//STR_TBL_DISCOUNT_COUPON_MSG_TERM2 // ~ 까지.
	//STR_UI_HOUR						// ~ 시간
	//STR_POPUP_WHAT_MINUTE				// ~ 분
	//STR_TBL_DISCOUNT_COUPON_MSG_TERM1 // ~ 남음.

	UINT32 sec = UserInfoContext::i()->GetInventorySecTime(ItemTerm);
	UINT32 term = ConvertSecToUseDay(sec);

	if( term > 1 )
	{
		// X월 X일 남음.
		INT32 nMonth, nDay;
		
		// 절대 기간은 쿠폰의 기간으로 계산, 
		// 상대 기간은 인벤버퍼에 있는 쿠폰의 만료날로 계산
		if(pCoupon->m_ui8SaleType == DAY_TYPE_DATE)
			nMonth = pCoupon->m_ui32EndDate / 10000;
		else
			nMonth = ItemTerm / 10000;
		
		nMonth = nMonth % 10000;
		nDay = nMonth % 100;
		nMonth = nMonth / 100;

		i3::stack_wstring wstrTemp;
		i3::sprintf(wstrTemp, GAME_RCSTRING("STR_POPUP_WHAT_MONTH"), nMonth);
		i3::generic_string_cat(wstrTemp, L" ");

		i3::stack_wstring wstrDay;
		i3::sprintf(wstrDay, GAME_RCSTRING("STR_POPUP_WHAT_DAY"), nDay);
		i3::generic_string_cat(wstrTemp, wstrDay);

		i3::sprintf(wstrDest, GAME_RCSTRING("STR_TBL_DISCOUNT_COUPON_MSG_TERM2"), wstrTemp);
	}
	else
	{	
		// X시간 X분 남음.
		if(sec > 0)
		{
			// ConvertSecToUseHour() 메서드는 반올림이 되는 문제로 직접 코드로 작성
			UINT32 hour;			
			{
				UINT32 time = sec;

				UINT32 accum = time / 3600;
				hour = accum;
			}


			UINT32 minute = ConvertSecToUseMinute(sec);
			minute -= hour * 60;

			i3::stack_wstring wstrTemp;

			if(hour > 0)
			{
				// X시간
				i3::sprintf(wstrTemp, L"%d%s", hour, GAME_RCSTRING("STR_UI_HOUR"));

				if(minute > 0)
				{
					i3::stack_wstring wstrMinute;
					i3::sprintf(wstrMinute, GAME_RCSTRING("STR_POPUP_WHAT_MINUTE"), minute);

					i3::generic_string_cat(wstrTemp, L" ");	// 공백문자 추가
					i3::generic_string_cat(wstrTemp, wstrMinute);
				}
			}
			else
			{
				if(minute > 0)
				{
					i3::sprintf(wstrTemp, GAME_RCSTRING("STR_POPUP_WHAT_MINUTE"), minute);
				}
			}
			
			i3::sprintf(wstrDest, GAME_RCSTRING("STR_TBL_DISCOUNT_COUPON_MSG_TERM1"), wstrTemp);
		}
	}
}

#endif