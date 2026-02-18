#include "pch.h"
#include "PopupShopGift.h"
#include "GlobalVariables.h"
#include "CommunityContext.h"
#include "Shop.h"

#if defined (LOCALE_CHINA)
#include "../SDOALink/SDOALink.h"
//#include "CHttpFCMInfo.h"
#endif
I3_CLASS_INSTANCE(CPopupShopGift, CPopupBase);

CPopupShopGift::CPopupShopGift()
{
	i3mem::FillZero( &m_Goods, sizeof(SHOP_GOODS));

	m_pGui = NULL;
	m_pMainForm = NULL;
	m_pMainCaption = NULL;
	m_pButtonOk = NULL;
	m_pButtonCancel = NULL;

	m_pButtonCashOk = NULL;
	m_pButtonCashCancel = NULL;
	m_pButtonCashCharge = NULL;
	m_pButtonCashChargeMark = NULL;
	m_pButtonCashChargeStatic = NULL;

	m_pSaleText = NULL;

	i3mem::FillZero(m_pStaticPoint, sizeof(m_pStaticPoint));

	m_pStaticHavePoint = NULL;
	m_pStaticRemainPoint = NULL;
	m_pValueBuyPoint = NULL;
	m_pValueHavePoint = NULL;
	m_pValueRemainPoint = NULL;

	m_pReceiverInput = NULL;
	m_pComboFriend = NULL;
	m_pComboClan = NULL;

	m_pWeaponInfoFrame = NULL;
	m_pWeaponInfoValue = NULL;
	m_pWeaponInfoKey = NULL;
	m_pWeaponInfoImage = NULL;
	m_pWeaponInfoExtension = NULL;
	m_pWeaponInfoUsed = NULL;

	m_pCharacterInfoFrame = NULL;
	m_pCharacterInfoValue = NULL;
	m_pCharacterInfoImage = NULL;
	m_pCharacterInfoUsed = NULL;

	m_pItemInfoFrame = NULL;
	m_pItemInfoValue = NULL;
	m_pItemInfoImage = NULL;
	m_pItemInfoUsed = NULL;

	m_szReceiverNick[0] = 0;

	m_WaitAuthBuy	= FALSE;

	m_idxClanPopupStartSlot = 0;
	m_idxFriendPopupStartSlot = 0;
}

CPopupShopGift::~CPopupShopGift()
{
}

void CPopupShopGift::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{		
	switch(pNotify->m_nID)
	{
	case GCI_SHOP_B_POPUP_GIFT_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)_InputConfirm();
		break;
	case GCI_SHOP_B_POPUP_GIFT_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _InputCancel();
		break;
	case GCI_SHOP_B_POPUP_GIFT_CASH_CHARGE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _InputCashCharge();
		break;
	case GCI_SHOP_B_POPUP_GIFT_FRIEND:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _ComboFriend();
		break;
	case GCI_SHOP_B_POPUP_GIFT_CLAN:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _ComboClan();
		break;
	case GCI_SHOP_L_POPUP_GIFT_FRIEND_SCROLL:
		if (I3GUI_LIST_NOTIFY_CLICKED == pNotify->m_nEvent) _SelectFriend();
		break;
	case GCI_SHOP_L_POPUP_GIFT_CLAN_SCROLL:
		if (I3GUI_LIST_NOTIFY_CLICKED == pNotify->m_nEvent) _SelectClanMember();
		break;
	}
 	
	BOOL bPopupList = FALSE;
	i3GuiObjBase * pFocusControl = i3GuiRoot::getFocusControl();

	//	친구 팝업 리스트가 포커스를 읽으면 자동으로 비활성화 시킨다.
	if( m_pFriendDropDown->isEnable() && pFocusControl != m_pFriendDropDown)
	{
		m_pFriendDropDown->SetEnable( FALSE);
	}

	//  클랜원 팝업 리스트가 포커스를 읽으면 자동으로 비활성화 시킨다.
	if( m_pClanDropDown->isEnable() && pFocusControl != m_pClanDropDown)
	{
		m_pClanDropDown->SetEnable( FALSE);
	}

	//	팝업 리스트를 활성화되어 있을때는 OK / Cancel 입력을 막는다.
	if( m_pFriendDropDown->isEnable() || m_pClanDropDown->isEnable())
	{
		bPopupList = TRUE;
	}

	m_pButtonOk->setInputDisable(bPopupList);
	m_pButtonCancel->setInputDisable(bPopupList);
	m_pButtonCashOk->setInputDisable(bPopupList);
	m_pButtonCashCancel->setInputDisable(bPopupList);
	m_pButtonCashCharge->setInputDisable(bPopupList);
}

