#if !defined( __UI_POPUP_USEITEM_CHANGECOLOR_CROSSHAIR_H__)
#define __UI_POPUP_USEITEM_CHANGECOLOR_CROSSHAIR_H__

#include "UI/UIPopupColorItemBase.h"

class UIPopupUseItemChangeCrosshairColor_V11 : public UIPopupColorItemBase
{
	I3_CLASS_DEFINE( UIPopupUseItemChangeCrosshairColor_V11, UIPopupColorItemBase);

protected:
	virtual LuaState *	_SetItemInfo( void);

public:
	virtual void	OnCreate( i3GameNode * pParent);

	virtual bool	OnClick( i3UIControl * pControl);
};

#endif
