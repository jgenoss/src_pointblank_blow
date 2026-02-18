#include "pch.h"
#include "GameCharaHUDControlSting.h"
#include "CrosshairMgr.h"

I3_CLASS_INSTANCE( CGameCharaHUDControlSting);

/*virtual*/ bool CGameCharaHUDControlSting::OnApplyCrosshair()
{
	if( CGameCharaHUDControlDino::OnApplyCrosshair() == false)
		return false;

	CrosshairMgr::i()->setCrosshairType(CH_MODE_STING);

	return true;
}

