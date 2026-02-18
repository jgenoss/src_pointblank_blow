#include "pch.h"
#include "StepShop_Tab.h"
#include "StepShop.h"
#include "GuiNotifyReceiver.h"



GoodsPriceFrame::GoodsPriceFrame()
{
	i3mem::FillZero(pGoodsRadio, sizeof(pGoodsRadio));
	i3mem::FillZero(pUseDayValue, sizeof(pUseDayValue));
	i3mem::FillZero(pUseDayStatic, sizeof(pUseDayStatic));
	i3mem::FillZero(pSaleText, sizeof(pSaleText));
	i3mem::FillZero(pBuyPointValue, sizeof(pBuyPointValue));
	i3mem::FillZero(pBuyPointStatic, sizeof(pBuyPointStatic));
	i3mem::FillZero(pCashSymbol, sizeof(pCashSymbol));

	pHavePointStatic	= NULL;
	pHavePointValue		= NULL;
	pHavePointUnitStatic	= NULL;
	pHaveCashSymbol		= NULL;

	pRemainPointStatic	= NULL;
	pRemainPointValue	= NULL;
	pRemainPointUnitStatic	= NULL;
	pRemainCashSymbol	= NULL;

	pPointInfoStatic	= NULL;

	pBuy				= NULL;
	pGift				= NULL;
}

void	GoodsPriceFrame::EnableAll(BOOL bValue)
{
	for(INT32 i=0; i<GOODS_MAX_IN_PACK; i++)
	{
		pGoodsRadio[i]->SetEnable(bValue);
		pUseDayValue[i]->SetEnable(bValue);
		pUseDayStatic[i]->SetEnable(bValue);
		if ( pSaleText[i] )
			pSaleText[i]->SetEnable(bValue);
		pBuyPointValue[i]->SetEnable(bValue);
		pBuyPointStatic[i]->SetEnable(bValue);

#if defined( LOCALE_KOREA)
		pCashSymbol[i]->SetEnable(bValue);
#else
		pCashSymbol[i]->SetEnable(FALSE);
#endif
	}


	pHavePointStatic->SetEnable(bValue);
	pHavePointValue->SetEnable(bValue);
	pHavePointUnitStatic->SetEnable(bValue);

	pRemainPointStatic->SetEnable(bValue);
	pRemainPointValue->SetEnable(bValue);
	pRemainPointUnitStatic->SetEnable(bValue);

#if defined( LOCALE_KOREA)
	pHaveCashSymbol->SetEnable(bValue);
	pRemainCashSymbol->SetEnable(bValue);
#else
	pHaveCashSymbol->SetEnable(FALSE);
	pRemainCashSymbol->SetEnable(FALSE);
#endif

	pBuy->SetEnable(bValue);

#if defined (USE_SHOP_GIFT)
	pGift->SetEnable(bValue);
#else
	pGift->SetEnable(FALSE);
#endif	
	
}

INT32	GoodsPriceFrame::SetGoodsRadio(UINT32 idx)
{
	if ( idx >= GOODS_MAX_IN_PACK )
		idx = 0;

	// 모든 Shape을 비활성화한다.
	for(INT32 i=0; i<GOODS_MAX_IN_PACK; i++)
		pGoodsRadio[i]->SetAllShapeEnable(FALSE);

	UINT32 idxState[GOODS_MAX_IN_PACK] = {0, };
	idxState[idx] = 2;	// 2는 선택된 라디오 버튼 인덱스입니다.

	for(INT32 i=0; i<GOODS_MAX_IN_PACK; i++)
		pGoodsRadio[i]->SetShapeEnable(GUI_SHAPE_OFFSET + idxState[i], TRUE);

	return idx;
}

CRepairFrame::CRepairFrame(CTabBase * pOwner)
{
	m_pFrame			= NULL;
	m_pDoRepair			= NULL;

	m_pArrow			= NULL;
	m_pRepairText		= NULL;
	m_pRepairPrice		= NULL;
	m_pRepairPriceType	= NULL;

	m_pHaveMoneyText	= NULL;
	m_pHaveMoneyValue	= NULL;
	m_pHaveMoneyType	= NULL;

	m_pRemainMoneyText	= NULL;
	m_pRemainMoneyValue	= NULL;
	m_pRemainMoneyType	= NULL;

	m_pOwner			= pOwner;
}



