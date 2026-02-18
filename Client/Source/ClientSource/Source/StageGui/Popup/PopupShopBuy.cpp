#include "pch.h"
#include "PopupShopBuy.h"
#include "GlobalVariables.h"
#include "PopupUseItem.h"
#include "Shop.h"

#if !defined( ENABLE_UI2)

#if defined (LOCALE_CHINA)
#include "../SDOALink/SDOALink.h"
//#include "CHttpFCMInfo.h"
#endif
//	구입후 시간동안 서버 응답을 기다립니다.
#define BUY_TIMEOUT			20

I3_CLASS_INSTANCE(CPopupShopBuy, CPopupBase);

CPopupShopBuy::CPopupShopBuy()
{
	i3mem::FillZero( &m_BuyGoods, sizeof( SHOP_GOODS));

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

	m_bConfirm = FALSE;
	m_fTimeout = 0.0f;

	m_pCashSymbol = NULL;
	m_pHaveCashSymbol = NULL;
	m_pRemainCashSymbol = NULL;
}

CPopupShopBuy::~CPopupShopBuy()
{
}

void CPopupShopBuy::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
	case GCI_SHOP_B_POPUP_BUY_OK:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _InputConfirm();
		break;
	case GCI_SHOP_B_POPUP_BUY_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _InputCancel();
		break;
	case GCI_SHOP_B_POPUP_BUY_CASH_CHARGE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) _InputCashCharge();
		break;
	}
}

BOOL CPopupShopBuy::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	char temp[256] = "";

	LinkControlEx(plist, "ShopBuy", m_pGui, -1);
	LinkControlEx(plist, "shopBuyPopup", m_pMainForm, -1);
	LinkControlEx(plist, "shopBuyCaption", m_pMainCaption, -1);
	LinkControlEx(plist, "shopBuyOk", m_pButtonOk, GCI_SHOP_B_POPUP_BUY_OK);
	LinkControlEx(plist, "shopBuyCancel", m_pButtonCancel, GCI_SHOP_B_POPUP_BUY_CANCEL);

	LinkControlEx(plist, "shopBuyCashOk", m_pButtonCashOk, GCI_SHOP_B_POPUP_BUY_OK);
	LinkControlEx(plist, "shopBuyCashCancel", m_pButtonCashCancel, GCI_SHOP_B_POPUP_BUY_CANCEL);
	LinkControlEx(plist, "shopBuyCashCharge", m_pButtonCashCharge, GCI_SHOP_B_POPUP_BUY_CASH_CHARGE);	
	LinkControlEx(plist, "shopBuyCashChargeMark", m_pButtonCashChargeMark, -1);
	LinkControlEx(plist, "shopBuyCashChargeStatic", m_pButtonCashChargeStatic, -1);
	
	LinkControlEx(plist, "shopBuyUseSaleText", m_pSaleText, -1);

	for(INT32 i=0; i<STATIC_POINT_COUNT; i++)
	{
		sprintf_s(temp, "shopBuyPointStatic%d", i+1);
		LinkControlEx(plist, temp, m_pStaticPoint[i], -1);
	}

	LinkControlEx(plist, "shopBuyHavePointStatic", m_pStaticHavePoint, -1);
	LinkControlEx(plist, "shopBuyRemainPointStatic", m_pStaticRemainPoint, -1);

	LinkControlEx(plist, "shopBuyPointValue", m_pValueBuyPoint, -1);
	LinkControlEx(plist, "shopBuyHavePointValue", m_pValueHavePoint, -1);
	LinkControlEx(plist, "shopBuyRemainPointValue", m_pValueRemainPoint, -1);

	// 한국 N코인 심볼 이미지
	LinkControlEx(plist, "shopBuyCashSymbol", m_pCashSymbol, -1);
	LinkControlEx(plist, "shopBuyHaveCashSymbol", m_pHaveCashSymbol, -1);
	LinkControlEx(plist, "shopBuyRemainCashSymbol", m_pRemainCashSymbol, -1);
	
	//	Buy weapon info
	{
		LinkControlEx(plist, "shopBuyWeaponInfo", m_pWeaponInfoFrame, -1);

		LinkControlEx(plist, "shopBuyWeaponValue", m_pWeaponInfoValue, -1);
		LinkControlEx(plist, "shopBuyWeaponKey", m_pWeaponInfoKey, -1);
		LinkControlEx(plist, "shopBuyWeaponDummy", m_pWeaponInfoImage, -1);
		LinkControlEx(plist, "shopBuyWeaponExtension", m_pWeaponInfoExtension, -1);
		LinkControlEx(plist, "shopBuyWeaponUsed", m_pWeaponInfoUsed, -1);
	}

	//	Buy Character info
	{
		LinkControlEx(plist, "shopBuyCharacterInfo", m_pCharacterInfoFrame, -1);

		LinkControlEx(plist, "shopBuyCharacterValue", m_pCharacterInfoValue, -1);
		LinkControlEx(plist, "shopBuyCharacterDummy", m_pCharacterInfoImage, -1);
		LinkControlEx(plist, "shopBuyCharacterUsed", m_pCharacterInfoUsed, -1);
	}

	//	Buy item info
	{
		LinkControlEx(plist, "shopBuyItemInfo", m_pItemInfoFrame, -1);

		LinkControlEx(plist, "shopBuyItemValue", m_pItemInfoValue, -1);
		LinkControlEx(plist, "shopBuyItemDummy", m_pItemInfoImage, -1);
		LinkControlEx(plist, "shopBuyItemUsed", m_pItemInfoUsed, -1);
	}

	return FALSE;
}

