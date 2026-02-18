#pragma once

#include "MainWeapon_Grenade.h"
#include "MainWeapon_DecoyBomb.h"

class WeaponDecoySndInterval;

class MainWeapon_EventBomb : public MainWeapon_DecoyBomb
{

public:

	MainWeapon_EventBomb(WeaponBase* p);
	virtual ~MainWeapon_EventBomb();

private :

	// virtual function
	virtual const WeaponDecoySndInterval *	_GetSndInterval(void) override;
	virtual i3SoundPlayInfo	*				_GetSoundPlayInfo(void) override;
};