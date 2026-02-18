#include "pch.h"
#include "WeaponWPSmokeShell.h"
#include "MainWeapon_WPSmokeShell.h"

I3_CLASS_INSTANCE(WeaponWPSmokeShell);

WeaponWPSmokeShell::WeaponWPSmokeShell() {}
WeaponWPSmokeShell::~WeaponWPSmokeShell() {}

MainWeapon*	WeaponWPSmokeShell::CreateMainWeapon()
{
	MainWeapon_WPSmokeShell * p = new MainWeapon_WPSmokeShell(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_WPSmokeShell));
	return p;
}
