#if !defined( __UI_POPUP_SENDCLANMAIL_V11_H__)
#define __UI_POPUP_SENDCLANMAIL_V11_H__

#include "../UI/UIPopupInteractBase.h"
#include "UIPopupSendMail_V11.h"

class UIPopupSendClanMail_V11 : public UIPopupInteractBase
{
	I3_CLASS_DEFINE( UIPopupSendClanMail_V11, UIPopupInteractBase);
public:
	UIPopupSendClanMail_V11();
	virtual ~UIPopupSendClanMail_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnKey_Enter( void) override;
	
public:
	bool			SendMsg( void);	
	void			CloseMsg(void);

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene);

private:
	i3UIEditBox *			m_pNote;

};

#endif