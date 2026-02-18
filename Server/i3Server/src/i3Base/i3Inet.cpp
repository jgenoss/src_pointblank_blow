#include "i3CommonType.h"
#include "i3Inet.h"

I3_CLASS_INSTANCE( i3Udp, i3ElementBase);
I3_CLASS_INSTANCE( i3Tcp, i3ElementBase);

static bool		s_bInetInited = false;

//////////////////////////////////////////////////////////////////////////////////////////////////
//i3Net
BOOL i3Net::Init(void)
{
	return i3Net::Create(); 
}

BOOL i3Net::Create(void)
{
#if defined( I3_WINDOWS )// || defined( I3_WINCE )
	WSADATA wsaData;		
	if( ::WSAStartup( MAKEWORD(2,2), &wsaData ) != 0 )
	{
		I3TRACE("WSAStartup Failed. \n");
		return FALSE; 
	}

	if( wsaData.wVersion != MAKEWORD(2,2) )
	{
		::WSACleanup();
		I3TRACE("WSAStart Version Failed. \n");
		return FALSE; 
	}
#endif

	s_bInetInited = true;

	return TRUE; 
}

BOOL i3Net::GetMyAddress(char * pAdress)
{
#if defined( I3_WINDOWS )// || defined( I3_WINCE )
	struct	hostent *pHosten = NULL;
	char	szHostName[30];
	if( SOCKET_ERROR != gethostname(szHostName,30) )
	{
		pHosten = gethostbyname((const char FAR*)szHostName);
		if(pHosten)
		{	
			memcpy( pAdress ,pHosten->h_addr,4);
		}else return FALSE; 
	}else return FALSE; 
#endif
	return TRUE; 
}

void i3Net::DestroySocket( SOCKET hSocket )
{
	if( INVALID_SOCKET != hSocket )
	{
		::shutdown(hSocket, SD_BOTH);
		::closesocket(hSocket);
		hSocket = INVALID_SOCKET;
	}
}

char * i3Net::GetMyAddressList( INT32 * pOutCount)
{
#if defined( I3_WINDOWS )
	struct	hostent *pHosten = NULL;
	char	szHostName[30];
	if( SOCKET_ERROR != gethostname(szHostName,30) )
	{
		pHosten = gethostbyname((const char FAR*)szHostName);
		if( pHosten)
		{
			INT32 i = 0;
			INT32 count = 0;

			while( pHosten->h_addr_list[ count] != NULL)
				count++;

			char * pAddress = (char*) i3MemAlloc( sizeof(char) * 4 * count);
			char * pTemp = pAddress;

			while( pHosten->h_addr_list[ i] != NULL)
			{	
				memcpy( pTemp, pHosten->h_addr_list[i++], 4);
				pTemp += 4;
			}

			*pOutCount = count;

			return pAddress;
		}
	}
#endif

	return NULL;
}

void i3Net::GetName( INT32 Socket, SOCKADDR_IN * plocaladdr )
{
#if defined( I3_WINDOWS )// || defined( I3_WINCE )
	sockaddr	addr;
	INT32		len = sizeof( sockaddr );
	::getsockname( Socket, &addr, &len );
	i3mem::Copy( plocaladdr, &addr, sizeof( SOCKADDR_IN ) );	
#endif
}

//INT32 i3Net::GetMACAddress(char* pMAC,INT32 nNICCount)
//{
//	INT32 nAdapterCount = 0;
//
//#if defined( I3_WINDOWS )	
//	if( 16 < nNICCount) nNICCount = 16;
//
//	IP_ADAPTER_INFO AdapterInfo[16];	// NICs 16개까지	제한
//	PIP_ADAPTER_INFO pAdapter = NULL;
//	ULONG ulOutBufLen = sizeof(AdapterInfo);
//
//	if( ::GetAdaptersInfo(AdapterInfo, &ulOutBufLen) == ERROR_SUCCESS ) 
//	{
//		pAdapter = AdapterInfo;
//		while(pAdapter && nAdapterCount < nNICCount)
//		{
//			nAdapterCount++;
//
//			i3mem::Copy(pMAC, pAdapter->Address, sizeof(char) * MAX_ADAPTER_ADDRESS_LENGTH);
//			
//			pAdapter = pAdapter->Next;
//			pMAC += sizeof(char) * MAX_ADAPTER_ADDRESS_LENGTH;
//		}
//	}
//#endif
//
//	return nAdapterCount;	// 얻어낸 MAC address 수
//}

