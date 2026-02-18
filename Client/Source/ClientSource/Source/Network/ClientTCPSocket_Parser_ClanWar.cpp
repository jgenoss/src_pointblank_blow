#include "pch.h"
#include "ClientTCPSocket.h"

#include "ClanSeasonContext.h"
#include "ClanGameContext.h"

namespace
{
	const char * getProtocolName( PROTOCOL protocol)
	{
		switch( protocol)
		{
		case PROTOCOL_CLAN_WAR_PRESEASON_RANKING_ACK			:	return "PROTOCOL_CLAN_WAR_PRESEASON_RANKING_ACK";
		case PROTOCOL_CLAN_WAR_MATCH_TEAM_COUNT_ACK				:	return "PROTOCOL_CLAN_WAR_MATCH_TEAM_COUNT_ACK";
		case PROTOCOL_CLAN_WAR_MATCH_TEAM_LIST_ACK				:	return "PROTOCOL_CLAN_WAR_MATCH_TEAM_LIST_ACK";
		case PROTOCOL_CLAN_WAR_CREATE_TEAM_ACK					:	return "PROTOCOL_CLAN_WAR_CREATE_TEAM_ACK";
		case PROTOCOL_CLAN_WAR_JOIN_TEAM_ACK					:	return "PROTOCOL_CLAN_WAR_JOIN_TEAM_ACK";
		case PROTOCOL_CLAN_WAR_LEAVE_TEAM_ACK					:	return "PROTOCOL_CLAN_WAR_LEAVE_TEAM_ACK";
		case PROTOCOL_CLAN_WAR_CHANGE_OPERATION_ACK				:	return "PROTOCOL_CLAN_WAR_CHANGE_OPERATION_ACK";
		case PROTOCOL_CLAN_WAR_CHANGE_MAX_PER_ACK				:	return "PROTOCOL_CLAN_WAR_CHANGE_MAX_PER_ACK";
		case PROTOCOL_CLAN_WAR_MATCHMAKING_ACK					:	return "PROTOCOL_CLAN_WAR_MATCHMAKING_ACK";
		case PROTOCOL_CLAN_WAR_CANCEL_MATCHMAKING_ACK			:	return "PROTOCOL_CLAN_WAR_CANCEL_MATCHMAKING_ACK";
		case PROTOCOL_CLAN_WAR_TEAM_CHATTING_ACK				:	return "PROTOCOL_CLAN_WAR_TEAM_CHATTING_ACK";
		case PROTOCOL_CLAN_WAR_MATCH_TEAM_INFO_ACK				:	return "PROTOCOL_CLAN_WAR_MATCH_TEAM_INFO_ACK";
		case PROTOCOL_CLAN_WAR_MERCENARY_LIST_ACK				:	return "PROTOCOL_CLAN_WAR_MERCENARY_LIST_ACK";
		case PROTOCOL_CLAN_WAR_REGIST_MERCENARY_ACK				:	return "PROTOCOL_CLAN_WAR_REGIST_MERCENARY_ACK";
		case PROTOCOL_CLAN_WAR_REMOVE_MERCENARY_ACK				:	return "PROTOCOL_CLAN_WAR_REMOVE_MERCENARY_ACK";
		case PROTOCOL_CLAN_WAR_INVITE_MERCENARY_SENDER_ACK		:	return "PROTOCOL_CLAN_WAR_INVITE_MERCENARY_SENDER_ACK";
		case PROTOCOL_CLAN_WAR_INVITE_MERCENARY_RECEIVER_ACK	:	return "PROTOCOL_CLAN_WAR_INVITE_MERCENARY_RECEIVER_ACK";
		case PROTOCOL_CLAN_WAR_INVITE_ACCEPT_ACK				:	return "PROTOCOL_CLAN_WAR_INVITE_ACCEPT_ACK";
		case PROTOCOL_CLAN_WAR_INVITE_DENIAL_ACK				:	return "PROTOCOL_CLAN_WAR_INVITE_DENIAL_ACK";
		case PROTOCOL_CLAN_WAR_MERCENARY_DETAIL_INFO_ACK		:	return "PROTOCOL_CLAN_WAR_MERCENARY_DETAIL_INFO_ACK";
		case PROTOCOL_CLAN_WAR_FORCE_MATCH_ROOM_LEAVE_ACK		:	return "PROTOCOL_CLAN_WAR_FORCE_MATCH_ROOM_LEAVE_ACK";
		case PROTOCOL_CLAN_WAR_MATCH_MAKING_RESULT_ACK			:	return "PROTOCOL_CLAN_WAR_MATCH_MAKING_RESULT_ACK";
		case PROTOCOL_CLAN_WAR_RESULT_ACK						:	return "PROTOCOL_CLAN_WAR_RESULT_ACK";
		case PROTOCOL_CLAN_WAR_REPLACE_CLAN_NAME_ACK			:	return "PROTOCOL_CLAN_WAR_REPLACE_CLAN_NAME_ACK";
		case PROTOCOL_CLAN_WAR_REPLACE_CLAN_MARK_ACK			:	return "PROTOCOL_CLAN_WAR_REPLACE_CLAN_MARK_ACK";
		case PROTOCOL_CLAN_WAR_REPLACE_CLAN_COLOR_NAME_ACK		:	return "PROTOCOL_CLAN_WAR_REPLACE_CLAN_COLOR_NAME_ACK";
		default :
			I3PRINTLOG(I3LOG_FATAL,  "PROTOCOL_CLAN_WAR_XXX 프로토콜 이름 등록해주이소~");
			return "PROTOCOL_CLAN_WAR_UNKNOWN";
		}
	}
}

