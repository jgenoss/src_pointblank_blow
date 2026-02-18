#include "pch.h"



I3_CLASS_INSTANCE( CSmtpMail, i3Thread );

#define EMAIL_RECONNECT_TIME				10 	 

//------------------------------------------------------------------------------------------
CSmtpMail::CSmtpMail(void)
{
	// Thread
	m_bRunning			= FALSE;

	// Base
	i3mem::FillZero ( &m_BaseEmailInfo, sizeof(BASE_EMAIL_INFO) );
	

	// Socket
	m_socket					= INVALID_SOCKET;
	m_recvEvent					= INVALID_HANDLE_VALUE;
	m_i32receivedPacketSize		= 0;
	Reset();

	i3String::Copy( m_BaseEmailInfo.m_strSenderEmailAddress, "Unknown@mail.com", MAX_LENGTH_EMAIL_ADDRESS ) ;
}

CSmtpMail::~CSmtpMail(void)
{
	OnDestroy(); 
}

void CSmtpMail::Reset(void)
{
	m_bIsSendingEmail			= FALSE;
	m_eStepSendEmail			= STEP_SEND_EMAIL_NONE;
	m_ui32CurRecvEmailAddressCount	= 0;

	m_ui32LastSendTime			= i3ThreadTimer::GetServerTime();
	m_ui32LastMailFinishTime	= i3ThreadTimer::GetServerTime();

	m_CurrenEmailSend.Reset();
}


INT32	CSmtpMail::OnCreate( BASE_EMAIL_INFO *pBaseEmailInfo  )
{
	if( NULL == pBaseEmailInfo ) return CREATE_SMTPEMAIL_FAIL_BASE_INFO;

	m_BaseEmailInfo.m_ui32SmtpServerAddress		= pBaseEmailInfo->m_ui32SmtpServerAddress;
	m_BaseEmailInfo.m_ui32RecvEmailCount		= min( pBaseEmailInfo->m_ui32RecvEmailCount, MAX_RECV_EMAIL_ADDRESS ) ; 
	m_BaseEmailInfo.m_ui32AckWaitTime			= min( pBaseEmailInfo->m_ui32AckWaitTime, MAX_ACK_WAIT_TIME ) ; 
	m_BaseEmailInfo.m_ui32EmailSendDelayTime	= min( pBaseEmailInfo->m_ui32EmailSendDelayTime, MAX_EMAIL_SEND_DELAY_TIME ) ; 
 
	i3mem::Copy( m_BaseEmailInfo.m_strSenderEmailAddress, pBaseEmailInfo->m_strSenderEmailAddress, MAX_LENGTH_EMAIL_ADDRESS ) ;
	i3mem::Copy( m_BaseEmailInfo.m_aRecvEmailAddress[0].m_strEmailAddress, pBaseEmailInfo->m_aRecvEmailAddress[0].m_strEmailAddress, sizeof(RECV_EMAIL_ADDRESS) * MAX_RECV_EMAIL_ADDRESS  ) ;


	//////////////////////////////////////////////////////////////
	//////////////		Create Socket		//////////////////////
	INT32 i32Rv = CreateSocket( m_BaseEmailInfo.m_ui32SmtpServerAddress ); 
	if ( CREATE_SMTPEMAIL_SUCCESS != i32Rv ) return i32Rv;



	//////////////////////////////////////////////////////////////
	//////////////		Create Thread		//////////////////////
	if( FALSE == i3Thread::Create( "SmtpMail", 0, 4096, NULL, PRIORITY_NORMAL ) )	 return CREATE_SMTPEMAIL_FAIL_THREAD_CREATE;

	return CREATE_SMTPEMAIL_SUCCESS; 
}





INT32	CSmtpMail::CreateSocket(UINT32 ui32SmtpServerAddress)
{
	//////////////////////////////////////////////////////////////
	//////////////		Create Socket		//////////////////////
	m_recvEvent = ::WSACreateEvent();	// 리시브 이벤트 생성

	// 소켓 생성
	m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_socket)	return CREATE_SMTPEMAIL_FAIL_SOCKET_CREATE;
	

	// 서버 접속
	struct sockaddr_in serverAddr;
	i3mem::FillZero(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ui32SmtpServerAddress;
	serverAddr.sin_port = ::htons(SMTP_PORT);

	if( SOCKET_ERROR == ::connect(m_socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) ) return CREATE_SMTPEMAIL_FAIL_SOCKET_CONNECT;
	

	INT32 i32optVal = 1024*1024*2;
	INT32 i32optLen = sizeof(INT32);
	if( SOCKET_ERROR == setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char*)&i32optVal, i32optLen )) return CREATE_SMTPEMAIL_FAIL_SOCKET_SET_OPTION;
	

	m_i32receivedPacketSize = 0;		// 리시브 관련 멤버 초기화

	m_ui32SmtpConnectTime		= i3ThreadTimer::GetServerTime();

	return CREATE_SMTPEMAIL_SUCCESS;
}

