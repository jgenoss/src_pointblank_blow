#pragma once

#include "WeaponGrenadeShell.h"

class WeaponFlash;

class WeaponFlashbangShell : public WeaponGrenadeShell
{
	I3_CLASS_DEFINE( WeaponFlashbangShell, WeaponGrenadeShell);

private:
	virtual MainWeapon*	CreateMainWeapon() override;

public:
	WeaponFlashbangShell();
	virtual ~WeaponFlashbangShell();

	virtual void _OnCheckCollisionByExplosion() override;
	virtual bool _OnCheckCharaCollisionByExplosion(CGameCharaBase * pChara, UINT32 nColGroup, REAL32 rRange) override;
};