void CPopupShopBuy::OnInitControl(void)
{
	char szTemp[256] = "";

	m_pGui->SetEnable(FALSE);

	INIT_WIN_CAPTION(m_pMainCaption, GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));

	INIT_CAPTION_BUTTON(m_pButtonOk, GAME_STRING("STBL_IDX_BUTTON_OK"));
	INIT_CAPTION_BUTTON(m_pButtonCancel, GAME_STRING("STBL_IDX_BUTTON_CANCEL"));

	INIT_CAPTION_BUTTON(m_pButtonCashOk, GAME_STRING("STBL_IDX_BUTTON_OK"));
	INIT_CAPTION_BUTTON(m_pButtonCashCancel, GAME_STRING("STBL_IDX_BUTTON_CANCEL"));
	INIT_BUTTON_EX(m_pButtonCashCharge);
	INIT_VALUE_CAPTION2(m_pButtonCashChargeStatic, GAME_STRING("STR_POPUP_CHARGE_CASH"));/*N코인 충전*/

	m_pButtonCashOk->SetEnable(FALSE);
	m_pButtonCashCancel->SetEnable(FALSE);
	m_pButtonCashCharge->SetEnable(FALSE);

	m_pButtonCashChargeMark->setInputDisable(TRUE);
	m_pButtonCashChargeStatic->setInputDisable(TRUE);

	m_pSaleText->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);

	sprintf_s(szTemp, "%s :", GAME_STRING("STBL_IDX_SHOP_USE_DAY"));/*일*/
	m_pSaleText->SetTextColor(GetColor(GCT_DEFAULT));

	sprintf_s(szTemp, "%s :", GAME_STRING("STBL_IDX_SHOP_HAVE_MONEY"));/*보유 Point*/
	m_pStaticHavePoint->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pStaticHavePoint->SetTextColor(GetColor(GCT_DEFAULT));
	m_pStaticHavePoint->SetText(szTemp);

	sprintf_s(szTemp, "%s :", GAME_STRING("STBL_IDX_SHOP_REMAIN_MONEY"));/*잔액*/
	m_pStaticRemainPoint->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pStaticRemainPoint->SetTextColor(GetColor(GCT_DEFAULT));
	m_pStaticRemainPoint->SetText(szTemp);

	for(INT32 i=0; i<STATIC_POINT_COUNT; i++)
	{
		m_pStaticPoint[i]->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pStaticPoint[i]->SetText(GAME_STRING("STBL_IDX_SHOP_MONEY"));
		m_pStaticPoint[i]->SetTextColor(GetColor(GCT_DEFAULT));
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


	for ( INT32 i = 0 ; i < MAX_EFFECT_BTN ; ++i )
		m_BtnEffector[i].SetEffectTime(0.25f);

	m_BtnEffector[EFFECT_BTN_OK].Set(m_pButtonOk);
	m_BtnEffector[EFFECT_BTN_CASH].Set(m_pButtonCashOk);

	m_pCashSymbol->SetEnable(FALSE);
	m_pHaveCashSymbol->SetEnable(FALSE);
	m_pRemainCashSymbol->SetEnable(FALSE);

	CPopupBase::OnInitControl();
}

