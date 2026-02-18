#include "pch.h"
#include "Gateway.h"
#include "Receiver.h"

I3_CLASS_INSTANCE(CGateway, i3Thread);

CGateway* g_pGateway = NULL;

CGateway::CGateway()
{
	m_bIsRunning = FALSE;
	m_ppReceiver = NULL;
}

CGateway::~CGateway()
{
	if (m_ppReceiver)
	{
		for(UINT32 i = 0; i < m_ui32ReceiverCount; i++)
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
///   리시버와 패킷 퍼버 연결
/// param strIp			자신의 ip - 호스트, 클라이언트 양쪽으로 사용
/// param portNum		열어둘 port - 호스트로서의 포트
/// param pPacketLocker 패킷버퍼 관리 클래스
/// return				성공여부
BOOL CGateway::Create(char* pstrIp,INT16 ui16portNum,CPacketLocker* pPacketLocker)
{
	I3ASSERT(NULL == m_ppReceiver);
	I3ASSERT(g_pConfig);
	I3ASSERT(pPacketLocker);

	// 서버 소켓 생성
	if (FALSE == m_ServerSocket.OnCreate(inet_addr(pstrIp), ui16portNum))
	{
		goto socket_destroy;
	}

	// 리시버 생성
	m_ui32ReceiverCount = g_pConfig->m_ui32ServerCount;

	m_ppReceiver = (CReceiver**)i3MemAlloc(sizeof(CReceiver*) * m_ui32ReceiverCount);

	for(UINT32 i = 0; i < m_ui32ReceiverCount; i++)
	{
		m_ppReceiver[i] = CReceiver::NewObject();
		m_ppReceiver[i]->Create(i, pPacketLocker);
	}
	
	if (FALSE == i3Thread::Create("MessengerGateway"))
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
BOOL CGateway::SendPacket(UINT32 ui32idxReceiver,i3NetworkPacket* pPacket)
{
	if (ui32idxReceiver < m_ui32ReceiverCount)
	{
		return m_ppReceiver[ui32idxReceiver]->SendPacket(pPacket);
	}

	//char message[MAX_STRING_COUNT];
	//i3String::Format(message, "[ERROR] CGateway::SendPacket() - idx overflow(%d) Protocol ID(%d)", idxReceiver, pPacket->GetProtocolID());
	//COMMON_LOG(message);
	//쓰레드 확인하고 보내야함.

	return FALSE;
}

/// 연결 끊기 요청
/// param idxReceiver	연결을 끊을 리시버
void CGateway::RequestDisconnect(UINT32 ui32idxReceiver)
{
	if (ui32idxReceiver < m_ui32ReceiverCount)
	{
		m_ppReceiver[ui32idxReceiver]->RequestDisconnect();
		return;
	}

	//char message[MAX_STRING_COUNT];
	//i3String::Format(message, "[ERROR] CGateway::RequestDisconnect() - idx overflow(%d)", idxReceiver);
	//COMMON_LOG(message);
	//쓰레드 확인하고 보내야함.
}

/// 쓰래드
/// param pUserData		사용하지 않는다
/// return				항상 0
UINT32 CGateway::OnRunning(void* pUserData)
{
	m_i32LogIdx = g_pLog->AddLogThread(L"MSGW", 0, m_ThreadID);

	m_bIsRunning = TRUE;

	while(m_bIsRunning)
	{
		//접속 여부 확인 하는 프로세쓰 
		AcceptGameServer();
		g_pConfig->OnUpdate();

		::Sleep(1);
	}

	return 0;
}

/// 연결된 게임서버의 수를 받는다.
/// return	접속 중인 게임서버의 수
UINT32 CGateway::GetConnectedGameServerCount(void)
{
	UINT32 ui32Count = 0;
	
	for(UINT32 i = 0; i < g_pConfig->m_ui32ServerCount; i++)
	{
		if (m_ppReceiver[i]->IsWork())
		{
			ui32Count++;
		}
	}

	return ui32Count;
}

/// 게임서버가 연결 중인지 확인한다
/// param idx	게임서버 배열 idx
/// return		연결여부
BOOL CGateway::IsConnectedGameServer(UINT32 ui32idx)
{
	if (NULL == m_ppReceiver)	return FALSE;

	return m_ppReceiver[ui32idx]->IsWork();
}

/// 게임서버의 접속을 기다린다.
void CGateway::AcceptGameServer(void)
{
	struct timeval timeout = { 0, I3ACCEPTOR_RECV_TIME};
	struct sockaddr_in FromAddr; 
	INT32  i32AddrSize = sizeof(sockaddr_in);

	// 게임서버 연결 확인
	SOCKET SocketAccept = INVALID_SOCKET;
	SocketAccept = m_ServerSocket.AcceptConnection( &timeout, &FromAddr, &i32AddrSize);

	if (INVALID_SOCKET != SocketAccept)
	{
		for(UINT32 i = 0; i < g_pConfig->m_ui32ServerCount; i++)
		{
			// IP가 GameServer 인가?				
			if (FromAddr.sin_addr.s_addr == g_pConfig->m_pui32ServerIp[i])
			{
				if (FALSE == m_ppReceiver[i]->SetSocket(SocketAccept))
				{
					// 이미 종료 중이면, 종료요청 필요없다
					if (!m_ppReceiver[i]->IsDisconnecting())
					{
						g_pLog->WriteLog( L"not disconnecting state"  );
						m_ppReceiver[i]->Disconnect();
					}


					INT32 i32count = 0;
					for(;;)
					{
						if (m_ppReceiver[i]->SetSocket(SocketAccept))
						{
							break;
						}

						i32count++;


						if (i32count > 1000)
						{
							::shutdown(SocketAccept, SD_BOTH);
							::closesocket(SocketAccept);

							g_pLog->WriteLog( L"Disconnected - Failed SetSocket by DuplicateConnted"   );

							return;
						}

						::Sleep(1);
					}


					g_pLog->WriteLog( L"DuplicateConnected GameServer %d", i   );
				}

				// 연결 통보
				i3NetworkPacket	StartPacket(PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS);	

				g_pLog->WriteLog( L"Connect GameServer %d", i  );

				m_ppReceiver[i]->SendPacket(&StartPacket);
				break;
			}
		}
	}
}
