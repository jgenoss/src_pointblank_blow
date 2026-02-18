#include "pch.h"
#include "LogFile.h"
#include "UdpWorker.h"
#include "UdpBuffer.h"
#include "UdpServer.h"
#include "GroupMaker.h"
#include "RelayGroup.h"
#include "RelayMember.h"

I3_CLASS_INSTANCE(CUdpWorker, i3Thread);

CUdpWorker::CUdpWorker()
{
	m_ServerSocket		= INVALID_SOCKET;
	m_pRecvBuffer		= NULL;
	m_pSendBuffer		= NULL;
	m_bRunning			= FALSE;

	m_i32ErrorCount		= 0;
	m_ui32ErrorTime		= g_pConfig->GetServerTime();

	i3mem::FillZero( m_pui32PacketCount, sizeof( UINT32 )* PACKETCOUNT_IDX );
}

CUdpWorker::~CUdpWorker()
{
	m_bRunning = FALSE;
	if (m_ServerSocket != INVALID_SOCKET)
	{
		closesocket( m_ServerSocket );
		m_ServerSocket = INVALID_SOCKET;
	}

	i3Thread::WaitForFinish();	

	I3_SAFE_RELEASE( m_pRecvBuffer );
	I3_SAFE_RELEASE( m_pSendBuffer );
}

BOOL CUdpWorker::Create(char* pszName, UINT32 Attr, UINT32 StackSize, void* pUserData, INT32 priority)
{
	return i3Thread::Create( pszName, Attr, StackSize, pUserData, priority );
}

BOOL CUdpWorker::Create( INT32 nWorkerIndex, UINT32 wIP, UINT16 sPort )
{
	m_wIP				= wIP;
	m_sPort				= sPort;
	m_i32WorkerIndex	= nWorkerIndex;

	// 소켓 생성
	m_ServerSocket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( INVALID_SOCKET == m_ServerSocket )
	{
		WRITE_LOG_MAIN( "[Error] Socket Init" );
		return FALSE;
	}

	INT32 bufsz = 1024 * 1024 * 100;

	::setsockopt( m_ServerSocket, SOL_SOCKET, SO_RCVBUF, (const char *) &bufsz, sizeof(INT32));
	::setsockopt( m_ServerSocket, SOL_SOCKET, SO_SNDBUF, (const char *) &bufsz, sizeof(INT32));

	// 소켓 바인드
	sockaddr_in addr;
	i3mem::FillZero(&addr, sizeof(sockaddr_in));
	addr.sin_family			= AF_INET;
	addr.sin_addr.s_addr	= wIP;
	addr.sin_port			= htons( sPort );

	if ( SOCKET_ERROR == bind( m_ServerSocket, (sockaddr*)&addr, sizeof(sockaddr_in)) )
	{
		return FALSE;
	}

	m_pRecvBuffer	= CUdpBuffer::NewObject();
	m_pRecvBuffer->InitReceive();

	m_pSendBuffer	= CUdpBuffer::NewObject();
	// Send Buffer 를 쓰는곳이 UDPPROTOCOL_SERVER_JOIN_GROUP_ACK 밖에 없으므로 우선 정의
	m_pSendBuffer->InitSend();
	m_pSendBuffer->SetUdpProtocolId( UDPPROTOCOL_SERVER_JOIN_GROUP_ACK );

	// Thread 생성
	char threadName[MAX_STRING_COUNT];
	sprintf(threadName, "UdpWorker%03d", nWorkerIndex);

	return Create( threadName );
}

UINT32 CUdpWorker::OnRunning(void* pUserData)
{
	// 버퍼를 받아야 동작.
	
	m_bRunning = TRUE;
    while( m_bRunning )
	{
		// Recv
		if( RecvPacket() )
		{
			m_pui32PacketCount[ g_pUdpServer->GetCountIdx() ]++;
            // Packet
            PacketParsing();
		}
	}

	return 0;
}

