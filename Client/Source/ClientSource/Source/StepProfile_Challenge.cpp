#include "pch.h"
#include "StepProfile_Challenge.h"
#include "GuiNotifyReceiver.h"
#include "StepProfile_CardShopWin.h"
#include "StepProfile_CardInfo.h"
#include "StepProfile_CardCompleteWin.h"
#include "QuestTextDictionary.h"
#include "StepProfile.h"
#include "StepProfileDef.h"

I3_CLASS_INSTANCE( CStepProfile_Challenge, i3ElementBase);



CStepProfile_Challenge::CStepProfile_Challenge()
{
	m_pNotifyBase = NULL;

	m_pChallengeSect = NULL;
	m_pCommonKey = NULL;
		
	i3mem::FillZero(m_commonSlot, sizeof(m_commonSlot));


	m_pPrizeNote = NULL;

	m_pPrizeRibbonDummy = NULL;
	m_pPrizeMedalDummy = NULL;
	m_pPrizeDescription = NULL;
	m_pPrizeNameKey = NULL;
	m_pPrizeNameValue = NULL;
	m_pPrizeExpKey = NULL;
	m_pPrizeExpValue = NULL;
	m_pPrizePointKey = NULL;
	m_pPrizePointValue = NULL;

	m_pCardKey = NULL;

	m_pCardBg = NULL;	
	m_pCard = NULL;	
	m_pCardTypeCombo = NULL;
	m_pCardTypeButton = NULL;
	m_pCardTypePopup = NULL;

	m_pCardBuyOne = NULL;
	m_pCardBuyAll = NULL;
	m_pCardDeleteAll = NULL;
	m_selectedCardSlot = -1;
	m_selectedCardSet = -1;
	m_selectedQuest = -1;

	m_pCardShopWin = NULL;


	i3mem::FillZero(m_cardSlot, sizeof(m_cardSlot));


	m_pCardInfo = NULL;
	m_pCcBulletKey = NULL;
	m_pCardBulletKey = NULL;
	

	// 카드정보 윈도우
	m_pCardInfoBG	= NULL;
	m_pCardInfoWin2 = NULL; 
	m_pCardInfoConfirm2 = NULL;

	m_pCardCompleteWin = NULL;

	m_bIsMyInfoView	= true;

}

CStepProfile_Challenge::~CStepProfile_Challenge()
{
	I3_SAFE_RELEASE( m_pCardShopWin);
	I3_SAFE_RELEASE( m_pCardInfoBG);
	I3_SAFE_RELEASE( m_pCardCompleteWin);
}

void CStepProfile_Challenge::SetEnable( BOOL bEnable)
{
	m_pChallengeSect->SetEnable( bEnable);

	if( bEnable)
	{
		Event_ChangeUserInfo( m_selectedCardSlot);
	}
}

BOOL CStepProfile_Challenge::LinkControl( CGuiNotifyReceiver * pBase, i3BinList * plist, i3GameNode * pControl)
{
	INT32 i;
	char strName[ MAX_STRING_COUNT];

	m_pNotifyBase = pBase;

	LinkControlEx(plist, "prChallengeSect", m_pChallengeSect, -1);
	LinkControlEx(plist, "prCommonKey", m_pCommonKey, -1);
	
	for( i = 0; i < MAX_COMMON_SLOT; i++)
	{
		sprintf_s(strName, "prCslot%d", i + 1);
		LinkControlEx(plist, strName, m_commonSlot[i]._pSlot, GCI_PRS_B_CSLOT_1 + i);

		sprintf_s(strName, "prCslotState%d", i + 1);
		LinkControlEx(plist, strName, m_commonSlot[i]._pState, -1);

		sprintf_s(strName, "prCslotNotice%d", i + 1);
		LinkControlEx(plist, strName, m_commonSlot[i]._pNotice, -1);
	}


	LinkControlEx(plist, "prPrizeNote", m_pPrizeNote, -1);

	LinkControlEx(plist, "prPrizeRibbonDummy", m_pPrizeRibbonDummy, -1);
	LinkControlEx(plist, "prPrizeMedalDummy", m_pPrizeMedalDummy, -1);
	LinkControlEx(plist, "prPrizeDescription", m_pPrizeDescription, -1);
	LinkControlEx(plist, "prPrizeNameKey", m_pPrizeNameKey, -1);
	LinkControlEx(plist, "prPrizeNameValue", m_pPrizeNameValue, -1);
	LinkControlEx(plist, "prPrizeExpKey", m_pPrizeExpKey, -1);
	LinkControlEx(plist, "prPrizeExpValue", m_pPrizeExpValue, -1);
	LinkControlEx(plist, "prPrizePointKey", m_pPrizePointKey, -1);
	LinkControlEx(plist, "prPrizePointValue", m_pPrizePointValue, -1);

	LinkControlEx(plist, "prCardKey", m_pCardKey, -1);

	LinkControlEx(plist, "prCardBg", m_pCardBg, GCI_PRS_B_CHALLENGE_COMPLATE);
	LinkControlEx(plist, "prCard", m_pCard, -1);
	LinkControlEx(plist, "prCardTypeCombo", m_pCardTypeCombo, -1);
	LinkControlEx(plist, "prCardTypeButton", m_pCardTypeButton, GCI_PRS_B_CARD_TYPE);		
	LinkControlEx(plist, "prCardTypePopup", m_pCardTypePopup, GCI_PRS_L_CARD_TYPE);

	m_pCardTypePopup->CreateTextEx( g_pConfigEx->GetQuest().GetCardCount(),  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );

	LinkControlEx(plist, "prCardBuyOne", m_pCardBuyOne, GCI_PRS_B_CARD_BUY_ONE);
	LinkControlEx(plist, "prCardBuyAll", m_pCardBuyAll, GCI_PRS_B_CARD_BUY_ALL);
	LinkControlEx(plist, "prCardDeleteAll", m_pCardDeleteAll, GCI_PRS_B_CARD_DELETE_ALL);

	for( i = 0; i < CARD_SLOT_COUNT; i++)
	{
		sprintf_s(strName, "prCardSlot%d", i + 1);
		LinkControlEx(plist, strName, m_cardSlot[i]._pSlot, GCI_PRS_B_CARD_SLOT_1 + i);
		
		sprintf_s(strName, "prCard%d", i + 1);
		LinkControlEx(plist, strName, m_cardSlot[i]._pCard, -1);
	}

	LinkControlEx(plist, "prCardInfo", m_pCardInfo, GCI_PRS_B_CARD_INFO);
	LinkControlEx(plist, "prCcBulletKey", m_pCcBulletKey, -1);
	LinkControlEx(plist, "prCardBulletKey", m_pCardBulletKey, -1);
	


	// CardShopWin
	m_pCardShopWin = CStepProfile_CardShopWin::NewObject();
	m_pCardShopWin->LinkControl( pBase, plist, pControl);

	// 임무카드 정보 윈도우
	m_pCardInfoBG = CStepProfile_CardInfo::NewObject();
	m_pCardInfoBG->LinkControl( pBase, plist, pControl);

	LinkControlEx(plist, "prCardInfoWin2", m_pCardInfoWin2, -1);	
	LinkControlEx(plist, "prciConfirm2", m_pCardInfoConfirm2, GCI_PRS_S_CARD_INFO_CONFIRM);

	// CardComplateWin
	m_pCardCompleteWin = CStepProfile_CardCompleteWin::NewObject();
	m_pCardCompleteWin->LinkControl( pBase, plist, pControl);


	return TRUE;
}

