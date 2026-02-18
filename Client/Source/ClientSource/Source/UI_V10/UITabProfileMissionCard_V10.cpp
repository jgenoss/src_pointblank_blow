#include "pch.h"
#include "UITabProfileMissionCard_V10.h"
#include "UIFloatMissionBoard_V10.h"
#include "UI/UIMainFrame_V10.h"
#include "UIPhaseProfile_V10.h"
#include "UI/UIUtil.h"
#include "../StepProfileDef.h"
#include "../CommunityContext.h"
#include "MissionCard/MCardUtil.h"
#include "MissionCard/MCardSet.h"
#include "MissionCard/MCardSetInfo.h"
#include "MissionCard/MCard.h"
#include "MissionCard/MCardInfo.h"
#include "MissionCard/MCardQuest.h"

#include "UserInfoContext.h"

static UITabProfileMissionCard_V10 * GetThis()
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

	I3ASSERT( pFrame);

	UIPhaseSubBase* pSubPhase = pFrame->GetCurrentSubPhase();

	UITabProfileBase_V10* pTab = static_cast<UIPhaseProfile_V10*>(pSubPhase)->GetMissionCard();

	I3ASSERT( i3::same_of<UITabProfileMissionCard_V10*>(pTab)); 

	return static_cast<UITabProfileMissionCard_V10*>(pTab);
}


//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UITabProfileMissionCard_V10_SelectCard( LuaState * L)
	{
		INT32 Idx = i3Lua::GetIntegerArg(L, 1);
		GetThis()->SelectCardSetType_Server(Idx);
		return 0;
	}

	int UITabProfileMissionCard_V10_CardInfo_Confirm( LuaState * L)
	{
		GetThis()->CardInfo();
		return 0;
	}

	int UITabProfileMissionCard_V10_CardBuy_Confirm( LuaState * L)
	{
		GetThis()->CardBuy();
		return 0;
	}

	int UITabProfileMissionCard_V10_CardDelete_Confirm( LuaState * L)
	{
		GetThis()->CardDelete();
		return 0;
	}
}

LuaFuncReg UITabProfileMissionCard_V10_Glue[] =
{
	{"ClickInfoCard",				UITabProfileMissionCard_V10_CardInfo_Confirm		},
	{"ClickBuyCard",				UITabProfileMissionCard_V10_CardBuy_Confirm			},
	{"ClickDeleteCard",				UITabProfileMissionCard_V10_CardDelete_Confirm		},
	{"SelectCard",					UITabProfileMissionCard_V10_SelectCard				},
	{nullptr,					nullptr}
};

I3_CLASS_INSTANCE( UITabProfileMissionCard_V10);

UITabProfileMissionCard_V10::UITabProfileMissionCard_V10() : m_selectedCardSetIndexFromList(-1), m_nextSetTypeForDelete(QUEST_CARDSET_TYPE_NONE)
{
	::memset(m_CardSlot, 0, sizeof(m_CardSlot));
	::memset(m_CardBG, 0, sizeof(m_CardBG));

	m_pCardInfo = nullptr;
}

UITabProfileMissionCard_V10::~UITabProfileMissionCard_V10()
{

}

/*virtual*/ void UITabProfileMissionCard_V10::_InitializeAtLoad( i3UIScene * pScene)
{
	UITabProfileBase_V10::_InitializeAtLoad(pScene);

	char szTemp[MAX_STRING_COUNT];
	
	for(INT32 i = 0; i < MAX_CARD_PER_CARDSET; i++)
	{
		sprintf_s(szTemp, "%d", i);
		m_CardSlot[i] = (i3UIButtonImageSet *)pScene->FindChildByName(szTemp);
		//m_CardSlot[i]->SetViewOnly(true);
	}
	for(INT32 i = 0; i < MAX_CARD_PER_CARDSET; i++)
	{
		sprintf_s(szTemp, "BG%d", i);
		m_CardBG[i] = (i3UIButtonImageSet *)pScene->FindChildByName(szTemp);
		m_CardBG[i]->SetViewOnly(true);
	}

	m_pCardInfo = (i3UIButtonImageSet*) pScene->FindChildByName("InfoCard");
	if(m_pCardInfo != nullptr)
		m_pCardInfo->SetViewOnly(true);

	GetSlide()->AddFrameWnds(GetScene(0));
}

void UITabProfileMissionCard_V10::OnLoadAllScenes()
{
	InitCardList_Lua();			// 서버의 카드목록 리스트..

	UpdateCardSetListSelection();

	UITabProfileMissionCard_V10::UpdateCardSlotsAndInfoUI();

	////카드 카운트를 계산
	//SetMissionPossibleCount_Lua();
}



