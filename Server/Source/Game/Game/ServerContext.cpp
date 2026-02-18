#include "pch.h"
#include "ServerContext.h"
#include "RoomManager.h"
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
	for(INT32 i = 0; i < g_pContextMain->m_i32SessionCount; i++)
	{
		pUserSession = CUserSession::NewObject();
		if( NULL == pUserSession )				return FALSE;
		if( FALSE == pUserSession->Create() )	return FALSE;
		m_UserList.Add(pUserSession);
	}	

	//Create User Session
	g_pUserManager = CUserSessionManager::NewObject(); 
	g_pUserManager->OnCreate(&m_UserList); 	

	//Room Manager 
	m_pRoomManager = CRoomManager::NewObject(); 
	m_pRoomManager->OnCreate(); 
	g_pRoomManager = m_pRoomManager; 

	//Create Server
	Rv = i3NetworkServerContext::OnCreate(SocketCount, pAddress, pPort, pTimeOut, WorkCount, g_pUserManager);

	UINT32	* pui32WorkerThreadIdList = g_pUserManager->GetWorkerThreadIdList();
	wchar_t wstrLog[ MAX_PATH ];

	if( pui32WorkerThreadIdList != NULL)
	{
		for( INT32 i = 0 ; i < WorkCount; i++ )
		{
			i3String::Format( wstrLog, MAX_PATH, L"WORKIOCP%02d", i );
			 g_pLog->AddLogThread( wstrLog, 0, pui32WorkerThreadIdList[i] );
		}
	}

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
	
	I3_SAFE_RELEASE( m_pRoomManager ); 	
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
INT32 CServerContext::GetRoomCount(void)
{	
	return m_pRoomManager->GetUseRoomCount(); 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CServerContext::OnUpdate(INT32 Command)
{	
	if( Command & SERVER_UPDATE_ROOM	)	m_pRoomManager->OnUpdate();
	if( Command & SERVER_UPDATE_PLAYER	)	m_pSessionManager->OnUpdate();
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CServerContext::PlayerDump(void)
{	
	return; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CServerContext::RoomDump(void)
{
	m_pRoomManager->Dump(); 
	return; 
}
