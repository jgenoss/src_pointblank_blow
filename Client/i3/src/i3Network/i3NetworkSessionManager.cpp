#include "i3NetworkDef.h"
#include "i3NetworkSession.h"
#include "i3NetworkSessionManager.h"

I3_CLASS_INSTANCE(i3NetworkSessionManager);

bool i3NetworkSessionManager::OnCreate(const i3::vector<i3NetworkSession*>& SessionList)
{
	m_MaxSessionIdx = (INT32)SessionList.size(); 
	m_pSessionList = (i3NetworkSession**)i3MemAlloc( sizeof(i3NetworkSession * ) * m_MaxSessionIdx );

	for(INT32 i = 0; i < m_MaxSessionIdx; i++)
	{
		m_pSessionList[i] = SessionList[i]; 
		m_pSessionList[i]->m_SessionIdx = i;
	}

	return true;
}

//
// i3List에서 동적 배열로 넘기는 방법..
//
// char **타입의 list.Items멤버변수를 reinterpret_cast<i3NetworkSession**>( )를 통해 첫 인수로 넘기고, list.GetCount()를 두번째 인수로 넣으면 될듯 싶다..
//

bool i3NetworkSessionManager::OnCreate(i3NetworkSession** ppArray, INT32 ElemSize)
{
	
	m_MaxSessionIdx = ElemSize;
	m_pSessionList = (i3NetworkSession**)i3MemAlloc( sizeof(i3NetworkSession * ) * m_MaxSessionIdx );

	for(INT32 i = 0; i < m_MaxSessionIdx; i++)
	{
		m_pSessionList[i] = ppArray[i]; 
		m_pSessionList[i]->m_SessionIdx = i;
	}

	return true;
	
}




bool i3NetworkSessionManager::OnDestroy(void)
{
	I3MEM_SAFE_FREE( m_pSessionList );
	return true; 
}

DWORD i3NetworkSessionManager::ConnectSession(SOCKET Socket, struct sockaddr_in * pAddr)
{
	i3NetworkSession * pNewSession = nullptr; 
	m_SessionMutex.Enter(); 
	{
		pNewSession = FindNewSession();
		if(pNewSession != nullptr)
		{
			if( pNewSession->OnCreate(Socket, pAddr) )
			{
				m_ActiveSessionCount++; 
			}
		}
	}
	m_SessionMutex.Leave();
	return (DWORD)pNewSession; 
}

void i3NetworkSessionManager::DisConnectSession(i3NetworkSession * pSession, bool bForceMain)
{
	m_SessionMutex.Enter(); 
	{
		if(pSession->GetIsActive()) 
		{
			if( pSession->OnDestroy(bForceMain) )
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
	i3NetworkSession * pNewSession = nullptr;
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
	
	return nullptr; 
}

void i3NetworkSessionManager::DisConnectAllSession(void)
{
	for(INT32 i = 0; i < m_MaxSessionIdx; i++)
	{
		DisConnectSession( m_pSessionList[i], true);
	}
	return; 
}