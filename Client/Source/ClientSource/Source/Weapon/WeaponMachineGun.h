#if !defined( __WEAPON_MACHINEGUN_H__)
#define __WEAPON_MACHINEGUN_H__

#include "WeaponBase.h"

class WeaponMachineGun : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponMachineGun, WeaponBase);
protected:
	INT8				m_aiiFire1;		// MachineGun
	INT8				m_aiiFire2;
	INT8				m_aiiFire3;
	INT8				m_aiiFire4;

	// visible check in ReloadMagazine
	bool				m_bMachineGunChangeBullet;

protected:
	virtual	void	_OnInitVairable( void) override;
	
	virtual void	_OnProcessFireMiddleAnim( REAL32 rDelay) override;
	virtual void	_OnProcessFireEndAnim( REAL32 rDelay) override;
	virtual void	_OnPlayWeaponFireAnim( void) override;

public:
	WeaponMachineGun();
	virtual ~WeaponMachineGun();

	virtual void	Reset( void) override;
	virtual void	OnBindResource( void) override;

	virtual INT32	_OnProcessFireBulletCount( void) override;


	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;
	virtual bool	OnLoadMag( i3AIContext * pCtx, REAL32 tm) override;
	/****************************************************************************/


};

#endif
