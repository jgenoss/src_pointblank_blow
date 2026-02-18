#include "pch.h"
#include "GameEventHandler.h"
#include "CommunityContext.h"
#include "BattleSlotContext.h"
#include "StageBattle/UserContext.h"

#include "i3Base/string/ext/utf16_to_mb.h"


//======================================================================================//
//										Clan Event										//
//======================================================================================//

const char * CGameEventHandler::__getClanEventName( GAME_EVENT evt) const
{
	switch( evt)
	{
	case EVENT_CLAN_ENTER								: return "EVENT_CLAN_ENTER";
	case EVENT_CLAN_LIST_CONTEXT						: return "EVENT_CLAN_LIST_CONTEXT";
	case EVENT_CLAN_LEAVE								: return "EVENT_CLAN_LEAVE";
	case EVENT_CLAN_CREATE								: return "EVENT_CLAN_CREATE";
	case EVENT_CLAN_DUPLICATE_NAME						: return "EVENT_CLAN_DUPLICATE_NAME";
	case EVENT_CLAN_DUPLICATE_ADDRESS					: return "EVENT_CLAN_DUPLICATE_ADDRESS";
	case EVENT_CLAN_DUPLICATE_MARK						: return "EVENT_CLAN_DUPLICATE_MARK";
	case EVENT_CLAN_CHANGE_MARK							: return "EVENT_CLAN_CHANGE_MARK";
	case EVENT_CLAN_JOIN_REQUEST						: return "EVENT_CLAN_JOIN_REQUEST";
	case EVENT_CLAN_JOINCHECK_AUTHORITY					: return "EVENT_CLAN_JOINCHECK_AUTHORITY";
	case EVENT_CLAN_CANCEL_REQUEST						: return "EVENT_CLAN_CANCEL_REQUEST";
	case EVENT_CLAN_MEMBER_CONTEXT						: return "EVENT_CLAN_MEMBER_CONTEXT";
	case EVENT_CLAN_REQUEST_CONTEXT						: return "EVENT_CLAN_REQUEST_CONTEXT";
	case EVENT_CLAN_ACCEPT_REQUEST						: return "EVENT_CLAN_ACCEPT_REQUEST";
	case EVENT_CLAN_DENIAL_REQUEST						: return "EVENT_CLAN_DENIAL_REQUEST";
	case EVENT_CLAN_SECESSION							: return "EVENT_CLAN_SECESSION";
	case EVENT_CLAN_COMMISSIONMASTER					: return "EVENT_CLAN_COMMISSIONMASTER";
	case EVENT_CLAN_COMMISSIONSTAFF						: return "EVENT_CLAN_COMMISSIONSTAFF";
	case EVENT_CLAN_COMMISSION_REGULAR					: return "EVENT_CLAN_COMMISSION_REGULAR";
	case EVENT_CLAN_DISMISSALMEMBER						: return "EVENT_CLAN_DISMISSALMEMBER";
	case EVENT_CLAN_DISSOLUTION							: return "EVENT_CLAN_DISSOLUTION";
	case EVENT_CLAN_NOTICE_MODIFY						: return "EVENT_CLAN_NOTICE_MODIFY";
	case EVENT_CLAN_GUIDE_MODIFY						: return "EVENT_CLAN_GUIDE_MODIFY";
	case EVENT_CLAN_ACCEPT_OPTION_MODIFY				: return "EVENT_CLAN_ACCEPT_OPTION_MODIFY";
	case EVENT_CLAN_MANAGEMENT							: return "EVENT_CLAN_MANAGEMENT";
	case EVENT_CLAN_CREATION_INFO						: return "EVENT_CLAN_CREATION_INFO";
	case EVENT_CLAN_LIST_N								: return "EVENT_CLAN_LIST_N";
	case EVENT_CLAN_LIST_DETAIL_INFO					: return "EVENT_CLAN_LIST_DETAIL_INFO";
	case EVENT_CLAN_DETAIL_INFO_N						: return "EVENT_CLAN_DETAIL_INFO_N";
	case EVENT_CLAN_MEMBER_LIST_N						: return "EVENT_CLAN_MEMBER_LIST_N";
	case EVENT_CLAN_REQUEST_LIST_N						: return "EVENT_CLAN_REQUEST_LIST_N";
	case EVENT_CLAN_REQUEST_INFO_N						: return "EVENT_CLAN_REQUEST_INFO_N";
	case EVENT_CLAN_INVITE								: return "EVENT_CLAN_INVITE";
	case EVENT_CLAN_INVITE_ACCEPT_N						: return "EVENT_CLAN_INVITE_ACCEPT_N";
	case EVENT_CLAN_NOTE								: return "EVENT_CLAN_NOTE";
	case EVENT_COMMUNITY_RECORD_CLAN_N					: return "EVENT_COMMUNITY_RECORD_CLAN_N";
	case EVENT_CLAN_COMMUNITY_LIST						: return "EVENT_CLAN_COMMUNITY_LIST";
	case EVENT_CLAN_MATCH_RESULT_CONTEXT				: return "EVENT_CLAN_MATCH_RESULT_CONTEXT";
	case EVENT_CLAN_MATCH_RESULT_LIST					: return "EVENT_CLAN_MATCH_RESULT_LIST";
	case EVENT_CLAN_MATCH_RESULT_EMOTION				: return "EVENT_CLAN_MATCH_RESULT_EMOTION";

	case EVENT_CLAN_WAR_PRESEASON_RANKING				: return "EVENT_CLAN_WAR_PRESEASON_RANKING";
	case EVENT_CLAN_WAR_GET_TEAM_LIST					: return "EVENT_CLAN_WAR_GET_TEAM_LIST";
	case EVENT_CLAN_WAR_CREATE_TEAM						: return "EVENT_CLAN_WAR_CREATE_TEAM";
	case EVENT_CLAN_WAR_JOIN_TEAM						: return "EVENT_CLAN_WAR_JOIN_TEAM";
	case EVENT_CLAN_WAR_LEAVE_TEAM						: return "EVENT_CLAN_WAR_LEAVE_TEAM";
	case EVENT_CLAN_WAR_CHANGE_OPERATION				: return "EVENT_CLAN_WAR_CHANGE_OPERATION";
	case EVENT_CLAN_WAR_CHANGE_MAX_PERSON				: return "EVENT_CLAN_WAR_CHANGE_MAX_PERSON";
	case EVENT_CLAN_WAR_REGISTER_MATCHMAKING			: return "EVENT_CLAN_WAR_REGISTER_MATCHMAKING";
	case EVENT_CLAN_WAR_REGISTER_CANCEL_MATCHMAKING		: return "EVENT_CLAN_WAR_REGISTER_CANCEL_MATCHMAKING";
	case EVENT_CLAN_WAR_GET_MERCENARY_LIST				: return "EVENT_CLAN_WAR_GET_MERCENARY_LIST";
	case EVENT_CLAN_WAR_REGISTER_MERCENARY				: return "EVENT_CLAN_WAR_REGISTER_MERCENARY";
	case EVENT_CLAN_WAR_REMOVE_MERCENARY				: return "EVENT_CLAN_WAR_REMOVE_MERCENARY";
	case EVENT_CLAN_WAR_SEND_INVITE_MERCENARY			: return "EVENT_CLAN_WAR_SEND_INVITE_MERCENARY";
	case EVENT_CLAN_WAR_ACCEPT_INVITE					: return "EVENT_CLAN_WAR_ACCEPT_INVITE";
	case EVENT_CLAN_WAR_DENIAL_INVITE					: return "EVENT_CLAN_WAR_DENIAL_INVITE";
	case EVENT_CLAN_WAR_MERCENARY_DETAIL_INFO			: return "EVENT_CLAN_WAR_MERCENARY_DETAIL_INFO";
	case EVENT_CLAN_WAR_RESULT							: return "EVENT_CLAN_WAR_RESULT";
	default												: return NULL;
	}
}

