#include "pch.h"
#include "GameCharaActionControl1PVNetwork.h"
#include "GlobalVariables.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaActionControl1PVNetwork, CGameCharaActionControl1PV );

CGameCharaActionControl1PVNetwork::CGameCharaActionControl1PVNetwork()
{
}

CGameCharaActionControl1PVNetwork::~CGameCharaActionControl1PVNetwork()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	Action

//bool CGameCharaActionControl1PVNetwork::OnActionRunLower( VEC3D * pDir)
//{
//	EXT_ASSERT( pDir != NULL);
//
//	if( i3Vector::IsSame( pDir, &I3_ZEROVECTOR) )
//		return false;
//
//	////////////////////////////////////////////////////////////////////
//	REAL32 fSyncTime = 0.f;
//	REAL32 fScaleTime = 1.f;
//	ID_LOWER_AI nId = getAnimContext()->getMoveIdByDir( pDir);
//	CHARA_ACTION_LOWER state = CHARA_LOWER_RUN;
//	REAL32 fVel = 1.f;
//	
//	if( nId != ID_LOWER_IDLE )
//	{
//		fVel = getMoveContext()->getSpeed();
//		fScaleTime = _CalcRunLowerScaleTime( nId, fVel);
//	}
//
//	if( (getAnimContext()->getOldLowerID() != nId) )
//	{
//		setLower( state);
//		_Animation_Lower( nId, fSyncTime, fScaleTime);
//	}
//
//	return true;
//}

//bool CGameCharaActionControl1PVNetwork::OnActionJumpStart( void)
//{
//	/// Death 상태에서 Jump packet을 받는다면?? 문제가 생길 수 있다. 2010.08.25 순구
//	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )	return false;
//
//	m_pOwner->addCharaStateMask( CHARA_STATE_JUMP);
//	getMoveContext()->setElapsedTakeOffTime( 0.f);
//
//	OnCmdAnimPlayJump();
//
//	g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, getMoveContext()->getFloorTerrainType(), GTBT_JUMP_START, m_pOwner->GetPos(), m_pOwner->is1PV());
//
//	return true;
//}
