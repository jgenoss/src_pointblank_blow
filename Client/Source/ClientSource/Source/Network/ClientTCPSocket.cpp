#include "pch.h"
#include "ClientTCPSocket.h"
#include "../GlobalVariables.h"
#include "StageBattle.h"
#include "SecurityHeader.h"
#include "CommunityContext.h"
#include "ServerInfo.h"
#include "Shop.h"
#include "ClientDef.h"
#include "GameLoginContext.h"

#include "UtilBattle.h"

#include "LogRespawn.h"
#include "GameStateMgr.h"

#include "UI/UIUtil.h"
#include "UI/UIMainFrame.h"
#include "UI/UIBattleDef.h"

#include "IGShopContext.h"
#include "IGShopItemController.h"

#if defined( _XTRAP_)
#include "../../Xtrap/Include/Xtrap_C_Interface.h"
#include "../../Xtrap/Include/Xtrap_S_Interface.h"

#ifdef _XTRAP_DUMP
#include "../../Xtrap/Etc/HexaDump4XTrap.h"
#endif
#endif

#include "GameNetworkManager.h"
#include "StageBattle/NetworkContext.h"

#include "i3Base/string/ext/mb_to_utf16.h"

I3_CLASS_INSTANCE(ClientTCPSocket);

ClientTCPSocket::ClientTCPSocket(void) : m_rLastSendTime(0.f), m_bShowMessage(false)
	, m_ConnectionSuccess(false), m_i16PacketRandSeed(0)
	, m_TargetServerType(SERVER_TYPE_INVALID), m_ServerEntered(false)
#if defined( I3_DEBUG)
	, m_iID(0), m_UsingType(0), m_iSendPacketSize(0), m_iSendPacketCount(0), m_iRecvPacketSize(0), m_iRecvPacketCount(0)
#endif
{
	i3mem::FillZero( &m_InAddr, sizeof( m_InAddr));

	__Register_Auth_Function();
	__Register_Base_Function();
	__Resigter_Battle_Function();

	__Register_Clan_Function();
	__Register_Inventory_Function();
	__Register_Lobby_Function();
	__Register_Room_Function();
	__Register_ServerMessage_Function();
	__Register_Shop_Function();
	__Register_Medal_Function();
	__Register_Gacha_Function();
	__Register_QuickJoin_Function();
	__Register_Cheat_Function();
	__Register_GMAuthority_Function();
	__Register_ClanWar_Function();
}

ClientTCPSocket::~ClientTCPSocket(void)
{
	OnDestroy();
}

bool ClientTCPSocket::OnCreate(char * Address, UINT16 PortNo)
{
	m_rLastSendTime	+= m_SendTimer.GetDeltaSec();

	m_ConnectionInfo.Reset();

#if !defined( I3_NO_PROFILE)
	NETPROFILE::TCPReset(CGameNetworkManager::i()->GetProfileData()); 
#endif


#if defined( _FROST_ )
	//
	// frost 사용시 socket 생성 이외의 socket 함수들은 모두 frost 함수로 대체해야합니다.
	//
/*	UINT32 ip = ::inet_addr(Address);


	// 소켓 생성
	SOCKET	sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == sock)
	{
		I3TRACE("[ClientTCPSocket::OnCreate] Error Socket Is Invalid \n"); 
		return false;
	}

	// 서버 접속
	struct sockaddr_in serverAddr;
	i3mem::FillZero(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ip;
	serverAddr.sin_port = ::htons(PortNo);*/

	// 로그인일 시도하는 사용자의 
	frostSetUserName( GameLoginContext::i()->getUserID());

/*	INT32 result = frostConnect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr));

	if (SOCKET_ERROR == result)
	{
		i3Net::GetError("[i3NetworkClientSocket::OnConnectTo]");
		return false;
	}*/

//	SetSocket( sock );
//	return true;
#endif

	I3TRACE("[ClientTCPSocket::OnCreate] %s |%d \n", Address, PortNo); 
	m_bShowMessage = true; 
	return i3NetworkClientSocket::OnCreate( Address, PortNo);

//#endif	//defined( _FROST_ )
}