void	UITabProfileMissionCard_V10::UpdateCardSlotsAndInfoUI()
{

	// InitButtonState
	for(INT32 i = 0; i < MAX_CARD_PER_CARDSET; i++)
		m_CardBG[i]->SetShapeIdx(I3UI_BUTTON_SHAPE_NORMAL);

	//장착하고 있는 카드를 가지고있는 CardSet이라면 카드 리스트에서 카드 타입(종류)를 변경할 시 해당 장착카드의 포커스 및 정보로 갱신
	INT32 cardIdx = -1;
	bool infocard = false;
	
	if ( MCardUtil::IsServerListIdxCurrentSelectedSet(m_selectedCardSetIndexFromList) ) // 매니저의 셀렉션과 UI셀렉션이 다를수 있음..
	{
		const MCard* pCard = MCardUtil::GetSelectedMCard();

		if (pCard)
		{
			cardIdx = pCard->GetMyIndex();
			m_CardBG[cardIdx]->SetShapeIdx(I3UI_BUTTON_SHAPE_PUSHED);
			
			infocard = true;
			m_pCardInfo->SetShapeIdx(cardIdx);
		}
	}
	else
		infocard = false;

	LuaState * L = _CallLuaFunction("SetInfoCardEnable");
	i3Lua::PushBoolean(L, infocard);
	_EndLuaFunction(L,1);

	// Pushed로 바뀔 때 MissionBoard도 같이 바꿉니다.
	_SetMissionBoard();
	UpdatePrize_Lua();				// 우측 하단에 보상을 표시한다.
	_UpdateChallengeCardInfo();	// 구입,상세정보, 삭제 버튼 표시여부
	_UpdateChallengeSlotState();	// 가운데 하단 임무카드 내용 갱신
	_UpdateCardSlotImage();		//
}

/*virtual*/ void UITabProfileMissionCard_V10::OnUnloadAllScenes()
{

}

namespace
{


	bool CheckEventCardSlot(i3UIControl* m_pCtrl, i3UIButtonImageSet* (&aCardSlot)[ MAX_CARD_PER_CARDSET ])
	{
		for(INT32 i=0; i < MAX_CARD_PER_CARDSET; i++)
		{
			if(m_pCtrl == aCardSlot[i])
				return true;
		}
		return false;
	}

}


/*virtual*/ bool UITabProfileMissionCard_V10::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/ )
{
	if( event == I3_EVT_UI_NOTIFY)
	{
		I3UI_CONTROL_NOTIFY * pParam = (I3UI_CONTROL_NOTIFY*) param2;
		
		if( pParam->m_nEvent == I3UI_EVT_CLICKED && CheckEventCardSlot(pParam->m_pCtrl, m_CardSlot))
		{
			// 구매 카드셋이 아닌 경우의 10종 카드선택이라면 처리 생략..
			if (MCardUtil::IsServerListIdxCurrentSelectedSet(m_selectedCardSetIndexFromList) )
			{
				I3ASSERT( pParam->m_pCtrl != nullptr);

				MCard* pCard = MCardUtil::GetSelectedMCard();

				INT32 prevSelectedCardSlot = (pCard) ? pCard->GetMyIndex() : -1;
				INT32 newSelectedCardSlot = i3::atoi( pParam->m_pCtrl->GetName());

				m_CardBG[newSelectedCardSlot]->SetShapeIdx(I3UI_BUTTON_SHAPE_PUSHED);

				if( newSelectedCardSlot != prevSelectedCardSlot)
				{
					if (prevSelectedCardSlot >= 0 && prevSelectedCardSlot < MAX_CARD_PER_CARDSET)
						m_CardBG[prevSelectedCardSlot]->SetShapeIdx(I3UI_BUTTON_SHAPE_NORMAL);
				}

				SelectCard(newSelectedCardSlot);
			}
		}
		//else if( pParam->m_nEvent == I3UI_EVT_ENTERMOUSE && CheckEventCardSlot(pParam->m_pCtrl, m_CardSlot) )
		//{
		//	INT32 idxCard = i3::atoi(pParam->m_pCtrl->GetName());

		//	m_CardBG[idxCard]->SetShapeIdx(I3UI_BUTTON_SHAPE_PUSHED);
		//}
		//else if( pParam->m_nEvent == I3UI_EVT_LEAVEMOUSE && CheckEventCardSlot(pParam->m_pCtrl, m_CardSlot) )
		//{
		//	INT32 idxCard = i3::atoi(pParam->m_pCtrl->GetName());

		//	MCard* pCard = MCardUtil::GetSelectedMCard();
		//	INT32 selCardIdx =  (pCard) ? pCard->GetMyIndex() : -1;

		//	if( selCardIdx != idxCard )
		//		m_CardBG[idxCard]->SetShapeIdx(I3UI_BUTTON_SHAPE_NORMAL);
		//}
	}

	return UITabProfileBase_V10::OnEvent( event, pObj, param2, code);
}

