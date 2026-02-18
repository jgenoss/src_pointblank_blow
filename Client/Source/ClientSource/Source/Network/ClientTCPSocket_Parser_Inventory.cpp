#include "pch.h"
#include "ClientTCPSocket.h"


#include "UI/UIUtil.h"
#include "UserInfoContext.h"
#include "MailBoxContext.h"

namespace
{
	const char * getProtocolName( PROTOCOL protocol)
	{
		switch( protocol)
		{
		case PROTOCOL_INVENTORY_GET_INFO_ACK : return "PROTOCOL_INVENTORY_GET_INFO_ACK";
		case PROTOCOL_INVENTORY_ENTER_ACK : return "PROTOCOL_INVENTORY_ENTER_ACK";
		case PROTOCOL_INVENTORY_LEAVE_ACK : return "PROTOCOL_INVENTORY_LEAVE_ACK";
		default:
			I3PRINTLOG(I3LOG_FATAL,  "PROTOCOL_INVENTORY_XXX 프로토콜 이름 등록해주이소~");
			return "PROTOCOL_INVENTORY_UNKNOWN";
		}
	}
}

void ClientTCPSocket::_PacketParsingInventory( i3NetworkPacket * pPacket)
{
	i3::unordered_map< UINT32, netFunc >::iterator it;
	it = m_NetFuncMap_Inventory.find( pPacket->GetProtocolID());

	if ( it != m_NetFuncMap_Inventory.end() )
	{
		I3PRINTLOG(I3LOG_QA, "* %s -->", getProtocolName( pPacket->GetProtocolID()));
		netFunc & mf = it->second;
		(this->*mf)(pPacket);
		I3PRINTLOG(I3LOG_QA, "* %s <--", getProtocolName( pPacket->GetProtocolID()));
	}
	else
		__Parse_UnknownProtocol( pPacket);
}

void ClientTCPSocket::__Register_Inventory_Function( void)
{
	RegisterParser( PROTOCOL_INVENTORY_GET_INFO_ACK,		&ClientTCPSocket::__Parse_Inventory_GetInfo);
	RegisterParser( PROTOCOL_INVENTORY_ENTER_ACK,			&ClientTCPSocket::__Parse_Inventory_Enter);
	RegisterParser( PROTOCOL_INVENTORY_LEAVE_ACK,			&ClientTCPSocket::__Parse_Inventory_Leave);
}

void ClientTCPSocket::__Parse_Inventory_GetInfo( i3NetworkPacket * pPacket)
{
	PACKET_INVENTORY_GET_INFO_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	CInvenList::i()->AddItem( &Recv.m_Inventory, Recv.m_Inventory.GetCount() );

	//제압 모드 보상으로 받은 아이템일 경우는 바로 적용 하지 않습니다.
	//보상 받는 시점이 배틀 상태가 아니기 때문에 패킷을 보내게 되면 해킹 이슈로 서버 접속 끈김.
	if( Recv.m_ui8ChangeType != ITEM_INSERT_REQUEST_DOMI_REWARD)
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_INVEN_GETINFO,Recv.m_Inventory[0]._ui32ItemID);

	// 장성급 베레모 지급 관련
	if( Recv.m_ui8ChangeType == ITEM_INSERT_REQUEST_GENERAL )
	{
		if( 0 < Recv.m_Inventory.GetCount() )
		{
			UserInfoContext::i()->SetGeneralRankInfo( &Recv.m_Inventory );
		}
	}

	// LoginEvent Popup
	if( Recv.m_ui8ChangeType == ITEM_INSERT_REQUEST_EVENT_CONNECT)
	{
		// 11338 로그인 보상 팝업이 출력된 상태에서 좌측 배너가 클릭 가능했던 문제 수정
		//GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_SUCCESS"));
		MailBoxContext::i()->setRecvLoginEvent(true);
	}	
}

#ifdef USE_AAS_SYSTEM_FULL
void ClientTCPSocket::__Parse_Inventory_Enter( i3NetworkPacket * pPacket)
{
	INT32 i32Result;
	UINT32 nReturnCode;

	pPacket->ReadData(&i32Result, sizeof(INT32));
	pPacket->ReadData(&nReturnCode, sizeof(UINT32));

	if ( EV_SUCCESSED( i32Result ) )
	{
		UserInfoContext::i()->SetInvenServerTime(nReturnCode);
		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_ENTER_INVENTORY, nReturnCode);
	}
	else
	{
		// 셧다운 제한으로 인해 인벤토리에 진입할 수 없다는 에러메세지를 출력해주세요
	}
}
#else
void ClientTCPSocket::__Parse_Inventory_Enter( i3NetworkPacket * pPacket)
{
	UINT32 nReturnCode;
	pPacket->ReadData(&nReturnCode, sizeof(UINT32));
	UserInfoContext::i()->SetInvenServerTime(nReturnCode);
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_ENTER_INVENTORY, nReturnCode);
}
#endif

void ClientTCPSocket::__Parse_Inventory_Leave( i3NetworkPacket * pPacket)
{
	PACKET_INVENTORY_LEAVE_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_LEAVE_INVENTORY, Recv.m_TResult);
}

