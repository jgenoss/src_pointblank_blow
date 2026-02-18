#if !defined( __WEAPON_C5_H__)
#define __WEAPON_C5_H__

#include "WeaponBase.h"

class WeaponC5 : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponC5, WeaponBase);

private:
	virtual MainWeapon*	CreateMainWeapon() override;

protected:
	virtual void	_OnHitCharaByExplosion( CGameCharaBase * pChara, CHARA_HIT_PART part, VEC3D * pDir, VEC3D * pStart) override;
	void			_PlayBoundSound( I3_TERRAIN_TYPE nTerrainType);

	virtual void	_InitCollideeGroup(void) override;
public:

	virtual bool	IsGrenade(void) const override { return true; }

	virtual REAL32	GetThrowSpeed( void) override;

public:
	WeaponC5();
	virtual ~WeaponC5();

	virtual void	SetHitCollideeGroup(void) override;

	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;
	//virtual void	Fire(void) override;
	virtual void	FireEnd(void) override;

	//	네트워크용
	virtual void	NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE ) override;
	virtual void	NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nNetSlotIdx ) override;
	virtual void	NET_Bound( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType ) override;
	virtual void	NET_FirstThrow( VEC3D * pStartPos, VEC3D * pDir, INT32 nNetSlotIdx ) override;

	// UI weapon animation
	virtual void	PlayAnim_UI_IdleB() override;

	// Drive function
	virtual void	OnFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnEndFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	//
	virtual void	ReturnInstance() override;

};

#endif