/*virtual*/ bool UITabProfileMissionCard_V10::OnClick( i3UIControl * pControl)
{
	if( UITabProfileBase_V10::OnClick( pControl) == false)
		return false;

	if( i3::same_of<i3UIListView_Item*>(pControl)) 
	{
		PlaySound( UISND_CLICK);
	}

	return true;
}

//
void UITabProfileMissionCard_V10::UpdatePrize_Lua()
{
	if( IsReadyToUpdate() == false) return;

	// 보상을 표시한다.
	INT32 prize = 0;
	INT32 point = 0;
	INT32 exp = 0;
//
		
//	MCardSet* pCardSet = MCardUtil::GetSelectedMCardSet();
//	MCard* pCard = MCardUtil::GetSelectedMCard();
		
	// 선택된 셋이 실제로 서버에 현재 선택된 세트인 경우에만 표시????
	if ( MCardUtil::IsServerListIdxCurrentSelectedSet( m_selectedCardSetIndexFromList ) )
	{	
		MCard* pCard = MCardUtil::GetSelectedMCard();

		if (pCard)
		{
			const MCardInfo* pCardInfo = pCard->GetMCardInfo();
			prize = pCardInfo->mCardPrize.prize;
			point  = pCardInfo->mCardPrize.point;
			exp    = pCardInfo->mCardPrize.exp;
		}
	}

	BADGE_TYPE BadgeType = PrizeToBadgeType((UINT8)prize);

	//Badge Type Lua이미지 출력
	LuaState * L = _CallLuaFunction("SetBadgeType");
	i3Lua::PushInteger(L, (INT32)BadgeType);
	_EndLuaFunction(L, 1);

	const i3::rc_wstring descriptionText = MCardUtil::GetPrizeDescription(BadgeType);
	i3::rc_wstring wstrNameText;

	switch (BadgeType)
	{
	case BADGE_TYPE_RIBBON:
		wstrNameText = GAME_RCSTRING("STR_TBL_PROFILE_RIBBON_TEXT2");	/*약장*/
	break;

	case BADGE_TYPE_ENSIGN:
		wstrNameText = GAME_RCSTRING("STR_TBL_PROFILE_ENSIGN_TEXT2");	/*휘장*/
	break;

	case BADGE_TYPE_MEDAL:
		wstrNameText = GAME_RCSTRING("STR_TBL_PROFILE_MEDALS_TEXT2");	/*훈장*/
	break;

	case BADGE_TYPE_MASTER:
		wstrNameText = GAME_RCSTRING("STR_TBL_PROFILE_MEDALS_TEXT2");	/*훈장*/
	break;
	}

	i3::stack_wstring pointText;
	i3::stack_wstring expText;

	i3::itoa(point, pointText);
	i3::itoa(exp, expText);

	L = _CallLuaFunction( "SetPrizeName");
	i3Lua::PushStringUTF16To8( L, wstrNameText);
	_EndLuaFunction( L, 1);

	L = _CallLuaFunction( "SetPrizeDescription");
	i3Lua::PushStringUTF16To8( L, descriptionText);
	_EndLuaFunction( L, 1);

	L = _CallLuaFunction( "SetPrizePoint");
	i3Lua::PushStringUTF16To8( L, pointText);
	_EndLuaFunction( L, 1);

	L = _CallLuaFunction( "SetPrizeExp");
	i3Lua::PushStringUTF16To8( L, expText);
	_EndLuaFunction( L, 1);

}

//

void UITabProfileMissionCard_V10::SelectCard( INT32 cardIdx) // 카드탭에서 카드를 선택했을시.
{
	// 해당 카드셋이 구매 소유중인게 아닌 케이스라면..
	if ( MCardUtil::IsServerListIdxCurrentSelectedSet(m_selectedCardSetIndexFromList) == false )
	{
		return;
	}

	if( cardIdx < 0 || cardIdx >= MAX_CARD_PER_CARDSET )
		return;

	bool bSelectEnable = false;

	MCardSet* pSet = MCardUtil::GetSelectedMCardSet();

	if (pSet != nullptr)
	{
		MCard* pCard = pSet->GetMCard(cardIdx);
		bSelectEnable = pCard->IsSelectEnableCard();
	}

	// 진행 카드 변경 및 활성화
	if( bSelectEnable )
	{
		MCardUtil::SelectActiveCardToServer( cardIdx );
	}
	else
	{
		INT32 cardSetType = (pSet) ? pSet->GetMCardSetInfo()->setID : QUEST_CARDSET_TYPE_NONE;
		// 튜토리얼 임무카드는 없는 경우 선택안되게한다.
		if ( QUEST_CARDSET_TYPE_NONE == cardSetType
			|| QUEST_CARDSET_TYPE_TUTORIAL == cardSetType)
		{
			LuaState * L = _CallLuaFunction("SetAllDefault");
			_EndLuaFunction(L,0);
		}
	}
	
	UITabProfileMissionCard_V10::UpdateCardSlotsAndInfoUI();

}

