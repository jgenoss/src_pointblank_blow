#include "pch.h"
#include "ModuleMessengerSocket.h"
#include "UserManager.h"
#include "UserSession.h"
#include "ModuleContextNc.h"

I3_CLASS_INSTANCE( CModuleMessengerSocket, i3NetworkClientSocket );


CModuleMessengerSocket::CModuleMessengerSocket(void)
{
	m_bConnect		= FALSE; 
}

CModuleMessengerSocket::~CModuleMessengerSocket(void)
{
	OnDestroy();
}

BOOL CModuleMessengerSocket::OnCreate(char * pAddress, UINT16 ui16PortNo)
{
	BOOL bRv = i3NetworkClientSocket::OnCreate(pAddress, ui16PortNo);

	m_ui32LastSendTime		= g_pContextMain->GetServerTime();
	m_ui32LastRecvTime		= g_pContextMain->GetServerTime(); 	
	return bRv; 
}

void CModuleMessengerSocket::OnDestroy(void)
{
	DisConnect();
}

void CModuleMessengerSocket::DisConnect()
{
	m_bConnect = FALSE; 
	i3NetworkClientSocket::DisConnect();
}

void CModuleMessengerSocket::PacketParsing(i3NetworkPacket  * pPacket)
{
	INT32		i32Temp;
	PROTOCOL	ProtocolID = pPacket->GetProtocolID(); 

	switch(ProtocolID)
	{
	case PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS:		
		{
			//연결 되었음
			g_pModuleLogFile->Write_M_Log(MODULE_MESSENGER_THREAD_IDX, NULL, "[SUCC] MESSENGER SERVER CONNECT MESSAGE RECV"); 			
			m_bConnect = TRUE; 
		}
		break; 
	case PROTOCOL_BASE_HEART_BIT_ACK:	
		{
			break;
		}
	case PROTOCOL_MESSENGER_ENTER_USER_ACK:
		{
			INT32 i32sessionIdx;
			TUID i64UID;
			
			pPacket->ReadData(&i32sessionIdx, sizeof(INT32));
			pPacket->ReadData(&i64UID, sizeof(TUID));

			CUserSession* pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID);

			if (pSession != NULL)
			{
				pPacket->ReadData(&pSession->m_ui8FriendCount, sizeof(UINT32));
#if defined( USE_BLOCK_LIST  )
				pPacket->ReadData(&pSession->m_ui8BlockCount, sizeof(UINT32));
#endif
			}
		}
		break;
	case PROTOCOL_MESSENGER_NOTE_SEND_ACK:
		{
			INT32 sessionIdx;
			TUID i64UID;
			
			pPacket->ReadData(&sessionIdx, sizeof(INT32));
			pPacket->ReadData(&i64UID, sizeof(TUID));

			CUserSession* pSession = g_pUserManager->GetSession(sessionIdx, i64UID);

			if (pSession != NULL)
			{
				INT32 result;

				pPacket->ReadData(&result, sizeof(INT32));

				pSession->SendMessengerSendNote(result);
			}
		}
		break;
	case PROTOCOL_MESSENGER_NOTE_LIST_ACK:
		{
			INT32 i32sessionIdx;
			TUID i64UID;

			pPacket->ReadData(&i32sessionIdx, sizeof(INT32));
			pPacket->ReadData(&i64UID, sizeof(TUID));

			CUserSession* pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID);

			if (pSession != NULL)
			{
				UINT8 pageCount;
				UINT8 noteCount;

				pPacket->ReadData(&pageCount, sizeof(UINT8));
				pPacket->ReadData(&noteCount, sizeof(UINT8));
				
				noteCount = MIN(noteCount, MAX_NOTE_COUNT_PER_PAGE);

				if (0 < noteCount)
				{
					BOOL brokenPacket = FALSE;

					NOTE_HEADER noteHeaderList[MAX_NOTE_COUNT_PER_PAGE];
					NOTE_BODY noteBodyList[MAX_NOTE_COUNT_PER_PAGE];
					
					pPacket->ReadData(noteHeaderList, sizeof(NOTE_HEADER) * noteCount);					

					for(INT32 i = 0; i < noteCount; i++)
					{
						NOTE_BODY* body = &noteBodyList[i];

						pPacket->ReadData(&body->_senderSize, sizeof(UINT8));
						pPacket->ReadData(&body->_messageSize, sizeof(UINT8));

						body->_senderSize = MIN(body->_senderSize, NET_NICK_NAME_SIZE);
						body->_messageSize = MIN(body->_messageSize, NET_NOTE_SIZE);

						if (0 < body->_senderSize )
						{
							pPacket->ReadData(body->_sender, body->_senderSize);
							body->_sender[NET_NICK_NAME_SIZE-1] = '\0';
						}
						else
						{
							// 쪽지함 데이터가 깨졌다.
							brokenPacket = TRUE;
							g_pModuleLogFile->Write_M_Log(MODULE_MESSENGER_THREAD_IDX, NULL, "Broken NoteBox");
							break;
						}
						if( 0 < body->_messageSize)
						{
							pPacket->ReadData(body->_message, body->_messageSize);
							body->_message[NET_NOTE_SIZE-1] = '\0';
						}
						else
						{
							pPacket->ReadData(&body->_message[0], sizeof(UINT8));
							UINT8 uiSize = MIN(body->_message[0], NET_CLAN_REQUEST_SIZE-1);
							pPacket->ReadData(&body->_message[1], uiSize);
							body->_message[NET_NOTE_SIZE-1] = '\0';
						}
					}

					// 패킷을 모두 받았다면, 유저에게 전송한다.
					if (!brokenPacket)
					{
						pSession->SendMessengerNoteList(pageCount, noteCount, noteHeaderList, noteBodyList);
					}
				}
			}
		}
		break;
	case PROTOCOL_MESSENGER_NOTE_CHECK_READED_ACK:
		{
			INT32 i32sessionIdx;
			TUID i64UID;

			pPacket->ReadData(&i32sessionIdx, sizeof(INT32));
			pPacket->ReadData(&i64UID, sizeof(TUID));

			CUserSession* pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID); 

			if (pSession != NULL)
			{
				UINT8 readedCount;
				INT32 noteIdList[MAX_NORMAL_NOTE_COUNT];

				pPacket->ReadData(&readedCount, sizeof(UINT8));

				readedCount = MIN(readedCount, MAX_NORMAL_NOTE_COUNT);

				pPacket->ReadData(noteIdList, sizeof(INT32) * readedCount);
				pSession->SendMessengerCheckReadedNote(readedCount, noteIdList);
			}
		}
		break;
	case PROTOCOL_MESSENGER_NOTE_DELETE_ACK:
		{
			INT32	i32sessionIdx;
			TUID	i64UID;
			INT32	result;

			pPacket->ReadData(&i32sessionIdx, sizeof(INT32));
			pPacket->ReadData(&i64UID, sizeof(TUID));
			pPacket->ReadData(&result, sizeof(INT32));

			CUserSession* pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID);

			if (pSession != NULL)
			{
				UINT8 deletedCount;
				INT32 noteIdList[MAX_NORMAL_NOTE_COUNT];

				pPacket->ReadData(&deletedCount, sizeof(UINT8));

				deletedCount = MIN(deletedCount, MAX_NORMAL_NOTE_COUNT);

				pPacket->ReadData(noteIdList, sizeof(INT32) * deletedCount);
				pSession->SendMessengerDeleteNote(result, deletedCount, noteIdList);
			}
		}
		break;
	case PROTOCOL_MESSENGER_NOTE_RECEIVE_ACK:
		{
			// 항상 성공한 경우로 들어옴
			// EVENT_ERROR_SUCCESS == result

			INT32	i32sessionIdx;
			TUID	i64UID;	
			INT32	i32result;
			
			
			pPacket->ReadData(&i32sessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(TUID));
			pPacket->ReadData(&i32result,		sizeof(INT32));

			CUserSession* pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID);

			if (pSession != NULL)
			{
				NOTE_NODE	note_node;

				pPacket->ReadData(&note_node, sizeof(NOTE_NODE));				
								
				note_node._body._senderSize = MIN(note_node._body._senderSize, NET_NICK_NAME_SIZE);
				note_node._body._messageSize = MIN(note_node._body._messageSize, NET_NOTE_SIZE);

				if (0 < note_node._body._senderSize)
				{	
					note_node._body._sender[NET_NICK_NAME_SIZE-1] = '\0';
				}

				if( 0 < note_node._body._messageSize)
				{					
					note_node._body._message[NET_NOTE_SIZE-1] = '\0';
				}
				else
				{						
					note_node._body._message[NET_NOTE_SIZE-1] = '\0';
				}

				// 유저 데이터 (현재 Gift에서 사용함)
				//pPacket->ReadData(&note_node._body._user, sizeof(void));
				
				pSession->SendMessengerReceiveNote(note_node._head, note_node._body._senderSize, note_node._body._messageSize, note_node._body._sender, note_node._body._message);
			}
		}
		break;
	case PROTOCOL_MESSENGER_CLAN_NOTE_SEND_ACK:
		{
			INT32 i32sessionIdx;
			TUID i64UID;
			
			pPacket->ReadData(&i32sessionIdx, sizeof(INT32));
			pPacket->ReadData(&i64UID, sizeof(TUID));

			CUserSession* pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID);

			if (pSession != NULL)
			{
				INT32 i32result;

				pPacket->ReadData(&i32result, sizeof(INT32));

				pSession->SendMessengerClanNote(i32result);
			}
		}
		break;
	case  PROTOCOL_MESSENGER_FRIEND_INFO_ACK	:					//친구 정보를 받는다. 
		{	
			INT32				i32sessionIdx;
			TUID				i64UID;
			UINT8				ui8FriendCount;
			FRIEND_INFO_STRUCT	aFriendInfo[ FRIEND_MAX_COUNT ];

			pPacket->ReadData( &i32sessionIdx,	sizeof(INT32) );
			pPacket->ReadData( &i64UID,			sizeof(TUID) ); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData( &ui8FriendCount, sizeof(UINT8) );	LENGTH_CHECK( ui8FriendCount, FRIEND_MAX_COUNT );

				for( UINT32 i = 0; i < ui8FriendCount ; i++ )
				{
					pPacket->ReadData( &aFriendInfo[i]._ui8NickSize,	sizeof(UINT8) ); 					LENGTH_CHECK( aFriendInfo[i]._ui8NickSize, NET_NICK_NAME_SIZE );
					pPacket->ReadData( aFriendInfo[i]._strNickName,	aFriendInfo[i]._ui8NickSize );
					pPacket->ReadData( &aFriendInfo[i]._BasicInfo,	sizeof(FRIEND_INFO));

					aFriendInfo[i]._strNickName[NET_NICK_NAME_SIZE -1] = 0; 
				}
				//SetValue
				pSession->m_ui8FriendCount = ui8FriendCount; 
				pSession->SendFriendList( ui8FriendCount, aFriendInfo ); 
			}
			else
			{
				//유저없음 -명일-
				//SendUnknownUser_A(dbIdx); 
			}
		}
		break; 
	case PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK			: //변경된 친구정보를 받는다. 
		{
			INT32		i32sessionIdx;
			TUID		i64UID;
			UINT8		ui8State; 
			UINT8		ui8FriendIdx;
			FRIEND_INFO FriendBasicInfo; 
			pPacket->ReadData( &i32sessionIdx,	sizeof(INT32) );
			pPacket->ReadData( &i64UID,			sizeof(TUID) );			
			CUserSession * pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID); 
			if(pSession != NULL)
			{
				UINT8	ui8NickSize; 
				char	strNickName[NET_NICK_NAME_SIZE]; 
				pPacket->ReadData( &ui8State,		sizeof(UINT8) );		//상태				
				pPacket->ReadData( &ui8FriendIdx,	sizeof(UINT8) );		//친구인덱스
				pPacket->ReadData( &ui8NickSize,	sizeof(UINT8) );		//닉 네임 

				if( ui8FriendIdx >= FRIEND_MAX_COUNT )
				{
					g_pModuleLogFile->Write_M_Log(MODULE_MESSENGER_THREAD_IDX,pSession, "AUTH_FRIEND_INFO_CHANGE_ACK ERROR");
					break; //패킷을 무시한다. 
				}

				if(ui8NickSize > NET_NICK_NAME_SIZE)
				{
					ui8NickSize = NET_NICK_NAME_SIZE;
				}
				pPacket->ReadData( &strNickName[0], ui8NickSize );		//닉 네임 
				strNickName[NET_NICK_NAME_SIZE -1] = 0;				
				pPacket->ReadData( &FriendBasicInfo,sizeof(FRIEND_INFO));	//친구정보
				pSession->SetFriendChangeInfo_A(ui8FriendIdx, ui8State, ui8NickSize, strNickName, &FriendBasicInfo); 
			}
			else
			{
				//유저없음 -명일-
				//SendUnknownUser_A(dbIdx); 
			}
		}
		break; 
	case PROTOCOL_MESSENGER_FRIEND_ACCEPT_ACK:					//친구등록 에러 코드만 날라 온다. 정상일때는 PROTOCOL_AUTH_FRIEND_INFO_CHANGE_ACK 으로 날라옴.
		{
			INT32 i32sessionIdx;
			TUID i64UID;

			pPacket->ReadData( &i32sessionIdx,	sizeof(INT32) );
			pPacket->ReadData( &i64UID,			sizeof(TUID) ); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData( &i32Temp,		sizeof(INT32) ); 
				pSession->SendFriendAcceptError(i32Temp); 
			}
			else
			{
				//유저없음 -명일-
				//SendUnknownUser_A(dbIdx);  
			}
		}
		break; 
	case PROTOCOL_MESSENGER_FRIEND_INSERT_ACK:					//친구추가 에러 코드만 날라 온다. 정상일때는 PROTOCOL_AUTH_FRIEND_INFO_CHANGE_ACK 으로 날라옴.
		{
			INT32 i32sessionIdx;
			TUID i64UID;

			pPacket->ReadData( &i32sessionIdx,	sizeof(INT32) );
			pPacket->ReadData( &i64UID,			sizeof(TUID) ); 
			CUserSession * pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData( &i32Temp,		 sizeof(INT32) ); 
				pSession->SendFriendInsertError(i32Temp); 
			}
			else
			{
				//유저없음 -명일-
				//SendUnknownUser_A(dbIdx); 
			}
		}
		break; 
	case PROTOCOL_MESSENGER_FRIEND_DELETE_ACK:					//친구삭제 에러 코드만 날라 온다. 정상일때는 PROTOCOL_AUTH_FRIEND_INFO_CHANGE_ACK 으로 날라옴.
		{
			INT32 i32sessionIdx;
			TUID i64UID;

			pPacket->ReadData( &i32sessionIdx,	sizeof(INT32) );
			pPacket->ReadData( &i64UID,			sizeof(TUID) );
			CUserSession * pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData( &i32Temp, sizeof(INT32) );
				pSession->SendFriendDeleteError(i32Temp);
			}
			else
			{
				//유저없음 -명일-
				//SendUnknownUser_A(dbIdx); 
			}
		}
		break; 
	case PROTOCOL_MESSENGER_FRIEND_INVITED_ACK:		// 배틀룸에 친구 초대가 실패하면 에러 코드를 전송한다.(EVENT_ERROR_EVENT_NOT_LOCATION_LOBBY_USER)
		{
			INT32		i32sessionIdx;
			TUID		i64UID;
			INT32		i32Result;

			pPacket->ReadData( &i32sessionIdx,	sizeof(INT32) );
			pPacket->ReadData( &i64UID,			sizeof(TUID) ); 			

			CUserSession * pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData( &i32Result, sizeof(INT32) );
				pSession->SendFriendInvitedError(i32Result);
			}
			else
			{
				//유저없음 -명일-
				//SendUnknownUser_A(dbIdx); 
			}		
		}
		break;
	case PROTOCOL_MESSENGER_FRIEND_INVITED_REQUEST_ACK		:	// 배틀룸에 친구초대가 성공한 경우 친구에게 초대 메시지를 전송한다.
		{
			INT32	i32sessionIdx;
			TUID	i64UID;
			UINT8	ui8FriendIdx;

			pPacket->ReadData( &i32sessionIdx,	sizeof(INT32) );
			pPacket->ReadData( &i64UID,			sizeof(TUID) ); 
			pPacket->ReadData( &ui8FriendIdx,	sizeof(UINT8) );
			g_pUserManager->OnFriendInviteUserServer(i32sessionIdx, i64UID, ui8FriendIdx);
		}
		break; 
	case PROTOCOL_MESSENGER_FRIEND_ROOM_ENTER_ACK	:   break;
	case PROTOCOL_MESSENGER_FRIEND_ROOM_LEAVE_ACK	:   break;
