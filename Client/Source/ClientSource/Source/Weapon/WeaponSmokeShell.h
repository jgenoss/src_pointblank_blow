#pragma once

#include "WeaponGrenadeShell.h"


class WeaponSmokeShell : public WeaponGrenadeShell
{
	I3_CLASS_DEFINE( WeaponSmokeShell, WeaponGrenadeShell);

private:
	virtual MainWeapon*	CreateMainWeapon() override;

public:
	WeaponSmokeShell();
	virtual ~WeaponSmokeShell();
};