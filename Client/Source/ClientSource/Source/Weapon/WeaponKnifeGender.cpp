#include "pch.h"
#include "WeaponKnifeGender.h"
#include "MainWeapon_KnifeGender.h"

I3_CLASS_INSTANCE( WeaponKnifeGender );//, WeaponBase);

WeaponKnifeGender::WeaponKnifeGender( void)
{ 
}

WeaponKnifeGender::~WeaponKnifeGender( void)
{
}

MainWeapon*	WeaponKnifeGender::CreateMainWeapon() 
{
	MainWeapon_KnifeGender * p = new MainWeapon_KnifeGender(this);	
	MEMDUMP_NEW( p, sizeof( MainWeapon_KnifeGender));
	return p;
}