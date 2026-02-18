#if !defined( __WEAPON_ROCKET_LAUNCHER_SHELL_H__)
#define __WEAPON_ROCKET_LAUNCHER_SHELL_H__

#include "weaponbase.h"

#define NET_ROCKETLAUNCHER_OFFTIME		7.f				// 네트워크로 7초간 못 받으면 삭제

#define GRENADESHELL_STATE_LAUNCH			0x0001
#define GRENADESHELL_STATE_EXPLOSION		0x0002
#define GRENADESHELL_STATE_MISFIRE			0x0004


class WeaponRocketLauncherShell : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponRocketLauncherShell, WeaponBase);

private:

	virtual MainWeapon*	CreateMainWeapon() override;

protected:
	virtual void	_InitCollideeGroup( void) override;

public:
	WeaponRocketLauncherShell();
	virtual ~WeaponRocketLauncherShell();

	virtual bool	IsGrenade(void) const override { return true; }
		
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;

	virtual void	Reset() override;
	//virtual void	Fire(void) override;
	virtual void	FireEnd(void) override;

	//	네트워크용
	virtual void	NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE ) override;
	virtual void	NET_Bound( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType ) override;

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual void	OnFire( i3AIContext * pCtx, REAL32 tm) override {}
	virtual void	OnEndFire( i3AIContext * pCtx, REAL32 tm) override;
	/****************************************************************************/

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;


};

#endif // __WEAPON_ROCKET_LAUNCHER_SHELL_H__