//
void UITabProfileMissionCard_V10::SelectCardSetType_Server(INT32 serverIdx) // 리스트박스에서 카드를 선택했을시 호출
{
	m_selectedCardSetIndexFromList = serverIdx;

	INT32 cardSetType = MCardUtil::GetMCardSetType_Server(serverIdx);
	INT32 slotIdx = MCardUtil::FindCardSetSlotWithCardSetType(cardSetType);

	if (slotIdx != -1)		// -1인 경우는 보유하고 있지 않음... 보유한게 아니면 서버에 셀렉션을 보낼 필요없음..
	{
		MCardUtil::SelectActiveCardSetToServer(slotIdx);
	}
	
	UITabProfileMissionCard_V10::UpdateCardSlotsAndInfoUI();
}


namespace
{
	enum CARD_SET_TEXT_COLOR
	{
		CARD_SET_USER_HAVE = 0,
		CARD_SET_TUTORIAL,
		CARD_SET_DEFAULT,
		CARD_SET_TEXT_COLOR_MAX,
	};
}

void	UITabProfileMissionCard_V10::InitCardList_Lua()
{
	// 루아쪽 부터 그냥 지우고 시작..
	LuaState * L = _CallLuaFunction("ClearMissionList");
	_EndLuaFunction(L,0);

	//	각 나라별로 이벤트 임무카드를 사용하지 않는다면 Config/ConfigEx.PEF 파일에서 
	//	QuestCard 항목의 13_EVENT 에서 값을 0으로 하고 파일을 배포하면 해당 클라이언트에서는 이벤트 임무카드가 안보입니다.

	const INT32 currentCardSetCount_Server = MCardUtil::GetMCardSetCount_Server();
	i3::rc_wstring CardSetNameColor;

	for( INT32 i = 0; i < currentCardSetCount_Server ; ++i)
	{
		INT32 cardSetType = MCardUtil::GetMCardSetType_Server(i);
		bool HasCard = MCardUtil::HasMCardSet(cardSetType);
		
		INT32 CardColorType = -1;
		if (HasCard)
		{
			CardColorType = CARD_SET_USER_HAVE;
		}
		else
		{
			if (QUEST_CARDSET_TYPE_TUTORIAL == cardSetType || QUEST_CARDSET_TYPE_DINO_TUTORIAL == cardSetType ||
				QUEST_CARDSET_TYPE_HUMAN_TUTORIAL == cardSetType)
				CardColorType = CARD_SET_TUTORIAL;
			else
				CardColorType = CARD_SET_DEFAULT;
		}

		switch(CardColorType)
		{
		case CARD_SET_USER_HAVE:
			CardSetNameColor += L"{col:255,211,3}";
			break;
		case CARD_SET_TUTORIAL:
			CardSetNameColor += L"{col:155,155,155}";
			break;
		case CARD_SET_DEFAULT:
			CardSetNameColor += L"{col:255,255,255}";
			break;
		}
		
		const MCardSetInfo* pInfo =	MCardUtil::FindMCardSetInfoBySetType(cardSetType);

		if (pInfo == nullptr)
			pInfo = MCardUtil::FindMCardSetInfoBySetType(QUEST_CARDSET_TYPE_TUTORIAL);
		
		I3ASSERT(pInfo != nullptr);
		
		//const i3::rc_wstring& wstrCardSetName = pInfo->wstrName;
		CardSetNameColor += pInfo->wstrName;
		CardSetNameColor += L"{/col};";
	}

	// Text Setting
	L = _CallLuaFunction("SetCardList");
	i3Lua::PushStringUTF16To8(L, CardSetNameColor);
	i3Lua::PushInteger(L, 0);
	_EndLuaFunction(L, 2);
	
	if(currentCardSetCount_Server == 0)
	{
		L = _CallLuaFunction("SetCardList");
		i3Lua::PushStringUTF16To8(L, L"Mission card's not detected.");
		i3Lua::PushInteger(L, 0);
		_EndLuaFunction(L, 2);

		L = _CallLuaFunction("SetAllDefault");
		_EndLuaFunction(L,0);
	}
}



void UITabProfileMissionCard_V10::_ChangeActiveCardSet(INT32 cardSetType)
{
	INT32 slotIdx = MCardUtil::FindCardSetSlotWithCardSetType(cardSetType);

	if (slotIdx != -1)
	{
		MCardUtil::SelectActiveCardSetToServer(slotIdx);
	}
}
//

