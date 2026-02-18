#include "pch.h"
#include "StepShop.h"
#include "StageReady.h"
#include "GlobalVariables.h"
#include "StageGui/Popup/PopupExtension.h"
#include "StageGui/Popup/PopupUseItem.h"
#include "StageGui/Popup/PopupDeleteItem.h"
#include "ShopContext.h"

#include "Shop.h"
#if defined (LOCALE_CHINA)
#include "../SDOALink/SDOALink.h"
//#include "CHttpFCMInfo.h"
#endif

I3_CLASS_INSTANCE(CStepShop, CStepBase);



CStepShop::CStepShop()
: m_TabWeapon(this)
, m_TabChara(this)
, m_TabItem(this)
, m_TabPackage(this)

{
	CTabPackage temp(this);

	m_pShopContext = g_pFramework->GetShopContext();
	I3ASSERT( m_pShopContext != NULL);

	m_pShopGui		= NULL;

	m_pShopWin		= NULL;
	m_pShopCaption	= NULL;
	m_pLeave		= NULL;
	m_pCashCharge	= NULL;
	m_pCurrentTab	= NULL;
}

CStepShop::~CStepShop()
{
}

void CStepShop::OnPreGuiNotify(void)
{
	CStepBase::OnPreGuiNotify();

	if ( m_pCurrentTab )
		m_pCurrentTab->OnPreGuiNotify();
}

void CStepShop::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
	//	Common	
	case GCI_SHOP_B_WEAPON_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			ChangeTab(&m_TabWeapon, true);
		break;

	case GCI_SHOP_B_CHARACTER_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			ChangeTab(&m_TabChara, true);
		break;

	case GCI_SHOP_B_ITEM_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			ChangeTab(&m_TabItem, true);
		break;

	case GCI_SHOP_B_SET_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			ChangeTab(&m_TabPackage, true);
		break;

	case GCI_SHOP_B_LEAVE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			OnLeave();
		break;

	case GCI_SHOP_B_CASH_CHARGE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			OnCashCharge();
		break;
	}

	if ( m_pCurrentTab )
		m_pCurrentTab->OnGuiNotify(pNotify);
}

BOOL CStepShop::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	//	Shop common
	LinkControlEx(plist, "Shop", m_pShopGui, -1);

	LinkControlEx(plist, "shopWin", m_pShopWin, -1);
	LinkControlEx(plist, "shopWinCaption", m_pShopCaption, -1);

	LinkControlEx(plist, "shopLeave", m_pLeave, GCI_SHOP_B_LEAVE);
	LinkControlEx(plist, "shopCashCharge", m_pCashCharge, GCI_SHOP_B_CASH_CHARGE);


	m_TabWeapon.OnLinkControl(plist);
	m_TabChara.OnLinkControl(plist);
	m_TabItem.OnLinkControl(plist);
	m_TabPackage.OnLinkControl(plist);

	return FALSE;
}

void CStepShop::OnInitControl(void)
{
	m_pShopGui->SetEnable(FALSE);

	INIT_WIN_CAPTION(m_pShopCaption, GAME_STRING("STBL_IDX_SHOP_CAPTION"));
	INIT_BUTTON_EX(m_pLeave);
	INIT_BUTTON_EX(m_pCashCharge);
	
	m_pCashCharge->SetEnable(g_pConfigEx->GetShop().Enable_ChargeCash);

	m_TabWeapon.Init();
	m_TabChara.Init();
	m_TabItem.Init();
	m_TabPackage.Init();


	CStepBase::OnInitControl();
}

void CStepShop::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CStepBase::OnUpdate(rDeltaSeconds);

	if ( m_pCurrentTab )
		m_pCurrentTab->OnUpdate(rDeltaSeconds);


	// 구입관련 팝업창이 떠 있다면 버튼효과를 하지 않는다.
	if( ((CStageReady*)m_pParent)->IsEnablePopup( POPUP_SHOP_BUY) ||
		((CStageReady*)m_pParent)->IsEnablePopup( POPUP_SHOP_GIFT) )
	{
		m_BtnEffector.Update(FALSE, rDeltaSeconds);
	}
	else
	{
		m_BtnEffector.Update(TRUE, rDeltaSeconds);
	}

}