void ClientTCPSocket::OnDestroy()
{
	DisConnect();
}

void ClientTCPSocket::DisConnect(void)
{
	DEV_LOG("ClientTCPSocket::OnDestroy() %d / %d /", m_TargetServerType, m_ConnectionInfo.m_SessionIndex); 

	if(m_bShowMessage)
	{
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_M_DESTROYED_NETWORK);
	}
	m_TargetServerType = SERVER_TYPE_INVALID;
	SetEnteredServer(false);
	m_rLastSendTime		= 0.0f;
	m_ConnectionSuccess	= false;

	i3NetworkClientSocket::DisConnect();
}

void ClientTCPSocket::ForceDestroy(void)
{
	m_bShowMessage = false;
	OnDestroy();
	m_bShowMessage = true;
	return;
}


INT32	ClientTCPSocket::_GetEncryptKey()
{
	return (m_ConnectionInfo.m_SessionIndex % 7) + 1;
}

bool ClientTCPSocket::_SendTCPProfile( UINT16 ui16PacketSize, PROTOCOL TProtocol )
{
	//I3ASSERT(IsConnected());
	if ( ! IsConnected() )
	{
		DEV_LOG("ClientTCPSocket::SendMessage - ! IsConnected() %d / %d", m_UsingType, TProtocol );

		if ( ! m_ConnectionSuccess )
		{
			I3PRINTLOG(I3LOG_NOTICE,"Invalid Send Packet : %d / %d", m_UsingType, TProtocol );
			return false;
		}
	}

#if !defined( I3_NO_PROFILE)
	//Send Buffer
	NETPROFILE::info_t * pData = CGameNetworkManager::i()->SetProfileData();	
	pData->m_nTcpSendMessage[ pData->m_nTcpRecvIdx] = TProtocol; 
	pData->m_nTcpSendIdx++; 
	if( pData->m_nTcpSendIdx == MAX_TCP_MESSAGE_COUNT)pData->m_nTcpSendIdx = 0; 
	//Update DbgMsg Window
	g_pFramework->UpdateTcpMessage( 1 );
#endif

	//Add Size
#if defined( I3_DEBUG)
	m_iSendPacketSize += ui16PacketSize;
	m_iSendPacketCount++;
#endif

	m_rLastSendTime	+= m_SendTimer.GetDeltaSec();

#if 0	
	PROTOCOL ProtocolId = TProtocol;
	DumpSendMessage(ProtocolId);
#endif

	return true;
}

INT32 ClientTCPSocket::SendMessage(i3NetworkPacket * pPacket)
{
	i3NetworkPacket EncriptPacket;
	_EncriptPacket( &EncriptPacket, pPacket );
#if defined(USE_PACKET_ENCRIPT)
	EncriptPacket.Encript( _GetEncryptKey() ); 
#endif

	if( false == _SendTCPProfile( static_cast<UINT16>(EncriptPacket.GetPacketSize()), EncriptPacket.GetProtocolID() ) )	return 0;

//#if defined (_FROST_)
//	return ClientTCPSocket::SendPacketMessage( &EncriptPacket );
//#else
	return i3NetworkClientSocket::SendPacketMessage( &EncriptPacket ); 
//#endif
}

INT32 ClientTCPSocket::SendMessage( S2MOPacketBase* pS2MOPacket )
{
	_ChangeEncriptSeed();
	pS2MOPacket->SetPacketRandSeed( m_i16PacketRandSeed );

	i3NetworkPacket EncriptPacket;
	S2MOMakePacket( pS2MOPacket, const_cast<char*>(EncriptPacket.GetPacketBuffer()) );
#if defined(USE_PACKET_ENCRIPT)
	EncriptPacket.Encript( _GetEncryptKey() ); 
#endif

	if( false == _SendTCPProfile( static_cast<UINT16>(EncriptPacket.GetPacketSize()), EncriptPacket.GetProtocolID() ) )	return 0;

//#if defined (_FROST_)
//	return ClientTCPSocket::SendPacketMessage( &EncriptPacket );
//#else
	return i3NetworkClientSocket::SendPacketMessage( &EncriptPacket ); 
//#endif
}

