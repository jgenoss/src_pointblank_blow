#if !defined( __UI_POPUP_USERACTIONINTERACTADDFRIEND_H__)
#define __UI_POPUP_USERACTIONINTERACTADDFRIEND_H__

#include "UIPopupUserActionInteractBase.h"

class UIPopupUserActionInteractAddFriend : public UIPopupUserActionInteractBase
{
	I3_CLASS_DEFINE( UIPopupUserActionInteractAddFriend);

public:
	UIPopupUserActionInteractAddFriend();
	virtual ~UIPopupUserActionInteractAddFriend();

	virtual void	OnCreate( i3GameNode * pParent);

	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);

	virtual void	OnOKButton( void);
};

#endif
