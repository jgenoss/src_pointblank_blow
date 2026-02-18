#include "pch.h"
#include "WeaponCSSmoke.h"
#include "BattleServerContext.h"
#include "BattleObjContext.h"
#include "MainWeapon_CSSmoke.h"

I3_CLASS_INSTANCE(WeaponCSSmoke);//, WeaponGrenade);

WeaponCSSmoke::WeaponCSSmoke()
{

}

WeaponCSSmoke::~WeaponCSSmoke()
{
}

/*virtual*/ void WeaponCSSmoke::Reset()
{
	WeaponGrenade::Reset();

}

/*virtual*/ void WeaponCSSmoke::OnUpdate( REAL32 rDeltaSeconds)
{
	WeaponGrenade::OnUpdate( rDeltaSeconds);
}

MainWeapon*	WeaponCSSmoke::CreateMainWeapon()
{
	MainWeapon_CSSmoke * p = new MainWeapon_CSSmoke(this);
	MEMDUMP_NEW( p, sizeof(MainWeapon_CSSmoke));
	return p;
}
