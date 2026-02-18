#pragma once

#include "MainWeapon_Dual.h"

class MainWeapon_DualHandGun : public MainWeapon_Dual
{
public:
	MainWeapon_DualHandGun(WeaponBase* p);


	INT32			getNextUseFireWeapon( void)				{ return m_nNextUseFireWeapon; }	
		
	bool			Fire_DualHandGun( WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE);
	void			NET_Fire_DualHandGun( VEC3D *pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE);

	void			OnPlayLoadMagAnimation_DualHandGun();
	void			OnLoadMag_DualHandGun(i3AIContext * pCtx, REAL32 tm, bool bDoneLoadMag );

private:

	virtual void	Reset() override;
	virtual void	LoadMagazine( void) override;
	virtual void	ReturnToReady( void) override;

	INT32			m_nNextUseFireWeapon;
};

