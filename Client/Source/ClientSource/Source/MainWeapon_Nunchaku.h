#pragma once

#include "MainWeapon_Knife.h"

class MainWeapon_Nunchaku : public MainWeapon_Knife
{
public:
	MainWeapon_Nunchaku(WeaponBase* p);

	virtual void OnPlayChangeAni(REAL32 rTimeScale) override;
};