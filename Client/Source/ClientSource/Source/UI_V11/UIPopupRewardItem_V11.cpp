#include "pch.h"
#include "UIPopupRewardItem_V11.h"
#include "UI/UIMainFrame_V11.h"
#include "UI/UIUtil.h"

#include "../StepProfileDef.h"

//#include "UIPopupQueryUseItem.h"
#include "UI/ItemImageMgr.h"

#include "MissionCard/MCardSet.h"
#include "MissionCard/MCardUtil.h"
#include "MissionCard/MCardSetInfo.h"
#include "MissionCard/MCardInfo.h"


I3_CLASS_INSTANCE( UIPopupRewardItem_V11);

static UIPopupRewardItem_V11 * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);
	return (UIPopupRewardItem_V11*) pFrame->GetPopup( UPW_REWARDITEM );
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//

extern "C" {
	int UIPopupRewardItem_V11_Confirm(LuaState * L)
	{
		GetThis()->Confirm();
		return 0;
	}
}

LuaFuncReg	UIPopupRewardItem_Glue[] =
{
	{ "Confirm",	UIPopupRewardItem_V11_Confirm },
	{ nullptr,			nullptr }
};




UIPopupRewardItem_V11::UIPopupRewardItem_V11()
{
}

UIPopupRewardItem_V11::~UIPopupRewardItem_V11()
{
}

/*virtual*/ void UIPopupRewardItem_V11::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);
	AddScene( "UIRe/PBRe_PopUp_missionCard_Complete.i3UIe",  UIPopupRewardItem_Glue);
}

/*virtual*/ void UIPopupRewardItem_V11::OnLoadAllScenes()
{
	m_ImgBox[0] = GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), "i3UIButtonItem1");
	m_ImgBox[1] = GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), "i3UIButtonItem2");
	m_ImgBox[2] = GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), "i3UIButtonImageSet");
	m_ImgBox[3] = GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), "i3UIButtonImageSet0");
}

/*virtual*/ void UIPopupRewardItem_V11::OnUnloadAllScenes()
{
	for(size_t i =0; i<MAX_REWARD; ++i)
		I3_SAFE_RELEASE(m_ImgBox[i]);
}

/*virtual*/ bool UIPopupRewardItem_V11::OnEntranceStart( void * pArg1, void * pArg2)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	for(int i = 0; i < MAX_REWARD; ++i)
		m_ImgBox[i]->SetEnable(false);

	const MCardSet* pSet = MCardUtil::GetSelectedMCardSet();

	INT32 cardSetType = (pSet ) ? pSet->GetMCardSetInfo()->setID : QUEST_CARDSET_TYPE_NONE;
	
	const MCardInfo* pCardInfo = static_cast<const MCardInfo*>(pArg1);
	bool             bIsFinalReward = *static_cast<bool*>(pArg2);

	_InitUI();
	_SetBasicInfo(pCardInfo, 0, 0);
	_SetPrizeInfo(pCardInfo, cardSetType, bIsFinalReward);

	return true;
}

void UIPopupRewardItem_V11::_InitUI(void)
{
	LuaState * L = _CallLuaFunction("InitUI");
	_EndLuaFunction(L,0);
}


void UIPopupRewardItem_V11::_SetBasicInfo(const MCardInfo* pCardInfo, INT32 bonusExp, INT32 bonusPoint)
{
	I3ASSERT(pCardInfo);

	UINT8		prize		= UINT8(pCardInfo->mCardPrize.prize);
	
	BADGE_TYPE	BadgeType	= PrizeToBadgeType(prize);

	// 보상 내용에 대해서 출력합니다.
	i3::stack_wstring wstrExpText;
	i3::stack_wstring wstrPointText;

	INT32 totalExp		= pCardInfo->mCardPrize.exp;
	INT32 totalPoint	= pCardInfo->mCardPrize.point;


	i3::sprintf(wstrExpText, GAME_RCSTRING("STR_TBL_GUI_READY_REWARD_EXP"), totalExp + bonusExp );/*보상 경험치: %d*/
	i3::sprintf(wstrPointText, GAME_RCSTRING("STR_TBL_GUI_READY_REWARD_POINT"), totalPoint + bonusPoint );/*보상 Point: %d*/

	LuaState * L = _CallLuaFunction("SetBasicInfo");
	i3Lua::PushInteger(L, (INT32)BadgeType);
	i3Lua::PushStringUTF16To8(L, wstrExpText);
	i3Lua::PushStringUTF16To8(L, wstrPointText);
	_EndLuaFunction(L, 3);
}


