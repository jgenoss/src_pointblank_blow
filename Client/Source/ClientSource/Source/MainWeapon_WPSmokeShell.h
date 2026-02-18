#pragma once

#include "MainWeapon_GrenadeShell.h"

class MainWeapon_WPSmoke;

class MainWeapon_WPSmokeShell : public MainWeapon_GrenadeShell
{
public:
	MainWeapon_WPSmokeShell(WeaponBase* p);
	virtual ~MainWeapon_WPSmokeShell();

	virtual void OnUpdate_GrenadeShell(REAL32 tm) override;
	virtual void Explosion() override;
	virtual void Reset() override;

private:
	bool m_bSmokeExplosed;
	MainWeapon_WPSmoke* m_pWPSmoke;
};