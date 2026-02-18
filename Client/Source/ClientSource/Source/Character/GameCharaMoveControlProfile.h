#if !defined( __GAME_CHARA_MOVE_CONTROL_PROFILE_H__)
#define __GAME_CHARA_MOVE_CONTROL_PROFILE_H__

#include "GameCharaMoveControl.h"

class CGameCharaMoveControlProfile : public CGameCharaMoveControl
{
	I3_CLASS_DEFINE( CGameCharaMoveControlProfile, CGameCharaMoveControl);

public:
	virtual void	OnProcessFloorCheck( REAL32 rDeltatime) override {}
};

#endif
