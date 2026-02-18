#include "pch.h"
#include "UserSession.h"
#include "RoomManager.h"
#include "Room.h"
#include "ModuleTrans.h"

BOOL	CUserSession::_PacketParsingGMChat(i3NetworkPacket* pRecvPacket, i3NetworkPacket* pSendPacket, BOOL& bIsKilled)
{
	BOOL		bSendMessage = FALSE;
	PROTOCOL	ProtocolID = pRecvPacket->GetProtocolID(); 
	
	switch( ProtocolID )
	{
	case PROTOCOL_GMCHAT_START_CHAT_REQ:
		{
			PACKET_GMCHAT_START_CHAT_REQ Recv;
			S2MORecvPacket( &Recv, pRecvPacket->GetPacketBuffer() );

			PACKET_GMCHAT_START_CHAT_ACK Send;
		
			if( false == CheckAuthLevel() )
			{	// GM이나 MOD가 아니면 게임 종료시킨다.
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID );
				break;
			}

			CUserSession* pSession = NULL;

			switch(Recv.m_ui8idxType)
			{
			case GMCHAT_INDEX_LOBBY:	//상대가 로비에 있을때
				pSession = g_pUserManager->GetSession( Recv.m_i32idx );
				break;
			case GMCHAT_INDEX_ROOM:		//상대가 방에 있을때 ( GM위치 - 로비 or 방 )
				{
					//룸인덱스 범위를 벗어나면 탐색을 중단한다.
					if( Recv.m_i32idx < 1 ) break;

					//Client에서 받은 방번호로 방을 찾는다(RoomIdx는 0부터 시작하므로 -1 한 후 찾는다.)
					CRoom* pRoom = g_pRoomManager->GetRoom( m_i32ChannelNum, Recv.m_i32idx - 1 );

					//오픈된 방이 아닐 경우 탐색을 중단한다.
					if( FALSE == pRoom->m_bIsCreate ) break;

					for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; ++i )
					{
						CUserSession* pRoomUser = pRoom->GetSession(i);

						if( NULL == pRoomUser )
							continue;

						if( 0 == i3String::Compare( pRoomUser->m_UserInfoBasic.m_strNick, &Recv.m_strNickname ))
						{
							pSession = pRoomUser;
							break;
						}
						 
					}
				}
				break;
			case GMCHAT_INDEX_ROOMSLOT:	//상대와 GM이 같은방에 있을때
				{					
					CHECKVALID_ROOM;

					if( NULL == m_pRoom ) break;

					pSession = m_pRoom->GetSession( Recv.m_i32idx );
				}
				break;
			default:
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID, KILL_REASON_UNKNOWN_PACKET );
				break;
			}

			//해당 유저의 세션을 찾지 못한경우
			if( NULL == pSession )
			{				
				Send.m_TResult = EVENT_ERROR_GMCHAT_USER_NOT_INSERVER;
				S2MOSendEvent( &Send );
				break;
			}
			else if ( 0 != pSession->m_i64GMUID && m_i64UID != pSession->m_i64GMUID )
			{
				Send.m_TResult = EVENT_ERROR_GMCHAT_OTHER_GM_CONTACT;
				S2MOSendEvent( &Send );
				break;
			}

			//유저 UID를 GM에게 전달
			Send.m_TResult			= EVENT_ERROR_SUCCESS;
			Send.m_i64UserUID		= pSession->m_i64UID;
			Send.m_strNickname 		= Recv.m_strNickname;
			pSession->m_i64GMUID	= this->m_i64UID;

			S2MOSendEvent( &Send );
		}
		break;

	case PROTOCOL_GMCHAT_SEND_CHAT_REQ:
		{
			PACKET_GMCHAT_SEND_CHAT_REQ Recv;
			S2MORecvPacket( &Recv, pRecvPacket->GetPacketBuffer() );

			PACKET_GMCHAT_SEND_CHAT_ACK Send;
			
			CUserSession* pSession = NULL;

			if( 0 != m_i64GMUID )			//유저->GM으로 메세지를 보내는 경우
			{
				pSession = g_pUserManager->GetSession( m_i64GMUID );
			}
			else if( 0 != Recv.m_i64UID )	//GM->유저로 메세지를 보내는 경우
			{
				if( false == CheckAuthLevel() )
				{	// GM이나 MOD가 아니면 게임 종료시킨다.
					_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID );
					break;
				}

				pSession = g_pUserManager->GetSession( Recv.m_i64UID );
			}
			

			if( NULL == pSession )
			{
				Send.m_TResult = EVENT_ERROR_GMCHAT_RECIVER_NOTFOUND;
				S2MOSendEvent( &Send );
				break;
			}

			Send.m_TResult			= EVENT_ERROR_SUCCESS;
			Send.m_strNickname		= Recv.m_strNickname;
			Send.m_strChatMessage	= Recv.m_strChatMessage;

			pSession->S2MOSendEvent( &Send );
		}
		break;

	case PROTOCOL_GMCHAT_END_CHAT_REQ:
		{
			PACKET_GMCHAT_END_CHAT_REQ Recv;
			S2MORecvPacket( &Recv, pRecvPacket->GetPacketBuffer() );

			PACKET_GMCHAT_END_CHAT_ACK Send;			

			if( false == CheckAuthLevel() )
			{	// GM이나 MOD가 아니면 게임 종료시킨다.
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID );
				break;
			}

			CUserSession* pSession = g_pUserManager->GetSession( Recv.m_i64UserUID );

			//해당 유저의 세션을 찾지 못한경우
			if( NULL == pSession )
			{				
				Send.m_TResult = EVENT_ERROR_GMCHAT_USER_NOT_INSERVER;
				S2MOSendEvent( &Send );
				break;
			}

			pSession->m_i64GMUID	= 0;

			Send.m_TResult			= EVENT_ERROR_SUCCESS;
			pSession->S2MOSendEvent( &Send );
		}
		break;

	case PROTOCOL_GMCHAT_APPLY_PENALTY_REQ:
		{
			PACKET_GMCHAT_APPLY_PENALTY_REQ Recv;
			S2MORecvPacket( &Recv, pRecvPacket->GetPacketBuffer() );

			if( false == CheckAuthLevel() )
			{	// GM이나 MOD가 아니면 게임 종료시킨다.
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID );
				break;
			}

			if( FALSE == g_pModuleTrans->AuthSendApplyPenalty( m_WorkThreadIdx, this, &Recv ))
			{
				PACKET_GMCHAT_APPLY_PENALTY_ACK Send;
				Send.m_TResult = EVENT_ERROR_FAIL;
				S2MOSendEvent( &Send );
			}

		}
		break;
		
	default:
		{
		char strTemp[256];
		sprintf( strTemp, "UnKnown Packet - %d", ProtocolID );
		I3TRACE( strTemp );
		I3TRACE("UnKnown Packet - Kill User"); 
		bIsKilled = TRUE; 
		m_ui32KillReason = KILL_REASON_UNKNOWN_PACKET;
		}
		break;
	}

	return bSendMessage;
}

void CUserSession::SendGMChatUserNoti( UINT8 ui8PenaltyType, INT32	i32PenaltyAmount )
{
	PACKET_GMCHAT_NOTI_USER_PENALTY_ACK SendNoti;
	
	SendNoti.m_ui8PenaltyType	= ui8PenaltyType;	
	SendNoti.m_i32PenaltyAmount = i32PenaltyAmount;
				
	S2MOSendEvent( &SendNoti );
}