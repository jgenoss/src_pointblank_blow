#if !defined( __UI_BACKGROUND_BASE_H__)
#define __UI_BACKGROUND_BASE_H__

#include "UIBase.h"

class UIBackgroundBase : public UIBase
{
	I3_ABSTRACT_CLASS_DEFINE( UIBackgroundBase, UIBase);

public:
	UIBackgroundBase();
	virtual ~UIBackgroundBase();
};

#endif