void CStepProfile_Challenge::Init( void)
{
	INT32 i;

	m_bIsMyInfoView = true;
	

	m_pCcBulletKey->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCcBulletKey->SetTextColor( GetColor(GCT_DEFAULT));
	m_pCcBulletKey->SetText( GAME_STRING("STR_TBL_PROFILE_CHALLENGE_CARD_CONTENTS"));/*임무 내용*/

	
	m_pCardBulletKey->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCardBulletKey->SetTextColor( GetColor(GCT_DEFAULT));
	m_pCardBulletKey->SetText( GAME_STRING("STR_TBL_PROFILE_REWARD_TEXT"));/*보상*/


	// 공통 임무	
	m_pCommonKey->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);		
	m_pCommonKey->SetTextColor( GetColor(GCT_DEFAULT));
	m_pCommonKey->SetText( GAME_STRING("STR_TBL_PROFILE_COMMON_CHALLENGE"));/*공통임무*/
	
	for( i = 0; i < MAX_COMMON_SLOT; i++)
	{
		INIT_BUTTON_EX(m_commonSlot[i]._pSlot);
		m_commonSlot[i]._pState->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_commonSlot[i]._pState->setInputDisable(TRUE);
		m_commonSlot[i]._pNotice->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_commonSlot[i]._pNotice->setInputDisable(TRUE);
	}

	for( i = 0; i < CARD_SLOT_COUNT; i++)
	{
		INIT_CARD_EX(m_cardSlot[i]._pSlot);
		m_cardSlot[i]._pCard->setInputDisable(TRUE);
	}
	m_pCardBuyOne->SetEnable(FALSE);
	m_pCardBuyAll->SetEnable(FALSE);
	m_pCardDeleteAll->SetEnable(FALSE);
	m_pCardInfo->SetEnable(FALSE);


	// 보상 설명
	m_pPrizeNote->SetEnable(FALSE);
	
	m_pPrizeDescription->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pPrizeDescription->SetTextColor( GetColor(GCT_DEFAULT));

#if defined(GUI_ADJUST_RUSSIA)
	m_pPrizeNameKey->ReCreateTextEx(GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_SIZE_ANY_TEXT);
	m_pPrizeExpKey->ReCreateTextEx(GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_SIZE_ANY_TEXT);
	m_pPrizePointKey->ReCreateTextEx(GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_SIZE_ANY_TEXT);
#endif

	m_pPrizeRibbonDummy->SetEnable(FALSE);
	m_pPrizeMedalDummy->SetEnable(FALSE);

	m_pPrizeNameKey->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPrizeNameValue->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPrizeExpKey->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPrizeExpValue->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPrizeExpKey->SetText( GAME_STRING("STR_TBL_PROFILE_GET_EXP"));/*획득 경험치 :*/

	m_pPrizePointKey->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPrizePointValue->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);

	m_pPrizeNameKey->SetTextColor( GetColor(GCT_DEFAULT));
	m_pPrizeNameValue->SetTextColor( GetColor(GCT_DEFAULT));
	m_pPrizeNameKey->SetText( GAME_STRING("STR_TBL_PROFILE_PRIZE_NAME"));/*명칭 :*/

	m_pPrizeExpKey->SetTextColor( GetColor(GCT_DEFAULT));
	m_pPrizeExpValue->SetTextColor( GetColor(GCT_DEFAULT));

	m_pPrizePointKey->SetTextColor( GetColor(GCT_DEFAULT));
	m_pPrizePointValue->SetTextColor( GetColor(GCT_DEFAULT));
	m_pPrizePointKey->SetText( GAME_STRING("STR_TBL_PROFILE_GET_POINT"));/*획득 Point :*/

	INIT_CARD_EX(m_pCardBg);
	m_pCardBg->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pCard->setInputDisable(TRUE);

	// 임무카드
	m_pCardKey->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCardKey->SetTextColor( GetColor(GCT_DEFAULT));
	m_pCardKey->SetText( GAME_STRING("STBL_IDX_SHOP_FILTER_CARD") );/*카드*/

	// 포인트 블랭크 카드
	m_pCardTypeCombo->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCardTypeCombo->SetTextColor( GetColor(GCT_FOCUS));	
	INIT_COMBO_EX( m_pCardTypeButton);
	
	m_pCardTypePopup->SetSelColor( 100, 100, 100, 100);
	m_pCardTypePopup->SetTextColor( GetColor(GCT_DEFAULT));
	m_pCardTypePopup->SetExclusiveEnableControl( TRUE);
	m_pCardTypePopup->SetEnable( FALSE);	

	m_pCardShopWin->Init();

	m_pCardInfoBG->Init();

	// 임무카드 정보 윈도우
	m_pCardInfoBG->SetEnable(FALSE);
	m_pCardInfoWin2->SetEnable(FALSE);

	{
		INIT_BUTTON_EX( m_pCardInfoConfirm2);
		{
			m_pCardInfoConfirm2->CreateCaption( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE);
			m_pCardInfoConfirm2->SetCaptionColor( GetColor(GCT_VALUE));
			m_pCardInfoConfirm2->SetCaption( GAME_STRING("STR_TBL_PROFILE_CONFIRM"));/*확인*/
		}
	}

	m_pCardCompleteWin->Init();



	{
		INIT_BUTTON_EX( m_pCardBuyAll);
		{
			m_pCardBuyAll->CreateCaption( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE);
			m_pCardBuyAll->SetCaptionColor( GetColor(GCT_VALUE));
			m_pCardBuyAll->SetCaption( GAME_STRING("STR_TBL_PROFILE_CARD_BUY"));	/*구입*/

		}
	}

	{
		INIT_BUTTON_EX( m_pCardDeleteAll);
		{
			m_pCardDeleteAll->CreateCaption( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE);
			m_pCardDeleteAll->SetCaptionColor( GetColor(GCT_VALUE));
			m_pCardDeleteAll->SetCaption( GAME_STRING("STR_TBL_PROFILE_CARD_DELETE"));/*삭제*/
		}
	}

	{
		INIT_BUTTON_EX( m_pCardInfo);
		{
			m_pCardInfo->CreateCaption( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE);
			m_pCardInfo->SetCaptionColor( GetColor(GCT_VALUE));
			m_pCardInfo->SetCaption( GAME_STRING("STR_TBL_PROFILE_CARD_INFO"));/*카드정보*/
		}
	}


}