void CGameEventHandler::__register_Clan( void)
{
	regSendEvent( EVENT_CLAN_ENTER,						&CGameEventHandler::__evtClan_EnterPage);
	regSendEvent( EVENT_CLAN_LIST_CONTEXT,				&CGameEventHandler::__evtClan_ListContext);
	regSendEvent( EVENT_CLAN_LEAVE,						&CGameEventHandler::__evtClan_LeavePage);
	regSendEvent( EVENT_CLAN_CREATE,					&CGameEventHandler::__evtClan_Create);
	regSendEvent( EVENT_CLAN_DUPLICATE_NAME,			&CGameEventHandler::__evtClan_DuplicateName);
	regSendEvent( EVENT_CLAN_DUPLICATE_ADDRESS,			&CGameEventHandler::__evtClan_DuplicateURL);
	regSendEvent( EVENT_CLAN_DUPLICATE_MARK,			&CGameEventHandler::__evtClan_DuplicateMark);
	regSendEvent( EVENT_CLAN_CHANGE_MARK,				&CGameEventHandler::__evtClan_ChangeMark);
	regSendEvent( EVENT_CLAN_JOIN_REQUEST,				&CGameEventHandler::__evtClan_RequestJoin);
	regSendEvent( EVENT_CLAN_JOINCHECK_AUTHORITY,		&CGameEventHandler::__evtClan_CheckAuthorityForJoin);
	regSendEvent( EVENT_CLAN_CANCEL_REQUEST,			&CGameEventHandler::__evtClan_CancelRequestJoin);
	regSendEvent( EVENT_CLAN_MEMBER_CONTEXT,			&CGameEventHandler::__evtClan_MemberContext);
	regSendEvent( EVENT_CLAN_REQUEST_CONTEXT,			&CGameEventHandler::__evtClan_RequestContext);
	regSendEvent( EVENT_CLAN_ACCEPT_REQUEST,			&CGameEventHandler::__evtClan_AcceptRequest);
	regSendEvent( EVENT_CLAN_DENIAL_REQUEST,			&CGameEventHandler::__evtClan_DenialRequest);
	regSendEvent( EVENT_CLAN_SECESSION,					&CGameEventHandler::__evtClan_Secession);
	regSendEvent( EVENT_CLAN_COMMISSIONMASTER,			&CGameEventHandler::__evtClan_CommisionMaster);
	regSendEvent( EVENT_CLAN_COMMISSIONSTAFF,			&CGameEventHandler::__evtClan_CommisionStaff);
	regSendEvent( EVENT_CLAN_COMMISSION_REGULAR,		&CGameEventHandler::__evtClan_CommisionRegular);
	regSendEvent( EVENT_CLAN_DISMISSALMEMBER,			&CGameEventHandler::__evtClan_DismissalMember);
	regSendEvent( EVENT_CLAN_DISSOLUTION,				&CGameEventHandler::__evtClan_Dissolution);
	regSendEvent( EVENT_CLAN_NOTICE_MODIFY,				&CGameEventHandler::__evtClan_ModifyNotice);
	regSendEvent( EVENT_CLAN_GUIDE_MODIFY,				&CGameEventHandler::__evtClan_ModifyGuide);
	regSendEvent( EVENT_CLAN_ACCEPT_OPTION_MODIFY,		&CGameEventHandler::__evtClan_ModifyAcceptOption);
	regSendEvent( EVENT_CLAN_MANAGEMENT,				&CGameEventHandler::__evtClan_Menagement);
	regSendEvent( EVENT_CLAN_CREATION_INFO,				&CGameEventHandler::__evtClan_CreationInfo);

	regSendEvent( EVENT_CLAN_LIST_N,					&CGameEventHandler::__evtClan_List);
	regSendEvent( EVENT_CLAN_LIST_DETAIL_INFO,			&CGameEventHandler::__evtClan_ListDetailInfo);
	regSendEvent( EVENT_CLAN_DETAIL_INFO_N,				&CGameEventHandler::__evtClan_DetailInfo);
	regSendEvent( EVENT_CLAN_MEMBER_LIST_N,				&CGameEventHandler::__evtClan_MemberList);
	regSendEvent( EVENT_CLAN_REQUEST_LIST_N,			&CGameEventHandler::__evtClan_RequestList);
	regSendEvent( EVENT_CLAN_REQUEST_INFO_N,			&CGameEventHandler::__evtClan_RequestInfo);
	regSendEvent( EVENT_CLAN_INVITE,					&CGameEventHandler::__evtClan_Invite);
	regSendEvent( EVENT_CLAN_INVITE_ACCEPT_N,			&CGameEventHandler::__evtClan_AcceptInvite);
	regSendEvent( EVENT_CLAN_NOTE,						&CGameEventHandler::__evtClan_Note);

	regSendEvent( EVENT_COMMUNITY_RECORD_CLAN_N,		&CGameEventHandler::__evtClan_DetailMemberInfo);
	regSendEvent( EVENT_CLAN_COMMUNITY_LIST,			&CGameEventHandler::__evtClan_MemberInfo);

	regSendEvent( EVENT_CLAN_MATCH_RESULT_CONTEXT,		&CGameEventHandler::__evtClan_MatchResultContext);
	regSendEvent( EVENT_CLAN_MATCH_RESULT_LIST,			&CGameEventHandler::__evtClan_MatchResultList);
	regSendEvent( EVENT_CLAN_MATCH_RESULT_EMOTION,		&CGameEventHandler::__evtClan_MatchResultEmotion);

	regSendEvent( EVENT_CLAN_WAR_PRESEASON_RANKING,				&CGameEventHandler::__evtClanWar_PreSeasonRanking,					new S2MOPacketBase(PROTOCOL_CLAN_WAR_CHECK_PRESEASON_RANKING_REQ) );
	regSendEvent( EVENT_CLAN_WAR_GET_TEAM_LIST,					&CGameEventHandler::__evtClanWar_GetTeamList,						new S2MOPacketBase(PROTOCOL_CLAN_WAR_MATCH_TEAM_LIST_REQ) );
	regSendEvent( EVENT_CLAN_WAR_CREATE_TEAM,					&CGameEventHandler::__evtClanWar_CreateTeam,						new PACKET_CLAN_WAR_CREATE_TEAM_REQ() );
	regSendEvent( EVENT_CLAN_WAR_JOIN_TEAM,						&CGameEventHandler::__evtClanWar_JoinTeam,							new PACKET_CLAN_WAR_JOIN_TEAM_REQ() );
	regSendEvent( EVENT_CLAN_WAR_LEAVE_TEAM,					&CGameEventHandler::__evtClanWar_LeaveTeam,							new PACKET_CLAN_WAR_LEAVE_TEAM_REQ() );
	regSendEvent( EVENT_CLAN_WAR_CHANGE_OPERATION,				&CGameEventHandler::__evtClanWar_ChangeOperation,					new PACKET_CLAN_WAR_CHANGE_OPERATION_REQ() );
	regSendEvent( EVENT_CLAN_WAR_CHANGE_MAX_PERSON,				&CGameEventHandler::__evtClanWar_ChangeMaxPerson,					new PACKET_CLAN_WAR_CHANGE_MAX_PER_REQ() );
	regSendEvent( EVENT_CLAN_WAR_REGISTER_MATCHMAKING,			&CGameEventHandler::__evtClanWar_RegisterMatchMaking,				new PACKET_CLAN_WAR_MATCHMAKING_REQ() );
	regSendEvent( EVENT_CLAN_WAR_REGISTER_CANCEL_MATCHMAKING,	&CGameEventHandler::__evtClanWar_RegisterCancelMatchMaking,			new PACKET_CLAN_WAR_CANCEL_MATCHMAKING_REQ());
	
	regSendEvent( EVENT_CLAN_WAR_GET_MERCENARY_LIST,	&CGameEventHandler::__evtClanWar_GetMercenaryList,			new PACKET_CLAN_WAR_MERCENARY_LIST_REQ() );
	regSendEvent( EVENT_CLAN_WAR_REGISTER_MERCENARY,	&CGameEventHandler::__evtClanWar_RegisterMercenary,			new PACKET_CLAN_WAR_REGIST_MERCENARY_REQ() );
	regSendEvent( EVENT_CLAN_WAR_REMOVE_MERCENARY,		&CGameEventHandler::__evtClanWar_RemoveMercenary,			new PACKET_CLAN_WAR_REMOVE_MERCENARY_REQ() );
	regSendEvent( EVENT_CLAN_WAR_SEND_INVITE_MERCENARY,	&CGameEventHandler::__evtClanWar_SendInviteMercenary,		new PACKET_CLAN_WAR_INVITE_MERCENARY_REQ() );
	regSendEvent( EVENT_CLAN_WAR_ACCEPT_INVITE,			&CGameEventHandler::__evtClanWar_AcceptInvite,				new PACKET_CLAN_WAR_INVITE_ACCEPT_REQ() );
	regSendEvent( EVENT_CLAN_WAR_DENIAL_INVITE,			&CGameEventHandler::__evtClanWar_DenialInvite,				new PACKET_CLAN_WAR_INVITE_DENIAL_REQ() );
	regSendEvent( EVENT_CLAN_WAR_MERCENARY_DETAIL_INFO, &CGameEventHandler::__evtClanWar_MercenaryDetail,		new PACKET_CLAN_WAR_MERCENARY_DETAIL_INFO_REQ() );
	regSendEvent( EVENT_CLAN_WAR_RESULT,				&CGameEventHandler::__evtClanWar_Result);
}

