#include "pch.h"
#include "TaskProcessor.h"
#include "PacketLocker.h"
#include "Gateway.h"
#include "DataBase.h"
#include "Receiver.h"
#include "ConnectedUser.h"
#include "NoteBox.h"

void CTaskProcessor::_DoTaskForGameDataBase(UINT32 ui32ServerIdx)
{
	i3NetworkPacket packet;
	packet.CopyToBuffer((char*)m_pui8RecvBuffer, m_ui32ReadedLength);

	switch( packet.GetProtocolID() )
	{
	case PROTOCOL_BASE_HEART_BIT_REQ		:
		{
			i3NetworkPacket	SendPacket(PROTOCOL_BASE_HEART_BIT_ACK);
			SendPacketMessage(ui32ServerIdx, &SendPacket);
		}
		break;
	case PROTOCOL_MESSENGER_LOGIN_REQ:
		{
			USER_NODE node;
			OGame2Mess_Login_Req	stGame2Mess_Login_Req;
			MakePacket_OGame2Mess_Login_Req(&stGame2Mess_Login_Req, &packet);

			if(0 > stGame2Mess_Login_Req.iUID)
			{
				g_pLog->WriteLog( L"[EMERGENCY ERROR]PROTOCOL_MESSENGER_LOGIN_REQ FAIL(UID:%I64d,SIDX:%d,NICK:%s,RANK:%d)", stGame2Mess_Login_Req.iUID, stGame2Mess_Login_Req.iSessionIdx, stGame2Mess_Login_Req.pNickName, stGame2Mess_Login_Req.iRank  );
				return;
			}

			if( m_pUserFinder->FindUid(stGame2Mess_Login_Req.iUID, &node) )
			{
				_RemoveUser(node._i32serveridx, stGame2Mess_Login_Req.iUID); 
				g_pLog->WriteLog( L"Re LogIn ServerIdx = %d SessionIdx = %d UID = %I64d ", ui32ServerIdx, stGame2Mess_Login_Req.iSessionIdx, stGame2Mess_Login_Req.iUID  );
			}

			// 닉네임을 입력하여 주지 않는 경우- 에러임
			// 유저 최초 생성시에도 같은 경우가 발생.
			// GameServer의 PROTOCOL_LOBBY_CREATE_NICK_NAME_ACK 시에 재전송됨.
			if (0 == i3String::Compare(stGame2Mess_Login_Req.pNickName,_TT("")))
			{
				break;
			}

			// 정상적인 경우 메신저 서버에 저장
			if (-1 == _AddUser(ui32ServerIdx, stGame2Mess_Login_Req.iSessionIdx, stGame2Mess_Login_Req.iUID, stGame2Mess_Login_Req.iRank, stGame2Mess_Login_Req.pNickName ))
			{
				g_pLog->WriteLog( L"[ERROR] PROTOCOL_MESSENGER_LOGIN_REQ::_AddUser() - Save Fail"  );
			}
			//else
			//{
			//테스트 후 삭제
			//g_pLog->WriteLog(m_i32TaskIdx,"PROTOCOL_MESSENGER_LOGIN_REQ : _AddUser[Uid(%I64d), Nick(%s)]", stGame2Mess_Login_Req.iUID, stGame2Mess_Login_Req.pNickName);
			//}
		}
		break;
	case PROTOCOL_MESSENGER_LOGOUT_REQ:
		{
			OGame2Mess_Logout_Req	stGame2Mess_Logout_Req;
			MakePacket_OGame2Mess_Logout_Req(&stGame2Mess_Logout_Req, &packet);

			_RemoveUser( ui32ServerIdx, stGame2Mess_Logout_Req.iUID );
		}
		break;
	case PROTOCOL_MESSENGER_KICKUSER_REQ:				//메신저는 이 패킷 필요없을듯. 
		{
			OGame2Mess_KickUser_Req		stGame2Mess_KickUser_Req;
			MakePacket_OGame2Mess_KickUser_Req(&stGame2Mess_KickUser_Req, &packet);

			_RemoveUser( ui32ServerIdx, stGame2Mess_KickUser_Req.iUID ); 
		}
		break;
	case PROTOCOL_MESSENGER_ENTER_USER_REQ:
		{
			// Community
			USER_NODE	node;
			OGame2Mess_Enter_User_Req	stGame2Mess_Enter_User_Req;
			MakePacket_OGame2Mess_Enter_User_Req(&stGame2Mess_Enter_User_Req, &packet);

			if( !m_pUserFinder->FindUid(stGame2Mess_Enter_User_Req.iUID, &node) )
			{
				//g_pLog->WriteLog(m_i32TaskIdx, "UserEnterError - UserFinder Find. Uid = %I64d", iUID);
				break; 
			}

			node._i32serveridx		= ui32ServerIdx;
			node._i32sessionidx		= stGame2Mess_Enter_User_Req.iSessionIdx;
			m_pUserFinder->UpdateByUid(&node);
			m_pUserFinder->ChangeState(stGame2Mess_Enter_User_Req.iUID, MAKE_FRIEND_STATE(FRIEND_STATE_ONLINE, ui32ServerIdx, 0, 0));

			// 적용된 내용을 친구들에게 전송
			_ProcessFriendInfo(stGame2Mess_Enter_User_Req.iUID);

#ifdef DEF_TEST_LOG
			if(FRIEND_MAX_COUNT < node._friendCount || RANK_MAX < node._iRank || MAX_CONNECT_USER_COUNT < node._idxBuffer)
			{
				I3BCLOG(LOCATION, "CTaskProcessor::OnRunning()[UID(%d)][_iRank(%d)][_idxBuffer(%d)][FriendCnt(%d]",
					node._iDbIdx, node._iRank, node._idxBuffer, node._friendCount);	
			}
#endif

#if defined( USE_BLOCK_LIST )
			i3NetworkPacket	SendPacket(PROTOCOL_MESSENGER_ENTER_USER_ACK);
			MakePacket_OGame2Mess_Enter_User_Ack(&SendPacket,
				stGame2Mess_Enter_User_Req.iSessionIdx,
				stGame2Mess_Enter_User_Req.iUID,
				node._friendCount,
				node._ui32BlockCount
				);
#else
			i3NetworkPacket	SendPacket(PROTOCOL_MESSENGER_ENTER_USER_ACK);
			MakePacket_OGame2Mess_Enter_User_Ack(&SendPacket,
				stGame2Mess_Enter_User_Req.iSessionIdx,
				stGame2Mess_Enter_User_Req.iUID,
				node._ui32friendCount );
#endif

			// Send Game Server
			SendPacketMessage(ui32ServerIdx, &SendPacket);
		}
		break;
	case PROTOCOL_MESSENGER_LEAVE_USER_REQ:
		{
			OGame2Mess_Leave_User_Req	stGame2Mess_Leave_User_Req;
			MakePacket_OGame2Mess_Leave_User_Req(&stGame2Mess_Leave_User_Req, &packet);

			//아래는 다른 솔루션을 찾는 방법으로 처리 합니다. 
			/*
			// Community
			USER_NODE	node;
			if (m_pUserFinder->FindUid(userId, &node))
			{
			m_pUserFinder->UpdateByUid(&node);
			m_pCommunity->ChangeState(userId, MAKE_FRIEND_STATE(FRIEND_STATE_OFFLINE, iServerIdx, 0, 0));
			}
			else
			{
			g_pLog->WriteLog(m_i32TaskIdx, "UserLeaveError - UserFinder Find. Uid = %d", userId);
			}
			*/

		}
		break;	
	case PROTOCOL_MESSENGER_CHANGE_NICKNAME_REQ:
		{
			USER_NODE	node;
			OGame2Mess_Change_NickName_Req	stReq;
			

			stReq.eWorkType = FRIEND_CHANGE_NICK;

			packet.ReadData( &stReq.iSessionIdx,	sizeof( INT32 ) );
			packet.ReadData( &stReq.iUID,			sizeof( T_UID ) );
			packet.ReadData( stReq.nickname,		sizeof(TTCHAR) * NET_NICK_NAME_SIZE );
			stReq.nickname[NET_NICK_NAME_SIZE-1] = '\0';
			
			// UserFinder에서의 NickName변경			
			if( !m_pUserFinder->FindUid(stReq.iUID, &node) )
			{
				// 최초 생성 유저만 들어옴. 로그 필요성 없음.
				//g_pLog->WriteLog(m_i32TaskIdx,"Change NickName Error - UserFinder Find. Uid(%I64d), Nick(%s)", stGame2Mess_Change_NickName_Req.iUID, stGame2Mess_Change_NickName_Req.nickname);
				break; 
			}

			stReq.ui8NickColor = node._ui8NickColor;
			// UserFinder에 적용
#ifdef USE_CHANGE_NICK
#ifdef USE_HASH_NICK
			m_pUserFinder->UpdateNickName(stGame2Mess_Change_NickName_Req.iUID, stGame2Mess_Change_NickName_Req.nickname); 			
			// 적용된 내용을 친구들에게 전송
			_ProcessFriendInfo(stGame2Mess_Change_NickName_Req.iUID);
#else
			//g_CurrentUpdateUser++;
			// 변경된 자신의 닉네임 저장
			
			m_pUserFinder->UpdateForNickName(stReq.iUID, stReq.nickname);
			_InChangeNick(&stReq);
#endif
#endif
		}
		break;
	case PROTOCOL_MESSENGER_CHANGE_RANK_REQ:
		{
			OGame2Mess_Change_Rank_Req	stGame2Mess_Change_Rank_Req;
			MakePacket_OGame2Mess_Change_Rank_Req(&stGame2Mess_Change_Rank_Req, &packet);

			// UserFinder에 적용
#ifdef USE_CHANGE_RANK
#ifdef USE_HASH_NICK
			m_pUserFinder->UpdateRank(stGame2Mess_Change_Rank_Req.iUID, stGame2Mess_Change_Rank_Req.iRank); 			
			// 적용된 내용을 친구들에게 전송
			_ProcessFriendInfo(stGame2Mess_Change_Rank_Req.iUID);
#else
			// 변경된 자신의 계급 저장
			m_pUserFinder->UpdateRank(stGame2Mess_Change_Rank_Req.iUID, stGame2Mess_Change_Rank_Req.iRank);
			_InChangeRank(stGame2Mess_Change_Rank_Req);
#endif
#endif
		}
		break;
	case PROTOCOL_ALIVE_USER_REQ :
		{
			OGame2Mess_Alive_User_Req	stGame2Mess_Alive_User_Req;
			MakePacket_OGame2Mess_Alive_User_Req(&stGame2Mess_Alive_User_Req, &packet);

			if( !m_pUserFinder->UpdateAlivePacket(stGame2Mess_Alive_User_Req.iUID, stGame2Mess_Alive_User_Req.iSessionIdx, ui32ServerIdx) )
			{
				; // Skip
				//g_pLog->WriteLog(m_i32TaskIdx,"Alive User Error - UserFinder Find. Uid = %I64d", stGame2Mess_Alive_User_Req.iUID);                

				////추후 닉네임은 넣어주어야 합니다.
				////닉네임은 넣어주어야 합니다. 				
				/*#ifdef USE_ALIVE_USER
				_AddUser(iServerIdx, stGame2Mess_Alive_User_Req.iSessionIdx, stGame2Mess_Alive_User_Req.iUID, stGame2Mess_Alive_User_Req.pNickName);
				#endif*/
			}
		}
		break; 	
	case PROTOCOL_MESSENGER_SEND_WHISPER_REQ:
		{
			INT32	i32result = EVENT_ERROR_FAIL;

			INT64	i64ReceiverUID;
			INT32	i32ReceiverServerIdx;
			INT32	i32ReceiverSessionIdx;

			OGame2Mess_Send_Whisper_Req		stGame2Mess_Send_Whisper_Req;
			MakePacket_OGame2Mess_Send_Whisper_Req(&stGame2Mess_Send_Whisper_Req, &packet);

			// 닉네임으로 uid를 찾는다.			
			if( m_pUserFinder->GetUserInfoByNick( stGame2Mess_Send_Whisper_Req.receiverNick, &i64ReceiverUID, &i32ReceiverSessionIdx, &i32ReceiverServerIdx) )
			{
#if defined( USE_BLOCK_LIST )
				// 메시지 받는 사람의 차단 리스트에 보내는 사람이 등록되어있는지 체크
				BOOL bIsBlock = _CheckBlock( stGame2Mess_Send_Whisper_Req.iUID, i64ReceiverUID );

				if( FALSE == bIsBlock )
				{
					i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_RECV_WHISPER_ACK);
					MakePacket_OGame2Mess_Recv_Whisper_Ack(&sendPacket, 
						i32ReceiverSessionIdx,
						i64ReceiverUID,
						stGame2Mess_Send_Whisper_Req.senderNick,
						stGame2Mess_Send_Whisper_Req.messageSize,
						stGame2Mess_Send_Whisper_Req.message);
					// Send Game Server
					SendPacketMessage(i32ReceiverServerIdx, &sendPacket);
				}
				// else // 차단 되었다면 무시한다. 별도의 ACK를 전송하지 않는다.
#else

				i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_RECV_WHISPER_ACK);
				MakePacket_OGame2Mess_Recv_Whisper_Ack(&sendPacket, 
					i32ReceiverSessionIdx,
					i64ReceiverUID,
					stGame2Mess_Send_Whisper_Req.senderNick,
					stGame2Mess_Send_Whisper_Req.bIsGMUser,
					stGame2Mess_Send_Whisper_Req.messageSize,
					stGame2Mess_Send_Whisper_Req.message,
					stGame2Mess_Send_Whisper_Req.ui8NickColor);
				// Send Game Server
				SendPacketMessage(i32ReceiverServerIdx, &sendPacket);
#endif
			}
			else
			{
				i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_SEND_WHISPER_ACK);
				MakePacket_OGame2Mess_Send_Whisper_Ack(&sendPacket, 
					stGame2Mess_Send_Whisper_Req.iSessionIdx,
					stGame2Mess_Send_Whisper_Req.iUID,
					i32result,
					stGame2Mess_Send_Whisper_Req.receiverNick,
					stGame2Mess_Send_Whisper_Req.ui8NickColor);
				// Send Game Server
				SendPacketMessage(ui32ServerIdx, &sendPacket);
			}
		}
		break;
	case PROTOCOL_MESSENGER_SEND_WHISPER_BY_UID_REQ:
		{
			INT32 i32result = EVENT_ERROR_FAIL;

			INT32	i32ReceiverServerIdx;
			INT32	i32ReceiverSessionIdx;
			TTCHAR	strReceiverNick[NET_NICK_NAME_SIZE];
			i3mem::FillZero(strReceiverNick, sizeof(TTCHAR) * NET_NICK_NAME_SIZE);

			OGame2Mess_Send_Whisper_By_UID_Req		stGame2Mess_Send_Whisper_By_UID_Req;
			MakePacket_OGame2Mess_Send_Whisper_By_UID_Req(&stGame2Mess_Send_Whisper_By_UID_Req, &packet);

			// Receiver uid가 유효한지 확인한다.
			if( m_pUserFinder->GetCurrentUserInfoByUID( stGame2Mess_Send_Whisper_By_UID_Req.iReceiverUID, &i32ReceiverSessionIdx, &i32ReceiverServerIdx, strReceiverNick) )
			{
#if defined( USE_BLOCK_LIST )
				// 메시지 받는 사람의 차단 리스트에 보내는 사람이 등록되어있는지 체크
				BOOL bIsBlock = _CheckBlock( stGame2Mess_Send_Whisper_By_UID_Req.iUID, stGame2Mess_Send_Whisper_By_UID_Req.iReceiverUID );

				if( FALSE == bIsBlock )
				{
					i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_RECV_WHISPER_ACK);
					MakePacket_OGame2Mess_Recv_Whisper_Ack(&sendPacket, 
						i32ReceiverSessionIdx,
						stGame2Mess_Send_Whisper_By_UID_Req.iReceiverUID,
						stGame2Mess_Send_Whisper_By_UID_Req.senderNick,
						stGame2Mess_Send_Whisper_By_UID_Req.messageSize,
						stGame2Mess_Send_Whisper_By_UID_Req.message);
					// Send Game Server
					SendPacketMessage((UINT32)i32ReceiverServerIdx, &sendPacket);
				}
				// else // 차단 되었다면 무시한다. 별도의 ACK를 전송하지 않는다.
#else
				i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_RECV_WHISPER_ACK);
				MakePacket_OGame2Mess_Recv_Whisper_Ack(&sendPacket, 
					i32ReceiverSessionIdx,
					stGame2Mess_Send_Whisper_By_UID_Req.iReceiverUID,
					stGame2Mess_Send_Whisper_By_UID_Req.senderNick,
					stGame2Mess_Send_Whisper_By_UID_Req.bIsGMUser,
					stGame2Mess_Send_Whisper_By_UID_Req.messageSize,
					stGame2Mess_Send_Whisper_By_UID_Req.message,
					stGame2Mess_Send_Whisper_By_UID_Req.ui8NickColor);
				// Send Game Server
				SendPacketMessage((UINT32)i32ReceiverServerIdx, &sendPacket);

