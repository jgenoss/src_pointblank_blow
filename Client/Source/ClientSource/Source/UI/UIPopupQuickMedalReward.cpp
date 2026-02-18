#include "pch.h"
#include "UIPopupQuickMedalReward.h"
#include "UIMainFrame.h"
#include "MedalManager.h"
#include "UIShopFunction.h"
#include "UIUtil.h"
#include "UserInfoContext.h"

#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/mb_to_utf16.h"


extern "C" {
	int UIPopupQuickMedaiReward_MovePage( LuaState * L)				// 데이터 복사
	{
		INT32 i32Value = i3Lua::GetIntegerArg( L, 1);

		UIPopupQuickMedaiReward * p = static_cast<UIPopupQuickMedaiReward*>( GameUI::GetPopup(UPW_MEDAL_QUICK_REWARD) );
		if( p != nullptr)
			p->MovePage(i32Value);
		return 0;
	}
}

LuaFuncReg UIPopupQuickMedaiReward_Glue[] = {
	{"MovePage",					UIPopupQuickMedaiReward_MovePage},
	{nullptr,							nullptr}
};

I3_CLASS_INSTANCE( UIPopupQuickMedaiReward);//, UIPopupUseItemBase);

#define MAX_QUICK_REWARD_COUNT	3

UIPopupQuickMedaiReward::UIPopupQuickMedaiReward(): m_pInviteMsg(nullptr)
{
	m_i32CurrentPage = 0;
	m_pWaitMedalResult = nullptr;

	m_UIStyle |= UI_STYLE_INCLUDE_OKBUTTON;
}

UIPopupQuickMedaiReward::~UIPopupQuickMedaiReward()
{
}

void UIPopupQuickMedaiReward::__SetQuestName(i3::stack_wstring & out, const char * szQuestName, const INT32 level)
{
	out = GAME_QUEST_RCSTRING(szQuestName);
	out += L" ";
	i3::stack_wstring wstrTemp;
	i3::sprintf(wstrTemp, GAME_RCSTRING("STR_UI_MEDAL_FASTGET_LV"), level);
	out += wstrTemp; 
}

void UIPopupQuickMedaiReward::__SetQuestDesc(i3::stack_wstring & out, const MEDAL_INFO * pInfo, const INT32 level)
{
	INT32 count = pInfo->m_ui16Count[level] - pInfo->m_ui16Count[level - 1];
	i3::sprintf(out, GAME_QUEST_RCSTRING(pInfo->m_szDescription), count);
}

void UIPopupQuickMedaiReward::__SetQuestReward(i3::stack_wstring & out, const MQF_REWARD_INFO * pInfo)
{
	i3::sprintf(out, GAME_RCSTRING("STR_UI_MEDAL_FASTGET_REWARD"), pInfo->m_ui32RewardPoint, pInfo->m_ui32RewardExp);
	out += L'\n';

	i3::rc_wstring wstrItemName;
	usf::GetShopItemName(wstrItemName, pInfo->m_ui32RewardItemID);
	out.append(wstrItemName.begin(), wstrItemName.end()); 
}

void UIPopupQuickMedaiReward::_SetRewordList(void)
{
	for(INT32 i = 0 ; i < MAX_QUICK_REWARD_COUNT ; ++i)
	{	
		INT32 realIdx = i + (m_i32CurrentPage * MAX_QUICK_REWARD_COUNT);

		if( realIdx < CMedalManager::i()->getQuickRewardCount() )
			_SetRewardData(i);
		else
			_SetEmptyData(i);
	}

	_SetPageButton();
}