void CStepShop::OnExitKey(void)
{
	OnLeave();
}

void CStepShop::OnWheelUpdate(INT32 scrollby)
{
	if ( m_pCurrentTab )
		m_pCurrentTab->OnWheelUpdate(scrollby);
}

void CStepShop::SetInputDisable(BOOL bDisable)
{
	m_pShopGui->setInputDisable(bDisable);
}

BOOL CStepShop::GetInputDisable(void)
{
	return m_pShopGui->getInputDisable();
}

void CStepShop::OnStart(void)
{
	CStepBase::OnStart();


	m_BtnEffector.Reset();


	// 닉네임 설정 전이라면, 시작하지 않는다
	if( ! g_pGameContext->HaveNickName() )
	{
		OnLeave();
		return;
	}

	// Default View Tab
	ChangeTab(&m_TabWeapon);

	SetPushedStateButton(MBT_SHOP, TRUE);
	
}

void CStepShop::OnEnd(void)
{
	CStepBase::OnEnd();

	ChangeTab(NULL);

	SetPushedStateButton(MBT_SHOP, FALSE);
}


void CStepShop::OnInitRun(void)
{
	CStepBase::OnInitRun(); 

	ChangeTab(&m_TabWeapon);

	MoveCamera(CL_WEAPON); 
}

BOOL CStepShop::OnSliding(RT_REAL32 rDeltaSeconds)
{
	if (FALSE == CStepBase::OnSliding(rDeltaSeconds))
	{
		return FALSE;
	}


	// 시간에 따른 기준위치 지정
	m_pShopGui->setPositionY(CStepBase::GetSlideY());


	// 퇴장 시, 비활성화. 그외 활성화
	BOOL enableWindow = TRUE;

	if (SS_OUT == CStepBase::GetSlideState())
	{
		enableWindow = FALSE;
	}

	m_pShopGui->SetEnable(enableWindow);

	return TRUE;
}

void CStepShop::EnterShop(void)
{
	if( !m_pShopContext->InServer())
		m_pShopContext->EnterServer();
	//CStageReady * pReady = ((CStageReady*)m_pParent);
	g_FirstShop = FALSE;

#if defined(USE_LINK_SHOP)
	if(g_pGameContext->IsLinkShopMessage())
	{
		(((CStageReady*)m_pParent)->TogglePopup(POPUP_LINK_SHOP, (void*) 2));
		g_pGameContext->RemoveGameFlag( GAMEFLAG_LINKSHOPMSG);
	}
#endif

	//무기 탭에 대한 흔적을 초기화 한다.
	m_TabWeapon._ResetVestige();
}

void CStepShop::LeaveShop(void)
{
	if( m_pShopContext->InServer())
	{
		m_pShopContext->LeaveServer();
	}

	//	이전 장착으로 돌아갑니다.
	UpdateCharacter(VT_DEFAULT);

	ChangeTab(NULL);
}



void CStepShop::OnLeave(void)
{
	CStageReady * pParent = ((CStageReady*)m_pParent);
	I3ASSERT( pParent->IsTypeOf( CStageReady::GetClassMeta()));

	if( g_pFramework->GetClanContext()->InBattleTeam())
	{
		if( g_pFramework->GetClanContext()->GetInClanBattleLobby())
			pParent->ChangeStep(STEP_CLAN_BATTLE_LOBBY);		//	클랜전 로비 예외 처리
		else
			pParent->ChangeStep(STEP_NULL);
	}
	else
	{
		pParent->ChangeStep(STEP_NULL);
	}

	ChangeTab(NULL);
}

void CStepShop::OnCashCharge(void)
{
	//	캐시 충전 웹페이지 호출
#if !defined(LOCALE_CHINA)
	::ExcuteCashWeb();
#else
	g_pSDOA_Link->OpenWidget();
#endif
}