bool CGameEventHandler::__evtClan_EnterPage( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_CLIENT_ENTER_REQ);
	return true;
}

bool CGameEventHandler::__evtClan_ListContext( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_CLIENT_CLAN_CONTEXT_REQ);
	return true;
}

bool CGameEventHandler::__evtClan_LeavePage( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_CLIENT_LEAVE_REQ);
	return true;
}

bool CGameEventHandler::__evtClan_Create( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_CREATE_CLAN_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(PACKET_CS_NEW_CREATE_CLAN_REQ));
	return true;
}

bool CGameEventHandler::__evtClan_DuplicateName( GAMEEVENTINFO * pInfo)
{
//	UINT8 ui8Name	= (UINT8)i3::generic_string_size( (wchar_t*) pInfo->_pArg )+1;

	size_t size = *(size_t*)pInfo->_pArg1;
	i3::wliteral_range wstrName((const wchar_t*)pInfo->_pArg, (const wchar_t*)pInfo->_pArg + size);

	i3::stack_wstring strName = wstrName;

	UINT8 ui8Name	= UINT8(strName.size()) + 1;
	
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_CHECK_DUPLICATE_REQ);
	pInfo->_SendPacket.WriteData(&ui8Name,	sizeof(UINT8));
	pInfo->_SendPacket.WriteData(strName.c_str(), ui8Name * sizeof(TTCHAR));
	return true;
}

