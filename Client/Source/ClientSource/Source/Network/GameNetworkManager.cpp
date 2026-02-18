#include "pch.h"
#include "GameNetwork.h"
#include "GameNetworkManager.h"
#include "../GlobalVariables.h"
#include "../GameUDPHandler.h"
#include "BattleServerContext.h"
#include "BattleSlotContext.h"
#include "StageBattle/NetworkContext.h"

#define HEARTBIT_SEND_TIME			60.f

CGameNetworkManager::CGameNetworkManager()
{
	m_pTCPClientSocket	= nullptr;
	m_pReserveSocket	= nullptr;
	m_pUDPClientSocket	= nullptr;
	m_pUDPHolePuncher	= nullptr;
//	m_pUdpRelay			= nullptr;
	m_pUDPRendezvous	= nullptr;
	m_bRendezvousUpdate	= false;
#if defined( THREAD_TCP)
	m_pTCPThread		= nullptr;
#endif

#if !defined( I3_NO_PROFILE )
	NETPROFILE::Reset(&m_ProfileData); 
#endif
}

CGameNetworkManager::~CGameNetworkManager()
{
#if defined( THREAD_TCP)
	I3_SAFE_RELEASE( m_pTCPThread);
#endif
	DestroyTCPSocket();
	DestroyUDPSocket();
}

// revision 40466
bool CGameNetworkManager::GetConnectedMacAddress( UINT64* pMacAddress)
{
#if defined( I3_WINDOWS )
	I3PRINTLOG(I3LOG_QA,"GetMacAddress >>>>>>>>>>>>>>>");

	IP_ADAPTER_INFO AdapterInfo[15];	// NICs 15개까지	제한
	PIP_ADAPTER_INFO pAdapter = nullptr;
	ULONG ulOutBufLen = sizeof(AdapterInfo);

	if( ::GetAdaptersInfo(AdapterInfo, &ulOutBufLen) == ERROR_SUCCESS )
	{
		// revision 40630
		SOCKADDR_IN addr = GetTCPSocket()->GetAddr();

		char* pszSocketIp = ::inet_ntoa( addr.sin_addr);
		IP_ADDR_STRING* pIpAddr = nullptr;

		pAdapter = AdapterInfo;					
		while( pAdapter)
		{//Adapter Info

			if( MIB_IF_TYPE_ETHERNET != pAdapter->Type && IF_TYPE_IEEE80211 != pAdapter->Type )
			{
				pAdapter = pAdapter->Next;
				continue;
			}

			//I3PRINTLOG(I3LOG_NOTICE," [Adaptor Info] - %d", cnt);
			//I3PRINTLOG(I3LOG_NOTICE,"	Name : %s", pAdapter->AdapterName);
			//I3PRINTLOG(I3LOG_NOTICE,"	Desc : %s", pAdapter->Description);
			//I3PRINTLOG(I3LOG_NOTICE,"	Mac : %s", pAdapter->Address);
			//I3PRINTLOG(I3LOG_NOTICE,"	Type : %d", pAdapter->Type);

			pIpAddr = &pAdapter->IpAddressList;	
			while( pIpAddr)
			{//IP Info
				//I3PRINTLOG(I3LOG_NOTICE,"	IP : %s", pIpAddr->IpAddress.String);
				//I3PRINTLOG(I3LOG_NOTICE,"	MASK : %s", pIpAddr->IpMask.String);

				if( strcmp( pIpAddr->IpAddress.String, pszSocketIp) == 0)
				{
					i3mem::Copy( pMacAddress, pAdapter->Address, sizeof(UINT64));

					//I3PRINTLOG(I3LOG_NOTICE,"******* Find Connect Socket IP : %s ********", pIpAddr->IpAddress.String);
					I3PRINTLOG(I3LOG_NOTICE,"GetMacAddress <<<<<<<<<<<<<<<<<");
					return true;
				}
				pIpAddr = pIpAddr->Next;
			}

			pAdapter = pAdapter->Next;
		}
	}
#endif
	*pMacAddress = 0;

	I3PRINTLOG(I3LOG_NOTICE,"GetMacAddress <<<<<<<<<<<<<<<<<(fail)");

	return false;
}

