#include "pch.h"
#include "TaskProcessor.h"
#include "PacketLocker.h"
#include "Gateway.h"
#include "DataBase.h"
#include "Receiver.h"
#include "ConnectedUser.h"
#include "NoteBox.h"

I3_CLASS_INSTANCE(CTaskProcessor, i3Thread);

CTaskProcessor* g_pTaskProcessor = NULL;

CTaskProcessor::CTaskProcessor()
{
	m_bIsRunning		= FALSE;

	m_pui8RecvBuffer	= NULL;
	m_pui8SendBuffer	= NULL;
	m_pPacketLocker		= NULL;
	m_pUserFinder		= NULL;

	m_ui32DataSize		= 0;
	m_ui32ReadedLength	= 0;
	m_ui32CheckUserIdx	= 0;

	m_pUserFinder		= NULL;
	m_pPacketLocker		= NULL;
	m_pGateway			= NULL;

	i3mem::FillZero(m_strLogBuffer, sizeof(char) * MAX_STRING_COUNT);

#ifdef USE_CHANGE_NICK	
	m_ppI_ChangeNick	= NULL;
#endif
#ifdef USE_CHANGE_RANK	
	m_ppI_ChangeRank	= NULL;
#endif
	m_ppI_ClanGroupNote	= NULL;

#ifdef USE_CHANGE_NICK
	m_ui8CurrentChangeNickCount		= 0;
	m_i32DelayWoringChangeNickCount	= 0;
#endif
#ifdef USE_CHANGE_RANK
	m_ui8CurrentChangeRankCount		= 0;
	m_i32DelayWoringChangeRankCount	= 0;
#endif
	m_ui32CurrentGroupCount			= 0;
	m_i32DelayWoringGroupCount		= 0;

}

CTaskProcessor::~CTaskProcessor()
{
	OnDestroy();
}

BOOL CTaskProcessor::Create(UINT32 ui32DataSize,CPacketLocker* pPacketLocker,CGateway* pGateway)
{
	I3ASSERT(NULL == m_pui8RecvBuffer);
	I3ASSERT(NULL == m_pui8SendBuffer);
	I3ASSERT(ui32DataSize);
	I3ASSERT(pPacketLocker);
	I3ASSERT(pGateway);


	g_pLog->WriteLog( L"[BEGIN] Task Create"  );

	m_ui32CheckUserIdx	= 0;
	m_pui8RecvBuffer	= (UINT8*)i3MemAlloc(ui32DataSize);
	m_pui8SendBuffer	= (UINT8*)i3MemAlloc(ui32DataSize);
	m_ui32DataSize		= ui32DataSize;
	m_pPacketLocker		= pPacketLocker;
	m_pGateway			= pGateway;

	g_pLog->WriteLog( L"[BEGIN] UserFinder"  );
	m_pUserFinder = CUserFinder::NewObject();
	if(!m_pUserFinder->Create() )return FALSE;

	g_pLog->WriteLog( L"[SUCC] UserFinder"  );
#ifdef USE_CHANGE_NICK	
	// Change Nick
	m_ppI_ChangeNick	= new Local_CRingBuffer;
#endif
#ifdef USE_CHANGE_RANK
	// Change Rank	
	m_ppI_ChangeRank	= new Local_CRingBuffer;
#endif
	// Clan Group Note
	m_ppI_ClanGroupNote	= new Local_CRingBuffer;

#ifdef USE_CHANGE_NICK
	// Change Nick
	if( !m_ppI_ChangeNick->OnCreate( sizeof(IN_CHANGE_NICK), UPDATE_NICK_BUFFER_COUNT ) )return FALSE;
#endif
#ifdef USE_CHANGE_RANK
	// Change Rank
	if( !m_ppI_ChangeRank->OnCreate( sizeof(IN_CHANGE_RANK), UPDATE_RANK_BUFFER_COUNT ) )return FALSE;
#endif
	// Clan Group Note
	if( !m_ppI_ClanGroupNote->OnCreate( sizeof(OGame2Mess_Clan_Group_Note_Send_Req), CLAN_GROUP_NOTE_BUFFER_COUNT ) )return FALSE;	

	if(!i3Thread::Create("TaskProcessor", 0, 4096, NULL, PRIORITY_HIGHEST))return FALSE;
	
	g_pLog->WriteLog( L"[SUCC] Task Create"  );
	return TRUE;
}

