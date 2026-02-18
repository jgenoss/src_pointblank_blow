#pragma once

#include "MainWeapon_GrenadeShell.h"

class MainWeapon_Flash;

class MainWeapon_FlashbangShell : public MainWeapon_GrenadeShell
{
public:
	MainWeapon_FlashbangShell(WeaponBase* p);
	virtual ~MainWeapon_FlashbangShell();

	virtual void OnUpdate_GrenadeShell(REAL32 tm) override;
	virtual void Explosion() override;
	virtual void Reset() override;

	void _OnCheckCollisionByExplosion();
	bool _OnCheckCharaCollisionByExplosion( CGameCharaBase * pChara, UINT32 nColGroup, REAL32 rRange);

private:
	bool m_bFlashExplosed;
	MainWeapon_Flash* m_pFlash;
};