void CGameNetworkManager::CreateTCPSocket( void)
{
	m_pTCPClientSocket	= ClientTCPSocket::new_object();
#if defined( I3_DEBUG)
	m_pTCPClientSocket->m_iID = 0;
#endif

	m_pReserveSocket	= ClientTCPSocket::new_object();

	#if defined( I3_DEBUG)
		m_pReserveSocket->m_iID = 1;
	#endif

#if defined(RUSSIA_RESET_SOCKET)
	I3_SAFE_RELEASE( m_pTCPClientSocket );		// revision 46973 로그인 시 발생하는 크래시 수정
	m_pTCPClientSocket	= ClientTCPSocket::NewObject();
	m_pTCPClientSocket->m_iID = 0;
#endif

	_CreateTcpThread();
}

void CGameNetworkManager::_CreateTcpThread( void)
{
#if defined( THREAD_TCP)
	I3ASSERT( m_pTCPThread == nullptr);
	m_pTCPThread = cThreadTCP::new_object();
	m_pTCPThread->Create( "TCPThread", 0, 1024, nullptr, PRIORITY_HIGHEST);
#endif
}

bool CGameNetworkManager::ConnectTCP( char * IP, UINT16 Port)
{
	I3ASSERT( m_pTCPClientSocket != nullptr);

	if( m_pTCPClientSocket->OnCreate( IP, g_pServerInfo->GetLogInServerPort()) == false)
	{	// 접속 실패
		m_pTCPClientSocket->ForceDestroy();
		return false;
	}

	return true;
}

bool CGameNetworkManager::ConnectReserveToGateway( char * IP, UINT16 Port)
{
	I3ASSERT( m_pReserveSocket != nullptr);

	m_pReserveSocket->SetTargetServerType(SERVER_TYPE_GATEWAY);

	//Create Tcp (Server Connect)
	if( m_pReserveSocket->OnCreate( IP, Port) == false)
	{
		// 접속 실패
		m_pReserveSocket->ForceDestroy();
		return false;
	}
	return true;
}

bool CGameNetworkManager::ConnectReserveToGame( char * IP, UINT16 Port)
{
	I3ASSERT( m_pReserveSocket != nullptr);

	m_pReserveSocket->SetTargetServerType(SERVER_TYPE_GAME);

	//Create Tcp (Server Connect)
	if( m_pReserveSocket->OnCreate( IP, Port) == false)
	{
		// 접속 실패
		m_pReserveSocket->ForceDestroy();
		return false;
	}

	return true;
}

void CGameNetworkManager::DisconnectTCP( void)
{
	if( m_pTCPClientSocket != nullptr )
	{
		m_pTCPClientSocket->ForceDestroy();
	}

	I3TRACE0( "CGameNetworkManager::Disconnect\n");
}

void CGameNetworkManager::DisconnectReserve( void)
{
	I3ASSERT(m_pReserveSocket->IsConnected() == false);
	m_pReserveSocket->ForceDestroy();
}

void CGameNetworkManager::Update( REAL32 DeltaTime)
{
	I3ASSERT( m_pTCPClientSocket != nullptr);

	//1 . Server Packet Parsing
	if (m_pTCPClientSocket->IsConnected())
		m_pTCPClientSocket->SelectEvent();

	I3ASSERT( m_pReserveSocket != nullptr);
	if (m_pReserveSocket->IsConnected())
		m_pReserveSocket->SelectEvent();

	if( BattleServerContext::i()->IsP2PActivate())
	{
		I3ASSERT( m_pUDPHolePuncher != nullptr);
		m_pUDPHolePuncher->OnUpdate( DeltaTime );

		CGameUDPHandler::i()->P2PUpdate( DeltaTime);
	}
}

void CGameNetworkManager::UpdateHeartbeat( REAL32 AccTime)
{
	I3ASSERT( m_pTCPClientSocket != nullptr);

	if (m_pTCPClientSocket->IsConnected())
	{
		if( (m_pTCPClientSocket->GetLastSendTime() + HEARTBIT_SEND_TIME) < AccTime)
		{
			I3TRACE("Send Heart Bit %f \n", AccTime);
			m_pTCPClientSocket->SendHeartBitMessage();
		}
	}

	I3ASSERT( m_pReserveSocket != nullptr);

	if (m_pReserveSocket->IsConnected())
	{
		if ( (m_pReserveSocket->GetLastSendTime() + HEARTBIT_SEND_TIME) < AccTime )
		{
			I3TRACE( "Send Heart Bit %f (ReserveSocket)\n", AccTime);
			m_pReserveSocket->SendHeartBitMessage();
		}
	}
}

