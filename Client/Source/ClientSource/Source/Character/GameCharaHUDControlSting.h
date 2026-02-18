#if !defined( __GAME_CHARA_HUD_CONTROL_STING_H__)
#define __GAME_CHARA_HUD_CONTROL_STING_H__

#include "GameCharaHUDControlDino.h"

class CGameCharaHUDControlSting : public CGameCharaHUDControlDino
{
	I3_CLASS_DEFINE( CGameCharaHUDControlSting, CGameCharaHUDControlDino);

public:
	virtual bool	OnApplyCrosshair( void) override;
};

#endif
