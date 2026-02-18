#include "pch.h"
#include "TaskProcessor.h"
#include "PacketLocker.h"
#include "Gateway.h"
#include "DataBase.h"
#include "Receiver.h"
#include "ConnectedUser.h"
#include "NoteBox.h"

void CTaskProcessor::_WorkForNoteList(void)
{
	OUT_NOTE_LIST * pOutData;
	INT32			i32ServerIdx;
	INT32			i32SessionIdx;

	pOutData = g_pDataBase->GetOutNoteList();
	if( !m_pUserFinder->GetUserInfoByUID(pOutData->i64UID, &i32SessionIdx, &i32ServerIdx) )
	{
		g_pDataBase->OutNoteListIdx();
		return;
	}

	INT32 i32TotalPage = pOutData->iCount / MAX_NOTE_COUNT_PER_PAGE;
	if (0 < pOutData->iCount % MAX_NOTE_COUNT_PER_PAGE)i32TotalPage++;

	for(INT32 i32Page = 0; i32Page < i32TotalPage; i32Page++)
	{
		UINT8 ui8curPage = (UINT8)i32Page;
		UINT8 ui8noteCount;

		//보낼 페이지 계산 
		ui8noteCount = MAX_NOTE_COUNT_PER_PAGE;
		if (i32Page == (i32TotalPage-1))//마지막 페이지 이면 
		{
			ui8noteCount = (UINT8)pOutData->iCount % MAX_NOTE_COUNT_PER_PAGE;
			if (0 == ui8noteCount)ui8noteCount = MAX_NOTE_COUNT_PER_PAGE;
		}
		//------------------------------------------------------------------
		// 패킷 생성
		i3NetworkPacket sendPacket( PROTOCOL_MESSENGER_NOTE_LIST_ACK );
		MakePacket_OGame2Mess_Note_List_Ack(&sendPacket,
											i32SessionIdx,
											pOutData->i64UID,
											ui8curPage,
											ui8noteCount,
											pOutData);
		// Send to GateWay Server (Game Server)
		g_pGateway->SendPacket(i32ServerIdx, &sendPacket);
	}

	g_pDataBase->OutNoteListIdx(); 
	return; 
}