#endif

			}
			else
			{
				i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_SEND_WHISPER_ACK);
				MakePacket_OGame2Mess_Send_Whisper_Ack(&sendPacket, 
					stGame2Mess_Send_Whisper_By_UID_Req.iSessionIdx,
					stGame2Mess_Send_Whisper_By_UID_Req.iUID,
					i32result,
					stGame2Mess_Send_Whisper_By_UID_Req.receiverNick,
					stGame2Mess_Send_Whisper_By_UID_Req.ui8NickColor);
				// Send Game Server
				SendPacketMessage(ui32ServerIdx, &sendPacket);
			}
		}
		break;
		////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////
		//쪽지 
		////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////

	case PROTOCOL_MESSENGER_CLAN_NOTE_SEND_REQ:
		{
			OGame2Mess_Clan_Note_Send_Req	stGame2Mess_Clan_Note_Send_Req;
			MakePacket_OGame2Mess_Clan_Note_Send_Req(&stGame2Mess_Clan_Note_Send_Req, &packet);

			// 금지어 검사(스페이스)
			BOOL bEcode = _FindTatooWord(stGame2Mess_Clan_Note_Send_Req.clanName);
			if (FALSE == bEcode)
			{
				INT32 i32result = EVENT_ERROR_NOTE_SEND_UKNOWN_NICK;
				i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_NOTE_SEND_ACK);
				MakePacket_OGame2Mess_Clan_Note_Send_Ack(&sendPacket,
					stGame2Mess_Clan_Note_Send_Req.iSessionIdx,
					stGame2Mess_Clan_Note_Send_Req.iUID,
					i32result);
				// Send Game Server
				g_pGateway->SendPacket(ui32ServerIdx,	&sendPacket);
				break;
			}

			// 발신 유저가 서버에서 유효한가를 검사
			if( !m_pUserFinder->IsAliveUser(stGame2Mess_Clan_Note_Send_Req.iUID) )
			{
				INT32 i32result = EVENT_ERROR_FAIL;
				i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_CLAN_NOTE_SEND_ACK);
				MakePacket_OGame2Mess_Clan_Note_Send_Ack(&sendPacket,
					stGame2Mess_Clan_Note_Send_Req.iSessionIdx,
					stGame2Mess_Clan_Note_Send_Req.iUID,
					i32result);
				// Send Game Server
				g_pGateway->SendPacket(ui32ServerIdx,	&sendPacket);
				break;
			}

			INT8 i8MessageType;
			i8MessageType = NOTE_COMMAND_TYPE_ADD_CLAN; 
			if (stGame2Mess_Clan_Note_Send_Req.isAskNote)i8MessageType = NOTE_COMMAND_TYPE_ADD_CLANASK; 

