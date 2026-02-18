#if !defined( __UI_POPUP_REPLYMAIL_V11_H__)
#define __UI_POPUP_REPLYMAIL_V11_H__

#include "UI/UIPopupBase.h"

class UIPopupReplyMail_V11 : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupReplyMail_V11, UIPopupBase);
	typedef UIPopupBase	base;

private:
	i3::rc_wstring  m_UserName;

public:
	UIPopupReplyMail_V11();
	virtual ~UIPopupReplyMail_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;
	/*
	param[in] pArg1 i3::rc_wstring*
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual bool	OnCloseButton( void) override;
	virtual void	OnLoadAllScenes() override;
	virtual void	ProcessGameEvent(GAME_EVENT evt,INT32 arg, const i3::user_data& UserData) override;

	void			OnSendNote();
	virtual bool	OnKey_Enter() override { OnSendNote(); return true; }

private:
	void			_SetUserState(const i3::rc_wstring pName);

};

#endif

