#if !defined( __WEAPON_DUAL_CIC_H__)
#define __WEAPON_DUAL_CIC_H__

#include "WeaponDual.h"

class CGameCharaBase;

enum WEAPON_CIC_CURRENTUSE
{
	WEAPON_CIC_CURRENTUSE_RIGHT,
	WEAPON_CIC_CURRENTUSE_LEFT,
};

class WeaponDualCIC : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponDualCIC, WeaponBase);
private:
	virtual MainWeapon*		CreateMainWeapon();
public:
	WeaponDualCIC();
	virtual ~WeaponDualCIC();

	virtual bool	Fire( WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/) override;
	virtual void	LoadMagazine( void ) override;

	/****************************************************************************/

	virtual void	NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE) override;
	
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual CWeaponInfo * getRealWeaponInfo( void) override;

	WEAPON_CIC_CURRENTUSE getCurrentUseWeapon();
};

#endif
