#if !defined( __WEAPONDUALSHOTGUN_H__ )
#define __WEAPONDUALSHOTGUN_H__

#include "WeaponShotGun.h"
#include "WeaponShotGunGenderWithDual.h"

class WeaponDualShotGun : public WeaponShotGun
{
	I3_CLASS_DEFINE( WeaponDualShotGun, WeaponShotGun);

private:
	virtual	MainWeapon *	CreateMainWeapon(void) override;

	i3Node *		m_pBullet[LOD_COUNT][BULLET_COUNT];
	INT8			m_i8AIIndex[GENDER_COUNT][AI_COUNT];	

protected:
	virtual	void	_OnInitVairable( void) override;
	virtual void	_OnAddTerrainEffect( WEAPON_TERRAIN_EFFECT_INFO * pInfo) override;
	virtual void	_PlayFireSound( WEAPON::FIRE_SOUND_TYPE type = WEAPON::FIRE_SOUND_NORMAL) override;

	void			_VisibleBullet( bool bEnable);
	void			_BindAnimationIndex(CGameCharaBase * pOwner);

public:
	WeaponDualShotGun();
	virtual ~WeaponDualShotGun();

	virtual void	OnBindResource(void) override;
	virtual void	SetOwner( CGameCharaBase * pOwner) override;

	/****************************************************************************/
	/*							Weapon Order Functions							*/
	/****************************************************************************/
	virtual bool	Fire( WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/) override;
	virtual void	FireEnd(void) override;

	/****************************************************************************/
	/*							Animation Play Functions						*/
	/****************************************************************************/
	virtual void	OnPlayChangeAniImpl( REAL32 rTimeScale = 1.f) override;

	/****************************************************************************/
	/*								Network Functions							*/
	/****************************************************************************/
	virtual void	NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE ) override;

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual void	OnEndFire( i3AIContext * pCtx, REAL32 tm) override;

	//////////////////////////////////////////////////////////////////////////////

	virtual REAL32	GetFireDelay(void) override;

	virtual bool	isDualGun( void) override { return true; }

	virtual INT32   _OnProcessFireBulletCount(void) override;
};

#endif	// __WEAPONDUALSHOTGUN_H__
