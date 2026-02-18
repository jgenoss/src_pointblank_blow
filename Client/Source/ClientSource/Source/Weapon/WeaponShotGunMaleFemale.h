#if !defined( __WEAPON_SHOTGUN_MALEFEMALE_H__)
#define __WEAPON_SHOTGUN_MALEFEMALE_H__

#include "WeaponShotGun.h"

#define BULLETCOUNT	3

class WeaponShotGunMaleFemale : public WeaponShotGun
{
	I3_CLASS_DEFINE( WeaponShotGunMaleFemale, WeaponShotGun);

	i3Node*			m_pBullet[ BULLETCOUNT][ 3];		// 3은 LOD 카운트
	
	bool	m_bFirstLoadMag;

private:
	INT32	m_iLoadedBulletCount;
	INT32	m_iLoadableBulletCount;

	INT8	m_aii_LoadMag_Ready_Male; 
	INT8	m_aii_LoadMag_Ready_Female;

	INT8	m_aii_LoadMag_Male;		
	INT8	m_aii_LoadMag_Female;	

	INT8	m_aii_LoadMag2_Male;		
	INT8	m_aii_LoadMag2_Female;	

	INT8	m_aii_LoadBullet_Male;	
	INT8	m_aii_LoadBullet_Female;	

	bool	m_LoadMagChg;

protected:
	virtual	void	_OnInitVairable( void) override;
	virtual void	_OnProcessFireMiddleAnim( REAL32 rDelay) override;

	void			_VisibleBullet( bool bEnable);

public:
	virtual bool	isBulletFireWeapon( void) override { return !(getFireOrder() == WEAPON::SECONDARY_FIRE); }

public:
	WeaponShotGunMaleFemale();
	virtual ~WeaponShotGunMaleFemale();

	virtual void	OnBindResource() override;

	virtual void	OnSetVisibleExtension( bool bVal) override;

	virtual bool	NotMasterLoadMagAnim(INT32 idx) override;

	virtual void	LoadMagazine_Ready( void) override;

	virtual void	OnPlayLoadBulletAnimation( void) override;
	virtual void	OnPlayLoadMagAnimation( void) override;
	virtual void	OnPlayLoadMagReadyAnimation( void) override;

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;
	virtual bool	OnLoadMag( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnLoadMag_Ready( i3AIContext * pCtx, REAL32 tm) override;
	/****************************************************************************/
	
	virtual REAL32	GetMoveSpeed( WEAPON_MOVE_SPEED_TYPE type) override;
	virtual REAL32	GetRange() override;
};

#endif
