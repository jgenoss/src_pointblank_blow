#if !defined( __UI_POPUP_USERACTIONPLAYWITH_H__)
#define __UI_POPUP_USERACTIONPLAYWITH_H__

#include "UIPopupUserActionInteractBase.h"

class UIPopupUserActionInteractPlayWith : public UIPopupUserActionInteractBase
{
	I3_CLASS_DEFINE( UIPopupUserActionInteractPlayWith);

public:
	UIPopupUserActionInteractPlayWith();
	virtual ~UIPopupUserActionInteractPlayWith();

	virtual void	OnCreate( i3GameNode * pParent);
};

#endif
