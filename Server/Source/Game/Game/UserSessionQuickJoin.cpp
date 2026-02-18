#include "pch.h"
#include "UserSession.h"
#include "RoomManager.h"

BOOL	CUserSession::_PacketParsingQuickJoin( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &isKilled )
{	
	PROTOCOL	ProtocolID = pPacket->GetProtocolID();	
	BOOL		bSendMessage = FALSE; 

	switch( ProtocolID )
	{
	case PROTOCOL_QUICKJOIN_START_REQ:	// 빠른 입장 시작 요청.
		{
			CHECKVALID_DBINDEX;
			if( m_i32MainTask < GAME_TASK_INFO || m_i32MainTask > GAME_TASK_LOBBY )
			{	// 포지션 체크. 패킷 버립니다.
				break;
			}

			// 데이터 Read, Check
			PACKET_QUICKJOIN_START_REQ PacketReq;
			pPacket->ReadData( &PacketReq,	sizeof(PACKET_QUICKJOIN_START_REQ) );		

			if( PacketReq.m_ui8QuickJoinIdx >= MAX_QUICK_JOIN_INFO_COUNT ) 
			{
				break;
			}

			QUICKJOIN_SEARCH_INFO QS_Info;
			QS_Info.Reset();
			QS_Info.m_eStageUID	= StageID::GetStageUID( PacketReq.m_aQuickJoinInfo[PacketReq.m_ui8QuickJoinIdx].m_ui32StageID );
			QS_Info.m_ui32Rule	= StageID::GetRule( PacketReq.m_aQuickJoinInfo[PacketReq.m_ui8QuickJoinIdx].m_ui32StageID );
			
			PACKET_QUICKJOIN_START_ACK PacketAck;
			i3mem::FillZero( &PacketAck, sizeof( PACKET_QUICKJOIN_START_ACK ));
			i3mem::Copy( PacketAck.m_aQuickJoinInfo, PacketReq.m_aQuickJoinInfo, sizeof( QUICKJOIN_INFO ) * MAX_QUICK_JOIN_INFO_COUNT );
			
			PacketAck.m_i32Rv = g_pRoomManager->SearchQuickJoinRoom(this, &QS_Info ); 
			
			if( EVENT_ERROR_QUICK_JOIN_WRONG_INFO == PacketAck.m_i32Rv ) 
			{
				break;
			}
			
			// 빠른입장 설정정보 DB 저장
			i3mem::Copy( m_aQuickJoinInfo, PacketReq.m_aQuickJoinInfo, sizeof( QUICKJOIN_INFO ) * MAX_QUICK_JOIN_INFO_COUNT );
			ChangeDataBaseInfo( SAVE_DATABASE_QUICK_JOIN );

			// 방검색 성공시 방입장.
			if( EV_SUCCESSED( PacketAck.m_i32Rv ) )
			{
				INT32 i32QuickJoinRv = _OnJoinRoomByQuickJoin( QS_Info.m_i32ChannelIdx, QS_Info.m_i32RoomIdx ) ;
				
				if( EV_SUCCESSED( i32QuickJoinRv ) )
				{	// 성공일 경우 방 슬롯 인덱스가 리턴되므로 변환해줍니다.
					i32QuickJoinRv = EVENT_ERROR_SUCCESS;
				}

				PacketAck.m_i32Rv = i32QuickJoinRv;
			}

			i3mem::Copy( &PacketAck.m_QuickJoin, &QS_Info.m_RoomInfo2ndBest, sizeof( ROOM_INFO_2ND_BEST )  );			
			PacketAck.m_ui8ChannelIdx	= (UINT8)QS_Info.m_i32ChannelIdx;
			PacketAck.m_ui32RoomIdx		= QS_Info.m_i32RoomIdx;
			PacketAck.m_ui8ChannelType	= (UINT8)QS_Info.m_ui8ChannelType;

			bSendMessage = TRUE;
			pSendPacket->SetProtocol( PROTOCOL_QUICKJOIN_START_ACK );			
			pSendPacket->WriteData( &PacketAck, sizeof(PACKET_QUICKJOIN_START_ACK) );		
		}
		break;

	case PROTOCOL_QUICKJOIN_ENTER_2ND_BEST_ROOM_REQ:	// 차선책 방 입장 요청.
		{
			CHECKVALID_DBINDEX;  
		
			if( m_i32MainTask < GAME_TASK_INFO || m_i32MainTask > GAME_TASK_LOBBY )
			{	// 포지션 체크. 패킷 버립니다.
				break;
			}

			PACKET_QUICKJOIN_ENTER_2ND_BEST_ROOM_REQ PacketReq;
			pPacket->ReadData( &PacketReq,	sizeof(PACKET_QUICKJOIN_ENTER_2ND_BEST_ROOM_REQ) );		

			// Check 
			if( PacketReq.m_ui32ChannelIdx >= GAME_CHANNEL_COUNT || PacketReq.m_ui32RoomIdx >= g_pContextMain->m_ui32ChannelSessionCount  ) 
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_QUICK_JOIN_WRONG_INFO_2ND_BEST, ProtocolID, KILL_REASON_HACK_PACKET );
				break;
			}

			PACKET_QUICKJOIN_ENTER_REC_ROOM_ACK PacketAck;

			INT32 i32QuickJoinRv =  _OnJoinRoomByQuickJoin( PacketReq.m_ui32ChannelIdx, PacketReq.m_ui32RoomIdx ) ;
				
			if( EV_SUCCESSED( i32QuickJoinRv ) ) 
			{	// 성공일 경우 방 슬롯 인덱스가 리턴되므로 변환해줍니다.
				i32QuickJoinRv = EVENT_ERROR_SUCCESS;
			}
				
			PacketAck.m_i32Rv = i32QuickJoinRv;

			PacketAck.m_ui32ChannelIdx	= PacketReq.m_ui32ChannelIdx;
			PacketAck.m_ui32RoomIdx		= PacketReq.m_ui32RoomIdx;

			bSendMessage = TRUE;
			pSendPacket->SetProtocol( PROTOCOL_QUICKJOIN_ENTER_REC_ROOM_ACK );			
			pSendPacket->WriteData( &PacketAck, sizeof(PACKET_QUICKJOIN_ENTER_REC_ROOM_ACK) );		

		}
		break;
	}

	return bSendMessage;
}