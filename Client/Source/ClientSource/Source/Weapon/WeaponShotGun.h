#if !defined( __WEAPON_SHOTGUN_H)
#define __WEAPON_SHOTGUN_H

#include "WeaponBase.h"

enum EShotgunMasterLoadMagOrder
{
	ESMLMO_C = 0,
	ESMLMO_B
};


const INT32 g_i870MCS_MasterBulletCount = 4;

class WeaponShotGun : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponShotGun, WeaponBase);
protected:

public:
	WeaponShotGun(void);
	virtual ~WeaponShotGun(void);

	virtual	void	_OnInitVairable( void) override;
	virtual void	Reset( void) override;
	virtual void	_OnPlayWeaponFireAnim(void) override;

	virtual REAL32	GetAnimFireScaleTime( CHARA_ACTION_UPPER upperState, ID_UPPER_AI ai) override;
	

	// 마스터 관련
private:
	INT32			m_iShotgunMasterLoadMagCnt;

	INT8			m_aiiLoadB_Master1PV;
	INT8			m_aiiLoadB_Master3PV;
	INT8			m_aiiLoadC_Master1PV;
	INT8			m_aiiLoadC_Master3PV;
	INT8			m_aiiLoadD_Master1PV;
	INT8			m_aiiLoadD_Master3PV;

	i3Node*			m_pBullet[g_i870MCS_MasterBulletCount];			


public:
	virtual void	OnBindResource( void) override;

	virtual void	OnPlayLoadBulletAnimation( void) override;
	virtual void	OnPlayLoadMagAnimation( void) override;

	virtual void	OnSetVisibleExtension( bool bVal) override;
	
	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	void			OnLoadC_MasterRun(i3AIContext * pCtx, REAL32 tm);
	void			OnLoadB_MasterRun(i3AIContext * pCtx, REAL32 tm);
	void			RepeatLoadMagShotgunMaster(i3AIContext * pCtx, REAL32 tm, EShotgunMasterLoadMagOrder eMagOrder);
	/****************************************************************************/
	
};

#endif	//__WEAPON_SHOTGUN_H
