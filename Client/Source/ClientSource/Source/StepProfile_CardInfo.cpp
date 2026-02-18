#include "pch.h"
#include "StepProfile_CardInfo.h"
#include "GuiNotifyReceiver.h"
#include "QuestTextDictionary.h"

I3_CLASS_INSTANCE( CStepProfile_CardInfo, i3ElementBase);

CStepProfile_CardInfo::CStepProfile_CardInfo()
{
	INT32 i;

	m_pNotifyBase	= NULL;

	m_pCardInfoCaption = NULL;
	m_pCardInfoConfirm = NULL;

	m_pCaptionTitle = NULL;
	m_pCardInfoCardImage = NULL;
	m_pCardInfoCardName = NULL;
	m_pCardInfoPriceKey = NULL;
	m_pCardInfoPriceValue = NULL;
	m_pCardInfoPointKey = NULL;
	m_pCardInfoPointValue = NULL;
	m_pCardInfoExpKey = NULL;
	m_pCardInfoExpValue = NULL;

	// 지급표장
	m_pSupplyMedalTitle = NULL;

	for( i = 0; i < BADGE_TYPE_COUNT; i++)
	{
		m_supplyMedal[i]._pMedalBg		= NULL;
		m_supplyMedal[i]._pRibbonImage	= NULL;
		m_supplyMedal[i]._pMedalImage	= NULL;
		m_supplyMedal[i]._pMedalName	= NULL;
		m_supplyMedal[i]._pMedalCount	= NULL;
	}
	
	// 최종 추가 보상 포인트/경험치
	m_pFinalPrizePointExpTitle = NULL;
	m_pFinalPrizePointKey	= NULL;
	m_pFinalPrizePointText	= NULL;
	m_pFinalPrizeExpKey		= NULL;
	m_pFinalPrizeExpText	= NULL;

	m_pFinalPrizeItemKey	= NULL;
	m_pFinalPrizeItemName = NULL;
	m_pFinalPrizeItemImage = NULL;

	m_pCardInfoWin = NULL;
}


CStepProfile_CardInfo::~CStepProfile_CardInfo()
{
}


