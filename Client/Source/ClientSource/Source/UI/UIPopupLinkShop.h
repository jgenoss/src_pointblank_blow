#if !defined( __UI_POPUP_LINKSHOP_H__)
#define __UI_POPUP_LINKSHOP_H__

#include "UIPopupBase.h"

class UIPopupLinkShop : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupLinkShop, UIPopupBase);

public:
	UIPopupLinkShop();
	virtual ~UIPopupLinkShop();

	virtual void	OnCreate( i3GameNode * pParent);
};

#endif
