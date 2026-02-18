#if !defined( __UI_POPUP_CLAN_WARNING_H__)
#define __UI_POPUP_CLAN_WARNING_H__

#include "UIPopupClanBase.h"

class UIPopupClanWarning : public UIPopupClanBase
{
	I3_CLASS_DEFINE( UIPopupClanWarning, UIPopupClanBase); 
	i3UIButton* m_pBtnOK;
public:
	bool	m_bPopupOpenYN;
	UIPopupClanWarning(void);
	virtual ~UIPopupClanWarning(void);

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override {}

	void			_CreateConfirm( void);
	virtual bool	OnOKButton( void) override;
	virtual bool	OnCloseButton( void) override;
	virtual void	OnExitEnd( void) override;
};
#endif