#include "pch.h"
#include "UDPPacketReceiver.h"
#include "../GlobalVariables.h"
#include "ClientUDPSocket.h"
#include "UdpHolePuncherForClient.h"
#include "UdpRelay.h"
#include "UDPRendezvous.h"

#include "../StageBattle/UserContext.h"
#include "../StageBattle/NetworkContext.h"
#include "BattleServerContext.h"
#include "BattleSlotContext.h"
#include "GameNetworkManager.h"
#include "BattleSyncTimer.h"

I3_CLASS_INSTANCE( UDPPacketReceiver);//, i3Thread );

UDPPacketReceiver::UDPPacketReceiver() : m_bIsRun(false), m_bExitResult(true), m_DediRecvTime(0.0f)
	, m_WriteIdx(0), m_ReadIdx(0), m_iRecvPlayerInfoIdx(0)
	, m_iRecvAIDinosInfoIdx(0), m_iRecvGrenadeInfoIdx(0)
	, m_iRecvObjectInfoIdx(0), m_iRecvDropedWeaponInfoIdx(0), m_iRecvNPCInfoIdx(0)
	, m_iRecvMissionInfoIdx(0), m_nLostTimeCount(0), m_iRecvDomiObjectInfoIdx(0)
	, m_iRecvDomiEventInfoIdx(0), m_iRecvSentryGunDegreeIdx(0)
	, m_pRecvAIDino(nullptr), m_pRecvAIDinosInfo(nullptr), m_pRecvDomiObjectInfo(nullptr)
	, m_pRecvDomiEventInfo(nullptr), m_pRecvSentryGunDegree(nullptr), m_pRecvNPCInfo(nullptr)
{	
	for( INT32 i = 0; i <  USER_SLOT_COUNT; i++ ) m_pRecvTime[i]	= 0.0f;

	m_pConnectSig = i3Signal::new_object();
	m_pConnectSig->Create();
	m_pConnectSig->Reset();
}

UDPPacketReceiver::~UDPPacketReceiver()
{
	UDPPacketReceiver::OnDestroy();			// 소멸자에서 어차피 가상함수 동작이 안되므로, 클래스 범위한정으로 명시만 해둔다..

	Destroy();

	I3_SAFE_RELEASE( m_pConnectSig);
}

void UDPPacketReceiver::Create( INT32 ThreadLevel )
{
	m_bIsRun			= true;
	i3Thread::Create( "UDPPacketReceiver", 0, 4096, 0, ThreadLevel );
}

void UDPPacketReceiver::Reset( STAGE_MODE type)
{
	for( INT32 i = 0; i < RECV_BUFFER_COUNT; i++ )
	{
		m_RecvData[ i ].Reset();
		m_pRecvCharaInfo[ i ].Reset();
		m_pRecvGrenadeInfo[ i ].Reset();
		m_pRecvObjectInfo[ i ].Reset();
		m_pRecvDropedWeaponInfo[ i ].Reset();
		//m_pRecvNPCInfo[ i ].Reset();
	}

	// Game Recv Data
	m_iRecvPlayerInfoIdx		= 0;
	m_iRecvAIDinosInfoIdx		= 0;
	m_iRecvGrenadeInfoIdx		= 0;
	m_iRecvObjectInfoIdx		= 0;
	m_iRecvDropedWeaponInfoIdx	= 0;
	m_iRecvMissionInfoIdx		= 0;
	m_iRecvNPCInfoIdx			= 0;

	m_WriteIdx					= 0;
	m_ReadIdx					= 0;

	m_iRecvDomiObjectInfoIdx	= 0;
	m_iRecvDomiEventInfoIdx		= 0;
	m_iRecvSentryGunDegreeIdx	= 0;

	for( INT32 i = 0; i < RECV_BUFFER_MISSION_COUNT; i++)
	{
		m_pRecvMissionIntrudeInfo[ i ].Reset();
	}

	//Set RecvTime
	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		m_pRecvTime[ i] = 0.f;
	m_DediRecvTime = 0.f;

	m_nLostTimeCount		= 0;
}

void UDPPacketReceiver::Connect( STAGE_MODE type)
{
	Reset( type);

	I3ASSERT( CGameNetworkManager::i()->GetUDPSocket() != nullptr);

	m_pConnectSig->Raise();
}

