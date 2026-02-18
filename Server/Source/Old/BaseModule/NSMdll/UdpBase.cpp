#include "stdafx.h"
#include "Memory.h"
#include "NetworkBase.h"
#include "UdpBase.h"
#include <crtdbg.h>

NSM_cUdpBase::NSM_cUdpBase()
{
	m_Socket = INVALID_SOCKET;
}

NSM_cUdpBase::~NSM_cUdpBase()
{
	if( m_Socket != INVALID_SOCKET)
	{
		DeleteSocket();
	}
}

INT32 NSM_cUdpBase::CreateSocket(void)
{
	if(m_Socket != INVALID_SOCKET)_ASSERT(0);
	m_Socket = (int)::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	return m_Socket; 
}

void NSM_cUdpBase::DeleteSocket(void)
{
	::shutdown( m_Socket, 0); 
	::closesocket( m_Socket ); 
	m_Socket = INVALID_SOCKET; 
	return; 
}

INT32 NSM_cUdpBase::Bind(UINT32 Ip, UINT16 Port)
{
	INT32 Rv; 
	struct sockaddr_in serveraddr; 
	memset(&serveraddr, 0, sizeof(serveraddr));

	serveraddr.sin_family		= AF_INET;
	serveraddr.sin_addr.s_addr	= Ip; 
	serveraddr.sin_port			= htons(Port);

	Rv = bind(m_Socket, (struct sockaddr *)&serveraddr, sizeof(serveraddr)); 

	return Rv; 
}

INT32 NSM_cUdpBase::BindAny(UINT16 Port)
{
	struct sockaddr_in serveraddr; 
	INT32 Rv; 
	
	memset(&serveraddr, 0, sizeof(serveraddr));

	serveraddr.sin_family		= AF_INET;
	serveraddr.sin_addr.s_addr	= htonl(INADDR_ANY); 
	serveraddr.sin_port			= htons(Port);
	
	Rv = bind( m_Socket, (struct sockaddr *)&serveraddr, sizeof(serveraddr) ); 

	return Rv; 
}

INT32 NSM_cUdpBase::Send(const char * pBuffer, INT32 Byte, UINT32 Ip, UINT16 Port, INT32 flag)
{
	SOCKADDR_IN cast; 
    INT32 Rv; 

	cast.sin_family			= AF_INET;
	cast.sin_addr.s_addr	= Ip; 
	cast.sin_port			= htons(Port);

	Rv = sendto( m_Socket, pBuffer, Byte, flag, (const sockaddr *)&cast, sizeof(sockaddr) ); 

	return Rv; 
}

INT32 NSM_cUdpBase::Recv(char * pBuffer, INT32 Byte, INT32 flags, _SOCKADDR * pFrom, INT32 * pFromlen)
{	
	INT32 Rv; 

	Rv = recvfrom(m_Socket, pBuffer, Byte, flags, pFrom, pFromlen);

	return Rv; 
}

INT32 NSM_cUdpBase::GetOption( INT32 nLevel, INT32 nOption, char * pOut, INT32 * pReturnSize )
{
	INT32 Rv = ::getsockopt( m_Socket, nLevel, nOption, pOut, pReturnSize );
	return Rv;
}

INT32 NSM_cUdpBase::SetOption( INT32 nLevel, INT32 nOption, const char * pszValue, INT32 nSize )
{
	INT32	Rv = ::setsockopt( m_Socket, nLevel, nOption, pszValue, nSize );
	return Rv;
}

void NSM_cUdpBase::GetName( _SOCKADDR * pLocal )
{
	sockaddr	addr;
	INT32		len;

	len = sizeof( sockaddr );

	if( ::getsockname( m_Socket, &addr, &len ) < 0 )
	{
		NSM_nsNetworkBase::GetError( "[NSM_cUdpBase::GetName] get socket name failed." );
	}

	NSM_nsMemory::Copy( pLocal, &addr, sizeof( _SOCKADDR ) );

	return;
}

