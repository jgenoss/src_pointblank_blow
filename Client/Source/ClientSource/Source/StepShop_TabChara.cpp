#include "pch.h"
#include "StepShop_TabChara.h"
#include "StepShop.h"
#include "GuiNotifyReceiver.h"




CTabChara::CTabChara(CStepShop * pOwner)
: CTabBase(MAX_SLOT_FOR_SHOP_CHARACTER, SLOT_HORI_COUNT_CHARA, pOwner)
{
	i3mem::FillZero(m_Slot, sizeof(m_Slot));
	m_pAllGroup		= NULL;
	m_pTypeGroup	= NULL;
	m_pHeadGroup	= NULL;
	
	m_pDescription	= NULL;
	m_pInfo			= NULL;
	m_pInfoDummy	= NULL;
	m_pInfoValue	= NULL;
	m_pTermKey		= NULL;
	m_pTermValue	= NULL;
	m_pTypeCombo	= NULL;
	m_pTypeButton	= NULL;
	m_pTypePopup	= NULL;

	m_nClassType	= (CHARACTER_CLASS_TYPE)-2;
	//// 1.5 Check CharaTeam
	//m_nFilter		= CHARACTER_TEAM_RB;

}


CTabChara::~CTabChara()
{
}


void	CTabChara::Init()
{
	CTabBase::Init();

	char temp[MAX_STRING_COUNT] = "";

	m_pSect->SetEnable(FALSE);

	INIT_CAPTION_BUTTON(m_pAllGroup, GAME_STRING("STBL_IDX_SHOP_FILTER_ALL"));
	INIT_CAPTION_BUTTON(m_pTypeGroup, GAME_STRING("STBL_IDX_SHOP_FILTER_CHARA"));
	INIT_CAPTION_BUTTON(m_pHeadGroup, GAME_STRING("STBL_IDX_SHOP_FILTER_HEAD"));
	
	m_pAllGroup->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	m_pScrollList->CreateTextEx(SLOT_VERT_COUNT_CHARA,  GetDefaultFontName());
	m_pScrollList->SetSelColor(0, 0, 0, 0);
	
	m_pInfo->SetAllShapeEnable(FALSE);
	m_pInfo->SetShapeEnable(GUI_SHAPE_OFFSET, TRUE);
	m_pInfoValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTermKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTermValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pInfoValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pTermKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pTermValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pTermKey->SetText(GAME_STRING("STBL_IDX_SHOP_USE_TERM"));	

	m_pDescription->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_1024, GAME_FONT_DEFAULT_SIZE, FALSE, NULL);
	m_pDescription->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pDescription->SetTextColor(GetColor(GCT_DEFAULT));	
	//m_pDescription->SetTextSpace(0, 2);
	m_pDescription->SetTextAutoWrap(TRUE);	
	
	INIT_CAPTION_BUTTON(m_PriceFrame.pBuy, GAME_STRING("STBL_IDX_SHOP_BUY"));
	INIT_CAPTION_BUTTON(m_PriceFrame.pGift, GAME_STRING("STBL_IDX_SHOP_GIFT"))

	m_pTypeCombo->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTypeCombo->SetTextColor(GetColor(GCT_DEFAULT));
	m_pTypeCombo->SetText(GAME_STRING("STR_SHOP_VIEW_TOTAL"));/*전체보기*/
	INIT_COMBO_EX(m_pTypeButton);

	//// 1.5 Check CharaTeam
	//m_nFilter = CHARACTER_TEAM_RB;
	
	m_pTypePopup->CreateTextEx(3,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pTypePopup->SetSelColor(100, 100, 100, 100);
	m_pTypePopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pTypePopup->SetExclusiveEnableControl(TRUE);
	m_pTypePopup->AddElement(GAME_STRING("STBL_IDX_SHOP_FILTER_EVERYTHING"));
	m_pTypePopup->AddElement(GAME_STRING("STBL_IDX_SHOP_FILTER_TYPE_RED"));
	m_pTypePopup->AddElement(GAME_STRING("STBL_IDX_SHOP_FILTER_TYPE_BLUE"));
	m_pTypePopup->SetEnable(FALSE);

	for(UINT32 i = 0; i < MAX_SLOT_FOR_SHOP_CHARACTER; i++)
	{
		INIT_BUTTON_EX(m_Slot[i]._pSlot);
		
		m_Slot[i]._pValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_Slot[i]._pValue->SetTextColor(GetColor(GCT_DEFAULT));
		m_Slot[i]._pValue->SetTextSpace(0, -3);
		m_Slot[i]._pUsed->SetEnable(FALSE);

		m_Slot[i]._pTerm->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_Slot[i]._pTerm->SetTextColor(GetColor(GCT_DEFAULT));	

		m_Slot[i]._pImage->setInputDisable(TRUE);
		m_Slot[i]._pValue->setInputDisable(TRUE);
		m_Slot[i]._pUsed->setInputDisable(TRUE);
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

	// Character Goods frame
	{
		m_PriceFrame.pPointInfoStatic->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_PriceFrame.pPointInfoStatic->SetTextColor(GetColor(GCT_DEFAULT));
		m_PriceFrame.pHavePointStatic->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
		m_PriceFrame.pHavePointStatic->SetTextColor(GetColor(GCT_DEFAULT));
		m_PriceFrame.pRemainPointStatic->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
		m_PriceFrame.pRemainPointStatic->SetTextColor(GetColor(GCT_DEFAULT));
		m_PriceFrame.pHavePointValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
		m_PriceFrame.pHavePointValue->SetTextColor(GetColor(GCT_DEFAULT));
		m_PriceFrame.pRemainPointValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
		m_PriceFrame.pRemainPointValue->SetTextColor(GetColor(GCT_DEFAULT));

		m_PriceFrame.pPointInfoStatic->SetText(GAME_STRING("STBL_IDX_SHOP_INFO_POINT"));
		sprintf_s(temp, "%s :", GAME_STRING("STBL_IDX_SHOP_HAVE_MONEY"));
		m_PriceFrame.pHavePointStatic->SetText(temp);
		sprintf_s(temp, "%s :", GAME_STRING("STBL_IDX_SHOP_REMAIN_MONEY"));
		m_PriceFrame.pRemainPointStatic->SetText(temp);
		m_PriceFrame.pHavePointValue->SetText("0");
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


BOOL	CTabChara::OnOpen()
{
	CTabBase::OnOpen();

	m_nClassType	= (CHARACTER_CLASS_TYPE)-2;
	m_pTypePopup->SetSelectedElement(0, FALSE);

	OnCharacterTypePopup(0);
	_ChangeCharacterClassType(CHARACTER_CLASS_ALL);
	return TRUE;
}


void	CTabChara::OnClose()
{
	CTabBase::OnClose();

	i3GuiButton* pButton = GetGroupButton(m_nClassType);
	if ( pButton )
		pButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);
}


void	CTabChara::OnUpdate(RT_REAL32 rDeltaSec)
{
	_UpdateSlotList();
	_UpdateDesc();
	_UpdateGoodsFrame(rDeltaSec);
}


void	CTabChara::OnFocus(UINT32 idxItem)
{
	if(MAX_SLOT_FOR_SHOP_CHARACTER <= idxItem)
		return;

	if (FALSE == m_Slot[idxItem]._pSlot->GetShapeEnable(SLOT_SHAPE_INDEX_PUSHED))
	{
		m_Slot[idxItem]._pSlot->SetShapeEnable(SLOT_SHAPE_INDEX_NORMAL, FALSE);
		m_Slot[idxItem]._pSlot->SetShapeEnable(SLOT_SHAPE_INDEX_HOVER, TRUE);
	}
}


void	CTabChara::OnPreGuiNotify()
{
}


void	CTabChara::OnLinkControl(i3BinList * plist)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	//	Character
	LinkControlEx(plist, "shopCharaTab", m_pTab, GCI_SHOP_B_CHARACTER_TAB);
	LinkControlEx(plist, "shopCharacter", m_pSect, -1);
	LinkControlEx(plist, "shopCharacterScrollList", m_pScrollList, GCI_SHOP_L_CHARACTER_SCROLL);

	LinkControlEx(plist, "shopAllCharacter", m_pAllGroup, GCI_SHOP_B_ALL_CHARACTER);
	LinkControlEx(plist, "shopTypeCharacter", m_pTypeGroup, GCI_SHOP_B_Type);
	LinkControlEx(plist, "shopHairCharacter", m_pHeadGroup, GCI_SHOP_B_HAIR);

	
	for(UINT32 i = 0; i < MAX_SLOT_FOR_SHOP_CHARACTER; i++)
	{
		char strName[MAX_STRING_COUNT];

		sprintf_s(strName, "shopCharacterSlot%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pSlot, GCI_SHOP_B_CHARACTER_SLOT_1 + i);

		sprintf_s(strName, "shopCharacterDummy%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pImage, -1);

		sprintf_s(strName, "shopCharacterValue%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pValue, -1);

		sprintf_s(strName, "shopCharacterTerm%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pTerm, -1);

		sprintf_s(strName, "shopCharacterUsed%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pUsed, -1);

		sprintf_s(strName, "shopCharacterSaleMark%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pSaleMark, -1);

		sprintf_s(strName, "shopCharaFirstSaleText%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pSaleText[SALE_TEXT_FIRST], -1);

		sprintf_s(strName, "shopCharaSecondSaleText%d", i + 1);
		LinkControlEx(plist, strName, m_Slot[i]._pSaleText[SALE_TEXT_SECOND], -1);
	}

	LinkControlEx(plist, "shopCharacterInfo", m_pInfo, -1);
	LinkControlEx(plist, "shopCharacterInfoDummy", m_pInfoDummy, -1);
	LinkControlEx(plist, "shopCharacterDescription", m_pDescription, -1);
	LinkControlEx(plist, "shopCharacterInfoValue", m_pInfoValue, -1);
	LinkControlEx(plist, "shopCharacterTermKey", m_pTermKey, -1);
	LinkControlEx(plist, "shopCharacterTermValue", m_pTermValue, -1);
	LinkControlEx(plist, "shopCharacterGoodsBuy", m_PriceFrame.pBuy, GCI_SHOP_B_CHARACTER_BUY);
	LinkControlEx(plist, "shopCharacterGoodsGift", m_PriceFrame.pGift, GCI_SHOP_B_CHARACTER_GIFT);
	LinkControlEx(plist, "shopCharacterTypeCombo", m_pTypeCombo, -1);
	LinkControlEx(plist, "shopCharacterTypeButton", m_pTypeButton, GCI_SHOP_B_CHARACTER_TYPE_BUTTON);
	LinkControlEx(plist, "shopCharacterTypePopup", m_pTypePopup, GCI_SHOP_L_CHARACTER_TYPE_POPUP);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	//	Goods frame - komet
	{
		char szTemp[256] = "";
		LinkControlEx(plist, "shopCharacterPointInfoStatic", m_PriceFrame.pPointInfoStatic, -1);

		for( INT32 i=0; i<GOODS_MAX_IN_PACK; i++)
		{
			sprintf_s(szTemp, "shopCharacterGoodsRadio0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pGoodsRadio[i], GCI_SHOP_R_CHARACTER_GOODS_RADIO_1 + i);

			//	사용 기간
			sprintf_s(szTemp, "shopCharacterGoodsDayStatic0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pUseDayStatic[i], -1);

			sprintf_s(szTemp, "shopCharacterGoodsDayValue0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pUseDayValue[i], GCI_SHOP_S_CHARACTER_USE_DAY_1 + i);

			//	구입 포인트
			sprintf_s(szTemp, "shopCharacterGoodsPointValue0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pBuyPointValue[i], GCI_SHOP_S_CHARACTER_BUY_POINT_1 + i);

			// 화폐단위
			sprintf_s(szTemp, "shopCharacterGoodsPointStatic0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pBuyPointStatic[i], -1);

			// 한국 N코인 심볼 이미지
			sprintf_s(szTemp, "shopCharaGoodsCashSymbol0%d", i+1);
			LinkControlEx(plist, szTemp, m_PriceFrame.pCashSymbol[i], -1);
		}

		LinkControlEx(plist, "shopCharacterGoodsHavePointStatic", m_PriceFrame.pHavePointStatic, -1);
		LinkControlEx(plist, "shopCharacterGoodsHavePointValue", m_PriceFrame.pHavePointValue, GCI_SHOP_S_CHARACTER_HAVE_POINT);
		LinkControlEx(plist, "shopCharaGoodsHaveCashSymbol", m_PriceFrame.pHaveCashSymbol, -1);
		LinkControlEx(plist, "shopCharacterGoodsPointStatic05", m_PriceFrame.pHavePointUnitStatic, -1);
		LinkControlEx(plist, "shopCharacterGoodsRemainPointStatic", m_PriceFrame.pRemainPointStatic, -1);
		LinkControlEx(plist, "shopCharacterGoodsRemainPointValue", m_PriceFrame.pRemainPointValue, GCI_SHOP_S_CHARACTER_REMAIN_POINT);
		LinkControlEx(plist, "shopCharaGoodsRemainCashSymbol", m_PriceFrame.pRemainCashSymbol, -1);
		LinkControlEx(plist, "shopCharacterGoodsPointStatic06", m_PriceFrame.pRemainPointUnitStatic, -1);
	}

	CTabBase::OnClose();
}


void	CTabChara::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
	case GCI_SHOP_R_CHARACTER_GOODS_RADIO_1:
	case GCI_SHOP_R_CHARACTER_GOODS_RADIO_2:
	case GCI_SHOP_R_CHARACTER_GOODS_RADIO_3:
	case GCI_SHOP_R_CHARACTER_GOODS_RADIO_4:
		if( I3GUI_RADIO_NOTIFY_CLICKED == pNotify->m_nEvent)
			OnSelectGoodsRadio(pNotify->m_nID - GCI_SHOP_R_CHARACTER_GOODS_RADIO_1);
		break;

	case GCI_SHOP_B_ALL_CHARACTER:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			_ChangeCharacterClassType(CHARACTER_CLASS_ALL);
		break;

	case GCI_SHOP_B_Type:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			_ChangeCharacterClassType(CHARACTER_CLASS_CHARA_TYPE);
		break;

	case GCI_SHOP_B_HAIR:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			_ChangeCharacterClassType(CHARACTER_CLASS_HEADGEAR);
		break;

	case GCI_SHOP_B_DINO:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			_ChangeCharacterClassType(CHARACTER_CLASS_CHARA_DINO);
		break;

	case GCI_SHOP_L_CHARACTER_SCROLL:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent)
			OnChangeScroll();
		break;

	case GCI_SHOP_B_CHARACTER_SLOT_1:	// fall through
	case GCI_SHOP_B_CHARACTER_SLOT_2:	// fall through
	case GCI_SHOP_B_CHARACTER_SLOT_3:	// fall through
	case GCI_SHOP_B_CHARACTER_SLOT_4:	// fall through
	case GCI_SHOP_B_CHARACTER_SLOT_5:	// fall through
	case GCI_SHOP_B_CHARACTER_SLOT_6:	// fall through	
		if (I3GUI_STATIC_NOTIFY_ONMOUSE == pNotify->m_nEvent)
			OnFocus(pNotify->m_nID - GCI_SHOP_B_CHARACTER_SLOT_1);
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent)
			OnSelectSlot(pNotify->m_nID - GCI_SHOP_B_CHARACTER_SLOT_1);
		if (I3GUI_STATIC_NOTIFY_DOUBLECLICK == pNotify->m_nEvent)
			m_pOwner->OnGoodsBuy(pNotify->m_nID - GCI_SHOP_B_CHARACTER_SLOT_1, TRUE);
		break;

	case GCI_SHOP_B_CHARACTER_BUY:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			m_pOwner->OnGoodsBuy(pNotify->m_nID - GCI_SHOP_B_CHARACTER_SLOT_1);
		break;

	case GCI_SHOP_B_CHARACTER_GIFT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			m_pOwner->OnGoodsGift(pNotify->m_nID - GCI_SHOP_B_CHARACTER_SLOT_1);
		break;

	case GCI_SHOP_B_CHARACTER_TYPE_BUTTON:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			OnCharacterTypeButton();
		break;

	case GCI_SHOP_L_CHARACTER_TYPE_POPUP:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent)
			OnCharacterTypePopup(pNotify->m_nPriParam);
		break;
	}
}


void	CTabChara::OnCharacterTypePopup(UINT32 idxItem)
{
	//// 1.5 Check CharaTeam
	//switch( idxItem)
	//{
	//case 0:		m_nFilter = CHARACTER_TEAM_RB;			break;
	//case 1:		m_nFilter = CHARACTER_TEAM_RED;			break;
	//case 2:		m_nFilter = CHARACTER_TEAM_BLUE;		break;
	//default:	m_nFilter = CHARACTER_TEAM_RB;			break;
	//}

	const char * pszText = m_pTypePopup->GetText(idxItem);
	m_pTypeCombo->SetText(pszText);

	m_pFocusedGoods = NULL;
	m_idxStartSlot = 0;
	m_pScrollList->SetCurValue(0.0f);

	MakeGroupList();
}


void	CTabChara::_UpdateSlotList(void)
{
	for(UINT32 i = 0; i < MAX_SLOT_FOR_SHOP_CHARACTER ; i++)
	{
		m_Slot[i]._pSlot->SetShapeEnable(SLOT_SHAPE_INDEX_SELECTED, FALSE);
		m_Slot[i]._pSlot->setInputDisable(TRUE);
		m_Slot[i]._pSlot->SetEnable(FALSE);

		m_Slot[i]._pImage->SetEnable(FALSE);
		m_Slot[i]._pValue->SetEnable(FALSE);
		m_Slot[i]._pUsed->SetEnable(FALSE);

		m_Slot[i]._pSaleMark->SetEnable( FALSE);
		m_Slot[i]._pSaleMark->SetAllShapeEnable( FALSE);
	}


	GameGUI::Reset(m_pScrollList, GameGUI::CalcuScrollCount(m_RefGoodsList.GetCount(), SLOT_HORI_COUNT_CHARA));

	CShop* shop = g_pGameContext->GetShop();
	char szString[MAX_STRING_COUNT] = "";

	//	상품을 화면에 출력한다.
	for(INT32 i=0; i<MAX_SLOT_FOR_SHOP_CHARACTER; i++)
	{
		if( i + m_idxStartSlot >= m_RefGoodsList.GetCount() || (i + m_idxStartSlot) < 0)
			continue;

		SHOP_GOODS_PACK * pGoodsPack = (SHOP_GOODS_PACK*) m_RefGoodsList.GetItem(i + m_idxStartSlot);
		if( pGoodsPack == NULL)
			continue;

		SHOP_GOODS * pGoods = pGoodsPack->GetGoods(0);
		if( pGoods == NULL)
			continue;

		INT32 CharaID = pGoods->_ItemID;

		// 관심 아이템 체크
		SetItemSaleMark( m_Slot[i]._pSaleMark, pGoods->_SaleMark);	

		//	상품 이미지 셋팅
		g_pFramework->SetCharacterShape(m_Slot[i]._pImage, CharaID);

		//	선택 슬롯
		if (m_pFocusedGoods == pGoodsPack)
		{
			m_Slot[i]._pSlot->SetShapeEnable(SLOT_SHAPE_INDEX_SELECTED, TRUE);
		}

		// 슬록 활성화
		m_Slot[i]._pSlot->SetShapeEnable(SLOT_SHAPE_INDEX_PUSHED, FALSE);
		m_Slot[i]._pSlot->setInputDisable(FALSE);
		m_Slot[i]._pSlot->SetEnable(TRUE);

		UINT8 Type = 0xFF;
		UINT32 Arg = 0xFFFFFFFF;
		// 장비 여부
		if (g_pFramework->GetCharacterType(CharaID, &Type, &Arg))
		{
			m_Slot[i]._pTerm->SetTextColor(GetColor(GCT_DEFAULT));

			BOOL Expendable = (pGoods->_AuthType == AUTH_COUNT) ? TRUE : FALSE;
			m_pOwner->_GetGoodsUseDay(Expendable, Type, Arg, szString, MAX_STRING_COUNT);
		}
		else
		{
			//	미보유
			i3String::Copy( szString, GAME_STRING("STBL_IDX_ITEM_NO_HAVE"), 256 );

			m_Slot[i]._pTerm->SetTextColor( GetColor(GCT_LIGHT_GREEN));
		}

		m_Slot[i]._pTerm->SetText(szString);
		m_Slot[i]._pTerm->SetEnable(TRUE);

#ifndef NC_BUILD
		bool noName = false;
#endif

		// 이름	
		const CEquipInfo * pEquipInfo = g_pCharaInfoDataBase->GetInfo(CharaID);
		if( pEquipInfo)
		{
			g_pCharaInfoDataBase->GetCharacterName(CharaID, szString, sizeof(szString));
		}		
		else
		{
			i3String::Format(szString, MAX_STRING_COUNT, "INVALID\nGoods:%d\nItem:%d", 
				pGoods->_GoodsID, pGoods->_ItemID);
#ifndef NC_BUILD
			noName = true;
#endif
		}

		if( i3String::Length(szString) == 0 && pEquipInfo)
		{
			i3String::Format(szString, MAX_STRING_COUNT, "NO NAME\nGoods:%d\nItem:%d\nCharaType:%d\nNum:%d", 
				pGoods->_GoodsID, pGoods->_ItemID, pEquipInfo->getType(), ((CCharaInfo*)pEquipInfo)->GetIDNumber() );
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
				i3String::Format(szString, sizeof(szString), 
					"%s\nGoods:%d\nItem:%d\nCharaType:%d\nNum:%d", szString,
					pGoods->_GoodsID, pGoods->_ItemID, pEquipInfo->getType(), ((CCharaInfo*)pEquipInfo)->GetIDNumber());
			}
			m_Slot[i]._pValue->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP );
			m_Slot[i]._pSaleMark->SetEnable( FALSE);
			m_Slot[i]._pImage->SetEnable( FALSE );
		}
#endif

		m_Slot[i]._pValue->SetText(szString);
		m_Slot[i]._pValue->SetEnable(TRUE);

		UINT32 BuyType = (UINT32)shop->GetGoodsPrice(pGoods->_GoodsID, NULL, NULL);
		m_pOwner->_PrintSaleText(m_Slot[i]._pSaleText, BuyType);
	}
}

void	CTabChara::_UpdateDesc(void)
{
	// 초기화
	m_pInfo->SetShapeEnable(GUI_SHAPE_OFFSET + 3, FALSE);
	m_pInfo->SetEnable(TRUE);
	m_pInfoDummy->SetEnable(FALSE);
	m_pInfoValue->SetEnable(FALSE);
	m_pTermKey->SetEnable(FALSE);
	m_pTermValue->SetEnable(FALSE);
	m_pDescription->SetEnable(FALSE);
	//m_pDetail->SetEnable(FALSE);
	//m_pEquip->SetEnable(FALSE);

	if (NULL == m_pFocusedGoods)
		return;

	SHOP_GOODS * pGoods = m_pFocusedGoods->GetGoods(0);
	if( pGoods == NULL)
		return;

	INT32 CharaID = pGoods->_ItemID;


	// 이미지
	if (g_pFramework->SetCharacterShape(m_pInfoDummy, CharaID))
	{
		m_pInfo->SetShapeEnable(GUI_SHAPE_OFFSET + 3, FALSE);
		m_pInfoDummy->SetEnable(TRUE);
	}

	// 이름
	char szString[2048] = "";
	g_pCharaInfoDataBase->GetCharacterName(CharaID, szString, sizeof(szString));
	m_pInfoValue->SetText(szString);
	m_pInfoValue->SetEnable(TRUE);


	BOOL Rv = g_pCharaInfoDataBase->GetDescription(CharaID, szString, 2048);
	if ( Rv )
	{
		m_pDescription->SetText(szString);
		m_pDescription->SetEnable(TRUE);
	}

	m_pTermKey->SetEnable(FALSE);

}


void	CTabChara::OnCharacterTypeButton(void)
{
	m_pOwner->ExclusionPopup(m_pTypePopup);
}


void	CTabChara::_ChangeCharacterClassType(CHARACTER_CLASS_TYPE nClassType)
{
	if( m_nClassType == nClassType)
		return;

	i3GuiButton* pButton = NULL;

	pButton = GetGroupButton(m_nClassType);
	if ( pButton )
		pButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	pButton = GetGroupButton(nClassType);
	I3ASSERT( pButton != NULL );
	if ( ! pButton )
		return;
	pButton->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	m_nClassType = nClassType;
	MakeGroupList();

	m_pFocusedGoods = NULL;
	m_idxStartSlot = 0;
	m_pScrollList->SetCurValue(0.0f);

	OnSelectSlot(0, TRUE);
}


void	CTabChara::MakeGroupList()
{
	//	필터를 거친 상품만 리스트에 넣는다.
	m_RefGoodsList.Clear();

	CShopContext * pShopContext = g_pFramework->GetShopContext();
	INT32 goodsPackCount = pShopContext->GetGoodsPackCount(SHOP_CHARACTER_GOODS);

	for(INT32 i = 0 ; i < goodsPackCount ; i++)
	{
		SHOP_GOODS_PACK * pGoodsPack = pShopContext->GetGoods(SHOP_CHARACTER_GOODS, i);
		if( pGoodsPack == NULL)
			continue;

		SHOP_GOODS * pGoods = pGoodsPack->GetGoods(0);
		if( pGoods == NULL)
			continue;

		INT32 CharaClass = GET_ITEM_FLAG_TYPE(pGoods->_ItemID);
		//INT32 CharaID = pGoods->_ItemID;

		//	class
		switch(m_nClassType)
		{
		case CHARACTER_CLASS_CHARA_TYPE:
			{
				if( CharaClass != ITEM_TYPE_CHARA)
					continue;
			}
			break;
		case CHARACTER_CLASS_HEADGEAR:
		case CHARACTER_CLASS_FACEGEAR:
			{
				 if(CharaClass != ITEM_TYPE_HEADGEAR)
					 continue;
			}
			break;
		case CHARACTER_CLASS_CHARA_DINO:
			{
				if(CharaClass != ITEM_TYPE_CHARA)
					continue;
			}
			break;
		case CHARACTER_CLASS_ALL:
		default:
			break;
		}

		//// 1.5 Check CharaTeam
		//	team filter
		//switch(m_nFilter) 
		//{
		//case CHARACTER_TEAM_RED:
		//	{
		//		if( !(GET_ITEM_FLAG_CLASS(CharaID) & CHARACTER_TEAM_RED))
		//			continue;
		//	}
		//	break;
		//case CHARACTER_TEAM_BLUE:
		//	{
		//		if( !(GET_ITEM_FLAG_CLASS(CharaID) & CHARACTER_TEAM_BLUE))
		//			continue;
		//	}
		//	break;
		//case CHARACTER_TEAM_RB:
		//default:
		//	break;
		//}

		m_RefGoodsList.Add((void*) pGoodsPack);
	}

}


i3GuiButton *	CTabChara::GetGroupButton(CHARACTER_CLASS_TYPE GroupUsage)
{
	i3GuiButton* pButton = NULL;

	switch(GroupUsage)
	{
	case CHARACTER_CLASS_ALL		:	pButton = m_pAllGroup;		break;
	case CHARACTER_CLASS_CHARA_TYPE	:	pButton = m_pTypeGroup;		break;
	case CHARACTER_CLASS_HEADGEAR		:
	case CHARACTER_CLASS_FACEGEAR		:	pButton = m_pHeadGroup;		break;
	case CHARACTER_CLASS_ITEM		:								break;
	case CHARACTER_CLASS_EVENT		:								break;
	case CHARACTER_CLASS_CHARA_DINO	:	pButton = m_pTypeGroup;		break;
	}

	return pButton;
}


