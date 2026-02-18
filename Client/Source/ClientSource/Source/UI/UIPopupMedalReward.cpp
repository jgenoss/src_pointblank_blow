#include "pch.h"
#include "UIPopupMedalReward.h"
#include "UIMainFrame.h"
#include "MedalManager.h"
#include "UIShopFunction.h"
#include "UIUtil.h"

#include "i3Base/string/ext/sprintf.h"

I3_CLASS_INSTANCE( UIPopupMedalReward);//, UIPopupUseItemBase);

UIPopupMedalReward::UIPopupMedalReward()
{
}

UIPopupMedalReward::~UIPopupMedalReward()
{
}

/*virtual*/ void UIPopupMedalReward::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene( "Scene/Popup/PointBlankRe_PopUp_MedalReward.i3UIs");
}

/*virtual*/ bool UIPopupMedalReward::OnEntranceStart(void * pArg1, void * pArg2)
{
	if(UIPopupBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	const MedalRewardResult * pResult = CMedalManager::i()->getMedalRewardResult(0);
	const MQF_REWARD_INFO * pReward = CMedalManager::i()->getMedalRewardInfo(0);

	// 현재 Medal Info를 받아오기 위함
	const CMedalManager::CurrentRewardInfo* info_ = CMedalManager::i()->getCurrentRewardInfo();

	i3::rc_wstring reward1;
	i3::wstring reward2;
	i3::wstring reward3;
	
	// 기획의도에 따라 우선순위를 소유권, 마스터훈장, MQF파일 보상 순으로 출력합니다.
	if ( (info_->medal_info_ != nullptr && info_->medal_info_->getRestrictionItem() != 0) &&
		pResult->_itemid == 0 &&
		info_->level_ == MEDAL_LEVEL_MAX )			// 소유권
	{
		i3::rc_wstring RestrictString;
		usf::GetShopItemName( RestrictString, info_->medal_info_->getRestrictionItem());

		i3::stack_wstring str = RestrictString;
		str += L' ';
		str += GAME_RCSTRING("STR_MEDAL_QWEAPON_UNLOCK");
		reward1 = str; 
	}
	else if (pReward->m_ui32RewardMaster > 0)	// 마스터훈장
	{
		i3::sprintf(reward1, L"%s", GAME_RCSTRING("STR_MEDAL_MASTERMEDAL"));
	}
	else										// MQF 파일 보상
	{
		usf::GetShopItemName( reward1, pResult->_itemid );
	}

	CMedalManager::i()->ClearCurrentRewardInfo();

	i3::sprintf(reward2, L"Point +%d", pReward->m_ui32RewardPoint);
	i3::sprintf(reward3, L"Exp +%d"	, pReward->m_ui32RewardExp);

	LuaState * L = _CallLuaFunction("setRewardInfo");
	i3Lua::PushStringUTF16To8(L, reward1.c_str());
	i3Lua::PushStringUTF16To8(L, reward2.c_str());
	i3Lua::PushStringUTF16To8(L, reward3.c_str());
	_EndLuaFunction(L, 3);
	return true;
}

/*virtual*/ bool UIPopupMedalReward::OnOKButton( void)
{
	
	return true;
}
