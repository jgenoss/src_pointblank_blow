#include "pch.h"
#include "ModuleLogSocket.h"
#include "LogFile.h"

I3_CLASS_INSTANCE(CModuleLogSocket, i3ElementBase); 

CModuleLogSocket::CModuleLogSocket()
{
	m_Socket				= INVALID_SOCKET; 
	m_RecvEvent				= INVALID_HANDLE_VALUE; 
	m_ReceivedPacketSize	= 0; 
	m_bSendOk				= TRUE; 

	return; 
}

CModuleLogSocket::~CModuleLogSocket()
{
	OnDestroy(); 
	return; 
}

BOOL CModuleLogSocket::OnDestroy(void)
{
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

BOOL CModuleLogSocket::OnConnect(char * Address, UINT16 PortNo)
{
	m_RecvEvent	= ::WSACreateEvent();
	m_Socket	= ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( m_Socket == INVALID_SOCKET )
	{		
		I3TRACE("[CModuleLogSocket::OnConnectTo] Error Socket Is Invalid \n"); 
		return FALSE; 
	}

	m_ReceivedPacketSize	= 0; 

	struct sockaddr_in serv_addr;
	i3mem::FillZero( &serv_addr, sizeof(serv_addr)); 
	serv_addr.sin_family		= AF_INET; 
	serv_addr.sin_addr.s_addr	= ::inet_addr(Address); 
	serv_addr.sin_port			= htons( PortNo ); 

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

			if( (nEvent.lNetworkEvents & FD_WRITE ) == FD_WRITE)
			{
				if( !m_bSendOk )
				{
					m_bSendOk = TRUE; 
					//WRITE_LOG(NULL, "[CModuleLogSocket::SelectEvent] START SEND MESSAGE"); 
				}
				return FALSE; 
			}
		}
	}
		
	//의미 없음..
	return FALSE; 
}

void CModuleLogSocket::OnReceive(void)
{	
	INT32 receivedByte = recv(m_Socket, &m_ReceiveBuffer[0], PACKETBUFFERSIZE, 0);

	if (SOCKET_ERROR == receivedByte)
	{
		DWORD errorCode = GetLastError();

		char message[MAX_STRING_COUNT];
		sprintf(message, "[ERROR] CModuleLogSocket::OnReceive() - errorCode(%d)", errorCode);
		//WRITE_LOG(NULL, message);

		return;
	}

	m_ReceivedPacketSize = receivedByte; 

	if(m_ReceivedPacketSize >= PACKETBUFFERSIZE) 
	{
		return; 
	}
	
	INT32 StartPos = 0; 
	INT32 ReadSize; 
	while( m_ReceivedPacketSize > 0 )
	{
		ReadSize = PacketParsing( &m_ReceiveBuffer[StartPos] );
		m_ReceivedPacketSize -= ReadSize;
		StartPos += ReadSize;
	}

	return; 
}

INT32 CModuleLogSocket::PacketParsing(char * pBuffer )
{
	return PACKETBUFFERSIZE; 
}		 

INT32 CModuleLogSocket::SendPacketMessage(char * pPacket, INT32 PacketSize)
{
	INT32		Rv = 0; 
	
	if( m_bSendOk )
	{
		INT32 sendedByte = send(m_Socket, pPacket, PacketSize, 0);
        
		if (SOCKET_ERROR == sendedByte)
		{
			DWORD errorCode = GetLastError();

			if (WSAEWOULDBLOCK == errorCode)
			{
				m_bSendOk = FALSE;
			}

			char message[MAX_STRING_COUNT];
			sprintf(message, "[ERROR] CModuleLogSocket::SendPacketMessage() - errorCode(%d)", errorCode);
			//WRITE_LOG(NULL, message);

			return 0;
		}		

		Rv = sendedByte;
	}
	else
	{
		//보내는 버퍼가 최대치를 넘었습니다. 그래서 보내지 못합니다.
		//WRITE_LOG(NULL, "[CModuleLogSocket::SendPacketessage] STOP SEND MESSAGE || ERROR_IO_PENDING 2-2");
		Rv = 1; 
	}
	
	return Rv; 
}

void CModuleLogSocket::GetName( SOCKADDR_IN * plocaladdr )
{
	sockaddr	addr;
	INT32	len = sizeof( sockaddr );
	::getsockname( m_Socket, &addr, &len );
	i3mem::Copy( &m_LocalAddr, &addr, sizeof( I3SOCKETADDR ) );
	if( plocaladdr != NULL )i3mem::Copy( plocaladdr, &m_LocalAddr, sizeof( SOCKADDR_IN ) );
}
