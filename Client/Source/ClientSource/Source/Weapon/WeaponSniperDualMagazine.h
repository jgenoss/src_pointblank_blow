#ifndef __WEAPON_SNIPER_DUAL_MAGAZINE_H__
#define __WEAPON_SNIPER_DUAL_MAGAZINE_H__
#include "WeaponDualMagazine.h"
#include "WeaponBase.h"

class WeaponSniperDualMagazine : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponSniperDualMagazine, WeaponBase);



protected:

	INT8				m_aiiLoadMagazine_DualShift1PV;
	INT8				m_aiiLoadMagazine_DualShift3PV;
public:
	WeaponSniperDualMagazine();
	virtual ~WeaponSniperDualMagazine();
	virtual void	Reset( void) override;
	virtual void	_OnProcessFireEndAnim( REAL32 rDelay) override;
	virtual void	OnDrop( void) override;

	
	virtual INT32	_OnProcessFireBulletCount( void) override;
	virtual void	OnBindResource( void) override;
	virtual void	OnPlayExtensionAnimation( bool bVal, bool bAnimPlay = true) override;
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;
	virtual bool	Fire( WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/) override;
	virtual bool	OnLoadMag( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnQuickLoadMag( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnEndQuickLoadMag( i3AIContext * pCtx, REAL32 tm) override;

};

#endif