#ifdef ACTIVATE_NOTE_DATABASE
			if( !g_pDataBase->InNoteCommandAddClan(i8MessageType, stGame2Mess_Clan_Note_Send_Req) )
				//if( !g_pDataBase->InNoteCommandAddClan(iMessageType, iUID, receiverUserId, clanName, message, clanNameSize, messageSize, clanIdx) )
#endif 
			{
				INT32 i32result = EVENT_ERROR_FAIL;
				i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_CLAN_NOTE_SEND_ACK);
				MakePacket_OGame2Mess_Clan_Note_Send_Ack(&sendPacket,
					stGame2Mess_Clan_Note_Send_Req.iSessionIdx,
					stGame2Mess_Clan_Note_Send_Req.iUID,
					i32result);
				// Send Game Server
				g_pGateway->SendPacket(ui32ServerIdx,	&sendPacket);
			}
		}
		break;	
	case PROTOCOL_MESSENGER_CLAN_GROUP_NOTE_SEND_REQ:
		{
			INT32 i32result = EVENT_ERROR_FAIL;
			OGame2Mess_Clan_Group_Note_Send_Req	stGame2Mess_Clan_Group_Note_Send_Req;
			MakePacket_OGame2Mess_Clan_Group_Note_Send_Req(&stGame2Mess_Clan_Group_Note_Send_Req, &packet);

			// 금지어 검사(스페이스)
			BOOL bEcode = _FindTatooWord(stGame2Mess_Clan_Group_Note_Send_Req.clanName);
			if (FALSE == bEcode)
			{
				i32result = EVENT_ERROR_NOTE_SEND_UKNOWN_NICK;
				i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_NOTE_SEND_ACK);
				MakePacket_OGame2Mess_Clan_Group_Note_Send_Ack(&sendPacket,
					stGame2Mess_Clan_Group_Note_Send_Req.iSessionIdx,
					stGame2Mess_Clan_Group_Note_Send_Req.iUID,
					i32result);
				// Send Game Server
				g_pGateway->SendPacket(ui32ServerIdx,	&sendPacket);
				break; 
			}

			// 발신 유저가 서버에서 유효한가를 검사
			if( !m_pUserFinder->IsAliveUser(stGame2Mess_Clan_Group_Note_Send_Req.iUID) )
			{	
				i32result = EVENT_ERROR_FAIL;
				i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_CLAN_NOTE_SEND_ACK);
				MakePacket_OGame2Mess_Clan_Group_Note_Send_Ack(&sendPacket,
					stGame2Mess_Clan_Group_Note_Send_Req.iSessionIdx,
					stGame2Mess_Clan_Group_Note_Send_Req.iUID,
					i32result);
				// Send Game Server
				g_pGateway->SendPacket(ui32ServerIdx,	&sendPacket);
				break;
			}

			INT8 i8MessageType;
			i8MessageType = NOTE_COMMAND_TYPE_ADD_CLAN; 
			if (stGame2Mess_Clan_Group_Note_Send_Req.isAskNote)i8MessageType = NOTE_COMMAND_TYPE_ADD_CLANASK; 


			//#ifdef ACTIVATE_NOTE_DATABASE
			//			if( !g_pDataBase->InNoteCommandAddClan(iMessageType, stGame2Mess_Clan_Group_Note_Send_Req) )
			//				//if( !g_pDataBase->InNoteCommandAddClan(iMessageType, iUID, receiverUserId, clanName, message, clanNameSize, messageSize, clanIdx) )
			//#endif 
			// 버퍼에 작업을 설정하여 스레드 타임에 순차적으로 처리(nonsync)
			if (TRUE == _InClanGroupNoteAddClan(i8MessageType, stGame2Mess_Clan_Group_Note_Send_Req))
			{
				i32result = EVENT_ERROR_SUCCESS;
			}

			//if (EVENT_ERROR_SUCCESS != result)
			//{
			//	result = EVENT_ERROR_FAIL;
			i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_CLAN_NOTE_SEND_ACK);
			MakePacket_OGame2Mess_Clan_Group_Note_Send_Ack(&sendPacket,
				stGame2Mess_Clan_Group_Note_Send_Req.iSessionIdx,
				stGame2Mess_Clan_Group_Note_Send_Req.iUID,
				i32result);
			// Send Game Server
			g_pGateway->SendPacket(ui32ServerIdx,	&sendPacket);
			//}
		}
		break;
	case PROTOCOL_MESSENGER_NOTE_SEND_REQ:
		{
			OGame2Mess_Note_Send_Req	stGame2Mess_Note_Send_Req;
			MakePacket_OGame2Mess_Note_Send_Req(&stGame2Mess_Note_Send_Req, &packet);

			// 금지어 검사(스페이스)
			BOOL bEcode = _FindTatooWord(stGame2Mess_Note_Send_Req.nickname);
			if (FALSE == bEcode)
			{
				INT32 result = EVENT_ERROR_NOTE_SEND_UKNOWN_NICK;
				i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_NOTE_SEND_ACK);
				MakePacket_OGame2Mess_Note_Send_Ack(&sendPacket,
					stGame2Mess_Note_Send_Req.iSessionIdx,
					stGame2Mess_Note_Send_Req.iUID,
					result);
				g_pGateway->SendPacket(ui32ServerIdx,	&sendPacket);
				break;
			}

			USER_NODE node;	//이 녀석 필요함 
			if( !m_pUserFinder->FindUid(stGame2Mess_Note_Send_Req.iUID, &node) )
			{
				INT32 i32result = EVENT_ERROR_FAIL;
				i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_NOTE_SEND_ACK);
				MakePacket_OGame2Mess_Note_Send_Ack(&sendPacket,
					stGame2Mess_Note_Send_Req.iSessionIdx,
					stGame2Mess_Note_Send_Req.iUID,
					i32result);
				g_pGateway->SendPacket(ui32ServerIdx,	&sendPacket);
				break;
			}

