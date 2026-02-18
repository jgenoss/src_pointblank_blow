#include "pch.h"
#include "GameCharaHUDControlElite.h"
#include "CrosshairMgr.h"

I3_CLASS_INSTANCE( CGameCharaHUDControlElite);

/*virtual*/ bool CGameCharaHUDControlElite::OnApplyCrosshair()
{
	if( CGameCharaHUDControlDino::OnApplyCrosshair() == false)
		return false;

	CrosshairMgr::i()->setCrosshairType(CH_MODE_ELITE);

	return true;
}

