#include "pch.h"
#include "UIPopupRewardItem.h"
#include "UIMainFrame.h"
#include "UIUtil.h"

#include "../StepProfileDef.h"
#include "../QuestTextDictionary.h"

//#include "UIPopupQueryUseItem.h"
#include "ItemImageMgr.h"


I3_CLASS_INSTANCE( UIPopupRewardItem);//, UIPopupBase);

static UIPopupRewardItem * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);
	return (UIPopupRewardItem*) pFrame->GetPopup( UPW_REWARDITEM );
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//

extern "C" {
	int UIPopupRewardItem_Confirm(LuaState * L)
	{
		GetThis()->Confirm();
		return 0;
	}
}

LuaFuncReg	UIPopupRewardItem_Glue[] =
{
	{ "Confirm",	UIPopupRewardItem_Confirm },
	{ NULL,			NULL }
};




UIPopupRewardItem::UIPopupRewardItem()
{
}

UIPopupRewardItem::~UIPopupRewardItem()
{
}

/*virtual*/ void UIPopupRewardItem::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);
	AddScene( "UIRe/Scene/Popup/PBRe_PopUp_missionCard_Complete.i3UIe",  UIPopupRewardItem_Glue);
}

/*virtual*/ void UIPopupRewardItem::OnLoadAllScenes()
{
	m_LeftImgBox = GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), "i3UIButtonItem1");
	m_RightImgBox = GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), "i3UIButtonItem2");
}

/*virtual*/ void UIPopupRewardItem::OnUnloadAllScenes()
{
	I3_SAFE_RELEASE(m_LeftImgBox);
	I3_SAFE_RELEASE(m_RightImgBox);
}

/*virtual*/ bool UIPopupRewardItem::OnEntranceStart( void * pArg1, void * pArg2)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;
	UINT8 ActiveCardSet = g_pGameContext->getQuestManager()->GetActiveCardsetID();

	_InitUI();
	_SetBasicInfo(0,0);
	_SetPrizeInfo((CARDSET_ID)ActiveCardSet);

	//팝업이 표시된 이후 퀘스트 초기화
	g_pGameContext->getQuestManager()->ResetGainedPrizeByBattle();
	return true;
}

void UIPopupRewardItem::_InitUI(void)
{
	LuaState * L = _CallLuaFunction("InitUI");
	_EndLuaFunction(L,0);
}


void UIPopupRewardItem::_SetBasicInfo(INT32 bonusExp, INT32 bonusPoint)
{
	CQuestManager * pQuestManager = g_pGameContext->getQuestManager();

	I3ASSERT( pQuestManager != NULL);
	//I3ASSERT( pQuestManager->isQuestActivated());

	UINT8		prize		= static_cast<UINT8>(pQuestManager->GetGainedPrizeByBattle());
	BADGE_TYPE	BadgeType	= PrizeToBadgeType(prize);

	// 보상 내용에 대해서 출력합니다.
	char expText[MAX_STRING_COUNT];
	char pointText[MAX_STRING_COUNT];

	INT32 totalExp		= pQuestManager->getGainedTotalExp();
	INT32 totalPoint	= pQuestManager->getGainedTotalPoint();

	sprintf_s(expText, GAME_STRING("STR_TBL_GUI_READY_REWARD_EXP"), totalExp + bonusExp );/*보상 경험치: %d*/
	sprintf_s(pointText, GAME_STRING("STR_TBL_GUI_READY_REWARD_POINT"), totalPoint + bonusPoint );/*보상 Point: %d*/
	
	LuaState * L = _CallLuaFunction("SetBasicInfo");
	i3Lua::PushInteger(L, (INT32)BadgeType);
	i3Lua::PushA2UTF8String(L, expText);
	i3Lua::PushA2UTF8String(L, pointText);
	_EndLuaFunction(L, 3);
}


void UIPopupRewardItem::_SetPrizeInfo( CARDSET_ID CardsetID)
{
	bool previewMode = false;

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	if( g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LCTRL ) &&
		g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LALT) )
	{
		previewMode = true;
	}
