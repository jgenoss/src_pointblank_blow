#pragma once

#include "MainWeapon_Dual.h"

class MainWeapon_DualCIC : public MainWeapon_Dual 
{
public:
	MainWeapon_DualCIC(WeaponBase* p);

	WEAPON_CIC_CURRENTUSE	getCurrentUseWeapon( void)				{ return m_nCurrentUseWeapon; }
	void					setCurrentUseWeapon( WEAPON_CIC_CURRENTUSE val)	{ m_nCurrentUseWeapon = val; }

	bool					Fire_DualCIC( WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE);
	void					LoadMagazine_DualCIC();
	void					NET_Fire_DualCIC( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE);
	void					OnUpdate_DualCIC( REAL32 rDeltaSeconds);

private:

	virtual void			ReturnToReady() override;
	virtual void			OnEndLoadMag( i3AIContext * pCtx, REAL32 tm) override;

	WEAPON_CIC_CURRENTUSE	m_nCurrentUseWeapon;
};