void CSmtpMail::OnDestroy(void)
{
	
	//////////////////////////////////////////////////////////////
	//////////////		Destroy Thread		//////////////////////
	m_bRunning = FALSE;
	i3Thread::WaitForFinish();


	//////////////////////////////////////////////////////////////
	//////////////		Destroy Socket		//////////////////////
	DestroySocket();	
}

void CSmtpMail::DestroySocket(void)
{
	//////////////////////////////////////////////////////////////
	//////////////		Destroy Socket		//////////////////////
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
	m_i32receivedPacketSize = 0;

}

INT32 CSmtpMail::SmtpServerConnect()
{
	if( m_ui32SmtpConnectTime + EMAIL_RECONNECT_TIME > i3ThreadTimer::GetServerTime() )	return 1;	// 이전 리커넥 타임 체크. 10초에 한번 리커넥.
	m_ui32SmtpConnectTime = i3ThreadTimer::GetServerTime();

	DestroySocket();

	INT32 i32Rv = CreateSocket( m_BaseEmailInfo.m_ui32SmtpServerAddress );

	if( CREATE_SMTPEMAIL_SUCCESS == i32Rv ) return i32Rv;

	DestroySocket();

	return i32Rv;
}


////////////////////////////////////////////////////////////////////////////////
//////////////////////	Thread		////////////////////////////////////////////
bool CSmtpMail::OnRunningEx()
{
	return false;
}

UINT32 CSmtpMail::OnRunning( void * pUserData )
{
	bool	bWorked;
	m_bRunning = TRUE;
	while( m_bRunning )
	{
		//-------------------------------------------------------------------------------
		//1. Connect Check . SMTP 서버와 연결 끊겼다면 재연결.
		if(FALSE == IsConnect())
		{
			if( CREATE_SMTPEMAIL_SUCCESS == SmtpServerConnect() )
			{
				// 로그 확인용 - 엔진으로 분리시 제거.
				g_pLog->WriteLog( L"[CSmtpMail::OnRunning] ReConnect SmtpServer Success / Reason : Disconnect " );
				Reset();	// 원점으로.
			}
			else
			{
				// 로그 확인용 - 엔진으로 분리시 제거.
				g_pLog->WriteLog( L"[CSmtpMail::OnRunning] ReConnect SmtpServer Fail /  Reason : Disconnect " );
			}

			::WaitForSingleObject( m_Handle, 1000);	// 1초 휴식..
			continue;
		}



		//-------------------------------------------------------------------------------
		//2. Send -  쓰레드별 메일 전송 버퍼에 쌓인 것들 STMP서버에 전송. 
		bWorked = OnRunningEx();


		//-------------------------------------------------------------------------------
		// 3. SMTP 서버로 부터 응답 수신.
		SelectEvent();	


		//-------------------------------------------------------------------------------
		//4. Time Out Check -  메일 전송 중일떄 SMTP 서버로 부터 ACK 패킷이 지정된 시간안에 수신 됐는지..
		if(   TRUE == m_bIsSendingEmail &&  ( i3ThreadTimer::GetServerTime() >  m_ui32LastSendTime +  m_BaseEmailInfo.m_ui32AckWaitTime ) )
		{
			if( CREATE_SMTPEMAIL_SUCCESS == SmtpServerConnect() )
			{
				// 로그 확인용 - 엔진으로 분리시 제거.
				g_pLog->WriteLog( L"[CSmtpMail::OnRunning] ReConnect SmtpServer Success / Reason : Recv Time over" );
				Reset();	// 원점으로.
			}
			else
			{
				// 로그 확인용 - 엔진으로 분리시 제거.
				g_pLog->WriteLog( L"[CSmtpMail::OnRunning] ReConnect SmtpServer Fail / Reason : Recv Time over" );
			}

			::WaitForSingleObject( m_Handle, 1000);	// 1초 휴식..
			continue;
		}


		if( false == bWorked ) ::WaitForSingleObject( m_Handle, 1);	// 한일이 없으면  1ms 휴식..

	}
	return 0; 
}