void UDPPacketReceiver::Destroy( void)
{
	if( m_pRecvAIDino != nullptr)
		delete[] m_pRecvAIDino;

	if( m_pRecvAIDinosInfo != nullptr)
		delete[] m_pRecvAIDinosInfo;

	if( m_pRecvDomiObjectInfo != nullptr)
		delete[] m_pRecvDomiObjectInfo;

	if( m_pRecvDomiEventInfo != nullptr)
		delete[] m_pRecvDomiEventInfo;

	if( m_pRecvSentryGunDegree != nullptr)
		delete[] m_pRecvSentryGunDegree;

	if( m_pRecvNPCInfo != nullptr)
		delete[] m_pRecvNPCInfo;
}

void UDPPacketReceiver::OnDestroy()
{
	m_bIsRun = false;

	m_pConnectSig->Raise();
	
	while(m_bExitResult == false)
	{
		i3Timer::Sleep( 10);
	}
	
	m_State = THREAD_STOPPED;
	I3TRACE( "UDPPacketReceiver::OnDestroy() OK.\n" );
}

UINT32 UDPPacketReceiver::OnRunning( void * pUserData )
{
	m_bExitResult = false;
	INT32 nSize;
	while( m_bIsRun )
	{
		ClientUDPSocket * pUDPSocket = CGameNetworkManager::i()->GetUDPSocket();
		if( pUDPSocket != nullptr)
		{
			nSize = pUDPSocket->Recv();
			switch(nSize)
			{
			case 0				: //Socket 에러 입니다. socket이 아직 준비되지 않은 경우 들어올 수 있습니다.
				//i3Net::GetError( "UDPReceive failed(Socket)" );
				//m_bIsRun = false; 
				break; 
			case SOCKET_ERROR	: //Recv 에러입니다.
				{
					// WSAENOTSOCK 추가 2008.11.19
					INT32 nError = i3Net::GetError( "UDPReceive failed(Recv)" );
					if( nError == WSAEINTR || nError == WSAESHUTDOWN || nError == WSAETIMEDOUT || nError == WSAEINVAL || nError == WSAENOTSOCK)
					{
						m_pConnectSig->WaitFor();
					}
				}
				break; 
			default:			//정상적인 Recv입니다. 
				_PacketParsing( pUDPSocket->getPacket(), nSize, pUDPSocket->getRecvAddr() );
				break; 
			}
		}
	}

	I3TRACE( "UDPPacketReceiver Thread Finish.\n" );
	m_bExitResult = true;

	return 0;
}

RECV_P2P_DATA *	UDPPacketReceiver::ReadGameData( void )
{
	RECV_P2P_DATA * pRv = nullptr; 
	UINT32	ReadPos;
	ReadPos = m_ReadIdx % RECV_BUFFER_COUNT;
	pRv = &m_RecvData[ ReadPos ];	
	return pRv;
}

RECV_P2P_DATA * UDPPacketReceiver::ReadGameData( INT32 idx)
{
	RECV_P2P_DATA * pRv = nullptr; 
	UINT32	ReadPos;
	ReadPos = (m_ReadIdx + idx) % RECV_BUFFER_COUNT;
	pRv = &m_RecvData[ ReadPos ];	
	return pRv;
}

