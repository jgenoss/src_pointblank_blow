#include "pch.h"
#include "WeaponFryingPan.h"
#include "WeaponMiniAxe.h"
#include "WeaponFunction.h"
#include "MainWeapon_FryingPan.h"

I3_CLASS_INSTANCE(WeaponFryingPan);

MainWeapon*	WeaponFryingPan::CreateMainWeapon()
{
	MainWeapon_FryingPan* p = new MainWeapon_FryingPan(this);
	MEMDUMP_NEW(p, sizeof(MainWeapon_FryingPan));
	return p;
}

WeaponFryingPan::WeaponFryingPan()
{
}

WeaponFryingPan::~WeaponFryingPan()
{

}

void WeaponFryingPan::PlayAnim_UI_IdleB()
{

}

void WeaponFryingPan::_OnPlayWeaponIdleAnim()
{
	static_cast<MainWeapon_FryingPan*>(GetMainWeapon())->OnIdleAnim();
}

void WeaponFryingPan::_OnPlayWeaponFireAnim()
{
	static_cast<MainWeapon_FryingPan*>(GetMainWeapon())->OnFireAnim();
}

void WeaponFryingPan::_OnPlayWeaponSecondFireAnim()
{
	static_cast<MainWeapon_FryingPan*>(GetMainWeapon())->OnSecondaryFireAnim();
}