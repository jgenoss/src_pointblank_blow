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
			CHECKVALID_LOBBY;

			if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
			{
				m_pRoom->MoveClan(this, TRUE); 
			}

			if( FALSE == g_pContextMain->IsContentsEnabled(CONTENTS_CLAN) )
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_EVENT , ProtocolID, KILL_REASON_HACK_PACKET, L"ClanClose" );
				break;
			}

			bSendMessage = TRUE; 
			pSendPacket->SetProtocol( PROTOCOL_CS_CLIENT_ENTER_ACK ); 
			pSendPacket->WriteData( &m_UserInfoBasic.m_ui32Clanidx,		sizeof(UINT32)); 
			pSendPacket->WriteData( &m_UserInfoBasic.m_ui32ClanState,	sizeof(UINT32));		// 모든 서버 함께 수정합니다..
			
			m_i32SubTask = GAME_TASK_SUB_CLAN;

			if(m_UserInfoBasic.m_ui32Clanidx != 0 && m_UserInfoBasic.m_ui32ClanState != 0)
			{
				//클랜에 가입이 되어 있다.
				// 클랜 서버에 알려줌
				g_pModuleClan->ClanSendPageEnter(m_WorkThreadIdx, this );
				SetVisitClanPage(m_UserInfoBasic.m_ui32Clanidx);
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
			CHECKVALID_LOBBY;

			//방에 있을때는 방에 대한 정보도 표시해줘야 한다. 
			if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
			{
				m_pRoom->MoveClan(this, FALSE); 
			}
			m_i32SubTask = GAME_TASK_SUB_NONE;

			g_pModuleClan->ClanSendPageLeave(m_WorkThreadIdx, this );
			
			InitVisitClanPage();

			bSendMessage = TRUE; 
			pSendPacket->SetProtocol( PROTOCOL_CS_CLIENT_LEAVE_ACK ); 
		}
		break; 
	case PROTOCOL_CS_CREATE_CLAN_REQ :		//080304	081224
		{
			CHECKVALID_LOBBY;

			if( FALSE == g_pContextMain->IsContentsEnabled(CONTENTS_CLAN) )
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_EVENT , ProtocolID, KILL_REASON_HACK_PACKET, L"ClanClose" );
				break;
			}

			//1. 자신이 클랜을 생성할수 있는 확인한다. 			
			if( m_UserInfoBasic.m_ui32Rank < g_pContextMain->m_ui8ClanCreateRank || m_UserInfoBasic.m_ui32Point < (UINT32)g_pContextMain->m_i32ClanCreatePoint )
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
				TTCHAR ClanName[NET_CLAN_NAME_SIZE]; 
				TTCHAR ClanIntro[NET_CLAN_INTRO_SIZE];
				TTCHAR ClanAzit[NET_CLAN_AZIT_URL_SIZE];

				pPacket->ReadData( &ui32Mark,		sizeof( UINT32));
				pPacket->ReadData( ClanName,		sizeof(TTCHAR) * NET_CLAN_NAME_SIZE ); 
				pPacket->ReadData( ClanIntro,		sizeof(TTCHAR) * NET_CLAN_INTRO_SIZE ); 
				pPacket->ReadData( ClanAzit,		sizeof(TTCHAR) * NET_CLAN_AZIT_URL_SIZE ); 
				

				ClanName[NET_CLAN_NAME_SIZE - 1] = 0; 
				ClanIntro[NET_CLAN_INTRO_SIZE - 1] = 0; 
				ClanAzit[NET_CLAN_AZIT_URL_SIZE - 1] = 0; 

				if( _CheckClanName( ClanName ) )
				{	// 클랜 이름을 정상으로 사용할 수 있는지 검사
					g_pModuleClan->ClanSendCreate(m_WorkThreadIdx, this, ClanName, ClanIntro, ClanAzit, ui32Mark);
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
			CHECKVALID_LOBBY;

			CLAN_CONTEXT* pClanContext = g_pContextMain->GetClanContext();

			UINT32	ui32StartIdx;					// 클랜서버와 함께 수정합니다..
			UINT8	ui8size = 0;  
			pPacket->ReadData( &ui32StartIdx, sizeof(UINT32) );

			//I3TRACE("_CLAN_LIST_REQ Recv / StartIdx:%d  ", ui32StartIdx);
			
			if( pClanContext->_totalCount > ui32StartIdx )
			{
				CLAN_LIST_BASIC_INFO* pClanList = g_pContextMain->GetClanList();
								
				bSendMessage = TRUE; 			
				pSendPacket->SetProtocol( PROTOCOL_CS_CLIENT_CLAN_LIST_ACK );
				pSendPacket->WriteData( &ui32StartIdx,	sizeof(UINT32) );
				pSendPacket->WriteData( &ui8size,	sizeof(UINT8) );
				
				for( UINT32 i = ui32StartIdx ; i < pClanContext->_totalCount ; ++i )
				{
					//실제 Write 할 크기를 계산한다
					INT32	i32Realsize	=	sizeof(CLAN_BASIC_INFO) +		
											sizeof(CLAN_BASIC_RECORD) +
											sizeof(CLAN_BASIC_SORT_INFO) +
											sizeof(UINT16) +
											sizeof(TTCHAR) * pClanList[ ui32StartIdx + ui8size ].m_ui16introsize;

					// 8192를 넘어가면 루프를 빠져나가고 다음패킷에 쓴다
					if(PACKETBUFFERSIZE < pSendPacket->GetPacketSize() + i32Realsize )	break;

					pSendPacket->WriteData( &pClanList[ ui32StartIdx + ui8size ].m_ClanBasicInfo,		sizeof(CLAN_BASIC_INFO)); 
					pSendPacket->WriteData( &pClanList[ ui32StartIdx + ui8size ].m_ClanBasicRecord,		sizeof(CLAN_BASIC_RECORD)); 
					pSendPacket->WriteData( &pClanList[ ui32StartIdx + ui8size ].m_ClanBsicSortInfo,	sizeof(CLAN_BASIC_SORT_INFO)); 
					pSendPacket->WriteData( &pClanList[ ui32StartIdx + ui8size ].m_ui16introsize,		sizeof(UINT16));
					pSendPacket->WriteData( &pClanList[ ui32StartIdx + ui8size ].m_intro,				sizeof(TTCHAR) * pClanList[ ui32StartIdx + ui8size ].m_ui16introsize);
					ui8size++;
				}

				pSendPacket->WriteData( sizeof(UINT32) , &ui8size, sizeof(UINT8) );

				//InitVisitClanPage();
			}
			else
			{
				m_ui32KillReason	= KILL_REASON_HACK_PACKET;
				bisKilled		= TRUE;
			}
		}
		break; 
	case PROTOCOL_CS_CHECK_DUPLICATE_REQ:	//중복 검사 하기	080304		081224
		{
			CHECKVALID_LOBBY;

			UINT8	ui8NameSize;
			TTCHAR	strClanName[NET_CLAN_NAME_SIZE];

			pPacket->ReadData( &ui8NameSize,	sizeof(UINT8) );		LENGTH_CHECK( ui8NameSize, NET_CLAN_NAME_SIZE );
			pPacket->ReadData( strClanName,		sizeof(TTCHAR) * ui8NameSize ); 
			strClanName[NET_CLAN_NAME_SIZE - 1] = 0; 

			if( _CheckClanName( strClanName ) )
			{	// 클랜 이름 사용이 가능한지 확인
				g_pModuleClan->ClanSendDuplicateName(m_WorkThreadIdx, this, strClanName ); 
			}
			else
			{
				bSendMessage = TRUE; 
				i32Temp = EVENT_ERROR_FAIL;
				pSendPacket->SetProtocol( PROTOCOL_CS_CHECK_DUPLICATE_ACK ); 
				pSendPacket->WriteData(&i32Temp,	sizeof(INT32));
			}
		}
		break; 
	case PROTOCOL_CS_CHECK_DUPLICATE_AZIT_URL_REQ:	//중북 검사하기		080304
		{
			CHECKVALID_LOBBY;

			UINT8	ui8NameSize;
			TTCHAR	strAzitName[NET_CLAN_AZIT_URL_SIZE]; 
			pPacket->ReadData(&ui8NameSize,		sizeof(UINT8) );		LENGTH_CHECK( ui8NameSize, NET_CLAN_NAME_SIZE );
			pPacket->ReadData(strAzitName,		sizeof(TTCHAR) * ui8NameSize );
			strAzitName[NET_CLAN_AZIT_URL_SIZE - 1] = 0; 

			g_pModuleClan->ClanSendDuplicateAzit( m_WorkThreadIdx, this, strAzitName ); 
		}
		break; 
	case PROTOCOL_CS_JOIN_REQUEST_REQ		:	//클랜 조인 요청 하기	080304
		{
			CHECKVALID_LOBBY;

			UINT8	ui8Size;
			UINT32	ui32ClanDBIdx; 
			TTCHAR	strContents[NET_CLAN_REQUEST_SIZE]; 

			pPacket->ReadData(&ui32ClanDBIdx,	sizeof(UINT32));
			pPacket->ReadData(&ui8Size,			sizeof(UINT8));				LENGTH_CHECK( ui8Size, NET_CLAN_REQUEST_SIZE );
			pPacket->ReadData(strContents,		sizeof(TTCHAR) * ui8Size ); 

			strContents[ NET_CLAN_REQUEST_SIZE - 1 ] = '\0';

			g_pModuleClan->ClanSendJoinRequest(m_WorkThreadIdx, this, ui32ClanDBIdx, strContents ); 
		}
		break;
	case PROTOCOL_CS_CANCEL_REQUEST_REQ	:				// 가입신청 취소 요청	080304
		{
			CHECKVALID_LOBBY;
			g_pModuleClan->ClanSendCancelRequest( m_WorkThreadIdx, this ); 
		}
		break;
	case PROTOCOL_CS_CLAN_LIST_DETAIL_INFO_REQ:
		{
			CHECKVALID_LOBBY;

			INT32 i32ClanIdx; 
			UINT8 ui8Type;
			pPacket->ReadData(&i32ClanIdx,	sizeof(UINT32));
			pPacket->ReadData(&ui8Type,		sizeof(UINT8));

			g_pModuleClan->ClanSendClanListDetailInfo(m_WorkThreadIdx, this, i32ClanIdx, ui8Type); 
		}
		break;
	case PROTOCOL_CS_DETAIL_INFO_REQ	:				// 클랜 상세정보 요청	080304
		{
			CHECKVALID_LOBBY;
			UINT32 ui32ClanIdx;
			UINT8 ui8Type;
			pPacket->ReadData(&ui32ClanIdx,	sizeof(UINT32));
			pPacket->ReadData(&ui8Type,		sizeof(UINT8));
			SetVisitClanPage(ui32ClanIdx);
			g_pModuleClan->ClanSendDetailInfo(m_WorkThreadIdx, this, ui32ClanIdx, ui8Type); 
		}
		break; 
	case PROTOCOL_CS_MEMBER_CONTEXT_REQ	:				// 클랜멤버 목록 정보 요청	080304
		{
			CHECKVALID_LOBBY;

			i32Temp = EVENT_ERROR_SUCCESS;
			if( false == m_bVisitClanPageEnter )
			{				
				if(m_UserInfoBasic.m_ui32Clanidx		== 0)
					i32Temp = SERVER_VALUE_UNKNOWN; 
				if(m_UserInfoBasic.m_ui32ClanState	== CLAN_MEMBER_LEVEL_UNKNOWN )
					i32Temp = SERVER_VALUE_UNKNOWN; 
			}

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
			CHECKVALID_LOBBY;

			i32Temp = EVENT_ERROR_SUCCESS; 
			if( false == m_bVisitClanPageEnter )
			{
				if(m_UserInfoBasic.m_ui32Clanidx	  == 0)
					i32Temp = SERVER_VALUE_UNKNOWN; 
				if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_UNKNOWN )
					i32Temp = SERVER_VALUE_UNKNOWN; 
			}

			if(i32Temp == EVENT_ERROR_SUCCESS)
			{
				UINT8 section; 
				pPacket->ReadData(&section, sizeof(UINT8)); 
				g_pModuleClan->ClanSendMemberList(m_WorkThreadIdx, this, section); 
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
			CHECKVALID_LOBBY;
			i32Temp = EVENT_ERROR_SUCCESS; 
			if(m_UserInfoBasic.m_ui32Clanidx	  == 0)i32Temp = SERVER_VALUE_UNKNOWN; 
			if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_UNKNOWN )i32Temp = SERVER_VALUE_UNKNOWN;

			if(i32Temp == EVENT_ERROR_SUCCESS)
			{			
				T_UID ClanMemberUID; 
				pPacket->ReadData(&ClanMemberUID, sizeof(T_UID)); 
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
			CHECKVALID_LOBBY;
			CHECKVALID_CLAN_PAGE;

			i32Temp = EVENT_ERROR_SUCCESS; 
			if(m_UserInfoBasic.m_ui32Clanidx	  == 0)i32Temp = SERVER_VALUE_UNKNOWN;  
			if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_UNKNOWN )i32Temp = SERVER_VALUE_UNKNOWN;
			if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_REGULAR )i32Temp = SERVER_VALUE_UNKNOWN;
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
			CHECKVALID_LOBBY;
			CHECKVALID_CLAN_PAGE;

			i32Temp = EVENT_ERROR_SUCCESS; 
			if(m_UserInfoBasic.m_ui32Clanidx	  == 0)i32Temp = SERVER_VALUE_UNKNOWN; 
			if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_UNKNOWN )i32Temp = SERVER_VALUE_UNKNOWN;
			if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_REGULAR )i32Temp = SERVER_VALUE_UNKNOWN;

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
			CHECKVALID_LOBBY;

			i32Temp = EVENT_ERROR_SUCCESS; 
			if(m_UserInfoBasic.m_ui32Clanidx	  == 0)i32Temp = SERVER_VALUE_UNKNOWN; 
			if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_UNKNOWN )i32Temp = SERVER_VALUE_UNKNOWN;
			if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_REGULAR )i32Temp = SERVER_VALUE_UNKNOWN;
			if(i32Temp == EVENT_ERROR_SUCCESS)
			{			
				T_UID i64RequestIdx;
				pPacket->ReadData(&i64RequestIdx, sizeof(T_UID)); 
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
			CHECKVALID_LOBBY;

			i32Temp = EVENT_ERROR_SUCCESS; 
			if(m_UserInfoBasic.m_ui32Clanidx	  == 0)i32Temp = SERVER_VALUE_UNKNOWN; 
			if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_UNKNOWN )i32Temp = SERVER_VALUE_UNKNOWN;
			if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_REGULAR )i32Temp = SERVER_VALUE_UNKNOWN;

			if(i32Temp == EVENT_ERROR_SUCCESS)
			{			
				UINT8 ui8count; 
				T_UID ai64List[ MAX_MEMBER_COUNT ];
				pPacket->ReadData(&ui8count,	sizeof(UINT8) ); 
				if( ui8count == 0 )	break;
				if( ui8count > MAX_MEMBER_COUNT )	break;
			
				pPacket->ReadData(ai64List,		(sizeof(T_UID) * ui8count) ); 

				for( INT32 i = 0 ; i < ui8count ; ++i )
				{
					if( 0 == ai64List[i] ) 
					{	 
						return FALSE; 
					}
				} 

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
			CHECKVALID_LOBBY;

			i32Temp = EVENT_ERROR_SUCCESS; 
			if(m_UserInfoBasic.m_ui32Clanidx	  == 0)i32Temp = SERVER_VALUE_UNKNOWN; 
			if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_UNKNOWN )i32Temp = SERVER_VALUE_UNKNOWN;
			if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_REGULAR )i32Temp = SERVER_VALUE_UNKNOWN;

			if(i32Temp == EVENT_ERROR_SUCCESS)
			{			
				UINT8	count; 
				T_UID  List[ MAX_MEMBER_COUNT ];
				pPacket->ReadData(&count,	sizeof(UINT8)); 
				if( count > MAX_MEMBER_COUNT ){	break;	}		//카운터 이상 
				pPacket->ReadData(List,		sizeof(T_UID) * count);
				g_pModuleClan->ClanSendDenialRequest(m_WorkThreadIdx, this, count ,List);
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
			CHECKVALID_LOBBY;

			//UINT8	ui8Room;			
			//pPacket->ReadData(&ui8Room,  sizeof(UINT8));
			
			if( 0 < m_i64FindUID )
			{
				g_pModuleClan->ClanSendInvite(m_WorkThreadIdx, this, m_i64FindUID );

				//////////////////////////////////////////////////////////////////////////
				// 유저찾기 계정 초기화
				m_strFindNick[0] = '\0';
				m_i64FindUID = 0;

				//////////////////////////////////////////////////////////////////////////
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
			CHECKVALID_LOBBY;
			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic.m_ui32ClanState != CLAN_MEMBER_LEVEL_UNKNOWN )	i32Temp = EVENT_ERROR_CLAN_MEMBER;

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

				UINT32 ui32PositionState = FRIEND_STATE_OFFLINE;
				switch (GetPosition())
				{
				case GAME_TASK_LOGIN:
				case GAME_TASK_CHANNEL:
					ui32PositionState = FRIEND_STATE_ONLINE;
					break;
				case GAME_TASK_LOBBY:
					ui32PositionState = FRIEND_STATE_LOBBY;
					break;
				case GAME_TASK_READY_ROOM:
				case GAME_TASK_BATTLE:
					ui32PositionState = FRIEND_STATE_ROOM;
					break;
				}

				UINT32 wState = MAKE_FRIEND_STATE( ui32PositionState, ui32ServerIdx, m_i32ChannelNum, i32RoomIdx );

				g_pModuleClan->ClanSendInviteAcceptRequest( m_WorkThreadIdx, this, ui32ClanDBIdx, wState, ui8Accept );
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
			CHECKVALID_LOBBY;
			i32Temp = EVENT_ERROR_SUCCESS; 
			if(m_UserInfoBasic.m_ui32Clanidx	== 0)
			{
				i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX; 
			}
			else
			{
				if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_UNKNOWN )	i32Temp = EVENT_ERROR_CLAN_NOMEMBER;
				if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_MASTER )		i32Temp = EVENT_ERROR_CLAN_LEVEL_MASTER;			
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
			CHECKVALID_LOBBY;
			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic.m_ui32Clanidx	  == 0)	
			{
				i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			}
			else 
			{
                if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_UNKNOWN )	i32Temp = EVENT_ERROR_CLAN_NOMEMBER;
				if(m_UserInfoBasic.m_ui32ClanState != CLAN_MEMBER_LEVEL_MASTER )		i32Temp = EVENT_ERROR_CLAN_CLOSECLAN_FAIL;
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
			CHECKVALID_LOBBY;
			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic.m_ui32Clanidx	  == 0)	
			{
				i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			}
			else 
			{
                if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_UNKNOWN )	i32Temp = EVENT_ERROR_CLAN_NOMEMBER;
				if(m_UserInfoBasic.m_ui32ClanState != CLAN_MEMBER_LEVEL_MASTER )		i32Temp = EVENT_ERROR_CLAN_COMMISSION_FAIL;
			}

			T_UID i64UID = 0;
			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				pPacket->ReadData( &i64UID,	sizeof(T_UID) );	// -- 현우 일해라 --
				UINT32 wRank = CLAN_RANK_A2;
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
			CHECKVALID_LOBBY;
			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic.m_ui32Clanidx	  == 0)	
			{
				i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			}
			else 
			{
                if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_UNKNOWN )	i32Temp = EVENT_ERROR_CLAN_NOMEMBER;
				if(m_UserInfoBasic.m_ui32ClanState != CLAN_MEMBER_LEVEL_MASTER )		i32Temp = EVENT_ERROR_CLAN_COMMISSION_FAIL;
			}

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				UINT8 ui8count;
				T_UID ai64Idx[ MAX_MEMBER_COUNT ];
				pPacket->ReadData( &ui8count,	sizeof(UINT8) );
				if( ui8count > MAX_MEMBER_COUNT ){	break; }
				pPacket->ReadData( ai64Idx,		sizeof(T_UID)*ui8count );
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
			CHECKVALID_LOBBY;
			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic.m_ui32Clanidx	  == 0)	
			{
				i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			}
			else 
			{
                if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_UNKNOWN )	i32Temp = EVENT_ERROR_CLAN_NOMEMBER;
				if(m_UserInfoBasic.m_ui32ClanState != CLAN_MEMBER_LEVEL_MASTER )		i32Temp = EVENT_ERROR_CLAN_COMMISSION_FAIL;
			}

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				UINT8  count;
				T_UID Idx[  MAX_MEMBER_COUNT  ];
				pPacket->ReadData( &count,	sizeof(UINT8) );
				if( count > MAX_MEMBER_COUNT ){	break; }
				pPacket->ReadData( Idx,		sizeof(T_UID)*count );
				g_pModuleClan->ClanSendCommissionRegularRequest(m_WorkThreadIdx, this, count, Idx );
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
			CHECKVALID_LOBBY;
			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic.m_ui32Clanidx	  == 0)	
			{
				i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			}
			else 
			{
                if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_UNKNOWN )		i32Temp = EVENT_ERROR_CLAN_NOMEMBER;
				else if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_REGULAR )	i32Temp = EVENT_ERROR_CLAN_COMMISSION_FAIL;
			}

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				UINT8 count;
				T_UID Idx[ MAX_MEMBER_COUNT ];
				pPacket->ReadData( &count,	sizeof(UINT8) );
				if( count > MAX_MEMBER_COUNT ){	break; }
				pPacket->ReadData( Idx,		sizeof(T_UID) * count );
				g_pModuleClan->ClanSendDeportationRequest(m_WorkThreadIdx, this, count, Idx );
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
			CHECKVALID_LOBBY;
			TTCHAR	strChat[MAX_CHATTING_COUNT]; 
			UINT16	ui16Type; 
			UINT16	ui16ChatSize; 
			bool	bIsGMUser;
			pPacket->ReadData( &ui16Type,		sizeof(UINT16));
			pPacket->ReadData( &ui16ChatSize,	sizeof(UINT16));

			ui16ChatSize = MIN(ui16ChatSize, MAX_CHATTING_COUNT);	
			pPacket->ReadData(strChat,			sizeof(TTCHAR) * ui16ChatSize);
			strChat[MAX_CHATTING_COUNT - 1] = 0; 

			bIsGMUser = CheckAuthLevel();

			if ( !_CheckChat(strChat, ZLOG_CHATTING_TYPE_CLAN, (INT16)ui16ChatSize, PROTOCOL_CS_CHATTING_ACK) )
			{
				// 채팅제한 상태라면 뿌려줄 필요 없으니 여기서 빠져나갑니다.
				break;
			}
			
			g_pModuleClan->ClanSendChatingRequest(	m_WorkThreadIdx, this, (UINT8)ui16ChatSize, strChat, bIsGMUser );
			if( m_ui8LocaleCode == LOCALE_CODE_RUSSIA )
			{
				INT32 i32ChattingType = ZLOG_CHATTING_TYPE_CLAN;
				g_pModuleContextNC->LogSendMessage(m_WorkThreadIdx, LOG_MESSAGE_U_CHATTING, this, &i32ChattingType, strChat);
			}
		}
		break;
	case PROTOCOL_CS_PAGE_CHATTING_REQ:								//1224
		{
			CHECKVALID_LOBBY;
			TTCHAR	pStr[MAX_CHATTING_COUNT]; 
			UINT16	Type; 
			UINT16	ChatSize;
			bool	bIsGMUser;
			pPacket->ReadData( &Type,		sizeof(UINT16));
			pPacket->ReadData( &ChatSize,	sizeof(UINT16));
			
			ChatSize = MIN(ChatSize, MAX_CHATTING_COUNT);	
			pPacket->ReadData( pStr,	sizeof(TTCHAR) * ChatSize );	
			pStr[MAX_CHATTING_COUNT - 1] = 0; 

			bIsGMUser = CheckAuthLevel();

			if (!_CheckChat(pStr, ZLOG_CHATTING_TYPE_CLAN, (INT16)ChatSize, PROTOCOL_CS_PAGE_CHATTING_ACK))
			{
				// 채팅제한 상태라면 뿌려줄 필요 없으니 여기서 빠져나갑니다.
				break;
			}
			
			g_pModuleClan->ClanSendPageChatingRequest(	m_WorkThreadIdx, this, (UINT8)ChatSize, pStr, bIsGMUser );

			if( m_ui8LocaleCode == LOCALE_CODE_RUSSIA )
			{
				INT32 i32ChattingType = ZLOG_CHATTING_TYPE_CLAN;
				g_pModuleContextNC->LogSendMessage(m_WorkThreadIdx, LOG_MESSAGE_U_CHATTING, this, &i32ChattingType, pStr);
			}
		}
		break;
	case PROTOCOL_CS_REPLACE_NOTICE_REQ:							//1224
		{
			CHECKVALID_LOBBY;
			i32Temp = EVENT_ERROR_SUCCESS;
			if(m_UserInfoBasic.m_ui32Clanidx	  == 0)									i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			else if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_REGULAR )	i32Temp = EVENT_ERROR_CLAN_NOTICE_FAIL;

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				UINT8 ui8NoticeSize;
				TTCHAR strNotice[NET_CLAN_INTRO_SIZE];
				pPacket->ReadData( &ui8NoticeSize,	sizeof(UINT8) );		LENGTH_CHECK( ui8NoticeSize, NET_CLAN_INTRO_SIZE );
				pPacket->ReadData( strNotice,		sizeof(TTCHAR) * ui8NoticeSize );
				strNotice[ NET_CLAN_INTRO_SIZE - 1 ] = 0;
				g_pModuleClan->ClanSendNoticeRequest(m_WorkThreadIdx, this, ui8NoticeSize, strNotice );
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
			if(m_UserInfoBasic.m_ui32Clanidx	  == 0)									i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			else if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_REGULAR )	i32Temp = EVENT_ERROR_CLAN_INTRO_FAIL;

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				UINT8 ui8IntroSize;
				TTCHAR strIntro[NET_CLAN_INTRO_SIZE];
				pPacket->ReadData( &ui8IntroSize,	sizeof(UINT8) );		LENGTH_CHECK( ui8IntroSize, NET_CLAN_INTRO_SIZE );
				pPacket->ReadData( strIntro,		sizeof(TTCHAR) * ui8IntroSize );
				strIntro[ NET_CLAN_INTRO_SIZE - 1 ] = 0;
				g_pModuleClan->ClanSendIntroRequest(m_WorkThreadIdx, this, ui8IntroSize, strIntro );
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
			if(m_UserInfoBasic.m_ui32Clanidx	  == 0)									i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			else if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_REGULAR )	i32Temp = EVENT_ERROR_CLAN_INTRO_FAIL;

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				UINT8 ui8Type;
				UINT8 ui8NoteSize;
				TTCHAR  strNote[ NET_NOTE_SIZE ];
				pPacket->ReadData( &ui8Type,		sizeof(UINT8) );
				pPacket->ReadData( &ui8NoteSize,	sizeof(UINT8) );

				ui8NoteSize = MIN( ui8NoteSize, NET_NOTE_SIZE ); 
				pPacket->ReadData( strNote,		sizeof(TTCHAR) * ui8NoteSize );
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
			if(m_UserInfoBasic.m_ui32Clanidx	  == 0)									i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			else if(m_UserInfoBasic.m_ui32ClanState != CLAN_MEMBER_LEVEL_MASTER )	i32Temp = EVENT_ERROR_CLAN_COMMISSION_FAIL;

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
			if(m_UserInfoBasic.m_ui32Clanidx	  == 0)									i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			else if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_UNKNOWN )	i32Temp = EVENT_ERROR_CLAN_NOMEMBER;

			if( i32Temp == EVENT_ERROR_SUCCESS )
			{
				T_UID  wObjectIdx; 
				pPacket->ReadData(&wObjectIdx,  sizeof(T_UID));			
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
			
			g_pModuleClan->ClanSendCheckJoinAuthorityRequest( m_WorkThreadIdx, this, ui32ClanDBIdx, m_ui8Age, (UINT8)m_UserInfoBasic.m_ui32Rank );
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
	case PROTOCOL_CS_CLAN_MATCH_RESULT_CONTEXT_REQ:
		{
			CHECKVALID_CHANNEL;
			//CHECKVALID_CLAN_PAGE;

			PACKET_CS_CLAN_MATCH_RESULT_CONTEXT_REQ			PacketReq;
			PACKET_H_CLAN_ACK								PacketAckH;
			pPacket->ReadData( &PacketReq, sizeof(PACKET_CS_CLAN_MATCH_RESULT_CONTEXT_REQ) );
			
			PacketAckH.m_i32Result = IsVisitClanNo(PacketReq.m_ui32CUID);

			if(PacketAckH.m_i32Result == EVENT_ERROR_SUCCESS)
			{
				g_pModuleClan->ClanSendMatchResultContext(m_WorkThreadIdx, this ); 
			}
			else
			{
				I3TRACE("Clan Match Result Context Req Error"); 
				bSendMessage = TRUE; 
				pSendPacket->SetProtocol( PROTOCOL_CS_CLAN_MATCH_RESULT_CONTEXT_ACK ); 
				pSendPacket->WriteData( &PacketAckH, sizeof(PACKET_H_CLAN_ACK) ); 
			}
		}
		break; 
	case PROTOCOL_CS_CLAN_MATCH_RESULT_LIST_REQ:
		{
			CHECKVALID_CHANNEL;
			//CHECKVALID_CLAN_PAGE;
			
			PACKET_CS_CLAN_MATCH_RESULT_LIST_REQ		PacketReq;
			PACKET_H_CLAN_ACK								PacketAckH	= {0,};
			pPacket->ReadData( &PacketReq, sizeof(PACKET_CS_CLAN_MATCH_RESULT_LIST_REQ) );

			PacketAckH.m_i32Result = IsVisitClanNo(PacketReq.m_ui32CUID);
			
			if(PacketAckH.m_i32Result == EVENT_ERROR_SUCCESS)
			{				
				g_pModuleClan->ClanSendMatchResultList(m_WorkThreadIdx, this, PacketReq.m_ui8Section); 
			}
			else
			{
				I3TRACE("Clan Match Result Context List Error"); 
				bSendMessage = TRUE; 
				pSendPacket->SetProtocol( PROTOCOL_CS_CLAN_MATCH_RESULT_LIST_ACK );
				pSendPacket->WriteData( &PacketAckH, sizeof(PACKET_H_CLAN_ACK) ); 
			}
		}
		break;
	case PROTOCOL_CS_CLAN_MATCH_RESULT_EMOTION_SAVE_REQ:
		{
			CHECKVALID_CHANNEL;
			CHECKVALID_CLAN_PAGE;

			i32Temp = EVENT_ERROR_SUCCESS;
			PACKET_CS_CLAN_MATCH_RESULT_EMOTION_SAVE_REQ	PacketReq;
			pPacket->ReadData( &PacketReq, sizeof(PACKET_CS_CLAN_MATCH_RESULT_EMOTION_SAVE_REQ) );

			// 자신의 클랜인지
			if(m_UserInfoBasic.m_ui32Clanidx == 0)									i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			else if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_UNKNOWN )	i32Temp = EVENT_ERROR_CLAN_NOMEMBER;

			// 자신의 클랜인지
			if(m_UserInfoBasic.m_ui32Clanidx != PacketReq.m_ui32CUID)			i32Temp = EVENT_ERROR_CLAN_NOMEMBER;

			// 감정표현 값 검사
			if(CLAN_EMOTION_TYPE_NONE >= PacketReq.m_ui8Emotion || MAX_CLAN_EMOTION_TYPE <= PacketReq.m_ui8Emotion )	i32Temp = EVENT_ERROR_GAME_WRONG_DATA;
			
			// 조건이 올바르면 정보전송
			if(i32Temp == EVENT_ERROR_SUCCESS)
			{
				g_pModuleClan->ClanSendMatchResultEmotionSave(m_WorkThreadIdx, this, PacketReq.m_ui8MatchCount, PacketReq.m_ui8Emotion, m_UserInfoBasic.m_strNick);
			}
			/*else
			{
				SendErrorMessage( m_WorkThreadIdx, i32Temp, ProtocolID, PROTOCOL_CS_CLAN_MATCH_RESULT_EMOTION_SAVE_ACK, KILL_REASON_HACK_PACKET);
			}*/
		}
		break;
	case PROTOCOL_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_REQ:
		{
			CHECKVALID_CHANNEL;
			CHECKVALID_CLAN_PAGE;

			i32Temp = EVENT_ERROR_SUCCESS;
			PACKET_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_REQ		PacketReq;
			pPacket->ReadData( &PacketReq, sizeof(PACKET_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_REQ) );

			// 자신의 클랜인지
			if(m_UserInfoBasic.m_ui32Clanidx  == 0)									i32Temp = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
			else if(m_UserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_UNKNOWN )	i32Temp = EVENT_ERROR_CLAN_NOMEMBER;

			// 자신이 클랜마스터인지
			if(m_UserInfoBasic.m_ui32ClanState != CLAN_MEMBER_LEVEL_MASTER )		i32Temp = EVENT_ERROR_CLAN_COMMISSION_FAIL;

			// 자동승인 값 검사
			if(CLAN_AUTO_OPTION_MANUAL > PacketReq.m_ui32AutoAcceptOption || MAX_CLAN_AUTO_OPTION <= PacketReq.m_ui32AutoAcceptOption )	i32Temp = EVENT_ERROR_GAME_WRONG_DATA;

			// 조건이 올바르면 정보전송
			if(i32Temp == EVENT_ERROR_SUCCESS)
			{
				g_pModuleClan->ClanSendMatchResultAutoAcceptOption(m_WorkThreadIdx, this, PacketReq.m_ui32AutoAcceptOption);
			}
			/*else
			{
				SendErrorMessage( m_WorkThreadIdx, i32Temp, ProtocolID, PROTOCOL_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_ACK, KILL_REASON_HACK_PACKET);
			}*/
		}
		break;

	default:
			char strTemp[256];
			sprintf( strTemp, "UnKnown Packet - %d", ProtocolID );
			I3TRACE( strTemp );
			I3TRACE("UnKnown Packet - Kill User"); 
			bisKilled = TRUE; 
			m_ui32KillReason = KILL_REASON_UNKNOWN_PACKET;
			break;
	}

	return bSendMessage;
}


