#include "pch.h"
#include "ClientTCPSocket.h"

#include "UI/UIUtil.h"


#include "InviteContext.h"
#include "LobbyContext.h"
#include "UserInfoContext.h"
#include "GameNetworkManager.h"
#include "StageBattle/NetworkContext.h"

#include "ValidCheck.h"

#include "i3Base/string/ext/mb_to_utf16.h"

#include "UI/UICharaInfoMgr.h"

#include "../USG_Miscs.h"


namespace
{
	const char * getProtocolName( PROTOCOL protocol)
	{
		switch( protocol)
		{
		case PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS : return "PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS";
		case PROTOCOL_SERVER_MESSAGE_DISCONNECTIONSUCCESS : return "PROTOCOL_SERVER_MESSAGE_DISCONNECTIONSUCCESS";
		case PROTOCOL_SERVER_MESSAGE_KICK_PLAYER : return "PROTOCOL_SERVER_MESSAGE_KICK_PLAYER";
		case PROTOCOL_SERVER_MESSAGE_KICK_BATTLE_PLAYER : return "PROTOCOL_SERVER_MESSAGE_KICK_BATTLE_PLAYER";
		case PROTOCOL_SERVER_MESSAGE_INVITED : return "PROTOCOL_SERVER_MESSAGE_INVITED";
		case PROTOCOL_SERVER_MESSAGE_ERROR : return "PROTOCOL_SERVER_MESSAGE_ERROR";
		case PROTOCOL_SERVER_MESSAGE_ANNOUNCE : return "PROTOCOL_SERVER_MESSAGE_ANNOUNCE";
		case PROTOCOL_SERVER_MESSAGE_FRIEND_INVITED : return "PROTOCOL_SERVER_MESSAGE_FRIEND_INVITED";
		case PROTOCOL_SERVER_MESSAGE_ROOM_DESTROY : return "PROTOCOL_SERVER_MESSAGE_ROOM_DESTROY";
		case PROTOCOL_SERVER_MESSAGE_GAMEGUARD_ERROR : return "PROTOCOL_SERVER_MESSAGE_GAMEGUARD_ERROR";
		case PROTOCOL_SERVER_MESSAGE_EVENT_COUPON : return "PROTOCOL_SERVER_MESSAGE_EVENT_COUPON";
		case PROTOCOL_SERVER_MESSAGE_CHANGE_INVENTORY : return "PROTOCOL_SERVER_MESSAGE_CHANGE_INVENTORY";
		default :
			I3PRINTLOG(I3LOG_FATAL,  "PROTOCOL_SM_XXX 프로토콜 이름 등록해주이소~");
			return "PROTOCOL_SM_UNKNOWN";
		}
	}
}

void ClientTCPSocket::_PacketParsingServer_message(i3NetworkPacket  * pPacket)
{
	i3::unordered_map< UINT32, netFunc >::iterator it;
	it = m_NetFuncMap_ServerMessage.find( pPacket->GetProtocolID());

	if ( it != m_NetFuncMap_ServerMessage.end() )
	{
		I3PRINTLOG(I3LOG_QA, "* %s -->", getProtocolName( pPacket->GetProtocolID()));
		netFunc & mf = it->second;
		(this->*mf)(pPacket);
		I3PRINTLOG(I3LOG_QA, "* %s <--", getProtocolName( pPacket->GetProtocolID()));
	}
	else
		__Parse_UnknownProtocol( pPacket);
}

void ClientTCPSocket::__Register_ServerMessage_Function( void)
{
	RegisterParser( PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS,			&ClientTCPSocket::__Parse_ServerMessage_ConnectionSuccess);
	RegisterParser( PROTOCOL_SERVER_MESSAGE_DISCONNECTIONSUCCESS,		&ClientTCPSocket::__Parse_ServerMessage_DisconnectionSuccess);
	RegisterParser( PROTOCOL_SERVER_MESSAGE_KICK_PLAYER,				&ClientTCPSocket::__Parse_ServerMessage_KickPlayer);
	RegisterParser( PROTOCOL_SERVER_MESSAGE_KICK_BATTLE_PLAYER,			&ClientTCPSocket::__Parse_ServerMessage_KickPlayerFromBattle);
	RegisterParser( PROTOCOL_SERVER_MESSAGE_INVITED,					&ClientTCPSocket::__Parse_ServerMessage_Invited);
	RegisterParser( PROTOCOL_SERVER_MESSAGE_ERROR,						&ClientTCPSocket::__Parse_ServerMessage_Error);
	RegisterParser( PROTOCOL_SERVER_MESSAGE_ANNOUNCE,					&ClientTCPSocket::__Parse_ServerMessage_Announce);
	RegisterParser( PROTOCOL_SERVER_MESSAGE_FRIEND_INVITED,				&ClientTCPSocket::__Parse_ServerMessage_FriendInvited);
	RegisterParser( PROTOCOL_SERVER_MESSAGE_ROOM_DESTROY,				&ClientTCPSocket::__Parse_None);		/// 구현필요
	RegisterParser( PROTOCOL_SERVER_MESSAGE_GAMEGUARD_ERROR,			&ClientTCPSocket::__Parse_ServerMessage_GameguardError);
	RegisterParser( PROTOCOL_SERVER_MESSAGE_EVENT_COUPON,				&ClientTCPSocket::__Parse_ServerMessage_EventCoupon);
	RegisterParser( PROTOCOL_SERVER_MESSAGE_CHANGE_INVENTORY,			&ClientTCPSocket::__Parse_ServerMessage_ChangeInventory);	// 사용안함 (1.0)
}

