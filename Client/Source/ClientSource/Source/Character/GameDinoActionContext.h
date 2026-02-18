#if !defined( __GAME_DINO_ACTION_CONTEXT_H__)
#define __GAME_DINO_ACTION_CONTEXT_H__

#include "GameCharaActionContext.h"

class CGameDinoActionContext : public CGameCharaActionContext
{
	I3_CLASS_DEFINE( CGameDinoActionContext, CGameCharaActionContext);

	friend class CGameCharaBase;

protected:
	/** \brief 고함지르기. */
	void	_Play_Dino_Roar( void);
	/** \brief 대기동작. */
	void	_Play_Dino_Idle( void);
	/** \brief 스턴상태. */
	void	_Play_Dino_Stun( void);
};

#endif
