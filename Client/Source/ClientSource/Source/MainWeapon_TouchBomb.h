#pragma once

#include "MainWeapon_Grenade.h"

class MainWeapon_TouchBomb : public MainWeapon_Grenade
{
public:
	MainWeapon_TouchBomb(WeaponBase *p);

	virtual void	OnInitVariable() override;
	virtual void	Reset() override;

public:
	virtual void	OnUpdate_Grenade(REAL32 rDeltaSeconds) override;
	virtual void	CheckWorldCollision(REAL32 rDeltaSeconds) override;

private:
	bool			explosion;
};