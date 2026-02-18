#include "pch.h"
#include "ClientTCPSocket.h"
#include "CommunityContext.h"

#include "UI/UIUtil.h"
#include "UI/UIMainFrame.h"

#include "ClanGameContext.h"
#include "UserInfoContext.h"

#include "i3Base/string/ext/mb_to_utf16.h"

namespace
{
	const char * getProtocolName( PROTOCOL protocol)
	{
		switch( protocol)
		{
		case PROTOCOL_CS_CLIENT_ENTER_ACK : return "PROTOCOL_CS_CLIENT_ENTER_ACK";
		case PROTOCOL_CS_CLIENT_LEAVE_ACK : return "PROTOCOL_CS_CLIENT_LEAVE_ACK";
		case PROTOCOL_CS_CLIENT_CLAN_CONTEXT_ACK : return "PROTOCOL_CS_CLIENT_CLAN_CONTEXT_ACK";
		case PROTOCOL_CS_CLIENT_CLAN_LIST_ACK : return "PROTOCOL_CS_CLIENT_CLAN_LIST_ACK";
		case PROTOCOL_CS_MEMBER_INFO_ACK : return "PROTOCOL_CS_MEMBER_INFO_ACK";
		case PROTOCOL_CS_MEMBER_INFO_CHANGE_ACK : return "PROTOCOL_CS_MEMBER_INFO_CHANGE_ACK";
		case PROTOCOL_CS_MEMBER_INFO_INSERT_ACK : return "PROTOCOL_CS_MEMBER_INFO_INSERT_ACK";
		case PROTOCOL_CS_MEMBER_INFO_DELETE_ACK : return "PROTOCOL_CS_MEMBER_INFO_DELETE_ACK";
		case PROTOCOL_CS_CREATE_CLAN_ACK : return "PROTOCOL_CS_CREATE_CLAN_ACK";
		case PROTOCOL_CS_CHECK_DUPLICATE_ACK : return "PROTOCOL_CS_CHECK_DUPLICATE_ACK";
		case PROTOCOL_CS_CHECK_DUPLICATE_AZIT_URL_ACK : return "PROTOCOL_CS_CHECK_DUPLICATE_AZIT_URL_ACK";
		case PROTOCOL_CS_CHECK_MARK_ACK : return "PROTOCOL_CS_CHECK_MARK_ACK";
		case PROTOCOL_CS_REPLACE_MARK_ACK : return "PROTOCOL_CS_REPLACE_MARK_ACK";
		case PROTOCOL_CS_REPLACE_PERSONMAX_ACK : return "PROTOCOL_CS_REPLACE_PERSONMAX_ACK";
		case PROTOCOL_CS_REPLACE_MANAGEMENT_ACK : return "PROTOCOL_CS_REPLACE_MANAGEMENT_ACK";
		case PROTOCOL_CS_JOIN_REQUEST_ACK : return "PROTOCOL_CS_JOIN_REQUEST_ACK";
		case PROTOCOL_CS_NOTE_ACK : return "PROTOCOL_CS_NOTE_ACK";
		case PROTOCOL_CS_CHECK_JOIN_AUTHORITY_ACK : return "PROTOCOL_CS_CHECK_JOIN_AUTHORITY_ACK";
		case PROTOCOL_CS_CANCEL_REQUEST_ACK : return "PROTOCOL_CS_CANCEL_REQUEST_ACK";
		case PROTOCOL_CS_CLAN_LIST_DETAIL_INFO_ACK : return "PROTOCOL_CS_CLAN_LIST_DETAIL_INFO_ACK";
		case PROTOCOL_CS_DETAIL_INFO_ACK : return "PROTOCOL_CS_DETAIL_INFO_ACK";
		case PROTOCOL_CS_MEMBER_CONTEXT_ACK : return "PROTOCOL_CS_MEMBER_CONTEXT_ACK";
		case PROTOCOL_CS_MEMBER_LIST_ACK : return "PROTOCOL_CS_MEMBER_LIST_ACK";
		case PROTOCOL_CS_REQUEST_CONTEXT_ACK : return "PROTOCOL_CS_REQUEST_CONTEXT_ACK";
		case PROTOCOL_CS_REQUEST_LIST_ACK : return "PROTOCOL_CS_REQUEST_LIST_ACK";
		case PROTOCOL_CS_REQUEST_INFO_ACK : return "PROTOCOL_CS_REQUEST_INFO_ACK";
		case PROTOCOL_CS_ACCEPT_REQUEST_ACK : return "PROTOCOL_CS_ACCEPT_REQUEST_ACK";
		case PROTOCOL_CS_DENIAL_REQUEST_ACK : return "PROTOCOL_CS_DENIAL_REQUEST_ACK";
		case PROTOCOL_CS_INVITE_ACK : return "PROTOCOL_CS_INVITE_ACK";
		case PROTOCOL_CS_INVITE_ACCEPT_ACK : return "PROTOCOL_CS_INVITE_ACCEPT_ACK";
		case PROTOCOL_CS_SECESSION_CLAN_ACK : return "PROTOCOL_CS_SECESSION_CLAN_ACK";
		case PROTOCOL_CS_COMMISSION_MASTER_ACK : return "PROTOCOL_CS_COMMISSION_MASTER_ACK";
		case PROTOCOL_CS_COMMISSION_STAFF_ACK : return "PROTOCOL_CS_COMMISSION_STAFF_ACK";
		case PROTOCOL_CS_COMMISSION_REGULAR_ACK : return "PROTOCOL_CS_COMMISSION_REGULAR_ACK";
		case PROTOCOL_CS_DEPORTATION_ACK : return "PROTOCOL_CS_DEPORTATION_ACK";
		case PROTOCOL_CS_ACCEPT_REQUEST_RESULT_ACK : return "PROTOCOL_CS_ACCEPT_REQUEST_RESULT_ACK";
		case PROTOCOL_CS_DENIAL_REQUEST_RESULT_ACK : return "PROTOCOL_CS_DENIAL_REQUEST_RESULT_ACK";
		case PROTOCOL_CS_DEPORTATION_RESULT_ACK : return "PROTOCOL_CS_DEPORTATION_RESULT_ACK";
		case PROTOCOL_CS_COMMISSION_MASTER_RESULT_ACK : return "PROTOCOL_CS_COMMISSION_MASTER_RESULT_ACK";
		case PROTOCOL_CS_COMMISSION_STAFF_RESULT_ACK : return "PROTOCOL_CS_COMMISSION_STAFF_RESULT_ACK";
		case PROTOCOL_CS_COMMISSION_REGULAR_RESULT_ACK : return "PROTOCOL_CS_COMMISSION_REGULAR_RESULT_ACK";
		case PROTOCOL_CS_CHANGE_CLAN_EXP_RANK_ACK : return "PROTOCOL_CS_CHANGE_CLAN_EXP_RANK_ACK";
		case PROTOCOL_CS_CHANGE_CLAN_PER_RANK_ACK : return "PROTOCOL_CS_CHANGE_CLAN_PER_RANK_ACK";
		case PROTOCOL_CS_REPLACE_MARK_RESULT_ACK : return "PROTOCOL_CS_REPLACE_MARK_RESULT_ACK";
		case PROTOCOL_CS_REPLACE_MANAGEMENT_RESULT_ACK: return "PROTOCOL_CS_REPLACE_MANAGEMENT_RESULT_ACK";
		case PROTOCOL_CS_REPLACE_NAME_RESULT_ACK : return "PROTOCOL_CS_REPLACE_NAME_RESULT_ACK";
		case PROTOCOL_CS_ROOM_INVITED_ACK : return "PROTOCOL_CS_ROOM_INVITED_ACK";
		case PROTOCOL_CS_ROOM_INVITED_RESULT_ACK : return "PROTOCOL_CS_ROOM_INVITED_RESULT_ACK";
		case PROTOCOL_CS_CLOSE_CLAN_ACK : return "PROTOCOL_CS_CLOSE_CLAN_ACK";
		case PROTOCOL_CS_CHATTING_ACK : return "PROTOCOL_CS_CHATTING_ACK";
		case PROTOCOL_CS_PAGE_CHATTING_ACK : return "PROTOCOL_CS_PAGE_CHATTING_ACK";
		case PROTOCOL_CS_MEMBER_DETAIL_INFO_ACK : return "PROTOCOL_CS_MEMBER_DETAIL_INFO_ACK";
		case PROTOCOL_CS_REPLACE_MEMBER_BASIC_RESULT_ACK : return "PROTOCOL_CS_REPLACE_MEMBER_BASIC_RESULT_ACK";
		case PROTOCOL_CS_REPLACE_NOTICE_ACK : return "PROTOCOL_CS_REPLACE_NOTICE_ACK";
		case PROTOCOL_CS_REPLACE_INTRO_ACK : return "PROTOCOL_CS_REPLACE_INTRO_ACK";
		case PROTOCOL_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_ACK : return "PROTOCOL_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_ACK";
		case PROTOCOL_CS_RECORD_RESET_RESULT_ACK : return "PROTOCOL_CS_RECORD_RESET_RESULT_ACK";
		case PROTOCOL_CS_POINT_RESET_RESULT_ACK : return "PROTOCOL_CS_POINT_RESET_RESULT_ACK";
		case PROTOCOL_CS_REPLACE_COLOR_NAME_RESULT_ACK : return "PROTOCOL_CS_REPLACE_COLOR_NAME_RESULT_ACK";
		case PROTOCOL_CS_CREATE_CLAN_CONDITION_ACK : return "PROTOCOL_CS_CREATE_CLAN_CONDITION_ACK";
		case PROTOCOL_CS_CLAN_MATCH_RESULT_CONTEXT_ACK : return "PROTOCOL_CS_CLAN_MATCH_RESULT_CONTEXT_ACK";
		case PROTOCOL_CS_CLAN_MATCH_RESULT_LIST_ACK : return "PROTOCOL_CS_CLAN_MATCH_RESULT_LIST_ACK";
		case PROTOCOL_CS_CLAN_MATCH_RESULT_EMOTION_SAVE_ACK : return "PROTOCOL_CS_CLAN_MATCH_RESULT_EMOTION_SAVE_ACK";
		default :
			I3PRINTLOG(I3LOG_FATAL,  "PROTOCOL_CLAN_XXX 프로토콜 이름 등록해주이소~");
			return "PROTOCOL_CLAN_UNKNOWN";
		}
	}
}

