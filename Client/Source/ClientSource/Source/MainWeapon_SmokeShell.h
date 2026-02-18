#pragma once

#include "MainWeapon_GrenadeShell.h"

class MainWeapon_Smoke;

class MainWeapon_SmokeShell : public MainWeapon_GrenadeShell
{
public:
	MainWeapon_SmokeShell(WeaponBase* p);
	virtual ~MainWeapon_SmokeShell();

	virtual void OnUpdate_GrenadeShell(REAL32 tm) override;
	virtual void Explosion() override;
	virtual void Reset() override;

private:
	MainWeapon_Smoke* m_pSmoke;
	bool m_bSmokeExplosed;
};