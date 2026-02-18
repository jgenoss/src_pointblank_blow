#ifndef __NETWORK_UDP_BASE_H
#define __NETWORK_UDP_BASE_H

#include <ws2tcpip.h>					//
#include <Iphlpapi.h>					// MAC adress ¾ò±â

typedef struct sockaddr		_SOCKADDR;	//

class EXPORT_BASE NSM_cUdpBase
{	
protected: 
	int	m_Socket;

public:
	NSM_cUdpBase(); 
	virtual ~NSM_cUdpBase(); 

	INT32	CreateSocket(void); 
	void	DeleteSocket(void);

	INT32	BindAny(UINT16 Port);
	INT32	Bind(UINT32 Ip, UINT16 Port); 
	INT32	Send(const char * pBuffer, INT32 Byte, UINT32 Ip, UINT16 Port, INT32 flag = 0);
	INT32	Recv(char * pBuffer, INT32 Byte, INT32 flags = 0, _SOCKADDR * pFrom = NULL, INT32 * pFromlen = NULL); 

	INT32	GetOption( INT32 nlevel, INT32 nOption, char * pOut, INT32 * pReturnSize );
	INT32	SetOption( INT32 nlevel, INT32 nOption, const char * pszValue, INT32 nSize );
	void	GetName( _SOCKADDR * pLocal );

	INT32	GetSocketHandle( void )				{ return m_Socket; }
}; 

#endif