bool UDPPacketReceiver::_CheckingNetwork( sockaddr_in * pInAddr, UINT8 nUserIdx, REAL32 fTime)
{
	//0. 인덱스 체크
	if( BattleServerContext::i()->IsUseDedicationMode() )
	{
		if( nUserIdx != SLOT_DEDICATION_IDX )
		{	
			char szTemp[ MAX_PATH ];
			sprintf_s( szTemp, "UDP Out nUserIdx != SLOT_DEDICATION_IDX:nUserIdx= %d", nUserIdx );
			I3PRINTLOG(I3LOG_DEFALUT,  szTemp );
			
			return false; 
		}
		if( m_DediRecvTime > fTime)	
		{	
			char szTemp[ MAX_PATH ];
			sprintf_s( szTemp, "UDP Out m_DediRecvTime > fTime:m_DediRecvTime= %.4f,nUserIdx: %d", m_DediRecvTime, nUserIdx );
			I3PRINTLOG(I3LOG_DEFALUT,  szTemp );
			
			return false; 
		}
		m_DediRecvTime = fTime;	
	}
	else 
	{
		if( nUserIdx >= USER_SLOT_COUNT )		
		{	
			char szTemp[ MAX_PATH ];
			sprintf_s( szTemp, "UDP Out nUserIdx >= USER_SLOT_COUNT:nUserIdx= %d", nUserIdx );
			I3PRINTLOG(I3LOG_DEFALUT,  szTemp );
			
			return false; 
		}
		//2. Time 체크
		if( m_pRecvTime[nUserIdx] > fTime )
		{
			m_nLostTimeCount++;
			char szTemp[ MAX_PATH ];
			sprintf_s( szTemp, "UDP Out m_pRecvTime[nUserIdx] > fTime:m_pRecvTime[nUserIdx]= %.4f,nUserIdx= %d", m_pRecvTime[nUserIdx], nUserIdx );
			I3PRINTLOG(I3LOG_DEFALUT,  szTemp );
			
			return false; 
		}
		m_pRecvTime[ nUserIdx] = fTime;	
	}

	//1. 받을수 있는 상태인지.
	if( BattleServerContext::i()->IsUseBattleServer() )
	{
		if (UDP_STATE_DONE != NetworkContext::i()->relayServerState)
		{
			char szTemp[ MAX_PATH ];
			sprintf_s( szTemp, "UDP Out UDP_STATE_DONE != relayServerState:relayServerState= %d,nUserIdx: %d", NetworkContext::i()->relayServerState, nUserIdx );
			I3PRINTLOG(I3LOG_DEFALUT,  szTemp );
			return false;
		}

		if (pInAddr->sin_addr.s_addr != NetworkContext::i()->relayServerIp)	return false;
	}
	else
	{		
		I3ASSERT_RETURN(nUserIdx < SLOT_MAX_COUNT, false);
			
		if( NetworkContext::i()->UdpHoleState[nUserIdx] != UDP_STATE_DONE )	
		{
			char szTemp[ MAX_PATH ];
			sprintf_s( szTemp, "UDP Out: %d, %d", BattleSlotContext::i()->getMainSlotIdx(), nUserIdx );
			I3PRINTLOG(I3LOG_DEFALUT,  szTemp );
			return false;
		}
		if( !_CheckNetAddress( pInAddr, nUserIdx ) )						return false;
	}

	return true; 
}

bool UDPPacketReceiver::_CheckNetAddress( SOCKADDR_IN * pInAddr, INT32 Idx )
{
	const UDP_TYPE		NetType		= NetworkContext::i()->UdpHoleType[Idx];
	const NET_UDP_INFO * pNetInfo	= &NetworkContext::i()->NetInfo[Idx];

	switch(NetType)
	{
	case UDP_TYPE_REAL	: 
		if( pNetInfo->m_nRealIP != pInAddr->sin_addr.s_addr)
		{
			//Table에 있는 주소하고 틀린 주소 입니다. 잘못된 사용자 일수도 있고 네트웍에서 주소를 제대로 받지 못한경우도 있습니다.
			return false;
		}
		break; 
	case UDP_TYPE_NAT	: 
		if( pNetInfo->m_nLocalIP != pInAddr->sin_addr.s_addr)
		{
			//Table에 있는 주소하고 틀린 주소 입니다. 잘못된 사용자 일수도 있고 네트웍에서 주소를 제대로 받지 못한경우도 있습니다.
			return false;
		}
		break; 
	default				: 
		return false;
		break; 
	}

	return true;
}

bool UDPPacketReceiver::_CheckActionState( UINT32 nState )
{
	UINT8	nBody, nBody2, nUpper, nLower, nFireOrder, nJumpState;

	UDP_UTIL::ConvertNetActiontoChara( nullptr, nullptr, nullptr, nullptr, &nBody, &nBody2, &nUpper, &nLower, &nJumpState, &nFireOrder, nullptr, nState );

	if( nBody >= CHARA_BODYLOWER_MAXCOUNT )		return false;
	if( nBody2 >= CHARA_BODYUPPER_MAXCOUNT  )	return false;
	if( nUpper >= CHARA_UPPER_MAXCOUNT  )	return false;
	if( nLower >= CHARA_LOWER_MAXCOUNT  )	return false;

	return true;
}

