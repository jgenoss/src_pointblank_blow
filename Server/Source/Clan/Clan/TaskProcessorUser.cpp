#include "pch.h"
#include "TaskProcessor.h"
#include "PacketLocker.h"
#include "Gateway.h"
//#include "LogFile.h"
#include "DataBase.h"
#include "Receiver.h"

#define READ_CLAN_WAR_INFO	pPacket->ReadData(&TTeamID, sizeof(TTeamID));	\
pPacket->ReadData(&TMerID, sizeof(TMerID)); \
stClanWarInfo.m_TTeamID = TTeamID; \
stClanWarInfo.m_TMerID = TMerID; \


BOOL CTaskProcessor::_DoTaskForGameUser( UINT32 ui32idxBuffer, i3NetworkPacket* pPacket )
{
	T_TeamID	TTeamID;
	T_MerID		TMerID;
	MATCH_MEMBER_INFO	stMemberInfo;

	CLAN_USER_INFO	stUserInfo;
	stUserInfo.ui32ServerIdx = ui32idxBuffer;
	pPacket->ReadData( &stUserInfo.i32SessionIdx,	sizeof(INT32) );
	pPacket->ReadData( &stUserInfo.i64UID,			sizeof(T_UID) );
	pPacket->ReadData( &stUserInfo.ui32ClanDBIdx,	sizeof(UINT32) );

	CLAN_WAR_INFO	stClanWarInfo;
	i3mem::Copy( &stClanWarInfo, &stUserInfo, sizeof(CLAN_USER_INFO));

	switch(pPacket->GetProtocolID()) 
	{
	case PROTOCOL_CS_USER_LOGIN_REQ:				// 로그인 요청
		{	
			UserLogIn( &stUserInfo );
		}
		break;
	case PROTOCOL_CS_USER_LOGOUT_REQ:				// 로그아웃 요청
		{
			READ_CLAN_WAR_INFO;

			UserLogOut( &stClanWarInfo );
		}
		break;	
	case PROTOCOL_CS_MEMBER_INFO_CHANGE_REQ:		// 유저 정보(상태) 변경 요청
		{
			UINT64 ui64State;
			pPacket->ReadData(&ui64State,					sizeof(UINT64));

			UserStateChange(&stUserInfo, ui64State);
		}
		break;
	case PROTOCOL_CS_USER_SERVER_ENTER_REQ:			// 서버 이동 요청
		{
			UserServerEnter( &stUserInfo );
		}
		break;
	case PROTOCOL_CS_USER_SERVER_LEAVE_REQ:
		{
			READ_CLAN_WAR_INFO;
			UserServerLeave( &stClanWarInfo );
		}
		break;
	case PROTOCOL_CS_USER_CHANEL_ENTER_REQ:			// 채널 입장 요청
		{
			UINT32 wChannel;

			pPacket->ReadData(&wChannel, sizeof(UINT32));

			UserChannelEnter( &stUserInfo, wChannel );
		}
		break;
	case PROTOCOL_CS_USER_CHANEL_LEAVE_REQ:			// 채널 이동 요청
		{
			READ_CLAN_WAR_INFO;
			UserChannelLeave( &stClanWarInfo );
		}
		break;
    case PROTOCOL_CS_USER_ROOM_ENTER_REQ:			// 방 이동 요청
		{
			UINT32 ui32Room;
			bool	bIsMatchMaking;

			pPacket->ReadData(&ui32Room,		sizeof(UINT32)); 
			pPacket->ReadData(&bIsMatchMaking,	sizeof(bool));

			UserRoomEnter( &stUserInfo, ui32Room, bIsMatchMaking );
		}
		break;
	case PROTOCOL_CS_USER_ROOM_LEAVE_REQ:			// 방 이동 요청
		{
			UserRoomLeave( &stUserInfo );
		}
		break;
	case PROTOCOL_CS_USER_PAGE_ENTER_REQ:			// 클랜 페이지 입장 요청
		{
			UserClanPageEnter( &stUserInfo );
		}
		break;
	case PROTOCOL_CS_USER_PAGE_LEAVE_REQ:			// 클랜 페이지 퇴장 요청
		{
			UserClanPageLeave( &stUserInfo );
		}
		break;
	case PROTOCOL_CS_MEMBER_CONTEXT_REQ:
		{
			SendMemberContext( &stUserInfo );
		}
		break;
	case PROTOCOL_CS_REQUEST_CONTEXT_REQ:
		{
			SendRequestContext( &stUserInfo );
		}
		break;
	case PROTOCOL_CS_MEMBER_LIST_REQ:
		{
			UINT8	wSection;

			pPacket->ReadData(&wSection, sizeof(UINT8));

			SendMemberList( &stUserInfo, wSection );
		}
		break;
	case PROTOCOL_CS_REQUEST_LIST_REQ:
		{
			UINT8 ui8Section;

			pPacket->ReadData(&ui8Section, sizeof(UINT8));

			SendRequestList( &stUserInfo, ui8Section );
		}
		break;
	case PROTOCOL_CS_CLAN_LIST_DETAIL_INFO_REQ:				// Clan 상세 정보
		{
			UINT8 ui8Type;
			
			pPacket->ReadData(&ui8Type, sizeof(UINT8));

			ClanListDetailInfo( &stUserInfo, ui8Type );
		}
		break;
	case PROTOCOL_CS_DETAIL_INFO_REQ:						// Clan 상세 정보
		{
			UINT8 ui8Type;

			pPacket->ReadData(&ui8Type, sizeof(UINT8));

			ClanDetailInfo( &stUserInfo, ui8Type );
		}
		break;
	case PROTOCOL_CS_MEMBER_DETAIL_INFO_REQ:		// Clan 맴버 상세 정보
		{ 
			T_UID wMemberDBIdx;

			pPacket->ReadData(&wMemberDBIdx, sizeof(T_UID));

			ClanDetailInfoMember( &stUserInfo, wMemberDBIdx );
		}
		break;
	case PROTOCOL_CS_REQUEST_INFO_REQ:				// Clan 가입 신청자 상세 정보
		{
			T_UID i64RequestDBIdx;
			pPacket->ReadData(&i64RequestDBIdx, sizeof(T_UID));

			ClanDetailInfoRequest( &stUserInfo, i64RequestDBIdx );
		}
		break;

	case PROTOCOL_CS_CREATE_CLAN_REQ:								// 클랜 생성
		{
			UINT8 ui8SizeNick;
			UINT8 ui8SizeName;
			UINT8 ui8SizeIntro;
			UINT8 ui8SizeAzitURL;

			CLAN_CS_CREATE_INFO	ClanCreateInfo;
			
			pPacket->ReadData(&ClanCreateInfo.wChannelNum,	sizeof(UINT8));
			pPacket->ReadData(&ClanCreateInfo.nRoomIdx,		sizeof(INT32));
			pPacket->ReadData(&ClanCreateInfo.wMark,		sizeof(UINT32));
			pPacket->ReadData(&ClanCreateInfo.ui32Rank,		sizeof(UINT32));
			pPacket->ReadData(&ClanCreateInfo.m_ui8NickColor,		sizeof(UINT8));
			
			pPacket->ReadData(&ui8SizeNick,					sizeof(UINT8));		LENGTH_CHECK( ui8SizeNick, NET_NICK_NAME_SIZE );
			pPacket->ReadData(&ui8SizeName,					sizeof(UINT8));		LENGTH_CHECK( ui8SizeName, NET_CLAN_NAME_SIZE );
			pPacket->ReadData(&ui8SizeIntro,				sizeof(UINT8));		LENGTH_CHECK( ui8SizeIntro, NET_CLAN_INTRO_SIZE );
			pPacket->ReadData(&ui8SizeAzitURL,				sizeof(UINT8));		LENGTH_CHECK( ui8SizeAzitURL, NET_CLAN_AZIT_URL_SIZE );
			
			pPacket->ReadData(ClanCreateInfo.strNick,		sizeof(TTCHAR) * ui8SizeNick);
			pPacket->ReadData(ClanCreateInfo.szClanName,	sizeof(TTCHAR) * ui8SizeName);
			pPacket->ReadData(ClanCreateInfo.szClanIntro,	sizeof(TTCHAR) * ui8SizeIntro);
			pPacket->ReadData(ClanCreateInfo.szAzitURL,		sizeof(TTCHAR) * ui8SizeAzitURL);

			ClanCreateInfo.m_ui8MemberCount	= g_pConfig->m_ui8CreateMemberCount;

			// 오버플로우 방지
			ClanCreateInfo.strNick[ NET_NICK_NAME_SIZE-1 ]			= '\0';
			ClanCreateInfo.szClanName[ NET_CLAN_NAME_SIZE-1 ]		= '\0';
			ClanCreateInfo.szClanIntro[ NET_CLAN_INTRO_SIZE-1 ]		= '\0';
			ClanCreateInfo.szAzitURL[ NET_CLAN_AZIT_URL_SIZE-1 ]	= '\0';

			// 클랜 생성				
			ClanCreate( &stUserInfo, &ClanCreateInfo );
			g_pConfig->SetClanCount(GetClanCount());
		}
		break;
	case PROTOCOL_CS_CLOSE_CLAN_REQ:				// Clan 해체
		{
			ClanClose( &stUserInfo );
		}
		break;
	case PROTOCOL_CS_JOIN_REQUEST_REQ:				// 클랜 가입 신청서
		{
			UINT8 ui8Size;
			UINT8 ui8NickSize;
			CLAN_CS_JOIN_INFO	ClanJoinInfo;

			pPacket->ReadData(&ClanJoinInfo.wChannelNum,	sizeof(UINT8));
			pPacket->ReadData(&ClanJoinInfo.nRoomIdx,		sizeof(INT32));
			pPacket->ReadData(&ui8Size,						sizeof(UINT8));		LENGTH_CHECK( ui8Size, NET_CLAN_REQUEST_SIZE );
			pPacket->ReadData(ClanJoinInfo.szContents,		sizeof(TTCHAR) * ui8Size);
			pPacket->ReadData(&ClanJoinInfo.wAge,			sizeof(UINT8));
			pPacket->ReadData(&ClanJoinInfo.wRank,			sizeof(UINT8));
			pPacket->ReadData(&ClanJoinInfo.wViewRank,		sizeof(UINT8));
			pPacket->ReadData(&ClanJoinInfo.ui8NickColor,		sizeof(UINT8));
			pPacket->ReadData(&ui8NickSize,					sizeof(UINT8));		LENGTH_CHECK( ui8NickSize, NET_NICK_NAME_SIZE );
			pPacket->ReadData(ClanJoinInfo.strNick,			sizeof(TTCHAR) * ui8NickSize);
			pPacket->ReadData(&ClanJoinInfo._ui32Killcount,	sizeof(UINT32));
			pPacket->ReadData(&ClanJoinInfo._ui32Death,		sizeof(UINT32));
			pPacket->ReadData(&ClanJoinInfo._ui32Match,		sizeof(UINT32));
			pPacket->ReadData(&ClanJoinInfo._ui32Win,		sizeof(UINT32));
			pPacket->ReadData(&ClanJoinInfo._ui32Lose,		sizeof(UINT32));
			pPacket->ReadData(&ClanJoinInfo._ui32AssistCount, sizeof(UINT32));

			ClanJoinInfo.szContents[NET_CLAN_REQUEST_SIZE - 1]	= '\0';
			ClanJoinInfo.strNick[ NET_NICK_NAME_SIZE-1 ]		= '\0';

			ClanRequestJoin( &stUserInfo, &ClanJoinInfo );
		}
		break;
	case PROTOCOL_CS_ACCEPT_REQUEST_REQ:			// 클랜 가입 신청 수락
		{
			UINT8	ui8Count;
			T_UID	ai64UID[ MAX_REQUEST_COUNT ];

			pPacket->ReadData(&ui8Count,	sizeof(UINT8));
			//if( ui8Count > MAX_REQUEST_COUNT ) break;
			LENGTH_CHECK( ui8Count, MAX_REQUEST_COUNT );

			pPacket->ReadData( ai64UID,		sizeof(T_UID) * ui8Count);

			ClanRequestAccept( &stUserInfo, ui8Count, ai64UID );
		}
		break;
	case PROTOCOL_CS_CANCEL_REQUEST_REQ:			// 클랜 가입 신청 취소
		{
			ClanRequestCancel( &stUserInfo );
		}
		break;	
	case PROTOCOL_CS_DENIAL_REQUEST_REQ:			// 클랜 가입 신청 거절
		{
			UINT8	ui8Count;
			T_UID	ai64UID[ MAX_REQUEST_COUNT ];

			pPacket->ReadData(&ui8Count,		sizeof(UINT8));
			//if( ui8Count > MAX_REQUEST_COUNT ) break;
			LENGTH_CHECK( ui8Count, MAX_REQUEST_COUNT );

			pPacket->ReadData( ai64UID,	sizeof(T_UID) * ui8Count);

			ClanRequestDenial( &stUserInfo, ui8Count, ai64UID );
		}
		break;
	case PROTOCOL_CS_INVITE_ACCEPT_REQ:
		{
			UINT32	ui32State;
			UINT8	ui8Accept;
			UINT32	ui32Rank = 0;
			UINT8	ui8NickSize = 0;
			TTCHAR	strNick[ NET_NICK_NAME_SIZE ];
			UINT8	ui8NickColor = 0;

			pPacket->ReadData(&ui32State,		sizeof(UINT32));
			pPacket->ReadData(&ui8Accept,		sizeof(UINT8));
			if( 1 == ui8Accept )
			{
				pPacket->ReadData(&ui32Rank,	sizeof(UINT32));
				pPacket->ReadData(&ui8NickColor,	sizeof(UINT8));
				pPacket->ReadData(&ui8NickSize,	sizeof(UINT8));		LENGTH_CHECK( ui8NickSize, NET_NICK_NAME_SIZE );
				pPacket->ReadData(strNick,		sizeof(TTCHAR) * ui8NickSize);
				strNick[ NET_NICK_NAME_SIZE-1 ] = '\0';
			}

			ClanInviteAccept( &stUserInfo, ui32State, ui8Accept, ui32Rank, strNick, ui8NickColor );
		}
		break;
	case PROTOCOL_CS_COMMISSION_MASTER_REQ:
		{
			T_UID	i64MemberDBIdx;
			UINT8	ui8CommisionRank = 0;

			pPacket->ReadData(&i64MemberDBIdx,	sizeof(T_UID));
			pPacket->ReadData(&ui8CommisionRank, sizeof(UINT8));

			ClanCommissionMaster( &stUserInfo, i64MemberDBIdx, ui8CommisionRank);
		}
		break;
	case PROTOCOL_CS_COMMISSION_STAFF_REQ:
		{
			UINT8	ui8Count;
			INT64	ai64UID[ MAX_REQUEST_COUNT ];

			pPacket->ReadData( &ui8Count,		sizeof(UINT8));		LENGTH_CHECK( ui8Count, MAX_REQUEST_COUNT );
			pPacket->ReadData( ai64UID,			sizeof(T_UID) * ui8Count);

			ClanCommissionStaff( &stUserInfo, ui8Count, ai64UID );
		}
		break;
	case PROTOCOL_CS_COMMISSION_REGULAR_REQ:
		{
			UINT8	ui8Count;
			INT64	ai64UID[ MAX_MEMBER_COUNT ];

			pPacket->ReadData( &ui8Count,		sizeof(UINT8));		
			LENGTH_CHECK( ui8Count, MAX_MEMBER_COUNT );
			pPacket->ReadData( ai64UID,			sizeof(T_UID) * ui8Count);

			ClanCommissionRegular( &stUserInfo, ui8Count, ai64UID );
		}
		break;
	case PROTOCOL_CS_SECESSION_CLAN_REQ:							// 클랜 탈퇴 요청
		{
			ClanSecession( &stUserInfo );
		}
		break;
	case PROTOCOL_CS_DEPORTATION_REQ:								// 클랜 제명 요청
		{
			UINT8	ui8Count;
			INT64	ai64MemberDBIdx[ MAX_REQUEST_COUNT ];

			pPacket->ReadData(&ui8Count,		sizeof(UINT8));		LENGTH_CHECK( ui8Count, MAX_REQUEST_COUNT );
			pPacket->ReadData(ai64MemberDBIdx,	sizeof(T_UID) * ui8Count);

			ClanDeportation( &stUserInfo, ui8Count, ai64MemberDBIdx );
		}
		break;
	case PROTOCOL_CS_CHATTING_REQ:									// 클랜 채팅 요청
		{
			UINT8	ui8ChatSize;
			UINT8	ui8NickColor;
			TTCHAR	strChating[NET_CLAN_CHATING_SIZE];
			bool	bIsGMUser;

			pPacket->ReadData(&bIsGMUser,		sizeof(bool));
			pPacket->ReadData(&ui8NickColor,	sizeof(UINT8));
			pPacket->ReadData(&ui8ChatSize,		sizeof(UINT8));
			pPacket->ReadData(strChating,		sizeof(TTCHAR) * ui8ChatSize);

			strChating[ NET_CLAN_CHATING_SIZE - 1 ] = '\0';

			ClanMemChatting( &stUserInfo, ui8ChatSize, strChating, bIsGMUser, ui8NickColor );
		}
		break;
	case PROTOCOL_CS_PAGE_CHATTING_REQ:
		{
			UINT8	ui8ChatSize;
			UINT8	ui8NickColor;
			TTCHAR	strChating[NET_CLAN_CHATING_SIZE];
			bool	bIsGMUser;

			pPacket->ReadData(&bIsGMUser,		sizeof(bool));
			pPacket->ReadData(&ui8NickColor,	sizeof(UINT8));
			pPacket->ReadData(&ui8ChatSize,		sizeof(UINT8));
			pPacket->ReadData(strChating,		sizeof(TTCHAR) * ui8ChatSize);

			strChating[ NET_CLAN_CHATING_SIZE - 1 ] = '\0';

			ClanPageChatting( &stUserInfo, ui8ChatSize, strChating, bIsGMUser, ui8NickColor );
		}
		break;
	case PROTOCOL_CS_REPLACE_NOTICE_REQ:	// 작업완
		{
			UINT8	ui8Size;
			TTCHAR	strNotice[ NET_CLAN_NOTICE_SIZE ];

			pPacket->ReadData( &ui8Size,		sizeof(UINT8) );	LENGTH_CHECK( ui8Size, NET_CLAN_NOTICE_SIZE );
			pPacket->ReadData( strNotice,		sizeof(TTCHAR) * ui8Size );
			strNotice[ NET_CLAN_NOTICE_SIZE-1 ] = '\0';

			ClanReplaceNotice( &stUserInfo, strNotice );
		}
		break;

	case PROTOCOL_CS_REPLACE_INTRO_REQ:		// 작업완
		{
			UINT8	ui8Size;
			TTCHAR	strIntro[ NET_CLAN_INTRO_SIZE ];

			pPacket->ReadData( &ui8Size,		sizeof(UINT8) );	
			LENGTH_CHECK( ui8Size, NET_CLAN_INTRO_SIZE );
			pPacket->ReadData( strIntro,		sizeof(TTCHAR) * ui8Size );
			strIntro[ NET_CLAN_INTRO_SIZE-1 ] = '\0';

			ClanReplaceIntro( &stUserInfo, strIntro );
		}
		break;	
	case PROTOCOL_CS_REPLACE_NAME_REQ:		// 작업완
		{	// -- 현우 일감 --	사이즈 넣기
			TTCHAR	strName[ NET_CLAN_NAME_SIZE  ];

			pPacket->ReadData( strName,						sizeof(TTCHAR) * NET_CLAN_NAME_SIZE );
			strName[ NET_CLAN_NAME_SIZE-1 ] = '\0';

			ClanReplaceClanName( &stUserInfo, strName );
		}
		break;
	case PROTOCOL_CS_CHECK_MARK_REQ:
		{
			UINT32	ui32Mark;

			pPacket->ReadData(&ui32Mark,		sizeof(UINT32));

			ClanCheckMark( &stUserInfo, ui32Mark );
		}
		break;	
	case PROTOCOL_CS_REPLACE_MARK_REQ:
		{
			UINT32	ui32Mark;

			pPacket->ReadData(&ui32Mark,		sizeof(UINT32));

			ClanReplaceMark( &stUserInfo, ui32Mark );
		}
		break;
	case PROTOCOL_CS_REPLACE_PERSONMAX_REQ:
		{
			UINT16	ui16AddPersonCount = 0;

			pPacket->ReadData( &ui16AddPersonCount,			sizeof(UINT16) );

			ClanReplacePersonMax( &stUserInfo, ui16AddPersonCount );
		}
		break;
	case PROTOCOL_CS_REPLACE_MANAGEMENT_REQ:
		{
			CLAN_MANAGEMENT		Management;
			
			pPacket->ReadData(&Management,		sizeof(CLAN_MANAGEMENT));

			ClanReplaceManagement( &stUserInfo, &Management );
		}
		break;
	case PROTOCOL_CS_NOTE_REQ:
		{
			UINT8	ui8Type;
			UINT8	ui8NoteSize;
			TTCHAR	strNote[ NET_NOTE_SIZE ];

			pPacket->ReadData(&ui8Type,			sizeof(UINT8));
			pPacket->ReadData(&ui8NoteSize,		sizeof(UINT8));
			pPacket->ReadData(strNote,			sizeof(TTCHAR) * ui8NoteSize);

			strNote[ NET_NOTE_SIZE - 1 ] = '\0';

			ClanNote( &stUserInfo, ui8Type, ui8NoteSize, strNote );
		}
		break;
	case PROTOCOL_CS_ROOM_INVITED_REQ:
		{
			T_UID i64UID;			
			pPacket->ReadData(&i64UID,	sizeof(T_UID));

			ClanMemberInvitedRoom( &stUserInfo, i64UID );
		}
		break;
	case PROTOCOL_CS_CHECK_JOIN_AUTHORITY_REQ:
		{
			UINT8 ui8Age;
			UINT8 ui8Rank;

			pPacket->ReadData(&ui8Age,	sizeof(UINT8));
			pPacket->ReadData(&ui8Rank,	sizeof(UINT8));

			ClanCheckJoinAuthority( &stUserInfo, ui8Age, ui8Rank );
		}
		break;	
	case PROTOCOL_CS_REPLACE_MEMBER_NICK_REQ:
		{
			READ_CLAN_WAR_INFO;

			pPacket->ReadData(stMemberInfo.m_strNick,						sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
			stMemberInfo.m_strNick[ NET_NICK_NAME_SIZE - 1 ] = '\0';

			ClanReplaceMemberNick( &stClanWarInfo, &stMemberInfo );
		}
		break;
	case PROTOCOL_CS_REPLACE_MEMBER_COLOR_NICK_REQ:
		{
			READ_CLAN_WAR_INFO;

			pPacket->ReadData( &stMemberInfo.m_ui8NickColor,		sizeof(UINT8));

			ClanReplaceMemberColorNick( &stClanWarInfo, &stMemberInfo );
		}
		break;
	case PROTOCOL_CS_REPLACE_MEMBER_RANK_REQ:
		{
			READ_CLAN_WAR_INFO;

			pPacket->ReadData( &stMemberInfo.m_ui8Rank,		sizeof(UINT8));

			ClanReplaceMemberRank( &stClanWarInfo, &stMemberInfo  );
		}
		break;
	case PROTOCOL_CS_RECORD_RESET_REQ:
		{
			ClanBattleRecordReset( &stUserInfo );
		}
		break;
	case PROTOCOL_CS_POINT_RESET_REQ:
		{
			ClanBattlePointReset( &stUserInfo );
		}
		break;
	case PROTOCOL_CS_EXPUP_REQ:
		{
			UINT32 ui32ItemID, ui32Date;
			pPacket->ReadData( &ui32ItemID,		sizeof(UINT32));
			pPacket->ReadData( &ui32Date,		sizeof(UINT32));

			ClanBattleExpUp( &stUserInfo, ui32ItemID, ui32Date );
		}
		break;
	case PROTOCOL_CS_CHECK_DUPLICATE_REQ:
		{
			UINT8	ui8Size;
			TTCHAR	strClanName[ NET_CLAN_NAME_SIZE ];
			
			pPacket->ReadData(&ui8Size,		sizeof(UINT8));
			pPacket->ReadData(strClanName,	sizeof(TTCHAR) * ui8Size);
			strClanName[ NET_CLAN_NAME_SIZE - 1 ] = '\0';

			ClanCheckName( &stUserInfo, strClanName );
		}
		break;
	case PROTOCOL_CS_CHECK_DUPLICATE_AZIT_URL_REQ:
		{
			UINT8	ui8Size;
			TTCHAR	strAzit[ NET_CLAN_AZIT_URL_SIZE ];
			
			pPacket->ReadData(&ui8Size,		sizeof(UINT8));
			pPacket->ReadData(strAzit,		sizeof(TTCHAR) * ui8Size);
			strAzit[ NET_CLAN_AZIT_URL_SIZE - 1 ] = '\0';

			ClanCheckAzit( &stUserInfo, strAzit );
		}
		break;
	case PROTOCOL_CS_REPLACE_COLOR_NAME_REQ:
		{
			UINT32 ui32ItemID, ui32Date;
			UINT8 ui8Color;
			pPacket->ReadData( &ui32ItemID,		sizeof(UINT32));
			pPacket->ReadData( &ui32Date,		sizeof(UINT32));
			pPacket->ReadData( &ui8Color,		sizeof(UINT8));

			ClanBattleReplaceColorName( &stUserInfo, ui32ItemID, ui32Date, ui8Color );
		}
		break;	
	case PROTOCOL_CS_INVITE_REQ:
		{
			T_UID	i64UID;
		
			pPacket->ReadData( &i64UID,						sizeof(T_UID) );

			ClanInvite( &stUserInfo, i64UID );
		}
		break;
	
	case PROTOCOL_CS_CLAN_ITEM_DELETE_REQ:
		{
			UINT32 ui32ItemID;
			pPacket->ReadData( &ui32ItemID,					sizeof(UINT32) );

			ClanItemDelete( &stUserInfo, ui32ItemID );
		}
		break;
	
	 case PROTOCOL_CS_USER_CHANGE_STATE_REQ:			 	 
		{
			FRIEND_STATE eState;
			pPacket->ReadData( &eState,						sizeof(FRIEND_STATE) );

			CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2( stUserInfo.ui32ClanDBIdx );
			if( NULL == pstClan ) break;

			CLAN_MEMBER_DETAIL_INFO* pstMember = (CLAN_MEMBER_DETAIL_INFO*)pstClan->_pMember->FindItemEx( stUserInfo.i64UID );
			if( pstMember ) 
			{
				UINT32 ui32Match		= (UINT32)GET_FRIEND_MATCH( pstMember->_state );
				UINT32 ui32Channel		= GET_FRIEND_CHANNEL( (UINT32)pstMember->_state );
				UINT32 ui32RoomIdx		= GET_FRIEND_ROOMIDX( pstMember->_state );

				pstMember->_state		= MAKE_FRIEND_MATCH_STATE( ui32Match, eState, ui32idxBuffer, ui32Channel, ui32RoomIdx );
				pstMember->_session		= stUserInfo.i32SessionIdx;

				SendClanMemberStateChange( pstClan, pstMember );
				break;
			}

			CLAN_REQUEST_SERVER_INFO*	pstRequest	= (CLAN_REQUEST_SERVER_INFO*)pstClan->_pRequest->FindItemEx( stUserInfo.i64UID );
			if( !pstRequest ) break;

			UINT32 ui32Match		= (UINT32)GET_FRIEND_MATCH( pstRequest->_state );
			UINT32 ui32Channel		= GET_FRIEND_CHANNEL( pstRequest->_state );
			UINT32 ui32RoomIdx		= GET_FRIEND_ROOMIDX( pstRequest->_state );

			pstRequest->_state		= MAKE_FRIEND_MATCH_STATE( ui32Match, eState, ui32idxBuffer, ui32Channel, ui32RoomIdx );
			pstRequest->_sessionIdx	= stUserInfo.i32SessionIdx;

		}
		break;




	case PROTOCOL_GC_CLAN_USER_INFO_REQ:
		{
			PACKET_GC_NEW_CLAN_USER_INFO_REQ	PacketReqGC;
			pPacket->ReadData( &PacketReqGC,	sizeof(PACKET_GC_NEW_CLAN_USER_INFO_REQ) );

			stUserInfo.ui32ServerIdx	= ui32idxBuffer;	
			stUserInfo.i32SessionIdx	= PacketReqGC.m_i32SessionIdx;
			stUserInfo.i64UID			= PacketReqGC.m_i64UID;
			
			ClanFindUserInfo( &stUserInfo, PacketReqGC.m_i64FindUID );
		}
		break;
	case PROTOCOL_CS_CLAN_MATCH_RESULT_CONTEXT_REQ:
		{
			PACKET_GC_CS_CLAN_MATCH_RESULT_CONTEXT_REQ	PacketReqGC;
			pPacket->ReadData( &PacketReqGC,				sizeof(PACKET_GC_CS_CLAN_MATCH_RESULT_CONTEXT_REQ) );			

			// 사용안함.
			//SendMatchResultContext( ui32idxBuffer, &PacketReqGC );
		}
		break;
	case PROTOCOL_CS_CLAN_MATCH_RESULT_LIST_REQ:
		{
			PACKET_GC_CS_CLAN_MATCH_RESULT_LIST_REQ		PacketReqGC;
			pPacket->ReadData( &PacketReqGC,	sizeof(PACKET_GC_CS_CLAN_MATCH_RESULT_LIST_REQ) );

			stUserInfo.ui32ServerIdx	= ui32idxBuffer;
			stUserInfo.i32SessionIdx	= PacketReqGC.m_i32SessionIdx;
			stUserInfo.i64UID			= PacketReqGC.m_i64UID;
			stUserInfo.ui32ClanDBIdx	= PacketReqGC.m_ui32CUID;

			// 사용안함.
			//SendMatchResultList( ui32idxBuffer, &PacketReqGC );
		}
		break;
	case PROTOCOL_CS_CLAN_MATCH_RESULT_EMOTION_SAVE_REQ:
		{
			PACKET_GC_CS_CLAN_MATCH_RESULT_EMOTION_SAVE_REQ	PacketReqGC;
			pPacket->ReadData( &PacketReqGC,	sizeof(PACKET_GC_CS_CLAN_MATCH_RESULT_EMOTION_SAVE_REQ) );
			
			// 사용안함.
			//ClanMatchResultEmotionSave( ui32idxBuffer, &PacketReqGC );
		}
		break;
	case PROTOCOL_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_REQ:
		{
			PACKET_GC_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_REQ	PacketReqGC;
			pPacket->ReadData( &PacketReqGC,	sizeof(PACKET_GC_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_REQ) );

			ClanRequestAutoAcceptOption( ui32idxBuffer, &PacketReqGC );
		}
		break;


	 default:
		 return FALSE;
	}

	return TRUE;
}
  