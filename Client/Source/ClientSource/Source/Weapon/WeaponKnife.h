#if !defined( __WEAPON_KNIFE_H)
#define __WEAPON_KNIFE_H

#include "WeaponBase.h"

enum WEAPON_KNIFE_ATTACK_TYPE
{
	WEAPON_KNIFE_ATTACK = 0,			//	베기 공격
	WEAPON_KNIFE_ATTACK2,				//	베기 컷 공격
	WEAPON_KNIFE_SECONDARY_ATTACK,		//	찌르기 공격

	WEAPON_KNIFE_ATTACK_COUNT
};

class WeaponKnife : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponKnife, WeaponBase);

protected:
	virtual void	_InitCollideeGroup( void) override;
public:
	WeaponKnife( void);
	virtual ~WeaponKnife( void);

	virtual MainWeapon*	CreateMainWeapon() override;

	virtual void	Reset( void) override;
	virtual bool	Fire( WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/) override;
	virtual void	FireEnd(void) override;
	virtual void	FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos ) override;	// 실제 컬리젼 계산
	virtual void	ReturnToReady( void) override;

	virtual bool	isBulletFireWeapon( void) override { return false; }

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnFire( i3AIContext * pCtx, REAL32 tm) override;		// 발사중일 경우 호출됨
	virtual void	OnSecondaryFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnEndFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnUI_ShowWeapon( i3AIContext * pCtx, REAL32 tm) override;
	/****************************************************************************/

	virtual void	PlayAnim_UI_IdleA( void) override;
	virtual void	PlayAnim_UI_IdleB( void) override;

//	void			SetAttackType( WEAPON_KNIFE_ATTACK_TYPE type)		{ m_nAttackType = type;			}
	//	네트워크용
	virtual void	NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE ) override;				// 조준점 및 애니메이션을 세팅
	virtual void	OnPlayChangeAniImpl( REAL32 rTimeScale = 1.f) override;

protected:
	bool	DoFireCollision( i3CollideeLine * pCol, INT32 idxGroup);
	I3_PHYSIX_HIT_RESULT * DoFireCollisionKnife( i3CollideeLine * pline, INT32 idxGroup, bool *bOwnerWarn);

	virtual void	OnDrop( void) override;

	virtual void	_OnProcessFireEndAnim( REAL32 rDelay) override;

};

#endif //__WEAPON_KNIFE_H