bool UDPPacketReceiver::_CheckIndex( UINT8 type, INT32 idx)
{
	INT32 limit = 0;
	char * pszSUBHead = nullptr;

	switch( type)
	{
	case P2P_SUB_HEAD_USER :
		limit = SLOT_MAX_COUNT;
		pszSUBHead = "P2P_SUB_HEAD_USER";
		break;
	case P2P_SUB_HEAD_AI :
		limit = MAX_COUNT_DOMI_DINO;
		pszSUBHead = "P2P_SUB_HEAD_AI";
		break;
	case P2P_SUB_HEAD_GRENADE :
		limit = MAX_THROWWEAPON_COUNT<<2;	// 제압 공룡 포이즌 그레네이드 통합 사이즈
		pszSUBHead = "P2P_SUB_HEAD_GRENADE";
		break;
	case P2P_SUB_HEAD_DROPEDWEAPON :
		limit = MAX_DROPEDWEAPON_COUNT;
		pszSUBHead = "P2P_SUB_HEAD_DROPEDWEAPON";
		break;
	case P2P_SUB_HEAD_OBJECT :
	case P2P_SUB_HEAD_OBJECT_DUMMY1 :
	case P2P_SUB_HEAD_OBJECT_DUMMY2 :
	case P2P_SUB_HEAD_OBJECT_DUMMY3 :
	case P2P_SUB_HEAD_STAGEINFO_DUMMY1 :
	case P2P_SUB_HEAD_STAGEINFO_DUMMY2 :
	case P2P_SUB_HEAD_STAGEINFO_DUMMY3 :
	case P2P_SUB_HEAD_STAGEINFO_DUMMY4 :
	case P2P_SUB_HEAD_STAGEINFO_DUMMY5 :
		limit = OBJ_TOTAL_COUNT;
		pszSUBHead = "P2P_SUB_HEAD_OBJECT";
		break;
	
	case P2P_SUB_HEAD_NPC :
		limit = MAX_NPC_COUNT;
		pszSUBHead = "P2P_SUB_HEAD_NPC";
		break;
	case P2P_SUB_HEAD_STAGEINFO_CHARA :
		limit = SLOT_MAX_COUNT;
		pszSUBHead = "P2P_SUB_HEAD_STAGEINFO_CHARA";
		break;
	
	case P2P_SUB_HEAD_STAGEINFO_MISSION :
		limit = SLOT_MAX_COUNT;
		pszSUBHead = "P2P_SUB_HEAD_STAGEINFO_MISSION";
		break;
//#ifdef DOMI_UDP_OBJECT
	case P2P_SUB_HEAD_DOMISKILLOBJECT :
		limit = 32;
		pszSUBHead = "P2P_SUB_HEAD_DOMISKILLOBJECT";
		break;
	case P2P_SUB_HEAD_DOMIEVENT :
		limit = 32;
		pszSUBHead = "P2P_SUB_HEAD_DOMIEVENT";
		break;
	case P2P_SUB_HEAD_SENTRYGUN:
		limit = 32;
		pszSUBHead = "P2P_SUB_HEAD_SENTRYGUN";
		break;
//#endif
	default :
		I3TRACE( "[UDPPacketReceiver] Invalid recv packet type.. %d\n", type);
		return false;
		break; 
	}

	if( idx >= limit)
	{
		I3ASSERT( pszSUBHead != nullptr);
		I3PRINTLOG(I3LOG_WARN,  "[UDPPacketReceiver::%s] invalid index..%d", pszSUBHead, idx);
		return false;
	}

	return true;
}

