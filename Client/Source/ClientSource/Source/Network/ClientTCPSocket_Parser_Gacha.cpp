#include "pch.h"
#include "ClientTCPSocket.h"
#include "Shop.h"
#include "IGShopContext.h"
#include "IGShopItemController.h"
#include "GachaContext.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "UserInfoContext.h"

namespace
{
	const char * getProtocolName( PROTOCOL protocol)
	{
		switch( protocol)
		{
		case PROTOCOL_GACHA_ENTER_ACK : return "PROTOCOL_GACHA_ENTER_ACK";
		case PROTOCOL_GACHA_LEAVE_ACK : return "PROTOCOL_GACHA_LEAVE_ACK";
		case PROTOCOL_GACHA_ITEM_INFO_ACK : return "PROTOCOL_GACHA_ITEM_INFO_ACK";
		case PROTOCOL_GACHA_SHOP_STATE_ACK : return "PROTOCOL_GACHA_SHOP_STATE_ACK";
		case PROTOCOL_GACHA_PURCHASE_ACK : return "PROTOCOL_GACHA_PURCHASE_ACK";
		case PROTOCOL_GACHA_NOTICE_ACK : return "PROTOCOL_GACHA_NOTICE_ACK";
		case PROTOCOL_GACHA_WINNING_USER_ACK : return "PROTOCOL_GACHA_WINNING_USER_ACK ";
		case PROTOCOL_GACHA_GET_PURCHASE_COUNT_ACK : return "PROTOCOL_GACHA_GET_PURCHASE_COUNT_ACK";
		default :
			I3PRINTLOG(I3LOG_FATAL,  "PROTOCOL_GACHA_XXX 프로토콜 이름 등록해주이소~");
			return "PROTOCOL_GACHA_UNKNOWN";
		}
	}
}

void ClientTCPSocket::_PacketParsingGacha(i3NetworkPacket  * pPacket)
{
	i3::unordered_map< UINT32, netFunc >::iterator it;
	it = m_NetFuncMap_Gacha.find( pPacket->GetProtocolID());

	if ( it != m_NetFuncMap_Gacha.end() )
	{
		I3PRINTLOG(I3LOG_QA, "* %s -->", getProtocolName( pPacket->GetProtocolID()));
		netFunc & mf = it->second;
		(this->*mf)(pPacket);
		I3PRINTLOG(I3LOG_QA, "* %s <--", getProtocolName( pPacket->GetProtocolID()));
	}
	else
		__Parse_UnknownProtocol( pPacket);
}

void ClientTCPSocket::__Register_Gacha_Function( void)
{
	RegisterParser( PROTOCOL_GACHA_ENTER_ACK,				&ClientTCPSocket::__Parse_Gacha_Enter);
	RegisterParser( PROTOCOL_GACHA_LEAVE_ACK,				&ClientTCPSocket::__Parse_Gacha_Leave);
	RegisterParser( PROTOCOL_GACHA_ITEM_INFO_ACK,			&ClientTCPSocket::__Parse_Gacha_ItemInfo);
	RegisterParser( PROTOCOL_GACHA_SHOP_STATE_ACK,			&ClientTCPSocket::__Parse_Gacha_State);
	RegisterParser( PROTOCOL_GACHA_PURCHASE_ACK,			&ClientTCPSocket::__Parse_Gacha_Purchase);
	RegisterParser( PROTOCOL_GACHA_NOTICE_ACK,				&ClientTCPSocket::__Parse_Gacha_Notice);
	RegisterParser( PROTOCOL_GACHA_WINNING_USER_ACK ,		&ClientTCPSocket::__Parse_Gacha_WinningUserAck);
	RegisterParser( PROTOCOL_GACHA_GET_PURCHASE_COUNT_ACK,	&ClientTCPSocket::__Parse_Gacha_Purchase_Count);
}


void ClientTCPSocket::__Parse_Gacha_Enter( i3NetworkPacket * pPacket)
{
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_ENTER_GACHA );
}

void ClientTCPSocket::__Parse_Gacha_Leave( i3NetworkPacket * pPacket)
{
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_LEAVE_GACHA );
}

