#include "stdafx.h"
#include "Memory.h"
#include "NetworkBase.h"
#include "TcpBase.h"
#include <crtdbg.h>

NSM_cTcpBase::NSM_cTcpBase()
{
	m_Socket = INVALID_SOCKET; 
	CreateSocket(); 
}

NSM_cTcpBase::~NSM_cTcpBase()
{
	if( m_Socket != INVALID_SOCKET)
	{
		DeleteSocket();
	}
}

int NSM_cTcpBase::CreateSocket(void)
{
	if(m_Socket != INVALID_SOCKET)_ASSERT(0); 	
	m_Socket = (int)::socket(AF_INET, SOCK_STREAM, 0); 
	return m_Socket; 
}

void NSM_cTcpBase::DeleteSocket(void) 
{
	::shutdown( m_Socket, SD_BOTH); 
	::closesocket( m_Socket );
	m_Socket = INVALID_SOCKET; 

	return; 
}

BOOL NSM_cTcpBase::Connect(const char * pAddress, UINT16 PortNo)
{
	if(m_Socket == INVALID_SOCKET)_ASSERT(0);

	struct sockaddr_in serv_addr;
	NSM_nsMemory::FillZero( &serv_addr, sizeof(serv_addr)); 
	serv_addr.sin_family		= AF_INET; 
	serv_addr.sin_addr.s_addr	= ::inet_addr(pAddress); 
	serv_addr.sin_port			= htons( PortNo ); 

	if( ::connect(m_Socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr) ) != 0)
	{
		NSM_nsNetworkBase::GetError("[NSM_cTcpBase::Connect] Connect Failed");
		return FALSE;
	}


	return TRUE; 
}

BOOL NSM_cTcpBase::Connect(unsigned int Address, UINT16 PortNo)
{
	if(m_Socket == INVALID_SOCKET)_ASSERT(0);

	struct sockaddr_in serv_addr;
	NSM_nsMemory::FillZero( &serv_addr, sizeof(serv_addr)); 
	serv_addr.sin_family		= AF_INET; 
	serv_addr.sin_addr.s_addr	= Address; 
	serv_addr.sin_port			= htons( PortNo ); 

	if( ::connect(m_Socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr) ) != 0)
	{
		NSM_nsNetworkBase::GetError("[NSM_cTcpBase::Connect] Connect Failed");
		return FALSE;
	}

	return TRUE; 
}


void NSM_cTcpBase::Send(const char * pBuffer, int Byte, int flags)
{
	::send(m_Socket, pBuffer, Byte, flags); 
	return; 
}

int NSM_cTcpBase::Recv(char * pBuffer, int Byte, int flags)
{
	int Rv; 
	Rv = ::recv(m_Socket, pBuffer, Byte, flags); 
	return Rv; 
}

void NSM_cTcpBase::GetName( _SOCKADDR * pLocal )
{
	sockaddr addr;
	int	len = sizeof( sockaddr );

	if( ::getsockname( m_Socket, &addr, &len ) < 0 )
	{
		NSM_nsNetworkBase::GetError( "[NSM_cTcpBase::GetName] get socket name failed.\n" );
	}

	NSM_nsMemory::Copy( pLocal, &addr, sizeof( sockaddr ) );
	return;
}


