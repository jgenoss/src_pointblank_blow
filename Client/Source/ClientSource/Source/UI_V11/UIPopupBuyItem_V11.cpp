#include "pch.h"
#include "UI_V11/UIPopupBuyItem_V11.h"
#include "UI_V11/UIPhaseShop_V11.h"
#include "UI_V11/UITabShopBase.h"

#include "UI/UIMainFrame_V11.h"
#include "UI/UIUtil.h"
#include "UI/UIShopFunction.h"
#include "UI/ItemImageMgr.h"
#include "UI/PurchaseMgr.h"
#include "UI/BuyUtil.h"
#include "UI/CBFuction.h"

#include "Shop.h"
#include "ShopContext.h"
#include "InvenList.h"

#if defined( ENABLE_UI2)

I3_CLASS_INSTANCE( UIPopupBuyItem_V11);

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//

static UIPopupBuyItem_V11* GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

	I3ASSERT( pFrame);

	UIPopupBuyItem_V11* pPopup = static_cast<UIPopupBuyItem_V11*>(pFrame->GetPopup(UPW_BUYITEM));

	I3ASSERT( pPopup);

	return pPopup;
}

extern "C" {

	int UIPopup_BuyItem_Radio_Checked(LuaState* L)
	{
		INT32 radioIdx = i3Lua::GetIntegerArg( L, 1);

		GetThis()->OnRadioChecked_Date(radioIdx);

		return 0;
	}

	int UIPopup_BuyItem_CashCharge( LuaState * L)
	{
		// 캐쉬 충전
		GetThis()->OnCashCharge();
		return 0;
	}

	int	UIPopupBuyItem_OnClickCoupon( LuaState * L)
	{
		i3UIControl * pBtn = (i3UIControl * )i3Lua::GetUserDataArg(L , 1);

		// 쿠폰 사용
		GetThis()->OnClickCouponBtn(pBtn);
		return 0;
	}
	int PopupBuy_ClickTab(LuaState * L)
	{
		INT32 TabIdx = i3Lua::GetIntegerArg( L, 1);

		GetThis()->ClickPriceTab(TabIdx);	
		return 0;
	}

	int UIPopup_BuyItem_Purchase(LuaState* L)
	{
		GetThis()->SendBuyEvent();
		return 0;
	}

}

LuaFuncReg UIPopupBuyItem_Glue[] =
{
	{	"UIPopup_BuyItem_Radio_Checked"	,	UIPopup_BuyItem_Radio_Checked },
	{	"UIPopup_BuyItem_Purchase"		,	UIPopup_BuyItem_Purchase },
	{	"UIPopup_BuyItem_CashCharge"	,	UIPopup_BuyItem_CashCharge },
	{   "PopupBuy_ClickTab"				,	PopupBuy_ClickTab },
	{	"OnClickCoupon"					,	UIPopupBuyItem_OnClickCoupon},
	{	NULL,								NULL}
};


UIPopupBuyItem_V11::UIPopupBuyItem_V11() : m_pSel_GoodsPack(NULL), m_iRadioIndex(-1), 
m_pImageSet(NULL),
m_bSucess(false), m_bFromExtension(false), m_buy_mtd(GOODS_PRICE_ALL), m_numRadio(0) 
{
	memset(m_aiRadioMap, 0,  sizeof(INT32) * MAX_POPUP_BUY_RADIO_COUNT);
}

UIPopupBuyItem_V11::~UIPopupBuyItem_V11()
{
}
/*virtual*/ void UIPopupBuyItem_V11::OnLoadAllScenes()
{
	m_pImageSet = GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), "SBN_itemSet");
}

/*virtual*/ void UIPopupBuyItem_V11::OnUnloadAllScenes()
{
	I3_SAFE_RELEASE(m_pImageSet);
}


//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//

