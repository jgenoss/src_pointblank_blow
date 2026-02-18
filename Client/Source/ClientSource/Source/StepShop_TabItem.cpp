#include "pch.h"
#include "StepShop_TabItem.h"
#include "StepShop.h"
#include "GuiNotifyReceiver.h"




CTabItem::CTabItem(CStepShop * pOwner)
: CTabBase(MAX_SLOT_FOR_SHOP_ITEM, SLOT_HORI_COUNT_ITEM, pOwner)
{
	// cashitem initialize
	i3mem::FillZero(m_Slot, sizeof(m_Slot));
	m_pInfo = NULL;
	m_pInfoDummy = NULL;
	m_pInfoValue = NULL;
	m_pInfoDesc = NULL;

	m_nCashItemUsageType = (ITEM_TYPE)-2;
}


CTabItem::~CTabItem()
{
}


void	CTabItem::Init()
{
	CTabBase::Init();

	char temp[MAX_STRING_COUNT] = "";

	INIT_CAPTION_BUTTON(m_pAllGroup, GAME_STRING("STBL_IDX_SHOP_FILTER_ALL"));
	INIT_CAPTION_BUTTON(m_pExpendableGroup, GAME_STRING("STBL_IDX_SHOP_FILTER_COUNT"));
	INIT_CAPTION_BUTTON(m_pMaintenanceGroup, GAME_STRING("STBL_IDX_SHOP_FILTER_TERM"));

	m_pScrollList->CreateTextEx(SLOT_VERT_COUNT_ITEM,  GetDefaultFontName());
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
		
	for(UINT32 i = 0; i < MAX_SLOT_FOR_SHOP_ITEM; i++)
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


BOOL	CTabItem::OnOpen()
{
	if ( ! g_pConfigEx->GetShop().Item.Enable )
	{
		STRING_POPUP(GAME_STRING("STR_SHOP_PREPARE_PLAN"));/*차후 지원될 예정입니다.*/
		return FALSE;
	}
	CTabBase::OnOpen();

	m_nCashItemUsageType = (ITEM_TYPE)-2;

	_ChangeCashItemUsage(ITEM_TYPE_UNKNOWN);
	return TRUE;
}


void	CTabItem::OnClose()
{
	CTabBase::OnClose();

	i3GuiButton* pButton = GetGroupButton(m_nCashItemUsageType);
	if ( pButton )
		pButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);
}


void	CTabItem::OnUpdate(RT_REAL32 rDeltaSec)
{
	_UpdateSlotList();
	_UpdateDesc();
	_UpdateGoodsFrame(rDeltaSec);
}


void	CTabItem::OnFocus(UINT32 idxSlot)
{
	if(MAX_SLOT_FOR_SHOP_ITEM <= idxSlot)
		return;

	if (FALSE == m_Slot[idxSlot]._pSlot->GetShapeEnable(SLOT_SHAPE_INDEX_PUSHED))
	{
		m_Slot[idxSlot]._pSlot->SetShapeEnable(SLOT_SHAPE_INDEX_NORMAL, FALSE);
		m_Slot[idxSlot]._pSlot->SetShapeEnable(SLOT_SHAPE_INDEX_HOVER, TRUE);
	}
}


void	CTabItem::OnPreGuiNotify()
{
}


