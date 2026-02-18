#include "stdafx.h"
#include "NetworkBase.h"
#include "Memory.h"
#include <Iphlpapi.h>

bool NSM_nsNetworkBase::Init(void)
{
	WSADATA wsaData;		
	if( ::WSAStartup( MAKEWORD(2,2), &wsaData ) != 0 )
	{
		NSM_nsNetworkBase::Destroy(); 		
		return false; 
	}

	if( wsaData.wVersion != MAKEWORD(2,2) )
	{
		NSM_nsNetworkBase::Destroy(); 
		return false; 
	}

	return true; 
}

void NSM_nsNetworkBase::Destroy(void)
{
	::WSACleanup();
	return; 
}

bool NSM_nsNetworkBase::GetMyAddress(char * pAdress)
{
	struct	hostent *pHosten = NULL;
	char	szHostName[30];
	if( SOCKET_ERROR != gethostname(szHostName,30) )
	{
		pHosten = gethostbyname((const char FAR*)szHostName);
		if(pHosten)
		{
			memcpy( pAdress ,pHosten->h_addr,4);
		}else return false; 
	}else return false; 

	return TRUE; 
}

void NSM_nsNetworkBase::GetName( INT32 Socket, SOCKADDR_IN * plocaladdr )
{
	sockaddr	addr;
	INT32		len = sizeof( sockaddr );
	::getsockname( Socket, &addr, &len );
	NSM_nsMemory::Copy( plocaladdr, &addr, sizeof( SOCKADDR_IN ) );

	return; 
}

INT32 NSM_nsNetworkBase::GetMACAddress(UINT64* pMAC, INT32 nNICCount)
{
	INT32 nAdapterCount = 0;

	if( 16 < nNICCount) nNICCount = 16;

	IP_ADAPTER_INFO AdapterInfo[16];	// NICs 16개까지	제한
	PIP_ADAPTER_INFO pAdapter = NULL;
	ULONG ulOutBufLen = sizeof(AdapterInfo);

	if( ::GetAdaptersInfo(AdapterInfo, &ulOutBufLen) == ERROR_SUCCESS ) 
	{
		pAdapter = AdapterInfo;
		while(pAdapter && nAdapterCount < nNICCount)
		{
			nAdapterCount++;

			NSM_nsMemory::Copy(pMAC, pAdapter->Address, sizeof(UINT64));
			
			pAdapter = pAdapter->Next;
			pMAC++;
		}
	}

	return nAdapterCount;	// 얻어낸 MAC address 수
}

SOCKET NSM_nsNetworkBase::CreateListenTCPSocket(UINT32 ui32IP, UINT16 ui16Port, INT32 i32RcvBufSize)
{
	SOCKET RvSocket = INVALID_SOCKET; 

	struct sockaddr_in sockAddr;

	NSM_nsMemory::FillZero( &sockAddr, sizeof( sockAddr ) );
	sockAddr.sin_family			= AF_INET;
	sockAddr.sin_addr.s_addr	= ui32IP;
	sockAddr.sin_port			= ::htons( ui16Port );

	//Set Socket 
	RvSocket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );	
	if(RvSocket == INVALID_SOCKET)return RvSocket; 

	//Sets a socket option
	INT32 reuse = 1;		
	::setsockopt( RvSocket, SOL_SOCKET, SO_REUSEADDR, ( char* )&reuse, sizeof( reuse ) );

	//Recieve Buffer 크기를 지정한 경우 버퍼크기를 변경
	if( 0 < i32RcvBufSize )
	{
		if( SOCKET_ERROR  == setsockopt( RvSocket, SOL_SOCKET, SO_RCVBUF, (char*)&i32RcvBufSize, sizeof(INT32) ) )	return false;
	}

	//Controls the I/O mode of a socket 비동기식으로 설정 
	unsigned long argp = 1;
	::ioctlsocket( RvSocket, FIONBIO, &argp );

	//Bind Socket 
	if( ::bind( RvSocket, ( struct sockaddr * )&sockAddr, sizeof( sockAddr ) ) == SOCKET_ERROR)
	{			
		::shutdown(RvSocket, SD_BOTH);
		::closesocket(RvSocket);
		RvSocket = INVALID_SOCKET;
		return RvSocket; 
	}

	//Listen Socket
	if(	::listen( RvSocket, 5 ) == SOCKET_ERROR ) 
	{		
		return RvSocket; 
	}

	return RvSocket; 
}

