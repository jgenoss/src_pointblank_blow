#include "pch.h"
#include "LogFile.h"
#include "GroupMaker.h"
#include "RelayGroup.h"

CGroupMaker* g_pGroupMaker = NULL;

I3_CLASS_INSTANCE(CGroupMaker, i3ElementBase);

CGroupMaker::CGroupMaker()
{
	m_ppGroups			= NULL;
	m_pwGroupIdx		= NULL;
	m_wGroupCount		= 0;
	m_wGroupCreateCount	= 0;
	m_wGroupDeleteCount	= 0;
}

CGroupMaker::~CGroupMaker()
{
	for( UINT32 i = 0 ; i < g_pConfig->m_wGroupCount ; i++ )
	{
		I3_SAFE_RELEASE( m_ppGroups[i] );
	}
	I3MEM_SAFE_FREE( m_ppGroups );
	I3MEM_SAFE_FREE( m_pwGroupIdx );
}

// -----------------------------------------------------------
// Name : Create
// Desc : Create
// -----------------------------------------------------------
BOOL CGroupMaker::Create(void)
{
	// 한번만 호출한다.
	I3ASSERT(m_ppGroups == NULL);

	m_ppGroups		= (CRelayGroup**) i3MemAlloc( sizeof( CRelayGroup*) * g_pConfig->m_wGroupCount );
	m_pwGroupIdx	= (UINT32*) i3MemAlloc( sizeof( UINT32 ) * g_pConfig->m_wGroupCount );

	for( UINT32 i = 0 ; i < g_pConfig->m_wGroupCount ; i++ )
	{
		m_ppGroups[ i ] = CRelayGroup::NewObject();
		m_ppGroups[ i ]->Create();
		m_pwGroupIdx[ i ] = i;
	}


	return TRUE;
}

// -----------------------------------------------------------
// Name : JoinGroup
// Desc : 해당 그룹에 유저 IP와 Port 를 등록한다.
// -----------------------------------------------------------
BOOL CGroupMaker::JoinGroup( INT32 i32Thread, RELAY_MEMBER_INFO* pRelayMember, UINT16 sPort )
{
	// 해당 그룹에 참여
	CRelayGroup* pGroup = GetGroup( pRelayMember->_wGroupIdx );

	if( NULL == pGroup )	return FALSE;

	if ( FALSE == pGroup->JoinGroup( i32Thread, pRelayMember, sPort ))
	{
		char szTemp[MAX_PATH];
		sprintf( szTemp, "[ERROR] JoinGroup Group:%u", pRelayMember->_wGroupIdx );
		WRITE_LOG_RELAY( i32Thread, szTemp );
		
		return FALSE;
	}

	return TRUE;
}