#ifdef USE_MESS_FIND

		// 유저찾기 NEW. 2012.06.05 노준영. 
		// 두 유저가 동일한 Game서버에 있을때 받음.
	case PROTOCOL_MESSENGER_FIND_USER_ACK:	
		{
			INT32	i32SessionIdx;		// 찾기 요청한 유저 세션Idx
			TUID	i64UID;				// 찾기 요청한 유저 UID
			INT32	i32Result;			// 찾기 결과
			TUID	i64SearchUID;		// 찾고자 하는 유저 UID
			UINT32	ui32UserConnectState;	// 찾고자 하는 유저 접속상태
			INT32	i32FUserSessionIdx;		// 찾고자 하는 유저 세션Idx

			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(TUID));
			pPacket->ReadData(&i32Result,		sizeof(INT32));

	
			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);

			if (NULL == pSession ) break;// 찾기 요청한 유저가 없다. 답없음. 멘붕 

			if (EV_SUCCESSED(i32Result))
			{
				pPacket->ReadData(&i64SearchUID,		sizeof(TUID));
				pPacket->ReadData(&i32FUserSessionIdx,	sizeof(INT32));
				pPacket->ReadData(&ui32UserConnectState,sizeof(UINT32));

				CUserSession* pSearchSession = g_pUserManager->GetSession(i32FUserSessionIdx, i64SearchUID );
				if( NULL != pSearchSession )
				{
					NEW_FIND_USER_INFO	stNewFindUserInfo;
					i3mem::FillZero( &stNewFindUserInfo,	sizeof( NEW_FIND_USER_INFO ) );

					stNewFindUserInfo.m_ui8Rank = pSearchSession->m_UserInfoBasic._rank;	// 랭크
					stNewFindUserInfo.m_i32State = ui32UserConnectState;	// 유저 상태. 방, 채널, 서버 인덱스

					i3String::Copy(stNewFindUserInfo.m_strClanName, pSearchSession->m_UserInfoBasic._clanName, NET_CLAN_NAME_SIZE );

					i3mem::Copy(&stNewFindUserInfo.m_Record, &pSearchSession->m_UserInfoRecord,			sizeof(USER_INFO_RECORD) ); 
					i3mem::Copy(&stNewFindUserInfo.m_Weapon, &pSearchSession->m_UserInfoEquipmentWeapon,	sizeof(USER_INFO_EQUIPMENT_WEAPON) ); 
					i3mem::Copy(&stNewFindUserInfo.m_Chara,	 &pSearchSession->m_UserInfoEquipmentChara,	sizeof(USER_INFO_EQUIPMENT_CHARA) ); 


					pSession->SendFindUser(i32Result, &stNewFindUserInfo );

				}
				else // 찾고자 하는 유저가 없다.
				{
					i32Result = EVENT_ERROR_NEW_USER_FIND_FUSER_OFFLINE;
					pSession->SendFindUser(i32Result );
				}
			}
			else // Mess서버에서 찾기 실패.
			{
				pSession->SendFindUser(i32Result );
			}

		}
		break;
