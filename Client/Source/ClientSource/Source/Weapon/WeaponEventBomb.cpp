#include "pch.h"
#include "WeaponEventBomb.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaBoneContext.h"

#include "MainWeapon_EventBomb.h"

I3_CLASS_INSTANCE(WeaponEventBomb);

WeaponEventBomb::WeaponEventBomb(void)
{

}

WeaponEventBomb::~WeaponEventBomb(void)
{

}

MainWeapon*	WeaponEventBomb::CreateMainWeapon()
{
	MainWeapon_EventBomb * p = new MainWeapon_EventBomb(this);
	MEMDUMP_NEW(p, sizeof(MainWeapon_EventBomb));
	return p;
}