void CStepProfile_Challenge::Update( REAL32 tm)
{	
//	_UpdateChallengePrizeState();
	//_UpdateChallengeCardInfo();
}

void CStepProfile_Challenge::GuiNotify( I3GUI_CONTROL_NOTIFY* pNotify)
{
	m_pCardInfoBG->GuiNotify( pNotify);

	switch( pNotify->m_nID)
	{
	case GCI_PRS_B_CARD_SLOT_1:
	case GCI_PRS_B_CARD_SLOT_2:
	case GCI_PRS_B_CARD_SLOT_3:
	case GCI_PRS_B_CARD_SLOT_4:
	case GCI_PRS_B_CARD_SLOT_5:
	case GCI_PRS_B_CARD_SLOT_6:
	case GCI_PRS_B_CARD_SLOT_7:
	case GCI_PRS_B_CARD_SLOT_8:
	case GCI_PRS_B_CARD_SLOT_9:
	case GCI_PRS_B_CARD_SLOT_10:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnCardSlot(pNotify->m_nID - GCI_PRS_B_CARD_SLOT_1);
		break;
	case GCI_PRS_B_CARD_BUY_ALL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnCardBuyAll();
		break;
	case GCI_PRS_B_CARD_DELETE_ALL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnCardDeleteAll();
		break;
	case GCI_PRS_B_CARD_INFO:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnCardInfo();
		break;
	case GCI_PRS_B_SHOP_CONFIRM:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnShopConfirm();
		break;
	case GCI_PRS_B_SHOP_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnShopCancel();
		break;
	case GCI_PRS_B_CHALLENGE_COMPLATE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnComplateChallenge();
		break;
	case GCI_PRS_B_CARD_TYPE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnCardTypeCombo();
		break;
	case GCI_PRS_L_CARD_TYPE:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent)OnCardTypeSelect(pNotify->m_nPriParam);
		break;
	case GCI_PRS_B_COMPLATE_CONFIRM:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnComplateConfirm();
		break;
	case GCI_PRS_S_CARD_INFO_CONFIRM:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnCardInfoConfirm();
		break;

	// 
	case GCI_PRS_L_PROFILE_TARGET :
	case GCI_PRS_L_PROFILE_USERLIST :
		if( pNotify->m_nEvent == I3GUI_POPUPLIST_NOTIFY_CLICKED)	Event_ChangeUserInfo();
		break;

	case GCI_PRS_B_PROFILE_TARGET :
	case GCI_PRS_B_PROFILE_USERLIST :
	case GCI_PRS_B_PROFILE_USER_SLOT_1 :
	case GCI_PRS_B_PROFILE_USER_SLOT_2 :
	case GCI_PRS_B_PROFILE_USER_SLOT_3 :
	case GCI_PRS_B_PROFILE_USER_SLOT_4 :
	case GCI_PRS_B_PROFILE_USER_SLOT_5 :
		if( pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)		Event_ChangeUserInfo();
		break;
	}
}

void CStepProfile_Challenge::Reset( void)
{	
}

void CStepProfile_Challenge::OnEnd( void)
{
	if (m_pCardInfoBG->isEnable())
	{
		OnCardInfoConfirm();		
	}
	if (m_pCardInfoWin2->isEnable())
	{
		OnCardInfoConfirm();		
	}
}


void CStepProfile_Challenge::OnCardSlot(INT32 cardIdx)
{
	if( cardIdx < 0 || cardIdx >= CARD_SLOT_COUNT)
		return;

	CQuestManager * pQuestManager = g_pGameContext->getQuestManager(true);
	if( pQuestManager == NULL )
		return;

	if( m_bIsMyInfoView)
	{	
		if (m_selectedCardSlot == cardIdx)	return;

		// 진행 카드 변경 및 활성화
		if (_IsPossibilityActiveCard(cardIdx))
		{
			pQuestManager->ChangeActiveCard(cardIdx);
		}
		else
		{
			// 튜토리얼 임무카드는 없는 경우 선택안되게한다.
			CARDSET_ID CardsetID = GetSelectedCardsetID();
 			if ( QUEST_CARDSET_TYPE_NONE == CardsetID
				|| QUEST_CARDSET_TYPE_TUTORIAL == CardsetID)
			{
				return;
			}
		}

		m_selectedCardSlot = cardIdx;
		m_selectedQuest = -1;
	}

	m_pPrizeNote->SetEnable(FALSE);

	Event_ChangeUserInfo( cardIdx);
}


