#include "pch.h"
#include "ClientTCPSocket.h"

#include "ClanGameContext.h"
#include "UserInfoContext.h"
#include "../StageBattle/NetworkContext.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"

namespace
{
	const char * getProtocolName( PROTOCOL protocol)
	{
		switch( protocol)
		{
		case PROTOCOL_CS_MATCH_ALL_TEAM_CONTEXT_ACK :	return "PROTOCOL_CS_MATCH_ALL_TEAM_CONTEXT_ACK";
		case PROTOCOL_CS_MATCH_TEAM_LIST_ACK :			return "PROTOCOL_CS_MATCH_TEAM_LIST_ACK";
		case PROTOCOL_CS_MATCH_ALL_TEAM_LIST_ACK :		return "PROTOCOL_CS_MATCH_ALL_TEAM_LIST_ACK";
		case PROTOCOL_CS_MATCH_TEAM_DETAIL_INFO_ACK :	return "PROTOCOL_CS_MATCH_TEAM_DETAIL_INFO_ACK";
		case PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_ACK :	return "PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_ACK";
		case PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_RESULT_ACK :	return "PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_RESULT_ACK";
		case PROTOCOL_CS_MATCH_TEAM_INFO_ACK :			return "PROTOCOL_CS_MATCH_TEAM_INFO_ACK";
		case PROTOCOL_CS_MATCH_FIGHT_REQUEST_ACK :		return "PROTOCOL_CS_MATCH_FIGHT_REQUEST_ACK";
		case PROTOCOL_CS_MATCH_CHATING_ACK :			return "PROTOCOL_CS_MATCH_CHATING_ACK";
		case PROTOCOL_CS_MATCH_FIGHT_REQUEST_RESULT_ACK :	return "PROTOCOL_CS_MATCH_FIGHT_REQUEST_RESULT_ACK";
		case PROTOCOL_CS_MATCH_FIGHT_ACCECT_ACK :		return "PROTOCOL_CS_MATCH_FIGHT_ACCECT_ACK";
		case PROTOCOL_CS_MATCH_FIGHT_ACCECT_RESULT_ACK :	return "PROTOCOL_CS_MATCH_FIGHT_ACCECT_RESULT_ACK";
		case PROTOCOL_CS_MATCH_FIGHT_INFO_RESULT_ACK :	return "PROTOCOL_CS_MATCH_FIGHT_INFO_RESULT_ACK";
		case PROTOCOL_CS_MATCH_FIGHT_CHANGE_INFO_ACK :	return "PROTOCOL_CS_MATCH_FIGHT_CHANGE_INFO_ACK";
		case PROTOCOL_CS_MATCH_ROOM_CREATE_GS_REQ :		return "PROTOCOL_CS_MATCH_ROOM_CREATE_GS_REQ";
		case PROTOCOL_CS_MATCH_ROOM_JOIN_GS_REQ :		return "PROTOCOL_CS_MATCH_ROOM_JOIN_GS_REQ";
		case PROTOCOL_CS_MATCH_NEW_OPERATION_ACK :		return "PROTOCOL_CS_MATCH_NEW_OPERATION_ACK";
	
		case PROTOCOL_CS_MATCH_NEW_CLAN_INFO_ACK :		return "PROTOCOL_CS_MATCH_NEW_CLAN_INFO_ACK";
		case PROTOCOL_CS_MATCH_NEW_HONOR_ACK :			return "PROTOCOL_CS_MATCH_NEW_HONOR_ACK";
		case PROTOCOL_CS_MATCH_NEW_HONOR_REQUEST_ACK :	return "PROTOCOL_CS_MATCH_NEW_HONOR_REQUEST_ACK";
		case PROTOCOL_CS_MATCH_NEW_HONOR_ACCEPT_ACK :	return "PROTOCOL_CS_MATCH_NEW_HONOR_ACCEPT_ACK";

		default :
			I3FATAL( "PROTOCOL_CLANMATCH_XXX 프로토콜 이름 등록해주이소~");
			return "PROTOCOL_CLANMATCH_UNKNOWN";
		}
	}
}

void ClientTCPSocket::_PacketParsingClanBattle( i3NetworkPacket  * pPacket )
{
	i3::unordered_map< UINT32, netFunc >::iterator it;
	it = m_NetFuncMap_ClanMatch.find( pPacket->GetProtocolID());

	if ( it != m_NetFuncMap_ClanMatch.end() )
	{
		NETLOG( "* %s -->", getProtocolName( pPacket->GetProtocolID()) );
		netFunc & mf = it->second;
		(this->*mf)(pPacket);
		NETLOG( "* %s <--", getProtocolName( pPacket->GetProtocolID()) );
	}
	else
		__Parse_UnknownProtocol( pPacket);
}

