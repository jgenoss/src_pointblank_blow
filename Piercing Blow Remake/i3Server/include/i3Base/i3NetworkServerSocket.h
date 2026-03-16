#ifndef __I3NETWORKSERVERSOCKET_H
#define __I3NETWORKSERVERSOCKET_H

#include <Winsock2.h>
#include "i3ElementBase.h"
#include "i3MetaTemplate.h"

class I3_EXPORT_BASE i3NetworkServerSocket : public i3ElementBase
{	
	I3_CLASS_DEFINE(i3NetworkServerSocket); 
private:
	SOCKET m_ListenSock;

protected: 
	
public:
	i3NetworkServerSocket(); 
	virtual ~i3NetworkServerSocket(); 
	
	BOOL	OnCreate( UINT32 IpNo, UINT16 portNo, int backlogCount = SOMAXCONN );
	void	OnDestroy(void);

	SOCKET	AcceptConnection( struct timeval * ptimeout, struct sockaddr_in * pAddr = NULL, INT32 * pSize = NULL); 
	SOCKET	getListenSocket(void)		{ return m_ListenSock; }
	
};

#endif