#include "pch.h"
#include "WeaponWPSmoke.h"
#include "BattleServerContext.h"
#include "BattleObjContext.h"
#include "MainWeapon_WPSmoke.h"

I3_CLASS_INSTANCE( WeaponWPSmoke);//, WeaponGrenade);

WeaponWPSmoke::WeaponWPSmoke()
{

}

WeaponWPSmoke::~WeaponWPSmoke()
{
}

/*virtual*/ void WeaponWPSmoke::Reset()
{
	WeaponGrenade::Reset();

}

/*virtual*/ void WeaponWPSmoke::OnUpdate( REAL32 rDeltaSeconds)
{
	WeaponGrenade::OnUpdate( rDeltaSeconds);
}

MainWeapon*	WeaponWPSmoke::CreateMainWeapon()
{
	MainWeapon_WPSmoke * p = new MainWeapon_WPSmoke(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_WPSmoke));
	return p;
}
