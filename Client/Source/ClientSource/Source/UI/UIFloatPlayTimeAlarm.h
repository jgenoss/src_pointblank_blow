#if !defined( __UI_FLOAT_PLAYTIMEALARM_H__)
#define __UI_FLOAT_PLAYTIMEALARM_H__

#include "UIFloatingBase.h"

class UIFloatPlayTimeAlarm : public UIFloatingBase
{
	I3_CLASS_DEFINE( UIFloatPlayTimeAlarm, UIFloatingBase);

private:
	//i3UIImageBoxEx*	m_pImgBox;
	//i3UIControl*	m_pTextControl;
	i3UIStaticText* m_pText;

	REAL32 m_FloatLifeTime;

	//void			__UpdateUserInfo( void);

public:
	UIFloatPlayTimeAlarm();
	virtual ~UIFloatPlayTimeAlarm();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArv2 = nullptr) override;
	virtual bool	OnExitStart( void) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;
	virtual void	_ClearAtUnload(i3UIScene * pScene) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;

private:
	void PlayTimeNotice();
	void ShutDownTimeNotice();
/*
protected:
	void			_SetClanMark(UINT32);

	void			_SetDefaultInfo(const USER_INFO_BASIC & pInfo);
	void			_SetDetailInfo(const USER_INFO_BASIC & pInfo);
	void			_SetUserExp(const USER_INFO_BASIC & pInfo);
	void			_SetNextPay(const USER_INFO_BASIC & pInfo);
	void			_SetSeasonInfo(void);

public:
	void			OnModifyCurrentPosition(const char * pszStr);
	void			ToggleUserInfo(bool bDetail, bool bClickBtn);
	void			CloseDetailInfo(i3UIControl* pCtrl);*/


	//----------------------------------------------------------------------//
	//						Received Game Eent Functions					//
	//----------------------------------------------------------------------//
private:
	typedef void	(UIFloatPlayTimeAlarm::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func > m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_PlayTimeNotice( INT32, const i3::user_data&);
	void			ReceiveGameEvent_ShutDownTimeNotice( INT32, const i3::user_data&);
	
};

#endif
