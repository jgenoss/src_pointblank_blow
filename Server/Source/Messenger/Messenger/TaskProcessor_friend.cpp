#include "pch.h"
#include "TaskProcessor.h"
#include "PacketLocker.h"
#include "Gateway.h"
#include "DataBase.h"
#include "Receiver.h"
#include "ConnectedUser.h"
#include "NoteBox.h"

void CTaskProcessor::_WorkForFriendList(void)
{
	OUT_FRIEND_LIST* pData = g_pDataBase->GetOutFriendListData();

	USER_NODE node;
	if( !m_pUserFinder->FindUid(pData->i64uid, &node) )return;
	
	// 유저 자신의 리스트를 업데이트 합니다. 
	#ifdef DEF_TEST_LOG
	// 임시
	if(FRIEND_MAX_COUNT < pData->friendCount || RANK_MAX < node._iRank || MAX_CONNECT_USER_COUNT < node._idxBuffer)
	{
		I3BCLOG(LOCATION, "CTaskProcessor::_WorkForFriendList()[UID(%I64d)][_iRank(%d)][_idxBuffer(%d)][FriendCnt(%d][Nick(%s)]",
				pData->i64uid, node._iRank, node._idxBuffer, pData->friendCount, node._nick);
	}
	#endif

	// 의심가는 코드
	node._ui32friendCount = MIN(pData->friendCount, FRIEND_MAX_COUNT);
	i3mem::Copy( node._afriend, pData->friendNode, (sizeof( FRIEND_NODE)*node._ui32friendCount) );
	m_pUserFinder->UpdateByUid(&node);

	#ifdef DEF_TEST_LOG
	// 임시
	if(FRIEND_MAX_COUNT < node._friendCount || RANK_MAX < node._iRank || MAX_CONNECT_USER_COUNT < node._idxBuffer)
	{
		I3BCLOG(LOCATION, "CTaskProcessor::_WorkForFriendList()[UID(%I64d)][_iRank(%d)][_idxBuffer(%d)][FriendCnt(%d][Nick(%s)]",
				node._i64DbIdx, node._iRank, node._idxBuffer, node._friendCount, node._nick);
	}
	#endif

	// 업데이트 된 노드 구조를 이용하여, 친구 리스트를 만듭니다.
	for(UINT32 i = 0; i < node._ui32friendCount; i++)
	{
		if (FRIEND_STATE_OFFLINE == GET_FRIEND_STATE(node._afriend[i]._i32state))		//이거 할 필요없을듯. 송과장 
		{
			USER_NODE friendNode;
			if( m_pUserFinder->FindUid(node._afriend[i]._i64fuid, &friendNode) )
			{
				#ifdef DEF_TEST_LOG
				// 임시
				if(FRIEND_MAX_COUNT < friendNode._friendCount || RANK_MAX < friendNode._iRank || MAX_CONNECT_USER_COUNT < friendNode._idxBuffer)
				{
					I3BCLOG(LOCATION, "CTaskProcessor::_WorkForFriendList()[UID(%I64d)][_iRank(%d)][_idxBuffer(%d)][FriendCnt(%d][Nick(%s)]",
							friendNode._i64DbIdx, friendNode._iRank, friendNode._idxBuffer, friendNode._friendCount, friendNode._nick);
				}
				#endif

				for(UINT32 j = 0; j < friendNode._ui32friendCount; j++)
				{
					if (friendNode._afriend[j]._i64fuid == pData->i64uid)
					{
						node._afriend[i]._i32state		 = (INT32)friendNode._ui32connectState;
						friendNode._afriend[j]._i32state = (INT32)node._ui32connectState;

						UINT8 ui8changeState = FRIEND_CHANGE_STATE_CHANGE;
						UINT8 ui8length = (UINT8)(i3String::Length(friendNode._afriend[j]._strnick) + 1);
						UINT8 ui8friendIdx = (UINT8)j;

						FRIEND_INFO info;
						info._fuid	= friendNode._afriend[j]._i64fuid;
						info._rank	= friendNode._afriend[j]._ui8rank;
						info._state = (UINT32)friendNode._afriend[j]._i32state;
						info._nickcolor = friendNode._afriend[j]._ui8nickcolor;

						i3NetworkPacket friendPacket(PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK);
						MakePacket_OGame2Mess_Friend_Info_Change_Ack(&friendPacket,
																	 friendNode._i32sessionidx,
																	 friendNode._i64UID,
																	 ui8changeState,
																	 ui8friendIdx,
																	 ui8length,
																	 friendNode._afriend[j]._strnick,
																	 info);
						// Send Game Server
						g_pGateway->SendPacket(friendNode._i32serveridx, &friendPacket);

						m_pUserFinder->UpdateByUid(&friendNode);
						break;
					}
				}
			}
		}
	}

	UINT8 ui8friendCount = (UINT8)node._ui32friendCount;

	i3NetworkPacket packet(PROTOCOL_MESSENGER_FRIEND_INFO_ACK);
	MakePacket_OGame2Mess_Friend_Info_Ack(&packet,
										  node._i32sessionidx,
										  node._i64UID,
										  ui8friendCount,
										  node._afriend);

	#ifdef DEF_TEST_LOG
	// 임시
	if(FRIEND_MAX_COUNT < ui8friendCount || RANK_MAX < node._iRank || MAX_CONNECT_USER_COUNT < node._idxBuffer)
	{
		I3BCLOG(LOCATION, "CTaskProcessor::Working_FriendList()[UID(%I64d)][_iRank(%d)][_idxBuffer(%d)][FriendCnt(%d][Nick(%s)]",
				node._i64DbIdx, node._iRank, node._idxBuffer, ui8friendCount, node._nick);
	}
	#endif

	// Send Game Server
	g_pGateway->SendPacket(node._i32serveridx, &packet);
	m_pUserFinder->UpdateByUid(&node);

	if(pData->iRank != node._ui32Rank)
	{
	// UserFinder에 적용
#ifdef USE_CHANGE_RANK	
	#ifdef USE_HASH_NICK
		m_pUserFinder->UpdateRank(node._i64UID, node._iRank);
		// 적용된 내용을 친구들에게 전송
		_ProcessFriendInfo(stGame2Mess_Change_Rank_Req.iUID);
	#else
		// 변경된 자신의 계급 저장
		OGame2Mess_Change_Rank_Req	stGame2Mess_Change_Rank_Req;
		stGame2Mess_Change_Rank_Req.iSessionIdx = node._i32sessionidx;
		stGame2Mess_Change_Rank_Req.iUID		= node._i64UID;
		stGame2Mess_Change_Rank_Req.iRank		= node._ui32Rank;
		m_pUserFinder->UpdateRank(node._i64UID, node._ui32Rank);
		_InChangeRank(stGame2Mess_Change_Rank_Req);
	#endif
#endif
	}

	return; 
}