void ClientTCPSocket::__Register_ClanMatch_Function( void)
{

	RegisterParser( PROTOCOL_CS_MATCH_TEAM_DETAIL_INFO_ACK,			&ClientTCPSocket::__Parse_ClanMatch_TeamDetailInfo);
	RegisterParser( PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_ACK,			&ClientTCPSocket::__Parse_ClanMatch_ChangeTeam);
	RegisterParser( PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_RESULT_ACK,	&ClientTCPSocket::__Parse_ClanMatch_ChangeTeamResult);
	RegisterParser( PROTOCOL_CS_MATCH_TEAM_INFO_ACK,				&ClientTCPSocket::__Parse_ClanMatch_TeamInfo);
	RegisterParser( PROTOCOL_CS_MATCH_FIGHT_REQUEST_ACK,			&ClientTCPSocket::__Parse_ClanMatch_RequestFight);
	RegisterParser( PROTOCOL_CS_MATCH_CHATING_ACK,					&ClientTCPSocket::__Parse_ClanMatch_Chat);
	RegisterParser( PROTOCOL_CS_MATCH_FIGHT_REQUEST_RESULT_ACK,		&ClientTCPSocket::__Parse_ClanMatch_RequestFightResult);
	RegisterParser( PROTOCOL_CS_MATCH_FIGHT_ACCECT_ACK,				&ClientTCPSocket::__Parse_ClanMatch_AcceptFight);
	RegisterParser( PROTOCOL_CS_MATCH_FIGHT_ACCECT_RESULT_ACK,		&ClientTCPSocket::__Parse_ClanMatch_AcceptFightResult);
	RegisterParser( PROTOCOL_CS_MATCH_FIGHT_INFO_RESULT_ACK,		&ClientTCPSocket::__Parse_ClanMatch_FightInfoResult);
	RegisterParser( PROTOCOL_CS_MATCH_FIGHT_CHANGE_INFO_ACK,		&ClientTCPSocket::__Parse_ClanMatch_ChangeFightInfo);
	RegisterParser( PROTOCOL_CS_MATCH_ROOM_CREATE_GS_REQ,			&ClientTCPSocket::__Parse_ClanMatch_CreateRoomGS);
	RegisterParser( PROTOCOL_CS_MATCH_ROOM_JOIN_GS_REQ,				&ClientTCPSocket::__Parse_ClanMatch_JoinRoomGS);
	//CLAN NEW PROTOCOL
	RegisterParser( PROTOCOL_CS_MATCH_NEW_OPERATION_ACK,			&ClientTCPSocket::__Parse_Clan_SquadRename);

	RegisterParser( PROTOCOL_CS_MATCH_NEW_CLAN_INFO_ACK,			&ClientTCPSocket::__Parse_Clan_MatchClanInfo);
	RegisterParser( PROTOCOL_CS_MATCH_NEW_HONOR_ACK,				&ClientTCPSocket::__Parse_Clan_Honor);
	RegisterParser( PROTOCOL_CS_MATCH_NEW_HONOR_REQUEST_ACK,		&ClientTCPSocket::__Parse_Clan_Honorenemy);
	RegisterParser( PROTOCOL_CS_MATCH_NEW_HONOR_ACCEPT_ACK,			&ClientTCPSocket::__Parse_Clan_Honoraccept);
}


void ClientTCPSocket::__Parse_ClanMatch_TeamDetailInfo( i3NetworkPacket * pPacket)
{
	INT32 Temp;

	pPacket->ReadData(&Temp,		sizeof(INT32));
	if( EV_SUCCESSED( Temp ) )
	{
		CLAN_DETAIL_INFO * pInfo = ClanGameContext::i()->getClanDetailInfo();

		pPacket->ReadData(pInfo,	sizeof(CLAN_DETAIL_INFO));

	}
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_MATCH_TEAM_DETAIL_INFO, Temp );
}

void ClientTCPSocket::__Parse_ClanMatch_ChangeTeam( i3NetworkPacket * pPacket)
{
	INT32 Temp;

	pPacket->ReadData(&Temp,	sizeof(INT32));
	if( EV_SUCCESSED( Temp ) )
	{
		UINT8 cPerMax;
		pPacket->ReadData(&cPerMax,	sizeof(UINT8));

		CLAN_BATTLE_TEAM_INFO*  pTeamInfo = ClanGameContext::i()->getClanBattleMyTeamInfo();

		pTeamInfo->_Team._PerMax = cPerMax;
	}
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_MATCH_TEAM_CHANGE_PER, Temp );
}