void ClientTCPSocket::_PacketParsingClan(i3NetworkPacket  * pPacket)
{	
	i3::unordered_map< UINT32, netFunc >::iterator it;
	it = m_NetFuncMap_Clan.find( pPacket->GetProtocolID());

	if ( it != m_NetFuncMap_Clan.end() )
	{
		I3PRINTLOG(I3LOG_QA, "* %s -->", getProtocolName( pPacket->GetProtocolID()));
		netFunc & mf = it->second;
		(this->*mf)(pPacket);
		I3PRINTLOG(I3LOG_QA, "* %s <--", getProtocolName( pPacket->GetProtocolID()));
	}
	else
		__Parse_UnknownProtocol( pPacket);
}

void ClientTCPSocket::__Register_Clan_Function( void)
{
	RegisterParser( PROTOCOL_CS_CLIENT_ENTER_ACK,					&ClientTCPSocket::__Parse_Clan_Enter);
	RegisterParser( PROTOCOL_CS_CLIENT_LEAVE_ACK,					&ClientTCPSocket::__Parse_Clan_Leave);
	RegisterParser( PROTOCOL_CS_CLIENT_CLAN_CONTEXT_ACK,			&ClientTCPSocket::__Parse_Clan_ContextInfo);
	RegisterParser( PROTOCOL_CS_CLIENT_CLAN_LIST_ACK,				&ClientTCPSocket::__Parse_Clan_ClanList);
	RegisterParser( PROTOCOL_CS_MEMBER_INFO_ACK,					&ClientTCPSocket::__Parse_Clan_MemberInfo);
	RegisterParser( PROTOCOL_CS_MEMBER_INFO_CHANGE_ACK,				&ClientTCPSocket::__Parse_Clan_ChangeMember);
	RegisterParser( PROTOCOL_CS_MEMBER_INFO_INSERT_ACK,				&ClientTCPSocket::__Parse_Clan_AddMember);
	RegisterParser( PROTOCOL_CS_MEMBER_INFO_DELETE_ACK,				&ClientTCPSocket::__Parse_Clan_DeleteMember);
	RegisterParser( PROTOCOL_CS_CREATE_CLAN_ACK,					&ClientTCPSocket::__Parse_Clan_Create);
	RegisterParser( PROTOCOL_CS_CHECK_DUPLICATE_ACK,				&ClientTCPSocket::__Parse_Clan_DuplicateName);
	RegisterParser( PROTOCOL_CS_CHECK_DUPLICATE_AZIT_URL_ACK,		&ClientTCPSocket::__Parse_Clan_DuplicateAzitURL);
	RegisterParser( PROTOCOL_CS_CHECK_MARK_ACK,						&ClientTCPSocket::__Parse_Clan_DuplicateMark);
	RegisterParser( PROTOCOL_CS_REPLACE_MARK_ACK,					&ClientTCPSocket::__Parse_Clan_ReplaceMark);
	RegisterParser( PROTOCOL_CS_REPLACE_PERSONMAX_ACK,				&ClientTCPSocket::__Parse_Clan_ChangePersonMax);
	RegisterParser( PROTOCOL_CS_REPLACE_MANAGEMENT_ACK,				&ClientTCPSocket::__Parse_Clan_Management);
	RegisterParser( PROTOCOL_CS_JOIN_REQUEST_ACK,					&ClientTCPSocket::__Parse_Clan_RequestJoin);
	RegisterParser( PROTOCOL_CS_NOTE_ACK,							&ClientTCPSocket::__Parse_Clan_Note);
	RegisterParser( PROTOCOL_CS_CHECK_JOIN_AUTHORITY_ACK,			&ClientTCPSocket::__Parse_Clan_JoinAuthoriy);
	RegisterParser( PROTOCOL_CS_CANCEL_REQUEST_ACK,					&ClientTCPSocket::__Parse_Clan_CancelRequest);
	RegisterParser( PROTOCOL_CS_CLAN_LIST_DETAIL_INFO_ACK,			&ClientTCPSocket::__Parse_Clan_ListDetailInfo);
	RegisterParser( PROTOCOL_CS_DETAIL_INFO_ACK,					&ClientTCPSocket::__Parse_Clan_DetailInfo);
	RegisterParser( PROTOCOL_CS_MEMBER_CONTEXT_ACK,					&ClientTCPSocket::__Parse_Clan_MemberContext);
	RegisterParser( PROTOCOL_CS_MEMBER_LIST_ACK,					&ClientTCPSocket::__Parse_Clan_MemberList);
	RegisterParser( PROTOCOL_CS_REQUEST_CONTEXT_ACK,				&ClientTCPSocket::__Parse_Clan_RequestContext);
	RegisterParser( PROTOCOL_CS_REQUEST_LIST_ACK,					&ClientTCPSocket::__Parse_Clan_RequestList);
	RegisterParser( PROTOCOL_CS_REQUEST_INFO_ACK,					&ClientTCPSocket::__Parse_Clan_RequestInfo);
	RegisterParser( PROTOCOL_CS_ACCEPT_REQUEST_ACK,					&ClientTCPSocket::__Parse_Clan_AcceptRequest);
	RegisterParser( PROTOCOL_CS_DENIAL_REQUEST_ACK,					&ClientTCPSocket::__Parse_Clan_DenineRequest);
	RegisterParser( PROTOCOL_CS_INVITE_ACK,							&ClientTCPSocket::__Parse_Clan_Invite);
	RegisterParser( PROTOCOL_CS_INVITE_ACCEPT_ACK,					&ClientTCPSocket::__Parse_Clan_AcceptInvite);
	RegisterParser( PROTOCOL_CS_SECESSION_CLAN_ACK,					&ClientTCPSocket::__Parse_Clan_Session);
	RegisterParser( PROTOCOL_CS_COMMISSION_MASTER_ACK,				&ClientTCPSocket::__Parse_Clan_Commission_Master);
	RegisterParser( PROTOCOL_CS_COMMISSION_STAFF_ACK,				&ClientTCPSocket::__Parse_Clan_Commission_Staff);
	RegisterParser( PROTOCOL_CS_COMMISSION_REGULAR_ACK,				&ClientTCPSocket::__Parse_Clan_Commission_Regular);
	RegisterParser( PROTOCOL_CS_DEPORTATION_ACK,					&ClientTCPSocket::__Parse_Clan_Deportation);
	RegisterParser( PROTOCOL_CS_ACCEPT_REQUEST_RESULT_ACK,			&ClientTCPSocket::__Parse_Clan_AcceptRequestResult);
	RegisterParser( PROTOCOL_CS_DENIAL_REQUEST_RESULT_ACK,			&ClientTCPSocket::__Parse_Clan_DenialRequestResult);
	RegisterParser( PROTOCOL_CS_DEPORTATION_RESULT_ACK,				&ClientTCPSocket::__Parse_Clan_DeportationResult);
	RegisterParser( PROTOCOL_CS_COMMISSION_MASTER_RESULT_ACK,		&ClientTCPSocket::__Parse_Clan_Commission_MasterResult);
	RegisterParser( PROTOCOL_CS_COMMISSION_STAFF_RESULT_ACK,		&ClientTCPSocket::__Parse_Clan_Commission_StaffResult);
	RegisterParser( PROTOCOL_CS_COMMISSION_REGULAR_RESULT_ACK,		&ClientTCPSocket::__Parse_Clan_Commission_RegularResult);
	RegisterParser( PROTOCOL_CS_CHANGE_CLAN_EXP_RANK_ACK,			&ClientTCPSocket::__Parse_Clan_ChangeExpRank);
	RegisterParser( PROTOCOL_CS_CHANGE_CLAN_PER_RANK_ACK,			&ClientTCPSocket::__Parse_Clan_ChangePerRank);
	RegisterParser( PROTOCOL_CS_REPLACE_MARK_RESULT_ACK,			&ClientTCPSocket::__Parse_Clan_ChangeMarkResult);
	RegisterParser( PROTOCOL_CS_REPLACE_MANAGEMENT_RESULT_ACK,		&ClientTCPSocket::__Parse_Clan_ChangeManagementResult);
	RegisterParser( PROTOCOL_CS_REPLACE_NAME_RESULT_ACK,			&ClientTCPSocket::__Parse_Clan_ChangeName);
	RegisterParser( PROTOCOL_CS_ROOM_INVITED_ACK,					&ClientTCPSocket::__Parse_Clan_InviteRoom);
	RegisterParser( PROTOCOL_CS_ROOM_INVITED_RESULT_ACK,			&ClientTCPSocket::__Parse_Clan_InviteRoomResult);
	RegisterParser( PROTOCOL_CS_CLOSE_CLAN_ACK,						&ClientTCPSocket::__Parse_Clan_Close);
	RegisterParser( PROTOCOL_CS_CHATTING_ACK,						&ClientTCPSocket::__Parse_Clan_Chat);
	RegisterParser( PROTOCOL_CS_PAGE_CHATTING_ACK,					&ClientTCPSocket::__Parse_Clan_ChatAtMemberPage);
	RegisterParser( PROTOCOL_CS_MEMBER_DETAIL_INFO_ACK,				&ClientTCPSocket::__Parse_Clan_MemberDetailInfo);
	RegisterParser( PROTOCOL_CS_REPLACE_MEMBER_BASIC_RESULT_ACK,	&ClientTCPSocket::__Parse_Clan_ChangeMemberBasicInfoResult);
	RegisterParser( PROTOCOL_CS_REPLACE_NOTICE_ACK,					&ClientTCPSocket::__Parse_Clan_ChangeNotice);
	RegisterParser( PROTOCOL_CS_REPLACE_INTRO_ACK,					&ClientTCPSocket::__Parse_Clan_ChangeIntro);
	RegisterParser( PROTOCOL_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_ACK,&ClientTCPSocket::__Parse_Clan_ChangeAcceptOption);
	RegisterParser( PROTOCOL_CS_RECORD_RESET_RESULT_ACK,			&ClientTCPSocket::__Parse_Clan_ResetRecordResult);
	RegisterParser( PROTOCOL_CS_POINT_RESET_RESULT_ACK,				&ClientTCPSocket::__Parse_Clan_ResetPointResult);
	RegisterParser( PROTOCOL_CS_REPLACE_COLOR_NAME_RESULT_ACK,		&ClientTCPSocket::__Parse_Clan_ChangeColorNameResult);
	RegisterParser( PROTOCOL_CS_CREATE_CLAN_CONDITION_ACK,			&ClientTCPSocket::__Parse_Clan_CheckConditionForCreation);
	RegisterParser( PROTOCOL_CS_CLAN_MATCH_RESULT_CONTEXT_ACK,		&ClientTCPSocket::__Parse_Clan_MatchResultContext);
	RegisterParser( PROTOCOL_CS_CLAN_MATCH_RESULT_LIST_ACK,			&ClientTCPSocket::__Parse_Clan_MatchResultList);
	RegisterParser( PROTOCOL_CS_CLAN_MATCH_RESULT_EMOTION_SAVE_ACK, &ClientTCPSocket::__Parse_Clan_MatchResultEmotion);

}

