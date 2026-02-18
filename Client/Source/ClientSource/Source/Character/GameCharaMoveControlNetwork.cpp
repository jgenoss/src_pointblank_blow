#include "pch.h"
#include "GameCharaMoveControlNetwork.h"
#include "GlobalVariables.h"
#include "GameCharaContexts.h"

#define NET_PHYSIX_FLOORCHECK_TIME			0.25f	// netchara

I3_CLASS_INSTANCE( CGameCharaMoveControlNetwork, CGameCharaMoveControl);

CGameCharaMoveControlNetwork::CGameCharaMoveControlNetwork()
{
}

CGameCharaMoveControlNetwork::~CGameCharaMoveControlNetwork()
{
}

void CGameCharaMoveControlNetwork::OnProcessFloorCheck( REAL32 rDeltatime)
{
	CGameCharaMoveControl::OnProcessFloorCheck( rDeltatime);

	if( m_pOwner->getActionContext()->isLower( CHARA_LOWER_JUMP ))
		return;

	REAL32 rElapsed = m_pOwner->getMoveContext()->getFloorCheckTimer();
	rElapsed += rDeltatime;
	m_pOwner->getMoveContext()->setFloorCheckTimer( rElapsed);

	if( rElapsed > NET_PHYSIX_FLOORCHECK_TIME )		// 초당 4회만 체크합니다.
	{	
		m_pOwner->getMoveContext()->CheckFloorTerrain();
		m_pOwner->getMoveContext()->setFloorCheckTimer( 0.f);
	}
}

