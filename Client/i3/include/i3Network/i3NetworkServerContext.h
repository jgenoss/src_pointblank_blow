#ifndef __I3NETWORKSERVER_H__
#define __I3NETWORKSERVER_H__

class i3NetworkAcceptor; 
class i3NetworkCompletionHandler; 
class i3NetworkSessionManager; 

class I3_EXPORT_NETWORK i3NetworkServerContext : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3NetworkServerContext, i3ElementBase ); 
protected: 
	i3NetworkAcceptor			* m_pAcceptor = nullptr;				// 郴何俊辑 积己 
	i3NetworkCompletionHandler	* m_pCompletionHandler = nullptr;		// 郴何俊辑 积己 
	i3NetworkSessionManager		* m_pSessionManager = nullptr;		// 寇何俊辑 积己 

public :
	virtual ~i3NetworkServerContext(void); 
	
	virtual bool OnCreate(UINT8 SocketCount, UINT32 * pAddress, UINT16 * pPort, UINT8 * pTimeOut, INT32 WorkCount, i3NetworkSessionManager * pSessionManager);
	virtual void OnUpdate(INT32 Command) {}
	virtual bool OnDestroy(void);

	//Check Server 
	INT32 GetSessionCount(void);

}; 
#endif
