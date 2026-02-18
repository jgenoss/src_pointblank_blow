#ifndef __I3NETWORKACCEPTOR_H
#define __I3NETWORKACCEPTOR_H

class i3NetworkCompletionHandler; 
class i3NetworkServerSocket; 
class i3NetworkSessionManager; 

/*2007. 11. 01 멀티로 변경한다. 

*/
class I3_EXPORT_NETWORK i3NetworkAcceptor : public i3Thread
{
	I3_CLASS_DEFINE(i3NetworkAcceptor); 
private:	
	BOOL							m_IsRunning; 
	i3NetworkCompletionHandler	*	m_pIOCP; 

protected: 	
	i3NetworkSessionManager		*	m_pSessionManager;	

	UINT8							m_SocketCount;		//
	UINT8						*	m_pTimeOut;			//
	i3NetworkServerSocket		**	m_ppServerSock;		//
public: 	
	i3NetworkAcceptor(); 	
	virtual ~i3NetworkAcceptor(); 

	//BOOL OnCreate(char * pStrAddress, INT16 PortNo, i3NetworkSessionManager * m_pSessionManager, i3NetworkCompletionHandler * pIOCP);	
	BOOL OnCreate(UINT8 SocketCount, UINT32 * pIpAddress, UINT16 * pPortNo, UINT8 * pTimeOut, i3NetworkSessionManager * pSessionManager, i3NetworkCompletionHandler * pIOCP);

	BOOL OnDestroy(void);

	virtual void	BeforeRunning( void * pUserData);
	virtual UINT32	OnRunning( void * pUserData);		
	virtual void	AfterRunning( void * pUserData);	
};

#endif