BOOL CPopupShopGift::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	// 구입 인증 대기시에는 키 입력을 막는다.
	if ( TRUE == m_WaitAuthBuy )
		if( code == I3_EVT_CODE_ACTIVATE && I3_EVT_INPUT == event)
			return TRUE;

	/*if( code == I3_EVT_CODE_ACTIVATE)
	{
		if (I3_EVT_INPUT == event)
		{
			if( g_pFramework->getMouse()->GetButtonState() & (I3I_MOUSE_LBUTTON))
			{
				if( !m_pFriendScroll->getIsFocus())
				{
					m_pFriendDropDown->SetEnable(FALSE);
				}

				if( !m_pClanScroll->getIsFocus())
				{
					m_pClanDropDown->SetEnable(FALSE);
				}
			}
		}
	}*/

	return CPopupBase::OnEvent(event, pObj, param2, code);
}

void CPopupShopGift::OnGameEvent( INT32 event, INT32 arg)
{
	//	이벤트 결과를 받아 처리한다.
	switch( event )
	{
	case EVENT_AUTH_GIFT_GOODS:		//	선물하기
		{
			if( EV_SUCCESSED( arg))
			{	
				char temp[MAX_STRING_COUNT] = "";
				sprintf_s( temp, GAME_STRING("STR_POPUP_MESSAGE_SUCCESS_GIFT"), m_szReceiverNick);/*[%s] 님에게\n선물이 정상적으로 발송되었습니다.*/

				CAPTION_POPUP(temp, GAME_STRING("STR_POPUP_CAPTION_SUCCESS_GIFT"));/*선물하기 성공*/
			}
			else
			{
				switch( arg)
				{
				case EVENT_ERROR_EVENT_GIFT_GOODS_NOT_ENOUGH_POINT:
					CAPTION_POPUP( GAME_STRING("STR_POPUP_MESSAGE_CANNOT_GIFT"), 
						GAME_STRING("STR_POPUP_CAPTION_FAILED_GIFT"));/*선물하기 실패*//*잔액이 부족하여 물품을 선물할 수 없습니다.*/
					break;
				case EVENT_ERROR_EVENT_GIFT_GOODS_WRONG_RECEIVE_USER :
					CAPTION_POPUP( GAME_STRING("STR_POPUP_MESSAGE_CANNOT_FIND_USER"), 
						GAME_STRING("STR_POPUP_CAPTION_FAILED_GIFT"));/*선물하기 실패*//*유저를 찾을 수 없습니다.*/
					break;
			#if defined(LOCALE_CHINA)
				case EVENT_ERROR_SNDA_SPEND:
					// 샨다 빌링 재확인하기
					//if (g_pSDOA_Link->OpenWindow(L"HTML", L"", g_strRedirectUrl, 
					//	0, 0, 480, 320, L"xul=WebBrowser.xml;sf=true;cp=true;ap=center") == SDOA_OK)
					//{
					//	 open the webpage successfully
					//}

					//if (g_pSDOA_Link->CloseWindow(L"Test") == SDOA_OK)
					//{
					//	// fail to close the webpage
					//}
					break;
			#endif
				case EVENT_ERROR_EVENT_GIFT_GOODS_PARAMETER_GOODS:
				case EVENT_ERROR_EVENT_GIFT_GOODS_PARAMETER_USER:
				case EVENT_ERROR_FAIL:
				default:
					CAPTION_POPUP( GAME_STRING("STR_POPUP_MESSAGE_FAILED_BY_NET"), 
						GAME_STRING("STR_POPUP_CAPTION_FAILED_GIFT"));/*네트워크에 문제로 실패했습니다.\n 다시 시도해 주십시오.*//*선물하기 실패*/
					break;
				}
			}
			this->OnClosePopup();	//	선물하기가 성공하면 팝업을 닫는다.
		}
		break;
	}
}

