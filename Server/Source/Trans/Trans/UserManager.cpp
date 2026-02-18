#include "pch.h"
#include "UserManager.h"

I3_CLASS_INSTANCE( CUserSessionManager, i3NetworkSessionManager );

CUserSessionManager::CUserSessionManager()
{
	m_pi32ServerLink	= NULL;
	m_ppRing			= NULL;
	m_pi32LogIdx		= NULL;
	m_pStateLog			= NULL;
}

CUserSessionManager::~CUserSessionManager()
{
	OnDestroy(); 
}

BOOL CUserSessionManager::OnCreate( INT32 i32SessionCount, INT32 i32ThreadCount )
{
	m_i32Hour			= i3ThreadTimer::LocalTime().GetHour();

	m_MaxSessionIdx		= i32SessionCount;
	m_i32ThreadCount	= i32ThreadCount;

	m_pi32ServerLink = new INT32[ m_MaxSessionIdx ];
	if( NULL == m_pi32ServerLink )																		return FALSE;
	i3mem::FillZero( m_pi32ServerLink, sizeof(INT32)*m_MaxSessionIdx );

	m_pStateLog		= (SV_STATE_LOG*)i3MemAlloc( sizeof(SV_STATE_LOG)*m_i32ThreadCount );
	if( NULL == m_pStateLog )																			return FALSE;
	i3mem::FillZero( m_pStateLog, sizeof(SV_STATE_LOG)*m_i32ThreadCount );

	m_pi32LogIdx	= (INT32*)i3MemAlloc( sizeof(INT32)*m_i32ThreadCount );
	if( NULL == m_pi32LogIdx )																			return FALSE;

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
		if( (NULL == pUserSession) || ( FALSE == pUserSession->Create( m_ppRing, m_pi32LogIdx, m_pStateLog ) ) )		return FALSE;

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

	I3_SAFE_DELETE_ARRAY( m_pi32ServerLink );
	I3MEM_SAFE_FREE( m_pi32LogIdx );
	I3MEM_SAFE_FREE( m_pStateLog );

	return i3NetworkSessionManager::OnDestroy(); 
}

ULONG_PTR CUserSessionManager::ConnectSession( SOCKET hSocket, struct sockaddr_in * pAddr)
{
	// 이번에 붙은놈이 있는 주소인가.
	INT32 i32ServerNum = -1; 
	for( UINT32 i = 0; i < g_pConfig->GetGameServerCount(); i++)
	{
		// IP가 GameServer 인가?				
		if( pAddr->sin_addr.s_addr == g_pConfig->m_pui32GameServerIp[i] )
		{
			i32ServerNum = (UINT32)i; 
			break; 
		}
	}
	if( -1 == i32ServerNum )					return NULL;

	// 이미 붙어있는 Session 은 아닌가.
	CUserSession* pSession;
	for( INT32 i = 0 ; i < m_MaxSessionIdx ; i++ )
	{
		pSession = (CUserSession*)m_pSessionList[ i ]; 
		if( FALSE == pSession->GetIsActive() ) continue;

		if( i32ServerNum == pSession->GetServerIdx() )
		{
			pSession->OnDisconnect();
		}
	}

	m_SessionMutex.Enter(); 
	{
		pSession = (CUserSession*)FindNewSession();
		if(pSession != NULL)
		{
			if( pSession->OnConnect( hSocket, pAddr, i32ServerNum ) )
			{
				m_pi32ServerLink[ i32ServerNum ] = pSession->m_SessionIdx;
				m_ActiveSessionCount++; 
			}
		}
	}
	m_SessionMutex.Leave();
	return (ULONG_PTR)pSession; 
}

void CUserSessionManager::OnUpdate( INT32 i32LogIdx )
{
	if( m_i32Hour != i3ThreadTimer::LocalTime().GetHour() )
	{
		for( INT32 i = 0 ; i < m_i32ThreadCount ; i++ )
		{
			g_pLog->WriteLog( L"US State [%02d] - RecvCount:%d, RecvSize:%d, CompletedCount : %d, CompletedSize : %d",
				i, m_pStateLog[i].m_i32RecvCount, m_pStateLog[i].m_i32RecvSize, m_pStateLog[i].m_i32CompletedCount, m_pStateLog[i].m_i32CompletedSize );
		}

		i3mem::FillZero( m_pStateLog, sizeof(SV_STATE_LOG)*m_i32ThreadCount );
		m_i32Hour = i3ThreadTimer::LocalTime().GetHour();
	}
}

BOOL CUserSessionManager::GetBuffer( INT32 i32Idx, INT32& i32ServerIdx, i3NetworkPacket * pPacket )
{
	if( i32Idx < 0 )							return FALSE;
	if( i32Idx >= m_i32ThreadCount )			return FALSE;

	i3RingBuffer* pRing;
	pRing = m_ppRing[ i32Idx ];

	if( 0 == pRing->GetBufferCount() )			return FALSE;

	IOCP_RING_PUSH* pPop	= (IOCP_RING_PUSH*)pRing->Pop();
	i32ServerIdx			= pPop->m_i32ServerIdx;
	INT16 i16Size			= (INT16)(*(INT16*)pPop->m_pBuffer)+PACKETHEADERSIZE;
	pPacket->CopyToBuffer( pPop->m_pBuffer, i16Size );
	pRing->PopIdx();

	if (i32ServerIdx < 0 || i32ServerIdx >= m_MaxSessionIdx)
	{
		g_pLog->WriteLog( L"####Server Index Check#### ThreadIdx : %d, ServerIdx : %d, Protocol : %d", i32Idx, i32ServerIdx, pPacket->GetProtocolID() );
		return FALSE;
	}

	m_pStateLog[ i32Idx ].m_i32CompletedCount++;
	m_pStateLog[ i32Idx ].m_i32CompletedSize += i16Size;
	return TRUE;
}

BOOL CUserSessionManager::IsActive( INT32 i32Idx )
{
	if( i32Idx < 0 )							return FALSE;
	if( i32Idx >= m_MaxSessionIdx )				return FALSE;

	return ((CUserSession*)m_pSessionList[ i32Idx ])->GetIsActive();
}

BOOL CUserSessionManager::SendPacketMessage( INT32 i32Idx, i3NetworkPacket * pPacket )
{
	if( i32Idx < 0 )							return FALSE;
	if( i32Idx >= m_MaxSessionIdx )				return FALSE;

	INT32 i32SessionIdx = m_pi32ServerLink[ i32Idx ];
	return ((CUserSession*)m_pSessionList[ i32SessionIdx ])->SendPacketMessage( pPacket );
}

bool CUserSessionManager::SendAllPacketMessage( i3NetworkPacket * pPacket )
{
	bool bResult = true;
	CUserSession* pSession = nullptr;

	for( INT32 i = 0 ; i < m_MaxSessionIdx ; i++ )
	{
		pSession = static_cast<CUserSession*>(m_pSessionList[ i ]); 
		if( FALSE == pSession->GetIsActive() )	continue;
			
		if( FALSE == pSession->SendPacketMessage( pPacket ) )
		{	
			bResult = false;
		}
	}
	return bResult;
}