void CStepShop::OnGoodsBuy(UINT32 idxSlot, BOOL bDoubleClicked)
{
	I3ASSERT(m_pCurrentTab != NULL);

	SHOP_GOODS_PACK * pGoodsPack = m_pCurrentTab->GetFocusGoodsPack();
	if(pGoodsPack == NULL)
		return;

	SHOP_GOODS * pBuyGoods = pGoodsPack->GetGoods(m_pCurrentTab->GetCurrentSelectedGoodsIndex());

	//	자신이 구입 가능한지 여부 판단
	if( _CheckBuyingGoods( pBuyGoods, FALSE, bDoubleClicked))
	{
		UINT32	BuyPriceType	= m_pCurrentTab->GetSelectItemCurrentPriceType();
		((CStageReady*)m_pParent)->TogglePopup(POPUP_SHOP_BUY, pBuyGoods, &BuyPriceType);		//	구입창 띄운다.
	}
}

void CStepShop::OnGoodsGift(UINT32 idxSlot)
{
#if defined USE_SHOP_GIFT
	I3ASSERT(m_pCurrentTab != NULL);

	SHOP_GOODS_PACK * pGoodsPack = m_pCurrentTab->GetFocusGoodsPack();
	if(pGoodsPack == NULL)
		return;

	SHOP_GOODS * pGiftGoods = pGoodsPack->GetGoods(m_pCurrentTab->GetCurrentSelectedGoodsIndex());

#if defined( USE_SHOP_GIFT_CHECK_RANK)
				
	#if defined (LOCALE_KOREA)

	CShop* shop = g_pGameContext->GetShop();
	SHOP_GOODS_LIST * pGoodsList = shop->GetGoodsListData(pGiftGoods->_GoodsID);

	INT32 ItemCount = pGoodsList->_ui8ItemCount;
	for ( INT32 i = 0 ; i < ItemCount ; ++i )
	{
		if ( pGoodsList->_ppItemData[i]->_ui32ItemID == MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_EXPENDABLE, CASHITEM_GROUP_DEFCON_3, CASHITEM_DAY_NO) )
		{
			const UINT32 nLimitRank = SHOP_GIFT_REQUIREMENT_RANK;	//	제한 계급

			//	계급 이상만 확인
			if( !CheckYourRank( nLimitRank))
			{
				char szMsg[256] = "";

				// "상품을 선물하기 위해서는 {col:253,167,0,255}%s 이상{/col}의 계급이 필요합니다."
				sprintf_s( szMsg, GAME_STRING("STR_SHOP_GIFT_NEED_RANK"), GetCharaRank( nLimitRank));

				STRING_POPUP( szMsg);
				return;
			}
		}
	}

	#else

	//	계급 이상만 선물 하기 활성화
	if( !CheckYourRank( SHOP_GIFT_REQUIREMENT_RANK))
	{
		char * szMsg[256] = "";

		// "상품을 선물하기 위해서는 {col:253,167,0,255}%s 이상{/col}의 계급이 필요합니다."
		sprintf_s( szMsg, GAME_STRING("STR_SHOP_GIFT_NEED_RANK"), GetCharaRank( nLimitRank));

		STRING_POPUP( szMsg);
		return;
	}

	#endif

#endif

	//	자신이 구입 가능한지 여부 판단
	if( _CheckBuyingGoods( pGiftGoods, TRUE))
	{
		((CStageReady*)m_pParent)->TogglePopup(POPUP_SHOP_GIFT, pGiftGoods);		//	선물 가능
	}

#else

	STRING_POPUP(GAME_STRING("STR_SHOP_PREPARE_PLAN"));/*차후 지원될 예정입니다.*/

#endif
}

void CStepShop::OnGameEvent( INT32 event, INT32 arg)
{
	//	이벤트 결과를 받아 처리한다.
	switch( event )
	{
	case EVENT_BUY_GOODS:
		{
			if( EV_SUCCESSED(arg))
			{
				if ( m_pCurrentTab )
					m_pCurrentTab->OnBuyGoodsSuccess();
			}
		}
		break;
	}
}

