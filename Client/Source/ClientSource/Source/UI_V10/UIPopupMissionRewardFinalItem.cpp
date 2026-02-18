#include "pch.h"
#include "UIPopupMissionRewardFinalItem.h"
#include "UI/UIMainFrame_V10.h"
#include "UI/UIUtil.h"
#include "UI/ItemImageMgr.h"

#include "../StepProfileDef.h"

#include "MissionCard/MCardSet.h"
#include "MissionCard/MCardUtil.h"
#include "MissionCard/MCardSetInfo.h"
#include "MissionCard/MCardInfo.h"

I3_CLASS_INSTANCE(UIPopupMissionRewardFinalItem);

static UIPopupMissionRewardFinalItem * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);
	return (UIPopupMissionRewardFinalItem*) pFrame->GetPopup( UPW_REWARDITEM_FINALITEM );
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//

extern "C" {
	int UIPopupMissionRewardFinalItem_Confirm(LuaState * L)
	{
		GetThis()->Confirm();
		return 0;
	}
}

LuaFuncReg	UIPopupMissionRewardFinalItem_Glue[] =
{
	{ "Confirm",	UIPopupMissionRewardFinalItem_Confirm },
	{ nullptr,			nullptr }
};

UIPopupMissionRewardFinalItem::UIPopupMissionRewardFinalItem() : m_ImageBox(nullptr), m_nItemIDForCardSetComplete(0)
{

}

UIPopupMissionRewardFinalItem::~UIPopupMissionRewardFinalItem()
{

}

void UIPopupMissionRewardFinalItem::OnCreate( i3GameNode * pParent )
{
	UIPopupBase::OnCreate( pParent);

	// AddScene
	AddScene("Scene/Popup/PBRe_PopUp_MissionCard_reward_FinalItem.i3UIs", UIPopupMissionRewardFinalItem_Glue);
}

bool UIPopupMissionRewardFinalItem::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/ )
{
	if( UIPopupBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	const MCardInfo* pCardInfo = static_cast<const MCardInfo*>(pArg1);

	_SetInfo(pCardInfo);
	
	return true;
}

bool UIPopupMissionRewardFinalItem::OnExitStart( void )
{
	if(UIPopupBase::OnExitStart() == false)
		return false;

	return true;
}

void UIPopupMissionRewardFinalItem::Confirm()
{
	_NextCardSet();
	GetMainFrame()->TogglePopup(UPW_REWARDITEM_FINALITEM);
}

void UIPopupMissionRewardFinalItem::_SetInfo(const MCardInfo* pCardInfo )
{
	const MCardSet* pSet = MCardUtil::GetSelectedMCardSet();

	INT32 cardSetType = (pSet ) ? pSet->GetMCardSetInfo()->setID : QUEST_CARDSET_TYPE_NONE;

	const MCardSetInfo* pSetInfo = MCardUtil::FindMCardSetInfoBySetType(cardSetType);

	//임무카드 보상 아이템 관련 일단 하드코딩 처리
	if (pSetInfo)
	{
		if (pSetInfo->prizeCompletion.vecPrizeItem.size() > 0)
			m_nItemIDForCardSetComplete = MCardUtil::ConvertItemID_V10_TO_V20(pSetInfo->prizeCompletion.vecPrizeItem[0].m_ui32ItemID);
		else
			m_nItemIDForCardSetComplete = 0;

		GetItemImageMgr()->SelectImage(m_ImageBox, m_nItemIDForCardSetComplete);
	}

	// 정보 세팅
	{
		INT32 _Point = (pSetInfo) ? pSetInfo->prizeCompletion.point : 0;
		INT32 _Exp = (pSetInfo) ? pSetInfo->prizeCompletion.exp : 0;

		i3::stack_wstring wstrExpText;
		i3::stack_wstring wstrPointText;
		
		if(_Exp > 0)
			i3::sprintf(wstrExpText, GAME_RCSTRING("STR_TBL_GUI_READY_REWARD_EXP"), _Exp);/*보상 경험치: %d*/
		if(_Point > 0)
			i3::sprintf(wstrPointText, GAME_RCSTRING("STR_TBL_GUI_READY_REWARD_POINT"), _Point);/*보상 경험치: %d*/

		i3::stack_wstring wstrRewardText;
		wstrRewardText += wstrPointText;
		if(_Exp > 0)
			wstrRewardText += L" / ";
		wstrRewardText += wstrExpText;

		LuaState * L = _CallLuaFunction("SetInfo");
		if(L != nullptr)
		{
			i3Lua::PushStringUTF16To8(L, wstrRewardText);
			_EndLuaFunction(L, 1);
		}

	}
}

void UIPopupMissionRewardFinalItem::_InitializeAtLoad( i3UIScene * pScene )
{
	UIPopupBase::_InitializeAtLoad(pScene);

	m_ImageBox = GetItemImageMgr()->CreateImgSetCtrl(pScene, "RewardItem");
}

void UIPopupMissionRewardFinalItem::_ClearAtUnload( i3UIScene * pScene )
{
	UIPopupBase::_ClearAtUnload(pScene);

	I3_SAFE_RELEASE(m_ImageBox);
	m_nItemIDForCardSetComplete = 0;
}

void UIPopupMissionRewardFinalItem::_NextCardSet()
{
	// 퀘스트를 삭제 및 다음 퀘스트 카드가 있다면 강제 셀렉트.
	MCardSet* pNextSet = MCardUtil::FindNextAvailableCardSet();

	INT32 nextSetType = QUEST_CARDSET_TYPE_NONE;
	if (pNextSet)
		nextSetType = pNextSet->GetMCardSetInfo()->setID;

	MCardUtil::RemoveSelectedCardSetAfterFinalPrize();		// 기존코드도 서버에 보내는건 없는것 같다..
	MCardUtil::SelectActiveCardSetToServerWithSetType(nextSetType);
}
