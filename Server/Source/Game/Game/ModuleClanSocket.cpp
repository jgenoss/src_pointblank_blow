#include "pch.h"
#include "ModuleClanSocket.h"

#include "UserManager.h"
#include "UserSession.h"
#include "ModuleTrans.h"
#include "ModuleClan.h"
#include "ModuleMessenger.h"
#include "ModuleContextNc.h"
#include "RoomManager.h"
#include "Room.h"

I3_CLASS_INSTANCE( CModuleClanSocket, i3NetworkClientSocket );


CModuleClanSocket::CModuleClanSocket(void)
{
	m_bConnect			= FALSE;
	m_bNewClanCreate	= FALSE;
}

CModuleClanSocket::~CModuleClanSocket(void)
{
	OnDestroy();
}

BOOL CModuleClanSocket::OnCreate( UINT32 ui32IP, UINT16 ui16PortNo)
{
	BOOL bRv = i3NetworkClientSocket::OnCreate( ui32IP, ui16PortNo);
	m_ui32LastSendTime	= i3ThreadTimer::GetServerTime();
	m_ui32LastRecvTime	= i3ThreadTimer::GetServerTime();

	// 서버에 클랜 서버 갯수를 보내준다.
	if( bRv )
	{
		INT8 i8ChannelCount = 0;
		INT8 ai8Channel[ 255 ];
		
		for( UINT8 i = 0 ; i < GAME_CHANNEL_COUNT ; i++ )
		{
			if( g_pContextMain->m_pChannelType[i] == CHANNEL_TYPE_CLAN	||
				g_pContextMain->m_pChannelType[i] == CHANNEL_TYPE_PCCAFE_CLAN )
			{
				ai8Channel[ i8ChannelCount ] = i;
				i8ChannelCount++;
			}
		}
		
		i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_CHANNEL_REQ );
		SendPacket.WriteData( &i8ChannelCount,	sizeof(INT8) );
		SendPacket.WriteData( ai8Channel,			sizeof(INT8) * i8ChannelCount );
		SendPacketMessage(&SendPacket);
	}

	return bRv; 
}

void CModuleClanSocket::OnDestroy(void)
{
	DisConnect(); 
}

void CModuleClanSocket::DisConnect()
{
	m_bConnect = FALSE; 
	i3NetworkClientSocket::DisConnect();
}