/*virtual*/ void UIPopupBuyItem_V11::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	// Load Scene
	AddScene( "UIRe1/Scene/Popup/PointBlankRe_Shop_BuyNow.i3UIs", UIPopupBuyItem_Glue);
}
bool	UIPopupBuyItem_V11::OnEntranceStart( void * pArg1/* = NULL*/, void * pArg2/* = NULL*/)
{
	if( UIPopupBase::OnEntranceStart(pArg1, pArg2) == false)		// 으음...
		return false;

	// 쿠폰 인덱스 초기화
	m_nCouponIndex = -1;

	UIPhaseShop_V11* phase = static_cast<UIPhaseShop_V11*>(GetMainFrame()->GetSubPhase(USP_SHOP));
	phase->CloseTooltip();

	i3UIScene* scn = GetScene(0);
	LuaState* L;

	L = UILua::CallLuaFunction(scn, "OnLoadScene");
	i3Lua::PushBoolean(L, g_pConfigEx->GetShop().Enable_ChargeCash );
	UILua::EndLuaFunction(L, 1);

	// 여기 
	m_bFromExtension = ((BOOL)pArg2 != 0);

	SHOP_GOODS_PACK* pGoodsPack =  m_pSel_GoodsPack = reinterpret_cast<SHOP_GOODS_PACK*>(pArg1);

	m_buy_mtd = GOODS_PRICE_ALL;

	bool	bTapButton = usf::CheckGoodsPackBothPrices(pGoodsPack);

	L = UILua::CallLuaFunction(scn, "EnablePriceTab");
	i3Lua::PushBoolean(L, bTapButton);
	UILua::EndLuaFunction(L, 1);

	if (bTapButton)
	{
		m_buy_mtd = GOODS_PRICE_POINT;			// 탭존재시 포인트 우선..
	}

	i3::rc_wstring item_name;
	SHOP_GOODS* pGoods = pGoodsPack->GetGoods(0);
	usf::GetShopItemName(item_name, pGoods);

	L = UILua::CallLuaFunction( scn, "SetItemName");
	i3Lua::PushStringUTF16To8(L, item_name);
	UILua::EndLuaFunction(L, 1);

	GetItemImageMgr()->SelectImage(m_pImageSet, pGoods);

	UIPopupBuyItem_V11::UpdateInfoByTabChange(0);

	_SetCouponEnable();

	return true;
}
void	UIPopupBuyItem_V11::UpdateInfoByTabChange(INT32 radioIdx)
{
	m_iRadioIndex = radioIdx; 

	SHOP_GOODS_PACK* pGoodsPack =  m_pSel_GoodsPack;
	usf::CalcGoodsPackMapByPriceType(pGoodsPack, m_numRadio, m_aiRadioMap, m_buy_mtd);

	if (m_iRadioIndex >= m_numRadio)
		m_iRadioIndex = m_numRadio - 1;

	i3UIScene* scn = GetScene(0);
	if (scn)
	{
		i3::wstring astrRadioText[MAX_POPUP_BUY_RADIO_COUNT];
		i3::wstring astrStaticText[MAX_POPUP_BUY_RADIO_COUNT];
		//	CShop* shop = g_pGameContext->GetShop();

		I3ASSERT(m_numRadio <= MAX_POPUP_BUY_RADIO_COUNT);

		INT32 nIndex;
		for( nIndex = 0 ; nIndex < m_numRadio ; nIndex++)
		{
			SHOP_GOODS * pGoods2 = pGoodsPack->GetGoods(m_aiRadioMap[nIndex]);

			usf::GetShopPeriodCountString(pGoods2, astrRadioText[nIndex]);
			usf::GetShopPriceString(pGoods2, astrStaticText[nIndex], m_buy_mtd);
		}

		for ( ; nIndex < MAX_POPUP_BUY_RADIO_COUNT ; ++nIndex)
		{
			astrRadioText[nIndex].clear();
			astrStaticText[nIndex].clear();
		}


		LuaState* L = UILua::CallLuaFunction(scn, "SetInfo");

		i3Lua::PushStringUTF16To8(L, astrRadioText[0]);
		i3Lua::PushStringUTF16To8(L, astrRadioText[1]);
		i3Lua::PushStringUTF16To8(L, astrRadioText[2]);
		i3Lua::PushStringUTF16To8(L, astrRadioText[3]);

		i3Lua::PushStringUTF16To8(L, astrStaticText[0]);
		i3Lua::PushStringUTF16To8(L, astrStaticText[1]);
		i3Lua::PushStringUTF16To8(L, astrStaticText[2]);
		i3Lua::PushStringUTF16To8(L, astrStaticText[3]);

		i3Lua::PushInteger(L, m_numRadio);
		i3Lua::PushInteger(L, m_iRadioIndex);
		UILua::EndLuaFunction(L, 10);
	}
}