////////////////////////////////////////////////////////////////////////////////
//////////////////////	Socket		////////////////////////////////////////////
void CSmtpMail::OnReceive(void)
{
	INT32 i32receivedByte = ::recv(m_socket, &m_receiveBuffer[m_i32receivedPacketSize], (PACKETBUFFERSIZE - m_i32receivedPacketSize), 0);

	if (SOCKET_ERROR == i32receivedByte)
	{
		OnDestroy();
		return;		
	}

	if (0 == i32receivedByte)return;

	// 받은 데이터 크기 누적
	m_i32receivedPacketSize += i32receivedByte;

	 //버퍼상의 파싱 시작 위치 
	INT32	i32startIdx = 0;
	INT32	i32ReadSize; 
	BOOL	bRead	 = TRUE; 
	while(bRead)
	{
		i32ReadSize = PacketParsing( &m_receiveBuffer[i32startIdx],  m_i32receivedPacketSize - i32startIdx); 
		if(i32ReadSize == 0)bRead = FALSE; 

		i32startIdx = i32startIdx + i32ReadSize; 
		if(i32startIdx >= m_i32receivedPacketSize)bRead = FALSE; 
	}

	//Data Rearrange
	m_i32receivedPacketSize -= i32startIdx;
	if (0 < i32startIdx && 0 < m_i32receivedPacketSize)
	{
		memmove(m_receiveBuffer, m_receiveBuffer + i32startIdx, m_i32receivedPacketSize);
	}

	return;
}

BOOL	CSmtpMail::SendEmailStart(void) // 이메일 전송 시작
{
	char strSendPacket[MAX_PATH];

	// 메일 보내는 사람 메일 서버 주소 추출.
	char* pSenderEmailAddress;	// 포인터 연산용.
	pSenderEmailAddress = m_BaseEmailInfo.m_strSenderEmailAddress;
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
		// 로그 확인용 - 엔진으로 분리시 제거.
		g_pLog->WriteLog( L"//////////////////////////////////////////////////////////////////////////////////////////////////////////////////" );
		g_pLog->WriteLog( L"//////////////////////			EMail Send Start		////////////////////////////////" );

		m_eStepSendEmail	= STEP_SEND_EMAIL_HELLO;
		m_ui32LastSendTime	= i3ThreadTimer::GetServerTime();
		m_bIsSendingEmail	= TRUE; // 메일 보내는중.
	}

	return TRUE;
}

INT32 CSmtpMail::SendPacketMessage(char * pPacket, INT32 i32size)
{
	// 소켓이 닫혔으면 보네지 않는다.
	if (INVALID_SOCKET == m_socket)	return 0;

	// 패킷을 다 보낼때까지 반복한다.
	INT32 i32sendIdx = 0;
	while(i32size > i32sendIdx)
	{
		INT32 sendedByte = ::send(m_socket, pPacket + i32sendIdx, i32size - i32sendIdx, 0);

		// 전송 실패 
		if (SOCKET_ERROR == sendedByte)return ::WSAGetLastError();

		// 연결이 끊어졌다.
		if (0 == sendedByte)return 0;

		// 전송 바이트만큼 인덱스 증가
		i32sendIdx += sendedByte;
	}

	return i32sendIdx;
}

bool CSmtpMail::SelectEvent(void)
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
				DestroySocket();	// 소켓만 Destroy
				bWorking = true;
			}
			return	bWorking;
		}
	}
	return bWorking; 
}