BOOL CStepProfile_Challenge::OnPopupConfirm( void)
{
	if (m_pCardInfoBG->isEnable())
	{
		OnCardInfoConfirm();
		return TRUE;
	}
	if (m_pCardInfoWin2->isEnable())
	{
		OnCardInfoConfirm();
		return TRUE;
	}
	return FALSE;
}

BOOL CStepProfile_Challenge::OnPopupCancel( void)
{
	if (m_pCardCompleteWin->isEnable())
	{
		OnComplateConfirm();
		return TRUE;
	}
	if (m_pCardInfoWin2->isEnable())
	{
		OnCardInfoConfirm();
		return TRUE;
	}
	if (m_pCardInfoBG->isEnable())
	{
		OnCardInfoConfirm();
		return TRUE;
	}

	if (m_pCardShopWin->isEnable())
	{
		OnShopCancel();
		return TRUE;
	}

	return FALSE;
}

void CStepProfile_Challenge::OnShopConfirm(void)
{
	// 상품 구입
	{
		_BuyCardSet(GetSelectedCardsetID());
	}

	// 구입 윈도우 비활성화
	m_pCardShopWin->SetEnable(FALSE);

	((CStepProfile*)m_pNotifyBase)->EnableInputProfileWin();

	Event_ChangeUserInfo( m_selectedCardSlot);
}

void CStepProfile_Challenge::OnShopCancel(void)
{
	// 구입 윈도우 비활성화
	m_pCardShopWin->SetEnable(FALSE);

	((CStepProfile*)m_pNotifyBase)->EnableInputProfileWin();
}

void CStepProfile_Challenge::OnComplateChallenge(void)
{
	STRING_POPUP_C( GAME_STRING("STR_TBL_PROFILE_NOTICE_NEW_CHALLENGE_CARD_MESSAGE"), MAKE_CALLBACK(CbComplateChallenge), this);/*새로운 임무카드를 받을 수 있습니다.*/

	Event_ChangeUserInfo( m_selectedCardSlot);
}

void CStepProfile_Challenge::OnComplateConfirm(void)
{
	// 완료된 카드셋을 보상과 교환하여야한다.
	// GUI 복귀
	m_pCardCompleteWin->SetEnable(FALSE);
	((CStepProfile*)m_pNotifyBase)->EnableInputProfileWin();

	Event_ChangeUserInfo( m_selectedCardSlot);
}

void CStepProfile_Challenge::OnCardInfoConfirm(void)
{
	// GUI 복귀
	m_pCardInfoBG->SetEnable(FALSE);
	m_pCardInfoWin2->SetEnable(FALSE);

	((CStepProfile*)m_pNotifyBase)->EnableInputProfileWin();
}

void CStepProfile_Challenge::OnUpdateNote(UINT32 idxChallenge)
{

}

void CStepProfile_Challenge::OnUpdatePrize( UINT32 idxPrize)
{
	// 보상을 표시한다.
	UINT32 prize = 0;
	UINT32 point = 0;
	UINT32 exp = 0;


	CQuestManager * pQuestManager = g_pGameContext->getQuestManager(m_bIsMyInfoView);
	if( pQuestManager == NULL)
		return;


	BOOL HasCardset = pQuestManager->HasCardSet( GetSelectedCardsetID() );


	if( HasCardset && pQuestManager->IsActivedCardset() && ((pQuestManager->getActiveCardIndex() == idxPrize) || idxPrize == -1 ) )
	{
		CCardInfo * pActiveCardInfo = pQuestManager->getActiveCard();
		I3ASSERT(pActiveCardInfo != NULL);
		if ( pActiveCardInfo )
		{
			prize = pActiveCardInfo->GetPrize();
			point = pActiveCardInfo->GetPoint();
			exp = pActiveCardInfo->GetExp();
		}
	}

	char descriptionText[MAX_STRING_COUNT] = {};
	char nameText[MAX_STRING_COUNT] = {};
	BOOL EnableRibbon	= FALSE;
	BOOL EnableMedal	= FALSE;

	I3ASSERT( prize < 0xFF);
	BADGE_TYPE BadgeType = PrizeToBadgeType( (UINT8)prize);

	switch (BadgeType)
	{
		// 약장
	case BADGE_TYPE_RIBBON:
		EnableRibbon = TRUE;

		// 설명, 이름
		g_pFramework->SetRibbonShape(m_pPrizeRibbonDummy, FIXED_BADGE_NUM_RIBBON);
		sprintf_s( nameText, GAME_STRING("STR_TBL_PROFILE_RIBBON_TEXT2"));	/*약장*/
		g_pQuestTextDictionary->GetRibbonDescriptionString( descriptionText, MAX_STRING_COUNT, FIXED_BADGE_NUM_RIBBON);
		break;

		// 휘장
	case BADGE_TYPE_ENSIGN:
		EnableMedal = TRUE;

		// 설명, 이름
		g_pFramework->SetEnsignShape(m_pPrizeMedalDummy, FIXED_BADGE_NUM_ENSIGN);
		sprintf_s( nameText, GAME_STRING("STR_TBL_PROFILE_ENSIGN_TEXT2"));	/*휘장*/
		g_pQuestTextDictionary->GetEnsignDescriptionString( descriptionText, MAX_STRING_COUNT, FIXED_BADGE_NUM_ENSIGN);
		break;

		// *
		// * 여기서 어떻거 나눠야되는지 모르겠음.
		// * 
		// 훈장 이미지	
	case BADGE_TYPE_MEDAL:
		EnableMedal = TRUE;

		// 설명, 이름
		g_pFramework->SetMedalShape( m_pPrizeMedalDummy, FIXED_BADGE_NUM_MEDAL);
		sprintf_s( nameText, GAME_STRING("STR_TBL_PROFILE_MEDALS_TEXT2"));	/*훈장*/
		g_pQuestTextDictionary->GetMedalDescriptionString(descriptionText, MAX_STRING_COUNT, FIXED_BADGE_NUM_MEDAL);
		break;
		// 마스터 훈장
	case BADGE_TYPE_MASTER:
		EnableMedal = TRUE;

		// 설명, 이름
		g_pFramework->SetMedalShape( m_pPrizeMedalDummy, FIXED_BADGE_NUM_MASTER);
		sprintf_s( nameText, GAME_STRING("STR_TBL_PROFILE_MEDALS_TEXT2"));	/*훈장*/
		g_pQuestTextDictionary->GetMedalDescriptionString(descriptionText, MAX_STRING_COUNT, FIXED_BADGE_NUM_MASTER);
		break;
	}


	m_pPrizeRibbonDummy->SetEnable(EnableRibbon);
	m_pPrizeMedalDummy->SetEnable(EnableMedal);
	m_pPrizeDescription->SetText(descriptionText);
	m_pPrizeNameValue->SetText(nameText);


	// 포인트, 경험치 표시
	char pointText[MAX_STRING_COUNT];
	char expText[MAX_STRING_COUNT];

	sprintf_s(pointText, "%d", point );
	sprintf_s(expText, "%d", exp );
	m_pPrizePointValue->SetText(pointText);
	m_pPrizeExpValue->SetText(expText);


	// 상세정보창 교체
	m_pPrizeNote->SetEnable(TRUE);

	m_selectedQuest = -1;
}