INT32 i3Net::GetMACAddress(UINT64* pMAC,INT32 nNICCount)
{
	INT32 nAdapterCount = 0;

#if defined( I3_WINDOWS )
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

			i3mem::Copy(pMAC, pAdapter->Address, sizeof(UINT64)); // sizeof(char) * MAX_ADAPTER_ADDRESS_LENGTH
			
			pAdapter = pAdapter->Next;
			pMAC++;
		}
	}
#endif

	return nAdapterCount;	// 얻어낸 MAC address 수
}

void i3Net::Destroy(void)
{
#if defined( I3_WINDOWS )// || defined( I3_WINCE )
	::WSACleanup();
#endif
	return; 
}

INT32 i3Net::GetError(char * pString)
{
	INT32 ErrorId; 
#if defined( I3_WINDOWS )// || defined( I3_WINCE )
	ErrorId = WSAGetLastError();

#ifdef I3_DEBUG
	switch( ErrorId )
	{		
	case WSAEINTR				:	I3TRACE("Error %s %s\n", pString, "Interrupted function call. "); break; 
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
//	case WSAINVALIDPROCTABLE	:	I3TRACE("Error %s %s\n", pString, "Invalid procedure table from service provider. "); break; 
//	case WSAINVALIDPROVIDER		:	I3TRACE("Error %s %s\n", pString, "Invalid service provider version number. "); break; 
//	case WSAPROVIDERFAILEDINIT	:	I3TRACE("Error %s %s\n", pString, "Unable to initialize a service provider. "); break; 
//	case WSASYSCALLFAILURE		:	I3TRACE("Error %s %s\n", pString, "System call failure. "); break; 
	default 					:	I3TRACE("Error %s %s 0X%x \n", pString, "Unknown", ErrorId); break;
	}
#endif

#endif
	return ErrorId;
}

#if defined( I3_DEBUG)
void i3Net::DumpMACAddress(void)
{
	//INT64 mac_addr[16];
	//INT32 nGetCount = i3Net::GetMACAddress(mac_addr, 16);

	//for(INT32 i = 0; i < nGetCount; i++)
	//{
	//	I3PRINTF("NIC %2d: 0x%016lx\n", mac_addr[i]);
	//}
}
#endif

char* i3Net::GetIPToSockA( sockaddr_in* pSockAddr, char* strIP )
{
	return GetIPToLongA( pSockAddr->sin_addr.S_un.S_addr, strIP );	
}

wchar_t* i3Net::GetIPToSockW( sockaddr_in* pSockAddr, wchar_t* wstrIP )
{
	return GetIPToLongW( pSockAddr->sin_addr.S_un.S_addr, wstrIP );
}

char* i3Net::GetIPToLongA( UINT32 ui32IP, char* strIP )
{
	INT32 i32Count = 0;
	INT32 i32Count2;
	UINT32 ui32Temp;
	char strTemp[ 4 ];

	for( INT32 i = 0 ; i < 4 ; i++ )
	{
		ui32Temp = (ui32IP >> (8 * i)) & 0x000000FF;

		i32Count2 = 0;
		do
		{
			strTemp[ i32Count2 ] = (ui32Temp % 10) + L'0';
			ui32Temp /= 10;
			i32Count2++;
		} while( 0 < ui32Temp );
		
		do
		{
			i32Count2--;
			strIP[ i32Count ] = strTemp[ i32Count2 ];
			i32Count++;
		} while( 0 < i32Count2 );
		strIP[ i32Count ] = '.';
		i32Count++;
	}
	strIP[ i32Count-1 ] = L'\0';

	return strIP;
}