void ClientTCPSocket::__Parse_ClanMatch_ChangeTeamResult( i3NetworkPacket * pPacket)
{
	UINT8 cPerMax;

	pPacket->ReadData(&cPerMax,	sizeof(UINT8));

	CLAN_BATTLE_TEAM_INFO*  pTeamInfo = ClanGameContext::i()->getClanBattleMyTeamInfo();

	pTeamInfo->_Team._PerMax = cPerMax;

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_MATCH_TEAM_CHANGE_PER_RESULT_N );
}

void ClientTCPSocket::__Parse_ClanMatch_TeamInfo( i3NetworkPacket * pPacket)
{
	CLAN_BATTLE_TEAM_INFO*	pTeamInfo	= ClanGameContext::i()->getClanBattleMyTeamInfo();
	CLAN_BATTLE_MEMBER*		pTeamMember = ClanGameContext::i()->getClanBattleMyTeamMember();

	pPacket->ReadData(&pTeamInfo->_Team,		sizeof(CLAN_BATTLE_TEAM));
	pPacket->ReadData(pTeamMember,				sizeof(CLAN_BATTLE_MEMBER) * pTeamInfo->_Team._PerMax );
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_MATCH_TEAM_INFO_N );
}

void ClientTCPSocket::__Parse_ClanMatch_RequestFight( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_MATCH_FIGHT_REQUEST, Temp );
}

void ClientTCPSocket::__Parse_ClanMatch_Chat( i3NetworkPacket * pPacket)
{
	UINT8 cNickSize;
	UINT8 cChatSize;
	UINT8 ui8NickColor;
	TTCHAR  szNick[NET_NICK_NAME_SIZE]; 
	TTCHAR  String[MAX_STRING_COUNT];
	bool bGameMaster;
	::memset(String, 0, sizeof(TTCHAR) * MAX_STRING_COUNT);

	pPacket->ReadData( &cNickSize,	sizeof(UINT8) );
	pPacket->ReadData( szNick,		cNickSize * sizeof(TTCHAR) );
	pPacket->ReadData( &bGameMaster, sizeof(bool));
	pPacket->ReadData( &ui8NickColor,	sizeof(UINT8) );	// 컬러닉네임
	pPacket->ReadData( &cChatSize,	sizeof(UINT8) );
	pPacket->ReadData( String,		cChatSize * sizeof(TTCHAR) );

	szNick[ NET_NICK_NAME_SIZE - 1 ] = '\0';
	String[ MAX_STRING_COUNT - 1 ] = '\0';

#ifdef __UNICODE_STRING__
	i3::rc_wstring wstrNick = szNick;
#else
	i3::rc_wstring wstrNick;
	i3::mb_to_utf16(szNick, wstrNick);
#endif

#ifdef __UNICODE_STRING__
	i3::wstring		wstrChat = String;
#else
	i3::wstring		wstrChat;
	i3::mb_to_utf16(String, wstrChat);						
#endif

	g_pFramework->getChatBox()->PutUserChatting( wstrNick, wstrChat, bGameMaster, CHATTING_TYPE_MATCH );
}

void ClientTCPSocket::__Parse_ClanMatch_RequestFightResult( i3NetworkPacket * pPacket)
{
	CLAN_BATTLE_TEAM_INFO*	pTeamInfo = ClanGameContext::i()->getClanBattleReqTeamInfo();

	pPacket->ReadData(pTeamInfo,	sizeof(CLAN_BATTLE_TEAM_INFO));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_M_MATCH_FIGHT_RESULT );
}

void ClientTCPSocket::__Parse_ClanMatch_AcceptFight( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_MATCH_FIGHT_ACCECT, Temp );
}

void ClientTCPSocket::__Parse_ClanMatch_AcceptFightResult( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_M_MATCH_FIGHT_ACCECT_RESULT, Temp );
}

void ClientTCPSocket::__Parse_ClanMatch_FightInfoResult( i3NetworkPacket * pPacket)
{
	CLAN_BATTLE_TEAM_INFO*	pTeamInfo = ClanGameContext::i()->getClanBattleReqTeamInfo();
	pPacket->ReadData(&pTeamInfo->_Team,	sizeof(CLAN_BATTLE_TEAM));
	pPacket->ReadData(&pTeamInfo->_Clan,	sizeof(CLAN_BATTLE_CLAN));
}