BOOL CPopupShopGift::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	char temp[256] = "";

	LinkControlEx(plist, "ShopGift", m_pGui, -1);
	LinkControlEx(plist, "shopGiftPopup", m_pMainForm, -1);
	LinkControlEx(plist, "shopGiftCaption", m_pMainCaption, -1);
	LinkControlEx(plist, "shopGiftOk", m_pButtonOk, GCI_SHOP_B_POPUP_GIFT_OK);
	LinkControlEx(plist, "shopGiftCancel", m_pButtonCancel, GCI_SHOP_B_POPUP_GIFT_CANCEL);

	LinkControlEx(plist, "shopGiftCashOk", m_pButtonCashOk, GCI_SHOP_B_POPUP_GIFT_OK);
	LinkControlEx(plist, "shopGiftCashCancel", m_pButtonCashCancel, GCI_SHOP_B_POPUP_GIFT_CANCEL);
	LinkControlEx(plist, "shopGiftCashCharge", m_pButtonCashCharge, GCI_SHOP_B_POPUP_GIFT_CASH_CHARGE);
	LinkControlEx(plist, "shopGiftCashChargeMark", m_pButtonCashChargeMark, -1);
	LinkControlEx(plist, "shopGiftCashChargeStatic", m_pButtonCashChargeStatic, -1);


	for(INT32 i=0; i<3; i++)
	{
		sprintf_s(temp, "shopGiftPointStatic%d", i+1);
		LinkControlEx(plist, temp, m_pStaticPoint[i], -1);
	}

	LinkControlEx(plist, "shopGiftHavePointStatic", m_pStaticHavePoint, -1);
	LinkControlEx(plist, "shopGiftRemainPointStatic", m_pStaticRemainPoint, -1);

	LinkControlEx(plist, "shopGiftUseDaySaleText", m_pSaleText, -1);

	LinkControlEx(plist, "shopGiftPointValue", m_pValueBuyPoint, -1);
	LinkControlEx(plist, "shopGiftHavePointValue", m_pValueHavePoint, -1);
	LinkControlEx(plist, "shopGiftRemainPointValue", m_pValueRemainPoint, -1);

	LinkControlEx(plist, "shopGiftReceiverNameStatic", m_pStaticReceiverName, -1);
	LinkControlEx(plist, "shopGiftReceiverSelectStatic", m_pStaticReceiverSelect, -1);
	LinkControlEx(plist, "shopGiftReceiverInput", m_pReceiverInput, -1);
	LinkControlEx(plist, "shopGiftFriend", m_pComboFriend, GCI_SHOP_B_POPUP_GIFT_FRIEND);
	LinkControlEx(plist, "shopGiftClan", m_pComboClan, GCI_SHOP_B_POPUP_GIFT_CLAN);

	LinkControlEx(plist, "shopGiftFriendScroll", m_pFriendScroll, GCI_SHOP_L_POPUP_GIFT_FRIEND_SCROLL);
	LinkControlEx(plist, "shopGiftFriendDropDown", m_pFriendDropDown, -1);

	LinkControlEx(plist, "shopGiftClanScroll", m_pClanScroll, GCI_SHOP_L_POPUP_GIFT_CLAN_SCROLL);
	LinkControlEx(plist, "shopGiftClanDropDown", m_pClanDropDown, -1);

	// 한국 N코인 심볼 이미지
	LinkControlEx(plist, "shopGiftCashSymbol", m_pCashSymbol, -1);
	LinkControlEx(plist, "shopGiftHaveCashSymbol", m_pHaveCashSymbol, -1);
	LinkControlEx(plist, "shopGiftRemainCashSymbol", m_pRemainCashSymbol, -1);
	
	//	weapon info
	{
		LinkControlEx(plist, "shopGiftWeaponInfo", m_pWeaponInfoFrame, -1);

		LinkControlEx(plist, "shopGiftWeaponValue", m_pWeaponInfoValue, -1);
		LinkControlEx(plist, "shopGiftWeaponKey", m_pWeaponInfoKey, -1);
		LinkControlEx(plist, "shopGiftWeaponDummy", m_pWeaponInfoImage, -1);
		LinkControlEx(plist, "shopGiftWeaponExtension", m_pWeaponInfoExtension, -1);
		LinkControlEx(plist, "shopGiftWeaponUsed", m_pWeaponInfoUsed, -1);
	}

	//	Character info
	{
		LinkControlEx(plist, "shopGiftCharacterInfo", m_pCharacterInfoFrame, -1);

		LinkControlEx(plist, "shopGiftCharacterValue", m_pCharacterInfoValue, -1);
		LinkControlEx(plist, "shopGiftCharacterDummy", m_pCharacterInfoImage, -1);
		LinkControlEx(plist, "shopGiftCharacterUsed", m_pCharacterInfoUsed, -1);
	}

	//	item info
	{
		LinkControlEx(plist, "shopGiftItemInfo", m_pItemInfoFrame, -1);

		LinkControlEx(plist, "shopGiftItemValue", m_pItemInfoValue, -1);
		LinkControlEx(plist, "shopGiftItemDummy", m_pItemInfoImage, -1);
		LinkControlEx(plist, "shopGiftItemUsed", m_pItemInfoUsed, -1);
	}

	return FALSE;
}