INT32 CSmtpMail::PacketParsing( char * pBuffer, INT32 i32Packetsize )
{
	char strSendPacket[MAX_EMAIL_SIZE];

	// STMP 서버로 부터 온 응답 패킷 파싱.
	char strReturnCode[SMTP_SUCCESS_RETURN_CODE_SIZE];	// 리턴 코드
	i3String::Copy(strReturnCode, pBuffer, SMTP_SUCCESS_RETURN_CODE_SIZE );
	UINT32	ui32Rv = ::atoi( strReturnCode ) ;


	// 현재 메일전송 진행 상태에 따른 처리,
	switch( m_eStepSendEmail ) 
	{
	case STEP_SEND_EMAIL_HELLO :	
		{
			if( SMTP_SUCCESS_RETURN_CODE_HELLO_BEGIN == ui32Rv || SMTP_SUCCESS_RETURN_CODE_HELLO_AFTER == ui32Rv )
			{
				// 메일 송신인 주소 입력.
				i3String::Format( strSendPacket, MAX_EMAIL_SIZE, "MAIL FROM: <%s>\r\n", m_BaseEmailInfo.m_strSenderEmailAddress );
				if( 0 == SendPacketMessage(strSendPacket, i3String::Length( strSendPacket ) ) )	FailSendPacket( m_eStepSendEmail, strSendPacket ) ;
				else 
				{
					m_ui32LastSendTime	= i3ThreadTimer::GetServerTime();
					m_eStepSendEmail = STEP_SEND_EMAIL_FROMTO;
				}
			}
			else RecvErrorPacket( m_eStepSendEmail, strReturnCode );
		}
		break;
	case STEP_SEND_EMAIL_FROMTO :	
		{
			if( SMTP_SUCCESS_RETURN_CODE_FROMTO == ui32Rv )
			{
				// 보내야 할 이메일 계정수만큼 반복됬으면 다음단계로 넘어간다.
				if( m_BaseEmailInfo.m_ui32RecvEmailCount <= m_ui32CurRecvEmailAddressCount )
				{
					// 메일 작성 시작.
					i3String::Format( strSendPacket, MAX_EMAIL_SIZE, "DATA\r\n"  );
					if( 0 == SendPacketMessage(strSendPacket, i3String::Length( strSendPacket ) ) )	FailSendPacket( m_eStepSendEmail, strSendPacket ) ;
					else 
					{
						m_ui32LastSendTime	= i3ThreadTimer::GetServerTime();
						m_eStepSendEmail = STEP_SEND_EMAIL_DATA;
					}
				}
				else
				{
					// 메일 수신인 주소 입력.
					i3String::Format( strSendPacket, MAX_EMAIL_SIZE, "RCPT TO: <%s>\r\n", m_BaseEmailInfo.m_aRecvEmailAddress[m_ui32CurRecvEmailAddressCount].m_strEmailAddress );
					if( 0 == SendPacketMessage(strSendPacket, i3String::Length( strSendPacket ) ) )	FailSendPacket( m_eStepSendEmail, strSendPacket ) ;
					else 
					{
						m_ui32LastSendTime			= i3ThreadTimer::GetServerTime();
						m_ui32CurRecvEmailAddressCount++;
					}
				}
			}
			else RecvErrorPacket( m_eStepSendEmail, strReturnCode );
		}
		break;
	case STEP_SEND_EMAIL_DATA :	
		{
			if(  SMTP_SUCCESS_RETURN_CODE_DATA == ui32Rv )
			{
				// 메일 제목 / 내용 입력.
				i3String::Format( strSendPacket, MAX_EMAIL_SIZE, "Subject:%s \r\n\r\n%s\r\n.\r\n" , m_CurrenEmailSend.m_strSubject ,m_CurrenEmailSend.m_strBody );
				if( 0 == SendPacketMessage(strSendPacket, i3String::Length( strSendPacket ) ) )	FailSendPacket( m_eStepSendEmail, strSendPacket ) ;
				else 
				{
					m_ui32LastSendTime			= i3ThreadTimer::GetServerTime();
					m_eStepSendEmail			= STEP_SEND_EMAIL_EMAIL;
				}
			}
			else RecvErrorPacket( m_eStepSendEmail, strReturnCode );
		}
		break;
	case STEP_SEND_EMAIL_EMAIL :	
		{
			if( SMTP_SUCCESS_RETURN_CODE_EMAIL == ui32Rv )
			{
				Reset();
			}
			else RecvErrorPacket( m_eStepSendEmail, strReturnCode );
		}
		break;
	
	default : // 버그아님.
		{
		}
		break;
	}

	return i32Packetsize;
}

void CSmtpMail::FailSendPacket( STEP_SEND_EMAIL eStep, char * pSendPacket )
{
	Reset();

	return;
}


void CSmtpMail::RecvErrorPacket( STEP_SEND_EMAIL eStep ,char * pErrorCode )
{
	Reset();

	return;
}