void	CTabItem::OnLinkControl(i3BinList * plist)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	//	Item
	LinkControlEx(plist, "shopItemTab", m_pTab, GCI_SHOP_B_ITEM_TAB);
	LinkControlEx(plist, "shopItem", m_pSect, -1);
	LinkControlEx(plist, "shopItemScrollList", m_pScrollList, GCI_SHOP_L_ITEM_SCROLL);

	LinkControlEx(plist, "shopItemInfo", m_pInfo, -1);
	LinkControlEx(plist, "shopItemInfoDummy", m_pInfoDummy, -1);
	LinkControlEx(plist, "shopItemInfoValue", m_pInfoValue, -1);
	LinkControlEx(plist, "shopItemAllGroup", m_pAllGroup, GCI_SHOP_B_ITEM_ALL_GROUP);
	LinkControlEx(plist, "shopItemExpendableGroup", m_pExpendableGroup, GCI_SHOP_B_ITEM_EXPENDABLE_GROUP);
	LinkControlEx(plist, "shopItemMaintenanceGroup", m_pMaintenanceGroup, GCI_SHOP_B_ITEM_MAINTENANCE_GROUP);

	for(UINT32 i = 0; i < MAX_SLOT_FOR_SHOP_ITEM; i++)
	{
		char strName[MAX_STRING_COUNT];

		sprintf_s(strName, "shopItemSlot%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pSlot, GCI_SHOP_B_ITEM_SLOT_1 + i);

		sprintf_s(strName, "shopItemDummy%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pImage, -1);

		sprintf_s(strName, "shopItemValue%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pValue, -1);

		sprintf_s(strName, "shopItemTerm%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pTerm, -1);

		sprintf_s(strName, "shopItemSaleMark%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pSaleMark, -1);

		sprintf_s(strName, "shopItemFirstSaleText%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pSaleText[SALE_TEXT_FIRST], -1);

		sprintf_s(strName, "shopItemSecondSaleText%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pSaleText[SALE_TEXT_SECOND], -1);
	}	

	LinkControlEx(plist, "shopItemDescription", m_pInfoDesc, -1);

	LinkControlEx(plist, "shopItemGoodsBuy", m_PriceFrame.pBuy, GCI_SHOP_B_ITEM_BUY);
	LinkControlEx(plist, "shopItemGoodsGift", m_PriceFrame.pGift, GCI_SHOP_B_ITEM_GIFT);
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	//	Goods frame - komet
	{
		char szTemp[256] = "";
		LinkControlEx(plist, "shopItemPointInfoStatic", m_PriceFrame.pPointInfoStatic, -1);

		for( INT32 i=0; i<GOODS_MAX_IN_PACK; i++)
		{
			sprintf_s(szTemp, "shopItemGoodsRadio0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pGoodsRadio[i], GCI_SHOP_R_ITEM_GOODS_RADIO_1 + i);

			//	사용 기간
			sprintf_s(szTemp, "shopItemGoodsDayStatic0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pUseDayStatic[i], -1);

			sprintf_s(szTemp, "shopItemGoodsDayValue0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pUseDayValue[i], GCI_SHOP_S_ITEM_USE_DAY_1 + i);

			//	구입 포인트			
			sprintf_s(szTemp, "shopItemGoodsPointValue0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pBuyPointValue[i], GCI_SHOP_S_ITEM_BUY_POINT_1 + i);

			// 화폐단위
			sprintf_s(szTemp, "shopItemGoodsPointStatic0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pBuyPointStatic[i], -1);	

			// 한국 N코인 심볼 이미지
			sprintf_s(szTemp, "shopItemGoodsCashSymbol0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pCashSymbol[i], -1);
		}

		LinkControlEx(plist, "shopItemGoodsHavePointStatic", m_PriceFrame.pHavePointStatic, -1);
		LinkControlEx(plist, "shopItemGoodsHavePointValue", m_PriceFrame.pHavePointValue, GCI_SHOP_S_ITEM_HAVE_POINT);
		LinkControlEx(plist, "shopItemGoodsHaveCashSymbol", m_PriceFrame.pHaveCashSymbol, -1);
		LinkControlEx(plist, "shopItemGoodsPointStatic05", m_PriceFrame.pHavePointUnitStatic, -1);
		LinkControlEx(plist, "shopItemGoodsRemainPointStatic", m_PriceFrame.pRemainPointStatic, -1);	
		LinkControlEx(plist, "shopItemGoodsRemainPointValue", m_PriceFrame.pRemainPointValue, GCI_SHOP_S_ITEM_REMAIN_POINT);
		LinkControlEx(plist, "shopItemGoodsRemainCashSymbol", m_PriceFrame.pRemainCashSymbol, -1);
		LinkControlEx(plist, "shopItemGoodsPointStatic06", m_PriceFrame.pRemainPointUnitStatic, -1);
	}

	CTabBase::OnClose();
}


void	CTabItem::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
		//	Item
	case GCI_SHOP_L_ITEM_SCROLL:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent)
			OnChangeScroll();
		break;

	case GCI_SHOP_R_ITEM_GOODS_RADIO_1:
	case GCI_SHOP_R_ITEM_GOODS_RADIO_2:
	case GCI_SHOP_R_ITEM_GOODS_RADIO_3:
	case GCI_SHOP_R_ITEM_GOODS_RADIO_4:
		if( I3GUI_RADIO_NOTIFY_CLICKED == pNotify->m_nEvent)
			OnSelectGoodsRadio(pNotify->m_nID - GCI_SHOP_R_ITEM_GOODS_RADIO_1);
		break;

	case GCI_SHOP_B_ITEM_ALL_GROUP:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			_ChangeCashItemUsage(ITEM_TYPE_UNKNOWN);
		break;

	case GCI_SHOP_B_ITEM_EXPENDABLE_GROUP:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			_ChangeCashItemUsage(ITEM_TYPE_EXPENDABLE);
		break;

	case GCI_SHOP_B_ITEM_MAINTENANCE_GROUP:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			_ChangeCashItemUsage(ITEM_TYPE_MAINTENANCE);
		break;

	case GCI_SHOP_B_ITEM_SLOT_1:	// fall through
	case GCI_SHOP_B_ITEM_SLOT_2:	// fall through
	case GCI_SHOP_B_ITEM_SLOT_3:	// fall through
	case GCI_SHOP_B_ITEM_SLOT_4:	// fall through
	case GCI_SHOP_B_ITEM_SLOT_5:	// fall through
	case GCI_SHOP_B_ITEM_SLOT_6:	// fall through	
		if (I3GUI_STATIC_NOTIFY_ONMOUSE == pNotify->m_nEvent)
			OnFocus(pNotify->m_nID - GCI_SHOP_B_ITEM_SLOT_1);
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent)
			OnSelectSlot(pNotify->m_nID - GCI_SHOP_B_ITEM_SLOT_1);
		if (I3GUI_STATIC_NOTIFY_DOUBLECLICK == pNotify->m_nEvent)
			m_pOwner->OnGoodsBuy(pNotify->m_nID - GCI_SHOP_B_ITEM_SLOT_1);
		break;

	case GCI_SHOP_B_ITEM_BUY:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			m_pOwner->OnGoodsBuy(pNotify->m_nID - GCI_SHOP_B_ITEM_SLOT_1);
		break;

	case GCI_SHOP_B_ITEM_GIFT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			m_pOwner->OnGoodsGift(pNotify->m_nID - GCI_SHOP_B_ITEM_SLOT_1);
		break;
	}
}


void	CTabItem::_ChangeCashItemUsage(ITEM_TYPE nItemUsage)
{
	if( m_nCashItemUsageType == nItemUsage) return;

	i3GuiButton* pButton = NULL;

	// 기존 버튼을 활성화시킨다.
	pButton = GetGroupButton(m_nCashItemUsageType);
	if ( pButton )
		pButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	// 새로 변경되는 그룹의 버튼을 비활성화 한다.
	pButton = GetGroupButton(nItemUsage);
	I3ASSERT( pButton != NULL );
	if ( ! pButton )
		return;
	pButton->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	m_nCashItemUsageType = nItemUsage;
	MakeGroupList();

	m_pFocusedGoods = NULL;
	m_idxStartSlot = 0;
	m_pScrollList->SetCurValue(0.0f);

	OnSelectSlot(0, TRUE);
}


void	CTabItem::_UpdateSlotList(void)
{
	for(UINT32 i = 0; i < MAX_SLOT_FOR_SHOP_ITEM; i++)
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
	GameGUI::Reset(m_pScrollList, GameGUI::CalcuScrollCount(m_RefGoodsList.GetCount(), SLOT_HORI_COUNT_ITEM));

	CShop* shop = g_pGameContext->GetShop();
	char szString[256] = "";

	//	상품을 화면에 출력한다.
	for(INT32 i=0; i<MAX_SLOT_FOR_SHOP_ITEM; i++)
	{
		if( (i + m_idxStartSlot) >= m_RefGoodsList.GetCount() || (i + m_idxStartSlot) < 0)
			continue;

		SHOP_GOODS_PACK * pGoodsPack = (SHOP_GOODS_PACK*) m_RefGoodsList.GetItem(i + m_idxStartSlot);
		if( pGoodsPack == NULL)
			continue;

		SHOP_GOODS * pGoods = pGoodsPack->GetGoods(0);
		if( pGoods == NULL)
			continue;

		UINT8 itemType = 0;
		UINT32 itemArg = 0;
		UINT32 itemTerm = 0;
		BOOL bUsed = FALSE;

		INT32 itemID		= pGoods->_ItemID;
		INT32 itemUsage		= GET_ITEM_FLAG_TYPE(itemID);

		// 관심 아이템 체크
		SetItemSaleMark( m_Slot[i]._pSaleMark, pGoods->_SaleMark);				

		//	이미지
		g_pFramework->SetCashItemShape(m_Slot[i]._pImage, itemID);

		//	선택 슬롯
		if (m_pFocusedGoods == pGoodsPack)
		{
			m_Slot[i]._pSlot->SetShapeEnable(SLOT_SHAPE_INDEX_SELECTED, TRUE);
		}

		// 슬록 활성화
		m_Slot[i]._pSlot->SetShapeEnable(SLOT_SHAPE_INDEX_PUSHED, FALSE);
		m_Slot[i]._pSlot->setInputDisable(FALSE);
		m_Slot[i]._pSlot->SetEnable(TRUE);	

#ifndef NC_BUILD
		bool noName = false;
#endif

		// 이름
		char Name[MAX_STRING_COUNT] = {};

		if( g_pShopItemInfoDataBase->getInfo(itemID))
		{
			g_pShopItemInfoDataBase->GetItemName(itemID, Name, MAX_STRING_COUNT);
		}
		else
		{
			i3String::Format(Name, MAX_STRING_COUNT, "INVALID\nGoods:%d\nItem:%d", 
				pGoods->_GoodsID, pGoods->_ItemID);

#ifndef NC_BUILD
			noName = true;
#endif
		}

		if( i3String::Length(Name) == 0)
		{
			i3String::Format(Name, MAX_STRING_COUNT, "NO NAME\nGoods:%d\nItem:%d", 
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
				INT32 number	= GET_ITEM_FLAG_CLASS(itemID);
				i3String::Format(Name, sizeof(Name), 
					"%s\nGoods:%d\nItem:%d\nUsage:%d\nNum:%d", Name,
					pGoods->_GoodsID, pGoods->_ItemID, itemUsage, number);
			}
			m_Slot[i]._pValue->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP );
			m_Slot[i]._pSaleMark->SetEnable( FALSE);
			m_Slot[i]._pImage->SetEnable( FALSE );
		}
#endif

		m_Slot[i]._pValue->SetText(Name);
		m_Slot[i]._pValue->SetEnable(TRUE);

		// 보유 여부
		switch(itemUsage)
		{
		case ITEM_TYPE_MAINTENANCE:
			{
				// 지속성 일경우 소유한 같은 계열 아이템의 날짜를 합한다. (보유기간 합 = (1일 * 1일권 갯수) + (7일 * 7일권 갯수) + (30일 * 30일권 갯수))
				for(INT32 i = 0; i < pGoodsPack->GetGoodsCount(); i++)
				{
					SHOP_GOODS * pGoods = pGoodsPack->GetGoods(i);

					if( g_pFramework->GetCashItemTypeByID(pGoods->_ItemID, &itemType, &itemArg))
					{
						INT32 Day = GET_ITEM_FLAG_NUMBER(pGoods->_ItemID);
						//	갯수에 따른 사용기간 누적
						itemTerm += Day * itemArg;
					}
				}

				// 자기가 현재 사용중인 아이템의 현재 남은 기간도 포함해야 됩니다.
				INT32 ItemKeyID = GetCashItemByGroup(itemID);
				if( ItemKeyID != 0)
				{
					if( g_pFramework->GetCashItemTypeByID(ItemKeyID, &itemType, &itemArg))
					{
						//	현재 사용중인 남은 기간 누적
						UINT32 term = 0;
						if( itemArg > 0)
						{
							if( ITEM_ATTR_TYPE_BUY == itemType)
							{
								term = itemArg; 
							}
							else
							{
								term = g_pGameContext->GetInventorySecTime(itemArg); 
							}
							

							if( term > 0)
							{
								//	김성백 실장님 부탁으로 코드 추가합니다. 기간 보정 - komet
								if( term >= ITEM_TERM_ADJUST_VALUE)
									term -= ITEM_TERM_ADJUST_VALUE;

								term = ConvertSecToUseHour(term);
								
								itemTerm = (itemTerm *24) + term;
							}
						}

						FormatHaveItemTermText(szString, 256, itemTerm);
					}
					else
					{
						//	보유중 : %d일
						sprintf_s(szString, "%s : {col:255,255,255,255}%d%s{/col}", 
											GAME_STRING("STBL_IDX_ITEM_HAVE_TERM"), itemTerm, GAME_STRING("STBL_IDX_ITEM_TERM_UNIT"));
					}
				}
				else
				{
					if(itemTerm > 0)
					{
						//	보유중 : %d일
						sprintf_s(szString, "%s : {col:255,255,255,255}%d%s{/col}", 
											GAME_STRING("STBL_IDX_ITEM_HAVE_TERM"), itemTerm, GAME_STRING("STBL_IDX_ITEM_TERM_UNIT"));
					}
				}

				if(itemTerm > 0)	bUsed = TRUE;
			}
			break;
		case ITEM_TYPE_EXPENDABLE:
			{
				if( g_pFramework->GetCashItemTypeByID(itemID, &itemType, &itemArg))
				{
					itemTerm = itemArg;	//	아이템 갯수
				}

				if(itemTerm > 0)
				{
					sprintf_s(szString, "%s : {col:255,255,255,255}%d%s{/col}", 
											GAME_STRING("STBL_IDX_ITEM_HAVE_COUNT"), itemTerm, GAME_STRING("STBL_IDX_ITEM_COUNT_UNIT"));

					bUsed = TRUE;
				}
			}
			break;
		}

		if(bUsed)
		{
			m_Slot[i]._pTerm->SetTextColor(GetColor(GCT_DEFAULT));
		}
		else
		{
			//	미보유
			i3String::Copy( szString, GAME_STRING("STBL_IDX_ITEM_NO_HAVE"), 256 );

			m_Slot[i]._pTerm->SetTextColor( GetColor(GCT_LIGHT_GREEN));
		}

		m_Slot[i]._pTerm->SetText(szString);
		m_Slot[i]._pTerm->SetEnable(TRUE);

		UINT32 BuyType = (UINT32)shop->GetGoodsPrice(pGoods->_GoodsID, NULL, NULL);
		m_pOwner->_PrintSaleText(m_Slot[i]._pSaleText, BuyType);
	}
}


void	CTabItem::_UpdateDesc(void)
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


	INT32 ItemID	= pGoods->_ItemID;


	//	이미지
	if( g_pFramework->SetCashItemShape(m_pInfoDummy, ItemID))
	{
		m_pInfo->SetShapeEnable(GUI_SHAPE_OFFSET + 3, FALSE);	
	}

	ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(ItemID);
	if( pInfo)
	{
		char Name[MAX_STRING_COUNT] = {};
		g_pShopItemInfoDataBase->GetItemName(ItemID, Name, MAX_STRING_COUNT);

		m_pInfoValue->SetText(Name);
		m_pInfoDesc->SetText(pInfo->GetDescription());
	}
	else
	{
		char Name[MAX_STRING_COUNT] = "";
		i3String::Format(Name, MAX_STRING_COUNT, "%s\nGoods:%d\nItem:%d", INVALID_TEXT, pGoods->_GoodsID, pGoods->_ItemID);
		m_pInfoValue->SetText(Name);
		m_pInfoDesc->SetText(Name);
	}

	m_pInfoValue->SetEnable(TRUE);
	m_pInfoDesc->SetEnable(TRUE);
}


void	CTabItem::MakeGroupList()
{
	//	필터를 거친 상품만 리스트에 넣는다.
	m_RefGoodsList.Clear();

	CShopContext * pShopContext = g_pFramework->GetShopContext();
	INT32 goodsPackCount = pShopContext->GetGoodsPackCount(SHOP_CASH_ITEM_GOODS);

	for(INT32 i=0; i<goodsPackCount; i++)
	{
		SHOP_GOODS_PACK * pGoodsPack = pShopContext->GetGoods(SHOP_CASH_ITEM_GOODS, i);
		if( pGoodsPack == NULL)
			continue;

		SHOP_GOODS * pGoods = pGoodsPack->GetGoods(0);
		if( pGoods == NULL)
			continue;

		INT32 usage			= GET_ITEM_FLAG_TYPE(pGoods->_ItemID);

		//	class
		switch(m_nCashItemUsageType)
		{
		case ITEM_TYPE_MAINTENANCE:
			{
				if( usage != ITEM_TYPE_MAINTENANCE)
					continue;
			}
			break;
		case ITEM_TYPE_EXPENDABLE:
			{
				 if(usage != ITEM_TYPE_EXPENDABLE)
					 continue;
			}
			break;
		case ITEM_TYPE_UNKNOWN:
		default:
			break;
		}

		m_RefGoodsList.Add((void*) pGoodsPack);
	}
}


i3GuiButton *	CTabItem::GetGroupButton(ITEM_TYPE GroupUsage)
{
	i3GuiButton* pButton = NULL;

	switch(GroupUsage)
	{
	case ITEM_TYPE_UNKNOWN:			pButton = m_pAllGroup;			break;
	case ITEM_TYPE_EXPENDABLE:	pButton = m_pExpendableGroup;	break;
	case ITEM_TYPE_MAINTENANCE:	pButton = m_pMaintenanceGroup;	break;
	}

	return pButton;
}