void CPopupShopGift::OnInitControl(void)
{
	char szTemp[256] = "";

	m_pGui->SetEnable(FALSE);

	INIT_WIN_CAPTION(m_pMainCaption, GAME_STRING("STBL_IDX_EP_SHOP_GIFT_CAPTION"));

	INIT_CAPTION_BUTTON(m_pButtonOk, GAME_STRING("STBL_IDX_BUTTON_OK"));
	INIT_CAPTION_BUTTON(m_pButtonCancel, GAME_STRING("STBL_IDX_BUTTON_CANCEL"));
	INIT_CAPTION_BUTTON(m_pComboFriend, GAME_STRING("STBL_IDX_SHOP_GIFT_TO_FRIEND"));
	INIT_CAPTION_BUTTON(m_pComboClan, GAME_STRING("STBL_IDX_SHOP_GIFT_TO_CLAN"));

	INIT_CAPTION_BUTTON(m_pButtonCashOk, GAME_STRING("STBL_IDX_BUTTON_OK"));
	INIT_CAPTION_BUTTON(m_pButtonCashCancel, GAME_STRING("STBL_IDX_BUTTON_CANCEL"));
	INIT_BUTTON_EX(m_pButtonCashCharge);
	INIT_VALUE_CAPTION2(m_pButtonCashChargeStatic, GAME_STRING("STR_POPUP_CHARGE_CASH"));/*톨 충전*/

	m_pButtonCashOk->SetEnable(FALSE);
	m_pButtonCashCancel->SetEnable(FALSE);
	m_pButtonCashCharge->SetEnable(FALSE);

	m_pButtonCashChargeMark->setInputDisable(TRUE);
	m_pButtonCashChargeStatic->setInputDisable(TRUE);

	//	"일"
	sprintf_s(szTemp, "%s :", GAME_STRING("STBL_IDX_SHOP_USE_DAY"));
	m_pSaleText->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSaleText->SetTextColor(GetColor(GCT_DEFAULT));


	//	"보유 Point"
	sprintf_s(szTemp, "%s :", GAME_STRING("STBL_IDX_SHOP_HAVE_MONEY"));
	m_pStaticHavePoint->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pStaticHavePoint->SetTextColor(GetColor(GCT_DEFAULT));
	m_pStaticHavePoint->SetText(szTemp);

	//	"잔액"
	sprintf_s(szTemp, "%s :", GAME_STRING("STBL_IDX_SHOP_REMAIN_MONEY"));
	m_pStaticRemainPoint->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pStaticRemainPoint->SetTextColor(GetColor(GCT_DEFAULT));
	m_pStaticRemainPoint->SetText(szTemp);

	//	받는사람
	m_pStaticReceiverName->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pStaticReceiverName->SetTextColor(GetColor(GCT_DEFAULT));
	m_pStaticReceiverName->SetText(GAME_STRING("STBL_IDX_SHOP_RECEIVER"));

	//	목록선택
	m_pStaticReceiverSelect->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pStaticReceiverSelect->SetTextColor(GetColor(GCT_DEFAULT));
	m_pStaticReceiverSelect->SetText(GAME_STRING("STR_POPUP_SELECT_ELEMENT"));/*목록선택*/

	//	닉네임 입력
	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = NICKNAME_CHAR_COUNT;
	ime._StyleIME = IME_STYLE_LIMIT_CHARACTER;

	m_pReceiverInput->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pReceiverInput->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pReceiverInput->SetTextColor(GetColor(GCT_DEFAULT));
	m_pReceiverInput->SetText("");

	for(INT32 i=0; i<3; i++)
	{
		m_pStaticPoint[i]->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pStaticPoint[i]->SetTextColor(GetColor(GCT_DEFAULT));
		m_pStaticPoint[i]->SetText(GAME_STRING("STBL_IDX_SHOP_MONEY"));
	}

	m_pValueBuyPoint->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pValueBuyPoint->SetTextColor(GetColor(GCT_DEFAULT));
	m_pValueBuyPoint->SetText("0");

	m_pValueHavePoint->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pValueHavePoint->SetTextColor(GetColor(GCT_DEFAULT));
	m_pValueHavePoint->SetText("0");

	m_pValueRemainPoint->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pValueRemainPoint->SetTextColor(GetColor(GCT_DEFAULT));
	m_pValueRemainPoint->SetText("0");

	m_pWeaponInfoFrame->SetEnable(FALSE);
	m_pWeaponInfoFrame->SetAllShapeEnable(FALSE);
	m_pWeaponInfoFrame->SetShapeEnable(GUI_SHAPE_OFFSET, TRUE);
	m_pWeaponInfoKey->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pWeaponInfoKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pWeaponInfoValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);	
	m_pWeaponInfoValue->SetTextColor(GetColor(GCT_DEFAULT));	
	m_pWeaponInfoExtension->SetEnable(FALSE);
	m_pWeaponInfoUsed->SetEnable(FALSE);	

	m_pCharacterInfoFrame->SetEnable(FALSE);
	m_pCharacterInfoFrame->SetAllShapeEnable(FALSE);
	m_pCharacterInfoFrame->SetShapeEnable(GUI_SHAPE_OFFSET, TRUE);
	m_pCharacterInfoImage->SetEnable(FALSE);
	m_pCharacterInfoValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCharacterInfoValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pCharacterInfoUsed->SetEnable(FALSE);	

	m_pItemInfoFrame->SetEnable(FALSE);
	m_pItemInfoFrame->SetAllShapeEnable(FALSE);
	m_pItemInfoFrame->SetShapeEnable(GUI_SHAPE_OFFSET, TRUE);
	m_pItemInfoImage->SetEnable(FALSE);
	m_pItemInfoValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pItemInfoValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pItemInfoUsed->SetEnable(FALSE);	
	
	//	Friend popup
	{		
		m_pFriendScroll->CreateTextEx(GIFT_POPUP_ELE_COUNT,  GetDefaultFontName());
		m_pFriendScroll->SetSelColor(100, 100, 100, 100);
		m_pFriendScroll->Clear();

		m_idxFriendPopupStartSlot = 0;
		m_pFriendScroll->SetCurValue(0.0f);

		m_pFriendDropDown->SetEnable(FALSE);		
	}

	//	clan member popup
	{
		m_pClanScroll->CreateTextEx(GIFT_POPUP_ELE_COUNT,  GetDefaultFontName());
		m_pClanScroll->SetSelColor(100, 100, 100, 100);
		m_pClanScroll->Clear();

		m_idxClanPopupStartSlot = 0;
		m_pClanScroll->SetCurValue(0.0f);

		m_pClanDropDown->SetEnable(FALSE);		
	}

	m_pCashSymbol->SetEnable(FALSE);
	m_pHaveCashSymbol->SetEnable(FALSE);
	m_pRemainCashSymbol->SetEnable(FALSE);

	CPopupBase::OnInitControl();
}

