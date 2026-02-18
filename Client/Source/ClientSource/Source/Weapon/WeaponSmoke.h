#if !defined( _WEAPON_SMOKE_H__)
#define _WEAPON_SMOKE_H__

#include "WeaponGrenade.h"

class WeaponSmoke :	public WeaponGrenade
{
	I3_CLASS_DEFINE( WeaponSmoke, WeaponGrenade);

protected:
	virtual MainWeapon*	CreateMainWeapon() override;

	virtual void	_OnCheckCollisionByExplosion( void) override {}

public:
	WeaponSmoke( void);
	virtual ~WeaponSmoke( void);

	virtual void	Reset() override;
	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;
	virtual void	SetReceiveStates(GAMEINFO_THROW_WEAPON * pGrenadeInfo, GAMEINFO_THROW_WEAPON_NETDATA * pNetInfo) override;
};

#endif