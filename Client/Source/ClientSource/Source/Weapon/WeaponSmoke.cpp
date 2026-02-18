#include "pch.h"
#include "WeaponSmoke.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "TutorialMode.h"
#include "GameMode.h"
#include "MyRoomInfoContext.h"
#include "BattleObjContext.h"
#include "MainWeapon_Smoke.h"

I3_CLASS_INSTANCE( WeaponSmoke);//, WeaponGrenade);

WeaponSmoke::WeaponSmoke( void)
{
}

WeaponSmoke::~WeaponSmoke(  void)
{

}

void WeaponSmoke::Reset()
{
	WeaponGrenade::Reset();
}

void WeaponSmoke::OnUpdate(REAL32 rDeltaSeconds)
{
	MainWeapon_Grenade* mainWeapon = static_cast<MainWeapon_Grenade*>(m_mainWeapon.get());
	mainWeapon->OnUpdate_Grenade(rDeltaSeconds);

	WeaponBase::OnUpdate( rDeltaSeconds);
	
}

void WeaponSmoke::SetReceiveStates(GAMEINFO_THROW_WEAPON * pGrenadeInfo, GAMEINFO_THROW_WEAPON_NETDATA * pNetInfo)
{
	MainWeapon_Grenade* mainWeapon = static_cast<MainWeapon_Grenade*>(m_mainWeapon.get());

	mainWeapon->SetReceiveStates(pGrenadeInfo, pNetInfo);
}


MainWeapon*	WeaponSmoke::CreateMainWeapon()
{
	MainWeapon_Smoke * p = new MainWeapon_Smoke(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_Smoke));
	return p;
}