void ClientTCPSocket::__Parse_ServerMessage_ConnectionSuccess( i3NetworkPacket * pPacket)
{
	PACKET_H_SERVER_MESSAGE_CONNECTIONSUCCESS PacketHeader;
	pPacket->ReadData(&PacketHeader, sizeof(PACKET_H_SERVER_MESSAGE_CONNECTIONSUCCESS));
		
	m_ConnectionInfo.Reset();
	m_ConnectionInfo.m_SessionIndex = PacketHeader.m_ui32SessionIdx;
	m_ConnectionInfo.m_GamePort = PacketHeader.m_ui16ClientPort;
	m_i16PacketRandSeed = PacketHeader.m_i16PacketRandSeed;

	i3Net::GetName(GetSocket(), &m_InAddr);
	m_ConnectionInfo.m_IsRealIp = (m_InAddr.sin_addr.s_addr == PacketHeader.m_ui32ConIp) ? true : false;

	m_ConnectionInfo.m_MyUdpInfo.m_nRealIP = PacketHeader.m_ui32ConIp;
	m_ConnectionInfo.m_MyUdpInfo.m_nRealPort = 0;
	m_ConnectionInfo.m_MyUdpInfo.m_nLocalIP = m_InAddr.sin_addr.s_addr;
	m_ConnectionInfo.m_MyUdpInfo.m_nLocalPort = m_ConnectionInfo.m_GamePort;

	switch(m_TargetServerType)
	{
	case SERVER_TYPE_GATEWAY:
		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CONNECT_SERVER_GATEWAY );
		I3PRINTLOG(I3LOG_QA," ** SERVER_TYPE_GATEWAY");
		break;
	case SERVER_TYPE_GAME:
#if defined(AUTO_LOBBY)
		GameEventSender::i()->SetEventOnce(EVENT_USER_LEAVE);
#else
		/*if (g_pEnvSet->m_nUIVersion > 0)
			GameEventSender::i()->SetEventOnce(EVENT_USER_LEAVE);
		else*/
		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CONNECT_SERVER_GAME );
#endif
		I3PRINTLOG(I3LOG_QA," ** SERVER_TYPE_GAME");
		break;
	default:	I3ASSERT_0;	break;
	}

	m_ConnectionSuccess = true;
}


void ClientTCPSocket::__Parse_ServerMessage_KickPlayer( i3NetworkPacket * pPacket)
{
	bool isGMKick = false;
	pPacket->ReadData(&isGMKick, sizeof(bool));

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_M_KICK_USER, isGMKick);
}

void ClientTCPSocket::__Parse_ServerMessage_Invited( i3NetworkPacket * pPacket)
{
	// 초대 정보 읽기
	INVITE_INFO info;
	TTCHAR szNick[NET_NICK_NAME_SIZE];				
	pPacket->ReadData(szNick, NET_NICK_NAME_SIZE*sizeof(TTCHAR));

	info._wstr_nick = szNick;

	pPacket->ReadData(&info._idxRoom,	sizeof(INT32));
	pPacket->ReadData(&info._iUID,		sizeof(INT64));
	pPacket->ReadData(info._pw,			NET_ROOM_PW);

	// 초대 정보는 1개만 쌓는다 - 출력 중 초대메시지 포함 2개만 허용
//	i3TStack<INVITE_INFO>* pStack = InviteContext::i()->getStackInviteInfo();

	if (InviteContext::i()->GetNumInviteInfo() < 1)
	{
		InviteContext::i()->PushInviteInfo(info);
	}

	/* PushEvent하지 않음 */
}

