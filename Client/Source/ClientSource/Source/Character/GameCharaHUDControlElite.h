#if !defined( __GAME_CHARA_HUD_CONTROL_ELITE_H__)
#define __GAME_CHARA_HUD_CONTROL_ELITE_H__

#include "GameCharaHUDControlDino.h"

class CGameCharaHUDControlElite : public CGameCharaHUDControlDino
{
	I3_CLASS_DEFINE( CGameCharaHUDControlElite, CGameCharaHUDControlDino);

public:

	virtual bool	OnApplyCrosshair( void) override;
};

#endif
