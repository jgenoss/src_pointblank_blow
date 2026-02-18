#if !defined( __WEAPON_DUAL_UZI_SILENCER_H__)
#define __WEAPON_DUAL_UZI_SILENCER_H__

#include "WeaponDualUzi.h"

class CGameCharaBase;

class WeaponDualUziWithSilencer : public WeaponDualUzi
{
	I3_CLASS_DEFINE( WeaponDualUziWithSilencer, WeaponDualUzi);

private:

	virtual MainWeapon*		CreateMainWeapon() override;

protected:
	

public:
	WeaponDualUziWithSilencer();
	virtual ~WeaponDualUziWithSilencer();
};

#endif

