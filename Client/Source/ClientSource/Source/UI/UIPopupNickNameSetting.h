#if !defined( __UI_POPUP_NICKNAMESETTING_H__)
#define __UI_POPUP_NICKNAMESETTING_H__

#include "UIPopupBase.h"

class UIPopupNickNameSetting : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupNickNameSetting, UIPopupBase);

private:
	i3UIEditBox *	m_pEd;
	bool			m_bCreate;
	bool			m_bChangeNameChecked;

	i3::rc_wstring	m_wstrChangeName;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload(i3UIScene * pScene) override;

public:
	UIPopupNickNameSetting();
	virtual ~UIPopupNickNameSetting();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual bool	OnKeyInput( i3InputDeviceManager * pInputMng) override;
	virtual bool	OnKey_Enter( void) override;
	virtual bool	OnKey_Escape() override { return true; }			// 닉네임은 강제되기 때문에 ESC를 금하도록 처리..(11.10.21.수빈)

	virtual void	OnEntranceEnd( void) override;
	virtual bool	OnExitStart( void) override;

	virtual bool	OnOKButton( void) override;

	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override;
	
	virtual bool	OnPlayClickSound(i3UIControl * pCtrl) override;

	bool			CreateNickName( void);
	void			SetEditFocus( bool bFlag);
	void			CheckButton(void);
	void			ChangeTextMsg( INT32 arg);
};

#endif
