#if !defined( __WEAPON_CROSSFIRE_H__)
#define __WEAPON_CROSSFIRE_H__

#include "WeaponDualHandGun.h"

class WeaponCrossFire : public WeaponDualHandGun
{
	I3_CLASS_DEFINE( WeaponCrossFire, WeaponDualHandGun);
private:
	virtual MainWeapon* CreateMainWeapon();

public:
	WeaponCrossFire(void);
	virtual ~WeaponCrossFire(void);

	virtual	bool	Fire( WEAPON::FIRE_ORDER type) override;
	virtual	void	NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type) override;

	virtual void	OnPlayChangeAniImpl( REAL32 rTimeScale = 1.f) override;

	virtual	REAL32	GetRange(void) override;
	virtual	REAL32	GetFireDelay(void) override;

};


#endif