wchar_t* i3Net::GetIPToLongW( UINT32 ui32IP, wchar_t* wstrIP )
{
	INT32 i32Count = 0;
	INT32 i32Count2;
	UINT32 ui32Temp;
	wchar_t wstrTemp[ 4 ];

	for( INT32 i = 0 ; i < 4 ; i++ )
	{
		ui32Temp = (ui32IP >> (8 * i)) & 0x000000FF;

		i32Count2 = 0;
		do
		{
			wstrTemp[ i32Count2 ] = (ui32Temp % 10) + L'0';
			ui32Temp /= 10;
			i32Count2++;
		} while( 0 < ui32Temp );
		
		do
		{
			i32Count2--;
			wstrIP[ i32Count ] = wstrTemp[ i32Count2 ];
			i32Count++;
		} while( 0 < i32Count2 );
		wstrIP[ i32Count ] = '.';
		i32Count++;
	}
	wstrIP[ i32Count-1 ] = L'\0';

	return wstrIP;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//i3UDP
i3Udp::i3Udp()
{
	m_Socket = INVALID_SOCKET;
}

i3Udp::~i3Udp()
{
	if( m_Socket != INVALID_SOCKET)
	{
		DeleteSocket();
	}
}

SOCKET i3Udp::CreateSocket(void)
{
	if( s_bInetInited == false)i3Net::Init();

#ifdef I3_WINDOWS
	#ifdef I3_DEBUG
	if(m_Socket != INVALID_SOCKET)	I3ASSERT_0; 	
	#endif
	m_Socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if( INVALID_SOCKET == m_Socket )	return INVALID_SOCKET;

	if( m_Socket != INVALID_SOCKET)
	{
		int bufsz = 0xFFFF;

		::setsockopt( m_Socket, IPPROTO_UDP, SO_RCVBUF, (const char *) &bufsz, sizeof(int));
		::setsockopt( m_Socket, IPPROTO_UDP, SO_SNDBUF, (const char *) &bufsz, sizeof(int));
	}
#endif

	return m_Socket; 
}

void i3Udp::DeleteSocket(void)
{
#ifdef I3_WINDOWS
	::shutdown( m_Socket, 0); 
	::closesocket( m_Socket ); 
	m_Socket = INVALID_SOCKET; 
#endif
	return; 
}

void i3Udp::Bind(UINT32 Ip, UINT16 Port)
{
#ifdef I3_WINDOWS
	struct sockaddr_in serveraddr; 
	memset(&serveraddr, 0, sizeof(serveraddr));

	serveraddr.sin_family		= AF_INET;
	serveraddr.sin_addr.s_addr	= Ip; 
	serveraddr.sin_port			= htons(Port);

	if( bind(m_Socket, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		i3Net::GetError("[i3Udp::Bind] Connect Failed");
	}

#endif 
	return; 
}

void i3Udp::BindAny(UINT16 Port)
{
#ifdef I3_WINDOWS
	struct sockaddr_in serveraddr; 
	
	memset(&serveraddr, 0, sizeof(serveraddr));

	serveraddr.sin_family		= AF_INET;
	serveraddr.sin_addr.s_addr	= htonl(INADDR_ANY); 
	serveraddr.sin_port			= htons(Port);
	
	if( bind(m_Socket, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		i3Net::GetError("[i3Udp::BindAny] Connect Failed");
	}
#endif
	return; 
}

void i3Udp::Send(const char * pBuffer, INT32 Byte, UINT32 Ip, UINT16 Port, INT32 flag)
{
#ifdef I3_WINDOWS
	SOCKADDR_IN cast; 
       
	cast.sin_family			= AF_INET;
	cast.sin_addr.s_addr	= Ip; 
	cast.sin_port			= htons(Port);

	if( sendto(m_Socket, pBuffer, Byte, flag, (const sockaddr *)&cast, sizeof(sockaddr) ) == SOCKET_ERROR )
	{
		i3Net::GetError("[i3Udp::Send] Connect Failed");
	}
#endif
	return; 
}

INT32 i3Udp::Recv(char * pBuffer, INT32 Byte, INT32 flags, I3SOCKETADDR * pFrom, INT32 * pFromlen)
{	
	INT32 Rv = -1; 
#ifdef I3_WINDOWS
	Rv = recvfrom(m_Socket, pBuffer, Byte, flags, pFrom, pFromlen);
	if(Rv < 0)i3Net::GetError("[i3Udp::Recv]");	
#endif
	return Rv; 
}

INT32 i3Udp::GetOption( INT32 nLevel, INT32 nOption, char * pOut, INT32 * pReturnSize )
{
	INT32	Rv = -1;
#if defined( I3_WINDOWS )
	Rv = ::getsockopt( m_Socket, nLevel, nOption, pOut, pReturnSize );
#endif
	return Rv;
}

INT32 i3Udp::SetOption( INT32 nLevel, INT32 nOption, const char * pszValue, INT32 nSize )
{
	INT32	Rv = -1;
#if defined( I3_WINDOWS )
	Rv = ::setsockopt( m_Socket, nLevel, nOption, pszValue, nSize );
#endif
	return Rv;
}

void i3Udp::GetName( I3SOCKETADDR * pLocal )
{	
#if defined( I3_WINDOWS )
	sockaddr	addr;
	INT32		len;

	len = sizeof( sockaddr );

	if( ::getsockname( m_Socket, &addr, &len ) < 0 )
	{
		i3Net::GetError( "[i3Udp::GetName] get socket name failed." );
	}

	i3mem::Copy( pLocal, &addr, sizeof( I3SOCKETADDR ) );
#endif
	return;
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//i3TCP
i3Tcp::i3Tcp()
{
	m_Socket = INVALID_SOCKET; 
	CreateSocket(); 
}

i3Tcp::~i3Tcp()
{
	if( m_Socket != INVALID_SOCKET)
	{
		DeleteSocket();
	}
}

SOCKET i3Tcp::CreateSocket(void)
{
#ifdef I3_WINDOWS
	#ifdef I3_DEBUG
	if(m_Socket != INVALID_SOCKET)I3ASSERT_0; 	
	#endif

	m_Socket = ::socket(AF_INET, SOCK_STREAM, 0); 
#endif
	return m_Socket; 
}

void i3Tcp::DeleteSocket(void) 
{
#ifdef I3_WINDOWS
	::shutdown( m_Socket, SD_BOTH); 
	::closesocket( m_Socket );
	m_Socket = INVALID_SOCKET; 
#endif
	return; 
}

BOOL i3Tcp::Connect(const char * pAddress, UINT16 PortNo)
{
#ifdef I3_WINDOWS
	#ifdef I3_DEBUG
	if(m_Socket == INVALID_SOCKET)I3ASSERT_0;
	#endif

	struct sockaddr_in serv_addr;
	i3mem::FillZero( &serv_addr, sizeof(serv_addr)); 
	serv_addr.sin_family		= AF_INET; 
	serv_addr.sin_addr.s_addr	= ::inet_addr(pAddress); 
	serv_addr.sin_port			= htons( PortNo ); 

	if( ::connect(m_Socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr) ) != 0)
	{
		i3Net::GetError("[i3Tcp::Connect] Connect Failed");
		return FALSE;
	}
#endif

	return TRUE; 
}

BOOL i3Tcp::Connect(UINT32 Address, UINT16 PortNo)
{
#ifdef I3_WINDOWS
	#ifdef I3_DEBUG
	if(m_Socket == INVALID_SOCKET)I3ASSERT_0;
	#endif

	struct sockaddr_in serv_addr;
	i3mem::FillZero( &serv_addr, sizeof(serv_addr)); 
	serv_addr.sin_family		= AF_INET; 
	serv_addr.sin_addr.s_addr	= Address; 
	serv_addr.sin_port			= htons( PortNo ); 

	if( ::connect(m_Socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr) ) != 0)
	{
		i3Net::GetError("[i3Tcp::Connect] Connect Failed");
		return FALSE;
	}
#endif

	return TRUE; 
}


INT32 i3Tcp::Send(const char * pBuffer, INT32 Byte, INT32 flags)
{
	INT32 i32Rv = -1;
#ifdef I3_WINDOWS
	i32Rv = ::send(m_Socket, pBuffer, Byte, flags); 
#endif
	return i32Rv; 
}

INT32 i3Tcp::Recv(char * pBuffer, INT32 Byte, INT32 flags)
{
	INT32 Rv = -1; 
#ifdef I3_WINDOWS
	Rv = ::recv(m_Socket, pBuffer, Byte, flags); 
#endif
	return Rv; 
}

void i3Tcp::GetName( I3SOCKETADDR * pLocal )
{
#ifdef I3_WINDOWS
	sockaddr	addr;
	INT32	len = sizeof( sockaddr );

	if( ::getsockname( m_Socket, &addr, &len ) < 0 )
	{
		i3Net::GetError( "[i3Tcp::GetName] get socket name failed.\n" );
	}

	i3mem::Copy( pLocal, &addr, sizeof( sockaddr ) );
#endif
	return;
}