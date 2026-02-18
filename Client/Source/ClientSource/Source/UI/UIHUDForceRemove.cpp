#include "pch.h"
#include "UIHUDForceRemove.h"

#include "UIBattleFrame.h"
#include "UIBattlePopupAccuseCrime.h"
#include "UIHUDIngameChat.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UIUtil.h"
#include "UIBattlePopupExit.h"
#include "i3Base/string/ext/itoa.h"
#include "i3Base/string/ext/format_string.h"

I3_CLASS_INSTANCE( UIHUDForceRemove);//, UIHUDBase);

static UIHUDForceRemove * GetThis()
{
	return (UIHUDForceRemove*) UIBattleFrame::i()->GetHUD(UIHUD_FORCE_REMOVE);
}

extern "C" 
{
	int	Close_ForceExit_Result( LuaState * L)
	{
		return 0;
	}
}

LuaFuncReg UIHUDForciblyRemoveResult_Glue[] = 
{
	{"Close_ForceExit_Result",	Close_ForceExit_Result	},
	{nullptr,						nullptr}
};

/************************/
namespace inner
{
	i3::rc_wstring get_force_remove_reason_str(FORCEREMOVE_CAUSE reason);

	REAL32 elapsed_time = 0.f;

	// 종료시간
	const REAL32 max_close_time = 5.f;
	bool is_close_time_check = false;

	// 투표시작 대기시간
	bool is_start_time_check = false;
	const INT32 max_start_time = 3;

	// 투표 대기
	bool is_vote_time_check = false;

	// 슬라이딩 기능 
	REAL32 max_sliding_time = 1.0f; // 슬라이드가 반만 접힐 때는 0.5초
	bool is_open = false;
	REAL32 slide_time = 0.f;

	// reset 값
	REAL32 fmw_pos_x = 0.f;
}

/************************/
UIHUDForceRemove::UIHUDForceRemove()
{
	m_SecCount.SetReverse(true);
	m_SecCountCB.SetObj(this);
	m_SecCount.SetCallback(&m_SecCountCB);

	RegisterGameEventFunc( EVENT_KICK_VOTE_SUGGEST_RESULT,		&UIHUDForceRemove::ReceiveGameEvent_KickVote_SuggestResult);
	RegisterGameEventFunc( EVENT_KICK_VOTE_START,				&UIHUDForceRemove::ReceiveGameEvent_KickVote_Start);
	RegisterGameEventFunc( EVENT_KICK_VOTE,						&UIHUDForceRemove::ReceiveGameEvent_KickVote_Update);
	RegisterGameEventFunc( EVENT_KICK_VOTE_COUNT,				&UIHUDForceRemove::ReceiveGameEvent_KickVote_Count);
	RegisterGameEventFunc( EVENT_KICK_VOTE_CANCEL,				&UIHUDForceRemove::ReceiveGameEvent_KickVote_Cancel);
	RegisterGameEventFunc( EVENT_KICK_VOTE_RESULT,				&UIHUDForceRemove::ReceiveGameEvent_KickVote_Result);

	m_progressValue = 1.f; 
	m_bReporter = false;
	m_bUISTART_finished = false;
	m_NextUIState = -1;
	m_CurUIState = -1;
}

UIHUDForceRemove::~UIHUDForceRemove()
{
}