void ClientTCPSocket::__Parse_Gacha_ItemInfo( i3NetworkPacket * pPacket)
{
	GACHA_GROUP_INFO  TempGachaGroup;

	bool bActive = false;
	bool bLoadGachaShopFile = false;

	pPacket->ReadData(&bLoadGachaShopFile,	sizeof(bool));

	if( bLoadGachaShopFile ) 
	{
		// GachaShop.dat에서 정보를 load 합니다.
		GachaContext::i()->LoadGachaShopFile();
		return;
	}

	pPacket->ReadData(&TempGachaGroup.m_eGachaGroup,	sizeof(GACHA_GROUP));
	pPacket->ReadData(&bActive,							sizeof(bool));

	if( bActive )
	{
		//pPacket->ReadData(&TempGachaGroup.m_ui32PaymentGoodsID,		sizeof(UINT32));
		//pPacket->ReadData(&TempGachaGroup.m_i32RequireTicket,		sizeof(INT32));
		//pPacket->ReadData(&TempGachaGroup.m_i32GloryCount,			sizeof(INT32));
		pPacket->ReadData(&TempGachaGroup.m_ui32LuckyGoodsID,		sizeof(UINT32));

		pPacket->ReadData(&TempGachaGroup.m_i32WinItemCount,		sizeof(INT32));
		pPacket->ReadData(TempGachaGroup.m_arWinItems,				sizeof(GACHA_ITEM_INFO)*TempGachaGroup.m_i32WinItemCount);

		pPacket->ReadData(&TempGachaGroup.m_i32RandomItemCnt,		sizeof(INT32));
		pPacket->ReadData(TempGachaGroup.m_arRandomGoodsIDs,		sizeof(UINT32)*TempGachaGroup.m_i32RandomItemCnt);

		if(TempGachaGroup.m_eGachaGroup > GACHA_GROUP_NONE)
		{

			GachaContext::i()->SetGachaData(TempGachaGroup.m_eGachaGroup, &TempGachaGroup);
			GameEventReceiver::i()->PushReceivedGameEvent( EVENT_ENTER_GACHA_INFO, (INT32)TempGachaGroup.m_eGachaGroup );
		}
	}

	if( TempGachaGroup.m_eGachaGroup >= GACHA_GROUP_COUNT-1 ) 
	{
		// 마지막 데이터라면 파일로 남깁니다.
		if( !GachaContext::i()->SaveGachaShopFile() )
		{
			I3PRINTLOG(I3LOG_QA, "Fail Save GachaShopFile ");	
		}
		else
		{
			I3PRINTLOG(I3LOG_QA, "Success Save GachaShopFile ");	
		}
	}

}

void ClientTCPSocket::__Parse_Gacha_State( i3NetworkPacket * pPacket)
{
	GACHA_GROUP eGroup;
	INT32 i32WinItemCount = 0;

	pPacket->ReadData(&eGroup,			sizeof(GACHA_GROUP));
	pPacket->ReadData(&i32WinItemCount, sizeof(INT32));

	if( eGroup <= GACHA_GROUP_NONE ||  eGroup >= GACHA_GROUP_COUNT ) return;

	GACHA_GROUP_INFO* pGroupinfo = GachaContext::i()->SetGachaData((INT32)eGroup);
	for(INT32 i = 0; i < i32WinItemCount; ++i)
	{
		pPacket->ReadData(&pGroupinfo->m_arWinItems[i].m_i32RemainAmount, sizeof(INT32));
	}

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_END_GACHA_STATE );
}


void ClientTCPSocket::__Parse_Gacha_Purchase_Count( i3NetworkPacket * pPacket)
{
	GACHA_GROUP eGroup;
	INT32 i32WinItemCount = 0;

	pPacket->ReadData(&eGroup,			sizeof(GACHA_GROUP));
	pPacket->ReadData(&i32WinItemCount, sizeof(INT32));

	if( eGroup <= GACHA_GROUP_NONE ||  eGroup >= GACHA_GROUP_COUNT ) return;

	GACHA_GROUP_INFO* pGroupinfo = GachaContext::i()->SetGachaData((INT32)eGroup);
	for(INT32 i = 0; i < i32WinItemCount; ++i)
	{
		pPacket->ReadData(&pGroupinfo->m_arWinItems[i].m_i32PurchaseAmount, sizeof(INT32));
	}

	// EVENT_END_GACHA_STATE 이거 바꿔야될듯?
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_END_GACHA_STATE );
}