void CTaskProcessor::_WorkForNoteCommand(void)
{
	OUT_NOTE_COMMAND * pOutData; 
	INT32	i32ServerIdx = -1;
	INT32	i32SessionIdx = -1;

	pOutData = g_pDataBase->GetOutNoteCommand();
	if (NULL == pOutData)
	{
		g_pLog->WriteLog( L"_WorkForNoteCommand() : LOCAL_RINGBUFFER Pop Error "  );
		return;
	}
	
#ifdef OLD_SERVER_CODE
	if(!m_pUserFinder->GetUserInfoByUID(pOutData->i64UID, &i32SessionIdx, &i32ServerIdx))
	{//유저가 없다. 
		g_pDataBase->OutNoteCommandIdx();
		return; 
	}
#endif

	switch(pOutData->note_info.iType)
	{
	case NOTE_COMMAND_TYPE_ADD_NOTICE	:
	case NOTE_COMMAND_TYPE_ADD_NORMAL	:
		{//접속해 있는 유저에게 보내는 부분은 만들어야 함 
			// 송신자 검색
			if(TRUE == m_pUserFinder->GetUserInfoByUID(pOutData->note_info.i64UID, &i32SessionIdx, &i32ServerIdx))
			{
				// 쪽지 보내기 결과를 송신자에게 전달(성공 실패 여부를 전달)
				// 단, 송신자가 없다면 전달 안됨
				i3NetworkPacket sender_sendPacket(PROTOCOL_MESSENGER_NOTE_SEND_ACK);
				MakePacket_OGame2Mess_Note_Send_Ack(&sender_sendPacket,
													i32SessionIdx,
													pOutData->note_info.i64UID,
													pOutData->note_info.iResult);
				// Send to GameServer
				g_pGateway->SendPacket(i32ServerIdx, &sender_sendPacket);
			}

			// 성공시에만 수신자에게 전송 처리로 진행
			if( EVENT_ERROR_SUCCESS != pOutData->note_info.iResult )
			{
				break;
			}

			// 수신자 검색
			if(!m_pUserFinder->GetUserInfoByUID(pOutData->note_info.i64RecvUID, &i32SessionIdx, &i32ServerIdx))
			{//유저가 없다. 
				break;
			}

			i3NetworkPacket receiver_sendPacket(PROTOCOL_MESSENGER_NOTE_RECEIVE_ACK);
			MakePacket_OGame2Mess_Note_Receive_Ack(&receiver_sendPacket,
												   i32SessionIdx,
												   pOutData->note_info.i64RecvUID,
												   pOutData->note_info.iResult,
												   pOutData->note_node);
			// Send to GameServer
			g_pGateway->SendPacket(i32ServerIdx, &receiver_sendPacket);

		}
		break; 
	case NOTE_COMMAND_TYPE_ADD_CLAN		:
	case NOTE_COMMAND_TYPE_ADD_CLANASK	:
		{//접속해 있는 유저에게 보내는 부분은 만들어야 함 
			// 송신자 검색
			if(TRUE == m_pUserFinder->GetUserInfoByUID(pOutData->note_info.i64UID, &i32SessionIdx, &i32ServerIdx))
			{
				// 쪽지 보내기 성공의 결과를 보낼 필요가 없을때 사용(팝업 메시지를 두개 만들지 않기 위해)
				
				switch((INT32)pOutData->note_node._body._message[1])
				{
				case NOTE_MESSAGE_CLAN_INVITE_ACCEPT:
				case NOTE_MESSAGE_CLAN_INVITE_DENIAL:
				case NOTE_MESSAGE_CLAN_SECESSION:
				case NOTE_MESSAGE_CLAN_DEPORTATION:
				case NOTE_MESSAGE_CLAN_MASTER:
				case NOTE_MESSAGE_CLAN_STAFF:
				case NOTE_MESSAGE_CLAN_REGULAR:
					{
						if(EVENT_ERROR_SUCCESS != pOutData->note_info.iResult)
						{
							// 쪽지 보내기 결과를 송신자에게 전달(성공 실패 여부를 전달)
							// 단, 송신자가 없다면 전달 안됨
							i3NetworkPacket sender_sendPacket(PROTOCOL_MESSENGER_CLAN_NOTE_SEND_ACK);
							MakePacket_OGame2Mess_Clan_Note_Send_Ack(&sender_sendPacket,
																	 i32SessionIdx,
																	 pOutData->note_info.i64UID,
																	 pOutData->note_info.iResult);
							// Send to GameServer
							g_pGateway->SendPacket(i32ServerIdx, &sender_sendPacket);
						}
					}
					break;
				case NOTE_MESSAGE_CLAN_INVITE:
				case NOTE_MESSAGE_CLAN_JOIN_ACCEPT:
				case NOTE_MESSAGE_CLAN_JOIN_DENIAL:
				default:
					// 쪽지 보내기 결과를 송신자에게 전달(성공 실패 여부를 전달)
					// 단, 송신자가 없다면 전달 안됨
					i3NetworkPacket sender_sendPacket(PROTOCOL_MESSENGER_CLAN_NOTE_SEND_ACK);
					MakePacket_OGame2Mess_Clan_Note_Send_Ack(&sender_sendPacket,
															 i32SessionIdx,
															 pOutData->note_info.i64UID,
															 pOutData->note_info.iResult);
					// Send to GameServer
					g_pGateway->SendPacket(i32ServerIdx, &sender_sendPacket);
					break;
				}
			}

			// 성공시에만 수신자에게 결과 전송함.
			if (EVENT_ERROR_SUCCESS != pOutData->note_info.iResult)
			{
				break;
			}

			// 수신자 검색
			if(!m_pUserFinder->GetUserInfoByUID(pOutData->note_info.i64RecvUID, &i32SessionIdx, &i32ServerIdx))
			{//유저가 없다. 
				break;
			}

			i3NetworkPacket receiver_sendPacket(PROTOCOL_MESSENGER_NOTE_RECEIVE_ACK);
			MakePacket_OGame2Mess_Note_Receive_Ack(&receiver_sendPacket,
													i32SessionIdx,
													pOutData->note_info.i64RecvUID,
													pOutData->note_info.iResult,
													pOutData->note_node);
			// Send to GameServer
			g_pGateway->SendPacket(i32ServerIdx, &receiver_sendPacket);
			
		}
		break; 
	case NOTE_COMMAND_TYPE_READ	:
		{				
			if(!m_pUserFinder->GetUserInfoByUID(pOutData->note_info.i64UID, &i32SessionIdx, &i32ServerIdx))
			{//유저가 없다. 
				break;
			}

			i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_NOTE_CHECK_READED_ACK);
			MakePacket_OGame2Mess_Note_Check_Readed_Ack(&sendPacket,
														i32SessionIdx,
														pOutData->note_info.i64UID,
														pOutData->note_info.iCount,
														(INT32*)pOutData->note_info.pList);
			// Send Gateway Server
			g_pGateway->SendPacket(i32ServerIdx, &sendPacket);
		}
		break; 
	case NOTE_COMMAND_TYPE_DEL	: 
		{
			if(!m_pUserFinder->GetUserInfoByUID(pOutData->note_info.i64UID, &i32SessionIdx, &i32ServerIdx))
			{//유저가 없다. 
				break;
			}

			i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_NOTE_DELETE_ACK);
			MakePacket_OGame2Mess_Note_Delete_Ack(&sendPacket,
													i32SessionIdx,
													pOutData->note_info.i64UID,
													pOutData->note_info.iResult,
													pOutData->note_info.iCount,
													(INT32*)pOutData->note_info.pList);
			// Send Gateway Server
			g_pGateway->SendPacket(i32ServerIdx, &sendPacket);
		}
		break;
	case NOTE_COMMAND_TYPE_ADD_GIFT:
		{
			//접속해 있는 유저에게 보내는 부분은 만들어야 함 
			// 송신자 == 자신
			if (TRUE == m_pUserFinder->GetUserInfoByUID(pOutData->note_info.i64UID, &i32SessionIdx, &i32ServerIdx))
			{
				if (EVENT_ERROR_SUCCESS != pOutData->note_info.iResult)
				{
					break;
				}

				// 수신자 검색
				if (!m_pUserFinder->GetUserInfoByUID(pOutData->note_info.i64RecvUID, &i32SessionIdx, &i32ServerIdx))
				{//유저가 없다. 
					break;
				}

				i3NetworkPacket receiver_sendPacket(PROTOCOL_MESSENGER_NOTE_RECEIVE_ACK);
				MakePacket_OGame2Mess_Note_Receive_Ack(&receiver_sendPacket,
					i32SessionIdx,
					pOutData->note_info.i64RecvUID,
					pOutData->note_info.iResult,
					pOutData->note_node);
				// Send to GameServer
				g_pGateway->SendPacket(i32ServerIdx, &receiver_sendPacket);

			}			
		}
		break;
	}

	g_pDataBase->OutNoteCommandIdx();
	return; 
}

