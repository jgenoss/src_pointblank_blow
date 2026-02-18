#if !defined( __GAME_CHARA_HUD_CONTROL_TREX_H__)
#define __GAME_CHARA_HUD_CONTROL_TREX_H__

#include "GameCharaHUDControlDino.h"

class CGameCharaHUDControlTRex : public CGameCharaHUDControlDino
{
	I3_CLASS_DEFINE( CGameCharaHUDControlTRex, CGameCharaHUDControlDino);

public:
	virtual bool	OnApplyCrosshair( void) override;
};

#endif