void CGameNetworkManager::ChangeSocket(void)
{
	I3TRACE0("CGameContext::ChangeSocket\n");
	I3ASSERT( m_pTCPClientSocket != nullptr);
	I3ASSERT( m_pReserveSocket != nullptr);

	if (m_pTCPClientSocket->IsConnected())
	{
		DEV_LOG("   ForceDestroy Socket");
		DEV_LOG("     %d / %s", m_pTCPClientSocket->GetConnectionInfo().m_SessionIndex, inet_ntoa(m_pTCPClientSocket->m_InAddr.sin_addr));
		m_pTCPClientSocket->ForceDestroy();
	}

	swap(m_pTCPClientSocket, m_pReserveSocket);

	DEV_LOG("   NewSocket");
	DEV_LOG("     %d / %s", m_pTCPClientSocket->GetConnectionInfo().m_SessionIndex, inet_ntoa(m_pTCPClientSocket->m_InAddr.sin_addr));

	m_pTCPClientSocket->m_UsingType		= 1;
	m_pReserveSocket->m_UsingType		= 2;
}

void CGameNetworkManager::DestroyTCPSocket( void)
{	
	I3_SAFE_RELEASE( m_pTCPClientSocket );
	I3_SAFE_RELEASE( m_pReserveSocket );
}

void CGameNetworkManager::SetEnteredServer( bool bEnter)
{
	if( m_pTCPClientSocket != nullptr)
		m_pTCPClientSocket->SetEnteredServer( bEnter);
}

bool CGameNetworkManager::GetEnteredServer( void)
{
	if( m_pTCPClientSocket != nullptr)
		return m_pTCPClientSocket->GetEnteredServer();
	return false;
}

bool CGameNetworkManager::IsTCPConnected( void)
{
	I3ASSERT( m_pTCPClientSocket != nullptr);
	return (bool)(m_pTCPClientSocket->IsConnected() ? true : false);
}

bool CGameNetworkManager::IsRealIP( void)
{
	if( m_pTCPClientSocket != nullptr)
		return (bool)(m_pTCPClientSocket->GetConnectionInfo().m_IsRealIp ? true : false);

	return false;
}

INT32 CGameNetworkManager::GetSessionIndex( void)
{
	I3ASSERT( m_pTCPClientSocket != nullptr);
	return m_pTCPClientSocket->GetConnectionInfo().m_SessionIndex;
}

UINT16 CGameNetworkManager::GetGamePort( void)
{
	I3ASSERT( m_pTCPClientSocket != nullptr);
	return m_pTCPClientSocket->GetConnectionInfo().m_GamePort;
}

void CGameNetworkManager::SetGamePort( UINT16 Port)
{
	I3ASSERT( m_pTCPClientSocket != nullptr);
	m_pTCPClientSocket->SetConnectionUDPPort( Port );
}

const NET_UDP_INFO * CGameNetworkManager::GetMyNetInfo( void ) const
{
	return &m_pTCPClientSocket->GetConnectionInfo().m_MyUdpInfo;
}

NET_UDP_INFO * CGameNetworkManager::SetMyNetInfo( void )
{
	return &m_pTCPClientSocket->GetUDPInfo();
}

bool CGameNetworkManager::SendTCPMessage( i3NetworkPacket * pPacket)
{
	return (bool)(m_pTCPClientSocket->SendMessage( pPacket) > 0);
}

bool CGameNetworkManager::SendTCPMessage( S2MOPacketBase* pS2MOPacket )
{
	return (bool)(m_pTCPClientSocket->SendMessage( pS2MOPacket ) > 0);
}

bool CGameNetworkManager::SendReserveMessage( i3NetworkPacket * pPacket)
{
	return (bool)(m_pReserveSocket->SendMessage( pPacket) > 0);
}