#ifdef ACTIVATE_NOTE_DATABASE
			if( !g_pDataBase->InNoteCommandAdd(NOTE_COMMAND_TYPE_ADD_NORMAL, node._strnick, stGame2Mess_Note_Send_Req) )

#endif
			{
				INT32 i32result = EVENT_ERROR_FAIL;
				i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_NOTE_SEND_ACK);
				MakePacket_OGame2Mess_Note_Send_Ack(&sendPacket,
					stGame2Mess_Note_Send_Req.iSessionIdx,
					stGame2Mess_Note_Send_Req.iUID,
					i32result);
				g_pGateway->SendPacket(ui32ServerIdx,	&sendPacket);
			}

		}
		break;
	case PROTOCOL_MESSENGER_NOTE_SEND_FIND_UID_REQ:
		{
			OGame2Mess_Note_Send_Find_UID_Req	stGame2Mess_Note_Send_Find_UID_Req;
			MakePacket_OGame2Mess_Note_Send_Find_UID_Req(&stGame2Mess_Note_Send_Find_UID_Req, &packet);

			USER_NODE node;	//이 녀석 필요함 
			if( !m_pUserFinder->FindUid(stGame2Mess_Note_Send_Find_UID_Req.iUID, &node) )
			{
				INT32 result = EVENT_ERROR_FAIL;
				i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_NOTE_SEND_ACK);
				MakePacket_OGame2Mess_Note_Send_Ack(&sendPacket,
					stGame2Mess_Note_Send_Find_UID_Req.iSessionIdx,
					stGame2Mess_Note_Send_Find_UID_Req.iUID,
					result);
				g_pGateway->SendPacket(ui32ServerIdx,	&sendPacket);
				break;
			}
#ifdef ACTIVATE_NOTE_DATABASE
			if( !g_pDataBase->InNoteCommandAddFindUID(NOTE_COMMAND_TYPE_ADD_NORMAL, node._strnick, stGame2Mess_Note_Send_Find_UID_Req) )
				//if( !g_pDataBase->InNoteCommandAdd(NOTE_COMMAND_TYPE_ADD_NORMAL, iUID, node._nick, nickname, message, messageSize) )