void CPopupShopGift::OnOpenPopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnOpenPopup( pArg1, pArg2);

	SHOP_GOODS * pGoods = (SHOP_GOODS*) pArg1; 
	I3ASSERT( pGoods != NULL);

#if defined(USE_SHOP_GIFT)
	//I3ASSERT_0;	// 포인트, 캐쉬 모두 판매시 문제가 될수 있음. 코드 수정해야함.
#endif

	i3mem::Copy( &m_Goods, pGoods, sizeof(SHOP_GOODS));
	
	m_pGui->SetEnable(TRUE);

	m_pReceiverInput->SetIMEText("");
	m_pReceiverInput->SetFocus(TRUE);

	m_pButtonOk->setInputDisable(FALSE);
	m_pButtonCancel->setInputDisable(FALSE);
	m_pButtonCashOk->setInputDisable(FALSE);
	m_pButtonCashCancel->setInputDisable(FALSE);
	m_pButtonCashCharge->setInputDisable(FALSE);

	m_szReceiverNick[0] = 0;

	// Friend popup
	{
		INT32 cnt = g_pCommunity->GetFriendCount();
		
		m_pFriendDropDown->SetEnable(FALSE);
		m_pFriendScroll->Clear();

		for(INT32 i=0; i<cnt; i++)
		{
			const FRIEND_NODE* node = g_pCommunity->GetViewFriend(i);
			//if(NULL == node) continue;

			m_pFriendScroll->AddElement( (char*)node->_nick);
		}		

		m_idxFriendPopupStartSlot = 0;
		m_pFriendScroll->SetCurValue(0.0f);
	}

	// Clan member popup
	{
		INT32 cnt = g_pCommunity->GetClanMemberCount();

		m_pClanDropDown->SetEnable(FALSE);
		m_pClanScroll->Clear();

		for(INT32 i=0; i<cnt; i++)
		{
			const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(i);

			m_pClanScroll->AddElement( (char*)node->_nick);
		}		

		m_idxClanPopupStartSlot = 0;
		m_pClanScroll->SetCurValue(0.0f);
	}

	
	if ( g_pConfigEx->GetShop().Enable_ChargeCash )
	{
		CShop* shop = g_pGameContext->GetShop();

		UINT32 PointPrice = 0, CashPrice = 0;
		GOODS_PRICE_TYPE PriceType = shop->GetGoodsPrice(m_Goods._GoodsID, &PointPrice, &CashPrice);

		//	캐시 상품일 경우 캐시 충전 활성화
		if( PriceType == GOODS_PRICE_CASH)
		{
			m_pButtonOk->SetEnable(FALSE);
			m_pButtonCancel->SetEnable(FALSE);

			m_pButtonCashOk->SetEnable(TRUE);
			m_pButtonCashCancel->SetEnable(TRUE);
			m_pButtonCashCharge->SetEnable(TRUE);

#if defined( LOCALE_KOREA)
			m_pCashSymbol->SetEnable( TRUE);
			m_pHaveCashSymbol->SetEnable( TRUE);
			m_pRemainCashSymbol->SetEnable( TRUE);
#endif
		}
		else	//	일반 포인트 상품
		{
			m_pButtonOk->SetEnable(TRUE);
			m_pButtonCancel->SetEnable(TRUE);

			m_pButtonCashOk->SetEnable(FALSE);
			m_pButtonCashCancel->SetEnable(FALSE);
			m_pButtonCashCharge->SetEnable(FALSE);

#if defined( LOCALE_KOREA)
			m_pCashSymbol->SetEnable( FALSE);
			m_pHaveCashSymbol->SetEnable( FALSE);
			m_pRemainCashSymbol->SetEnable( FALSE);
#endif
		}
	}

	_SetWaitAuthBuy(FALSE);
}

