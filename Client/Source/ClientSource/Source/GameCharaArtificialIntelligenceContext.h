#if !defined( __GAME_CHARA_ARTIFICIAL_INTELLIGENCE_CONTEXT_H__)
#define __GAME_CHARA_ARTIFICIAL_INTELLIGENCE_CONTEXT_H__

#include "GameCharaContextBase.h"

#define MAX_AICOMMAND_BUF	64

class CGameCharaAIContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaAIContext, CGameCharaContextBase);
protected:
	REAL32			m_rAICheckTimer;			
	INT32			m_nCurrentAIState;			
	INT32			m_nCurrentTarget;			
	INT32			m_nNextTarget;				
	BOOL			m_bUseAI;
	BOOL			m_bArrowFire;
	VEC3D			m_vTargetDir;
	REAL32			m_rRespawnTimer;
	REAL32			m_rDeathSendTime;
	INT32			m_nFireEffectAttachBoneIdx;
	INT32			m_nFireEffectIdx;
	REAL32			m_rIdleSoundPlayTime;
	REAL32			m_rIdleSoundPlayCheckTime;

protected: 	
	INT32			RoleFindTarget( void);
	void			ProcessDeath( REAL32 rDeltatime);	
	void			ProcessDamage( REAL32 rDeltatime);	
	void			ProcessTargetDir( REAL32 rDeltatime);	
	void			ProcessIdle( REAL32 rDeltatime);
	void			ProcessAttack( REAL32 rDeltatime);	
	BOOL			FindTarget( void);	

public:
	CGameCharaAIContext();
	virtual ~CGameCharaAIContext();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0);	
	virtual void	OnReset( void);

	// change view
	virtual void	OnChangeLocal( void)				{	m_bUseAI = FALSE;	}
	virtual void	OnChangeNetwork( void)				{	m_bUseAI = FALSE;	}
	virtual void	OnChangeNPC( void)					{	m_bUseAI = TRUE;	}
	virtual void	OnFirstUpdate( REAL32 rDeltatime)	{;}	
	virtual void	OnLastUpdate( REAL32 rDeltatime)	{;}
	virtual void	OnUpdate( REAL32 rDeltatime);

	void			SetCommand( INT32 cmd );	
	INT32			getCurrentAIState( void)			{ return m_nCurrentAIState; }	
	INT32			getCurrentTarget( void)				{ return m_nCurrentTarget; }
	void			setCurrentTarget( INT32 idx)		{ m_nCurrentTarget = idx; }

};

#endif
