#include "pch.h"
#include "WeaponCrossFire.h"
#include "GameCharaEquipContext.h"

#include "MainWeapon_CrossFire.h"

I3_CLASS_INSTANCE( WeaponCrossFire);//, WeaponDualHandGun);

WeaponCrossFire::WeaponCrossFire(void)
{
	
}

WeaponCrossFire::~WeaponCrossFire(void)
{
}

MainWeapon* WeaponCrossFire::CreateMainWeapon()
{
	MainWeapon_CrossFire * p = new MainWeapon_CrossFire(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_CrossFire));
	return p;
}

/*virtual*/ bool WeaponCrossFire::Fire( WEAPON::FIRE_ORDER type)
{
	MainWeapon_CrossFire* mainWeapon = static_cast<MainWeapon_CrossFire*>(GetMainWeapon());
	return mainWeapon->Fire_CrossFire(type);
}


/*virtual*/ void WeaponCrossFire::NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type)
{
	MainWeapon_CrossFire* mainWeapon = static_cast<MainWeapon_CrossFire*>(GetMainWeapon());
	mainWeapon->NET_Fire_CrossFire(pStartPos, pTargetPos, type);
}


/*virtual*/ void WeaponCrossFire::OnPlayChangeAniImpl( REAL32 rTimeScale /*= 1.f*/)
{
	// revision 28684
	MainWeapon_CrossFire* mainWeapon = static_cast<MainWeapon_CrossFire*>(GetMainWeapon());
	mainWeapon->NET_ChangeDualGun( m_pOwner->getViewType(), rTimeScale);
}


/*virtual*/ REAL32 WeaponCrossFire::GetRange(void)
{
	MainWeapon_CrossFire* mainWeapon = static_cast<MainWeapon_CrossFire*>(GetMainWeapon());
	return mainWeapon->GetRange_CrossFire();
}


/*virtual*/ REAL32 WeaponCrossFire::GetFireDelay(void)
{
	MainWeapon_CrossFire* mainWeapon = static_cast<MainWeapon_CrossFire*>(GetMainWeapon());
	return mainWeapon->GetFireDelay_CrossFire();
}