#endif
			{
				INT32 result = EVENT_ERROR_FAIL;
				i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_NOTE_SEND_ACK);
				MakePacket_OGame2Mess_Note_Send_Ack(&sendPacket,
					stGame2Mess_Note_Send_Find_UID_Req.iSessionIdx,
					stGame2Mess_Note_Send_Find_UID_Req.iUID,
					result);
				g_pGateway->SendPacket(ui32ServerIdx,	&sendPacket);
			}
		}
		break;
	case PROTOCOL_MESSENGER_NOTE_CHECK_READED_REQ				:
		{
			OGame2Mess_Note_Check_Readed_Req	stGame2Mess_Note_Check_Readed_Req;
			MakePacket_OGame2Mess_Note_Check_Readed_Req(&stGame2Mess_Note_Check_Readed_Req, &packet);

			if( !m_pUserFinder->IsAliveUser(stGame2Mess_Note_Check_Readed_Req.iUID) )
			{
				//일단 이렇게 처리한다. 리턴값에 에러코드가 없습니다. 
				break; 
			}
#ifdef ACTIVATE_NOTE_DATABASE
			if(!g_pDataBase->InNoteCommand( NOTE_COMMAND_TYPE_READ, stGame2Mess_Note_Check_Readed_Req.iUID, stGame2Mess_Note_Check_Readed_Req.iCount, stGame2Mess_Note_Check_Readed_Req.pList) )
#endif
			{
				stGame2Mess_Note_Check_Readed_Req.iCount = 0; 
				i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_NOTE_CHECK_READED_ACK);
				MakePacket_OGame2Mess_Note_Check_Readed_Ack(&sendPacket,
					stGame2Mess_Note_Check_Readed_Req.iSessionIdx,
					stGame2Mess_Note_Check_Readed_Req.iUID,
					stGame2Mess_Note_Check_Readed_Req.iCount,
					stGame2Mess_Note_Check_Readed_Req.pList);
				// Send Gateway Server
				g_pGateway->SendPacket(ui32ServerIdx, &sendPacket);
			}
		}
		break;
	case PROTOCOL_MESSENGER_NOTE_DELETE_REQ:
		{
			INT32	i32Result = EVENT_ERROR_FAIL;
			OGame2Mess_Note_Delete_Req	stGame2Mess_Note_Delete_Req;
			MakePacket_OGame2Mess_Note_Delete_Req(&stGame2Mess_Note_Delete_Req, &packet);

			if( !m_pUserFinder->IsAliveUser(stGame2Mess_Note_Delete_Req.iUID) )
			{
				//일단 이렇게 처리한다. 리턴값에 에러코드가 없습니다. 
				break;
			}
#ifdef ACTIVATE_NOTE_DATABASE
			if(!g_pDataBase->InNoteCommand( NOTE_COMMAND_TYPE_DEL, stGame2Mess_Note_Delete_Req.iUID, stGame2Mess_Note_Delete_Req.iCount, stGame2Mess_Note_Delete_Req.pList) )
#endif 
			{
				stGame2Mess_Note_Delete_Req.iCount = 0; 
				i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_NOTE_DELETE_ACK);
				MakePacket_OGame2Mess_Note_Delete_Ack(&sendPacket,
					stGame2Mess_Note_Delete_Req.iSessionIdx,
					stGame2Mess_Note_Delete_Req.iUID,
					i32Result,
					stGame2Mess_Note_Delete_Req.iCount,
					stGame2Mess_Note_Delete_Req.pList);
				// Send Gateway Server
				g_pGateway->SendPacket(ui32ServerIdx, &sendPacket);
			}
		}
		break;
		////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////
		//친구
		////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////
	case PROTOCOL_MESSENGER_FRIEND_ACCEPT_REQ:
		{
			INT32 i32result = EVENT_ERROR_SUCCESS;
			IN_FRIEND_CONTROL inData;
			USER_NODE node;

			OGame2Mess_Friend_Accept_Req		stGame2Mess_Friend_Accept_Req;
			MakePacket_OGame2Mess_Friend_Accept_Req(&stGame2Mess_Friend_Accept_Req, &packet);

			if (FALSE == m_pUserFinder->FindUid(stGame2Mess_Friend_Accept_Req.iUID, &node))
			{
				i32result = EVENT_ERROR_EVENT_FRIEND_ACCEPT_PARAMETER_USER;
			}
			else if (stGame2Mess_Friend_Accept_Req.iFriendIdx >= node._ui32friendCount)
			{
				i32result = EVENT_ERROR_EVENT_FRIEND_ACCEPT_PARAMETER_IDX;
			}

			if (EV_FAILED(i32result))
			{
				i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_FRIEND_ACCEPT_ACK);
				MakePacket_OGame2Mess_Friend_Accept_Ack(&sendPacket,
					stGame2Mess_Friend_Accept_Req.iSessionIdx,
					stGame2Mess_Friend_Accept_Req.iUID,
					i32result);
				// Send Game Server
				SendPacketMessage(ui32ServerIdx, &sendPacket);
			}
			else
			{
				inData.eWorkType	= FRIEND_CONTROL_AUTH;
				inData.i64uid		= stGame2Mess_Friend_Accept_Req.iUID;
				inData.i64fuid		= node._afriend[stGame2Mess_Friend_Accept_Req.iFriendIdx]._i64fuid;
				inData.idx			= stGame2Mess_Friend_Accept_Req.iFriendIdx;

#ifdef ACTIVATE_FRIEND_DATABASE
				if ( !g_pDataBase->InFriendControl( &inData))
#endif
				{
					// Failed.
					i32result = EVENT_ERROR_EVENT_FRIEND_ACCEPT_QUERY;

					i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_FRIEND_ACCEPT_ACK);
					MakePacket_OGame2Mess_Friend_Accept_Ack(&sendPacket,
						stGame2Mess_Friend_Accept_Req.iSessionIdx,
						stGame2Mess_Friend_Accept_Req.iUID,
						i32result);
					// Send Game Server
					SendPacketMessage(ui32ServerIdx, &sendPacket);
				}
			}
		}
		break;
	case PROTOCOL_MESSENGER_FRIEND_DELETE_REQ:
		{
			INT32 i32result = EVENT_ERROR_SUCCESS;
			USER_NODE node;
			IN_FRIEND_CONTROL inData;

			OGame2Mess_Friend_Delete_Req		stGame2Mess_Friend_Delete_Req;
			MakePacket_OGame2Mess_Friend_Delete_Req(&stGame2Mess_Friend_Delete_Req, &packet);

			if (FALSE == m_pUserFinder->FindUid(stGame2Mess_Friend_Delete_Req.iUID, &node))
			{
				i32result = EVENT_ERROR_EVENT_FRIEND_ACCEPT_PARAMETER_USER;
			}
			else if (stGame2Mess_Friend_Delete_Req.iFriendIdx >= node._ui32friendCount)
			{
				i32result = EVENT_ERROR_EVENT_FRIEND_ACCEPT_PARAMETER_IDX;
			}

			if (EV_FAILED(i32result))
			{
				i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_FRIEND_DELETE_ACK);
				MakePacket_OGame2Mess_Friend_Delete_Ack(&sendPacket,
					stGame2Mess_Friend_Delete_Req.iSessionIdx,
					stGame2Mess_Friend_Delete_Req.iUID,
					i32result);
				// Send Game Server
				SendPacketMessage(ui32ServerIdx, &sendPacket);
			}
			else
			{
				inData.eWorkType = FRIEND_CONTROL_DELETE;
				inData.i64uid = stGame2Mess_Friend_Delete_Req.iUID;
				inData.i64fuid = node._afriend[stGame2Mess_Friend_Delete_Req.iFriendIdx]._i64fuid;
				inData.idx = stGame2Mess_Friend_Delete_Req.iFriendIdx;
#ifdef ACTIVATE_FRIEND_DATABASE
				if ( !g_pDataBase->InFriendControl( &inData))
#endif
				{
					// Failed.
					i32result = EVENT_ERROR_EVENT_FRIEND_ACCEPT_QUERY;

					i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_FRIEND_DELETE_ACK);
					MakePacket_OGame2Mess_Friend_Delete_Ack(&sendPacket,
						stGame2Mess_Friend_Delete_Req.iSessionIdx,
						stGame2Mess_Friend_Delete_Req.iUID,
						i32result);
					// Send Game Server
					SendPacketMessage(ui32ServerIdx, &sendPacket);
				}
			}
		}
		break;
	case PROTOCOL_MESSENGER_FRIEND_INSERT_REQ:
		{
			INT32 i32result = EVENT_ERROR_SUCCESS;
			//INT64 i64fuid = 0;
			USER_NODE node;

			OGame2Mess_Friend_Insert_Req		stGame2Mess_Friend_Insert_Req;
			MakePacket_OGame2Mess_Friend_Insert_Req(&stGame2Mess_Friend_Insert_Req, &packet);

			// 금지어 검사(스페이스)
			BOOL bEcode = _FindTatooWord(stGame2Mess_Friend_Insert_Req.inData.strNick);
			if (FALSE == bEcode)
			{
				i32result = EVENT_ERROR_NOTE_SEND_UKNOWN_NICK;
			}
			else if (stGame2Mess_Friend_Insert_Req.iUID == 0)
			{
				i32result = EVENT_ERROR_EVENT_FRIEND_INSERT_PARAMETER_USER;
			}
			else if (FALSE == m_pUserFinder->FindUid(stGame2Mess_Friend_Insert_Req.iUID, &node))
			{
				i32result = EVENT_ERROR_EVENT_FRIEND_INSERT_PARAMETER_USER;
			}
			else if (node._ui32friendCount >= FRIEND_MAX_COUNT)
			{
				i32result = EVENT_ERROR_EVENT_FRIEND_INSERT_FULL;
			}
			else
			{
				for(UINT32 i = 0; i < node._ui32friendCount; i++)
				{
					if ( node._afriend[i]._i64fuid == 0 )	// 지원님 질문
					{
						i32result = EVENT_ERROR_EVENT_FRIEND_INSERT_PARAMETER_USER;
					}
				}
			}

			if (EV_FAILED(i32result))
			{
				// Failed
				i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_FRIEND_INSERT_ACK);
				MakePacket_OGame2Mess_Friend_Insert_Ack(&sendPacket,
					stGame2Mess_Friend_Insert_Req.iSessionIdx,
					stGame2Mess_Friend_Insert_Req.iUID,
					i32result);
				// Send Game Server
				SendPacketMessage(ui32ServerIdx, &sendPacket);
			}
			else
			{
				stGame2Mess_Friend_Insert_Req.inData.eWorkType = FRIEND_CONTROL_ADD;
				stGame2Mess_Friend_Insert_Req.inData.i64uid = stGame2Mess_Friend_Insert_Req.iUID;
				stGame2Mess_Friend_Insert_Req.inData.i64fuid = 0;
#ifdef ACTIVATE_FRIEND_DATABASE
				if ( !g_pDataBase->InFriendControl( &stGame2Mess_Friend_Insert_Req.inData))
#endif
				{
					// Failed
					i32result = EVENT_ERROR_EVENT_FRIEND_INSERT_FULL;

					i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_FRIEND_INSERT_ACK);
					MakePacket_OGame2Mess_Friend_Insert_Ack(&sendPacket,
						stGame2Mess_Friend_Insert_Req.iSessionIdx,
						stGame2Mess_Friend_Insert_Req.iUID,
						i32result);
					// Send Game Server
					SendPacketMessage(ui32ServerIdx, &sendPacket);
				}
			}
		}
		break;
	case PROTOCOL_MESSENGER_FRIEND_INVITED_REQ:
		{
			INT32 i32result = EVENT_ERROR_SUCCESS;
			USER_NODE node;
			USER_NODE friendNode;
			friendNode._ui32friendCount = 0; 

			OGame2Mess_Friend_Invited_Req		stGame2Mess_Friend_Invited_Req;
			MakePacket_OGame2Mess_Friend_Invited_Req(&stGame2Mess_Friend_Invited_Req, &packet);

			if (FALSE == m_pUserFinder->FindUid(stGame2Mess_Friend_Invited_Req.iUID, &node))
			{
				i32result = EVENT_ERROR_EVENT_FRIEND_INVITED_PARAMETER_USER;
			}
			else if (stGame2Mess_Friend_Invited_Req.iFriendIdx >= node._ui32friendCount)
			{
				i32result = EVENT_ERROR_EVENT_FRIEND_INVITED_PARAMETER_IDX;
			}
			else if (FALSE == m_pUserFinder->FindUid(node._afriend[stGame2Mess_Friend_Invited_Req.iFriendIdx]._i64fuid, &friendNode))
			{
				i32result = EVENT_ERROR_EVENT_FRIEND_INVITED_NOT_CONNECTED;
			}

			if (EV_FAILED(i32result))
			{
				/* Do nothing*/
				g_pLog->WriteLog( L"[ERROR] Friend Invited Fail [uid:%I64d][Error:%x]",stGame2Mess_Friend_Invited_Req.iUID, i32result  );

				i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_FRIEND_INVITED_ACK);
				MakePacket_OGame2Mess_Friend_Invited_Ack(&sendPacket,
					stGame2Mess_Friend_Invited_Req.iSessionIdx,
					stGame2Mess_Friend_Invited_Req.iUID,
					EVENT_ERROR_EVENT_FAIL_INVITED_USER);
				// Send Game Server
				SendPacketMessage(ui32ServerIdx, &sendPacket);
			}
			else
			{
#ifdef DEF_TEST_LOG
				// 임시
				if(FRIEND_MAX_COUNT < node._friendCount || RANK_MAX < node._iRank || MAX_CONNECT_USER_COUNT < node._idxBuffer)
				{
					I3BCLOG(LOCATION, "CTaskProcessor::OnRunning()[UID(%I64d)][_iRank(%d)][_idxBuffer(%d)][FriendCnt(%d][Nick(%s)]",
						node._i64DbIdx, node._iRank, node._idxBuffer, node._friendCount, node._nick);

					I3BCLOG(LOCATION, "CTaskProcessor::OnRunning()[UID(%I64d)][_iRank(%d)][_idxBuffer(%d)][FriendCnt(%d][Nick(%s)]",
						friendNode._i64DbIdx, friendNode._iRank, friendNode._idxBuffer, friendNode._friendCount, friendNode._nick);
				}
#endif

				for(UINT32 i = 0; i < friendNode._ui32friendCount; i++)
				{
					if (friendNode._afriend[i]._i64fuid == stGame2Mess_Friend_Invited_Req.iUID)
					{
						UINT8 ui8friendIdx = (UINT8)i;

						UINT32	ui32CurrentServer =  GET_FRIEND_SERVER(friendNode._ui32connectState);
						//UINT32	CurrentChannel =  GET_FRIEND_CHANNEL(friendNode._connectState);

#ifdef USE_SERVER_DEBUG_TEST
						// 자신의 PC에서 테스트시만 사용됨. 
						// 같은 종류의 서버 두개를 하나의 머신에서 사용하지 못하기 때문.
						// 디버깅 작업시 주의
						if ( 0 == CurrentServer)
						{
							CurrentServer = 1;
						}
#endif
						if (0 == ui32CurrentServer)
						{
							i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_FRIEND_INVITED_ACK);
							MakePacket_OGame2Mess_Friend_Invited_Ack(&sendPacket,
								stGame2Mess_Friend_Invited_Req.iSessionIdx,
								stGame2Mess_Friend_Invited_Req.iUID,
								EVENT_ERROR_EVENT_FAIL_INVITED_USER);
							// Send Game Server
							SendPacketMessage(ui32ServerIdx, &sendPacket);
							continue;
						}
						//else if (3 == CurrentServer)
						//{
						//	i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_FRIEND_INVITED_ACK);
						//	MakePacket_OGame2Mess_Friend_Invited_Ack(&sendPacket,
						//											 stGame2Mess_Friend_Invited_Req.iSessionIdx,
						//											 stGame2Mess_Friend_Invited_Req.iUID,
						//											 EVENT_ERROR_EVENT_FAIL_INVITED_USER_IN_CLAN_MATCH);
						//	// Send Game Server
						//	SendPacketMessage(iServerIdx, &sendPacket);
						//}

						i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_FRIEND_INVITED_REQUEST_ACK);
						sendPacket.WriteData(&stGame2Mess_Friend_Invited_Req.iUID,	sizeof(T_UID));
						sendPacket.WriteData(&friendNode._i32sessionidx,			sizeof(INT32));
						sendPacket.WriteData(&friendNode._i64UID,					sizeof(T_UID));
						sendPacket.WriteData(&ui8friendIdx,							sizeof(UINT8));
						
						// Send Game Server
						g_pGateway->SendPacket(friendNode._i32serveridx, &sendPacket);

						i32result = EVENT_ERROR_SUCCESS;
					}
				}
			}
		}
		break;	
	case PROTOCOL_MESSENGER_FRIEND_ROOM_ENTER_REQ:
		{
			OGame2Mess_Friend_Room_Enter_Req		stGame2Mess_Friend_Room_Enter_Req;
			MakePacket_OGame2Mess_Friend_Room_Enter_Req(&stGame2Mess_Friend_Room_Enter_Req, &packet);

#ifdef USE_SERVER_DEBUG_TEST
			ui32ServerIdx = ui32ServerIdx + 1;
#endif
			BOOL bRet = m_pUserFinder->ChangeState(stGame2Mess_Friend_Room_Enter_Req.iUID, MAKE_FRIEND_STATE(FRIEND_STATE_ROOM, ui32ServerIdx, stGame2Mess_Friend_Room_Enter_Req.channel, stGame2Mess_Friend_Room_Enter_Req.roomIdx));

			// 적용된 내용을 친구들에게 전송
			if(bRet)_ProcessFriendInfo(stGame2Mess_Friend_Room_Enter_Req.iUID);
		}
		break;
	case PROTOCOL_MESSENGER_FRIEND_ROOM_LEAVE_REQ:
		{
			OGame2Mess_Friend_Room_Leave_Req		stGame2Mess_Friend_Room_Leave_Req;
			MakePacket_OGame2Mess_Friend_Room_Leave_Req(&stGame2Mess_Friend_Room_Leave_Req, &packet);

			INT64 i64UID = stGame2Mess_Friend_Room_Leave_Req.iUID;
			USER_NODE *pUser = m_pUserFinder->GetFindUserNode(i64UID);

			if (!pUser)	break;

			UINT32 ui32Channel = GET_FRIEND_CHANNEL(pUser->_ui32connectState);
			BOOL bRet = m_pUserFinder->ChangeState(i64UID, MAKE_FRIEND_STATE(FRIEND_STATE_LOBBY, ui32ServerIdx, ui32Channel, 0));

			// 적용된 내용을 친구들에게 전송
			if(bRet)_ProcessFriendInfo( stGame2Mess_Friend_Room_Leave_Req.iUID );
		}
		break;
	case PROTOCOL_MESSENGER_RESET_FRIEND_INFO_REQ:
		{
			INT32 i32result = EVENT_ERROR_SUCCESS;
			USER_NODE node;

			OGame2Mess_Reset_Friend_Info_Req		stGame2Mess_Reset_Friend_Info_Req;
			MakePacket_OGame2Mess_Reset_Friend_Info_Req(&stGame2Mess_Reset_Friend_Info_Req, &packet);


			if (FALSE == m_pUserFinder->FindUid(stGame2Mess_Reset_Friend_Info_Req.iUID, &node))
			{
				i32result = ERROR_EVENT_FRIEND_INFO_PARAMETER_USER;
				g_pLog->WriteLog( L"[ERROR] User find fail (user info) -> uid(%I64d), error(0x%x)", stGame2Mess_Reset_Friend_Info_Req.iUID, i32result  );
				break;
			}

#ifdef ACTIVATE_FRIEND_DATABASE
			In_FriendList inData;
			inData.i64uid	= stGame2Mess_Reset_Friend_Info_Req.iUID;
			inData.bReset = TRUE;
			if ( !g_pDataBase->InFriendList( &inData))
			{
				g_pLog->WriteLog( L"[ERROR] _DoTaskForGameDataBase(Friend List buffer is Full (user info) -> uid(%I64d), error(0x%x))", stGame2Mess_Reset_Friend_Info_Req.iUID, i32result  );
			}
#endif
		}
		break;


		// 유저찾기 NEW 버전. 2012.06.07 노준영
	case PROTOCOL_MESSENGER_FIND_USER_REQ:
		{
			INT32 i32Result = EVENT_ERROR_NEW_USER_FIND;

			OGame2Mess_Find_User_Req		stGame2Mess_Find_User_Req;
			MakePacket_OGame2Mess_Find_User_Req(&stGame2Mess_Find_User_Req, &packet);

			// 해당 닉네임의 유저를 찾습니다.
			if( m_pUserFinder->IsAliveUser( stGame2Mess_Find_User_Req.iUID ) )
			{
				if(g_pDataBase->InUserSearch(stGame2Mess_Find_User_Req.iUID, stGame2Mess_Find_User_Req.strfUserNick) )
				{
					i32Result = EVENT_ERROR_SUCCESS; 
				}
				else i32Result = EVENT_ERROR_NEW_USER_FIND_MS_BUFFER_FULL;
			}
			else i32Result = EVENT_ERROR_NEW_USER_FIND_RUSER_WRONG;

			if (EV_FAILED(i32Result))	 // 여기서 실패하면 바로 ACK 날립니다.
			{
				i3NetworkPacket sendPacket( PROTOCOL_MESSENGER_FIND_USER_ACK );
				MakePacket_OGame2Mess_Find_User_Ack(&sendPacket,
					stGame2Mess_Find_User_Req.iSessionIdx,
					stGame2Mess_Find_User_Req.iUID,
					i32Result);
				// Send Game Server
				SendPacketMessage(ui32ServerIdx, &sendPacket);
			}
		}
		break;

		/* 유저찾기 구버전 
		#ifdef USE_MESS_FIND
		case PROTOCOL_MESSENGER_FIND_USER_REQ:
		{
		INT32 iResult = EVENT_ERROR_FAIL;

		OGame2Mess_Find_User_Req		stGame2Mess_Find_User_Req;
		MakePacket_OGame2Mess_Find_User_Req(&stGame2Mess_Find_User_Req, &packet);

		// 해당 닉네임의 유저를 찾습니다.
		#ifdef ACTIVATE_FRIEND_DATABASE
		if( m_pUserFinder->IsAliveUser( stGame2Mess_Find_User_Req.iUID ) )
		{
		if(g_pDataBase->InUserSearch(stGame2Mess_Find_User_Req.iUID, stGame2Mess_Find_User_Req.strfUserNick) )
		{
		iResult = EVENT_ERROR_SUCCESS; 
		}
		}
		#endif

		if( iResult == EVENT_ERROR_FAIL )
		{
		i3NetworkPacket sendPacket( PROTOCOL_MESSENGER_FIND_USER_ACK );
		MakePacket_OGame2Mess_Find_User_Ack(&sendPacket,
		stGame2Mess_Find_User_Req.iSessionIdx,
		stGame2Mess_Find_User_Req.iUID,
		iResult);
		// Send Game Server
		SendPacketMessage(iServerIdx, &sendPacket);
		}
		}
		break;
		#endif
		*/

		////////////////////////////////////////////////////////////////////////////////////
		//	유저가 채널 선택을 한 경우
		////////////////////////////////////////////////////////////////////////////////////
	case PROTOCOL_MESSENGER_FRIEND_LOBBY_ENTER_REQ:
		{
			// Community
			USER_NODE	node;

			OGame2MessSendChannelNumReq		stGame2MessSendChannelNumReq;
			MakePacket_OGame2MessSendChannelNumReq(&stGame2MessSendChannelNumReq, &packet);


			if( !m_pUserFinder->FindUid(stGame2MessSendChannelNumReq.iUID, &node) )
			{
				//g_pLog->WriteLog(m_i32TaskIdx,"UserEnterError - UserFinder Find. Uid = %I64d", iUID);
				break; 
			}

			node._i32serveridx			= ui32ServerIdx;
			node._i32sessionidx			= stGame2MessSendChannelNumReq.iSessionIdx;
			m_pUserFinder->UpdateByUid(&node);
			m_pUserFinder->ChangeState(stGame2MessSendChannelNumReq.iUID, MAKE_FRIEND_STATE(FRIEND_STATE_LOBBY, ui32ServerIdx, stGame2MessSendChannelNumReq.iChanelNum, 0));

			// 적용된 내용을 친구들에게 전송
			_ProcessFriendInfo(stGame2MessSendChannelNumReq.iUID);
		}
		break;
	case PROTOCOL_MESSENGER_FRIEND_LOBBY_LEAVE_REQ:
	{
		// Community
		USER_NODE	node;

		INT32 i32SessionIdx;
		INT64 i64UID;

		packet.ReadData(&i32SessionIdx, sizeof(INT32));
		packet.ReadData(&i64UID, sizeof(INT64));

		if (!m_pUserFinder->FindUid(i64UID, &node))
		{
			//sprintf(m_strLogBuffer, "UserEnterError - UserFinder Find. Uid = %I64d", iUID);
			//g_pLogFile->Write_Log_Task(m_strLogBuffer);
			break;
		}

		node._i32serveridx = ui32ServerIdx;
		node._i32sessionidx = i32SessionIdx;
		m_pUserFinder->UpdateByUid(&node);
		m_pUserFinder->ChangeState(i64UID, MAKE_FRIEND_STATE(FRIEND_STATE_ONLINE, ui32ServerIdx, 0, 0));

		// 적용된 내용을 친구들에게 전송
		_ProcessFriendInfo(i64UID);
	}
	break;