void CStepProfile_Challenge::OnCardTypeSelect(INT32 idx)
{
	m_selectedCardSet = idx;
	m_selectedCardSlot = -1;

	_ChangeActiveCardSet(GetSelectedCardsetID());

	m_selectedQuest = -1;
	m_pPrizeNote->SetEnable(FALSE);

	Event_ChangeUserInfo( m_selectedCardSlot);
}

void CStepProfile_Challenge::OnCardBuyAll(void)
{
	// 선택된 카드의 인덱스
	CARDSET_ID cardSetId = GetSelectedCardsetID();

	if(!_CheckBuyQuestCard(cardSetId))
		return; 

	CQuestManager * pQuestManager = g_pGameContext->getQuestManager();
	if( pQuestManager == NULL)
		return;

	if( pQuestManager->HasCardSet(cardSetId) )
	{
		STRING_POPUP(GAME_STRING("STR_TBL_PROFILE_CARDSET_DUPLICATE"));
		return;
	}

	m_pCardShopWin->OnCardBuyAll( cardSetId);

	
	// 구입 윈도우 활성화
	m_pCardShopWin->SetEnable(TRUE);

	((CStepProfile*)m_pNotifyBase)->DisableInputProfileWin();
}

void CStepProfile_Challenge::OnCardDeleteAll(void)
{
	// 가지고 있는 카드셋만 삭제가 가능하다.
	// 활성화되어있다면, 가지고 있는 카드셋이다.
	if (_IsActiveViewCardSet())
	{
		STRING_POPUP_Q(GAME_STRING("STR_TBL_PROFILE_DELETE_CARD_MESSAGE"), MAKE_CALLBACK(CbDeleteCardSet), this);/*선택하신 종류의 모든 임무카드는\n삭제되며, 복구할 수 없습니다.\n \n그래도 삭제하시겠습니까?*/
	}
}

void CStepProfile_Challenge::OnCardInfo(void)
{
	if( m_selectedCardSet == -1 || m_selectedCardSet >= 255)
	{
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
		I3WARN( "invalid selectedcardset.");
#endif
		return;
	}

	if( GetSelectedCardsetID() == QUEST_CARDSET_TYPE_EVENT)
	{
		m_pCardInfoWin2->SetEnable(TRUE);
	}
	else 
	{
		m_pCardInfoBG->SetEnable(TRUE);

		((CStepProfile*)m_pNotifyBase)->DisableInputProfileWin();

		// 카드정보
		CARDSET_ID cardSetId = GetSelectedCardsetID();
		m_pCardInfoBG->OnCardInfo( cardSetId);
	}
}

void CStepProfile_Challenge::OnCardTypeCombo(void)
{
	//	각 나라별로 이벤트 임무카드를 사용하지 않는다면 Config/ConfigEx.PEF 파일에서 
	//	QuestCard 항목의 13_EVENT 에서 값을 0으로 하고 파일을 배포하면 해당 클라이언트에서는 이벤트 임무카드가 안보입니다.
	//	- komet
	INT32 currentCardSetCount;
	char cardSetName[MAX_STRING_COUNT];
	I3COLOR color;

	m_pNotifyBase->ExclusionPopup( m_pCardTypePopup);
	
	CQuestManager* pQuestManager = g_pGameContext->getQuestManager(m_bIsMyInfoView);
	if( pQuestManager == NULL)
		return;

	currentCardSetCount = g_pConfigEx->GetQuest().GetCardCount();

	m_pCardTypePopup->Clear();
	m_pCardTypePopup->SetDisplayCount( currentCardSetCount);

	for( INT32 i = 0; i < currentCardSetCount; i++)
	{
		UINT8 CardsetID = static_cast<UINT8>(g_pConfigEx->GetQuest().GetCard(i));

		g_pQuestTextDictionary->GetCardSetNameString( cardSetName, MAX_STRING_COUNT, CardsetID);

		bool HasCard = pQuestManager->HasCardSet(CardsetID);
		if (HasCard)
		{
			i3Color::Set(&color, (UINT8) 255, 211, 3, 255);
		}
		else
		{
			if (QUEST_CARDSET_TYPE_TUTORIAL == CardsetID ||
				QUEST_CARDSET_TYPE_DINO_TUTORIAL == CardsetID ||
				QUEST_CARDSET_TYPE_HUMAN_TUTORIAL == CardsetID)
			{
				i3Color::Set(&color, (UINT8) 155, 155, 155, 255);
			}
			else
			{
				i3Color::Set(&color, (UINT8) 255, 255, 255, 255);
			}
		}

		m_pCardTypePopup->SetTextColor(i, &color);
		m_pCardTypePopup->AddElement(cardSetName);
	}
}


