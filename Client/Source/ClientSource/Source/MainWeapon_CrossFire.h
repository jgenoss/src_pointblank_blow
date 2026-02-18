#pragma once

#include "MainWeapon_DualHandGun.h"

class MainWeapon_CrossFire : public MainWeapon_DualHandGun
{
public:
	MainWeapon_CrossFire(WeaponBase* p);
	
	bool			Fire_CrossFire( WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE);
	void			NET_Fire_CrossFire( VEC3D *pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE);

	bool			isDualFired(void)	{ return m_bDualFired;		}

	REAL32			GetRange_CrossFire(void);
	REAL32			GetFireDelay_CrossFire(void);


private:
	bool		m_bDualFired;
};