void CTaskProcessor::_WorkForFriendControl(void)
{
	OUT_FRIEND_CONTROL* pData = g_pDataBase->GetOutFriendControlData();

	switch(pData->eWorkType)
	{
	case FRIEND_CONTROL_AUTH:
		{
			USER_NODE node;
			UINT8 ui8idx = pData->idx;
			if( m_pUserFinder->FindUid(pData->i64uid, &node))
			{
				// node에 Data가 있음
				// 에러 처리
				if (EVENT_ERROR_SUCCESS != pData->result )
				{
					i3NetworkPacket packet(PROTOCOL_MESSENGER_FRIEND_ACCEPT_ACK);
					MakePacket_OGame2Mess_Friend_Accept_Ack(&packet,
															node._i32sessionidx,
															node._i64UID,
															pData->result);
					// Send Game Server
					SendPacketMessage((UINT32)node._i32serveridx, &packet);
					break;
				}

				// 로그를 위해 게임서버에 친구 수락통보해야한다
				{
					UINT8 ui8state = FRIEND_CHANGE_STATE_ACCEPT; 
					UINT8 ui8friendIdx = ui8idx;

					i3NetworkPacket authPacket(PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK);
					/*MakePacket_OGame2Mess_Friend_Info_Change_Ack(&authPacket,
																 node._sessionidx,
																 node._iDbIdx,
																 state,
																 friendIdx,
																 nicknameSize,
																 friendNode._friend[j]._nick,
																 info);*/

					authPacket.WriteData(&node._i32sessionidx,	sizeof(INT32));
					authPacket.WriteData(&node._i64UID,			sizeof(INT64));
					authPacket.WriteData(&ui8state,				sizeof(UINT8));
					authPacket.WriteData(&ui8friendIdx,			sizeof(UINT8));

					// Send Game Server
					g_pGateway->SendPacket((UINT32)node._i32serveridx, &authPacket);
				}

				USER_NODE friendNode;

				node._afriend[ui8idx]._i32state = MAKE_FRIEND_STATE(FRIEND_STATE_OFFLINE, 0, 0, 0);

				if (m_pUserFinder->FindUid(pData->i64fuid, &friendNode))
				{
					#ifdef DEF_TEST_LOG
					// 임시
					if(FRIEND_MAX_COUNT < friendNode._friendCount || RANK_MAX < friendNode._iRank || MAX_CONNECT_USER_COUNT < friendNode._idxBuffer)
					{
						I3BCLOG(LOCATION, "CTaskProcessor::OnRunning()[UID(%I64d)][_iRank(%d)][_idxBuffer(%d)][FriendCnt(%d][Nick(%s)]",
								friendNode._i64DbIdx, friendNode._iRank, friendNode._idxBuffer, friendNode._friendCount, friendNode._nick);	
					}
					#endif

					for(UINT32 i = 0; i < friendNode._ui32friendCount; i++)
					{
						if (friendNode._afriend[i]._i64fuid == pData->i64uid)
						{
							node._afriend[ui8idx]._i32state = (INT32)friendNode._ui32connectState;
							friendNode._afriend[i]._i32state = (INT32)node._ui32connectState;

							UINT8 ui8changeState	= FRIEND_CHANGE_STATE_CHANGE;
							UINT8 ui8length			= (UINT8)(i3String::Length(friendNode._afriend[i]._strnick) + 1);
							UINT8 ui8friendIdx		= (UINT8)i;

							FRIEND_INFO info;
							info._fuid = friendNode._afriend[i]._i64fuid;
							info._rank = friendNode._afriend[i]._ui8rank;
							info._state = (UINT32)friendNode._afriend[i]._i32state;
							info._nickcolor = friendNode._afriend[i]._ui8nickcolor;

							i3NetworkPacket friendPacket(PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK);
							MakePacket_OGame2Mess_Friend_Info_Change_Ack(&friendPacket,
																		 friendNode._i32sessionidx,
																		 friendNode._i64UID,
																		 ui8changeState,
																		 ui8friendIdx,
																		 ui8length,
																		 friendNode._afriend[i]._strnick,
																		 info);
							// Send Game Server
							g_pGateway->SendPacket((UINT32)friendNode._i32serveridx, &friendPacket);

							m_pUserFinder->UpdateByUid(&friendNode);

							break;
						}
					}
				}

				UINT8 ui8changeState	= FRIEND_CHANGE_STATE_CHANGE;
				UINT8 ui8length			= (UINT8)(i3String::Length(node._afriend[ui8idx]._strnick) + 1);
				UINT8 ui8friendIdx		= ui8idx;

				FRIEND_INFO info;
				info._fuid = node._afriend[ui8idx]._i64fuid;
				info._rank = node._afriend[ui8idx]._ui8rank;
				info._state = (UINT32)node._afriend[ui8idx]._i32state;
				info._nickcolor = node._afriend[ui8idx]._ui8nickcolor;

				i3NetworkPacket packet(PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK);
				MakePacket_OGame2Mess_Friend_Info_Change_Ack(&packet,
															 node._i32sessionidx,
															 node._i64UID,
															 ui8changeState,
															 ui8friendIdx,
															 ui8length,
															 node._afriend[ui8idx]._strnick,
															 info);
				// Send Game Server
				g_pGateway->SendPacket((UINT32)node._i32serveridx, &packet);

				m_pUserFinder->UpdateByUid(&node);
			}
		}
		break;

	case FRIEND_CONTROL_DELETE:
		{
			USER_NODE node;
			USER_NODE friendNode;

			if( m_pUserFinder->FindUid(pData->i64uid, &node))
			{
				// node에 Data가 있음
				// 에러 처리
				if (EVENT_ERROR_SUCCESS != pData->result )
				{
					i3NetworkPacket packet(PROTOCOL_MESSENGER_FRIEND_ACCEPT_ACK);
					MakePacket_OGame2Mess_Friend_Accept_Ack(&packet,
															node._i32sessionidx,
															node._i64UID,
															pData->result);
					// Send Game Server
					SendPacketMessage(node._i32serveridx, &packet);
					break;
				}

				// 로그를 위해 게임서버에 친구 삭제통보해야한다
				{
					UINT8 ui8state = FRIEND_CHANGE_STATE_DELETE;
					UINT8 ui8friendIdx = (UINT8)pData->idx;

					i3NetworkPacket deletePacket(PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK);

					deletePacket.WriteData(&node._i32sessionidx,	sizeof(INT32));
					deletePacket.WriteData(&node._i64UID,			sizeof(INT64));
					deletePacket.WriteData(&ui8state,				sizeof(UINT8));
					deletePacket.WriteData(&ui8friendIdx,			sizeof(UINT8));

					// Send Game Server
					g_pGateway->SendPacket(node._i32serveridx, &deletePacket);
				}

				// Friend Info List요청(기존 리스트 삭제후, 다시 받음 - 기존 MakeFriendList의 일을 수행함)
				{
					In_FriendList inData;
					inData.i64uid = pData->i64uid;
					inData.bReset = FALSE;

					if ( !g_pDataBase->InFriendList( &inData))
					{
						g_pLog->WriteLog( L"[ERROR] _WorkForFriendControl(Friend List buffer is Full (user info) -> uid(%I64d))", pData->i64uid );
					}
				}

				if (m_pUserFinder->FindUid(pData->i64fuid, &friendNode))
				{
					#ifdef DEF_TEST_LOG
					// 임시
					if(FRIEND_MAX_COUNT < friendNode._friendCount || RANK_MAX < friendNode._iRank || MAX_CONNECT_USER_COUNT < friendNode._idxBuffer)
					{
						I3BCLOG(LOCATION, "CTaskProcessor::_WorkForFriendControl()[UID(%I64d)][_iRank(%d)][_idxBuffer(%d)][FriendCnt(%d][Nick(%s)]",
								friendNode._i64DbIdx, friendNode._iRank, friendNode._idxBuffer, friendNode._friendCount, friendNode._nick);	
					}
					#endif

					for(UINT32 j = 0; j < friendNode._ui32friendCount; j++)
					{
						if (friendNode._afriend[j]._i64fuid == pData->i64uid)
						{
							UINT8 ui8currentState = (UINT8)GET_FRIEND_STATE(friendNode._afriend[j]._i32state);

							if (FRIEND_STATE_OFFLINE <= ui8currentState)
							{
								friendNode._afriend[j]._i32state = MAKE_FRIEND_STATE(FRIEND_STATE_OFFLINE, 0, 0, 0);

								UINT8 ui8changeState	= FRIEND_CHANGE_STATE_CHANGE;
								UINT8 ui8length			= (UINT8)(i3String::Length(friendNode._afriend[j]._strnick) + 1);
								UINT8 ui8friendIdx		= (UINT8)j;

								FRIEND_INFO info;
								info._fuid = friendNode._afriend[j]._i64fuid;
								info._rank = friendNode._afriend[j]._ui8rank;
								info._state = friendNode._afriend[j]._i32state;

								i3NetworkPacket friendPacket(PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK);
								MakePacket_OGame2Mess_Friend_Info_Change_Ack(&friendPacket,
																			 friendNode._i32sessionidx,
																			 friendNode._i64UID,
																			 ui8changeState,
																			 ui8friendIdx,
																			 ui8length,
																			 friendNode._afriend[j]._strnick,
																			 info);
								// Send Game Server
								g_pGateway->SendPacket(friendNode._i32serveridx, &friendPacket);

								m_pUserFinder->UpdateByUid(&friendNode);
							}

							break;
						}
					}
				}

				// 클라이언트에 응답 (GameServer의 코드상으로는 에러시에만 사용한다고 되어 있으나, 실제 클라이언트에서 성공여부를 체크하여 성공 팝업창에 사용함)
				INT32 i32result = EVENT_ERROR_SUCCESS;
				i3NetworkPacket SuccessPacket(PROTOCOL_MESSENGER_FRIEND_DELETE_ACK);
				SuccessPacket.WriteData(&node._i32sessionidx,	sizeof(INT32));
				SuccessPacket.WriteData(&node._i64UID,			sizeof(INT64));
				SuccessPacket.WriteData(&i32result,				sizeof(INT32));
				g_pGateway->SendPacket(node._i32serveridx, &SuccessPacket);

			}
		}
		break;

	case FRIEND_CONTROL_ADD:
		{
			USER_NODE node;
			USER_NODE friendUserNode;
			UINT8 ui8idxFriend = 0;
			UINT8 ui8changeState;

			if( m_pUserFinder->FindUid(pData->i64uid, &node))
			{
				// node에 Data가 있음
				// 에러 처리
				if (EVENT_ERROR_SUCCESS != pData->result )
				{
					i3NetworkPacket packet(PROTOCOL_MESSENGER_FRIEND_INSERT_ACK);
					MakePacket_OGame2Mess_Friend_Insert_Ack(&packet,
															node._i32sessionidx,
															node._i64UID,
															pData->result);
					// Send Game Server
					SendPacketMessage(node._i32serveridx, &packet);
					break;
				}

				if (m_pUserFinder->FindUid(pData->i64fuid, &friendUserNode))
				{					
					if (FRIEND_STATE_OFFLINE <= GET_FRIEND_STATE(pData->targetFriendNode._i32state))
					{
						pData->targetFriendNode._i32state = friendUserNode._ui32connectState;
					}

					// 자신이 상대 메신저에 등록되어있는지 확인한다.							
					BOOL bFound = FALSE;

					#ifdef DEF_TEST_LOG
					// 임시
					if(FRIEND_MAX_COUNT < friendUserNode._friendCount || RANK_MAX < friendUserNode._iRank || MAX_CONNECT_USER_COUNT < friendUserNode._idxBuffer)
					{
						I3BCLOG(LOCATION, "CTaskProcessor::_WorkForFriendControl()[UID(%I64d)][_iRank(%d)][_idxBuffer(%d)][FriendCnt(%d][Nick(%s)]",
								friendUserNode._i64DbIdx, friendUserNode._iRank, friendUserNode._idxBuffer, friendUserNode._friendCount, friendUserNode._nick);
					}
					#endif

					for(UINT32 i = 0; i < friendUserNode._ui32friendCount; i++)
					{
						if (friendUserNode._afriend[i]._i64fuid == pData->i64uid)
						{
							bFound = TRUE;
							ui8idxFriend = (UINT8)i;
							break;
						}
					}

					// 있으면 상태를 변경하고, 없으면 새로 추가한다.
					if (bFound)
					{
						ui8changeState = FRIEND_CHANGE_STATE_CHANGE;
					}
					else
					{
						ui8changeState = FRIEND_CHANGE_STATE_INSERT;

						#ifdef DEF_TEST_LOG
						// 임시
						if(FRIEND_MAX_COUNT < friendUserNode._friendCount || RANK_MAX < friendUserNode._iRank || MAX_CONNECT_USER_COUNT < friendUserNode._idxBuffer)
						{
							I3BCLOG(LOCATION, "CTaskProcessor::_WorkForFriendControl()[UID(%I64d)][_iRank(%d)][_idxBuffer(%d)][FriendCnt(%d][Nick(%s)]",
									friendUserNode._i64DbIdx, friendUserNode._iRank, friendUserNode._idxBuffer, friendUserNode._friendCount, friendUserNode._nick);	
						}
						#endif

						i3mem::Copy(&friendUserNode._afriend[friendUserNode._ui32friendCount], &pData->friendNode, sizeof(FRIEND_NODE));
						ui8idxFriend = (UINT8)friendUserNode._ui32friendCount;
						friendUserNode._ui32friendCount++;

						#ifdef DEF_TEST_LOG
						// 임시
						if(FRIEND_MAX_COUNT < friendUserNode._friendCount || RANK_MAX < friendUserNode._iRank || MAX_CONNECT_USER_COUNT < friendUserNode._idxBuffer)
						{
							I3BCLOG(LOCATION, "CTaskProcessor::_WorkForFriendControl()[UID(%I64d)][_iRank(%d)][_idxBuffer(%d)][FriendCnt(%d][Nick(%s)]",
									friendUserNode._i64DbIdx, friendUserNode._iRank, friendUserNode._idxBuffer, friendUserNode._friendCount, friendUserNode._nick);	
						}
						#endif
					}

					// 변경 시, 등록 대기 상태
					if (FRIEND_CHANGE_STATE_CHANGE != ui8changeState || FRIEND_STATE_HOLD != GET_FRIEND_STATE(friendUserNode._afriend[ui8idxFriend]._i32state))
					{
						UINT8 ui8length = (UINT8)(i3String::Length(pData->friendNode._strnick) + 1);

						FRIEND_INFO info;
						info._fuid = pData->friendNode._i64fuid;
						info._rank = pData->friendNode._ui8rank;
						info._nickcolor = pData->friendNode._ui8nickcolor;

						if (FRIEND_CHANGE_STATE_INSERT == ui8changeState)
						{
							info._state = pData->friendNode._i32state;
						}
						else
						{
							friendUserNode._afriend[ui8idxFriend]._i32state = node._ui32connectState;
							pData->friendNode._i32state = node._ui32connectState;
							info._state = node._ui32connectState;
						}

						// Update
						i3mem::Copy(friendUserNode._afriend[ui8idxFriend]._strnick, pData->friendNode._strnick, sizeof(TTCHAR) * ui8length);
						friendUserNode._afriend[ui8idxFriend]._strnick[ui8length - 1] = '\0';

						friendUserNode._afriend[ui8idxFriend]._ui8nickcolor = pData->friendNode._ui8nickcolor;

						// 닉네임 저장
						m_pUserFinder->UpdateFriendListNickName(friendUserNode._i64UID, ui8idxFriend, friendUserNode._afriend[ui8idxFriend]._strnick, pData->friendNode._ui8nickcolor);

						i3NetworkPacket packet(PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK);
						MakePacket_OGame2Mess_Friend_Info_Change_Ack(&packet,
																	 friendUserNode._i32sessionidx,
																	 friendUserNode._i64UID,
																 	 ui8changeState,
																	 ui8idxFriend,
																	 ui8length,
																	 pData->friendNode._strnick,
																	 info);
						// Send Game Server
						g_pGateway->SendPacket(friendUserNode._i32serveridx, &packet);

						m_pUserFinder->UpdateByUid(&friendUserNode);
					}
				}

				#ifdef DEF_TEST_LOG
				// 임시
				if(FRIEND_MAX_COUNT < node._friendCount || RANK_MAX < node._iRank || MAX_CONNECT_USER_COUNT < node._idxBuffer)
				{
					I3BCLOG(LOCATION, "CTaskProcessor::_WorkForFriendControl()[UID(%I64d)][_iRank(%d)][_idxBuffer(%d)][FriendCnt(%d][Nick(%s)]",
							node._i64DbIdx, node._iRank, node._idxBuffer, node._friendCount, node._nick);	
				}
				#endif

				i3mem::Copy(&node._afriend[node._ui32friendCount], &pData->targetFriendNode, sizeof(FRIEND_NODE));
				node._ui32friendCount++;

				#ifdef DEF_TEST_LOG
				// 임시
				if(FRIEND_MAX_COUNT < node._friendCount || RANK_MAX < node._iRank || MAX_CONNECT_USER_COUNT < node._idxBuffer)
				{
					I3BCLOG(LOCATION, "CTaskProcessor::_WorkForFriendControl()[UID(%I64d)][_iRank(%d)][_idxBuffer(%d)][FriendCnt(%d][Nick(%s)]",
							node._i64DbIdx, node._iRank, node._idxBuffer, node._friendCount, node._nick);
				}
				#endif

				ui8changeState = FRIEND_CHANGE_STATE_INSERT;
				UINT8 ui8length = (UINT8)(i3String::Length(pData->targetFriendNode._strnick) + 1);
				ui8idxFriend = (UINT8)(node._ui32friendCount - 1);

				FRIEND_INFO info;
				info._fuid = pData->targetFriendNode._i64fuid;
				info._rank = pData->targetFriendNode._ui8rank;
				info._state = pData->targetFriendNode._i32state;
				info._nickcolor = pData->targetFriendNode._ui8nickcolor;

				i3NetworkPacket packet(PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK);
				MakePacket_OGame2Mess_Friend_Info_Change_Ack(&packet,
															 node._i32sessionidx,
															 node._i64UID,
															 ui8changeState,
															 ui8idxFriend,
															 ui8length,
															 pData->targetFriendNode._strnick,	
															 info);
				// Send Game Server
				g_pGateway->SendPacket(node._i32serveridx, &packet);

				m_pUserFinder->UpdateByUid(&node);
			}
		}
		break;
	}

	return; 
}