// Clan Group Note
void CTaskProcessor::_OutWorkForClanGroupNoteCommand(void)
{
	OUT_NOTE_COMMAND * pOutData;
	INT32	i32ServerIdx = -1;
	INT32	i32SessionIdx = -1;

//	char				strTemp[SQL_STRING_COUNT];

	pOutData = g_pDataBase->GetOutClanGroupNoteCommand();
	if (NULL == pOutData)
	{
		return;
	}

	//g_pLog->WriteLog(m_i32TaskIdx,"[Call]_OutWorkForClanGroupNoteCommand()");

	switch(pOutData->note_info.iType)
	{
	case NOTE_COMMAND_TYPE_ADD_NOTICE	:
	case NOTE_COMMAND_TYPE_ADD_NORMAL	:
		{//접속해 있는 유저에게 보내는 부분은 만들어야 함 		

			// 성공시에만 수신자에게 전송 처리로 진행
			if( EVENT_ERROR_SUCCESS != pOutData->note_info.iResult )
			{
				break;
			}			
			else
			{
				// 송신자 검색
				if(TRUE == m_pUserFinder->GetUserInfoByUID(pOutData->note_info.i64UID, &i32SessionIdx, &i32ServerIdx))
				{
					// 쪽지 보내기 결과를 송신자에게 전달(성공 실패 여부를 전달)
					// 단, 송신자가 없다면 전달 안됨
					i3NetworkPacket sender_sendPacket(PROTOCOL_MESSENGER_NOTE_SEND_ACK);
					MakePacket_OGame2Mess_Note_Send_Ack(&sender_sendPacket,
						i32SessionIdx,
						pOutData->note_info.i64UID,
						pOutData->note_info.iResult);
					// Send to GameServer
					g_pGateway->SendPacket(i32ServerIdx, &sender_sendPacket);
				}
			}

			// 수신자 검색
			if(!m_pUserFinder->GetUserInfoByUID(pOutData->note_info.i64RecvUID, &i32SessionIdx, &i32ServerIdx))
			{//유저가 없다. 
				break;
			}

			i3NetworkPacket receiver_sendPacket(PROTOCOL_MESSENGER_NOTE_RECEIVE_ACK);
			MakePacket_OGame2Mess_Note_Receive_Ack(&receiver_sendPacket,
													i32SessionIdx,
													pOutData->note_info.i64RecvUID,
													pOutData->note_info.iResult,
													pOutData->note_node);
			// Send to GameServer
			g_pGateway->SendPacket(i32ServerIdx, &receiver_sendPacket);

		}
		break; 
	case NOTE_COMMAND_TYPE_ADD_CLAN		:
	case NOTE_COMMAND_TYPE_ADD_CLANASK	:
		{//접속해 있는 유저에게 보내는 부분은 만들어야 함 
			// 송신자 검색
			if(TRUE == m_pUserFinder->GetUserInfoByUID(pOutData->note_info.i64UID, &i32SessionIdx, &i32ServerIdx))
			{
				// 쪽지 보내기 성공의 결과를 보낼 필요가 없을때 사용(팝업 메시지를 두개 만들지 않기 위해)

				switch((INT32)pOutData->note_node._body._message[1])
				{
				case NOTE_MESSAGE_CLAN_INVITE_ACCEPT:
				case NOTE_MESSAGE_CLAN_INVITE_DENIAL:
				case NOTE_MESSAGE_CLAN_SECESSION:
				case NOTE_MESSAGE_CLAN_DEPORTATION:
				case NOTE_MESSAGE_CLAN_MASTER:
				case NOTE_MESSAGE_CLAN_STAFF:
				case NOTE_MESSAGE_CLAN_REGULAR:
					{
						if(EVENT_ERROR_SUCCESS != pOutData->note_info.iResult)
						{
							// 쪽지 보내기 결과를 송신자에게 전달(성공 실패 여부를 전달)
							// 단, 송신자가 없다면 전달 안됨
							i3NetworkPacket sender_sendPacket(PROTOCOL_MESSENGER_CLAN_NOTE_SEND_ACK);
							MakePacket_OGame2Mess_Clan_Note_Send_Ack(&sender_sendPacket,
																	 i32SessionIdx,
																	 pOutData->note_info.i64UID,
																	 pOutData->note_info.iResult);
							// Send to GameServer
							g_pGateway->SendPacket(i32ServerIdx, &sender_sendPacket);
						}
					}
					break;
				case NOTE_MESSAGE_CLAN_INVITE:
				case NOTE_MESSAGE_CLAN_JOIN_ACCEPT:
				case NOTE_MESSAGE_CLAN_JOIN_DENIAL:
				default:
					// 쪽지 보내기 결과를 송신자에게 전달(성공 실패 여부를 전달)
					// 단, 송신자가 없다면 전달 안됨
					//i3NetworkPacket sender_sendPacket(PROTOCOL_MESSENGER_CLAN_NOTE_SEND_ACK);
					//MakePacket_OGame2Mess_Clan_Note_Send_Ack(&sender_sendPacket,
					//										 iSessionIdx,
					//										 pOutData->note_info.i64UID,
					//										 pOutData->note_info.iResult);
					//// Send to GameServer
					//g_pGateway->SendPacket(iServerIdx, &sender_sendPacket);
					break;
				}
			}

			// 성공시에만 수신자에게 결과 전송함.
			if (EVENT_ERROR_SUCCESS != pOutData->note_info.iResult)
			{
				break;
			}

			// 수신자 검색
			if(!m_pUserFinder->GetUserInfoByUID(pOutData->note_info.i64RecvUID, &i32SessionIdx, &i32ServerIdx))
			{//유저가 없다. 
				break;
			}

			i3NetworkPacket receiver_sendPacket(PROTOCOL_MESSENGER_NOTE_RECEIVE_ACK);
			MakePacket_OGame2Mess_Note_Receive_Ack(&receiver_sendPacket,
													i32SessionIdx,
													pOutData->note_info.i64RecvUID,
													pOutData->note_info.iResult,
													pOutData->note_node);
			// Send to GameServer
			g_pGateway->SendPacket(i32ServerIdx, &receiver_sendPacket);

		}
		break; 
	case NOTE_COMMAND_TYPE_READ	:
		{				
			if(!m_pUserFinder->GetUserInfoByUID(pOutData->note_info.i64UID, &i32SessionIdx, &i32ServerIdx))
			{//유저가 없다. 
				break;
			}

			i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_NOTE_CHECK_READED_ACK);
			MakePacket_OGame2Mess_Note_Check_Readed_Ack(&sendPacket,
														i32SessionIdx,
														pOutData->note_info.i64UID,	
														pOutData->note_info.iCount,	
														(INT32*)pOutData->note_info.pList);
			// Send Gateway Server
			g_pGateway->SendPacket(i32ServerIdx, &sendPacket);
		}
		break; 
	case NOTE_COMMAND_TYPE_DEL	: 
		{
			if(!m_pUserFinder->GetUserInfoByUID(pOutData->note_info.i64UID, &i32SessionIdx, &i32ServerIdx))
			{//유저가 없다. 
				break;
			}

			i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_NOTE_DELETE_ACK);
			MakePacket_OGame2Mess_Note_Delete_Ack(&sendPacket,
												 i32SessionIdx,
												 pOutData->note_info.i64UID,
												 pOutData->note_info.iResult,
												 pOutData->note_info.iCount,
												 (INT32*)pOutData->note_info.pList);
			// Send Gateway Server
			g_pGateway->SendPacket(i32ServerIdx, &sendPacket);
		}
		break;
	}

	g_pDataBase->OutClanGroupNoteCommandIdx();

	return;
}
