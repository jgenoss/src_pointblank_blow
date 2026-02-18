#if !defined( __UI_TAB_PROFILE_RECORD_H__)
#define __UI_TAB_PROFILE_RECORD_H__

#include "UITabProfileBase.h"

class UITabProfileRecord : public UITabProfileBase
{
	I3_CLASS_DEFINE( UITabProfileRecord, UITabProfileBase);

public:
	UITabProfileRecord();
	virtual ~UITabProfileRecord();

	virtual void	OnCreate( i3GameNode * pParent) override;
};

#endif

