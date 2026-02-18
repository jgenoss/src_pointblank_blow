#if !defined( __WEAPON_GRENADE_LAUNCHER_H__)
#define __WEAPON_GRENADE_LAUNCHER_H__

#include "WeaponBase.h"

class WeaponGrenadeLauncher : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponGrenadeLauncher, WeaponBase);

private:
	INT32				m_nLoadedShell;
	INT32				m_nTotalShell;

	UINT32				m_uCount;

protected:
	INT8				m_aiiFireShell;
	INT8				m_aiiRemoveShell;
	INT8				m_aiiReloadShell;

	INT32				m_idxShellFireEffect;
	INT32				m_idxGrenadeFXBone;
	
	bool				m_bLoadedShell;

public:
	INT32			getTotalShellCount( void);
	void			setTotalShellCount( INT32 nCount);

	INT32			getLoadedShellCount( void);
	void			setLoadedShellCount( INT32 nCount);

	bool			isLoadedShell( void)						{ return m_bLoadedShell; }

	virtual CWeaponInfo * getRealWeaponInfo( void) override { if( isMultipleExtension() ? GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER : isExtensionActivate() == true ) return m_pExtWeaponInfo; return m_pWeaponInfo; }

public:
	WeaponGrenadeLauncher();
	virtual ~WeaponGrenadeLauncher();

	virtual void	Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon) override;
	virtual void	Reset( void) override;
	virtual void	OnBindResource(void) override;
	
	virtual bool	Fire( WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/) override;
	virtual void	OnPlayExtensionAnimation( bool bVal, bool bAnimPlay = true) override;

	virtual REAL32	GetThrowSpeed() override;

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

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnShellFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnShellReload( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnShellRemove( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnExtensionActivate( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnExtensionDeactivate( i3AIContext * pCtx, REAL32 tm) override;
	/****************************************************************************/

	virtual void	SetExtWeaponInfo() override;

	virtual void	GetNetBullet( GAMEINFO_BULLET* pBullet ) override;
	virtual void	SetNetBullet( GAMEINFO_BULLET* pBullet ) override;

private :
	MATRIX * getGrenadeFXMatrix(void)
	{
		if (m_idxGrenadeFXBone == -1)	return nullptr;
		return getSceneObject()->GetBody()->getCacheMatrix(m_idxGrenadeFXBone);
	}
};

#endif
