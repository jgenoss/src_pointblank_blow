#include "pch.h"

#include "UI_V11/UITabShopBase.h"
#include "UI_V11/UIPhaseShop_V11.h"

#include "UI/UIShopFunction.h"
#include "Shop.h"

#include "i3Base/string/compare/generic_iless.h"




UITabShopBase::UITabShopBase(UIPhaseSubBase* phaseShop, UIToolTipShop* pTT) : m_phaseShop(phaseShop), m_toolTip(pTT) //, m_currFilterType(0)
{
	
}

UITabShopBase::~UITabShopBase()
{
}

void	UITabShopBase::MakeGroupList(i3::vector<SHOP_GOODS_PACK*>& refGoodsList)
{
	this->_MakeGroupList_ByShopCategory(refGoodsList);
}

void	UITabShopBase::MakeFilterList()
{
	this->_MakeFilterList_ByShopCategory();
}

void	UITabShopBase::_MakeGroupList_ByShopCategory(i3::vector<SHOP_GOODS_PACK*>& refGoodsList)
{
	refGoodsList.clear();
}

void	UITabShopBase::_MakeFilterList_ByShopCategory()
{
	i3UIScene* scn = m_phaseShop->GetMenuScene();
	
	LuaState* L = UILua::CallLuaFunction(scn, "ClearFilterList");
	UILua::EndLuaFunction(L, 0);
}

bool	UITabShopBase::cbSortByName(SHOP_GOODS_PACK* pGoodsPack1, SHOP_GOODS_PACK* pGoodsPack2)
{

	SHOP_GOODS* pGoods1 = pGoodsPack1->GetGoods(0);	SHOP_GOODS* pGoods2 = pGoodsPack2->GetGoods(0);
	
	i3::rc_wstring str1, str2;
	usf::GetShopItemName(str1, pGoods1);	usf::GetShopItemName(str2, pGoods2);
	
	if (str1.size() == 0 || str2.size() == 0)
	{
		if ( str1.size() == str2.size() ) return false;
		return (str1.size() != 0);
	}

	return i3::generic_iless()(str1, str2);
}


bool	UITabShopBase::cbSortByPoint(SHOP_GOODS_PACK* pGoodsPack1, SHOP_GOODS_PACK* pGoodsPack2)
{
	SHOP_GOODS* pGoods1 = pGoodsPack1->GetGoods(0);	SHOP_GOODS* pGoods2 = pGoodsPack2->GetGoods(0);

	if( pGoods1 == NULL || pGoods2 == NULL )
		return 0;

	GOODS_PRICE_INFO info1 , info2 ;
	CShop::i()->GetGoodsPrice(pGoods1->_GoodsID, info1, PRICE_TYPE_SHOP_REAL );
	CShop::i()->GetGoodsPrice(pGoods2->_GoodsID, info2, PRICE_TYPE_SHOP_REAL );

	if (info1._Type == GOODS_PRICE_CASH && info2._Type == GOODS_PRICE_CASH)
	{
		return COMPARE_DESC(info1._Cash, info2._Cash) < 0;
	}

	if (info1._Type != GOODS_PRICE_CASH && info2._Type != GOODS_PRICE_CASH)		
	{
		return COMPARE_DESC(info1._Point, info2._Point) < 0;
	}

	// 섞여 있는 경우...포인트가 앞으로 가도록..
	return (info1._Type != GOODS_PRICE_CASH);
}

bool	UITabShopBase::cbSortByCash(SHOP_GOODS_PACK* pGoodsPack1, SHOP_GOODS_PACK* pGoodsPack2)
{
	SHOP_GOODS* pGoods1 = pGoodsPack1->GetGoods(0);	SHOP_GOODS* pGoods2 = pGoodsPack2->GetGoods(0);

	if( pGoods1 == NULL || pGoods2 == NULL )
		return 0;

	GOODS_PRICE_INFO info1 , info2 ;
	CShop::i()->GetGoodsPrice(pGoods1->_GoodsID, info1, PRICE_TYPE_SHOP_REAL );
	CShop::i()->GetGoodsPrice(pGoods2->_GoodsID, info2, PRICE_TYPE_SHOP_REAL );

	if (info1._Type == GOODS_PRICE_CASH && info2._Type == GOODS_PRICE_CASH)
	{
		return COMPARE_DESC(info1._Cash, info2._Cash) < 0;
	}

	if (info1._Type != GOODS_PRICE_CASH && info2._Type != GOODS_PRICE_CASH)		// 양쪽 다 포인트
	{
		return COMPARE_DESC(info1._Point, info2._Point) < 0;
	}

	return (info1._Type == GOODS_PRICE_CASH); 
}


















