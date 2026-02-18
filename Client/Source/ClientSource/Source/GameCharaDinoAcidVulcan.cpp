#include "pch.h"
#include "GameCharaDinoAcidVulcan.h"

#include "GameCharaContexts.h"
#include "StageBattle.h"
#include "Crosshair_Dino_Raptor.h"
#include "WeaponDino.h"
#include "CrosshairMgr.h"
#include "GameStateMgr.h"

#include "ui/UIHudDefine.h"
#include "ui/UIHudUtil.h"

#include "./StageBattle/UserContext.h"
#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"

I3_CLASS_INSTANCE( CGameCharaDinoAcidVulcan);

/*virtual*/ void CGameCharaDinoAcidVulcan::_CreateDino( void)
{
	CGameCharaDinoAcid::_CreateDino();
}

/*vurtual*/ void CGameCharaDinoAcidVulcan::OnUpdate( REAL32 rDeltaSeconds)
{
	CGameCharaDinoAcid::OnUpdate( rDeltaSeconds);
}

/*vurtual*/ void CGameCharaDinoAcidVulcan::OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaDinoAcid::OnDinoAttackRun(pCtx, tm);
}

/*vurtual*/ void CGameCharaDinoAcidVulcan::ResetChara()
{
	CGameCharaDinoAcid::ResetChara();
}
