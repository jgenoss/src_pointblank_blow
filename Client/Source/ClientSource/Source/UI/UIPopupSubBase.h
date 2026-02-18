#if !defined( __UI_POPUP_SUBBASE_H__)
#define __UI_POPUP_SUBBASE_H__

#include "UIPopupBase.h"

class UIPopupSubBase : public UIPopupBase
{
	I3_ABSTRACT_CLASS_DEFINE( UIPopupSubBase, UIPopupBase);

protected:
	virtual void	_OnClose() = 0;
};

#endif
