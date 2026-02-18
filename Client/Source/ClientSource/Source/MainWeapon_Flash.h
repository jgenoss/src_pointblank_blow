#pragma once

#include "MainWeapon_Grenade.h"

class MainWeapon_Flash : public MainWeapon_Grenade 
{
public:
	MainWeapon_Flash(WeaponBase* p); 
	virtual ~MainWeapon_Flash();

	void	_OnCheckCollisionByExplosion();
	bool	_OnCheckCharaCollisionByExplosion( CGameCharaBase * pChara, UINT32 nColGroup, REAL32 rExplosionRange);
};

