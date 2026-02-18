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
	return FALSE;

	//INT32		Temp;
	//PROTOCOL	ProtocolID = pPacket->GetProtocolID();	
	//BOOL		bSendMessage = FALSE; 

	//switch( ProtocolID )
	//{
	//case PROTOCOL_CS_MATCH_TEAM_CONTEXT_REQ:											//20090103
	//	{
	//		CHECKVALID_LOBBY;
	//		g_pModuleClan->ClanSendMatchTeamContextRequest( m_WorkThreadIdx, this );
	//	}
	//	break;
	//case PROTOCOL_CS_MATCH_ALL_TEAM_CONTEXT_REQ:										//20090103
	//	{
	//		CHECKVALID_LOBBY;
	//		g_pModuleClan->ClanSendMatchAllTeamContextRequest( m_WorkThreadIdx, this, (UINT8)m_i32ChannelNum );
	//	}
	//	break;
	//case PROTOCOL_CS_MATCH_ALL_TEAM_LIST_REQ:												//20090103
	//	{
 //           CHECKVALID_LOBBY;
 //			Temp = EVENT_ERROR_SUCCESS;
	//		if(m_UserInfoBasic.m_ui32Clanidx	 == 0)Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

	//		if( Temp == EVENT_ERROR_SUCCESS )
	//		{
	//			UINT16 sSection;

	//			pPacket->ReadData(&sSection,  sizeof(UINT16));				
	//			g_pModuleClan->ClanSendMatchAllTeamListRequest( m_WorkThreadIdx, this, (UINT8)m_i32ChannelNum, sSection );
	//		}
	//		else
	//		{
	//			bSendMessage = TRUE;
	//			pSendPacket->SetProtocol( PROTOCOL_CS_MATCH_ALL_TEAM_LIST_ACK );
	//			pSendPacket->WriteData( &Temp, sizeof(UINT32) );

	//			I3TRACE( "PROTOCOL_CS_MATCH_ALL_TEAM_LIST_REQ Error");
	//		}
	//	}
	//	break;
	//case PROTOCOL_CS_MATCH_TEAM_DETAIL_INFO_REQ:												//20090103
	//	{
	//		CHECKVALID_LOBBY;
 //			Temp = EVENT_ERROR_SUCCESS;
	//		if(m_UserInfoBasic.m_ui32Clanidx	== 0)Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

	//		if( Temp == EVENT_ERROR_SUCCESS )
	//		{
	//			UINT32 wTeamIdx;
	//			pPacket->ReadData(&wTeamIdx,	sizeof(UINT32));
	//			g_pModuleClan->ClanSendMatchTeamDetailRequest( m_WorkThreadIdx, this, wTeamIdx );
	//		}
	//		else
	//		{
	//			bSendMessage = TRUE;
	//			pSendPacket->SetProtocol( PROTOCOL_CS_MATCH_TEAM_DETAIL_INFO_ACK );
	//			pSendPacket->WriteData( &Temp, sizeof(UINT32) );

	//			I3TRACE( "PROTOCOL_CS_MATCH_TEAM_DETAIL_INFO_REQ Error");
	//		}
	//	}
	//	break;
	//case PROTOCOL_CS_MATCH_TEAM_JOIN_REQ:												//20090103
	//	{
 //           CHECKVALID_LOBBY;
 //			
	//		if( m_UserInfoBasic.m_ui32Clanidx == 0 )
	//		{
	//			bSendMessage = TRUE;
	//			pSendPacket->SetProtocol( PROTOCOL_CS_MATCH_TEAM_JOIN_ACK );

	//			EVENT_ERROR eErrorCode = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
	//			pSendPacket->WriteData(&eErrorCode, sizeof(eErrorCode));

	//			I3TRACE( "PROTOCOL_CS_MATCH_TEAM_JOIN_REQ Error");
	//		}

	//		UINT32  ui32TeamIdx;
	//		UINT8	ui8Type;
	//		pPacket->ReadData(&ui32TeamIdx,	sizeof(UINT32));
	//		pPacket->ReadData(&ui8Type,		sizeof(UINT8));
	//		g_pModuleClan->ClanSendMatchTeamJoinRequest( m_WorkThreadIdx, this, ui32TeamIdx, ui8Type );
	//	}
	//	break;

	//case PROTOCOL_CS_MATCH_TEAM_LEAVE_REQ:												//20090103
	//	{
 //           CHECKVALID_LOBBY;
 //			Temp = EVENT_ERROR_SUCCESS;
	//		if(m_UserInfoBasic.m_ui32Clanidx == 0)Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

	//		if( Temp == EVENT_ERROR_SUCCESS )
	//		{
	//			g_pModuleClan->ClanSendMatchTeamLeaveRequest( m_WorkThreadIdx, this );
	//		}
	//		else
	//		{
	//			bSendMessage = TRUE;
	//			pSendPacket->SetProtocol( PROTOCOL_CS_MATCH_TEAM_LEAVE_ACK );
	//			pSendPacket->WriteData( &Temp, sizeof(UINT32) );

	//			I3TRACE( "PROTOCOL_CS_MATCH_TEAM_LEAVE_REQ Error");
	//		}
	//	}
	//	break;
	//case PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_REQ:											//20090103
	//	{
	//		CHECKVALID_LOBBY;
 //			Temp = EVENT_ERROR_SUCCESS;
	//		if(m_UserInfoBasic.m_ui32Clanidx == 0)Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

	//		if( Temp == EVENT_ERROR_SUCCESS )
	//		{
	//			UINT8	ui8PerMax;
	//			pPacket->ReadData(&ui8PerMax,		sizeof(UINT8));
	//			g_pModuleClan->ClanSendMatchTeamChangePerRequest( m_WorkThreadIdx, this, ui8PerMax );
	//		}
	//		else
	//		{				
	//			bSendMessage = TRUE;
	//			pSendPacket->SetProtocol( PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_ACK );
	//			pSendPacket->WriteData( &Temp, sizeof(UINT32) );

	//			I3TRACE( "PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_REQ Error");
	//		}
	//	}
	//	break;
	////case PROTOCOL_CS_MATCH_FIGHT_QUICK_REQUEST_REQ:										//20090103
	////	{
	////		CHECKVALID_LOBBY;
 ////			Temp = EVENT_ERROR_SUCCESS;
	////		if(m_UserInfoBasic.m_ui32Clanidx == 0)Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

	////		if( Temp == EVENT_ERROR_SUCCESS )
	////		{
	////			UINT8	ui8Count;
	////			pPacket->ReadData(&ui8Count,		sizeof(UINT8));
	////			g_pModuleClan->ClanSendMatchFightQuickRequestRequest( m_WorkThreadIdx, this, ui8Count );
	////		}
	////		else
	////		{				
	////			bSendMessage = TRUE;
	////			pSendPacket->SetProtocol( PROTOCOL_CS_MATCH_FIGHT_QUICK_REQUEST_ACK );
	////			pSendPacket->WriteData( &Temp, sizeof(UINT32) );

	////			I3TRACE( "PROTOCOL_CS_MATCH_FIGHT_QUICK_REQUEST_REQ Error");
	////		}
	////	}
	////	break;
	//case PROTOCOL_CS_MATCH_FIGHT_REQUEST_REQ:										//20090103
	//	{
 //           CHECKVALID_LOBBY;
 //			Temp = EVENT_ERROR_SUCCESS;
	//		if(m_UserInfoBasic.m_ui32Clanidx == 0)Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

	//		if( Temp == EVENT_ERROR_SUCCESS )
	//		{
	//			UINT32 wRequsetTeamIdx;
	//			pPacket->ReadData(&wRequsetTeamIdx, sizeof(UINT32));
	//			g_pModuleClan->ClanSendMatchFightRequestRequest( m_WorkThreadIdx, this, wRequsetTeamIdx );
	//		}
	//		else
	//		{
	//			bSendMessage = TRUE;
	//			pSendPacket->SetProtocol( PROTOCOL_CS_MATCH_FIGHT_REQUEST_ACK );
	//			pSendPacket->WriteData( &Temp, sizeof(UINT32) );

	//			I3TRACE( "PROTOCOL_CS_MATCH_FIGHT_REQUEST_REQ Error");
	//		}
	//	}
	//	break;
	//case PROTOCOL_CS_MATCH_FIGHT_ACCECT_REQ:										//20090103
	//	{
	//		CHECKVALID_LOBBY;
	//		Temp = EVENT_ERROR_SUCCESS;
	//		if(m_UserInfoBasic.m_ui32Clanidx == 0)Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

	//		if( Temp == EVENT_ERROR_SUCCESS )
	//		{
	//			UINT32 ui32SpIdx;
	//			UINT32 ui32RequsetTeamIdx;
	//			UINT8  ui8Accept;

	//			pPacket->ReadData(&ui32SpIdx,			sizeof(UINT32));
	//			pPacket->ReadData(&ui32RequsetTeamIdx,	sizeof(UINT32));
	//			pPacket->ReadData(&ui8Accept,			sizeof(UINT8));

	//			g_pModuleClan->ClanSendMatchFightAccectRequest( m_WorkThreadIdx, this, ui32SpIdx, ui32RequsetTeamIdx, ui8Accept );
	//		}
	//		else
	//		{
	//			bSendMessage = TRUE;
	//			pSendPacket->SetProtocol( PROTOCOL_CS_MATCH_FIGHT_ACCECT_ACK );
	//			pSendPacket->WriteData( &Temp, sizeof(UINT32) );

	//			I3TRACE( "PROTOCOL_CS_MATCH_FIGHT_ACCECT_REQ Error");
	//		}
	//	}
	//	break;
	//case PROTOCOL_CS_MATCH_ROOM_CREATE_GS_ACK		:							//방 만들기 요청 	완료	확인 0315	0829
	//	{
	//		CHECKVALID_LOBBY; 

	//		if( m_i32RoomIdx > SERVER_VALUE_UNKNOWN )
	//		{
	//			_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_POSITION, ProtocolID );
	//			return FALSE; 
	//		}

	//		ROOM_INFO_CREATE InfoCreate;
	//		InfoCreate.Reset();
	//		
	//		pPacket->ReadData(InfoCreate.m_ui32ClanIdx,	sizeof(UINT32) * TEAM_COUNT );			//Room.cpp에서 내용 확인, 내용이 이상해도 내부에서 기본값으로 생성
	//		pPacket->ReadData(&InfoCreate.RoomInfoBasic,	sizeof(ROOM_INFO_BASIC));			//Room.cpp에서 내용 확인, 내용이 이상해도 내부에서 기본값으로 생성
	//		
	//		if( SERVER_VALUE_UNKNOWN < CreateRoom( m_WorkThreadIdx, &InfoCreate ) )
	//		{
	//			g_pModuleClan->ClanSendBattleRoomJoin( m_WorkThreadIdx, this, InfoCreate.m_ui32ClanIdx[0],InfoCreate.m_ui32ClanIdx[1],m_i32RoomIdx );
	//		}
	//	}
	//	break;
	//case PROTOCOL_CS_MATCH_ROOM_JOIN_GS_ACK:
	//	{
	//		CHECKVALID_LOBBY;

	//		

	//		if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
	//		{
	//			_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_POSITION, ProtocolID );
	//			return FALSE; 
	//		}

	//		UINT32			ui32RoomIdx; 
	//		ROOM_INFO_JOIN	InfoJoin;
	//		InfoJoin.bMatch		= TRUE;
	//		InfoJoin.cForce		= 1;
	//		
	//		pPacket->ReadData(&ui32RoomIdx,				sizeof(UINT32) );
	//		pPacket->ReadData(&InfoJoin.wMatchTeam,		sizeof(UINT32) );
	//		
	//		Temp = g_pRoomManager->OnJoinRoom( m_WorkThreadIdx, this, m_i32ChannelNum, ui32RoomIdx, &InfoJoin );

	//		if( EV_FAILED( Temp ) )
	//		{
	//			PACKET_ROOM_JOIN_ACK Packet;
	//			Packet.m_TResult = Temp;
	//			S2MOSendEvent( &Packet );
	//		}
	//	}
	//	break;
	//case PROTOCOL_CS_MATCH_CHATING_REQ:										//20090103
	//	{
	//		CHECKVALID_LOBBY;
	//		Temp = EVENT_ERROR_SUCCESS;
	//		if(m_UserInfoBasic.m_ui32Clanidx == 0)Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

	//		if( Temp == EVENT_ERROR_SUCCESS )
	//		{
	//			UINT16	ui16Type;
	//			UINT16	ui16ChatSize;
	//			TTCHAR	strChating[NET_CLAN_CHATING_SIZE];
	//			bool	bIsGMUser = CheckAuthLevel();
	//			
	//			pPacket->ReadData(&ui16Type,		sizeof(UINT16));
	//			pPacket->ReadData(&ui16ChatSize,	sizeof(UINT16));
	//			if( ui16ChatSize > NET_CLAN_CHATING_SIZE ) break;
	//			pPacket->ReadData(strChating,		sizeof(TTCHAR) * ui16ChatSize);
	//			strChating[ NET_CLAN_CHATING_SIZE - 1 ] = '\0';

	//			if ( !_CheckChat(strChating, ZLOG_CHATTING_TYPE_CLAN, (INT16)ui16ChatSize, PROTOCOL_CS_MATCH_CHATING_ACK) )
	//			{
	//				// 채팅제한 상태라면 뿌려줄 필요 없으니 여기서 빠져나갑니다.
	//				break;
	//			}

	//			g_pModuleClan->ClanSendMatchChattingRequest( m_WorkThreadIdx, this, (UINT8)ui16ChatSize, strChating, bIsGMUser );
	//		}
	//	}
	//	break;

	//case PROTOCOL_CS_MATCH_NEW_GET_CLAN_ROOM_INFO_REQ:
	//	{
	//		CHECKVALID_CHANNEL;
	//		//CHECKVALID_CLAN_PAGE;
	//		//m_bRequestClanRoomList = TRUE;

	//		g_pModuleClan->ClanSendMatchGetClanRoomInfo( m_WorkThreadIdx, this, (UINT8)m_i32ChannelNum );
	//	}
	//	break;

	////case PROTOCOL_CS_MATCH_NEW_FIGHT_QUICK_REQUEST_REQ:
	////	{
	////		g_pModuleClan->ClanSendMatchFightQuickRequestRequest(m_WorkThreadIdx, this, (UINT8)m_i32ChannelNum);
	////	}
	////	break;

	//default:
	//	{
	//		char strTemp[256];
	//		sprintf( strTemp, "UnKnown Packet - %d\n", ProtocolID );
	//		I3TRACE( strTemp );
	//		I3TRACE( "UnKnown Packet - Kill User"); 
	//		bisKilled = TRUE; 
	//		m_ui32KillReason = KILL_REASON_UNKNOWN_PACKET;
	//	}
	//	break;
	//}

	//return bSendMessage;
}
