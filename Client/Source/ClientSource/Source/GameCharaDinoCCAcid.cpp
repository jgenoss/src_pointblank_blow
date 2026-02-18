#include "pch.h"
#include "GameCharaDinoCCAcid.h"

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

I3_CLASS_INSTANCE( CGameCharaDinoCCAcid);//, CGameCharaDino);

/*virtual*/ void CGameCharaDinoCCAcid::_CreateDino( void)
{
	CGameCharaDinoAcid::_CreateDino();
}

/*vurtual*/ void CGameCharaDinoCCAcid::OnUpdate( REAL32 rDeltaSeconds)
{
	CGameCharaDinoAcid::OnUpdate( rDeltaSeconds);
}

/*vurtual*/ void CGameCharaDinoCCAcid::OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaDinoAcid::OnDinoAttackRun(pCtx, tm);
}

/*vurtual*/ void CGameCharaDinoCCAcid::ResetChara()
{
	CGameCharaDinoAcid::ResetChara();
}