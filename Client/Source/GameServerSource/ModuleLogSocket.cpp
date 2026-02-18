#include "pch.h"
#include "ModuleLogSocket.h"

I3_CLASS_INSTANCE(CModuleLogSocket, i3ElementBase); 

CModuleLogSocket::CModuleLogSocket()
{
	m_Socket				= INVALID_SOCKET; 
	m_RecvEvent				= INVALID_HANDLE_VALUE; 
	m_i32ReceivedPacketSize	= 0;

	return; 
}

CModuleLogSocket::~CModuleLogSocket()
{
	OnDestroy(); 
	return; 
}

BOOL CModuleLogSocket::OnDestroy(void)
{
	if(!g_pContextMain->m_bLogDActive)return TRUE; //사용하지 않음 

	if(  m_RecvEvent != INVALID_HANDLE_VALUE )
	{
		::WSACloseEvent( m_RecvEvent );
		m_RecvEvent = INVALID_HANDLE_VALUE;
	}

	if(m_Socket != INVALID_SOCKET)
	{
		::shutdown( m_Socket, SD_BOTH); 
		::closesocket( m_Socket );
		m_Socket = INVALID_SOCKET; 
	}

	return TRUE; 
}

BOOL CModuleLogSocket::OnConnect(char * pAddress, UINT16 ui16PortNo)
{
	if(!g_pContextMain->m_bLogDActive)return TRUE; //사용하지 않음 

	m_RecvEvent	= ::WSACreateEvent();
	m_Socket	= ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( m_Socket == INVALID_SOCKET )
	{		
		I3TRACE("[CModuleLogSocket::OnConnectTo] Error Socket Is Invalid \n"); 
		return FALSE; 
	}

	m_i32ReceivedPacketSize	= 0; 

	struct sockaddr_in serv_addr;
	i3mem::FillZero( &serv_addr, sizeof(serv_addr)); 
	serv_addr.sin_family		= AF_INET; 
	serv_addr.sin_addr.s_addr	= ::inet_addr(pAddress); 
	serv_addr.sin_port			= htons( ui16PortNo ); 

	if( ::connect(m_Socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr) ) != 0)
	{		
		i3Net::GetError("[CModuleLogSocket::OnConnectTo]"); 
		return FALSE; 
	}
	return TRUE; 
}

//무언가를 받았을때 TRUE 를 리턴해 줘요.
BOOL CModuleLogSocket::SelectEvent(void)
{
	WSANETWORKEVENTS nEvent; 
	i3mem::FillZero( &nEvent, sizeof( nEvent ) ); 

	if( 0 == ::WSAEventSelect( m_Socket, m_RecvEvent, FD_WRITE | FD_READ | FD_CLOSE ) )
	{	
		if( 0 == ::WSAEnumNetworkEvents( m_Socket, m_RecvEvent, &nEvent ))
		{	
			if( (nEvent.lNetworkEvents & FD_READ )	== FD_READ)
			{
				OnReceive();
				return TRUE; 
			}

			if( (nEvent.lNetworkEvents & FD_CLOSE ) == FD_CLOSE)
			{		
				OnDestroy();
				return FALSE; 
			}
		}
	}
		
	//의미 없음..
	return FALSE; 
}

void CModuleLogSocket::OnReceive(void)
{	
	INT32 i32receivedByte = recv(m_Socket, &m_ReceiveBuffer[0], PACKETBUFFERSIZE, 0);

	if (SOCKET_ERROR == i32receivedByte)
	{
		DWORD errorCode = GetLastError();

		char strmessage[MAX_STRING_COUNT];
		i3String::Format(strmessage, MAX_STRING_COUNT, "[ERROR] CModuleLogSocket::OnReceive() - errorCode(%d)", errorCode);
		g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL, strmessage);

		return;
	}

	m_i32ReceivedPacketSize = i32receivedByte; 

	if(m_i32ReceivedPacketSize >= PACKETBUFFERSIZE) 
	{
		return; 
	}
	
	INT32 i32StartPos = 0; 
	INT32 i32ReadSize; 
	while( m_i32ReceivedPacketSize > 0 )
	{
		i32ReadSize = PacketParsing( &m_ReceiveBuffer[i32StartPos] );
		m_i32ReceivedPacketSize -= i32ReadSize;
		i32StartPos += i32ReadSize;
	}

	return; 
}

INT32 CModuleLogSocket::PacketParsing(char * pBuffer )
{
	return PACKETBUFFERSIZE; 
}		 

INT32 CModuleLogSocket::SendPacketMessage(char * pPacket, INT32 i32PacketSize)
{
	if( FALSE == g_pContextMain->m_bLogDActive)	return 1; //사용하지 않음 

	if (INVALID_SOCKET == m_Socket)	return 0;

	INT32 i32SendIdx = 0;
	while( i32PacketSize > i32SendIdx )
	{
		INT32 i32sendedByte = send( m_Socket, pPacket+i32SendIdx, i32PacketSize-i32SendIdx, 0 );
	    
		if (SOCKET_ERROR == i32sendedByte)
		{
			DWORD errorCode = GetLastError();

			// 2011.10.07
			if ( errorCode != WSAEWOULDBLOCK )
			{
				char strmessage[MAX_STRING_COUNT];
				i3String::Format(strmessage, MAX_STRING_COUNT, "[ERROR] CModuleLogSocket::SendPacketMessage() - errorCode(%d)", errorCode);
				g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX, NULL, strmessage);
			}

			return 0;
		}		

		i32SendIdx += i32sendedByte;
	}
	
	return i32SendIdx; 
}

void CModuleLogSocket::GetName( SOCKADDR_IN * plocaladdr )
{
	sockaddr	addr;
	INT32	i32len = sizeof( sockaddr );
	::getsockname( m_Socket, &addr, &i32len );
	i3mem::Copy( &m_LocalAddr, &addr, sizeof( I3SOCKETADDR ) );
	if( plocaladdr != NULL )i3mem::Copy( plocaladdr, &m_LocalAddr, sizeof( SOCKADDR_IN ) );
}