void CTaskProcessor::OnDestroy(void)
{
	I3MEM_SAFE_FREE( m_pui8RecvBuffer );
	I3MEM_SAFE_FREE( m_pui8SendBuffer );
	I3_SAFE_RELEASE( m_pUserFinder );

#ifdef USE_CHANGE_NICK	
	// Change Nick
	if( m_ppI_ChangeNick   != NULL )	I3_SAFE_DELETE( m_ppI_ChangeNick );
#endif
	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
#ifdef USE_CHANGE_RANK
	// Change Rank
	if( m_ppI_ChangeRank   != NULL )	I3_SAFE_DELETE( m_ppI_ChangeRank );
#endif
	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
	// Clan Group Note
	if( m_ppI_ClanGroupNote   != NULL )	I3_SAFE_DELETE( m_ppI_ClanGroupNote );
	return; 
}

INT32 CTaskProcessor::GetUserCount(void)
{
	return m_pUserFinder->GetUserCount();
}

BOOL CTaskProcessor::_AddUser(UINT32 ui32ServerIdx, UINT32 ui32SessionIdx, INT64 i64UID, INT32 i32Rank, TTCHAR * pstrNickName)
{
	////////////////////////////////////////////////////////////////////////////////
	//동접자 리스트에 등록
	USER_NODE node;
	i3mem::FillZero(&node, sizeof(USER_NODE));

	node._i64UID			= i64UID;
	node._i32serveridx		= ui32ServerIdx;
	node._i32sessionidx		= ui32SessionIdx;
	node._ui32Rank			= i32Rank;
	node._ui32connectState	= MAKE_FRIEND_STATE(FRIEND_STATE_ONLINE, ui32ServerIdx, 0, 0);
	node._ui32friendCount	= 0;
#if defined( USE_BLOCK_LIST )
	node._ui32BlockCount	= 0;
#endif
	node._ui32LastRecvTimeST	= i3ThreadTimer::GetServerTime();

	// 닉네임 길이 검사
	INT32 i32len = i3String::Length(pstrNickName);
	if(NET_NICK_NAME_SIZE <= i32len)
	{
		I3BCLOG(LOCATION, "CTaskProcessor::_AddUser(iServerIdx(%d),iSessionIdx(%d),iUID(%I64d),Rank(%d),Nick(%s))[Len(%d)]",ui32ServerIdx,ui32SessionIdx,i64UID, i32Rank, pstrNickName, i32len);	
	}

	i3mem::FillZero(node._strnick, NET_NICK_NAME_SIZE);
	i3String::NCopy(node._strnick, pstrNickName, NET_NICK_NAME_SIZE-1);

	if( !m_pUserFinder->Insert(&node) )
	{		
		g_pLog->WriteLog( L"AddUser2 Error ServerIdx = %u SessionIdx = %u UID = %I64d ",ui32ServerIdx, ui32SessionIdx, i64UID  );
		return FALSE;
	}

	if( 0 > node._i64UID )
	{		
		g_pLog->WriteLog( L"[EMERGENCY ERROR] CTaskProcessor::_AddUser() : ServerIdx = %u, SessionIdx = %u, UID = %I64d, node.UID = %I64d ",ui32ServerIdx, ui32SessionIdx, i64UID, node._i64UID   );
		return FALSE;
	}

	// 친구 리스트를 받습니다.
	{
		In_FriendList inData;
		inData.i64uid	= node._i64UID;
		inData.iRank	= node._ui32Rank;
		inData.bReset	= FALSE;

#ifdef ACTIVATE_FRIEND_DATABASE
		if ( !g_pDataBase->InFriendList( &inData))
		{
			g_pLog->WriteLog( L"[ERROR] CTaskProcessor::_AddUser() : Friend List buffer is Full (user info) -> uid(%I64d)", node._i64UID  );
		}
#endif 
	}

#if defined( USE_BLOCK_LIST )
	// 차단 리스트를 받습니다.
	{
		In_BlockList inData;
		inData._i64uid		= node._i64UID;

		if ( !g_pDataBase->InBlockList( &inData))
		{
			g_pLog->WriteLog( L"[ERROR] CTaskProcessor::_AddUser() : Block List buffer is Full (user info) -> uid(%I64d)", node._i64UID  );
		}
	}
#endif

#ifdef ACTIVATE_NOTE_DATABASE
	g_pDataBase->InNoteList( node._i64UID );
#endif

	return TRUE;
}

