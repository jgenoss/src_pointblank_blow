#include "pch.h"
#include "GameCharaHUDControlRaptor.h"
#include "CrosshairMgr.h"

I3_CLASS_INSTANCE( CGameCharaHUDControlRaptor);

/*virtual*/ bool CGameCharaHUDControlRaptor::OnApplyCrosshair()
{
	if( CGameCharaHUDControlDino::OnApplyCrosshair() == false)
		return false;

	CrosshairMgr::i()->setCrosshairType(CH_MODE_RAPTOR);

	return true;
}

