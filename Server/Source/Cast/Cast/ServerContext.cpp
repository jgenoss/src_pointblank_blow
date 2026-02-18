#include "pch.h"
#include "ServerContext.h"
#include "UserManager.h"
#include "UserSession.h"

I3_CLASS_INSTANCE( CServerContext, i3NetworkServerContext );

CServerContext::CServerContext()
{	
}

CServerContext::~CServerContext()
{
	OnDestroy(); 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
BOOL CServerContext::OnCreate( UINT8 ui8SocketCount, UINT32* pui32Address, UINT16* pui16Port, UINT8* pui8TimeOut, INT32 i32WorkCount, i3NetworkSessionManager* pSessionManager )
{	
	BOOL bRv;

	//Create Server
	bRv = i3NetworkServerContext::OnCreate( ui8SocketCount, pui32Address, pui16Port, pui8TimeOut, i32WorkCount, pSessionManager );

	return bRv; 
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
void CServerContext::OnUpdate( INT32 i32Command )
{	
}