BOOL CStepProfile_CardInfo::LinkControl( CGuiNotifyReceiver * pBase, i3BinList * plist, i3GameNode * pControl)
{
	INT32 i;
	char strName[ MAX_STRING_COUNT];

	m_pNotifyBase = pBase;

	LinkControlEx(plist, "prCardInfoWin", m_pCardInfoWin, -1);	
	LinkControlEx(plist, "prciCaption", m_pCardInfoCaption, -1);
	LinkControlEx(plist, "prciConfirm", m_pCardInfoConfirm, GCI_PRS_S_CARD_INFO_CONFIRM);

	LinkControlEx(plist, "prciCardInfoTitle", m_pCaptionTitle, -1);
	LinkControlEx(plist, "prciCardInfoCardImage", m_pCardInfoCardImage, -1);
	LinkControlEx(plist, "prciCardInfoCardName", m_pCardInfoCardName, -1);
	LinkControlEx(plist, "prciCardInfoPriceKey", m_pCardInfoPriceKey, -1);
	LinkControlEx(plist, "prciCardInfoPriceValue", m_pCardInfoPriceValue, -1);
	LinkControlEx(plist, "prciCardInfoPointKey", m_pCardInfoPointKey, -1);
	LinkControlEx(plist, "prciCardInfoPointValue", m_pCardInfoPointValue, -1);
	LinkControlEx(plist, "prciCardInfoExpKey", m_pCardInfoExpKey, -1);
	LinkControlEx(plist, "prciCardInfoExpValue", m_pCardInfoExpValue, -1);

	// 지급표장
	LinkControlEx(plist, "prciSupplyMedalTitle", m_pSupplyMedalTitle, -1);
	
	for( i = 0; i < BADGE_TYPE_COUNT; i++)
	{
		sprintf_s(strName, "prciSupplyMedalBg%d", i);
		LinkControlEx(plist, strName, m_supplyMedal[i]._pMedalBg, -1);
		sprintf_s(strName, "prciSupplyMedalRibbonImage%d", i);
		LinkControlEx(plist, strName, m_supplyMedal[i]._pRibbonImage, -1);
		sprintf_s(strName, "prciSupplyMedalMedalImage%d", i);
		LinkControlEx(plist, strName, m_supplyMedal[i]._pMedalImage, -1);
		sprintf_s(strName, "prciSupplyMedalName%d", i);
		LinkControlEx(plist, strName, m_supplyMedal[i]._pMedalName, -1);
		sprintf_s( strName, "prciSupplyMedalCount%d", i);
		LinkControlEx(plist,  strName, m_supplyMedal[ i]._pMedalCount, -1);
	}

	
	// 최종 추가 보상 포인트/경험치
	LinkControlEx(plist, 	"prciFinalPrizePointExpTitle",	m_pFinalPrizePointExpTitle,		-1);

	LinkControlEx(plist, 	"prciFinalPrizePointTextKey",	m_pFinalPrizePointKey,			-1);
	LinkControlEx(plist, 	"prciFinalPrizePointTextValue", m_pFinalPrizePointText,			-1);
	LinkControlEx(plist, 	"prciFinalPrizeExpTextKey",		m_pFinalPrizeExpKey,			-1);
	LinkControlEx(plist, 	"prciFinalPrizeExpTextValue",	m_pFinalPrizeExpText,			-1);

	// 아이템
	LinkControlEx(plist, 	"prciFinalPrizeItemTextKey",	m_pFinalPrizeItemKey,			-1);
	LinkControlEx(plist, 	"prciFinalPrizeItemTextValue",	m_pFinalPrizeItemName,			-1);
	LinkControlEx(plist, 	"prciFinalPrizeItemImage",		m_pFinalPrizeItemImage,			-1);

	return TRUE;
}

