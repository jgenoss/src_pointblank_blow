#if !defined( __WEAPON_LOAD_BULLET_TYPE_H__)
#define __WEAPON_LOAD_BULLET_TYPE_H__

#include "WeaponBase.h"

enum HUMAN_TYPE
{
	HUMAN_MALE,
	HUMAN_FEMALE,

	HUMAN_MAX
};

enum WEAPON_ACTION_TYPE
{
	WEAPON_ACTION_NORMAL,
	WEAPON_ACTION_MASTER,

	WEAPON_ACTION_MAX
};

class WeaponLoadBullet_Type : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponLoadBullet_Type, WeaponBase);
protected:
	
	INT8	m_aii_LoadMag_Ready[CHARA_VIEW_MAX][HUMAN_MAX][WEAPON_ACTION_MAX];	// 재장전 준비 동작
	INT8	m_aii_FireBullet[CHARA_VIEW_MAX][HUMAN_MAX][WEAPON_ACTION_MAX];		// 무기 발사후 볼트액션 동작

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

	void _LoadBullet( i3AIContext * pCtx, REAL32 tm);

public:
	
	virtual bool	isBulletFireWeapon( void) override { return !(getFireOrder() == WEAPON::SECONDARY_FIRE); }

public:
	WeaponLoadBullet_Type();
	virtual ~WeaponLoadBullet_Type();

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
	virtual void	OnFireBullet( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnEndFireBullet( i3AIContext * pCtx, REAL32 tm) override;
	/****************************************************************************/

	virtual void	PlayAnim_UI_IdleB( void) override;

	void			MeleeCollision( VEC3D * pStartPos, VEC3D * pTargetPos);

	virtual REAL32	GetMoveSpeed( WEAPON_MOVE_SPEED_TYPE type) override;
	virtual REAL32	GetRange() override;
	virtual REAL32	GetDPD_Rate() override;
	virtual REAL32	GetDamage( bool bRandomDamage = true) override;
};

#endif
