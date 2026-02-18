#if !defined( __UI_POPUP_ADDFRIEND_H__)
#define __UI_POPUP_ADDFRIEND_H__

#include "UIPopupInteractBase.h"

class UIPopupAddFriend : public UIPopupInteractBase
{
	I3_CLASS_DEFINE( UIPopupAddFriend);

public:
	UIPopupAddFriend();
	virtual ~UIPopupAddFriend();

	virtual void	OnCreate( i3GameNode * pParent);

	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);

	virtual bool	OnOKButton( void);
};

#endif
