#pragma once

#include "WeaponKnuckle.h"

class WeaponDualThrowKnife : public WeaponKnuckle
{
	I3_CLASS_DEFINE( WeaponDualThrowKnife, WeaponKnuckle);
private:
	virtual MainWeapon*		CreateMainWeapon() override;

public:
	WeaponDualThrowKnife();
	virtual ~WeaponDualThrowKnife();

	virtual bool	Fire( WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/) override;
	virtual void	Reset(void) override;

	virtual void	NET_Fire(VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE) override;
	virtual void	NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nThrowNetIdx ) override;
	
	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual	 void	OnFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual	 void	OnSecondaryFire( i3AIContext * pCtx, REAL32 tm) override;
	/****************************************************************************/

public:
	bool			getDoThrowKnife() { return m_DoThrowKnife;  }
	
private:
	INT32				m_iAttackSoundPlayCount;
	bool				m_DoThrowKnife;
};
