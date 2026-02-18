#include "pch.h"
#include "WeaponHandGrenade.h"
#include "GameCharaBase.h"
#include "GameCharaAnimContext.h"

#include "MainWeapon_HandGrenade.h"


#define	DETACHARROW_TIME			0.01f
I3_CLASS_INSTANCE( WeaponHandGrenade);//, WeaponBase);


MainWeapon*	WeaponHandGrenade::CreateMainWeapon()
{
	MainWeapon_HandGrenade * p = new MainWeapon_HandGrenade(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_HandGrenade));
	return p;
}

WeaponHandGrenade::WeaponHandGrenade()
{

}

WeaponHandGrenade::~WeaponHandGrenade()
{
}

void WeaponHandGrenade::PostCreate( void)
{
	I3ASSERT(m_pExtWeaponInfo != nullptr);
}

void WeaponHandGrenade::Reset()
{
	WeaponBase::Reset();		//	먼저 해야함.
}

/*virtual*/ void WeaponHandGrenade::OnFire( i3AIContext * pCtx, REAL32 tm)
{
	MainWeapon_HandGrenade* mainWeapon = static_cast<MainWeapon_HandGrenade*>(GetMainWeapon());
	mainWeapon->OnFire_HandGrenade(pCtx, tm);
}

