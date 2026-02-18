#if !defined( __WEAPON_KNIFE_GENDER_H)
#define __WEAPON_KNIFE_GENDER_H

#include "WeaponBase.h"
#include "WeaponKnife.h"


class WeaponKnifeGender : public WeaponKnife
{
	I3_CLASS_DEFINE( WeaponKnifeGender, WeaponKnife);

public:
	WeaponKnifeGender( void);
	virtual ~WeaponKnifeGender( void);
	virtual MainWeapon*	CreateMainWeapon() override;
};

#endif //__WEAPON_KNIFE_H