void ClientTCPSocket::__Parse_Clan_Enter( i3NetworkPacket * pPacket)
{
	USER_INFO_BASIC		MyUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);

	pPacket->ReadData( &MyUserInfoBasic.m_ui32Clanidx,	sizeof(UINT32) );
	pPacket->ReadData( &MyUserInfoBasic.m_ui32ClanState,	sizeof(UINT32) );

	UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);

	if( (MyUserInfoBasic.m_ui32Clanidx != 0) && (MyUserInfoBasic.m_ui32ClanState != 0) )
	{//클랜이 있음
		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_ENTER, 1 );
	}
	else
	{//클랜이 없음 
		CLAN_CONTEXT ClanContext; 
		CLAN_CONTEXT * pClanContext = ClanGameContext::i()->getClanContext(); 
		pPacket->ReadData( &ClanContext, sizeof(CLAN_CONTEXT));	//클랜 컨텍스트를 받는다. 

		if( ClanContext._version != pClanContext->_version )
		{
			i3mem::Copy( pClanContext, &ClanContext, sizeof(CLAN_CONTEXT));		//Set ClanContext 

			//Create ClanList Buffer 
			ClanGameContext::i()->DeleteClanList(); 
			if( pClanContext->_sectionCount > 0)
			{
				ClanGameContext::i()->CreateClanList( pClanContext->_sectionSize * pClanContext->_sectionCount );
			}
		}

		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_ENTER, 0 );
	}
}