void CPopupShopGift::OnClosePopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnClosePopup();

	m_pGui->SetEnable(FALSE);
}

void CPopupShopGift::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CPopupBase::OnUpdate( rDeltaSeconds);

	BOOL IsPackageGoods = FALSE;
	CShop* shop = g_pGameContext->GetShop();
	if ( shop->IsPackageGoods(m_Goods._GoodsID) )
	{
		IsPackageGoods = TRUE;
		_SetPackageGoodsInfo(&m_Goods);
	}
	
	if (!IsPackageGoods)
	{
		//	상품 종류에 따라 분류하여 상품 목록에 추가합니다.
		ITEM_TYPE itemClass = (ITEM_TYPE)GET_ITEM_FLAG_TYPE(m_Goods._ItemID);
		switch(itemClass)
		{
		//	무기
		case ITEM_TYPE_PRIMARY:
		case ITEM_TYPE_SECONDARY:
		case ITEM_TYPE_MELEE:
		case ITEM_TYPE_THROWING1:
		case ITEM_TYPE_THROWING2:
			{
				_SetWeaponInfo(&m_Goods);
			}
			break;
		case ITEM_TYPE_CHARA:
		case ITEM_TYPE_HEADGEAR:
			{
				_SetCharacterInfo(&m_Goods);
			}
			break;
		case ITEM_TYPE_MAINTENANCE:
		case ITEM_TYPE_EXPENDABLE:
		case ITEM_TYPE_TEMP:
			{
				_SetItemInfo(&m_Goods);
			}
			break;
		}
	}

	//	계산 표시
	_SetPointInfo(&m_Goods);
}

