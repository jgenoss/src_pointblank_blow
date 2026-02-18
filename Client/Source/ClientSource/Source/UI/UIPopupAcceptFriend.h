#if !defined( __UI_POPUP_ACCEPT_FRIEND_H__)
#define __UI_POPUP_ACCEPT_FRIEND_H__

#include "UIPopupInteractBase.h"

class UIPopupAcceptFriend : public UIPopupInteractBase
{
	I3_CLASS_DEFINE( UIPopupAcceptFriend);

public:
	UIPopupAcceptFriend();
	virtual ~UIPopupAcceptFriend();

	virtual void	OnCreate( i3GameNode * pParent);
	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);
	virtual bool	OnOKButton( void);
	virtual bool	OnKey_Enter() { OnOKButton(); return true; }
};

#endif
