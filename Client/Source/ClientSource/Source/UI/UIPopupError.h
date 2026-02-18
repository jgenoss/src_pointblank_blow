#if !defined( __UI_POPUP_ERROR_H__)
#define __UI_POPUP_ERROR_H__

#if legacy_gui_scene

#include "UIPopupBase.h"

class UIPopupError : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupError, UIPopupBase);

public:
	UIPopupError();
	virtual ~UIPopupError();

	virtual void	OnCreate( i3GameNode * pParent);
	void			OnSetMessage( const i3::wliteral_range& wMessageRng);
};

#endif

#endif
