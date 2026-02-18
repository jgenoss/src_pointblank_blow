#if !defined( __WEAPON_SHOTGUN_FIRE13PV_H_)
#define __WEAPON_SHOTGUN_FIRE13PV_H_

#include "WeaponBase.h"

#define BULLETCOUNT_REMINGTON_ETA	5

class WeaponShotGunFire13PV : public WeaponShotGun
{
	I3_CLASS_DEFINE( WeaponShotGunFire13PV, WeaponShotGun);
protected:

public:
	WeaponShotGunFire13PV();
	~WeaponShotGunFire13PV();

	virtual void	_OnInitVairable() override;
	virtual void	Reset() override;
	virtual void	_OnPlayWeaponFireAnim() override;

	virtual REAL32	GetAnimFireScaleTime( CHARA_ACTION_UPPER upperState, ID_UPPER_AI ai) override;

private:
	INT8			m_aiFire_1PV;
	INT8			m_aiFire_3PV;

	i3Node*			m_pBulletForRemington[ BULLETCOUNT_REMINGTON_ETA][ 3];		// 3은 LOD 카운트

private:
	void			_VisibleBullet( bool bEnable);

public:

	virtual REAL32	GetAnimLoadBulletScaleTime( CHARA_VIEW_TYPE eCAT) override;

	virtual void	OnSetVisibleExtension( bool bVal) override;

	virtual void	OnBindResource() override;
	virtual void	OnPlayLoadBulletAnimation() override;
	virtual void	OnPlayLoadMagAnimation() override;
	virtual void	OnPlayChangeAniImpl( REAL32 rTimeScale = 1.f) override;

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual void	OnLoadBullet( i3AIContext * pCtx, REAL32 tm) override;
	virtual bool	OnLoadMag( i3AIContext * pCtx, REAL32 tm) override;
	/****************************************************************************/

};

#endif // __WEAPON_SHOTGUN_FIRE13PV_H_