void CPopupShopBuy::OnOpenPopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnOpenPopup( pArg1, pArg2);

	I3ASSERT(pArg1 != NULL);
	I3ASSERT(pArg2 != NULL);

	SHOP_GOODS * pGoods = (SHOP_GOODS*) pArg1;
	I3ASSERT( pGoods != NULL);


	//
	// SHOP_GOODS에 가격 형식을 구입하는 형식으로 덮어씌웁니다.
	//
	i3mem::Copy( &m_BuyGoods, pGoods, sizeof( SHOP_GOODS));	
	m_BuyGoods._PriceType	= *static_cast<UINT32*>(pArg2);

    m_pGui->SetEnable(TRUE);

	m_bConfirm = FALSE;
	m_fTimeout = 0.0f;

	if ( g_pConfigEx->GetShop().Enable_ChargeCash )
	{
		GOODS_PRICE_TYPE PriceType = m_BuyGoods._PriceType;

		//	캐시 상품일 경우 캐시 충전 활성화
		if( PriceType == GOODS_PRICE_CASH )
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

	_SetButtonState(I3GUI_CONTROL_STATE_NORMAL);
}

void CPopupShopBuy::OnClosePopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnClosePopup();

	m_pGui->SetEnable(FALSE);

	m_bConfirm = FALSE;
	m_fTimeout = 0.0f;
}