SOCKET NSM_nsNetworkBase::AcceptTCPConnect(SOCKET ListenSocket, struct timeval * ptimeout, struct sockaddr_in * pAddr)
{
	SOCKET AcceptSocket = INVALID_SOCKET;
	
	fd_set fds; 
	FD_ZERO(&fds); 
	FD_SET( ListenSocket, &fds );

	if(::select( 0, &fds, NULL, NULL, ptimeout) == SOCKET_ERROR ) 
	{
		return AcceptSocket; 
	}

	if( FD_ISSET( ListenSocket, &fds ) )
	{
		INT32 AddrSize = sizeof(sockaddr_in); 
		AcceptSocket = ::accept( ListenSocket, (struct sockaddr *)pAddr, &AddrSize);
	}

	return AcceptSocket; 
}

void NSM_nsNetworkBase::DestroyTCPSocket(SOCKET hSocket)
{
	if (INVALID_SOCKET != hSocket)
	{
		::shutdown(hSocket, SD_BOTH);
		::closesocket(hSocket);
		hSocket = INVALID_SOCKET;
	}
}

INT32 NSM_nsNetworkBase::GetError(char * pString)
{
	INT32 ErrorId; 

	ErrorId = WSAGetLastError();

	/*
	switch( ErrorId )
	{		
	case WSAEINT32R				:	I3TRACE("Error %s %s\n", pString, "INT32errupted function call. "); break; 
	case WSAEACCES				:	I3TRACE("Error %s %s\n", pString, "Permission denied. "); break; 
	case WSAEFAULT				:	I3TRACE("Error %s %s\n", pString, "Bad address. "); break; 
	case WSAEINVAL				:	I3TRACE("Error %s %s\n", pString, "Invalid argument. "); break; 
	case WSAEMFILE				:	I3TRACE("Error %s %s\n", pString, "Too many open files. "); break; 
	case WSAEWOULDBLOCK			:	I3TRACE("Error %s %s\n", pString, "Resource temporarily unavailable. "); break; 
	case WSAEINPROGRESS			:	I3TRACE("Error %s %s\n", pString, "Operation now in progress. "); break; 
	case WSAEALREADY			:	I3TRACE("Error %s %s\n", pString, "Operation already in progress. "); break; 
	case WSAENOTSOCK			:	I3TRACE("Error %s %s\n", pString, "Socket operation on nonsocket. "); break; 
	case WSAEDESTADDRREQ		:	I3TRACE("Error %s %s\n", pString, "Destination address required. "); break; 
	case WSAEMSGSIZE			:	I3TRACE("Error %s %s\n", pString, "Message too long. "); break; 
	case WSAEPROTOTYPE			:	I3TRACE("Error %s %s\n", pString, "Protocol wrong type for socket. "); break; 
	case WSAENOPROTOOPT			:	I3TRACE("Error %s %s\n", pString, "Bad protocol option. "); break; 
	case WSAEPROTONOSUPPORT		:	I3TRACE("Error %s %s\n", pString, "Protocol not supported. "); break; 
	case WSAESOCKTNOSUPPORT		:	I3TRACE("Error %s %s\n", pString, "Socket type not supported. "); break; 
	case WSAEOPNOTSUPP			:	I3TRACE("Error %s %s\n", pString, "Operation not supported. "); break; 
	case WSAEPFNOSUPPORT		:	I3TRACE("Error %s %s\n", pString, "Protocol family not supported. "); break; 
	case WSAEAFNOSUPPORT		:	I3TRACE("Error %s %s\n", pString, "Address family not supported by protocol family. "); break; 
	case WSAEADDRINUSE			:	I3TRACE("Error %s %s\n", pString, "Address already in use. "); break; 
	case WSAEADDRNOTAVAIL		:	I3TRACE("Error %s %s\n", pString, "Cannot assign requested address. "); break; 
	case WSAENETDOWN			:	I3TRACE("Error %s %s\n", pString, "Network is down. "); break; 
	case WSAENETUNREACH			:	I3TRACE("Error %s %s\n", pString, "Network is unreachable. "); break; 
	case WSAENETRESET			:	I3TRACE("Error %s %s\n", pString, "Network dropped connection on reset. "); break; 
	case WSAECONNABORTED		:	I3TRACE("Error %s %s\n", pString, "Software caused connection abort. "); break; 
	case WSAECONNRESET			:	I3TRACE("Error %s %s\n", pString, "Connection reset by peer. "); break; 
	case WSAENOBUFS				:	I3TRACE("Error %s %s\n", pString, "No buffer space available. "); break; 
	case WSAEISCONN				:	I3TRACE("Error %s %s\n", pString, "Socket is already connected. "); break; 
	case WSAENOTCONN			:	I3TRACE("Error %s %s\n", pString, "Socket is not connected. "); break; 
	case WSAESHUTDOWN			:	I3TRACE("Error %s %s\n", pString, "Cannot send after socket shutdown. "); break; 
	case WSAETIMEDOUT			:	I3TRACE("Error %s %s\n", pString, "Connection timed out. "); break; 
	case WSAECONNREFUSED		:	I3TRACE("Error %s %s\n", pString, "Connection refused. "); break; 
	case WSAEHOSTDOWN			:	I3TRACE("Error %s %s\n", pString, "Host is down. "); break; 
	case WSAEHOSTUNREACH		:	I3TRACE("Error %s %s\n", pString, "No route to host. "); break; 
	case WSAEPROCLIM			:	I3TRACE("Error %s %s\n", pString, "Too many processes. "); break; 
	case WSASYSNOTREADY			:	I3TRACE("Error %s %s\n", pString, "Network subsystem is unavailable. "); break; 
	case WSAVERNOTSUPPORTED		:	I3TRACE("Error %s %s\n", pString, "Winsock.dll version out of range. "); break; 
	case WSANOTINITIALISED		:	I3TRACE("Error %s %s\n", pString, "Successful WSAStartup not yet performed. "); break; 
	case WSAEDISCON				:	I3TRACE("Error %s %s\n", pString, "Graceful shutdown in progress. "); break; 
	case WSATYPE_NOT_FOUND		:	I3TRACE("Error %s %s\n", pString, "Class type not found. "); break; 
	case WSAHOST_NOT_FOUND		:	I3TRACE("Error %s %s\n", pString, "Host not found. "); break; 
	case WSATRY_AGAIN			:	I3TRACE("Error %s %s\n", pString, "Nonauthoritative host not found. "); break; 
	case WSANO_RECOVERY			:	I3TRACE("Error %s %s\n", pString, "This is a nonrecoverable error. "); break; 
	case WSANO_DATA				:	I3TRACE("Error %s %s\n", pString, "Valid name, no data record of requested type. "); break; 
	case WSA_INVALID_HANDLE		:	I3TRACE("Error %s %s\n", pString, "Specified event object handle is invalid. "); break; 
	case WSA_INVALID_PARAMETER	:	I3TRACE("Error %s %s\n", pString, "One or more parameters are invalid. "); break; 
	case WSA_IO_INCOMPLETE		:	I3TRACE("Error %s %s\n", pString, "Overlapped I/O event object not in signaled state. "); break; 
	case WSA_IO_PENDING			:	I3TRACE("Error %s %s\n", pString, "Overlapped operations will complete later. "); break; 
	case WSA_NOT_ENOUGH_MEMORY	:	I3TRACE("Error %s %s\n", pString, "Insufficient memory available. "); break; 
	case WSA_OPERATION_ABORTED	:	I3TRACE("Error %s %s\n", pString, "Overlapped operation aborted. "); break; 
	default 					:	I3TRACE("Error %s %s 0X%x \n", pString, "Unknown", ErrorId); break;
	}
	*/

	return ErrorId;
}


