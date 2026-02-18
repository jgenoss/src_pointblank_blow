#ifndef	__WEAPON_DUAL_GRENADE_H__
#define	__WEAPON_DUAL_GRENADE_H__

#include "WeaponGrenade.h"


class WeaponDualGrenade	: public WeaponGrenade
{
	I3_CLASS_DEFINE( WeaponDualGrenade, WeaponGrenade);

private:
	virtual MainWeapon*	CreateMainWeapon() override;

public:
	WeaponDualGrenade();
	virtual	~WeaponDualGrenade();

	virtual void	Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon) override;
	virtual void	OnBindResource(void) override;
	virtual void	Reset() override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual bool	Fire( WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/) override;
	virtual void	OnFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnEndFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	ReturnInstance() override;

	virtual void	SetOwner( CGameCharaBase * pOwner) override;
	virtual void	AttachNode( i3Node * pSceneNode, i3GameNode * pOwner) override;
	virtual void	DetachNode( void ) override;
	virtual void	OnSetBlendAlpha( REAL32 val) override;
	virtual void	OnSetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular) override;
	virtual void	ReturnToReady( void) override;

	bool			getThrowFirstGrenade();

protected:
	virtual void	_OnPlayWeaponIdleAnim(void) override;
	virtual void	_OnPlayWeaponFireAnim(void) override;

};


#endif // __WEAPON_DUAL_GRENADE_H__