void ClientTCPSocket::__Parse_Clan_Leave( i3NetworkPacket * pPacket)
{
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_LEAVE );
}

void ClientTCPSocket::__Parse_Clan_ContextInfo( i3NetworkPacket * pPacket)
{
	CLAN_CONTEXT ClanContext; 
	CLAN_CONTEXT * pClanContext = ClanGameContext::i()->getClanContext(); 
	pPacket->ReadData( &ClanContext, sizeof(CLAN_CONTEXT));	//클랜 컨텍스트를 받는다. 

	if( ClanContext._version != pClanContext->_version )
	{
		i3mem::Copy( pClanContext, &ClanContext, sizeof(CLAN_CONTEXT));		//Set ClanContext 

		//Create ClanList Buffer 
		ClanGameContext::i()->DeleteClanList(); 
		if( pClanContext->_sectionCount > 0)
		{
			ClanGameContext::i()->CreateClanList( pClanContext->_sectionSize * pClanContext->_sectionCount );
		}
	}

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_LIST_CONTEXT, 0 );
}

void ClientTCPSocket::__Parse_Clan_ClanList( i3NetworkPacket * pPacket)
{
	//UINT32 ui32SectionIdx;	//section
	UINT32	begClanIdx;
	pPacket->ReadData(&begClanIdx,	sizeof(UINT32));	

	UINT8   numClanToRead;
	pPacket->ReadData(&numClanToRead,	sizeof(UINT8) );   

	
	CLAN_CONTEXT		 *	pClanContext	= ClanGameContext::i()->getClanContext();
	CLAN_LIST_BASIC_INFO *	pClanList		= ClanGameContext::i()->getClanList();
	if(pClanList == nullptr)
	{
		if(pClanContext->_sectionCount > 0)
		{
			pClanList = ClanGameContext::i()->CreateClanList( pClanContext->_sectionSize * pClanContext->_sectionCount );
		}
		I3ASSERT_RETURN(pClanList);
	}

	I3ASSERT_RETURN((begClanIdx + UINT32(numClanToRead)) <= (UINT32(pClanContext->_sectionSize) * UINT32(pClanContext->_sectionCount)) );

//	if(pClanContext->_totalCount > (ui32SectionIdx * pClanContext->_sectionSize))
//	{
//		UINT8 uiSectionSize;
//		pPacket->ReadData(&uiSectionSize,									sizeof(UINT8) );   

		CLAN_LIST_BASIC_INFO* pInfo = &pClanList[begClanIdx];
		const std::size_t num_info_to_read = numClanToRead;
		UINT16 numCharToRead;
		
		for (size_t i = 0; i < num_info_to_read ; ++i, ++pInfo )
		{
			pPacket->ReadData(&pInfo->m_ClanBasicInfo, sizeof(CLAN_BASIC_INFO));			
			pPacket->ReadData(&pInfo->m_ClanBasicRecord, sizeof(CLAN_BASIC_RECORD));
			pPacket->ReadData(&pInfo->m_ClanBsicSortInfo, sizeof(CLAN_BASIC_SORT_INFO));

			pPacket->ReadData(&numCharToRead, sizeof(UINT16));
			pPacket->ReadData(pInfo->m_intro, sizeof(TTCHAR) * numCharToRead);
			pInfo->m_intro[numCharToRead] = 0;
		}

//		pPacket->ReadData(&pClanList[ui32SectionIdx * pClanContext->_sectionSize], (sizeof(CLAN_LIST_BASIC_INFO) * uiSectionSize) );  

		g_pFramework->GetClanContext()->SetUpdateClanListAck_BeginClanIdx( (INT32) begClanIdx);
		g_pFramework->GetClanContext()->SetUpdateClanListAck_CurrRecvClanCount( (INT32) numClanToRead);

//	}

}


void ClientTCPSocket::__Parse_Clan_MemberInfo( i3NetworkPacket * pPacket)
{
	// 전체개수를 받고
	UINT8 count;
	pPacket->ReadData(&count, sizeof(UINT8));

	if (count > MAX_MEMBER_COUNT)
	{
		I3TRACE("[ERROR] PROTOCOL_CS_MEMBER_INFO_ACK - Invalid friend count(%d)\n", count);
		return;
	}

//	CCommunityContext::i()->ClearClanMember();	// (1)

	// 개수만큼 돌면서
	for(UINT32 i = 0; i < count; i++)			
	{	
		// 목록을 읽어 기록한다
		CLAN_MEMBER_NODE node;

		_ReadClanMemberInfoToNode( pPacket, &node );
		CCommunityContext::i()->AddClanMember(&node); // (2)
	}
}

void ClientTCPSocket::__Parse_Clan_ChangeMember( i3NetworkPacket * pPacket)
{
	INT64 i64UID;
    UINT64 ui64State;

	pPacket->ReadData(&i64UID,		sizeof(INT64));
	pPacket->ReadData(&ui64State,	sizeof(UINT64)); 

	CCommunityContext::i()->ChangeClanMember( i64UID, ui64State );
	g_pFramework->GetClanContext()->SetMemberInfoChange( i64UID, ui64State );

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_UPDATE_USER_LIST);
}

void ClientTCPSocket::__Parse_Clan_AddMember( i3NetworkPacket * pPacket)
{
	CLAN_MEMBER_NODE node;

	_ReadClanMemberInfoToNode( pPacket, &node );
	CCommunityContext::i()->AddClanMember(&node); // (2)

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_MEMBER_INSERT);
}

void ClientTCPSocket::__Parse_Clan_DeleteMember( i3NetworkPacket * pPacket)
{
	INT64 i64UID;

	pPacket->ReadData(&i64UID, sizeof(INT64));
	CCommunityContext::i()->DeleteClamMember(i64UID);
}

void ClientTCPSocket::__Parse_Clan_Create( i3NetworkPacket * pPacket)
{
	PACKET_H_CLAN_ACK PacketHeader;
	pPacket->ReadData(&PacketHeader,	sizeof(INT32));
	
	if(PacketHeader.m_i32Result > -1)
	{
		PACKET_D_CS_NEW_CREATE_CLAN Info;

		pPacket->ReadData(&Info, sizeof(PACKET_D_CS_NEW_CREATE_CLAN));
		USER_INFO_BASIC	MyUserInfoBasic;
		UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
		MyUserInfoBasic.m_ui32Clanidx		= Info.m_ClanDetailInfo.m_ui32CID;
		MyUserInfoBasic.m_ui32ClanState		= CLAN_MEMBER_LEVEL_MASTER;
		MyUserInfoBasic.m_ui8ClanExpRank	= Info.m_ClanDetailInfo.m_expRank;
		MyUserInfoBasic.m_ui32ClanMark		= Info.m_ClanDetailInfo.m_mark;
		MyUserInfoBasic.m_ui8ClanPerRank	= Info.m_ClanDetailInfo.m_personRank;
		MyUserInfoBasic.m_ui8ClanNamecolor	= Info.m_ClanDetailInfo.m_color;	

		i3::safe_string_copy( MyUserInfoBasic.m_strClanName, Info.m_ClanDetailInfo.m_name, NET_CLAN_NAME_SIZE );

		MyUserInfoBasic.m_ui32Point = Info.m_UserPoint;

		UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);
		i3mem::Copy(ClanGameContext::i()->getMyClanDetailInfo(), &Info.m_ClanDetailInfo, sizeof(CLAN_DETAIL_INFO));

	}
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_CREATE, PacketHeader.m_i32Result );
}

void ClientTCPSocket::__Parse_Clan_DuplicateName( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_DUPLICATE_NAME, Temp );
}

void ClientTCPSocket::__Parse_Clan_DuplicateAzitURL( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_DUPLICATE_ADDRESS, Temp );
}