bool CGameEventHandler::__evtClan_DuplicateURL( GAMEEVENTINFO * pInfo)
{
	
	const i3::wliteral_range strURL = ((const wchar_t*)pInfo->_pArg);

	UINT8 ui8Name	= UINT8(strURL.size()) + 1;
	
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_CHECK_DUPLICATE_AZIT_URL_REQ);
	pInfo->_SendPacket.WriteData(&ui8Name,	sizeof(UINT8));
	pInfo->_SendPacket.WriteData(strURL.c_str(), ui8Name * sizeof(TTCHAR) );
	return true;
}

bool CGameEventHandler::__evtClan_DuplicateMark( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_CHECK_MARK_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(INT32));
	return true;
}

bool CGameEventHandler::__evtClan_ChangeMark( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_REPLACE_MARK_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(INT32));
	return true;
}

bool CGameEventHandler::__evtClan_RequestJoin( GAMEEVENTINFO * pInfo)
{
	const wchar_t* wstr_message = static_cast<const wchar_t*>(pInfo->_pArg1);

	const wchar_t* str_message = wstr_message;
	UINT8 ui8Size = (UINT8)i3::generic_string_size( str_message )+1;

	pInfo->_SendPacket.SetProtocol( PROTOCOL_CS_JOIN_REQUEST_REQ );
	pInfo->_SendPacket.WriteData( pInfo->_pArg,		sizeof(UINT32) );
	pInfo->_SendPacket.WriteData( &ui8Size,	sizeof(UINT8) );
	pInfo->_SendPacket.WriteData( str_message,	ui8Size * sizeof(TTCHAR) );
	return true;
}

