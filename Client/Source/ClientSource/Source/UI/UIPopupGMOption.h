#if !defined( __UI_POPUP_GM_OPTION_H__)
#define __UI_POPUP_GM_OPTION_H__

#include "UIPopupBase.h"

enum GM_OPTION
{
	GM_OPTION_NONE				= 0,
	GM_OPTION_REESRTICT_CHAT	= 1,
	GM_OPTION_BAN_PLAYER		= 2,
};

enum PENALTY_NAME
{
	NONE_PENALTY					= 0,
	CHATTING_RESTRICTION_5MIN		= 1,
	CHATTING_RESTRICTION_10MIN		= 2,
	CHATTING_RESTRICTION_30MIN		= 3,
	CHATTING_RESTRICTION_60MIN		= 4,
	CHATTING_RESTRICTION_RELEASE	= 5,
	ACCOUNT_BLOCK_ONCE				= 6,
	ACCOUNT_BLOCK_1DAY				= 7,
	ACCOUNT_BLOCK_3DAY				= 8,
	ACCOUNT_BLOCK_7DAY				= 9,
	ACCOUNT_BLOCK_ETERNAL			= 10,
	PENALTY_MAX						= 11
};

class CGMChatBuffer;

class UIPopupGMOption : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupGMOption, UIPopupBase);
private:
	i3UIEditBox *	m_pChatLogBox;
	i3UIEditBox *	m_pChatEdit;

	CGMChatBuffer *	m_pChatBuffer;

	i3::rc_wstring	m_wstrUserNick;
	i3::wstring		m_strChatFilePath;

private:
	void			__CreateChatLogFile(void);
	void			__WriteChatLogFile(const i3::wstring& wstr);

protected:
	void			_SetUserInfo(void);
	void			_SendChatMessage(void);
	void			_UpdateChatLogBox(void);

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

public:
	UIPopupGMOption();
	virtual ~UIPopupGMOption();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd(void) override;
	virtual void	OnExitEnd(void) override;

	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;
	virtual bool	OnKey_Enter(void) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;

public:
	void			CopyData(i3UIEditBox * pEdit);
	void			UserPenaltyApply(INT32 idx);
	void			SetUserNickName(const i3::rc_wstring& wstr_nick) { m_wstrUserNick = wstr_nick; }

private:
	typedef void	(UIPopupGMOption::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func > m_EventFuncMap;

	void		RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void		ReceiveGameEvent_GMPopup_Open( INT32 arg, const i3::user_data&);
	void		ReceiveGameEvent_Chat_Message_Receive( INT32 arg, const i3::user_data&);
};

#endif // __UI_POPUP_GM_OPTION_H__
