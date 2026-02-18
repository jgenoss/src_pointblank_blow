#include "pch.h"
#include "GameCharaHUDControlTRex.h"
#include "CrosshairMgr.h"

I3_CLASS_INSTANCE( CGameCharaHUDControlTRex);

/*virtual*/ bool CGameCharaHUDControlTRex::OnApplyCrosshair( void)
{
	if( CGameCharaHUDControlDino::OnApplyCrosshair() == false)
		return false;

	CrosshairMgr::i()->setCrosshairType( CH_MODE_TREX);

	return true;
}
