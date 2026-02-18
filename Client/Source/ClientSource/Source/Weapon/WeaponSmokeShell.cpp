#include "pch.h"
#include "WeaponSmokeShell.h"
#include "MainWeapon_SmokeShell.h"

I3_CLASS_INSTANCE(WeaponSmokeShell);

WeaponSmokeShell::WeaponSmokeShell() {}
WeaponSmokeShell::~WeaponSmokeShell() {}

MainWeapon*	WeaponSmokeShell::CreateMainWeapon()
{
	MainWeapon_SmokeShell * p = new MainWeapon_SmokeShell(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_SmokeShell));
	return p;
}
