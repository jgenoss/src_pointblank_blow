#if !defined( _WEAPON_DECOYBOMB_H__)
#define _WEAPON_DECOYBOMB_H__

#include "WeaponBase.h"

class WeaponDecoyBomb :	public WeaponGrenade
{
	I3_CLASS_DEFINE( WeaponDecoyBomb, WeaponGrenade);

protected:
	virtual MainWeapon* CreateMainWeapon() override;
public:
	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;
	virtual void	Reset() override;
	virtual void	Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon) override;
public:
	WeaponDecoyBomb(void);
	virtual ~WeaponDecoyBomb(void);
};

#endif