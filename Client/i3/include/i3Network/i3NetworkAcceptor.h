#ifndef __I3NETWORKACCEPTOR_H
#define __I3NETWORKACCEPTOR_H

class i3NetworkCompletionHandler; 
class i3NetworkServerSocket; 
class i3NetworkSessionManager; 

/*2007. 11. 01 멀티로 변경한다. 

*/
class I3_EXPORT_NETWORK i3NetworkAcceptor : public i3Thread
{
	I3_EXPORT_CLASS_DEFINE(i3NetworkAcceptor, i3Thread); 
private:	
	bool							m_IsRunning = false;
	i3NetworkCompletionHandler	*	m_pIOCP = nullptr;

protected: 	
	i3NetworkSessionManager		*	m_pSessionManager = nullptr;

	UINT8							m_SocketCount = 0;
	UINT8						*	m_pTimeOut = nullptr;
	i3NetworkServerSocket		**	m_ppServerSock = nullptr;

public:
	virtual ~i3NetworkAcceptor(); 

	bool OnCreate(UINT8 SocketCount, UINT32 * pIpAddress, UINT16 * pPortNo, UINT8 * pTimeOut, i3NetworkSessionManager * pSessionManager, i3NetworkCompletionHandler * pIOCP);
	
	virtual UINT32	OnRunning( void * pUserData) override;
};

#endif