#if !defined( __WEAPON_DUAL_SMG_H__)
#define __WEAPON_DUAL_SMG_H__

#include "WeaponDual.h"

class WeaponDualSMG : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponDualSMG, WeaponBase);
private:
	virtual MainWeapon*		CreateMainWeapon() override;
public:
	WeaponDualSMG();
	virtual ~WeaponDualSMG();

	virtual void	Reset( void) override;
	virtual void	OnBindResource(void) override;

	virtual bool	Fire( WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/) override;
	virtual void	LoadMagazine( void) override;
	virtual void	ReturnToReady( void) override;

	virtual void	NET_Fire( VEC3D *pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE) override;
	virtual void	OnPlayLoadMagAnimation( void) override;
	virtual void	OnPlayChangeAniImpl(REAL32 rTimeScale = 1.f) override;

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual bool	OnLoadMag( i3AIContext * pCtx, REAL32 tm) override;
	/****************************************************************************/

	virtual bool	isDualGun( void) override { return true;}

	virtual INT32   _OnProcessFireBulletCount(void) override;

};

#endif