void UIPopupRewardItem_V11::_SetPrizeInfo(const MCardInfo* pCardInfo, INT32 cardSetType, bool bIsFinalReward )
{
	if( cardSetType >= QUEST_CARDSET_TYPE_COUNT || 
		cardSetType == QUEST_CARDSET_TYPE_NONE)
		return;

	bool previewMode = false;

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	//임무카드 최종 보상 미리보기
	if( g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LCTRL ) &&
		g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LALT) )
	{
		previewMode = true;
	}
#endif
	//최종 보상창이 아니라면 나간다
	if (previewMode == false && bIsFinalReward == false)
		return;


	INT32 iGetMedal = _GetRewardMedalByCardSet( cardSetType );
	
	if(iGetMedal != -1)
		_SetFinalMedal(true);
	else
		_SetFinalMedal(false);

	
	const MCardSetInfo* pSetInfo = MCardUtil::FindMCardSetInfoBySetType(cardSetType);
	

	INT32 bonusPoint = (pSetInfo) ? pSetInfo->prizeCompletion.point : 0;			// 보너스 포인트
	INT32 bonusExp = (pSetInfo) ? pSetInfo->prizeCompletion.exp : 0;				// 보너스 경험치

	_SetBasicInfo( pCardInfo, bonusExp, bonusPoint);

	//임무카드 보상 아이템 관련 일단 하드코딩 처리
	INT32 nItemIDCount = 0;
	
	if (pSetInfo)
	{
		const i3::svector<MCardPrizeInfo_Mqf_ItemInfo>& vecPrizeItem = pSetInfo->prizeCompletion.vecPrizeItem;
		nItemIDCount = INT32(vecPrizeItem.size() );
	}
		
	for(int i = 0; i < nItemIDCount; ++i)
	{
		m_ImgBox[i]->SetEnable(true);

		m_nItemIDForCardSetComplete[i] = MCardUtil::ConvertItemID_V10_TO_V20(pSetInfo->prizeCompletion.vecPrizeItem[i].m_ui32ItemID);
			
		GetItemImageMgr()->SelectImage(m_ImgBox[i], m_nItemIDForCardSetComplete[i]);
	}

	// 퀘스트를 삭제 및 다음 퀘스트 카드가 있다면 강제 셀렉트.
	MCardSet* pNextSet = MCardUtil::FindNextAvailableCardSet();

	INT32 nextSetType = QUEST_CARDSET_TYPE_NONE;
	if (pNextSet)
		nextSetType = pNextSet->GetMCardSetInfo()->setID;

	MCardUtil::RemoveSelectedCardSetAfterFinalPrize();		// 기존코드도 서버에 보내는건 없는것 같다..
	MCardUtil::SelectActiveCardSetToServerWithSetType(nextSetType);

}

void UIPopupRewardItem_V11::_SetFinalMedal(bool Sig)
{
	LuaState * L = _CallLuaFunction("SetFinalMedal");
	i3Lua::PushBoolean(L, Sig);
	_EndLuaFunction(L, 1);
}

INT32 UIPopupRewardItem_V11::_GetRewardMedalByCardSet( INT32 cardSetType)
{
	//	임무카드 보상 메달 (없으면 -1 반환)
	switch( cardSetType)
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


void UIPopupRewardItem_V11::Confirm(void)
{
	GetMainFrame()->TogglePopup(UPW_REWARDITEM);
}