void UITabProfileMissionCard_V10::_UpdateChallengeCardInfo()
{
	if( IsReadyToUpdate() == false) return;

	INT32 cardSetType = MCardUtil::GetMCardSetType_Server(m_selectedCardSetIndexFromList);
	
	if (cardSetType == QUEST_CARDSET_TYPE_NONE )	// 리스트셀렉션은 셋슬롯과 다름..
	{
		LuaState * L;

		L = _CallLuaFunction("SetBuyButtonEnable");
		i3Lua::PushBoolean(L, false);
		_EndLuaFunction(L, 1);
	
		L = _CallLuaFunction("SetInfoButtonEnable");
		i3Lua::PushBoolean(L, false);
		_EndLuaFunction(L, 1);
	
		L = _CallLuaFunction("SetDeleteButtonEnable");
		i3Lua::PushBoolean(L, false);
		_EndLuaFunction(L, 1);
		return;
	}
	
	// 현재 진행중인 카드셋 종류 표시  ( 쓰이지 않아서 통으로 생략...2015.06.19.수빈)
	
	// 튜토리얼, 이벤트 카드의 경우 구입할 수 없다.

		
	if( QUEST_CARDSET_TYPE_TUTORIAL == cardSetType || QUEST_CARDSET_TYPE_EVENT == cardSetType)
	{
		{
			LuaState * L = _CallLuaFunction("SetBuyButtonEnable");
			i3Lua::PushBoolean(L, false);
			_EndLuaFunction(L, 1);
		}

		{
			LuaState * L = _CallLuaFunction("SetInfoButtonEnable");
			i3Lua::PushBoolean(L, true);
			_EndLuaFunction(L, 1);
		}

		{
			LuaState * L = _CallLuaFunction("SetDeleteButtonEnable");
			i3Lua::PushBoolean(L, false);
			_EndLuaFunction(L, 1);
		}
	}
	else
	{
		{
			LuaState * L = _CallLuaFunction("SetBuyButtonEnable");
			i3Lua::PushBoolean(L, true);
			_EndLuaFunction(L, 1);
		}

		{
			LuaState * L = _CallLuaFunction("SetInfoButtonEnable");
			i3Lua::PushBoolean(L, true);
			_EndLuaFunction(L, 1);
		}

		// 카드셋을 가지고 있을때만 삭제가 가능하다.
		{
			bool bDeleteEnable = MCardUtil::IsServerListIdxCurrentSelectedSet(m_selectedCardSetIndexFromList);
			LuaState * L = _CallLuaFunction("SetDeleteButtonEnable");
			i3Lua::PushBoolean(L, bDeleteEnable);
			_EndLuaFunction(L, 1);
		
		}
	}

}

void UITabProfileMissionCard_V10::_UpdateChallengeSlotState()
{
	if( IsReadyToUpdate() == false) return;


	// 활성화 카드가 없는 경우 모든 임무 비활성화
	MCardSet* pSelSet = MCardUtil::GetSelectedMCardSet();
	MCard* pSelCard = MCardUtil::GetSelectedMCard();
	INT32  cardSetType = (pSelSet) ? pSelSet->GetMCardSetInfo()->setID : QUEST_CARDSET_TYPE_NONE;

	
	if (MCardUtil::IsServerListIdxCurrentSelectedSet(m_selectedCardSetIndexFromList) == false || 
		 pSelCard == nullptr || 
		( pSelCard->IsBattleAvailableCard() == false && cardSetType == QUEST_CARDSET_TYPE_TUTORIAL) )
	{
		for( INT32 i = 0; i < MAX_CARDSET_PER_USER; i++)
		{
			SetCCardNotice(i, L"");
			SetCCardComplete(i, L"");
			SetCCardTItle(L"");
		}
		return;
	}
	
	if (pSelSet == nullptr)
	{
		return;
	}

	// Card Title을 세팅합니다.
	SetCCardTItle(pSelSet->GetMCardSetInfo()->wstrTitle);

	for( INT32 i = 0; i < MAX_QUEST_PER_CARD_NEW; i++)
	{
		MCardQuest* pQuest = pSelCard->GetMCardQuest(i);
		
		if (pQuest->IsActive() == false)
			continue;

		i3::rc_wstring stateText;

		if ( pSelCard->IsQuestCompleted_OffBattle(i) == false )
		{
			stateText = GAME_RCSTRING("STR_TBL_PROFILE_CHALLENGE_TEXT");/*임무*/ 
		}
		else
		{
			stateText = GAME_RCSTRING("STR_TBL_PROFILE_COMPLETE_TEXT");/*완료*/ 	
		}

		i3::rc_wstring wstrStateText = MCardUtil::GetCurrHUDDesc(i);

		SetCCardComplete(i, stateText);
		SetCCardNotice(i, wstrStateText);
	}
}

