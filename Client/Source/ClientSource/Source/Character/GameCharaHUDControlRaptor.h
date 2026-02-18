#if !defined( __GAME_CHARA_HUD_CONTROL_RAPTOR_H__)
#define __GAME_CHARA_HUD_CONTROL_RAPTOR_H__

#include "GameCharaHUDControlDino.h"

class CGameCharaHUDControlRaptor : public CGameCharaHUDControlDino
{
	I3_CLASS_DEFINE( CGameCharaHUDControlRaptor, CGameCharaHUDControlDino);

public:
	virtual bool	OnApplyCrosshair( void) override;
};

#endif
