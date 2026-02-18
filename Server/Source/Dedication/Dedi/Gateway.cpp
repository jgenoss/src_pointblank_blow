#include "pch.h"
#include "Gateway.h"
#include "Receiver.h"
#include "LogFile.h"

I3_CLASS_INSTANCE(CGateway, i3Thread);

CGateway* g_pGateway = NULL;

CGateway::CGateway()
{	
	m_IsRunning = FALSE;
	m_ppReceiver = NULL;
}

CGateway::~CGateway()
{
	if (m_ppReceiver)
	{
		for(UINT32 i = 0; i < m_nReceiverCount; i++)
		{
			m_ppReceiver[i]->Terminate();
			I3_SAFE_RELEASE(m_ppReceiver[i]);
		}

		I3MEM_SAFE_FREE(m_ppReceiver);
	}
}

/// 생성
///   소켓 생성
///   리시버 생성
///   리시버와 패킷버퍼 연결
/// param strIp			자신의 ip - 호스트, 클라이언트 양쪽으로 사용
/// param portNum		열어둘 port - 호스트로서의 포트
/// param pPacketLocker 패킷버퍼 관리 클래스
/// return				성공여부
BOOL CGateway::Create(char* strIp,INT16 portNum,CPacketLocker* pPacketLocker)
{
	I3ASSERT(NULL == m_ppReceiver);
	I3ASSERT(g_pConfig);
	I3ASSERT(pPacketLocker);

	// 서버 소켓 생성
	if (FALSE == m_ServerSocket.OnCreate(inet_addr(strIp), portNum))
	{
		goto socket_destroy;
	}

	// 리시버 생성
	m_nReceiverCount = g_pConfig->m_ServerCount;

	m_ppReceiver = (CReceiver**)i3MemAlloc(sizeof(CReceiver*) * m_nReceiverCount);

	for(UINT32 i = 0; i < m_nReceiverCount; i++)
	{
		m_ppReceiver[i] = CReceiver::NewObject();
		m_ppReceiver[i]->Create(i, pPacketLocker);
	}


	if (FALSE == i3Thread::Create("RelayGateway"))
	{
		goto socket_destroy;
	}

	return TRUE;


socket_destroy:

	m_ServerSocket.OnDestroy();

	return FALSE;
}

/// 클라이언트로 패킷 전송
/// param idxReceiver	클라이언트 배열 idx
/// param pPacket		패킷
/// return				성공여부
BOOL CGateway::SendPacket(UINT32 idxReceiver,i3NetworkPacket* pPacket)
{
	if (idxReceiver < m_nReceiverCount)
	{
		return m_ppReceiver[idxReceiver]->SendPacket(pPacket);		
	}

	char message[MAX_STRING_COUNT];
	sprintf(message, "[ERROR] CGateway::SendPacket() - idx overflow(%d)", idxReceiver);
	//WRITE_LOG(message);

	return FALSE;
}

/// 연결 끊기 요청
/// param idxReceiver	연결을 끊을 리시버
void CGateway::RequestDisconnect(UINT32 idxReceiver)
{
	if (idxReceiver < m_nReceiverCount)
	{
		m_ppReceiver[idxReceiver]->RequestDisconnect();
		return;
	}

	char message[MAX_STRING_COUNT];
	sprintf(message, "[ERROR] CGateway::RequestDisconnect() - idx overflow(%d)", idxReceiver);
	//WRITE_LOG(message);
}

/// 쓰래드
/// param pUserData		사용하지 않는다
/// return				항상 0
UINT32 CGateway::OnRunning(void* pUserData)
{
	m_IsRunning = TRUE;

	while(m_IsRunning)
	{
		AcceptGameServer();        

		::Sleep(1);
	}

	return 0;
}

/// 연결된 게임서버의 수를 받는다.
/// return	접속 중인 게임서버의 수
UINT32 CGateway::GetConnectedGameServerCount(void)
{
	UINT32 nCount = 0;
	
	for(UINT32 i = 0; i < g_pConfig->m_ServerCount; i++)
	{
		if (m_ppReceiver[i]->IsWork())
		{
			nCount++;
		}
	}

	return nCount;
}

/// 게임서버가 연결 중인지 확인한다
/// param idx	게임서버 배열 idx
/// return		연결여부
BOOL CGateway::IsConnectedGameServer(UINT32 idx)
{
	if (NULL == m_ppReceiver)	return FALSE;

	return m_ppReceiver[idx]->IsWork();
}

/// 게임서버의 접속을 기다린다.
void CGateway::AcceptGameServer(void)
{
	struct timeval timeout = { 0, I3ACCEPTOR_RECV_TIME};
	struct sockaddr_in FromAddr; 
	INT32  AddrSize = sizeof(sockaddr_in);

	// 게임서버 연결 확인
	SOCKET SocketAccept = INVALID_SOCKET;
	SocketAccept = m_ServerSocket.AcceptConnection( &timeout, &FromAddr, &AddrSize);

	if (INVALID_SOCKET != SocketAccept)
	{
		for(UINT32 i = 0; i < g_pConfig->m_ServerCount; i++)
		{
			// IP가 GameServer 인가?				
			if (FromAddr.sin_addr.s_addr == g_pConfig->m_pServerIp[i])
			{
				BOOL addServerCount = TRUE;
				if (FALSE == m_ppReceiver[i]->SetSocket(SocketAccept))
				{
					// 이미 종료 중이면, 종료요청 필요없다
					if (!m_ppReceiver[i]->IsDisconnecting())
					{
						//WRITE_LOG(i, "not disconnecting state");

						m_ppReceiver[i]->Disconnect();
						addServerCount = FALSE;
					}


					INT32 count = 0;
					for(;;)
					{
						if (m_ppReceiver[i]->SetSocket(SocketAccept))
						{
							break;
						}

						count++;


						if (count > 1000)
						{
							::shutdown(SocketAccept, SD_BOTH);
							::closesocket(SocketAccept);

							char message[MAX_STRING_COUNT];
							sprintf(message, "Disconnected - Failed SetSocket by DuplicateConnted");
							//WRITE_LOG(i, message);

							return;
						}

						::Sleep(1);
					}


					char strBuf[MAX_STRING_COUNT];
					sprintf(strBuf, "DuplicateConnected GameServer %d", i);
					//WRITE_LOG(i, strBuf);
				}

				//g_ConnectServer = g_ConnectServer | (0x01 << i);
				
				if (addServerCount)
				{
					g_pConfig->AddConnectedGameServer();
				}
				
				// 연결 통보 - Client가 접속할 수 있는 외부 UDP IP/Port를 알려준다.
				i3NetworkPacket	StartPacket(PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS);	
				StartPacket.WriteData(&g_pConfig->m_UdpIp,			sizeof(UINT32));
				StartPacket.WriteData(&g_pConfig->m_cUdpPortCount,	sizeof(UINT32));
				StartPacket.WriteData(g_pConfig->m_pUdpPort,		(UINT16)(sizeof(UINT16)*g_pConfig->m_cUdpPortCount) );

				char strBuf[MAX_STRING_COUNT];
				sprintf(strBuf, "Connect GameServer %d", i);
				//WRITE_LOG(i, strBuf);

				m_ppReceiver[i]->SendPacket(&StartPacket);
				break;
			}
		}
	}
}
