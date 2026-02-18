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

BOOL	CUserSession::_PacketParsingClan( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled )
{
	BOOL		bSendMessage = FALSE;
	INT32		i32Temp;
	PROTOCOL	ProtocolID = pPacket->GetProtocolID(); 
	
	switch( ProtocolID )
	{
	case PROTOCOL_CS_CLIENT_ENTER_REQ :			//080303
		{
			CHECKVALID_CHANNEL;

			if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
			{
				m_pRoom->MoveClan(this, TRUE); 
			}

			bSendMessage = TRUE; 
			pSendPacket->SetProtocol( PROTOCOL_CS_CLIENT_ENTER_ACK ); 
			pSendPacket->WriteData( &m_UserInfoBasic._clanidx,		sizeof(INT32)); 
			pSendPacket->WriteData( &m_UserInfoBasic._clanstate,	sizeof(INT32));		// 모든 서버 함께 수정합니다..
			
			m_i32SubTask = GAME_TASK_SUB_CLAN;

			if(m_UserInfoBasic._clanidx != 0 && m_UserInfoBasic._clanstate != 0)
			{
				//클랜에 가입이 되어 있다.
				// 클랜 서버에 알려줌
				g_pModuleClan->ClanSendPageEnter(m_WorkThreadIdx, this );
			}
			else
			{
				//가입된 클랜이 없다. 
				pSendPacket->WriteData( g_pContextMain->GetClanContext(), sizeof(CLAN_CONTEXT));	//클랜 컨텍스트를 보낸다. 
			}
		}
		break;
	case PROTOCOL_CS_CLIENT_CLAN_CONTEXT_REQ:		//081224
		{
			CHECKVALID_DBINDEX;

			bSendMessage = TRUE;
			pSendPacket->SetProtocol( PROTOCOL_CS_CLIENT_CLAN_CONTEXT_ACK ); 
			pSendPacket->WriteData( g_pContextMain->GetClanContext(), sizeof(CLAN_CONTEXT));	//클랜 컨텍스트를 보낸다. 
		}
		break;
	case PROTOCOL_CS_CLIENT_LEAVE_REQ :				//080303
		{
			CHECKVALID_CHANNEL;

			//방에 있을때는 방에 대한 정보도 표시해줘야 한다. 
			if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
			{
				m_pRoom->MoveClan(this, FALSE); 
			}
			m_i32SubTask = GAME_TASK_SUB_NONE;

			if(m_UserInfoBasic._clanidx != 0 && m_UserInfoBasic._clanstate != 0)
			{	//클랜에 가입이 되어 있다. // 클랜 서버에 알려줌
				g_pModuleClan->ClanSendPageLeave(m_WorkThreadIdx, this );
			}

			bSendMessage = TRUE; 
			pSendPacket->SetProtocol( PROTOCOL_CS_CLIENT_LEAVE_ACK ); 
		}
		break; 
	case PROTOCOL_CS_CREATE_CLAN_REQ :		//080304	081224
		{
			CHECKVALID_CHANNEL;

			//1. 자신이 클랜을 생성할수 있는 확인한다. 
			UINT32 ui32Rank = m_UserInfoBasic._rank;
			if( -1 != GetCheckCashitem( MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_DISGUISE_RANK, CASHITEM_DAY_NO) ) )
			{
				ui32Rank = m_UserInfoInvItemData._wRank;
			}
			if( ui32Rank < g_pContextMain->m_ui8ClanCreateRank || m_UserInfoBasic._point < (UINT32)g_pContextMain->m_i32ClanCreatePoint )
			{
				i32Temp = EVENT_ERROR_FAIL;
				bSendMessage = TRUE; 
				pSendPacket->SetProtocol( PROTOCOL_CS_CREATE_CLAN_ACK ); 
				pSendPacket->WriteData(&i32Temp,	sizeof(INT32));

				I3TRACE("Create Clan Rank Fail"); 
			}
			else
			{
				UINT32 ui32Mark;
				UINT8 ui8Name, ui8Intro, ui8Azit;
				char strClanName[NET_CLAN_NAME_SIZE]; 
				char strClanIntro[NET_CLAN_INTRO_SIZE];
				char strClanAzit[NET_CLAN_AZIT_URL_SIZE];

				pPacket->ReadData( &ui8Name,	sizeof(UINT8) );	LENGTH_CHECK( ui8Name, NET_CLAN_NAME_SIZE );
				pPacket->ReadData( &ui8Intro,	sizeof(UINT8) ); 	LENGTH_CHECK( ui8Intro, NET_CLAN_INTRO_SIZE );
				pPacket->ReadData( &ui8Azit,	sizeof(UINT8) ); 	LENGTH_CHECK( ui8Azit, NET_CLAN_AZIT_URL_SIZE );

				pPacket->ReadData( strClanName,	ui8Name ); 
				pPacket->ReadData( strClanIntro,ui8Intro ); 
				pPacket->ReadData( strClanAzit,	ui8Azit ); 
				pPacket->ReadData( &ui32Mark,	sizeof( UINT32)); 

				strClanName[NET_CLAN_NAME_SIZE - 1] = 0; 
				strClanIntro[NET_CLAN_INTRO_SIZE - 1] = 0; 
				strClanAzit[NET_CLAN_AZIT_URL_SIZE - 1] = 0; 

				if( _CheckClanName( strClanName ) )
				{	// 클랜 이름을 정상으로 사용할 수 있는지 검사
					g_pModuleClan->ClanSendCreate(m_WorkThreadIdx, this, strClanName, strClanIntro, strClanAzit, ui32Mark);
				}
				else
				{
					i32Temp = EVENT_ERROR_FAIL;
					bSendMessage = TRUE; 
					pSendPacket->SetProtocol( PROTOCOL_CS_CREATE_CLAN_ACK ); 
					pSendPacket->WriteData(&i32Temp,	sizeof(INT32));
				}
			}
		}
		break;
	case PROTOCOL_CS_CLIENT_CLAN_LIST_REQ:			//클랜 페이지 던지기	//081224
		{
			CHECKVALID_CHANNEL;

			CLAN_CONTEXT* pClanContext = g_pContextMain->GetClanContext();

			UINT32	ui32SectionIdx;					// 클랜서버와 함께 수정합니다..
			UINT16	ui16SectionIdx;
			UINT8	ui8Sectionsize = pClanContext->_sectionSize;  
			pPacket->ReadData( &ui32SectionIdx, sizeof(UINT32) );

			ui16SectionIdx = (UINT16)ui32SectionIdx;
			if( ui16SectionIdx == (pClanContext->_sectionCount-1) )
			{
				ui8Sectionsize = pClanContext->_totalCount % pClanContext->_sectionSize;
			}

			if( pClanContext->_sectionCount > ui16SectionIdx )
			{
				CLAN_BASIC_INFO* pClanList = g_pContextMain->GetClanList();

				UINT32 ui32SectionIdx = ui16SectionIdx;
				bSendMessage = TRUE; 			
				pSendPacket->SetProtocol( PROTOCOL_CS_CLIENT_CLAN_LIST_ACK ); 
				pSendPacket->WriteData( &ui32SectionIdx,	sizeof(UINT32) );
				pSendPacket->WriteData( &ui8Sectionsize,	sizeof(UINT8) );
				pSendPacket->WriteData( &pClanList[ui16SectionIdx * pClanContext->_sectionSize], (sizeof(CLAN_BASIC_INFO) * ui8Sectionsize) ); 
			}
			else
			{
				m_ui32KillReason	= KILL_REASON_HACK_PACKET;
				bisKilled		= TRUE;
			}
		}
		break; 
	case PROTOCOL_CS_CLIENT_CHECK_DUPLICATE_REQ:	//중복 검사 하기	080304		081224
		{
			CHECKVALID_CHANNEL;

			UINT8	ui8NameSize;
			char	strClanName[NET_CLAN_NAME_SIZE];

			pPacket->ReadData( &ui8NameSize,	sizeof(UINT8) );		LENGTH_CHECK( ui8NameSize, NET_CLAN_NAME_SIZE );
			pPacket->ReadData( strClanName,		ui8NameSize ); 
			strClanName[NET_CLAN_NAME_SIZE - 1] = 0; 

			if( _CheckClanName( strClanName ) )
			{	// 클랜 이름 사용이 가능한지 확인
				g_pModuleClan->ClanSendDuplicateName(m_WorkThreadIdx, this, strClanName ); 
			}
			else
			{
				bSendMessage = TRUE; 
				i32Temp = EVENT_ERROR_FAIL;
				pSendPacket->SetProtocol( PROTOCOL_CS_CLIENT_CHECK_DUPLICATE_ACK ); 
				pSendPacket->WriteData(&i32Temp,	sizeof(INT32));
			}
		}
		break; 
	case PROTOCOL_CS_CHECK_DUPLICATE_AZIT_URL_REQ:	//중북 검사하기		080304
		{
			CHECKVALID_CHANNEL;

			UINT8	ui8NameSize;
			char	strAzitName[NET_CLAN_AZIT_URL_SIZE]; 
			pPacket->ReadData(&ui8NameSize,		sizeof(UINT8) );		LENGTH_CHECK( ui8NameSize, NET_CLAN_NAME_SIZE );
			pPacket->ReadData(strAzitName,		ui8NameSize );
			strAzitName[NET_CLAN_AZIT_URL_SIZE - 1] = 0; 

			g_pModuleClan->ClanSendDuplicateAzit( m_WorkThreadIdx, this, strAzitName ); 
		}
		break; 
	case PROTOCOL_CS_JOIN_REQUEST_REQ		:	//클랜 조인 요청 하기	080304
		{
			CHECKVALID_CHANNEL;

			UINT8	ui8Size;
			UINT32	ui32ClanDBIdx; 
			char	strContents[NET_CLAN_REQUEST_SIZE]; 

			pPacket->ReadData(&ui32ClanDBIdx,	sizeof(UINT32));
			pPacket->ReadData(&ui8Size,			sizeof(UINT8));				LENGTH_CHECK( ui8Size, NET_CLAN_REQUEST_SIZE );
			pPacket->ReadData(strContents,		ui8Size ); 

			strContents[ NET_CLAN_REQUEST_SIZE - 1 ] = '\0';

			UINT8 ui8Rank = (UINT8)m_UserInfoBasic._rank;
			if( -1 != GetCheckCashitem( MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_DISGUISE_RANK, CASHITEM_DAY_NO) ) )
			{
				ui8Rank = (UINT8)m_UserInfoInvItemData._wRank;
			}

			g_pModuleClan->ClanSendJoinRequest(m_WorkThreadIdx, this, ui32ClanDBIdx, strContents, m_ui8Age, ui8Rank ); 
		}
		break;
	case PROTOCOL_CS_CANCEL_REQUEST_REQ	:				// 가입신청 취소 요청	080304
		{
			CHECKVALID_CHANNEL;
			g_pModuleClan->ClanSendCancelRequest( m_WorkThreadIdx, this ); 
		}
		break; 
	case PROTOCOL_CS_DETAIL_INFO_REQ	:				// 클랜 상세정보 요청	080304
		{
			CHECKVALID_CHANNEL;
			INT32 i32ClanIdx; 
			UINT8 ui8Type;
			pPacket->ReadData(&i32ClanIdx,	sizeof(UINT32));
			pPacket->ReadData(&ui8Type,		sizeof(UINT8));
			g_pModuleClan->ClanSendDetailInfo(m_WorkThreadIdx, this, i32ClanIdx, ui8Type); 
		}
		break; 
	case PROTOCOL_CS_MEMBER_CONTEXT_REQ	:				// 클랜멤버 목록 정보 요청	080304
		{
			CHECKVALID_CHANNEL;

			i32Temp = EVENT_ERROR_SUCCESS; 
			if(m_UserInfoBasic._clanidx		== 0)
				i32Temp = SERVER_VALUE_UNKNOWN; 
			if(m_UserInfoBasic._clanstate	== CLAN_MEMBER_LEVEL_UNKNOWN )
				i32Temp = SERVER_VALUE_UNKNOWN; 

			if(i32Temp == EVENT_ERROR_SUCCESS)
			{
				g_pModuleClan->ClanSendMemberContext(m_WorkThreadIdx, this ); 
			}
			else
			{
				I3TRACE("Clan Member Context Req Error"); 
      			bSendMessage = TRUE; 
				pSendPacket->SetProtocol( PROTOCOL_CS_MEMBER_CONTEXT_ACK ); 
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) ); 
			}
		}
		break; 
	case PROTOCOL_CS_MEMBER_LIST_REQ	:				// 클랜멤버 목록 보기 요청	080304
		{
			CHECKVALID_CHANNEL;

			i32Temp = EVENT_ERROR_SUCCESS; 
			if(m_UserInfoBasic._clanidx	  == 0)
				i32Temp = SERVER_VALUE_UNKNOWN; 
			if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_UNKNOWN )
				i32Temp = SERVER_VALUE_UNKNOWN; 

			if(i32Temp == EVENT_ERROR_SUCCESS)
			{
				UINT8 ui8section; 
				pPacket->ReadData(&ui8section, sizeof(UINT8)); 
				g_pModuleClan->ClanSendMemberList(m_WorkThreadIdx, this, ui8section); 
			}
			else
			{
				I3TRACE("Clan Member Context List Error"); 
      			bSendMessage = TRUE; 
				pSendPacket->SetProtocol( PROTOCOL_CS_MEMBER_LIST_ACK );
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) ); 
			}
		}
		break; 
	case PROTOCOL_CS_MEMBER_DETAIL_INFO_REQ:			// 081224
		{
			CHECKVALID_CHANNEL;
			i32Temp = EVENT_ERROR_SUCCESS; 
			if(m_UserInfoBasic._clanidx	  == 0)i32Temp = SERVER_VALUE_UNKNOWN; 
			if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_UNKNOWN )i32Temp = SERVER_VALUE_UNKNOWN;

			if(i32Temp == EVENT_ERROR_SUCCESS)
			{			
				TUID ClanMemberUID; 
				pPacket->ReadData(&ClanMemberUID, sizeof(TUID)); 
				g_pModuleClan->ClanSendMemberDetailInfo(m_WorkThreadIdx, this, ClanMemberUID);
			}
			else
			{
				I3TRACE("Clan Member Detail  Error"); 
      			bSendMessage = TRUE; 
				pSendPacket->SetProtocol(PROTOCOL_CS_MEMBER_DETAIL_INFO_ACK);
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) );
			}
		}
		break;
	case PROTOCOL_CS_REQUEST_CONTEXT_REQ:				// 가입신청서 목록 정보 요청	080304
		{
			CHECKVALID_CHANNEL;
			i32Temp = EVENT_ERROR_SUCCESS; 
			if(m_UserInfoBasic._clanidx	  == 0)i32Temp = SERVER_VALUE_UNKNOWN;  
			if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_UNKNOWN )i32Temp = SERVER_VALUE_UNKNOWN;
			if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_REGULAR )i32Temp = SERVER_VALUE_UNKNOWN;
			if(i32Temp == EVENT_ERROR_SUCCESS)
			{
				g_pModuleClan->ClanSendRequestContext(m_WorkThreadIdx, this ); 
			}
			else
			{
				I3TRACE("Clan Request Context Error"); 
      			bSendMessage = TRUE; 
				pSendPacket->SetProtocol(PROTOCOL_CS_REQUEST_CONTEXT_ACK);
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) );
			}
		}
		break; 
	case PROTOCOL_CS_REQUEST_LIST_REQ	:				// 가입신청서 목록 보기 요청	080304
		{
			CHECKVALID_CHANNEL;
			i32Temp = EVENT_ERROR_SUCCESS; 
			if(m_UserInfoBasic._clanidx	  == 0)i32Temp = SERVER_VALUE_UNKNOWN; 
			if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_UNKNOWN )i32Temp = SERVER_VALUE_UNKNOWN;
			if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_REGULAR )i32Temp = SERVER_VALUE_UNKNOWN;

			if(i32Temp == EVENT_ERROR_SUCCESS)
			{			
				UINT8 ui8Section; 
				pPacket->ReadData(&ui8Section, sizeof(UINT8)); 
				g_pModuleClan->ClanSendRequestList(m_WorkThreadIdx, this, ui8Section);
			}
			else
			{
				I3TRACE("Clan Request List Error"); 
      			bSendMessage = TRUE; 
				pSendPacket->SetProtocol(PROTOCOL_CS_REQUEST_LIST_ACK);
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) );
			}
		}
		break;
	case PROTOCOL_CS_REQUEST_INFO_REQ	:				// 가입신청서 상세정보 보기 요청	080304
		{
			CHECKVALID_CHANNEL;
			i32Temp = EVENT_ERROR_SUCCESS; 
			if(m_UserInfoBasic._clanidx	  == 0)i32Temp = SERVER_VALUE_UNKNOWN; 
			if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_UNKNOWN )i32Temp = SERVER_VALUE_UNKNOWN;
			if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_REGULAR )i32Temp = SERVER_VALUE_UNKNOWN;
			if(i32Temp == EVENT_ERROR_SUCCESS)
			{			
				TUID i64RequestIdx;
				pPacket->ReadData(&i64RequestIdx, sizeof(TUID)); 
				g_pModuleClan->ClanSendRequestInfo(m_WorkThreadIdx, this, i64RequestIdx);
			}
			else
			{
				I3TRACE("Clan Request Info Error"); 
      			bSendMessage = TRUE; 
				pSendPacket->SetProtocol(PROTOCOL_CS_REQUEST_INFO_ACK);
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) );
			}
		}
		break; 
	case PROTOCOL_CS_ACCEPT_REQUEST_REQ	:				// 가입신청서 수락 요청	080304
		{
			CHECKVALID_CHANNEL;
			i32Temp = EVENT_ERROR_SUCCESS; 
			if(m_UserInfoBasic._clanidx	  == 0)i32Temp = SERVER_VALUE_UNKNOWN; 
			if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_UNKNOWN )i32Temp = SERVER_VALUE_UNKNOWN;
			if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_REGULAR )i32Temp = SERVER_VALUE_UNKNOWN;

			if(i32Temp == EVENT_ERROR_SUCCESS)
			{			
				UINT8	ui8count; 
				INT64	ai64List[ MAX_MEMBER_COUNT ];
				pPacket->ReadData(&ui8count,	sizeof(UINT8) );
				if( ui8count > MAX_MEMBER_COUNT )
				{
					break;
				}
				pPacket->ReadData(ai64List,		(sizeof(TUID) * ui8count) ); 
				g_pModuleClan->ClanSendAcceptRequest(m_WorkThreadIdx, this, ui8count ,ai64List);
			}
			else
			{
				I3TRACE("Clan Accept Request Error"); 
      			bSendMessage = TRUE; 
				pSendPacket->SetProtocol(PROTOCOL_CS_ACCEPT_REQUEST_ACK);
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) );
			}
		}
		break; 
	case PROTOCOL_CS_DENIAL_REQUEST_REQ	:				// 가입신청서 거절 요청			
		{
			CHECKVALID_CHANNEL;
			i32Temp = EVENT_ERROR_SUCCESS; 
			if(m_UserInfoBasic._clanidx	  == 0)i32Temp = SERVER_VALUE_UNKNOWN; 
			if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_UNKNOWN )i32Temp = SERVER_VALUE_UNKNOWN;
			if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_REGULAR )i32Temp = SERVER_VALUE_UNKNOWN;

			if(i32Temp == EVENT_ERROR_SUCCESS)
			{			
				UINT8	ui8count; 
				INT64	ai64List[ MAX_MEMBER_COUNT ];
				pPacket->ReadData(&ui8count,	sizeof(UINT8)); 
				if( ui8count > MAX_MEMBER_COUNT ){	break;	}		//카운터 이상 
				pPacket->ReadData(ai64List,		sizeof(INT64) * ui8count);
				g_pModuleClan->ClanSendDenialRequest(m_WorkThreadIdx, this, ui8count ,ai64List);
			}
			else
			{
				I3TRACE("Clan Denial Request Error"); 
      			bSendMessage = TRUE; 
				pSendPacket->SetProtocol(PROTOCOL_CS_DENIAL_REQUEST_ACK);
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) );
			}
		}
		break;
	case PROTOCOL_CS_INVITE_REQ:
		{
			CHECKVALID_CHANNEL;

			INT32	i32Idx;
			TUID	i64ClientUID;
			UINT8	ui8Room;
			TUID	i64UID = 0;
			

			pPacket->ReadData(&ui8Room,  sizeof(UINT8));

			if(0 == ui8Room)	{pPacket->ReadData(&i64ClientUID,	sizeof(TUID));}
			else				{pPacket->ReadData(&i32Idx,			sizeof(INT32));}

			switch( ui8Room )
			{
				case 0:
					i64UID = i64ClientUID;
					break;
				case 1:		// 1일경우 방에 있는 상태로 Idx 에는 SlotIdx 가 날아 옵니다.
					if( m_i32RoomIdx != SERVER_VALUE_UNKNOWN )
					{
						CUserSession* pSession = m_pRoom->GetSession( i32Idx );
						if( pSession )
						{
							i64UID = pSession->m_i64UID;
						}
					}
					break;
				case 2:		// 2일경우 SessionIdx가 날아 옵니다.
					{
						CUserSession* pSession = g_pUserManager->GetSession(i32Idx);
						if( pSession )
						{
                            i64UID = pSession->m_i64UID;
						}
					}
					break;
			}

			if( 0 < i64UID )
			{
				g_pModuleClan->ClanSendInvite(m_WorkThreadIdx, this, i64UID );
			}
			// 기존 클랜초대의 경우, 메신져서버에 쪽지 발송후에 무조건 성공메세지 무조건 정상처리하였음.
			// 그렇기때문에, 쪽지의 에러처리에 대응하지 못함. 
			// 수정: 쪽지 에러처리는 메신져 서버에서 대응 하고, 기존 게임서버에 있던 성공처리 주석..(아래 코드 두줄)
//			pSendPacket->SetProtocol(PROTOCOL_CS_INVITE_ACK);
//			pSendPacket->WriteData( &Temp, sizeof(INT32) );
		}
		break;
	case PROTOCOL_CS_INVITE_ACCEPT_REQ:
		{
			CHECKVALID_CHANNEL;
			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic._clanstate != CLAN_MEMBER_LEVEL_UNKNOWN )	i32Temp = EVENT_ERROR_CLAN_MEMBER;

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				UINT32 ui32ClanDBIdx;
				UINT32 ui32ServerIdx;
				UINT8  ui8Accept;

				INT32 i32RoomIdx = 0;

				pPacket->ReadData(&ui32ClanDBIdx,	sizeof(UINT32));
				pPacket->ReadData(&ui32ServerIdx,	sizeof(UINT32));
				pPacket->ReadData(&ui8Accept,		sizeof(UINT8));

				if( m_i32RoomIdx != SERVER_VALUE_UNKNOWN )
				{
					i32RoomIdx = m_i32RoomIdx;
				}

				UINT32 ui32State = MAKE_FRIEND_STATE( FRIEND_STATE_ONLINE, ui32ServerIdx, m_i32ChannelNum, i32RoomIdx );

				g_pModuleClan->ClanSendInviteAcceptRequest( m_WorkThreadIdx, this, ui32ClanDBIdx, ui32State, ui8Accept );
			}
			else 
			{
				bSendMessage = TRUE;
				pSendPacket->SetProtocol(PROTOCOL_CS_INVITE_ACCEPT_ACK);
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) );
			}
		}
		break;
	case PROTOCOL_CS_SECESSION_CLAN_REQ:							// 클랜 탈퇴 요청
		{
			CHECKVALID_CHANNEL;
			i32Temp = EVENT_ERROR_SUCCESS; 
			if(m_UserInfoBasic._clanidx	== 0)
			{
				i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX; 
			}
			else
			{
				if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_UNKNOWN )	i32Temp = EVENT_ERROR_CLAN_NOMEMBER;
				if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_MASTER )		i32Temp = EVENT_ERROR_CLAN_LEVEL_MASTER;			
			}

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				g_pModuleClan->ClanSendSecessionRequest(m_WorkThreadIdx, this );
			}
			else
			{
				I3TRACE( "Clan Secession Request Error"); 
				bSendMessage = TRUE; 
				pSendPacket->SetProtocol(PROTOCOL_CS_SECESSION_CLAN_ACK);
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) );
			}
		}
		break;
	case PROTOCOL_CS_CLOSE_CLAN_REQ:								// 클랜 해체 요청
		{
			CHECKVALID_CHANNEL;
			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic._clanidx	  == 0)	
			{
				i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			}
			else 
			{
				if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_UNKNOWN )	i32Temp = EVENT_ERROR_CLAN_NOMEMBER;
				if(m_UserInfoBasic._clanstate != CLAN_MEMBER_LEVEL_MASTER )		i32Temp = EVENT_ERROR_CLAN_CLOSECLAN_FAIL;
			}

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				g_pModuleClan->ClanSendCloseRequest( m_WorkThreadIdx, this );
			}
			else
			{
				I3TRACE( "Clan Close Request Error"); 
				bSendMessage = TRUE; 
				pSendPacket->SetProtocol(PROTOCOL_CS_CLOSE_CLAN_ACK);
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) );
			}
		}
		break;
	case PROTOCOL_CS_COMMISSION_MASTER_REQ:							// 클랜 마스터 위임 요청 20080404
		{
			CHECKVALID_CHANNEL;
			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic._clanidx	  == 0)	
			{
				i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			}
			else 
			{
                if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_UNKNOWN )	i32Temp = EVENT_ERROR_CLAN_NOMEMBER;
				if(m_UserInfoBasic._clanstate != CLAN_MEMBER_LEVEL_MASTER )		i32Temp = EVENT_ERROR_CLAN_COMMISSION_FAIL;
			}

			TUID i64UID = 0;
			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				pPacket->ReadData( &i64UID,	sizeof(TUID) );	// -- 현우 일해라 --
				UINT32 wRank = CLAN_RANK_A2;//g_pDataBase->GetInvenItemDisguiseRank( m_WorkThreadIdx, Idx );
				if( wRank < CLAN_RANK_A2 )
				{
					i32Temp = EVENT_ERROR_FAIL;
				}
			}

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				g_pModuleClan->ClanSendCommissionMasterRequest(m_WorkThreadIdx, this, i64UID );
			}
			else
			{
				I3TRACE("Clan Master Commission Request Error"); 
      			bSendMessage = TRUE; 
				pSendPacket->SetProtocol(PROTOCOL_CS_COMMISSION_MASTER_ACK);
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) );
			}
		}
		break;
	case PROTOCOL_CS_COMMISSION_STAFF_REQ:
		{
			CHECKVALID_CHANNEL;
			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic._clanidx	  == 0)	
			{
				i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			}
			else 
			{
                if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_UNKNOWN )	i32Temp = EVENT_ERROR_CLAN_NOMEMBER;
				if(m_UserInfoBasic._clanstate != CLAN_MEMBER_LEVEL_MASTER )		i32Temp = EVENT_ERROR_CLAN_COMMISSION_FAIL;
			}

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				UINT8	ui8count;
				INT64	ai64Idx[ MAX_MEMBER_COUNT ];
				pPacket->ReadData( &ui8count,	sizeof(UINT8) );
				if( ui8count > MAX_MEMBER_COUNT ){	break; }
				pPacket->ReadData( ai64Idx,		sizeof(TUID)*ui8count );
				g_pModuleClan->ClanSendCommissionStaffRequest(m_WorkThreadIdx, this, ui8count, ai64Idx );
			}
			else
			{
				I3TRACE("Clan Staff Commission Request Error"); 
      			bSendMessage = TRUE; 
				pSendPacket->SetProtocol(PROTOCOL_CS_COMMISSION_STAFF_ACK);
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) );
			}
		}
		break;
	case PROTOCOL_CS_COMMISSION_REGULAR_REQ:
		{
			CHECKVALID_CHANNEL;
			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic._clanidx	  == 0)	
			{
				i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			}
			else 
			{
                if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_UNKNOWN )	i32Temp = EVENT_ERROR_CLAN_NOMEMBER;
				if(m_UserInfoBasic._clanstate != CLAN_MEMBER_LEVEL_MASTER )		i32Temp = EVENT_ERROR_CLAN_COMMISSION_FAIL;
			}

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				UINT8	ui8count;
				INT64	ai64Idx[  MAX_MEMBER_COUNT  ];
				pPacket->ReadData( &ui8count,	sizeof(UINT8) );
				if( ui8count > MAX_MEMBER_COUNT ){	break; }
				pPacket->ReadData( ai64Idx,		sizeof(TUID)*ui8count );
				g_pModuleClan->ClanSendCommissionRegularRequest(m_WorkThreadIdx, this, ui8count, ai64Idx );
			}
			else
			{
				I3TRACE("PROTOCOL_CS_COMMISSION_REGULAR_REQ Error"); 
      			bSendMessage = TRUE; 
				pSendPacket->SetProtocol(PROTOCOL_CS_COMMISSION_REGULAR_ACK);
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) );
			}
		}
		break;
	case PROTOCOL_CS_DEPORTATION_REQ:								// 클랜 강제 제명 요청 20080404
		{
			CHECKVALID_CHANNEL;
			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic._clanidx	  == 0)	
			{
				i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			}
			else 
			{
                if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_UNKNOWN )		i32Temp = EVENT_ERROR_CLAN_NOMEMBER;
				else if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_REGULAR )	i32Temp = EVENT_ERROR_CLAN_COMMISSION_FAIL;
			}

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				UINT8 ui8count;
				INT64 ai64Idx[ MAX_MEMBER_COUNT ];
				pPacket->ReadData( &ui8count,	sizeof(UINT8) );
				if( ui8count > MAX_MEMBER_COUNT ){	break; }
				pPacket->ReadData( ai64Idx,		sizeof(TUID) * ui8count );
				g_pModuleClan->ClanSendDeportationRequest(m_WorkThreadIdx, this, ui8count, ai64Idx );
			}
			else 
			{
				I3TRACE("Clan Deportation Request Error"); 
      			bSendMessage = TRUE; 
				pSendPacket->SetProtocol(PROTOCOL_CS_DEPORTATION_ACK);
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) );
			}
		}
		break;
	case PROTOCOL_CS_CHATTING_REQ:									//1224 
		{
			CHECKVALID_CHANNEL;
			char strStr[MAX_CHATTING_COUNT]; 
			UINT16 ui16Type; 
			UINT16 ui16ChatSize; 
			pPacket->ReadData( &ui16Type,		sizeof(UINT16));
			pPacket->ReadData( &ui16ChatSize,	sizeof(UINT16));

			ui16ChatSize = MIN(ui16ChatSize, MAX_CHATTING_COUNT);	
			pPacket->ReadData(strStr, ui16ChatSize);
			strStr[MAX_CHATTING_COUNT - 1] = 0;

			if (_CheckDuplicateChat(strStr, ZLOG_CHATTING_TYPE_CLAN, PROTOCOL_CS_CHATTING_ACK) != CHAT_LIMIT_NORMAL)
			{
				// 채팅제한 상태라면 뿌려줄 필요 없으니 여기서 빠져나갑니다.
				break;
			}
			
			g_pModuleClan->ClanSendChatingRequest(	m_WorkThreadIdx, this, (UINT8)ui16ChatSize, strStr );
#if defined( LOCALE_NORTHAMERICA ) || defined( LOCALE_RUSSIA )
			INT32 i32ChattingType = ZLOG_CHATTING_TYPE_CLAN;
			g_pModuleContextNC->LogSendMessage( m_WorkThreadIdx, LOG_MESSAGE_U_CHATTING, this, &i32ChattingType, strStr );
#endif
		}
		break;
	case PROTOCOL_CS_PAGE_CHATTING_REQ:								//1224
		{
			CHECKVALID_CHANNEL;
			char pStr[MAX_CHATTING_COUNT]; 
			UINT16 Type; 
			UINT16 ChatSize; 
			pPacket->ReadData( &Type,		sizeof(UINT16));
			pPacket->ReadData( &ChatSize,	sizeof(UINT16));
			
			ChatSize = MIN(ChatSize, MAX_CHATTING_COUNT);	
			pPacket->ReadData( pStr,	ChatSize );	
			pStr[MAX_CHATTING_COUNT - 1] = 0; 

			if (_CheckDuplicateChat(pStr, ZLOG_CHATTING_TYPE_CLAN, PROTOCOL_CS_PAGE_CHATTING_ACK) != CHAT_LIMIT_NORMAL)
			{
				// 채팅제한 상태라면 뿌려줄 필요 없으니 여기서 빠져나갑니다.
				break;
			}
			
			g_pModuleClan->ClanSendPageChatingRequest(	m_WorkThreadIdx, this, (UINT8)ChatSize, pStr );
#if defined( LOCALE_NORTHAMERICA ) || defined( LOCALE_RUSSIA )
			INT32 i32ChattingType = ZLOG_CHATTING_TYPE_CLAN;
			g_pModuleContextNC->LogSendMessage( m_WorkThreadIdx, LOG_MESSAGE_U_CHATTING, this, &i32ChattingType, pStr );
#endif
		}
		break;
	case PROTOCOL_CS_REPLACE_NOTICE_REQ:							//1224
		{
			CHECKVALID_CHANNEL;
			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic._clanidx	  == 0)									i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			else if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_REGULAR )	i32Temp = EVENT_ERROR_CLAN_NOTICE_FAIL;

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				UINT8 ui8NoticeSize;
				char szNotice[NET_CLAN_INTRO_SIZE];
				pPacket->ReadData( &ui8NoticeSize,	sizeof(UINT8) );		LENGTH_CHECK( ui8NoticeSize, NET_CLAN_INTRO_SIZE );
				pPacket->ReadData( szNotice,		ui8NoticeSize );
				szNotice[ NET_CLAN_INTRO_SIZE - 1 ] = 0;
				g_pModuleClan->ClanSendNoticeRequest(m_WorkThreadIdx, this, ui8NoticeSize, szNotice );
			}
			else
			{
				I3TRACE("Clan Notice Request Error"); 
      			bSendMessage = TRUE; 
				pSendPacket->SetProtocol(PROTOCOL_CS_REPLACE_NOTICE_ACK);
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) );
			}
		}
		break;
	case PROTOCOL_CS_REPLACE_INTRO_REQ:								//1224
		{
			CHECKVALID_CHANNEL;
			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic._clanidx	  == 0)									i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			else if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_REGULAR )	i32Temp = EVENT_ERROR_CLAN_INTRO_FAIL;

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				UINT8 ui8IntroSize;
				char szIntro[NET_CLAN_INTRO_SIZE];
				pPacket->ReadData( &ui8IntroSize,	sizeof(UINT8) );		LENGTH_CHECK( ui8IntroSize, NET_CLAN_INTRO_SIZE );
				pPacket->ReadData( szIntro,			ui8IntroSize );
				szIntro[ NET_CLAN_INTRO_SIZE - 1 ] = 0;
				g_pModuleClan->ClanSendIntroRequest(m_WorkThreadIdx, this, ui8IntroSize, szIntro );
			}
			else
			{
				I3TRACE("Clan Intro Request Error"); 
      			bSendMessage = TRUE; 
				pSendPacket->SetProtocol( PROTOCOL_CS_REPLACE_INTRO_ACK );
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) );
			}
		}
		break;
	case PROTOCOL_CS_NOTE_REQ:
		{
			CHECKVALID_CHANNEL;
			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic._clanidx	  == 0)									i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			else if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_REGULAR )	i32Temp = EVENT_ERROR_CLAN_INTRO_FAIL;

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				UINT8 ui8Type;
				UINT8 ui8NoteSize;
				char  strNote[ NET_NOTE_SIZE ];
				pPacket->ReadData( &ui8Type,		sizeof(UINT8) );
				pPacket->ReadData( &ui8NoteSize,	sizeof(UINT8) );

				ui8NoteSize = MIN( ui8NoteSize, NET_NOTE_SIZE ); 
				pPacket->ReadData( strNote,		ui8NoteSize );
				strNote[ NET_NOTE_SIZE - 1 ] = 0;

				g_pModuleClan->ClanSendNoteRequest(m_WorkThreadIdx, this, ui8Type, ui8NoteSize, strNote );
			}
			else
			{
				I3TRACE("PROTOCOL_CS_NOTE_REQ Error"); 
      			bSendMessage = TRUE; 
				pSendPacket->SetProtocol( PROTOCOL_CS_NOTE_ACK );
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) );
			}
		}
		break;
	case PROTOCOL_CS_CHECK_MARK_REQ:
		{
 			CHECKVALID_CHANNEL; 
			
			UINT32 ui32Mark;
			pPacket->ReadData( &ui32Mark,	sizeof(UINT32) );

			g_pModuleClan->ClanSendCheckMarkRequest(m_WorkThreadIdx, this, ui32Mark );			
		}
		break;
	case PROTOCOL_CS_REPLACE_MANAGEMENT_REQ:
		{
			CHECKVALID_CHANNEL;
			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic._clanidx	  == 0)									i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			else if(m_UserInfoBasic._clanstate != CLAN_MEMBER_LEVEL_MASTER )	i32Temp = EVENT_ERROR_CLAN_COMMISSION_FAIL;

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				CLAN_MANAGEMENT		Management;
				
				pPacket->ReadData( &Management,		sizeof(CLAN_MANAGEMENT) );

				g_pModuleClan->ClanSendManagementRequest(m_WorkThreadIdx, this, &Management );
			}
			else
			{
				I3TRACE("Clan Mark Request Error"); 
      			bSendMessage = TRUE; 
				pSendPacket->SetProtocol( PROTOCOL_CS_REPLACE_MANAGEMENT_ACK );
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) );
			}
		}
		break;
	case PROTOCOL_CS_ROOM_INVITED_REQ:
		{
			CHECKVALID_CHANNEL;
 			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic._clanidx	  == 0)									i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			else if(m_UserInfoBasic._clanstate == CLAN_MEMBER_LEVEL_UNKNOWN )	i32Temp = EVENT_ERROR_CLAN_NOMEMBER;

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				TUID  wObjectIdx; 
				pPacket->ReadData(&wObjectIdx,  sizeof(TUID));			
				g_pModuleClan->ClanSendRoomInvitedRequest( m_WorkThreadIdx, this, wObjectIdx );		
			}
			else
			{
				I3TRACE("Clan Invited Request Error");
				bSendMessage = TRUE;
				pSendPacket->SetProtocol( PROTOCOL_CS_ROOM_INVITED_ACK );
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) );
			}
		}
		break;
	case PROTOCOL_CS_CHECK_JOIN_AUTHORITY_REQ:
		{
			CHECKVALID_CHANNEL;

			UINT32 ui32ClanDBIdx;
			pPacket->ReadData(&ui32ClanDBIdx,  sizeof(UINT32));

			UINT8 ui8Rank = (UINT8)m_UserInfoBasic._rank;
			if( -1 != GetCheckCashitem( MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_DISGUISE_RANK, CASHITEM_DAY_NO) ) )
			{
				ui8Rank = (UINT8)m_UserInfoInvItemData._wRank;
			}

			g_pModuleClan->ClanSendCheckJoinAuthorityRequest( m_WorkThreadIdx, this, ui32ClanDBIdx, m_ui8Age, ui8Rank );
		}
		break;
	case PROTOCOL_CS_CREATE_CLAN_CONDITION_REQ:
		{
			CHECKVALID_CHANNEL;

			bSendMessage = TRUE; 
			pSendPacket->SetProtocol( PROTOCOL_CS_CREATE_CLAN_CONDITION_ACK ); 
			pSendPacket->WriteData( &g_pContextMain->m_ui8ClanCreateRank,	sizeof(UINT8) );
			pSendPacket->WriteData( &g_pContextMain->m_i32ClanCreatePoint,	sizeof(UINT32));
		}
		break;
	default:
			char strTemp[256];
			i3String::Format( strTemp, 256, "UnKnown Packet - %d", ProtocolID );
			I3TRACE( strTemp );
			I3TRACE("UnKnown Packet - Kill User"); 
			bisKilled = TRUE; 
			m_ui32KillReason = KILL_REASON_UNKNOWN_PACKET;
			break;
	}

	return bSendMessage;
}


