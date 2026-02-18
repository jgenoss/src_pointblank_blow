#if !defined( __UI_POPUP_COLOR_ITEMBASE_H__)
#define __UI_POPUP_COLOR_ITEMBASE_H__

#include "UIPopupUseItemBase.h"

#define UI_COLOR_SLOT_MAX	10

class UIPopupColorItemBase : public UIPopupUseItemBase
{
	I3_ABSTRACT_CLASS_DEFINE( UIPopupColorItemBase, UIPopupUseItemBase);

public:
	UIPopupColorItemBase();
	virtual ~UIPopupColorItemBase();

	virtual void	OnEntranceEnd( void) override;

	virtual bool	OnOKButton( void) override;

	I3COLOR *		GetColor( INT32 idx);
};

#endif