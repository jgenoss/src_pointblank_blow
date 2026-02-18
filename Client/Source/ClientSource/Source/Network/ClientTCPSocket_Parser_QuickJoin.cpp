#include "pch.h"
#include "ClientTCPSocket.h"
#include "../QuickJoinContext.h"

namespace
{
	const char * getProtocolName( PROTOCOL protocol)
	{
		switch( protocol)
		{
		case PROTOCOL_QUICKJOIN_START_ACK : return "PROTOCOL_QUICKJOIN_START_ACK";
		case PROTOCOL_QUICKJOIN_ENTER_REC_ROOM_ACK : return "PROTOCOL_QUICKJOIN_ENTER_REC_ROOM_ACK";
		default :
			I3PRINTLOG(I3LOG_FATAL,  "PROTOCOL_QUICKJOIN_XXX 프로토콜 이름 등록해주이소~");
			return "PROTOCOL_QUICKJOIN_UNKNOWN";
		}
	}
}

void ClientTCPSocket::_PacketParsingQuickJoin( i3NetworkPacket  * pPacket)
{
	i3::unordered_map< UINT32, netFunc >::iterator it;
	it = m_NetFuncMap_QuickJoin.find( pPacket->GetProtocolID());

	if ( it != m_NetFuncMap_QuickJoin.end() )
	{
		I3PRINTLOG(I3LOG_QA, "* %s -->", getProtocolName( pPacket->GetProtocolID()) );
		netFunc & mf = it->second;
		(this->*mf)(pPacket);
		I3PRINTLOG(I3LOG_QA, "* %s <--", getProtocolName( pPacket->GetProtocolID()) );
	}
	else
		__Parse_UnknownProtocol( pPacket);
}

void ClientTCPSocket::__Register_QuickJoin_Function( void)
{
	RegisterParser( PROTOCOL_QUICKJOIN_START_ACK,			&ClientTCPSocket::__Parse_QuickJoin_Start);
	RegisterParser( PROTOCOL_QUICKJOIN_ENTER_REC_ROOM_ACK,	&ClientTCPSocket::__Parse_QuickJoin_SecondBest);
}

void ClientTCPSocket::__Parse_QuickJoin_Start( i3NetworkPacket * pPacket)
{
	PACKET_QUICKJOIN_START_ACK data;
 	pPacket->ReadData(&data, sizeof(PACKET_QUICKJOIN_START_ACK));

	QuickJoinContext* ctx = QuickJoinContext::i();

	ctx->setResult( data.m_i32Rv);
	ctx->setChannelIndex( data.m_ui8ChannelIdx);
	ctx->setRoomIndex( data.m_ui32RoomIdx);
	ctx->setChannelType( data.m_ui8ChannelType);
	ctx->setRoomState( data.m_ui8RoomState);

	if (ctx->getResult() == EVENT_ERROR_SUCCESS ) 
	{
		g_pServerInfo->SetCurGameServerType(static_cast<SChannelType>(ctx->getChannelType()));
		g_pServerInfo->SetCurChannelNumber(ctx->getChannelIndex());
		g_pServerInfo->SetGameServer(ctx->getServerIndex(), S_CHANNEL_TYPE_ALL);
	}
	
	ctx->setQuickJoinSecondInfo()->m_i32ScoreType = data.m_QuickJoin.m_NowScore.ScoreType;
	ctx->setQuickJoinSecondInfo()->m_i32PlayTime = data.m_QuickJoin.m_NowScore.PlayTime;
	for (INT32 i=0; i<2; i++)
	{
		ctx->setQuickJoinSecondInfo()->m_i32Score[i] = data.m_QuickJoin.m_NowScore.Score[i];
	}
	
	ctx->setQuickJoinSecondInfo()->m_ui32StageID = data.m_QuickJoin.m_QuickJoin.m_ui32StageID;

	for (INT32 i=0; i<MAX_QUICK_JOIN_INFO_COUNT; i++)
	{
		ctx->setQuickJoinInfo(i)->m_ui32StageID = data.m_aQuickJoinInfo[i].m_ui32StageID;
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_QUICKJOIN_START);
}

void ClientTCPSocket::__Parse_QuickJoin_SecondBest( i3NetworkPacket * pPacket)
{
	PACKET_QUICKJOIN_ENTER_REC_ROOM_ACK data;
	pPacket->ReadData(&data, sizeof(PACKET_QUICKJOIN_ENTER_REC_ROOM_ACK));

	QuickJoinContext* ctx = QuickJoinContext::i();

	ctx->setResult( data.m_i32Rv);
	I3ASSERT( data.m_ui32ChannelIdx < 256);
	ctx->setChannelIndex( (UINT8) data.m_ui32ChannelIdx);
	ctx->setRoomIndex( data.m_ui32RoomIdx);

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_QUICKJOIN_SECONDBEST);
}