// -----------------------------------------------------------
// Name : WithdrawGroup
// Desc : 해당 그룹에서 유저를 탈퇴시킨다. Client -> Relay
// -----------------------------------------------------------
/*
BOOL	CGroupMaker::WithdrawGroup( INT32 i32Thread, RELAY_MEMBER_INFO* pRelayMember )
{
	CRelayGroup* pRelayGroup =  g_pGroupMaker->GetGroup( pRelayMember->_wGroupIdx );
	if( NULL == pRelayGroup ) return FALSE;

	if( RGS_USE != pRelayGroup->GetGroupState() )									return FALSE;

	if( 0 > pRelayMember->_cSlotIdx || pRelayMember->_cSlotIdx >= SLOT_MAX_COUNT )	return FALSE;

	if( FALSE == pRelayGroup->GetMember( pRelayMember->_cSlotIdx )->IsMember() )	return TRUE;

	// 비밀키값 확인
	if( FALSE == pRelayGroup->CheckEncryptKey( pRelayMember->_wEncryptKey ) )
	{
		char szTemp[MAX_PATH];
		sprintf( szTemp, "[ERROR] WithdrawGroup Encrypt Group:%d, OrgKey:%d, UserKey:%d, IP:%d, Port:%d", 
			pRelayMember->_wGroupIdx, pRelayGroup->GetEncryptKey(), pRelayMember->_wEncryptKey, pRelayMember->_wIp, pRelayMember->_sPort );
		WRITE_LOG_RELAY( i32Thread, szTemp );
		return FALSE;
	}

	if( pRelayGroup->GetMember( pRelayMember->_cSlotIdx )->GetIp() != pRelayMember->_wIp )	
	{
		char szTemp[MAX_PATH];
		sprintf( szTemp, "[ERROR] WithdrawGroup IP Group:%d, OrgIP:%d, UserIP:%d", 
			pRelayMember->_wGroupIdx, pRelayGroup->GetMember( pRelayMember->_cSlotIdx )->GetIp(), pRelayMember->_wIp );
		WRITE_LOG_RELAY( i32Thread, szTemp );
		return FALSE;
	}

	if( pRelayGroup->GetMember( pRelayMember->_cSlotIdx )->GetPort() != pRelayMember->_sPort )
	{
		char szTemp[MAX_PATH];
		sprintf( szTemp, "[ERROR] WithdrawGroup Port Group:%d, OrgPort:%d, UserPort:%d", 
			pRelayMember->_wGroupIdx, pRelayGroup->GetMember( pRelayMember->_cSlotIdx )->GetPort(), pRelayMember->_sPort );
		WRITE_LOG_RELAY( i32Thread, szTemp );
		return FALSE;
	}
	if (pRelayGroup->Withdraw( pRelayMember->_cSlotIdx ))			return TRUE;

	return FALSE;
}*/

BOOL CGroupMaker::GetRelayTarget( INT32 i32Thread, CUdpBuffer* pRecvBuffer )
{
	CRelayGroup* pGroup = GetGroup( pRecvBuffer->GetGroupIdx() );
	if( NULL == pGroup )							return FALSE;

	if( RGS_USE != pGroup->GetGroupState() )		return FALSE;

	// 릴레이 대상 찾기
	if ( pGroup->GetRelayTarget( i32Thread, pRecvBuffer ) )	return TRUE;
		
	return FALSE;
}

CRelayGroup* CGroupMaker::GetGroup( UINT32 wGroupIdx ) const
{
	if( wGroupIdx >= g_pConfig->m_wGroupCount )	return NULL;

	return m_ppGroups[ wGroupIdx ];
}

// -----------------------------------------------------------
// Name : MakeGroupId_C
// Desc : 비어있는 방을 찾아낸다 ( 비어있는 방 목록을 가지고 있는 리스트를 만들어서(m_pwGroupIdx) 하나씩 가져온다 )
// -----------------------------------------------------------
UINT32 CGroupMaker::MakeGroupId_C()
{
	BOOL	bContinue = TRUE;
	UINT32	wCount = 0;

 	while( bContinue ) 
	{
		UINT32 wIdx = m_pwGroupIdx[ m_wGroupCreateCount ];
		CRelayGroup* pGroups = m_ppGroups[ wIdx ];

		m_wGroupCreateCount++;
		m_wGroupCreateCount %= g_pConfig->m_wGroupCount;

		if( RGS_READY == pGroups->GetGroupState() )
		{
			return wIdx;
		}
		wCount++;

		if( wCount >= g_pConfig->m_wGroupCount ) break;
	}

	return INVALID_RELAY_GROUP_ID;
}

// -----------------------------------------------------------
// Name : RegisterGroup_C
// Desc : 그룹의 Init
// -----------------------------------------------------------
UINT32 CGroupMaker::RegisterGroup_C( BATTLE_ROOM_INFO * pRelayRoomInfo, UINT32 wGroupIdx )
{
	// 해당 그룹을 찾아 등록
	CRelayGroup* pGroup = GetGroup( wGroupIdx );
	if( NULL == pGroup )return RELAY_ENCRYPTKEY_DEFAULT;

	UINT32 wKey = pGroup->Register_C( pRelayRoomInfo );
	if( RELAY_ENCRYPTKEY_DEFAULT != wKey )	m_wGroupCount++;

	return wKey;
}