void ClientTCPSocket::__Parse_ServerMessage_FriendInvited( i3NetworkPacket * pPacket)
{
	SERVER_INVITER_INFO R_UserInfo; 
	pPacket->ReadData(&R_UserInfo ,	sizeof(SERVER_INVITER_INFO) );
}

void ClientTCPSocket::__Parse_ServerMessage_Announce( i3NetworkPacket * pPacket)
{
	// 운영툴 공지
	PACKET_MESSAGE_ANNOUNCE_ACK	Packet;
	S2MORecvPacket( &Packet, pPacket->GetPacketBuffer() );

	TTCHAR strAnnounce[NOTICE_STRING_COUNT] = {0,};

	Packet.m_strChatMessage.GetValue( strAnnounce, _countof(strAnnounce) ) ;

	i3::rc_wstring wstrAnnounce = strAnnounce;
	INT32 i32RGB = Packet.m_i32RGB;

	switch(Packet.m_i32Type)
	{
	case NOTICE_TYPE_NORMAL		:
		g_pFramework->SetNotice_Temporary(wstrAnnounce);
		break;
	case NOTICE_TYPE_EMERGENCY	:
		GameUI::MsgBox( MSG_TYPE_INBATTLE, wstrAnnounce);
		break;
	case NOTICE_TYPE_SLIDING:
		g_pFramework->SetNotice_Permanant(wstrAnnounce, i32RGB);
		break;
	case NOTICE_TYPE_CHAT:
		g_pFramework->setChatNotice(wstrAnnounce, i32RGB);
		g_pFramework->getChatBox()->PutSystemChatting(g_pFramework->getChatNotice());
		break;
	}
}

void ClientTCPSocket::__Parse_ServerMessage_Error( i3NetworkPacket * pPacket)
{
	UINT32 ui32CutTime;
	UINT32 ui32ErrorCode;
	pPacket->ReadData( &ui32CutTime,	sizeof( UINT32 ) );
	pPacket->ReadData( &ui32ErrorCode,	sizeof( UINT32 ) );
	CGameNetworkManager::i()->DisconnectTCP();
	switch( ui32ErrorCode )
	{
	case EVENT_ERROR_GAMEGUARD_ERROR:
		{
			GameEventReceiver::i()->PushReceivedGameEvent(EVENT_M_GAMEGUARD_ERROR);
			I3TRACE("PROTOCOL_SERVER_MESSAGE_GAMEGUARD_ERROR %x\n" , ui32ErrorCode);
			DEV_LOG("net error 3 %x[%d]", ui32ErrorCode, m_UsingType);
		}
		break;
	case EVENT_ERROR_LOGIN_ERROR:
	case EVENT_ERROR_CHANNEL_TIMEOUT:
		{
			GameEventReceiver::i()->PushReceivedGameEvent(EVENT_M_DISCONNECTION, ui32ErrorCode);
			I3TRACE("PROTOCOL_SERVER_MESSAGE_DISCONNECTIONSUCCESS %x\n" , ui32ErrorCode);
			DEV_LOG("net error 2 %x[%d]", ui32ErrorCode, m_UsingType);
		}
		break;
	case EVENT_ERROR_LOGIN_INVALID_VERSION:
	case EVENT_ERROR_GAME_GUARD_INVALID_VERSION:
		{
			GameEventReceiver::i()->PushReceivedGameEvent(EVENT_M_DISCONNECTION, ui32ErrorCode);
			I3TRACE("INVALID VERSION\n");
			DEV_LOG("INVALID VERSION");
		}
		break;
	case EVENT_ERROR_AAS_FORCE_LOGOUT:
		{
			GameEventReceiver::i()->PushReceivedGameEvent(EVENT_M_SHUTDOWNTIME, ui32ErrorCode);
			I3TRACE("ShutDown System\n");
			DEV_LOG("ShutDown System");
		}
		break;
	case EVENT_ERROR_GM_KICK_USER:
		{
			GameEventReceiver::i()->PushReceivedGameEvent(EVENT_GM_EXIT_USER);
			I3TRACE("GM EXIT USER %x\n" , ui32ErrorCode);
		}
		break;
	case EVENT_ERROR_HACKING_TOOL:
		{
			usg_misc::HackToolDetectionCtx::i()->isHackDetected = true;
			GameEventReceiver::i()->PushReceivedGameEvent(EVENT_HACKTOOL_DETECTED);
		}
		break;
	default:
		{
			switch( ui32ErrorCode & 0xFFFF0000 )
			{
			case EVENT_ERROR_HACKING_USER:
				{
					I3PRINTLOG(I3LOG_QA, "핵 감지에 걸렸어요.. 서버팀에 문의하세요...");
				}
				break;
			default:
				{
					GameEventReceiver::i()->PushReceivedGameEvent(EVENT_M_ERROR);
					I3TRACE("PROTOCOL_SERVER_MESSAGE_ERROR %x\n", ui32ErrorCode);
					DEV_LOG("net error 1 %x[%d]", ui32ErrorCode, m_UsingType);
				}
				break;
			}
		}
		break;
	}
}

