#ifndef __PB_SERVER_H__
#define __PB_SERVER_H__

class CRoomManager; 

class CServerContext : public i3NetworkServerContext
{
	I3_CLASS_DEFINE( CServerContext ); 

protected: 		
	CRoomManager		*	m_pRoomManager; 
	i3List					m_UserList; 
public :

	CServerContext(void);
	virtual ~CServerContext(void); 
	virtual BOOL OnCreate(UINT8 SocketCount, UINT32 * pAddress, UINT16 * pPort, UINT8 * pTimeOut, INT32 WorkCount, i3NetworkSessionManager * pSessionManager);
	virtual void OnUpdate(INT32 Command); 
	virtual BOOL OnDestroy(void); 

	//Get User List
	i3List * GetUserList(void)	{ return &m_UserList; }

}; 

extern CServerContext * g_pServerContext; 

#endif