//
void UITabProfileMissionCard_V10::_UpdateCardSlotImage()
{
	if( IsReadyToUpdate() == false) return;

	INT32 cardSetTypeOfList = QUEST_CARDSET_TYPE_NONE;

	if (m_selectedCardSetIndexFromList != -1)
		cardSetTypeOfList = MCardUtil::GetMCardSetType_Server(m_selectedCardSetIndexFromList);
	
	i3UIControl *cardTemp = nullptr;
	COLOR cardColor;

	//템플릿을 해당 Select된 카드로 교체해 준다.

	//임무카드 이미지 불러오기//////////////////////////////////////////////////////////////////////
	INT32 cardSetTypeForUI = cardSetTypeOfList;

	if (cardSetTypeForUI == QUEST_CARDSET_TYPE_NONE)
		cardSetTypeForUI = QUEST_CARDSET_TYPE_TUTORIAL;	
	
	const MCardSetInfo* pCardSetInfo = MCardUtil::FindMCardSetInfoBySetType(cardSetTypeForUI);
	I3ASSERT(pCardSetInfo);
	
	i3::stack_string strTemplate;
	i3::utf16_to_mb(pCardSetInfo->wstrUITemplateName, strTemplate);

	for(INT32 i = 0 ; i < MAX_CARD_PER_CARDSET; i++)
	{
		((i3UIControl *)m_CardSlot[i])->SetTemplateByName(strTemplate.c_str());
		m_CardSlot[i]->SetShapeIdx(i);
	}

	if(m_pCardInfo != nullptr)
	{
		((i3UIControl*) m_pCardInfo)->SetTemplateByName(strTemplate.c_str());
	}

	///////////////////////////////////////////////////////////////////////////////////////////////

	INT32 setSlot = MCardUtil::FindCardSetSlotWithCardSetType( cardSetTypeOfList);		// 실제로 보유중인지 여부..
	MCardSet* pCurrSet = MCardUtil::GetSelectedMCardSet();
	bool isSelectedSet = (pCurrSet && pCurrSet->GetMyIndex() == setSlot );
	
	for( INT32 i = 0; i < MAX_CARD_PER_CARDSET; i++)
	{
		//g_pFramework->SetCardShape( m_cardSlot[i]._pCard, GetSelectedCardsetID(), i);
		//SetCardImageShape(i, reinterpret_cast<INT32>GetSelectedCardsetID());
		cardTemp = (i3UIControl *)m_CardSlot[i];
		if(  isSelectedSet  )
		{
			MCard* pCard = pCurrSet->GetMCard(i);
			if( pCard->IsActive() )
			{
				if( QUEST_CARDSET_TYPE_TUTORIAL == cardSetTypeOfList )
				{
					// 튜토리얼 임무카드의 경우 순차적으로 진행/출력된다.
					// 첫 카드는 무조건 활성화된다.
					// 앞카드가 완료되어야 활성화 할 수 있다.
					if( pCard->IsAllQuestCompleted_OffBattle() )
					{
						cardColor.r = 255;	cardColor.g = 255;	cardColor.b = 255;	cardColor.a = 255;
						cardTemp->setColor(&cardColor);
						//m_cardSlot[i]._pCard->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 255);
					}
					else if (0 == i ||	pCurrSet->GetMCard(i-1)->IsAllQuestCompleted_OffBattle() ) 
					{
						cardColor.r = 255;
						cardColor.g = 255;
						cardColor.b = 255;
						cardColor.a = 165;
						cardTemp->setColor(&cardColor);
						//m_cardSlot[i]._pCard->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 165);
					}
					else
					{
						cardColor.r = 255;
						cardColor.g = 255;
						cardColor.b = 255;
						cardColor.a = 51;
						cardTemp->setColor(&cardColor);
						//m_cardSlot[i]._pCard->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 51);
					}
				}
				else
				{
					// 그 이외 임무카의 경우 원하는 것먼저 진행/출력할 수 있다.
					if(	pCard->IsAllQuestCompleted_OffBattle() ) 
					{
						cardColor.r = 255;	cardColor.g = 255;	cardColor.b = 255;	cardColor.a = 255;
						cardTemp->setColor(&cardColor);
					}
					else
					{
						cardColor.r = 255;	cardColor.g = 255;	cardColor.b = 255;	cardColor.a = 165;
						cardTemp->setColor(&cardColor);
					}
				}
			}
			else
			{
				cardColor.r = 255;	cardColor.g = 255;	cardColor.b = 255;	cardColor.a = 51;
				cardTemp->setColor(&cardColor);
			}
		}
		else
		{
			cardColor.r = 255;	cardColor.g = 255;	cardColor.b = 255;	cardColor.a = 51;
			cardTemp->setColor(&cardColor);
		}

	}
}

//