bool CGameEventHandler::__evtClan_CheckAuthorityForJoin( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_CHECK_JOIN_AUTHORITY_REQ);
	pInfo->_SendPacket.WriteData( pInfo->_pArg,	sizeof(UINT32));	// 클랜 DB 인덱스
	return true;
}

bool CGameEventHandler::__evtClan_CancelRequestJoin( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_CANCEL_REQUEST_REQ);
	return true;
}

bool CGameEventHandler::__evtClan_ListDetailInfo( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_CLAN_LIST_DETAIL_INFO_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(PACKET_CS_NEW_CLAN_LIST_DETAIL_INFO_REQ));
	return true;
}

bool CGameEventHandler::__evtClan_DetailInfo( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_DETAIL_INFO_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(PACKET_CS_NEW_DETAIL_INFO_REQ));
	return true;
}

bool CGameEventHandler::__evtClan_MemberContext( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_MEMBER_CONTEXT_REQ);
	return true;
}

bool CGameEventHandler::__evtClan_MemberList( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_MEMBER_LIST_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(UINT8));
	return true;
}

bool CGameEventHandler::__evtClan_RequestContext( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_REQUEST_CONTEXT_REQ);
	return true;
}

bool CGameEventHandler::__evtClan_RequestList( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_REQUEST_LIST_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(PACKET_CS_NEW_REQUEST_LIST_REQ));

	return true;
}