void ClientTCPSocket::__Parse_Clan_DuplicateMark( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_DUPLICATE_MARK, Temp );
}

void ClientTCPSocket::__Parse_Clan_ReplaceMark( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_CHANGE_MARK, Temp );
}

void ClientTCPSocket::__Parse_Clan_ChangePersonMax( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));

	if( EV_SUCCESSED(Temp) )
	{
		pPacket->ReadData(&ClanGameContext::i()->getMyClanDetailInfo()->m_maxPerson,	sizeof(UINT8));
	}

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_PERSON_PLUS, Temp);
}

void ClientTCPSocket::__Parse_Clan_Management( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	
	pPacket->ReadData(&Temp,	sizeof(INT32));
 	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_MANAGEMENT, Temp );
}

void ClientTCPSocket::__Parse_Clan_RequestJoin( i3NetworkPacket * pPacket)
{
	INT32 Temp;

	pPacket->ReadData(&Temp,	sizeof(INT32));
	if(Temp > -1)
	{
		pPacket->ReadData(&Temp,	sizeof(INT32));

		USER_INFO_BASIC MyUserInfoBasic;
		UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
		MyUserInfoBasic.m_ui32Clanidx	= Temp;
		MyUserInfoBasic.m_ui32ClanState	= 0;
		UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);
	}

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_JOIN_REQUEST, Temp );
}

void ClientTCPSocket::__Parse_Clan_Note( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_NOTE, Temp );
}

void ClientTCPSocket::__Parse_Clan_JoinAuthoriy( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_JOINCHECK_AUTHORITY, Temp );
}

void ClientTCPSocket::__Parse_Clan_CancelRequest( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_CANCEL_REQUEST, Temp );
}

void ClientTCPSocket::__Parse_Clan_ListDetailInfo( i3NetworkPacket * pPacket)
{
	PACKET_H_CLAN_ACK PacketHeader;
	pPacket->ReadData(&PacketHeader,	sizeof(INT32));

	if(PacketHeader.m_i32Result > -1)
	{
		PACKET_D_CS_NEW_CLAN_LIST_DETAIL_INFO ClanDetailInfo;

		pPacket->ReadData(&ClanDetailInfo, sizeof(PACKET_D_CS_NEW_CLAN_LIST_DETAIL_INFO));
		i3mem::Copy(ClanGameContext::i()->getClanDetailInfo() , &ClanDetailInfo.m_ClanDetailInfo, sizeof(CLAN_DETAIL_INFO));

		// 클랜 목록에 해당 클랜 정보도 업데이트한다.
		CLAN_LIST_BASIC_INFO * clanList = ClanGameContext::i()->getClanList();

		if (clanList)
		{
			CLAN_CONTEXT* clanContext = ClanGameContext::i()->getClanContext();

			for(UINT32 i = 0 ; i < clanContext->_totalCount; i++)
			{
				if (ClanDetailInfo.m_ClanDetailInfo.m_ui32CID == clanList[i].m_ClanBasicInfo._idx)
				{
					clanList[i].m_ClanBasicInfo._person = ClanDetailInfo.m_ClanDetailInfo.m_currentPerson;
					clanList[i].m_ClanBasicInfo._rank = ClanDetailInfo.m_ClanDetailInfo.m_expRank;
					break;
				}

			}
		}
	}

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_LIST_DETAIL_INFO, PacketHeader.m_i32Result );
}

void ClientTCPSocket::__Parse_Clan_DetailInfo( i3NetworkPacket * pPacket)
{
	PACKET_H_CLAN_ACK PacketHeader;

	pPacket->ReadData(&PacketHeader,	sizeof(PACKET_H_CLAN_ACK));
	if(EV_SUCCESSED(PacketHeader.m_i32Result))
	{
		CLAN_DETAIL_INFO_ALL * pClanDetailInfoAll;

		// 자기 클랜 페이지를 들어가는 경우에는 MyClanDetailInfoAll()
		// 다른 클랜에 방문자로 들어가는 경우에는 ClanDetailInfoAll() 에 설정
		if(g_pFramework->GetClanContext()->IsClanVisit() == false)
		{
			pClanDetailInfoAll = ClanGameContext::i()->setMyClanDetailInfoAll();
		}
		else
		{
			pClanDetailInfoAll = ClanGameContext::i()->setClanDetailInfoAll();
		}

		pPacket->ReadData(pClanDetailInfoAll, sizeof(CLAN_DETAIL_INFO_ALL));

		// 클랜 목록에 해당 클랜 정보도 업데이트한다.
		CLAN_LIST_BASIC_INFO * clanList = ClanGameContext::i()->getClanList();

		if (clanList)
		{
			CLAN_CONTEXT* clanContext = ClanGameContext::i()->getClanContext();

			for(UINT32 i = 0 ; i < clanContext->_totalCount; i++)
			{
				if (pClanDetailInfoAll->m_ClanDetailInfo.m_ui32CID == clanList[i].m_ClanBasicInfo._idx)
				{
					clanList[i].m_ClanBasicInfo._person = pClanDetailInfoAll->m_ClanDetailInfo.m_currentPerson;
					clanList[i].m_ClanBasicInfo._rank = pClanDetailInfoAll->m_ClanDetailInfo.m_expRank;
					break;
				}
			}
		}
	}

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_DETAIL_INFO, PacketHeader.m_i32Result );
}

void ClientTCPSocket::__Parse_Clan_MemberContext( i3NetworkPacket * pPacket)
{
	INT32 Temp;

	pPacket->ReadData(&Temp,	sizeof(INT32));
	if(Temp > -1)
	{
		CLAN_MEMBER_CONTEXT * pClanMemberContext = ClanGameContext::i()->getMyClanMemberContext(); 
		pPacket->ReadData(pClanMemberContext, sizeof(CLAN_MEMBER_CONTEXT)); 
		ClanGameContext::i()->DeleteMyClanMemberList();
		if(pClanMemberContext->_sectionCount > 0)
		{
			INT32 MemberCount = pClanMemberContext->_sectionSize * pClanMemberContext->_sectionCount;
			ClanGameContext::i()->CreateMyClanMemberList(MemberCount);
		}
	}
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_MEMBER_CONTEXT, Temp );
}

void ClientTCPSocket::__Parse_Clan_MemberList( i3NetworkPacket * pPacket)
{
	INT32 Temp;

	pPacket->ReadData(&Temp,	sizeof(INT32));
	if(Temp > -1)
	{
		UINT8 section;
		UINT8 count;

		pPacket->ReadData(&section,	sizeof(UINT8));
		pPacket->ReadData(&count,	sizeof(UINT8));

		CLAN_MEMBER_CONTEXT		* pClanMemberContext	= ClanGameContext::i()->getMyClanMemberContext();
		INT32 ClanMemberMaxCount		= ClanGameContext::i()->GetMyClanMemberMaxCount();
		if(ClanMemberMaxCount <= 0)
		{
			if( pClanMemberContext->_sectionCount >  0)
			{
				INT32	MemberCount = pClanMemberContext->_sectionSize * pClanMemberContext->_sectionCount;
				ClanGameContext::i()->CreateMyClanMemberList(MemberCount);
			}
		}

		if(pClanMemberContext->_totalCount > (section * pClanMemberContext->_sectionSize) )
		{
			INT32	Size = sizeof(CLAN_MEMBER_BASIC_INFO) * count;
			CLAN_MEMBER_BASIC_INFO * pMemberBasicInfo = (CLAN_MEMBER_BASIC_INFO *)i3MemAlloc(Size);
			pPacket->ReadData(pMemberBasicInfo, Size);

			INT32 StartIndex = section * pClanMemberContext->_sectionSize;
			for (INT32 i = 0 ; i < count ; ++i )
			{
				ClanGameContext::i()->setMyClanMember(StartIndex+i, *(pMemberBasicInfo+i));
			}

			I3MEM_SAFE_FREE(pMemberBasicInfo);

			GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_MEMBER_LIST, (INT32)section );
		}
	}
}