void ClientTCPSocket::_PacketParsingClanWar( i3NetworkPacket  * pPacket )
{
	i3::unordered_map< UINT32, netFunc >::iterator it;
	it = m_NetFuncMap_ClanWar.find( pPacket->GetProtocolID());

	if ( it != m_NetFuncMap_ClanWar.end() )
	{
		I3PRINTLOG(I3LOG_QA, "* %s -->", getProtocolName( pPacket->GetProtocolID()) );
		netFunc & mf = it->second;
		(this->*mf)(pPacket);
		I3PRINTLOG(I3LOG_QA, "* %s <--", getProtocolName( pPacket->GetProtocolID()) );
	}
	else
		__Parse_UnknownProtocol( pPacket);
}

void ClientTCPSocket::__Register_ClanWar_Function( void)
{
	RegisterParser( PROTOCOL_CLAN_WAR_PRESEASON_RANKING_ACK,			&ClientTCPSocket::__Parse_ClanWar_PreSeasonRanking);
	RegisterParser( PROTOCOL_CLAN_WAR_MATCH_TEAM_COUNT_ACK,				&ClientTCPSocket::__Parse_ClanWar_AllMatchingInfo);
	RegisterParser( PROTOCOL_CLAN_WAR_MATCH_TEAM_LIST_ACK,				&ClientTCPSocket::__Parse_ClanWar_MatchTeamList);
	RegisterParser( PROTOCOL_CLAN_WAR_CREATE_TEAM_ACK,					&ClientTCPSocket::__Parse_ClanWar_CreateTeam);
	RegisterParser( PROTOCOL_CLAN_WAR_JOIN_TEAM_ACK,					&ClientTCPSocket::__Parse_ClanWar_JoinTeam);
	RegisterParser( PROTOCOL_CLAN_WAR_LEAVE_TEAM_ACK,					&ClientTCPSocket::__Parse_ClanWar_LeaveTeam);
	RegisterParser( PROTOCOL_CLAN_WAR_CHANGE_OPERATION_ACK,				&ClientTCPSocket::__Parse_ClanWar_ChangeOperation);
	RegisterParser( PROTOCOL_CLAN_WAR_CHANGE_MAX_PER_ACK,				&ClientTCPSocket::__Parse_ClanWar_ChangeMaxPerson);
	RegisterParser( PROTOCOL_CLAN_WAR_MATCHMAKING_ACK,					&ClientTCPSocket::__Parse_ClanWar_RegisterMatchMaking );
	RegisterParser( PROTOCOL_CLAN_WAR_CANCEL_MATCHMAKING_ACK,			&ClientTCPSocket::__Parse_ClanWar_RegisterCancelMatchMaking);
	RegisterParser( PROTOCOL_CLAN_WAR_TEAM_CHATTING_ACK,				&ClientTCPSocket::__Parse_ClanWar_TeamChatting );
	RegisterParser( PROTOCOL_CLAN_WAR_MATCH_TEAM_INFO_ACK,				&ClientTCPSocket::__Parse_ClanWar_BattleTeamInfo);

	RegisterParser( PROTOCOL_CLAN_WAR_MERCENARY_LIST_ACK,				&ClientTCPSocket::__Parse_ClanWar_MercenaryList );
	RegisterParser( PROTOCOL_CLAN_WAR_MERCENARY_DETAIL_INFO_ACK,		&ClientTCPSocket::__Parse_ClanWar_MerceDetailInfo );
	RegisterParser( PROTOCOL_CLAN_WAR_REGIST_MERCENARY_ACK,				&ClientTCPSocket::__Parse_ClanWar_RegisterMercenary );
	RegisterParser( PROTOCOL_CLAN_WAR_REMOVE_MERCENARY_ACK,				&ClientTCPSocket::__Parse_ClanWar_RemoveMercenary );
	RegisterParser( PROTOCOL_CLAN_WAR_INVITE_MERCENARY_SENDER_ACK,		&ClientTCPSocket::__Parse_ClanWar_InviteMercenarySender );
	RegisterParser( PROTOCOL_CLAN_WAR_INVITE_MERCENARY_RECEIVER_ACK,	&ClientTCPSocket::__Parse_ClanWar_InviteMercenaryReceiver );
	RegisterParser( PROTOCOL_CLAN_WAR_INVITE_ACCEPT_ACK,				&ClientTCPSocket::__Parse_ClanWar_AcceptInvite );
	RegisterParser( PROTOCOL_CLAN_WAR_INVITE_DENIAL_ACK,				&ClientTCPSocket::__Parse_ClanWar_DenialInvite );
	RegisterParser( PROTOCOL_CLAN_WAR_FORCE_MATCH_ROOM_LEAVE_ACK,		&ClientTCPSocket::__Parse_ClanWar_ForceMatchroomLeave );
	RegisterParser( PROTOCOL_CLAN_WAR_MATCH_MAKING_RESULT_ACK,			&ClientTCPSocket::__Parse_ClanWar_MatchResult);
	RegisterParser(	PROTOCOL_CLAN_WAR_RESULT_ACK,						&ClientTCPSocket::__Parse_ClanWar_Result);
	RegisterParser( PROTOCOL_CLAN_WAR_REPLACE_CLAN_NAME_ACK,			&ClientTCPSocket::__Parse_ClanWar_Replace_Clan_Name);
	RegisterParser( PROTOCOL_CLAN_WAR_REPLACE_CLAN_MARK_ACK,			&ClientTCPSocket::__Parse_ClanWar_Replace_Clan_Mark);
	RegisterParser( PROTOCOL_CLAN_WAR_REPLACE_CLAN_COLOR_NAME_ACK,		&ClientTCPSocket::__Parse_ClanWar_Replace_Clan_Color_Name);
	
}