void CStepShop::AuthStartSuccess(INT32 arg)
{		
	SHOP_GOODS * pGoods = g_pFramework->GetShopContext()->GetSelectedGoods();

	//	인증된 아이템
	if(! pGoods)
		return;


	INT32 goodsClass = GET_ITEM_FLAG_TYPE(pGoods->_ItemID);
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
			if (EV_SUCCESSED(arg))
			{
				//	아이템을 인벤에 장비
				if(g_pFramework->GetShopContext()->EquipGoods( pGoods))
					PlaySound(GUI_SND_ITEM_EQUIP);	//	장착하면 사운드
			}
			else
			{
				STRING_POPUP(GAME_STRING("STR_SHOP_FAILED_AUTH"));/*인증에 실패하였습니다. 잠시 후 다시 시도해주세요.*/
			}
		}
		break;
	case ITEM_TYPE_MAINTENANCE:		//	아이템(지속성)
	case ITEM_TYPE_EXPENDABLE:		//	아이템(소모성)
		{
			//	아이템 인증후 결과 (아이템 사용 결과는 CPopupUseItem::OnGameEvent에서 따로 처리합니다.)
		}
		break;
	}

}

BOOL CStepShop::CheckVaildGoods( SHOP_GOODS * pGoods)
{
	// Goods.pef에 등록되지 않는 아이템을 상점에서 구매못하게 한다.
	CShop* shop = g_pGameContext->GetShop();
	if (shop->IsPackageGoods( pGoods->_GoodsID))
	{		
		// 세트 아이템일 경우
		return TRUE;		//	셋트 아이템 제대로 Goods.pef에 등록 안되어있는 인도네시아 라이브 때문에 셋트 아이템 Vaild 체크는 예외처리.

	}
	else
	{		
		INT32 goodsClass = GET_ITEM_FLAG_TYPE(pGoods->_ItemID);
		INT32 num = GET_ITEM_FLAG_NUMBER( pGoods->_ItemID);

		switch((ITEM_TYPE) goodsClass)
		{
		case ITEM_TYPE_PRIMARY:
		case ITEM_TYPE_SECONDARY:
		case ITEM_TYPE_MELEE:
		case ITEM_TYPE_THROWING1:
		case ITEM_TYPE_THROWING2:
			{
				WEAPON_CLASS_TYPE type = (WEAPON_CLASS_TYPE) GET_ITEM_FLAG_CLASS( pGoods->_ItemID);
				
				CWeaponInfo * pInfo = g_pWeaponInfoDataBase->getWeaponInfo(type, num);
				if( pInfo == NULL)
					return FALSE;
			}
			break;
		case ITEM_TYPE_CHARA:
			{
				CCharaInfo * pInfo = (CCharaInfo*) g_pCharaInfoDataBase->getCharaInfoItemId( pGoods->_ItemID);
				if( pInfo == NULL)
					return FALSE;
			}
			break;
		case ITEM_TYPE_HEADGEAR:
			{
				CHARACTER_CLASS_TYPE type = (CHARACTER_CLASS_TYPE) GET_ITEM_FLAG_CLASS( pGoods->_ItemID);

				switch( type)
				{
				case CHARACTER_CLASS_HEADGEAR:	//	헬멧
					{
						CHeadInfo * pInfo = g_pCharaInfoDataBase->getHeadInfo( num);
						if( pInfo == NULL)
							return FALSE;
					}
					break;
				case CHARACTER_CLASS_ITEM:	// 베레모
					{
						CBeretInfo * pInfo = g_pCharaInfoDataBase->getBeretInfo( num);
						if( pInfo == NULL)
							return FALSE;
					}
					break;
				case CHARACTER_CLASS_FACEGEAR:	// 마스크
					{
						CMaskInfo * pInfo = g_pCharaInfoDataBase->getMaskInfo( (MASK_ID) num);
						if( pInfo == NULL)
							return FALSE;
					}
					break;
				case CHARACTER_CLASS_EVENT:	// 이벤트용 모자
					{
						CHeadInfo * pInfo = g_pCharaInfoDataBase->getEventInfo( (EVENT_EQUIP_ID) num);
						if( pInfo == NULL)
							return FALSE;
					}
					break;
				}
			}
			break;
		case ITEM_TYPE_MAINTENANCE:
		case ITEM_TYPE_EXPENDABLE:
			{
				ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo( pGoods->_ItemID);
				if( pInfo == NULL)
					return FALSE;
			}
			break;
		}
	}

	return TRUE;
}

