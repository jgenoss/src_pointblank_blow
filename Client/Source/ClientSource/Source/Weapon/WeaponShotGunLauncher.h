#if !defined( __WEAPON_SHOTGUN_LAUNCHER_H__)
#define __WEAPON_SHOTGUN_LAUNCHER_H__

#include "WeaponBase.h"

class WeaponShotGunLauncher : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponShotGunLauncher, WeaponBase);
private:
	INT32			m_nLoadedShell;
	INT32			m_nTotalShell;

protected:
	INT8			m_aiiFireShell;
	INT8			m_aiiFireShell1PV, m_aiiFireShell3PV;
	INT8			m_aiiReloadShell1PV, m_aiiReloadShell3PV;

	INT32			m_idxShellFireEffect;
	INT32			m_idxLauncherFXBone;

	INT32			m_idxLauncherFXBullet;
	INT32			m_idxLSSBulletEffect;
	
	bool			m_bLoadedShell;

public:
	INT32			getTotalShellCount(void);
	void			setTotalShellCount( INT32 nCount);

	INT32			getLoadedShellCount(void);
	void			setLoadedShellCount( INT32 nCount);

	bool			isLoadedShell( void)						{ return m_bLoadedShell; }

	virtual CWeaponInfo * getRealWeaponInfo(void) override { if( GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER )return m_pExtWeaponInfo; return m_pWeaponInfo; }

public:
	WeaponShotGunLauncher();
	virtual ~WeaponShotGunLauncher();

	virtual void	Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon) override;
	virtual void	Reset(void) override;
	virtual void	OnBindResource(void) override;

	virtual bool	Fire( WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/) override;
	virtual void	OnPlayExtensionAnimation( bool bVal, bool bAnimPlay = true) override;
	virtual void	OnProcessExtension( bool bVal) override;
	virtual bool	OnCheckExtensionPossible( bool bVal) override;	// revision 46324
	virtual REAL32	GetAnimFireScaleTime( CHARA_ACTION_UPPER upperState, ID_UPPER_AI ai) override;

	void			LoadShell(void);

	void			GetTargetDir( VEC3D * pDir);

	virtual MATRIX *		getLauncherFXMatrix(void) { 
		if( m_idxLauncherFXBone == -1)	return nullptr;
		return getSceneObject()->GetBody()->getCacheMatrix( m_idxLauncherFXBone); 
	}

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnShellFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnEndShellFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnShellReload( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnEndShellReload( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnExtensionActivate( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnExtensionDeactivate( i3AIContext * pCtx, REAL32 tm) override;
	/****************************************************************************/

	virtual void	SetExtWeaponInfo() override;

	virtual void	GetNetBullet(GAMEINFO_BULLET* pBullet) override;
	virtual void	SetNetBullet(GAMEINFO_BULLET* pBullet) override;

	virtual bool	hasLoadedBullet() override {
		return ((GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER && isExtensionActivate()) ? getLoadedShellCount() > 0 : getLoadedBulletCount() > 0);
	}

protected:
	virtual void	_OnEmitFireEffect(void) override;
	virtual void	_OnPlayFireSound(void) override;
	virtual void	_OnPlayWeaponFireAnim(void) override;
	
	// 발사에 따른 총알 카운트를 계산합니다.
	virtual INT32	_OnProcessFireBulletCount(void) override;
	virtual void	_OnProcessFireFirstAnim(void) override;
	virtual void	_OnProcessFireMiddleAnim( REAL32 rDelay) override;
	virtual void	_OnProcessFireEndAnim( REAL32 rDelay) override;
};

#endif
