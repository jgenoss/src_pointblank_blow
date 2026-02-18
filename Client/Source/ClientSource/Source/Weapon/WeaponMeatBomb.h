#if !defined( __WEAPON_MEATBOMB_H__)
#define __WEAPON_MEATBOMB_H__

#include "WeaponBase.h"


class WeaponMeatBomb :	public WeaponGrenade
{
	I3_CLASS_DEFINE( WeaponMeatBomb, WeaponGrenade );

protected:

	virtual MainWeapon*	CreateMainWeapon() override;

protected:
	virtual void	_InitCollideeGroup( void) override;
	virtual void	Reset() override;

	virtual void	NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nNetSlotIdx ) override;

public:
	WeaponMeatBomb( void );
	virtual ~WeaponMeatBomb( void );

	virtual void	Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon) override;
	
	virtual void	SetHitCollideeGroup(void) override;
};


#endif
