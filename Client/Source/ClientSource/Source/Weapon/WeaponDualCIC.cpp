#include "pch.h"
#include "WeaponDualCIC.h"
#include "GameCharaBase.h"
#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"
#include "Camera/GameCamera_FPS.h"
#include "GlobalVariables.h"

#include "MainWeapon_DualCIC.h"

I3_CLASS_INSTANCE( WeaponDualCIC);

WeaponDualCIC::WeaponDualCIC()
{
	
}

WeaponDualCIC::~WeaponDualCIC()
{

}

MainWeapon*		WeaponDualCIC::CreateMainWeapon()
{
	MainWeapon_DualCIC * p = new MainWeapon_DualCIC(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_DualCIC));
	return p;
}

/*virtual*/void WeaponDualCIC::NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type)
{
	MainWeapon_DualCIC* mainWeapon = static_cast<MainWeapon_DualCIC*>(this->GetMainWeapon());
	
	mainWeapon->NET_Fire_DualCIC(pStartPos, pTargetPos, type);
}

/*virtual*/bool WeaponDualCIC::Fire( WEAPON::FIRE_ORDER type)
{	
	MainWeapon_DualCIC* mainWeapon = static_cast<MainWeapon_DualCIC*>(this->GetMainWeapon());
	
	return mainWeapon->Fire_DualCIC(type);
}

/*virtual*/void WeaponDualCIC::LoadMagazine()
{	
	MainWeapon_DualCIC* mainWeapon = static_cast<MainWeapon_DualCIC*>(this->GetMainWeapon());

	return mainWeapon->LoadMagazine_DualCIC();
}

/*virtual*/void	WeaponDualCIC::OnUpdate( REAL32 rDeltaSeconds)
{
	WeaponBase::OnUpdate( rDeltaSeconds);

	MainWeapon_DualCIC* mainWeapon = static_cast<MainWeapon_DualCIC*>(this->GetMainWeapon());
	
	mainWeapon->OnUpdate_DualCIC(rDeltaSeconds);

}

/*virtual*/CWeaponInfo* WeaponDualCIC::getRealWeaponInfo(void)
{
	MainWeapon_DualCIC* mainWeapon = static_cast<MainWeapon_DualCIC*>(this->GetMainWeapon());

	if( mainWeapon->getCurrentUseWeapon() == WEAPON_CIC_CURRENTUSE_LEFT)
	{
		return mainWeapon->getLeftWeapon()->getWeaponInfo();
	}
	else
	{
		return this->getWeaponInfo();
	}
}

WEAPON_CIC_CURRENTUSE WeaponDualCIC::getCurrentUseWeapon(void)
{
	MainWeapon_DualCIC* mainWeapon = static_cast<MainWeapon_DualCIC*>(this->GetMainWeapon());

	return mainWeapon->getCurrentUseWeapon();
}
