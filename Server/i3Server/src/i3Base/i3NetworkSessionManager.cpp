#include "i3CommonType.h"
#include "i3NetworkSession.h"
#include "i3NetworkSessionManager.h"

I3_CLASS_INSTANCE(i3NetworkSessionManager, i3ElementBase); 

i3NetworkSessionManager::i3NetworkSessionManager()
{
	m_NewSessionIdx				= 0;
	m_ActiveSessionCount		= 0;
	m_pSessionList				= NULL; 
	m_pui32WorkerThreadIdList	= NULL;

	m_MaxSessionIdx = 0;
}

i3NetworkSessionManager::~i3NetworkSessionManager()
{
	
}

BOOL i3NetworkSessionManager::OnCreate(i3List * pSessionList)
{
	m_MaxSessionIdx = pSessionList->GetCount(); 
	m_pSessionList = (i3NetworkSession**)i3MemAlloc( sizeof(i3NetworkSession * ) * m_MaxSessionIdx );

	for(INT32 i = 0; i < m_MaxSessionIdx; i++)
	{
		m_pSessionList[i] = (i3NetworkSession * )pSessionList->GetItem(i); 
		m_pSessionList[i]->m_SessionIdx = i;
	}

	return TRUE;
}

BOOL i3NetworkSessionManager::OnDestroy(void)
{
	I3MEM_SAFE_FREE( m_pSessionList );
	I3MEM_SAFE_FREE( m_pui32WorkerThreadIdList );
	return TRUE; 
}

void i3NetworkSessionManager::OnUpdate(void)
{
	return; 
}

ULONG_PTR i3NetworkSessionManager::ConnectSession( SOCKET hSocket, struct sockaddr_in * pAddr )
{
	i3NetworkSession * pNewSession = NULL; 
	m_SessionMutex.Enter(); 
	{
		pNewSession = FindNewSession();
		if(pNewSession != NULL)
		{
			if( pNewSession->OnConnect( hSocket, pAddr ) )
			{
				m_ActiveSessionCount++; 
			}
		}
	}
	m_SessionMutex.Leave();
	return (ULONG_PTR)pNewSession; 
}

void i3NetworkSessionManager::DisConnectSession(i3NetworkSession * pSession, BOOL bForceMain)
{
	m_SessionMutex.Enter(); 
	{
		if(pSession->GetIsActive()) 
		{
			if( pSession->OnDisconnect(bForceMain) )
			{
				m_ActiveSessionCount--;
			}
		}
	} 
	m_SessionMutex.Leave(); 
	return; 
}

i3NetworkSession * i3NetworkSessionManager::FindNewSession(void)
{
	i3NetworkSession * pNewSession = NULL;
	for(INT32 i = 0; i < m_MaxSessionIdx; i++)
	{
		pNewSession = m_pSessionList[m_NewSessionIdx]; 
		if(!pNewSession->GetIsActive())
		{
			m_NewSessionIdx++; 
			if(m_NewSessionIdx == m_MaxSessionIdx)m_NewSessionIdx = 0; 
			return pNewSession;
		}

		m_NewSessionIdx++; 
		if(m_NewSessionIdx == m_MaxSessionIdx)m_NewSessionIdx = 0; 
	}
	
	return NULL; 
}

void i3NetworkSessionManager::DisConnectAllSession(void)
{
	for(INT32 i = 0; i < m_MaxSessionIdx; i++)
	{
		DisConnectSession( m_pSessionList[i], TRUE); 
	}
	return; 
}