bool UDPPacketReceiver::_CheckCode( UINT8 type, UINT16 Code, UINT16 Rv)
{
	if( !(Rv & UDP_ERROR) &&
		(Code == Rv + sizeof( N_GAME_SUBHEAD)))
	{
		return true;
	}

	char * pszSUBHead = nullptr;

	switch( type)
	{
	case P2P_SUB_HEAD_USER:					pszSUBHead = "P2P_SUB_HEAD_USER";				break;
	case P2P_SUB_HEAD_AI:					pszSUBHead = "P2P_SUB_HEAD_AI";					break;
	case P2P_SUB_HEAD_GRENADE:				pszSUBHead = "P2P_SUB_HEAD_GRENADE";			break;
	case P2P_SUB_HEAD_DROPEDWEAPON:			pszSUBHead = "P2P_SUB_HEAD_DROPEDWEAPON";		break;
	case P2P_SUB_HEAD_OBJECT:				pszSUBHead = "P2P_SUB_HEAD_OBJECT";				break;
	case P2P_SUB_HEAD_NPC:					pszSUBHead = "P2P_SUB_HEAD_NPC";				break;
	case P2P_SUB_HEAD_STAGEINFO_MISSION:	pszSUBHead = "P2P_SUB_HEAD_STAGEINFO_MISSION";	break;

//#ifdef DOMI_UDP_OBJECT
	case P2P_SUB_HEAD_DOMISKILLOBJECT:		pszSUBHead = "P2P_SUB_HEAD_DOMISKILLOBJECT";	break;
	case P2P_SUB_HEAD_DOMIEVENT:			pszSUBHead = "P2P_SUB_HEAD_DOMIEVENT";			break;
	case P2P_SUB_HEAD_SENTRYGUN:			pszSUBHead = "P2P_SUB_HEAD_SENTRYGUN";			break;
//#endif
	}

	I3ASSERT( pszSUBHead != nullptr);

	return false;
}

void UDPPacketReceiver::_PacketParsing( char * pPacket, INT32 nSize, sockaddr * pAddr )
{
	INT32	nReadPos = 0;
	N_GAME_HEADER *	pHead	= nullptr;
	sockaddr_in *	pInAddr	= nullptr;

	pInAddr	= (sockaddr_in *) pAddr;
	
	// Check from where
	if( pInAddr->sin_addr.s_addr == 0 )
	{
		return;
	}

	if( GetRecvCount() >= RECV_BUFFER_COUNT)
	{
		I3TRACE( "UDP Receive buffer full..\n");
		return;
	}

	//////////////////////////////////////////////////////////////////////////////////
	// 
	// 1. Read Header //추후 검사하는 곳으로 이동합니다. 
	pHead		= (N_GAME_HEADER *) &pPacket[ nReadPos ];
	nReadPos	+= sizeof( N_GAME_HEADER );
	if(pHead->_Size != nSize )
	{
		char szTrace[ 256] = {0,};
		sprintf_s( szTrace, "패킷 사이즈가 틀립니다. ID: %d S : %d - %d", pHead->_ProtocolID, pHead->_Size, nSize );
		I3PRINTLOG(I3LOG_NOTICE, szTrace );
		return;
	}
#ifdef __USE_PACKET_OPTIMIZE__
	//g_ui8PacketUpdateIdx = pHead->m_ui8PacketUpdateIdx;
#endif

	// 핑 체크 1이면 핑을 받은 경우입니다.
	// Host는 받은 경우.. GameContext에서 타임을 계산할 것입니다.
	// Client가 받은 경우 Host에게 다음 패킷을 보낼때 핑 패킷을 보내도록 합니다.
	UINT8 iPing = (pHead->_ProtocolID >> 7) & 0x01;

	//////////////////////////////////////////////////////////////////////////////////
	//  
	// 2. Parsing Packet 
	switch( pHead->_ProtocolID & 0x7F )
	{
	/////////////////////////////////////////////////////////////////////////////////
	// 홀펀칭 패킷(패킷에 대해 검사하지 않는다. )
	case	UDPPROTOCOL_HOLEPUNCH		:
	case	UDPPROTOCOL_HOLEPUNCH_END	:
		if (pHead->_Index < SLOT_MAX_COUNT &&
			CGameNetworkManager::i()->GetUDPHolePuncher()->P2PHolePunchRecv( pInAddr->sin_addr.s_addr, htons( pInAddr->sin_port ), pPacket))
		{
			m_pRecvTime[pHead->_Index] = 0.0f;
		}
		break;
	/////////////////////////////////////////////////////////////////////////////////
	//방장이 받는 패킷  
	case UDPPROTOCOL_CN_INFO : 
	case UDPPROTOCOL_CS_INFO : 
		if( !_CheckingNetwork( pInAddr, pHead->_Index, pHead->_GameTime) )	return;

		nReadPos += _GamePacketParsing( &pPacket[ nReadPos], pHead->_Size - sizeof(N_GAME_HEADER), iPing, pHead->_RoundNum );
		if( pHead->_Size != nReadPos)
		{
			I3TRACE( "invalid parsing size:%d, %d\n", pHead->_Size, nReadPos);
			return;
		}
		break;
	/////////////////////////////////////////////////////////////////////////////////
	//배틀서버 사용할때 받는 패킷 
	case UDPPROTOCOL_SERVER_JOIN_GROUP_ACK:
		{// 릴레이 서버로 그만 보내도 된다.

			GAME_ROOM_OPTION RoomOption;
			RoomOption.UnPacking( &pPacket[ nReadPos] );
			
			BattleSlotContext::i()->setGameRoomOption(&RoomOption);

			CGameNetworkManager::i()->GetUDPHolePuncher()->RelayHolePunchRecv( &pPacket[ nReadPos] );
			for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)m_pRecvTime[i] = 0.0f;
			m_DediRecvTime = 0.0f; 
		}
		break;
	case UDPPROTOCOL_SERVER_WITHDRAW_GROUP_ACK	:
		break;
	case UDPPROTOCOL_DSERVER_PING				: 
		_DediPingPacketParsing(pHead); 
		break; 
	default:
		I3TRACE( "--UDP(CN) Invalid Protocol. %d, readsize %d, readpos %d\n", pHead->_ProtocolID, nSize, nReadPos );
		return;
	}

	return;
}

