#if !defined( __WEAPON_DUAL_HANDGUN_H__)
#define __WEAPON_DUAL_HANDGUN_H__

#include "WeaponDual.h"

class WeaponDualHandGun : public WeaponDual
{
	I3_CLASS_DEFINE( WeaponDualHandGun, WeaponDual);

protected:

	// 소음기 모델링이 있는 권총용
	i3::vector<i3Node*>	m_SilencerMaterialList_Right;
	i3::vector<i3Node*>	m_SilencerMaterialList_Left;

private:
	virtual MainWeapon* CreateMainWeapon();

public:
	WeaponDualHandGun();
	virtual ~WeaponDualHandGun();

	virtual void	Create(CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon) override;

	virtual bool	Fire( WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/) override;

	virtual void	NET_Fire( VEC3D *pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE) override;
	virtual void	OnPlayLoadMagAnimation( void) override;
	virtual bool	OnLoadMag( i3AIContext * pCtx, REAL32 tm) override;
	virtual bool	isDualGun( void) override { return true;}

	// UI weapon animation
	virtual void	PlayAnim_UI_IdleA( void) override {	Reset(); }
	virtual void	PlayAnim_UI_IdleB( void) override {	Reset(); }

	virtual void	OnSetVisibleExtension(bool bVal) override;

protected:

	virtual void	_OnInitVairable(void) override;
};


#endif
