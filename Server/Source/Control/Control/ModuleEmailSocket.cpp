#include "pch.h"
#include "ModuleEmailSocket.h"
#include "Logfile.h"

I3_CLASS_INSTANCE( CModuleEmailSocket, i3ElementBase );


CModuleEmailSocket::CModuleEmailSocket(void)
{
	m_socket					= INVALID_SOCKET;
	m_recvEvent					= INVALID_HANDLE_VALUE;
	m_receivedPacketSize		= 0;

	Reset();
}

CModuleEmailSocket::~CModuleEmailSocket(void)
{
#ifdef I3_DEBUG
	//// 테스트 코드
	char strTemp[500];
	i3String::Format( strTemp, 500, "CModuleEmailSocket::~CModuleEmailSocket]" ) ;
	WRITE_LOG_EMAIL( strTemp );
#endif
	OnDestroy();
}

void CModuleEmailSocket::Reset(void)
{
	m_bIsSendingEmail			= FALSE;		
	m_ui32CurSendEmailCount		= 0;	
	m_eStepSendEmail			= STEP_SEND_EMAIL_NONE;	
	m_ui32EmailReceiverCount	= 0;

	m_ui32LastSendTime			= g_pControl->GetServerTime();
	m_ui32LastRecvTime			= g_pControl->GetServerTime();
}

BOOL CModuleEmailSocket::OnCreate( UINT32 ui32EmailIP, UINT16 PortNo)
{
	// 리시브 이벤트 생성
	m_recvEvent = ::WSACreateEvent();

	// 소켓 생성
	m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_socket)
	{
		return FALSE;
	}

	// 서버 접속
	struct sockaddr_in serverAddr;
	i3mem::FillZero(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ui32EmailIP;
	serverAddr.sin_port = ::htons(PortNo);

	int result = ::connect(m_socket, (sockaddr*)&serverAddr, sizeof(serverAddr));

	INT32 i32optVal = 1024*1024*2;
	INT32 i32optLen = sizeof(INT32);
	setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char*)&i32optVal, i32optLen );

	if (SOCKET_ERROR == result)
	{
		return FALSE;
	}

	// 리시브 관련 멤버 초기화
	m_receivedPacketSize = 0;

	return TRUE;
}

