#if !defined( __GAME_CHARA_ANIMCALLBACK_1PV_H__)
#define __GAME_CHARA_ANIMCALLBACK_1PV_H__

#include "GameCharaAnimControl.h"

class CGameCharaAnimControl1PV : public CGameCharaAnimControl
{
	I3_CLASS_DEFINE( CGameCharaAnimControl1PV , CGameCharaAnimControl);

public:
	virtual void	OnChangeAIContext( const char * pszPVState = "1PV:") override;

	// Anim Callback
	virtual void	OnMoveRun( i3AIContext * pCtx, REAL32 tm) override;
};

#endif
