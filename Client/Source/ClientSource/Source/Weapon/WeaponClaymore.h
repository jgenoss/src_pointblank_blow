#if !defined( __WEAPON_CLAYMORE_H__)
#define __WEAPON_CLAYMORE_H__

#include "WeaponGrenade.h"

class WeaponClaymore :	public WeaponGrenade
{
	I3_CLASS_DEFINE( WeaponClaymore, WeaponGrenade );

protected:
	virtual MainWeapon*	CreateMainWeapon();

	virtual void		_InitCollideeGroup( void) override;
	virtual void		_PlayFireSound( WEAPON::FIRE_SOUND_TYPE type = WEAPON::FIRE_SOUND_NORMAL) override;

	void				_SetWeaponLookDir(void);
	void				_CheckChainExplosion(REAL32 rRange, REAL32 rRadian = I3_PI);

public:
	WeaponClaymore( void );
	virtual ~WeaponClaymore( void );

	virtual void		SetHitCollideeGroup(void) override;

	virtual void		OnUpdate(REAL32 rDeltaSeconds) override;
	virtual void		_OnCheckCollisionByExplosion( void) override;
	virtual bool		IsValidExplosionArea(VEC3D * pTargetPos, REAL32 rRange, REAL32 rRadian, bool bChkWorld = true) override;

	// 애니메이션
	virtual void		OnFire(i3AIContext * pCtx, REAL32 tm) override;
	virtual void		OnEndFire(i3AIContext * pCtx, REAL32 tm) override;

	//	네트워크용
	virtual void		NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nNetSlotIdx ) override;

	void				SetWeaponLookDir(VEC3D * pLookDir);
};

#endif // __WEAPON_CLAYMORE_H__