#if defined( I3_DEBUG)
void ClientTCPSocket::DumpSendMessage(UINT16 Protocol)
{
	GlobalFunc::PB_TRACE("ID %d, DumpSendMessage %d", m_iID, Protocol);

	switch( Protocol )
	{
	case PROTOCOL_BATTLE_SENDPING_REQ					: GlobalFunc::PB_TRACE("PROTOCOL_BATTLE_SENDPING_REQ		\n");  break;
	case PROTOCOL_LOBBY_ENTER_REQ						: GlobalFunc::PB_TRACE("PROTOCOL_LOBBY_ENTER_REQ		\n");  break;
	case PROTOCOL_BATTLE_TIMERSYNC_REQ					: GlobalFunc::PB_TRACE("PROTOCOL_BATTLE_TIMERSYNC_REQ\n");	break;
	case PROTOCOL_BATTLE_MISSION_BOMB_INSTALL_REQ		: GlobalFunc::PB_TRACE("PROTOCOL_BATTLE_MISSION_BOMB_INSTALL_REQ\n");	break;
	default:
		GlobalFunc::PB_TRACE("Warrring UnKnown TCP Packet (%d)\n", Protocol);
		break;
	}
}
#endif

INT16 ClientTCPSocket::_ChangeEncriptSeed()
{
	m_i16PacketRandSeed = (((m_i16PacketRandSeed * 214013L + 2531011L) >> 16) & 0x7fff);
	return m_i16PacketRandSeed;
}

void ClientTCPSocket::_EncriptPacket( i3NetworkPacket* pOutPacket, i3NetworkPacket* pInPacket )
{
	_ChangeEncriptSeed();

	pOutPacket->SetProtocol( pInPacket->GetProtocolID() );
	pOutPacket->WriteData( &m_i16PacketRandSeed,		sizeof(UINT16) );
	pOutPacket->WriteData( pInPacket->GetFieldBuffer(), pInPacket->GetDataFieldSize() );
}

INT32	ClientTCPSocket::SendHeartBitMessage()
{
	// 서버에서 PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS 를 받기전에는 하트비트를 보내지 않는다.
	if ( ! m_ConnectionSuccess )
		return 0;

	i3NetworkPacket SendPacket(PROTOCOL_BASE_HEART_BIT_REQ);
	return SendMessage(&SendPacket);
}

INT32	ClientTCPSocket::SendHeartBit2Message()
{
	// 서버에서 PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS 를 받기전에는 하트비트를 보내지 않는다.
	if ( ! m_ConnectionSuccess )
		return 0;

	i3NetworkPacket SendPacket(PROTOCOL_BASE_HEART_BIT_2);
	return SendMessage(&SendPacket);
}

/*#if defined(_FROST_)
INT32 ClientTCPSocket::SendPacketMessage(i3NetworkPacket* packet)
{
	// 소켓이 닫혔으면 보네지 않는다.
	if (INVALID_SOCKET == GetSocket())	return 0;


	// 패킷을 다 보낼때까지 반복한다.
	INT32 sendIdx = 0;
	while(packet->GetPacketSize() > sendIdx)
	{
		INT32 sendedByte = frostSend(GetSocket(), packet->GetPacketBuffer() + sendIdx, packet->GetPacketSize() - sendIdx, 0);

		// 전송 실패 
		if (SOCKET_ERROR == sendedByte)
		{
			char message[MAX_PATH];
			INT32 error = ::WSAGetLastError();

			sprintf_s(message, "[i3NetworkClientSocket::SendPacketMessage] lastError = %d", error);
			i3Net::GetError(message);
			
			return -1;
		}

		// 연결이 끊어졌다.
		if (0 == sendedByte)
		{
			return 0;
		}

		// 전송 바이트만큼 인덱스 증가
		sendIdx += sendedByte;
	}

	return sendIdx;
}

void	ClientTCPSocket::OnReceive(void)
{
	// 패킷을 받는다.
	INT32 receivedByte = frostRecv(GetSocket(), GetRecvBuffer(), PACKETBUFFERSIZE, 0);

	ReceiveProcess(receivedByte);
}
#endif*/

