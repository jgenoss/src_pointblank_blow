#if !defined( __WEAPON_SHOTGUN_13PV_H)
#define __WEAPON_SHOTGUN_13PV_H

#include "WeaponBase.h"

class WeaponShotGun13PV : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponShotGun13PV, WeaponBase);
protected:

public:
	WeaponShotGun13PV(void);
	virtual ~WeaponShotGun13PV(void);

	virtual	void	_OnInitVairable( void) override;
	virtual void	Reset( void) override;
	virtual void	_OnPlayWeaponFireAnim(void) override;

	virtual REAL32	GetAnimFireScaleTime( CHARA_ACTION_UPPER upperState, ID_UPPER_AI ai) override;
	

	// 마스터 관련
private:
	INT8			m_aiiLoad_Mag1PV;
	INT8			m_aiiLoad_Mag3PV;
	INT8			m_aiiLoad_Bullet1PV;
	INT8			m_aiiLoad_Bullet3PV;


public:
	virtual void	OnBindResource( void) override;

	virtual void	OnPlayLoadBulletAnimation( void) override;
	virtual void	OnPlayLoadMagAnimation( void) override;
};

#endif	//__WEAPON_SHOTGUN_13PV_H