void ClientTCPSocket::__Parse_ClanWar_PreSeasonRanking( i3NetworkPacket * pPacket)
{
	PACKET_CLAN_WAR_PRESEASON_RANKING_INFO_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	i3::rc_wstring wstrSeasonName;
	wstrSeasonName = &Recv.m_strSeasonName;

	INT32 i32Count = Recv.m_i32SeasonRankingCount;
	CLAN_BASIC_INFO * pInfo = &Recv.m_aSeasonRankingClan;

	ClanSeasonContext::i()->SetPreSeasonInfo(wstrSeasonName, pInfo, i32Count);
}

void ClientTCPSocket::__Parse_ClanWar_AllMatchingInfo( i3NetworkPacket * pPacket)
{
	PACKET_CLAN_WAR_MATCH_TEAM_COUNT_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	ClanGameContext::i()->setClanAllMatingInfo(Recv);

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_WAR_ALL_MATCHING_INFO);
}

void ClientTCPSocket::__Parse_ClanWar_MatchTeamList( i3NetworkPacket * pPacket)
{
	PACKET_CLAN_WAR_MATCH_TEAM_LIST_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	if( EV_SUCCESSED(Recv.m_TResult) )
	{
		ClanGameContext::i()->setMyClanMatchTeamList(Recv);	
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_WAR_GET_TEAM_LIST, Recv.m_TResult);
}

