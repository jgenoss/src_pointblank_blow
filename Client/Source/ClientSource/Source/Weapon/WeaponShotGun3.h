#if !defined( __WEAPON_SHOTGUN_3_H__)
#define __WEAPON_SHOTGUN_3_H__

#include "WeaponShotGun.h"

#define BULLETCOUNT_M1887	5

class WeaponShotGun3 : public WeaponShotGun
{
	I3_CLASS_DEFINE( WeaponShotGun3, WeaponShotGun);
protected:
	INT8	m_aii_LoadMag_Ready_1PV;
	INT8	m_aii_LoadMag_Ready_3PV;
	INT8	m_aii_LoadBullet_1PV;
	INT8	m_aii_LoadBullet_3PV;
	INT8	m_aii_Change_1PV;
	INT8	m_aii_Change_3PV;
	INT8	m_aii_SecondaryAttack_1PV;
	INT8	m_aii_SecondaryAttack_3PV;

	INT8	m_aii_UI_Rebel_Male;
	INT8	m_aii_UI_Rebel_Female;
	INT8	m_aii_UI_Rebel_DFox;
	INT8	m_aii_UI_Rebel_ViperRed;
	INT8	m_aii_UI_SWAT_Male;
	INT8	m_aii_UI_SWAT_Female;
	INT8	m_aii_UI_SWAT_Leopard;
	INT8	m_aii_UI_SWAT_Hide;

	i3Node*			m_pBulletForM1887[ BULLETCOUNT_M1887][ 3];		// 3은 LOD 카운트
	
	bool	m_bFirstLoadMag;

private:
	INT32	m_iLoadedBulletCount;
	INT32	m_iLoadableBulletCount;

protected:
	virtual	void	_OnInitVairable( void) override;
	virtual void	_OnProcessFireMiddleAnim( REAL32 rDelay) override;
	virtual void	_OnPlayWeaponSecondFireAnim( void) override;
	virtual void	_OnPlayTerrainHitSound( WEAPON::FIRE_ORDER fireOrder, UINT32 type, VEC3D * pPos) override;

	virtual void	_OnAddTerrainEffect( WEAPON_TERRAIN_EFFECT_INFO * pInfo) override;

	void			_VisibleBullet( bool bEnable);

public:

	virtual bool	isBulletFireWeapon( void) override { return !(getFireOrder() == WEAPON::SECONDARY_FIRE); }

public:
	WeaponShotGun3();
	virtual ~WeaponShotGun3();

	virtual void	OnBindResource() override;

	virtual void	OnSetVisibleExtension( bool bVal) override;

	virtual bool	NotMasterLoadMagAnim(INT32 idx) override;

	virtual bool	Fire( WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/) override;
	virtual void	FireEnd( void) override;
	virtual void	LoadMagazine_Ready( void) override;

	virtual REAL32	GetAnimFireScaleTime( CHARA_ACTION_UPPER upperState, ID_UPPER_AI ai) override;
	virtual REAL32	GetAnimLoadBulletScaleTime( CHARA_VIEW_TYPE eCAT) override;

	virtual void	OnPlayLoadBulletAnimation( void) override;
	virtual void	OnPlayLoadMagAnimation( void) override;
	virtual void	OnPlayChangeAniImpl( REAL32 rTimeScale = 1.f) override;
	virtual void	OnPlayLoadMagReadyAnimation( void) override;

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnLoadBullet( i3AIContext * pCtx, REAL32 tm) override;
	virtual bool	OnLoadMag( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnSecondaryFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnSwaping( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnLoadMag_Ready( i3AIContext * pCtx, REAL32 tm) override;
	/****************************************************************************/

	void			MeleeCollision( VEC3D * pStartPos, VEC3D * pTargetPos);

	virtual REAL32	GetMoveSpeed( WEAPON_MOVE_SPEED_TYPE type) override;
	virtual REAL32	GetRange() override;
	virtual REAL32	GetDPD_Rate() override;
	virtual REAL32	GetDamage( bool bRandomDamage = true) override;
};

#endif
