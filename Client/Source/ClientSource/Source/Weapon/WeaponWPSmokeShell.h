#pragma once

#include "WeaponGrenadeShell.h"


class WeaponWPSmokeShell : public WeaponGrenadeShell
{
	I3_CLASS_DEFINE( WeaponWPSmokeShell, WeaponGrenadeShell);

private:
	virtual MainWeapon*	CreateMainWeapon() override;

public:
	WeaponWPSmokeShell();
	virtual ~WeaponWPSmokeShell();
};