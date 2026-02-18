#include "pch.h"
#include "ModuleGipSocket.h"
#include "GIP_Packet_PB.h"
// #include "ModuleLogFile.h"

I3_CLASS_INSTANCE(CModuleGipSocket, i3ElementBase); 

CModuleGipSocket::CModuleGipSocket()
{
	m_Socket				= INVALID_SOCKET; 
	m_RecvEvent				= INVALID_HANDLE_VALUE; 
	m_ReceivedPacketSize	= 0; 	
	m_bGreeting				= FALSE; 
	m_bSendOk				= TRUE; 
	m_iSendErrorCount		= 0; 

	return; 
}

CModuleGipSocket::~CModuleGipSocket()
{
	OnDestroy(); 
	return; 
}

BOOL CModuleGipSocket::OnDestroy(void)
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

BOOL CModuleGipSocket::OnConnect(char * Address, UINT16 PortNo)
{
	m_bGreeting			= FALSE; 
	m_bSendOk			= TRUE; 
	m_iSendErrorCount	= 0; 

	m_RecvEvent	= ::WSACreateEvent();
	m_Socket	= ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( m_Socket == INVALID_SOCKET )
	{		
		I3TRACE("[CModuleGipSocket::OnConnectTo] Error Socket Is Invalid \n"); 
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
		i3Net::GetError("[CModuleGipSocket::OnConnectTo]"); 
		return FALSE; 
	}

	return TRUE; 
}

//무언가를 받았을때 TRUE 를 리턴해 줘요.
BOOL CModuleGipSocket::SelectEvent(void)
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
					g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL, "[CModuleGipSocket::SelectEvent] START SEND MESSAGE"); 
				}
				return FALSE; 
			}
		}
	}

	return FALSE; 
}

void CModuleGipSocket::OnReceive(void)
{	
	INT32 receivedByte = recv(m_Socket, &m_ReceiveBuffer[0], PACKETBUFFERSIZE, 0);

	if (SOCKET_ERROR == receivedByte)
	{
		DWORD errorCode = GetLastError();

		char message[MAX_STRING_COUNT];
		sprintf(message, "[ERORR] CModuleGipSocket::OnReceive() - errorCode(%d)", errorCode);
		g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL, message);
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

INT32 CModuleGipSocket::SendPacketMessage(char * pPacket, INT32 PacketSize)
{	
	INT32		Rv = 0; 
	
	if( m_bSendOk )
	{
		Rv = send(m_Socket, pPacket, PacketSize, 0);

		if (SOCKET_ERROR == Rv)
		{
			DWORD errorCode = GetLastError();
			char message[MAX_STRING_COUNT];
			sprintf(message, "[ERROR] CModuleGipSocket::SendPacketMessage() - errorCode(%d)", errorCode);
			g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX, NULL, message);

			Packet_TYPE_GAMESERVER_DATA* pData = (Packet_TYPE_GAMESERVER_DATA*)pPacket;
			g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX, NULL, pData->commandList.command );
			Rv = 0;

			if (WSAEWOULDBLOCK == errorCode)m_bSendOk = FALSE;
			m_iSendErrorCount++; 
		}
		else 
		{
			m_iSendErrorCount = 0; 
		}
	}
	else
	{
		//보내는 버퍼가 최대치를 넘었습니다. 그래서 보내지 못합니다.
		g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX, NULL, "[CModuleGipSocket::SendPacketessage] STOP SEND MESSAGE || ERROR_IO_PENDING 2-2"); 		
		Rv = 1; //가상으로 1바이트 보냈다고 보낸다. 
		m_iSendErrorCount++; 
	}

	return Rv; 
}

void CModuleGipSocket::GetName( SOCKADDR_IN * plocaladdr )
{
	sockaddr	addr;
	INT32	len = sizeof( sockaddr );
	::getsockname( m_Socket, &addr, &len );
	i3mem::Copy( &m_LocalAddr, &addr, sizeof( I3SOCKETADDR ) );
	if( plocaladdr != NULL )i3mem::Copy( plocaladdr, &m_LocalAddr, sizeof( SOCKADDR_IN ) );
	return; 
}