/************************/
/*virtual*/ void UIHUDForceRemove::_InitializeAtLoad(i3UIScene * pScene)
{
	UIHUDBase::_InitializeAtLoad(pScene);

	m_ProgressBar[0] = (i3UIProgressBar*)pScene->FindChildByName("start_progressbar");
	m_ProgressBar[1] = (i3UIProgressBar*)pScene->FindChildByName("guide_progressbar");
	m_ProgressBar[2] = (i3UIProgressBar*)pScene->FindChildByName("voted_progressbar");

	m_pMainFmw[0] = (i3UIFrameWnd*)pScene->FindChildByName("FmwStart");
	m_pMainFmw[1] = (i3UIFrameWnd*)pScene->FindChildByName("FmwVoteGuide");
	m_pMainFmw[2] = (i3UIFrameWnd*)pScene->FindChildByName("FmwVoted");
	m_pMainFmw[3] = (i3UIFrameWnd*)pScene->FindChildByName("FmwResult");
}
/*virtual*/ void UIHUDForceRemove::OnCreate( i3GameNode * pParent)
{
	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) == true ) return; // 튜토리얼에서는 로드하지 않도록

	UIHUDBase::OnCreate( pParent);

	m_wstrRemainTimeStr = GAME_RCSTRING("STR_HUD_FORCIBLYREMOVE_REMAIN_TIME_SUBJECT");
	m_wstrRemainSecondStr = GAME_RCSTRING("STR_HUD_FORCIBLYREMOVE_REMAIN_TIME_SECOND");

	m_VoteTime = LocaleValue::GetInt32("ForciblyRemoveTime"); //투표 제한 시간

	enum { DEFAULT_VOTE_TIME = 20, };
	if( m_VoteTime == 0 ) m_VoteTime = DEFAULT_VOTE_TIME; //혹시 설정 값이 없는 경우엔 20으로 초기화 한다.

	//AddScene("Scene/HUD/PBRe_ForceExit_Vote.i3UIs", UIHUDForciblyRemoveResult_Glue, true, false);
	AddScene("Scene/HUD/PBRe_ForceExit_Vote.i3UIs", UIHUDForciblyRemoveResult_Glue, true, false);
}

bool UIHUDForceRemove::OnEntranceStart(void * pArg1, void * pArg2)
{
	if( UIHUDBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	for (int i = 0; i < 3; ++i)
		m_ProgressBar[i]->setProgress(m_progressValue);

	return true;
}

void UIHUDForceRemove::OnLoadAllScenes()
{
	UIHUDBase::OnLoadAllScenes();

	m_numAgree = 1;
	m_numDisagree = 1;

	inner::is_close_time_check = false;
	m_bStartVote = false;
	m_bVoted = false;

	LuaState* L =0;

	L = _CallLuaFunction( "LoadAllScenes");
	_EndLuaFunction( L, 0);
	// 해상도 상관없이 미니맵 바로 우측에 고정되어야 합니다. 미니맵Width:163
	inner::fmw_pos_x = GameUI::AutoPosition::get_max_left() + 164.f;

	ResetVoteWnd();
	ChangeUIState(FORCE_REMOVE_UI_START);

	// ESC 팝업이 활성화된 상태에서 투표가 시작되면, ESC 팝업을 최상단에 그리도록 합니다
	if(UIBattleFrame::i()->IsOpenPopup(UBP_EXIT))
	{
		UIBattlePopupExit* pPopupExit = static_cast<UIBattlePopupExit*>(GameUI::GetBattlePopup(UBP_EXIT));
		if(pPopupExit != nullptr )
		{
			pPopupExit->AttachToTopScreen_ForceRemove();
		}
	}

	m_SecCount.SetBaseSec(m_VoteTime);
	m_SecCount.Start();
}

/*virtual*/ bool UIHUDForceRemove::OnKeyInput( i3InputDeviceManager * pInputMng)
{
	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) == true) return false; //튜토리얼 모드는 사용하지 않음
	if( UIHUDBase::OnKeyInput( pInputMng) )
		return true;

	if( m_bStartVote == true && m_bVoted == true ) return false; //이미 투표 했음.

	if( UIHUDIngameChat::instance()->GetEnableChat() ) return false; // 채팅 입력 중에는 투표가 불가능

	if(inner::is_start_time_check == true) return false;	// 투표시작 대기중에는 투표 불가능

	if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_9) == true ) // 찬성 버튼
	{	
		Vote( true);
		return true;
	}
	else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_0) == true ) // 반대 버튼
	{
		Vote( false);
		return true;
	}

	return false;
}

void UIHUDForceRemove::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
{
	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) == true) return; //튜토리얼 모드는 사용 안 는다.

	UIHUDBase::ProcessGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
}