bool CGameNetworkManager::IsReserveConnected( void)
{
	if( m_pReserveSocket != nullptr)
		return (bool)(m_pReserveSocket->IsConnected() ? true : false);

	return false;
}

void CGameNetworkManager::CreateUDPSocket( UINT16 Port)
{
	m_pUDPClientSocket = ClientUDPSocket::new_object();
	m_pUDPClientSocket->Create( Port );

	m_pUDPHolePuncher = CUdpHolePuncherForClient::new_object();
	m_pUDPHolePuncher->OnCreate();
}

void CGameNetworkManager::DestroyUDPSocket( void)
{
	I3PRINTLOG(I3LOG_NOTICE, "m_pUDPHolePuncher Release" ); 
	I3_SAFE_RELEASE( m_pUDPHolePuncher	);

	if( CGameUDPHandler::i() != nullptr)
		CGameUDPHandler::i()->UDPDisconnect();

	if(m_pUDPClientSocket != nullptr)
	{
		I3PRINTLOG(I3LOG_NOTICE, "m_pUDPClientSocket->CloseSocket()" );
		m_pUDPClientSocket->CloseSocket();
		Sleep(500);				//땜빵 성백 20080508
	}

	I3PRINTLOG(I3LOG_NOTICE, "m_pUDPClientSocket Release" );
	I3_SAFE_RELEASE( m_pUDPClientSocket );
}

void CGameNetworkManager::RelayHolePunchEndSend( UINT32 IP, UINT16 Port)
{
	I3ASSERT( m_pUDPHolePuncher != nullptr);
	m_pUDPHolePuncher->RelayHolePunchEndSend( IP, Port );
}

void CGameNetworkManager::InitRound( void)
{
	// UDP 처리용 버퍼 초기화
	CGameUDPHandler::i()->InitRound();
}

void CGameNetworkManager::InitUser( INT32 userIdx)
{
	CGameUDPHandler::i()->InitUserInfo( userIdx);
}

void CGameNetworkManager::Respawn( INT32 userIdx)
{
	CGameUDPHandler::i()->RespawnChara( userIdx);
}

void CGameNetworkManager::IntrudeBattle( bool bHost)
{
	CGameUDPHandler::i()->UDPConnect( bHost);
}

void CGameNetworkManager::PreStartBattleProcess( INT32 userIdx, bool bHost, bool bUseBattleServer)
{
	if( bUseBattleServer )
	{	// Releay 서버와 홀펀칭
		m_pUDPHolePuncher->setServerUDPState( SERVER_UDP_STATE_RELAY );

		if( bHost)
		{	
			if( userIdx != BattleSlotContext::i()->getMySlotIdx())
			{
				//클라이언트랑 시작
				CGameUDPHandler::i()->ResetRecvTime( userIdx);
			}
		}
	}
	else
	{	// UDP를 클라이언트와 할수 있도록 변경
		m_pUDPHolePuncher->setServerUDPState( SERVER_UDP_STATE_CLIENT );

		if( bHost)
		{
			if( userIdx != BattleSlotContext::i()->getMySlotIdx())
				m_pUDPHolePuncher->ClientStart( userIdx);
		}
		else if( userIdx == BattleSlotContext::i()->getMySlotIdx())
		{
			m_pUDPHolePuncher->ClientStart( BattleSlotContext::i()->getMainSlotIdx() );
			I3PRINTLOG(I3LOG_NOTICE, "============ PreStartBattle Ack=============");
		}
	}

	return; 
}

void CGameNetworkManager::StartBattleProcess( INT32 userIdx, INT32 MyIndex, bool bHost)
{
	CGameUDPHandler::i()->StartBattle( userIdx);

	if( bHost && (userIdx != MyIndex) )
	{	
		// 난입한 유저에게 Stage 정보를 보낸다.
		CGameUDPHandler::i()->RequestIntrudeProcess();
	}
}

void CGameNetworkManager::StartBattleUserProcess( INT32 userIdx)
{
	CGameUDPHandler::i()->StartBattle( userIdx);
}

