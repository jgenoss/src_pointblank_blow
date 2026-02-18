#include "pch.h"
#include "UserManager.h"

I3_CLASS_INSTANCE( CUserSessionManager, i3NetworkSessionManager );

CUserSessionManager::CUserSessionManager()
{
	m_pi32GameServerLink	= NULL;
	m_pi32BattleServerLink	= NULL;
	m_ppRing				= NULL;
	m_pStateLog				= NULL;
}

CUserSessionManager::~CUserSessionManager()
{
	OnDestroy(); 
}

BOOL CUserSessionManager::OnCreate( INT32 i32GameCount, INT32 i32BattleCount, INT32 i32ThreadCount )
{
	m_i32Hour			= i3ThreadTimer::LocalTime().GetHour();

	m_i32GameServerCount	= i32GameCount;
	m_i32BattleServerCount	= i32BattleCount;

	m_MaxSessionIdx		= m_i32GameServerCount + m_i32BattleServerCount;
	m_i32ThreadCount	= i32ThreadCount;

	m_pi32GameServerLink = new INT32[ m_i32GameServerCount ];
	if( NULL == m_pi32GameServerLink )																	return FALSE;
	i3mem::Fill( m_pi32GameServerLink, -1, sizeof(INT32)*m_i32GameServerCount );

	m_pi32BattleServerLink = new INT32[ m_i32BattleServerCount ];
	if( NULL == m_pi32BattleServerLink )																return FALSE;
	i3mem::Fill( m_pi32BattleServerLink, -1, sizeof(INT32)*m_i32BattleServerCount );

	m_pStateLog		= (SV_STATE_LOG*)i3MemAlloc( sizeof(SV_STATE_LOG)*m_i32ThreadCount );
	if( NULL == m_pStateLog )																			return FALSE;
	i3mem::FillZero( m_pStateLog, sizeof(SV_STATE_LOG)*m_i32ThreadCount );

	m_ppRing = (i3RingBuffer**)i3MemAlloc( sizeof(i3RingBuffer*)*m_i32ThreadCount );
	if( NULL == m_ppRing )																				return FALSE;

	for( INT32 i = 0 ; i < m_i32ThreadCount ; i++ )
	{
		m_ppRing[i] = i3RingBuffer::NewObject();
		if( (NULL == m_ppRing[i]) || (FALSE == m_ppRing[i]->Create( sizeof(IOCP_RING_PUSH), 1024 ) ))	return FALSE;
	}

	m_pSessionList = (i3NetworkSession**)i3MemAlloc( sizeof(i3NetworkSession*) * m_MaxSessionIdx );
	if( NULL == m_pSessionList )																		return FALSE;
	CUserSession * pUserSession; 
	for(INT32 i = 0; i < m_MaxSessionIdx; i++)
	{
		pUserSession = CUserSession::NewObject();
		if( (NULL == pUserSession) ||
			( FALSE == pUserSession->Create( m_ppRing, m_pStateLog, this ) ) )			return FALSE;

		m_pSessionList[i] = pUserSession;
		m_pSessionList[i]->m_SessionIdx = i;
	}

	return TRUE;
}

BOOL CUserSessionManager::OnDestroy(void)
{
	CUserSession * pUserSession; 
	for(INT32 i = 0; i < m_MaxSessionIdx; i++)
	{
		pUserSession = (CUserSession*) m_pSessionList[i]; 
		I3_SAFE_RELEASE( pUserSession ); 
	}

	if( m_ppRing )
	{
		for( INT32 i = 0 ; i < m_i32ThreadCount ; i++ )
		{
			I3_SAFE_RELEASE( m_ppRing[i] );
		}
		I3MEM_SAFE_FREE( m_ppRing );
	}

	I3_SAFE_DELETE_ARRAY( m_pi32GameServerLink );
	I3_SAFE_DELETE_ARRAY( m_pi32BattleServerLink );
	I3MEM_SAFE_FREE( m_pStateLog );

	return i3NetworkSessionManager::OnDestroy(); 
}

ULONG_PTR CUserSessionManager::ConnectSession( SOCKET hSocket, struct sockaddr_in * pAddr)
{
	//// 이번에 붙은놈이 있는 주소인가.
	//INT32 i32ServerNum = -1;
	//for (UINT32 i = 0; i < g_pConfig->GetGameServerCount(); i++)
	//{
	//	// IP가 GameServer 인가?				
	//	if (pAddr->sin_addr.s_addr == g_pConfig->SetGameServerIP( [i])
	//	{
	//		i32ServerNum = (UINT32)i;
	//		break;
	//	}
	//}
	//if (-1 == i32ServerNum)					return NULL;

	// 이미 붙어있는 Session 은 아닌가.
	CUserSession* pSession;
	for( INT32 i = 0 ; i < m_MaxSessionIdx ; i++ )
	{
		pSession = (CUserSession*)m_pSessionList[ i ]; 
		if( FALSE == pSession->GetIsActive() ) continue;
	}

	m_SessionMutex.Enter(); 
	{
		pSession = (CUserSession*)FindNewSession();
		if(pSession != NULL)
		{
			if( pSession->OnConnect( hSocket, pAddr ) )
			{
				//m_pi32ServerLink[ i32ServerNum ]	= pSession->m_SessionIdx;

				m_ActiveSessionCount++; 
			}
		}
	}
	m_SessionMutex.Leave();
	return (ULONG_PTR)pSession; 
}

