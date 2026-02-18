#if !defined( __UI_POPUP_TWITCHLOGIN_H__)
#define __UI_POPUP_TWITCHLOGIN_H__

#include "UIPopupBase.h"

class UIPopupTwitchLogin : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupTwitchLogin, UIPopupBase);
public:
	void clicked(INT32 idx);

public:

	UIPopupTwitchLogin();
	virtual ~UIPopupTwitchLogin();

	//가상함수
	//========================================================
	
	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	virtual void	OnEntranceEnd(void) override;
	virtual void	OnLoadAllScenes() override;

	virtual void	OnUnloadAllScenes() override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code) override;

	virtual bool	OnKeyInput( i3InputDeviceManager * pInputMng) override;
	virtual bool	OnKey_Enter( void) override;

	//========================================================

private:

	void	ClickBanner();
	void	ClearEditBox();
	void	SetWaringTitle(bool result);

	bool	CheckInput();
	bool	TwitchTVLoginProcess();

private:

	i3UIEditBox *	m_pIDBox;
	i3UIEditBox *	m_pPwBox;
};

#endif
