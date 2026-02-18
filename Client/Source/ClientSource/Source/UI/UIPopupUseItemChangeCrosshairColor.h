#if !defined( __UI_POPUP_USEITEM_CHANGECOLOR_CROSSHAIR_H__)
#define __UI_POPUP_USEITEM_CHANGECOLOR_CROSSHAIR_H__

#include "UIPopupColorItemBase.h"

class UIPopupUseItemChangeCrosshairColor : public UIPopupColorItemBase
{
	I3_CLASS_DEFINE( UIPopupUseItemChangeCrosshairColor, UIPopupColorItemBase);

protected:
	virtual LuaState *	_SetItemInfo( void);

public:
	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual bool	OnClick( i3UIControl * pControl) override;
};

#endif