CTabBase::CTabBase(INT32 SlotMaxCount, INT32 SlotHoriCount, CStepShop * pOwner)
: m_SlotMaxCount(SlotMaxCount)
, m_SlotHoriCount(SlotHoriCount)
{
	m_pOwner = pOwner;

	m_pTab			= NULL;
	m_pSect			= NULL;
	m_pScrollList	= NULL;
	m_idxStartSlot	= -1;

	m_SelectGoodsIndexInPack			= 0;
	m_SelectItemPriceType			= 0;
	m_SelectItemCurrentPriceType	= 0;
	m_pFocusedGoods					= NULL;

	m_OldidxStartSlot = -1;
	m_OldidxTotalSlot = -1;
	m_bVestigeDuty = false;
	m_OldWeaponUsageType = WEAPON_SLOT_UNKNOWN;
}


CTabBase::~CTabBase()
{
}


void	CTabBase::Init()
{
	INIT_TAB_EX(m_pTab);
}


BOOL	CTabBase::OnOpen()
{
	m_pTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pSect->SetEnable(TRUE);
	m_pScrollList->SetCurValue(0.0f);
	m_idxStartSlot = 0;

	m_RefGoodsList.Clear();

	m_SelectGoodsIndexInPack		= 0;
	m_SelectItemPriceType			= 0;
	m_SelectItemCurrentPriceType	= 0;

	m_PriceFrame.SetGoodsRadio();
	m_pFocusedGoods = NULL;

#if !defined (LOCALE_KOREA)
	m_pOwner->SetEffectButton(m_PriceFrame.pBuy);
#endif

	return TRUE;
}


void	CTabBase::OnClose()
{
	m_pTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pSect->SetEnable(FALSE);
	m_RefGoodsList.Clear();
}


BOOL	CTabBase::OnSelectSlot(UINT32 idxSlot, BOOL ForceSelect)
{
	if(m_SlotMaxCount <= (INT32)idxSlot || m_RefGoodsList.GetCount() <= 0)
		return FALSE;

	INT32 idx = idxSlot + m_idxStartSlot;
	I3ASSERT( idx >= 0 && m_RefGoodsList.GetCount() > idx);

	m_OldidxStartSlot = m_idxStartSlot;
	m_OldidxTotalSlot = idx;

	SHOP_GOODS_PACK * pSelected = (SHOP_GOODS_PACK*) m_RefGoodsList.GetItem(idx);

	// 선택되어진 상품이 포인트, 캐쉬템으로 구성되어져 있을경우, 해당하는 플래그값을 설정합니다.
	{
		m_SelectItemPriceType			= pSelected->GetPriceTypeAll();
		m_SelectItemCurrentPriceType	= CTabBase::_GetCurrnetGoodsBuyFlag(m_SelectItemPriceType);
	}

	if( m_pFocusedGoods != pSelected)
	{
		m_pFocusedGoods = pSelected;

		// 아래 인자로 넘기는 "pSelected->GetLastIndex(GOODS_PRICE_ALL)"은 캐쉬아이템과 포인트 아이템을
		// 혼용해서 판매할 시에 문제가 발생 할 수 있습니다.
		INT32 RadioIndex = 0;
		#if defined( LOCALE_PHILIPPINES )
			RadioIndex = 0;
		#else
			pSelected->GetLastIndex(m_SelectItemCurrentPriceType, &RadioIndex);
		#endif
		OnSelectGoodsRadio(RadioIndex);
		if ( ForceSelect == FALSE )
			m_pOwner->PlaySound(GUI_SND_ITEM_SELECTING);
	}

#if defined (USE_SHOP_GIFT)

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

#else
	//DEACTIVATE_BUTTON(m_PriceFrame.pGift);	// 버튼 비활성화
	m_PriceFrame.pGift->SetEnable(FALSE);
#endif

	return TRUE;
}


void	CTabBase::OnSelectGoodsRadio(UINT32 RadioIndex)
{
	m_PriceFrame.SetGoodsRadio(RadioIndex);
	m_SelectGoodsIndexInPack = RadioIndex;
}


void	CTabBase::OnChangeScroll()
{
	m_idxStartSlot = m_pScrollList->GetCurrentTop() * m_SlotHoriCount;
	m_OldidxStartSlot = m_idxStartSlot;
}


void	CTabBase::OnWheelUpdate(INT32 ScrollBy)
{
	if (0 < ScrollBy)
	{
		for(INT32 i = 0; i < ScrollBy; i++)
			m_pScrollList->MovePrev();
	}
	else
	{
		for(INT32 i = 0; i > ScrollBy; i--)
			m_pScrollList->MoveNext();
	}

	OnChangeScroll();
}


