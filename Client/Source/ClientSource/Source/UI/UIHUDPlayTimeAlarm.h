#pragma once

#include "UIHUDBase.h"

class UIHUDPlayTimeAlarm : public UIHUDBase
{
	I3_CLASS_DEFINE(UIHUDPlayTimeAlarm, UIHUDBase);

public:
	static UIHUDPlayTimeAlarm* instance();

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

public:
	UIHUDPlayTimeAlarm();
	virtual ~UIHUDPlayTimeAlarm();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData) override;

	virtual bool OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void OnEntranceEnd() override;

protected:
	i3UIFrameWnd *	m_pMainFrame;
	i3UIStaticText*	m_pAlarmText;

	UINT			m_AlarmState;
	REAL32			m_HUDLifeTime;

protected:
	void AIModeNotice();
	void ShutDownTimeNotice();
	void PlayTimeNotice();
	void AlarmClear();

public:
	/** \brief 업데이트 돕니다. */
	void			OnUpdate(REAL32 rDeltaSeconds);

	//----------------------------------------------------------------------//
	//						Received Game Eent Functions					//
	//----------------------------------------------------------------------//
private:
	typedef void	(UIHUDPlayTimeAlarm::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func > m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_PlayTimeNotice( INT32, const i3::user_data&);
	void			ReceiveGameEvent_ShutDownTimeNotice( INT32, const i3::user_data&);
	void			ReceiveGameEvent_ClosePlayTimeAlarm( INT32, const i3::user_data&);
};