/*
	case PROTOCOL_MESSENGER_FIND_USER_ACK:
		{
			INT32 sessionIdx;
			TUID i64UID;

			pPacket->ReadData(&sessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,		sizeof(TUID));

			CUserSession* session = g_pUserManager->GetSession(sessionIdx, i64UID);

			if (session != NULL)
			{
				INT32 result;
				TUID i64SearchUID		= 0;
				UINT32 userConnectState = 0;
				
				pPacket->ReadData(&result, sizeof(INT32));

				if (EV_SUCCESSED(result))
				{
					pPacket->ReadData(&i64SearchUID, sizeof(TUID));
					pPacket->ReadData(&userConnectState, sizeof(UINT32));					
				}

				session->SendFindUser(result, i64SearchUID, userConnectState);
			}
			else
			{
				//유저없음 -명일-
				//SendUnknownUser_A(dbIdx); 
			}
		}
		break;
*/
#endif
	case PROTOCOL_MESSENGER_SEND_WHISPER_ACK:
		{
			INT32 i32sessionIdx;
			TUID i64UID;

			pPacket->ReadData(&i32sessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(TUID));

			CUserSession* session = g_pUserManager->GetSession(i32sessionIdx, i64UID);
			
			if (session != NULL)
			{
				INT32 i32result;
				char receiverNick[NET_NICK_NAME_SIZE];

				pPacket->ReadData(&i32result,	sizeof(INT32));
				pPacket->ReadData(receiverNick, NET_NICK_NAME_SIZE);

				session->SendWhisperError(i32result, receiverNick);
			}
			else
			{
				//유저없음 -명일-
				//SendUnknownUser_A(dbIdx); 
			}
		}
		break;
	case PROTOCOL_MESSENGER_RECV_WHISPER_ACK:
		{
			INT32 i32sessionIdx;
			TUID i64UID;

			pPacket->ReadData(&i32sessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(TUID));

			CUserSession* session = g_pUserManager->GetSession(i32sessionIdx, i64UID);

			if (session != NULL)
			{
				char	strsenderNick[NET_NICK_NAME_SIZE];
				INT16	i16messageSize;
				char	strmessage[MAX_STRING_COUNT];

				pPacket->ReadData(strsenderNick,	NET_NICK_NAME_SIZE);
				pPacket->ReadData(&i16messageSize,	sizeof(INT16));
				i16messageSize = MIN(i16messageSize, MAX_STRING_COUNT);

				pPacket->ReadData(strmessage, i16messageSize);			
				strmessage[i16messageSize - 1] = '\0';


				session->SendWhisper(strsenderNick, strmessage, i16messageSize);
			}
			else
			{
				//유저없음 -명일-
				//SendUnknownUser_A(dbIdx);  				
			}
		}
		break;