bool CGameEventHandler::__evtClan_RequestInfo( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_REQUEST_INFO_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(T_UID));
	return true;
}

bool CGameEventHandler::__evtClan_AcceptRequest( GAMEEVENTINFO * pInfo)
{
	UINT8 count = *(UINT8*)pInfo->_pArg;
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_ACCEPT_REQUEST_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg,	sizeof(UINT8));
	pInfo->_SendPacket.WriteData(pInfo->_pArg1, sizeof(T_UID) * count);
	return true;
}

bool CGameEventHandler::__evtClan_DenialRequest( GAMEEVENTINFO * pInfo)
{
	UINT8 count = *(UINT8*)pInfo->_pArg;
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_DENIAL_REQUEST_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg,	sizeof(UINT8));
	pInfo->_SendPacket.WriteData(pInfo->_pArg1, sizeof(T_UID) * count);
	return true;
}

bool CGameEventHandler::__evtClan_Secession( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_SECESSION_CLAN_REQ);
	return true;
}

bool CGameEventHandler::__evtClan_CommisionMaster( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_COMMISSION_MASTER_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg,	sizeof(T_UID));
	return true;
}

bool CGameEventHandler::__evtClan_CommisionStaff( GAMEEVENTINFO * pInfo)
{
	UINT8 count = *(UINT8*) pInfo->_pArg;
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_COMMISSION_STAFF_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg,	sizeof(UINT8));
	pInfo->_SendPacket.WriteData(pInfo->_pArg1,	sizeof(T_UID) * count);
	return true;
}

bool CGameEventHandler::__evtClan_CommisionRegular( GAMEEVENTINFO * pInfo)
{
	UINT8 count = *(UINT8*) pInfo->_pArg;
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_COMMISSION_REGULAR_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg,	sizeof(UINT8));
	pInfo->_SendPacket.WriteData(pInfo->_pArg1,	sizeof(T_UID) * count);
	return true;
}

bool CGameEventHandler::__evtClan_DismissalMember( GAMEEVENTINFO * pInfo)
{
	UINT8 count = *(UINT8*) pInfo->_pArg;
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_DEPORTATION_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg,	sizeof(UINT8));
	pInfo->_SendPacket.WriteData(pInfo->_pArg1,	sizeof(T_UID) * count);
	return true;
}

bool CGameEventHandler::__evtClan_Dissolution( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_CLOSE_CLAN_REQ);
	return true;
}

bool CGameEventHandler::__evtClan_ModifyNotice( GAMEEVENTINFO * pInfo)
{
	const wchar_t* wszNotice = (const wchar_t*)pInfo->_pArg1;

	TTCHAR szNotice[NET_CLAN_INTRO_SIZE];

	i3::safe_string_copy(szNotice, wszNotice, NET_CLAN_INTRO_SIZE);

	UINT8 length = (UINT8)i3::generic_string_size(szNotice) + 1;

	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_REPLACE_NOTICE_REQ);
	pInfo->_SendPacket.WriteData(&length,	sizeof(UINT8));
	pInfo->_SendPacket.WriteData(szNotice,	length * sizeof(TTCHAR));
	return true;
}

bool CGameEventHandler::__evtClan_ModifyGuide( GAMEEVENTINFO * pInfo)
{
	const wchar_t* wszIntro = (const wchar_t*)pInfo->_pArg1;
	
	TTCHAR szIntro[NET_CLAN_INTRO_SIZE];

	i3::safe_string_copy(szIntro, wszIntro, NET_CLAN_INTRO_SIZE);

	UINT8 length = (UINT8)i3::generic_string_size(szIntro) + 1;
	
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_REPLACE_INTRO_REQ);
	pInfo->_SendPacket.WriteData(&length,	sizeof(UINT8));
	pInfo->_SendPacket.WriteData(szIntro,	length * sizeof(TTCHAR) );
	return true;
}

bool CGameEventHandler::__evtClan_ModifyAcceptOption( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(PACKET_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_REQ));
	return true;
}

bool CGameEventHandler::__evtClan_Menagement( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_REPLACE_MANAGEMENT_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg,	sizeof(CLAN_MANAGEMENT));
	return true;
}

bool CGameEventHandler::__evtClan_List( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_CLIENT_CLAN_LIST_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(UINT32));
	return true;
}

bool CGameEventHandler::__evtClan_CreationInfo( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_CREATE_CLAN_CONDITION_REQ);
	return true;
}

