#include "pch.h"
#include "UserSession.h"
#include "RoomManager.h"
#include "UdpHolePunching.h"
#include "DataBase.h"
#include "Usermanager.h"
#include "ServerContext.h"
#include "Room.h"
#include "ModuleClan.h"
#include "ModuleContextNc.h"
#include "ModuleMessenger.h"

BOOL	CUserSession::_PacketParsingClanBattle( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled )
{
	INT32		i32Temp;
	PROTOCOL	ProtocolID = pPacket->GetProtocolID();	
	BOOL		bSendMessage = FALSE; 

	switch( ProtocolID )
	{
	case PROTOCOL_CS_MATCH_TEAM_CONTEXT_REQ:											//20090103
		{
			CHECKVALID_CHANNEL;
			g_pModuleClan->ClanSendMatchTeamContextRequest( m_WorkThreadIdx, this );
		}
		break;
	case PROTOCOL_CS_MATCH_ALL_TEAM_CONTEXT_REQ:										//20090103
		{
			CHECKVALID_CHANNEL;
			g_pModuleClan->ClanSendMatchAllTeamContextRequest( m_WorkThreadIdx, this, (UINT8)m_i32ChannelNum );
		}
		break;
	case PROTOCOL_CS_MATCH_TEAM_LIST_REQ:												//20090103
		{
			CHECKVALID_CHANNEL;
			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic._clanidx	  == 0)									i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				UINT8 ui8Section;

				pPacket->ReadData(&ui8Section,  sizeof(UINT8));	

				g_pModuleClan->ClanSendMatchTeamListRequest( m_WorkThreadIdx, this, ui8Section );
			}
			else
			{	
				bSendMessage = TRUE;
				pSendPacket->SetProtocol( PROTOCOL_CS_MATCH_TEAM_LIST_ACK );
				pSendPacket->WriteData( &i32Temp, sizeof(UINT8) );

				I3TRACE( "PROTOCOL_CS_MATCH_TEAM_LIST_REQ Error");
			}
		}
		break;
	case PROTOCOL_CS_MATCH_ALL_TEAM_LIST_REQ:												//20090103
		{
			CHECKVALID_CHANNEL;
 			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic._clanidx	 == 0)i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				UINT16 ui16Section;

				pPacket->ReadData(&ui16Section,  sizeof(UINT16));				
				g_pModuleClan->ClanSendMatchAllTeamListRequest( m_WorkThreadIdx, this, (UINT8)m_i32ChannelNum, ui16Section );
			}
			else
			{
				bSendMessage = TRUE;
				pSendPacket->SetProtocol( PROTOCOL_CS_MATCH_ALL_TEAM_LIST_ACK );
				pSendPacket->WriteData( &i32Temp, sizeof(UINT32) );

				I3TRACE( "PROTOCOL_CS_MATCH_ALL_TEAM_LIST_REQ Error");
			}
		}
		break;
	case PROTOCOL_CS_MATCH_TEAM_DETAIL_INFO_REQ:												//20090103
		{
			CHECKVALID_CHANNEL;
 			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic._clanidx	== 0)i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				UINT32 ui32TeamIdx;
				pPacket->ReadData(&ui32TeamIdx,	sizeof(UINT32));
				g_pModuleClan->ClanSendMatchTeamDetailRequest( m_WorkThreadIdx, this, ui32TeamIdx );
			}
			else
			{
				bSendMessage = TRUE;
				pSendPacket->SetProtocol( PROTOCOL_CS_MATCH_TEAM_DETAIL_INFO_ACK );
				pSendPacket->WriteData( &i32Temp, sizeof(UINT32) );

				I3TRACE( "PROTOCOL_CS_MATCH_TEAM_DETAIL_INFO_REQ Error");
			}
		}
		break;
	case PROTOCOL_CS_MATCH_TEAM_CREATE_REQ:												//20090103
		{
			CHECKVALID_CHANNEL;
 			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic._clanidx	== 0)i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				INT8  i8PerCount;
				pPacket->ReadData(&i8PerCount,  sizeof(INT8));
				g_pModuleClan->ClanSendMatchTeamCreateRequest( m_WorkThreadIdx, this, (INT8)m_i32ChannelNum, i8PerCount );
			}
			else
			{
				bSendMessage = TRUE;
				pSendPacket->SetProtocol( PROTOCOL_CS_MATCH_TEAM_CREATE_ACK );
				pSendPacket->WriteData( &i32Temp, sizeof(UINT32) );

				I3TRACE( "PROTOCOL_CS_MATCH_TEAM_CREATE_REQ Error");
			}
		}
		break;
	case PROTOCOL_CS_MATCH_TEAM_JOIN_REQ:												//20090103
		{
			CHECKVALID_CHANNEL;
 			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic._clanidx	  == 0)i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				UINT32  ui32TeamIdx;
				UINT8	ui8Type;
				pPacket->ReadData(&ui32TeamIdx,	sizeof(UINT32));
				pPacket->ReadData(&ui8Type,		sizeof(UINT8));
				g_pModuleClan->ClanSendMatchTeamJoinRequest( m_WorkThreadIdx, this, ui32TeamIdx, ui8Type );
			}
			else
			{
				bSendMessage = TRUE;
				pSendPacket->SetProtocol( PROTOCOL_CS_MATCH_TEAM_JOIN_ACK );
				pSendPacket->WriteData( &i32Temp, sizeof(UINT32) );

				I3TRACE( "PROTOCOL_CS_MATCH_TEAM_JOIN_REQ Error");
			}
		}
		break;
	case PROTOCOL_CS_MATCH_TEAM_LEAVE_REQ:												//20090103
		{
			CHECKVALID_CHANNEL;
 			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic._clanidx == 0)i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				g_pModuleClan->ClanSendMatchTeamLeaveRequest( m_WorkThreadIdx, this );
			}
			else
			{
				bSendMessage = TRUE;
				pSendPacket->SetProtocol( PROTOCOL_CS_MATCH_TEAM_LEAVE_ACK );
				pSendPacket->WriteData( &i32Temp, sizeof(UINT32) );

				I3TRACE( "PROTOCOL_CS_MATCH_TEAM_LEAVE_REQ Error");
			}
		}
		break;
	case PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_REQ:											//20090103
		{
			CHECKVALID_CHANNEL;
 			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic._clanidx == 0)i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				UINT8	ui8PerMax;
				pPacket->ReadData(&ui8PerMax,		sizeof(UINT8));
				g_pModuleClan->ClanSendMatchTeamChangePerRequest( m_WorkThreadIdx, this, ui8PerMax );
			}
			else
			{				
				bSendMessage = TRUE;
				pSendPacket->SetProtocol( PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_ACK );
				pSendPacket->WriteData( &i32Temp, sizeof(UINT32) );

				I3TRACE( "PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_REQ Error");
			}
		}
		break;
	case PROTOCOL_CS_MATCH_FIGHT_QUICK_REQUEST_REQ:										//20090103
		{
			CHECKVALID_CHANNEL;
 			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic._clanidx == 0)i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				UINT8	ui8Count;
				pPacket->ReadData(&ui8Count,		sizeof(UINT8));
				g_pModuleClan->ClanSendMatchFightQuickRequestRequest( m_WorkThreadIdx, this, ui8Count );
			}
			else
			{				
				bSendMessage = TRUE;
				pSendPacket->SetProtocol( PROTOCOL_CS_MATCH_FIGHT_QUICK_REQUEST_ACK );
				pSendPacket->WriteData( &i32Temp, sizeof(UINT32) );

				I3TRACE( "PROTOCOL_CS_MATCH_FIGHT_QUICK_REQUEST_REQ Error");
			}
		}
		break;
	case PROTOCOL_CS_MATCH_FIGHT_REQUEST_REQ:										//20090103
		{
			CHECKVALID_CHANNEL;
 			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic._clanidx == 0)i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				UINT32 ui32RequsetTeamIdx;
				pPacket->ReadData(&ui32RequsetTeamIdx, sizeof(UINT32));
				g_pModuleClan->ClanSendMatchFightRequestRequest( m_WorkThreadIdx, this, ui32RequsetTeamIdx );
			}
			else
			{
				bSendMessage = TRUE;
				pSendPacket->SetProtocol( PROTOCOL_CS_MATCH_FIGHT_REQUEST_ACK );
				pSendPacket->WriteData( &i32Temp, sizeof(UINT32) );

				I3TRACE( "PROTOCOL_CS_MATCH_FIGHT_REQUEST_REQ Error");
			}
		}
		break;
	case PROTOCOL_CS_MATCH_FIGHT_ACCECT_REQ:										//20090103
		{
			CHECKVALID_CHANNEL;
			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic._clanidx == 0)i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				UINT32 ui32SpIdx;
				UINT32 ui32RequsetTeamIdx;
				UINT8  ui8Accept;

				pPacket->ReadData(&ui32SpIdx,			sizeof(UINT32));
				pPacket->ReadData(&ui32RequsetTeamIdx,	sizeof(UINT32));
				pPacket->ReadData(&ui8Accept,			sizeof(UINT8));

				g_pModuleClan->ClanSendMatchFightAccectRequest( m_WorkThreadIdx, this, ui32SpIdx, ui32RequsetTeamIdx, ui8Accept );
			}
			else
			{
				bSendMessage = TRUE;
				pSendPacket->SetProtocol( PROTOCOL_CS_MATCH_FIGHT_ACCECT_ACK );
				pSendPacket->WriteData( &i32Temp, sizeof(UINT32) );

				I3TRACE( "PROTOCOL_CS_MATCH_FIGHT_ACCECT_REQ Error");
			}
		}
		break;
	case PROTOCOL_CS_MATCH_ROOM_CREATE_GS_ACK		:							//방 만들기 요청 	완료	확인 0315	0829
		{
			CHECKVALID_CHANNEL; 

			if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_POSITION, ProtocolID );
				return FALSE; 
			}

			ROOM_INFO_CREATE InfoCreate;
			InfoCreate.Reset();
			
			pPacket->ReadData(&InfoCreate.wMatchTeamIdx,	sizeof(UINT32));			//Room.cpp에서 내용 확인, 내용이 이상해도 내부에서 기본값으로 생성
			pPacket->ReadData(&InfoCreate.wMatchReqTeamIdx,	sizeof(UINT32));			//Room.cpp에서 내용 확인, 내용이 이상해도 내부에서 기본값으로 생성
			pPacket->ReadData(&InfoCreate.RoomInfoBasic,	sizeof(ROOM_INFO_BASIC));	//Room.cpp에서 내용 확인, 내용이 이상해도 내부에서 기본값으로 생성
			
			if( SERVER_VALUE_UNKNOWN < _CreateRoom( &InfoCreate ) )
			{
				g_pModuleClan->ClanSendBattleRoomJoin( MODULE_CLAN_THREAD_IDX, this, InfoCreate.wMatchTeamIdx, InfoCreate.wMatchReqTeamIdx, m_i32RoomIdx );
			}
		}
		break;
	case PROTOCOL_CS_MATCH_ROOM_JOIN_GS_ACK:
		{
			CHECKVALID_CHANNEL; 

			if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_POSITION, ProtocolID );
				return FALSE; 
			}

			UINT32			ui32RoomIdx; 
			ROOM_INFO_JOIN	InfoJoin;
			InfoJoin.Reset();
			InfoJoin.bMatch		= TRUE;
			InfoJoin.cForce		= 1;
			
			pPacket->ReadData(&ui32RoomIdx,				sizeof(UINT32) );
			pPacket->ReadData(&InfoJoin.wMatchTeam,		sizeof(UINT32) );
			
			i32Temp = g_pRoomManager->OnJoinRoom( this, m_i32ChannelNum, ui32RoomIdx, &InfoJoin );

			if( EV_FAILED( i32Temp ) )
			{
				bSendMessage = TRUE;
				pSendPacket->SetProtocol(PROTOCOL_LOBBY_JOIN_ROOM_ACK);
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) );				//방에 들어가지 못하는 내용이 들어가 있음 
			}
		}
		break;
	case PROTOCOL_CS_MATCH_CHATING_REQ:										//20090103
		{
			CHECKVALID_CHANNEL;
			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic._clanidx == 0)i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				UINT16	ui16Type;
				UINT16	ui16ChatSize;
				char	strChating[NET_CLAN_CHATING_SIZE];
				
				pPacket->ReadData(&ui16Type,			sizeof(UINT16));
				pPacket->ReadData(&ui16ChatSize,		sizeof(UINT16));
				if( ui16ChatSize > NET_CLAN_CHATING_SIZE ) break;
				pPacket->ReadData(strChating,		ui16ChatSize);
				strChating[ NET_CLAN_CHATING_SIZE - 1 ] = '\0';

				if (_CheckDuplicateChat(strChating, ZLOG_CHATTING_TYPE_CLAN, PROTOCOL_CS_MATCH_CHATING_ACK) != CHAT_LIMIT_NORMAL)
				{
					// 채팅제한 상태라면 뿌려줄 필요 없으니 여기서 빠져나갑니다.
					break;
				}

				g_pModuleClan->ClanSendMatchChattingRequest( m_WorkThreadIdx, this, (UINT8)ui16ChatSize, strChating );
			}
		}
		break;
	default:
		{
			char strTemp[256];
			i3String::Format( strTemp, 256, "UnKnown Packet - %d", ProtocolID );
			I3TRACE( strTemp );
			I3TRACE( "UnKnown Packet - Kill User"); 
			bisKilled = TRUE; 
			m_ui32KillReason = KILL_REASON_UNKNOWN_PACKET;
		}
		break;
	}

	return bSendMessage;
}