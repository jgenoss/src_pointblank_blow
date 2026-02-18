#include "pch.h"
#include "ModuleMessengerSocket.h"
#include "UserManager.h"
#include "UserSession.h"
#include "ModuleClan.h"
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

BOOL CModuleMessengerSocket::OnCreate( UINT32 ui32IP, UINT16 ui16PortNo)
{
	BOOL bRv = i3NetworkClientSocket::OnCreate( ui32IP, ui16PortNo );

	m_ui32LastSendTime		= i3ThreadTimer::GetServerTime();
	m_ui32LastRecvTime		= i3ThreadTimer::GetServerTime();
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
			g_pLog->WriteLog( L"[SUCC] MESSENGER SERVER CONNECT MESSAGE RECV" );
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
			T_UID i64UID;
			
			pPacket->ReadData(&i32sessionIdx, sizeof(INT32));
			pPacket->ReadData(&i64UID, sizeof(T_UID));

			CUserSession* pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID);

			if (pSession != NULL)
			{
				pPacket->ReadData(&pSession->m_ui8FriendCount, sizeof(UINT8));
#if defined( USE_BLOCK_LIST  )
				pPacket->ReadData(&pSession->m_ui8BlockCount, sizeof(UINT8));
#endif
			}
		}
		break;
	case PROTOCOL_MESSENGER_NOTE_SEND_ACK:
		{
			INT32 i32sessionIdx;
			T_UID i64UID;
			
			pPacket->ReadData(&i32sessionIdx, sizeof(INT32));
			pPacket->ReadData(&i64UID, sizeof(T_UID));

			CUserSession* pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID);

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
			T_UID i64UID;

			pPacket->ReadData(&i32sessionIdx, sizeof(INT32));
			pPacket->ReadData(&i64UID, sizeof(T_UID));

			CUserSession* pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID);

			if (pSession != NULL)
			{
				UINT8 ui8pageCount;
				UINT8 ui8noteCount;

				pPacket->ReadData(&ui8pageCount, sizeof(UINT8));
				pPacket->ReadData(&ui8noteCount, sizeof(UINT8));
				
				ui8noteCount = MIN(ui8noteCount, MAX_NOTE_COUNT_PER_PAGE);

				if (0 < ui8noteCount)
				{
					BOOL bBrokenPacket = FALSE;

					NOTE_HEADER aNoteHeaderList[MAX_NOTE_COUNT_PER_PAGE];
					NOTE_BODY aNoteBodyList[MAX_NOTE_COUNT_PER_PAGE];
					
					pPacket->ReadData(aNoteHeaderList, sizeof(NOTE_HEADER) * ui8noteCount);

					for(INT32 i = 0; i < ui8noteCount; i++)
					{
						NOTE_BODY* body = &aNoteBodyList[i];

						pPacket->ReadData(&body->_senderSize,	sizeof(UINT8));
						pPacket->ReadData(&body->_messageSize,	sizeof(UINT8));

						body->_senderSize = MIN(body->_senderSize, NET_NICK_NAME_SIZE);
						body->_messageSize = MIN(body->_messageSize, NET_NOTE_SIZE);

						if (0 < body->_senderSize )
						{
							pPacket->ReadData(body->_sender,	sizeof(TTCHAR) * body->_senderSize);
							body->_sender[NET_NICK_NAME_SIZE-1] = '\0';
						}
						else
						{
							// 쪽지함 데이터가 깨졌다.
							bBrokenPacket = TRUE;
							g_pLog->WriteLog( L"Broken NoteBox" );
							break;
						}
						if( 0 < body->_messageSize)
						{
							pPacket->ReadData(body->_message,	sizeof(TTCHAR) * body->_messageSize);
							body->_message[NET_NOTE_SIZE-1] = '\0';
						}
						else
						{
							pPacket->ReadData(&body->_message[0], sizeof(TTCHAR));
							UINT8 uiSize = (UINT8)(MIN(body->_message[0], NET_NOTE_SIZE-1));
							pPacket->ReadData(&body->_message[1], sizeof(TTCHAR) * uiSize);
							body->_message[NET_NOTE_SIZE-1] = '\0';
						}
					}

					// 패킷을 모두 받았다면, 유저에게 전송한다.
					if (!bBrokenPacket)
					{
						pSession->SendMessengerNoteList(ui8pageCount, ui8noteCount, aNoteHeaderList, aNoteBodyList);
					}
				}
			}
		}
		break;
	case PROTOCOL_MESSENGER_NOTE_CHECK_READED_ACK:
		{
			INT32 i32sessionIdx;
			T_UID i64UID;

			pPacket->ReadData(&i32sessionIdx, sizeof(INT32));
			pPacket->ReadData(&i64UID, sizeof(T_UID));

			CUserSession* pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID); 

			if (pSession != NULL)
			{
				UINT8 ui8readedCount;
				INT32 ai32noteIdList[MAX_NORMAL_NOTE_COUNT];

				pPacket->ReadData(&ui8readedCount, sizeof(UINT8));

				ui8readedCount = MIN(ui8readedCount, MAX_NORMAL_NOTE_COUNT);

				pPacket->ReadData(ai32noteIdList, sizeof(INT32) * ui8readedCount);
				pSession->SendMessengerCheckReadedNote(ui8readedCount, ai32noteIdList);
			}
		}
		break;
	case PROTOCOL_MESSENGER_NOTE_DELETE_ACK:
		{
			INT32	i32sessionIdx;
			T_UID	i64UID;
			INT32	i32result;

			pPacket->ReadData(&i32sessionIdx, sizeof(INT32));
			pPacket->ReadData(&i64UID, sizeof(T_UID));
			pPacket->ReadData(&i32result, sizeof(INT32));

			CUserSession* pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID);

			if (pSession != NULL)
			{
				UINT8 deletedCount;
				INT32 noteIdList[MAX_NORMAL_NOTE_COUNT];

				pPacket->ReadData(&deletedCount, sizeof(UINT8));

				deletedCount = MIN(deletedCount, MAX_NORMAL_NOTE_COUNT);

				pPacket->ReadData(noteIdList, sizeof(INT32) * deletedCount);
				pSession->SendMessengerDeleteNote(i32result, deletedCount, noteIdList);
			}
		}
		break;
	case PROTOCOL_MESSENGER_NOTE_RECEIVE_ACK:
		{
			// 항상 성공한 경우로 들어옴
			// EVENT_ERROR_SUCCESS == result

			INT32	i32sessionIdx;
			T_UID	i64UID;	
			INT32	i32result;
			
			pPacket->ReadData(&i32sessionIdx, sizeof(INT32));
			pPacket->ReadData(&i64UID, sizeof(T_UID));
			pPacket->ReadData(&i32result, sizeof(INT32));

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

				note_node._body._message[NET_NOTE_SIZE-1] = '\0';

				// 유저 데이터 (현재 Gift에서 사용함)
				//pPacket->ReadData(&note_node._body._user, sizeof(void));
				
				pSession->SendMessengerReceiveNote(note_node._head, note_node._body._senderSize, note_node._body._messageSize, note_node._body._sender, note_node._body._message);
			}
		}
		break;
	case PROTOCOL_MESSENGER_CLAN_NOTE_SEND_ACK:
		{
			INT32 i32sessionIdx;
			T_UID i64UID;
			
			pPacket->ReadData(&i32sessionIdx, sizeof(INT32));
			pPacket->ReadData(&i64UID, sizeof(T_UID));

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
			T_UID				i64UID;
			UINT8				ui8FriendCount;
			FRIEND_INFO_STRUCT	aFriendInfo[ FRIEND_MAX_COUNT ];

			pPacket->ReadData( &i32sessionIdx, sizeof(INT32) );
			pPacket->ReadData( &i64UID,		 sizeof(T_UID) ); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData( &ui8FriendCount, sizeof(UINT8) );	LENGTH_CHECK( ui8FriendCount, FRIEND_MAX_COUNT );

				for( UINT32 i = 0; i < ui8FriendCount ; i++ )
				{
					pPacket->ReadData( &aFriendInfo[i]._NickSize,	sizeof(UINT8) ); 					LENGTH_CHECK( aFriendInfo[i]._NickSize, NET_NICK_NAME_SIZE );
					pPacket->ReadData( aFriendInfo[i]._NickName,	sizeof(TTCHAR) * aFriendInfo[i]._NickSize );
					pPacket->ReadData( &aFriendInfo[i]._BasicInfo,	sizeof(FRIEND_INFO));

					aFriendInfo[i]._NickName[NET_NICK_NAME_SIZE -1] = 0; 
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
			T_UID		i64UID;
			UINT8		ui8State; 
			UINT8		ui8FriendIdx;
			FRIEND_INFO FriendBasicInfo; 
			pPacket->ReadData( &i32sessionIdx,	sizeof(INT32) );
			pPacket->ReadData( &i64UID,		sizeof(T_UID) );			
			CUserSession * pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID); 
			if(pSession != NULL)
			{
				UINT8	ui8NickSize; 
				TTCHAR	strNickName[NET_NICK_NAME_SIZE]; 
				pPacket->ReadData( &ui8State,			sizeof(UINT8) );		//상태				
				pPacket->ReadData( &ui8FriendIdx,		sizeof(UINT8) );		//친구인덱스
				pPacket->ReadData( &ui8NickSize,		sizeof(UINT8) );		//닉 네임 

				if( ui8FriendIdx >= FRIEND_MAX_COUNT )
				{
					g_pLog->WriteLogSession( pSession, L"AUTH_FRIEND_INFO_CHANGE_ACK ERROR" );
					break; //패킷을 무시한다. 
				}

				if(ui8NickSize > NET_NICK_NAME_SIZE)
				{
					ui8NickSize = NET_NICK_NAME_SIZE;
				}
				pPacket->ReadData( &strNickName[0], sizeof(TTCHAR) * ui8NickSize );		//닉 네임 
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
			T_UID i64UID;

			pPacket->ReadData( &i32sessionIdx,	sizeof(INT32) );
			pPacket->ReadData( &i64UID,			sizeof(T_UID) ); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData( &i32Temp,	sizeof(INT32) ); 
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
			T_UID i64UID;

			pPacket->ReadData( &i32sessionIdx,	sizeof(INT32) );
			pPacket->ReadData( &i64UID,			sizeof(T_UID) ); 
			CUserSession * pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData( &i32Temp,	sizeof(INT32) ); 
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
			T_UID i64UID;

			pPacket->ReadData( &i32sessionIdx,	sizeof(INT32) );
			pPacket->ReadData( &i64UID,		sizeof(T_UID) );
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
			T_UID		i64UID;
			INT32		i32Result;

			pPacket->ReadData( &i32sessionIdx,	sizeof(INT32) );
			pPacket->ReadData( &i64UID,			sizeof(T_UID) ); 			

			CUserSession * pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData( &i32Result,	sizeof(INT32) );
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
			T_UID i64ReqUID;	// 초대 요청한 유저

			// 초대 받은 유저
			INT32 i32sessionIdx;
			T_UID i64UID;	

			UINT8 ui8FriendIdx;

			pPacket->ReadData( &i64ReqUID,			sizeof(T_UID) ); 
			pPacket->ReadData( &i32sessionIdx,		sizeof(INT32) );
			pPacket->ReadData( &i64UID,				sizeof(T_UID) ); 
			pPacket->ReadData( &ui8FriendIdx,		sizeof(UINT8) );

			CUserSession * pInvitedSession = g_pUserManager->GetSession(i32sessionIdx, i64UID);
			if( NULL == pInvitedSession ) break;

			// 용병이라면 초대 요청한 유저에게 실패를 보냅니다.
			if( pInvitedSession->IsMercenary() )
			{
				T_RESULT TResult = EVENT_ERROR_EVENT_FAIL_INVITED_USER_IN_CLAN_MATCH;
				i3NetworkPacket FainPacket( PROTOCOL_MESSENGER_FRIEND_INVITE_FAIL_REQ );
				FainPacket.WriteData(&i64ReqUID,	sizeof(T_UID));
				FainPacket.WriteData(&TResult,		sizeof(T_RESULT));
				SendPacketMessage( &FainPacket );
				break;
			}

			// 친구 초대 성공.
			i3NetworkPacket SendPacket( PROTOCOL_AUTH_FRIEND_INVITED_REQUEST_ACK );
			SendPacket.WriteData(&ui8FriendIdx, sizeof(UINT8));
			pInvitedSession->SendPacketMessage( &SendPacket );
	
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
			T_UID	i64UID;				// 찾기 요청한 유저 UID
			INT32	i32Result;			// 찾기 결과
			T_UID	i64SearchUID;		// 찾고자 하는 유저 UID
			UINT32	ui32UserConnectState;	// 찾고자 하는 유저 접속상태
			INT32	i32FUserSessionIdx;		// 찾고자 하는 유저 세션Idx

			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(T_UID));
			pPacket->ReadData(&i32Result,		sizeof(INT32));

	
			CUserSession* pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID);

			if (NULL == pSession ) break;// 찾기 요청한 유저가 없다. 답없음. 멘붕 

			if (EV_SUCCESSED(i32Result))
			{
				pPacket->ReadData(&i64SearchUID, sizeof(T_UID));
				pPacket->ReadData(&i32FUserSessionIdx, sizeof(INT32));
				pPacket->ReadData(&ui32UserConnectState, sizeof(UINT32));

				//////////////////////////////////////////////////////////////////////////
				// 유저찾기 계정 갱신(최신화)
				pSession->m_i64FindUID = i64SearchUID;

				//////////////////////////////////////////////////////////////////////////
				CUserSession* pSearchSession = g_pUserManager->GetSession(i32FUserSessionIdx, i64SearchUID );
				if( NULL != pSearchSession )
				{
					NEW_FIND_USER_INFO	stNewFindUserInfo;
					i3mem::FillZero( &stNewFindUserInfo,	sizeof( NEW_FIND_USER_INFO ) );

					stNewFindUserInfo.m_ui8Rank = (UINT8)pSearchSession->m_ui32ViewRank;	// 랭크
					stNewFindUserInfo.m_i32State = ui32UserConnectState;	// 유저 상태. 방, 채널, 서버 인덱스

					i3String::Copy(stNewFindUserInfo.m_strClanName, pSearchSession->m_UserInfoBasic.m_strClanName, sizeof(TTCHAR) * NET_CLAN_NAME_SIZE );

					i3mem::Copy(&stNewFindUserInfo.m_Record, &pSearchSession->m_UserInfoRecord,			sizeof(USER_INFO_RECORD) ); 

					ITEM_INFO aCharEquipment[ CHAR_EQUIPMENT_COUNT ];
					ITEM_INFO aCommonEquips[ CHAR_EQUIPMENT_COMMON_COUNT ];
					ITEM_INFO ItemInfoBlueChara;

					INT32 i32BlueSlot = pSearchSession->m_MultiSlot.GetTeamSlot(TEAM_BLUE);
					pSearchSession->m_MultiSlot.GetCharEquipment( i32BlueSlot, CHAR_EQUIPMENT_CHARA, &ItemInfoBlueChara  );
					stNewFindUserInfo.m_CharaItemID[TEAM_BLUE] = ItemInfoBlueChara.m_TItemID;

					INT32 i32RedSlot = pSearchSession->m_MultiSlot.GetTeamSlot(TEAM_RED);
					pSearchSession->m_MultiSlot.GetCharEquipmentAll( i32RedSlot, aCharEquipment );
					pSearchSession->m_MultiSlot.GetCommonEquipmentAll( aCommonEquips );

					stNewFindUserInfo.m_CharaItemID[TEAM_RED] = aCharEquipment[CHAR_EQUIPMENT_CHARA].m_TItemID;

					USER_INFO_EQUIPMENT_CHARA EquipmentChar;
					USER_INFO_EQUIPMENT_WEAPON EquipmentWeapon;
					// vv3 - check
					for( INT32 i = 0 ; i < CHAR_EQUIPMENT_WEAPON_COUNT ; i++ )
					{
						EquipmentWeapon.m_TItemID[ i ]	= aCharEquipment[ i ].m_TItemID;
					}
					for( INT32 i = 0 ; i < CHAR_EQUIPMENT_PARTS_COUNT ; i++ )
					{
						EquipmentChar.SetEquipNum( (CHAR_EQUIPMENT_PARTS)i, aCharEquipment[ CHAR_EQUIPMENT_WEAPON_COUNT+i ].m_TItemID );
					}

					//EquipmentChar.SetEquipNum( BATTLE_EQUIPMENT_PARTS_DINO,	aCommonEquips[ CHAR_EQUIPMENT_COMMON_DINO ].m_TItemID );

					i3mem::Copy(&stNewFindUserInfo.m_Weapon, &EquipmentWeapon,	sizeof(USER_INFO_EQUIPMENT_WEAPON) ); 
					i3mem::Copy(&stNewFindUserInfo.m_Chara,	 &EquipmentChar,	sizeof(USER_INFO_EQUIPMENT_CHARA) ); 


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
			T_UID i64UID;

			pPacket->ReadData(&sessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,		sizeof(T_UID));

			CUserSession* session = g_pUserManager->GetSession(sessionIdx, i64UID);

			if (session != NULL)
			{
				INT32 result;
				T_UID i64SearchUID		= 0;
				UINT32 userConnectState = 0;
				
				pPacket->ReadData(&result, sizeof(INT32));

				if (EV_SUCCESSED(result))
				{
					pPacket->ReadData(&i64SearchUID, sizeof(T_UID));
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
			T_UID i64UID;

			pPacket->ReadData(&i32sessionIdx, sizeof(INT32));
			pPacket->ReadData(&i64UID, sizeof(T_UID));

			CUserSession* pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID);
			
			if (pSession != NULL)
			{
				INT32 i32result;
				TTCHAR strReceiverNick[NET_NICK_NAME_SIZE];
				UINT8 ui8NickColor = 0;

				pPacket->ReadData(&i32result, sizeof(INT32));
				pPacket->ReadData(strReceiverNick, sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
				pPacket->ReadData(&ui8NickColor, sizeof(UINT8));

				pSession->SendWhisperError(i32result, strReceiverNick, ui8NickColor);
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
			T_UID i64UID;

			pPacket->ReadData(&i32sessionIdx, sizeof(INT32));
			pPacket->ReadData(&i64UID, sizeof(T_UID));

			CUserSession* pSession = g_pUserManager->GetSession(i32sessionIdx, i64UID);

			if (pSession != NULL)
			{
				TTCHAR	strSenderNick[NET_NICK_NAME_SIZE];
				INT16	i16messageSize;
				TTCHAR	strMessage[MAX_STRING_COUNT];
				bool	bIsGMUser;
				UINT8 ui8NickColor = 0;

				pPacket->ReadData(strSenderNick,		sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
				pPacket->ReadData(&bIsGMUser,		sizeof(bool));
				pPacket->ReadData(&i16messageSize,		sizeof(INT16));
				i16messageSize = MIN(i16messageSize,	MAX_STRING_COUNT);

				pPacket->ReadData(strMessage,			sizeof(TTCHAR) * i16messageSize);
				strMessage[i16messageSize - 1] = '\0';

				pPacket->ReadData(&ui8NickColor, sizeof(UINT8));


				pSession->SendWhisper(strSenderNick, strMessage, i16messageSize, bIsGMUser,ui8NickColor);
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
			T_UID  i64UID;
			UINT8 ui8Rank;
			INT32 i32Idx;
			UINT8 ui8NickSize;
			UINT8 ui8CommentSize;
			TTCHAR strNick[NET_NICK_NAME_SIZE];
			TTCHAR strComment[BLOCK_COMMENT_MAX];
 
			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32) );
			pPacket->ReadData( &i64UID,				sizeof(T_UID) ); 
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
				pPacket->ReadData( strNick,				sizeof(TTCHAR) * ui8NickSize  );
				pPacket->ReadData( &ui8CommentSize,		sizeof(UINT8) );
				pPacket->ReadData( strComment,			sizeof(TTCHAR) * ui8CommentSize  );

				pSession->SendBlockInsertResult(i32Result, i32Idx, ui8Rank, ui8NickSize, strNick, ui8CommentSize, strComment); 
			}
		}
		break; 

	case PROTOCOL_MESSENGER_BLOCK_DELETE_ACK:					// 차단삭제 결과 
		{
			INT32 i32SessionIdx;
			INT32 i32Result;
			INT32 i32Idx = 0;
			T_UID  i64UID;
 
			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32) );
			pPacket->ReadData( &i64UID,				sizeof(T_UID) ); 
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
			T_UID				i64UID;
			UINT8				ui8BlockCount;
			BLOCK_INFO_STRUCT	pBlockInfo[ BLOCK_MAX_COUNT ];

			pPacket->ReadData( &i32SessionIdx, sizeof(INT32) );
			pPacket->ReadData( &i64UID,		 sizeof(T_UID) ); 			
			CUserSession * pSession = g_pUserManager->GetSession(i32SessionIdx, i64UID); 
			if(pSession != NULL)
			{
				pPacket->ReadData( &ui8BlockCount, sizeof(UINT8) );	LENGTH_CHECK( ui8BlockCount, BLOCK_MAX_COUNT );

				for( UINT32 i = 0; i < ui8BlockCount ; i++ )
				{
					pPacket->ReadData( &pBlockInfo[i]._NickSize,	sizeof(UINT8) ); 					LENGTH_CHECK( pBlockInfo[i]._NickSize, NET_NICK_NAME_SIZE );
					pPacket->ReadData( pBlockInfo[i]._NickName,		sizeof(TTCHAR) * pBlockInfo[i]._NickSize );
					pPacket->ReadData( &pBlockInfo[i]._CommentSize,	sizeof(UINT8) ); 					LENGTH_CHECK( pBlockInfo[i]._CommentSize, BLOCK_COMMENT_MAX );
					pPacket->ReadData( pBlockInfo[i]._Comment,		sizeof(TTCHAR) * pBlockInfo[i]._CommentSize );
					pPacket->ReadData( &pBlockInfo[i]._BasicInfo,	sizeof(BLOCK_INFO));

					pBlockInfo[i]._NickName[NET_NICK_NAME_SIZE -1] = 0; 
					
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
	// NEW 유저 찾기. 2012.06.07 노준영. 
	// 찾기 요청한 유저와 찾고자 하는 유저가 다른서버에 있을때 찾고자 하는 유저가 있는 서버에서 받는 패킷.
	case PROTOCOL_MESSENGER_FUSER_INFO_REQ :	
		{
			INT32	i32RUserSessionIdx;	// 찾기 요청한 유저 세션 인덱스.
			INT32	i32FUserSessionIdx;	// 찾고자 하는 유저 세션 인덱스.
			T_UID	i64RUserUID;	// 찾기 요청한 유저 UID.
			T_UID	i64FUserUID;	// 찾고자 하는 유저 UID.


			pPacket->ReadData(&i32RUserSessionIdx, sizeof(INT32));
			pPacket->ReadData(&i64RUserUID, sizeof(T_UID));
			pPacket->ReadData(&i32FUserSessionIdx, sizeof(INT32) ) ;
			pPacket->ReadData(&i64FUserUID, sizeof(T_UID));

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
				i3String::Copy(stNewFindUserInfoSev.m_NewFindUserInfo.m_strClanName, pFUserSession->m_UserInfoBasic.m_strClanName, NET_CLAN_NAME_SIZE );

				i3mem::Copy(&stNewFindUserInfoSev.m_NewFindUserInfo.m_Record,	&pFUserSession->m_UserInfoRecord,			sizeof(USER_INFO_RECORD) ); 

				ITEM_INFO ItemInfoBlueChara;
				INT32 i32BlueSlot = pFUserSession->m_MultiSlot.GetTeamSlot(TEAM_BLUE);
				pFUserSession->m_MultiSlot.GetCharEquipment( i32BlueSlot, CHAR_EQUIPMENT_CHARA, &ItemInfoBlueChara  );
				stNewFindUserInfoSev.m_NewFindUserInfo.m_CharaItemID[TEAM_BLUE] = ItemInfoBlueChara.m_TItemID;

				INT32 i32RedSlot = pFUserSession->m_MultiSlot.GetTeamSlot(TEAM_RED);
				ITEM_INFO aCharEquipment[ CHAR_EQUIPMENT_COUNT ];
				ITEM_INFO aCommonEquips[ CHAR_EQUIPMENT_COMMON_COUNT ];
				pFUserSession->m_MultiSlot.GetCharEquipmentAll( i32RedSlot, aCharEquipment );
				pFUserSession->m_MultiSlot.GetCommonEquipmentAll( aCommonEquips );
				stNewFindUserInfoSev.m_NewFindUserInfo.m_CharaItemID[TEAM_RED] = aCharEquipment[CHAR_EQUIPMENT_CHARA].m_TItemID;

				USER_INFO_EQUIPMENT_CHARA EquipmentChar;
				USER_INFO_EQUIPMENT_WEAPON EquipmentWeapon;

				for( INT32 i = 0 ; i < CHAR_EQUIPMENT_WEAPON_COUNT ; i++ )
				{
					EquipmentWeapon.m_TItemID[ i ]	= aCharEquipment[ i ].m_TItemID;
				}
				// vv3 - check
				for( INT32 i = 0 ; i < CHAR_EQUIPMENT_PARTS_COUNT ; i++ )
				{
					EquipmentChar.SetEquipNum( (CHAR_EQUIPMENT_PARTS)i, aCharEquipment[ CHAR_EQUIPMENT_WEAPON_COUNT+i ].m_TItemID ); 
				}

				//EquipmentChar.SetEquipNum( BATTLE_EQUIPMENT_PARTS_DINO,			aCommonEquips[ CHAR_EQUIPMENT_COMMON_DINO ].m_TItemID );

				i3mem::Copy(&stNewFindUserInfoSev.m_NewFindUserInfo.m_Weapon,	&EquipmentWeapon,	sizeof(USER_INFO_EQUIPMENT_WEAPON) ); 
				i3mem::Copy(&stNewFindUserInfoSev.m_NewFindUserInfo.m_Chara,	&EquipmentChar,	sizeof(USER_INFO_EQUIPMENT_CHARA) ); 

				SendPacket.WriteData( &stNewFindUserInfoSev, sizeof( NEW_FIND_USER_INFO_SEV ) );
				SendPacketMessage(&SendPacket);	// Mess 서버로 바로 쏜다.

			}
			else 	// 찾고자 하는 유저가 없다. 멘붕 ㅜㅜ
			{
				stNewFindUserInfoSev.m_i32Result = EVENT_ERROR_NEW_USER_FIND_FUSER_OFFLINE;

				i3NetworkPacket SendPacket2( PROTOCOL_MESSENGER_FUSER_INFO_ACK  );
				SendPacket2.WriteData( &stNewFindUserInfoSev, sizeof( NEW_FIND_USER_INFO_SEV ) );
				SendPacketMessage(&SendPacket2 );	// Mess 서버로 바로 쏜다.
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
		g_pLog->WriteLog( L"[CModuleMessengerSocket::PacketParsing]Unknown Message" );
		break; 
	}

	m_ui32LastSendTime	= i3ThreadTimer::GetServerTime();
	m_ui32LastRecvTime	= i3ThreadTimer::GetServerTime();

	return; 
}