void ClientTCPSocket::__Parse_Gacha_Purchase( i3NetworkPacket * pPacket)
{
	// PACKET_GACHA_PURCHASE_REQ에 대한 결과 패킷

	// PACKET_GACHA_RESULT_HEADER 를 먼저읽고
	// m_i32ResultCount 만큼 PACKET_GACHA_RESULT_DATA 를 더읽는다.
	GachaContext::i()->ClearGachaRst();

	PACKET_GACHA_RESULT_HEADER stGachaRstHeader;
	pPacket->ReadData(&stGachaRstHeader, sizeof(stGachaRstHeader));	
	GachaContext::i()->SetGachaRstHeader(&stGachaRstHeader);
	
	UserInfoContext::i()->SetMyCash(stGachaRstHeader.m_ui32Cash);
	UserInfoContext::i()->SetMyPoint(stGachaRstHeader.m_ui32Point);
	
	for(INT32 i=0; i<stGachaRstHeader.m_i32ResultCount; i++)
	{
		PACKET_GACHA_RESULT_DATA stGachaResult;
		pPacket->ReadData(&stGachaResult, sizeof(PACKET_GACHA_RESULT_DATA));

		GachaContext::i()->PushGachaRstData(&stGachaResult);
	}
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_REFRESH_CASH);
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_START_GACHA , (INT32)stGachaRstHeader.m_eErrorCode);
}

void ClientTCPSocket::__Parse_Gacha_Notice( i3NetworkPacket * pPacket)
{
	UINT8 ui8NoticeCount = 0;
	pPacket->ReadData(&ui8NoticeCount, sizeof(UINT8));

	TTCHAR strNoticeAll[GACHA_NOTICE_LENGTH*GACHA_NOTICE_COUNT];
	strNoticeAll[0]= 0;
	for( UINT8 i = 0 ; i < ui8NoticeCount; ++i )
	{
		UINT8 ui8Size = 0;
		TTCHAR strNotice[GACHA_NOTICE_LENGTH];

		pPacket->ReadData(&ui8Size, sizeof(UINT8));
		ui8Size = MIN( ui8Size, GACHA_NOTICE_LENGTH -1 );
		pPacket->ReadData(strNotice, sizeof(TTCHAR)*ui8Size);
		strNotice[ui8Size] = 0;

		i3::generic_string_cat(strNoticeAll,strNotice);
		i3::generic_string_cat(strNoticeAll, _TT("            ") );
	}

	i3::rc_wstring rc_wstrNotice = strNoticeAll;

	GachaContext::i()->SetGachaNotice(rc_wstrNotice );
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_GACHA_ANNOUNCE, GachaContext::EANNOUNCE_NOTICE);

}

void ClientTCPSocket::__Parse_Gacha_WinningUserAck( i3NetworkPacket * pPacket)
{
	PACKET_GACHA_WINNING_USER_ACK	stLuckyUser;
	S2MORecvPacket( &stLuckyUser, pPacket->GetPacketBuffer() );

	i3::rc_wstring wstrLuckyUser = &stLuckyUser.m_strNickName;

	// 9803 : 채팅창에 당첨된 유저 닉네임이 전부 출력되지 않도록 수정
	i3::rc_wstring temp = L"**";
	i3::erase_string(wstrLuckyUser, 0, i3::generic_string_size(temp));
	i3::insert_string(wstrLuckyUser, 0, temp, i3::generic_string_size(temp));

	GachaContext::i()->SetGachaLuckyUser( wstrLuckyUser );
	GachaContext::i()->SetGachaLuckyGoodsId( stLuckyUser.m_ui32LuckyGoodsID );
	GachaContext::i()->SetJackpotAnnounceChat();
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_GACHA_ANNOUNCE, GachaContext::EANNOUNCE_WINITEMUSER);
}

