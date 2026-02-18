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
BOOL CServerContext::OnCreate(UINT8 ui8SocketCount, UINT32 * pui32Address, UINT16 * pui16Port, UINT8 * pui8TimeOut, INT32 i32WorkCount, i3NetworkSessionManager * pSessionManager)
{	
	BOOL bRv; 

	//Create Session 
	CUserSession * pUserSession; 
	for(INT32 i = 0; i < g_pContextMain->m_i32SessionCount; i++)
	{
		pUserSession = CUserSession::NewObject();
		if( NULL == pUserSession ) return FALSE;
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
	bRv = i3NetworkServerContext::OnCreate(ui8SocketCount, pui32Address, pui16Port, pui8TimeOut, i32WorkCount, g_pUserManager);

	return bRv; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
BOOL CServerContext::OnDestroy(void)
{
	I3_SAFE_RELEASE( m_pSessionManager );  // g_pUserManager == m_pSessionManager; 먼저 삭제해 주어야 한다.
	BOOL bRv = i3NetworkServerContext::OnDestroy(); 	
	
	I3_SAFE_RELEASE( m_pRoomManager ); 	
	CUserSession * pUserSession; 
	for(INT32 i = 0; i < m_UserList.GetCount(); i++)
	{
		pUserSession = (CUserSession*) m_UserList.GetItem(i); 
		I3_SAFE_RELEASE( pUserSession ); 
	}

	return bRv; 
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
void CServerContext::OnUpdate(INT32 i32Command)
{	
	if( i32Command & SERVER_UPDATE_ROOM	)	m_pRoomManager->OnUpdate();
	if( i32Command & SERVER_UPDATE_PLAYER )	m_pSessionManager->OnUpdate();
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
