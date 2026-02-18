#if !defined( __UI_POPUP_TRACKINGUSER_H__)
#define __UI_POPUP_TRACKINGUSER_H__

#include "UIPopupInteractBase.h"

// 이 클래스 현재 쓰이고 있지 않은듯 하다.. (2014.09.01.수빈)

class UIPopupTrackingUser : public UIPopupInteractBase
{
	I3_CLASS_DEFINE( UIPopupTrackingUser, UIPopupInteractBase);

public:
	UIPopupTrackingUser();
	virtual ~UIPopupTrackingUser();

	virtual void	OnCreate( i3GameNode * pParent) override;
};

#endif