INT32 CStepProfile_Challenge::_FindCardSetIdx(CARDSET_ID cardSetId)
{
	const INT32 CardSetCount = g_pConfigEx->GetQuest().GetCardCount();
	for(INT32 i = 0; i < CardSetCount ; i++)
	{
		if (g_pConfigEx->GetQuest().GetCard(i) == cardSetId)
		{
			return i;
		}
	}

	// not HasCard
	return -1;
}


void CStepProfile_Challenge::_BuyCardSet(CARDSET_ID cardSetId)
{
	g_pGameContext->SetEvent(EVENT_QUEST_BUY_CARD_SET, &cardSetId);
}

void CStepProfile_Challenge::_DeleteCardSet(UINT8 cardSetIdx)
{
	g_pGameContext->SetEvent(EVENT_QUEST_DELETE_CARD_SET, &cardSetIdx);
}

BOOL CStepProfile_Challenge::_IsActiveViewCardSet(void)
{
	CQuestManager * pQuestManager = g_pGameContext->getQuestManager(m_bIsMyInfoView);
	if( pQuestManager == NULL)
		return FALSE;

	if (pQuestManager->IsActivedCardset())
	{
		return m_selectedCardSet == _FindCardSetIdx( pQuestManager->GetActiveCardsetID());
	}
	else
	{
		return FALSE;
	}
}

void CStepProfile_Challenge::_ChangeActiveCardSet(CARDSET_ID cardSetId)
{
	if( m_bIsMyInfoView == false)
		return;

	CQuestManager * pQuestManager = g_pGameContext->getQuestManager();
	if( pQuestManager == NULL)
		return;

	INT32	cardSetIndex = pQuestManager->GetCardsetIndex(cardSetId);
	if ( cardSetIndex != -1 )
		pQuestManager->ChangeActiveCardSet(cardSetIndex);
}


void CStepProfile_Challenge::CbDeleteCardSet(void* pThis,INT32 nParam)
{
	CQuestManager * pQuestManager = g_pGameContext->getQuestManager();
	if( pQuestManager == NULL)
		return;

	if (MBR_OK == nParam)
	{
		CStepProfile_Challenge * profile = (CStepProfile_Challenge*) pThis;

		// 가지고 있는 카드셋만 삭제할 수 있다.
		// 현재 활성화 되어있다면, 가지고 있는 카드셋이다.
		if (profile->_IsActiveViewCardSet())		
		{
			UINT8 cardSetIdx = pQuestManager->getActiveCardSetIndex();
			profile->_DeleteCardSet(cardSetIdx);
		}
	}
}

void CStepProfile_Challenge::CbComplateChallenge(void* pThis,INT32 nParam)
{
	g_pGameContext->SetEvent(EVENT_GET_NEW_CHALLENGE_CARD);
}


void CStepProfile_Challenge::OnGameEvent( INT32 event, INT32 arg)
{
	switch( event)
	{
	case EVENT_QUEST_BUY_CARD_SET :
		if (EV_SUCCESSED(arg))
		{
			STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_BUY_CARD"));/*카드한벌 구입에 성공하였습니다.*/
			Event_ChangeUserInfo( m_selectedCardSlot);
		}
		else
		{
			if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_BUY_CARD_PARAMETER_FAIL))	STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_FAIL_BUY_CARD_BY_NO_CARD_INFO"));/*구입할 카드 정보를 찾지못해 구매에 실패하였습니다.*/
			else if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_BUY_CARD_PRICE_FAIL))	STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_NO_POINT_TO_GET_ITEM"));/*잔액이 부족하여 아이템을 구입할 수 없습니다.*/
			else if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_BUY_CARD_UNKOWN_FAIL))	STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_LIMIT_CARD_COUNT"));/*보유할 수 있는 임무카드의 종류는\n총 3종으로 제한되어 있습니다.\n \n현재 보유하신 임무를 모두 완수하거나 삭제 하신 후 다시 구입해주시기 바랍니다.*/
			else if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_BUY_CARD_DISQUALIFIED_FAIL))	STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_DID_NOT_TUTORIAL_MISSION_CARD"));
			
			else I3ASSERT_0;
		}
		break;
	case EVENT_QUEST_DELETE_CARD_SET :
		if (EV_SUCCESSED(arg))
		{
			STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_DELETE_CARD"));/*카드한벌을 삭제하였습니다.*/
			Event_ChangeUserInfo( );
		}
		else
		{
			if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_DELETE_CARD_PARAMETER_FAIL))		STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_FAIL_DELETE_CARD_BY_NOT_INFO"));/*삭제할 카드 정보를 찾지못해 삭제에 실패하였습니다.*/
			else if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_DELETE_CARD_UNKOWN_FAIL))	STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_FAIL_DELETE_ITEM_BY_NETWORK_PROBLEM"));/*네트워크 문제로 아이템의 삭제를 실패하였습니다.*/
			else I3ASSERT_0;
		}
		break;
	}
}

void CStepProfile_Challenge::Event_ChangeUserInfo( INT32 iCardSlot)
{
	OnUpdatePrize( iCardSlot);

	_UpdateChallengeCardInfo( iCardSlot);
	_UpdateChallengeSlotState( iCardSlot);
	_UpdateChallengeInfo( iCardSlot);
}

BOOL CStepProfile_Challenge::_IsPossibilityActiveCard(UINT32 cardIdx)
{
	if( cardIdx >= CARD_SLOT_COUNT)
		return FALSE;

	CQuestManager * pQuestManager = g_pGameContext->getQuestManager();
	if( pQuestManager == NULL)
		return FALSE;


	// 카드를 가지고 있어야한다.
	if ( !pQuestManager->IsActivedCardset() )
		return FALSE;

	if (m_selectedCardSet == -1)
		return FALSE;

	CARDSET_ID CardsetID = pQuestManager->GetActiveCardsetID();
	if (CardsetID != GetSelectedCardsetID() )
		return FALSE;

	if ( pQuestManager->CanSelectCard(cardIdx) )
		return TRUE;
	return FALSE;
}

