#pragma once

#include "../UI/UIPopupBase.h"

class UIPopupUserSearch_V10 : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupUserSearch_V10, UIPopupBase);

	enum
	{
		USERSEARCH = 1,
		FINDUSER,

		UF_MAX,
	};

public:
	UIPopupUserSearch_V10();
	virtual ~UIPopupUserSearch_V10();

public:
	void			ClickOK();
	void			ClickCancel();

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

public:
	virtual void	OnCreate( i3GameNode * pParent) override;
	/*
	param[in] pArg1 INT32*
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd( void) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;
	virtual bool	OnKeyInput(i3InputDeviceManager * pInputMng) override;
	virtual bool	OnKey_Enter( void) override;

private:
	void			_SetStaticText();
	void			_UserSearch();
	void			_AddUser();
	bool			_CheckNickName(i3::wstring& nickname);

private:
	typedef void	(UIPopupUserSearch_V10::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func > m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_UserInfo( INT32, const i3::user_data&);
	void			ReceiveGameEvent_AddFriend( INT32, const i3::user_data&);

private:
	INT32			idxType_;
	i3UIEditBox *	textEditBox_;
};