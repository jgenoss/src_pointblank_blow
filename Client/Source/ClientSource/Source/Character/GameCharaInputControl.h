#if !defined( __GAME_CHARA_INPUT_CONTROL_H__)
#define __GAME_CHARA_INPUT_CONTROL_H__

#include "GameCharaControlBase.h"

class CGameCharaInputControl : public CGameCharaControlBase
{
	I3_CLASS_DEFINE( CGameCharaInputControl, CGameCharaControlBase);

public:
	
	virtual void	OnFirstUpdate( REAL32 rDeltatime)	{}
	virtual void	OnUpdate( REAL32 rDeltatime)		{}
	virtual void	OnLastUpdate( REAL32 rDeltatime)	{}
};

#endif