BOOL CStepShop::_CheckBuyingGoods(SHOP_GOODS * pGoods, BOOL bGift /*= FALSE*/, BOOL bDoubleClicked)
{
	I3ASSERT(m_pCurrentTab != NULL);
	char temp[MAX_STRING_COUNT] = "";
	INT32 flag = pGoods->_ItemID;
	if( flag <= -1)
		return FALSE;

	// Goods.pef에 등록되지 않는 아이템을 상점에서 구매못하게 한다.
	if( CheckVaildGoods( pGoods) == FALSE)	
	{
		I3FATAL( "pef에 등록되지 않는 아이템입니다.");
		return FALSE;
	}
	
	CLAN_DETAIL_INFO * pClanInfo = g_pGameContext->getMyClanDetailInfo();

	switch( GET_ITEM_FLAG_TYPE(pGoods->_ItemID))
	{
		case ITEM_TYPE_PRIMARY:
		case ITEM_TYPE_SECONDARY:
		case ITEM_TYPE_MELEE:
		case ITEM_TYPE_THROWING1:
		case ITEM_TYPE_THROWING2:
		case ITEM_TYPE_CHARA:
		case ITEM_TYPE_HEADGEAR:
			{					
#if !defined( ENABLE_BUY_USER_TITLE_WEAPON)
				char szMessage[1024] = "";
				char szNeedUserTitle[MAX_STRING_COUNT] = "";

				//	호칭 무기라도 캐시면 구입할 수 있다.
				#if defined( ENABLE_BUY_CASH_WEAPON_IF_NEED_USER_TITLE) 
				if( m_pCurrentTab->GetSelectItemCurrentPriceType() & GOODS_PRICE_CASH)
				{
					return TRUE;
				}
				#endif

				//	구입하기 전에 호칭무기인지 검사하고 구입 가능한지 검사
				if( CheckGoodsForUserTitle( pGoods, szNeedUserTitle) == FALSE)
				{
					//	호칭 무기라도 캐시면 구입할 수 있다.
					#if defined( ENABLE_BUY_CASH_WEAPON_IF_NEED_USER_TITLE) 
					// 더블 클릭으로 무기를 살경우 호칭제한무기라도 캐쉬로는 살 수 있기때문에 그것에 대한 예외처리를 합니다.
					if( bDoubleClicked )
					{
						SHOP_GOODS_PACK * pGoodsPack = m_pCurrentTab->GetFocusGoodsPack();
						if( pGoodsPack)
						{
							INT32 goodsCount = pGoodsPack->GetGoodsCount();

							for(INT32 i=0; i<goodsCount; i++)
							{
								SHOP_GOODS * pSelectedGoods = pGoodsPack->GetGoods(i);

								UINT32 CashPrice = 0;
								CShop* shop = g_pGameContext->GetShop();
								shop->GetGoodsPrice(pSelectedGoods->_GoodsID, NULL, &CashPrice);
								if (0 < CashPrice)
								{
									memcpy( pGoods, pSelectedGoods, sizeof(SHOP_GOODS));
									return TRUE;
								}
							}
						}
					}
					#endif

					if( bGift)
					{
						/*해당 아이템은 %s 호칭을\n획득하여야 선물할 수 있습니다.\n(호칭 획득 후 선물 가능)*/
						sprintf_s( szMessage, GAME_STRING("STR_SHOP_REQUIRE_DESIGNATION_TO_GIFT"), szNeedUserTitle);
					}
					else
					{
						/*해당 아이템은 %s 호칭을\n획득하여야 구입할 수 있습니다.\n(호칭 획득 후 구입 가능)*/
						sprintf_s( szMessage, GAME_STRING("STR_SHOP_REQUIRE_DESIGNATION_TO_BUY"), szNeedUserTitle);
					}

					STRING_POPUP( szMessage);	

					return FALSE;
				}

#endif
			}
			break;
		case ITEM_TYPE_MAINTENANCE:
		case ITEM_TYPE_EXPENDABLE:
			{				
				if( !bGift && !g_pFramework->GetClanContext()->IsMaster())	//	마스터만 구입 가능 여부 판단 (선물 예외)
				{
					//	캐시 아이템중 클랜 마스터만 구입, 사용이 가능하다.
					//	클랜 마스터용 아이템인지 여부 판단.
#if defined(CHANGE_ITEM_USE_CHECK)
					ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(flag);
					if ( pInfo != NULL && pInfo->GetPurchaseAuthority() == PURCHASE_AUTH_CLAN_MASTER )
					{
						const char * Msg = pInfo->GetPurchaseAuthorityDesc();	// 구입할 수 없는 메시지
						CAPTION_POPUP(Msg, GAME_STRING("STR_SHOP_CANNOT_ITEM_BUYING"));
						return FALSE;
					}
#else
					if( CheckCashItemOnlyClanMaster( flag))
					{
						CAPTION_POPUP(GAME_STRING("STR_SHOP_ONLY_MASTER"), 
									GAME_STRING("STR_SHOP_CANNOT_ITEM_BUYING"));/*아이템 구입 불가*//*클랜 마스터만\n 구매할 수 있는 아이템입니다.*/
						return FALSE;
					}
#endif
				}

				if( !bGift)		//	선물하기는 구매에 제한이 없다.
				{
					//	아이템 별로 구매시 예외처리
					switch( GET_ITEM_FLAG_CLASS( flag))
					{
					case CASHITEM_GROUP_CLAN_PERSON_PLUS_50:
						{
							//	클랜원 증가 아이템은 최대 회원수를 넘을시 구매 불가
							if( pClanInfo->_maxPerson >= MAX_MEMBER_COUNT)
							{
								sprintf_s( temp, GAME_STRING("STR_SHOP_LIMITED_CLAN_MEMBER"), MAX_MEMBER_COUNT);/*아이템을 구입할 수 없습니다.\n \n 클랜 최대 인원수는\n {col:255,153,51,255}%d{/col}명으로 제한되어 있습니다.*/
								CAPTION_POPUP( temp, GAME_STRING("STR_SHOP_CANNOT_ITEM_BUYING"));/*아이템 구입 불가*/

								return FALSE;
							}
						}
						break;
					}
				}
			}
			break;
	}

	return TRUE;

}



