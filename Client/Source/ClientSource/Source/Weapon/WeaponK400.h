#if !defined( _WEAPON_K400_H__)
#define _WEAPON_K400_H__

#include "WeaponGrenade.h"

class WeaponK400 :	public WeaponGrenade
{
	I3_CLASS_DEFINE( WeaponK400, WeaponGrenade);

public:
	WeaponK400(void);
	virtual ~WeaponK400(void);
};

#endif