void CStepProfile_CardInfo::Init( void)
{


	{
		INIT_WIN_CAPTION2(m_pCardInfoCaption, GAME_STRING("STR_TBL_PROFILE_CHALLENGE_CARD_INFO"), GetColor(GCT_DEFAULT));
	}
	{
		INIT_BUTTON_EX( m_pCardInfoConfirm);
			
		m_pCardInfoConfirm->CreateCaption( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE);
		m_pCardInfoConfirm->SetCaptionColor( GetColor(GCT_VALUE));
		m_pCardInfoConfirm->SetCaption( GAME_STRING("STR_TBL_PROFILE_CONFIRM"));/*확인*/
	}

	m_pCaptionTitle->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCaptionTitle->SetTextColor( GetColor(GCT_DEFAULT));
	m_pCaptionTitle->SetText(GAME_STRING("STR_TBL_PROFILE_CARD_INFO"));/*카드정보*/

	m_pCardInfoCardName->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCardInfoCardName->SetTextColor( GetColor(GCT_DEFAULT));	

	m_pCardInfoPriceKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCardInfoPriceKey->SetTextColor( GetColor(GCT_DEFAULT));
	m_pCardInfoPriceKey->SetText(GAME_STRING("STR_TBL_PROFILE_CARD_PRICE"));/*구입 가격:*/

	m_pCardInfoPriceValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCardInfoPriceValue->SetTextColor( GetColor(GCT_DEFAULT));	

	m_pCardInfoPointKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCardInfoPointKey->SetTextColor( GetColor(GCT_DEFAULT));
	m_pCardInfoPointKey->SetText(GAME_STRING("STR_TBL_PROFILE_TOTAL_GET_POINT"));/*총 지급포인트:*/

	m_pCardInfoPointValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCardInfoPointValue->SetTextColor( GetColor(GCT_DEFAULT));

	m_pCardInfoExpKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCardInfoExpKey->SetTextColor( GetColor(GCT_DEFAULT));
	m_pCardInfoExpKey->SetText(GAME_STRING("STR_TBL_PROFILE_TOTAL_GET_EXT"));/*총 획득경험치:*/

	m_pCardInfoExpValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCardInfoExpValue->SetTextColor( GetColor(GCT_DEFAULT));

	// 지급표장
	m_pSupplyMedalTitle->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSupplyMedalTitle->SetTextColor( GetColor(GCT_DEFAULT));
	m_pSupplyMedalTitle->SetText( GAME_STRING("STR_TBL_PROFILE_SUPPLY_MEDAL"));/*지급표장*/	

	for( INT32 i = 0; i < BADGE_TYPE_COUNT; i++)
	{	
		m_supplyMedal[i]._pRibbonImage->SetEnable( FALSE);
		m_supplyMedal[i]._pMedalImage->SetEnable( FALSE);
		m_supplyMedal[i]._pMedalName->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_supplyMedal[i]._pMedalName->SetTextColor( GetColor(GCT_DEFAULT));

		m_supplyMedal[ i]._pMedalCount->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	}

	m_supplyMedal[ BADGE_TYPE_RIBBON]._pMedalName->SetText( GAME_STRING("STR_TBL_PROFILE_RIBBON_TEXT2"));	/*약장*/
	g_pFramework->SetRibbonShape( m_supplyMedal[ BADGE_TYPE_RIBBON]._pRibbonImage, FIXED_BADGE_NUM_RIBBON);
	m_supplyMedal[ BADGE_TYPE_RIBBON]._pRibbonImage->SetEnable(TRUE);
	
	m_supplyMedal[ BADGE_TYPE_ENSIGN]._pMedalName->SetText( GAME_STRING("STR_TBL_PROFILE_ENSIGN_TEXT2"));	/*휘장*/
	g_pFramework->SetEnsignShape( m_supplyMedal[ BADGE_TYPE_ENSIGN]._pMedalImage, FIXED_BADGE_NUM_ENSIGN);
	m_supplyMedal[ BADGE_TYPE_ENSIGN]._pMedalImage->SetEnable( TRUE);

	m_supplyMedal[ BADGE_TYPE_MEDAL]._pMedalName->SetText( GAME_STRING("STR_TBL_PROFILE_MEDALS_TEXT2"));		/*훈장*/
	g_pFramework->SetMedalShape( m_supplyMedal[ BADGE_TYPE_MEDAL]._pMedalImage, FIXED_BADGE_NUM_MEDAL);
	m_supplyMedal[ BADGE_TYPE_MEDAL]._pMedalImage->SetEnable( TRUE);

	m_supplyMedal[ BADGE_TYPE_MASTER]._pMedalName->SetText( GAME_STRING("STR_TBL_PROFILE_MASTER_MEDALS_TEXT"));	/*마스터 훈장*/
	g_pFramework->SetMedalShape( m_supplyMedal[ BADGE_TYPE_MASTER]._pMedalImage, FIXED_BADGE_NUM_MASTER);
	m_supplyMedal[ BADGE_TYPE_MASTER]._pMedalImage->SetEnable( TRUE);


	// 최종 추가 보상 포인트/경험치
	m_pFinalPrizePointExpTitle->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pFinalPrizePointExpTitle->SetTextColor( GetColor(GCT_DEFAULT));
	m_pFinalPrizePointExpTitle->SetText( GAME_STRING("STR_TBL_PROFILE_LAST_REWARD_POINT"));/*최종 추가 보상 포인트/경험치*/

	m_pFinalPrizePointKey->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pFinalPrizePointKey->SetTextColor( GetColor( GCT_DEFAULT));
	m_pFinalPrizePointKey->SetText( GAME_STRING("STR_TBL_PROFILE_CHALLENGE_PRIZE_POINT_TEXT"));	/*포인트*/

	m_pFinalPrizePointText->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pFinalPrizePointText->SetTextColor( GetColor(GCT_DEFAULT));

	m_pFinalPrizeExpKey->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pFinalPrizeExpKey->SetTextColor( GetColor( GCT_DEFAULT));
	m_pFinalPrizeExpKey->SetText( GAME_STRING("STR_TBL_PROFILE_CHALLENGE_PRIZE_EXP_TEXT"));		/*경험치*/

	m_pFinalPrizeExpText->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pFinalPrizeExpText->SetTextColor( GetColor(GCT_DEFAULT));


	m_pFinalPrizeItemKey->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pFinalPrizeItemKey->SetTextColor( GetColor( GCT_DEFAULT));
	m_pFinalPrizeItemKey->SetText( GAME_STRING("STBL_IDX_CHARA_SLOT_TITLE4"));							/*아이템*/


	m_pFinalPrizeItemName->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pFinalPrizeItemName->SetTextColor( GetColor(GCT_DEFAULT));
}