void CModuleClanSocket::PacketParsing(i3NetworkPacket  * pPacket)
{
	PROTOCOL	ProtocolID = pPacket->GetProtocolID(); 

	INT32 i32SessionIdx; 
	T_UID i64UID;
	INT32 i32Result; 


	switch(ProtocolID)
	{
	case PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS	:		//연결되었음
		{
			m_bConnect = TRUE;
			g_pLog->WriteLog( L"[SUCC] CLAN SERVER CONNECT MESSAGE RECV");

			CLAN_CONTEXT* pClanContext		= g_pContextMain->GetClanContext();
			CLAN_CONTEXT* pClanContextBack	= g_pContextMain->GetClanContextBack();

			INT32 i32SeasonRankingCount = 0;
			CLAN_BASIC_INFO	pSeasonRankingClan[MAX_SEASON_RANKING_TEAM_COUNT];

			pPacket->ReadData( pClanContext,							sizeof(CLAN_CONTEXT) );
			i3mem::Copy( pClanContextBack, pClanContext,				sizeof( CLAN_CONTEXT ) );

			pPacket->ReadData( &i32SeasonRankingCount,					sizeof(INT32) );
			pPacket->ReadData( pSeasonRankingClan,						sizeof(CLAN_BASIC_INFO)*i32SeasonRankingCount );

			g_pContextMain->InsertSeasonRanking( i32SeasonRankingCount,pSeasonRankingClan);

			i3NetworkPacket SendPacket( PROTOCOL_CS_CLAN_LIST_STREAM_REQ );
			SendPacketMessage(&SendPacket);	
		}
		break;
	case PROTOCOL_CS_CLAN_CONTEXT_ACK			:			//클랜 정보 받기.
		{
			CLAN_CONTEXT stClanContext; 
			pPacket->ReadData( &stClanContext, sizeof(CLAN_CONTEXT) ); 

			CLAN_CONTEXT* pClanContext = g_pContextMain->GetClanContext();

			if( pClanContext->_version != stClanContext._version )
			{	// Front Buffer 가 다른 버전이면 Back Context를 갱신한 뒤 리스트를 다시 받음.
				CLAN_CONTEXT* pClanContextBack = g_pContextMain->GetClanContextBack();

				i3mem::Copy( pClanContextBack, &stClanContext, sizeof(CLAN_CONTEXT) ); 
				
				i3NetworkPacket SendPacket( PROTOCOL_CS_CLAN_LIST_STREAM_REQ );
				SendPacketMessage(&SendPacket);
			}
		}
		break; 
	case PROTOCOL_CS_CLAN_LIST_STREAM_ACK		:			//클랜정보 스트림으로 받기.
		{
			UINT32	ui32StartIdx;
			UINT8	ui8Count; 

			pPacket->ReadData( &ui32StartIdx,	sizeof(UINT32) );
			pPacket->ReadData( &ui8Count,		sizeof(UINT8) );

			//I3TRACE("Normal Recv / StartIdx:%d / Count : %d \n", ui32StartIdx, ui8Count);

			CLAN_CONTEXT* pClanContext = g_pContextMain->GetClanContextBack();
			CLAN_LIST_BASIC_INFO* pClanList = g_pContextMain->GetClanListBack();

			if(ui32StartIdx < pClanContext->_totalCount )
			{
				for( INT32 i = 0 ; i < ui8Count ; ++i )
				{
					pPacket->ReadData( &pClanList[ui32StartIdx + i ].m_ClanBasicInfo,		sizeof(CLAN_BASIC_INFO));
					pPacket->ReadData( &pClanList[ui32StartIdx + i ].m_ClanBasicRecord,		sizeof(CLAN_BASIC_RECORD));
					pPacket->ReadData( &pClanList[ui32StartIdx + i ].m_ClanBsicSortInfo,	sizeof(CLAN_BASIC_SORT_INFO));
					pPacket->ReadData( &pClanList[ui32StartIdx + i ].m_ui16introsize,		sizeof(UINT16));
					pPacket->ReadData( &pClanList[ui32StartIdx + i ].m_intro,				sizeof(TTCHAR) * pClanList[ui32StartIdx + i ].m_ui16introsize);
				}					

				if( (UINT32)(ui32StartIdx + ui8Count) >= pClanContext->_totalCount )
				{
					// 다 받았으면 Context를 변경해준다.
					g_pContextMain->SetChangeClanActive();
				}
				
			}
			else
			{
				g_pLog->WriteLog( L"CLAN_LIST_STREAM Error section"); 
			}
		}
		break;
	case PROTOCOL_CS_USER_LOGIN_ACK			:
		{
			UINT32	ui32CID=0, ui32Mark=0;
			UINT8	ui8State=0, ui8ExpRank=0, ui8PerRank=0, ui8NameSize;
			TTCHAR	strClanName[NET_CLAN_NAME_SIZE];

			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(T_UID));
			pPacket->ReadData(&i32Result,		sizeof(INT32));
			if( EV_SUCCESSED( i32Result ) )
			{
				pPacket->ReadData(&ui32CID,			sizeof(UINT32));
				pPacket->ReadData(&ui8State,		sizeof(UINT8)); 
				if( CLAN_MEMBER_LEVEL_UNKNOWN < ui8State )
				{
					pPacket->ReadData(&ui8ExpRank,		sizeof(UINT8)); 
					pPacket->ReadData(&ui8PerRank,		sizeof(UINT8)); 
					pPacket->ReadData(&ui32Mark,		sizeof(UINT32)); 
					pPacket->ReadData(&ui8NameSize,		sizeof(UINT8)); LENGTH_CHECK( ui8NameSize, NET_CLAN_NAME_SIZE );
					pPacket->ReadData(strClanName,		sizeof(TTCHAR) * ui8NameSize); 
				}
			}
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if( NULL != pSession )
			{
				pSession->SetClan_C( i32Result, ui32CID, ui8State, ui8ExpRank, ui8PerRank, ui32Mark, strClanName );
			}
		}
		break;
	case PROTOCOL_CS_USER_LOGOUT_ACK		: break; 	//받을 필요없음 
	case PROTOCOL_CS_MEMBER_INFO_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				UINT8					ui8Count;
				CLANMEMBER_INFO_STRUCT	aClanMember[ MAX_MEMBER_COUNT ];

				pPacket->ReadData(&ui8Count, sizeof(UINT8));

				if( ui8Count <= MAX_MEMBER_COUNT )
				{
					for( UINT8 i = 0 ; i < ui8Count ; i++ )
					{
						pPacket->ReadData(&aClanMember[i]._NickSize,	sizeof(UINT8));
						if( aClanMember[i]._NickSize > NET_NICK_NAME_SIZE ) return;

						pPacket->ReadData(aClanMember[i]._NickName,		sizeof(TTCHAR) * aClanMember[i]._NickSize );
						pPacket->ReadData(&aClanMember[i]._BasicInfo,	sizeof(CLANMEMBER_INFO) );

						aClanMember[i]._NickName[ NET_NICK_NAME_SIZE - 1 ] = '\0';
					}

					pSession->SendClanMemberInfo( ui8Count, aClanMember );
				}
				else
				{
					g_pLog->WriteLog( L"PROTOCOL_CS_MEMBER_INFO_ACK Error count");
				}
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_MEMBER_INFO_CHANGE_ACK:
		{
			UINT8	ui8UserCount;
			INT32	ai32Session[ MAX_MEMBER_COUNT ];
			T_UID	ai64UIDSendUser[ MAX_MEMBER_COUNT ];
			UINT64	ui64State;

			pPacket->ReadData(&ui8UserCount,		sizeof(UINT8));
			if( ui8UserCount > MAX_MEMBER_COUNT )		break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64UIDSendUser,		sizeof(T_UID) * ui8UserCount); 
			pPacket->ReadData(&i64UID,				sizeof(T_UID));
			pPacket->ReadData(&ui64State,			sizeof(UINT64));

			if( g_pUserManager )		// 서버 안켜지는 문제 땜빵코드입니다.. -- 현우 일해라 --
			{
				for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
				{
					CUserSession * pSession = g_pUserManager->GetSession( ai32Session[ i ], ai64UIDSendUser[ i ]); 
					if(pSession != NULL)
					{
						pSession->SendClanMemberInfoChange( i64UID, ui64State );
					}
					else
					{
						//SendUnknownUser_A(wDBIdx[ i ]); 				
					}
				}
			}
		}
		break;
	case PROTOCOL_CS_MEMBER_INFO_INSERT_ACK:
		{
			UINT8	ui8UserCount;
			INT32	ai32Session[ MAX_MEMBER_COUNT ];
			T_UID	ai64UIDSendUser[ MAX_MEMBER_COUNT ];
			CLANMEMBER_INFO_STRUCT	ClanMember;

			pPacket->ReadData(&ui8UserCount,				sizeof(UINT8));
			if( ui8UserCount > MAX_MEMBER_COUNT )				break;
			pPacket->ReadData( ai32Session,					sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64UIDSendUser,				sizeof(T_UID) * ui8UserCount); 
			pPacket->ReadData(&ClanMember._NickSize,		sizeof(UINT8));
			if( ClanMember._NickSize > NET_NICK_NAME_SIZE ) break;
			pPacket->ReadData(ClanMember._NickName,			sizeof(TTCHAR) * ClanMember._NickSize);
			pPacket->ReadData(&ClanMember._BasicInfo,		sizeof(CLANMEMBER_INFO));

			ClanMember._NickName[ NET_NICK_NAME_SIZE - 1 ] = '\0';

			for( INT32 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[ i ], ai64UIDSendUser[ i ]);
				if(pSession != NULL)
				{
					pSession->SendClanMemberInfoInsert( &ClanMember );
				}
				else
				{
					//SendUnknownUser_A(wDBIdx[ i ]);
				}
			}
		}
		break;
	case PROTOCOL_CS_MEMBER_INFO_DELETE_ACK:
		{
			UINT8	ui8UserCount;
			INT32	ai32SSession[ MAX_MEMBER_COUNT ];
			T_UID	ai64UID[ MAX_MEMBER_COUNT ];
			T_UID	i64MemberUID;

			pPacket->ReadData( &ui8UserCount,				sizeof(UINT8));					LENGTH_CHECK( ui8UserCount, MAX_MEMBER_COUNT );
			pPacket->ReadData( ai32SSession,				sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64UID,						sizeof(T_UID) * ui8UserCount); 
			pPacket->ReadData( &i64MemberUID,				sizeof(T_UID));

			CUserSession * pSession = NULL;
			for( INT32 i = 0 ; i < ui8UserCount ; i++ )
			{
				pSession = g_pUserManager->GetSession( ai32SSession[ i ], ai64UID[ i ]);
				if( NULL == pSession ) continue;
					
				pSession->SendClanMemberInfoDelete( i64MemberUID );
			}
		}
		break;
	case PROTOCOL_CS_CREATE_CLAN_ACK		: //클랜을 생성합니다. 
		{
			CLAN_DETAIL_INFO ClanDetailInfo; 
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,			sizeof(INT32)); 
				if( i32Result > SERVER_VALUE_UNKNOWN )
				{
					//m_bNewClanCreate = TRUE; //클랜정보를 다시 갱신 합니다. 
					pPacket->ReadData(&ClanDetailInfo,	sizeof(CLAN_DETAIL_INFO));
				}
				pSession->SendClanCreate_C(i32Result, &ClanDetailInfo);
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break; 
	case PROTOCOL_CS_JOIN_REQUEST_ACK		: 
		{
			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				INT32 ClanIdx; 
				pPacket->ReadData(&i32Result,		sizeof(INT32));
				pPacket->ReadData(&ClanIdx,			sizeof(INT32));
				pSession->SendClanJoinRequest(i32Result, ClanIdx); 
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_CHECK_DUPLICATE_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,		sizeof(INT32));

				i3NetworkPacket SendPacket( PROTOCOL_CS_CHECK_DUPLICATE_ACK );
				SendPacket.WriteData(&i32Result, sizeof(INT32));
				pSession->SendPacketMessage( &SendPacket );
			}
		}
		break;
	case PROTOCOL_CS_CHECK_DUPLICATE_AZIT_URL_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,	sizeof(INT32));

				i3NetworkPacket SendPacket( PROTOCOL_CS_CHECK_DUPLICATE_AZIT_URL_ACK );
				SendPacket.WriteData(&i32Result, sizeof(INT32));
				pSession->SendPacketMessage( &SendPacket );
			}
		}
		break;
	case PROTOCOL_CS_CANCEL_REQUEST_ACK		: 
		{
			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(T_UID));																																																																																																
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result, sizeof(INT32));
				pSession->SendClanCancleRequest(i32Result);
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;	
	case PROTOCOL_CS_INVITE_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			pPacket->ReadData(&i32Result,			sizeof(INT32));
			if( EV_SUCCESSED( i32Result ) )
			{
				CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
				if(pSession != NULL)
				{
					T_UID	i64RecvUID;
					pPacket->ReadData(&i64RecvUID,	sizeof(T_UID));

					// 가입 권유 쪽지
					TTCHAR   strMessage[ NET_NOTE_SIZE ];
					strMessage[ 0 ] = sizeof(TTCHAR) + 1;
					strMessage[ 1 ] = NOTE_MESSAGE_CLAN_INVITE;
					strMessage[ 2 ] = '\0';

					g_pModuleMessenger->MessengerSendClanNote( MODULE_CLAN_THREAD_IDX, pSession, i64RecvUID, pSession->m_UserInfoBasic.m_ui32Clanidx, 1, 0, strMessage, pSession->m_UserInfoBasic.m_strClanName );
				}
			}
		}
		break;
	case PROTOCOL_CS_CLAN_LIST_DETAIL_INFO_ACK: 
		{
			CLAN_DETAIL_INFO ClanDetailInfo; 
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID));
			pPacket->ReadData(&i32Result,			sizeof(INT32));
			if( EV_SUCCESSED( i32Result ) )
			{
				pPacket->ReadData(&ClanDetailInfo,	sizeof(CLAN_DETAIL_INFO));
			}

			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);
			if(pSession != NULL)
			{
				pSession->SendClanListDetailInfo(i32Result, &ClanDetailInfo); 
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_DETAIL_INFO_ACK: 
		{
			CLAN_DETAIL_INFO	ClanDetailInfo;
			CLAN_TOP_MEMBER		ClanTopMember;
			CLAN_NEWS			ClanNews;

			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID));
			pPacket->ReadData(&i32Result,			sizeof(INT32));
			if( EV_SUCCESSED( i32Result ) )
			{
				pPacket->ReadData(&ClanDetailInfo,	sizeof(CLAN_DETAIL_INFO));
				pPacket->ReadData(&ClanTopMember,	sizeof(CLAN_TOP_MEMBER));
				pPacket->ReadData(&ClanNews,		sizeof(CLAN_NEWS));
			}

			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);
			if(pSession != NULL)
			{
				pSession->SendClanDetailInfo(i32Result, &ClanDetailInfo, &ClanTopMember, &ClanNews); 

				g_pModuleClan->ClanSendMemberContext(MODULE_CLAN_THREAD_IDX, pSession ); 
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_MEMBER_CONTEXT_ACK		:	//Member Context 받기 
		{
			CLAN_MEMBER_CONTEXT ClanMemberContext; 
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID));
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,			sizeof(INT32)); 
				pPacket->ReadData(&ClanMemberContext,	sizeof(CLAN_MEMBER_CONTEXT));
				pSession->SendClanMemberContext(i32Result, &ClanMemberContext); 
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break; 
	case PROTOCOL_CS_MEMBER_LIST_ACK		:	//Member List 받기 
		{
			UINT8 ui8section	= 0; 
			UINT8 ui8count		= 0; 

			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID));
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,		sizeof(INT32)); 
				pPacket->ReadData(&ui8section,		sizeof(UINT8));
				pPacket->ReadData(&ui8count,		sizeof(UINT8));
				
				if( ui8count < 200 )
				{
					pPacket->ReadData( m_aTempMemberBasicInfo, (sizeof(CLAN_MEMBER_BASIC_INFO) * ui8count)); 
					pSession->SendClanMemberList(i32Result, ui8section, ui8count, m_aTempMemberBasicInfo); 
				}
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break; 
	case PROTOCOL_CS_MEMBER_DETAIL_INFO_ACK:
		{
			//UINT8 section	= 0; 
			//UINT8 count		= 0; 

			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID));
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				USER_INFO_RECORD		Record;
				pPacket->ReadData(&i32Result,			sizeof(INT32)); 
				if( i32Result > SERVER_VALUE_UNKNOWN ) 
				{
					pPacket->ReadData(&Record,			sizeof(USER_INFO_RECORD));
					pSession->SendClanMemberDetailInfo( i32Result, &Record );
				}
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_REQUEST_CONTEXT_ACK	: 
		{
			CLAN_REQUEST_CONTEXT ClanRequestContext; 
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,			sizeof(INT32)); 
				pPacket->ReadData(&ClanRequestContext,	sizeof(CLAN_REQUEST_CONTEXT)); 
				pSession->SendClanRequestContext(i32Result, &ClanRequestContext);
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break; 
	case PROTOCOL_CS_REQUEST_LIST_ACK		: 
		{
			UINT8	ui8section = 0; 
			UINT8	ui8count = 0; 
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{//
				pPacket->ReadData(&i32Result,		sizeof(INT32)); 
				pPacket->ReadData(&ui8section,		sizeof(UINT8));
				pPacket->ReadData(&ui8count,		sizeof(UINT8));
				if( ui8count < 200 )
				{
					pPacket->ReadData( m_aTempRequestBasicInfo, (sizeof(CLAN_REQUEST_BASIC_INFO) * ui8count)); 
					pSession->SendClanRequestList(i32Result, ui8section, ui8count, m_aTempRequestBasicInfo); 
				}                
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_REQUEST_INFO_ACK		: 
		{
			CLAN_REQUEST_DETAIL_INFO ClanRequestDetailInfo; 
			pPacket->ReadData(&i32SessionIdx,			sizeof(INT32));
			pPacket->ReadData(&i64UID,					sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,				sizeof(INT32)); 
				pPacket->ReadData(&ClanRequestDetailInfo,	sizeof(CLAN_REQUEST_DETAIL_INFO)); 
				pSession->SendClanRequestDetailInfo(i32Result, &ClanRequestDetailInfo);
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break; 	
	case PROTOCOL_CS_ACCEPT_REQUEST_ACK		: 
		{
			INT32	i32Count;
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Count,		sizeof(INT32));

				pSession->SendClanAcceptRequest( i32Count );

				if( i32Count > MAX_MEMBER_COUNT ) 
				{
					g_pLog->WriteLog( L"PROTOCOL_CS_ACCEPT_REQUEST_ACK Error" );
					break;
				}

				if( i32Count > 0 )
				{
					T_UID i64ClanMemberUID[ MAX_MEMBER_COUNT ];
					pPacket->ReadData( i64ClanMemberUID,	sizeof(T_UID) * i32Count);
					// 가입 쪽지
					TTCHAR   strMessage[ NET_NOTE_SIZE ];
					strMessage[ 0 ] = (TTCHAR)(sizeof(UINT8) + 1); // NULL
					strMessage[ 1 ] = (TTCHAR)(NOTE_MESSAGE_CLAN_INVITE_ACCEPT);
					strMessage[ 2 ] = '\0';
					g_pModuleMessenger->MessengerSendClanGroupNote( MODULE_CLAN_THREAD_IDX, pSession, i32Count, i64ClanMemberUID, pSession->m_UserInfoBasic.m_ui32Clanidx, 0, 0, strMessage, pSession->m_UserInfoBasic.m_strClanName );

					for( INT32 i = 0 ; i < i32Count ; i++ )
					{
						g_pModuleContextNC->LogSendMessage( MODULE_CLAN_THREAD_IDX, LOG_MESSAGE_U_CLAN_JOIN, pSession, &i64ClanMemberUID[i] );
					}
				}
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break; 
	case PROTOCOL_CS_DENIAL_REQUEST_ACK		: 
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,	sizeof(INT32));

				pSession->SendClanDenialRequest(i32Result);

				if( i32Result > MAX_MEMBER_COUNT ) 
				{
					g_pLog->WriteLog( L"PROTOCOL_CS_DENIAL_REQUEST_ACK Error" );
					break;
				}

				if( i32Result > 0 )
				{
					T_UID i64ClanMemberUID[ MAX_MEMBER_COUNT ];					

					pPacket->ReadData( i64ClanMemberUID,	sizeof(T_UID) * i32Result);

					// 거절 쪽지
					TTCHAR   strMessage[ NET_NOTE_SIZE ];
					strMessage[ 0 ] = (TTCHAR)(sizeof(UINT8) + 1); // NULL
					strMessage[ 1 ] = (TTCHAR)(NOTE_MESSAGE_CLAN_INVITE_DENIAL);
					strMessage[ 2 ] = '\0';

					g_pModuleMessenger->MessengerSendClanGroupNote( MODULE_CLAN_THREAD_IDX, pSession, i32Result, i64ClanMemberUID, pSession->m_UserInfoBasic.m_ui32Clanidx, 0, 0, strMessage, pSession->m_UserInfoBasic.m_strClanName );
				}
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_INVITE_ACCEPT_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				UINT32				ui32ClanIdx;
				T_UID				wMasterDBIdx;
				CLAN_DETAIL_INFO	DetailInfo;

				pPacket->ReadData(&i32Result,		sizeof(INT32));
				pPacket->ReadData(&ui32ClanIdx,		sizeof(UINT32));
				pPacket->ReadData(&wMasterDBIdx,	sizeof(T_UID));

				// 쪽지               
				if( EV_SUCCESSED( i32Result ) )
				{	// 클랜 가입 수락 쪽지
					pPacket->ReadData(&DetailInfo,		sizeof(	CLAN_DETAIL_INFO));
					
					TTCHAR   strMessage[ NET_NOTE_SIZE ];
					strMessage[ 0 ] = (TTCHAR)(sizeof(UINT8) + i3String::Length( pSession->m_UserInfoBasic.m_strNick ) + 1);
					strMessage[ 1 ] = (TTCHAR)(NOTE_MESSAGE_CLAN_JOIN_ACCEPT);
					i3String::Copy( &strMessage[ 2 ], pSession->m_UserInfoBasic.m_strNick, NET_NOTE_SIZE-2 );
					
					g_pModuleMessenger->MessengerSendClanNote( MODULE_CLAN_THREAD_IDX, pSession, wMasterDBIdx, ui32ClanIdx, 0, 0, strMessage, DetailInfo.m_name );

					pSession->SendClanAcceptResultRequest( &DetailInfo );
				}
				else if( EV_IS_ERROR( i32Result, EVENT_ERROR_CLAN_INVITED_REFUSAL ) )
				{	// 클랜 가입 거부 쪽지
					UINT8	ui8NameSize;
					TTCHAR	strClanName[ NET_CLAN_NAME_SIZE ];

					pPacket->ReadData( &ui8NameSize,	sizeof(	UINT8));	LENGTH_CHECK( ui8NameSize, NET_CLAN_NAME_SIZE );
					pPacket->ReadData( strClanName,		sizeof(TTCHAR) * ui8NameSize );
					strClanName[ NET_CLAN_NAME_SIZE-1 ] = '\0';

					TTCHAR   strMessage[ NET_NOTE_SIZE ];
					strMessage[ 0 ] = (TTCHAR)(sizeof(UINT8) + i3String::Length( pSession->m_UserInfoBasic.m_strNick ) + 1);
					strMessage[ 1 ] = (TTCHAR)(NOTE_MESSAGE_CLAN_JOIN_DENIAL);
					i3String::Copy( &strMessage[ 2 ], pSession->m_UserInfoBasic.m_strNick, NET_NOTE_SIZE-2 );

					g_pModuleMessenger->MessengerSendClanNote( MODULE_CLAN_THREAD_IDX, pSession, wMasterDBIdx, ui32ClanIdx, 0, 0, strMessage, strClanName );
				}
				else
				{ 
					pSession->SendClanAcceptRequestError( i32Result );
				}
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_CHATTING_ACK:
		{
			UINT8	ui8UserCount;
			INT32	ai32Session[ MAX_MEMBER_COUNT ];
			T_UID	ai64RecvUID[ MAX_MEMBER_COUNT ];
			UINT8	ui8NickSize;
			UINT8	ui8NickColor;
			UINT8	ui8ChatSize;
			TTCHAR	strNick[ NET_NICK_NAME_SIZE ];
			TTCHAR	strChating[ NET_CLAN_CHATING_SIZE ];
			bool	bIsGMUser;

			pPacket->ReadData( &ui8UserCount,		sizeof(UINT8) );
			if( ui8UserCount > MAX_MEMBER_COUNT ) break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(T_UID) * ui8UserCount);
			pPacket->ReadData( &ui8NickSize,		sizeof(UINT8) );
			if( ui8NickSize > NET_NICK_NAME_SIZE )	break;
			pPacket->ReadData( strNick,				sizeof(TTCHAR) * ui8NickSize );
			pPacket->ReadData( &bIsGMUser,			sizeof(bool) );
			pPacket->ReadData( &ui8NickColor,		sizeof(UINT8) );
			pPacket->ReadData( &ui8ChatSize,		sizeof(UINT8) );
			pPacket->ReadData( strChating,			sizeof(TTCHAR) * ui8ChatSize );

			strNick[ NET_NICK_NAME_SIZE -1 ] = '\0';
			strChating[ NET_CLAN_CHATING_SIZE - 1] = '\0';

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[i], ai64RecvUID[i] ); 
				if(pSession != NULL)
				{
					pSession->SendClanChatingRequest( ui8NickSize, strNick, ui8ChatSize, strChating, bIsGMUser, ui8NickColor );
				}
				else
				{
					//SendUnknownUser_A( iRecvUID[i] );
				}
			}
		}
		break;
	case PROTOCOL_CS_PAGE_CHATTING_ACK:
		{
			UINT8	ui8UserCount;
			INT32	ai32Session[ MAX_MEMBER_COUNT ];
			T_UID	i64RecvUID[ MAX_MEMBER_COUNT ];
			UINT8	ui8NickSize;
			UINT8	ui8NickColor;
			UINT8	ui8ChatSize;
			TTCHAR	strNick[ NET_NICK_NAME_SIZE ];
			TTCHAR	strChating[ NET_CLAN_CHATING_SIZE ];
			bool	bIsGMUser;

			pPacket->ReadData( &ui8UserCount,		sizeof(UINT8) );
			if( ui8UserCount > MAX_MEMBER_COUNT ) break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( i64RecvUID,			sizeof(T_UID) * ui8UserCount);
			pPacket->ReadData( &ui8NickSize,		sizeof(UINT8) );
			if( ui8NickSize > NET_NICK_NAME_SIZE )	break;
			pPacket->ReadData( strNick,				sizeof(TTCHAR) * ui8NickSize );
			pPacket->ReadData( &bIsGMUser,			sizeof(bool) );
			pPacket->ReadData( &ui8NickColor,		sizeof(UINT8) );
			pPacket->ReadData( &ui8ChatSize,		sizeof(UINT8) );
			pPacket->ReadData( strChating,			sizeof(TTCHAR) * ui8ChatSize );

			strNick[ NET_NICK_NAME_SIZE -1 ] = '\0';
			strChating[ NET_CLAN_CHATING_SIZE - 1] = '\0';

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[i], i64RecvUID[i] ); 
				if(pSession != NULL)
				{
					pSession->SendClanPageChatingRequest( ui8NickSize, strNick, ui8ChatSize, strChating, bIsGMUser, ui8NickColor );
				}
				else
				{
					//SendUnknownUser_A( wDBIdx[i] );
				}
			}
		}
		break;
	case PROTOCOL_CS_SECESSION_CLAN_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID));
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData( &i32Result, sizeof(INT32) );

				if( EV_SUCCESSED( i32Result ) )
				{
					UINT32	ui32Master;
					pPacket->ReadData( &ui32Master, sizeof(UINT32) );

					// 탈퇴 쪽지
					TTCHAR   strMessage[ NET_NOTE_SIZE ];
					strMessage[ 0 ] = (TTCHAR)(sizeof(UINT8) + i3String::Length( pSession->m_UserInfoBasic.m_strNick ) + 1);
					strMessage[ 1 ] = (TTCHAR)NOTE_MESSAGE_CLAN_SECESSION;
					i3String::Copy( &strMessage[ 2 ], pSession->m_UserInfoBasic.m_strNick, NET_NOTE_SIZE-2 );

					g_pModuleContextNC->LogSendMessage( MODULE_CLAN_THREAD_IDX, LOG_MESSAGE_U_CLAN_SECESSION, pSession ); 
					g_pModuleMessenger->MessengerSendClanNote( MODULE_CLAN_THREAD_IDX, pSession, ui32Master, pSession->m_UserInfoBasic.m_ui32Clanidx, 0, 0, strMessage, pSession->m_UserInfoBasic.m_strClanName );
				}

				pSession->SendClanSecessionRequest(i32Result);
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;	
	case PROTOCOL_CS_DEPORTATION_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,		sizeof(INT32));

				if( i32Result > MAX_MEMBER_COUNT ) 
				{
					g_pLog->WriteLog( L"PROTOCOL_CS_DEPORTATION_ACK Error" );
					break;
				}

				if( i32Result > 0 )
				{
					T_UID ai64ClanMemberUID[ MAX_MEMBER_COUNT ];
					pPacket->ReadData(ai64ClanMemberUID,	sizeof(T_UID) * i32Result);
				
					// 제명 쪽지
					TTCHAR   strMessage[ NET_NOTE_SIZE ];
					strMessage[ 0 ] = (TTCHAR)(sizeof(UINT8) + 1); // NULL
					strMessage[ 1 ] = (TTCHAR)(NOTE_MESSAGE_CLAN_DEPORTATION);
					strMessage[ 2 ] = '\0';
					g_pModuleMessenger->MessengerSendClanGroupNote( MODULE_CLAN_THREAD_IDX, pSession, i32Result, ai64ClanMemberUID, pSession->m_UserInfoBasic.m_ui32Clanidx, 0, 0, strMessage, pSession->m_UserInfoBasic.m_strClanName );


					for( INT32 i = 0 ; i < i32Result ; i++ )
					{
						g_pModuleContextNC->LogSendMessage( MODULE_CLAN_THREAD_IDX, LOG_MESSAGE_U_CLAN_DEPORTATION, pSession, &ai64ClanMemberUID[i] );
					}
				}

				pSession->SendClanDeportationRequest(i32Result);
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_CLOSE_CLAN_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				//m_bNewClanCreate = TRUE;
				pPacket->ReadData(&i32Result,		sizeof(INT32));
				if( EV_SUCCESSED( i32Result ) )
				{
					UINT8	ui8ClanRank;
					UINT32	ui32ClanExp;
					REAL32	r32ClanPoint;
					pPacket->ReadData(&ui8ClanRank,				sizeof(UINT8)); 
					pPacket->ReadData(&ui32ClanExp,				sizeof(UINT32)); 
					pPacket->ReadData(&r32ClanPoint,			sizeof(REAL32)); 

					g_pModuleContextNC->LogSendMessage( MODULE_CLAN_THREAD_IDX, LOG_MESSAGE_U_CLAN_DELETE, pSession, &ui8ClanRank, &ui32ClanExp, &r32ClanPoint );
				}

				pSession->SendClanCloseRequest(i32Result);
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;	
	case PROTOCOL_CS_COMMISSION_MASTER_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,		sizeof(INT32));			

				pSession->SendClanCommissionMasterRequest(i32Result);

				if( EV_SUCCESSED( i32Result ) )
				{
					UINT32 wMaserDBIDx;
					pPacket->ReadData(&wMaserDBIDx,		sizeof(UINT32));

					// 마스터 임명 쪽지
					TTCHAR   strMessage[ NET_NOTE_SIZE ];
					strMessage[ 0 ] = (TTCHAR)(sizeof(UINT8) + 1);
					strMessage[ 1 ] = (TTCHAR)(NOTE_MESSAGE_CLAN_MASTER);
					strMessage[ 2 ] = '\0';

					g_pModuleMessenger->MessengerSendClanNote( MODULE_CLAN_THREAD_IDX, pSession, wMaserDBIDx, pSession->m_UserInfoBasic.m_ui32Clanidx, 0, 0, strMessage, pSession->m_UserInfoBasic.m_strClanName );
				}
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_COMMISSION_STAFF_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,		sizeof(INT32));				

				pSession->SendClanCommissionStaffRequest(i32Result);

				if( i32Result > MAX_MEMBER_COUNT ) 
				{
					g_pLog->WriteLog( L"PROTOCOL_CS_COMMISSION_STAFF_ACK Error" );
					break;
				}

				if( i32Result > 0 )
				{
					T_UID ai64ClanMemberUID[ MAX_MEMBER_COUNT ];
					pPacket->ReadData( ai64ClanMemberUID,		sizeof(T_UID) * i32Result );

					// 스텝 임명 쪽지					
					TTCHAR   strMessage[ NET_NOTE_SIZE ];
					strMessage[ 0 ] = (TTCHAR)(sizeof(UINT8) + 1);
					strMessage[ 1 ] = (TTCHAR)(NOTE_MESSAGE_CLAN_STAFF);
					strMessage[ 2 ] = '\0';

					g_pModuleMessenger->MessengerSendClanGroupNote( MODULE_CLAN_THREAD_IDX, pSession, i32Result, ai64ClanMemberUID, pSession->m_UserInfoBasic.m_ui32Clanidx, 0, 0, strMessage, pSession->m_UserInfoBasic.m_strClanName );
				}
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_COMMISSION_REGULAR_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,		sizeof(INT32));

				pSession->SendClanCommissionRegularRequest(i32Result);

				if( i32Result > MAX_MEMBER_COUNT ) 
				{
					g_pLog->WriteLog( L"PROTOCOL_CS_COMMISSION_REGULAR_ACK Error" );
					break;
				}

				if( i32Result > 0 )
				{
					T_UID ai64ClanMemberUID[ MAX_MEMBER_COUNT ];
					pPacket->ReadData( ai64ClanMemberUID,		sizeof(T_UID) * i32Result );

					// 일반 임명 쪽지
					TTCHAR   strMessage[ NET_NOTE_SIZE ];
					strMessage[ 0 ] = (TTCHAR)(sizeof(UINT8) + 1);
					strMessage[ 1 ] = (TTCHAR)(NOTE_MESSAGE_CLAN_REGULAR);
					strMessage[ 2 ] = '\0';

					g_pModuleMessenger->MessengerSendClanGroupNote( MODULE_CLAN_THREAD_IDX, pSession, i32Result, ai64ClanMemberUID, pSession->m_UserInfoBasic.m_ui32Clanidx, 0, 0, strMessage, pSession->m_UserInfoBasic.m_strClanName );
				}
			}
			else
			{
				//SendUnknownUser_A(iDbIdx);
			}
		}
		break;	
	case PROTOCOL_CS_REPLACE_NOTICE_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,		sizeof(INT32));

				pSession->SendClanReplaceNoticeRequest(i32Result);
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_REPLACE_INTRO_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,		sizeof(INT32));

				pSession->SendClanReplaceIntroRequest(i32Result);
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_NOTE_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				T_UID ai64ClanMemberUID[ MAX_MEMBER_COUNT ];
				UINT8  ui8MessageSize;
				TTCHAR   strMessage[ NET_NOTE_SIZE ];
				pPacket->ReadData(&i32Result,		sizeof(INT32));				

				pSession->SendClanNoteRequest(i32Result);

				if( i32Result > MAX_MEMBER_COUNT ) 
				{
					g_pLog->WriteLog( L"PROTOCOL_CS_NOTE_ACK Error" );
					break;
				}

				if( i32Result > 0 )
				{
					pPacket->ReadData(ai64ClanMemberUID,	sizeof(T_UID) * i32Result);
					pPacket->ReadData(&ui8MessageSize,	sizeof(UINT8));
					pPacket->ReadData(strMessage,		sizeof(TTCHAR) * ui8MessageSize);

					g_pModuleMessenger->MessengerSendClanGroupNote( MODULE_CLAN_THREAD_IDX, pSession, i32Result, ai64ClanMemberUID, pSession->m_UserInfoBasic.m_ui32Clanidx, 0, ui8MessageSize, strMessage, pSession->m_UserInfoBasic.m_strClanName );
				}
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_REPLACE_NAME_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,		sizeof(INT32));

				pSession->SendClanReplaceNameRequest(i32Result);
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_CHECK_MARK_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,		sizeof(INT32));

				pSession->SendClanCheckMarkRequest(i32Result);
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_CHANGE_CLAN_EXP_RANK_ACK:
		{
			UINT8	ui8UserCount;
			INT32	ai32Session[ MAX_MEMBER_COUNT ];
			T_UID	ai64RecvUID[ MAX_MEMBER_COUNT ];
			UINT8	ui8Rank;

			pPacket->ReadData(&ui8UserCount,		sizeof(UINT8));
			if( ui8UserCount > MAX_MEMBER_COUNT )		break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(T_UID) * ui8UserCount); 
			pPacket->ReadData(&ui8Rank,				sizeof(UINT8));

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[ i ], ai64RecvUID[ i ]); 
				if(pSession != NULL)
				{
					pSession->SendClanChangeExpRankRequest(ui8Rank);
				}
				else
				{
					//SendUnknownUser_A(wDBIdx[ i ]); 				
				}
			}
		}
		break;
	case PROTOCOL_CS_CHANGE_CLAN_PER_RANK_ACK:
		{
			UINT8	ui8UserCount;
			INT32	ai32Session[ MAX_MEMBER_COUNT ];
			T_UID	ai64RecvUID[ MAX_MEMBER_COUNT ];
			UINT8	ui8Rank;

			pPacket->ReadData(&ui8UserCount,		sizeof(UINT8));
			if( ui8UserCount > MAX_MEMBER_COUNT )		break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(T_UID) * ui8UserCount); 
			pPacket->ReadData(&ui8Rank,				sizeof(UINT8));

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[ i ], ai64RecvUID[ i ]); 
				if(pSession != NULL)
				{
					pSession->SendClanChangePerRankRequest(ui8Rank);
				}
				else
				{
					//SendUnknownUser_A(wDBIdx[ i ]); 				
				}
			}
		}
		break;
	case PROTOCOL_CS_REPLACE_MARK_RESULT_ACK:		
		{
			UINT8	ui8UserCount;
			INT32	ai32Session[ MAX_MEMBER_COUNT ];
			T_UID	ai64RecvUID[ MAX_MEMBER_COUNT ];
			UINT32  ui32Mark;

			pPacket->ReadData(&ui8UserCount,		sizeof(UINT8));
			if( ui8UserCount > MAX_MEMBER_COUNT )		break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(T_UID) * ui8UserCount); 
			pPacket->ReadData(&ui32Mark,			sizeof(UINT32));

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[ i ], ai64RecvUID[ i ]); 
				if(pSession != NULL)
				{
					pSession->SendClanReplaceMarkResltRequest(ui32Mark);
				}
				else
				{
					//SendUnknownUser_A(wDBIdx[ i ]); 				
				}
			}

			// Trans 서버 NODE 에도 Update시켜줍니다.
			g_pModuleTrans->AuthSendChangeClanMark( MODULE_CLAN_THREAD_IDX, ui32Mark, ui8UserCount, ai64RecvUID );
		}
		break;
	case PROTOCOL_CS_REPLACE_MANAGEMENT_RESULT_ACK:
		{
			UINT8			ui8UserCount;
			INT32			ai32Session[ MAX_MEMBER_COUNT ];
			T_UID			ai64RecvUID[ MAX_MEMBER_COUNT ];
			UINT8			ui8Authority;

			pPacket->ReadData(&ui8UserCount,		sizeof(UINT8));
			if( ui8UserCount > MAX_MEMBER_COUNT )		break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(T_UID) * ui8UserCount); 
			pPacket->ReadData(&ui8Authority,		sizeof(UINT8));

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[ i ], ai64RecvUID[ i ]); 
				if(pSession != NULL)
				{
					pSession->SendClanReplaceManagementResltRequest( ui8Authority );
				}
				else
				{
					//SendUnknownUser_A(wDBIdx[ i ]); 				
				}
			}
		}
		break;
	case PROTOCOL_CS_REPLACE_NAME_RESULT_ACK:
		{
			UINT8	ui8UserCount;
			INT32	ai32Session[ MAX_MEMBER_COUNT ];
			T_UID	ai64RecvUID[ MAX_MEMBER_COUNT ];
			TTCHAR	strName[ NET_CLAN_NAME_SIZE ];

			pPacket->ReadData(&ui8UserCount,		sizeof(UINT8));
			if( ui8UserCount > MAX_MEMBER_COUNT )		break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(T_UID) * ui8UserCount); 
			pPacket->ReadData( strName,				sizeof(TTCHAR) * NET_CLAN_NAME_SIZE);

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[ i ], ai64RecvUID[ i ]); 
				if(pSession != NULL)
				{
					pSession->SendClanReplaceNameResultRequest( strName );
				}
				else
				{
					//SendUnknownUser_A(wDBIdx[ i ]); 				
				}
			}

			// Trans 서버 NODE 에도 Update시켜줍니다.
			g_pModuleTrans->AuthSendChangeClanName( MODULE_CLAN_THREAD_IDX, strName, ui8UserCount, ai64RecvUID );

		}
		break;
	case PROTOCOL_CS_REPLACE_MARK_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,		sizeof(INT32));

				pSession->SendClanReplaceMarkRequest(i32Result);
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_REPLACE_PERSONMAX_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,		sizeof(INT32));
				UINT8 ui8ClanMaxCount = 0;

				if( EV_SUCCESSED(i32Result) )
				{
					pPacket->ReadData(&ui8ClanMaxCount,	sizeof(UINT8));
				}

				pSession->SendClanReplacePersonMaxRequest( i32Result, ui8ClanMaxCount );
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_REPLACE_MANAGEMENT_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,		sizeof(INT32));

				pSession->SendClanReplaceManagementRequest(i32Result);
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_CHECK_JOIN_AUTHORITY_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,		sizeof(INT32));

				pSession->SendClanCheckJoinAuthorityRequest(i32Result);
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_REPLACE_MEMBER_NICK_ACK:
		{
			pPacket->ReadData(&i32SessionIdx, sizeof(INT32));
			pPacket->ReadData(&i64UID,		sizeof(T_UID));
			
			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);

			if (pSession)
			{
				pPacket->ReadData(&i32Result, sizeof(INT32));

				// 이미 게임서버에서는 닉네임이 변경에 성공하였다.
				// Do nothing
			}
			else
			{
				//SendUnknownUser_A(iDbIdx);
			}
		}
		break;
	case PROTOCOL_CS_RECORD_RESET_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,		sizeof(INT32));

				pSession->SendClanRecordResetRequest(i32Result);
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_ACCEPT_REQUEST_RESULT_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				CLAN_DETAIL_INFO	DetailInfo;

				pPacket->ReadData(&DetailInfo,		sizeof(CLAN_DETAIL_INFO));
				
				pSession->SendClanAcceptResultRequest( &DetailInfo);
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_DENIAL_REQUEST_RESULT_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pSession->SendClanDenialResultRequest();
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_DEPORTATION_RESULT_ACK:
		{
			//UINT8					cUserCount;
			//INT32					nSession[ MAX_MEMBER_COUNT ];
			//T_UID					i64RecvUID[ MAX_MEMBER_COUNT ];

			//pPacket->ReadData( &cUserCount,		sizeof(UINT8) );
			//if( cUserCount > MAX_MEMBER_COUNT )	break;
			//pPacket->ReadData( nSession,			sizeof(INT32) * cUserCount);
			//pPacket->ReadData( i64RecvUID,		sizeof(T_UID) * cUserCount);			

			//for( UINT8 i = 0 ; i < cUserCount ; i++ )
			//{
			//	CUserSession * pSession = g_pUserManager->GetSession( nSession[i], i64RecvUID[i] ); 
			//	if(pSession != NULL)
			//	{
			//		pSession->SendClanDeportationResultRequest( );
			//	}
			//	else
			//	{
			//		//SendUnknownUser_A( wDBIdx[i] );
			//	}
			//}
			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData( &i64UID,				sizeof(T_UID));

			CUserSession * pSession = g_pUserManager->GetSession( i32SessionIdx, i64UID ); 
			if(pSession != NULL)
			{
				pSession->SendClanDeportationResultRequest( );
			}
		}
		break;
	case PROTOCOL_CS_COMMISSION_MASTER_RESULT_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pSession->SendClanMasterCommissionResultRequest();
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_COMMISSION_STAFF_RESULT_ACK:
		{
			//UINT8					cUserCount;
			//INT32					nSession;
			//T_UID					i64RecvUID[ MAX_MEMBER_COUNT ];

			//pPacket->ReadData( &cUserCount,			sizeof(UINT8) );
			//if( cUserCount > MAX_MEMBER_COUNT )	break;
			//pPacket->ReadData( iSessionIdx,			sizeof(INT32) * cUserCount);
			//pPacket->ReadData( i64RecvUID,			sizeof(T_UID) * cUserCount);	

			//for( UINT8 i = 0 ; i < cUserCount ; i++ )
			//{
			//	CUserSession * pSession = g_pUserManager->GetSession( nSession[i], i64RecvUID[i] ); 
			//	if(pSession != NULL)
			//	{
			//		pSession->SendClanStaffCommissionResultRequest( );
			//	}
			//	else
			//	{
			//		//SendUnknownUser_A( wDBIdx[i] );
			//	}
			//}

			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32) );
			pPacket->ReadData( &i64UID,				sizeof(T_UID) );	

			CUserSession * pSession = g_pUserManager->GetSession( i32SessionIdx, i64UID ); 
			if(pSession != NULL)
			{
				pSession->SendClanStaffCommissionResultRequest( );
			}
		}
		break;
	case PROTOCOL_CS_COMMISSION_REGULAR_RESULT_ACK:
		{
			//UINT8					cUserCount;
			//INT32					nSession[ MAX_MEMBER_COUNT ];
			//T_UID					i64RecvUID[ MAX_MEMBER_COUNT ];

			//pPacket->ReadData( &cUserCount,			sizeof(UINT8) );
			//if( cUserCount > MAX_MEMBER_COUNT )	break;
			//pPacket->ReadData( nSession,			sizeof(INT32) * cUserCount);
			//pPacket->ReadData( i64RecvUID,		sizeof(UINT32) * cUserCount);			

			//for( UINT8 i = 0 ; i < cUserCount ; i++ )
			//{
			//	CUserSession * pSession = g_pUserManager->GetSession( nSession[i], i64RecvUID[i] ); 
			//	if(pSession != NULL)
			//	{
			//		pSession->SendClanRegularCommissionResultRequest( );
			//	}
			//	else
			//	{
			//		//SendUnknownUser_A( wDBIdx[i] );
			//	}
			//}
			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32) );
			pPacket->ReadData( &i64UID,				sizeof(T_UID) );	

			CUserSession * pSession = g_pUserManager->GetSession( i32SessionIdx, i64UID ); 
			if(pSession != NULL)
			{
				pSession->SendClanRegularCommissionResultRequest( );
			}
		}
		break;		
	case PROTOCOL_CS_ROOM_INVITED_ACK:			
		{
			// 배틀룸에 친구 초대가 실패하면 에러 코드를 전송한다.(EVENT_ERROR_EVENT_NOT_LOCATION_LOBBY_USER)
			// 배틀룸에 초대한 친구가 클랜전의 팀등록 중이면 경고 코드를 전송한다.(EVENT_ERROR_EVENT_FAIL_INVITED_USER_IN_CLAN_MATCH)

			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,		sizeof(INT32));

				pSession->SendClanRoomInvitedRequest(i32Result);
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_ROOM_INVITED_RESULT_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				T_UID TSenderUID;
				pPacket->ReadData(&TSenderUID,		sizeof(T_UID));

				pSession->SendClanRoomInvitedResultRequest(TSenderUID);
			}
			else
			{
				//SendUnknownUser_A(iDbIdx);
			}
		}
		break;
	case PROTOCOL_CS_REPLACE_MEMBER_BASIC_RESULT_ACK:
		{
			UINT8					ui8UserCount;
			INT32					ai32Session[ MAX_MEMBER_COUNT ];
			T_UID					ai64RecvUID[ MAX_MEMBER_COUNT ];
			CLAN_MEMBER_BASIC_INFO	MemberBasicInfo;

			pPacket->ReadData( &ui8UserCount,		sizeof(UINT8) );
			if( ui8UserCount > MAX_MEMBER_COUNT )	break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(T_UID) * ui8UserCount);
			pPacket->ReadData( &MemberBasicInfo,	sizeof(CLAN_MEMBER_BASIC_INFO));

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[i], ai64RecvUID[i] ); 
				if(pSession != NULL)
				{
					pSession->SendClanReplaceMemberBasicResultRequest( &MemberBasicInfo );
				}
				else
				{
					//SendUnknownUser_A( wDBIdx[i] );
				}
			}
		}
		break;	
	case PROTOCOL_CS_RECORD_RESET_RESULT_ACK:
		{
			UINT8					ui8UserCount;
			INT32					ai32Session[ MAX_MEMBER_COUNT ];
			T_UID					ai64RecvUID[ MAX_MEMBER_COUNT ];

			pPacket->ReadData( &ui8UserCount,		sizeof(UINT8) );
			if( ui8UserCount > MAX_MEMBER_COUNT )	break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(T_UID) * ui8UserCount);

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[i], ai64RecvUID[i] ); 
				if(pSession != NULL)
				{
					pSession->SendClanRecordResetResultRequest( );
				}
				else
				{
					//SendUnknownUser_A( wDBIdx[i] );
				}
			}
		}
		break;
	case PROTOCOL_CS_POINT_RESET_RESULT_ACK:
		{
			UINT8					ui8UserCount;
			INT32					ai32Session[ MAX_MEMBER_COUNT ];
			T_UID					ai64RecvUID[ MAX_MEMBER_COUNT ];

			pPacket->ReadData( &ui8UserCount,		sizeof(UINT8) );
			if( ui8UserCount > MAX_MEMBER_COUNT )	break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(T_UID) * ui8UserCount);

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[i], ai64RecvUID[i] ); 
				if(pSession != NULL)
				{
					pSession->SendClanPointResetResultRequest();
				}
				else
				{
					//SendUnknownUser_A( wDBIdx[i] );
				}
			}
		}
		break;
	case PROTOCOL_CS_REPLACE_COLOR_NAME_RESULT_ACK:
		{
			UINT8					ui8UserCount;
			INT32					ai32Session[ MAX_MEMBER_COUNT ];
			T_UID					ai64RecvUID[ MAX_MEMBER_COUNT ];
			UINT8					ui8Color;

			pPacket->ReadData( &ui8UserCount,		sizeof(UINT8) );
			if( ui8UserCount > MAX_MEMBER_COUNT )	break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(T_UID) * ui8UserCount);
			pPacket->ReadData( &ui8Color,			sizeof(UINT8));

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[i], ai64RecvUID[i] ); 
				if(pSession != NULL)
				{
					pSession->SendClanReplacColorNameResultRequest( ui8Color );
				}
				else
				{
					//SendUnknownUser_A( wDBIdx[i] );
				}
			}
		}
		break;
	case PROTOCOL_BASE_HEART_BIT_ACK		:
		break;
	case PROTOCOL_CS_MATCH_TEAM_CONTEXT_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				CLAN_BATTLE_TEAM_CONTEXT  TeamContext;

				pPacket->ReadData(&TeamContext,		sizeof(CLAN_BATTLE_TEAM_CONTEXT));

				pSession->SendClanMatchTeamContextRequest( &TeamContext );
			}
			else
			{
				//SendUnknownUser_A(iDbIdx);
			}
		}
		break;
	case PROTOCOL_CS_MATCH_ALL_TEAM_CONTEXT_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				CLAN_BATTLE_ALL_TEAM_CONTEXT AllTeamContext;

				pPacket->ReadData(&AllTeamContext,	sizeof(CLAN_BATTLE_ALL_TEAM_CONTEXT));

				pSession->SendClanMatchAllTeamContextRequest( &AllTeamContext );
			}
			else
			{
				//SendUnknownUser_A(iDbIdx);
			}
		}
		break;
	
	case PROTOCOL_CS_MATCH_ALL_TEAM_LIST_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				UINT16					sTotalCount;
				UINT16					sSectionCount;
				UINT16					sSection;
				UINT8					cCount;
				CLAN_BATTLE_TEAM_INFO	BattleTeamInfo[ 64 ];

				pPacket->ReadData(&sTotalCount,		sizeof(UINT16));
				pPacket->ReadData(&sSectionCount,	sizeof(UINT16));
				pPacket->ReadData(&sSection,		sizeof(UINT16));
				pPacket->ReadData(&cCount,			sizeof(UINT8));
				if( cCount > 64 ) 
				{
					g_pLog->WriteLog( L"PROTOCOL_CS_MATCH_ALL_TEAM_LIST_ACK Count Error");
					return;
				}

				pPacket->ReadData(BattleTeamInfo,	sizeof(CLAN_BATTLE_TEAM_INFO) * cCount );

				pSession->SendClanMatchAllTeamListRequest( sTotalCount, sSectionCount, sSection, cCount, BattleTeamInfo );
			}
			else
			{
				//SendUnknownUser_A(iDbIdx);
			}
		}
		break;
	case PROTOCOL_CS_MATCH_TEAM_DETAIL_INFO_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				CLAN_DETAIL_INFO	Detail;

				pPacket->ReadData(&i32Result,		sizeof(INT32));
				pPacket->ReadData(&Detail,			sizeof(CLAN_DETAIL_INFO));

				pSession->SendClanMatchTeamDetailRequest( i32Result, &Detail );
			}
			else
			{
				//SendUnknownUser_A(iDbIdx);
			}
		}
		break;
	//case PROTOCOL_CS_MATCH_TEAM_CREATE_ACK:
	//	{
	//		pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
	//		pPacket->ReadData(&i64UID,				sizeof(T_UID)); 			
	//		CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
	//		if(pSession != NULL)
	//		{
	//			INT32				i32Success;
	//			UINT32				ui32RoomIdx;			//RoomIdx
	//			CLAN_BATTLE_TEAM	Team;

	//			pPacket->ReadData(&i32Success,		sizeof(INT32));
	//			pPacket->ReadData(&ui32RoomIdx,		sizeof(UINT32));
	//			pPacket->ReadData(&Team,			sizeof(CLAN_BATTLE_TEAM));

	//			pSession->SendClanMatchTeamCreateRequest( i32Success, ui32RoomIdx, &Team );
	//		}
	//		else
	//		{
	//			//SendUnknownUser_A(iDbIdx);
	//		}
	//	}
	//	break;
	
	//case PROTOCOL_CS_MATCH_TEAM_LEAVE_ACK:
	//	{
	//		pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
	//		pPacket->ReadData(&i64UID,				sizeof(T_UID)); 			
	//		CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
	//		if(pSession != NULL)
	//		{
	//			INT32	nSuccess;

	//			pPacket->ReadData(&nSuccess,		sizeof(INT32));

	//			pSession->SendClanMatchTeamLeaveRequest( nSuccess );
	//		}
	//		else
	//		{
	//			//SendUnknownUser_A(iDbIdx);
	//		}
	//	}
	//	break;
	case PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				UINT8 ui8PerMax;
				
				pPacket->ReadData(&i32Result,		sizeof(INT32));
				pPacket->ReadData(&ui8PerMax,			sizeof(UINT8));

				pSession->SendClanMatchTeamChangePerRequest( i32Result, ui8PerMax );
			}
			else
			{
				//SendUnknownUser_A(iDbIdx);
			}
		}
		break;
	case PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_RESULT_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				UINT8 cPerMax;
				pPacket->ReadData(&cPerMax,			sizeof(UINT8));

				pSession->SendClanMatchTeamChangePerResultRequest( cPerMax );
			}
			else
			{
				//SendUnknownUser_A(iDbIdx);
			}
		}
		break;
	//case PROTOCOL_CS_MATCH_TEAM_INFO_ACK:
	//	{
	//		UINT8	ui8UserCount;
	//		INT32	ai32Session[ CLAN_TEAM_MEMBER_COUNT ];
	//		T_UID	ai64RecvUID[ CLAN_TEAM_MEMBER_COUNT ];
	//		CLAN_BATTLE_TEAM		Team;
	//		CLAN_BATTLE_MEMBER		aMemberInfo[ CLAN_TEAM_MEMBER_COUNT ];

	//		pPacket->ReadData( &ui8UserCount,		sizeof(UINT8) );
	//		if( ui8UserCount > CLAN_TEAM_MEMBER_COUNT )	break;
	//		pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
	//		pPacket->ReadData( ai64RecvUID,			sizeof(T_UID) * ui8UserCount);
	//		pPacket->ReadData(&Team,				sizeof(CLAN_BATTLE_TEAM));
	//		pPacket->ReadData(aMemberInfo,			sizeof(CLAN_BATTLE_MEMBER) * Team._PerMax);

	//		for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
	//		{
	//			CUserSession * pSession = g_pUserManager->GetSession( ai32Session[i], ai64RecvUID[i] ); 
	//			if(pSession != NULL)
	//			{
	//				pSession->SendClanMatchTeamInfoRequest( &Team, aMemberInfo );
	//			}
	//			else
	//			{
	//				//SendUnknownUser_A( wDBIdx[i] );
	//			}
	//		}
	//	}
	//	break;

	/*case PROTOCOL_TEAM_MEMBER_CHANGE_ACK:
		{
			INT32 i32MemberCount;
			pPacket->ReadData(&i32MemberCount, sizeof(i32MemberCount));

			CLAN_BATTLE_TEAM stTeamInfo;
			pPacket->ReadData(&stTeamInfo, sizeof(stTeamInfo));

			for( INT32 i = 0; i < i32MemberCount; ++i )
			{
				CLAN_BATTLE_MEMBER stMemberInfo;
				pPacket->ReadData(&stMemberInfo, sizeof(stMemberInfo));

				CUserSession* pTeamMember = g_pUserManager->GetSession(stMemberInfo._i64UID);
				if( pTeamMember )
				{
					g_pLog->WriteLog(g_i32TotalLogIdx, "[ERROR][CModuleClanSocket::OnCreate] Not found member. (UID : %I64d)", stMemberInfo._i64UID);
					continue;
				}

				pTeamMember->SendClanMatchTeamInfoRequest(&stTeamInfo, &stMemberInfo);
			}
		}
		break;
*/
	case PROTOCOL_CS_MATCH_FIGHT_REQUEST_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				INT32 nSuccess;

				pPacket->ReadData(&nSuccess,		sizeof(INT32));

				pSession->SendClanMatchFightRequestRequest( nSuccess );
			}
			else
			{
				//SendUnknownUser_A(iDbIdx);
			}
		}
		break;
	case PROTOCOL_CS_MATCH_CHATING_ACK:
		{
			// 한번에 받아서 처리합니다. // 
			UINT8	ui8UserCount;
			INT32	ai32Session[ CLAN_TEAM_MEMBER_COUNT ];
			T_UID	ai64RecvUID[ CLAN_TEAM_MEMBER_COUNT ];
			UINT8	ui8NickSize;
			UINT8	ui8NickColor;
			UINT8	ui8ChatSize;
			TTCHAR	strNick[ NET_NICK_NAME_SIZE ];
			TTCHAR	strChating[ NET_CLAN_CHATING_SIZE ];
			bool	bIsGMUser;

			pPacket->ReadData( &ui8UserCount,		sizeof(UINT8) );
			if( ui8UserCount > CLAN_TEAM_MEMBER_COUNT ) break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(T_UID) * ui8UserCount);
			pPacket->ReadData( &ui8NickSize,		sizeof(UINT8) );
			if( ui8NickSize > NET_NICK_NAME_SIZE )	break;
			pPacket->ReadData( strNick,				sizeof(TTCHAR) * ui8NickSize );
			pPacket->ReadData( &bIsGMUser,			sizeof(bool) );
			pPacket->ReadData( &ui8NickColor,		sizeof(UINT8) );
			pPacket->ReadData( &ui8ChatSize,		sizeof(UINT8) );
			pPacket->ReadData( strChating,			sizeof(TTCHAR) * ui8ChatSize );

			strNick[ NET_NICK_NAME_SIZE -1 ] = '\0';
			strChating[ NET_CLAN_CHATING_SIZE - 1] = '\0';

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[i], ai64RecvUID[i] ); 
				if(pSession != NULL)
				{
					pSession->SendClanMatchChatingRequest( ui8NickSize, strNick, ui8ChatSize, strChating, bIsGMUser, ui8NickColor );
				}
				else
				{
					//SendUnknownUser_A( wDBIdx[i] );
				}
			}
		}
		break;
	case PROTOCOL_CS_MATCH_FIGHT_REQUEST_RESULT_ACK:
		{
			// 한번에 받아서 처리합니다. // 
			UINT8					ui8UserCount;
			INT32					ai32Session[ CLAN_TEAM_MEMBER_COUNT ];
			T_UID					ai64RecvUID[ CLAN_TEAM_MEMBER_COUNT ];
			CLAN_BATTLE_TEAM_INFO	TeamInfo;

			pPacket->ReadData( &ui8UserCount,		sizeof(UINT8) );
			if( ui8UserCount > CLAN_TEAM_MEMBER_COUNT )	break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(T_UID) * ui8UserCount);
			pPacket->ReadData(&TeamInfo,			sizeof(CLAN_BATTLE_TEAM_INFO));

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[i], ai64RecvUID[i] ); 
				if(pSession != NULL)
				{
					pSession->SendClanMatchFightResultRequest( &TeamInfo );
				}
				else
				{
					//SendUnknownUser_A( wDBIdx[i] );
				}
			}
		}
		break;
	case PROTOCOL_CS_MATCH_FIGHT_ACCECT_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				INT32 nSuccess;

				pPacket->ReadData(&nSuccess,		sizeof(INT32));

				pSession->SendClanMatchFightAccectRequest( nSuccess );
			}
			else
			{
				//SendUnknownUser_A(iDbIdx);
			}
		}
		break;	
	case PROTOCOL_CS_MATCH_FIGHT_ACCECT_RESULT_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				INT32 nSuccess;

				pPacket->ReadData(&nSuccess,		sizeof(INT32));

				pSession->SendClanMatchFightAccectResultRequest( nSuccess );
			}
			else
			{
				//SendUnknownUser_A(iDbIdx);
			}
		}
		break;
	case PROTOCOL_CS_MATCH_ROOM_CREATE_GS_REQ:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if( !pSession )
				break;

			UINT32				ui32TeamIdx;
			UINT32				ui32RequsetTeamIdx;
			UINT8				ui8PerMax;
			CLAN_BATTLE_TEAM	BattleTeam;
			CLAN_BATTLE_CLAN	BattleClan;

			pPacket->ReadData(&ui32TeamIdx,			sizeof(UINT32));
			pPacket->ReadData(&ui32RequsetTeamIdx,	sizeof(UINT32));
			pPacket->ReadData(&ui8PerMax,			sizeof(UINT8));
			pPacket->ReadData(&BattleTeam,			sizeof(CLAN_BATTLE_TEAM));
			pPacket->ReadData(&BattleClan,			sizeof(CLAN_BATTLE_CLAN));

			pSession->SendClanMatchFightIfoResultRequest( &BattleTeam, &BattleClan );
			pSession->SendClanBattleRoomCreate( ui32TeamIdx, ui32RequsetTeamIdx, ui8PerMax );
		}
		break;
	case PROTOCOL_CS_MATCH_ROOM_JOIN_GS_REQ:
		{
			UINT8 ui8UserCount;
			pPacket->ReadData(&ui8UserCount, sizeof(ui8UserCount));
			if( CLAN_TEAM_MEMBER_COUNT < ui8UserCount )
				break;

			INT32				ai32Session[ CLAN_TEAM_MEMBER_COUNT ];
			T_UID				ai64RecvUID[ CLAN_TEAM_MEMBER_COUNT ];
			UINT32				ui32RoomIdx;
			UINT32				ui32TeamIdx;
			CLAN_BATTLE_TEAM	BattleTeam;
			CLAN_BATTLE_CLAN	BattleClan;

			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(T_UID) * ui8UserCount);
			pPacket->ReadData(&ui32RoomIdx,			sizeof(UINT32) );
			pPacket->ReadData(&ui32TeamIdx,			sizeof(UINT32) );
			pPacket->ReadData(&BattleTeam,			sizeof(CLAN_BATTLE_TEAM));
			pPacket->ReadData(&BattleClan,			sizeof(CLAN_BATTLE_CLAN));

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[i], ai64RecvUID[i] ); 
				if(pSession != NULL)
				{
					pSession->SendClanMatchFightIfoResultRequest( &BattleTeam, &BattleClan );
					pSession->SendClanBattleRoomJoin( ui32RoomIdx, ui32TeamIdx );
				}
				else
				{
					//SendUnknownUser_A( wDBIdx[i] );
				}
			}
		}
		break;
	case PROTOCOL_CS_MATCH_ROOM_LEAVE_GS_REQ:
		{
			UINT8				ui8UserCount;
			INT32				ai32Session[ CLAN_TEAM_MEMBER_COUNT ];
			T_UID				ai64RecvUID[ CLAN_TEAM_MEMBER_COUNT ];
			CLAN_BATTLE_CLAN	BattleClan;

			pPacket->ReadData( &ui8UserCount,		sizeof(UINT8) );
			if( ui8UserCount > CLAN_TEAM_MEMBER_COUNT )	break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(T_UID) * ui8UserCount);

			pPacket->ReadData(&BattleClan,		sizeof(CLAN_BATTLE_CLAN));

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[i], ai64RecvUID[i] ); 
				if(pSession != NULL)
				{
					//pSession->SendClanBattleRoomLeave();
					pSession->SendClanBattleChangeClanInfo( &BattleClan );
				}
				else
				{
					//SendUnknownUser_A( wDBIdx[i] );
				}
			}
		}
		break;
	
		
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// CLAN Client

	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// CLAN CS
	case PROTOCOL_CS_CLAN_MATCH_RESULT_CONTEXT_ACK:
		{
			PACKET_GC_H_CLAN_ACK							PacketAckGCH	= {0,};
			PACKET_H_CLAN_ACK								PacketAckH		= {0,};
			PACKET_D_CS_CLAN_MATCH_RESULT_CONTEXT			PacketAckD		= {0,};

			pPacket->ReadData(&PacketAckGCH, sizeof(PACKET_GC_H_CLAN_ACK));
			PacketAckH.m_i32Result	= PacketAckGCH.m_i32Result;
			
			CUserSession * pSession = g_pUserManager->GetSession(PacketAckGCH.m_i32SessionIdx, PacketAckGCH.m_i64UID); 
			if(pSession != NULL)
			{
				if(EVENT_ERROR_SUCCESS == PacketAckH.m_i32Result)
				{					
					pPacket->ReadData(&PacketAckD, sizeof(PACKET_D_CS_CLAN_MATCH_RESULT_CONTEXT));
					pSession->SendClanMatchResultContext(&PacketAckH, &PacketAckD); 
				}				
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break; 
	case PROTOCOL_CS_CLAN_MATCH_RESULT_LIST_ACK		:	//Member List 받기 
		{
			PACKET_GC_H_CLAN_ACK							PacketAckGCH	= {0,};
			PACKET_H_CLAN_ACK								PacketAckH		= {0,};
			PACKET_D_CS_CLAN_MATCH_RESULT_LIST_CNT		PacketAckDCnt	= {0,};
			PACKET_D_CS_CLAN_MATCH_RESULT_LIST			arrPacketAckD[MATCH_RESULT_SECTION_SIZE];
			
			pPacket->ReadData(&PacketAckGCH, sizeof(PACKET_GC_H_CLAN_ACK));
			PacketAckH.m_i32Result	= PacketAckGCH.m_i32Result;

			CUserSession * pSession = g_pUserManager->GetSession(PacketAckGCH.m_i32SessionIdx, PacketAckGCH.m_i64UID);
			if(pSession != NULL)
			{
				pPacket->ReadData( &PacketAckDCnt,	sizeof(PACKET_D_CS_CLAN_MATCH_RESULT_LIST_CNT) ); 

				if( PacketAckDCnt.m_ui8iSection < ((MATCH_RESULT_SECTION_SIZE * MAX_LMRC)+10) )
				{
					pPacket->ReadData( arrPacketAckD, (sizeof(PACKET_D_CS_CLAN_MATCH_RESULT_LIST) * PacketAckDCnt.m_ui8iSection)); 
					pSession->SendClanMatchResultList(&PacketAckH, &PacketAckDCnt, arrPacketAckD); 
				}
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break; 
	case PROTOCOL_CS_CLAN_MATCH_RESULT_EMOTION_SAVE_ACK:
		{
			PACKET_GC_H_CLAN_ACK								PacketAckGCH	= {0,};
			PACKET_H_CLAN_ACK									PacketAckH		= {0,};			

			pPacket->ReadData(&PacketAckGCH, sizeof(PACKET_GC_H_CLAN_ACK));
			PacketAckH.m_i32Result	= PacketAckGCH.m_i32Result;

			CUserSession * pSession = g_pUserManager->GetSession(PacketAckGCH.m_i32SessionIdx, PacketAckGCH.m_i64UID);
			if(pSession != NULL)
			{
				PACKET_D_CS_CLAN_MATCH_RESULT_EMOTION_SAVE		PacketAckD		= {0,};
				pPacket->ReadData(&PacketAckD,	sizeof(PACKET_D_CS_CLAN_MATCH_RESULT_EMOTION_SAVE)); 
				if(EV_SUCCESSED(PacketAckGCH.m_i32Result))	{ pSession->SendClanMatchResultEmotionSave(&PacketAckH, &PacketAckD); }
				else										{ pSession->SendClanMatchResultEmotionSaveFail(&PacketAckH); }				
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_ACK:
		{
			PACKET_GC_H_CLAN_ACK								PacketAckGCH	= {0,};
			PACKET_H_CLAN_ACK									PacketAckH		= {0,};

			pPacket->ReadData(&PacketAckGCH, sizeof(PACKET_GC_H_CLAN_ACK));
			PacketAckH.m_i32Result	= PacketAckGCH.m_i32Result;

			CUserSession * pSession = g_pUserManager->GetSession(PacketAckGCH.m_i32SessionIdx, PacketAckGCH.m_i64UID);
			if(pSession != NULL)
			{
				pSession->SendClanRequestAutoAcceptOption(&PacketAckH); 
			}
			else
			{
				//SendUnknownUser_A(iDbIdx); 				
			}
		}
		break;
	case PROTOCOL_CS_GS_CREATE_CLAN_REQ:
		{
			//CLAN_CONTEXT	stClanContext;
			//UINT16			ui16ClanIdx;
			//CLAN_BASIC_INFO stClanInfo;
			//CLAN_BASIC_INFO* pClanList = g_pContextMain->GetClanList();

			//pPacket->ReadData( &stClanContext,		sizeof(CLAN_CONTEXT));
			//pPacket->ReadData( &ui16ClanIdx,		sizeof(UINT16));
			//pPacket->ReadData( &stClanInfo,			sizeof(CLAN_BASIC_INFO));

			//g_pContextMain->m_ClanContext._totalCount	= stClanContext._totalCount;
			//g_pContextMain->m_ClanContext._sectionCount	= stClanContext._sectionCount;
			//g_pContextMain->m_ClanContext._sectionSize	= stClanContext._sectionSize;

			//i3mem::Copy( &pClanList[ ui16ClanIdx ], &stClanInfo, sizeof(CLAN_BASIC_INFO) );
		}
		break;
	case PROTOCOL_CS_GS_DESTROY_CLAN_REQ:
		{
		}
		break;
	case PROTOCOL_GC_CLAN_USER_INFO_ACK:
		{					
		}
		break;


	case PROTOCOL_CLAN_WAR_MATCH_TEAM_COUNT_ACK:
		{
			INT32 i32MatchedTeamCount = 0;
			INT32 i32MatchingTeamCount = 0;
			
			pPacket->ReadData(&i32MatchedTeamCount, sizeof(INT32));
			pPacket->ReadData(&i32MatchingTeamCount, sizeof(INT32));

			g_pContextMain->SetMatchedTeamCount( i32MatchedTeamCount );
			g_pContextMain->SetMatchingTeamCount( i32MatchingTeamCount ); 
		}
		break;

	case PROTOCOL_CLAN_WAR_MATCH_TEAM_LIST_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession == NULL)	break;

			PACKET_CLAN_WAR_MATCH_TEAM_LIST_ACK	SendPacket;
			MATCH_TEAM_INFO	aTeamInfo[MAX_CLAN_MATCH_TEAM_COUNT];
			UINT8 ui8TeamCount = 0;

			pPacket->ReadData(&ui8TeamCount,		sizeof(UINT8)); 
			pPacket->ReadData( aTeamInfo,			sizeof(MATCH_TEAM_INFO) * ui8TeamCount );
			SendPacket.m_TResult = EVENT_ERROR_SUCCESS;
			SendPacket.m_ui8MyClanTeam = ui8TeamCount;
			 
			SendPacket.m_aTeamInfo.SetValue( aTeamInfo, 0, ui8TeamCount );

			pSession->S2MOSendEvent(&SendPacket);

		}
		break;

	case PROTOCOL_CLAN_WAR_CREATE_TEAM_ACK:
		{			 
			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(T_UID));

			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);
			if( !pSession ) break;
			
			PACKET_CLAN_WAR_CREATE_TEAM_ACK SendPacket;
			T_RESULT TResult;
			MATCH_TEAM_INFO TeamInfo;

			pPacket->ReadData(&TResult, sizeof(T_RESULT)); 
			SendPacket.m_TResult = TResult; 
			if( EV_SUCCESSED( TResult ) )
			{
				pPacket->ReadData(&TeamInfo, sizeof(MATCH_TEAM_INFO));
				SendPacket.m_TeamlInfo = TeamInfo; 
				pSession->SetMatchTeamId (TeamInfo.m_TTeamID );
			}

			pSession->S2MOSendEvent(&SendPacket);
		}
		break;
	case PROTOCOL_CLAN_WAR_JOIN_TEAM_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID));

			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if( !pSession )
				break;

			PACKET_CLAN_WAR_JOIN_TEAM_ACK SendPacket;
			T_RESULT TResult;
			

			pPacket->ReadData(&TResult,		sizeof(T_RESULT));
			SendPacket.m_TResult = TResult;

			if( EV_FAILED( TResult ) )
			{
				// 매치팀 입장하는 유저에게만 전송.
				pSession->S2MOSendEvent(&SendPacket);
				break;
			}

			MATCH_MEMBER_INFO		aMemberInfo[MAX_CLAN_MATCH_MEMBER];

			// Read Pacekt.
			MATCH_TEAM_DETAIL_INFO	stTeamDetail;
			UINT8	ui8OperationSize; 
			TTCHAR	strOperation[NET_CLAN_OPERATION_SIZE]; // 작전명
			INT8	i8JoinSlot;

			pPacket->ReadData(&i8JoinSlot,						sizeof(INT8));	
			pPacket->ReadData(&stTeamDetail,					sizeof(MATCH_TEAM_INFO));	
			pPacket->ReadData(&stTeamDetail.m_i8LeaderSlot,		sizeof(INT8));
			pPacket->ReadData(&ui8OperationSize,				sizeof(UINT8));
			pPacket->ReadData(strOperation,						sizeof(TTCHAR)*ui8OperationSize);
			pPacket->ReadData( aMemberInfo,	sizeof(MATCH_MEMBER_INFO) * stTeamDetail.m_ui8CurPer);
			strOperation[ui8OperationSize] = '\0'; 
			
			// Make Packet
			i3String::Copy( stTeamDetail.m_strOperation, strOperation, NET_CLAN_OPERATION_SIZE );
			stTeamDetail.m_strOperation[ui8OperationSize] = '\0';
			
			SendPacket.m_i8JoinSlot		= i8JoinSlot;
			SendPacket.m_TeamDetailInfo = stTeamDetail;
			SendPacket.m_aMemberInfo.SetValue( aMemberInfo, 0, stTeamDetail.m_ui8CurPer );

			// Packet Send. 매치팀 유저 모두에게 전송.
			for( INT32 i = 0 ;  i < stTeamDetail.m_ui8CurPer ; ++i )
			{
				CUserSession* pMemberSession = g_pUserManager->GetSession(aMemberInfo[i].m_TUID); 
				if( !pMemberSession ) continue;

				// 본인 입장인지 알려줍니다.
				i8JoinSlot == aMemberInfo[i].m_i8Slot ? SendPacket.m_bIsMyEnter = true : SendPacket.m_bIsMyEnter = false;
 
				pMemberSession->S2MOSendEvent(&SendPacket);
			} 

			pSession->SetMatchTeamId(stTeamDetail.m_TTeamID);
		}
		break;
	case PROTOCOL_CLAN_WAR_LEAVE_TEAM_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID));

			PACKET_CLAN_WAR_LEAVE_TEAM_ACK SendPacket;  
			T_RESULT TResult;
			INT8 i8LeaveSlot;
			INT8 i8LeaderSlot;

			pPacket->ReadData(&TResult,			sizeof(T_RESULT));
			pPacket->ReadData(&i8LeaveSlot,		sizeof(INT8)); 
			pPacket->ReadData(&i8LeaderSlot,	sizeof(INT8));

			SendPacket.m_TResult		= TResult;
			SendPacket.m_i8LeaveSlot	= i8LeaveSlot;
			SendPacket.m_i8LeaderSlot	= i8LeaderSlot;
			SendPacket.m_bIsMyLeave		= false;

			INT32 i32CurMember = 0;
			pPacket->ReadData(&i32CurMember,			sizeof(INT32));

			 
			// 나머지 팀원들에게 전송.( 나간 유저 제외 )
			g_pUserManager->OnSendClanMatchTeamAllUser( i32CurMember, pPacket, &SendPacket ); 
				 
			// 퇴장요청한 멤버에게 전송. 
			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if( pSession )
			{
				SendPacket.m_bIsMyLeave = true;
				SendPacket.m_TResult = EVENT_ERROR_SUCCESS;

				pSession->SetMatchTeamId (MATCH_TEAM_INVALID_IDX);  
				pSession->S2MOSendEvent(&SendPacket);
			}
			
		}
		break;

	case PROTOCOL_CLAN_WAR_CHANGE_OPERATION_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID));

			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if( !pSession )
				break;

			PACKET_CLAN_WAR_CHANGE_OPERATION_ACK SendPacket;
			T_RESULT TResult;
			pPacket->ReadData(&TResult,		sizeof(T_RESULT));
			SendPacket.m_TResult = TResult;

			if( EV_FAILED( TResult) )
			{
				pSession->S2MOSendEvent(&SendPacket);
				break;
			}

			UINT8 ui8Length = 0;
			TTCHAR strOperation[NET_CLAN_OPERATION_SIZE];

			pPacket->ReadData(&ui8Length,				sizeof(UINT8));
			pPacket->ReadData(strOperation,				sizeof(TTCHAR)*ui8Length);
			strOperation[ui8Length] = '\0';

			SendPacket.m_strOperation = strOperation;

			INT32 i32CurMember = 0;
			pPacket->ReadData(&i32CurMember,			sizeof(INT32));

			// Packet Send. 매치팀 유저 모두에게 전송.
			g_pUserManager->OnSendClanMatchTeamAllUser( i32CurMember, pPacket, &SendPacket );
		}
		break;

	case PROTOCOL_CLAN_WAR_CHANGE_MAX_PER_ACK :
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID));

			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if( !pSession )
				break;

			PACKET_CLAN_WAR_CHANGE_MAX_PER_ACK  SendPacket;
			T_RESULT TResult;
			UINT8 ui8MaxPer;
			pPacket->ReadData(&TResult,			sizeof(T_RESULT));
			pPacket->ReadData(&ui8MaxPer,		sizeof(UINT8));

			SendPacket.m_TResult = TResult;
			SendPacket.m_ui8MaxMemberCount = ui8MaxPer;

			INT32 i32CurMember = 0;
			pPacket->ReadData(&i32CurMember,	sizeof(INT32));

			// Packet Send. 매치팀 유저 모두에게 전송.
			g_pUserManager->OnSendClanMatchTeamAllUser( i32CurMember, pPacket, &SendPacket );
		}
		break;

	case PROTOCOL_CLAN_WAR_MATCHMAKING_ACK:
		{
			PACKET_CLAN_WAR_MATCHMAKING_ACK  SendPacket;
			T_RESULT TResult; 
			pPacket->ReadData(&TResult,				sizeof(T_RESULT));
			SendPacket.m_TResult = TResult;

			INT32 i32CurMember = 0;
			pPacket->ReadData(&i32CurMember,		sizeof(INT32));

			// Packet Send. 매치팀 유저 모두에게 전송.
			g_pUserManager->OnSendClanMatchTeamAllUser( i32CurMember, pPacket, &SendPacket );
		}
		break;

	case PROTOCOL_CLAN_WAR_CANCEL_MATCHMAKING_ACK:
	{
		PACKET_CLAN_WAR_CANCEL_MATCHMAKING_ACK  SendPacket;
		T_RESULT TResult; 
		pPacket->ReadData(&TResult,				sizeof(T_RESULT));
		SendPacket.m_TResult = TResult;

		INT32 i32CurMember = 0;
		pPacket->ReadData(&i32CurMember,		sizeof(INT32));

		// Packet Send. 매치팀 유저 모두에게 전송.
		g_pUserManager->OnSendClanMatchTeamAllUser( i32CurMember, pPacket, &SendPacket );
	}
	break;

	case PROTOCOL_CLAN_WAR_CREATE_ROOM_REQ:
		{
			UINT8 ui8MaxPlayer = 0;
			pPacket->ReadData(&i64UID,			sizeof(T_UID));
			pPacket->ReadData(&ui8MaxPlayer,	sizeof(UINT8));
			 
			ROOM_INFO_CREATE stCreateInfo;
			pPacket->ReadData(&stCreateInfo.m_TMatchingID,		sizeof(T_MatchingID));

			for( INT32 i = 0 ; i < TEAM_COUNT; ++i )
			{
				pPacket->ReadData(&stCreateInfo.m_ui32ClanIdx[i],		sizeof(UINT32));
				pPacket->ReadData(&stCreateInfo.m_TMatchTeamID[i],		sizeof(T_TeamID));
			} 


			CUserSession* pSession = g_pUserManager->GetSession(i64UID);
			if( !pSession)
			{
				break;
			}

			if( pSession->GetPosition() < GAME_TASK_LOBBY || pSession->m_i32ChannelNum == SERVER_VALUE_UNKNOWN || SERVER_VALUE_UNKNOWN < pSession->m_i32RoomIdx )
			{
				g_pModuleClan->SendCreateClanMatchRoomAck(MODULE_CLAN_THREAD_IDX, pSession, stCreateInfo.m_TMatchingID, EVENT_ERROR_GAME_WRONG_POSITION);
				break;
			}

			stCreateInfo.RoomInfoAdd._SubType			= BATTLE_ROUND_TYPE_5 | BATTLE_TIME_MISSION_TYPE_3;
			stCreateInfo.RoomInfoBasic._PersonPlayerMax	= ui8MaxPlayer * TEAM_COUNT;
			stCreateInfo.RoomInfoBasic._StageID			= StageID::MakeStageID ( STAGE_MODE_BOMB, WEAPON_ALL, AI_USE_NO, true, true, 0, STAGE_UID_DOWNTOWN );				
			stCreateInfo.RoomInfoBasic._InfoFlag		= ROOM_INFO_FLAG_INTER_ENTER | ROOM_INFO_FLAG_OBSERVER | ROOM_INFO_FLAG_ATK_DEF_AUTO_CHANGE;
			stCreateInfo.RoomInfoBasic._WeaponFlag		= 0x0F; 
			stCreateInfo.RoomInfoBasic._IsClanMatch		= true;
			i3String::Copy( stCreateInfo.RoomInfoBasic._Title, L"Clan Battle", NET_ROOM_NAME_SIZE );

			INT32 i32MatchRoomIdx = pSession->CreateRoom(MODULE_CLAN_THREAD_IDX, &stCreateInfo);
			g_pModuleClan->SendCreateClanMatchRoomAck(MODULE_CLAN_THREAD_IDX, pSession, stCreateInfo.m_TMatchingID, i32MatchRoomIdx);
		}
		break;

	case PROTOCOL_CLAN_WAR_JOIN_ROOM_REQ:
		{
			T_MatchingID TMatchingID;
			INT32 i32RoomIndex;
			INT32 i32ChannelIdx;
			T_UID THostUID;
			CLAN_MATCHING_TEAM_INFO	aClanInfo[TEAM_COUNT];

			// Packet Read
			pPacket->ReadData(&TMatchingID,		sizeof(T_MatchingID));
			pPacket->ReadData(&i32ChannelIdx,	sizeof(INT32)); 
			pPacket->ReadData(&i32RoomIndex,	sizeof(INT32));
			pPacket->ReadData(&THostUID,		sizeof(T_UID));
			pPacket->ReadData(aClanInfo,		sizeof(CLAN_MATCHING_TEAM_INFO)*TEAM_COUNT);

			PACKET_CLAN_WAR_MATCH_TEAM_INFO_ACK	TeamInfoPacket;
			TeamInfoPacket.m_aClanInfo.SetValue( aClanInfo, 0, TEAM_COUNT );

			ROOM_INFO_JOIN	InfoJoin;
			InfoJoin.bMatch		= TRUE;
			InfoJoin.cForce		= 1;
	
			MATCHMAKING_RECORD		aRecordPacket[SLOT_MAX_COUNT];
			MATCHMAKING_RECORD_SVR	aRecord[SLOT_MAX_COUNT];
			INT32 i32MemberIdx = 0;	// 정상적으로 방에 입장한 멤버 인덱스.
			
			for (INT32 i = 0; i < TEAM_COUNT; ++i)
			{
				UINT8 ui8CurPer = 0;
				pPacket->ReadData(&ui8CurPer, sizeof(UINT8));

				ui8CurPer = MIN(ui8CurPer, MAX_CLAN_MATCH_MEMBER);

				for (INT32 k = 0; k < ui8CurPer; ++k)
				{
					i32MemberIdx = MIN(i32MemberIdx, (SLOT_MAX_COUNT) - 1);

					MATCHMAKING_RECORD_SVR*pRecord = &aRecord[i32MemberIdx];

					pRecord->m_i8RoomSlotIdx = -1;
					pPacket->ReadData(&pRecord->m_TUID,				sizeof(T_UID));
					pPacket->ReadData(&pRecord->m_i32SessionIdx,	sizeof(INT32));
					pPacket->ReadData(&pRecord->m_ui32Killcount,	sizeof(UINT32));
					pPacket->ReadData(&pRecord->m_ui32Death,		sizeof(UINT32));
					pPacket->ReadData(&pRecord->m_ui32Win,			sizeof(UINT32));
					pPacket->ReadData(&pRecord->m_ui32Lose,			sizeof(UINT32));
					pPacket->ReadData(&pRecord->m_ui32Assistcount,  sizeof(UINT32));

					CUserSession* pSession = g_pUserManager->GetSession(pRecord->m_i32SessionIdx, pRecord->m_TUID);
					if (!pSession)
					{	// 로그아웃에 의해 매치 팀에서 퇴장처리 됩니다.
						//g_pModuleClan->SendJoinClanMatchRoomAck(MODULE_CLAN_THREAD_IDX, TMatchingID, EVENT_ERROR_NOT_FIND_USER);
						continue;
					}

					if (pRecord->m_TUID == THostUID)
					{
						pRecord->m_i8RoomSlotIdx = 0; 
						i3mem::Copy( &aRecordPacket[i32MemberIdx], pRecord, sizeof(MATCHMAKING_RECORD));

						++i32MemberIdx;
						continue; // 방장(레드팀리더)은 CreateRoom에 의해 입장된 상태.
					}

					InfoJoin.wMatchTeam = pSession->GetMatchTeamId();

					i32Result = pSession->JoinClanMatchRoom(i32ChannelIdx, i32RoomIndex, &InfoJoin);
					if ( EV_FAILED(i32Result))
					{	// 방입장 실패시 매치팀에서 퇴장시킵니다.
						g_pModuleClan->SendJoinClanMatchRoomAck(MODULE_CLAN_THREAD_IDX, pSession, i32Result, TMatchingID );
						continue;
					}

					pRecord->m_i8RoomSlotIdx = (INT8)i32Result;
					i3mem::Copy(&aRecordPacket[i32MemberIdx], pRecord, sizeof(MATCHMAKING_RECORD));

					pSession->SetForceLeaveMatchRoom(false);

					++i32MemberIdx;
				}
			}

			TeamInfoPacket.m_aMatchMakingRecord.SetValue(aRecordPacket, 0, i32MemberIdx);

			i32MemberIdx = MIN(i32MemberIdx, SLOT_MAX_COUNT);

			// 팀 정보 패킷 전송.
			for (INT32 h = 0; h < i32MemberIdx; ++h)
			{
				MATCHMAKING_RECORD_SVR*pRecord = &aRecord[h];

				// test
				char strTemp[MAX_PATH];
				i3String::Format(strTemp, MAX_PATH, "@@@@ Idx : %d / UID : %I64d / SlotIdx : %d / Kill : %d/ D : %d / Win : %d / Lose : %d \n",
					h, pRecord->m_TUID, pRecord->m_i8RoomSlotIdx, pRecord->m_ui32Killcount, pRecord->m_ui32Death, pRecord->m_ui32Win, pRecord->m_ui32Lose);
				I3TRACE(strTemp);

				CUserSession* pSession = g_pUserManager->GetSession(pRecord->m_i32SessionIdx, pRecord->m_TUID);
				if (pSession)
				{	// 로그아웃에 의해 매치 팀에서 퇴장처리 됩니다.
					pSession->S2MOSendEvent(&TeamInfoPacket);
				}	 
			}
		}
		break;

	case PROTOCOL_CLAN_WAR_LEAVE_ROOM_ACK:
		{
			T_UID TLeaderUID; 
			INT32 i32CurPer = 0;
			INT32 i32ChannelIdx;
			pPacket->ReadData(&i32ChannelIdx,				sizeof(INT32));
			pPacket->ReadData(&TLeaderUID,					sizeof(T_UID));
			pPacket->ReadData(&i32CurPer,					sizeof(INT32));
 
			g_pUserManager->OnSendClanMatchRoomLeave( TLeaderUID, i32CurPer, pPacket, i32ChannelIdx );

		}
		break;

	case PROTOCOL_CLAN_WAR_MERCENARY_LIST_ACK:
		{
			PACKET_CLAN_WAR_MERCENARY_LIST_ACK SendPacket;

			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(T_UID));

			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);
			if( !pSession ) break;

			SendPacket.m_TResult = EVENT_ERROR_SUCCESS;
			
			UINT16	ui16TotalCount = 0;
			UINT16	ui16CurCount = 0;
			UINT16	ui16StartIdx = 0;
			MERCENARY_INFO aMerInfo[MAX_MERCENARY_LIST_PER_PACKET];

			// Read
			pPacket->ReadData(&ui16TotalCount,	sizeof(UINT16));
			pPacket->ReadData(&ui16CurCount,	sizeof(UINT16));
			pPacket->ReadData(&ui16StartIdx,	sizeof(UINT16)); 
			pPacket->ReadData(aMerInfo,			sizeof(MERCENARY_INFO)* ui16CurCount);

			// Write 
			SendPacket.m_ui16TotalCount = ui16TotalCount;
			SendPacket.m_ui16CurCount	= ui16CurCount;
			SendPacket.m_ui16StartIdx	= ui16StartIdx;
			SendPacket.m_aMerInfo.SetValue( aMerInfo, 0, ui16CurCount ); 

			pSession->S2MOSendEvent(&SendPacket);
		}
		break;

	case PROTOCOL_CLAN_WAR_REGIST_MERCENARY_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID));
			
			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if( !pSession ) break;

			PACKET_CLAN_WAR_REGIST_MERCENARY_ACK  SendPacket;
			T_RESULT			TResult; 
			MERCENARY_INFO		stInfo;

			pPacket->ReadData(&TResult,				sizeof(T_RESULT));
			SendPacket.m_TResult	= TResult;

			if( EV_SUCCESSED( TResult ) )
			{
				pPacket->ReadData(&stInfo,				sizeof(MERCENARY_INFO)); 
	 
				pSession->SetMercenaryId( stInfo.m_TMercenaryID );
				pSession->SetMerClass(stInfo.m_ui8Class);
				pSession->SetMerRank(stInfo.m_ui8MerRank);
			}
			
			if (pSession->m_UserInfoBasic.m_ui32Clanidx <= 0 ||
				pSession->m_UserInfoBasic.m_ui32ClanState <= CLAN_MEMBER_LEVEL_UNKNOWN) // 클랜원이 아닌경우 채널입장 처리
			{
				PACKET_SELECT_CHANNEL_ACK ChannelPacket;
				ChannelPacket.m_TResult = TResult;
				if( EV_SUCCESSED( ChannelPacket.m_TResult ) )
				{
					ChannelPacket.m_TResult = g_pUserManager->OnEnterChannel( pSession, pSession->m_i32SelectChannelNum );
				}
				if( EV_SUCCESSED( ChannelPacket.m_TResult ) )
				{
					ChannelPacket.m_ui16ChannelNum		= (UINT16)(pSession->m_i32SelectChannelNum);
					ChannelPacket.m_strNormalNotice		= g_pContextMain->m_strNormalNotice;

					pSession->m_i32ChannelNum				= pSession->m_i32SelectChannelNum; 
					pSession->m_i32LogChannelNum			= ((g_pContextMain->m_i32ServerIdx*GAME_CHANNEL_COUNT) + (pSession->m_i32ChannelNum+1));				
				}
				pSession->S2MOSendEvent( &ChannelPacket );
			}

			pSession->S2MOSendEvent(&SendPacket); 
		}
		break;

	case PROTOCOL_CLAN_WAR_REMOVE_MERCENARY_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID));
			 
			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if( !pSession ) break;

			PACKET_CLAN_WAR_REMOVE_MERCENARY_ACK  SendPacket;
			T_RESULT			TResult;

			pPacket->ReadData(&TResult,				sizeof(T_RESULT));
			SendPacket.m_TResult	= TResult;

			if( EV_SUCCESSED( TResult ) )
			{
				pSession->SetMercenaryId( MERCENARY_INVALID_IDX );
				pSession->SetMerClass(INIT_UINT8);
				pSession->SetMerRank(INIT_UINT8);
			}

			pSession->S2MOSendEvent(&SendPacket); 
		}
		break;

	case PROTOCOL_CLAN_WAR_INVITE_MERCENARY_ACK:
		{ 
			pPacket->ReadData(&i64UID,		sizeof(T_UID));		// 초대한 팀의 리더
			
			CUserSession* pLeaderSession = g_pUserManager->GetSession(i64UID);
			if( !pLeaderSession ) break;
			
			T_RESULT	TResult;
			T_UID		TMerUID;
			T_MerID		TMerID;
			T_TeamID	TTeamID;
			TTCHAR		strClanName[NET_CLAN_NAME_SIZE];

			pPacket->ReadData(&TResult,		sizeof(T_RESULT));
			pPacket->ReadData(&TMerUID,		sizeof(T_UID)); 
			pPacket->ReadData(&TMerID,		sizeof(T_MerID));
			pPacket->ReadData(&TTeamID,		sizeof(T_TeamID));
			pPacket->ReadData(strClanName,	sizeof(TTCHAR) * NET_CLAN_NAME_SIZE);

			// 초대한 팀의 리더에게 보낼 패킷
			PACKET_CLAN_WAR_INVITE_MERCENARY_SENDER_ACK	SenderPacket;
			SenderPacket.m_TResult = TResult;

			if (EV_FAILED(TResult))
			{
				pLeaderSession->S2MOSendEvent(&SenderPacket);	// 초대한 팀의 리더에게 보냄.
				break;
			}
			
			CUserSession* pMerSession = g_pUserManager->GetSession(TMerUID);
			if (!pMerSession)
			{
				SenderPacket.m_TResult = EVENT_ERROR_NOT_FIND_USER;
				pLeaderSession->S2MOSendEvent(&SenderPacket);	// 초대한 팀의 리더에게 보냄.
				break;
			}

			
			// 용병 페널티 타임 체크.
			if (pMerSession->IsMerPenalty())
			{
				SenderPacket.m_TResult = EVENT_ERROR_CLAN_WAR_MERCENAY_DIS_TIME;
				pLeaderSession->S2MOSendEvent(&SenderPacket);	// 초대한 팀의 리더에게 보냄.

				// 해당 용병의 초대팀 정보를 리셋시켜줍니다.
				g_pModuleClan->SendClanWarReleaseInviteTeam(MODULE_CLAN_THREAD_IDX, pMerSession);
				break;
			}
			
			pLeaderSession->S2MOSendEvent(&SenderPacket);	// 초대한 팀의 리더에게 보냄.

			// 초대당한 용병에게 보냄.
			PACKET_CLAN_WAR_INVITE_MERCENARY_RECEIVER_ACK	ReciverPacket;
			ReciverPacket.m_TResult = EVENT_ERROR_SUCCESS;
			ReciverPacket.m_strClanName = strClanName; 
			pMerSession->S2MOSendEvent(&ReciverPacket); 
		}
		break;

	case PROTOCOL_CLAN_WAR_INVITE_ACCEPT_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID));
			
			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if( !pSession ) break;

			PACKET_CLAN_WAR_INVITE_ACCEPT_ACK  SendPacket;
			T_RESULT			TResult;

			pPacket->ReadData(&TResult,				sizeof(T_RESULT));
			SendPacket.m_TResult	= TResult;
 
			pSession->S2MOSendEvent(&SendPacket); 
		}
		break;

	case PROTOCOL_CLAN_WAR_INVITE_DENIAL_ACK:
		{
			PACKET_CLAN_WAR_INVITE_DENIAL_ACK  SendPacket;
			T_RESULT			TResult; 
			T_UID				TLeaderUID;	// 초대한 팀 리더
			T_MerID				TMerID;		// 거절한 용병

			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID));
			pPacket->ReadData(&TResult,				sizeof(T_RESULT));
			pPacket->ReadData(&TLeaderUID,			sizeof(T_UID));
			pPacket->ReadData(&TMerID,				sizeof(T_MerID));

			SendPacket.m_TResult	= TResult;
			SendPacket.m_TMerID		= TMerID;

			CUserSession* pMerSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if( pMerSession ) 
			{	// 용병에게 보냄
				pMerSession->S2MOSendEvent(&SendPacket); 
			}

			CUserSession* pLeaderSession = g_pUserManager->GetSession( TLeaderUID); 
			if( pLeaderSession ) 
			{	// 리더에게 보냄
				if( EV_SUCCESSED( TResult) )
				{
					pLeaderSession->S2MOSendEvent(&SendPacket); 
				}
			} 
		}
		break;

	case PROTOCOL_CLAN_WAR_TEAM_CHATTING_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID));

			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if( !pSession ) break;
			
			T_RESULT TResult;

			pPacket->ReadData(&TResult,		sizeof(T_RESULT));
			 
			if( EV_FAILED( TResult) )	break;

			UINT8			ui8ChatSize;
			TTCHAR			strChat[NET_CLAN_CHATING_SIZE];
			INT32			i32CurMember = 0;

			pPacket->ReadData(&ui8ChatSize,				sizeof(UINT8));
			pPacket->ReadData(strChat,					sizeof(TTCHAR)*ui8ChatSize);
			pPacket->ReadData(&i32CurMember,			sizeof(INT32));
			strChat[ui8ChatSize] = '\0';

			INT32 i32State		= g_ai32ChatLimitTime[CHAT_LIMIT_STATE_NORMAL];
			UINT8 ui8NickSize	= (UINT8)i3String::Length( pSession->GetNickName() );
			bool bIsGMUser		= pSession->CheckAuthLevel();
			UINT8 ui8NickColor	= pSession->GetNickColor();

			i3NetworkPacket SendPacket( PROTOCOL_CLAN_WAR_TEAM_CHATTING_ACK );
			SendPacket.WriteData(&i32State,			sizeof(INT32));
			SendPacket.WriteData(&ui8NickSize,		sizeof(UINT8));
			SendPacket.WriteData(pSession->GetNickName(),			sizeof(TTCHAR) * ui8NickSize);
			SendPacket.WriteData(&bIsGMUser,		sizeof(bool));	
			SendPacket.WriteData(&ui8NickColor,		sizeof(UINT8));	
			SendPacket.WriteData(&ui8ChatSize,		sizeof(UINT8));	
			SendPacket.WriteData(strChat,		sizeof(TTCHAR) * ui8ChatSize);
			pSession->SendPacketMessage(&SendPacket);	// 채팅 보낸 유저에게 보냄.

			// Packet Send. 매치팀 유저 모두에게 전송.
			for( INT32 i = 0 ;  i < i32CurMember ; ++i )
			{
				T_UID TMemberUID = 0;
				pPacket->ReadData(&TMemberUID,			sizeof(T_UID));

				if( TMemberUID == i64UID ) continue;

				CUserSession* pMemberSession = g_pUserManager->GetSession(TMemberUID); 
				if( !pMemberSession ) continue;

				pMemberSession->SendPacketMessage(&SendPacket);	// 팀원들에게 보냄. 
			}
		}
		break;

	case PROTOCOL_CLAN_WAR_CHANGE_MEMBER_INFO_ACK :
		{
			MATCH_MEMBER_INFO	stMemberInfo;
			UINT8				ui8Type;

			pPacket->ReadData(&ui8Type,				sizeof(UINT8));
			pPacket->ReadData(&stMemberInfo,		sizeof(MATCH_MEMBER_INFO));

			CUserSession* pMerSession = g_pUserManager->GetSession(stMemberInfo.m_TUID); 
			if( pMerSession ) 
			{
				// 용병 랭크 변경해 줍니다.
				pMerSession->SetMerRank( stMemberInfo.m_ui8Rank );
			}
			 
			PACKET_CLAN_WAR_CHANGE_MEMBER_INFO_ACK  SendPacket;
			SendPacket.m_ui8Type		= ui8Type;
			SendPacket.m_stMemberInfo	= stMemberInfo;

			INT32 i32CurMember = 0;
			pPacket->ReadData(&i32CurMember,	sizeof(INT32));

			// Packet Send. 매치팀 유저 모두에게 전송.
			g_pUserManager->OnSendClanMatchTeamAllUser( i32CurMember, pPacket, &SendPacket );
		}
		break;

	case PROTOCOL_CLAN_WAR_MERCENARY_DETAIL_INFO_ACK :
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID));

			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if( !pSession ) break;

			T_RESULT	TResult;
			T_MerID		TMerID;
			T_UID		TMerUID;
			INT32		i32MerSessionIdx;
			MERCENARY_DETAIL_INFO	DetailInfo;
			S2_USER_DETAIL_INFO		UserDetailInfo;
			
			pPacket->ReadData(&TResult,			sizeof(T_RESULT));
			pPacket->ReadData(&TMerID,			sizeof(T_MerID));
			pPacket->ReadData(&TMerUID,			sizeof(T_UID));
			pPacket->ReadData(&i32MerSessionIdx,sizeof(INT32));
			pPacket->ReadData(&DetailInfo,		sizeof(MERCENARY_DETAIL_INFO));
			
			PACKET_CLAN_WAR_MERCENARY_DETAIL_INFO_ACK  SendPacket;
			SendPacket.m_TResult	= TResult;
			SendPacket.m_TMerID		= TMerID;
			SendPacket.m_DetailInfo = DetailInfo;

			CUserSession* pMercenarySession = g_pUserManager->GetSession(i32MerSessionIdx,TMerUID);
			if( pMercenarySession )
			{
				pSession->_SetUserDetailInfo( &UserDetailInfo, pMercenarySession);
			}

			SendPacket.m_UserDetailInfo = UserDetailInfo;
			
			pSession->S2MOSendEvent(&SendPacket);  
		}
		break;
	
	case PROTOCOL_CLAN_WAR_LEAVE_TEAM_MERCENARY_ACK:
		{ 
			PACKET_CLAN_WAR_LEAVE_TEAM_ACK SendPacket;  
		 
			INT8 i8LeaderSlot; 
			UINT8 ui8MerCount = 0;
			TEAM_MEMBER_IDX	aMerIdx[CLAN_TEAM_MEMBER_COUNT];

			pPacket->ReadData(&i8LeaderSlot,	sizeof(INT8));
			pPacket->ReadData(&ui8MerCount,		sizeof(UINT8));
			pPacket->ReadData(aMerIdx,			sizeof(TEAM_MEMBER_IDX)*ui8MerCount);

			SendPacket.m_TResult		= EVENT_ERROR_SUCCESS;
			SendPacket.m_i8LeaderSlot	= i8LeaderSlot;
			
			for( UINT8 i = 0 ; i < ui8MerCount ; ++i )
			{
				CUserSession* pSession = g_pUserManager->GetSession(aMerIdx[i].m_i32SessionIdx, aMerIdx[i].m_TUID); 
				if( !pSession ) continue;
			
				SendPacket.m_bIsMyLeave		= true;
				pSession->SetMatchTeamId (MATCH_TEAM_INVALID_IDX);

				// 퇴장요청한 유저에게 전송.
				SendPacket.m_i8LeaveSlot = aMerIdx[i].m_i8Slot;
				 
				pSession->S2MOSendEvent(&SendPacket);			
			}
		}
		break;
 
	case PROTOCOL_CHEAT_CLAN_WAR_MATCHING_POINT_ACK :
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID));

			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if( !pSession ) break;

			INT32 i32MatchingPoint = 0;
			
			pPacket->ReadData(&i32MatchingPoint,	sizeof(INT32));
			
			i3NetworkPacket SendPacket( PROTOCOL_CHEAT_CLAN_WAR_MATCHING_POINT_ACK );
			SendPacket.WriteData( &i32MatchingPoint, sizeof(INT32));
			pSession->SendPacketMessage(&SendPacket);	
		}
		break;

	case PROTOCOL_CHEAT_CLAN_WAR_RESULT_ACK :
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID));

			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if( !pSession ) break;

			TEAM_TYPE	eWinTeam;
			double	ResultClanPoint[TEAM_COUNT] = {0.f,};	
			
			pPacket->ReadData(&eWinTeam,		sizeof(TEAM_TYPE));
			pPacket->ReadData(ResultClanPoint,	sizeof(double)*TEAM_COUNT);
			
			i3NetworkPacket SendPacket( PROTOCOL_CHEAT_CLAN_WAR_RESULT_ACK );
			SendPacket.WriteData( &eWinTeam,		sizeof(TEAM_TYPE));
			SendPacket.WriteData( ResultClanPoint,	sizeof(double)*TEAM_COUNT);
			pSession->SendPacketMessage(&SendPacket);	
		}
		break;

	case PROTOCOL_CLAN_WAR_MATCH_MAKING_RESULT_ACK:
		{
			UINT8		ui8UserCount;
			INT32		ai32Session[ MAX_MEMBER_COUNT ];
			T_UID		TRecvUID[ MAX_MEMBER_COUNT ];
			CLAN_MATCH_MAKING_RESULT	MatchMakingResult;

			PACKET_CLAN_WAR_MATCH_MAKING_RESULT_ACK SendPacket;
		 
			pPacket->ReadData( &ui8UserCount,		sizeof(UINT8) );
			ui8UserCount = MIN(ui8UserCount, MAX_MEMBER_COUNT);
			
			pPacket->ReadData( ai32Session,				sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( TRecvUID,				sizeof(T_UID) * ui8UserCount); 
			pPacket->ReadData( &MatchMakingResult,		sizeof(CLAN_MATCH_MAKING_RESULT));

			SendPacket.m_MatchMakingResult = MatchMakingResult;
			
			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[i], TRecvUID[i] ); 
				if(pSession)
				{
					pSession->S2MOSendEvent(&SendPacket);		
				}
				
			}
		}
		break;

	case PROTOCOL_CLAN_WAR_RESULT_ACK:
	{
		T_RESULT		TResult;
		UINT8			ui8ClanWarCount = 0;
		CLAN_WAR_RESULT	ClawWarResult[MAX_MATCH_RESULT_COUNT];

		PACKET_CLAN_WAR_RESULT_ACK SendPacket;

		pPacket->ReadData(&i32SessionIdx, sizeof(INT32));
		pPacket->ReadData(&i64UID, sizeof(T_UID));
		pPacket->ReadData(&TResult, sizeof(T_RESULT));

		CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);
		if (!pSession) break;

		SendPacket.m_TResult = TResult;

		if (EV_SUCCESSED(TResult))
		{
			pPacket->ReadData(&ui8ClanWarCount, sizeof(UINT8));
			pPacket->ReadData(ClawWarResult, sizeof(CLAN_WAR_RESULT) * ui8ClanWarCount);

			SendPacket.m_aClanWarResult.SetValue(ClawWarResult, 0, ui8ClanWarCount);
		}

		pSession->S2MOSendEvent(&SendPacket);
	}
	break;

	case PROTOCOL_CLAN_WAR_REPLACE_CLAN_NAME_ACK:
	{
		T_TeamID	TTeamID;			// 변경되는 매치팀 ID
		INT32		i32UserCount;
		TTCHAR		strClanName[NET_CLAN_NAME_SIZE];

		PACKET_CLAN_WAR_REPLACE_CLAN_NAME_ACK SendPacket;

		pPacket->ReadData(&TTeamID,		sizeof(T_TeamID));
		pPacket->ReadData(strClanName,	sizeof(TTCHAR) * NET_CLAN_NAME_SIZE);

		SendPacket.m_TTeamID = TTeamID;
		SendPacket.m_strClanName = strClanName;

		for (INT32 k = 0; k < TEAM_COUNT; ++k)
		{
			pPacket->ReadData(&i32UserCount, sizeof(INT32)); 
			i32UserCount = MIN(i32UserCount, CLAN_TEAM_MEMBER_COUNT); 
			g_pUserManager->OnSendClanMatchTeamAllUser(i32UserCount, pPacket, &SendPacket);
		}
	}
	break;

	case PROTOCOL_CLAN_WAR_REPLACE_CLAN_MARK_ACK:
	{
		T_TeamID	TTeamID;			// 변경되는 매치팀 ID
		INT32		i32UserCount;
		UINT32		ui32Mark;

		PACKET_CLAN_WAR_REPLACE_CLAN_MARK_ACK SendPacket;

		pPacket->ReadData(&TTeamID,		sizeof(T_TeamID));
		pPacket->ReadData(&ui32Mark,	sizeof(UINT32) );

		SendPacket.m_TTeamID = TTeamID;
		SendPacket.m_ui32Mark = ui32Mark;

		for (INT32 k = 0; k < TEAM_COUNT; ++k)
		{
			pPacket->ReadData(&i32UserCount, sizeof(INT32));
			i32UserCount = MIN(i32UserCount, CLAN_TEAM_MEMBER_COUNT);
			g_pUserManager->OnSendClanMatchTeamAllUser(i32UserCount, pPacket, &SendPacket);
		}
	}
	break;

	case PROTOCOL_CLAN_WAR_REPLACE_CLAN_COLOR_NAME_ACK:
	{
		T_TeamID	TTeamID;			// 변경되는 매치팀 ID
		INT32		i32UserCount;
		UINT8		ui8Color;

		PACKET_CLAN_WAR_REPLACE_CLAN_COLOR_NAME_ACK SendPacket;

		pPacket->ReadData(&TTeamID,		sizeof(T_TeamID));
		pPacket->ReadData(&ui8Color,	sizeof(UINT8) );

		SendPacket.m_TTeamID	= TTeamID;
		SendPacket.m_ui8Color	= ui8Color;

		for (INT32 k = 0; k < TEAM_COUNT; ++k)
		{
			pPacket->ReadData(&i32UserCount, sizeof(INT32));
			i32UserCount = MIN(i32UserCount, CLAN_TEAM_MEMBER_COUNT);
			g_pUserManager->OnSendClanMatchTeamAllUser(i32UserCount, pPacket, &SendPacket);
		}
	}
	break;

	case PROTOCOL_CLAN_WAR_MERCENARY_PENALTY_LOAD_ACK:
	{
		DATE32 dtMerPenaltyTime;

		pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
		pPacket->ReadData(&i64UID,				sizeof(T_UID));
		pPacket->ReadData(&dtMerPenaltyTime,	sizeof(DATE32));

		CUserSession * pSession = g_pUserManager->GetSession( i32SessionIdx, i64UID ); 
		if(pSession)
		{
			pSession->SetMerPenaltyTime(&dtMerPenaltyTime);
		}

	}
	break;

	default:
	{
		g_pLog->WriteLog(L"[CModuleClanSocket::PacketParsing]Unknown Message, Protocol ID:%d", ProtocolID);
		}
		break; 
	}

	m_ui32LastSendTime	= i3ThreadTimer::GetServerTime();
	m_ui32LastRecvTime	= i3ThreadTimer::GetServerTime();

	return; 
}
