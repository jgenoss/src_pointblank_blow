#if !defined( _WEAPON_ROCKET_LAUNCHER_H__)
#define _WEAPON_ROCKET_LAUNCHER_H__

#include "WeaponBase.h"

class WeaponRocketLauncher : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponRocketLauncher, WeaponBase);

protected:
	i3::vector<i3Node*>		m_ShellDummyList;
	INT32					m_idxShellFireEffect;

	UINT32					m_uCount;	// Hack count
	bool					m_IsDropped;

public:
	WeaponRocketLauncher(void);
	virtual ~WeaponRocketLauncher(void);

	virtual void	PostCreate( void) override;
	virtual void	Reset( void) override;
	virtual void	OnBindResource( void) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual bool	Fire( WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/) override;
	virtual void	AttachNode( i3Node * pSceneNode, i3GameNode * pOwner) override;
	virtual void	DetachNode( void ) override;

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
	virtual void	WeaponRocketLauncher::ReturnToReady( void) override;

	virtual void	NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nThrowNetIdx) override
	{
		VEC3D vDir;
		i3Vector::Sub( &vDir, pTargetPos, pStartPos);
		i3Vector::Normalize( &vDir, &vDir);
		ThrowShell( pStartPos, &vDir, GetThrowSpeed(), nThrowNetIdx);
	}

	void			ThrowShell( VEC3D * pStart, VEC3D * pDir, REAL32 rSpeed, INT32 nNetSlotIdx);
	void			LoadShell( void);
	void			RemoveShell( void);
	void			GetTargetDir( VEC3D * pDir);

	void			SetDropLauncher(bool bFlag) { m_IsDropped = bFlag; }
};

#endif // _WEAPON_ROCKET_LAUNCHER_H__