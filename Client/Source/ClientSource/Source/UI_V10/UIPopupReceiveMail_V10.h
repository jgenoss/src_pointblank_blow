#if !defined( __UI_POPUP_RECEIVEMAIL_V10_H__)
#define __UI_POPUP_RECEIVEMAIL_V10_H__

#include "UI/UIPopupReceiveMail.h"


class UIPopupReceiveMail_V10 : public UIPopupReceiveMail
{
	I3_CLASS_DEFINE( UIPopupReceiveMail_V10, UIPopupReceiveMail );

public:
	
	UIPopupReceiveMail_V10();
	virtual ~UIPopupReceiveMail_V10();

	virtual void		OnCreate( i3GameNode* pParent) override;
};

#endif