void UITabProfileMissionCard_V10::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	
	switch( event)
	{
	case EVENT_QUEST_BUY_CARD_SET :
		if (EV_SUCCESSED(arg))
		{
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_BASE_BUY_CARD"));/*카드한벌 구입에 성공하였습니다.*/ 
	
			InitCardList_Lua();
			//SetMissionPossibleCount_Lua();
			
			UITabProfileMissionCard_V10::UpdateCardSetListSelection();
			UITabProfileMissionCard_V10::UpdateCardSlotsAndInfoUI();
		}
		else
		{
			if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_BUY_CARD_PARAMETER_FAIL))	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_GUI_BASE_FAIL_BUY_CARD_BY_NO_CARD_INFO"));/*구입할 카드 정보를 찾지못해 구매에 실패하였습니다.*/
			else if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_BUY_CARD_PRICE_FAIL))	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_GUI_BASE_NO_POINT_TO_GET_ITEM"));/*잔액이 부족하여 아이템을 구입할 수 없습니다.*/
			else if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_BUY_CARD_UNKOWN_FAIL))	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_GUI_BASE_LIMIT_CARD_COUNT"));/*보유할 수 있는 임무카드의 종류는\n총 3종으로 제한되어 있습니다.\n \n현재 보유하신 임무를 모두 완수하거나 삭제 하신 후 다시 구입해주시기 바랍니다.*/
			else if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_BUY_CARD_DISQUALIFIED_FAIL))	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_GUI_BASE_DID_NOT_TUTORIAL_MISSION_CARD"));

			else I3ASSERT_0;
		}
		break;
	case EVENT_QUEST_DELETE_CARD_SET :
		if (EV_SUCCESSED(arg))
		{
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_BASE_DELETE_CARD"));/*카드한벌을 삭제하였습니다.*/

			InitCardList_Lua();
			////SetMissionPossibleCount_Lua();

			//if ( m_nextSetTypeForDelete != QUEST_CARDSET_TYPE_NONE)
			//{
			//	MCardUtil::SelectActiveCardSetToServerWithSetType(m_nextSetTypeForDelete);
			//}
			//UITabProfileMissionCard_V10::UpdateCardSetListSelection();		// 왼쪽 카드셋 목록 셀렉션 갱신..
			UITabProfileMissionCard_V10::UpdateCardSlotsAndInfoUI();		// 오른쪽 개별카드 관련 정보 갱신..
			
		}
		else
		{
			if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_DELETE_CARD_PARAMETER_FAIL))		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_BASE_FAIL_DELETE_CARD_BY_NOT_INFO"));/*삭제할 카드 정보를 찾지못해 삭제에 실패하였습니다.*/
			else if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_DELETE_CARD_UNKOWN_FAIL))	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_BASE_FAIL_DELETE_ITEM_BY_NETWORK_PROBLEM"));/*네트워크 문제로 아이템의 삭제를 실패하였습니다.*/
			else I3ASSERT_0;
		}
		break;
	}
}




void UITabProfileMissionCard_V10::CardInfo()
{
	UIMainFrame *pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT(pFrame);
	// 선택된 카드의 인덱스

	INT32 cardSetType = MCardUtil::GetMCardSetType_Server( m_selectedCardSetIndexFromList );

	pFrame->TogglePopup(UPW_MISSIONINFO, ((void*)&cardSetType));		// 팝업 구현은 나중에(2015.06.22.수빈)

}

namespace
{

	bool CheckBuyQuestCard(INT32 cardSetType)
	{
		bool Rv = true; 
	//
		INT32 MyRank = UserInfoContext::i()->GetMyRank();
		
		switch(cardSetType)
		{
		case QUEST_CARDSET_TYPE_COMMISSIONED_O	: 
			if( MyRank < 5)//하사 이상
			{
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_QEUST_ERROR_COMMISSIONED"));
				Rv = false; 
			}
			break; 
		case QUEST_CARDSET_TYPE_COMPANY_O		: 
			if( MyRank < 17)//소위 이상
			{
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_QEUST_ERROR_COMPANY"));
				Rv = false; 
			}
			break; 
		case QUEST_CARDSET_TYPE_FIELD_O			: 
			if( MyRank < 31)//소령 이상
			{
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_QEUST_ERROR_FIELD"));
				Rv = false; 
			}
			break; 		
		}

		return Rv; 
	}

}

void UITabProfileMissionCard_V10::CardBuy()
{
	UIMainFrame *pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT(pFrame);
	// 선택된 카드의 인덱스
	INT32 cardSetType = MCardUtil::GetMCardSetType_Server(m_selectedCardSetIndexFromList);

	// GetSelectedCardSetType();

	if(!CheckBuyQuestCard(cardSetType))
		return; 
	
	if( MCardUtil::HasMCardSet(cardSetType) )
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_PROFILE_CARDSET_DUPLICATE"));
		return;
	}

	pFrame->TogglePopup(UPW_MISSIONSHOP, ((void*)&cardSetType));	// 팝업 구현은 나중에(2015.06.22.수빈)
}