void CModuleEmailSocket::OnDestroy(void)
{
#ifdef I3_DEBUG
	//// 테스트 코드
	char strTemp[500];
	i3String::Format( strTemp, 500, "[CModuleEmailSocket::OnDestroy]" ) ;
	WRITE_LOG_EMAIL( strTemp );
#endif

	if (INVALID_HANDLE_VALUE != m_recvEvent)
	{
		::WSACloseEvent(m_recvEvent);
		m_recvEvent = INVALID_HANDLE_VALUE;
	}
	if (INVALID_SOCKET != m_socket)
	{
		::shutdown(m_socket, SD_BOTH);
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
	m_receivedPacketSize = 0;
}

void CModuleEmailSocket::OnReceive(void)
{
	INT32 receivedByte = ::recv(m_socket, &m_receiveBuffer[m_receivedPacketSize], (PACKETBUFFERSIZE - m_receivedPacketSize), 0);

	if (SOCKET_ERROR == receivedByte)
	{
		OnDestroy();
		return;		
	}

	if (0 == receivedByte)return;

//#ifdef I3_DEBUG
//	//// 테스트 코드
//	char strTemp[500];
//	i3String::Format( strTemp, 500, "[CModuleEmailSocket::OnReceive] / %d bytes / %s", receivedByte, m_receiveBuffer ) ;
//	WRITE_LOG_EMAIL( strTemp );
//#endif

	
	// 받은 데이터 크기 누적
	m_receivedPacketSize += receivedByte;

	 //버퍼상의 파싱 시작 위치 
	INT32	startIdx = 0;
	INT32	ReadSize; 
	BOOL	bRead	 = TRUE; 
	while(bRead)
	{
		ReadSize = PacketParsing( &m_receiveBuffer[startIdx],  m_receivedPacketSize - startIdx); 
		if(ReadSize == 0)bRead = FALSE; 

		startIdx = startIdx + ReadSize; 
		if(startIdx >= m_receivedPacketSize)bRead = FALSE; 
	}

	//Data Rearrange
	m_receivedPacketSize -= startIdx;
	if (0 < startIdx && 0 < m_receivedPacketSize)
	{
		memmove(m_receiveBuffer, m_receiveBuffer + startIdx, m_receivedPacketSize);
	}

	return;
}

BOOL	CModuleEmailSocket::SendEmailStart(void) // 이메일 전송 시작
{
	char strSendPacket[MAX_PATH];
	char strErrorLog[MAX_ERROR_LOG_SIZE];

	// 메일 보내는 사람 메일 서버 주소 추출.
	char* pSenderEmailAddress;	// 포인터 연산용.
	pSenderEmailAddress = g_pControl->m_stEmailSendSystem.m_strSenderEmailAddress;
	while ( '\0' != *pSenderEmailAddress )
	{
		if( '@' == *pSenderEmailAddress )
		{
			pSenderEmailAddress++;
			break;
		}

		pSenderEmailAddress++;
	}

	// TCP Send
	i3String::Format( strSendPacket, MAX_PATH, "HELO %s\r\n", pSenderEmailAddress );

	if( 0 == SendPacketMessage(strSendPacket, i3String::Length( strSendPacket ) ) )	
	{
		FailSendPacket( STEP_SEND_EMAIL_NONE, strSendPacket ) ;
		return FALSE;
	}
	else 
	{

		WRITE_LOG_EMAIL( "//////////////////////////////////////////////////////////////////////////////////////////" );
		WRITE_LOG_EMAIL( "//////////////////////			EMail Send Start		////////////////////////////////" );
#ifdef I3_DEBUG
		i3String::Format( strErrorLog, MAX_ERROR_LOG_SIZE, "[CModuleEmailSocket::SendEmailStart] Send Packet SUCCESS : %s ", strSendPacket );
		WRITE_LOG_EMAIL( strErrorLog );
		I3TRACE( strErrorLog );
#endif
		m_eStepSendEmail	= STEP_SEND_EMAIL_HELLO;
		m_ui32LastSendTime	= g_pControl->GetServerTime();
		m_bIsSendingEmail	= TRUE; // 메일 보내는중.
	}

	return TRUE;
}


	
	

INT32 CModuleEmailSocket::SendPacketMessage(char * pPacket, INT32 size)
{
	// 소켓이 닫혔으면 보네지 않는다.
	if (INVALID_SOCKET == m_socket)	return 0;

	// 패킷을 다 보낼때까지 반복한다.
	INT32 sendIdx = 0;
	while(size > sendIdx)
	{
		int sendedByte = ::send(m_socket, pPacket + sendIdx, size - sendIdx, 0);

		// 전송 실패 
		if (SOCKET_ERROR == sendedByte)return ::WSAGetLastError();

		// 연결이 끊어졌다.
		if (0 == sendedByte)return 0;

		// 전송 바이트만큼 인덱스 증가
		sendIdx += sendedByte;
	}

	return sendIdx;
}

bool CModuleEmailSocket::SelectEvent(void)
{
	bool bWorking = false;

	WSANETWORKEVENTS event;
	i3mem::FillZero(&event, sizeof(event));

	// 처리할 이벤트를 확인한다.
	if (SOCKET_ERROR != ::WSAEventSelect(m_socket, m_recvEvent, FD_WRITE | FD_READ | FD_CLOSE))
	{
		if (SOCKET_ERROR != ::WSAEnumNetworkEvents(m_socket, m_recvEvent, &event))
		{
			if(event.lNetworkEvents & FD_READ)
			{
				OnReceive();
				bWorking = true;
			}
			if(event.lNetworkEvents & FD_CLOSE) 
			{

				OnDestroy();
				bWorking = true;
			}
			return	bWorking;
		}
	}
	return bWorking; 
}


INT32 CModuleEmailSocket::PacketParsing( char * pBuffer, INT32 Packetsize )
{
	char strErrorLog[MAX_ERROR_LOG_SIZE];
	char strSendPacket[MAX_PATH];
	char strSendLog[MAX_ERROR_LOG_SIZE];

#ifdef I3_DEBUG
	i3String::Format( strErrorLog, MAX_ERROR_LOG_SIZE, "[CModuleEmailSocket::PacketParsing] %s ", pBuffer );
	WRITE_LOG_EMAIL( strErrorLog );
	I3TRACE( strErrorLog );
#endif

	// STMP 서버로 부터 온 응답 패킷 파싱.
	char strReturnCode[SMTP_SUCCESS_RETURN_CODE_SIZE];	// 리턴 코드.
	i3String::Copy(strReturnCode, pBuffer, SMTP_SUCCESS_RETURN_CODE_SIZE );


	// 현재 메일전송 진행 상태에 따른 처리,
	switch( m_eStepSendEmail ) 
	{
	case STEP_SEND_EMAIL_HELLO :
		{
			if( 0 == i3String::Compare( strReturnCode, SMTP_SUCCESS_RETURN_CODE_HELLO_BEGIN ) || 0 == i3String::Compare( strReturnCode, SMTP_SUCCESS_RETURN_CODE_HELLO_AFTER ) )
			{
				// TCP Send
				i3String::Format( strSendPacket, MAX_PATH, "MAIL FROM: <%s>\r\n", g_pControl->m_stEmailSendSystem.m_strSenderEmailAddress );
				if( 0 == SendPacketMessage(strSendPacket, i3String::Length( strSendPacket ) ) )	FailSendPacket( m_eStepSendEmail, strSendPacket ) ;
				else 
				{
					m_ui32LastSendTime	= g_pControl->GetServerTime();
					m_eStepSendEmail = STEP_SEND_EMAIL_FROMTO;
#ifdef I3_DEBUG
					i3String::Format( strSendLog, MAX_ERROR_LOG_SIZE, "[CModuleEmailSocket::PacketParsing] Send Packet SUCCESS : %s ", strSendPacket );
					WRITE_LOG_EMAIL( strSendLog );
					I3TRACE( strSendLog );
#endif
				}
			}
			else RecvErrorPacket( m_eStepSendEmail, strReturnCode );
		}
		break;
	case STEP_SEND_EMAIL_FROMTO :
		{
			if( 0 == i3String::Compare( strReturnCode, SMTP_SUCCESS_RETURN_CODE_FROMTO ) )
			{
				// 보내야 할 이메일 계정수만큼 반복됬으면 다음단계로 넘어간다.
				if( g_pControl->m_stEmailSendSystem.m_ui32EmailCount <= m_ui32EmailReceiverCount )
				{
					// TCP Send
					i3String::Format( strSendPacket, MAX_PATH, "DATA\r\n"  );
					if( 0 == SendPacketMessage(strSendPacket, i3String::Length( strSendPacket ) ) )	FailSendPacket( m_eStepSendEmail, strSendPacket ) ;
					else 
					{
						m_ui32LastSendTime	= g_pControl->GetServerTime();
						m_eStepSendEmail = STEP_SEND_EMAIL_DATA;
#ifdef I3_DEBUG
						i3String::Format( strSendLog, MAX_ERROR_LOG_SIZE, "[CModuleEmailSocket::PacketParsing] Send Packet SUCCESS : %s ", strSendPacket );
						WRITE_LOG_EMAIL( strSendLog );
						I3TRACE( strSendLog );
#endif
					}
				}
				else
				{
					// TCP Send
					i3String::Format( strSendPacket, MAX_PATH, "RCPT TO: <%s>\r\n", g_pControl->m_stEmailSendSystem.m_aEmailAddress[m_ui32EmailReceiverCount].m_strAddress );
					if( 0 == SendPacketMessage(strSendPacket, i3String::Length( strSendPacket ) ) )	FailSendPacket( m_eStepSendEmail, strSendPacket ) ;
					else 
					{
						m_ui32LastSendTime			= g_pControl->GetServerTime();
						m_ui32EmailReceiverCount++;
#ifdef I3_DEBUG
						i3String::Format( strSendLog, MAX_ERROR_LOG_SIZE, "[CModuleEmailSocket::PacketParsing] Send Packet SUCCESS : %s ", strSendPacket );
						WRITE_LOG_EMAIL( strSendLog );
						I3TRACE( strSendLog );
#endif
					}
				}
			}
			else RecvErrorPacket( m_eStepSendEmail, strReturnCode );
		}
		break;
	case STEP_SEND_EMAIL_DATA :	
		{
			if( 0 == i3String::Compare( strReturnCode, SMTP_SUCCESS_RETURN_CODE_DATA ) )
			{
				char strEmail[MAX_PATH];
				ASC_SERVER_TYPE eServerType = (ASC_SERVER_TYPE)g_pControl->m_i32ServerType;


				switch( eServerType ) 
				{
				case ASC_SERVER_TRANS	:	i3String::Format( strEmail, MAX_PATH, "PB_TRANS_SERVER_DOWN %s ", g_pControl->m_strServerIp); break;
				case ASC_SERVER_CLAN	:	i3String::Format( strEmail, MAX_PATH, "PB_CLAN_SERVER_DOWN %s ", g_pControl->m_strServerIp); break;
				case ASC_SERVER_MESSENGER : i3String::Format( strEmail, MAX_PATH, "PB_MESS_SERVER_DOWN %s ", g_pControl->m_strServerIp); break;
				case ASC_SERVER_BATTLE	:	i3String::Format( strEmail, MAX_PATH, "PB_BATTLE_SERVER_DOWN %s ", g_pControl->m_strServerIp); break;
				case ASC_SERVER_UPDATER :	i3String::Format( strEmail, MAX_PATH, "PB_UPDATE_SERVER_DOWN %s ", g_pControl->m_strServerIp); break;
				case ASC_SERVER_CONTROL :	i3String::Format( strEmail, MAX_PATH, "PB_CONTROL_SERVER_DOWN %s ", g_pControl->m_strServerIp); break;
				case ASC_SERVER_ZLOG	:	i3String::Format( strEmail, MAX_PATH, "PB_ZLOG_SERVER_DOWN %s ", g_pControl->m_strServerIp); break;
				case ASC_SERVER_GATEWAY :	i3String::Format( strEmail, MAX_PATH, "PB_GATEWAY_SERVER_DOWN %s ", g_pControl->m_strServerIp); break;
				case ASC_SERVER_SIA		:	i3String::Format( strEmail, MAX_PATH, "PB_SIA_SERVER_DOWN %s ", g_pControl->m_strServerIp); break;
				case ASC_SERVER_GAME	:	i3String::Format( strEmail, MAX_PATH, "PB_GAME_SERVER_DOWN %s ", g_pControl->m_strServerIp); break;
				case ASC_SERVER_BILLING :	i3String::Format( strEmail, MAX_PATH, "PB_BILLING_SERVER_DOWN %s ", g_pControl->m_strServerIp); break;
				default					:	i3String::Format( strEmail, MAX_PATH, "PB_UNKNOWN_SERVER_DOWN %s ", g_pControl->m_strServerIp); break;
				}

				// TCP Send
				i3String::Format( strSendPacket, MAX_PATH, "Subject:%s \r\n\r\n%s\r\n.\r\n" , strEmail,strEmail );
				if( 0 == SendPacketMessage(strSendPacket, i3String::Length( strSendPacket ) ) )	FailSendPacket( m_eStepSendEmail, strSendPacket ) ;
				else 
				{
					m_ui32LastSendTime			= g_pControl->GetServerTime();
					m_eStepSendEmail			= STEP_SEND_EMAIL_EMAIL;
#ifdef I3_DEBUG
					i3String::Format( strSendLog, MAX_ERROR_LOG_SIZE, "[CModuleEmailSocket::PacketParsing] Send Packet SUCCESS : %s ", strSendPacket );
					WRITE_LOG_EMAIL( strSendLog );
					I3TRACE( strSendLog );
#endif
				}
			}
			else RecvErrorPacket( m_eStepSendEmail, strReturnCode );
		}
		break;
	case STEP_SEND_EMAIL_EMAIL :	// 메일 제목,내용 입력.
		{
			if( 0 == i3String::Compare( strReturnCode, SMTP_SUCCESS_RETURN_CODE_EMAIL ) )
			{
				Reset();
		
				WRITE_LOG_EMAIL( "//////////////////////			EMail Send End		////////////////////////////////////" );
				WRITE_LOG_EMAIL( "//////////////////////////////////////////////////////////////////////////////////////////" );

			}
			else RecvErrorPacket( m_eStepSendEmail, strReturnCode );
		}
		break;
	
	default : // 버그아님.
		{
#ifdef I3_DEBUG			
			i3String::Format( strErrorLog, MAX_ERROR_LOG_SIZE, "[CModuleEmailSocket::SendEmail] case default / m_eStepSendEmail : %d / Return Code : %s ", (INT32)m_eStepSendEmail, strReturnCode );
			WRITE_LOG_EMAIL( strErrorLog );
			I3TRACE( strErrorLog );
#endif
		}
		break;
	}

	return Packetsize;
}

void CModuleEmailSocket::FailSendPacket( STEP_SEND_EMAIL eStep, char * pSendPacket )
{
	char strErrorLog[MAX_ERROR_LOG_SIZE];
	i3String::Format( strErrorLog, MAX_ERROR_LOG_SIZE, "[CModuleEmailSocket::FailSendPacket] STEP : %d / Packet : %s ", (INT32)eStep, pSendPacket );
	WRITE_LOG_EMAIL( strErrorLog );
	I3TRACE( strErrorLog );

	Reset();

	return;
}


void CModuleEmailSocket::RecvErrorPacket( STEP_SEND_EMAIL eStep ,char * pErrorCode )
{
	char strErrorLog[MAX_ERROR_LOG_SIZE];
	i3String::Format( strErrorLog, MAX_ERROR_LOG_SIZE, "[CModuleEmailSocket::RecvErrorPacket] STEP : %d / Error Code :  %s ", (INT32)eStep, pErrorCode );
	WRITE_LOG_EMAIL( strErrorLog );
	I3TRACE( strErrorLog );

	Reset();

	return;
}
