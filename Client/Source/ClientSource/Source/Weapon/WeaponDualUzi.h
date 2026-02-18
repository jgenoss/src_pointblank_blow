#if !defined( __WEAPON_DUAL_UZI_H__)
#define __WEAPON_DUAL_UZI_H__

#include "WeaponDualSMG.h"

class CGameCharaBase;

class WeaponDualUzi : public WeaponDualSMG
{
	I3_CLASS_DEFINE( WeaponDualUzi, WeaponDualSMG);

	virtual MainWeapon*		CreateMainWeapon() override;

public:

	WeaponDualUzi();
	virtual ~WeaponDualUzi();

	virtual void	ReturnInstance( void) override;
};

#endif