void CPopupShopBuy::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CPopupBase::OnUpdate( rDeltaSeconds);

	BOOL IsPackageGoods = FALSE;
	CShop* shop = g_pGameContext->GetShop();
	if (shop->IsPackageGoods(m_BuyGoods._GoodsID))
	{
		IsPackageGoods = TRUE;
		_SetPackageGoodsInfo( &m_BuyGoods);
	}



	if (!IsPackageGoods)
	{
		//	상품 종류에 따라 분류하여 상품 목록에 추가합니다.
		ITEM_TYPE itemClass = (ITEM_TYPE)GET_ITEM_FLAG_TYPE(m_BuyGoods._ItemID);
		switch(itemClass)
		{
		case ITEM_TYPE_PRIMARY:
		case ITEM_TYPE_SECONDARY:
		case ITEM_TYPE_MELEE:
		case ITEM_TYPE_THROWING1:
		case ITEM_TYPE_THROWING2:
			{
				_SetWeaponInfo( &m_BuyGoods);
			}
			break;
		case ITEM_TYPE_CHARA:
		case ITEM_TYPE_HEADGEAR:
			{
				_SetCharacterInfo( &m_BuyGoods);
			}
			break;
		case ITEM_TYPE_MAINTENANCE:
		case ITEM_TYPE_EXPENDABLE:
		case ITEM_TYPE_TEMP:
			{
				_SetItemInfo( &m_BuyGoods);
			}
			break;
		}	
	}

	//	계산 표시
	_SetPointInfo( &m_BuyGoods);

	//	서버로 구입 확인을 보내고 답변을 받을 때까지 기다린다. 
	//	만약 대기 시간이 초과될 경우 서버로 부터 답변을 못받았다고 판단되므로 게임을 강제 종료합니다.
	if(m_bConfirm) 
	{
		//	정해진 시간동안 서버 응답을 기다린다.
		if( m_fTimeout < BUY_TIMEOUT)
		{
			m_fTimeout += rDeltaSeconds;

			GAME_EVENT event = EVENT_NOTHING;
			INT32 arg = 0;

			g_pFramework->GetShopContext()->PopGameEvent( &event, &arg);

			//	서버로부터 응답을 받았으므로 정상 작동하고 팝업 종료합니다.
			if( event == EVENT_BUY_GOODS)
			{
				i3GuiRoot::setGuiDisableInput(FALSE);

				m_bConfirm = FALSE;
				m_fTimeout = 0.0f;

				if( EV_SUCCESSED( arg ))
				{
				//	구입 성공
#ifdef AUTH_FROM_BUY_GOODS
					//	여기 걸리지 않으면 무기는 구입했지만 인벤에는 없는 경우입니다.
					if( g_pFramework->HaveItem(m_BuyGoods._ItemID) )
					{
						CShop* shop = g_pGameContext->GetShop();

						if (shop->IsPackageGoods(m_BuyGoods._GoodsID))
						{
#if !defined(TEMP_ERASE_CODE)					// 커버러티에 따른...실행되지 않는 코드에 대한 더 넓은 매크로처리.(11.11.07.수빈)

							UINT32 point = 0;
							//	세트상품은 인증안함
							SHOP_GOODS_LIST * pGoodsList = shop->GetGoodsListData(m_BuyGoods._GoodsID);
							INT32 ItemCount = pGoodsList->_ui8ItemCount;
							for ( INT32 i = 0 ; i < ItemCount ; ++i )
							{
								if ( ITEM_TYPE_POINT == GET_ITEM_FLAG_TYPE(pGoodsList->_ppItemData[i]->_ui32ItemID) )
								{

									switch(GET_ITEM_FLAG_NUMBER(itemId))
									{
									case 1:	point += 1000;		break;
									case 2: point += 5000;		break;
									case 3: point += 10000;		break;
									case 4: point += 25000;		break;
									case 5: point += 50000;		break;
									case 6: point += 100000;	break;
									case 7: point += 20000;		break;
									}

								}
							}
#endif

#if !defined(TEMP_ERASE_CODE)
							if (0 < point)
							{
								char buyMessage[MAX_STRING_COUNT];

								//	"%d 포인트가 지급되었으며\n \n구입하신 아이템은 인벤토리에서\n확인하실 수 있습니다."
								sprintf_s(buyMessage, GAME_STRING("STBL_IDX_EP_SHOP_BUY_PRICE_SUCCESS"), point);
								CAPTION_POPUP(buyMessage, GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
							}
							else
#endif
							{

								CAPTION_POPUP(GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
							}

							OnClosePopup();
							return;
						}

//						break;
					}


					//	바로 아이템을 장착한다. (아이템 인증이 안된 경우 물어본다.)
					_AuthGoods( &m_BuyGoods); 
#else
					//	인증이 필요없는 아이템은 바로 구입 성공
					CAPTION_POPUP(GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), 
									GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
#endif
				}
				else
				{
					switch( arg )
					{
					case 0X83000045:
						{
							// 내구도로 바뀌면서 이미 가지고 있는 아이템이기 때문에 구매 실패 -- 영권작업 필요 --
							CAPTION_POPUP(GAME_STRING("STBL_IDX_EP_SHOP_BUY_FAIL_E"), GAME_STRING("STBL_IDX_EP_SHOP_BUY_FAIL_CAPTION_E"));
						}
						break;
					default:
						{
							CAPTION_POPUP(GAME_STRING("STBL_IDX_EP_SHOP_BUY_FAIL_E"), GAME_STRING("STBL_IDX_EP_SHOP_BUY_FAIL_CAPTION_E"));
						}
						break;
					}
				}

				//	팝업 종료
				OnClosePopup();
			}
		}
		else
		{
			//	네트워크 문제로 구입에 실패했으므로 에러 메시지를 보내고 게임을 강제 종료합니다.
			ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_SHOP_BUY_FAIL_E"), EXIT_CALLBACK(CbExitNormal), this);

			m_bConfirm = FALSE;
			m_fTimeout = 0.0f;

			//	팝업 종료
			OnClosePopup();
		}
	}

#if !defined (LOCALE_KOREA)
	if( m_pButtonOk->isEnable())
		m_BtnEffector[EFFECT_BTN_OK].Update(TRUE, rDeltaSeconds);

	if( m_pButtonCashOk->isEnable())
		m_BtnEffector[EFFECT_BTN_CASH].Update(TRUE, rDeltaSeconds);
#endif
}

void CPopupShopBuy::OnPreGuiNotify(void)
{
}

void CPopupShopBuy::OnExitKey(void)
{
	OnClosePopup();
}

void CPopupShopBuy::SendBuyEvent(SHOP_GOODS * pBuyGoods)
{
	//	서버에게 상품 구입 이벤트를 보냅니다.
	g_pFramework->GetShopContext()->BuyGoods(pBuyGoods);

	//	서버로 부터 구입에 대한 답변이 있을 때까지 무조건 유저의 입력을 막습니다. 중복 구매 버그를 막기 위해. komet
	i3GuiRoot::setGuiDisableInput(TRUE);

	m_bConfirm = TRUE;
	m_fTimeout = 0.0f;

	// 인증할대까지 버튼을 비활성화한다.
	_SetButtonState(I3GUI_CONTROL_STATE_DISABLED);
}

void CPopupShopBuy::_InputConfirm(void)
{
	//	돈이 충분하지 검사
	if( ! g_pFramework->GetShopContext()->CheckBuy(m_BuyGoods._GoodsID, m_BuyGoods._PriceType) )
	{
		//	잔액이 부족하여 물품을 구입할 수 없습니다.
		CAPTION_POPUP( GAME_STRING("STBL_IDX_EP_SHOP_DEFICIT_MONEY"), 
						GAME_STRING("STBL_IDX_EP_SHOP_BUY_FAIL_CAPTION_E"));

		OnClosePopup();
		return;
	}	


	CShop* shop = g_pGameContext->GetShop();
	SHOP_GOODS_LIST * pGoodsList = shop->GetGoodsListData(m_BuyGoods._GoodsID);

	if (pGoodsList && pGoodsList->_ui8ItemCount > 1 )
	{
		INT32 ItemCount = pGoodsList->_ui8ItemCount;
		for(INT32 j = 0; j < ItemCount ; j++)
		{
			BOOL	IsHave	= g_pFramework->GetInvenList()->HaveItem(pGoodsList->_ppItemData[j]->_ui32ItemID);
			if (IsHave)
			{
				CAPTION_POPUP_Q(GAME_STRING("STR_POPUP_Q_MESSAGE_BUY_SET_GOODS"), GAME_STRING("STR_POPUP_CAPTION_CONFIRM_BUY"), MAKE_CALLBACK(CbBuyForHavingGoods), this);
				return;
			}
		}

		SendBuyEvent( &m_BuyGoods);	
		return;
	}



	// 보유중인 아이템이 아니라면 바로 구입.
	if ( ! g_pFramework->GetInvenList()->HaveItem(m_BuyGoods._ItemID) )
	{
		SendBuyEvent( &m_BuyGoods);	
		return;
	}


	// 구입전 이미 구매 및 사용중인 아이템이면 다시한번 묻는다.
	AUTH_TYPE 	type = shop->GetAuthType( m_BuyGoods._ItemID );

	I3ASSERT(type == AUTH_SEC || type == AUTH_COUNT);
	if( type == AUTH_SEC)  
	{
		/*보유중인 아이템입니다.\n구입시 선택하신 기간만큼 보유중인\n아이템의 남은 기간이 증가합니다.\n \n구입하시겠습니까?*//*구입확인*/
		CAPTION_POPUP_Q(GAME_STRING("STR_POPUP_Q_MESSAGE_BUY_MAINTENANCE_ITEM"), 
			GAME_STRING("STR_POPUP_CAPTION_CONFIRM_BUY"), MAKE_CALLBACK(CbBuyForHavingGoods), this);
	}
	else if ( type == AUTH_COUNT)
	{
		/*보유중인 아이템입니다.\n구입시 선택하신 개수만큼 보유중인\n아이템의 수량이 증가합니다.\n \n구입하시겠습니까?*//*구입확인*/
		CAPTION_POPUP_Q(GAME_STRING("STR_POPUP_Q_MESSAGE_BUY_EXPENDABLE_ITEM"), 
			GAME_STRING("STR_POPUP_CAPTION_CONFIRM_BUY"), MAKE_CALLBACK(CbBuyForHavingGoods), this);
	}


	return;
}

void CPopupShopBuy::_InputCancel(void)
{
	//	종료
	OnClosePopup();
}

void CPopupShopBuy::_InputCashCharge(void)
{
	//	캐시 충전 웹페이지 호출
#if !defined(LOCALE_CHINA)
	::ExcuteCashWeb();
#else
	g_pSDOA_Link->OpenWidget();
#endif
}

void CPopupShopBuy::_SetWeaponInfo(SHOP_GOODS * pGoods)
{		
	m_pCharacterInfoFrame->SetEnable(FALSE);
	m_pItemInfoFrame->SetEnable(FALSE);

	SetShopWeapon( pGoods->_ItemID, m_pWeaponInfoFrame, m_pWeaponInfoImage, m_pWeaponInfoKey, m_pWeaponInfoValue, m_pWeaponInfoExtension);
}

void CPopupShopBuy::_SetCharacterInfo(SHOP_GOODS * pGoods)
{	
	m_pWeaponInfoFrame->SetEnable(FALSE);
	m_pItemInfoFrame->SetEnable(FALSE);

	SetShopCharacter( pGoods->_ItemID, m_pCharacterInfoFrame, m_pCharacterInfoImage, m_pCharacterInfoValue);
}

void CPopupShopBuy::_SetItemInfo(SHOP_GOODS * pGoods)
{		
	m_pCharacterInfoFrame->SetEnable(FALSE);
	m_pWeaponInfoFrame->SetEnable(FALSE);

	SetShopCashItem( pGoods->_ItemID, m_pItemInfoFrame, m_pItemInfoImage, m_pItemInfoValue);
}

void CPopupShopBuy::_SetPackageGoodsInfo(SHOP_GOODS* pGoods)
{
	m_pCharacterInfoFrame->SetEnable(FALSE);
	m_pWeaponInfoFrame->SetEnable(FALSE);

	SetPackageGoods(pGoods->_GoodsID, m_pItemInfoFrame, m_pItemInfoImage, m_pItemInfoValue);
}

void CPopupShopBuy::_SetPointInfo(SHOP_GOODS * pGoods)
{
	SetShopPointInfo( pGoods, m_pSaleText,
					m_pValueBuyPoint, m_pValueHavePoint, m_pValueRemainPoint,
					m_pStaticPoint, STATIC_POINT_COUNT);
}

BOOL CPopupShopBuy::_AuthGoods(SHOP_GOODS * pGoods)
{	
	I3ASSERT( pGoods!= NULL);

	// 사용여부
	UINT8 itemAttr = 0;
	UINT32 itemArg = 0;

	INT64	i64UID		= ItemIDToWareDBIndex(pGoods->_ItemID);
	INT32	gameItemId	= pGoods->_ItemID;
	ITEM_TYPE goodsClass = (ITEM_TYPE)GET_ITEM_FLAG_TYPE(gameItemId);

	//	바로 아이템을 장착한다. (아이템 인증이 안된 경우 물어본다.)
	switch( goodsClass)						
	{
		//	지속성
		case ITEM_TYPE_PRIMARY:
		case ITEM_TYPE_SECONDARY:
		case ITEM_TYPE_MELEE:
		case ITEM_TYPE_THROWING1:
		case ITEM_TYPE_THROWING2:
			{
				//	장비중이 아니면
				if( !g_pFramework->IsUsedWeapon(gameItemId))
				{
					const char * pMessage = GAME_STRING("STR_POPUP_Q_MESSAGE_EQUIP");	/*바로 장비 하시겠습니까?*/

					SHOP_ITEM_DATA* pItem = g_pGameContext->GetShop()->GetItemData( gameItemId );
					switch( pItem->_ui8AuthType )
					{
					case AUTH_COUNT:	// 개수제
						{
							STRING_POPUP_Q( pMessage, MAKE_CALLBACK(CbEquipGoods), this);
							return TRUE;
						}
						break;
					case AUTH_SEC:	// 기간제
						{
							g_pFramework->GetWeaponTypeByID(gameItemId, &itemAttr, &itemArg);
							if(ITEM_ATTR_TYPE_BUY == itemAttr)
							{
								if( i64UID != INVALID_WAREDB_INDEX )
								{
									STRING_POPUP_Q( pMessage, MAKE_CALLBACK(CbAuthGoods), this);
									return TRUE;
								}
							}
							else
							{
								STRING_POPUP_Q( pMessage, MAKE_CALLBACK(CbEquipGoods), this);
								return TRUE;
							}
						}
						break;
					}
				}
			}
			break;
		case ITEM_TYPE_CHARA:
		case ITEM_TYPE_HEADGEAR:	
			{
				const char * pMessage = GAME_STRING("STR_POPUP_Q_MESSAGE_CHANGE");	/*바로 변경 하시겠습니까?*/

				//	장비중이 아니면
				BOOL bNeedEquip = !g_pFramework->IsUsedCharacter(gameItemId);

				BOOL bHasInven = g_pFramework->GetCharacterType(gameItemId, &itemAttr, &itemArg);
				
				if (bHasInven)
				{
					//	이미 장비하고 있어도 뷰 캐릭터 팀과 상품의 팀이 다르면 장비해야 한다.
					if( g_pFramework->GetCurrentStage()->IsExactTypeOf( CStageReady::GetClassMeta()))
					{
						CStageReady * pStage = (CStageReady *) g_pFramework->GetCurrentStage();
						
						INT32 nTeam = GET_ITEM_FLAG_CLASS(gameItemId);
						if(CHARACTER_TEAM_RB != nTeam &&
							pStage->GetViewCharaInfo()->_charaInfo.GetTeam() != nTeam)
							bNeedEquip = TRUE;
					}
				}

				if (bNeedEquip)
				{
					SHOP_ITEM_DATA* pItem = g_pGameContext->GetShop()->GetItemData( gameItemId );
					switch( pItem->_ui8AuthType )
					{
					case AUTH_COUNT:	// 개수제
						STRING_POPUP_Q(pMessage, MAKE_CALLBACK(CbEquipGoods), this);
						return TRUE;
						break;
					case AUTH_SEC:
						if(ITEM_ATTR_TYPE_BUY == itemAttr)
						{
							if( i64UID != INVALID_WAREDB_INDEX )
							{
								STRING_POPUP_Q( pMessage, MAKE_CALLBACK(CbAuthGoods), this);
								return TRUE;
							}
						}
						else
						{
							STRING_POPUP_Q( pMessage, MAKE_CALLBACK(CbEquipGoods), this);
							return TRUE;
						}
						break;
					}
				}
			}
			break;
		case ITEM_TYPE_MAINTENANCE:
		case ITEM_TYPE_EXPENDABLE:
			{
				INVEN_ITEM_TYPE InvenItemType = g_pFramework->GetInvenList()->GetInvenItemType(i64UID);
				if( InvenItemType == INVEN_ITEM_CASHITEM)
				{
					CbAuthGoods( this, MBR_OK);	//	바로 사용여부를 물어본다.

					return TRUE;
				}
			}
			break;
	}

	//	장비가 필요하지 않을 경우
	CAPTION_POPUP(GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), 
					GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));

	return FALSE;
}

