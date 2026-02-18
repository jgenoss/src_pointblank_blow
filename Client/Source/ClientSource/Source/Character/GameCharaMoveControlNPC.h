#if !defined( __GAME_CHARA_MOVE_CONTROL_NPC_H__)
#define __GAME_CHARA_MOVE_CONTROL_NPC_H__

#include "GameCharaMoveControl.h"

class CGameCharaMoveControlNPC : public CGameCharaMoveControl
{
	I3_CLASS_DEFINE( CGameCharaMoveControlNPC, CGameCharaMoveControl);
protected:
public:
	CGameCharaMoveControlNPC();
	virtual ~CGameCharaMoveControlNPC();

	virtual void	OnProcessFloorCheck( REAL32 rDeltatime) {}
};

#endif
