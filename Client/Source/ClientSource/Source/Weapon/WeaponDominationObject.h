#if !defined( _WEAPON_DOMINATION_OBJECT_H__)
#define _WEAPON_DOMINATION_OBJECT_H__

#include "weaponbase.h"

#ifdef DOMI_OBJECT

class WeaponDominationObject :	public WeaponBase
{
	I3_CLASS_DEFINE( WeaponDominationObject, WeaponBase);

protected:
	REAL32			m_rInstallElapsedTime;		// 설치 타임 (DEBUG)
	REAL32			m_rUninstallElpasedTime;	// 해체 타임 (DEBUG)
	REAL32			m_rExplosionElapsedTime;	// 폭파 타임 (DEBUG)
	REAL32			m_rTracerUpdateTime;
	MATRIX			m_mtxOld;
	bool			m_bDisarm;					//	해제중 여부	


	i3Transform	*		m_pEffectBombLight;
	INT32				m_idxGlowLightEffect;
	INT32				m_idxLightEffectDummy;

	
	
public:
	bool			m_bEnableActive;			// 설치됨

protected:

	virtual	void	_OnInitVairable( void) override;
	virtual void	_InitCollideeGroup( void) override;
	void			_OnCollision(CGameCharaBase * pChara, i3CollideeLine * pLine, I3_PHYSIX_HIT_RESULT * pResult);
	
	
public:
	WeaponDominationObject(void);
	virtual ~WeaponDominationObject(void);

	virtual void	PostCreate() override;
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	Reset() override;
	//virtual void	Fire(void) override;
	virtual void	FireEnd(void) override;
	virtual void	ReturnToReady( void) override;

	virtual void	InstallToWorld(i3Node* pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pPos, bool bToWorld, MATRIX* pMat, REAL32 theta = 0.0f) override;
	virtual	void	ReturnInstance( void) override;

	

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	/****************************************************************************/

	void			ActiveDisarm( bool bEnable);
	void			Uninstall( void);
	REAL32			GetInstallElapsedTime( void)		{ return m_rInstallElapsedTime;			}
	REAL32			GetUninstallElapsedTime( void)		{ return m_rUninstallElpasedTime;		}
	REAL32			GetExplosionElapsedTime( void)		{ return m_rExplosionElapsedTime;		}
	bool			IsInstalled( void)					{ return m_bEnableActive;				}
};

#endif

#endif