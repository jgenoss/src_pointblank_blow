#include "pch.h"
#include "UserSession.h"
#include "RoomManager.h"
#include "UdpHolePunching.h"
#include "DataBase.h"
#include "Usermanager.h"
#include "ServerContext.h"
#include "Room.h"
#include "ModuleTrans.h"
#include "ModuleMessenger.h"
#include "ModuleContextNc.h"

BOOL	CUserSession::_PacketParsingAuth_server( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled )
{
	PROTOCOL	ProtocolID = pPacket->GetProtocolID();	
	BOOL		bSendMessage = FALSE; 

	switch( ProtocolID )
	{
	case PROTOCOL_AUTH_FRIEND_ACCEPT_REQ	:			//080303
		{
			CHECKVALID_CHANNEL;
			UINT8 ui8FriendIdx; 			
			pPacket->ReadData(&ui8FriendIdx, sizeof(UINT8));
			if(ui8FriendIdx < m_ui8FriendCount)		
			{
				g_pModuleMessenger->MessengerFriendAccept( m_WorkThreadIdx, this,ui8FriendIdx );
			}
			else
			{
                SendFriendAcceptError(EVENT_ERROR_EVENT_FRIEND_INVITED_PARAMETER_IDX); 			
			}
		}
		break; 
	case PROTOCOL_AUTH_FRIEND_INSERT_REQ	:			//080303
		{
			CHECKVALID_CHANNEL;
			char strnickname[NET_NICK_NAME_SIZE];

			pPacket->ReadData(strnickname, sizeof(strnickname) );
			strnickname[NET_NICK_NAME_SIZE - 1] = 0;

			INT32 i32friendDbIdx = 0;

			INT32 i32result = EVENT_ERROR_SUCCESS;
			//INT32 result = -1;//g_pDataBase->FindDbIdxByNickname_U(this, nickname, &friendDbIdx);

			if (EVENT_ERROR_SUCCESS == i32result)
			{
				g_pModuleMessenger->MessengerSendFriendInsert( m_WorkThreadIdx, this, strnickname);
			}
			else
				SendFriendInsertError(i32result);
		}
		break; 
	case PROTOCOL_AUTH_FRIEND_DELETE_REQ	:			//080303
		{
			CHECKVALID_CHANNEL;
			UINT8 ui8FriendIdx; 			
			pPacket->ReadData(&ui8FriendIdx, sizeof(UINT8));
			if(ui8FriendIdx < m_ui8FriendCount)
			{
				g_pModuleMessenger->MessengerSendFriendDelete( m_WorkThreadIdx, this, ui8FriendIdx );				
			}
			else
				SendFriendDeleteError(EVENT_ERROR_EVENT_FRIEND_INVITED_PARAMETER_IDX); 
		}
		break; 
	case PROTOCOL_AUTH_FRIEND_INVITED_REQ:				//다시확인해야 함...성백.
		{
			CHECKVALID_ROOM;
			UINT8  ui8InvitedUser; 
			pPacket->ReadData(&ui8InvitedUser,  sizeof(UINT8));			
			if(ui8InvitedUser < m_ui8FriendCount)
			{
				g_pModuleMessenger->MessengerSendFriendInviteUser( m_WorkThreadIdx, this, ui8InvitedUser);
			}
		}
		break; 
	case PROTOCOL_AUTH_SEND_WHISPER_REQ:				//081223
		{
			CHECKVALID_CHANNEL;
			
			INT16	i16messageSize;
			char	strreceiverNick[NET_NICK_NAME_SIZE];			
			char	strmessage[MAX_STRING_COUNT];

			pPacket->ReadData(strreceiverNick, NET_NICK_NAME_SIZE);

			pPacket->ReadData(&i16messageSize, sizeof(INT16));
			i16messageSize = MIN(i16messageSize, MAX_STRING_COUNT);
			pPacket->ReadData(strmessage, i16messageSize);
			strmessage[i16messageSize - 1] = 0;

			strreceiverNick[ NET_NICK_NAME_SIZE - 1 ]	= 0;

			if (_CheckDuplicateChat(strmessage, ZLOG_CHATTING_TYPE_WHISPER, PROTOCOL_AUTH_SEND_WHISPER_ACK) != CHAT_LIMIT_NORMAL)
			{
				// 채팅제한 상태라면 뿌려줄 필요 없으니 여기서 빠져나갑니다.
				break;
			}
			
#if defined( LOCALE_NORTHAMERICA ) || defined( LOCALE_RUSSIA )
			INT32 i32ChattingType = ZLOG_CHATTING_TYPE_WHISPER;
			g_pModuleContextNC->LogSendMessage( m_WorkThreadIdx, LOG_MESSAGE_U_CHATTING, this, &i32ChattingType, strmessage );
#endif
			g_pModuleMessenger->MessengerSendWhisper(	m_WorkThreadIdx, this, strreceiverNick, i16messageSize, strmessage);
		}
		break;
	case PROTOCOL_AUTH_SEND_WHISPER_FIND_UID_REQ:
		{

			CHECKVALID_CHANNEL;

			TUID	ReceiverUID;
			INT16	messageSize;		
			char	receiverNick[NET_NICK_NAME_SIZE];
			char	message[MAX_STRING_COUNT];
			
			i3mem::FillZero(receiverNick, NET_NICK_NAME_SIZE);

			pPacket->ReadData(&ReceiverUID, sizeof(TUID));			
			pPacket->ReadData(receiverNick, NET_NICK_NAME_SIZE);

			pPacket->ReadData(&messageSize, sizeof(INT16));
			messageSize = MIN(messageSize, MAX_STRING_COUNT);
			pPacket->ReadData(message, messageSize);
			message[messageSize - 1] = 0;

			if (_CheckDuplicateChat(message, ZLOG_CHATTING_TYPE_WHISPER, PROTOCOL_AUTH_SEND_WHISPER_ACK) != CHAT_LIMIT_NORMAL)
			{
				// 채팅제한 상태라면 뿌려줄 필요 없으니 여기서 빠져나갑니다.
				break;
			}

			g_pModuleMessenger->MessengerSendWhisperByUID( m_WorkThreadIdx, this, ReceiverUID, receiverNick, messageSize, message);			
		}
		break;
	case PROTOCOL_AUTH_FIND_USER_REQ:				//081224
		{
			CHECKVALID_CHANNEL;
			char strfindNick[NET_NICK_NAME_SIZE];
			pPacket->ReadData(strfindNick, NET_NICK_NAME_SIZE);
			strfindNick[ NET_NICK_NAME_SIZE - 1 ]	= 0;

		#ifdef USE_MESS_FIND
			// Send Messenger Server		
			g_pModuleMessenger->MessengerFindUser(m_WorkThreadIdx, this, strfindNick);
		#else
			#ifdef USE_TRANS_FIND
				// Send Trans Server
				g_pModuleContext->AuthSendFindNickName(m_WorkThreadIdx, this, strfindNick);
			#endif			
		#endif
		}
		break;	
#if defined( USE_BLOCK_LIST )
	case PROTOCOL_AUTH_BLOCK_INSERT_REQ	:			// 차단 유저 추가 요청
		{
			CHECKVALID_CHANNEL;
			INT32 i32BlockSessionIdx = -1;			// 차단할 유저의 세션 인덱스
			UINT8 ui8CommentSize = 0;
			char strComment[BLOCK_COMMENT_MAX];		// 차단 사유
			

			pPacket->ReadData(&i32BlockSessionIdx, sizeof(INT32));
			
			CUserSession *	pSession = g_pUserManager->GetSession( i32BlockSessionIdx );

			if( NULL == pSession )	SendBlockInsertResult( EVENT_ERROR_EVENT_BLOCK_PARAMETER_USER ); // 차단 하려는 유저가 정상(접속중)인지 체크
			else if( m_i64UID == pSession->GetUID() ) SendBlockInsertResult( EVENT_ERROR_EVENT_BLOCK_MY_NICKNAME ); // 자기 자신을 차단 했는지 체크
			else if( TRUE == _CheckIsSameClan( i32BlockSessionIdx ) ) SendBlockInsertResult( EVENT_ERROR_EVENT_BLOCK_INSERT_FAIL_CLAN );	// 두 유저가 동일한 클랜 원인지 체크.
			else
			{
				pPacket->ReadData(&ui8CommentSize, sizeof(UINT8));
				ui8CommentSize = MIN(ui8CommentSize, BLOCK_COMMENT_MAX - 1 );	
				pPacket->ReadData(strComment, sizeof(char) * ui8CommentSize);
				strComment[BLOCK_COMMENT_MAX - 1] = 0;

				if ( FALSE == g_pModuleMessenger->MessengerSendBlockInsert( m_WorkThreadIdx, this, (UINT8)pSession->m_UserInfoBasic._rank, pSession->m_UserInfoBasic._nick, strComment) )
				{ 
					SendBlockInsertResult( EVENT_ERROR_EVENT_BLOCK_UNKOWN );	
				} 
			}
		}
		break; 


	case PROTOCOL_AUTH_BLOCK_DELETE_REQ	:		// 차단 유저 삭제 요청
		{
			CHECKVALID_CHANNEL;
			INT32 i32Idx; 			
			pPacket->ReadData(&i32Idx, sizeof(INT32));
			g_pModuleMessenger->MessengerSendBlockDelete( m_WorkThreadIdx, this, i32Idx );				
		}
		break; 
#endif

	case PROTOCOL_MESSENGER_NOTE_SEND_REQ:			//081224
		{
			CHECKVALID_CHANNEL;

			UINT8 ui8nicknameSize;
			UINT8 ui8messageSize;
			char strnickname[NET_NICK_NAME_SIZE];
			char strmessage[NET_NOTE_SIZE];
            
			pPacket->ReadData(&ui8nicknameSize,	sizeof(UINT8));
			pPacket->ReadData(&ui8messageSize,		sizeof(UINT8));
			
			ui8nicknameSize	= MIN(ui8nicknameSize, NET_NICK_NAME_SIZE);
			ui8messageSize		= MIN(ui8messageSize,	NET_NOTE_SIZE);

			if (0 < ui8nicknameSize && 0 < ui8messageSize)
			{
				pPacket->ReadData(strnickname,	ui8nicknameSize);				
				pPacket->ReadData(strmessage,	ui8messageSize);			

				strnickname[ui8nicknameSize - 1]	= 0;
				strmessage[ui8messageSize - 1]	= 0;

				g_pModuleMessenger->MessengerSendNote(m_WorkThreadIdx, this, ui8nicknameSize, ui8messageSize, strnickname, strmessage);
			}
		}
		break;
	case PROTOCOL_MESSENGER_NOTE_SEND_FIND_UID_REQ:
		{
			CHECKVALID_CHANNEL;

			TUID	ReceiverUID;
			UINT8	ui8messageSize;			
			char	strmessage[NET_NOTE_SIZE];

			pPacket->ReadData(&ReceiverUID,		sizeof(TUID));
			pPacket->ReadData(&ui8messageSize,	sizeof(UINT8));			
			ui8messageSize		= MIN(ui8messageSize,	NET_NOTE_SIZE);

			if (0 < ReceiverUID && 0 < ui8messageSize)
			{
				pPacket->ReadData(strmessage,	ui8messageSize);			
				strmessage[ui8messageSize - 1]	= 0;

				g_pModuleMessenger->MessengerSendNoteByUID(m_WorkThreadIdx, this, ReceiverUID, ui8messageSize, strmessage);
			}		
		}
		break;
	//case PROTOCOL_MESSENGER_CLAN_NOTE_SEND_REQ:
	//	{
	//		CHECKVALID_CHANNEL;

	//		CLAN_NOTE ClanNote;
	//		char szMessage[ NET_NOTE_SIZE ];

	//		pPacket->ReadData( &ClanNote,	sizeof(CLAN_NOTE) );
	//		ClanNote._MessageSize = MIN( ClanNote._MessageSize, NET_NOTE_SIZE );

	//		pPacket->ReadData( szMessage,	ClanNote._MessageSize );

	//		g_pModuleMessenger->MessengerSendClanNote( m_WorkThreadIdx, this, ClanNote._DBIdx, m_UserInfoBasic._clanidx, ClanNote._IsAsk, ClanNote._MessageSize, szMessage );
	//	}
	//	break;
	case PROTOCOL_MESSENGER_NOTE_CHECK_READED_REQ:	//081224
		{
			CHECKVALID_CHANNEL;

			UINT8 ui8readCount;
			INT32 ai32noteIdList[MAX_NORMAL_NOTE_COUNT];

			pPacket->ReadData(&ui8readCount, sizeof(UINT8));
			
			ui8readCount = MIN(ui8readCount, MAX_NORMAL_NOTE_COUNT);

			if (0 < ui8readCount)
			{
				pPacket->ReadData(ai32noteIdList, sizeof(INT32) * ui8readCount);
				g_pModuleMessenger->MessengerCheckReadedNote(m_WorkThreadIdx, this, ui8readCount, ai32noteIdList);
			}
		}
		break;
	case PROTOCOL_MESSENGER_NOTE_DELETE_REQ:		//081224
		{
			CHECKVALID_CHANNEL;

			UINT8 ui8deleteCount;
			INT32 ai32noteIdList[MAX_NORMAL_NOTE_COUNT];

			pPacket->ReadData(&ui8deleteCount, sizeof(UINT8));

			ui8deleteCount = MIN(ui8deleteCount, MAX_NORMAL_NOTE_COUNT);	

			if (0 < ui8deleteCount)
			{
				pPacket->ReadData(ai32noteIdList, sizeof(INT32) * ui8deleteCount);
				g_pModuleMessenger->MessengerDeleteNote(m_WorkThreadIdx, this, ui8deleteCount, ai32noteIdList);
			}
		}
		break;

	default:
			bisKilled		= TRUE; 			
			m_ui32KillReason	= KILL_REASON_UNKNOWN_PACKET;
			I3TRACE( "UnKnown Packet - Kill User");
			break;
	}

	return bSendMessage;
}