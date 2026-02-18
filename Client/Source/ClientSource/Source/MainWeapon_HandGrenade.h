#pragma once

//

//
#include "MainWeapon_Throw.h"


class MainWeapon_HandGrenade : public MainWeapon_Throw
{
public:
	MainWeapon_HandGrenade(WeaponBase* p);

	void			OnFire_HandGrenade( i3AIContext * pCtx, REAL32 tm);

	void			ThrowShell( VEC3D * pStart, VEC3D * pDir, REAL32 rSpeed, INT32 nNetSlotIdx);


private:

	virtual void	OnCreate( bool bCreateInWeapon) override;
	virtual void	Reset() override;

	void			CalcBullet(void);
	void			CalcGrenadeDirection(void);
	void			GetTargetDir( VEC3D * pDir);

	UINT32			m_uCount;
	VEC3D			m_vRightDir;
	INT32			m_idxFxdummy;
	INT32			m_idxShellFireEffect;
	
	i3Transform *	m_pAttachObjectTransform;
	i3Object *		m_pAttachObject;
	REAL32			m_rBoundObjectLen;
	INT32			m_iAttackSoundPlayCount;
	bool			m_bExplosion;

};
