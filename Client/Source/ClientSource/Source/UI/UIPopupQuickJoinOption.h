#if !defined( __UI_POPUP_QUICKJOINOPTION_H__)
#define __UI_POPUP_QUICKJOINOPTION_H__

#include "UIPopupBase.h"

class UIPopupQuickJoinOption : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupQuickJoinOption, UIPopupBase);

public:
	UIPopupQuickJoinOption();
	virtual ~UIPopupQuickJoinOption();

	virtual void	OnCreate( i3GameNode * pParent);
};

#endif
