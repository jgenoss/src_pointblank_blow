#include "pch.h"
#include "WeaponDualUziWithSilencer.h"
#include "GlobalVariables.h"
#include "GameCharaWeaponContext.h"
#include "MainWeapon_DualUziWithSilencer.h"


I3_CLASS_INSTANCE( WeaponDualUziWithSilencer);//, WeaponDualUzi);

MainWeapon*	WeaponDualUziWithSilencer::CreateMainWeapon()
{
	MainWeapon_DualUziWithSilencer * p = new MainWeapon_DualUziWithSilencer(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_DualUziWithSilencer));
	return p;
}

WeaponDualUziWithSilencer::WeaponDualUziWithSilencer()
{
}


WeaponDualUziWithSilencer::~WeaponDualUziWithSilencer()
{
}