#if defined( USE_BLOCK_LIST )
	case PROTOCOL_MESSENGER_BLOCK_INSERT_ACK:					// 차단추가 결과 
		{
			INT32 i32SessionIdx;
			INT32 i32Result = EVENT_ERROR_EVENT_BLOCK_PARAMETER_USER;
			TUID  i64UID;
			UINT8 ui8Rank;
			INT32 i32Idx;
			UINT8 ui8NickSize;
			UINT8 ui8CommentSize;
			char strNick[NET_NICK_NAME_SIZE];
			char strComment[BLOCK_COMMENT_MAX];
 
			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32) );
			pPacket->ReadData( &i64UID,				sizeof(TUID) ); 
			pPacket->ReadData( &i32Result,			sizeof(INT32) );
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if( NULL == pSession || EV_FAILED(i32Result) )
			{
				// 잘못된 유저이거나 결과값이 실패인 경우
				pSession->SendBlockInsertResult(i32Result); 
			}
			else
			{
				// 차단 유저 추가 성공
				pPacket->ReadData( &i32Idx,				sizeof(INT32) );
				pPacket->ReadData( &ui8Rank,			sizeof(UINT8) );
				pPacket->ReadData( &ui8NickSize,		sizeof(UINT8) );
				pPacket->ReadData( strNick,				sizeof(char) * ui8NickSize  );
				pPacket->ReadData( &ui8CommentSize,		sizeof(UINT8) );
				pPacket->ReadData( strComment,			sizeof(char) * ui8CommentSize  );

				pSession->SendBlockInsertResult(i32Result, i32Idx, ui8Rank, ui8NickSize, strNick, ui8CommentSize, strComment); 
			}
		}
		break; 

	case PROTOCOL_MESSENGER_BLOCK_DELETE_ACK:					// 차단삭제 결과 
		{
			INT32 i32SessionIdx;
			INT32 i32Result;
			INT32 i32Idx = 0;
			TUID  i64UID;
 
			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32) );
			pPacket->ReadData( &i64UID,				sizeof(TUID) ); 
			pPacket->ReadData( &i32Result,			sizeof(INT32) );
			pPacket->ReadData( &i32Idx,				sizeof(INT32) );

			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL) 
			{
				pSession->SendBlockDeleteResult(i32Result, i32Idx); 
			}
		}
		break; 

		case  PROTOCOL_MESSENGER_BLOCK_INFO_ACK	:					//차단 목록 정보를 받는다. 
		{	
			INT32				i32SessionIdx;
			TUID				i64UID;
			UINT8				ui8BlockCount;
			BLOCK_INFO_STRUCT	pBlockInfo[ BLOCK_MAX_COUNT ];

			pPacket->ReadData( &i32SessionIdx, sizeof(INT32) );
			pPacket->ReadData( &i64UID,		 sizeof(TUID) ); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData( &ui8BlockCount, sizeof(UINT8) );	LENGTH_CHECK( ui8BlockCount, BLOCK_MAX_COUNT );

				for( UINT32 i = 0; i < ui8BlockCount ; i++ )
				{
					pPacket->ReadData( &pBlockInfo[i]._ui8NickSize,	sizeof(UINT8) ); 					LENGTH_CHECK( pBlockInfo[i]._ui8NickSize, NET_NICK_NAME_SIZE );
					pPacket->ReadData( pBlockInfo[i]._strNickName,	pBlockInfo[i]._ui8NickSize );
					pPacket->ReadData( &pBlockInfo[i]._ui8CommentSize,	sizeof(UINT8) ); 					LENGTH_CHECK( pBlockInfo[i]._ui8CommentSize, BLOCK_COMMENT_MAX );
					pPacket->ReadData( pBlockInfo[i]._strComment,	pBlockInfo[i]._ui8CommentSize );
					pPacket->ReadData( &pBlockInfo[i]._BasicInfo,	sizeof(BLOCK_INFO));

					pBlockInfo[i]._strNickName[NET_NICK_NAME_SIZE -1] = 0; 
					
					//pSession->m_i64BlockUID[i] = pBlockInfo[i]._BasicInfo._buid;	// 자신의 차단 유저 리스트에 저장.
				}
				//SetValue
				pSession->m_ui8BlockCount = ui8BlockCount; 
				pSession->SendBlockList( ui8BlockCount, pBlockInfo ); 
			}
			else
			{
			 
			}
		}
		break; 
