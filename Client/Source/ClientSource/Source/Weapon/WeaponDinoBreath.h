#if !defined( _WEAPON_DINO_BREATH_H__)
#define _WEAPON_DINO_BREATH_H__

#include "WeaponDino.h"

class WeaponDinoBreath :	public WeaponDino
{
	I3_CLASS_DEFINE( WeaponDinoBreath, WeaponDino);

public:
	WeaponDinoBreath( void);
	virtual ~WeaponDinoBreath( void);

public:
	virtual bool		DinoAttackProcess(REAL32 rAnimTime, REAL32 rTimeScale = 0.0f) override;
	void				ThrowShell(void);
};

#endif // _WEAPON_DINO_BREATH_H__