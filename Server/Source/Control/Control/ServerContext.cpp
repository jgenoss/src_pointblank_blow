#include "pch.h"
#include "ServerContext.h"
#include "UserManager.h"
#include "UserSession.h"

I3_CLASS_INSTANCE(CServerContext, i3NetworkServerContext); 

CServerContext * g_pServerContext = NULL; 

CServerContext::CServerContext()
{	
	m_pRoomManager		= NULL; 
}

CServerContext::~CServerContext()
{
	OnDestroy(); 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
BOOL CServerContext::OnCreate(UINT8 SocketCount, UINT32 * pAddress, UINT16 * pPort, UINT8 * pTimeOut, INT32 WorkCount, i3NetworkSessionManager * pSessionManager)
{	
	BOOL Rv; 

	//Create Session 
	CUserSession * pUserSession; 
	for(INT32 i = 0; i < g_pControl->m_UserCount; i++)
	{
		pUserSession = CUserSession::NewObject();
		m_UserList.Add(pUserSession);
	}

	//Create User Session
	g_pUserManager = CUserSessionManager::NewObject(); 
	g_pUserManager->OnCreate(&m_UserList); 	

	//Create Server
	Rv = i3NetworkServerContext::OnCreate(SocketCount, pAddress, pPort, pTimeOut, WorkCount, g_pUserManager);

	return Rv; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
BOOL CServerContext::OnDestroy(void)
{
	I3_SAFE_RELEASE( m_pSessionManager );  // g_pUserManager == m_pSessionManager; 먼저 삭제해 주어야 한다.
	BOOL Rv = i3NetworkServerContext::OnDestroy(); 	
	
	CUserSession * pUserSession; 
	for(INT32 i = 0; i < m_UserList.GetCount(); i++)
	{
		pUserSession = (CUserSession*) m_UserList.GetItem(i); 
		I3_SAFE_RELEASE( pUserSession ); 
	}

	return Rv; 
}


/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CServerContext::OnUpdate(INT32 Command)
{	
	m_pSessionManager->OnUpdate();
}
