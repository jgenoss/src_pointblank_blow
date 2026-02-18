#include "pch.h"
#include "WeaponDecoyBomb.h"
#include "GlobalVariables.h"
#include "StageBattle.h"

#include "GameCharaBoneContext.h"

#include "MainWeapon_DecoyBomb.h"

I3_CLASS_INSTANCE(WeaponDecoyBomb);

//
// WeaponDecoyBomb
//

WeaponDecoyBomb::WeaponDecoyBomb(void)
{
}

WeaponDecoyBomb::~WeaponDecoyBomb(void)
{
}

void WeaponDecoyBomb::Reset()
{
	WeaponGrenade::Reset();
}

void WeaponDecoyBomb::Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon)
{
	WeaponBase::Create( pOwner, Itemid, bCreateInWeapon);
}

MainWeapon*	WeaponDecoyBomb::CreateMainWeapon()
{
	MainWeapon_DecoyBomb * p = new MainWeapon_DecoyBomb(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_DecoyBomb));
	return p;
}

void WeaponDecoyBomb::OnUpdate(REAL32 rDeltaSeconds)
{
	WeaponGrenade::OnUpdate(rDeltaSeconds);

	MainWeapon_DecoyBomb* mainWeapon = static_cast<MainWeapon_DecoyBomb*>(m_mainWeapon.get());
	mainWeapon->UpdateDecoying(rDeltaSeconds);

}