UINT32	CTabBase::_GetCurrnetGoodsBuyFlag(UINT32 nGoodsBuyFlag)
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


void	CTabBase::_UpdateGoodsFrame(RT_REAL32 rDeltaSeconds)
{
	SHOP_GOODS_PACK * pGoodsPack = m_pFocusedGoods;


	BOOL	preBuyEnable	= (m_PriceFrame.pBuy->getGNodeState() & I3_GAMENODE_STATE_DISABLE) ? FALSE : TRUE;
	BOOL	preGiftEnable	= (m_PriceFrame.pGift->getGNodeState() & I3_GAMENODE_STATE_DISABLE) ? FALSE : TRUE;

	m_PriceFrame.EnableAll(FALSE);
	m_PriceFrame.pBuy->SetEnable(preBuyEnable);
	m_PriceFrame.pGift->SetEnable(preGiftEnable);

	if((pGoodsPack == NULL) || (pGoodsPack->GetGoodsCount() <= 0))
	{
		m_PriceFrame.pBuy->SetEnable(FALSE);
		m_PriceFrame.pGift->SetEnable(FALSE);
		return;
	}


	char szPriceUnit[32] = "";
	char temp[256] = "";

	CShop* shop = g_pGameContext->GetShop();
	INT32 goodsCount = pGoodsPack->GetGoodsCount();
	INT32 PriceFrameIndex = 0;
	for(INT32 nIndex = 0 ; nIndex < goodsCount ; nIndex++)
	{	
		SHOP_GOODS * pGoods = pGoodsPack->GetGoods(nIndex);

		UINT32 PointPrice = 0;
		UINT32 CashPrice = 0;
		GOODS_PRICE_TYPE PriceType = shop->GetGoodsPrice(pGoods->_GoodsID, &PointPrice, &CashPrice);


		// 현재 선택되어진 상품이 캐쉬상품과 포인트상품으로 구성되어 있다면, 해당하는 상품만 출력하도록 합니다.
		if( GOODS_PRICE_POINT & GetSelectItemCurrentPriceType() )
		{
			if( PriceType == GOODS_PRICE_CASH) continue;
		}
		else if( GOODS_PRICE_CASH & GetSelectItemCurrentPriceType() )
		{
			if( PriceType == GOODS_PRICE_POINT) continue;
		}

		// 현재 선택된 가격형식으로 저장합니다.
		PriceType &= GetSelectItemCurrentPriceType();

		//	한번에 슬롯에 디스플레이되는 하나의 상품은 최대 4개까지이다. (예: 1일, 3일, 7일, 30일)
		//	4개가 넘어가면 문제 생김. 밑에서 배열 오버플로우납니다. komet
		I3ASSERT( nIndex < GOODS_MAX_IN_PACK);
		if( nIndex >= GOODS_MAX_IN_PACK)	break;


		///
		///
		///
		OnUpdateGoodsFrameText(pGoods, PriceFrameIndex);


		//	아이템 가격 표시
		if(PriceType != GOODS_PRICE_UNKNOWN)
		{
			switch(PriceType & GetSelectItemCurrentPriceType())
			{
			case GOODS_PRICE_POINT:
				{
					sprintf_s(temp, "%d", PointPrice);
					m_PriceFrame.pBuyPointValue[PriceFrameIndex]->SetText(temp);

#if defined( LOCALE_KOREA)
					m_PriceFrame.pCashSymbol[PriceFrameIndex]->SetEnable( FALSE);
#endif
				}
				break;
			case GOODS_PRICE_CASH:
				{
					sprintf_s(temp, "%d", CashPrice);
					m_PriceFrame.pBuyPointValue[PriceFrameIndex]->SetText(temp);

#if defined( LOCALE_KOREA)
					m_PriceFrame.pCashSymbol[PriceFrameIndex]->SetEnable( TRUE);
#endif
				}
				break;
			default:
				{
					I3ASSERT_0;
					sprintf_s(temp, UNKNOWN_TEXT);
					m_PriceFrame.pBuyPointValue[PriceFrameIndex]->SetText(temp);

#if defined( LOCALE_KOREA)
					m_PriceFrame.pCashSymbol[PriceFrameIndex]->SetEnable( FALSE);
#endif
				}
				break;
			}
		}
		else
		{
			sprintf_s(temp, UNKNOWN_TEXT);
			m_PriceFrame.pBuyPointValue[PriceFrameIndex]->SetText(temp);
		}

		CopyPriceTypeText( PriceType, szPriceUnit, 32 );

		m_PriceFrame.pBuyPointValue[PriceFrameIndex]->SetEnable(TRUE);


		m_PriceFrame.pBuyPointStatic[PriceFrameIndex]->SetText(szPriceUnit);
		m_PriceFrame.pBuyPointStatic[PriceFrameIndex]->SetEnable(TRUE);

		m_PriceFrame.pGoodsRadio[PriceFrameIndex]->SetEnable(TRUE);

		++PriceFrameIndex;
	}

	SHOP_GOODS * pShopGoods = pGoodsPack->GetGoods(GetCurrentSelectedGoodsIndex());


	UINT32 PointPrice = 0;
	UINT32 CashPrice = 0;
	GOODS_PRICE_TYPE priceType = shop->GetGoodsPrice(pShopGoods->_GoodsID, &PointPrice, &CashPrice);

	priceType &= GetSelectItemCurrentPriceType();

	//	유저의 남은 돈 계산
	INT32 nHavePoint = 0, nRemainPoint = 0;
	if( priceType != GOODS_PRICE_UNKNOWN)
	{
		switch(priceType)
		{
		case GOODS_PRICE_POINT:	
			{
				nHavePoint = g_pGameContext->GetMyPoint();	
				nRemainPoint = nHavePoint - PointPrice;

#if defined( LOCALE_KOREA)
				m_PriceFrame.pHaveCashSymbol->SetEnable( FALSE);
				m_PriceFrame.pRemainCashSymbol->SetEnable( FALSE);
#endif
			}
			break;
		case GOODS_PRICE_CASH:	
			{
				nHavePoint = g_pGameContext->GetMyCash();	
				nRemainPoint = nHavePoint - CashPrice;

#if defined( LOCALE_KOREA)
				m_PriceFrame.pHaveCashSymbol->SetEnable( TRUE);
				m_PriceFrame.pRemainCashSymbol->SetEnable( TRUE);
#endif
			}
			break;
		default:				
			{
				I3ASSERT_0;
				nHavePoint = g_pGameContext->GetMyPoint();	
				nRemainPoint = nHavePoint - PointPrice;

#if defined( LOCALE_KOREA)
				m_PriceFrame.pHaveCashSymbol->SetEnable( FALSE);
				m_PriceFrame.pRemainCashSymbol->SetEnable( FALSE);
#endif
			}
			break;
		}
	}


	CopyPriceTypeText( priceType, szPriceUnit, 32 );

	m_PriceFrame.pHavePointUnitStatic->SetText(szPriceUnit);
	m_PriceFrame.pHavePointUnitStatic->SetEnable(TRUE);
	m_PriceFrame.pRemainPointUnitStatic->SetText(szPriceUnit);
	m_PriceFrame.pRemainPointUnitStatic->SetEnable(TRUE);

	m_PriceFrame.pHavePointStatic->SetEnable(TRUE);
	m_PriceFrame.pRemainPointStatic->SetEnable(TRUE);
	m_PriceFrame.pHavePointValue->SetEnable(TRUE);
	m_PriceFrame.pRemainPointValue->SetEnable(TRUE);

	m_PriceFrame.pBuy->SetEnable(TRUE);

#if defined (USE_SHOP_GIFT)
	m_PriceFrame.pGift->SetEnable(TRUE);
#else
	//DEACTIVATE_BUTTON(m_PriceFrame.pGift);	// 버튼 비활성화
	m_PriceFrame.pGift->SetEnable(FALSE);
#endif	

	//	보유 포인트
	sprintf_s(temp, "%d", nHavePoint);
	m_PriceFrame.pHavePointValue->SetText(temp);

	//	잔액
	sprintf_s(temp, "%d", nRemainPoint);
	m_PriceFrame.pRemainPointValue->SetText(temp);

	COLOR color;
	if( nRemainPoint < 0)
	{
		//	돈이 모자르면 빨강색
		i3Color::Set(&color, (UINT8) 255, 0, 0, 255);
	}
	else
	{
		i3Color::Set(&color, (UINT8) 139, 217, 37, 255);
	}

	m_PriceFrame.pRemainPointValue->SetTextColor(&color);

}


