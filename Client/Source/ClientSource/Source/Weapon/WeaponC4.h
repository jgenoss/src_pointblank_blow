#if !defined( _WEAPON_C4_H__)
#define _WEAPON_C4_H__

#include "weaponbase.h"

#define		UNINSTALL_TIME			7.f

enum BOMB_TIMER_STEP
{
	BOMB_TIMER_STEP_UNKNOWN = 0,
	BOMB_TIMER_STEP_OFF,		//	None
	BOMB_TIMER_STEP_1,			//	2
	BOMB_TIMER_STEP_2,			//	3
	BOMB_TIMER_STEP_3,			//	0
	BOMB_TIMER_STEP_4,			//	7
	//BOMB_TIMER_STEP_5,
	BOMB_TIMER_STEP_COMPLETE,	//	Set

	BOMB_TIMER_STEP_COUNT
};

class WeaponC4 :	public WeaponBase
{
	I3_CLASS_DEFINE( WeaponC4, WeaponBase);

protected:
	REAL32			m_rInstallElapsedTime;		// 설치 타임 (DEBUG)
	REAL32			m_rUninstallElpasedTime;	// 해체 타임 (DEBUG)
	REAL32			m_rExplosionElapsedTime;	// 폭파 타임 (DEBUG)
	REAL32			m_rTracerUpdateTime;
	MATRIX			m_mtxOld;
	bool			m_bDisarm;					//	해제중 여부	
	BOMB_TIMER_STEP	m_nTimerStep;

	i3Transform	*		m_pEffectBombLight;
	i3VertexArray *		m_pVA_BombTimer;
	i3GeometryAttr *	m_pTimerGeoAttr;

	VEC3D *				m_pTimerOrgPos;
	VEC3D *				m_pTimerOrgNormal;
	VEC2D *				m_pTimerOrgTex;

	INT32					m_nWarningCount;			//폭탄이 설치된후 발생되는 경계레벨
    
	i3TimeSequence		*	m_pActiveSoundSequence;
public:
	bool			m_bEnableActive;			// 설치됨

protected:
	void			_InitTimerGeometry(void);
	void			_CreateNumberVertex(void);

	virtual	void	_OnInitVairable( void) override;
	virtual void	_InitCollideeGroup( void) override;

	virtual void	_OnCheckCollisionByExplosion( void) override;
	void			_OnCollision(CGameCharaBase * pChara, i3CollideeLine * pLine, I3_PHYSIX_HIT_RESULT * pResult);

	//	한번 호출시 매번 VA를 Lock 하므로 사용시 조심해야 한다.
	bool			_SetBombTimerTextureCoord( BOMB_TIMER_STEP step = BOMB_TIMER_STEP_OFF);

	BOMB_TIMER_STEP _GetNextTimerStepByTime( REAL32 fTime);

public:
	WeaponC4(void);
	virtual ~WeaponC4(void);

	virtual void	PostCreate( void) override;
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	Explosion() override;
	virtual void	Reset() override;
	//virtual void	Fire(void) override;
	virtual void	FireEnd(void) override;
	virtual void	ReturnToReady( void) override;
	
	void			InstallC4ToWorld(i3Node* pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pPos, bool bToWorld, MATRIX* pMat);
	void			InstallC4ToMoveObject(i3Transform* pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pLocalPos, VEC3D * pWorldPos, MATRIX* pMat);
	
	virtual	void	ReturnInstance( void) override;

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual void	OnFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnEndFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	/****************************************************************************/

	void			UpdateActive( REAL32 rDeltaSeconds );
	void			ActiveDisarm( bool bEnable);
	void			Uninstall( void);
	REAL32			GetInstallElapsedTime( void)		{ return m_rInstallElapsedTime;			}
	REAL32			GetUninstallElapsedTime( void)		{ return m_rUninstallElpasedTime;		}
	REAL32			GetExplosionElapsedTime( void)		{ return m_rExplosionElapsedTime;		}
	bool			IsInstalled( void)					{ return m_bEnableActive;				}
	
	virtual bool	OnLostDevice( bool bLostDevice) override;
	virtual bool	OnRevive( i3RenderContext * pCtx) override;
};

#endif