#endif

#ifdef USE_UNDECIDED_DESIGN_TEST
	case PROTOCOL_MESSENGER_RECV_CHANNEL_NUM_ACK:	// Do nothing
		break;
#endif

	// NEW 유저 찾기. 2012.06.07 노준영. 
	// 찾기 요청한 유저와 찾고자 하는 유저가 다른서버에 있을때 찾고자 하는 유저가 있는 서버에서 받는 패킷.
	case PROTOCOL_MESSENGER_FUSER_INFO_REQ :	
		{
			INT32	i32RUserSessionIdx;	// 찾기 요청한 유저 세션 인덱스.
			INT32	i32FUserSessionIdx;	// 찾고자 하는 유저 세션 인덱스.
			TUID	i64RUserUID;	// 찾기 요청한 유저 UID.
			TUID	i64FUserUID;	// 찾고자 하는 유저 UID.


			pPacket->ReadData(&i32RUserSessionIdx, sizeof(INT32));
			pPacket->ReadData(&i64RUserUID, sizeof(TUID));
			pPacket->ReadData(&i32FUserSessionIdx, sizeof(INT32) ) ;
			pPacket->ReadData(&i64FUserUID, sizeof(TUID));

			CUserSession* pFUserSession = g_pUserManager->GetSession(i32FUserSessionIdx, i64FUserUID);

			i3NetworkPacket SendPacket( PROTOCOL_MESSENGER_FUSER_INFO_ACK  );
			NEW_FIND_USER_INFO_SEV stNewFindUserInfoSev;
			stNewFindUserInfoSev.m_i32RUserSessionIdx = i32RUserSessionIdx;
			stNewFindUserInfoSev.m_i32FUserSessionIdx = i32FUserSessionIdx;
			stNewFindUserInfoSev.m_i64RUserUID = i64RUserUID;
			stNewFindUserInfoSev.m_i64FUserUID = i64FUserUID;

			if( NULL != pFUserSession ) // 찾고자 하는 유저가 있다. 성공!
			{
				stNewFindUserInfoSev.m_i32Result = EVENT_ERROR_SUCCESS;
				i3String::Copy(stNewFindUserInfoSev.m_NewFindUserInfo.m_strClanName, pFUserSession->m_UserInfoBasic._clanName, NET_CLAN_NAME_SIZE );

				i3mem::Copy(&stNewFindUserInfoSev.m_NewFindUserInfo.m_Record,	&pFUserSession->m_UserInfoRecord,			sizeof(USER_INFO_RECORD) ); 
				i3mem::Copy(&stNewFindUserInfoSev.m_NewFindUserInfo.m_Weapon,	&pFUserSession->m_UserInfoEquipmentWeapon,	sizeof(USER_INFO_EQUIPMENT_WEAPON) ); 
				i3mem::Copy(&stNewFindUserInfoSev.m_NewFindUserInfo.m_Chara,	&pFUserSession->m_UserInfoEquipmentChara,	sizeof(USER_INFO_EQUIPMENT_CHARA) ); 

				SendPacket.WriteData( &stNewFindUserInfoSev, sizeof( NEW_FIND_USER_INFO_SEV ) );
				SendPacketMessage(&SendPacket);	// Mess 서버로 바로 쏜다.

			}
			else 	// 찾고자 하는 유저가 없다. 멘붕 ㅜㅜ
			{
				stNewFindUserInfoSev.m_i32Result = EVENT_ERROR_NEW_USER_FIND_FUSER_OFFLINE;

				i3NetworkPacket SendPacket( PROTOCOL_MESSENGER_FUSER_INFO_ACK  );
				SendPacket.WriteData( &stNewFindUserInfoSev, sizeof( NEW_FIND_USER_INFO_SEV ) );
				SendPacketMessage(&SendPacket);	// Mess 서버로 바로 쏜다.
			}


		}
		break;

		// NEW 유저 찾기. 2012.06.11 노준영. 
		// 두 유저가 다른 서버에 있을떄 찾기 요청한 유저가 접속한 Game서버가 받는다.
	case PROTOCOL_MESSENGER_FUSER_INFO_FIN_ACK :
		{
			NEW_FIND_USER_INFO_SEV stNewFindUserInfoSev;
			pPacket->ReadData(&stNewFindUserInfoSev, sizeof(NEW_FIND_USER_INFO_SEV));


			CUserSession* pSession = g_pUserManager->GetSession(stNewFindUserInfoSev.m_i64RUserUID);
			if( NULL != pSession )
			{
				if (EV_SUCCESSED(stNewFindUserInfoSev.m_i32Result))
				{
					pSession->SendFindUser(stNewFindUserInfoSev.m_i32Result, &stNewFindUserInfoSev.m_NewFindUserInfo );
				}
				else // Mess서버에서 찾기 실패.
				{
					pSession->SendFindUser(stNewFindUserInfoSev.m_i32Result );
				}

			}
			// else // 찾기 하는 유저가 없다. 답없음.
		}
		break;


	default: 
		g_pModuleLogFile->Write_M_Log(MODULE_MESSENGER_THREAD_IDX, NULL, "[CModuleMessengerSocket::PacketParsing]Unknown Message"); 
		break; 
	}

	m_ui32LastSendTime	= g_pContextMain->GetServerTime();
	m_ui32LastRecvTime	= g_pContextMain->GetServerTime();

	return; 
}