void	CPopupShopBuy::_SetButtonState(GUI_CONTROL_STATE State)
{
	// 버튼의 상태를 변경한다.
	m_pButtonOk->setControlState(State);
	m_pButtonCashOk->setControlState(State);
	m_pButtonCancel->setControlState(State);
	m_pButtonCashCancel->setControlState(State);
}

void CPopupShopBuy::CbBuyForHavingGoods(void * pThis, INT32 nParam)
{	
	CPopupShopBuy * pPopup = (CPopupShopBuy *) pThis;

	if( nParam == MBR_OK)
	{
		//	구입 이벤트를 서버에 보낸다.
		pPopup->SendBuyEvent( pPopup->GetBuyGoods());
	}
	else
	{
		pPopup->OnClosePopup();
	}
}

void CPopupShopBuy::CbAuthGoods(void* pThis, INT32 nParam)
{
	CPopupShopBuy * pPopup = (CPopupShopBuy *) pThis;

	SHOP_GOODS * pGoods = pPopup ->GetBuyGoods();
	I3ASSERT( pGoods != NULL);

	INT32 goodsClass = GET_ITEM_FLAG_TYPE(pGoods->_ItemID);

	if( nParam == MBR_OK)
	{	
		INT64 i64UID = ItemIDToWareDBIndex(pGoods->_ItemID);

		switch((ITEM_TYPE) goodsClass)
		{
		case ITEM_TYPE_PRIMARY:
		case ITEM_TYPE_SECONDARY:
		case ITEM_TYPE_MELEE:
		case ITEM_TYPE_THROWING1:
		case ITEM_TYPE_THROWING2:
		case ITEM_TYPE_CHARA:
		case ITEM_TYPE_HEADGEAR:
			{
				//	구입한 아이템의 인증을 요구
				g_pGameContext->SetEvent(EVENT_ITEM_AUTH, &i64UID);
			}
			break;
		case ITEM_TYPE_MAINTENANCE:
		case ITEM_TYPE_EXPENDABLE:
			{
#if defined(CHANGE_ITEM_USE_CHECK)
				ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(pGoods->_ItemID);
				if ( pInfo != NULL && pInfo->GetUsePosition() == USE_POS_ANYWHERE )
				{
					USE_ITEM_STYLE style = USE_ITEM_STYLE_BUYING;

					//	아이템을 사용할지 질문
					((CStageReady*)pPopup->getParent())->TogglePopup(POPUP_USE_ITEM, &pGoods->_ItemID, &style);
				}
				else
				{
					CAPTION_POPUP(GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), 
						GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));/*구입하신 물품은 인벤토리에서\n 확인하실 수 있습니다.*/
				}
#else
				//	클랜 관리 페이지용 아이템이면 상점에서 사용하지 못하도록 
				if( CheckCashItemOnlyClanManagementStep( pGoods->_ItemID))
				{
					CAPTION_POPUP(GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), 
									GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));/*구입하신 물품은 인벤토리에서\n 확인하실 수 있습니다.*/
				}
				//	인게임에서 사용하는 캐시 아이템인지 여부 판단.
				else if( CheckCashItemOnlyInGame( pGoods->_ItemID)) 
				{
					CAPTION_POPUP(GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), 
									GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));/*구입하신 물품은 인벤토리에서\n 확인하실 수 있습니다.*/
				}
				else
				{
					USE_ITEM_STYLE style = USE_ITEM_STYLE_BUYING;

					//	아이템을 사용할지 질문
					((CStageReady*)pPopup->getParent())->TogglePopup(POPUP_USE_ITEM, &pGoods->_ItemID, &style);
				}