BOOL CTaskProcessor::_RemoveUser(UINT32 ui32ServerIdx, INT64 i64UID)
{
	USER_NODE TempNode;
	if( !m_pUserFinder->FindUid(i64UID, &TempNode) )
	{
		//g_pLog->WriteLog(m_i32TaskIdx, "Remove User Error ServerIdx = %u UID = %I64d", iServerIdx, i64UID );
		return FALSE; 
	}

	// 적용된 내용을 친구들에게 전송
	m_pUserFinder->ChangeState(i64UID, MAKE_FRIEND_STATE(FRIEND_STATE_OFFLINE, 0, 0, 0));
	_ProcessFriendInfo(i64UID);
	m_pUserFinder->Delete(ui32ServerIdx, i64UID);

	return TRUE;
}

UINT32 CTaskProcessor::OnRunning(void* pUserData)
{
	m_i32LogIdx = g_pLog->AddLogThread(L"TSPR", 0, m_ThreadID);

	BOOL bIsWork;
	m_bIsRunning = TRUE;

	while(m_bIsRunning)
	{
		bIsWork = FALSE;

		for(UINT32 idx = 0; idx < m_pPacketLocker->getCount(); idx++)
		{
			m_ui32ReadedLength = m_pPacketLocker->Pop(idx, m_pui8RecvBuffer, m_ui32DataSize);
			if(m_ui32ReadedLength == 0)continue;
			_DoTaskForGameDataBase(idx);
			bIsWork = TRUE;
		}

		//-------------------------------------------------------------------------------------------------
		//DataBase에서 받아온 일 처리

		// Friend
		if( g_pDataBase->GetOutFriendListCount() > 0)
		{
			_WorkForFriendList();
			bIsWork = TRUE;
		}
		if( g_pDataBase->GetOutFriendControlCount() > 0)
		{
			_WorkForFriendControl();
			bIsWork = TRUE;
		}

		// Note
		if( g_pDataBase->GetOutNoteListCount() > 0)
		{
			_WorkForNoteList();
			bIsWork = TRUE;
		}
		if( g_pDataBase->GetOutNoteCommandCount() > 0)
		{
			_WorkForNoteCommand(); //종혁
			bIsWork = TRUE;
		}

#if defined( USE_BLOCK_LIST )
		// Block
		if( g_pDataBase->GetOutBlockListCount() > 0)
		{
			_WorkForBlockList();
			bIsWork = TRUE;
		}
		if( g_pDataBase->GetOutBlockControlCount() > 0)
		{
			_WorkForBlockControl();
			bIsWork = TRUE;
		}
#endif


#ifdef USE_CHANGE_NICK
		// Change Nick
		if( g_pDataBase->GetOutChangeNickCount() > 0)
		{
			_OutWorkForChangeNick();
			bIsWork = TRUE; 
		}

		if( 0 < _GetInChangeNick() &&
			DB_BUFFER_COUNT > _GetInChangeNick() &&
			0 >= g_pDataBase->GetInChangeNickCount())
		{			
			if (m_i32DelayWoringChangeNickCount > DELAY_WORKING)
			{
				m_i32DelayWoringChangeNickCount = 0;
				_InWorkForChangeNick();
				bIsWork = TRUE;
			}
			m_i32DelayWoringChangeNickCount++;
		}

#endif
#ifdef USE_CHANGE_RANK
		// Change Rank
		if( g_pDataBase->GetOutChangeRankCount() > 0)
		{
			_OutWorkForChangeRank();
			bIsWork = TRUE;
		}

		if( 0 < _GetInChangeRank() && 
			DB_BUFFER_COUNT > _GetInChangeRank() &&
			0 >= g_pDataBase->GetInChangeRankCount())
		{
			if (m_i32DelayWoringChangeRankCount > DELAY_WORKING)
			{
				m_i32DelayWoringChangeRankCount = 0;
				_InWorkForChangeRank();
				bIsWork = TRUE;
			}
			m_i32DelayWoringChangeRankCount++;
		}
#endif

		// Clan Group Note

		if( g_pDataBase->GetOutClanGroupNoteCommandCount() > 0)
		{
			_OutWorkForClanGroupNoteCommand();
			bIsWork = TRUE;
		}

		if( 0 < _GetInClanGroupNoteCount() && 
			DB_BUFFER_COUNT > _GetInClanGroupNoteCount() &&
			0 >= g_pDataBase->GetInClanGroupNoteCommandCount())
		{
			if (m_i32DelayWoringGroupCount > DELAY_WORKING)
			{
				m_i32DelayWoringGroupCount = 0;
				_InWorkForClanGroupNoteCommand();
				bIsWork = TRUE; 
			}
			m_i32DelayWoringGroupCount++;
		}


#ifdef USE_MESS_FIND
		//User Search
		if( g_pDataBase->GetOutUserCount() > 0)
		{
			OUT_USER_INFO_SEARCH *  pOutData = g_pDataBase->GetOutUserSearch(); 
			if( NULL == pOutData )
			{
				g_pLog->WriteLog( L"[TASKPROCESSOR] User Find pOutData NULL" );
			}
			else
			{
						INT32 i32ThisResult = EVENT_ERROR_SUCCESS;	// 이 안에서의 처리 결과 

						USER_NODE	RUserNode;	// 찾기 요청한 유저 노드
						USER_NODE	FUserNode;	// 찾고자 하는 유저 노드

						i3NetworkPacket sendPacket(PROTOCOL_MESSENGER_FIND_USER_ACK);

						// 찾기 요청한 유저 노드 검색.
						if (TRUE == m_pUserFinder->FindUid(pOutData->i64UID, &RUserNode))
						{
							// 0, 이전 단계들에서 찾기 실패했으면 실패패킷 보냄.
							if ( EV_FAILED ( pOutData->i32Reault  ) )
							{
								sendPacket.WriteData(&RUserNode._i32sessionidx,	sizeof(INT32));
								sendPacket.WriteData(&RUserNode._i64UID,		sizeof(INT64));
								sendPacket.WriteData(&pOutData->i32Reault,		sizeof(INT32));
								SendPacketMessage(RUserNode._i32serveridx, &sendPacket);
							}
							else
							{
								// 찾고자 하는  유저 노드 검색.
								if (TRUE == m_pUserFinder->FindUid(pOutData->i64SearchUID, &FUserNode))
								{
									// 두 유저가 같은 Game 서버에 있다면 해당 Game 서버에 바로 결과 보낸다.
									if(RUserNode._i32serveridx == FUserNode._i32serveridx)
									{
										sendPacket.WriteData(&RUserNode._i32sessionidx,	sizeof(INT32));	// 찾기 요청한 유저 Game 서버 세션 Idx
										sendPacket.WriteData(&RUserNode._i64UID,		sizeof(INT64)); // 찾기 요청한 유저 UID.
										sendPacket.WriteData(&pOutData->i32Reault,		sizeof(INT32));	// 찾기 결과. EVENT_ERROR_SUCCESS여야함.
										sendPacket.WriteData(&FUserNode._i64UID,		sizeof(INT64));	// 찾고자 하는 유저 UID.
										sendPacket.WriteData(&FUserNode._i32sessionidx,	sizeof(INT32));	// 찾고자 하는 유저 Game 서버 세션 Idx
										sendPacket.WriteData(&FUserNode._ui32connectState,	sizeof(UINT32));// 찾고하 하는 유저 상태.( 상태, 서버Idx, 채널Idx, 룸Idx 정보)

										SendPacketMessage(RUserNode._i32serveridx, &sendPacket);	// 찾기 요청한 유저가 접속중인 Game서버에 패킷 전송.
									}
									else	// 두유저가 다른 Game서버에 있다면 찾고자 하는 유저가 있는 Game 서버에 정보를 요청한다.
									{

										i3NetworkPacket sendPacketFUser(PROTOCOL_MESSENGER_FUSER_INFO_REQ);
										sendPacketFUser.WriteData(&RUserNode._i32sessionidx,		sizeof(INT32));	// 찾기 요청한 유저 Game 서버 세션 Idx
										sendPacketFUser.WriteData(&RUserNode._i64UID,				sizeof(INT64)); // 찾기 요청한 유저 UID.
										sendPacketFUser.WriteData(&FUserNode._i32sessionidx,		sizeof(INT32));	// 찾고자 하는 유저의 Game 서버 세션 Idx
										sendPacketFUser.WriteData(&FUserNode._i64UID,				sizeof(INT64)); // 찾고자 하는 유저의 UID.

										SendPacketMessage(FUserNode._i32serveridx, &sendPacketFUser);	// 찾고자 하는 유저가 접속중인 Game서버에 패킷 전송.
									}

								}
								else i32ThisResult = EVENT_ERROR_NEW_USER_FIND_FUSER_OFFLINE;	// 찾고자 하는 유저가 오프라인이다.
							}
						}
						// else // 없으면 답없다.ㅡㅡ;;

						// 윗단에서 실패
						if ( EV_FAILED ( i32ThisResult  ) )
						{
							sendPacket.WriteData(&RUserNode._i32sessionidx,	sizeof(INT32));
							sendPacket.WriteData(&pOutData->i64UID,			sizeof(INT64));
							sendPacket.WriteData(&i32ThisResult,			sizeof(INT32));
							SendPacketMessage(RUserNode._i32serveridx, &sendPacket);
						}
			}
			g_pDataBase->OutUserSearchIdx();
			bIsWork = TRUE; 
		}
#endif
		//-------------------------------------------------------------------------------------------------
		// User Update Time Check 
		m_ui32CheckUserIdx++; 
		if( m_pUserFinder->GetMaxUserCount() <= m_ui32CheckUserIdx	) m_ui32CheckUserIdx = 0;
		USER_NODE * pUserNode = m_pUserFinder->GetUserNode( m_ui32CheckUserIdx ); 

		if(pUserNode->_i64UID != 0)
		{
			if( pUserNode->_ui32LastRecvTimeST < i3ThreadTimer::GetServerTime() - (60 * 30)  )//30분
			{
				//LogOut User
				if( m_pUserFinder->IsAliveUser(pUserNode->_i64UID) )
				{
					g_pLog->WriteLog( L"Kill User AlivePacket TimeOut Server = %d UserIdx = %d DbIdx = %I64d", pUserNode->_i32serveridx, pUserNode->_i32sessionidx, pUserNode->_i64UID );
					_RemoveUser(pUserNode->_i32serveridx, pUserNode->_i64UID);
				}
				else
				{
					g_pLog->WriteLog( L"Kill User AlivePacket TimeOut1 Server = %d UserIdx = %d DbIdx = %I64d", pUserNode->_i32serveridx, pUserNode->_i32sessionidx, pUserNode->_i64UID );
				}
			}
		}

		if(!bIsWork)::Sleep(1);
	}

	return 0;
}

