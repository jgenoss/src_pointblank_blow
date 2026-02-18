#ifndef __I3NETWORKSERVER_H__
#define __I3NETWORKSERVER_H__

class i3NetworkAcceptor; 
class i3NetworkCompletionHandler; 
class i3NetworkSessionManager; 

class I3_EXPORT_NETWORK i3NetworkServerContext : public i3ElementBase
{
	I3_CLASS_DEFINE( i3NetworkServerContext ); 
protected: 
	i3NetworkAcceptor			* m_pAcceptor;				// 郴何俊辑 积己 
	i3NetworkCompletionHandler	* m_pCompletionHandler;		// 郴何俊辑 积己 
	i3NetworkSessionManager		* m_pSessionManager;		// 寇何俊辑 积己 

public :
	i3NetworkServerContext(void);
	virtual ~i3NetworkServerContext(void); 
	
	virtual BOOL OnCreate(UINT8 SocketCount, UINT32 * pAddress, UINT16 * pPort, UINT8 * pTimeOut, INT32 WorkCount, i3NetworkSessionManager * pSessionManager);
	virtual void OnUpdate(INT32 Command)		{;}
	virtual BOOL OnDestroy(void); 

	//Check Server 
	INT32 GetSessionCount(void);

}; 
#endif