void ClientTCPSocket::__Parse_UnknownProtocol( i3NetworkPacket * pPacket)
{
	I3PRINTLOG(I3LOG_WARN,  "아직 Parser를 등록 안한 Protocol입니다. ProtocolID: %d", pPacket->GetProtocolID());
}

void ClientTCPSocket::__Parse_None( i3NetworkPacket * pPacket)
{
	I3TRACE( "아무런 동작도 안하는 Protocol(%d)입니다.\n 차후 구현이 필요함!!!!!!!!!!!!!!!!!!!!!!!!!\n", pPacket->GetProtocolID());
}

void ClientTCPSocket::_ReadFriendInfoToNode(i3NetworkPacket* pPacket,struct FRIEND_NODE* node)
{
	// 목록을 읽어
	UINT8 length;
	TTCHAR nick[NET_NICK_NAME_SIZE];
	FRIEND_INFO info;

	pPacket->ReadData(&length, sizeof(UINT8));
	pPacket->ReadData(nick, sizeof(TTCHAR) * length);

	// 컬러닉네임
	pPacket->ReadData(&info, sizeof(FRIEND_INFO));

	// 기록한다
	nick[NET_NICK_NAME_SIZE - 1] = '\0';

	node->_wstr_nick = nick;

	node->_uid = info._fuid;
	node->_nickcolor = info._nickcolor;

	{
		// FRINED_INFO check
		UINT32 state = GET_FRIEND_STATE(info._state);
		UINT32 server = GET_FRIEND_SERVER(info._state);
		UINT32 channel = GET_FRIEND_CHANNEL(info._state);
		UINT32 room = GET_FRIEND_ROOMIDX(info._state);

		if (FRIEND_STATE_REQUEST <= state && FRIEND_STATE_ROOM >= state)
		{
			//server = NOT_CHECK_CLOSE(server);
			channel = MINMAX(0, channel, GAME_CHANNEL_COUNT - 1);
			room = MINMAX(0, room, VIEW_ROOM_COUNT - 1);
		}
		else if( state == FRIEND_STATE_CANT_INVITE)
		{
			state = FRIEND_STATE_CANT_INVITE;
		}
		else
		{
			state = FRIEND_STATE_OFFLINE;
		}

		node->_state = MAKE_FRIEND_STATE(state, server, channel, room);
	}

	node->_rank = MIN(info._rank, RANK_MAX );
}

void ClientTCPSocket::_ReadClanMemberInfoToNode(i3NetworkPacket* pPacket,struct CLAN_MEMBER_NODE* node)
{
	// 목록을 읽어
	UINT8 length;
	TTCHAR nick[NET_NICK_NAME_SIZE];
	::memset(nick, 0, sizeof(TTCHAR)*NET_NICK_NAME_SIZE);
	CLANMEMBER_INFO info;

	pPacket->ReadData(&length,	sizeof(UINT8));
	if( length > NET_NICK_NAME_SIZE ) return;
	pPacket->ReadData(nick,		length * sizeof(TTCHAR) );
	pPacket->ReadData(&info,	sizeof(CLANMEMBER_INFO));

	// 기록한다
	node->_wstr_nick = nick;
	node->_uid = info._fuid;
	node->_nickcolor = info.m_ui8NickColor;
	
	{
		// FRINED_INFO check
		UINT32 state	= static_cast<UINT32>(GET_FRIEND_STATE( info._state ));
		UINT32 match	= static_cast<UINT32>(GET_FRIEND_MATCH( info._state ));
		UINT32 server	= static_cast<UINT32>(GET_FRIEND_SERVER( info._state ));
		UINT32 channel	= static_cast<UINT32>(GET_FRIEND_CHANNEL( info._state ));
		UINT32 room		= static_cast<UINT32>(GET_FRIEND_ROOMIDX( info._state));

		if (FRIEND_STATE_REQUEST <= state && FRIEND_STATE_ROOM >= state)
		{
			//server = NOT_CHECK_CLOSE(server);
			channel = MINMAX(0, channel, GAME_CHANNEL_COUNT - 1);
			room = MINMAX(0, room, VIEW_ROOM_COUNT - 1);
		}
		else if( state == FRIEND_STATE_CANT_INVITE)
		{
			state = FRIEND_STATE_CANT_INVITE;
		}
		else
		{
			state = FRIEND_STATE_OFFLINE;
		}

		node->_state = MAKE_FRIEND_MATCH_STATE(match, state, server, channel, room);
	}

	node->_rank = MIN(info._rank, RANK_MAX);
}

