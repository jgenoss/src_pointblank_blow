#if !defined( CGAME_CHARA_LOCAL_H)
#define CGAME_CHARA_LOCAL_H

#include "GameCharaPlayer.h"



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Class : CGameCharaLocal
//	Desc: 주인공 캐릭터 (조작하는 자신)에 필요한 처리를 담당합니다. (키 입력, HUD)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CGameCharaLocal : public CGameCharaPlayer
{
	I3_CLASS_DEFINE(  CGameCharaLocal);

/////////////////////////////////////////////////	Variables

protected:

	//BOOL				m_bDisarmingBomb;		//	폭탄 해제중		//--> CHARA_STATE_DISARMINGBOMB
	//BOOL				m_bArmingBomb;			// 폭탄 설치중		//--> CHARA_STATE_ARMINGBOMB

	//i3CollideeLine			m_CameraLine;
	//REAL32					m_rWorldCollisionLength;

#if defined( I3_DEBUG)
	i3Node			*	m_pRenderableWorldCol;
	i3Node			*	m_pRenderableRoughHitCol;
#endif

/////////////////////////////////////////////////	Functions
public:
	CGameCharaLocal( void);
	virtual ~CGameCharaLocal( void);
	virtual void	Create( i3GameResChara * pRes, CHARA_INFO * pCharaInfo, const CCharaInfo * pDBInfo);
	virtual void	OnUpdate( REAL32 tm);

	virtual void	ResetChara( void);	
	virtual void	DumpChara( void);
	//virtual void	ProcessFirst( REAL32 tm);
	
	//	Network
	//void			ProcessNetworkMime( void);				//--> UpdaterLocal 20080130

	//	Anim (CallBack)
	//virtual void	OnIdleRun( i3AIContext * pCtx, REAL32 tm);
	//virtual void	OnAttackRun( i3AIContext * pCtx, REAL32 tm);
	//virtual void	OnJumpStartRun( i3AIContext * pCtx, REAL32 tm);
	//virtual void	OnLoadMagazineRun( i3AIContext * pCtx, REAL32 tm);
	//virtual void	OnLoadBulletRun( i3AIContext * pCtx, REAL32 tm);
	//virtual void	OnWeaponSwapRun( i3AIContext * pCtx, REAL32 tm);

	//	Action
	//BOOL	ACTION_ActiveMission( WeaponBase * pWeapon);	//--> GameCharaPlayer
	//BOOL	ACTION_ActiveDisarm( void);						//--> GameCharaPlayer
	//BOOL	ACTION_CancelMission( void);					//--> GameCharaPlayer
	//BOOL	ACTION_CancelDisarm( void);						//--> GameCharaPlayer
	//BOOL	IsDisarm( void)				{ return m_bDisarmingBomb;			}	//--> GameCharaPlayer
	//BOOL	IsArm(void)					{ return m_bArmingBomb; }				//--> GameCharaPlayer

	//	HUD
	//void			ShowDamageHUD( VEC3D * pShooter, VEC3D * pHitUser, REAL32 fHitTheta );

	//void			_ProcessScreenEffect( REAL32 tm);
	
protected:
	virtual void	OnInitVariable( void);

	//void			_SetMoveDir( UINT64 key);			//--> InputControlLocal dfly 20080129

	//BOOL			_CrossCollisionToWorld( void );

	// Weapon state set
	//void			_WeaponStateIdle( void);

	//	Network
	//void			_SaveNetworkData( void);
		
	//	Process
	//void			_ProcessOwner( REAL32 tm);			//--> UpdaterLocal	dfly 20080129
	//void			_ProcessMoveInput( REAL32 tm);		//--> InputControlLocal dfly 20080129
	//void			_ProcessWeapon( REAL32 tm);			//--> InputControlLocal	dfly 20080129
	//void			_ProcessExtension( REAL32 tm);		//--> InputControlLocal	dfly 20080129
};

#endif