#if defined( USE_BLOCK_LIST )
	case PROTOCOL_MESSENGER_BLOCK_INSERT_REQ:
		{
			INT32 i32result = EVENT_ERROR_SUCCESS;

			INT64 i64BlockUID = 0;
			USER_NODE node;
			USER_NODE bnode;

			OGame2Mess_Block_Insert_Req		stGame2Mess_Block_Insert_Req;
			MakePacket_OGame2Mess_Block_Insert_Req(&stGame2Mess_Block_Insert_Req, &packet);

			// 금지어 검사(스페이스)
			BOOL bEcode = _FindTatooWord(stGame2Mess_Block_Insert_Req._inData._strNick);
			if (FALSE == bEcode)	// 닉네임에 금지어 포함.
			{
				i32result = EVENT_ERROR_EVENT_BLOCK_NOT_FIND_NICK;
			}
			else if (FALSE == m_pUserFinder->FindUid(stGame2Mess_Block_Insert_Req._i64UID, &node))	// 자신의 node 검색 실패.
			{
				i32result = EVENT_ERROR_EVENT_BLOCK_PARAMETER_USER;
			}
			else if (FALSE == m_pUserFinder->FindNick(stGame2Mess_Block_Insert_Req._inData._strNick, &bnode))	// 차단 유저 node 검색 실패.
			{
				i32result = EVENT_ERROR_EVENT_BLOCK_PARAMETER_USER;
			}
			else if ( TRUE == _CheckMyFriendByNick( &node, bnode._i64UID) ) // 차단 하려는 유저가 자신의 친구인지 체크.
			{
				i32result = EVENT_ERROR_EVENT_BLOCK_INSERT_FAIL_FRIEND;
			}
			else if (node._ui32BlockCount >= BLOCK_MAX_COUNT)
			{
				i32result = EVENT_ERROR_EVENT_BLOCK_FULL;
			}
			else
			{
				INT32	i32BlockServerIdx; 
				INT32	i32BlockSessionIdx; 

				if( TRUE == m_pUserFinder->GetUserInfoByNick( stGame2Mess_Block_Insert_Req._inData._strNick, &i64BlockUID, &i32BlockServerIdx, &i32BlockSessionIdx) )
				{
					if( i64BlockUID == stGame2Mess_Block_Insert_Req._i64UID ) // 차단 하려는 유저가 자기 자신인지 체크
					{
						i32result = EVENT_ERROR_EVENT_BLOCK_MY_NICKNAME;
					}
					else
					{
						for(UINT32 i = 0; i < node._ui32BlockCount; i++)
						{
							if (node._pBlock[i]._i64buid == i64BlockUID)
							{
								i32result = EVENT_ERROR_EVENT_BLOCK_ALREADY_BLOCK;
							}
						}
					}
				}
			}

			if (EV_FAILED(i32result))
			{
				// Failed
				/*char  strNick[NET_NICK_NAME_SIZE];
				UINT8 ui8NickSize =  (UINT8)i3String::Length(strNick) + 1;*/

				i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_BLOCK_INSERT_ACK);
				MakePacket_OGame2Mess_Block_Insert_Ack(&sendPacket,
					stGame2Mess_Block_Insert_Req._i32SessionIdx,
					stGame2Mess_Block_Insert_Req._i64UID,
					i32result,
					0,			// 실패이므로 의미없는 값을 보낸다.
					0,
					0,
					0,
					0,
					0);
				// Send Game Server
				SendPacketMessage(ui32ServerIdx, &sendPacket);
			}
			else
			{
				stGame2Mess_Block_Insert_Req._inData._i32type	= BLOCK_CONTROL_ADD;
				stGame2Mess_Block_Insert_Req._inData._i64uid	= stGame2Mess_Block_Insert_Req._i64UID;
				stGame2Mess_Block_Insert_Req._inData._i64buid	= i64BlockUID;			

				if ( FALSE == g_pDataBase->InBlockControl( &stGame2Mess_Block_Insert_Req._inData))
				{
					i32result = EVENT_ERROR_EVENT_BLOCK_BUFFER_OVERFLOW;

					i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_BLOCK_INSERT_ACK);
					MakePacket_OGame2Mess_Block_Insert_Ack(&sendPacket,
						stGame2Mess_Block_Insert_Req._i32SessionIdx,
						stGame2Mess_Block_Insert_Req._i64UID,
						i32result,
						0,
						0,
						0,
						0,
						0,
						0 );
					// Send Game Server
					SendPacketMessage(ui32ServerIdx, &sendPacket);
				}
			}
		}
		break;


	case PROTOCOL_MESSENGER_BLOCK_DELETE_REQ:
		{
			INT32 i32result = EVENT_ERROR_SUCCESS;
			USER_NODE node;
			IN_BLOCK_CONTROL inData;

			OGame2Mess_Block_Delete_Req		stGame2Mess_Block_Delete_Req;
			MakePacket_OGame2Mess_Block_Delete_Req(&stGame2Mess_Block_Delete_Req, &packet);

			if (FALSE == m_pUserFinder->FindUid(stGame2Mess_Block_Delete_Req._i64UID, &node))
			{
				i32result = EVENT_ERROR_EVENT_BLOCK_PARAMETER_USER;
			}	
			if (EV_FAILED(i32result))
			{
				i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_BLOCK_DELETE_ACK);
				MakePacket_OGame2Mess_Block_Delete_Ack(&sendPacket,
					stGame2Mess_Block_Delete_Req._i32SessionIdx,
					stGame2Mess_Block_Delete_Req._i64UID,
					i32result,
					0);	// 실패이므로 의미 없는 값 보냄.

				// Send Game Server
				SendPacketMessage(ui32ServerIdx, &sendPacket);
			}
			else
			{
				inData._i32type = BLOCK_CONTROL_DELETE;
				inData._i32Idx	= stGame2Mess_Block_Delete_Req._i32Idx;
				inData._i64uid	= stGame2Mess_Block_Delete_Req._i64UID;
				// 필요없다 - inData._i64buid = node._pBlock[stGame2Mess_Block_Delete_Req._ui8BlockIdx]._i64buid;
				// inData. = stGame2Mess_Friend_Delete_Req.iFriendIdx;
				if ( FALSE == g_pDataBase->InBlockControl( &inData))
				{
					i32result = EVENT_ERROR_EVENT_BLOCK_BUFFER_OVERFLOW;

					i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_BLOCK_DELETE_ACK);
					MakePacket_OGame2Mess_Block_Delete_Ack(&sendPacket,
						stGame2Mess_Block_Delete_Req._i32SessionIdx,
						stGame2Mess_Block_Delete_Req._i64UID,
						i32result,
						0);	// 실패이므로 의미 없는 값 보냄.
					// Send Game Server
					SendPacketMessage(ui32ServerIdx, &sendPacket);
				}
			}
		}
		break;