void ClientTCPSocket::__Parse_Clan_RequestContext( i3NetworkPacket * pPacket)
{
	INT32 Temp;

	pPacket->ReadData(&Temp,	sizeof(INT32));
	if(Temp > -1)
	{
		CLAN_REQUEST_CONTEXT * pClanRequestContext = ClanGameContext::i()->getMyClanRequestContext(); 
		pPacket->ReadData(pClanRequestContext, sizeof(CLAN_REQUEST_CONTEXT));
		ClanGameContext::i()->DeleteMyClanRequestList(); 
		if(pClanRequestContext->_sectionCount > 0)
		{
			ClanGameContext::i()->CreateMyClanRequestList( pClanRequestContext->_sectionSize * pClanRequestContext->_sectionCount ); 
		}
	}
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_REQUEST_CONTEXT, Temp );
}

void ClientTCPSocket::__Parse_Clan_RequestList( i3NetworkPacket * pPacket)
{
	PACKET_H_CLAN_ACK PacketHeader;

	pPacket->ReadData(&PacketHeader,	sizeof(PACKET_H_CLAN_ACK));

	if(PacketHeader.m_i32Result > -1)
	{
		PACKET_D_CS_NEW_REQUEST_LIST stListAck;

		pPacket->ReadData(&stListAck,	sizeof(PACKET_D_CS_NEW_REQUEST_LIST));

		CLAN_REQUEST_CONTEXT	* pClanRequestContext	= ClanGameContext::i()->getMyClanRequestContext();
		CLAN_REQUEST_BASIC_INFO	* pRequestList			= ClanGameContext::i()->getMyClanRequest();

		if(pRequestList == nullptr)
		{
			if( pClanRequestContext->_sectionCount >  0)
			{
				pRequestList = ClanGameContext::i()->CreateMyClanRequestList(  pClanRequestContext->_sectionSize * pClanRequestContext->_sectionCount ); 
			}

			I3ASSERT_RETURN(pRequestList);
		}

		UINT32 ui32Count = (UINT32)(stListAck.m_ui8SectionIdx * pClanRequestContext->_sectionSize);

		if(pClanRequestContext->_totalCount > ui32Count )
		{
			pPacket->ReadData(&pRequestList[ ui32Count ],  (sizeof(CLAN_REQUEST_BASIC_INFO) * stListAck.m_ui8Size) );
			GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_MEMBER_LIST_N, (INT32)stListAck.m_ui8SectionIdx);
		}
	}
	else
	{
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_MEMBER_LIST_N, 0);
	}
}

void ClientTCPSocket::__Parse_Clan_RequestInfo( i3NetworkPacket * pPacket)
{
	INT32 Temp;

	pPacket->ReadData(&Temp,	sizeof(INT32));
	if(Temp > -1)
	{
		CLAN_REQUEST_DETAIL_INFO * pClanRequestDetailInfo = ClanGameContext::i()->getClanRequestDetailInfo(); 
		pPacket->ReadData(pClanRequestDetailInfo, sizeof(CLAN_REQUEST_DETAIL_INFO));
	}
}

void ClientTCPSocket::__Parse_Clan_AcceptRequest( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_ACCEPT_REQUEST, Temp );
}

void ClientTCPSocket::__Parse_Clan_DenineRequest( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_DENIAL_REQUEST, Temp );
}

void ClientTCPSocket::__Parse_Clan_Invite( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_INVITE, Temp );
}

void ClientTCPSocket::__Parse_Clan_AcceptInvite( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_INVITE_ACCEPT_N, Temp );
}

void ClientTCPSocket::__Parse_Clan_Session( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_SECESSION, Temp );
}

void ClientTCPSocket::__Parse_Clan_Commission_Master( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_COMMISSIONMASTER, Temp );
}

void ClientTCPSocket::__Parse_Clan_Commission_Staff( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
 	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_COMMISSIONSTAFF, Temp );
}

void ClientTCPSocket::__Parse_Clan_Commission_Regular( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_COMMISSION_REGULAR, Temp );
}

void ClientTCPSocket::__Parse_Clan_Deportation( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_DISMISSALMEMBER, Temp );
}

void ClientTCPSocket::__Parse_Clan_AcceptRequestResult( i3NetworkPacket * pPacket)
{
	pPacket->ReadData(ClanGameContext::i()->getMyClanDetailInfo(),	sizeof(CLAN_DETAIL_INFO));	

	USER_INFO_BASIC MyUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);

	MyUserInfoBasic.m_ui32Clanidx		= ClanGameContext::i()->getMyClanDetailInfo()->m_ui32CID;
	MyUserInfoBasic.m_ui32ClanState		= CLAN_MEMBER_LEVEL_REGULAR;
	MyUserInfoBasic.m_ui8ClanExpRank	= ClanGameContext::i()->getMyClanDetailInfo()->m_expRank;
	MyUserInfoBasic.m_ui8ClanPerRank	= ClanGameContext::i()->getMyClanDetailInfo()->m_personRank;
	MyUserInfoBasic.m_ui32ClanMark		= ClanGameContext::i()->getMyClanDetailInfo()->m_mark;
	MyUserInfoBasic.m_ui8ClanNamecolor= ClanGameContext::i()->getMyClanDetailInfo()->m_color;
	i3::safe_string_copy( MyUserInfoBasic.m_strClanName, ClanGameContext::i()->getMyClanDetailInfo()->m_name, NET_CLAN_NAME_SIZE );
	UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_M_CLAN_MEMBER_INFO);
}

void ClientTCPSocket::__Parse_Clan_DenialRequestResult( i3NetworkPacket * pPacket)
{
	USER_INFO_BASIC		MyUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
    MyUserInfoBasic.m_ui32Clanidx	= CLAN_MEMBER_LEVEL_UNKNOWN;
    MyUserInfoBasic.m_ui32ClanState	= CLAN_MEMBER_LEVEL_UNKNOWN;
	UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);
}

void ClientTCPSocket::__Parse_Clan_DeportationResult( i3NetworkPacket * pPacket)
{
	USER_INFO_BASIC		MyUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
	MyUserInfoBasic.m_ui32Clanidx		= CLAN_MEMBER_LEVEL_UNKNOWN;
	MyUserInfoBasic.m_ui32ClanState		= CLAN_MEMBER_LEVEL_UNKNOWN;
	UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_M_CLAN_DEPROTATION);
}

void ClientTCPSocket::__Parse_Clan_Commission_MasterResult( i3NetworkPacket * pPacket)
{
	USER_INFO_BASIC		MyUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
    MyUserInfoBasic.m_ui32ClanState	= CLAN_MEMBER_LEVEL_MASTER;
	UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_M_CLAN_CHANGE_GRADE);
}

void ClientTCPSocket::__Parse_Clan_Commission_StaffResult( i3NetworkPacket * pPacket)
{
	USER_INFO_BASIC		MyUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
    MyUserInfoBasic.m_ui32ClanState	= CLAN_MEMBER_LEVEL_STAFF;
	UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_M_CLAN_CHANGE_GRADE);
}

void ClientTCPSocket::__Parse_Clan_Commission_RegularResult( i3NetworkPacket * pPacket)
{
	USER_INFO_BASIC		MyUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
	MyUserInfoBasic.m_ui32ClanState	= CLAN_MEMBER_LEVEL_REGULAR;
	UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_M_CLAN_CHANGE_GRADE);
}