void ClientTCPSocket::__Parse_ClanWar_CreateTeam( i3NetworkPacket * pPacket)
{
	PACKET_CLAN_WAR_CREATE_TEAM_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	if( EV_SUCCESSED(Recv.m_TResult) )
	{
		ClanGameContext::i()->setMyMatchSquadInfo(Recv);	
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_WAR_CREATE_TEAM, Recv.m_TResult);
}

void ClientTCPSocket::__Parse_ClanWar_JoinTeam( i3NetworkPacket * pPacket)
{
	PACKET_CLAN_WAR_JOIN_TEAM_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	if( EV_SUCCESSED(Recv.m_TResult) )
	{
		ClanGameContext::i()->setMyMatchSquadInfo(Recv);	
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_WAR_JOIN_TEAM, Recv.m_TResult);
}

void ClientTCPSocket::__Parse_ClanWar_LeaveTeam( i3NetworkPacket * pPacket)
{
	PACKET_CLAN_WAR_LEAVE_TEAM_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	switch( Recv.m_TResult )
	{
	case EVENT_ERROR_SUCCESS				: ClanGameContext::i()->modifyMyMatchSquadInfo(Recv);	break;
	case EVENT_ERROR_CLAN_BATTLE_TEAM_CLOSE	: ClanGameContext::i()->resetMyMatchSquadInfo();		break;
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_WAR_LEAVE_TEAM, Recv.m_TResult);
}

void ClientTCPSocket::__Parse_ClanWar_ChangeOperation( i3NetworkPacket * pPacket)
{
	PACKET_CLAN_WAR_CHANGE_OPERATION_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	if( EV_SUCCESSED(Recv.m_TResult) )
	{
		ClanGameContext::i()->modifyMyMatchSquadOperation(Recv);	
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_WAR_CHANGE_OPERATION, Recv.m_TResult);
}

void ClientTCPSocket::__Parse_ClanWar_ChangeMaxPerson( i3NetworkPacket * pPacket)
{
	PACKET_CLAN_WAR_CHANGE_MAX_PER_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	if( EV_SUCCESSED(Recv.m_TResult) )
	{
		ClanGameContext::i()->modifyMyMatchSquadMaxPerson(Recv);	
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_WAR_CHANGE_MAX_PERSON, Recv.m_TResult);
}

void ClientTCPSocket::__Parse_ClanWar_RegisterMatchMaking( i3NetworkPacket * pPacket)
{
	PACKET_CLAN_WAR_MATCHMAKING_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_WAR_REGISTER_MATCHMAKING, Recv.m_TResult);
}

