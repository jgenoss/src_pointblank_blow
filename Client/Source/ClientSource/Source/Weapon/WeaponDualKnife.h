#if !defined( __WEAPON_DUAL_KNIFE_H__)
#define __WEAPON_DUAL_KNIFE_H__

#include "WeaponDual.h"

	enum	WEAPON_DUAL_HANDTYPE
	{
		WEAPON_DUAL_HANDTYPE_RIGHT = 0,
		WEAPON_DUAL_HANDTYPE_LEFT,

		WEAPON_DUAL_HANDTYPE_MAX
	};

class WeaponDualKnife : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponDualKnife, WeaponBase);

private:
	virtual MainWeapon*		CreateMainWeapon() override;

protected:
	virtual void	_InitCollideeGroup( void) override;

public:
	WeaponDualKnife();
	virtual ~WeaponDualKnife();

	virtual void	OnBindResource( void) override;
	virtual void	ReturnToReady( void) override;

	virtual bool	isBulletFireWeapon( void) override { return false; }

	virtual bool	Fire( WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/) override;
	virtual void	FireEnd(void) override;
	virtual void	FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos ) override;	// 실제 컬리젼 계산

	bool			DoFireCollision( i3CollideeLine * pCol, INT32 idxGroup);

	virtual void	PlayAnim_UI_IdleB() override;

//	virtual void	OnPlayChangeAniImpl( REAL32 rTimeScale = 1.f);
	virtual void	NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE) override;

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnSecondaryFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnSwaping( i3AIContext *pCtx, REAL32 tm) override;
	virtual void	OnUI_ShowWeapon( i3AIContext * pCtx, REAL32 tm) override;
	/****************************************************************************/


	
};

#endif
