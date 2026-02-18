#if !defined( __GAME_DINO_COLLISION_CONTEXT_H__)
#define __GAME_DINO_COLLISION_CONTEXT_H__

#include "../GameCharaCollisionContext.h"

class CGameDinoCollisionContext : public CGameCharaCollisionContext
{
	I3_CLASS_DEFINE( CGameDinoCollisionContext, CGameCharaCollisionContext);

public:
	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;
};

#endif
