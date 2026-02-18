#if !defined( __UI_POPUP_USERACTIONPLAYWITH_H__)
#define __UI_POPUP_USERACTIONPLAYWITH_H__

#include "UIPopupUserActionInteractBase.h"

class UIPopupUserActionInteractTracking : public UIPopupUserActionInteractBase
{
	I3_CLASS_DEFINE( UIPopupUserActionInteractTracking);

public:
	UIPopupUserActionInteractTracking();
	virtual ~UIPopupUserActionInteractTracking();

	virtual void	OnCreate( i3GameNode * pParent);
};

#endif
