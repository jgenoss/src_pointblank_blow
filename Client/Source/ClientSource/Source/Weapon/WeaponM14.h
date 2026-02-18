#pragma once

#include "WeaponGrenade.h"

class WeaponM14 : public WeaponMedicalKit
{
	I3_CLASS_DEFINE(WeaponM14, WeaponMedicalKit);

public:
	virtual MainWeapon*	CreateMainWeapon() override;

protected:
	virtual void	_InitCollideeGroup(void) override;

public:
	WeaponM14();
	virtual ~WeaponM14();

	virtual bool	_OnCheckCharaCollisionByExplosion(CGameCharaBase * pChara, UINT32 nColGroup, REAL32 rRange) override;
	virtual void	SetHitCollideeGroup(void) override;
};