BOOL CTaskProcessor::SendPacketMessage(UINT32 ui32idx,i3NetworkPacket* pPacket)
{
	return m_pGateway->SendPacket(ui32idx, pPacket);
}

void CTaskProcessor::_ProcessFriendInfo(INT64 i64UID)
{
	// 접속 중인 모든 친구에게 변경을 통보한다.
	USER_NODE node;

	if( !m_pUserFinder->FindUid(i64UID, &node) )return;

#ifdef DEF_TEST_LOG
	// 임시
	if(FRIEND_MAX_COUNT < node._friendCount || RANK_MAX < node._iRank || MAX_CONNECT_USER_COUNT < node._idxBuffer)
	{
		I3BCLOG(LOCATION, "CTaskProcessor::_ProcessFriendInfo()[UID(%I64d)][_iRank(%d)][_idxBuffer(%d)][FriendCnt(%d][Nick(%s)]",
			node._i64DbIdx, node._iRank, node._idxBuffer, node._friendCount, node._nick);	
	}
#endif

	for(UINT32 i = 0; i < node._ui32friendCount; i++)
	{
		if( FRIEND_STATE_ONLINE <= GET_FRIEND_STATE(node._afriend[i]._i32state) )
		{
			USER_NODE friendNode;

			if( !m_pUserFinder->FindUid(node._afriend[i]._i64fuid, &friendNode))continue; 

#ifdef DEF_TEST_LOG
			// 임시
			if(FRIEND_MAX_COUNT < friendNode._friendCount || RANK_MAX < friendNode._iRank || MAX_CONNECT_USER_COUNT < friendNode._idxBuffer)
			{
				I3BCLOG(LOCATION, "CTaskProcessor::OnRunning()[UID(%I64d)][_iRank(%d)][_idxBuffer(%d)][FriendCnt(%d][Nick(%s)]",
					friendNode._i64DbIdx, friendNode._iRank, friendNode._idxBuffer, friendNode._friendCount, friendNode._nick);	
			}
#endif

			// 친구의 친구목록에서 자신의 정보를 찾는다.
			for(UINT32 j = 0; j < friendNode._ui32friendCount; j++)
			{
				if (friendNode._afriend[j]._i64fuid == i64UID)
				{
					// 변경 내역 전송
					UINT8 ui8changeState	= FRIEND_CHANGE_STATE_CHANGE;
					UINT8 ui8friendIdx		= (UINT8)j;
					UINT8 ui8nickSize		= (UINT8)i3String::Length(friendNode._afriend[j]._strnick) + 1;

					FRIEND_INFO info;
					info._fuid		= friendNode._afriend[j]._i64fuid;
					info._rank		= friendNode._afriend[j]._ui8rank;
					info._state		= friendNode._afriend[j]._i32state;
					info._nickcolor = friendNode._afriend[j]._ui8nickcolor;

					i3NetworkPacket packet(PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK);
					MakePacket_OGame2Mess_Friend_Info_Change_Ack(&packet,
						friendNode._i32sessionidx,
						friendNode._i64UID,
						ui8changeState,
						ui8friendIdx,
						ui8nickSize,
						friendNode._afriend[j]._strnick,
						info);
					// Send Game Server
					g_pGateway->SendPacket(friendNode._i32serveridx, &packet);
					break;
				}
			}

		}
	}
}