void ClientTCPSocket::__Parse_ClanWar_RegisterCancelMatchMaking(i3NetworkPacket * pPacket)
{
	PACKET_CLAN_WAR_MATCHMAKING_ACK Recv;
	S2MORecvPacket(&Recv, pPacket->GetPacketBuffer());

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_WAR_REGISTER_CANCEL_MATCHMAKING, Recv.m_TResult);
}

void ClientTCPSocket::__Parse_ClanWar_TeamChatting( i3NetworkPacket * pPacket)
{
	INT32 i32Temp = 0;
	UINT8 cNickSize = 0;
	UINT8 cChatSize = 0;
	UINT8 ui8NickColor = 0;
	TTCHAR  szNick[NET_NICK_NAME_SIZE];
	TTCHAR  String[MAX_STRING_COUNT];
	bool bGameMaster = false;

	i3mem::FillZero(szNick, sizeof(szNick));
	i3mem::FillZero(String, sizeof(String));

	pPacket->ReadData( &i32Temp,		sizeof(INT32) );				// CHAT_LIMIT Type (서버에서만 쓰는거라 그냥 읽고 버립니다)
	pPacket->ReadData( &cNickSize,		sizeof(UINT8) );				// Nick Name Size
	pPacket->ReadData( szNick,			sizeof(TTCHAR) * cNickSize );	// Nick Name
	pPacket->ReadData( &bGameMaster,	sizeof(bool));					// GM Flag
	pPacket->ReadData( &ui8NickColor,	sizeof(UINT8) );				// 컬러닉네임
	pPacket->ReadData( &cChatSize,		sizeof(UINT8) );				// Chatting Message Size
	pPacket->ReadData( String,			sizeof(TTCHAR) * cChatSize);	// Chatting Message

	szNick[ NET_NICK_NAME_SIZE - 1 ] = '\0';
	String[ MAX_STRING_COUNT - 1 ] = '\0';

	i3::rc_wstring wstrNick = szNick;
	i3::wstring		wstrChat = String;

	g_pFramework->getChatBox()->PutUserChatting( wstrNick, wstrChat, bGameMaster, CHATTING_TYPE_MATCH );
}

void ClientTCPSocket::__Parse_ClanWar_BattleTeamInfo( i3NetworkPacket * pPacket)
{
	PACKET_CLAN_WAR_MATCH_TEAM_INFO_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	ClanGameContext::i()->setClanBattleTeamInfo(Recv);
}

void ClientTCPSocket::__Parse_ClanWar_MercenaryList( i3NetworkPacket * pPacket)
{
	PACKET_CLAN_WAR_MERCENARY_LIST_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	if( EV_SUCCESSED(Recv.m_TResult) )
	{
		ClanGameContext::i()->setClanMercenaryList(Recv);	
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_WAR_GET_MERCENARY_LIST, Recv.m_TResult);
}

void ClientTCPSocket::__Parse_ClanWar_RegisterMercenary( i3NetworkPacket * pPacket)
{
	PACKET_CLAN_WAR_REGIST_MERCENARY_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_WAR_REGISTER_MERCENARY, Recv.m_TResult);
}

void ClientTCPSocket::__Parse_ClanWar_RemoveMercenary( i3NetworkPacket * pPacket)
{
	PACKET_CLAN_WAR_REMOVE_MERCENARY_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_WAR_REMOVE_MERCENARY, Recv.m_TResult);
}

void ClientTCPSocket::__Parse_ClanWar_InviteMercenarySender( i3NetworkPacket * pPacket)
{
	PACKET_CLAN_WAR_INVITE_MERCENARY_SENDER_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_WAR_SEND_INVITE_MERCENARY, Recv.m_TResult);
}

void ClientTCPSocket::__Parse_ClanWar_InviteMercenaryReceiver( i3NetworkPacket * pPacket)
{
	PACKET_CLAN_WAR_INVITE_MERCENARY_RECEIVER_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	i3::rc_wstring wstrClanName;
	wstrClanName = &Recv.m_strClanName;

	GameEventReceiver::i()->PushReceivedGameEventUserData(EVENT_CLAN_WAR_RECV_INVITE_MERCENARY, wstrClanName, Recv.m_TResult);
}

