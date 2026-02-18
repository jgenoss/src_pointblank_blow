#include "pch.h"
#include "ModuleClanSocket.h"
#include "UserManager.h"
#include "UserSession.h"
#include "ModuleClan.h"
#include "ModuleMessenger.h"
#include "ModuleContextNc.h"
#include "ModuleTrans.h"

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

BOOL CModuleClanSocket::OnCreate(char * pAddress, UINT16 ui16PortNo)
{
	BOOL bRv = i3NetworkClientSocket::OnCreate(pAddress, ui16PortNo);
	m_ui32LastSendTime	= g_pContextMain->GetServerTime();
	m_ui32LastRecvTime	= g_pContextMain->GetServerTime();

	// 서버에 클랜 서버 갯수를 보내준다.
	if( bRv )
	{
		INT8 i8ChannelCount = 0;
		INT8 ai8Channel[ 255 ];
		
		for( UINT8 i = 0 ; i < GAME_CHANNEL_COUNT ; i++ )
		{
			if( g_pContextMain->m_pui8ChannelType[i] == CHANNEL_TYPE_CLAN	||
				g_pContextMain->m_pui8ChannelType[i] == CHANNEL_TYPE_PCCAFE_CLAN )
			{
				ai8Channel[ i8ChannelCount ] = i;
				i8ChannelCount++;
			}
		}
		
		i3NetworkPacket SendPacket( PROTOCOL_CS_MATCH_CHANNEL_REQ );
		SendPacket.WriteData( &i8ChannelCount,	sizeof(INT8) );
		SendPacket.WriteData( ai8Channel,		sizeof(INT8) * i8ChannelCount );
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

	INT32	i32SessionIdx; 
	TUID	i64UID;
	INT32	i32Result; 

	switch(ProtocolID)
	{
	case PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS	:		//연결되었음
		{
			m_bConnect = TRUE;
			g_pModuleLogFile->Write_M_Log(MODULE_CLAN_THREAD_IDX,NULL, "CLAN SERVER CONNECT MESSAGE RECV");

			CLAN_CONTEXT* pClanContext		= g_pContextMain->GetClanContext();
			CLAN_CONTEXT* pClanContextBack	= g_pContextMain->GetClanContextBack();

			pPacket->ReadData( pClanContext,							sizeof(CLAN_CONTEXT) );
			i3mem::Copy( pClanContextBack, pClanContext,				sizeof( CLAN_CONTEXT ) );

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
			UINT16	ui16SectionIdx;
			UINT8	ui8Count; 

			pPacket->ReadData( &ui16SectionIdx,	sizeof(UINT16) );
			pPacket->ReadData( &ui8Count,		sizeof(UINT8) );

			CLAN_CONTEXT* pClanContext = g_pContextMain->GetClanContextBack();
			CLAN_BASIC_INFO* pClanList = g_pContextMain->GetClanListBack();

			if( ui16SectionIdx < pClanContext->_sectionCount )
			{
				if( ui8Count <= pClanContext->_sectionSize )
				{
					pPacket->ReadData( &pClanList[ ui16SectionIdx * pClanContext->_sectionSize ], sizeof(CLAN_BASIC_INFO) * ui8Count );

					if( ui16SectionIdx+1 == pClanContext->_sectionCount )
					{
						// 다 받았으면 Context를 변경해준다.
						g_pContextMain->SetChangeClanActive();
					}
				}
				else 
				{
					g_pModuleLogFile->Write_M_Log(MODULE_CLAN_THREAD_IDX,NULL, "CLAN_LIST_STREAM Error count");
				}
			}
			else
			{
				g_pModuleLogFile->Write_M_Log(MODULE_CLAN_THREAD_IDX,NULL, "CLAN_LIST_STREAM Error section"); 
			}
		}
		break;
	case PROTOCOL_CS_USER_LOGIN_ACK			:
		{
			UINT32	ui32CID, ui32Mark;
			UINT8	ui8State, ui8ExpRank, ui8PerRank, ui8NameSize;
			char	strClanName[NET_CLAN_NAME_SIZE];

			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID));
			pPacket->ReadData(&i32Result,			sizeof(INT32));
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
					pPacket->ReadData(strClanName,		ui8NameSize); 
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
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				UINT8					ui8TotalCount;
				CLANMEMBER_INFO_STRUCT	aClanMember[ MAX_MEMBER_COUNT ];

				pPacket->ReadData(&ui8TotalCount, sizeof(UINT8));

				if( ui8TotalCount <= MAX_MEMBER_COUNT )
				{
					for( UINT8 i = 0 ; i < ui8TotalCount ; i++ )
					{
						pPacket->ReadData(&aClanMember[i]._ui8NickSize,		sizeof(UINT8));
						if( aClanMember[i]._ui8NickSize > NET_NICK_NAME_SIZE ) return;

						pPacket->ReadData(aClanMember[i]._strNickName,		aClanMember[i]._ui8NickSize );
						pPacket->ReadData(&aClanMember[i]._BasicInfo,	sizeof(CLANMEMBER_INFO) );

						aClanMember[i]._strNickName[ NET_NICK_NAME_SIZE - 1 ] = '\0';
					}

					pSession->SendClanMemberInfo( ui8TotalCount, aClanMember );
				}
				else
				{
					g_pModuleLogFile->Write_M_Log(MODULE_CLAN_THREAD_IDX,NULL, "PROTOCOL_CS_MEMBER_INFO_ACK Error count");
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
			TUID	ai64UIDSendUser[ MAX_MEMBER_COUNT ];
			TUID	i64UID;
			UINT64	ui64State;

			pPacket->ReadData(&ui8UserCount,		sizeof(UINT8));
			if( ui8UserCount > MAX_MEMBER_COUNT )		break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64UIDSendUser,		sizeof(TUID) * ui8UserCount); 
			pPacket->ReadData(&i64UID,				sizeof(TUID));
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
			TUID	ai64UIDSendUser[ MAX_MEMBER_COUNT ];
			CLANMEMBER_INFO_STRUCT	ClanMember;

			pPacket->ReadData(&ui8UserCount,				sizeof(UINT8));
			if( ui8UserCount > MAX_MEMBER_COUNT )				break;
			pPacket->ReadData( ai32Session,					sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64UIDSendUser,				sizeof(TUID) * ui8UserCount); 
			pPacket->ReadData(&ClanMember._ui8NickSize,		sizeof(UINT8));
			if( ClanMember._ui8NickSize > NET_NICK_NAME_SIZE ) break;
			pPacket->ReadData(ClanMember._strNickName,			ClanMember._ui8NickSize);
			pPacket->ReadData(&ClanMember._BasicInfo,		sizeof(CLANMEMBER_INFO));

			ClanMember._strNickName[ NET_NICK_NAME_SIZE - 1 ] = '\0';

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
			TUID	ai64UID[ MAX_MEMBER_COUNT ];
			TUID	i64MemberUID;

			pPacket->ReadData( &ui8UserCount,				sizeof(UINT8));					LENGTH_CHECK( ui8UserCount, MAX_MEMBER_COUNT );
			pPacket->ReadData( ai32SSession,				sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64UID,						sizeof(TUID) * ui8UserCount); 
			pPacket->ReadData( &i64MemberUID,				sizeof(TUID));

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
			pPacket->ReadData(&i64UID,			sizeof(TUID)); 
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
			pPacket->ReadData(&i64UID,			sizeof(TUID)); 
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
	case PROTOCOL_CS_CLIENT_CHECK_DUPLICATE_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,		sizeof(INT32));

				i3NetworkPacket SendPacket( PROTOCOL_CS_CLIENT_CHECK_DUPLICATE_ACK );
				SendPacket.WriteData(&i32Result, sizeof(INT32));
				pSession->SendPacketMessage( &SendPacket );
			}
		}
		break;
	case PROTOCOL_CS_CHECK_DUPLICATE_AZIT_URL_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,		sizeof(INT32));

				i3NetworkPacket SendPacket( PROTOCOL_CS_CHECK_DUPLICATE_AZIT_URL_ACK );
				SendPacket.WriteData(&i32Result, sizeof(INT32));
				pSession->SendPacketMessage( &SendPacket );
			}
		}
		break;
	case PROTOCOL_CS_CANCEL_REQUEST_ACK		: 
		{
			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(TUID));																																																																																																
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
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			pPacket->ReadData(&i32Result,			sizeof(INT32));
			if( EV_SUCCESSED( i32Result ) )
			{
				CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
				if(pSession != NULL)
				{
					TUID	i64RecvUID;
					pPacket->ReadData(&i64RecvUID,	sizeof(TUID));

					// 가입 권유 쪽지
					char   szMessage[ NET_NOTE_SIZE ];
					szMessage[ 0 ] = sizeof(UINT8) + 1;
					szMessage[ 1 ] = NOTE_MESSAGE_CLAN_INVITE;
					szMessage[ 2 ] = '\0';

					g_pModuleMessenger->MessengerSendClanNote( MODULE_CLAN_THREAD_IDX, pSession, i64RecvUID, pSession->m_UserInfoBasic._clanidx, 1, 0, szMessage, pSession->m_UserInfoBasic._clanName );
				}
			}
		}
		break;
	case PROTOCOL_CS_DETAIL_INFO_ACK		: 
		{
			CLAN_DETAIL_INFO ClanDetailInfo; 
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID));
			pPacket->ReadData(&i32Result,			sizeof(INT32));
			if( EV_SUCCESSED( i32Result ) )
			{
				pPacket->ReadData(&ClanDetailInfo,	sizeof(CLAN_DETAIL_INFO));
			}

			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);
			if(pSession != NULL)
			{
				pSession->SendClanDetailInfo(i32Result, &ClanDetailInfo); 
			}
		}
		break; 
	case PROTOCOL_CS_MEMBER_CONTEXT_ACK		:	//Member Context 받기 
		{
			CLAN_MEMBER_CONTEXT ClanMemberContext; 
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID));
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,			sizeof(INT32)); 
				pPacket->ReadData(&ClanMemberContext,	sizeof(CLAN_MEMBER_CONTEXT));
				pSession->SendClanMemberContext(i32Result, &ClanMemberContext); 
			}
		}
		break; 
	case PROTOCOL_CS_MEMBER_LIST_ACK		:	//Member List 받기 
		{
			UINT8 ui8section	= 0; 
			UINT8 ui8count		= 0; 

			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID));
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,			sizeof(INT32)); 
				pPacket->ReadData(&ui8section,			sizeof(UINT8));
				pPacket->ReadData(&ui8count,			sizeof(UINT8));
				
				if( ui8count < 200 )
				{
					pPacket->ReadData( m_aTempMemberBasicInfo, (sizeof(CLAN_MEMBER_BASIC_INFO) * ui8count)); 
					pSession->SendClanMemberList(i32Result, ui8section, ui8count, m_aTempMemberBasicInfo); 
				}
			}
		}
		break; 
	case PROTOCOL_CS_MEMBER_DETAIL_INFO_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID));
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
		}
		break;
	case PROTOCOL_CS_REQUEST_CONTEXT_ACK	: 
		{
			CLAN_REQUEST_CONTEXT ClanRequestContext; 
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,			sizeof(INT32)); 
				pPacket->ReadData(&ClanRequestContext,	sizeof(CLAN_REQUEST_CONTEXT)); 
				pSession->SendClanRequestContext(i32Result, &ClanRequestContext);
			}
		}
		break; 
	case PROTOCOL_CS_REQUEST_LIST_ACK		: 
		{
			UINT8	ui8section = 0; 
			UINT8	ui8count = 0; 
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{//
				pPacket->ReadData(&i32Result,			sizeof(INT32)); 
				pPacket->ReadData(&ui8section,			sizeof(UINT8));
				pPacket->ReadData(&ui8count,			sizeof(UINT8));
				if( ui8count < 200 )
				{
					pPacket->ReadData( m_aTempRequestBasicInfo, (sizeof(CLAN_REQUEST_BASIC_INFO) * ui8count)); 
					pSession->SendClanRequestList(i32Result, ui8section, ui8count, m_aTempRequestBasicInfo); 
				}                
			}
		}
		break;
	case PROTOCOL_CS_REQUEST_INFO_ACK		: 
		{
			CLAN_REQUEST_DETAIL_INFO ClanRequestDetailInfo; 
			pPacket->ReadData(&i32SessionIdx,			sizeof(INT32));
			pPacket->ReadData(&i64UID,					sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,				sizeof(INT32)); 
				pPacket->ReadData(&ClanRequestDetailInfo,	sizeof(CLAN_REQUEST_DETAIL_INFO)); 
				pSession->SendClanRequestDetailInfo(i32Result, &ClanRequestDetailInfo);
			}
		}
		break; 	
	case PROTOCOL_CS_ACCEPT_REQUEST_ACK		: 
		{
			INT32	i32Count;
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Count,		sizeof(INT32));

				pSession->SendClanAcceptRequest( i32Count );

				if( i32Count > MAX_MEMBER_COUNT ) 
				{
					g_pModuleLogFile->Write_M_Log(MODULE_CLAN_THREAD_IDX,NULL, "PROTOCOL_CS_ACCEPT_REQUEST_ACK Error" );
					break;
				}

				if( i32Count > 0 )
				{
					TUID i64ClanMemberUID[ MAX_MEMBER_COUNT ];
					pPacket->ReadData( i64ClanMemberUID,	sizeof(TUID) * i32Count);
					// 가입 쪽지
					char   szMessage[ NET_NOTE_SIZE ];
					szMessage[ 0 ] = (char)(sizeof(UINT8) + 1); // NULL
					szMessage[ 1 ] = (char)(NOTE_MESSAGE_CLAN_INVITE_ACCEPT);
					szMessage[ 2 ] = '\0';
					g_pModuleMessenger->MessengerSendClanGroupNote( MODULE_CLAN_THREAD_IDX, pSession, i32Count, i64ClanMemberUID, pSession->m_UserInfoBasic._clanidx, 0, 0, szMessage, pSession->m_UserInfoBasic._clanName );

					for( INT32 i = 0 ; i < i32Count ; i++ )
					{
						g_pModuleContextNC->LogSendMessage( MODULE_CLAN_THREAD_IDX, LOG_MESSAGE_U_CLAN_JOIN, pSession, &i64ClanMemberUID[i] );
					}
				}
			}
		}
		break; 
	case PROTOCOL_CS_DENIAL_REQUEST_ACK		: 
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,	sizeof(INT32));

				pSession->SendClanDenialRequest(i32Result);

				if( i32Result > MAX_MEMBER_COUNT ) 
				{
					g_pModuleLogFile->Write_M_Log(MODULE_CLAN_THREAD_IDX,NULL, "PROTOCOL_CS_DENIAL_REQUEST_ACK Error" );
					break;
				}

				if( i32Result > 0 )
				{
					TUID i64ClanMemberUID[ MAX_MEMBER_COUNT ];					

					pPacket->ReadData( i64ClanMemberUID,	sizeof(TUID) * i32Result);					

					// 거절 쪽지
					char   szMessage[ NET_NOTE_SIZE ];
					szMessage[ 0 ] = (char)(sizeof(UINT8) + 1); // NULL
					szMessage[ 1 ] = (char)(NOTE_MESSAGE_CLAN_INVITE_DENIAL);
					szMessage[ 2 ] = '\0';

					g_pModuleMessenger->MessengerSendClanGroupNote( MODULE_CLAN_THREAD_IDX, pSession, i32Result, i64ClanMemberUID, pSession->m_UserInfoBasic._clanidx, 0, 0, szMessage, pSession->m_UserInfoBasic._clanName );
				}
			}
		}
		break;
	case PROTOCOL_CS_INVITE_ACCEPT_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				UINT32				ui32ClanIdx;
				TUID				wMasterDBIdx;
				CLAN_DETAIL_INFO	DetailInfo;

				pPacket->ReadData(&i32Result,		sizeof(INT32));
				pPacket->ReadData(&ui32ClanIdx,		sizeof(UINT32));
				pPacket->ReadData(&wMasterDBIdx,	sizeof(TUID));

				// 쪽지               
				if( EV_SUCCESSED( i32Result ) )
				{	// 클랜 가입 수락 쪽지
					pPacket->ReadData(&DetailInfo,		sizeof(	CLAN_DETAIL_INFO));
					
					char   szMessage[ NET_NOTE_SIZE ];
					szMessage[ 0 ] = (char)(sizeof(UINT8) + i3String::Length( pSession->m_UserInfoBasic._nick ) + 1);
					szMessage[ 1 ] = (char)(NOTE_MESSAGE_CLAN_JOIN_ACCEPT);
					i3String::Copy( &szMessage[ 2 ], pSession->m_UserInfoBasic._nick, NET_NOTE_SIZE-2 );
					
					g_pModuleMessenger->MessengerSendClanNote( MODULE_CLAN_THREAD_IDX, pSession, wMasterDBIdx, ui32ClanIdx, 0, 0, szMessage, DetailInfo._name );

					pSession->SendClanAcceptResultRequest( &DetailInfo );
				}
				else if( EV_IS_ERROR( i32Result, EVENT_ERROR_CLAN_INVITED_REFUSAL ) )
				{	// 클랜 가입 거부 쪽지
					UINT8	ui8NameSize;
					char	strClanName[ NET_CLAN_NAME_SIZE ];

					pPacket->ReadData( &ui8NameSize,	sizeof(	UINT8));	LENGTH_CHECK( ui8NameSize, NET_CLAN_NAME_SIZE );
					pPacket->ReadData( strClanName,		ui8NameSize );
					strClanName[ NET_CLAN_NAME_SIZE-1 ] = '\0';

					char   szMessage[ NET_NOTE_SIZE ];
					szMessage[ 0 ] = (char)(sizeof(UINT8) + i3String::Length( pSession->m_UserInfoBasic._nick ) + 1);
					szMessage[ 1 ] = (char)(NOTE_MESSAGE_CLAN_JOIN_DENIAL);
					i3String::Copy( &szMessage[ 2 ], pSession->m_UserInfoBasic._nick, NET_NOTE_SIZE-2 );

					g_pModuleMessenger->MessengerSendClanNote( MODULE_CLAN_THREAD_IDX, pSession, wMasterDBIdx, ui32ClanIdx, 0, 0, szMessage, strClanName );
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
			TUID	ai64RecvUID[ MAX_MEMBER_COUNT ];
			UINT8	ui8NickSize;
			UINT8	ui8ChatSize;
			char	strNick[ NET_NICK_NAME_SIZE ];
			char	strChating[ NET_CLAN_CHATING_SIZE ];

			pPacket->ReadData( &ui8UserCount,		sizeof(UINT8) );
			if( ui8UserCount > MAX_MEMBER_COUNT ) break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(TUID) * ui8UserCount);
			pPacket->ReadData( &ui8NickSize,		sizeof(UINT8) );
			if( ui8NickSize > NET_NICK_NAME_SIZE )	break;
			pPacket->ReadData( strNick,				ui8NickSize );
			pPacket->ReadData( &ui8ChatSize,		sizeof(UINT8) );
			pPacket->ReadData( strChating,			ui8ChatSize );

			strNick[ NET_NICK_NAME_SIZE -1 ] = '\0';
			strChating[ NET_CLAN_CHATING_SIZE - 1] = '\0';

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[i], ai64RecvUID[i] ); 
				if(pSession != NULL)
				{
					pSession->SendClanChatingRequest( ui8NickSize, strNick, ui8ChatSize, strChating);
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
			TUID	ai64RecvUID[ MAX_MEMBER_COUNT ];
			UINT8	ui8NickSize;
			UINT8	ui8ChatSize;
			char	strNick[ NET_NICK_NAME_SIZE ];
			char	strChating[ NET_CLAN_CHATING_SIZE ];

			pPacket->ReadData( &ui8UserCount,		sizeof(UINT8) );
			if( ui8UserCount > MAX_MEMBER_COUNT ) break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(TUID) * ui8UserCount);
			pPacket->ReadData( &ui8NickSize,		sizeof(UINT8) );
			if( ui8NickSize > NET_NICK_NAME_SIZE )	break;
			pPacket->ReadData( strNick,				ui8NickSize );
			pPacket->ReadData( &ui8ChatSize,		sizeof(UINT8) );
			pPacket->ReadData( strChating,			ui8ChatSize );

			strNick[ NET_NICK_NAME_SIZE -1 ] = '\0';
			strChating[ NET_CLAN_CHATING_SIZE - 1] = '\0';

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[i], ai64RecvUID[i] ); 
				if(pSession != NULL)
				{
					pSession->SendClanPageChatingRequest( ui8NickSize, strNick, ui8ChatSize, strChating);
				}
			}
		}
		break;
	case PROTOCOL_CS_SECESSION_CLAN_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID));
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData( &i32Result, sizeof(INT32) );

				if( EV_SUCCESSED( i32Result ) )
				{
					UINT32	wMaster;
					pPacket->ReadData( &wMaster, sizeof(UINT32) );

					// 탈퇴 쪽지
					char   szMessage[ NET_NOTE_SIZE ];
					szMessage[ 0 ] = (char)(sizeof(UINT8) + i3String::Length( pSession->m_UserInfoBasic._nick ) + 1);
					szMessage[ 1 ] = (char)NOTE_MESSAGE_CLAN_SECESSION;
					i3String::Copy( &szMessage[ 2 ], pSession->m_UserInfoBasic._nick, NET_NOTE_SIZE-2 );

					g_pModuleContextNC->LogSendMessage( MODULE_CLAN_THREAD_IDX, LOG_MESSAGE_U_CLAN_SECESSION, pSession ); 
					g_pModuleMessenger->MessengerSendClanNote( MODULE_CLAN_THREAD_IDX, pSession, wMaster, pSession->m_UserInfoBasic._clanidx, 0, 0, szMessage, pSession->m_UserInfoBasic._clanName );
				}

				pSession->SendClanSecessionRequest(i32Result);
			}
		}
		break;	
	case PROTOCOL_CS_DEPORTATION_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,			sizeof(INT32));

				if( i32Result > MAX_MEMBER_COUNT ) 
				{
					g_pModuleLogFile->Write_M_Log(MODULE_CLAN_THREAD_IDX,NULL, "PROTOCOL_CS_DEPORTATION_ACK Error" );
					break;
				}

				if( i32Result > 0 )
				{
					TUID i64ClanMemberUID[ MAX_MEMBER_COUNT ];
					pPacket->ReadData(i64ClanMemberUID,	sizeof(TUID) * i32Result);
				
					// 제명 쪽지
					char   szMessage[ NET_NOTE_SIZE ];
					szMessage[ 0 ] = (char)(sizeof(UINT8) + 1); // NULL
					szMessage[ 1 ] = (char)(NOTE_MESSAGE_CLAN_DEPORTATION);
					szMessage[ 2 ] = '\0';
					g_pModuleMessenger->MessengerSendClanGroupNote( MODULE_CLAN_THREAD_IDX, pSession, i32Result, i64ClanMemberUID, pSession->m_UserInfoBasic._clanidx, 0, 0, szMessage, pSession->m_UserInfoBasic._clanName );


					for( INT32 i = 0 ; i < i32Result ; i++ )
					{
						g_pModuleContextNC->LogSendMessage( MODULE_CLAN_THREAD_IDX, LOG_MESSAGE_U_CLAN_DEPORTATION, pSession, &i64ClanMemberUID[i] );
					}
				}

				pSession->SendClanDeportationRequest(i32Result);
			}
		}
		break;
	case PROTOCOL_CS_CLOSE_CLAN_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,			sizeof(INT32));
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
		}
		break;	
	case PROTOCOL_CS_COMMISSION_MASTER_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,			sizeof(INT32));			

				pSession->SendClanCommissionMasterRequest(i32Result);

				if( EV_SUCCESSED( i32Result ) )
				{
					UINT32 ui32MaserDBIDx;
					pPacket->ReadData(&ui32MaserDBIDx,		sizeof(UINT32));

					// 마스터 임명 쪽지
					char   szMessage[ NET_NOTE_SIZE ];
					szMessage[ 0 ] = (char)(sizeof(UINT8) + 1);
					szMessage[ 1 ] = (char)(NOTE_MESSAGE_CLAN_MASTER);
					szMessage[ 2 ] = '\0';

					g_pModuleMessenger->MessengerSendClanNote( MODULE_CLAN_THREAD_IDX, pSession, ui32MaserDBIDx, pSession->m_UserInfoBasic._clanidx, 0, 0, szMessage, pSession->m_UserInfoBasic._clanName );
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
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,			sizeof(INT32));				

				pSession->SendClanCommissionStaffRequest(i32Result);

				if( i32Result > MAX_MEMBER_COUNT ) 
				{
					g_pModuleLogFile->Write_M_Log(MODULE_CLAN_THREAD_IDX,NULL, "PROTOCOL_CS_COMMISSION_STAFF_ACK Error" );
					break;
				}

				if( i32Result > 0 )
				{
					TUID i64ClanMemberUID[ MAX_MEMBER_COUNT ];
					pPacket->ReadData( i64ClanMemberUID,		sizeof(TUID) * i32Result );

					// 스텝 임명 쪽지					
					char   szMessage[ NET_NOTE_SIZE ];
					szMessage[ 0 ] = (char)(sizeof(UINT8) + 1);
					szMessage[ 1 ] = (char)(NOTE_MESSAGE_CLAN_STAFF);
					szMessage[ 2 ] = '\0';

					g_pModuleMessenger->MessengerSendClanGroupNote( MODULE_CLAN_THREAD_IDX, pSession, i32Result, i64ClanMemberUID, pSession->m_UserInfoBasic._clanidx, 0, 0, szMessage, pSession->m_UserInfoBasic._clanName );
				}
			}
		}
		break;
	case PROTOCOL_CS_COMMISSION_REGULAR_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,			sizeof(INT32));

				pSession->SendClanCommissionRegularRequest(i32Result);

				if( i32Result > MAX_MEMBER_COUNT ) 
				{
					g_pModuleLogFile->Write_M_Log(MODULE_CLAN_THREAD_IDX,NULL, "PROTOCOL_CS_COMMISSION_REGULAR_ACK Error" );
					break;
				}

				if( i32Result > 0 )
				{
					TUID ai64ClanMemberUID[ MAX_MEMBER_COUNT ];
					pPacket->ReadData( ai64ClanMemberUID,		sizeof(TUID) * i32Result );

					// 일반 임명 쪽지
					char   szMessage[ NET_NOTE_SIZE ];
					szMessage[ 0 ] = (char)(sizeof(UINT8) + 1);
					szMessage[ 1 ] = (char)(NOTE_MESSAGE_CLAN_REGULAR);
					szMessage[ 2 ] = '\0';

					g_pModuleMessenger->MessengerSendClanGroupNote( MODULE_CLAN_THREAD_IDX, pSession, i32Result, ai64ClanMemberUID, pSession->m_UserInfoBasic._clanidx, 0, 0, szMessage, pSession->m_UserInfoBasic._clanName );
				}
			}
		}
		break;	
	case PROTOCOL_CS_REPLACE_NOTICE_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,			sizeof(INT32));

				pSession->SendClanReplaceNoticeRequest(i32Result);
			}
		}
		break;
	case PROTOCOL_CS_REPLACE_INTRO_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,			sizeof(INT32));

				pSession->SendClanReplaceIntroRequest(i32Result);
			}
		}
		break;
	case PROTOCOL_CS_NOTE_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				TUID	ai64ClanMemberUID[ MAX_MEMBER_COUNT ];
				UINT8	ui8MessageSize;
				char	strMessage[ NET_NOTE_SIZE ];
				pPacket->ReadData(&i32Result,			sizeof(INT32));				

				pSession->SendClanNoteRequest(i32Result);

				if( i32Result > MAX_MEMBER_COUNT ) 
				{
					g_pModuleLogFile->Write_M_Log(MODULE_CLAN_THREAD_IDX,NULL, "PROTOCOL_CS_NOTE_ACK Error" );
					break;
				}

				if( i32Result > 0 )
				{
					pPacket->ReadData(ai64ClanMemberUID,	sizeof(TUID) * i32Result);
					pPacket->ReadData(&ui8MessageSize,		sizeof(UINT8));
					pPacket->ReadData(strMessage,			ui8MessageSize);

					g_pModuleMessenger->MessengerSendClanGroupNote( MODULE_CLAN_THREAD_IDX, pSession, i32Result, ai64ClanMemberUID, pSession->m_UserInfoBasic._clanidx, 0, ui8MessageSize, strMessage, pSession->m_UserInfoBasic._clanName );
				}
			}
		}
		break;
	case PROTOCOL_CS_REPLACE_NAME_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,			sizeof(INT32));

				if ( EV_SUCCESSED( i32Result ) )
				{
					g_pModuleTrans->AuthSendAuthItemData( MODULE_CLAN_THREAD_IDX, pSession, pSession->m_i64ChangeClanMarkDBIdx, MAKE_ITEM_FLAG(ITEM_CLASS_EXPENDABLE, CASHITEM_USAGE_EXPENDABLES, CASHITEM_GROUP_CHANGE_CLAN_NAME, CASHITEM_DAY_NO), 0, NULL );
				}

				pSession->SendClanReplaceNameRequest(i32Result);
			}
		}
		break;
	case PROTOCOL_CS_CHECK_MARK_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,			sizeof(INT32));

				pSession->SendClanCheckMarkRequest(i32Result);
			}
		}
		break;
	case PROTOCOL_CS_CHANGE_CLAN_EXP_RANK_ACK:
		{
			UINT8	ui8UserCount;
			INT32	ai32Session[ MAX_MEMBER_COUNT ];
			TUID	ai64RecvUID[ MAX_MEMBER_COUNT ];
			UINT8	ui8Rank;

			pPacket->ReadData(&ui8UserCount,		sizeof(UINT8));
			if( ui8UserCount > MAX_MEMBER_COUNT )		break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(TUID) * ui8UserCount); 
			pPacket->ReadData(&ui8Rank,				sizeof(UINT8));

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[ i ], ai64RecvUID[ i ]); 
				if(pSession != NULL)
				{
					pSession->SendClanChangeExpRankRequest(ui8Rank);
				}
			}
		}
		break;
	case PROTOCOL_CS_CHANGE_CLAN_PER_RANK_ACK:
		{
			UINT8	ui8UserCount;
			INT32	ai32Session[ MAX_MEMBER_COUNT ];
			TUID	ai64RecvUID[ MAX_MEMBER_COUNT ];
			UINT8	ui8Rank;

			pPacket->ReadData(&ui8UserCount,		sizeof(UINT8));
			if( ui8UserCount > MAX_MEMBER_COUNT )		break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(TUID) * ui8UserCount); 
			pPacket->ReadData(&ui8Rank,				sizeof(UINT8));

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[ i ], ai64RecvUID[ i ]); 
				if(pSession != NULL)
				{
					pSession->SendClanChangePerRankRequest(ui8Rank);
				}
			}
		}
		break;
	case PROTOCOL_CS_REPLACE_MARK_RESULT_ACK:		
		{
			UINT8	ui8UserCount;
			INT32	ai32Session[ MAX_MEMBER_COUNT ];
			TUID	ai64RecvUID[ MAX_MEMBER_COUNT ];
			UINT32  ui32Mark;

			pPacket->ReadData(&ui8UserCount,		sizeof(UINT8));
			if( ui8UserCount > MAX_MEMBER_COUNT )		break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(TUID) * ui8UserCount); 
			pPacket->ReadData(&ui32Mark,			sizeof(UINT32));

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[ i ], ai64RecvUID[ i ]); 
				if(pSession != NULL)
				{
					pSession->SendClanReplaceMarkResltRequest(ui32Mark);
				}
			}
		}
		break;
	case PROTOCOL_CS_REPLACE_MANAGEMENT_RESULT_ACK:
		{
			UINT8			ui8UserCount;
			INT32			ai32Session[ MAX_MEMBER_COUNT ];
			TUID			i64RecvUID[ MAX_MEMBER_COUNT ];
			UINT8			ui8Authority;

			pPacket->ReadData(&ui8UserCount,		sizeof(UINT8));
			if( ui8UserCount > MAX_MEMBER_COUNT )		break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( i64RecvUID,			sizeof(TUID) * ui8UserCount); 
			pPacket->ReadData(&ui8Authority,		sizeof(UINT8));

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[ i ], i64RecvUID[ i ]); 
				if(pSession != NULL)
				{
					pSession->SendClanReplaceManagementResltRequest( ui8Authority );
				}
			}
		}
		break;
	case PROTOCOL_CS_REPLACE_NAME_RESULT_ACK:
		{
			UINT8	ui8UserCount;
			INT32	ai32Session[ MAX_MEMBER_COUNT ];
			TUID	ai64RecvUID[ MAX_MEMBER_COUNT ];
			char    strName[ NET_CLAN_NAME_SIZE ];

			pPacket->ReadData(&ui8UserCount,		sizeof(UINT8));
			if( ui8UserCount > MAX_MEMBER_COUNT )		break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(TUID) * ui8UserCount); 
			pPacket->ReadData(&strName,				NET_CLAN_NAME_SIZE);

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[ i ], ai64RecvUID[ i ]); 
				if(pSession != NULL)
				{
					pSession->SendClanReplaceNameResultRequest( strName );
				}
			}
		}
		break;
	case PROTOCOL_CS_REPLACE_MARK_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,			sizeof(INT32));

				if ( EV_SUCCESSED( i32Result ) )
				{
					g_pModuleTrans->AuthSendAuthItemData( MODULE_CLAN_THREAD_IDX, pSession, pSession->m_i64ChangeClanMarkDBIdx, MAKE_ITEM_FLAG(ITEM_CLASS_EXPENDABLE, CASHITEM_USAGE_EXPENDABLES, CASHITEM_GROUP_CHANGE_CLAN_MARK, CASHITEM_DAY_NO), 0, NULL );
				}

				pSession->SendClanReplaceMarkRequest(i32Result);
			}
		}
		break;
	case PROTOCOL_CS_REPLACE_PERSONMAX_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				UINT8 ui8ClanMaxCount = 0;
				pPacket->ReadData(&ui8ClanMaxCount,	sizeof(UINT8));
				pSession->SendClanReplacePersonMaxRequest(ui8ClanMaxCount);
			}
		}
		break;
	case PROTOCOL_CS_REPLACE_MANAGEMENT_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,			sizeof(INT32));

				pSession->SendClanReplaceManagementRequest(i32Result);
			}
		}
		break;
	case PROTOCOL_CS_CHECK_JOIN_AUTHORITY_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,			sizeof(INT32));

				pSession->SendClanCheckJoinAuthorityRequest(i32Result);
			}
		}
		break;
	case PROTOCOL_CS_REPLACE_MEMBER_NICK_ACK:
		{
			pPacket->ReadData(&i32SessionIdx, sizeof(INT32));
			pPacket->ReadData(&i64UID,		sizeof(TUID));
			
			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);

			if (pSession)
			{
				pPacket->ReadData(&i32Result, sizeof(INT32));

				// 이미 게임서버에서는 닉네임이 변경에 성공하였다.
				// Do nothing
			}
		}
		break;
	case PROTOCOL_CS_RECORD_RESET_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,			sizeof(INT32));

				pSession->SendClanRecordResetRequest(i32Result);
			}
		}
		break;
	case PROTOCOL_CS_ACCEPT_REQUEST_RESULT_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				CLAN_DETAIL_INFO	DetailInfo;

				pPacket->ReadData(&DetailInfo,		sizeof(CLAN_DETAIL_INFO));
				
				pSession->SendClanAcceptResultRequest( &DetailInfo);
			}
		}
		break;
	case PROTOCOL_CS_DENIAL_REQUEST_RESULT_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pSession->SendClanDenialResultRequest();
			}
		}
		break;
	case PROTOCOL_CS_DEPORTATION_RESULT_ACK:
		{
			//UINT8					cUserCount;
			//INT32					nSession[ MAX_MEMBER_COUNT ];
			//TUID					i64RecvUID[ MAX_MEMBER_COUNT ];

			//pPacket->ReadData( &cUserCount,		sizeof(UINT8) );
			//if( cUserCount > MAX_MEMBER_COUNT )	break;
			//pPacket->ReadData( nSession,			sizeof(INT32) * cUserCount);
			//pPacket->ReadData( i64RecvUID,		sizeof(TUID) * cUserCount);			

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
			pPacket->ReadData( &i64UID,				sizeof(TUID));

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
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pSession->SendClanMasterCommissionResultRequest();
			}
		}
		break;
	case PROTOCOL_CS_COMMISSION_STAFF_RESULT_ACK:
		{
			//UINT8					cUserCount;
			//INT32					nSession;
			//TUID					i64RecvUID[ MAX_MEMBER_COUNT ];

			//pPacket->ReadData( &cUserCount,			sizeof(UINT8) );
			//if( cUserCount > MAX_MEMBER_COUNT )	break;
			//pPacket->ReadData( iSessionIdx,			sizeof(INT32) * cUserCount);
			//pPacket->ReadData( i64RecvUID,			sizeof(TUID) * cUserCount);	

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
			pPacket->ReadData( &i64UID,				sizeof(TUID) );	

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
			//TUID					i64RecvUID[ MAX_MEMBER_COUNT ];

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
			pPacket->ReadData( &i64UID,				sizeof(TUID) );	

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

			pPacket->ReadData(&i32SessionIdx,			sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData(&i32Result,			sizeof(INT32));

				pSession->SendClanRoomInvitedRequest(i32Result);
			}
		}
		break;
	case PROTOCOL_CS_ROOM_INVITED_RESULT_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				TUID TSenderUID;
				pPacket->ReadData(&TSenderUID,		sizeof(TUID));

				pSession->SendClanRoomInvitedResultRequest(TSenderUID);
			}
		}
		break;
	case PROTOCOL_CS_REPLACE_MEMBER_BASIC_RESULT_ACK:
		{
			UINT8					ui8UserCount;
			INT32					ai32Session[ MAX_MEMBER_COUNT ];
			TUID					ai64RecvUID[ MAX_MEMBER_COUNT ];
			CLAN_MEMBER_BASIC_INFO	MemberBasicInfo;

			pPacket->ReadData( &ui8UserCount,		sizeof(UINT8) );
			if( ui8UserCount > MAX_MEMBER_COUNT )	break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(TUID) * ui8UserCount);
			pPacket->ReadData( &MemberBasicInfo,	sizeof(CLAN_MEMBER_BASIC_INFO));

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[i], ai64RecvUID[i] ); 
				if(pSession != NULL)
				{
					pSession->SendClanReplaceMemberBasicResultRequest( &MemberBasicInfo );
				}
			}
		}
		break;	
	case PROTOCOL_CS_RECORD_RESET_RESULT_ACK:
		{
			UINT8					ui8UserCount;
			INT32					ai32Session[ MAX_MEMBER_COUNT ];
			TUID					ai64RecvUID[ MAX_MEMBER_COUNT ];			

			pPacket->ReadData( &ui8UserCount,		sizeof(UINT8) );
			if( ui8UserCount > MAX_MEMBER_COUNT )	break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(TUID) * ui8UserCount);			

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[i], ai64RecvUID[i] ); 
				if(pSession != NULL)
				{
					pSession->SendClanRecordResetResultRequest( );
				}
			}
		}
		break;
	case PROTOCOL_CS_POINT_RESET_RESULT_ACK:
		{
			UINT8					ui8UserCount;
			INT32					ai32Session[ MAX_MEMBER_COUNT ];
			TUID					ai64RecvUID[ MAX_MEMBER_COUNT ];			

			pPacket->ReadData( &ui8UserCount,		sizeof(UINT8) );
			if( ui8UserCount > MAX_MEMBER_COUNT )	break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(TUID) * ui8UserCount);			

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[i], ai64RecvUID[i] ); 
				if(pSession != NULL)
				{
					pSession->SendClanPointResetResultRequest();
				}
			}
		}
		break;
	case PROTOCOL_CS_REPLACE_COLOR_NAME_RESULT_ACK:
		{
			UINT8					ui8UserCount;
			INT32					ai32Session[ MAX_MEMBER_COUNT ];
			TUID					ai64RecvUID[ MAX_MEMBER_COUNT ];
			UINT8					ui8Color;

			pPacket->ReadData( &ui8UserCount,		sizeof(UINT8) );
			if( ui8UserCount > MAX_MEMBER_COUNT )	break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(TUID) * ui8UserCount);
			pPacket->ReadData( &ui8Color,			sizeof(UINT8));

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[i], ai64RecvUID[i] ); 
				if(pSession != NULL)
				{
					pSession->SendClanReplacColorNameResultRequest( ui8Color );
				}
			}
		}
		break;
	case PROTOCOL_BASE_HEART_BIT_ACK		:
		break;
	case PROTOCOL_CS_MATCH_TEAM_CONTEXT_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				CLAN_BATTLE_TEAM_CONTEXT  TeamContext;

				pPacket->ReadData(&TeamContext,		sizeof(CLAN_BATTLE_TEAM_CONTEXT));

				pSession->SendClanMatchTeamContextRequest( &TeamContext );
			}
		}
		break;
	case PROTOCOL_CS_MATCH_ALL_TEAM_CONTEXT_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				CLAN_BATTLE_ALL_TEAM_CONTEXT AllTeamContext;

				pPacket->ReadData(&AllTeamContext,	sizeof(CLAN_BATTLE_ALL_TEAM_CONTEXT));

				pSession->SendClanMatchAllTeamContextRequest( &AllTeamContext );
			}
		}
		break;
	case PROTOCOL_CS_MATCH_TEAM_LIST_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				UINT8					ui8TotalCount;
				UINT8					ui8SectionCount;
				UINT8					ui8Section;
				UINT8					ui8Count;
				CLAN_BATTLE_TEAM_LIST	aBattleTeamList[ 64 ];
			
				pPacket->ReadData(&ui8TotalCount,		sizeof(UINT8));
				if( 0 < ui8TotalCount )
				{
					pPacket->ReadData(&ui8SectionCount,	sizeof(UINT8));
					pPacket->ReadData(&ui8Section,		sizeof(UINT8));
					pPacket->ReadData(&ui8Count,		sizeof(UINT8));
					if( ui8Count > 64 ) 
					{
						g_pModuleLogFile->Write_M_Log(MODULE_CLAN_THREAD_IDX,NULL, "PROTOCOL_CS_MATCH_TEAM_LIST_ACK Counrt Error");
						return;
					}
					if( 0 < ui8Count )
					{
						pPacket->ReadData(aBattleTeamList,	sizeof(CLAN_BATTLE_TEAM_LIST) * ui8Count );
					}
				}

				pSession->SendClanMatchTeamListRequest( ui8TotalCount, ui8SectionCount, ui8Section, ui8Count, aBattleTeamList );
			}
		}
		break;
	case PROTOCOL_CS_MATCH_ALL_TEAM_LIST_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				UINT16					ui16TotalCount;
				UINT16					ui16SectionCount;
				UINT16					ui16Section;
				UINT8					ui8Count;
				CLAN_BATTLE_TEAM_INFO	aBattleTeamInfo[ 64 ];

				pPacket->ReadData(&ui16TotalCount,		sizeof(UINT16));
				pPacket->ReadData(&ui16SectionCount,	sizeof(UINT16));
				pPacket->ReadData(&ui16Section,			sizeof(UINT16));
				pPacket->ReadData(&ui8Count,			sizeof(UINT8));
				if( ui8Count > 64 ) 
				{
					g_pModuleLogFile->Write_M_Log(MODULE_CLAN_THREAD_IDX,NULL, "PROTOCOL_CS_MATCH_ALL_TEAM_LIST_ACK Count Error");
					return;
				}

				pPacket->ReadData(aBattleTeamInfo,	sizeof(CLAN_BATTLE_TEAM_INFO) * ui8Count );

				pSession->SendClanMatchAllTeamListRequest( ui16TotalCount, ui16SectionCount, ui16Section, ui8Count, aBattleTeamInfo );
			}
		}
		break;
	case PROTOCOL_CS_MATCH_TEAM_DETAIL_INFO_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				CLAN_DETAIL_INFO	Detail;

				pPacket->ReadData(&i32Result,		sizeof(INT32));
				pPacket->ReadData(&Detail,			sizeof(CLAN_DETAIL_INFO));

				pSession->SendClanMatchTeamDetailRequest( i32Result, &Detail );
			}
		}
		break;
	case PROTOCOL_CS_MATCH_TEAM_CREATE_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				INT32				i32Success;
				UINT32				ui32RoomIdx;			//RoomIdx
				CLAN_BATTLE_TEAM	Team;

				pPacket->ReadData(&i32Success,		sizeof(INT32));
				pPacket->ReadData(&ui32RoomIdx,		sizeof(UINT32));
				pPacket->ReadData(&Team,			sizeof(CLAN_BATTLE_TEAM));

				pSession->SendClanMatchTeamCreateRequest( i32Success, ui32RoomIdx, &Team );
			}
		}
		break;
	case PROTOCOL_CS_MATCH_TEAM_JOIN_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				INT32					i32Success;
				CLAN_BATTLE_TEAM_INFO	TeamInfo;
				CLAN_BATTLE_MEMBER		aMemberInfo[ CLAN_TEAM_MEMBER_COUNT ];

				pPacket->ReadData(&i32Success,		sizeof(INT32));
				pPacket->ReadData(&TeamInfo,		sizeof( CLAN_BATTLE_TEAM_INFO ));				
				if( TeamInfo._Team._PerMax > CLAN_TEAM_MEMBER_COUNT )
				{
					g_pModuleLogFile->Write_M_Log(MODULE_CLAN_THREAD_IDX,NULL, "PROTOCOL_CS_MATCH_TEAM_JOIN_ACK Count Error");
					return;
				}
				pPacket->ReadData(aMemberInfo,		sizeof(CLAN_BATTLE_MEMBER) * TeamInfo._Team._PerMax);

				pSession->SendClanMatchTeamJoinRequest( i32Success, &TeamInfo, aMemberInfo );
			}
		}
		break;
	case PROTOCOL_CS_MATCH_TEAM_LEAVE_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				INT32	i32Success;

				pPacket->ReadData(&i32Success,		sizeof(INT32));

				pSession->SendClanMatchTeamLeaveRequest( i32Success );
			}
		}
		break;
	case PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				UINT8 ui8PerMax;
				
				pPacket->ReadData(&i32Result,		sizeof(INT32));
				pPacket->ReadData(&ui8PerMax,		sizeof(UINT8));

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
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				UINT8 ui8PerMax;
				pPacket->ReadData(&ui8PerMax,			sizeof(UINT8));

				pSession->SendClanMatchTeamChangePerResultRequest( ui8PerMax );
			}
		}
		break;
	case PROTOCOL_CS_MATCH_TEAM_INFO_ACK:
		{
			UINT8	ui8UserCount;
			INT32	ai32Session[ CLAN_TEAM_MEMBER_COUNT ];
			TUID	ai64RecvUID[ CLAN_TEAM_MEMBER_COUNT ];
			CLAN_BATTLE_TEAM		Team;
			CLAN_BATTLE_MEMBER		aMemberInfo[ CLAN_TEAM_MEMBER_COUNT ];

			pPacket->ReadData( &ui8UserCount,		sizeof(UINT8) );
			if( ui8UserCount > CLAN_TEAM_MEMBER_COUNT )	break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(TUID) * ui8UserCount);
			pPacket->ReadData(&Team,				sizeof(CLAN_BATTLE_TEAM));
			pPacket->ReadData(aMemberInfo,			sizeof(CLAN_BATTLE_MEMBER) * Team._PerMax);

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[i], ai64RecvUID[i] ); 
				if(pSession != NULL)
				{
					pSession->SendClanMatchTeamInfoRequest( &Team, aMemberInfo );
				}
			}
		}
		break;
	case PROTOCOL_CS_MATCH_FIGHT_REQUEST_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				INT32 i32Success;

				pPacket->ReadData(&i32Success,		sizeof(INT32));

				pSession->SendClanMatchFightRequestRequest( i32Success );
			}
		}
		break;
	case PROTOCOL_CS_MATCH_CHATING_ACK:
		{
			// 한번에 받아서 처리합니다. // 
			UINT8	ui8UserCount;
			INT32	ai32Session[ CLAN_TEAM_MEMBER_COUNT ];
			TUID	ai64RecvUID[ CLAN_TEAM_MEMBER_COUNT ];
			UINT8	ui8NickSize;
			UINT8	ui8ChatSize;
			char	strNick[ NET_NICK_NAME_SIZE ];
			char	strChating[ NET_CLAN_CHATING_SIZE ];

			pPacket->ReadData( &ui8UserCount,		sizeof(UINT8) );
			if( ui8UserCount > CLAN_TEAM_MEMBER_COUNT ) break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(TUID) * ui8UserCount);
			pPacket->ReadData( &ui8NickSize,		sizeof(UINT8) );
			if( ui8NickSize > NET_NICK_NAME_SIZE )	break;
			pPacket->ReadData( strNick,				ui8NickSize );
			pPacket->ReadData( &ui8ChatSize,		sizeof(UINT8) );
			pPacket->ReadData( strChating,			ui8ChatSize );

			strNick[ NET_NICK_NAME_SIZE -1 ] = '\0';
			strChating[ NET_CLAN_CHATING_SIZE - 1] = '\0';

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[i], ai64RecvUID[i] ); 
				if(pSession == NULL)	continue;

				// 기획의도.
				if( pSession->GetPosition() != GAME_TASK_LOBBY)	continue;
				
				pSession->SendClanMatchChatingRequest( ui8NickSize, strNick, ui8ChatSize, strChating);
				
			}
		}
		break;
	case PROTOCOL_CS_MATCH_FIGHT_REQUEST_RESULT_ACK:
		{
			// 한번에 받아서 처리합니다. // 
			UINT8					ui8UserCount;
			INT32					ai32Session[ CLAN_TEAM_MEMBER_COUNT ];
			TUID					ai64RecvUID[ CLAN_TEAM_MEMBER_COUNT ];
			CLAN_BATTLE_TEAM_INFO	TeamInfo;

			pPacket->ReadData( &ui8UserCount,		sizeof(UINT8) );
			if( ui8UserCount > CLAN_TEAM_MEMBER_COUNT )	break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(TUID) * ui8UserCount);
			pPacket->ReadData(&TeamInfo,			sizeof(CLAN_BATTLE_TEAM_INFO));

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[i], ai64RecvUID[i] ); 
				if(pSession != NULL)
				{
					pSession->SendClanMatchFightResultRequest( &TeamInfo );
				}
			}
		}
		break;
	case PROTOCOL_CS_MATCH_FIGHT_ACCECT_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				INT32 i32Success;

				pPacket->ReadData(&i32Success,		sizeof(INT32));

				pSession->SendClanMatchFightAccectRequest( i32Success );
			}
		}
		break;	
	case PROTOCOL_CS_MATCH_FIGHT_ACCECT_RESULT_ACK:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				INT32 i32Success;

				pPacket->ReadData(&i32Success,		sizeof(INT32));

				pSession->SendClanMatchFightAccectResultRequest( i32Success );
			}
		}
		break;
	case PROTOCOL_CS_MATCH_ROOM_CREATE_GS_REQ:
		{
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(TUID)); 
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{	
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
		}
		break;
	case PROTOCOL_CS_MATCH_ROOM_JOIN_GS_REQ:
		{
			// 한번에 받아서 처리합니다. //
			UINT8				ui8UserCount;
			INT32				ai32Session[ CLAN_TEAM_MEMBER_COUNT ];
			TUID				ai64RecvUID[ CLAN_TEAM_MEMBER_COUNT ];
			UINT32				ui32RoomIdx;
			UINT32				ui32TeamIdx;
			CLAN_BATTLE_TEAM	BattleTeam;
			CLAN_BATTLE_CLAN	BattleClan;

			pPacket->ReadData( &ui8UserCount,		sizeof(UINT8) );
			if( ui8UserCount > CLAN_TEAM_MEMBER_COUNT )	break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(TUID) * ui8UserCount);
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
			}
		}
		break;
	case PROTOCOL_CS_MATCH_ROOM_LEAVE_GS_REQ:
		{
			UINT8				ui8UserCount;
			INT32				ai32Session[ CLAN_TEAM_MEMBER_COUNT ];
			TUID				ai64RecvUID[ CLAN_TEAM_MEMBER_COUNT ];
			CLAN_BATTLE_CLAN	BattleClan;

			pPacket->ReadData( &ui8UserCount,		sizeof(UINT8) );
			if( ui8UserCount > CLAN_TEAM_MEMBER_COUNT )	break;
			pPacket->ReadData( ai32Session,			sizeof(INT32) * ui8UserCount);
			pPacket->ReadData( ai64RecvUID,			sizeof(TUID) * ui8UserCount);

			pPacket->ReadData(&BattleClan,		sizeof(CLAN_BATTLE_CLAN));

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				CUserSession * pSession = g_pUserManager->GetSession( ai32Session[i], ai64RecvUID[i] ); 
				if(pSession != NULL)
				{
					pSession->SendClanBattleChangeClanInfo( &BattleClan );
				}
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
	default:
		{
			char strMessage[MAX_PATH];
			i3String::Format( strMessage, MAX_PATH, "[CModuleClanSocket::PacketParsing]Unknown Message, Protocol ID:%d", ProtocolID );
			g_pModuleLogFile->Write_M_Log( MODULE_CLAN_THREAD_IDX, NULL, strMessage );
		}
		break; 
	}

	m_ui32LastSendTime	= g_pContextMain->GetServerTime();
	m_ui32LastRecvTime	= g_pContextMain->GetServerTime();

	return; 
}
