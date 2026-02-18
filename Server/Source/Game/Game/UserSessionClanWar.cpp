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

BOOL	CUserSession::_PacketParsingClanWar( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled )
{
	if( !g_pContextMain->IsContentsEnabled(CONTENTS_CLAN_MATCH_NEW) )	return FALSE;

	BOOL		bSendMessage = FALSE;
	PROTOCOL	ProtocolID = pPacket->GetProtocolID(); 
	
	switch( ProtocolID )
	{
	case PROTOCOL_CLAN_WAR_CHECK_PRESEASON_RANKING_REQ :			
		{
			CHECKVALID_LOBBY;

			m_ui32ClanRankingDate = i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();

			ChangeDataBaseInfo(SAVE_DATABASE_CLAN);
		}
		break;

	case PROTOCOL_CLAN_WAR_MATCH_TEAM_LIST_REQ:						
		{
            CHECKVALID_LOBBY;
			
			PACKET_CLAN_WAR_MATCH_TEAM_LIST_ACK Packet;
			
			if(m_UserInfoBasic.m_ui32Clanidx	 == 0)
			{
				Packet.m_TResult = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
				S2MOSendEvent( &Packet );
				break;
			}
				 
			if( FALSE == g_pModuleClan->ClanSendMatchTeamListRequest( m_WorkThreadIdx, this ) )
			{
				Packet.m_TResult		= EVENT_ERROR_NETWORK;
				S2MOSendEvent( &Packet );
			}
		}
		break;

	case PROTOCOL_CLAN_WAR_CREATE_TEAM_REQ:								
		{
			CHECKVALID_LOBBY;
			CHECK_HAVE_NO_MATCH_TEAM;
			CHECK_NOT_MERCENARY;
			
			PACKET_CLAN_WAR_CREATE_TEAM_REQ Recv;
			S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

			PACKET_CLAN_WAR_CREATE_TEAM_ACK Packet;

			if(  m_UserInfoBasic.m_ui32Clanidx <= 0 )
			{
				Packet.m_TResult = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
				S2MOSendEvent( &Packet );
				break;
			}

			UINT8 ui8MaxPer = Recv.m_ui8MaxPer;
			if( ui8MaxPer < MIN_CLAN_MATCH_MEMBER || MAX_CLAN_MATCH_MEMBER < ui8MaxPer ) 
			{
				Packet.m_TResult = EVENT_ERROR_USER_HACK_PACKET;
				S2MOSendEvent( &Packet );
				break;
			}
				 
			if( FALSE == g_pModuleClan->ClanSendMatchTeamCreateRequest( m_WorkThreadIdx, this, ui8MaxPer ) )
			{
				Packet.m_TResult		= EVENT_ERROR_NETWORK;
				S2MOSendEvent( &Packet );
			}
		}
		break;
	case PROTOCOL_CLAN_WAR_JOIN_TEAM_REQ : 
		{
			CHECKVALID_LOBBY;
			CHECK_HAVE_NO_MATCH_TEAM;
			
			PACKET_CLAN_WAR_JOIN_TEAM_REQ Recv;
			S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

			if( FALSE == g_pModuleClan->SendClanMatchJoinTeamRequest( m_WorkThreadIdx, this, Recv.m_TTeamID ) )
			{
				PACKET_CLAN_WAR_JOIN_TEAM_ACK Packet;
				Packet.m_TResult		= EVENT_ERROR_NETWORK;
				S2MOSendEvent( &Packet );
			}
		}
		break;
	case PROTOCOL_CLAN_WAR_LEAVE_TEAM_REQ : 
		{
			CHECKVALID_LOBBY;
			CHECK_HAVE_MATCH_TEAM;
			
			PACKET_CLAN_WAR_LEAVE_TEAM_REQ Recv;
			S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

			if( FALSE == g_pModuleClan->SendClanMatchLeaveTeamRequest( m_WorkThreadIdx, this  ) )
			{
				PACKET_CLAN_WAR_LEAVE_TEAM_ACK Packet;
				Packet.m_TResult		= EVENT_ERROR_NETWORK;
				S2MOSendEvent( &Packet );
			}
		}
		break;
	case PROTOCOL_CLAN_WAR_CHANGE_OPERATION_REQ : 
		{
			CHECKVALID_LOBBY;
			CHECK_HAVE_MATCH_TEAM;
			
			PACKET_CLAN_WAR_CHANGE_OPERATION_REQ Recv;
			S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

			if( FALSE == g_pModuleClan->SendClanMatchChangeTeamOperation( m_WorkThreadIdx, this, &Recv.m_strOperation ) )
			{
				PACKET_CLAN_WAR_CHANGE_OPERATION_ACK Packet;
				Packet.m_TResult		= EVENT_ERROR_NETWORK;
				S2MOSendEvent( &Packet );
			}

		}
		break;

	case PROTOCOL_CLAN_WAR_CHANGE_MAX_PER_REQ : 
		{
			CHECKVALID_LOBBY;
			CHECK_HAVE_MATCH_TEAM;
			
			PACKET_CLAN_WAR_CHANGE_MAX_PER_REQ Recv;
			S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

			PACKET_CLAN_WAR_CHANGE_MAX_PER_ACK Packet;  
			 
			UINT8 ui8MaxPer = Recv.m_ui8MaxMemberCount; 
			if( ui8MaxPer < MIN_CLAN_MATCH_MEMBER || MAX_CLAN_MATCH_MEMBER < ui8MaxPer ) 
			{
				Packet.m_TResult = EVENT_ERROR_USER_HACK_PACKET;
				S2MOSendEvent( &Packet );
				break;
			}

			if( FALSE == g_pModuleClan->SendClanMatchChangeMaxPerCount( m_WorkThreadIdx, this, ui8MaxPer ) )
			{
				Packet.m_TResult		= EVENT_ERROR_NETWORK;
				S2MOSendEvent( &Packet );
			}
		}
		break;

	
	case PROTOCOL_CLAN_WAR_TEAM_CHATTING_REQ:
		{
			CHECKVALID_CHANNEL;
			CHECK_HAVE_MATCH_TEAM;

			UINT16	ui16Type;
			UINT16	ui16ChatSize;
			TTCHAR	strChating[NET_CLAN_CHATING_SIZE];

			pPacket->ReadData(&ui16Type,		sizeof(UINT16));
			pPacket->ReadData(&ui16ChatSize,	sizeof(UINT16));
			if( ui16ChatSize > NET_CLAN_CHATING_SIZE ) break;
			pPacket->ReadData(strChating,		sizeof(TTCHAR) * ui16ChatSize);
			strChating[ NET_CLAN_CHATING_SIZE - 1 ] = '\0';

			if ( !_CheckChat(strChating, ZLOG_CHATTING_TYPE_CLAN, (INT16)ui16ChatSize, PROTOCOL_CLAN_WAR_TEAM_CHATTING_ACK))
			{
				break;
			}

			g_pModuleClan->ClanSendTeamChattingRequest( m_WorkThreadIdx, this, (UINT8)ui16ChatSize, strChating );
		}
		break;

	case PROTOCOL_CLAN_WAR_RESULT_REQ:
	{
		CHECKVALID_CHANNEL;

		PACKET_CLAN_WAR_RESULT_ACK Packet;

		if (FALSE == g_pModuleClan->SendClanWarResult(m_WorkThreadIdx, this))
		{
			Packet.m_TResult = EVENT_ERROR_NETWORK;
			S2MOSendEvent(&Packet);
		}
	}
	break;


	case PROTOCOL_CLAN_WAR_MATCHMAKING_REQ : 
		{
			CHECKVALID_LOBBY;
			CHECK_HAVE_MATCH_TEAM;
			CHECK_NOT_MERCENARY;
			
			PACKET_CLAN_WAR_MATCHMAKING_REQ Recv;
			S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

			if( FALSE == g_pModuleClan->SendClanMatchMaking( m_WorkThreadIdx, this  ) )
			{
				PACKET_CLAN_WAR_MATCHMAKING_ACK Packet;
				Packet.m_TResult		= EVENT_ERROR_NETWORK;
				S2MOSendEvent( &Packet );
			}
		}
		break;
	
	case PROTOCOL_CLAN_WAR_CANCEL_MATCHMAKING_REQ:
		{
			CHECKVALID_LOBBY;
			CHECK_HAVE_MATCH_TEAM;
			CHECK_NOT_MERCENARY;

			PACKET_CLAN_WAR_CANCEL_MATCHMAKING_REQ Recv;
			S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );
			
			if( FALSE == g_pModuleClan->SendCancelClanMatchMaking( m_WorkThreadIdx, this  ) )
			{
				PACKET_CLAN_WAR_CANCEL_MATCHMAKING_ACK Packet;
				Packet.m_TResult		= EVENT_ERROR_NETWORK;
				S2MOSendEvent( &Packet );
			}
		}
		break;

	case PROTOCOL_CLAN_WAR_MERCENARY_LIST_REQ:
		{
			CHECKVALID_CHANNEL;
			//CHECK_NOT_MERCENARY;

			PACKET_CLAN_WAR_MERCENARY_LIST_ACK	Packet;
			
			if( FALSE == g_pModuleClan->SendClanMatchMercenaryListReq( m_WorkThreadIdx, this ) )
			{
				Packet.m_TResult		= EVENT_ERROR_NETWORK;
				S2MOSendEvent( &Packet );
			}
		}
		break;

	case PROTOCOL_CLAN_WAR_REGIST_MERCENARY_REQ:
		{
			CHECKVALID_CHANNEL;
			CHECK_NOT_MERCENARY;

			PACKET_CLAN_WAR_REGIST_MERCENARY_ACK Packet; 

			// GM 계정은 용병 등록 불가.
			if (CheckAuthLevel())
			{
				Packet.m_TResult = EVENT_ERROR_FAIL_REGIST_MERCENARY;
				S2MOSendEvent(&Packet);
				break;
			}

			T_RESULT TResult = CheckMercenaryRegist();
			if( EV_FAILED( TResult ) )
			{
				Packet.m_TResult = TResult;
				S2MOSendEvent( &Packet );
				break;
			}

			if( FALSE == g_pModuleClan->SendClanMatchRegistMercenary( m_WorkThreadIdx, this  ) )
			{
				Packet.m_TResult		= EVENT_ERROR_NETWORK;
				S2MOSendEvent( &Packet );
				break;
			}
		}
		break;

	case PROTOCOL_CLAN_WAR_REMOVE_MERCENARY_REQ:
		{
			CHECKVALID_CHANNEL;
			CHECK_MERCENARY;

			PACKET_CLAN_WAR_REMOVE_MERCENARY_ACK Packet; 

			if( FALSE == g_pModuleClan->SendClanMatchRemoveMercenary( m_WorkThreadIdx, this ) )
			{
				Packet.m_TResult		= EVENT_ERROR_NETWORK;
				S2MOSendEvent( &Packet );
				break;
			}
		}
		break;

	case PROTOCOL_CLAN_WAR_INVITE_MERCENARY_REQ:
		{
			CHECKVALID_CHANNEL;
			CHECK_HAVE_MATCH_TEAM;
			CHECK_NOT_MERCENARY;	

			PACKET_CLAN_WAR_INVITE_MERCENARY_REQ Recv;
			S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

			PACKET_CLAN_WAR_INVITE_MERCENARY_SENDER_ACK Packet;

			if(  m_UserInfoBasic.m_ui32Clanidx <= 0 )
			{	// 클랜원(리더)가 아니면 보낼수 없습니다.
				Packet.m_TResult = EVENT_ERROR_CLAN_WAR_NOT_LEADER;
				S2MOSendEvent( &Packet );
				break;
			}

			if(  FALSE == g_pModuleClan->SendClanMatchInviteMercenaryReq(m_WorkThreadIdx, this, Recv.m_TMerID ) )
			{
				Packet.m_TResult		= EVENT_ERROR_NETWORK;
				S2MOSendEvent( &Packet );
			}
		}
		break;

	case PROTOCOL_CLAN_WAR_INVITE_ACCEPT_REQ:
		{
			CHECKVALID_CHANNEL;
			CHECK_HAVE_NO_MATCH_TEAM;
			CHECK_MERCENARY;

			PACKET_CLAN_WAR_INVITE_ACCEPT_ACK Packet;

			if(  FALSE == g_pModuleClan->SendClanMatchInviteAcceptReq(m_WorkThreadIdx, this ) )
			{
				Packet.m_TResult		= EVENT_ERROR_NETWORK;
				S2MOSendEvent( &Packet );
			}
		}
		break;

	case PROTOCOL_CLAN_WAR_INVITE_DENIAL_REQ:
		{
			CHECKVALID_CHANNEL;
			CHECK_HAVE_NO_MATCH_TEAM;
			CHECK_MERCENARY;

			PACKET_CLAN_WAR_INVITE_DENIAL_ACK Packet;

			if(  FALSE == g_pModuleClan->SendClanMatchInviteDenialReq(m_WorkThreadIdx, this ) )
			{
				Packet.m_TResult		= EVENT_ERROR_NETWORK;
				S2MOSendEvent( &Packet );
			}
		}
		break;


	case PROTOCOL_CLAN_WAR_MERCENARY_DETAIL_INFO_REQ:
		{
			CHECKVALID_CHANNEL;
			
			PACKET_CLAN_WAR_MERCENARY_DETAIL_INFO_ACK Packet; 

			PACKET_CLAN_WAR_MERCENARY_DETAIL_INFO_REQ Recv;
			S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

			if( FALSE == g_pModuleClan->SendClanMatcMercenaryDetailInfo( m_WorkThreadIdx, this, Recv.m_TMerID  ) )
			{
				Packet.m_TResult		= EVENT_ERROR_NETWORK;
				S2MOSendEvent( &Packet );
			}
		}
		break;

	default:
		{
			// 계속 작업중이라 일단 로그만 남깁니다. 추후 Kick 처리함. 2015.01.26 노준영
			g_pLog->WriteLogSession( this, L"UnKnown Packet ClanWar/ ProtocolID : %d / UID : %I64d", ProtocolID, m_i64UID );
			//bisKilled = TRUE; 
			//m_ui32KillReason = KILL_REASON_UNKNOWN_PACKET;
		}
		break;
	}

	return bSendMessage;
}