#endif

		// New 유저 찾기. 2012.06.11 노준영.
		// 찾기 요청한 유저와 찾고자 하는 유저가 다른 Game서버에 있을때, 찾고자하는 유저가 접속한 Game서버로 부터 받는 패킷.
	case PROTOCOL_MESSENGER_FUSER_INFO_ACK:
		{
			NEW_FIND_USER_INFO_SEV	stNewFindUserInfoSev;

			packet.ReadData( &stNewFindUserInfoSev, sizeof( NEW_FIND_USER_INFO_SEV ) );

			USER_NODE	RUserNode;	// 찾기 요청한 유저 노드
			USER_NODE	FUserNode;	// 찾고자 하는 유저 노드

			i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_FUSER_INFO_FIN_ACK);

			// 찾기 결과 및 두 유저 노드 검색.
			if( TRUE == m_pUserFinder->FindUid(stNewFindUserInfoSev.m_i64RUserUID, &RUserNode) ) 
			{
				// Game서버에서 보내온 결과가 실패.
				if ( EV_FAILED(stNewFindUserInfoSev.m_i32Result) )
				{	// 찾기 실패.

					sendPacket.WriteData( &stNewFindUserInfoSev, sizeof( NEW_FIND_USER_INFO_SEV ) );
					SendPacketMessage(RUserNode._i32serveridx, &sendPacket);	// 찾기 요청한 유저가 있는 Game서버로 보냄
				}

				if( TRUE == m_pUserFinder->FindUid(stNewFindUserInfoSev.m_i64FUserUID, &FUserNode) ) 
				{	// 찾기 성공.

					stNewFindUserInfoSev.m_NewFindUserInfo.m_i32State = FUserNode._ui32connectState; // 유저 상태( 위치, 서버, 채널, 룸 Idx ) 추가.
					stNewFindUserInfoSev.m_NewFindUserInfo.m_ui8Rank = (UINT8)FUserNode._ui32Rank; // 유저 상태( 위치, 서버, 채널, 룸 Idx ) 추가.
					sendPacket.WriteData( &stNewFindUserInfoSev, sizeof( NEW_FIND_USER_INFO_SEV ) );
					SendPacketMessage(RUserNode._i32serveridx, &sendPacket);	// 찾기 요청한 유저가 있는 Game서버로 보냄

				}
				else
				{
					// 찾고자 하는 유저가 Mess 서버에 없다. 찾기 실패.
					stNewFindUserInfoSev.m_i32Result = EVENT_ERROR_NEW_USER_FIND_FUSER_OFFLINE;
					sendPacket.WriteData( &stNewFindUserInfoSev, sizeof( NEW_FIND_USER_INFO_SEV ) );
					SendPacketMessage(RUserNode._i32serveridx, &sendPacket);	// 찾기 요청한 유저가 있는 Game서버로 보냄	
				}
			}
			// else 찾기 요청한 유저가 없으면 답없다. 멘붕
		}
		break;	

	case PROTOCOL_MESSENGER_FRIEND_CHANGE_STATE_REQ:
		{
			INT32 i32SessionIdx;
			T_UID TUID;
			FRIEND_STATE eState;

			packet.ReadData( &i32SessionIdx,	sizeof( INT32 ) );
			packet.ReadData( &TUID,				sizeof( T_UID ) );
			packet.ReadData( &eState,			sizeof( FRIEND_STATE ) );

			BOOL bRet = m_pUserFinder->ChangeState(TUID, MAKE_FRIEND_STATE(eState, ui32ServerIdx, 0, 0));

			// 적용된 내용을 친구들에게 전송
			if(bRet)_ProcessFriendInfo( TUID );
		}
		break;

	case PROTOCOL_MESSENGER_CHANGE_COLOR_NICK_REQ:
		{
			USER_NODE	node;
			OGame2Mess_Change_NickName_Req stReq;
			i3mem::FillZero( &stReq, sizeof( OGame2Mess_Change_NickName_Req ) );

			stReq.eWorkType = FRIEND_CHANGE_NICKCOLOR;
			packet.ReadData( &stReq.iSessionIdx,		sizeof( INT32 ) );
			packet.ReadData( &stReq.iUID,				sizeof( T_UID ) );
			packet.ReadData( &stReq.ui8NickColor,		sizeof( UINT8 ) );

			if( !m_pUserFinder->FindUid(stReq.iUID, &node) )
			{
				break; 
			}

			// 닉네임은 유저 닉네임으로
			i3String::Copy( stReq.nickname, node._strnick, NET_NICK_NAME_SIZE );
			stReq.nickname[NET_NICK_NAME_SIZE-1] = 0;

			node._ui8NickColor = stReq.ui8NickColor;

			_InChangeNick(&stReq);
		}
		break;
	case PROTOCOL_MESSENGER_FRIEND_INVITE_FAIL_REQ:
		{
			T_RESULT TResult;
			USER_NODE node;

			packet.ReadData( &node._i64UID,		sizeof( T_UID ) );
			packet.ReadData( &TResult,			sizeof( T_RESULT ) );
 
			if (FALSE == m_pUserFinder->FindUid(node._i64UID, &node))
			{
				break;
			}

			i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_FRIEND_INVITED_ACK);
			sendPacket.WriteData( &node._i32sessionidx, sizeof( INT32 ) );
			sendPacket.WriteData( &node._i64UID,		sizeof( T_UID ) );
			sendPacket.WriteData( &TResult,				sizeof( T_RESULT ) );
			SendPacketMessage(node._i32serveridx, &sendPacket);	 
 
		}
		break;
	case PROTOCOL_MESSENGER_NOTE_SEND_GIFT_NOTICE_REQ:
		{
			OGame2Mess_Note_Send_Gift_Notice_Req	stGame2Mess_Note_Send_Gift_Notice_Req;
			MakePacket_OGame2Mess_Note_Send_Gift_Notice_Req(&stGame2Mess_Note_Send_Gift_Notice_Req, &packet);	
	
#ifdef ACTIVATE_NOTE_DATABASE
			if (!g_pDataBase->InNoteCommandAddGiftOff(NOTE_COMMAND_TYPE_ADD_GIFT,&stGame2Mess_Note_Send_Gift_Notice_Req))
#endif
			{
				g_pLog->WriteLog(L"PROTOCOL_MESSENGER_NOTE_SEND_GIFT_NOTICE_REQ Error");
			}

		}
		break;
	case PROTOCOL_MESSENGER_NOTE_SEND_USERGIFT_NOTICE_REQ:
	{
		OGame2Mess_Note_Send_Find_UID_Req	stGame2Mess_Note_Send_Find_UID_Req;
		MakePacket_OGame2Mess_Note_Send_User_Gift_Req(&stGame2Mess_Note_Send_Find_UID_Req, &packet);

		USER_NODE node;	
		if (!m_pUserFinder->FindUid(stGame2Mess_Note_Send_Find_UID_Req.iUID, &node))
		{
			INT32 result = EVENT_ERROR_FAIL;
			i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_NOTE_SEND_ACK);
			MakePacket_OGame2Mess_Note_Send_Ack(&sendPacket,
				stGame2Mess_Note_Send_Find_UID_Req.iSessionIdx,
				stGame2Mess_Note_Send_Find_UID_Req.iUID,
				result);
			g_pGateway->SendPacket(ui32ServerIdx, &sendPacket);
			break;
		}
#ifdef ACTIVATE_NOTE_DATABASE
		if (!g_pDataBase->InNoteCommandAddFindUID(NOTE_COMMAND_TYPE_ADD_GIFT, node._strnick, stGame2Mess_Note_Send_Find_UID_Req))
			//if( !g_pDataBase->InNoteCommandAdd(NOTE_COMMAND_TYPE_ADD_NORMAL, iUID, node._nick, nickname, message, messageSize) )
#endif
		{
			INT32 result = EVENT_ERROR_FAIL;
			i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_NOTE_SEND_ACK);
			MakePacket_OGame2Mess_Note_Send_Ack(&sendPacket,
				stGame2Mess_Note_Send_Find_UID_Req.iSessionIdx,
				stGame2Mess_Note_Send_Find_UID_Req.iUID,
				result);
			g_pGateway->SendPacket(ui32ServerIdx, &sendPacket);
		}
	}
	break;
	default: 
		{
			g_pLog->WriteLog( L"[WARNING] CTaskProcessor::_DoTaskForGame() - Unkown protocol id(%d)", packet.GetProtocolID()  );
		}
		break;
	}

	return; 
}