BOOL	CTabBase::OnUpdateGoodsFrameText(SHOP_GOODS * pGoods, INT32 GoodsIndex)
{
	char temp[256] = {};
	ITEM_TYPE goodsClass = (ITEM_TYPE)GET_ITEM_FLAG_TYPE(pGoods->_ItemID);

	BOOL	EnableUseDayControls = TRUE;

	switch( goodsClass)
	{
	case ITEM_TYPE_PRIMARY:
	case ITEM_TYPE_SECONDARY:
	case ITEM_TYPE_MELEE:
	case ITEM_TYPE_THROWING1:
	case ITEM_TYPE_THROWING2:
	case ITEM_TYPE_CHARA:
	case ITEM_TYPE_HEADGEAR:
		{
#if defined(USE_REPAIR_WEAPON)
			if ( g_pFramework->GetShopContext()->IsDurabilityItem(pGoods->_ItemID) )
			{
				if ( m_PriceFrame.pSaleText[GoodsIndex] )
				{
					m_PriceFrame.pSaleText[GoodsIndex]->SetEnable(TRUE);
					m_PriceFrame.pSaleText[GoodsIndex]->SetText(GAME_STRING("STR_SHOP_PERPETUITY_BUY"));
				}

				EnableUseDayControls = FALSE;
			}
			else
			{
				if ( m_PriceFrame.pSaleText[GoodsIndex] )
					m_PriceFrame.pSaleText[GoodsIndex]->SetEnable(FALSE);

				// 몇일 또는 몇개의 텍스트를 만든다.
				FormatAuthTypeText(pGoods, m_PriceFrame.pUseDayValue[GoodsIndex], m_PriceFrame.pUseDayStatic[GoodsIndex]);
			}

#else
			// 몇일 또는 몇개의 텍스트를 만든다.
			FormatAuthTypeText(pGoods, m_PriceFrame.pUseDayValue[GoodsIndex], m_PriceFrame.pUseDayStatic[GoodsIndex]);
#endif

		}
		break;

	case ITEM_TYPE_MAINTENANCE:
	case ITEM_TYPE_EXPENDABLE:
		{
			INT32 itemUsage = (ITEM_TYPE) GET_ITEM_FLAG_TYPE(pGoods->_ItemID);

			//	유료 아이템 Goods에는 pGoods->_AuthArg에 모두 갯수가 있기 때문에 예외 처리가 필요 komet
			switch( (ITEM_TYPE) itemUsage)
			{
				//	지속성
			case ITEM_TYPE_MAINTENANCE:
				{
					UINT32 term = GetMaintenanceItemHour(pGoods->_ItemID, pGoods->_AuthType, pGoods->_AuthArg);

					//	인증 기간
					if(term < 24)
					{
						sprintf_s(temp, "%s :",	GAME_STRING("STR_UI_HOUR"));
					}
					else
					{
						term = term /24;
						sprintf_s(temp, "%s :",	GAME_STRING("STBL_IDX_ITEM_TERM_UNIT"));						
					}
					m_PriceFrame.pUseDayStatic[GoodsIndex]->SetText(temp);
					sprintf_s(temp, "%d", term);
					m_PriceFrame.pUseDayValue[GoodsIndex]->SetText(temp);
					
					
				}
				break;
				//	소모성 
			case ITEM_TYPE_EXPENDABLE:
				{
					INT32 count = pGoods->_AuthArg;	//	개수

					sprintf_s(temp, "%d", count);
					m_PriceFrame.pUseDayValue[GoodsIndex]->SetText(temp);

#if defined( LOCALE_BRAZIL)
					sprintf_s(temp, "%s:", GAME_STRING("STBL_IDX_ITEM_COUNT_UNIT"));
#else
					sprintf_s(temp, "%s :", GAME_STRING("STBL_IDX_ITEM_COUNT_UNIT"));
#endif
					m_PriceFrame.pUseDayStatic[GoodsIndex]->SetText(temp);
				}
				break;
			}
		}
		break;
	default:
		{
			m_PriceFrame.pUseDayValue[GoodsIndex]->SetText(UNKNOWN_TEXT);
			m_PriceFrame.pUseDayStatic[GoodsIndex]->SetText(UNKNOWN_TEXT);
		}
		break;
	}

	m_PriceFrame.pUseDayValue[GoodsIndex]->SetEnable( EnableUseDayControls);
	m_PriceFrame.pUseDayStatic[GoodsIndex]->SetEnable( EnableUseDayControls);

	return TRUE;
}

void	CTabBase::_ResetVestige()
{
	m_bVestigeDuty = false;
}

void	CTabBase::_SetupVestige()
{
#ifndef NC_BUILD
	m_bVestigeDuty = true;
#else
	if( LocaleValue::Enable("Goods_Vestige") )
		m_bVestigeDuty = true;
	else
		m_bVestigeDuty = false;
#endif
}