bool CGameEventHandler::__evtClan_Invite( GAMEEVENTINFO * pInfo)
{// revision 54949 유저찾기로 클랜초대 되지 않는 현상 수정(hansoft. 2929, 4388)

	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_INVITE_REQ);				

	
	//UINT8 cRoom = *(UINT8*)pInfo->_pArg1;
	/*
	pInfo->_SendPacket.WriteData(pInfo->_pArg1,sizeof(UINT8));

	if(0 == cRoom)
		pInfo->_SendPacket.WriteData(pInfo->_pArg,	sizeof(T_UID));		// UID
	else if(1 == cRoom)
		pInfo->_SendPacket.WriteData(pInfo->_pArg,	sizeof(INT32));		// SlotIdx
	else if(2 == cRoom)
		pInfo->_SendPacket.WriteData(pInfo->_pArg,	sizeof(INT32));		// SessionID
	else
	{		// Nickname
		UINT8 cSize = (UINT8)(*(UINT8*)pInfo->_pArg2);
		pInfo->_SendPacket.WriteData(pInfo->_pArg2,	sizeof(UINT8));
		pInfo->_SendPacket.WriteData(pInfo->_pArg, cSize);
	}*/
	return true;
}

bool CGameEventHandler::__evtClan_AcceptInvite( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_INVITE_ACCEPT_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg,	sizeof(UINT32));
	pInfo->_SendPacket.WriteData(pInfo->_pArg1,	sizeof(UINT32));
	pInfo->_SendPacket.WriteData(pInfo->_pArg2,	sizeof(UINT8));
	return true;
}

bool CGameEventHandler::__evtClan_Note( GAMEEVENTINFO * pInfo)
{
	UINT8 wideSize = (UINT8)(*(UINT8*) pInfo->_pArg1);
	const wchar_t* wide_str = (const wchar_t*) pInfo->_pArg2;
	
	i3::stack_wstring strData(wide_str, wideSize);

	UINT8 strSize = UINT8(strData.size()) + 1;

	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_NOTE_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg,  sizeof(UINT8));
	pInfo->_SendPacket.WriteData(&strSize, sizeof(UINT8));
	pInfo->_SendPacket.WriteData(strData.c_str(), sizeof(TTCHAR) * strSize);
	return true;
}

bool CGameEventHandler::__evtClan_DetailMemberInfo( GAMEEVENTINFO * pInfo)
{
	i3mem::FillZero( CCommunityContext::i()->SetCommnuitRecord(), sizeof(USER_INFO_RECORD));
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_MEMBER_DETAIL_INFO_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(T_UID));
	return true;
}

bool CGameEventHandler::__evtClan_MemberInfo( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_MEMBER_INFO_REQ);
	return true;
}

bool CGameEventHandler::__evtClan_MatchResultContext( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_CLAN_MATCH_RESULT_CONTEXT_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(PACKET_CS_CLAN_MATCH_RESULT_CONTEXT_REQ));
	return true;
}

bool CGameEventHandler::__evtClan_MatchResultList( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_CLAN_MATCH_RESULT_LIST_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(PACKET_CS_CLAN_MATCH_RESULT_LIST_REQ));
	return true;
}

bool CGameEventHandler::__evtClan_MatchResultEmotion( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_CLAN_MATCH_RESULT_EMOTION_SAVE_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(PACKET_CS_CLAN_MATCH_RESULT_EMOTION_SAVE_REQ));
	return true;
}

// 클랜전 이전 시즌 랭킹 정보 받음 체크
bool CGameEventHandler::__evtClanWar_PreSeasonRanking( GAMEEVENTINFO * pInfo)
{
	return true;
}

// 클랜전 내 클랜 매치팀 목록 받기
bool CGameEventHandler::__evtClanWar_GetTeamList( GAMEEVENTINFO * pInfo)
{
	return true;
}

// 클랜 매치 팀 생성
bool CGameEventHandler::__evtClanWar_CreateTeam( GAMEEVENTINFO * pInfo)
{
	PACKET_CLAN_WAR_CREATE_TEAM_REQ * pSendPacket = static_cast<PACKET_CLAN_WAR_CREATE_TEAM_REQ*>(pInfo->m_S2MOPacket);
	pSendPacket->m_ui8MaxPer = *(reinterpret_cast<const UINT8*>(pInfo->_pArg));

	return true;
}