#endif
			}
			break;
		}
	}
	else
	{
		switch((ITEM_TYPE) goodsClass)
		{
		case ITEM_TYPE_PRIMARY:
		case ITEM_TYPE_SECONDARY:
		case ITEM_TYPE_MELEE:
		case ITEM_TYPE_THROWING1:
		case ITEM_TYPE_THROWING2:
			{
				//	인증을 원하지 않을 경우
				CAPTION_POPUP(GAME_STRING("STR_POPUP_MESSAGE_ITEM_BUY_COMPLETE_1"), 
							GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));/*구입하신 아이템은 인벤토리에서\n확인하실 수 있으며 구입후 한번도\n장비하지 않은 미사용 무기는\n게임 도중에 교체할 수 없습니다.*/
			}
			break;
		case ITEM_TYPE_CHARA:
		case ITEM_TYPE_HEADGEAR:
		case ITEM_TYPE_MAINTENANCE:
		case ITEM_TYPE_EXPENDABLE:
		case ITEM_TYPE_TEMP:
		default:
			{
				//	인증을 원하지 않을 경우
				CAPTION_POPUP(GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), 
							GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
			}
			break;
		}
	}
}

void CPopupShopBuy::CbEquipGoods(void* pThis, INT32 nParam)
{
	CPopupShopBuy * pPopup  = (CPopupShopBuy *) pThis;

	SHOP_GOODS * pGoods = pPopup->GetBuyGoods();
	I3ASSERT( pGoods != NULL);

	//	아이템을 인벤에 장비
	if( nParam == MBR_OK)
	{
		if( g_pFramework->GetShopContext()->EquipGoods( pGoods))
			pPopup->PlaySound(GUI_SND_ITEM_EQUIP);	//	장착하면 사운드
	}
	else
	{
		//	장비를 원하지 않을 경우
		CAPTION_POPUP(GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), 
						GAME_STRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
	}
}

#endif // ENABLE_UI2