void ClientTCPSocket::__Parse_ClanWar_AcceptInvite( i3NetworkPacket * pPacket)
{
	PACKET_CLAN_WAR_INVITE_ACCEPT_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_WAR_ACCEPT_INVITE, Recv.m_TResult);
}

void ClientTCPSocket::__Parse_ClanWar_DenialInvite( i3NetworkPacket * pPacket)
{
	PACKET_CLAN_WAR_INVITE_DENIAL_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	T_MerID MercenaryID = Recv.m_TMerID;

	GameEventReceiver::i()->PushReceivedGameEventUserData(EVENT_CLAN_WAR_DENIAL_INVITE, MercenaryID, Recv.m_TResult);
}

void ClientTCPSocket::__Parse_ClanWar_ForceMatchroomLeave( i3NetworkPacket *pPacket )
{
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_FORCE_LEAVE_MATCHROOM); 
}

void ClientTCPSocket::__Parse_ClanWar_MatchResult(i3NetworkPacket *pPacket)
{
	PACKET_CLAN_WAR_MATCH_MAKING_RESULT_ACK Recv;
	S2MORecvPacket(&Recv, pPacket->GetPacketBuffer());

	ClanGameContext::i()->ProcessMatchResult(Recv.m_MatchMakingResult);
}

void ClientTCPSocket::__Parse_ClanWar_Result(i3NetworkPacket *pPacket)
{
	PACKET_CLAN_WAR_RESULT_ACK Recv;
	S2MORecvPacket(&Recv, pPacket->GetPacketBuffer());

	GameEventReceiver::i()->PushReceivedGameEventUserData(EVENT_CLAN_WAR_RESULT,Recv, Recv.m_TResult ); 
}

void ClientTCPSocket::__Parse_ClanWar_MerceDetailInfo( i3NetworkPacket * pPacket )
{
	PACKET_CLAN_WAR_MERCENARY_DETAIL_INFO_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer());

	if( EV_SUCCESSED(Recv.m_TResult) )
	{
		ClanGameContext::i()->setClanMercenaryRecord(Recv); 
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_WAR_MERCENARY_DETAIL_INFO, Recv.m_TResult); 
}

void ClientTCPSocket::__Parse_ClanWar_Replace_Clan_Name(i3NetworkPacket *pPacket)
{
	PACKET_CLAN_WAR_REPLACE_CLAN_NAME_ACK Recv;
	S2MORecvPacket(&Recv, pPacket->GetPacketBuffer());

	i3::rc_wstring wstrClanName;
	wstrClanName = &Recv.m_strClanName;
	ClanGameContext::i()->setClanNameToTeamID(Recv.m_TTeamID, wstrClanName);
}

void ClientTCPSocket::__Parse_ClanWar_Replace_Clan_Mark(i3NetworkPacket *pPacket)
{
	PACKET_CLAN_WAR_REPLACE_CLAN_MARK_ACK Recv;
	S2MORecvPacket(&Recv, pPacket->GetPacketBuffer());

	UINT32 i32Temp = Recv.m_ui32Mark;
	ClanGameContext::i()->setClanMarkToTeamID(Recv.m_TTeamID, i32Temp);
}

void ClientTCPSocket::__Parse_ClanWar_Replace_Clan_Color_Name(i3NetworkPacket *pPacket)
{
	PACKET_CLAN_WAR_REPLACE_CLAN_COLOR_NAME_ACK Recv;
	S2MORecvPacket(&Recv, pPacket->GetPacketBuffer());

	ClanGameContext::i()->setClanColorNameToTeamID(Recv.m_TTeamID, Recv.m_ui8Color);
}