void CPopupShopGift::OnPreGuiNotify(void)
{
}

void CPopupShopGift::OnExitKey(void)
{
	OnClosePopup();
}

void CPopupShopGift::OnWheelUpdate(INT32 scrollby)
{	
	if (0 < scrollby)
	{
		for(INT32 i = 0; i < scrollby; i++)
		{
			if( m_pFriendScroll->isEnable())
				m_pFriendScroll->MovePrev();

			if( m_pClanScroll->isEnable())
				m_pClanScroll->MovePrev();
		}
	}
	else
	{
		for(INT32 i = 0; i > scrollby; i--)
		{
			if( m_pFriendScroll->isEnable())
				m_pFriendScroll->MoveNext();

			if( m_pClanScroll->isEnable())
				m_pClanScroll->MoveNext();
		}			
	}	

	if( m_pFriendScroll->isEnable())
		m_idxFriendPopupStartSlot = m_pFriendScroll->GetCurrentTop();

	if( m_pClanScroll->isEnable())
		m_idxClanPopupStartSlot = m_pClanScroll->GetCurrentTop();
}

void CPopupShopGift::_InputConfirm(void)
{	
	i3String::Copy( m_szReceiverNick, m_pReceiverInput->getIMETextBuf(), MAX_STRING_COUNT );

	//	 닉네임이 없다.
	if( i3String::Length(m_szReceiverNick) <= 0)
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_PLEASE_NAME"));/*닉네임을 적어주세요.*/
		return;
	}

	//	돈이 충분하지 검사
	if( g_pFramework->GetShopContext()->CheckBuy(m_Goods._GoodsID, m_Goods._PriceType ) )
	{
		g_pFramework->GetShopContext()->GiftGoods(&m_Goods, m_szReceiverNick);

		_SetWaitAuthBuy(TRUE);
	}
	else
	{
		//	잔액이 부족하여 물품을 구입할 수 없습니다.
		CAPTION_POPUP( GAME_STRING("STBL_IDX_EP_SHOP_DEFICIT_MONEY"), 
					GAME_STRING("STBL_IDX_EP_SHOP_BUY_FAIL_CAPTION_E"));

		OnClosePopup();
	}
}

void CPopupShopGift::_InputCancel(void)
{
	OnClosePopup();
}


void CPopupShopGift::_InputCashCharge(void)
{
	//	캐시 충전 웹페이지 호출
#if !defined(LOCALE_CHINA)
	::ExcuteCashWeb();
#else
	g_pSDOA_Link->OpenWidget();
#endif
}


