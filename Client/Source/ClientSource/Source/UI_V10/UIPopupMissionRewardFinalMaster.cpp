#include "pch.h"
#include "UIPopupMissionRewardFinalMaster.h"
#include "UI/UIMainFrame_V10.h"

#include "../StepProfileDef.h"
#include "UI/ItemImageMgr.h"

#include "MissionCard/MCardSet.h"
#include "MissionCard/MCardUtil.h"
#include "MissionCard/MCardSetInfo.h"
#include "MissionCard/MCardInfo.h"

I3_CLASS_INSTANCE(UIPopupMissionRewardFinalMaster);

static UIPopupMissionRewardFinalMaster * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);
	return (UIPopupMissionRewardFinalMaster*) pFrame->GetPopup( UPW_REWARDITEM_FINALMASTER );
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//

extern "C" {
	int UIPopupMissionRewardFinalMaster_Confirm(LuaState * L)
	{
		GetThis()->Confirm();
		return 0;
	}
}

LuaFuncReg	UIPopupMissionRewardFinalMaster_Glue[] =
{
	{ "Confirm",	UIPopupMissionRewardFinalMaster_Confirm },
	{ nullptr,			nullptr }
};

UIPopupMissionRewardFinalMaster::UIPopupMissionRewardFinalMaster()
{

}

UIPopupMissionRewardFinalMaster::~UIPopupMissionRewardFinalMaster()
{

}

void UIPopupMissionRewardFinalMaster::OnCreate( i3GameNode * pParent )
{
	UIPopupBase::OnCreate( pParent);

	// AddScene
	AddScene("Scene/Popup/PBRe_PopUp_MissionCard_reward_FinalMaster.i3UIs", UIPopupMissionRewardFinalMaster_Glue);
}

bool UIPopupMissionRewardFinalMaster::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/ )
{
	if( UIPopupBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	const MCardSet* pSet = MCardUtil::GetSelectedMCardSet();

	INT32 cardSetType = (pSet ) ? pSet->GetMCardSetInfo()->setID : QUEST_CARDSET_TYPE_NONE;

	const MCardInfo* pCardInfo = static_cast<const MCardInfo*>(pArg1);

	_SetInfo(pCardInfo, cardSetType);

	return true;
}

bool UIPopupMissionRewardFinalMaster::OnExitStart( void )
{
	if(UIPopupBase::OnExitStart() == false)
		return false;

	return true;
}

void UIPopupMissionRewardFinalMaster::Confirm()
{
	_NextCardSet();
	GetMainFrame()->TogglePopup(UPW_REWARDITEM_FINALMASTER);
}

void UIPopupMissionRewardFinalMaster::_SetInfo( const MCardInfo* pCardInfo, INT32 cardSetType)
{
	// 아이콘 끄기
	_SetDefaultIcon();

	const MCardSetInfo* pSetInfo = MCardUtil::FindMCardSetInfoBySetType(cardSetType);

	// 아래 임무카드 보상 및 지급 표장
	//EXPORT_AVALIABLE_CODE_VALUE;
	INT32 bonusPoint = (pSetInfo) ? pSetInfo->prizeCompletion.point : 0;
	INT32 bonusExp = (pSetInfo) ? pSetInfo->prizeCompletion.exp : 0;

	i3::stack_wstring wstrPoint, wstrExp, wstrMaster;

	// 최종 추가 보상 포인트/경험치
	i3::sprintf( wstrPoint, GAME_STRING("STR_TBL_GUI_READY_GET_POINT_BONUS"), bonusPoint);
	i3::sprintf( wstrExp, GAME_STRING("STR_TBL_GUI_READY_GET_EXP_BONUS"), bonusExp);
	i3::sprintf( wstrMaster, GAME_STRING("STR_TBL_GUI_READY_NOTICE_GET_SOMETHING1"), GAME_STRING("STR_TBL_PROFILE_MASTER_MEDALS_TEXT"));

	LuaState * L = _CallLuaFunction("SetInfo");
	if(L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, wstrPoint);
		i3Lua::PushStringUTF16To8(L, wstrExp);
		i3Lua::PushStringUTF16To8(L, wstrMaster);
		_EndLuaFunction(L, 3);
	}
}

void UIPopupMissionRewardFinalMaster::_SetDefaultIcon()
{
	LuaState * L = _CallLuaFunction("SetDefault");
	if(L != nullptr)
	{
		_EndLuaFunction(L,0);
	}
}

void UIPopupMissionRewardFinalMaster::_NextCardSet()
{
	// 퀘스트를 삭제 및 다음 퀘스트 카드가 있다면 강제 셀렉트.
	MCardSet* pNextSet = MCardUtil::FindNextAvailableCardSet();

	INT32 nextSetType = QUEST_CARDSET_TYPE_NONE;
	if (pNextSet)
		nextSetType = pNextSet->GetMCardSetInfo()->setID;

	MCardUtil::RemoveSelectedCardSetAfterFinalPrize();		// 기존코드도 서버에 보내는건 없는것 같다..
	MCardUtil::SelectActiveCardSetToServerWithSetType(nextSetType);
}