#endif
	//최종 보상창이 아니라면 나간다
	if(!previewMode && g_pGameContext->IsFinalPrizeFlag() == FALSE)
	{
		return;
	}
	g_pGameContext->RemoveGameFlag( GAMEFLAG_FINALPRIZE);

	// 보상 확인
	switch(CardsetID)
	{
		case QUEST_CARDSET_TYPE_TUTORIAL:
		case QUEST_CARDSET_TYPE_DINO_TUTORIAL:
		case QUEST_CARDSET_TYPE_HUMAN_TUTORIAL:
		case QUEST_CARDSET_TYPE_DINO_2ND:
		case QUEST_CARDSET_TYPE_HUMAN_2ND:
			//{
			//	INT32 ItemID = g_pQuestTextDictionary->GetCardSetBonusItemID(CardsetID);
				//GetItemImageMgr()->SelectImage(m_pImageSet, pGoods);
			//}
			//break;
		case QUEST_CARDSET_TYPE_ASSAULT			:
		case QUEST_CARDSET_TYPE_BACKUP			:
		case QUEST_CARDSET_TYPE_INFILTRATION	:
		case QUEST_CARDSET_TYPE_SPECIAL			:
		case QUEST_CARDSET_TYPE_DEFCON			:
		case QUEST_CARDSET_TYPE_COMMISSIONED_O	:
		case QUEST_CARDSET_TYPE_COMPANY_O		:
		case QUEST_CARDSET_TYPE_FIELD_O			:
		case QUEST_CARDSET_TYPE_EVENT			:
		{
			CQuestManager * pQuestManager = g_pGameContext->getQuestManager();
			I3ASSERT( pQuestManager != NULL);

			INT32 iGetMedal = _GetRewardMedalByCardSet( CardsetID );
			if(iGetMedal != -1)
				_SetFinalMedal(true);
			else
				_SetFinalMedal(false);
			CARDSET_ID CarsetID = pQuestManager->GetActiveCardsetID();

			INT32 bonusPoint = g_pQuestTextDictionary->GetCardSetBonusPoint(CardsetID);	// 보너스 포인트
			INT32 bonusExp = g_pQuestTextDictionary->GetCardSetBonusExp(CardsetID);		// 보너스 경험치

			_SetBasicInfo(bonusExp,bonusPoint);

			//임무카드 보상 아이템 관련 일단 하드코딩 처리
			if(CarsetID != QUEST_CARDSET_TYPE_NONE)
			{
				INT32 i;
				INT32 m_nItemIDCount = g_pQuestTextDictionary->GetCardSetBonusItemIDCount(CarsetID);
				for(i = 0; i < m_nItemIDCount; i++)
				{
					m_nItemIDForCardSetComplete[i] = g_pQuestTextDictionary->GetCardSetBonusItemID(CardsetID, i);
					if( i == 0)
					{
						m_LeftImgBox->SetEnable(TRUE);
						GetItemImageMgr()->SelectImage(m_LeftImgBox, m_nItemIDForCardSetComplete[i]);
					}
					else if( i== 1)
					{
						m_RightImgBox->SetEnable(TRUE);
						GetItemImageMgr()->SelectImage(m_RightImgBox,m_nItemIDForCardSetComplete[i]);
					}
				}
				if( i == 1)
				{
					i3UIButtonImageSet * L_Ctrl = (i3UIButtonImageSet *)(GetScene(0)->FindChildByName("i3UIButtonItem1"));
					VEC3D *origin_pos;
					REAL32 resize = L_Ctrl->getWidth() / 2;
					origin_pos = L_Ctrl->getPos();
					origin_pos->x = origin_pos->x + resize + 4; 
					L_Ctrl->setPos(origin_pos);
				}
			}
		}
			break;
		default:
			break;
	}
	// 퀘스트를 삭제 및 다음 퀘스트 카드가 있다면 강제 셀렉트.
	CQuestManager * QuestManager = g_pGameContext->getQuestManager();
	bool success = QuestManager->RemoveActiveCardset();
	if ( ! success )
	{
		I3ASSERT_0;
	}
	if( QuestManager->_SetNextAvailableCardSetIndex() != -1)
	{
		if( QuestManager->NoticeChangeActiveIndex() == false)
			I3NOTICE("Not Available Cardset.\n");
	}
}

void UIPopupRewardItem::_SetFinalMedal(bool Sig)
{
	LuaState * L = _CallLuaFunction("SetFinalMedal");
	i3Lua::PushBoolean(L, Sig);
	_EndLuaFunction(L, 1);
}

INT32 UIPopupRewardItem::_GetRewardMedalByCardSet( UINT32 cardSetId)
{
	//	임무카드 보상 메달 (없으면 -1 반환)
	switch( cardSetId)
	{
	case QUEST_CARDSET_TYPE_ASSAULT:
	case QUEST_CARDSET_TYPE_BACKUP:
	case QUEST_CARDSET_TYPE_INFILTRATION:
	case QUEST_CARDSET_TYPE_SPECIAL:
	case QUEST_CARDSET_TYPE_DEFCON:
	case QUEST_CARDSET_TYPE_COMMISSIONED_O:
	case QUEST_CARDSET_TYPE_COMPANY_O:
	case QUEST_CARDSET_TYPE_FIELD_O:
		return FIXED_BADGE_NUM_MASTER;
		break;
	}
	return -1;
}


void UIPopupRewardItem::Confirm(void)
{
	GetMainFrame()->TogglePopup(UPW_REWARDITEM);
}