void	UIPopupBuyItem_V11::ClickPriceTab(INT32 tab_idx)
{
	if (tab_idx == 0)
		m_buy_mtd = GOODS_PRICE_POINT;
	else
		m_buy_mtd = GOODS_PRICE_CASH;

	UIPopupBuyItem_V11::UpdateInfoByTabChange(0);
	UIPopupBuyItem_V11::_Update_Shop_Purchase_Info();
	UIPopupBuyItem_V11::_SetCouponEnable();
}
void	UIPopupBuyItem_V11::OnExitEnd(void)
{
	if (m_bSucess == true)
	{
		// 해당 목록을 갱신해주는게 좋겠다..
		UIPhaseShop_V11* phase = static_cast<UIPhaseShop_V11*>(GetMainFrame()->GetSubPhase(USP_SHOP));
		phase->UpdateGoods_IfPurchased();
	}
#if defined(USE_SALE_COUPON) && defined(ENABLE_UI2)
	else
	{
		RevertUseCoupon();
	}
#endif

	if (m_bFromExtension == true)
	{
		// 익스텐션창으로부터 구입창이라면 그 창으로 복귀시켜줌...
		GetMainFrame()->OpenPopup( UPW_SHOPEXTENSION, m_pSel_GoodsPack);
	}

	m_bSucess = false;

	m_bFromExtension = false;

	UIPopupBase::OnExitEnd();
}
void	UIPopupBuyItem_V11::OnRadioChecked_Date(INT32 radioIdx)
{
	m_iRadioIndex = radioIdx;			// 바로 변경..

	UIPopupBuyItem_V11::_Update_Shop_Purchase_Info();
}

void	UIPopupBuyItem_V11::_Update_Shop_Purchase_Info()
{
	SHOP_GOODS * pGoods = m_pSel_GoodsPack->GetGoods(m_aiRadioMap[m_iRadioIndex]);		// 변경되었을 것이며, 이 정보를 표시해두면 된다..

	i3::rc_wstring szSubjectPointCash;
	char szBuyPointCash[MAX_STRING_COUNT], szHavePointCash[MAX_STRING_COUNT], szRemainPointCash[MAX_STRING_COUNT];

	usf::SetShopPointInfo(pGoods, m_buy_mtd, szSubjectPointCash, szHavePointCash, MAX_STRING_COUNT,
		szBuyPointCash, MAX_STRING_COUNT, szRemainPointCash, MAX_STRING_COUNT, "  ", m_nCouponIndex);		

	LuaState* L = _CallLuaFunction("SetShopPointInfo");

	i3Lua::PushStringUTF16To8(L, szSubjectPointCash);
	i3Lua::PushA2UTF8String(L, szHavePointCash);
	i3Lua::PushA2UTF8String(L, szBuyPointCash);
	i3Lua::PushA2UTF8String(L, szRemainPointCash);

	_EndLuaFunction(L, 4);

	bool bDiscountEnable = false;
	if(pGoods->_SaleType == GOODS_PRICE_CASH && m_nCouponIndex >= 0 )
		bDiscountEnable = true;

	L = _CallLuaFunction("SetDiscountRate");
	i3Lua::PushBoolean(L, bDiscountEnable == true? TRUE : FALSE);
	_EndLuaFunction(L , 1);
}

void	UIPopupBuyItem_V11::SendBuyEvent()
{
#if defined(USE_SALE_COUPON) && defined(ENABLE_UI2)
	//const INVEN_BUFFER * pBuffer = CInvenList::i()->GetItem(INVEN::DB_COUPON, (GetCouponIdx()), true);
#else
	const INVEN_BUFFER * pBuffer = NULL;
#endif

	buy::op::push_immediate(m_pSel_GoodsPack, usf::g_price_idx, m_iRadioIndex); 
	buy::purchase purchase;
	purchase.type = buy::Immediate;
	buy::op::copy_buy_item_in_purchase(purchase.type, purchase.list);
	//((UIPhaseShop_V11*)GetMainFrame()->GetSubPhase(USP_SHOP))->Do_Process_Buy_V2(buy::Immediate, purchase);

	SetInputDisable(TRUE);
	m_bFromExtension = false;
}

