#ifndef __I3NETWORKSERVERSOCKET_H
#define __I3NETWORKSERVERSOCKET_H

#include <Winsock2.h>

class I3_EXPORT_NETWORK i3NetworkServerSocket : public i3ElementBase
{	
	I3_EXPORT_CLASS_DEFINE(i3NetworkServerSocket, i3ElementBase); 
private:
	SOCKET m_ListenSock = INVALID_SOCKET;
		
public:	
	bool	OnCreate( UINT32 IpNo, UINT16 portNo, int backlogCount = SOMAXCONN );
	void	OnDestroy(void);

	SOCKET	AcceptConnection( struct timeval * ptimeout, struct sockaddr_in * pAddr = nullptr, INT32 * pSize = nullptr); 
	SOCKET	getListenSocket(void)		{ return m_ListenSock; }
	
};

#endif