BOOL CUdpWorker::RecvPacket()
{
	m_pRecvBuffer->InitReceive();
	int nRv = ::recvfrom( m_ServerSocket, m_pRecvBuffer->GetBuffer(), UDP_SEND_PACKET_SIZE, 0, (sockaddr*) m_pRecvBuffer->GetAddress(), m_pRecvBuffer->GetAddressSize() );
		
	if( SOCKET_ERROR == nRv ) 
	{
		switch( GetLastError() )
		{
		case WSAECONNRESET:
			{	// 이 에러는 무시합니다. 클라이언트에서 패킷을 보내자마자 소켓을 닫았을때 나오는 에러입니다.
				// 한시간에 한번씩만 찍습니다.
				m_i32ErrorCount++;
				if( (m_ui32ErrorTime + 3600) < g_pConfig->GetServerTime() )
				{
					char logText[MAX_STRING_COUNT];
					sprintf(logText, "[ERROR] RecvPacket - ErrorCode(WSAECONNRESET) Count : %d", m_i32ErrorCount );
					WRITE_LOG_RELAY( m_i32WorkerIndex, logText);

					m_ui32ErrorTime	= g_pConfig->GetServerTime();
					m_i32ErrorCount	= 0;
				}
			}
			break;
		default:
			{
				char logText[MAX_STRING_COUNT];
				sprintf(logText, "[ERROR] RecvPacket - ErrorCode(%d)", GetLastError() );
				WRITE_LOG_RELAY( m_i32WorkerIndex, logText);
			}
			break;
		}
		return FALSE;
	}
	else if( nRv != m_pRecvBuffer->GetBufferSize() )
	{
		char logText[MAX_STRING_COUNT];
		sprintf(logText, "[ERROR] RecvPacketSize - ErrorCode(%d).", GetLastError() );
		WRITE_LOG_RELAY( m_i32WorkerIndex, logText );
		return FALSE;
	}

	return TRUE;
}

BOOL CUdpWorker::SendPacket()
{
	if( 0 == m_pSendBuffer->GetIp() || 0 == m_pSendBuffer->GetPort() )	return FALSE;

	int nRv = ::sendto( m_ServerSocket, m_pSendBuffer->GetBuffer(), m_pSendBuffer->GetBufferSize(), 0, (const sockaddr*)m_pSendBuffer->GetAddress(), sizeof(sockaddr_in) );

	if( SOCKET_ERROR != nRv && nRv == m_pSendBuffer->GetBufferSize() )	return TRUE;
	
	char logText[MAX_STRING_COUNT];
	sprintf(logText, "[ERROR] SendPacket - ErrorCode(%d).", GetLastError() );
	WRITE_LOG_RELAY( m_i32WorkerIndex, logText );

	return FALSE;
}

BOOL CUdpWorker::SendPacket( CUdpBuffer* pRecvBuffer )
{
	if( 0 == pRecvBuffer->GetIp() || 0 == pRecvBuffer->GetPort() )		return FALSE;

	int nRv = ::sendto( m_ServerSocket, pRecvBuffer->GetBuffer(), pRecvBuffer->GetBufferSize(), 0, (const sockaddr*)pRecvBuffer->GetAddress(), sizeof(sockaddr_in) );

	if( SOCKET_ERROR != nRv && nRv == pRecvBuffer->GetBufferSize() )	return TRUE;
	
	char logText[MAX_STRING_COUNT];
	sprintf(logText, "[ERROR] SendPacket - ErrorCode(%d).", GetLastError() );
	WRITE_LOG_RELAY( m_i32WorkerIndex, logText);

	return FALSE;
}

