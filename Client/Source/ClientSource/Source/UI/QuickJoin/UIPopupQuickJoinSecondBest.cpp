#include "pch.h"
#include "UIPopupQuickJoinSecondBest.h"
#include "QJStorage.h"
#include "../UIMainFrame.h"
#include "../../QuickJoinContext.h"
#include "../UIUtil.h"

#include "ClientStageInfoUtil.h"

extern "C"
{
	int UIPopupQuickJoinSecondBest_OKButtonClicked(LuaState* L)
	{
		UIPopupQuickJoinSecondBest * pThis = static_cast<UIPopupQuickJoinSecondBest*>(GameUI::GetPopup(UPW_QUICK_JOIN_SECOND_BEST));
		if( pThis != nullptr)
			pThis->AgreeToSecondBestRoom();
		return 0;
	}

	int UIPopupQuickJoinSecondBest_CancelButtonClicked(LuaState* L)
	{
		GameUI::ClosePopup(UPW_QUICK_JOIN_SECOND_BEST);
		return 0;
	}
}

LuaFuncReg QuickJoinSecondBest_Glue[] = 
{
	{"OKButtonClicked", UIPopupQuickJoinSecondBest_OKButtonClicked},
	{"CancelButtonClicked", UIPopupQuickJoinSecondBest_CancelButtonClicked},
	{nullptr, nullptr}
};

I3_CLASS_INSTANCE(UIPopupQuickJoinSecondBest);

UIPopupQuickJoinSecondBest::UIPopupQuickJoinSecondBest() : m_playTime(0), m_tm(0.0f), m_bAgreeJoin(false)
{

}

UIPopupQuickJoinSecondBest::~UIPopupQuickJoinSecondBest()
{

}

void UIPopupQuickJoinSecondBest::OnCreate(i3GameNode* parent)
{
	UIPopupBase::OnCreate(parent);

	// Load Scene
	AddScene("Scene/Popup/PBRe_PopUp_QuickEnter_SecondBest.i3UIs", QuickJoinSecondBest_Glue, true, false);
}

void UIPopupQuickJoinSecondBest::_InitializeAtLoad(i3UIScene* scn)
{
	UIPopupBase::_InitializeAtLoad(scn);
}

bool UIPopupQuickJoinSecondBest::OnEntranceStart(void* arg1, void* arg2)
{
	if (UIPopupBase::OnEntranceStart(arg1, arg2) == false) return false;

	_SetRecommendRoomInfo();
	m_playTime = QuickJoinContext::i()->getQuickJoinSecondInfo()->m_i32PlayTime;
	if (m_playTime < 0) m_playTime = 0;

	m_bAgreeJoin = 0;

	return true;
}

void UIPopupQuickJoinSecondBest::OnExitEnd()
{
	return UIPopupBase::OnExitEnd();
}

void UIPopupQuickJoinSecondBest::OnUpdate(REAL32 tm)
{
	UIPopupBase::OnUpdate(tm);

	if (QuickJoinContext::i()->getRoomState() == ROOM_STATE_BATTLE && isEnable())
		_UpdateRoomPlayTime(tm);

	if (QuickJoinContext::i()->getRoomState() == ROOM_STATE_EMPTY)
	{
		UIMainFrame* f = g_pFramework->GetUIMainframe();
		f->ClosePopup(UPW_QUICK_JOIN_SECOND_BEST);

		GameUI::MsgBox(MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOBBY_NOT_EXIST_ROOM"));
	}
}

void UIPopupQuickJoinSecondBest::AgreeToSecondBestRoom()
{
	if( !m_bAgreeJoin )
	{
		GameEventSender::i()->SetEvent(EVENT_QUICKJOIN_SECONDBEST);
		m_bAgreeJoin = true;
	}
}

void UIPopupQuickJoinSecondBest::_SetRecommendRoomInfo()
{
	const QuickJoinSecondBestInfo * pInfo = QuickJoinContext::i()->getQuickJoinSecondInfo();

	i3::stack_wstring redScore;
	i3::stack_wstring blueScore;
	i3::stack_wstring vs;

	if (QuickJoinContext::i()->getRoomState() == ROOM_STATE_BATTLE)
	{
		{
			i3::sprintf(redScore, L"%03d", pInfo->m_i32Score[TEAM_RED]);
			i3::sprintf(blueScore, L"%03d", pInfo->m_i32Score[TEAM_BLUE]);

			vs = L"VS";
		}
	}
	else
	{
		{
			i3::sprintf(redScore, L"%03d", pInfo->m_i32Score[TEAM_RED]);
			i3::sprintf(blueScore, L"%03d", pInfo->m_i32Score[TEAM_BLUE]);

			vs = L"VS";
		}
	}
	
	LuaState* L = _CallLuaFunction("SetRecommendRoomInfo");

	UINT32		rule = StageID::GetRule(pInfo->m_ui32StageID);
	STAGE_UID	suid = StageID::GetStageUID(pInfo->m_ui32StageID);
	
	
	i3::rc_wstring modeName;	modeName.clear();
	if(rule != 0)	modeName =  CStageInfoUtil::GetRuleData(rule, false)->getRuleName();

	i3::rc_wstring stageName;	stageName.clear();
	if(suid != 0)	stageName =  CStageInfoUtil::GetMapData(suid)->getDisplayName();

	// mode name
	i3Lua::PushStringUTF16To8(L, modeName);

	// stage name
	i3Lua::PushStringUTF16To8(L, stageName);

	// score info
	i3Lua::PushStringUTF16To8(L, redScore);
	i3Lua::PushStringUTF16To8(L, vs);
	i3Lua::PushStringUTF16To8(L, blueScore);

	_EndLuaFunction(L, 5);
}

void UIPopupQuickJoinSecondBest::_UpdateRoomPlayTime(REAL32 tm)
{
	m_tm += tm;

	if (m_tm > 1.0f && m_playTime > 0)
	{
		m_playTime -= 1; 

		m_tm = 0.0f;

		LuaState* L = _CallLuaFunction("SetPlayTime");

		i3::stack_wstring wstrTime;
		i3::sprintf(wstrTime, L"%02d : %02d", (INT32)(m_playTime/60), (INT32)(m_playTime%60));
		i3Lua::PushStringUTF16To8(L, wstrTime);

		_EndLuaFunction(L, 1);
	}
}

void UIPopupQuickJoinSecondBest::ProcessGameEvent(GAME_EVENT event, INT32 Arg, const i3::user_data& UserData)
{
	UIPopupBase::ProcessGameEvent(event, Arg, UserData);
}

bool UIPopupQuickJoinSecondBest::OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code )
{
	switch (event)
	{
	case I3_EVT_INPUT:
		{
			i3InputDeviceManager* mgr = (i3InputDeviceManager*)pObj;
			i3InputKeyboard* keyboard = mgr->GetKeyboard();

			if (keyboard->GetStrokeState(I3I_KEY_ENTER))
				AgreeToSecondBestRoom();
		}
		break;
	}

	return UIPopupBase::OnEvent(event, pObj, param2, code);
}