void ClientTCPSocket::__Parse_Clan_ChangeExpRank( i3NetworkPacket * pPacket)
{
	USER_INFO_BASIC		MyUserInfoBasic;

	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
	pPacket->ReadData(&MyUserInfoBasic.m_ui8ClanExpRank,	sizeof(UINT8));
	UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);
}

void ClientTCPSocket::__Parse_Clan_ChangePerRank( i3NetworkPacket * pPacket)
{
	USER_INFO_BASIC		MyUserInfoBasic;

	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
	pPacket->ReadData(&MyUserInfoBasic.m_ui8ClanPerRank,	sizeof(UINT8));
	UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);
}

void ClientTCPSocket::__Parse_Clan_ChangeMarkResult( i3NetworkPacket * pPacket)
{
	UINT32 wMark;

	pPacket->ReadData(&wMark,	sizeof(UINT32));

	USER_INFO_BASIC		MyUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
	MyUserInfoBasic.m_ui32ClanMark	= wMark;
	UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);

	ClanGameContext::i()->getMyClanDetailInfo()->m_mark = wMark;
}

void ClientTCPSocket::__Parse_Clan_ChangeManagementResult( i3NetworkPacket * pPacket)
{
	UINT8 cAuthority;

	pPacket->ReadData(&cAuthority,	sizeof(UINT8));
	ClanGameContext::i()->getMyClanDetailInfo()->m_authstaff	= cAuthority;
}

void ClientTCPSocket::__Parse_Clan_ChangeName( i3NetworkPacket * pPacket)
{
	TTCHAR szClanName[ NET_CLAN_NAME_SIZE ];

	pPacket->ReadData(szClanName,	NET_CLAN_NAME_SIZE * sizeof(TTCHAR) );

	USER_INFO_BASIC		MyUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
	i3::safe_string_copy( MyUserInfoBasic.m_strClanName, szClanName, NET_CLAN_NAME_SIZE );
	UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);

	i3::safe_string_copy( ClanGameContext::i()->getMyClanDetailInfo()->m_name, szClanName, NET_CLAN_NAME_SIZE );
}