void CUdpWorker::PacketParsing( )
{
	CUdpBuffer* pRecvBuffer = m_pRecvBuffer;

	UINT8 udpProtocol = pRecvBuffer->GetUdpProtocolId();

	switch( udpProtocol & 0x7F )
	{
	case 110:									// 에코 서버 프로토콜입니다. 게임에 사용하지 않습니다.
		{
			pRecvBuffer->ReadAppendData();
			pRecvBuffer->SetAddress( pRecvBuffer->GetIp(), pRecvBuffer->GetPort() );				
			SendPacket( pRecvBuffer );
		}
		break;

	case UDPPROTOCOL_CS_STAGEINFO:				// 현재 사용하지 않습니다.
	case UDPPROTOCOL_CN_INFO:					// 나머지 -> 방장
		{
			// Relay
			pRecvBuffer->ReadAppendData();

			// 방장 IP와 Port를 얻어온다 
			if ( g_pGroupMaker->GetRelayTarget( m_i32WorkerIndex, pRecvBuffer ) )	
			{
				// 성공
                SendPacket( pRecvBuffer );
			}
		}
		break;
	case UDPPROTOCOL_CS_INFO:					// 방장 -> 나머지
		{
			// Relay
			pRecvBuffer->ReadAppendData();

			// 전체에게 보내는 메시지
			if( 0xFF != pRecvBuffer->GetTargetIdx() )
			{
				WRITE_LOG_RELAY( m_i32WorkerIndex, "Impossible" );
			}
			{
				CRelayGroup* pRelayGroup =  g_pGroupMaker->GetGroup( pRecvBuffer->GetGroupIdx() );
				if( NULL == pRelayGroup )										return;

				// 사용중인 방인지 체크
				if( RGS_USE != pRelayGroup->GetGroupState() )					return;

				// 비밀 키값이 맞는지 확인
				if( FALSE == pRelayGroup->CheckEncryptKey( pRecvBuffer->GetEncryptKey() ) )
				{
					char szTemp[MAX_PATH];
					sprintf( szTemp, "[ERROR] UDPPROTOCOL_CS_INFO Encrypt MASTER Group:%d, OrgKey:%d, UserKey:%d, IP:%d, Port:%d", 
						pRecvBuffer->GetGroupIdx(), pRelayGroup->GetEncryptKey(), pRecvBuffer->GetEncryptKey(), pRecvBuffer->GetIp(), pRecvBuffer->GetPort() );
					WRITE_LOG_RELAY( m_i32WorkerIndex, szTemp );
					return;
				}

				pRelayGroup->SetGroupCreateTime();

				// 방안의 유저들에게 전달
				for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
				{
					if( i == pRecvBuffer->GetSlotIdx() )						continue;	// 방장은 받지 않는다.
					if( FALSE == pRelayGroup->CheckMember( (UINT8)i ) )				continue;

					CRelayMember* pMember = pRelayGroup->GetMember( i );
					pRecvBuffer->SetAddress( pMember->GetIp(), pMember->GetPort() );
					SendPacket( pRecvBuffer );
				}
			}
		}
		break;
	case UDPPROTOCOL_SERVER_JOIN_GROUP_REQ:
		{
			RELAY_MEMBER_INFO RelayMemberInfo;

			pRecvBuffer->ReadData(&RelayMemberInfo,			RELAY_MEMBER_RECV_SIZE );

			RelayMemberInfo._wIp	= pRecvBuffer->GetIp();
			RelayMemberInfo._sPort	= pRecvBuffer->GetPort();

			// 릴레이 그룹 가입
			if (g_pGroupMaker->JoinGroup( m_i32WorkerIndex, &RelayMemberInfo, m_sPort ))
			{
				m_pSendBuffer->SetAddress( pRecvBuffer->GetIp(), pRecvBuffer->GetPort() );
				SendPacket();

				I3TRACE( "Join Group Slot:%d, IP:%d, Port:%d \n", RelayMemberInfo._cSlotIdx, RelayMemberInfo._wIp, RelayMemberInfo._sPort);
			}
			else 
			{
				WRITE_LOG_RELAY( m_i32WorkerIndex, "[ERROR] UDPPROTOCOL_SERVER_JOIN_GROUP_REQ");
			}
		}
		break;
	case UDPPROTOCOL_SERVER_WITHDRAW_GROUP_REQ:
		{
			/*RELAY_MEMBER_INFO RelayMemberInfo;

			pRecvBuffer->ReadData(&RelayMemberInfo,			RELAY_MEMBER_RECV_SIZE );

			RelayMemberInfo._wIp	= pRecvBuffer->GetIp();
			RelayMemberInfo._sPort	= pRecvBuffer->GetPort();

			// 릴레이 그룹 탈퇴
			if ( FALSE == g_pGroupMaker->WithdrawGroup( &RelayMemberInfo ))
			{
				//COMMON_LOG("[Error] UDPPROTOCOL_SERVER_WITHDRAW_GROUP_REQ");
			}*/
		}
		break;
	case UDPPROTOCOL_SERVER_JOIN_GROUP_OK:
		// Do nothing
		break;
	default:
		// Do nothing
		break;
	}
}
