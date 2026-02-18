#pragma once

#include "MainWeapon_DualUzi.h"

class MainWeapon_DualUziWithSilencer : public MainWeapon_DualUzi 
{
public:
	MainWeapon_DualUziWithSilencer(WeaponBase* p);	
	
private:
	virtual void OnCreate(bool bCreateInWeapon);	
};