void ClientTCPSocket::__Parse_ClanMatch_ChangeFightInfo( i3NetworkPacket * pPacket)
{
	CLAN_BATTLE_TEAM_INFO*	pTeamInfo	= ClanGameContext::i()->getClanBattleMyTeamInfo();
	pPacket->ReadData(&pTeamInfo->_Clan,	sizeof(CLAN_BATTLE_CLAN));
	//GameEventReceiver::i()->PushReceivedGameEvent( EVENT_M_MATCH_FIGHT_CHANGE_INFO_RESULT, Temp );
}

void ClientTCPSocket::__Parse_ClanMatch_CreateRoomGS( i3NetworkPacket * pPacket)
{
	UINT32 ui32TeamIdx;
	UINT32 ui32RequsetTeamIdx;
	UINT8 ui8PerMax;

	pPacket->ReadData(&ui32TeamIdx,			sizeof(UINT32));
	pPacket->ReadData(&ui32RequsetTeamIdx,	sizeof(UINT32));
	pPacket->ReadData(&ui8PerMax,			sizeof(UINT8));

	ROOM_INFO_BASIC	stRoomInfoBasic;

	stRoomInfoBasic.Reset();

	i3::safe_string_copy( stRoomInfoBasic._Title, _TT("Clan Battle"), NET_ROOM_NAME_SIZE );

	stRoomInfoBasic._PersonPlayerMax	= ui8PerMax;
	stRoomInfoBasic._StageID	= STAGE_ID_BB_DOWNTOWN;	
	stRoomInfoBasic._StageMask	= STAGE_MASK_TYPE_CLANMATCH;
	stRoomInfoBasic._InfoFlag	= ROOM_INFO_FLAG_INTER_ENTER | ROOM_INFO_FLAG_OBSERVER;
	stRoomInfoBasic._WeaponFlag	= 0x0F;

	GameEventSender::i()->SetEvent( EVENT_MATCH_ROOM_CREATE, &ui32TeamIdx, &ui32RequsetTeamIdx, &stRoomInfoBasic );
}

void ClientTCPSocket::__Parse_ClanMatch_JoinRoomGS( i3NetworkPacket * pPacket)
{
	UINT32 ui32RoomIdx;
	UINT32 ui32TeamIdx;

	pPacket->ReadData(&ui32RoomIdx,		sizeof(UINT32));
	pPacket->ReadData(&ui32TeamIdx,		sizeof(UINT32));			

	GameEventSender::i()->SetEvent( EVENT_MATCH_ROOM_JOIN, &ui32RoomIdx, &ui32TeamIdx );
}


void ClientTCPSocket::__Parse_Clan_SquadRename( i3NetworkPacket * pPacket)
{
	CLAN_BATTLE_TEAM_INFO*	pTeamInfo	= ClanGameContext::i()->getClanBattleMyTeamInfo();

	PACKET_CS_MATCH_NEW_OPERATION_ACK squadinfo;
	pPacket->ReadData( &squadinfo, sizeof(PACKET_CS_MATCH_NEW_OPERATION_ACK) );

	i3::safe_string_copy( pTeamInfo->_Team.m_strOperation, squadinfo.m_strOperation, NET_CLAN_OPERATION_SIZE );

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_MATCH_TEAM_RENAME);
}

void ClientTCPSocket::__Parse_Clan_MatchClanInfo( i3NetworkPacket * pPacket)
{
	INT32 Temp;

	PACKET_D_CS_MATCH_NEW_CLAN_INFO*		ClanMatchInfo = ClanGameContext::i()->getClanMatchInfo();
	PACKET_D_CS_MATCH_NEW_CLAN_INFO MatchInfo;

	pPacket->ReadData(&Temp,	sizeof(INT32));
	if( EV_SUCCESSED( Temp ) )
	{
		pPacket->ReadData(&MatchInfo,	sizeof(PACKET_D_CS_MATCH_NEW_CLAN_INFO));
		*ClanMatchInfo = MatchInfo;
	}

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_MATCH_TEAM_MATCHINFO, Temp );
}
void ClientTCPSocket::__Parse_Clan_Honor( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_MATCH_TEAM_HONOR, Temp );
}
void ClientTCPSocket::__Parse_Clan_Honorenemy( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_MATCH_TEAM_HONORENEMY, Temp );
}
void ClientTCPSocket::__Parse_Clan_Honoraccept( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_MATCH_TEAM_HONORACCEPT, Temp );
}