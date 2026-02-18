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

	virtual BOOL OnCreate(UINT8 ui8SocketCount, UINT32 * pui32Address, UINT16 * pui16Port, UINT8 * pui8TimeOut, INT32 i32WorkCount, i3NetworkSessionManager * pSessionManager);
	virtual void OnUpdate(INT32 i32Command); 
	virtual BOOL OnDestroy(void); 

	//Check Room Count 
	INT32 GetRoomCount(void);

	//Get User List
	i3List * GetUserList(void)	{ return &m_UserList; }

	//Dump 
	void PlayerDump(void); 
	void RoomDump(void); 
}; 

extern CServerContext * g_pServerContext; 

#endif