void CPopupShopGift::_ComboFriend(void)
{
	m_pFriendDropDown->SetEnable( !m_pFriendDropDown->isEnable());

	if( m_pFriendDropDown->isEnable())
	{
		m_pFriendDropDown->SetFocus(TRUE);
	}

	m_pClanDropDown->SetEnable( FALSE);
}

void CPopupShopGift::_ComboClan(void)
{
	m_pClanDropDown->SetEnable( !m_pClanDropDown->isEnable());

	if( m_pClanDropDown->isEnable())
	{
		m_pClanDropDown->SetFocus(TRUE);
	}

	m_pFriendDropDown->SetEnable( FALSE);
}

void CPopupShopGift::_SelectFriend(void)
{
	INT32 idx = m_pFriendScroll->getSelectedElement();
	char * pszSelect= (char*) m_pFriendScroll->GetText( idx);

	m_pReceiverInput->SetIMEText( pszSelect);
	m_pReceiverInput->SetFocus(TRUE);

	m_pFriendDropDown->SetEnable( FALSE);
}

void CPopupShopGift::_SelectClanMember(void)
{
	INT32 idx = m_pClanScroll->getSelectedElement();
	char * pszSelect = (char*) m_pClanScroll->GetText( idx);

	m_pReceiverInput->SetIMEText( pszSelect);
	m_pReceiverInput->SetFocus(TRUE);

	m_pClanDropDown->SetEnable( FALSE);
}

void	CPopupShopGift::_SetWaitAuthBuy(BOOL Wait)
{
	GUI_CONTROL_STATE State = (Wait == TRUE) ? I3GUI_CONTROL_STATE_DISABLED : I3GUI_CONTROL_STATE_NORMAL;

	// 버튼의 상태를 변경한다.
	m_pButtonOk->setControlState(State);
	m_pButtonCashOk->setControlState(State);
	m_pButtonCancel->setControlState(State);
	m_pButtonCashCancel->setControlState(State);

	m_WaitAuthBuy = Wait;
}

void CPopupShopGift::_SetWeaponInfo(SHOP_GOODS * pGoods)
{	
	m_pWeaponInfoFrame->SetEnable(TRUE);
	m_pCharacterInfoFrame->SetEnable(FALSE);
	m_pItemInfoFrame->SetEnable(FALSE);

	SetShopWeapon( pGoods->_ItemID, m_pWeaponInfoFrame, m_pWeaponInfoImage, m_pWeaponInfoKey, m_pWeaponInfoValue, m_pWeaponInfoExtension);
}

void CPopupShopGift::_SetCharacterInfo(SHOP_GOODS * pGoods)
{
	m_pWeaponInfoFrame->SetEnable(FALSE);
	m_pCharacterInfoFrame->SetEnable(TRUE);
	m_pItemInfoFrame->SetEnable(FALSE);

	SetShopCharacter( pGoods->_ItemID, m_pCharacterInfoFrame, m_pCharacterInfoImage, m_pCharacterInfoValue);
}

void CPopupShopGift::_SetItemInfo(SHOP_GOODS * pGoods)
{	
	m_pWeaponInfoFrame->SetEnable(FALSE);
	m_pCharacterInfoFrame->SetEnable(FALSE);
	m_pItemInfoFrame->SetEnable(TRUE);

	SetShopCashItem( pGoods->_ItemID, m_pItemInfoFrame, m_pItemInfoImage, m_pItemInfoValue);
}

void CPopupShopGift::_SetPackageGoodsInfo(SHOP_GOODS* pGoods)
{
	m_pCharacterInfoFrame->SetEnable(FALSE);
	m_pWeaponInfoFrame->SetEnable(FALSE);
	m_pItemInfoFrame->SetEnable(TRUE);

	SetPackageGoods(pGoods->_GoodsID, m_pItemInfoFrame, m_pItemInfoImage, m_pItemInfoValue);
}

void CPopupShopGift::_SetPointInfo(SHOP_GOODS * pGoods)
{
	SetShopPointInfo( pGoods, m_pSaleText,
					m_pValueBuyPoint, m_pValueHavePoint, m_pValueRemainPoint,
					m_pStaticPoint, 3);
}
