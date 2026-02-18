#if !defined( _WEAPON_FLASH_H__)
#define _WEAPON_FLASH_H__

#include "WeaponGrenade.h"

class WeaponFlash :	public WeaponGrenade
{
	I3_CLASS_DEFINE( WeaponFlash, WeaponGrenade);

protected:
	virtual MainWeapon* CreateMainWeapon();

public:
	WeaponFlash( void);
	virtual ~WeaponFlash( void);

protected:
	virtual void	_OnCheckCollisionByExplosion() override;
	virtual bool	_OnCheckCharaCollisionByExplosion( CGameCharaBase * pChara, UINT32 nColGroup, REAL32 rExplosionRange) override;
};

#endif