void CUserSessionManager::OnUpdate()
{
	if( m_i32Hour != i3ThreadTimer::LocalTime().GetHour() )
	{
		for( INT32 i = 0 ; i < m_i32ThreadCount ; i++ )
		{
			g_pLog->WriteLog( L"US State [%02d] - RecvCount:%d, RecvSize:%d", 
				i, m_pStateLog[i].m_i32RecvCount, m_pStateLog[i].m_i32RecvSize );
		}

		i3mem::FillZero( m_pStateLog, sizeof(SV_STATE_LOG)*m_i32ThreadCount );
		m_i32Hour = i3ThreadTimer::LocalTime().GetHour();

		
	}

	//// 이미 붙어있는 Session 은 아닌가.
	//if ( 0 == i3ThreadTimer::LocalTime().GetSecond() % 10 )
	//{
	//	CUserSession* pSession;
	//	for (INT32 i = 0; i < m_MaxSessionIdx; i++)
	//	{
	//		pSession = (CUserSession*)m_pSessionList[i];
	//		if (!pSession) continue;
	//		if (!pSession->GetIsActive()) continue;
	//		if (INVALID_SOCKET == pSession->GetSocket())
	//		{ 
	//			pSession->OnDisconnect(TRUE);
	//		}
	//	}
	//}
}

BOOL CUserSessionManager::GetBuffer( INT32 i32SessionIdx, INT32& i32ServerIdx, i3NetworkPacket * pPacket )
{
	if( i32SessionIdx < 0 )							return FALSE;
	if( i32SessionIdx >= m_i32ThreadCount )			return FALSE;

	i3RingBuffer* pRing;
	pRing = m_ppRing[ i32SessionIdx ];

	if( 0 == pRing->GetBufferCount() )			return FALSE;

	IOCP_RING_PUSH* pPop	= (IOCP_RING_PUSH*)pRing->Pop();
	i32ServerIdx			= pPop->m_i32ServerIdx;

	INT16 i16Size			= (INT16)(*(INT16*)pPop->m_pBuffer)+PACKETHEADERSIZE;
	if (i16Size < 0)				return FALSE;
	if (i16Size > PACKETBUFFERSIZE) return FALSE;

	pPacket->CopyToBuffer( pPop->m_pBuffer, i16Size );
	pRing->PopIdx();

	return TRUE;
}

BOOL CUserSessionManager::IsActive( INT32 i32SessionIdx )
{
	if( i32SessionIdx < 0 )						return FALSE;
	if( i32SessionIdx >= m_MaxSessionIdx )		return FALSE;

	return ((CUserSession*)m_pSessionList[ i32SessionIdx ])->GetIsActive();
}

BOOL CUserSessionManager::SetServerIdx( ASC_SERVER_TYPE eServerType, INT32 i32Idx, INT32 i32SessionIdx )
{
	INT32*	pi32ServerCount;
	INT32*	pi32ServerLink;

	switch( eServerType )
	{
	case ASC_SERVER_GAME:
		{
			pi32ServerCount	= &m_i32GameServerCount;
			pi32ServerLink	= m_pi32GameServerLink;
		}
		break;
	case ASC_SERVER_BATTLE:
		{
			pi32ServerCount	= &m_i32BattleServerCount;
			pi32ServerLink	= m_pi32BattleServerLink;
		}
		break;
	default:
		return FALSE;
	}
	
	if( i32Idx >= *pi32ServerCount )	return FALSE;

	INT32 i32OldSessionIdx = pi32ServerLink[ i32Idx ];

	if( -1 != i32OldSessionIdx )
	{
		CUserSession* pSession = (CUserSession*)m_pSessionList[ i32OldSessionIdx ];
		if( pSession->CheckSameServer( eServerType, i32Idx ) )
		{
			pSession->OnDisconnect( TRUE );
		}
	}

	pi32ServerLink[ i32Idx ]		= i32SessionIdx;

	return TRUE;
}

BOOL CUserSessionManager::SendPacketMessage( INT32 i32SessionIdx, i3NetworkPacket * pPacket )
{
	if( i32SessionIdx < 0 )															return FALSE;
	if( i32SessionIdx >= m_MaxSessionIdx )											return FALSE;
	if( FALSE == ((CUserSession*)m_pSessionList[ i32SessionIdx ])->GetIsActive() )	return FALSE;

	return ((CUserSession*)m_pSessionList[ i32SessionIdx ])->SendPacketMessage( pPacket );
}

BOOL CUserSessionManager::SendPacketMessageGame( INT32 i32Idx, i3NetworkPacket * pPacket )
{
	if( i32Idx < 0 )							return FALSE;
	if( i32Idx >= m_i32GameServerCount )		return FALSE;

	INT32 i32SessionIdx = m_pi32GameServerLink[ i32Idx ];
	if( -1 == i32SessionIdx )					return FALSE;
	return SendPacketMessage( i32SessionIdx, pPacket );
}

BOOL CUserSessionManager::SendPacketMessageBattle( INT32 i32Idx, i3NetworkPacket * pPacket )
{
	if( i32Idx < 0 )							return FALSE;
	if( i32Idx >= m_i32BattleServerCount )		return FALSE;

	INT32 i32SessionIdx = m_pi32BattleServerLink[ i32Idx ];
	if( -1 == i32SessionIdx )					return FALSE;
	return SendPacketMessage( i32SessionIdx, pPacket );
}