namespace
{
	void CbDeleteCardSet(void* pThis,INT32 nParam)
	{
		if (MBR_OK == nParam)
		{
			// 가지고 있는 카드셋만 삭제할 수 있다.
			// 현재 활성화 되어있다면, 가지고 있는 카드셋이다.
			MCardSet* pCardSet = MCardUtil::GetSelectedMCardSet();

			if (pCardSet != nullptr)		
			{
				UINT8 cardSetSlot = (UINT8)pCardSet->GetMyIndex();
				GameEventSender::i()->SetEvent(EVENT_QUEST_DELETE_CARD_SET, &cardSetSlot);
			}
		}
	}
	
}


void UITabProfileMissionCard_V10::CardDelete()
{
	// 가지고 있는 카드셋만 삭제가 가능하다.
	// 활성화되어있다면, 가지고 있는 카드셋이다.
	if (MCardUtil::IsServerListIdxCurrentSelectedSet(m_selectedCardSetIndexFromList))
	{
		MCardSet* pNextSet = MCardUtil::FindNextAvailableCardSet();

		m_nextSetTypeForDelete = (pNextSet) ? pNextSet->GetMCardSetInfo()->setID : QUEST_CARDSET_TYPE_NONE;

		GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_TBL_PROFILE_DELETE_CARD_MESSAGE"), nullptr, MAKE_CALLBACK(CbDeleteCardSet), this);
		/*선택하신 종류의 모든 임무카드는\n삭제되며, 복구할 수 없습니다.\n \n그래도 삭제하시겠습니까?*/
	}
}

//
void UITabProfileMissionCard_V10::SetCCardNotice(INT32 idx, const i3::rc_wstring& wstrText)
{
	LuaState * L = _CallLuaFunction("SetCCardNotice");
	i3Lua::PushInteger(L, idx);
	i3Lua::PushStringUTF16To8(L, wstrText);
	_EndLuaFunction(L, 2);
}

void UITabProfileMissionCard_V10::SetCCardComplete( INT32 idx, const i3::rc_wstring& wstrText )
{
	LuaState * L = _CallLuaFunction("SetCCardComplete");
	i3Lua::PushInteger(L, idx);
	i3Lua::PushStringUTF16To8(L, wstrText);
	_EndLuaFunction(L, 2);
}

void UITabProfileMissionCard_V10::OnCreate( i3GameNode * pParent )
{
	UITabProfileBase_V10::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Main/PointBlankRe_MyInfomation_MissionCard.i3UIs", UITabProfileMissionCard_V10_Glue);
}

void UITabProfileMissionCard_V10::UpdateCardSetListSelection()
{
	const MCardSet* pCardSet = MCardUtil::GetSelectedMCardSet();

	m_selectedCardSetIndexFromList = -1;				// 카드셋타입이 아니고, 서버로부터 받은 목록 인덱스이다..
	INT32 cardSetType = QUEST_CARDSET_TYPE_NONE;

	if (pCardSet != nullptr)
	{
		cardSetType = pCardSet->GetMCardSetInfo()->setID;
		m_selectedCardSetIndexFromList =	MCardUtil::GetServerIndexWithMCardSetType(cardSetType);
	}
	else	// 셀렉션이 없으면 그냥 목록에서 가장 첫번째 것을 고르게 조정한다..
	{
		MCardSet* pNextSet = MCardUtil::FindNextAvailableCardSet();

		if (pNextSet)
		{
			MCardUtil::SelectActiveCardSetToServer(pNextSet->GetMyIndex());
			cardSetType = pNextSet->GetMCardSetInfo()->setID;
			m_selectedCardSetIndexFromList =	MCardUtil::GetServerIndexWithMCardSetType(cardSetType);
		}
	}

	if(cardSetType != QUEST_CARDSET_TYPE_NONE)				//초기 진입시 그전에 활성화된 CardSet과 CardIndex을 맞춰줌
	{
		LuaState * L = _CallLuaFunction("SetCardListSelection");
		i3Lua::PushInteger(L, m_selectedCardSetIndexFromList);
		_EndLuaFunction(L,1);
	}
}

void UITabProfileMissionCard_V10::SetCCardTItle( const i3::rc_wstring& wstrText )
{
	// CardSetTitle 설정합니다
	LuaState * L = _CallLuaFunction("SetMissionCardTitle");
	if(L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, wstrText);
		_EndLuaFunction(L, 1);
	}
}

void UITabProfileMissionCard_V10::_SetMissionBoard()
{
	UIFloatMissionBoard_V10 * missionboard = (UIFloatMissionBoard_V10*)GetMainFrame()->GetFloating(UFW_MISSIONBOARD);
	if( missionboard != nullptr)
	{
		missionboard->SetMissionCardSlot();
	}
}