void CStepShop::_GetGoodsUseDay(BOOL IsCountItem, UINT8 Type, UINT32 Arg, char * pOut, const size_t sizeOutMax)
{
	I3ASSERT( pOut != NULL);

	switch(Type)
	{
	case ITEM_ATTR_TYPE_AUTH:		//인증한 아이템
		{
			// 기간제 아이템이면.
			if ( IsCountItem != TRUE )
			{
				UINT32 term = 0;
				if( Arg > 0)
				{
					term = g_pGameContext->GetInventorySecTime(Arg);
					term -= ITEM_TERM_ADJUST_VALUE;		//	김성백 실장님 부탁으로 코드 추가합니다. 기간 보정 - komet
					if( 0 > (INT32)term ) term = 0;
					term = ConvertSecToUseHour(term);
				}
				FormatHaveItemTermText(pOut, sizeOutMax, term);
			}
			// 개수제라면
			else
			{
				sprintf_s(pOut, sizeOutMax, "%s : {col:255,255,255,255}%d%s{/col}", GAME_STRING("STBL_IDX_ITEM_HAVE_COUNT"), Arg, GAME_STRING("STBL_IDX_ITEM_COUNT_UNIT"));
			}
		}
		break;
	case ITEM_ATTR_TYPE_BUY:		//구입만한 아이템(기간제)
		{
			if ( IsCountItem != TRUE )
			{
				UINT32 term = ConvertSecToUseHour(Arg);
				if(term > 24)
				{
					term = term /24;
					//	보유중 : %d일
					sprintf_s(pOut, sizeOutMax, "%s : {col:255,255,255,255}%d%s{/col}", GAME_STRING("STBL_IDX_ITEM_HAVE_TERM"), term, GAME_STRING("STBL_IDX_ITEM_TERM_UNIT"));
				}
				else
				{
					//	보유중 : %d시간
					sprintf_s(pOut, sizeOutMax, "%s : {col:255,255,255,255}%d%s{/col}", GAME_STRING("STBL_IDX_ITEM_HAVE_TERM"), term, GAME_STRING("STR_UI_HOUR"));
				}
			}
			// 개수제라면
			else
			{
				sprintf_s(pOut, sizeOutMax, "%s : {col:255,255,255,255}%d%s{/col}", GAME_STRING("STBL_IDX_ITEM_HAVE_COUNT"), Arg, GAME_STRING("STBL_IDX_ITEM_COUNT_UNIT"));
			}
		}
		break;
	default:
		{
#if defined (ENABLE_INVEN_DISPLAY_OBT)
			//	보유중 : 무제한
			sprintf_s(pOut, "%s : {col:255,255,255,255}%s{/col}", GAME_STRING("STBL_IDX_ITEM_HAVE_TERM"), GAME_STRING("STBL_IDX_ITEM_UNLIMITED"));
#else
			i3String::Copy(pOut, "", sizeOutMax );
#endif
		}
		break;
	}
}


