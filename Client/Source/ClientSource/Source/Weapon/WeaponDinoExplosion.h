#if !defined( _WEAPON_DINO_EXPLOSION_H__)
#define _WEAPON_DINO_EXPLOSION_H__

#include "WeaponDino.h"

class WeaponDinoExplosion :	public WeaponDino
{
	I3_CLASS_DEFINE( WeaponDinoExplosion, WeaponDino);

public:
	WeaponDinoExplosion( void);
	virtual ~WeaponDinoExplosion( void);

public:
	void	ProcessDinoHit(CHARA_DAMAGEINFO * pDamageInfo);
};

#endif // _WEAPON_DINO_EXPLOSION_H__