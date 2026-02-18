#include "pch.h"
#include "StepShop_TabSet.h"
#include "StepShop.h"
#include "GuiNotifyReceiver.h"




CTabPackage::CTabPackage(CStepShop * pOwner)
: CTabBase(MAX_SLOT_FOR_SHOP_PACKAGE, SLOT_HORI_COUNT_PACKAGE, pOwner)
{
	i3mem::FillZero(m_Slot, sizeof(m_Slot));
	m_pInfo			= NULL;
	m_pInfoDummy	= NULL;
	m_pInfoValue	= NULL;
	m_pInfoDesc		= NULL;
}


CTabPackage::~CTabPackage()
{
}

void	CTabPackage::Init()
{
	CTabBase::Init();

	char temp[MAX_STRING_COUNT] = "";

	INIT_CAPTION_BUTTON(m_pAllGroup, GAME_STRING("STBL_IDX_SHOP_FILTER_ALL"));

	m_pScrollList->CreateTextEx(SLOT_VERT_COUNT_PACKAGE,  GetDefaultFontName());
	m_pScrollList->SetSelColor(0, 0, 0, 0);

	m_pAllGroup->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	INIT_BUTTON_EX(m_pInfo);

	m_pInfo->setInputDisable(TRUE);
	m_pInfo->SetAllShapeEnable(FALSE);
	m_pInfo->SetShapeEnable(GUI_SHAPE_OFFSET, TRUE);
	m_pInfoDummy->SetEnable(FALSE);
	m_pInfoValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pInfoValue->SetTextColor(GetColor(GCT_DEFAULT));

	m_pInfoDesc->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_1024, GAME_FONT_DEFAULT_SIZE, FALSE, NULL);
	m_pInfoDesc->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pInfoDesc->SetTextColor(GetColor(GCT_DEFAULT));
	//m_pInfoDesc->SetTextSpace(0, 2);
	m_pInfoDesc->SetTextAutoWrap(TRUE);

	INIT_CAPTION_BUTTON(m_PriceFrame.pBuy, GAME_STRING("STBL_IDX_SHOP_BUY"));
	INIT_CAPTION_BUTTON(m_PriceFrame.pGift, GAME_STRING("STBL_IDX_SHOP_GIFT"));

	for(UINT32 i = 0; i < MAX_SLOT_FOR_SHOP_PACKAGE; i++)
	{
		INIT_BUTTON_EX(m_Slot[i]._pSlot);

		m_Slot[i]._pValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_Slot[i]._pValue->SetTextColor(GetColor(GCT_DEFAULT));
		m_Slot[i]._pValue->SetTextSpace(0, -3);

		m_Slot[i]._pTerm->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_Slot[i]._pTerm->SetTextColor(GetColor(GCT_DEFAULT));

		m_Slot[i]._pImage->setInputDisable(TRUE);
		m_Slot[i]._pValue->setInputDisable(TRUE);
		m_Slot[i]._pTerm->setInputDisable(TRUE);

		m_Slot[i]._pSaleMark->SetEnable(FALSE);
		m_Slot[i]._pSaleMark->SetAllShapeEnable(FALSE);

		m_Slot[i]._pSaleText[SALE_TEXT_FIRST]->setInputDisable(TRUE);
		m_Slot[i]._pSaleText[SALE_TEXT_SECOND]->setInputDisable(TRUE);
		m_Slot[i]._pSaleText[SALE_TEXT_FIRST]->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
		m_Slot[i]._pSaleText[SALE_TEXT_SECOND]->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
		m_Slot[i]._pSaleText[SALE_TEXT_FIRST]->SetEnable(FALSE);
		m_Slot[i]._pSaleText[SALE_TEXT_SECOND]->SetEnable(FALSE);
	}

	// Item Goods frame
	{

		m_PriceFrame.pPointInfoStatic->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_PriceFrame.pPointInfoStatic->SetTextColor(GetColor(GCT_DEFAULT));
		m_PriceFrame.pPointInfoStatic->SetText(GAME_STRING("STBL_IDX_SHOP_INFO_POINT"));

		m_PriceFrame.pHavePointStatic->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
		m_PriceFrame.pHavePointStatic->SetTextColor(GetColor(GCT_DEFAULT));
		sprintf_s(temp, "%s :", GAME_STRING("STBL_IDX_SHOP_HAVE_MONEY"));
		m_PriceFrame.pHavePointStatic->SetText(temp);

		m_PriceFrame.pRemainPointStatic->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
		m_PriceFrame.pRemainPointStatic->SetTextColor(GetColor(GCT_DEFAULT));
		sprintf_s(temp, "%s :", GAME_STRING("STBL_IDX_SHOP_REMAIN_MONEY"));
		m_PriceFrame.pRemainPointStatic->SetText(temp);

		m_PriceFrame.pHavePointValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
		m_PriceFrame.pHavePointValue->SetTextColor(GetColor(GCT_DEFAULT));
		m_PriceFrame.pHavePointValue->SetText("0");

		m_PriceFrame.pRemainPointValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
		m_PriceFrame.pRemainPointValue->SetTextColor(GetColor(GCT_DEFAULT));
		m_PriceFrame.pRemainPointValue->SetText("0");

		for( INT32 i=0; i<GOODS_MAX_IN_PACK; i++)
		{
			m_PriceFrame.pUseDayStatic[i]->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
			m_PriceFrame.pUseDayStatic[i]->SetTextColor(GetColor(GCT_DEFAULT));
			sprintf_s(temp, "%s :", GAME_STRING("STBL_IDX_SHOP_USE_DAY"));
			m_PriceFrame.pUseDayStatic[i]->SetText(temp);

			m_PriceFrame.pUseDayValue[i]->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
			m_PriceFrame.pUseDayValue[i]->SetTextColor(GetColor(GCT_DEFAULT));
			m_PriceFrame.pUseDayValue[i]->SetText("0");

			m_PriceFrame.pBuyPointValue[i]->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
			m_PriceFrame.pBuyPointValue[i]->SetTextColor(GetColor(GCT_DEFAULT));
			m_PriceFrame.pBuyPointValue[i]->SetText("0");

			m_PriceFrame.pBuyPointStatic[i]->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
			m_PriceFrame.pBuyPointStatic[i]->SetTextColor(GetColor(GCT_DEFAULT));
			m_PriceFrame.pBuyPointStatic[i]->SetText(GAME_STRING("STBL_IDX_SHOP_MONEY"));
		}

		m_PriceFrame.pHavePointUnitStatic->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_PriceFrame.pHavePointUnitStatic->SetTextColor(GetColor(GCT_DEFAULT));
		m_PriceFrame.pHavePointUnitStatic->SetText(GAME_STRING("STBL_IDX_SHOP_MONEY"));

		m_PriceFrame.pRemainPointUnitStatic->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_PriceFrame.pRemainPointUnitStatic->SetTextColor(GetColor(GCT_DEFAULT));
		m_PriceFrame.pRemainPointUnitStatic->SetText(GAME_STRING("STBL_IDX_SHOP_MONEY"));


		ACTIVATE_BUTTON(m_PriceFrame.pBuy);

#if defined USE_SHOP_GIFT
		ACTIVATE_BUTTON(m_PriceFrame.pGift);
#else
		//DEACTIVATE_BUTTON(m_PriceFrame.pGift);	// 버튼 비활성화
		m_PriceFrame.pGift->SetEnable(FALSE);
#endif
	}

	CTabBase::OnClose();
}