/// 조금 더 정확하게 하기 위해 rDeltaSeconds는 쓰지 말고..
/// 쿼리 퍼포먼스카운팅 혹은 timeGetTime쪽으로 돌릴것...
void	UIHUDForceRemove::OnUpdate( REAL32 rDeltaSeconds)
{
	UIHUDBase::OnUpdate(rDeltaSeconds);
	m_SecCount.Update();

	SetProgressBar(rDeltaSeconds);
	SetPopupSlide(rDeltaSeconds);

	if( inner::is_close_time_check == true )
	{
		inner::elapsed_time += rDeltaSeconds;
		if(inner::max_close_time <= inner::elapsed_time)
		{
			UIBattleFrame::i()->EnableHUD(UIHUD_FORCE_REMOVE, false);
			inner::is_close_time_check = false;
			inner::elapsed_time = 0.f;
			m_progressValue = 1.f; 
		}
	}
}

// 팝업 상태가 변경될 때 슬라이딩 기능이 동작합니다.
void UIHUDForceRemove::SetPopupSlide(REAL32 rDeltaSeconds)
{
	// Vote Window Open
	if(inner::is_open)
	{
		if(inner::slide_time < inner::max_sliding_time)
		{
			inner::slide_time += rDeltaSeconds;
			SetVoteWnd_Open(m_CurUIState, rDeltaSeconds); 
		}

		if(m_pMainFmw[m_CurUIState]->getPos()->y > 0)
		{
			inner::is_open = false;
			inner::slide_time = 0.f; 
		}
	}

	// Vote Window Close
	// 투표 시작되고 3s 이후에 닫힘
	if( m_bUISTART_finished )
	{
		if(inner::slide_time < inner::max_sliding_time)
		{
			inner::slide_time += rDeltaSeconds;
			SetVoteWnd_Close(m_CurUIState, rDeltaSeconds, false); 
		}
		else
		{
			m_bUISTART_finished = false;
			inner::slide_time = 0.f;

			if(m_bReporter)
				ChangeUIState(FORCE_REMOVE_UI_VOTE);
			else
				ChangeUIState(FORCE_REMOVE_UI_GUIDE);
		}
	}

	if(inner::is_close_time_check == true)
	{
		// elapsed_time은 OnUpdate 단에서 처리한다.
		if(inner::max_close_time - inner::elapsed_time <= inner::max_sliding_time)
		{
			SetVoteWnd_Close(m_CurUIState, rDeltaSeconds, false); 
		}
	}

	// 비신고자: 투표안내 창이 떠있는 상태에서 유저가 투표를 했을 경우
	if( m_NextUIState == (INT32)FORCE_REMOVE_UI_VOTE )
	{
		inner::slide_time += rDeltaSeconds;
		INT32 ui_state = m_NextUIState - 1;
		
		SetVoteWnd_Close(ui_state, rDeltaSeconds, !m_bReporter); 

		if(inner::slide_time >= inner::max_sliding_time)
		{
			ChangeUIState(m_NextUIState);
			ResetVoteWnd();
		}
	}

	// Current:VOTED/GUIDE 다음 창으로 결과창이 나와야 함
	if( m_NextUIState == (INT32)FORCE_REMOVE_UI_RESULT)
	{
		inner::slide_time += rDeltaSeconds;
		
		SetVoteWnd_Close(m_CurUIState, rDeltaSeconds, false);

		if(inner::slide_time >= inner::max_sliding_time)
		{
			ChangeUIState(m_NextUIState);
			ResetVoteWnd();
		}
	}

}

