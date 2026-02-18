#if !defined( __UI_POPUP_GM_CHATBOX_H__)
#define __UI_POPUP_GM_CHATBOX_H__

#include "UIPopupBase.h"

class CGMChatBuffer;

class UIPopupGMChatBox : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupGMChatBox, UIPopupBase);

private:
	i3UIEditBox *	m_pChatLogBox;
	i3UIEditBox *	m_pChatEdit;

	CGMChatBuffer *	m_pChatBuffer;

protected:
	void			_SendChatMessage(void);
	void			_UpdateChatLogBox(void);

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

public:
	UIPopupGMChatBox();
	virtual ~UIPopupGMChatBox();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;
	virtual bool	OnKey_Enter(void) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;

private:
	typedef void	(UIPopupGMChatBox::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func > m_EventFuncMap;

	void		RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void		ReceiveGameEvent_Chat_Message_Receive( INT32 arg, const i3::user_data&);
};

#endif	// __UI_POPUP_GM_CHATBOX_H__
