#include "pch.h"
#include "UIPopupMissionReward.h"
#include "UI/UIMainFrame_V10.h"

#include "../StepProfileDef.h"

#include "MissionCard/MCardSet.h"
#include "MissionCard/MCardUtil.h"
#include "MissionCard/MCardSetInfo.h"
#include "MissionCard/MCardInfo.h"

I3_CLASS_INSTANCE( UIPopupMissionReward);

static UIPopupMissionReward * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);
	return (UIPopupMissionReward*) pFrame->GetPopup( UPW_REWARDITEM );
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//

extern "C" {
	int UIPopupMissionReward_Confirm(LuaState * L)
	{
		GetThis()->Confirm();
		return 0;
	}
}

LuaFuncReg	UIPopupMissionReward_Glue[] =
{
	{ "Confirm",	UIPopupMissionReward_Confirm },
	{ nullptr,			nullptr }
};

UIPopupMissionReward::UIPopupMissionReward() : isFinal(false), m_pCardInfo(nullptr)
{

}

UIPopupMissionReward::~UIPopupMissionReward()
{
}

void UIPopupMissionReward::OnCreate( i3GameNode * pParent )
{
	UIPopupBase::OnCreate( pParent);

	// Load Scene
	AddScene("Scene/Popup/PBRe_PopUp_MissionCard_reward.i3UIs", UIPopupMissionReward_Glue);
}

bool UIPopupMissionReward::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/ )
{
	if( UIPopupBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	m_pCardInfo = static_cast<MCardInfo*>(pArg1);
	isFinal = *static_cast<bool*>(pArg2);

	// 정보 설정
	_SetInfo();

	return true;
}

void UIPopupMissionReward::Confirm()
{
	GetMainFrame()->TogglePopup(UPW_REWARDITEM);

	if(isFinal)
	{
		const MCardSet* pSet = MCardUtil::GetSelectedMCardSet();

		INT32 cardSetType = (pSet ) ? pSet->GetMCardSetInfo()->setID : QUEST_CARDSET_TYPE_NONE;

		INT32 iGetMedal = _GetRewardMedalByCardSet( cardSetType );

		if(iGetMedal != -1)
			GetMainFrame()->OpenPopup( UPW_REWARDITEM_FINALMASTER, const_cast<MCardInfo*>(m_pCardInfo));
		else
			GetMainFrame()->OpenPopup( UPW_REWARDITEM_FINALITEM, const_cast<MCardInfo*>(m_pCardInfo));

		isFinal = false;
		m_pCardInfo = nullptr;
	}
}

void UIPopupMissionReward::_SetInfo()
{
	if(m_pCardInfo == nullptr) return;
	
	UINT8		prize		= UINT8(m_pCardInfo->mCardPrize.prize);

	BADGE_TYPE	BadgeType	= PrizeToBadgeType(prize);

	// 보상 내용에 대해서 출력합니다.
	i3::stack_wstring watrRewardText;
	i3::stack_wstring wstrExpText;
	i3::stack_wstring wstrPointText;

	switch(BadgeType)
	{
	case BADGE_TYPE_RIBBON:
		watrRewardText = GAME_STRING("STR_TBL_PROFILE_RIBBON_TEXT2");
		break;
	case BADGE_TYPE_ENSIGN:
		watrRewardText = GAME_STRING("STR_TBL_PROFILE_ENSIGN_TEXT2");
		break;
	case BADGE_TYPE_MEDAL:
	case BADGE_TYPE_MASTER:
		watrRewardText = GAME_STRING("STR_TBL_PROFILE_MEDALS_TEXT2");
		break;
	}
	i3::sprintf(wstrExpText, GAME_RCSTRING("STR_TBL_GUI_READY_REWARD_EXP"), m_pCardInfo->mCardPrize.exp);/*보상 경험치: %d*/
	i3::sprintf(wstrPointText, GAME_RCSTRING("STR_TBL_GUI_READY_REWARD_POINT"), m_pCardInfo->mCardPrize.point);/*보상 Point: %d*/

	LuaState * L = _CallLuaFunction("SetInfo");
	if(L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, watrRewardText);
		i3Lua::PushStringUTF16To8(L, wstrExpText);
		i3Lua::PushStringUTF16To8(L, wstrPointText);
		_EndLuaFunction(L, 3);
	}

	// Icon 세팅
	L = _CallLuaFunction("SetIcon");
	if(L != nullptr)
	{
		i3Lua::PushInteger(L, BadgeType);
		_EndLuaFunction(L, 1);
	}
}

bool UIPopupMissionReward::OnExitStart( void )
{
	if(UIPopupBase::OnExitStart() == false)
		return false;

	return true;
}

INT32 UIPopupMissionReward::_GetRewardMedalByCardSet( INT32 cardSetType )
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