void	UIPopupBuyItem_V11::OnCashCharge()
{
	//	캐시 충전 웹페이지 호출
#if defined(LOCALE_CHINA) 
	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_SHOP_PREPARE_PLAN"));/*차후 지원될 예정입니다.*/
	//g_pSDOA_Link->OpenWidget();
#elif defined(LOCALE_TAIWAN)
	g_pFramework->OpenWebBrowser(g_pConfigEx->GetShop().ChargeURL,NULL,false);
#else
	::ExcuteCashWeb();
#endif		
}
void	UIPopupBuyItem_V11::ChangeShopGoodsPack(SHOP_GOODS_PACK* goods_pack)
{
	m_pSel_GoodsPack = goods_pack;

	bool	bTapButton = usf::CheckGoodsPackBothPrices(goods_pack);

	i3UIScene* scn = GetScene(0);
	if (scn)
	{
		LuaState* L = UILua::CallLuaFunction(scn, "EnablePriceTab");
		i3Lua::PushBoolean(L, bTapButton);
		UILua::EndLuaFunction(L, 1);
	}

	if (bTapButton)
	{
		m_buy_mtd = GOODS_PRICE_POINT;			// 탭존재시 포인트 우선..
	}

	UIPopupBuyItem_V11::UpdateInfoByTabChange(0);

}
void UIPopupBuyItem_V11::_SetCouponEnable()
{
	// 보유중인 쿠폰 갯수를 받아옴
#if defined(USE_SALE_COUPON) && defined(ENABLE_UI2)
	INT32 CouponCnt = CInvenList::i()->GetDiscountCouponCount();
#else
	INT32 CouponCnt = 0;
#endif

	i3::rc_wstring szText;

	LuaState *  L = _CallLuaFunction("EnableUseCoupon");

	bool bEnable = false;
	bool bBtnEnable = false;

	// 쿠폰기능 사용 및 보유 쿠폰이 존재하며, 구매할 상품이 캐시아이템일때만 활성화시킨다
	if(IsCashItem())
	{
		if(LocaleValue::GetBOOL("EnableDiscountCoupon") == TRUE && CouponCnt > 0)
		{
			/*사용 가능 쿠폰 : {col:255,127,39,255}%d개{/col}*/
			i3::sprintf(szText, GAME_RCSTRING("STR_TBL_BUY_WINDOW_COUPON_NOTICE"), CouponCnt); 
			bEnable = true;
		}
	}
	else
	{
		RevertUseCoupon();
	}

	// 버튼이 사용중이라면 취소할수 있도록 버튼 활성화를 한다.
	if(m_nCouponIndex >= 0 || bEnable == true)
		bBtnEnable = true;

	if(L != NULL)
	{
		i3Lua::PushBoolean(L , bBtnEnable);
		i3Lua::PushBoolean(L , bEnable);
		i3Lua::PushStringUTF16To8(L , szText);
		_EndLuaFunction(L , 3);
	}
}

void UIPopupBuyItem_V11::OnClickCouponBtn(i3UIControl * pCtrl)
{
	// Blue Btn Template (Use Enable)
	i3::rc_wstring szTemplateName;
	i3::sprintf(szTemplateName , L"BtC3_List_sB");

	// [쿠폰 사용 버튼] 할인쿠폰 Popup 활성화
	if( i3::generic_find_first_not_of(szTemplateName,pCtrl->getTemplate()->GetName()) != NULL )
		GetMainFrame()->OpenPopup(UPW_DISCOUNT_COUPON, pCtrl);
	else
		RevertUseCoupon();
}

void UIPopupBuyItem_V11::UseDiscountCoupon(i3UIControl * pCtrl, INT32 nCouponIdx)
{
	// 여기에서 할인쿠폰 적용을 시킨 후, 버튼 템플릿 변경
	pCtrl->SetTemplateByName("BtC3_List_sR");
	m_nCouponIndex = nCouponIdx;
	UIPopupBuyItem_V11::_Update_Shop_Purchase_Info();
#if defined(USE_SALE_COUPON) && defined(ENABLE_UI2)
	INT64 nCouponDBIdx = CInvenList::i()->GetItem(INVEN::DB_COUPON, (GetCouponIdx()))->_i64ItemWareDBIdx;
	CInvenList::i()->SetUsedCoupon(nCouponDBIdx, true);
#endif
	_SetCouponEnable();
}

void UIPopupBuyItem_V11::RevertUseCoupon()
{
	if(m_nCouponIndex < 0)
		return;

	// [쿠폰 취소] 버튼 -> 취소후 사용 가능한 상태로 변경
	LuaState * L = _CallLuaFunction("GetUseCouponBtn");
	i3UIControl * pCtrl = reinterpret_cast<i3UIControl*>(_ReturnLuaPointer(L , 0));

	I3ASSERT(pCtrl != NULL);	
	pCtrl->SetTemplateByName("BtC3_List_sB");

#if defined(USE_SALE_COUPON) && defined(ENABLE_UI2)
	INT64 nCouponDBIdx = CInvenList::i()->GetItem(INVEN::DB_COUPON, (GetCouponIdx()), true)->_i64ItemWareDBIdx;
	CInvenList::i()->SetUsedCoupon(nCouponDBIdx, false);
#endif

	m_nCouponIndex = -1;

	UIPopupBuyItem_V11::_Update_Shop_Purchase_Info();
	_SetCouponEnable();
}

const bool UIPopupBuyItem_V11::IsCashItem()
{
	if((GetTargetGoods()->_SaleType & GOODS_PRICE_CASH) && (m_buy_mtd & GOODS_PRICE_CASH))
		return true;

	return false;
}
#endif
