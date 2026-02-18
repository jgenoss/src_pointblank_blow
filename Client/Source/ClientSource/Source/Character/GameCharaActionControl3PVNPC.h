#if !defined( __GAME_CHARA_ACTION_CONTROL_3PV_NPC_H__)
#define __GAME_CHARA_ACTION_CONTROL_3PV_NPC_H__

#include "GameCharaActionControl3PV.h"

class CGameCharaActionControl3PVNPC : public CGameCharaActionControl3PV
{
	I3_CLASS_DEFINE( CGameCharaActionControl3PVNPC, CGameCharaActionControl3PV);
protected:
public:
	CGameCharaActionControl3PVNPC();
	virtual ~CGameCharaActionControl3PVNPC();

	virtual bool	OnActionReset( void);
	virtual bool	OnActionStandup( void);
	virtual bool	OnActionSteadyLower( void);
	virtual bool	OnActionSteadyUpper( void);
	virtual bool	OnActionJumpStart( void);
	virtual bool	OnActionDamage( CHARA_HIT_PART part, VEC3D * pDir);
	virtual bool	OnActionDeath( INT32 iKillIdx, CHARA_DEATH_TYPE type, VEC3D * pDir);	

	
};

#endif