BOOL CStepProfile_Challenge::_CheckBuyQuestCard(INT32 iCardIdx)
{
	BOOL Rv = TRUE; 

	UINT32 MyRank = g_pGameContext->GetMyRank();
	switch(iCardIdx)
	{
	case QUEST_CARDSET_TYPE_COMMISSIONED_O	: 
		if( MyRank < 5)//하사 이상
		{
			STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_QEUST_ERROR_COMMISSIONED"));
			Rv = FALSE; 
		}
		break; 
	case QUEST_CARDSET_TYPE_COMPANY_O		: 
		if( MyRank < 17)//소위 이상
		{
			STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_QEUST_ERROR_COMPANY"));
			Rv = FALSE; 
		}
		break; 
	case QUEST_CARDSET_TYPE_FIELD_O			: 
		if( MyRank < 31)//소령 이상
		{
			STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_QEUST_ERROR_FIELD"));
			Rv = FALSE; 
		}
		break; 		
	}

	return Rv; 
}

void CStepProfile_Challenge::_UpdateChallengeSlotState( INT32 iCardSlot)
{
	CQuestManager * pQuestManager = g_pGameContext->getQuestManager();

	if( pQuestManager == NULL)
		return;

	// 활성화 카드가 없는 경우 모든 임무 비활성화
	if (FALSE == _IsActiveViewCardSet() || NULL == pQuestManager->getActiveCard() || m_selectedCardSet == -1)
	{
		for( INT32 i = 0; i < MAX_COMMON_SLOT; i++)
		{
			m_commonSlot[i]._pSlot->setControlState(I3GUI_CONTROL_STATE_DISABLED);
			m_commonSlot[i]._pState->SetText("");
			m_commonSlot[i]._pNotice->SetText("");
		}

		return;
	}

	// 선택된 카드에 포함된 임무와 그 상태를 출력한다.
	INT32 idx = 0;

	for( INT32 i = 0; i < MAX_QUEST_PER_CARD_NEW; i++)
	{
		if( ! pQuestManager->HasQuest(i) ) 
			continue;

		char stateText[MAX_STRING_COUNT] = {0, };
		I3COLOR color;

		if (TRUE != pQuestManager->isCompleteQuest(i))
		{
			i3String::Copy( stateText, GAME_STRING("STR_TBL_PROFILE_CHALLENGE_TEXT"), MAX_STRING_COUNT );/*임무*/
			i3Color::Set(&color, (UINT8) 253, 213, 149, 255);
		}
		else
		{
			i3String::Copy( stateText, GAME_STRING("STR_TBL_PROFILE_COMPLETE_TEXT"), MAX_STRING_COUNT );/*완료*/
			i3Color::Set(&color, (UINT8) 115, 232, 249, 255);
		}

		if( (iCardSlot == -1) || (pQuestManager->IsActivedCardset() && pQuestManager->getActiveCardIndex() == iCardSlot) )
		{
			m_commonSlot[idx]._pState->SetText( stateText);
			m_commonSlot[idx]._pState->SetTextColor( &color);
			m_commonSlot[idx]._pSlot->setControlState(I3GUI_CONTROL_STATE_DISABLED);

			{
				pQuestManager->GetQuestQueryHUDDesc( stateText, i, sizeof( MAX_STRING_COUNT));
				m_commonSlot[idx]._pNotice->SetText( stateText);
			}

			idx++;
			if(idx >= MAX_COMMON_SLOT)
				break;
		}
	}

	for( INT32 i = idx; i < MAX_COMMON_SLOT; i++)
	{
		m_commonSlot[i]._pState->SetText("");
		m_commonSlot[i]._pSlot->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_commonSlot[i]._pNotice->SetText( "");
	}

}


void CStepProfile_Challenge::_UpdateChallengeInfo( INT32 iCardSlot)
{
	CQuestManager * pQuestManager = g_pGameContext->getQuestManager();

	if( pQuestManager == NULL)						return;
	
	// 활성화된 카드가 없으면 진행중인 카드표시하지 않는다.
	if (FALSE == _IsActiveViewCardSet() || NULL == pQuestManager->getActiveCard() || m_selectedCardSet == -1)
	{
		m_pCard->SetEnable(FALSE);
		return;
	}


	CARDSET_ID cardSetId = pQuestManager->GetActiveCardsetID();
	// 현재 진행중인 카드 그림을 표시한다.
	if( cardSetId != QUEST_CARDSET_TYPE_NONE && (_IsActiveViewCardSet() && ((iCardSlot == -1) || (pQuestManager->getActiveCardIndex() == iCardSlot))) )
	{
		UINT8 cardIdx = pQuestManager->getActiveCardIndex();

		g_pFramework->SetCardShape(m_pCard, cardSetId, cardIdx);
		m_pCard->SetEnable(TRUE);

		// 활성화된 카드로 커서 이동
		if (QUEST_CARDSET_TYPE_TUTORIAL == GetSelectedCardsetID())
		{
			m_selectedCardSlot = cardIdx;
		}
	}
	else
	{
		// 보여지는 카드셋이 활성화 카드셋이 아닐경우 진행 중인 카드는 없다.
		m_pCard->SetEnable(FALSE);
	}
}

