#ifndef __NETWORK_BASE_H
#define __NETWORK_BASE_H

typedef struct sockaddr		_SOCKADDR;	//

#include "NSMHeader.h"

namespace NSM_nsNetworkBase
{
	EXPORT_BASE bool Init(void);
	EXPORT_BASE void Destroy(void);			

	EXPORT_BASE bool GetMyAddress( char * pAdress );
	EXPORT_BASE void GetName( INT32 Socket, SOCKADDR_IN * plocaladdr ); 	
	EXPORT_BASE INT32 GetMACAddress(UINT64 * pMAC,INT32 nNICCount = 1);
	EXPORT_BASE INT32 GetError( char * pString );

	EXPORT_BASE SOCKET	CreateListenTCPSocket(UINT32 ui32IP, UINT16 ui16Port, INT32 i32RcvBufSize = 0);
	EXPORT_BASE SOCKET	AcceptTCPConnect(SOCKET ListenSocket, struct timeval * ptimeout, struct sockaddr_in * pAddr); 
	EXPORT_BASE void	DestroyTCPSocket(SOCKET iSocket);
}; 

#endif
