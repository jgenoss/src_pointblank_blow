#pragma once

#include "MainWeapon_Dual.h"

class MainWeapon_DualSMG : public MainWeapon_Dual 
{
public:
	MainWeapon_DualSMG(WeaponBase* p);

	bool	Fire_DualSMG( WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE);
	void	NET_Fire_DualSMG( VEC3D *pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE);
	
	void	OnLoadMag_DualSMG(i3AIContext * pCtx, REAL32 tm, bool bDoneLoadMag );
private:
	virtual void	Reset() override;
	virtual void	LoadMagazine( void) override;
	virtual void	ReturnToReady() override;
};
