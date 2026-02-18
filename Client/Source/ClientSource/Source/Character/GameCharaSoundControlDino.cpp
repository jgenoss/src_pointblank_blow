#include "pch.h"
#include "GameCharaSoundControlDino.h"

I3_CLASS_INSTANCE( CGameCharaSoundControlDino);

/*virtual*/ void CGameCharaSoundControlDino::OnJumpEnd( void)
{
	g_pSndMng->ReserveSoundPlay( IT_DINO_TERRAIN_TYPE, getMoveContext()->getFloorTerrainType(), GTTBDTR_MOVE_RAPTOR_JUMP_END, m_pOwner->GetPos(), m_pOwner->is1PV());
}