INT32 CModuleGipSocket::PacketParsing(char * pBuffer )
{
	if( m_ReceivedPacketSize < sizeof(Packet_TYPE_RESULT) )
	{
		I3TRACE("CModuleGipSocket::PacketParsing 헤더 사이즈보다 작음. 무시 \n"); 
		return m_ReceivedPacketSize; 
	}

	Packet_TYPE_RESULT * pResultPacket = (Packet_TYPE_RESULT*)pBuffer;
	Packet_TYPE_RESULT ResultPacket;
	ResultPacket.result = ntohs(pResultPacket->result); 
	
	switch ( ResultPacket.result )
	{
	case MSG_RESULT_SUCCESS				: 
		I3TRACE("MSG_RESULT_SUCCESS\n"); 
		if(!m_bGreeting) 
		{
			if( pResultPacket->header.qid == 0 )
			{
				m_bGreeting = TRUE;
				m_iPId = pResultPacket->header.pid;	//이건 변환하지 않는다. 
				g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX, NULL, "[CModuleGipSocket::PacketParsing] GIP GREETING SUCCESS");
			}
		}
		break; 
	case MSG_RESULT_REQUEST_1			: //UserInfo 유저인포를 다시 보낸다. 
		{
			if( pResultPacket->header.qid != 0 )
			{
				if(m_RequestCount < GIP_REQUEST_MAX_COUNT)
				{
					m_RequestInfo[m_RequestCount]._Type = 1; 
					m_RequestInfo[m_RequestCount]._Qid	= pResultPacket->header.qid; 
					m_RequestCount++; 
				}
				g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL, "[CModuleGipSocket::PacketParsing] REQUEST_1");
				I3TRACE("MSG_RESULT_REQUEST_1\n");
			}
		}
		break; 
	case MSG_RESULT_REQUEST_2			: //CharaInfo 캐릭터 인포를 다시 보낸다. 
		{
			if( pResultPacket->header.qid != 0 )
			{
				if(m_RequestCount < GIP_REQUEST_MAX_COUNT)
				{
					m_RequestInfo[m_RequestCount]._Type = 2; 
					m_RequestInfo[m_RequestCount]._Qid	=  pResultPacket->header.qid; 
					m_RequestCount++; 
				}
				g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL, "[CModuleGipSocket::PacketParsing] REQUEST_2");
				I3TRACE("MSG_RESULT_REQUEST_2\n"); 
			}
		}
		break; 
	case MSG_RESULT_REQUEST_3				: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_REQUEST_3");				break; 
	case MSG_RESULT_REQUEST_4				: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_REQUEST_4");				break; 
	case MSG_RESULT_REQUEST_5				: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_REQUEST_5");				break; 
	case MSG_RESULT_REQUEST_6				: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_REQUEST_6");				break; 
	case MSG_RESULT_REQUEST_7				: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_REQUEST_7");				break; 
	case MSG_RESULT_REQUEST_8				: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_REQUEST_8");				break; 
	case MSG_RESULT_REQUEST_9				: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_REQUEST_9");				break; 
	case MSG_RESULT_REQUEST_10				: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_REQUEST_10");				break; 
	case MSG_RESULT_REQUEST_11				: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_REQUEST_11");				break; 
	case MSG_RESULT_REQUEST_12				: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_REQUEST_12");				break; 
	case MSG_RESULT_REQUEST_13				: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_REQUEST_13");				break; 
	case MSG_RESULT_REQUEST_14				: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_REQUEST_14");				break; 
	case MSG_RESULT_REQUEST_15				: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_REQUEST_15");				break; 
	case MSG_RESULT_INVALID_GAME_ID			: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_INVALID_GAME_ID");			break; 
	case MSG_RESULT_INVALID_SERVER_ID		: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_INVALID_SERVER_ID");		break; 
	case MSG_RESULT_OVERLAPPING_SERVER_ID	: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_OVERLAPPING_SERVER_ID");	break; 
	case MSG_RESULT_INVALID_CMD				: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_INVALID_CMD");				break; 
	case MSG_RESULT_INVALID_PARAM			: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_INVALID_PARAM");			break; 
	case MSG_RESULT_CMD_SYNTAX_ERROR		: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_CMD_SYNTAX_ERROR");			break; 
	//엔씨에 요청했으나 응답이 없어서 일단 로그를 삭제합니다. 
	//추후 동접이 많아지면 다시 요청합니다. 
	case MSG_RESULT_DB_TRANSACT_ERROR		: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_DB_TRANSACT_ERROR"); 		break; 
	case MSG_RESULT_DB_DISCONNECTED			: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_DB_DISCONNECTED");			break; 
	case MSG_RESULT_GENERAL_FAILURE			: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_GENERAL_FAILURE");			break; 
	case MSG_RESULT_UNSUPPORTED_VERSION		: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_UNSUPPORTED_VERSION");		break; 
	case MSG_RESULT_INVALID_PACKET_LENGTH	: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_INVALID_PACKET_LENGTH");	break; 
	case MSG_RESULT_BAD_HEADER_CRC			: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_BAD_HEADER_CRC");			break; 
	case MSG_RESULT_UNAUTHORIZED_ID			: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG_RESULT_UNAUTHORIZED_ID");			break;
	default									: g_pModuleLogFile->Write_M_Log(MODULE_NC_THREAD_IDX,NULL,"MSG RESULT UNKNOWN");					break;
	}

	return sizeof(Packet_TYPE_RESULT); 
}