void CGameNetworkManager::ChangeHostProcess( INT32 MainSlotIdx, bool bHost, bool bUseBattleServer)
{
	INT32 i;

	I3ASSERT( m_pUDPHolePuncher != nullptr);


	CGameUDPHandler::i()->ResetRecvTime( MainSlotIdx);
	CGameUDPHandler::i()->ChangeHost( bHost);

	if( bUseBattleServer )
		m_pUDPHolePuncher->setServerUDPState( SERVER_UDP_STATE_RELAY );
	else
		m_pUDPHolePuncher->setServerUDPState( SERVER_UDP_STATE_CLIENT );

	if( bHost )
	{
		// 홀펀칭을 모든 클라이언트와 한다..
		for( i = 0; i < USER_SLOT_COUNT; i++ )
		{
			if( i == BattleSlotContext::i()->getMySlotIdx() )
				continue;

			// 중요!! 방장은 홀펀칭을 원하는 사람과 해야됩니다. (이미 로딩이 끝나는 시점도 확인해야함)
			if( BattleSlotContext::i()->getSlotInfo( i )->_State > SLOT_STATE_LOAD )
			{
				m_pUDPHolePuncher->ClientStart( i);
			}
		}
	}
	else
	{
		// 클라이언트는 새로운 방장과 홀펀칭을 하도록
		m_pUDPHolePuncher->ClientStart( BattleSlotContext::i()->getMainSlotIdx() );
	}
}

void CGameNetworkManager::UserLeaveBattleProcess( INT32 userIdx, bool bUseRelayServer)
{
	if( !bUseRelayServer)
	{
		if( m_pUDPHolePuncher)
		{
			m_pUDPHolePuncher->ClientGameLeave( userIdx );
		}
	}
}

#if defined( THREAD_TCP)
I3_CLASS_INSTANCE( cThreadTCP);
cThreadTCP::cThreadTCP() : m_ElapsTime(0.f)
{
	m_pTimer = i3Timer::new_object();
	_setSignalTime( 10000);	// 10초에 한번씩 깨우자
}
cThreadTCP::~cThreadTCP()
{
	//Stop();

	I3_SAFE_RELEASE( m_pTimer);
}
void cThreadTCP::Run( void * pData)
{
	ClientTCPSocket * pTCPClientSocket = CGameNetworkManager::i()->GetTCPSocket();

	if (pTCPClientSocket == nullptr)
	{
		return;
	}

	if( pTCPClientSocket->IsConnected() )
	{
		I3ASSERT( pTCPClientSocket != nullptr);

		if( (pTCPClientSocket->GetLastSendTime() + HEARTBIT_SEND_TIME) < m_ElapsTime)
		{
			I3TRACE("Send Heart Bit %f \n", m_ElapsTime);
			pTCPClientSocket->SendHeartBitMessage();

			m_ElapsTime = pTCPClientSocket->GetLastSendTime();
		}

		m_ElapsTime += m_pTimer->GetDeltaSec();
	}
}

#endif


#if defined( I3_DEBUG)
void CGameNetworkManager::Dump( void)
{
	I3ASSERT( m_pTCPClientSocket != nullptr);

	I3PRINTLOG(I3LOG_NOTICE,"TCP Send %d %d",	m_pTCPClientSocket->m_iSendPacketCount, m_pTCPClientSocket->m_iSendPacketSize);
	I3PRINTLOG(I3LOG_NOTICE,"TCP Recv %d %d",	m_pTCPClientSocket->m_iRecvPacketCount, m_pTCPClientSocket->m_iRecvPacketSize);
}

void CGameNetworkManager::RoundEndDump( REAL32 GameTime)
{
	/*
	if( m_pUDPReceiver != nullptr)
	{
		I3PRINTLOG(I3LOG_NOTICE, "EndGame : %d, %d, %d", (INT32)GameTime, m_pUDPReceiver->m_nLostTimeCount, g_pCharaManager->getCharaCount() );
		I3PRINTLOG(I3LOG_NOTICE, "Send	: %d, %d", m_pUDPClientSocket->m_nSendCount, m_pUDPClientSocket->m_nSendBytes);
		I3PRINTLOG(I3LOG_NOTICE, "Recv	: %d, %d", m_pUDPClientSocket->m_nRecvCount, m_pUDPClientSocket->m_nRecvBytes);
	}
	*/
}

#endif


