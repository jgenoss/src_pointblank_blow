#if !defined( __GAME_CHARA_ANIM_CONTROL_NPC_H__)
#define __GAME_CHARA_ANIM_CONTROL_NPC_H__

#include "GameCharaAnimControl.h"

class CGameCharaAnimControl3PVNPC : public CGameCharaAnimControl
{
	I3_CLASS_DEFINE( CGameCharaAnimControl3PVNPC, CGameCharaAnimControl);
protected:
public:
	CGameCharaAnimControl3PVNPC();
	virtual ~CGameCharaAnimControl3PVNPC();
	
	virtual void	AnimDebug( i3AIContext * pCtx, REAL32 timeStart = 0.0f)							{}
	virtual void	OnChangeAIContext(void)																{}
	
	virtual void	PlayStop( void)																	{}
	virtual bool	PlayLower( ID_LOWER_AI id, REAL32 timeStart = 0.f, REAL32 timeScale = 1.f)		{ return false; }
	virtual bool	PlayUpper( ID_UPPER_AI id, REAL32 timeStart = 0.f, REAL32 timeScale = 1.f)		{ return false; }
	virtual bool	PlayUpperReturn( void)															{ return false; }
	virtual bool	PlayBodyLowerSync( void)														{ return false; }

	virtual void	OnIdleRun( i3AIContext * pCtx, REAL32 tm)										{}
	virtual void	OnMoveRun( i3AIContext * pCtx, REAL32 tm)										{}
	virtual void	OnAttackRun( i3AIContext * pCtx, REAL32 tm)										{}
	virtual void	OnAttackReadyFinish( i3AIContext * pCtx, REAL32 tm)								{}
	virtual void	OnJumpStartRun( i3AIContext * pCtx, REAL32 tm)									{}
	virtual void	OnJumpEndRun( i3AIContext * pCtx, REAL32 tm)									{}
	virtual void	OnJumpEndFinish( i3AIContext * pCtx, REAL32 tm)									{}
	virtual void	OnLoadMagazineRun( i3AIContext * pCtx, REAL32 tm)								{}
	virtual void	OnLoadBulletRun( i3AIContext * pCtx, REAL32 tm)									{}
	virtual void	OnLoadBulletFinish( i3AIContext * pCtx, REAL32 tm)								{}
	virtual void	OnWeaponSwapRun( i3AIContext * pCtx, REAL32 tm)									{}
	virtual void	OnDamageRun( i3AIContext * pCtx, REAL32 tm)										{}
	virtual void	OnDeathFinish( i3AIContext * pCtx, REAL32 tm)									{}
	virtual void	OnExtensionActivateRun( i3AIContext * pCtx, REAL32 tm)							{}
	virtual void	OnExtensionDeactivateRun( i3AIContext * pCtx, REAL32 tm)						{}
	virtual void	OnUI_IdleARun( i3AIContext * pCtx, REAL32 tm)									{}
	virtual void	OnUI_IdleAFinish( i3AIContext * pCtx, REAL32 tm)								{}
	virtual void	OnUI_IdleBRun( i3AIContext * pCtx, REAL32 tm)									{}
	virtual void	OnUI_IdleBFinish( i3AIContext * pCtx, REAL32 tm)								{}

	bool	PlayDefault( ID_DEFAULT_AI id, REAL32 timeStart = 0.f, REAL32 timeScale = 1.f);
};

#endif