BOOL	CTabPackage::OnOpen()
{
	if ( ! g_pConfigEx->GetShop().Set.Enable )
	{
		STRING_POPUP(GAME_STRING("STR_SHOP_PREPARE_PLAN"));/*차후 지원될 예정입니다.*/
		return FALSE;
	}

	CTabBase::OnOpen();
	OnPackageGoodsAllGroup();
	MakeGroupList();
	OnSelectSlot(0, TRUE);
	return TRUE;
}


void	CTabPackage::OnClose()
{
	CTabBase::OnClose();
}


void	CTabPackage::OnUpdate(RT_REAL32 rDeltaSec)
{
	_UpdateSlotList();
	_UpdateDesc();
	_UpdateGoodsFrame(rDeltaSec);
}


void	CTabPackage::OnFocus(UINT32 idxSlot)
{
	if(MAX_SLOT_FOR_SHOP_PACKAGE <= idxSlot)
		return;

	if (FALSE == m_Slot[idxSlot]._pSlot->GetShapeEnable(SLOT_SHAPE_INDEX_PUSHED))
	{
		m_Slot[idxSlot]._pSlot->SetShapeEnable(SLOT_SHAPE_INDEX_NORMAL, FALSE);
		m_Slot[idxSlot]._pSlot->SetShapeEnable(SLOT_SHAPE_INDEX_HOVER, TRUE);
	}
}


