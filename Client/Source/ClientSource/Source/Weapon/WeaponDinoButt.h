#if !defined( _WEAPON_DINO_BUTT_H__)
#define _WEAPON_DINO_BUTT_H__

#include "WeaponDino.h"

class WeaponDinoButt :	public WeaponDino
{
	I3_CLASS_DEFINE( WeaponDinoButt, WeaponDino);

public:
	WeaponDinoButt( void);
	virtual ~WeaponDinoButt( void);

public:
	void	ProcessDinoHit(CHARA_DAMAGEINFO * pDamageInfo);
};


#endif // _WEAPON_DINO_BUTT_H__