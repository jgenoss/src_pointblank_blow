#include "pch.h"
#include "RespawnState.h"

CRespawnState::CRespawnState()
{
	Reset();
}
CRespawnState::~CRespawnState()
{}

void CRespawnState::Reset()
{
	m_ui32FlagPos = 0;
	i3mem::FillZero( m_aPos,			sizeof(m_aPos));
	i3mem::FillZero( m_aui32RespawnIdx, sizeof(m_aui32RespawnIdx));
}

void CRespawnState::SetPos(UINT32 ui32SlotIdx, N_POS* pPos, UINT32 ui32RespawnIdx )
{
	I3ASSERT( IsOverflow( pPos ) == FALSE ); 
	I3ASSERT( ui32SlotIdx < SLOT_MAX_COUNT);
	I3ASSERT( IsPosSet( ui32SlotIdx ) == FALSE );
	
	if(  ui32SlotIdx < SLOT_MAX_COUNT )
	{
		pPos->CopyTo( &m_aPos[ ui32SlotIdx ]);
		m_aui32RespawnIdx[ ui32SlotIdx ] = ui32RespawnIdx;
		SetBit( &m_ui32FlagPos, ui32SlotIdx );
		return;
	}
}


void CRespawnState::ResetPos(UINT32 ui32SlotIdx )					// Death 발생시 호출
{
	I3ASSERT( ui32SlotIdx < SLOT_MAX_COUNT);
	
	if(  ui32SlotIdx < SLOT_MAX_COUNT )
	{
		ClearBit( &m_ui32FlagPos, ui32SlotIdx );
		return;
	}
}

// FALSE invalid postion
BOOL CRespawnState::CheckRespawnPos( UINT32 ui32SlotIdx, NxVec3* pRespawnPosInServer, UINT32 ui32RespawnIdx ) //
{
	I3ASSERT( ui32SlotIdx < SLOT_MAX_COUNT);
	I3ASSERT( pRespawnPosInServer != NULL );
	
	if(  ui32SlotIdx < SLOT_MAX_COUNT && pRespawnPosInServer != NULL )
	{
		if( !IsPosSet( ui32SlotIdx ) ) 
		{
			DSERROR("no respawn position(slot:%d)\n", ui32SlotIdx );
			return TRUE;
		}
	
		if( ui32RespawnIdx != m_aui32RespawnIdx[ui32SlotIdx] )
		{
			DSERROR("respawn idx is not valid(slot:%d, svr:%d, cli:%d)\n", ui32SlotIdx, m_aui32RespawnIdx[ ui32SlotIdx], ui32RespawnIdx );
			return TRUE;
		}

		NxVec3 vPos;
		UDP_UTIL::ConvertNet3DtoVec3D( (VEC3D*)&vPos, GetPos(ui32SlotIdx) );
	
		REAL32 r32DistanceSquared =  vPos.distanceSquared( *pRespawnPosInServer );
		I3TRACE( "slot:%d, r32DistanceSquared=%f\n", ui32SlotIdx, r32DistanceSquared);
		
		if( r32DistanceSquared < 4.0f ) 
		{
			return TRUE;
		}
		else
		{
			DSERROR("====RespawnHack====\n");
			DSERROR("Client Pos : %2f %2f %2f \n",vPos.x,vPos.y,vPos.z);
			DSERROR("Server Pos : %2f %2f %2f \n",pRespawnPosInServer->x,pRespawnPosInServer->y,pRespawnPosInServer->z);
			DSERROR("HMS: respawn r32DistanceSquared:%f\n", r32DistanceSquared );
			DSERROR("====RespawnHack====\n");
			return FALSE; // invalid position
		}
	}
	
	return TRUE;
}

	
	