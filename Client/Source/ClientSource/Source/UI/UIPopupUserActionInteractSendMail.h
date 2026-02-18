#if !defined( __UI_POPUP_USERACTIONINTERACTSENDMAIL_H__)
#define __UI_POPUP_USERACTIONINTERACTSENDMAIL_H__

#include "UIPopupUserActionInteractBase.h"

class UIPopupUserActionInteractSendMail : public UIPopupUserActionInteractBase
{
	I3_CLASS_DEFINE( UIPopupUserActionInteractSendMail);

public:
	UIPopupUserActionInteractSendMail();
	virtual ~UIPopupUserActionInteractSendMail();

	virtual void	OnCreate( i3GameNode * pParent);
};

#endif