void ClientTCPSocket::PacketParsing(i3NetworkPacket * pPacket)
{	
	PROTOCOL ProtocolId; 

	if( g_pFramework == nullptr)
		return;

#if defined(USE_PACKET_ENCRIPT)
	//pPacket->Decript( _GetEncryptKey() ); 
#endif
	//pPacket->Decompress();

	ProtocolId = pPacket->GetProtocolID();
	
#if !defined( I3_NO_PROFILE)	//이부분 차후 삭제합니다. 
	//Recv Buffer
	NETPROFILE::info_t * pData = CGameNetworkManager::i()->SetProfileData();
	pData->m_nTcpRecvMessage[ pData->m_nTcpRecvIdx] = ProtocolId; 
	pData->m_nTcpRecvIdx++; 
	if( pData->m_nTcpRecvIdx == MAX_TCP_MESSAGE_COUNT)
		pData->m_nTcpRecvIdx = 0; 	
	g_pFramework->UpdateTcpMessage( 0 );//Update DbgMsg Window
#endif	
	
#if defined( I3_DEBUG)
	m_iRecvPacketCount++; 
	m_iRecvPacketSize += pPacket->GetPacketSize(); 
#endif

	//소스를 보기 편하게 하기 위해서 변경합니다. 10-13
	//    Class 	
	//0 : OPER 관리							[ Game, Auth 공통]			Super User 
	//1 : Auth Server						[ GameServer <-> AuthServer  ]
	//2 : Auth Server NC					[ GameServer <-> AuthServer <-> NC]
	//3 : QATEAM TEST용 패킷				[ GameServer -> Client ]	Test User
	//5 : Clan								[ GameServer <-> Client ]
	//...................
	//9 : ServerM(서버에서 오는 메시지)		[ GameServer -> Client ]	User
	//A : Base(유저정보, 로그인, 채널 )		[ GameServer <-> Client ]	User
	//B : Lobby								[ GameServer <-> Client ]	User
	//C : Inventory							[ GameServer <-> Client ]	User
	//D : Shop								[ GameServer <-> Client ]	User
	//E : Room  							[ GameServer <-> Client ]	User
	//F : Room (Battle)						[ GameServer <-> Client ]	User
	switch( ProtocolId & 0xFF00)
	{
	case PROTOCOL_AUTH			: _PacketParsingAuth_server(pPacket);	break;
	case PROTOCOL_SERVER_MESSAGE: _PacketParsingServer_message(pPacket);break;

	case PROTOCOL_LOGIN			:
	case PROTOCOL_CHAR			:
	case PROTOCOL_BASE			: _PacketParsingBase(pPacket);			break;
	case PROTOCOL_LOBBY			: _PacketParsingLobby(pPacket);			break;
	case PROTOCOL_INVENTORY		: _PacketParsingInventory(pPacket);		break;
	case PROTOCOL_SHOP			: _PacketParsingShop(pPacket);			break;	
	case PROTOCOL_ROOM			: _PacketParsingRoom(pPacket);			break;
	case PROTOCOL_BATTLE		: _PacketParsingBattle(pPacket);		break;
	case PROTOCOL_CS			: _PacketParsingClan(pPacket);			break;
	case PROTOCOL_MEDAL			: _PacketParsingMedal(pPacket);			break;
	case PROTOCOL_SKILL			: _PacketParsingBase(pPacket);			break;
	case PROTOCOL_GACHA			: _PacketParsingGacha(pPacket);			break;	
	case PROTOCOL_QUICKJOIN		: _PacketParsingQuickJoin(pPacket);		break;
	case PROTOCOL_CHEAT			: _PacketParsingCheat(pPacket);			break;
	case PROTOCOL_GMCHAT		: _PacketParsingGMAuthority(pPacket);	break;
	case PROTOCOL_CLAN_WAR		: _PacketParsingClanWar(pPacket);		break;

	case PROTOCOL_COMMUNITY		: _PacketParsingBase(pPacket);			break;
	case PROTOCOL_RS_IGS		: _PacketParsingRsIgs(pPacket);			break;
	default: break;
	}
#if 0	
	DumpRecvPacket(ProtocolId);  
#endif 
	return; 
}  