void UDPPacketReceiver::_DediPingPacketParsing( N_GAME_HEADER *	pHeader )
{
	PING* pPingInfo = BattleServerContext::i()->SetPingInfo();
	pPingInfo->m_fPingRecvTime = BattleSyncTimer::i()->GetTimeElapsed();
	return; 
}

INT32 UDPPacketReceiver::_GamePacketParsing( char * pPacket, INT32 nSize, UINT8 iPing, UINT8 iRoundNum)
{
	UINT16	Rv;
	N_GAME_SUBHEAD * pHead;
	INT32	nReadPos = 0;
	bool	bRecv = true;
	

	// Check Ping
	// 앞으로 쓸 버퍼에 핑을 받았음을 체크
	if(iPing)
	{
		INT32	WritedPos	= m_WriteIdx % RECV_BUFFER_COUNT;

		m_RecvData[ WritedPos ].iPing		= iPing;
		m_RecvData[ WritedPos ].rPingSendTime = *((REAL32*) &pPacket[nReadPos]);

		nReadPos += sizeof(REAL32);
	}

	while( bRecv)
	{
		pHead	= (N_GAME_SUBHEAD *) &pPacket[ nReadPos ];
		nReadPos += sizeof( N_GAME_SUBHEAD );
		
		INT32 WritedPos = m_WriteIdx % RECV_BUFFER_COUNT;

		if( _CheckIndex( pHead->_Type, pHead->_Index))
		{
			//GlobalFunc::PB_TRACE("pHead->_Type %d", pHead->_Type);
			switch( pHead->_Type )
			{
			case P2P_SUB_HEAD_USER :
//				g_pFramework->PrintConsole(CONSOLE_LOG_PROTOCOL, "UDPPacketReceiver : P2P_SUB_HEAD_USER");
				Rv = m_pRecvCharaInfo[ m_iRecvPlayerInfoIdx ].Unpack( &pPacket[ nReadPos ]);

				m_RecvData[ WritedPos ].pArg	= (char*) &m_pRecvCharaInfo[ m_iRecvPlayerInfoIdx ];
				m_RecvData[ WritedPos ].Type	= INFO_DATA_CHARACTER;

				m_iRecvPlayerInfoIdx++;
				if( m_iRecvPlayerInfoIdx >= RECV_BUFFER_COUNT ) m_iRecvPlayerInfoIdx = 0;
				break;

			case P2P_SUB_HEAD_GRENADE :
				g_pFramework->PrintConsole(CONSOLE_LOG_PROTOCOL, "UDPPacketReceiver : P2P_SUB_HEAD_GRENADE");
				Rv = m_pRecvGrenadeInfo[ m_iRecvGrenadeInfoIdx ].Unpack( &pPacket[ nReadPos ]);

				m_RecvData[ WritedPos ].Type	= INFO_DATA_GRENADE;
				m_RecvData[ WritedPos ].pArg	= (char*) &m_pRecvGrenadeInfo[ m_iRecvGrenadeInfoIdx ];

				m_iRecvGrenadeInfoIdx++;
				if( m_iRecvGrenadeInfoIdx >= RECV_BUFFER_COUNT ) m_iRecvGrenadeInfoIdx = 0;
				break;

			case P2P_SUB_HEAD_DROPEDWEAPON :
				g_pFramework->PrintConsole(CONSOLE_LOG_PROTOCOL, "UDPPacketReceiver : P2P_SUB_HEAD_DROPEDWEAPON ");
				Rv = m_pRecvDropedWeaponInfo[ m_iRecvDropedWeaponInfoIdx ].Unpack( &pPacket[ nReadPos ]);

				m_RecvData[ WritedPos ].Type	= INFO_DATA_DROPEDWEAPON;
				m_RecvData[ WritedPos ].pArg	= (char*) &m_pRecvDropedWeaponInfo[ m_iRecvDropedWeaponInfoIdx ];

				m_iRecvDropedWeaponInfoIdx++;
				if( m_iRecvDropedWeaponInfoIdx >= RECV_BUFFER_COUNT ) m_iRecvDropedWeaponInfoIdx = 0;
				break;

			case P2P_SUB_HEAD_OBJECT :
			case P2P_SUB_HEAD_OBJECT_DUMMY1 :
			case P2P_SUB_HEAD_OBJECT_DUMMY2 :
			case P2P_SUB_HEAD_OBJECT_DUMMY3 :
			case P2P_SUB_HEAD_STAGEINFO_DUMMY1 :
			case P2P_SUB_HEAD_STAGEINFO_DUMMY2 :
			case P2P_SUB_HEAD_STAGEINFO_DUMMY3 :
			case P2P_SUB_HEAD_STAGEINFO_DUMMY4 :
			case P2P_SUB_HEAD_STAGEINFO_DUMMY5 :
				g_pFramework->PrintConsole(CONSOLE_LOG_PROTOCOL, "UDPPacketReceiver : P2P_SUB_HEAD_OBJECT... ");
				Rv = m_pRecvObjectInfo[ m_iRecvObjectInfoIdx ].Unpack( &pPacket[ nReadPos]);

				m_RecvData[ WritedPos ].Type	= INFO_DATA_OBJECT;
				m_RecvData[ WritedPos ].pArg	= (char*) &m_pRecvObjectInfo[ m_iRecvObjectInfoIdx ];
					
				m_iRecvObjectInfoIdx++;
				if( m_iRecvObjectInfoIdx >= RECV_BUFFER_COUNT ) m_iRecvObjectInfoIdx = 0;
				break;
		
				// 난입용 패킷
			case P2P_SUB_HEAD_STAGEINFO_CHARA :
				g_pFramework->PrintConsole(CONSOLE_LOG_PROTOCOL, "UDPPacketReceiver : P2P_SUB_HEAD_STAGEINFO_CHARA ");
				Rv = m_pRecvCharaInfo[ m_iRecvPlayerInfoIdx].Unpack_StageInfo( &pPacket[ nReadPos ]);

				m_RecvData[ WritedPos ].pArg	= (char*) &m_pRecvCharaInfo[ m_iRecvPlayerInfoIdx ];
				m_RecvData[ WritedPos ].Type	= INFO_DATA_CHARACTER;

				m_iRecvPlayerInfoIdx++;
				if( m_iRecvPlayerInfoIdx >= RECV_BUFFER_COUNT ) m_iRecvPlayerInfoIdx = 0;
				break;
			
			case P2P_SUB_HEAD_STAGEINFO_MISSION :
				m_pRecvMissionIntrudeInfo[ m_iRecvMissionInfoIdx ].setInstallerIndex( pHead->_Index);
				Rv = m_pRecvMissionIntrudeInfo[ m_iRecvMissionInfoIdx ].Unpack( &pPacket[ nReadPos]);

				m_RecvData[ WritedPos ].Type	= INFO_DATA_STAGEINFO_MISSION;
				m_RecvData[ WritedPos ].pArg	= (char*) &m_pRecvMissionIntrudeInfo[ m_iRecvMissionInfoIdx ];

				m_iRecvMissionInfoIdx++;
				if( m_iRecvMissionInfoIdx >= RECV_BUFFER_MISSION_COUNT ) m_iRecvMissionInfoIdx = 0;
				break;

				// 안국정 추가
			case P2P_SUB_HEAD_AI:
				I3ASSERT( m_pRecvAIDinosInfo != nullptr);
				Rv = m_pRecvAIDinosInfo[ m_iRecvAIDinosInfoIdx ].Unpack( &pPacket[ nReadPos ]);

				m_RecvData[ WritedPos ].pArg	= (char*) &m_pRecvAIDinosInfo[ m_iRecvAIDinosInfoIdx ];
				m_RecvData[ WritedPos ].Type	= INFO_DATA_AI;

				m_iRecvAIDinosInfoIdx++;
				if( m_iRecvAIDinosInfoIdx >= RECV_BUFFER_COUNT) m_iRecvAIDinosInfoIdx = 0;
				break;

			case P2P_SUB_HEAD_DOMISKILLOBJECT :
				I3ASSERT( m_pRecvDomiObjectInfo != nullptr);
				Rv = m_pRecvDomiObjectInfo[m_iRecvDomiObjectInfoIdx].Unpack( &pPacket[nReadPos]);
				m_RecvData[WritedPos].pArg = (char*)&m_pRecvDomiObjectInfo[m_iRecvDomiObjectInfoIdx];
				m_RecvData[WritedPos].Type = INFO_DATA_DOMISKILLOBJECT;
				m_iRecvDomiObjectInfoIdx++;
				if (m_iRecvDomiObjectInfoIdx >= RECV_BUFFER_COUNT) m_iRecvDomiObjectInfoIdx = 0;
				break;

			case P2P_SUB_HEAD_DOMIEVENT:
				I3ASSERT( m_pRecvDomiEventInfo != nullptr);
				Rv = m_pRecvDomiEventInfo[m_iRecvDomiEventInfoIdx].Unpack( &pPacket[nReadPos]);
				m_RecvData[WritedPos].pArg = (char*)&m_pRecvDomiEventInfo[m_iRecvDomiEventInfoIdx];
				m_RecvData[WritedPos].Type = INFO_DATA_DOMIEVENT;
				m_iRecvDomiEventInfoIdx++;
				if (m_iRecvDomiEventInfoIdx >= RECV_BUFFER_COUNT) m_iRecvDomiEventInfoIdx = 0;
				break;

			case P2P_SUB_HEAD_SENTRYGUN:
				I3ASSERT( m_pRecvSentryGunDegree != nullptr);
				Rv = m_pRecvSentryGunDegree[m_iRecvSentryGunDegreeIdx].Unpack( &pPacket[nReadPos]);
				m_RecvData[WritedPos].pArg = (char*)&m_pRecvSentryGunDegree[m_iRecvSentryGunDegreeIdx];
				m_RecvData[WritedPos].Type = INFO_DATA_SENTRYGUN_DEGREE;
				m_iRecvSentryGunDegreeIdx++;
				if (m_iRecvSentryGunDegreeIdx >= RECV_BUFFER_COUNT) m_iRecvSentryGunDegreeIdx = 0;
				break;

			default :
				I3TRACE( "invalid sub protocol %d\n", pHead->_Type);
				Rv = 0;
				break;
			}

			if( _CheckCode( pHead->_Type, pHead->_Code, Rv))
			{
				nReadPos += Rv;
			}
			else
				bRecv = false;
		}
		else
			bRecv = false;

		if( bRecv)
		{
			m_RecvData[ WritedPos ].Idx		= pHead->_Index; 
			m_RecvData[ WritedPos ].iRoundNum = iRoundNum;
#ifndef CHANGE_USURPATION
			m_RecvData[ WritedPos ].iUsurpationIdx = iState;
			i3mem::Copy( m_RecvData[ WritedPos ].rMissionObjPos , rObjectPos, sizeof(REAL16)*3 );
#endif
			m_WriteIdx ++;
		}

		if( nSize == nReadPos)
			bRecv = false;
		else if( nSize < nReadPos )
		{
			I3TRACE( "invalid size.. nSize : %d, nReadPos : %d \n", nSize, nReadPos);
			bRecv = false;
		}

		if( GetRecvCount() >= RECV_BUFFER_COUNT )
		{	
			UDPTRACE( "[UDPPacketReceiver::_GamePacketParsing()] Error Recv Buffer FULL \n"); 
			bRecv = false;
		}
	}

	return nReadPos;
}