void CStepProfile_CardInfo::Update( REAL32 tm)
{

}

void CStepProfile_CardInfo::GuiNotify( I3GUI_CONTROL_NOTIFY* pNotify)
{

}

void CStepProfile_CardInfo::OnCardInfo( CARDSET_ID cardSetId)
{
	INT32 i;
	char szWeaponName[ MAX_STRING_COUNT];
	char szText[MAX_STRING_COUNT];

	if( cardSetId >= QUEST_CARDSET_TYPE_COUNT)
		return;

	INT32 iRibbonCount = 0;
	INT32 iEnsignCount = 0;
	INT32 iMedalCount = 0;
	INT32 iMasterCount = 0;

	//g_pFramework->SetCardShape(m_pCardInfoCardImage, cardSetId, 0);
	g_pFramework->SetCardSetShape(m_pCardInfoCardImage, cardSetId);
	
	g_pQuestTextDictionary->GetCardSetNameString( szText, MAX_STRING_COUNT, cardSetId);
	m_pCardInfoCardName->SetText( szText);

	INT32 cardSetPrice = g_QuestCardSetBuyPoint[cardSetId];
	sprintf_s( szText, "{col:255,151,49,255}%d{/col}", cardSetPrice);
	m_pCardInfoPriceValue->SetText( szText);

	INT32 cardSetPoint = g_pQuestTextDictionary->GetCardSetTotalPoint(cardSetId);
	sprintf_s( szText, "{col:255,151,49,255}%d{/col} %s", cardSetPoint, GAME_STRING("STBL_IDX_SHOP_MONEY"));
	m_pCardInfoPointValue->SetText( szText);

	INT32 cardSetExp = g_pQuestTextDictionary->GetCardSetTotalExp(cardSetId);
	sprintf_s( szText, "{col:255,151,49,255}%d{/col} EXP", cardSetExp);
	m_pCardInfoExpValue->SetText( szText);


	iMasterCount += g_pQuestTextDictionary->GetCardSetBonusMasterBadgeCount(cardSetId);

	// 아래 임무카드 보상 및 지급 표장
	//EXPORT_AVALIABLE_CODE_VALUE;
	INT32 bonusPoint = g_pQuestTextDictionary->GetCardSetBonusPoint(cardSetId);
	INT32 bonusExp = g_pQuestTextDictionary->GetCardSetBonusExp(cardSetId);

	// 지급표장에 대해서 하드코딩 되어 있습니다.
	INT32 supplyTable[QUEST_CARDSET_TYPE_COUNT][ MAX_CARD_PER_CARDSET][2] =
	{
		{{0,}, },	//QUEST_CARDSET_TYPE_NONE
		{{0, 1}, {0, 2}, {0, 5}, {0, 3}, {0, 6}, {0, 9}, {0, 13}, {0, 4}, {0, 10}, {0, 14}},	//QUEST_CARDSET_TYPE_TUTORIAL

		{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},	//QUEST_CARDSET_TYPE_DINO_TUTORIAL
		{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},	//QUEST_CARDSET_TYPE_HUMAN_TUTORIAL

		{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},	//QUEST_CARDSET_TYPE_GOLD	//삭제 됨
		{{1, 11}, {1, 12}, {1, 13}, {1, 14}, {1, 15}, {1, 18}, {1, 19}, {1, 20}, {2, 11}, {2, 12}},	//QUEST_CARDSET_TYPE_ASSAULT
		{{1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 16}, {1, 19}, {1, 20}, {2, 11}, {2, 12}},	//QUEST_CARDSET_TYPE_BACKUP
		{{1, 6}, {1, 7}, {1, 8}, {1, 9}, {1, 10}, {1, 17}, {1, 19}, {1, 20}, {2, 11}, {2, 12}},	//QUEST_CARDSET_TYPE_INFILTRATION
		{{2, 1}, {2, 2}, {2, 3}, {2, 8}, {2, 9}, {2, 10}, {2, 4}, {2, 5}, {2, 6}, {2, 13}},	//QUEST_CARDSET_TYPE_SPECIAL
		{{2, 3}, {2, 6}, {2, 10}, {2, 12}, {2, 11}, {2, 12}, {2, 13}, {2, 7}, {2, 14}, {2, 15}},	//QUEST_CARDSET_TYPE_DEFCON
		{{1, 14}, {1, 9}, {1, 4}, {1, 15}, {1, 10}, {1, 5}, {1, 18}, {1, 16}, {1, 17}, {1, 20}},	//QUEST_CARDSET_TYPE_COMMISSIONED_O
		{{2, 1}, {2, 2}, {2, 3}, {2, 8}, {2, 9}, {2, 10}, {2, 4}, {2, 5}, {2, 6}, {2, 13}},	//QUEST_CARDSET_TYPE_COMPANY_O
		{{2, 2}, {2, 5}, {2, 9}, {2, 3}, {2, 5}, {2, 8}, {2, 13}, {2, 7}, {2, 14}, {2, 15}},	//QUEST_CARDSET_TYPE_FIELD_O

		{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},	//QUEST_CARDSET_TYPE_EVENT

		{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},	//QUEST_CARDSET_TYPE_DINO_2ND
		{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},	//QUEST_CARDSET_TYPE_HUMAN_2ND

		{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},	//QUEST_CARDSET_TYPE_DINO_3ND
		{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},	//QUEST_CARDSET_TYPE_HUMAN_3ND
	};

	for( i = 0; i < MAX_CARD_PER_CARDSET; i++)
	{
		INT32 supplyMedalType = -1;
		INT32 supplyMedalId = -1;

		if (QUEST_CARDSET_TYPE_EVENT != cardSetId)
		{
			supplyMedalType = supplyTable[cardSetId][i][0];
			supplyMedalId = supplyTable[cardSetId][i][1];
		}
		else
		{
			/*//영권 임무카드-- 이후 이벤트 카드를 할때 체크해 보면 될듯.
			// 이벤트 임무카드의 경우, 이벤트 기간 중엔 항상 정보를 받기 때문에
			// 받아 놓은 정보에서 보상부분을 직접 찾아 출력한다.
			CQuestManager* questManager = g_pGameContext->getQuestManager();
			if( questManager != NULL)
			{
				CCardSet* eventCardSet = questManager->getCardSet(MAX_CARDSET_PER_USER - 1);
				I3ASSERT(eventCardSet != NULL);

				CCardInfo* cardInfo = eventCardSet->getCard(i);
				UINT8 prize = cardInfo->GetPrize(4);
				// 보상 종류 구분 - 약장, 휘장, 훈장
				if (101 <= prize && prize < 240)
				{
					// 훈장
					supplyMedalType = 2;
					supplyMedalId = prize - 100;
				}
				else if (51 <= prize && prize < 101)
				{
					// 휘장
					supplyMedalType = 1;
					supplyMedalId = prize - 500;
				}		
				else
				{
					// 약장
					supplyMedalType = 0;
					supplyMedalId = prize;
				}
			}
			*/
		}

		if (supplyMedalId != 0)
		{
			if( supplyMedalType == 0)		iRibbonCount ++;
			else if (supplyMedalType == 1)	iEnsignCount ++;
			else							iMedalCount ++;
		}

	}	
	
	sprintf_s( szText, "{col:255,151,49,255}%d{/col} %s", iRibbonCount, GAME_STRING("STBL_IDX_ITEM_COUNT_UNIT") );
	m_supplyMedal[ BADGE_TYPE_RIBBON]._pMedalCount->SetText( szText);

	sprintf_s( szText, "{col:255,151,49,255}%d{/col} %s", iEnsignCount, GAME_STRING("STBL_IDX_ITEM_COUNT_UNIT") );
	m_supplyMedal[ BADGE_TYPE_ENSIGN]._pMedalCount->SetText( szText);

	sprintf_s( szText, "{col:255,151,49,255}%d{/col} %s", iMedalCount, GAME_STRING("STBL_IDX_ITEM_COUNT_UNIT") );
	m_supplyMedal[ BADGE_TYPE_MEDAL]._pMedalCount->SetText( szText);

	sprintf_s( szText, "{col:255,151,49,255}%d{/col} %s", iMasterCount, GAME_STRING("STBL_IDX_ITEM_COUNT_UNIT") );
	m_supplyMedal[ BADGE_TYPE_MASTER]._pMedalCount->SetText( szText);

	// 최종 추가 보상 포인트/경험치
	{
		sprintf_s( szText, "{col:255,151,49,255}+%d{/col}", bonusPoint);
		m_pFinalPrizePointText->SetText( szText);

		sprintf_s( szText, "{col:255,151,49,255}+%d{/col}", bonusExp);
		m_pFinalPrizeExpText->SetText( szText);
	}

	INT32 weaponFlag = g_pQuestTextDictionary->GetCardSetBonusItemID(cardSetId);

	ITEM_TYPE ItmeType =(ITEM_TYPE)GET_ITEM_FLAG_TYPE( weaponFlag);

	if (ItmeType == ITEM_TYPE_HEADGEAR)
	{
		//임시로 헤드타입은 마스크타입에 대한 정의만 해놓습니다 원빌드 후 차후 다른 아이템도 적용시켜야합니다.
		INT32 HeadNum = GET_ITEM_FLAG_NUMBER( weaponFlag);
		CMaskInfo *pMaskInfo = g_pCharaInfoDataBase->getMaskInfo((MASK_ID)HeadNum);
		I3ASSERT(pMaskInfo != NULL);
		
		pMaskInfo->getScriptName(szWeaponName);


		sprintf_s( szText, "{col:255,151,49,255}%s{/col}", szWeaponName);
		m_pFinalPrizeItemName->SetText( szText);
		g_pFramework->SetCharacterShape(m_pFinalPrizeItemImage, weaponFlag);
		m_pFinalPrizeItemImage->SetEnable(TRUE);

	}
	else if (ItmeType != 0)
	{
		WEAPON_CLASS_TYPE weaponType = (WEAPON_CLASS_TYPE)GET_ITEM_FLAG_CLASS( weaponFlag);
		INT32 weaponNum = GET_ITEM_FLAG_NUMBER( weaponFlag);

		GUI_WEAPON_VALID( weaponFlag, weaponNum);

		CWeaponInfo* pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( weaponType, weaponNum);
		I3ASSERT(pWeaponInfo != NULL);

		pWeaponInfo->GetWeaponInfoName( szWeaponName);

		sprintf_s( szText, "{col:255,151,49,255}%s{/col}", szWeaponName);
		m_pFinalPrizeItemName->SetText( szText);		
		g_pFramework->SetWeaponSelectMark( m_pFinalPrizeItemImage, (UINT8) weaponType, (UINT8) weaponNum); 
		m_pFinalPrizeItemImage->SetEnable(TRUE);
	}
	else
	{
		m_pFinalPrizeItemName->SetText("");
		m_pFinalPrizeItemImage->SetEnable(FALSE);
	}
}



