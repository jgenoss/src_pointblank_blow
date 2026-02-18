// GroupMaker.h
//
// YouJong Ha
//	Last update : 2010-10-01 (yyyy:mm:dd)
//	
// Description:
//	
//
//
// Dependency: 
//		
//	
#include "pch.h"
#include "GroupMaker.h"
#include "DediRoom.h"
#include "PacketLocker.h"
#include "UdpBufferPool.h"
#include "StageInfoTimer.h"
#include "ModuleControl.h"
#include "ModuleCast.h"

CGroupMaker* g_pRoomManager = NULL;

I3_CLASS_INSTANCE(CGroupMaker, i3ElementBase);

///////////////////////////////////////////////////////////////////////////////
//
// Function: 
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
CGroupMaker::CGroupMaker()
{
	m_ppRooms			= NULL;
	m_pi32GroupIdx		= NULL;
	m_i32AllocatedGroupCount = 0;
	m_i32RoomCount		= 0;
	m_i32GroupCreateCount	= 0;
	m_i32GroupDeleteCount	= 0;
	m_bAt				= FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: 
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
CGroupMaker::~CGroupMaker()
{
	if ( m_ppRooms )
	{	// 컨트롤 서버에 접속하지 못한경우에는 m_ppRooms가 NULL
		for( INT32 i = 0 ; i < m_i32AllocatedGroupCount ; i++ )
		{
			I3_SAFE_RELEASE( m_ppRooms[i] );
		}
	}
	I3MEM_SAFE_FREE( m_ppRooms );
	I3MEM_SAFE_FREE( m_pi32GroupIdx );
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: 
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
BOOL CGroupMaker::Create(void)
{
	// 한번만 호출한다.
	I3ASSERT(m_ppRooms == NULL);

	m_i32AllocatedGroupCount = g_pConfig->m_i32RoomCount;

	if( m_i32AllocatedGroupCount <= 0 )
	{
		DSERROR("[CGroupMaker::Create] Invalid Group Count: %d\n", m_i32AllocatedGroupCount );
		return FALSE;
	}

	m_ppRooms		= (CDediRoom**) i3MemAlloc( sizeof( CDediRoom*) * m_i32AllocatedGroupCount );
	m_pi32GroupIdx	= (INT32*) i3MemAlloc( sizeof( INT32 ) * m_i32AllocatedGroupCount );
	if( m_ppRooms == NULL || m_pi32GroupIdx == NULL )return FALSE;

	for( INT32 i = 0 ; i < m_i32AllocatedGroupCount ; i++ )
	{
		m_ppRooms[i] = CDediRoom::NewObject();
		if( m_ppRooms[i] == NULL) 	return FALSE; 
		if( m_ppRooms[i]->Create() == FALSE )return FALSE; 

		m_pi32GroupIdx[i] = i;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: GetRelayTarget
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
BOOL CGroupMaker::GetRelayTarget( INT32 i32Thread, CUdpBuffer* pRecvBuffer )
{
	CDediRoom* pRoom = GetGroup( pRecvBuffer->GetGroupIdx() );
	if( NULL == pRoom )							return FALSE;

	if( RGS_USE != pRoom->GetGroupState() )		return FALSE;

	// 릴레이 대상 찾기
	if ( pRoom->GetRelayTarget( i32Thread, pRecvBuffer ) )	return TRUE;
		
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: GetGroup
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
CDediRoom* CGroupMaker::GetGroup( INT32 ui32GroupIdx ) const
{
	if( m_ppRooms == NULL ) return NULL;	// dedication loading 중에 요청이 들어 올 수 있다
											// m_ppRooms는 control server로부터 room count를 받은 후에 생성됨
	
	if( ui32GroupIdx < 0 || ui32GroupIdx >= g_pConfig->m_i32RoomCount )	return NULL;

	return m_ppRooms[ ui32GroupIdx ];
}

CDediRoom*	 CGroupMaker::GetGroupInBattle( INT32 i32GroupIdx ) const
{
	CDediRoom* pRoom = GetGroup( i32GroupIdx );
	if( pRoom && pRoom->IsInBattle() ) return pRoom;
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: MakeGroupId_C
//
// Description:
//	- 비어있는 방을 찾아낸다 
//		- 비어있는 방 목록을 가지고 있는 리스트를 만들어서(m_pi32GroupIdx) 하나씩 가져온다
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
UINT32 CGroupMaker::MakeGroupId_C()
{
	BOOL	bContinue = TRUE;
	INT32	i32Count = 0;

 	if( m_ppRooms == NULL ) return INVALID_RELAY_GROUP_ID;	// dedication loading 중에 요청이 들어 올 수 있다
															// m_ppRooms는 control server로부터 room count를 받은 후에 생성됨
	
	while( bContinue ) 
	{
		INT32 i32Idx = m_pi32GroupIdx[ m_i32GroupCreateCount ];
		CDediRoom* pRoom = m_ppRooms[ i32Idx ];

		m_i32GroupCreateCount++;
		m_i32GroupCreateCount %= g_pConfig->m_i32RoomCount;

		if( RGS_READY == pRoom->GetGroupState() )
		{
			return i32Idx;
		}
		
		i32Count++;

		if( i32Count >= g_pConfig->m_i32RoomCount ) break;
	}

	// 비어있는 방을 찾을 수 없다.
	// Config 의  UDPGroupCount 를 늘려주어야 합니다.
	DSERROR( "[MakeGroupId_C] no more free group\n" );

	return INVALID_RELAY_GROUP_ID;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: RegisterGroup_C
//
// Description:
//	- 그룹 Init
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
UINT32 CGroupMaker::RegisterGroup_C( P_BATTLE_ROOM_CREATE_ACK* pInfo, const P_BATTLE_ROOM_CREATE_REQ *pReq, char* pExtInfo, UINT16 ui16ExtSize, INT32 i32SocketIdx )
{
	// 해당 그룹을 찾아 등록
	CDediRoom* pRoom = GetGroup( pInfo->_GroupIdx );
	if( NULL == pRoom )return RELAY_ENCRYPTKEY_DEFAULT;
	
	UINT32 wKey = pRoom->Register_C( pInfo, pReq, pExtInfo, ui16ExtSize, i32SocketIdx );
	if( RELAY_ENCRYPTKEY_DEFAULT != wKey )	m_i32RoomCount++;

	return wKey;
}

EVENT_ERROR CGroupMaker::EnterGroup_C( P_BATTLE_USER_ENTER_REQ* pReq )
{
	CDediRoom* pRoom = GetGroup( pReq->_wGroupIdx );
	if( !pRoom != NULL )
		return EVENT_ERROR_FAIL;

	if( !pRoom->Enter_C(pReq) )
		return EVENT_ERROR_FAIL;
	
	return EVENT_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: RevokeGroup_C
//
// Description:
//	- 그룹을 Destory
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
BOOL CGroupMaker::RevokeGroup_C( UINT32 wGroupIdx )
{
	// 해당 그룹을 찾아 해제
	CDediRoom* pRoom = GetGroup( wGroupIdx );

	if( NULL == pRoom )
	{
		DSERROR( "[RevokeGroup_C] pRoom NULL. RoomIdx:%d\n", wGroupIdx );
		return FALSE;
	}
	if( RGS_USE != pRoom->GetGroupState() )
	{
		DSERROR("[RevokeGroup_C] State NOT USE. RoomIdx:%d\n", wGroupIdx );
		return FALSE; 
	}

	pRoom->Revoke_C( wGroupIdx );
	if( 0 != m_i32RoomCount )
	{
		m_pi32GroupIdx[ m_i32GroupDeleteCount ] = wGroupIdx;

		m_i32GroupDeleteCount++;
		m_i32GroupDeleteCount %= g_pConfig->m_i32RoomCount;

		m_i32RoomCount--;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: RevokeGroupUpdate_C
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
BOOL CGroupMaker::RevokeGroupUpdate_C( UINT32 wGroupIdx )
{
	// 해당 그룹을 찾아 해제
	CDediRoom* pRoom = GetGroup( wGroupIdx );

	if( NULL == pRoom )
	{
		I3TRACE("[RevokeGroupUpdate_C] invalid group index %d\n", wGroupIdx );
		return FALSE;
	}

	pRoom->Revoke_C( wGroupIdx );

	if( 0 != m_i32RoomCount )
	{
		m_pi32GroupIdx[ m_i32GroupDeleteCount ] = wGroupIdx;

		m_i32GroupDeleteCount++;
		m_i32GroupDeleteCount %= g_pConfig->m_i32RoomCount;

		m_i32RoomCount--;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: SetRoundNumber_C
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
BOOL CGroupMaker::SetRoundInfo_C( P_BATTLE_ROUND_REQ * pRoundInfo )
{
	// 해당 그룹을 찾아 라운드 넘버를 설정한다
	CDediRoom* pRoom = GetGroup( pRoundInfo->_GroupIdx );
	if( NULL == pRoom ) return FALSE;

	// reset sync data
	pRoom->ResetSyncData_C	();							// reset
	pRoom->SetRoundInfo_C	( pRoundInfo );				// overwrite chara info
	pRoom->SetRoundNumber	( pRoundInfo->_iRound );		// 최대한 늦게 셋팅
	I3TRACE("round no=%d\n", pRoundInfo->_iRound );
	if( StageID::GetStageUID( pRoom->GetStageID() ) == STAGE_UID_HELISPOT ) pRoom->ResetRespawnHelicopter();
	
	pRoom->getStageInfoTimer()->ResetTimer( pRoom->GetGameTime() );
	g_pModuleCast->Send_PROTOCOL_BATTLE_RoundStart( MODULE_CAST_THREAD_IDX, &pRoom->GetRoomInfo() );
	pRoom->m_r32RoundStartTime = pRoom->GetGameTime();
	pRoom->m_RoundMissionState = MISSION_STATE_BATTLE;
	
	return TRUE;
}

BOOL CGroupMaker::SetQACommand_C( P_BATTLE_QA_AUTO_REQ* pQAInfo )
{
	CDediRoom* pRoom = GetGroup( pQAInfo->_ui32GroupIdx );
	if( NULL == pRoom ) return FALSE;

	pRoom->SetQACommand_C( pQAInfo );

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function: SetDinoInfo_C
//
// Description:
//	- 해당 그룹을 찾아 HP를 리셋합니다
//
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
// 
BOOL CGroupMaker::SetDinoInfo_C( P_BATTLE_M_DINO_INFO_REQ* pDinoInfo )
{
	CDediRoom* pRoom = GetGroup( pDinoInfo->_GroupIdx );
	if( NULL == pRoom )return FALSE;
	pRoom->SetDinoInfo_C( pDinoInfo);
	
	return TRUE;
}

BOOL CGroupMaker::RespawnReq_C( P_BATTLE_RESPAWN_REQ* pRespawn )
{
	CDediRoom* pRoom = GetGroup( pRespawn->_GroupIdx );
	if( !pRoom )
		return FALSE;

	return pRoom->Respawn_C( pRespawn );
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: LeaveGroup_C
//
// Description:
//	- 해당 그룹에서 유저를 탈퇴시킨다. Control -> Dedi
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
EVENT_ERROR CGroupMaker::LeaveGroup_C(P_BATTLE_USER_LEAVE_REQ* pReq)
{
	CDediRoom* pRoom = GetGroup( pReq->_wGroupIdx );
	if( !pRoom )
	{
		DSWARNING("[CGroupMaker::LeaveGroup_C] Invalid Group Index: %d\n", pReq->_wGroupIdx );
		return EVENT_ERROR_FAIL;
	}

	if( !pRoom->Leave_C(pReq->_cSlotIdx) )
		return EVENT_ERROR_FAIL;

	return EVENT_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: OnUpdate_C
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//

void	CGroupMaker::OnUpdate_C( UINT32 wUpdateCheckStartIdx )
{
	UINT32 wUpdateCheckEndIdx = wUpdateCheckStartIdx + UPDATE_CHECK_JUMP_COUNT;

	for( UINT32 i = wUpdateCheckStartIdx ; i < wUpdateCheckEndIdx ; i++ ) 
	{
		if( RGS_USE != m_ppRooms[ i ]->GetGroupState() )	continue;

		// 체크후 5분이 지나도 살아있는 방은 없애준다.
		if( ( m_ppRooms[ i ]->GetUdpRecvTime() + GROUP_DESTORY_TIME ) < i3ThreadTimer::GetServerTime() )
		{
			BOOL bRv = RevokeGroupUpdate_C( i );
			DSERROR("[Warning] OnUpdate RevokeGroup Idx:%u, Create:%u, Server:%u, Port:%d, Rv:%d \n", i, m_ppRooms[i]->GetUdpRecvTime(), i3ThreadTimer::GetServerTime(), m_ppRooms[i]->GetPort(), bRv);
		}
	}	
}

INT32	CGroupMaker::GetAllUserCount(void)
{
	INT32 i32UserCount = 0;
	for ( INT32 i = 0; i < m_i32AllocatedGroupCount; i++)
	{
		if (NULL == m_ppRooms[i]) continue;
		if (RGS_USE != m_ppRooms[i]->GetGroupState())	continue;

		i32UserCount += m_ppRooms[i]->GetPlayingUserCount();
	}

	return i32UserCount;
}



