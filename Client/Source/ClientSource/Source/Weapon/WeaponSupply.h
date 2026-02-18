#if !defined( _WEAPON_SUPPLY_H__)
#define _WEAPON_SUPPLY_H__

#include "weaponbase.h"

class i3Quad;
class WeaponSupply : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponSupply, WeaponBase);

public:
	WeaponSupply();
	virtual ~WeaponSupply();

	virtual void	PostCreate( void) override;
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	Reset() override;
	//virtual void	Fire() override;
	virtual void	FireEnd() override;
	virtual void	ReturnToReady() override;

	void			InstallToWorld(i3Node* pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pPos, bool bToWorld, MATRIX* pMat, REAL32 theta = 0.0f);
	virtual	void	ReturnInstance() override;

	virtual void	OnFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnEndFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	void			ActiveDisarm( bool bEnable);
	void			Uninstall();
	REAL32			GetInstallElapsedTime();
	REAL32			GetUninstallElapsedTime();
	REAL32			GetExplosionElapsedTime();
	bool			IsInstalled();

protected:
	virtual	void	_OnInitVairable() override;
	virtual void	_InitCollideeGroup( void) override;
	virtual void	_OnCheckCollisionByExplosion() override;
	void			_OnCollision(CGameCharaBase * pChara, i3CollideeLine * pLine, I3_PHYSIX_HIT_RESULT * pResult);

protected:
	REAL32			m_rInstallElapsedTime;
	REAL32			m_rUninstallElpasedTime;
	REAL32			m_rExplosionElapsedTime;
	REAL32			m_rTracerUpdateTime;
	MATRIX			m_mtxOld;
	bool			m_bDisarm;
	INT32			m_nWarningCount;

public:
	bool			m_bEnableActive;

#ifdef DOMI_TAG
	i3Quad*			m_pWeaponTag;
#endif
};

inline REAL32 WeaponSupply::GetInstallElapsedTime()		{ return m_rInstallElapsedTime;			}
inline REAL32 WeaponSupply::GetUninstallElapsedTime()	{ return m_rUninstallElpasedTime;		}
inline REAL32 WeaponSupply::GetExplosionElapsedTime()	{ return m_rExplosionElapsedTime;		}
inline bool   WeaponSupply::IsInstalled( void)			{ return m_bEnableActive;				}

#endif