// 클랜 매치 팀 입장
bool CGameEventHandler::__evtClanWar_JoinTeam( GAMEEVENTINFO * pInfo)
{
	PACKET_CLAN_WAR_JOIN_TEAM_REQ * pSendPacket = static_cast<PACKET_CLAN_WAR_JOIN_TEAM_REQ*>(pInfo->m_S2MOPacket);
	pSendPacket->m_TTeamID = *(reinterpret_cast<const T_TeamID*>(pInfo->_pArg));

	return true;
}

// 클랜 매치 팀 퇴장
bool CGameEventHandler::__evtClanWar_LeaveTeam( GAMEEVENTINFO * pInfo)
{
	return true;
}

// 클랜 매치 팀 작전명 변경
bool CGameEventHandler::__evtClanWar_ChangeOperation( GAMEEVENTINFO * pInfo)
{
	const i3::rc_wstring wstrOperation = *reinterpret_cast<const i3::rc_wstring*>(pInfo->_pArg);

	TTCHAR strOperation[NET_CLAN_OPERATION_SIZE];
	::memset(strOperation, 0, sizeof(strOperation));

	i3::safe_string_copy(strOperation, wstrOperation, NET_NICK_NAME_SIZE);

	PACKET_CLAN_WAR_CHANGE_OPERATION_REQ * pSendPacket = static_cast<PACKET_CLAN_WAR_CHANGE_OPERATION_REQ*>(pInfo->m_S2MOPacket);
	pSendPacket->m_strOperation = strOperation;

	return true;
}

// 클랜 매치 팀 최대 인원 수 변경
bool CGameEventHandler::__evtClanWar_ChangeMaxPerson( GAMEEVENTINFO * pInfo)
{
	PACKET_CLAN_WAR_CHANGE_MAX_PER_REQ * pSendPacket = static_cast<PACKET_CLAN_WAR_CHANGE_MAX_PER_REQ*>(pInfo->m_S2MOPacket);
	pSendPacket->m_ui8MaxMemberCount = *(reinterpret_cast<const UINT8*>(pInfo->_pArg));

	return true;
}

// 클랜전 팀 매치메이킹 요청
bool CGameEventHandler::__evtClanWar_RegisterMatchMaking( GAMEEVENTINFO * pInfo)
{
	return true;
}

// 클랜전 팀 메치메이킹 요청 취소
bool CGameEventHandler::__evtClanWar_RegisterCancelMatchMaking(GAMEEVENTINFO * pInfo)
{
	return true;
}

// 클랜전 용병 리스트 요청
bool CGameEventHandler::__evtClanWar_GetMercenaryList( GAMEEVENTINFO * pInfo)
{
	return true;
}

// 클랜전 용병 등록
bool CGameEventHandler::__evtClanWar_RegisterMercenary( GAMEEVENTINFO * pInfo)
{
	return true;
}

// 클랜전 용병 해제
bool CGameEventHandler::__evtClanWar_RemoveMercenary( GAMEEVENTINFO * pInfo)
{
	return true;
}

// 클랜전 용병 초대
bool CGameEventHandler::__evtClanWar_SendInviteMercenary( GAMEEVENTINFO * pInfo)
{
	PACKET_CLAN_WAR_INVITE_MERCENARY_REQ * pSendPacket = static_cast<PACKET_CLAN_WAR_INVITE_MERCENARY_REQ*>(pInfo->m_S2MOPacket);
	pSendPacket->m_TMerID = *(reinterpret_cast<const T_MerID*>(pInfo->_pArg));

	return true;
}

// 클랜전 용병 초대 수락
bool CGameEventHandler::__evtClanWar_AcceptInvite( GAMEEVENTINFO * pInfo)
{
	return true;
}

// 클랜전 용병 초대 거절
bool CGameEventHandler::__evtClanWar_DenialInvite( GAMEEVENTINFO * pInfo)
{
	return true;
}

// 클랜전 용병 상세정보 Req.
bool CGameEventHandler::__evtClanWar_MercenaryDetail(  GAMEEVENTINFO * pInfo )
{
	PACKET_CLAN_WAR_MERCENARY_DETAIL_INFO_REQ *pSendPacket = static_cast<PACKET_CLAN_WAR_MERCENARY_DETAIL_INFO_REQ*>(pInfo->m_S2MOPacket);
	pSendPacket->m_TMerID = *(reinterpret_cast<const T_MerID*>(pInfo->_pArg));

	return true;
}

bool CGameEventHandler::__evtClanWar_Result(GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CLAN_WAR_RESULT_REQ);
	return true; 
}