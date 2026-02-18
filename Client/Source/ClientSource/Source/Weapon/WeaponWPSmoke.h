#if !defined( __WEAPON_WP_SMOKE_H__)
#define __WEAPON_WP_SMOKE_H__

#include "WeaponGrenade.h"

class WeaponWPSmoke :	public WeaponGrenade
{
	I3_CLASS_DEFINE( WeaponWPSmoke, WeaponGrenade);

protected:
	virtual MainWeapon*	CreateMainWeapon() override;


protected:
	virtual void	_OnCheckCollisionByExplosion( void) override {}

public:
	WeaponWPSmoke( void);
	virtual ~WeaponWPSmoke( void);

	virtual void	Reset() override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
};

#endif