void CStepProfile_Challenge::_UpdateChallengeCardInfo( INT32 iCardSlot)
{
	I3COLOR color;
	char cardSetName[MAX_STRING_COUNT];
	char cardSetTitle[MAX_STRING_COUNT];

	// 화면에 보여줄 카드셋 설정이 초기화되지 않았다면 초기화한다.

	CQuestManager * pQuestManager = g_pGameContext->getQuestManager(m_bIsMyInfoView);
	if( pQuestManager == NULL)
		return;

	bool	isActivedCardset = pQuestManager->IsActivedCardset();
	// 선택된 카드가 없다면, 활성화 카드를 선택상태로 만든다.
	if (_IsActiveViewCardSet() && -1 == iCardSlot && isActivedCardset )
	{
		iCardSlot = pQuestManager->getActiveCardIndex();
	}

	if( m_selectedCardSet < 0)
	{
		if( isActivedCardset )
		{
			m_selectedCardSet = _FindCardSetIdx( pQuestManager->GetActiveCardsetID());

			// 현재 활성화된 카드 인덱스가 잘못될 경우를 확인합니다.
			if ( m_selectedCardSet == -1 )
				return;
		}
		else
		{
			// 활성화 카드셋이 없다면, 보유한 카드셋 중 첫번째 카드셋을 활성화한다.
			pQuestManager->SetFirstCardsetActive();
			return;
		}
	}



	if( m_selectedCardSet < 0 )
		return;

	// 현재 진행중인 카드셋 종류 표시
	g_pQuestTextDictionary->GetCardSetNameString( cardSetName, MAX_STRING_COUNT, GetSelectedCardsetID());
	g_pQuestTextDictionary->GetCardSetTitleString( cardSetTitle, MAX_STRING_COUNT, GetSelectedCardsetID());
	
	{
		BOOL bfound = pQuestManager->HasCardSet( GetSelectedCardsetID() );

		if( bfound)
		{
			i3Color::Set(&color, (UINT8) 255, 211, 3, 255);
		}
		else
		{
			CARDSET_ID CardsetID = GetSelectedCardsetID();
			if( QUEST_CARDSET_TYPE_TUTORIAL == CardsetID ||
				QUEST_CARDSET_TYPE_DINO_TUTORIAL == CardsetID ||
				QUEST_CARDSET_TYPE_HUMAN_TUTORIAL == CardsetID )
			{
				i3Color::Set(&color, (UINT8) 155, 155, 155, 255);
			}
			else
			{
				i3Color::Set(&color, (UINT8) 255, 255, 255, 255);				
			}			
		}

		m_pCardTypeCombo->SetTextColor(&color);
		m_pCardTypeCombo->SetText(cardSetName);
	}

    m_pCommonKey->SetText(cardSetTitle);


	if( m_bIsMyInfoView)
	{
		// 튜토리얼, 이벤트 카드의 경우 구입할 수 없다.
		if( (QUEST_CARDSET_TYPE_TUTORIAL == GetSelectedCardsetID()) || 
			(QUEST_CARDSET_TYPE_EVENT == GetSelectedCardsetID()) )
		{
			m_pCardBuyOne->SetEnable(FALSE);
			m_pCardBuyAll->SetEnable(FALSE);
			m_pCardDeleteAll->SetEnable(FALSE);		
			m_pCardInfo->SetEnable(TRUE);
		}
/*	
		else if( (QUEST_CARDSET_TYPE_NORMAL == GetSelectedCardsetID()) ||
			(QUEST_CARDSET_TYPE_SILVER == GetSelectedCardsetID()) )
		{
			// 삭제버튼만 활성화
			m_pCardBuyOne->SetEnable(FALSE);
			m_pCardBuyAll->SetEnable(FALSE);
			m_pCardDeleteAll->SetEnable(TRUE);
			m_pCardInfo->SetEnable(TRUE);
		}
		*/
		else
		{
			m_pCardBuyOne->SetEnable(FALSE);
			m_pCardBuyAll->SetEnable(TRUE);
			m_pCardInfo->SetEnable(TRUE);

			// 카드셋을 가지고 있을때만 삭제가 가능하다.
			if (_IsActiveViewCardSet())		m_pCardDeleteAll->SetEnable(TRUE);
			else							m_pCardDeleteAll->SetEnable(FALSE);
		}
	}
	else
	{
		m_pCardBuyOne->SetEnable(FALSE);
		m_pCardBuyAll->SetEnable(FALSE);
		m_pCardDeleteAll->SetEnable(FALSE);
		m_pCardInfo->SetEnable(TRUE);
	}


	// 진행중의 카드셋에 포함된 카드 그림을 출력한다.
	_UpdateCardSlotImage( iCardSlot);
}

void CStepProfile_Challenge::_UpdateCardSlotImage( INT32 iSelectedSlot)
{
	CQuestManager * pQuestManager = g_pGameContext->getQuestManager(m_bIsMyInfoView);
	if( pQuestManager == NULL)
		return;
	
	for( INT32 i = 0; i < MAX_CARD_PER_CARDSET; i++)
	{
		g_pFramework->SetCardShape( m_cardSlot[i]._pCard, GetSelectedCardsetID(), i);
		if( _IsActiveViewCardSet())
		{
			if( pQuestManager->HasCard( i))
			{
				if( QUEST_CARDSET_TYPE_TUTORIAL == GetSelectedCardsetID() )
				{
					// 튜토리얼 임무카드의 경우 순차적으로 진행/출력된다.
					// 첫 카드는 무조건 활성화된다.
					// 앞카드가 완료되어야 활성화 할 수 있다.
					if( pQuestManager->IsCompletedCard(i))
					{
						m_cardSlot[i]._pCard->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 255);
					}
					else if (0 == i || pQuestManager->IsCompletedCard(i - 1))
					{
						m_cardSlot[i]._pCard->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 165);
					}
					else
					{
						m_cardSlot[i]._pCard->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 51);
					}
				}
				else
				{
					// 그 이외 임무카의 경우 원하는 것먼저 진행/출력할 수 있다.
					if( pQuestManager->IsCompletedCard(i))	m_cardSlot[i]._pCard->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 255);
					else							m_cardSlot[i]._pCard->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 165);
				}
			}
			else
			{
				m_cardSlot[i]._pCard->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 51);
			}
		}
		else
		{
			m_cardSlot[i]._pCard->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 51);
		}

		m_cardSlot[i]._pSlot->setControlDisable(I3GUI_CONTROL_STATE_NORMAL);

		// 활성화된 카드의 경우 컨트롤 상태 반전
		if( iSelectedSlot == i)
		{
			m_cardSlot[i]._pSlot->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_PRESS_BUTTON_ONMOUSE);
			m_cardSlot[i]._pSlot->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_PRESS_BUTTON_ONMOUSE);
		}
		else
		{
			m_cardSlot[i]._pSlot->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_PRESS_BUTTON_NORMAL);
			m_cardSlot[i]._pSlot->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_PRESS_BUTTON_NORMAL);
		}
	}
}

CARDSET_ID CStepProfile_Challenge::GetSelectedCardsetID()
{
	return static_cast<CARDSET_ID>(g_pConfigEx->GetQuest().GetCard(m_selectedCardSet));
}