void UIPopupQuickMedaiReward::_SetRewardData(INT32 idx)
{
	INT32 realIdx = idx + (m_i32CurrentPage * MAX_QUICK_REWARD_COUNT);	

	const MEDAL_INFO * pInfo = CMedalManager::i()->getMedalFromQuickRewardList(realIdx);
	const INT32 level = CMedalManager::i()->getQuickRewardLevel(realIdx);

	i3::stack_wstring wstrQuestName ;
	i3::stack_wstring wstrQuestDesc ;
	i3::stack_wstring wstrQuestReward;

	__SetQuestName(wstrQuestName, pInfo->m_szNameKey, level);
	__SetQuestDesc(wstrQuestDesc, pInfo, level);
	__SetQuestReward(wstrQuestReward, &(pInfo->m_eRewardInfo[level-1]));
	
	LuaState * L = _CallLuaFunction("SetRewardData");

	if( L != nullptr )
	{
		i3Lua::PushInteger(L, idx);
		i3Lua::PushInteger(L , pInfo->m_ui16ImageIdx);
		i3Lua::PushStringUTF16To8(L , wstrQuestName );
		i3Lua::PushStringUTF16To8(L , wstrQuestDesc);
		i3Lua::PushStringUTF16To8(L , wstrQuestReward);
		_EndLuaFunction(L , 5);
	}
}

void UIPopupQuickMedaiReward::_SetEmptyData(INT32 idx)
{
	LuaState * L = _CallLuaFunction("SetRewardData");

	if( L != nullptr )
	{
		i3Lua::PushInteger(L, idx);
		i3Lua::PushInteger(L, -1);
		i3Lua::PushStringUTF16To8(L, GAME_RCSTRING("STR_UI_MEDAL_FASTGET_QUESTNAME"));
		i3Lua::PushStringUTF16To8(L, L"-");
		i3Lua::PushStringUTF16To8(L, L"-");
		_EndLuaFunction(L, 5);
	}
}

void UIPopupQuickMedaiReward::_SetPageButton(void)
{
	INT32 maxPage = i3Math::abs(CMedalManager::i()->getQuickRewardCount() - 1) / MAX_QUICK_REWARD_COUNT;

	LuaState * L = _CallLuaFunction("SetPageButton");

	if( L != nullptr )
	{
		i3Lua::PushBoolean(L, m_i32CurrentPage > 0);
		i3Lua::PushBoolean(L, m_i32CurrentPage < maxPage);
		_EndLuaFunction(L , 2);
	}
}

void UIPopupQuickMedaiReward::_SendRewardMedalList(void)
{
	if(CMedalManager::i()->isWaitResult() )
		return;

	CMedalManager::i()->ClearMedalRewardResultList();

	PACKET_H_GET_MEDAL_REWARD_REQ	ReqHeader;
	ReqHeader.m_ui8Count = (UINT8)CMedalManager::i()->getQuickRewardCount();

	MEDAL_REWARD * pReqData = (MEDAL_REWARD*)i3MemAlloc(sizeof(MEDAL_REWARD) * ReqHeader.m_ui8Count);
	i3mem::FillZero( pReqData, sizeof(MEDAL_REWARD) * ReqHeader.m_ui8Count );

	for(size_t idx = 0 ; idx < ReqHeader.m_ui8Count ; ++ idx)
	{
		const MedalCompleteInfo * pInfo = CMedalManager::i()->getQuickRewardMedal(idx);
		CMedalManager::i()->BuildMedalRewardPacket(&pReqData[idx], pInfo);
	}

	GameEventSender::i()->SetEvent(EVENT_MEDAL_GET_REWARD, &ReqHeader, pReqData);

	I3MEM_SAFE_FREE(pReqData);

	CMedalManager::i()->StartRewardResultWait();
	if(m_pWaitMedalResult == nullptr)
	{
		i3UIControl * pCtrl = (i3UIControl *)GetScene(0)->FindChildByName("main");
		m_pWaitMedalResult = GameUI::EnableLoadingIcon(this, pCtrl);
	}
}

/*virtual*/ void UIPopupQuickMedaiReward::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene( "Scene/Popup/PBRe_PopUp_QuickMedalReward.i3UIs", UIPopupQuickMedaiReward_Glue);
}

