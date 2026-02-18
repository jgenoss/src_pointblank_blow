#include "pch.h"
#include "Gateway.h"
#include "Receiver.h"
#include "TaskProcessor.h"

I3_CLASS_INSTANCE(CGateway, i3Thread);

CGateway* g_pGateway = NULL;

CGateway::CGateway()
{	
	m_bIsRunning = FALSE;
	m_ppReceiver = NULL;
}

CGateway::~CGateway()
{
	OnDestroy();
}

BOOL CGateway::Create( UINT32 ui32IPAddress, UINT16 ui16Port, CPacketLocker* pPacketLocker)
{
	I3ASSERT(NULL == m_ppReceiver);
	I3ASSERT(g_pConfig);
	I3ASSERT(pPacketLocker);


	// 서버 소켓 생성
	if( m_ServerSocket.OnCreate(ui32IPAddress, ui16Port) == FALSE )
		goto socket_destroy;

	// 리시버 생성
	m_ui32ReceiverCount = g_pConfig->m_ui32ServerCount;

	m_ppReceiver = (CReceiver**)i3MemAlloc(sizeof(CReceiver*) * m_ui32ReceiverCount);
	i3mem::FillZero( m_ppReceiver, sizeof(CReceiver*) * m_ui32ReceiverCount );

	for(UINT32 i = 0; i < m_ui32ReceiverCount; i++)
	{
		m_ppReceiver[i] = CReceiver::NewObject();
		m_ppReceiver[i]->Create(i, pPacketLocker);
	}

	

	if( i3Thread::Create("Gateway", CREATE_SUSPENDED) == FALSE )
		goto socket_destroy;
	
	return TRUE;


socket_destroy:

	m_ServerSocket.OnDestroy();

	return FALSE;
}

void CGateway::OnDestroy()
{
	m_bIsRunning = FALSE;
	i3Thread::WaitForFinish();

	if (m_ppReceiver)
	{
		for(UINT32 i = 0; i < m_ui32ReceiverCount; i++)
		{
			I3_SAFE_RELEASE(m_ppReceiver[i]);
		}

		I3MEM_SAFE_FREE(m_ppReceiver);
	}

	m_ServerSocket.OnDestroy();
}

BOOL CGateway::SendPacket(UINT32 ui32idxReceiver,i3NetworkPacket* pPacket)
{
	if (ui32idxReceiver < m_ui32ReceiverCount)
	{
		return m_ppReceiver[ui32idxReceiver]->SendPacket(pPacket);
	}

	return FALSE;
}

UINT32 CGateway::OnRunning(void* pUserData)
{
	m_i32LogIdx = g_pLog->AddLogThread(L"GATE", 0, m_ThreadID);

	m_bIsRunning = TRUE;
	
	while(m_bIsRunning)
	{
		AcceptGameServer();
		::Sleep(1);
	}

	return 0;
}

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

BOOL CGateway::IsConnectedGameServer(UINT32 ui32idx)
{
	if (NULL == m_ppReceiver)	return FALSE;

	return m_ppReceiver[ui32idx]->IsWork();
}

void CGateway::AcceptGameServer(void)
{
	struct timeval timeout = { 0, I3ACCEPTOR_RECV_TIME};
	struct sockaddr_in FromAddr; 
	INT32  i32AddrSize = sizeof(sockaddr_in);

	// 게임서버 연결 확인
	SOCKET SocketAccept = m_ServerSocket.AcceptConnection(&timeout, &FromAddr, &i32AddrSize);
	if( SocketAccept == INVALID_SOCKET )
		return;

	for( UINT32 i = 0; i < g_pConfig->m_ui32ServerCount; ++i )
	{	
		if( FromAddr.sin_addr.s_addr != g_pConfig->m_pui32ServerIp[i] )		// IP가 GameServer 인가?	
			continue;

		if( m_ppReceiver[i]->SetSocket(SocketAccept) == FALSE )
		{
			if( !m_ppReceiver[i]->IsDisconnecting() )	// 이미 종료 중이면, 종료요청 필요없다
			{
				g_pLog->WriteLog( L"Not disconnecting state Idx : %d", i );
				m_ppReceiver[i]->Disconnect();
			}

			INT32 i32count = 0;
			for( ;; )
			{
				if( m_ppReceiver[i]->SetSocket(SocketAccept) )
					break;

				i32count++;
				if( 1000 < i32count )
				{
					::shutdown(SocketAccept, SD_BOTH);
					::closesocket(SocketAccept);

					g_pLog->WriteLog( L"Disconnected - Failed SetSocket by DuplicateConnted");

					return;
				}

				::Sleep(1);
			}

			g_pLog->WriteLog( L"DuplicateConnected GameServer %d", i);
		}

		g_pLog->WriteLog( L"Connect GameServer %d", i);

		UINT8 ui8SectionSize = CLAN_SECTION_SIZE;
		INT32 SeasonRankingCount = g_pTaskProcessor->GetSeasonRankingCount();

		i3NetworkPacket	StartPacket(PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS);	
		StartPacket.WriteData(&g_pTaskProcessor->GetClanContext()->_totalCount,		sizeof(UINT32));
		StartPacket.WriteData(&ui8SectionSize,										sizeof(UINT8));
		StartPacket.WriteData(&g_pTaskProcessor->GetClanContext()->_sectionCount,	sizeof(UINT16));
		StartPacket.WriteData(&g_pTaskProcessor->GetClanContext()->_version,		sizeof(UINT32));
		StartPacket.WriteData(&SeasonRankingCount,									sizeof(INT32));
		StartPacket.WriteData(g_pTaskProcessor->GetSeasonRankingClan(),				(UINT16)(sizeof( CLAN_BASIC_INFO ) * SeasonRankingCount) );

		m_ppReceiver[i]->SendPacket(&StartPacket);

		break;
	}
}

