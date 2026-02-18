#if !defined( __UI_TAB_PROFILE_WEAPONRECORD_H__)
#define __UI_TAB_PROFILE_WEAPONRECORD_H__

#include "UITabProfileBase.h"

class UITabProfileWeaponRecord : public UITabProfileBase
{
	I3_CLASS_DEFINE( UITabProfileWeaponRecord, UITabProfileBase);

public:
	UITabProfileWeaponRecord();
	virtual ~UITabProfileWeaponRecord();

	virtual void	OnCreate( i3GameNode * pParent) override;
};

#endif
