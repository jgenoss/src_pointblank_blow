#if !defined( __WEAPON_CS_SMOKE_H__)
#define __WEAPON_CS_SMOKE_H__

#include "WeaponGrenade.h"

class WeaponCSSmoke :	public WeaponGrenade
{
	I3_CLASS_DEFINE(WeaponCSSmoke, WeaponGrenade);

protected:
	virtual MainWeapon*	CreateMainWeapon();


protected:
	virtual void	_OnCheckCollisionByExplosion( void) override {}

public:
	WeaponCSSmoke( void);
	virtual ~WeaponCSSmoke( void);

	virtual void	Reset() override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
};

#endif
