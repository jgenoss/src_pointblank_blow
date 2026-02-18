#if !defined( __GAME_DINO_ANIM_CONTROL_3PV_H__)
#define __GAME_DINO_ANIM_CONTROL_3PV_H__

#include "GameCharaAnimControl3PV.h"

class CGameDinoAnimControl3PV : public CGameCharaAnimControl3PV
{
	I3_CLASS_DEFINE( CGameDinoAnimControl3PV, CGameCharaAnimControl3PV);

public:
	virtual void	OnMoveRun( i3AIContext * pCtx, REAL32 tm) override;

	virtual void	OnJumpStartRun( i3AIContext * pCtx, REAL32 tm) override;
};

#endif