//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
/*
extern "C" {

	
	int UITabShopWeapon_ClickTab( LuaState * L)
	{
		INT32 tabIdx = i3Lua::GetIntegerArg( L, 0);

		// Tab에 따라 정보를 보여주도록 한다.

		return 0;
	}
	
	int UITabShopBase_ClickSort( LuaState * L)
	{
		return 0;
	}

	int UITabShopBase_ClickBuyButton( LuaState * L)
	{

		return 0;
	}

	int UITabShopBase_ClickPresentButton( LuaState * L)
	{
		return 0;
	}

	int UITabShopBase_ClickPreviousList( LuaState * L)
	{
		return 0;
	}

	int UITabShopBase_ClickNextList( LuaState * L)
	{
		return 0;
	}
}


LuaFuncReg UITabShopBase_Glue[] = {

	{"uiClickSort",				UITabShopBase_ClickSort},
	{"uiClickBuy",				UITabShopBase_ClickBuyButton},
	{"uiClickPresent",			UITabShopBase_ClickPresentButton},
	{"uiClickPrevList",			UITabShopBase_ClickPreviousList},
	{"uiClickNextList",			UITabShopBase_ClickNextList},
	{NULL,						NULL}
};
*/

// 기존코드와 달리 m_idxStartSlot을 0으로 초기화함..
// -1에 대한 예외처리를 안할수 있다면 -1은 하지 말것..11.05.18.수빈.

/*
UINT32	UITabShopBase::_GetCurrnetGoodsBuyFlag(UINT32 nGoodsBuyFlag)
{
	UINT32 Rev = 0;

	// 선택되어진 상품이 포인트, 캐쉬템으로 구성되어져 있을경우, 해당하는 플래그값을 설정합니다.
	if( GOODS_PRICE_ALL == nGoodsBuyFlag )
	{
		Rev = GOODS_PRICE_POINT;
	}
	else if( GOODS_PRICE_POINT == nGoodsBuyFlag )
	{
		Rev = GOODS_PRICE_POINT;
	}
	else if( GOODS_PRICE_CASH == nGoodsBuyFlag )
	{
		Rev = GOODS_PRICE_CASH;
	}

	return Rev;
}
*/

/*
BOOL	UITabShopBase::OnSelectSlot(UINT32 idxSlot, BOOL ForceSelect )
{
		
//	if(m_SlotMaxCount <= (INT32)idxSlot || (INT32)m_RefGoodsList.size() <= 0)
//		return FALSE;

	INT32 idx = idxSlot + m_idxStartSlot;
	I3ASSERT( idx >= 0 && (INT32)m_RefGoodsList.size() > idx);

	SHOP_GOODS_PACK * pSelected = m_RefGoodsList[idx];

	// 선택되어진 상품이 포인트, 캐쉬템으로 구성되어져 있을경우, 해당하는 플래그값을 설정합니다.
	{
		m_SelectItemPriceType			= pSelected->GetPriceTypeAll();
		m_SelectItemCurrentPriceType	= UITabShopBase::_GetCurrnetGoodsBuyFlag(m_SelectItemPriceType);
	}

	if( m_pFocusedGoods != pSelected)
	{
		m_pFocusedGoods = pSelected;

		// 아래 인자로 넘기는 "pSelected->GetLastIndex(GOODS_PRICE_ALL)"은 캐쉬아이템과 포인트 아이템을
		// 혼용해서 판매할 시에 문제가 발생 할 수 있습니다.
		INT32 RadioIndex = 0;
		pSelected->GetLastIndex(m_SelectItemCurrentPriceType, &RadioIndex);
//		OnSelectGoodsRadio(RadioIndex);
//		if ( ForceSelect == FALSE )
//			m_pOwner->PlaySound(GUI_SND_ITEM_SELECTING);

	}


	if( g_pGameContext->IsEnableGift() )
	{

#if defined (LOCALE_THAILAND)
	//	캐시템만 선물 하기 가능	
	SHOP_GOODS * pSelectedGoods = m_pFocusedGoods->GetGoods( m_SelectGoodsIndexInPack);

	if( IsCashBuyingFromGoods( pSelectedGoods))
	{
		ACTIVATE_BUTTON( m_PriceFrame.pGift);
	}
	else
	{
		DEACTIVATE_BUTTON( m_PriceFrame.pGift);
	}
#else
	ACTIVATE_BUTTON( m_PriceFrame.pGift);
#endif

	}
	else
	{
	//DEACTIVATE_BUTTON(m_PriceFrame.pGift);	// 버튼 비활성화
//	m_PriceFrame.pGift->SetEnable(FALSE);
	}

	return TRUE;
	
}
*/
