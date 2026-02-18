
#if !defined( _WEAPON_EVENTBOMB_H__)
#define _WEAPON_EVENTBOMB_H__

#include "WeaponDecoyBomb.h"

class WeaponDecoySndInterval;

class WeaponEventBomb :	public WeaponDecoyBomb
{
	I3_CLASS_DEFINE( WeaponEventBomb, WeaponDecoyBomb);

protected :

public:
	WeaponEventBomb(void);
	virtual ~WeaponEventBomb(void);
	virtual MainWeapon*	CreateMainWeapon() override;
};

#endif // _WEAPON_EVENTBOMB_H__