void ClientTCPSocket::__Parse_Clan_InviteRoom( i3NetworkPacket * pPacket)
{
#if defined (DEV_BUILD)
	INT32 returnCode;

	pPacket->ReadData(&returnCode, sizeof(INT32));

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_INVITED_CLAN_MESSAGE, returnCode);

	switch( returnCode)
	{
	case EVENT_ERROR_EVENT_FAIL_INVITED_USER:
		{
			/*초대한 유저가 채널 선택을 하지 않은 상태입니다.\n채널 선택 후 초대해 주시기 바랍니다.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_NETWORK_FAIL_INVITED_USER"));
		}
		break;			
	case EVENT_ERROR_EVENT_FAIL_INVITED_USER_IN_CLAN_MATCH:
		{
			/*클랜전을 대기 중인 유저입니다.\n해당 유저가 팀등록 상태가 아니면\n초대 가능합니다.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_NETWORK_FAIL_INVITED_USER_IN_CLAN_MATCH"));
		}
		break;			
	default:
		break;
	}
#endif
}

void ClientTCPSocket::__Parse_Clan_InviteRoomResult( i3NetworkPacket * pPacket)
{
	if(	g_pFramework->IsUIStage() == false)
	{				
		return;	// ReadyStage가 아니면 처리하지 않는다.
	}

	// 배틀 팀 중이면 리턴
	if( g_pFramework->GetClanContext()->InBattleTeam() ) 
		return;

	INT64 i64UID;
	pPacket->ReadData(&i64UID, sizeof(INT64));

	INT32 idx = CCommunityContext::i()->GetClanMemberIdx( i64UID );
	if( idx == -1 ) return;

	const CLAN_MEMBER_NODE* node = CCommunityContext::i()->GetClanMember((UINT32)idx );

	if (node)
	{
		INT32 viewIndex = CCommunityContext::i()->GetViewIndexOfRealClanMember(idx);

		if( CCommunityContext::i()->SetMovingToClanUser(viewIndex))
		{
			GameEventReceiver::i()->PushReceivedGameEvent(EVENT_INVITED_CLAN_MESSAGE);				
		}
		else
		{
			I3TRACE(" Invited message full\n");
		}
	}	
	else
	{
		I3TRACE(" Invalid friend idx(%d)\n", idx);
	}
}

void ClientTCPSocket::__Parse_Clan_Close( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_DISSOLUTION, Temp );
}

void ClientTCPSocket::__Parse_Clan_Chat( i3NetworkPacket * pPacket)
{
	UINT8 wNickSize;
	UINT8 wChatSize;
	UINT8 ui8NickColor;
	TTCHAR  szNick[NET_NICK_NAME_SIZE]; 
	TTCHAR  String[MAX_STRING_COUNT]; 
	bool bGameMaster;
	::memset(String, 0, sizeof(TTCHAR) * MAX_STRING_COUNT);

	pPacket->ReadData( &wNickSize,	sizeof(UINT8) );
	pPacket->ReadData( szNick,		wNickSize * sizeof(TTCHAR) );
	pPacket->ReadData( &bGameMaster, sizeof(bool));		// 20140924 김경주 GM FLAG
	pPacket->ReadData( &ui8NickColor,	sizeof(UINT8) );	// 컬러닉네임
	pPacket->ReadData( &wChatSize,	sizeof(UINT8) );
	pPacket->ReadData( String,		wChatSize * sizeof(TTCHAR) );

	szNick[ NET_NICK_NAME_SIZE - 1 ] = '\0';
	String[ MAX_STRING_COUNT - 1 ] = '\0';
	
	
	i3::rc_wstring wstrNick = szNick;
	i3::wstring wstr_chat = String;

	g_pFramework->getChatBox()->PutUserChatting( wstrNick, wstr_chat, bGameMaster, CHATTING_TYPE_CLAN); 
}

void ClientTCPSocket::__Parse_Clan_ChatAtMemberPage( i3NetworkPacket * pPacket)
{
	UINT8 wNickSize;
	UINT8 wChatSize;
	UINT8 ui8NickColor;
	TTCHAR  szNick[NET_NICK_NAME_SIZE]; 
	TTCHAR  String[MAX_STRING_COUNT]; 
	bool bGameMaster;
	::memset(String, 0, sizeof(TTCHAR) * MAX_STRING_COUNT);

	pPacket->ReadData( &wNickSize,	sizeof(UINT8) );
	pPacket->ReadData( szNick,		wNickSize * sizeof(TTCHAR) );
	pPacket->ReadData( &bGameMaster, sizeof(bool)); // 20140924 김경주 GM FLAG
	pPacket->ReadData( &ui8NickColor,	sizeof(UINT8) );	// 컬러닉네임
	pPacket->ReadData( &wChatSize,	sizeof(UINT8) );
	pPacket->ReadData( String,		wChatSize * sizeof(TTCHAR) );

	szNick[ NET_NICK_NAME_SIZE - 1 ] = '\0';
	String[ MAX_STRING_COUNT - 1 ] = '\0';

	i3::rc_wstring wstrNick = szNick;
	i3::wstring wstr_chat = String;
		
	g_pFramework->getChatBox()->PutUserChatting( wstrNick, wstr_chat, bGameMaster, CHATTING_TYPE_CLAN_MEMBER_PAGE ); 
}

void ClientTCPSocket::__Parse_Clan_MemberDetailInfo( i3NetworkPacket * pPacket)
{
	INT32 Temp;

	pPacket->ReadData(&Temp,	sizeof(INT32));
	if( EV_SUCCESSED( Temp ) )
	{
		USER_INFO_RECORD * pRecord = CCommunityContext::i()->SetCommnuitRecord();
		pPacket->ReadData(pRecord, sizeof(USER_INFO_RECORD));

		CCommunityContext::i()->SetCommunityUserInfo(true);
	}
}

void ClientTCPSocket::__Parse_Clan_ChangeMemberBasicInfoResult( i3NetworkPacket * pPacket)
{
	CLAN_MEMBER_BASIC_INFO MemberBasicInfo;

	pPacket->ReadData( &MemberBasicInfo,		sizeof(CLAN_MEMBER_BASIC_INFO) );

	CCommunityContext::i()->ChangeBasicInfoClanMember( &MemberBasicInfo );
	g_pFramework->GetClanContext()->SetMemberBasicInfo( &MemberBasicInfo );
}

void ClientTCPSocket::__Parse_Clan_ChangeNotice( i3NetworkPacket * pPacket)
{
	INT32 Temp;

	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_NOTICE_MODIFY, Temp );
}

void ClientTCPSocket::__Parse_Clan_ChangeIntro( i3NetworkPacket * pPacket)
{
	INT32 Temp;

	pPacket->ReadData(&Temp,	sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_GUIDE_MODIFY, Temp );
}

void ClientTCPSocket::__Parse_Clan_ChangeAcceptOption( i3NetworkPacket * pPacket)
{
	PACKET_H_CLAN_ACK PacketHeader;
	pPacket->ReadData(&PacketHeader, sizeof(PACKET_H_CLAN_ACK));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CLAN_ACCEPT_OPTION_MODIFY, PacketHeader.m_i32Result );
}

void ClientTCPSocket::__Parse_Clan_ResetRecordResult( i3NetworkPacket * pPacket)
{
	ClanGameContext::i()->getMyClanDetailInfo()->m_match = 0;
	ClanGameContext::i()->getMyClanDetailInfo()->m_win = 0;
	ClanGameContext::i()->getMyClanDetailInfo()->m_lose = 0;
}

void ClientTCPSocket::__Parse_Clan_ResetPointResult( i3NetworkPacket * pPacket)
{
	ClanGameContext::i()->getMyClanDetailInfo()->ResetClanPoint();
}

void ClientTCPSocket::__Parse_Clan_ChangeColorNameResult( i3NetworkPacket * pPacket)
{
	UINT8 cColor;

	pPacket->ReadData(&cColor,	sizeof(UINT8));
//	I3ASSERT( cColor < COLOR_CASHITEM_COUNT);		// 무의미 해서 주석..

	ClanGameContext::i()->getMyClanDetailInfo()->m_color = cColor;
}

void ClientTCPSocket::__Parse_Clan_CheckConditionForCreation( i3NetworkPacket * pPacket)
{
	UINT8	ui8Rank;
	UINT32	ui32Point;

	pPacket->ReadData(&ui8Rank,		sizeof(UINT8));
	pPacket->ReadData(&ui32Point,	sizeof(UINT32));

	g_pFramework->GetClanContext()->SetClanCreationRequirementInfo(ui8Rank, ui32Point);
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_CREATION_INFO);
}

void ClientTCPSocket::__Parse_Clan_MatchResultContext( i3NetworkPacket * pPacket)
{
	//PACKET_H_CLAN_ACK PacketHeader;

	//pPacket->ReadData(&PacketHeader, sizeof(PACKET_H_CLAN_ACK));

	//if(EV_SUCCESSED(PacketHeader.m_i32Result))
	//{
	//	PACKET_D_CS_CLAN_MATCH_RESULT_CONTEXT MatchResultCxt;
	//	pPacket->ReadData(&MatchResultCxt, sizeof(PACKET_D_CS_CLAN_MATCH_RESULT_CONTEXT));

	////	CLAN_MATCH_RESULT_CONTEXT * pContext = g_pFramework->GetClanContext()->SetClanResultContext();

	//	// 컨텍스트가 다른 경우에만 값 변경 후, 리스트 요청
	//	if(MatchResultCxt.m_ClanMathcResultContext.m_ui32Version != pContext->m_ui32Version)
	//	{
	//		i3mem::Copy(pContext, &MatchResultCxt.m_ClanMathcResultContext, sizeof(CLAN_MATCH_RESULT_CONTEXT));
	//		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_MATCH_RESULT_CONTEXT, PacketHeader.m_i32Result);
	//	}
	//}
}

void ClientTCPSocket::__Parse_Clan_MatchResultList( i3NetworkPacket * pPacket)
{
	//PACKET_H_CLAN_ACK PacketHeader;

	//pPacket->ReadData(&PacketHeader, sizeof(PACKET_H_CLAN_ACK));

	//if(EV_SUCCESSED(PacketHeader.m_i32Result))
	//{
	//	PACKET_D_CS_CLAN_MATCH_RESULT_LIST_CNT MatchResultCnt;
	//	pPacket->ReadData(&MatchResultCnt, sizeof(PACKET_D_CS_CLAN_MATCH_RESULT_LIST_CNT));

	//	if(MatchResultCnt.m_ui8iSection > 0)
	//	{
	//		PACKET_D_CS_CLAN_MATCH_RESULT_LIST MatchResultList;

	//		for( UINT8 i = 0; i < MatchResultCnt.m_ui8iSection; ++i)
	//		{
	//			pPacket->ReadData( &MatchResultList, sizeof( MatchResultList));

	//			INT32 i32PageNum = MatchResultCnt.m_ui8SectionIdx * MATCH_RESULT_SECTION_SIZE;

	//			CClanContext * pClanContext = g_pFramework->GetClanContext();
	//			for( UINT8  j = 0 ; j < MatchResultCnt.m_ui8iSection ; ++j)
	//			{
	//				CLAN_MATCH_RESULT * pInfo = pClanContext->SetClanResultList(i32PageNum + j);
	//				i3mem::Copy(pInfo, &MatchResultList.m_ClanMatchResult, sizeof(CLAN_MATCH_RESULT));
	//			}
	//		}
	//	}
	//}

	//GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_MATCH_RESULT_LIST, PacketHeader.m_i32Result);
}

void ClientTCPSocket::__Parse_Clan_MatchResultEmotion( i3NetworkPacket * pPacket)
{
	//PACKET_H_CLAN_ACK PacketHeader;

	//pPacket->ReadData(&PacketHeader, sizeof(PACKET_H_CLAN_ACK));

	//if(EV_SUCCESSED(PacketHeader.m_i32Result))
	//{
	//	PACKET_D_CS_CLAN_MATCH_RESULT_EMOTION_SAVE PacketDEmotion;
	//	
	//	pPacket->ReadData(&PacketDEmotion, sizeof(PACKET_D_CS_CLAN_MATCH_RESULT_EMOTION_SAVE));

	//	INT32 i32ModifySlot = g_pFramework->GetClanContext()->GetEmotionSlotIdx();
	//	CLAN_MATCH_RESULT * pInfo = g_pFramework->GetClanContext()->SetClanResultList(i32ModifySlot);

	//	pInfo->m_ui32PositiveCnt = PacketDEmotion.m_ui32PositiveCnt;
	//	pInfo->m_ui32NegativeCnt = PacketDEmotion.m_ui32NegativeCnt;

	//	i3::safe_string_copy(pInfo->m_strPositiveUserNick, PacketDEmotion.m_strLikeUserNick, NET_NICK_NAME_SIZE);
	//	i3::safe_string_copy(pInfo->m_strNegativeUserNick, PacketDEmotion.m_strOMGUserNick, NET_NICK_NAME_SIZE);
	//}

	//GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_MATCH_RESULT_EMOTION, PacketHeader.m_i32Result);
}