#ifdef USE_CHANGE_NICK
// Change Nick 
void CTaskProcessor::_OutWorkForChangeNick(void)
{
	USER_NODE node;

	OUT_CHANGE_NICK* pOutData = g_pDataBase->GetOutChangeNickData();
	if (NULL == pOutData)
	{
		// 로그 따로 남길 것.
		g_pLog->WriteLog( L"_OutWorkForChangeNickAndRank() : Invalid pData"  ); 
		return;
	}

	switch(pOutData->eWorktype)
	{
	case FRIEND_CHANGE_NICKCOLOR:
	case FRIEND_CHANGE_NICK:
		{
			// 온라인 상태인 친구에게만 자신의 상태 변경을 알려준다. 
			if( !m_pUserFinder->FindUid(pOutData->i64uid, &node) )	return; 

			USER_NODE friendNode;

			if( !m_pUserFinder->FindUid(pOutData->i64fuid, &friendNode))	return;
			
			// 친구의 친구목록에서 자신의 정보를 찾는다.
			for(UINT32 j = 0; j < friendNode._ui32friendCount; j++)
			{
				if (friendNode._afriend[j]._i64fuid != pOutData->i64uid) continue;

				// 변경 내역 전송
				UINT8 ui8changeState	= FRIEND_CHANGE_STATE_CHANGE;
				UINT8 ui8friendIdx		= (UINT8)j;

				FRIEND_INFO info;
				info._fuid  = friendNode._afriend[j]._i64fuid;
				info._rank  = friendNode._afriend[j]._ui8rank;
				info._state = friendNode._afriend[j]._i32state;
				info._nickcolor = friendNode._afriend[j]._ui8nickcolor = pOutData->ui8NickColor;

				// Update
				UINT8 ui8NickSendSize = (UINT8)(i3String::Length( pOutData->strNick ) + 1);
				i3mem::Copy(friendNode._afriend[j]._strnick, pOutData->strNick, sizeof(TTCHAR) * ui8NickSendSize);
				friendNode._afriend[j]._strnick[ui8NickSendSize - 1] = '\0';

				// 닉네임 저장
				m_pUserFinder->UpdateFriendListNickName(friendNode._i64UID, j, friendNode._afriend[j]._strnick, pOutData->ui8NickColor);

				if ( FRIEND_STATE_OFFLINE != GET_FRIEND_STATE(node._afriend[pOutData->idx]._i32state) )
				{
					// Data
					i3NetworkPacket packet(PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK);
					MakePacket_OGame2Mess_Friend_Info_Change_Ack(&packet,
						friendNode._i32sessionidx,
						friendNode._i64UID,
						ui8changeState,
						ui8friendIdx,
						ui8NickSendSize,
						friendNode._afriend[j]._strnick,
						info);
					// Send Game Server
					g_pGateway->SendPacket(friendNode._i32serveridx, &packet);
				}
				break;
			}

		}
		break;
	}

	return;
}

