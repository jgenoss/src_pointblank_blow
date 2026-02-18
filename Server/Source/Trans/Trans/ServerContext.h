#ifndef __SERVER_CONTEXT_H__
#define __SERVER_CONTEXT_H__

class CServerContext : public i3NetworkServerContext
{
	I3_CLASS_DEFINE( CServerContext ); 

protected:

	i3List					m_UserList; 
public :

	CServerContext(void);
	virtual ~CServerContext(void); 

	virtual BOOL			OnCreate( UINT8 ui8SocketCount, UINT32* pui32Address, UINT16* pui16Port, UINT8* pui8TimeOut, INT32 i32WorkCount, i3NetworkSessionManager* pSessionManager );
	virtual void			OnUpdate( INT32 i32Command ); 
	virtual BOOL			OnDestroy(); 

	//Get User List
	i3List*					GetUserList(void)						{ return &m_UserList; }
}; 

#endif
