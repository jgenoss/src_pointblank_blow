#include "pch.h"

#include "UIHUDPlayTimeAlarm.h"
#include "UIBattleFrame.h"

#include "ShutDownContext.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

UIHUDPlayTimeAlarm* UIHUDPlayTimeAlarm::instance()
{
	UIBattleFrame * pFrame = UIBattleFrame::i();
	return (UIHUDPlayTimeAlarm*) pFrame->GetHUD( UIHUD_PLAYTIME_ALARM );
}

I3_CLASS_INSTANCE( UIHUDPlayTimeAlarm);//, UIHUDBase);

UIHUDPlayTimeAlarm::UIHUDPlayTimeAlarm()
{
#if defined( I3_DEBUG)
	SetName( "PlayTimeAlarm");
#endif

	RegisterGameEventFunc( EVENT_PLAYTIME_ALARM,			&UIHUDPlayTimeAlarm::ReceiveGameEvent_PlayTimeNotice);
	RegisterGameEventFunc( EVENT_SHUTDOWNTIME_ALARM,		&UIHUDPlayTimeAlarm::ReceiveGameEvent_ShutDownTimeNotice);
	RegisterGameEventFunc( EVENT_CLOSE_PLAYTIME_ALARM,		&UIHUDPlayTimeAlarm::ReceiveGameEvent_ClosePlayTimeAlarm);

	m_AlarmState = 0;
	m_HUDLifeTime = 0;
	m_pMainFrame = nullptr;
	m_pAlarmText = nullptr;
}

UIHUDPlayTimeAlarm::~UIHUDPlayTimeAlarm()
{
	m_AlarmState = 0;
}

/*virtual*/ void UIHUDPlayTimeAlarm::_InitializeAtLoad( i3UIScene * pScene)
{
	UIHUDBase::_InitializeAtLoad( pScene);

	m_pMainFrame = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameWnd0");
	m_pAlarmText = (i3UIStaticText*) pScene->FindChildByName( "i3UIStaticText");

	SetEnable(true);
	m_pMainFrame->EnableCtrl(false);
	m_pAlarmText->EnableCtrl(false);
}

/*virtual*/ void UIHUDPlayTimeAlarm::_ClearAtUnload( i3UIScene * pScene)
{
	UIHUDBase::_ClearAtUnload( pScene);

	m_pMainFrame = nullptr;
	m_pAlarmText = nullptr;
}

//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//
/*virtual*/ void UIHUDPlayTimeAlarm::OnCreate( i3GameNode * pParent)
{
	UIHUDBase::OnCreate( pParent);

	// Load Scene
	i3::string fullPath = "Scene/Main/PointBlankRe_Time_Alarm.i3UIs";
	AddScene( fullPath.c_str(), nullptr, true, false);
}

/*virtual*/ bool UIHUDPlayTimeAlarm::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIHUDBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	return true;
}

void	UIHUDPlayTimeAlarm::OnEntranceEnd()
{
	UIHUDBase::OnEntranceEnd();

	m_pMainFrame->EnableCtrl(false);
	m_pAlarmText->EnableCtrl(false);

}

/*virtual*/ void UIHUDPlayTimeAlarm::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	UIHUDBase::ProcessGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
}

void UIHUDPlayTimeAlarm::OnUpdate(REAL32 rDeltaSeconds)
{
	i3_prof_func();

	UIHUDBase::OnUpdate(rDeltaSeconds);

	// 사용했던 알림 UI는 5초 후 스스로 닫음
	if(m_pMainFrame->isVisible())
	{
		m_HUDLifeTime += rDeltaSeconds;

		if (m_HUDLifeTime > 5.0f)
		{
			AlarmClear();
		}
	}
}

void UIHUDPlayTimeAlarm::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

// 현재 알림 UI를 사용 가능 할 경우만 출력
// 사용불가능한 상태일 경우 대기 시킴
void UIHUDPlayTimeAlarm::ReceiveGameEvent_PlayTimeNotice( INT32, const i3::user_data&)
{
	if (ShutDownContext::i()->GetAlarmState() == EMPTY_ALARM)
		PlayTimeNotice();
	else if(ShutDownContext::i()->GetAlarmState() != WAIT_SHUTDOWN_ALARM)
		ShutDownContext::i()->SetAlarmState(WAIT_PLAYTIME_ALARM);
}

void UIHUDPlayTimeAlarm::ReceiveGameEvent_ShutDownTimeNotice( INT32, const i3::user_data&)
{
	if (ShutDownContext::i()->GetAlarmState() == EMPTY_ALARM)
		ShutDownTimeNotice();
	else
		ShutDownContext::i()->SetAlarmState(WAIT_SHUTDOWN_ALARM);
}

void UIHUDPlayTimeAlarm::ReceiveGameEvent_ClosePlayTimeAlarm( INT32, const i3::user_data&)
{
	AlarmClear();
}

void UIHUDPlayTimeAlarm::AlarmClear()
{
	m_HUDLifeTime = 0.f;
	m_pAlarmText->EnableCtrl(false);
	m_pMainFrame->EnableCtrl(false);

	LuaState * L = _CallLuaFunction("SetAlarmText");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, L"");
		_EndLuaFunction(L, 1);
	}

	// 닫을 때 대기중인 알림이 있으면 이벤트를 다시 호출시킴
	if (ShutDownContext::i()->GetAlarmState() == WAIT_PLAYTIME_ALARM)
	{
		ShutDownContext::i()->SetAlarmState(EMPTY_ALARM);
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_PLAYTIME_ALARM);
	}
	else if (ShutDownContext::i()->GetAlarmState() == WAIT_SHUTDOWN_ALARM)
	{
		ShutDownContext::i()->SetAlarmState(EMPTY_ALARM);
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_SHUTDOWNTIME_ALARM);
	}
	else if (ShutDownContext::i()->GetAlarmState() != PRINT_NOTICE)
		ShutDownContext::i()->SetAlarmState(EMPTY_ALARM);

}

void UIHUDPlayTimeAlarm::PlayTimeNotice()
{
	i3::stack_wstring wstrTemp;

	i3::sprintf( wstrTemp, GAME_RCSTRING("STR_SHUTDOWN_INGAME_HUD_GAMECLOSE_TEXT2"), ShutDownContext::i()->GetPlayTime());

	LuaState * L = _CallLuaFunction("SetAlarmText");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, wstrTemp);
		_EndLuaFunction(L, 1);
	}

	m_AlarmState = PRINT_PLAYTIME_ALARM;
	m_pAlarmText->EnableCtrl(true);
	m_pMainFrame->EnableCtrl(true);
}

void UIHUDPlayTimeAlarm::ShutDownTimeNotice()
{
	i3::stack_wstring wstrTemp;
	i3::sprintf( wstrTemp, GAME_RCSTRING("STR_SHUTDOWN_INGAME_HUD_GAMECLOSE_TEXT"), ShutDownContext::i()->GetFnishTime());

	LuaState * L = _CallLuaFunction("SetAlarmText");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, wstrTemp);
		_EndLuaFunction(L, 1);
	}

	m_AlarmState = PRINT_SHUTDOWNTIME_ALARM;
	m_pAlarmText->EnableCtrl(true);
	m_pMainFrame->EnableCtrl(true);
}

void UIHUDPlayTimeAlarm::AIModeNotice()
{
	m_AlarmState = PRINT_NOTICE;
}