// -----------------------------------------------------------
// Name : RevokeGroup_C
// Desc : 그룹을 Destory.
// -----------------------------------------------------------
BOOL CGroupMaker::RevokeGroup_C( UINT32 wGroupIdx )
{
	// 해당 그룹을 찾아 해제
	CRelayGroup* pGroup = GetGroup( wGroupIdx );

	if( NULL == pGroup )
	{
		char szTemp[ MAX_PATH ];
		sprintf( szTemp, "RevokeGroup_C pGroup NULL = %d", wGroupIdx );
		WRITE_LOG_CONTROL( szTemp );
		return FALSE;
	}
	if( RGS_USE != pGroup->GetGroupState() )
	{
		char szTemp[ MAX_PATH ];
		sprintf( szTemp, "RevokeGroup_C State NOT USE = %d", wGroupIdx );
		WRITE_LOG_CONTROL( szTemp );
		return FALSE; 
	}

	pGroup->Revoke_C();
	if( 0 != m_wGroupCount )
	{
		m_pwGroupIdx[ m_wGroupDeleteCount ] = wGroupIdx;

		m_wGroupDeleteCount++;
		m_wGroupDeleteCount %= g_pConfig->m_wGroupCount;

		m_wGroupCount--;
	}
	return TRUE;
}

// -----------------------------------------------------------
// Name : RevokeGroup_MC
// Desc : 그룹을 Destory.
// -----------------------------------------------------------
BOOL CGroupMaker::RevokeGroupUpdate_C( UINT32 wGroupIdx )
{
	// 해당 그룹을 찾아 해제
	CRelayGroup* pGroup = GetGroup( wGroupIdx );

	if( NULL == pGroup )						return FALSE;

	pGroup->Revoke_C();

	if( 0 != m_wGroupCount )
	{
		m_pwGroupIdx[ m_wGroupDeleteCount ] = wGroupIdx;

		m_wGroupDeleteCount++;
		m_wGroupDeleteCount %= g_pConfig->m_wGroupCount;

		m_wGroupCount--;
	}
	return TRUE;
}

// -----------------------------------------------------------
// Name : WithdrawGroup_C
// Desc : 해당 그룹에서 유저를 탈퇴시킨다. Control -> Relay
// -----------------------------------------------------------
BOOL CGroupMaker::WithdrawGroup_C(UINT32 wGroupIdx, UINT8 cSlotIdx )
{
	// 해당 그룹에서 탈퇴
	CRelayGroup* pGroup = GetGroup( wGroupIdx );

	if( NULL == pGroup )	return FALSE;

	if (pGroup->Withdraw_C( cSlotIdx ))			return TRUE;

	return FALSE;
}

void	CGroupMaker::OnUpdate_C( UINT32 wUpdateCheckStartIdx )
{
	UINT32 wUpdateCheckEndIdx = wUpdateCheckStartIdx + UPDATE_CHECK_JUMP_COUNT;

	for( UINT32 i = wUpdateCheckStartIdx ; i < wUpdateCheckEndIdx ; i++ ) 
	{
		if( RGS_USE != m_ppGroups[ i ]->GetGroupState() )	continue;

		// 체크후 1시간이 지나도 살아있는 방은 없애준다.
		if( ( m_ppGroups[ i ]->GetGroupCreateTime() + GROUP_DESTORY_TIME ) < g_pConfig->GetServerTime() )
		{
			char szTemp[MAX_PATH];
			sprintf( szTemp, "[Warring] OnUpdate RevokeGroup Idx:%u, Create:%u, Server:%u, Port:%d", i, m_ppGroups[ i ]->GetGroupCreateTime(), g_pConfig->GetServerTime(), m_ppGroups[ i ]->GetPort() );
			WRITE_LOG_CONTROL( szTemp );

			RevokeGroupUpdate_C( i );
		}
	}	
}