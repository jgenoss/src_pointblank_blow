#if !defined( __GAME_CHARA_SOUND_CONTROL_DINO_H__)
#define __GAME_CHARA_SOUND_CONTROL_DINO_H__

#include "GameCharaSoundControl.h"

class CGameCharaSoundControlDino : public CGameCharaSoundControl
{
	I3_CLASS_DEFINE( CGameCharaSoundControlDino, CGameCharaSoundControl);

public:
	virtual void	OnJumpEnd( void) override;
};

#endif
