#if !defined( __WEAPON_KNUCKLE_H__)
#define __WEAPON_KNUCKLE_H__

#include "WeaponDual.h"

	enum	WEAPON_HUMAN_TYPE
	{
		WEAPON_MALE = 0,
		WEAPON_FEMALE,

		WEAPON_HUMAN_MAX
	};
	
class WeaponKnuckle : public WeaponDualKnife
{
	I3_CLASS_DEFINE( WeaponKnuckle, WeaponDualKnife);
private:
	virtual MainWeapon*		CreateMainWeapon() override;

protected:
	virtual void	_InitCollideeGroup( void) override;
	
public:
	WeaponKnuckle();
	virtual ~WeaponKnuckle();

	virtual void	OnBindResource( void) override;
	virtual void	ReturnInstance( void) override;

	virtual void	PlayAnim_UI_IdleA( void) override;
	virtual void	PlayAnim_UI_IdleB( void) override;

//	virtual void	OnPlayChangeAniImpl( REAL32 rTimeScale = 1.f);
	virtual void	NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE) override;

	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual	 void	OnFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual	 void	OnSecondaryFire( i3AIContext * pCtx, REAL32 tm) override;
	/****************************************************************************/
	
protected:
	virtual void	_OnPlayWeaponIdleAnim(void) override;
	virtual void	_OnPlayWeaponFireAnim(void) override;
	virtual void	_OnProcessFireEndAnim( REAL32 rDelay) override;
};

#endif