void CStepShop::TogglePopupExtension()
{
	((CStageReady*)m_pParent)->TogglePopup(POPUP_EXTENSION);
}


void CStepShop::_PrintSaleText(i3GuiEditBox* pSaleTextCtrl[SALE_TEXT_MAX], UINT32 BuyYupe)
{
	I3ASSERT( pSaleTextCtrl != NULL );
	I3ASSERT( pSaleTextCtrl[SALE_TEXT_FIRST] != NULL );
	I3ASSERT( pSaleTextCtrl[SALE_TEXT_SECOND] != NULL );

	pSaleTextCtrl[SALE_TEXT_FIRST]->SetEnable(FALSE);
	pSaleTextCtrl[SALE_TEXT_SECOND]->SetEnable(FALSE);

	// 상품의 판매 정보를 보여준다. 아래에서 위로 어떤 상품인지를 보여준다.
	i3GuiEditBox ** pSaleTextCtrl_iter = pSaleTextCtrl;
	if( BuyYupe & GOODS_PRICE_CASH )
	{
		(*pSaleTextCtrl_iter)->SetEnable(TRUE);
		(*pSaleTextCtrl_iter)->SetTextColor(GetColor(GCT_YELLOW));
		(*pSaleTextCtrl_iter)->SetText(GAME_STRING("STR_SHOP_CAN_BUYING_CASH"));	//톨 아이템
		pSaleTextCtrl_iter++;
	}

	if ( BuyYupe & GOODS_PRICE_POINT )
	{
		(*pSaleTextCtrl_iter)->SetEnable(TRUE);
		(*pSaleTextCtrl_iter)->SetTextColor(GetColor(GCT_VALUE));
		(*pSaleTextCtrl_iter)->SetText(GAME_STRING("STR_SHOP_POINTITEM"));			//포인트 아이템
	}
}


void	CStepShop::SetEffectButton(i3GuiButton * pButton)
{
	m_BtnEffector.Set(pButton);
}


void	CStepShop::ChangeTab(CTabBase * pNewTab, bool ResetVestige /*= false*/)
{
	if ( pNewTab != NULL && TRUE != pNewTab->OnOpen() )
	{
		pNewTab->OnClose();
		return;
	}


	if ( m_pCurrentTab != NULL && m_pCurrentTab != pNewTab)
	{
		if( ResetVestige )
		{
			m_pCurrentTab->_ResetVestige();
		}
		m_pCurrentTab->OnClose();
	}

	m_pCurrentTab = pNewTab;

	//if ( m_pCurrentTab != NULL )
	//	m_pCurrentTab->OnClose();

	//if ( pNewTab != NULL )
	//	pNewTab->OnOpen();
	//m_pCurrentTab = pNewTab;

}
