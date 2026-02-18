#if !defined( __UI_POPUP_COUPONLIST_H__)
#define __UI_POPUP_COUPONLIST_H__

#include "UIPopupBase.h"

class UIPopupCouponList : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupCouponList, UIPopupBase);

public:
	UIPopupCouponList();
	virtual ~UIPopupCouponList();

	virtual void	OnCreate( i3GameNode * pParent);
};

#endif
