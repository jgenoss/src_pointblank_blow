#if !defined( __WEAPON_SHOTGUN_LAUNCHER_WITH_LASER_H__)
#define __WEAPON_SHOTGUN_LAUNCHER_WITH_LASER_H__

#include "WeaponShotGunLauncher.h"

class CDotLazer;

class WeaponShotGunLauncherWithLaser :	public WeaponShotGunLauncher
{
	I3_CLASS_DEFINE( WeaponShotGunLauncherWithLaser, WeaponShotGunLauncher);
private:
	CDotLazer*				m_pDotLazer;

public:
	REAL32					m_fElapsedTime;
	REAL32					m_fLocalTime;

	bool					m_bReloaded;	

	VEC2D					m_vecRandPos[3];
	VEC2D					m_vecRandTangent[2];
	VEC2D					m_vecRandSub[2];

	REAL32					m_fRandX;
	REAL32					m_fRandY;
	bool					m_bCalcFirst;
	REAL32					m_fCalcTime;

protected:
	void			_CreateVertex(void);
	void			_UpdateVertex(void);
	void			_CreateLazerDot(void);
	void			_UpdateLazerDot(void);

public:
	WeaponShotGunLauncherWithLaser(void);
	virtual ~WeaponShotGunLauncherWithLaser(void);

	virtual void	Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon) override;
	virtual void	Reset( void) override;
	virtual void	OnBindResource( void) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void	AttachNode( i3Node * pSceneNode, i3GameNode * pOwner) override;
	virtual void	DetachNode( void ) override;
	virtual void	OnDrop( void) override;
	void			DotStampDisable();	

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual bool	OnLoadMag( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnEndLoadMag( i3AIContext * pCtx, REAL32 tm) override;
	/****************************************************************************/

	virtual void	GetShotTargetPos( VEC3D * pDest, VEC3D * pStart = nullptr) override;
	virtual void	FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos) override;

	bool			isBeing(VEC2D* pVec);
	bool			isBeing(REAL32 fVal1, REAL32 fVal2);

	void			setEnableDotLaser( bool bFlag)				{ g_pEnvSet->m_bEnableLaserSight = bFlag;		}
	bool			getEnableDotLaser(void)						{ return (g_pEnvSet->m_bEnableLaserSight == true); }

	// revision 47038  M4 CQB-R LV3(EV) 익스텐션 전환시 버그 수정
	void			OnProcessExtension( bool bVal);

	virtual bool	OnLostDevice( bool bLostDevice) override;
	virtual bool	OnRevive( i3RenderContext * pCtx) override;
	virtual void	ReturnInstance( void) override;
};

#endif // __WEAPON_SHOTGUN_LAUNCHER_WITH_LASER_H__