void	CTabPackage::OnLinkControl(i3BinList * plist)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	SetGoods
	LinkControlEx(plist, "shopSetTab", m_pTab, GCI_SHOP_B_SET_TAB);
	LinkControlEx(plist, "shopSet", m_pSect, -1);
	LinkControlEx(plist, "shopSetScrollList", m_pScrollList, GCI_SHOP_L_SET_SCROLL);

	LinkControlEx(plist, "shopSetInfo", m_pInfo, -1);
	LinkControlEx(plist, "shopSetInfoDummy", m_pInfoDummy, -1);
	LinkControlEx(plist, "shopSetInfoValue", m_pInfoValue, -1);
	LinkControlEx(plist, "shopSetAllGroup", m_pAllGroup, GCI_SHOP_B_SET_ALL_GROUP);

	for(INT32 i = 0; i < MAX_SLOT_FOR_SHOP_PACKAGE ; i++)
	{
		char strName[MAX_STRING_COUNT];

		sprintf_s(strName, "shopSetSlot%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pSlot, GCI_SHOP_B_SET_SLOT_1 + i);

		sprintf_s(strName, "shopSetDummy%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pImage, -1);

		sprintf_s(strName, "shopSetValue%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pValue, -1);

		sprintf_s(strName, "shopSetTerm%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pTerm, -1);

		sprintf_s(strName, "shopSetSaleMark%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pSaleMark, -1);

		sprintf_s(strName, "shopSetFirstSaleText%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pSaleText[SALE_TEXT_FIRST], -1);

		sprintf_s(strName, "shopSetSecondSaleText%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pSaleText[SALE_TEXT_SECOND], -1);
	}

	LinkControlEx(plist, "shopSetDescription", m_pInfoDesc, -1);

	LinkControlEx(plist, "shopSetGoodsBuy", m_PriceFrame.pBuy, GCI_SHOP_B_SET_BUY);
	LinkControlEx(plist, "shopSetGoodsGift", m_PriceFrame.pGift, GCI_SHOP_B_SET_GIFT);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	Goods frame - komet
	{
		char szTemp[256] = "";
		LinkControlEx(plist, "shopSetPointInfoStatic", m_PriceFrame.pPointInfoStatic, -1);

		for( INT32 i=0; i<GOODS_MAX_IN_PACK; i++)
		{
			sprintf_s(szTemp, "shopSetGoodsRadio0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pGoodsRadio[i], GCI_SHOP_R_SET_GOODS_RADIO_1 + i);

			//	사용 기간
			sprintf_s(szTemp, "shopSetGoodsDayStatic0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pUseDayStatic[i], -1);

			sprintf_s(szTemp, "shopSetGoodsDayValue0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pUseDayValue[i], GCI_SHOP_S_SET_USE_DAY_1 + i);

			//	구입 포인트
			sprintf_s(szTemp, "shopSetGoodsPointValue0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pBuyPointValue[i], GCI_SHOP_S_SET_BUY_POINT_1 + i);

			// 화폐단위
			sprintf_s(szTemp, "shopSetGoodsPointStatic0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pBuyPointStatic[i], -1);

			// 한국 N코인 심볼 이미지
			sprintf_s(szTemp, "shopSetGoodsCashSymbol0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pCashSymbol[i], -1);
		}


		LinkControlEx(plist, "shopSetGoodsHavePointStatic", m_PriceFrame.pHavePointStatic, -1);
		LinkControlEx(plist, "shopSetGoodsHavePointValue", m_PriceFrame.pHavePointValue, GCI_SHOP_S_SET_HAVE_POINT);
		LinkControlEx(plist, "shopSetGoodsHaveCashSymbol", m_PriceFrame.pHaveCashSymbol, -1);
		LinkControlEx(plist, "shopSetGoodsPointStatic05", m_PriceFrame.pHavePointUnitStatic, -1);
		LinkControlEx(plist, "shopSetGoodsRemainPointStatic", m_PriceFrame.pRemainPointStatic, -1);
		LinkControlEx(plist, "shopSetGoodsRemainPointValue", m_PriceFrame.pRemainPointValue, GCI_SHOP_S_SET_REMAIN_POINT);
		LinkControlEx(plist, "shopSetGoodsRemainCashSymbol", m_PriceFrame.pRemainCashSymbol, -1);
		LinkControlEx(plist, "shopSetGoodsPointStatic06", m_PriceFrame.pRemainPointUnitStatic, -1);
	}

	CTabBase::OnClose();
}


void	CTabPackage::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
	case GCI_SHOP_L_SET_SCROLL:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent)
			OnChangeScroll();
		break;

	case GCI_SHOP_R_SET_GOODS_RADIO_1:
	case GCI_SHOP_R_SET_GOODS_RADIO_2:
	case GCI_SHOP_R_SET_GOODS_RADIO_3:
	case GCI_SHOP_R_SET_GOODS_RADIO_4:
		if (I3GUI_RADIO_NOTIFY_CLICKED == pNotify->m_nEvent)
			OnSelectGoodsRadio(pNotify->m_nID - GCI_SHOP_R_SET_GOODS_RADIO_1);
		break;

	case GCI_SHOP_B_SET_ALL_GROUP:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			OnPackageGoodsAllGroup();
		break;

	case GCI_SHOP_B_SET_SLOT_1:
	case GCI_SHOP_B_SET_SLOT_2:
	case GCI_SHOP_B_SET_SLOT_3:
	case GCI_SHOP_B_SET_SLOT_4:
	case GCI_SHOP_B_SET_SLOT_5:
	case GCI_SHOP_B_SET_SLOT_6:
		if (I3GUI_STATIC_NOTIFY_ONMOUSE == pNotify->m_nEvent)
			OnFocus(pNotify->m_nID - GCI_SHOP_B_SET_SLOT_1);
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent)
			OnSelectSlot(pNotify->m_nID - GCI_SHOP_B_SET_SLOT_1);
		if (I3GUI_STATIC_NOTIFY_DOUBLECLICK == pNotify->m_nEvent)
			m_pOwner->OnGoodsBuy(pNotify->m_nID - GCI_SHOP_B_SET_SLOT_1);
		break;

	case GCI_SHOP_B_SET_BUY:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) m_pOwner->OnGoodsBuy(pNotify->m_nID - GCI_SHOP_B_SET_SLOT_1);
		break;

	case GCI_SHOP_B_SET_GIFT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) m_pOwner->OnGoodsGift(pNotify->m_nID - GCI_SHOP_B_SET_SLOT_1);
		break;
	}
}


void	CTabPackage::OnPackageGoodsAllGroup(void)
{
	m_pAllGroup->setControlState(I3GUI_CONTROL_STATE_DISABLED);
}

void	CTabPackage::_UpdateSlotList(void)
{
	for(UINT32 i = 0; i < MAX_SLOT_FOR_SHOP_PACKAGE ; i++)
	{
		m_Slot[i]._pSlot->SetShapeEnable(SLOT_SHAPE_INDEX_SELECTED, FALSE);
		m_Slot[i]._pSlot->setInputDisable(TRUE);
		m_Slot[i]._pSlot->SetEnable(FALSE);

		m_Slot[i]._pImage->SetEnable(FALSE);
		m_Slot[i]._pValue->SetEnable(FALSE);

		m_Slot[i]._pSaleMark->SetEnable( FALSE);
		m_Slot[i]._pSaleMark->SetAllShapeEnable( FALSE);
	}



	// 목록을 모두 지운다
	GameGUI::Reset(m_pScrollList, GameGUI::CalcuScrollCount(m_RefGoodsList.GetCount(), SLOT_HORI_COUNT_PACKAGE));

	CShop* shop = g_pGameContext->GetShop();

	//	상품을 화면에 출력한다.
	for(INT32 i = 0 ; i < MAX_SLOT_FOR_SHOP_PACKAGE ; i++)
	{
		if( i + m_idxStartSlot >= m_RefGoodsList.GetCount() || (i + m_idxStartSlot) < 0)
			continue;

		SHOP_GOODS_PACK * pGoodsPack = (SHOP_GOODS_PACK*) m_RefGoodsList.GetItem(i + m_idxStartSlot);
		if( pGoodsPack == NULL)
			continue;

		SHOP_GOODS * pGoods = pGoodsPack->GetGoods(0);
		if( pGoods == NULL)
			continue;

		INT32 GoodsID = pGoods->_GoodsID;

		// 관심 아이템 체크
		SetItemSaleMark( m_Slot[i]._pSaleMark, pGoods->_SaleMark);				

		//	선택 슬롯
		if (m_pFocusedGoods == pGoodsPack)
		{
			m_Slot[i]._pSlot->SetShapeEnable(SLOT_SHAPE_INDEX_SELECTED, TRUE);
		}

		//	이미지
		g_pFramework->SetPackageGoodsShape(m_Slot[i]._pImage, GoodsID);

	
		// 슬록 활성화
		m_Slot[i]._pSlot->SetShapeEnable(SLOT_SHAPE_INDEX_PUSHED, FALSE);
		m_Slot[i]._pSlot->setInputDisable(FALSE);
		m_Slot[i]._pSlot->SetEnable(TRUE);

#ifndef NC_BUILD
		bool noName = false;
#endif

		// 이름
		char szName[MAX_STRING_COUNT] = "";
		ShopPackageGoodsInfo* pInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(GoodsID);

		if( pInfo)
		{
			i3String::Copy( szName, pInfo->GetScriptName(), MAX_STRING_COUNT );

			if( i3String::Length(szName) == 0)
			{
				i3String::Format(szName, MAX_STRING_COUNT, "NO NAME\nGoods:%d\nItem:%d", 
					pGoods->_GoodsID, pGoods->_ItemID);
#ifndef NC_BUILD
			noName = true;
#endif
			}
		}
		else
		{
			i3String::Format(szName, MAX_STRING_COUNT, "INVALID\nGoods:%d\nItem:%d", 
				pGoods->_GoodsID, pGoods->_ItemID);
#ifndef NC_BUILD
			noName = true;
#endif
		}

#ifndef NC_BUILD
		m_Slot[i]._pValue->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE );
		m_Slot[i]._pSaleMark->SetEnable( TRUE );
		m_Slot[i]._pImage->SetEnable( TRUE );

		if( g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LCTRL ) )
		{
			if( !noName )
			{
				i3String::Format(szName, sizeof(szName), 
					"%s\nGoods:%d\nItem:%d", szName,
					pGoods->_GoodsID, pGoods->_ItemID);
			}
			m_Slot[i]._pValue->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP );
			m_Slot[i]._pSaleMark->SetEnable( FALSE);
			m_Slot[i]._pImage->SetEnable( FALSE );
		}
#endif

		m_Slot[i]._pValue->SetText(szName);
		m_Slot[i]._pValue->SetEnable(TRUE);
		m_Slot[i]._pTerm->SetEnable(FALSE);

		UINT32 BuyType = (UINT32)shop->GetGoodsPrice(pGoods->_GoodsID, NULL, NULL);
		m_pOwner->_PrintSaleText(m_Slot[i]._pSaleText, BuyType);
	}
}

void	CTabPackage::_UpdateDesc(void)
{
	// 초기화
	m_pInfo->SetShapeEnable(GUI_SHAPE_OFFSET + 3, FALSE);
	m_pInfo->SetEnable(TRUE);
	m_pInfoDummy->SetEnable(FALSE);
	m_pInfoValue->SetEnable(FALSE);
	m_pInfoDesc->SetEnable(FALSE);

	// 찾지 못하면 처리하지 않음
	if( m_pFocusedGoods == NULL)
		return;

	SHOP_GOODS * pGoods = m_pFocusedGoods->GetGoods(0);
	if( pGoods == NULL)
		return;

	//	아이템 ID를 Flag로 변환
	INT32 GoodsID = pGoods->_GoodsID;

	//	이미지
	if( g_pFramework->SetPackageGoodsShape(m_pInfoDummy, GoodsID))
	{
		m_pInfo->SetShapeEnable(GUI_SHAPE_OFFSET + 3, FALSE);
	}

	ShopPackageGoodsInfo* pInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(GoodsID);
	if( pInfo)
	{
		m_pInfoValue->SetText(pInfo->GetScriptName());		// 이름
		m_pInfoDesc->SetText(pInfo->GetDescription());	// 설명
	}

	m_pInfoValue->SetEnable(TRUE);
	m_pInfoDesc->SetEnable(TRUE);
}


void	CTabPackage::MakeGroupList()
{
	m_RefGoodsList.Clear();

	CShopContext * pShopContext = g_pFramework->GetShopContext();
	INT32 goodsPackCount = pShopContext->GetGoodsPackCount(SHOP_PACKAGE_GOODS);

	//	필터를 거친 상품만 리스트에 넣는다.
	for(INT32 i = 0 ; i < goodsPackCount ; i++)
	{
		SHOP_GOODS_PACK * pGoodsPack = pShopContext->GetGoods(SHOP_PACKAGE_GOODS, i);
		if( pGoodsPack == NULL)
			continue;

		SHOP_GOODS * pGoods = pGoodsPack->GetGoods(0);
		if( pGoods == NULL)
			continue;

		m_RefGoodsList.Add((void*) pGoodsPack);
	}
}


BOOL	CTabPackage::OnUpdateGoodsFrameText(SHOP_GOODS * pGoods, INT32 GoodsIndex)
{
	CShop* shop = g_pGameContext->GetShop();
	SHOP_GOODS_LIST * pGoodsList = shop->GetGoodsListData(pGoods->_GoodsID);
	if ( pGoodsList == NULL )
		return FALSE;

	char temp[MAX_STRING_COUNT] = {};
	BOOL isPeriod = FALSE;
	INT32 periodItemIdx = 0;
	INT32 notPointItemIdx = 0;

	INT32 ItemCount = pGoodsList->_ui8ItemCount;
	for(INT32 k = 0; k < ItemCount ; k++)
	{
		if (AUTH_SEC == pGoodsList->_ppItemData[k]->_ui8AuthType)
		{
			isPeriod = TRUE;
			periodItemIdx = k;
			break;
		}
		else if (ITEM_TYPE_POINT != GET_ITEM_FLAG_TYPE(pGoodsList->_ppItemData[k]->_ui32ItemID))
		{
			notPointItemIdx = k;
		}
	}

	if (isPeriod)
	{
		UINT32 seconds = pGoodsList->_pui32ItemArg[periodItemIdx];
		UINT32 term  = ConvertSecToUseHour(seconds);
		if(term < 24)
		{
			sprintf_s(temp, "%s :", GAME_STRING("STR_UI_HOUR"));
		}
		else
		{
			term = term /24;
			sprintf_s(temp, "%s :", GAME_STRING("STBL_IDX_ITEM_TERM_UNIT"));			
		}
		m_PriceFrame.pUseDayStatic[GoodsIndex]->SetText(temp);
		sprintf_s(temp, "%d", term);
		m_PriceFrame.pUseDayValue[GoodsIndex]->SetText(temp);
	}
	else
	{
		INT32 ItemID = pGoodsList->_ppItemData[notPointItemIdx]->_ui32ItemID;
		INT32 itemUsage = (ITEM_TYPE) GET_ITEM_FLAG_TYPE(ItemID);

		//	유료 아이템 Goods에는 pGoods->_AuthArg에 모두 갯수가 있기 때문에 예외 처리가 필요 komet
		switch( (ITEM_TYPE) itemUsage)
		{
			//	지속성
		case ITEM_TYPE_MAINTENANCE:
			{
				//	인증 기간
				UINT32 term = GetMaintenanceItemHour(pGoods->_ItemID, pGoods->_AuthType, pGoods->_AuthArg);

				if(term < 24 )
				{
					i3String::Format(temp, MAX_STRING_COUNT, "%s :", GAME_STRING("STR_UI_HOUR"));
				}
				else
				{
					term = term / 24;
					i3String::Format(temp, MAX_STRING_COUNT, "%s :", GAME_STRING("STBL_IDX_ITEM_TERM_UNIT"));					
				}
				m_PriceFrame.pUseDayStatic[GoodsIndex]->SetText(temp);
				i3String::Format(temp, MAX_STRING_COUNT, "%d", term);
				m_PriceFrame.pUseDayValue[GoodsIndex]->SetText(temp);
				
			}
			break;
			//	소모성
		case ITEM_TYPE_EXPENDABLE:
			{
				INT32 count = pGoods->_AuthArg;	//	개수

				i3String::Format(temp, MAX_STRING_COUNT, "%d", count);
				m_PriceFrame.pUseDayValue[GoodsIndex]->SetText(temp);

#if defined( LOCALE_BRAZIL)
				i3String::Format(temp, MAX_STRING_COUNT, "%s:", GAME_STRING("STBL_IDX_ITEM_COUNT_UNIT"));
#else
				i3String::Format(temp, MAX_STRING_COUNT, "%s :", GAME_STRING("STBL_IDX_ITEM_COUNT_UNIT"));
#endif
				m_PriceFrame.pUseDayStatic[GoodsIndex]->SetText(temp);
			}
			break;
		default:
			m_PriceFrame.pUseDayValue[GoodsIndex]->SetText(UNKNOWN_TEXT);
			m_PriceFrame.pUseDayStatic[GoodsIndex]->SetText(UNKNOWN_TEXT);
			break;
		}
	}


	m_PriceFrame.pUseDayValue[GoodsIndex]->SetEnable( TRUE);
	m_PriceFrame.pUseDayStatic[GoodsIndex]->SetEnable( TRUE);

	return TRUE;
}


