#if !defined( __UI_POPUP_USEITEMBASE_H__)
#define __UI_POPUP_USEITEMBASE_H__

#include "UIPopupItemBase.h"

class UIPopupUseItemBase : public UIPopupItemBase
{
	I3_ABSTRACT_CLASS_DEFINE( UIPopupUseItemBase, UIPopupItemBase);

public:
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override;
};

#endif