#endif

#ifdef USE_CHANGE_RANK
// Change Rank
void CTaskProcessor::_OutWorkForChangeRank(void)
{
	USER_NODE node;

	OUT_CHANGE_RANK* pOutData = g_pDataBase->GetOutChangeRankData();
	if (NULL == pOutData)
	{
		// 로그 따로 남길 것.
		g_pLog->WriteLog( L"_OutWorkForChangeRank() : Invalid pData"   ); 
		return;
	}

	switch(pOutData->type)
	{
	case FRIEND_CHANGE_RANK:
		{
			// 온라인 상태인 친구에게만 자신의 상태 변경을 알려준다.			
			if( !m_pUserFinder->FindUid(pOutData->i64uid, &node) )return;
			//if( FRIEND_STATE_ONLINE <= GET_FRIEND_STATE(node._friend[pOutData->idx]._state) )
			//{
				USER_NODE friendNode;

				if( !m_pUserFinder->FindUid(pOutData->i64fuid, &friendNode))
				{
					//g_pLog->WriteLog(m_i32TaskIdx, "_OutWorkForChangeNickAndRank() : OffLine User"  ); 
					return;
				}

				#ifdef DEF_TEST_LOG
				// 임시
				if(FRIEND_MAX_COUNT < friendNode._friendCount || RANK_MAX < friendNode._iRank || MAX_CONNECT_USER_COUNT < friendNode._idxBuffer)
				{
					I3BCLOG(LOCATION, "CTaskProcessor::_OutWorkForChangeNick()[UID(%I64d)][_iRank(%d)][_idxBuffer(%d)][FriendCnt(%d][Nick(%s)]",
							friendNode._i64DbIdx, friendNode._iRank, friendNode._idxBuffer, friendNode._friendCount, friendNode._nick);	
				}
				#endif

				// 친구의 친구목록에서 자신의 정보를 찾는다.
				for(UINT32 j = 0; j < friendNode._ui32friendCount; j++)
				{
					if (friendNode._afriend[j]._i64fuid == pOutData->i64uid)
					{
						// 변경 내역 전송
						UINT8 ui8changeState	= FRIEND_CHANGE_STATE_CHANGE;
						UINT8 ui8friendIdx		= (UINT8)j;
						UINT8 ui8NickSendSize	= (UINT8)(i3String::Length(friendNode._afriend[j]._strnick) + 1);

						// Update
						friendNode._afriend[j]._ui8rank = pOutData->rank;

						// Set Date
						FRIEND_INFO info;
						info._fuid		= friendNode._afriend[j]._i64fuid;
						info._rank		= friendNode._afriend[j]._ui8rank;
						info._state		= friendNode._afriend[j]._i32state;
						info._nickcolor	= friendNode._afriend[j]._ui8nickcolor;

						// Rank 저장
						m_pUserFinder->UpdateFriendListRank(friendNode._i64UID, j, info._rank);

						if( FRIEND_STATE_OFFLINE != GET_FRIEND_STATE(node._afriend[pOutData->idx]._i32state) )
						{
							i3NetworkPacket packet(PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK);
							MakePacket_OGame2Mess_Friend_Info_Change_Ack(&packet,
																		friendNode._i32sessionidx,
																		friendNode._i64UID,
																		ui8changeState,
																		ui8friendIdx,
																		ui8NickSendSize,
																		friendNode._afriend[j]._strnick,
																		info);
							// Send Game Server
							g_pGateway->SendPacket(friendNode._i32serveridx, &packet);
						}
						break;
					}
				}
			//}
		}
		break;
	}

	return;
}
#endif