void ClientTCPSocket::RegisterParser( UINT32 protocol, netFunc func)
{
	switch( protocol & 0xFF00)
	{
	case PROTOCOL_AUTH			:	m_NetFuncMap_Auth.insert(i3::unordered_map< UINT32, netFunc >::value_type( protocol, func));	break;
	case PROTOCOL_SERVER_MESSAGE:	m_NetFuncMap_ServerMessage.insert(i3::unordered_map< UINT32, netFunc >::value_type( protocol, func));	break;

	case PROTOCOL_LOGIN			:
	case PROTOCOL_CHAR			:
	case PROTOCOL_BASE			:	m_NetFuncMap_Base.insert(i3::unordered_map< UINT32, netFunc >::value_type( protocol, func));	break;
	case PROTOCOL_LOBBY			:	m_NetFuncMap_Lobby.insert(i3::unordered_map< UINT32, netFunc >::value_type( protocol, func));	break;
	case PROTOCOL_INVENTORY		:	m_NetFuncMap_Inventory.insert(i3::unordered_map< UINT32, netFunc >::value_type( protocol, func));	break;
	case PROTOCOL_SHOP			:	m_NetFuncMap_Shop.insert(i3::unordered_map< UINT32, netFunc >::value_type( protocol, func));	break;
	case PROTOCOL_ROOM			:	m_NetFuncMap_Room.insert(i3::unordered_map< UINT32, netFunc >::value_type( protocol, func));	break;
	case PROTOCOL_BATTLE		:	m_NetFuncMap_Battle.insert(i3::unordered_map< UINT32, netFunc >::value_type( protocol, func));	break;
	case PROTOCOL_CS			:	m_NetFuncMap_Clan.insert(i3::unordered_map< UINT32, netFunc >::value_type( protocol, func));	break;
	case PROTOCOL_MEDAL			:	m_NetFuncMap_Medal.insert(i3::unordered_map< UINT32, netFunc >::value_type( protocol, func));	break;
	case PROTOCOL_SKILL			:	m_NetFuncMap_Base.insert( i3::unordered_map< UINT32, netFunc >::value_type( protocol, func)); break;
	case PROTOCOL_GACHA			:	m_NetFuncMap_Gacha.insert(i3::unordered_map< UINT32, netFunc >::value_type( protocol, func));	break;
	
	case PROTOCOL_COMMUNITY		:	m_NetFuncMap_Base.insert( i3::unordered_map< UINT32, netFunc >::value_type( protocol, func)); break;
	case PROTOCOL_QUICKJOIN		:	m_NetFuncMap_QuickJoin.insert( i3::unordered_map< UINT32, netFunc>::value_type( protocol, func)); break;
	case PROTOCOL_RS_IGS		:	m_NetFuncMap_RsIgs.insert( i3::unordered_map< UINT32, netFunc>::value_type( protocol, func)); break;
	case PROTOCOL_CHEAT			:	m_NetFuncMap_Cheat.insert(i3::unordered_map< UINT32, netFunc>::value_type( protocol, func)); break;
	case PROTOCOL_GMCHAT		:	m_NetFuncMap_GMAuthority.insert( i3::unordered_map< UINT32, netFunc>::value_type( protocol, func)); break;
	case PROTOCOL_CLAN_WAR		:	m_NetFuncMap_ClanWar.insert( i3::unordered_map< UINT32, netFunc>::value_type( protocol, func)); break;
	default :
		m_NetFuncMap_Base.insert( i3::unordered_map< UINT32, netFunc >::value_type( protocol, func));
		break;
	}
}
