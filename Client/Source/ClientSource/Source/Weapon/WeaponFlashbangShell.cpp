#include "pch.h"
#include "WeaponFlashbangShell.h"
#include "MainWeapon_FlashbangShell.h"
#include "WeaponFlash.h"
#include "WeaponItemID.h"

I3_CLASS_INSTANCE(WeaponFlashbangShell);

WeaponFlashbangShell::WeaponFlashbangShell() {}
WeaponFlashbangShell::~WeaponFlashbangShell() {}

MainWeapon*	WeaponFlashbangShell::CreateMainWeapon()
{
	MainWeapon_FlashbangShell * p = new MainWeapon_FlashbangShell(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_FlashbangShell));
	return p;
}

void WeaponFlashbangShell::_OnCheckCollisionByExplosion()
{
	static_cast<MainWeapon_FlashbangShell*>(GetMainWeapon())->_OnCheckCollisionByExplosion();
}

bool WeaponFlashbangShell::_OnCheckCharaCollisionByExplosion(CGameCharaBase * pChara, UINT32 nColGroup, REAL32 rRange)
{
	return static_cast<MainWeapon_FlashbangShell*>(GetMainWeapon())->_OnCheckCharaCollisionByExplosion(pChara, nColGroup, rRange);
}