void UIHUDForceRemove::SetProgressBar(REAL32 rDeltaSeconds)
{
	if (inner::is_start_time_check == true &&
		m_CurUIState == FORCE_REMOVE_UI_START)
	{
		inner::elapsed_time += rDeltaSeconds;
		m_progressValue = 1.f - (inner::elapsed_time / inner::max_start_time);

		if(m_progressValue <= 0)				m_progressValue = 0.f;
		m_ProgressBar[m_CurUIState]->setProgress(m_progressValue);

		if (inner::max_start_time <= inner::elapsed_time)
		{
			inner::is_start_time_check = false;
			inner::elapsed_time = 0.f;			
			m_progressValue = 1.f;
		}
	}
	else if( inner::is_vote_time_check == true &&
		(m_CurUIState == FORCE_REMOVE_UI_GUIDE || m_CurUIState == FORCE_REMOVE_UI_VOTE))
	{
		inner::elapsed_time += rDeltaSeconds;
		// 12792 : 강퇴투표 UI가 실제로 출력되는 시간은 UI출력 전후에 슬라이드 팝업을 제외한 시간이므로 -2.f 
		m_progressValue = 1.f - (inner::elapsed_time / (m_VoteTime-inner::max_start_time-2.f));

		if(m_progressValue <= 0) 			m_progressValue = 0.f;
		m_ProgressBar[m_CurUIState]->setProgress(m_progressValue);

		if((m_VoteTime-inner::max_start_time-2.f) <= inner::elapsed_time )
		{
			inner::is_vote_time_check = false;
			inner::elapsed_time = 0.f;			
			m_progressValue = 1.f; 
		}
	}

}

/************************/
void UIHUDForceRemove::OnSecCount_Start(INT32 result_time)
{
}

void	UIHUDForceRemove::OnSecCount_Change(INT32 result_time)
{
	if ( m_VoteTime - result_time >= inner::max_start_time &&
		m_CurUIState == FORCE_REMOVE_UI_START)
	{
		m_bUISTART_finished = true; 
	}
}

void	UIHUDForceRemove::OnSecCount_DoneOrStop(INT32 result_time)
{
	//EndVote();
}

/************************/
void UIHUDForceRemove::Vote( bool bAgree)
{
	// 서버에 event를 보낸다.
	m_bMyVoteResult = bAgree;
	GameEventSender::i()->SetEvent( EVENT_KICK_VOTE, &bAgree);	// 서버와 값이 다름..

	m_bVoted = true;
}

void UIHUDForceRemove::EndVote()
{
	m_bStartVote = false;
	m_bVoted = false;

	inner::elapsed_time = 0.f;
	inner::is_close_time_check = true;
	inner::is_vote_time_check = false;
	inner::is_start_time_check = false;
	m_progressValue = 1.f;
	m_bReporter = false;
}

/************************/
void UIHUDForceRemove::to_Lua_Switch(FORCE_REMOVE_UI_TYPE type)
{
	LuaState* 	L = _CallLuaFunction( "Switch");
	i3Lua::PushInteger(L, type);
	_EndLuaFunction( L, 1);
}