/*virtual*/ bool UIPopupQuickMedaiReward::OnEntranceStart(void * pArg1, void * pArg2)
{
	if(UIPopupBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	CMedalManager::i()->SortQuickRewardList();	

	m_i32CurrentPage = 0;
	_SetRewordList();

	GetSlide()->ForceEnd();
	return true;
}

/*virtual*/ void UIPopupQuickMedaiReward::OnExitEnd(void)
{
	UIPopupBase::OnExitEnd();

	// Quick Reward List 초기화
	CMedalManager::i()->ClearQuickRewardList();

	if( m_pInviteMsg != nullptr)
	{
		m_pInviteMsg->SetEnable(true);
		m_pInviteMsg->ChangeMessage();
		UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
		pFrame->FocusPopup(m_pInviteMsg, true, __FILE__, __LINE__);	
		m_pInviteMsg = nullptr;
	}
	// 레벨업 체크
	if( 0 < UserInfoContext::i()->GetRankUpRank())
	{
		GetMainFrame()->OpenPopup(UPW_RANK_UP);
		UserInfoContext::i()->ResetRankUp();
	}
}

void	UIPopupQuickMedaiReward::OnUpdate(REAL32 rDeltaSeconds)
{
	UIPopupBase::OnUpdate(rDeltaSeconds);
	CMedalManager::i()->UpdateRewardResultWait(rDeltaSeconds);
}

void	UIPopupQuickMedaiReward::ProcessGameEvent( GAME_EVENT event, INT32 arg , const i3::user_data& UserData)
{
	switch( event)
	{
	case EVENT_MEDAL_GET_REWARD :
	case EVENT_MEDAL_GET_ALL_REWARD:
		{
			if( CMedalManager::i()->getMedalRewardResultCount() > 0)
			{
				const INT32 totalCnt = CMedalManager::i()->getMedalRewardResultCount();
				const INT32 failCnt = CMedalManager::i()->getMedalRewardResultFailCount();
				i3::rc_wstring wstrError;
				
				if( totalCnt > 0 && failCnt > 0 )
				{
					i3::rc_wstring wstrQuestName = CMedalManager::i()->getMedalRewardNameForFirstFail();					
				
					if( failCnt == 1 || totalCnt == 1 )
						i3::sprintf(wstrError, GAME_RCSTRING("STR_UI_MEDAL_FASTGET_REWARDFAILED_ONE"), wstrQuestName);
					else
						i3::sprintf(wstrError, GAME_RCSTRING("STR_UI_MEDAL_FASTGET_REWARDFAILED"), wstrQuestName, failCnt);	
				}
				else
					wstrError = GAME_RCSTRING("STR_SHOP_INFORM_SUCCESS_GIFT");

				GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrError, nullptr, MAKE_CALLBACK(Callback_Close), this);
			}

			if( m_pWaitMedalResult != nullptr)
			{
				GameUI::DisableLoadingIcon( m_pWaitMedalResult);
				m_pWaitMedalResult = nullptr;
			}

			CMedalManager::i()->EndRewardResultWait();
		}
		break;
	}
}

/*virtual*/ bool UIPopupQuickMedaiReward::OnOKButton(void)
{
	_SendRewardMedalList();
	return false;
}

void UIPopupQuickMedaiReward::MovePage(INT32 i32Value)
{
	INT32 maxPage = i3Math::abs(CMedalManager::i()->getQuickRewardCount() - 1)  / MAX_QUICK_REWARD_COUNT;

	m_i32CurrentPage += i32Value;

	if(m_i32CurrentPage < 0)				m_i32CurrentPage = 0;
	else if(m_i32CurrentPage >= maxPage)	m_i32CurrentPage = maxPage;

	_SetRewordList();
}

void UIPopupQuickMedaiReward::Callback_Close(void* pThis,INT32 nParam)
{
	GameUI::ClosePopup(UPW_MEDAL_QUICK_REWARD);
}