void ClientTCPSocket::__Parse_ServerMessage_DisconnectionSuccess( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData( &Temp,	sizeof( INT32 ) );
	CGameNetworkManager::i()->DisconnectTCP();
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_M_DISCONNECTION);
	I3TRACE("PROTOCOL_SERVER_MESSAGE_DISCONNECTIONSUCCESS %x\n" , Temp);
	DEV_LOG("net error 2 %x[%d]", Temp, m_UsingType);
}

void ClientTCPSocket::__Parse_ServerMessage_GameguardError( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData( &Temp,	sizeof( INT32 ) );
	CGameNetworkManager::i()->DisconnectTCP();
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_M_GAMEGUARD_ERROR);
	I3TRACE("PROTOCOL_SERVER_MESSAGE_GAMEGUARD_ERROR %x\n" , Temp);
	DEV_LOG("net error 3 %x[%d]", Temp, m_UsingType);
}

void ClientTCPSocket::__Parse_ServerMessage_KickPlayerFromBattle( i3NetworkPacket * pPacket)
{
	INT32 uErrorFlag;
	pPacket->ReadData(&uErrorFlag, sizeof(UINT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_M_KICK_USER, uErrorFlag);
}

namespace inner
{
	bool is_true_bit(UINT16 val, size_t bit)
	{
		return ( val & (0x1 << bit) ) ? true : false;
	}
}

void ClientTCPSocket::__Parse_ServerMessage_ChangeInventory( i3NetworkPacket * pPacket)
{
	PACKET_SERVER_MESSAGE_CHANGE_INVENTORY Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	S2_CHAR_CHANGE_EQUIP_INFO* pEquipInfo;
	for( INT32 i = 0 ; i < Recv.m_ChangeEquipInfo.GetCount() ; i++ )
	{
		pEquipInfo = &Recv.m_ChangeEquipInfo[ i ];

		CCharaInfoContext::i()->SetCharaEquipInfo( pEquipInfo->m_ui8SlotIdx, pEquipInfo->m_aCharEquipment );
	}

	// 서버로 동기화 합니다.
	CharaInfoMgr::instance()->list_up();

	// Equip 세팅 후 UI CharaInfo는 바꿔준다
	for (INT32 i = 0; i < TEAM_COUNT; ++i)
	{
		const CHARA_PARTS_INFO * pPartsInfo = CCharaInfoContext::i()->GetMainCharaPartsInfo((TEAM_TYPE)i);
		const CHARA_WEAPON_INFO* pWeaponInfo = CCharaInfoContext::i()->GetMainCharaWeaponInfo((TEAM_TYPE)i);

		if(pPartsInfo != nullptr)
			UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts(pPartsInfo);
		if(pWeaponInfo != nullptr)
			UserInfoContext::i()->SetMyUICharaInfo()->SetWeaponList(pWeaponInfo);
	}

	COMMON_PARTS_INFO CommonInfo;
	// vv3 - check
	if( ValidCheck::Item::isDinoItemID( Recv.m_aCommonEquips[ CHAR_EQUIPMENT_COMMON_DINO ].m_TItemID) == true)
		CommonInfo.SetParts( &Recv.m_aCommonEquips[ CHAR_EQUIPMENT_COMMON_DINO ] );
	else
		CommonInfo.SetPartsItemID( DEFAULT_DINO );

	CCharaInfoContext::i()->SetCommonPartsInfo( CommonInfo );

	if( g_pFramework->IsBattleStage() == false)
	{
		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_INVEN_CHANGE_CHARA_EQUIP);
	}

	UserInfoContext::i()->SetDefaultItemChanged(true);
}

void ClientTCPSocket::__Parse_ServerMessage_EventCoupon( i3NetworkPacket * pPacket)
{
	COUPON_INFO * couponInfo = LobbyContext::i()->setCouponInfo();
	UINT32 ui32Point = 0;

	pPacket->ReadData( couponInfo, sizeof(COUPON_INFO));
	pPacket->ReadData( &ui32Point, sizeof(UINT32));
	UserInfoContext::i()->SetMyPoint(ui32Point);

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_REFRESH_CASH);
}