void UIHUDForceRemove::to_Lua_VoteStart()
{
	i3::wstring reason_str = i3::format_string(L"%s", inner::get_force_remove_reason_str(m_Reason));

	i3::wstring nick_str = i3::format_string(L"%s",	m_wstrCandidateNickName);
	i3::wstring guide_str = i3::format_string(GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_START_NOTIFY"), nick_str); 

	LuaState* L = _CallLuaFunction("SetStartContext");
	i3Lua::PushStringUTF16To8( L, reason_str );
	i3Lua::PushStringUTF16To8( L, nick_str );
	i3Lua::PushStringUTF16To8( L, guide_str);
	UILua::EndLuaFunction(L , 3);
}

void UIHUDForceRemove::to_Lua_VoteCount()
{
	LuaState* L = _CallLuaFunction("SetVoteCount");

	i3::stack_wstring wstr_stack;

	i3Lua::PushInteger(L, m_CurUIState);

	i3::itoa( m_numAgree, wstr_stack);
	i3Lua::PushStringUTF16To8( L, wstr_stack );

	i3::itoa( m_numDisagree, wstr_stack);
	i3Lua::PushStringUTF16To8( L, wstr_stack );

	UILua::EndLuaFunction(L , 3);
}


void	UIHUDForceRemove::to_Lua_VoteResult(BATTLE_FORCIBLYREMOVE_RESULT Res)
{
	i3::stack_wstring wstrMsgFinal;	// 팝업 UI에 결과로 출력할 내용
	i3::stack_wstring wstrMsgChat;	// 채팅창에 출력할 내용

	switch (Res)
	{
	case BATTLE_FORCIBLYREMOVE_SUCCESS:
		{
			i3::sprintf(wstrMsgChat, GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_KICKED_BY_VOTE"), m_wstrCandidateNickName);
			i3::sprintf(wstrMsgFinal, GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_EXIT_NOTIFY")); /*강제 퇴장 되었습니다*/
			break;
		}
	case BATTLE_FORCIBLYREMOVE_SHORTAGES:
	case BATTLE_FORCIBLYREMOVE_NOALLY:
	case BATTLE_FORCIBLYREMOVE_NOENEMY:
		{
			i3::sprintf(wstrMsgChat, GAME_RCSTRING("STR_HUD_FORCIBLYREMOVE_RESULT_REJECT"));
			i3::sprintf(wstrMsgFinal, GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_REJECT_NOTIFY") );/*강제 퇴장이 부결되었습니다*/
			break;
		}
	case BATTLE_FORCIBLYREMOVE_UNKNOWN_ERROR:
		{
			wstrMsgFinal = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_UNKNOWN_ERROR"); /*알 수 없는 오류입니다.*/
		}	
		break;
	}

	// 신고자의 경우에는 채팅창에 결과가 출력되지 않음
	//if(m_bReporter)
		//g_pFramework->PutSystemChatting(wstrMsgChat);

	LuaState* L2 = _CallLuaFunction( "SetResultContext");
	i3Lua::PushInteger(L2, m_numAgree);
	i3Lua::PushInteger(L2, m_numDisagree);
	i3Lua::PushStringUTF16To8( L2, wstrMsgFinal);
	_EndLuaFunction( L2, 3);
}

void UIHUDForceRemove::to_Lua_VoteCancel()
{
	LuaState* L = 0;

	//투표 결과 화면 전환
	to_Lua_Switch((FORCE_REMOVE_UI_TYPE)m_CurUIState);

	/* 대상자가 이탈하여 투표가 중단됩니다.*/
	i3::stack_wstring wstrMsg;
	i3::sprintf(wstrMsg, GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_CANCEL_VOTE"));

	L = _CallLuaFunction( "SetResultContext");
	i3Lua::PushStringUTF16To8( L, wstrMsg);
	_EndLuaFunction( L, 2);
}

void UIHUDForceRemove::to_lua_VoteGuide()
{
	i3::wstring reason_str = i3::format_string(L"%s", inner::get_force_remove_reason_str(m_Reason));
	LuaState* L = _CallLuaFunction("SetGuideContext");
	i3Lua::PushStringUTF16To8( L, m_wstrCandidateNickName);
	i3Lua::PushStringUTF16To8( L, reason_str);
	UILua::EndLuaFunction(L, 2);

	to_Lua_VoteCount();
}

/************************/
i3::rc_wstring inner::get_force_remove_reason_str(FORCEREMOVE_CAUSE reason)
{
	switch(reason)
	{
	case FORCEREMOVE_CURSE: return GAME_RCSTRING("STR_HUD_FORCIBLYREMOVE_REASON_NOMANNER"); break;
	case FORCEREMOVE_ILLEGALPROGRAM: return GAME_RCSTRING("STR_HUD_FORCIBLYREMOVE_REASON_ILLEGALPROGRAM"); break;
	case FORCEREMOVE_ABUSE: return GAME_RCSTRING("STR_HUD_FORCIBLYREMOVE_REASON_ABUSE"); break;

	case FORCEREMOVE_ETC:
	default:
		break;
	}

	return GAME_RCSTRING("STR_HUD_FORCIBLYREMOVE_REASON_ETC");
}

void UIHUDForceRemove::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

//투표 시작(신고자용)
void UIHUDForceRemove::ReceiveGameEvent_KickVote_SuggestResult( INT32 arg, const i3::user_data&)
{
	if (arg != EVENT_ERROR_SUCCESS) return; //UIBattleFrame::ProcessGameEvent_Kick 에서 미리 처리된다.

	UIBattlePopupAccuseCrime * pAccuseCrimePopup = static_cast<UIBattlePopupAccuseCrime*>(GameUI::GetBattlePopup(UBP_ACCUSE_CRIME) );
	if( pAccuseCrimePopup != nullptr)
	{
		m_Reason = pAccuseCrimePopup->GetSelected_Cause();
		m_wstrCandidateNickName = pAccuseCrimePopup->GetSelected_NickName();
	}

	//투표는 시작되었고, 난 찬성으로 투표를 했다.
	m_bStartVote = true; 
	m_bVoted = true;
	m_bMyVoteResult = true;
	m_bReporter = true;

	m_SecCount.SetBaseSec(m_VoteTime);
	m_SecCount.Start();
	
	// CHANGEUI 함수가 먼저 호출되기 때문에 Lua 갱신을 해줄 필요가 있음
	to_Lua_VoteStart();
}

//투표 시작(비 신고자용)
void UIHUDForceRemove::ReceiveGameEvent_KickVote_Start( INT32 arg, const i3::user_data&)
{
	INT32 app_slot, cand_slot;
	DisassembleValue(arg, app_slot, cand_slot, (INT32&)m_Reason);

	//app_slot 변수를 1.1 소스는 사용하지만, 1.5 소스는 사용하지 않는다.
	//const char*  pszAppNick  = BattleSlotContext::i()->getNickForSlot(app_slot);
	m_wstrCandidateNickName = BattleSlotContext::i()->getNickForSlot( cand_slot);

	//투표는 시작되었고, 난 아직 투표를 하지 않았다.
	m_bStartVote = true;
	m_bVoted = false;
	m_bReporter = false;

	m_SecCount.SetBaseSec(m_VoteTime);
	m_SecCount.Start();

	// CHANGEUI 함수가 먼저 호출되기 때문에 Lua 갱신을 해줄 필요가 있음
	to_Lua_VoteStart();
}

//투표한 후에 호출된다.
void UIHUDForceRemove::ReceiveGameEvent_KickVote_Update( INT32 arg, const i3::user_data&)
{	 
	if (arg != EVENT_ERROR_SUCCESS) return; //UIBattleFrame::ProcessGameEvent_Kick 에서 미리 처리된다.

	if (m_bMyVoteResult)
		++m_numAgree;
	else
		++m_numDisagree;

	m_NextUIState = FORCE_REMOVE_UI_VOTE;
}

//현재 투표 진행 상황 표시 - 다른 유저가 투표하면 호출 된다.
void UIHUDForceRemove::ReceiveGameEvent_KickVote_Count( INT32 arg, const i3::user_data&)
{
	DisassembleValue(arg, m_numAgree, m_numDisagree);
	to_Lua_VoteCount();
}


// 강퇴후보자가 나간 경우..
void UIHUDForceRemove::ReceiveGameEvent_KickVote_Cancel( INT32 arg, const i3::user_data&)
{
	m_CurUIState = FORCE_REMOVE_UI_RESULT;
	inner::is_open = true;
	to_Lua_VoteCancel();
	m_SecCount.Stop();
	EndVote();
}

//투표 결과 - 강퇴자를 제외한 전원이 받습니다...
void UIHUDForceRemove::ReceiveGameEvent_KickVote_Result( INT32 arg, const i3::user_data&)
{
	//1.1 소스는 찬/반 스코어를 표시한다. 하지만 1.5 소스는 표시 할 필요가 없다.
	INT32 slot;
	BATTLE_FORCIBLYREMOVE_RESULT res;
	DisassembleValue(arg, slot, m_numAgree, m_numDisagree, (INT32&)res);

	//이 2개는 본질적으로 같은 값이어야하는데, 이유는 달라지면 서버수준에서 패킷이 
	//CANCEL로 들어오기 때문입니다...일치성여부를 고민할 필요가 있음.
	//szNickName과 slot의 닉네임문자열이 동일해야함... 비교검사를 하고 assert를 겁니다..
	//strncmp루틴 전체가 어서트에 들어갑니다..
	I3ASSERT(i3::generic_is_equal(m_wstrCandidateNickName, BattleSlotContext::i()->getNickForSlot(slot)) );

	m_VoteResult = res;
	m_NextUIState = FORCE_REMOVE_UI_RESULT;
	m_SecCount.Stop();
	EndVote();
}

void UIHUDForceRemove::ChangeUIState(INT32 State)
{
	if(m_CurUIState == State) return;
	if(State < 0 ) return;

	m_CurUIState = State;

	to_Lua_Switch((FORCE_REMOVE_UI_TYPE)m_CurUIState);

	switch (m_CurUIState)
	{
	case FORCE_REMOVE_UI_START:
		inner::is_start_time_check = true;
		inner::is_open = true;
		to_Lua_VoteStart();
		break;
	case FORCE_REMOVE_UI_GUIDE:
		inner::is_start_time_check = false;
		inner::is_vote_time_check = true;
		inner::is_open = true;
		to_lua_VoteGuide();
		break;
	case FORCE_REMOVE_UI_VOTE:
		if(m_bReporter) // 신고자는 Guide를 거치지 않고 바로 Vote로 이동함
		{
			inner::is_start_time_check = false;
			inner::is_vote_time_check = true;
			inner::is_open = true;
		}
		to_Lua_VoteCount();
		break;
	case FORCE_REMOVE_UI_RESULT:
		inner::is_open = true;
		to_Lua_VoteResult(m_VoteResult);
		break;
	default:
		break;
	}
}

void UIHUDForceRemove::SetVoteWnd_Close(INT32 ui_type, REAL32 rDeltaSeconds, bool half)
{
	if(ui_type < 0 || ui_type >= MAX_SLIDE_WND)	 return;

	float fmw_size_y	= m_pMainFmw[ui_type]->getSize()->y;
	if(half)		fmw_size_y /= 2;
	VEC3D* fmw_pos		= m_pMainFmw[ui_type]->getPos();
	inner::fmw_pos_x	= fmw_pos->x;

	float distance	= fmw_pos->y - (fmw_size_y * rDeltaSeconds) / inner::max_sliding_time;
	m_pMainFmw[ui_type]->setPos(fmw_pos->x, distance);
}

void UIHUDForceRemove::SetVoteWnd_Open(INT32 ui_type, REAL32 rDeltaSeconds)
{
	if(ui_type < 0 || ui_type >= MAX_SLIDE_WND)	 return;

	float fmw_size_y	= m_pMainFmw[ui_type]->getSize()->y;
	VEC3D* fmw_pos		= m_pMainFmw[ui_type]->getPos();

	float distance =  fmw_pos->y + (fmw_size_y * rDeltaSeconds) / inner::max_sliding_time ;
	m_pMainFmw[ui_type]->setPos(fmw_pos->x, distance);
}

void UIHUDForceRemove::ResetVoteWnd()
{
	inner::slide_time = 0.f;
	m_NextUIState = -1;

	for(int i=0; i<MAX_SLIDE_WND; i++)
	{
		float fmw_size_y	= m_pMainFmw[i]->getSize()->y;
		// Slide로 펼쳐져야 하기 때문에 Height만큼 위로 올라가 있어야 함
		m_pMainFmw[i]->setPos(inner::fmw_pos_x, 0-fmw_size_y);
	}

	// 비신고자의 경우 Guide 상태가 접혀져서 보이는 것 처럼 해야하기 때문에
	// Voted 상태가 슬라이드 오픈 될 필요가 없음
	if(!m_bReporter)
		m_pMainFmw[FORCE_REMOVE_UI_VOTE]->setPos(inner::fmw_pos_x, 0);
}