//////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
// Change Nick
#ifdef USE_CHANGE_NICK
BOOL	CTaskProcessor::_InChangeNick(OGame2Mess_Change_NickName_Req* pReq )
{
	OGame2Mess_Change_NickName_Req * pChangeNick = (OGame2Mess_Change_NickName_Req *)m_ppI_ChangeNick->PushPointer(); 
	if(pChangeNick == NULL)return FALSE; 

	i3mem::Copy(pChangeNick, pReq, sizeof(OGame2Mess_Change_NickName_Req));		
	m_ppI_ChangeNick->PushPointerIdx(); 

	return TRUE;
}

// 변경된 자신의 닉네임을 자신의 목록에 있는 친구들에게 전송
INT32 CTaskProcessor::_InWorkForChangeNick(void)
{
	OGame2Mess_Change_NickName_Req	*	pInChangeNick;
	IN_CHANGE_NICK						InChangeNick;

	INT32				i32WorkCount = 0;

	pInChangeNick = (OGame2Mess_Change_NickName_Req*)	m_ppI_ChangeNick->Pop();
	if(pInChangeNick != NULL)
	{
		// 자신의 친구 목록을 찾아 업데이트를 실시한다.		
		USER_NODE * pNode = m_pUserFinder->GetFindUserNode(pInChangeNick->iUID);
		if(pNode == NULL)return FALSE;

		// Set IN_FRIEND_CONTROL 
		InChangeNick.eWorktype	= pInChangeNick->eWorkType;
		InChangeNick.i64uid		= pInChangeNick->iUID;
		InChangeNick.i64fuid	= pNode->_afriend[m_ui8CurrentChangeNickCount]._i64fuid;
		InChangeNick.idx		= m_ui8CurrentChangeNickCount;
		i3mem::Copy(InChangeNick.strNick, pInChangeNick->nickname, sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
		InChangeNick.ui8NickColor = pInChangeNick->ui8NickColor;

		if (0 < InChangeNick.i64fuid)
		{
			// 한명씩 변경된 자신의 닉네임을 전송
			if (!g_pDataBase->InChageNick(&InChangeNick))
			{
				g_pLog->WriteLog( L"_InWorkForChangeNick(m_inCurrentChangeNickCount(%d), SUID(%I64d), RUID(%I64d), IDX(%d), Nick(%s)) : LOCAL_RINGBUFFER Push Error ",
					m_ui8CurrentChangeNickCount, InChangeNick.i64uid, InChangeNick.i64fuid, InChangeNick.idx, InChangeNick.strNick  ); 
			}
		}

#ifdef DEF_TEST_LOG
		// 임시
		if(FRIEND_MAX_COUNT < pNode->_friendCount || RANK_MAX < pNode->_iRank || MAX_CONNECT_USER_COUNT < pNode->_idxBuffer)
		{
			I3BCLOG(LOCATION, "CTaskProcessor::OnRunning()[UID(%I64d)][_iRank(%d)][_idxBuffer(%d)][FriendCnt(%d][Nick(%s)]",
				pNode->_i64DbIdx, pNode->_iRank, pNode->_idxBuffer, pNode->_friendCount, pNode->_nick);	
		}
#endif

		m_ui8CurrentChangeNickCount++;
		if (m_ui8CurrentChangeNickCount >= pNode->_ui32friendCount)
		{
			m_ui8CurrentChangeNickCount = 0;
			m_ppI_ChangeNick->PopIdx();
		}

		i32WorkCount++;
	}

	return i32WorkCount;
}
#endif

//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
// Change Rank
#ifdef USE_CHANGE_RANK
BOOL	CTaskProcessor::_InChangeRank(OGame2Mess_Change_Rank_Req stGame2Mess_Change_Rank_Req)
{	
	OGame2Mess_Change_Rank_Req * pChangeRank = (OGame2Mess_Change_Rank_Req *)m_ppI_ChangeRank->PushPointer(); 
	if(pChangeRank == NULL)return FALSE; 

	i3mem::Copy(pChangeRank, &stGame2Mess_Change_Rank_Req, sizeof(OGame2Mess_Change_Rank_Req));
	m_ppI_ChangeRank->PushPointerIdx(); 

	return TRUE;
}

// 변경된 자신의 계급을 자신의 목록에 있는 친구들에게 전송
INT32 CTaskProcessor::_InWorkForChangeRank(void)
{
	OGame2Mess_Change_Rank_Req	*	pInChangeRank;
	IN_CHANGE_RANK					InChangeRank;

	INT32							i32WorkCount = 0; 	

	pInChangeRank = (OGame2Mess_Change_Rank_Req*)	m_ppI_ChangeRank->Pop();
	if(pInChangeRank != NULL)
	{		
		// 자신의 친구 목록을 찾아 업데이트를 실시한다.		
		USER_NODE * pNode = m_pUserFinder->GetFindUserNode(pInChangeRank->iUID);
		if(pNode == NULL)return FALSE; 	

		// Set IN_CHANGE_RANK 
		InChangeRank.type = FRIEND_CHANGE_RANK;
		InChangeRank.i64uid = pInChangeRank->iUID;
		InChangeRank.i64fuid = pNode->_afriend[m_ui8CurrentChangeRankCount]._i64fuid;
		InChangeRank.idx = m_ui8CurrentChangeRankCount;
		InChangeRank.rank = (UINT8)pInChangeRank->iRank;

		if (0 < InChangeRank.i64fuid)
		{
			// 한명씩 변경된 자신의 계급을 전송
			if (!g_pDataBase->InChangeRank(&InChangeRank))
			{
				g_pLog->WriteLog( L"_InWorkForChangeRank(m_inCurrentChangeRankCount(%d), SUID(%I64d), RUID(%I64d), IDX(%d), Rank(%d)) : LOCAL_RINGBUFFER Push Error ",
					m_ui8CurrentChangeRankCount, InChangeRank.i64uid, InChangeRank.i64fuid, InChangeRank.idx, InChangeRank.rank );
			}
		}

		// 로그 따로 남길 것.
		//g_pLog->WriteLog(m_i32TaskIdx,"m_inCurrentChangeRankCount(%d), Rank(%d), SUID(%I64d), RUID(%I64d), IDX(%d)", m_inCurrentChangeRankCount, InChangeRank.rank, InChangeRank.i64uid, InChangeRank.i64fuid, InChangeRank.idx);

#ifdef DEF_TEST_LOG
		// 임시
		if(FRIEND_MAX_COUNT < pNode->_friendCount || RANK_MAX < InChangeRank.rank)
		{
			I3BCLOG(LOCATION, "CTaskProcessor::OnRunning()[UID(%I64d)][_iRank(%d)][FriendCnt(%d]",
				InChangeRank.i64uid, InChangeRank.rank, pNode->_friendCount);	
		}
#endif

		m_ui8CurrentChangeRankCount++;
		if (m_ui8CurrentChangeRankCount >= pNode->_ui32friendCount)
		{
			m_ui8CurrentChangeRankCount = 0;
			m_ppI_ChangeRank->PopIdx();
		}

		i32WorkCount++;
	}

	return i32WorkCount;
}
#endif
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
// Clan Group Note
BOOL	CTaskProcessor::_InClanGroupNoteAddClan(INT8 i8Type, OGame2Mess_Clan_Group_Note_Send_Req stGame2Mess_Clan_Group_Note_Send_Req)
{
	OGame2Mess_Clan_Group_Note_Send_Req * pNoteCommand = (OGame2Mess_Clan_Group_Note_Send_Req *)m_ppI_ClanGroupNote->PushPointer(); 
	if(pNoteCommand == NULL)return FALSE; 

	i3mem::Copy(pNoteCommand, &stGame2Mess_Clan_Group_Note_Send_Req, sizeof(OGame2Mess_Clan_Group_Note_Send_Req));		
	m_ppI_ClanGroupNote->PushPointerIdx(); 

	return TRUE;
}


//그룹 쪽지를 하나 하나 전송하는 작업
INT32 CTaskProcessor::_InWorkForClanGroupNoteCommand(void)
{
	OGame2Mess_Clan_Group_Note_Send_Req	*	pInNoteCommand; 

	INT32				i32WorkCount = 0;
	INT8				i8MessageType = 0;

	pInNoteCommand = (OGame2Mess_Clan_Group_Note_Send_Req*) m_ppI_ClanGroupNote->Pop(); 
	if(pInNoteCommand != NULL)
	{			
		// OUT_NOTE_INFO 설정				
		i8MessageType = NOTE_COMMAND_TYPE_ADD_CLAN;
		if (pInNoteCommand->isAskNote)	i8MessageType = NOTE_COMMAND_TYPE_ADD_CLANASK; 

		if (!g_pDataBase->InClanGroupNoteAddClan(i8MessageType, m_ui32CurrentGroupCount, *pInNoteCommand))
		{
			g_pLog->WriteLog( L"_InWorkForClanGroupNoteCommand() : LOCAL_RINGBUFFER Push Error "); 
		}

		m_ui32CurrentGroupCount++;
		if (m_ui32CurrentGroupCount >= pInNoteCommand->iCount)
		{
			m_ui32CurrentGroupCount = 0;
			m_ppI_ClanGroupNote->PopIdx();
		}

		i32WorkCount++;
	}

	return i32WorkCount;
}

//---------------------------------------------------------------------------------------------------------------------------------

BOOL CTaskProcessor::_FindTatooWord(TTCHAR *pstrName)
{
	// 금지어 검사(스페이스)
	if (-1 != i3String::ContainChar(pstrName,_TT(' ')))
	{
		return FALSE;
	}

	return TRUE;
}

#if defined( USE_BLOCK_LIST )
BOOL CTaskProcessor::_CheckBlock( INT64 i64SenderUID, INT64 i64RecverUID )
{
	BOOL bIsBlock = FALSE;

	USER_NODE	node;
	if ( TRUE == m_pUserFinder->FindUid( i64RecverUID, &node))
	{
		INT32 i32BlockCount = (INT32)node._ui32BlockCount;

		if( i32BlockCount >= BLOCK_MAX_COUNT ) return bIsBlock;

		for( INT32 i = 0; i < i32BlockCount ; i++ )
		{
			if( i64SenderUID == node._pBlock[i]._i64buid )
			{
				bIsBlock = TRUE;
				break;
			}
		}

	}

	return bIsBlock;
}

BOOL CTaskProcessor::_CheckMyFriendByNick( USER_NODE* node, INT64 i64FriendUID )
{
	BOOL bIsFriend = FALSE;

	for( UINT8 i = 0; i < node->_friendCount ; i++ )
	{
		if( node->_friend[i]._i64fuid == i64FriendUID ) 	// 친구라면
		{
			bIsFriend = TRUE;
			break;
		}
	}

	return bIsFriend;
}
#endif