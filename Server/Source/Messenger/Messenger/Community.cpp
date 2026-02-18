#include "pch.h"
#include "Community.h"
#include "UserFinder.h"
#include "Database.h"
#include "Gateway.h"

I3_CLASS_INSTANCE(CCommunity, i3ElementBase);

CCommunity::CCommunity()
{
	/* Do nothing */
}

CCommunity::~CCommunity()
{
	/* Do nothing */
}

///		친구 목록을 불러온다.
///		불러온 목록을 전달한다.
///		자신의 현재상태를 친구들에게 전달한다.
/// param uid	요청 userId(dbIdx)
/// return		errorCode
///				EVENT_ERROR_SUCCESS						성공
///				ERROR_EVENT_FRIEND_INFO_PARAMETER_USER	잘못된 userId
///				ERROR_EVENT_FRIEND_INFO_QUERY			db error
INT32 CCommunity::MakeFriendList(UINT32 uid)
{
	USER_NODE node;

	if (FALSE == g_pUserFinder->FindUid(uid, &node))
	{
		//COMMON_LOG("[ERROR] MakeFriendList() - parameter_user");
		return ERROR_EVENT_FRIEND_INFO_PARAMETER_USER;
	}

	if (NET_ERROR_DB_QUERY == g_pDataBase->GetFriendList(uid, &node._friendCount, node._friend))
	{
		//COMMON_LOG("[ERROR] MakeFriendList() - query");
		return ERROR_EVENT_FRIEND_INFO_QUERY;
	}
	g_pUserFinder->UpdateByUid(&node);
	for(UINT32 i = 0; i < node._friendCount; i++)
	{
		if (FRIEND_STATE_OFFLINE == GET_FRIEND_STATE(node._friend[i]._state))
		{
			USER_NODE friendNode;

			if (g_pUserFinder->FindUid(node._friend[i]._fuid, &friendNode))
			{
				for(UINT32 j = 0; j < friendNode._friendCount; j++)
				{
					if (friendNode._friend[j]._fuid == uid)
					{
						node._friend[i]._state = friendNode._connectState;
						friendNode._friend[j]._state = node._connectState;

						UINT8 changeState = FRIEND_CHANGE_STATE_CHANGE;
						UINT8 length = (UINT8)(i3String::Length(friendNode._friend[j]._nick) + 1);
						UINT8 friendIdx = (UINT8)j;

						FRIEND_INFO info;
						info._fuid = friendNode._friend[j]._fuid;
						info._rank = friendNode._friend[j]._rank;
						info._state = friendNode._friend[j]._state;

						i3NetworkPacket friendPacket(PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK);
						friendPacket.WriteData(&friendNode._sessionidx,		sizeof(INT32));
						friendPacket.WriteData(&friendNode._iDbIdx,			sizeof(UINT32));
						friendPacket.WriteData(&changeState,				sizeof(UINT8));
						friendPacket.WriteData(&friendIdx,					sizeof(UINT8));
						friendPacket.WriteData(&length,						sizeof(UINT8));
						friendPacket.WriteData(friendNode._friend[j]._nick,	sizeof(char) * length);
						friendPacket.WriteData(&info,						sizeof(FRIEND_INFO));

						g_pGateway->SendPacket(friendNode._serveridx, &friendPacket);

						g_pUserFinder->UpdateByUid(&friendNode);
						break;
					}
				}
			}
		}
	}

	UINT8 friendCount = (UINT8)node._friendCount;

	i3NetworkPacket packet(PROTOCOL_MESSENGER_FRIEND_INFO_ACK);
	packet.WriteData(&node._sessionidx,		sizeof(INT32));
	packet.WriteData(&node._iDbIdx,			sizeof(UINT32));
	packet.WriteData(&friendCount,			sizeof(UINT8));
	
	for(UINT32 i = 0; i < node._friendCount; i++)
	{
		UINT8 length = (UINT8)(i3String::Length(node._friend[i]._nick) + 1);
		
		FRIEND_INFO info;
		info._fuid = node._friend[i]._fuid;
		info._rank = node._friend[i]._rank;
		info._state = node._friend[i]._state;

		packet.WriteData(&length,				sizeof(UINT8));
		packet.WriteData(node._friend[i]._nick,	sizeof(char) * length);
		packet.WriteData(&info,					sizeof(FRIEND_INFO));
	}

	g_pGateway->SendPacket(node._serveridx, &packet);

	g_pUserFinder->UpdateByUid(&node);

	return EVENT_ERROR_SUCCESS;
}

///		목록에 친구를 수락요청 상태로 추가한다.
///		상대 목록에 자신을 등록대기 상태로 추가한다.
///		상대 목록에 자신이 이미 친구로 등록되어 있다면, 상대의 현재 상태로 추가한다.
///		상대 목록에 자신이 수락요청 상태로 등록되어 있다면, 서로 현재 상태로 추가한다.
/// param uid	요청 userId(dbIdx)
/// param fuid	대상 userId(dbIdx)
/// return		errorCode
///				EVENT_ERROR_SUCCESS								성공
///				EVENT_ERROR_EVENT_FRIEND_INSERT_PARAMETER_USER	잘못된 userId
///				EVENT_ERROR_EVENT_FRIEND_INSERT_FULL			친구목록이 꽉찼다
///				EVENT_ERROR_EVENT_FRIEND_INSERT_ALREADY_FRIEND	대상이 이미 친구목록에 있다
///				EVENT_ERROR_EVENT_FRIEND_INSERT_QUERY			db error
INT32 CCommunity::AddFriend(UINT32 uid,UINT32 fuid)
{
	USER_NODE node;
	FRIEND_NODE friendNode;			// 요청자 정보
	FRIEND_NODE targetFriendNode;	// 목표 정보

	if (uid == fuid)
	{
		return EVENT_ERROR_EVENT_FRIEND_INSERT_PARAMETER_USER;
	}

	if (FALSE == g_pUserFinder->FindUid(uid, &node))
	{
		return EVENT_ERROR_EVENT_FRIEND_INSERT_PARAMETER_USER;
	}

	if (node._friendCount >= FRIEND_MAX_COUNT)
	{
		return EVENT_ERROR_EVENT_FRIEND_INSERT_FULL;
	}

	for(UINT32 i = 0; i < node._friendCount; i++)
	{
		if (node._friend[i]._fuid == fuid)
		{
			return EVENT_ERROR_EVENT_FRIEND_INSERT_ALREADY_FRIEND;
		}
	}

	INT32 dbResult = g_pDataBase->AddFriend(uid, fuid, &friendNode, &targetFriendNode);
	if (EVENT_ERROR_EVENT_FRIEND_INSERT_FULL == dbResult)
	{
		return EVENT_ERROR_EVENT_FRIEND_INSERT_FULL;
	}
	else if (0 > dbResult)
	{
		return EVENT_ERROR_EVENT_FRIEND_INSERT_QUERY;
	}


	USER_NODE friendUserNode;

	if (g_pUserFinder->FindUid(fuid, &friendUserNode))
	{
		if (FRIEND_STATE_OFFLINE <= GET_FRIEND_STATE(targetFriendNode._state))
		{
			targetFriendNode._state = friendUserNode._connectState;
		}

		// 자신이 상대 메신저에 등록되어있는지 확인한다.
		UINT8 idxFriend = 0;
		UINT8 changeState;
		BOOL bFound = FALSE;
		for(UINT32 i = 0; i < friendUserNode._friendCount; i++)
		{
			if (friendUserNode._friend[i]._fuid == (INT32)uid)
			{
				bFound = TRUE;
				idxFriend = (UINT8)i;
				break;
			}
		}

		// 있으면 상태를 변경하고, 없으면 새로 추가한다.
		if (bFound)
		{
			changeState = FRIEND_CHANGE_STATE_CHANGE;
		}
		else
		{
			changeState = FRIEND_CHANGE_STATE_INSERT;

			i3mem::Copy(&friendUserNode._friend[friendUserNode._friendCount], &friendNode, sizeof(FRIEND_NODE));
			idxFriend = (UINT8)friendUserNode._friendCount;
			friendUserNode._friendCount++;
		}

		// 변경 시, 등록 대기 상태
		if (FRIEND_CHANGE_STATE_CHANGE != changeState || FRIEND_STATE_HOLD != GET_FRIEND_STATE(friendUserNode._friend[idxFriend]._state))
		{
			UINT8 length = (UINT8)(i3String::Length(friendNode._nick) + 1);		

			FRIEND_INFO info;
			info._fuid = friendNode._fuid;
			info._rank = friendNode._rank;
			
			if (FRIEND_CHANGE_STATE_INSERT == changeState)
			{
				info._state = friendNode._state;
			}
			else
			{
				friendUserNode._friend[idxFriend]._state = node._connectState;
				friendNode._state = node._connectState;
				info._state = node._connectState;
			}

			i3NetworkPacket packet(PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK);
			packet.WriteData(&friendUserNode._sessionidx,	sizeof(INT32));
			packet.WriteData(&friendUserNode._iDbIdx,		sizeof(UINT32));
			packet.WriteData(&changeState,					sizeof(UINT8));
			packet.WriteData(&idxFriend,					sizeof(UINT8));
			packet.WriteData(&length,						sizeof(UINT8));
			packet.WriteData(friendNode._nick,				sizeof(char) * length);
			packet.WriteData(&info,							sizeof(FRIEND_INFO));

			g_pGateway->SendPacket(friendUserNode._serveridx, &packet);

			g_pUserFinder->UpdateByUid(&friendUserNode);
		}
	}
	
	i3mem::Copy(&node._friend[node._friendCount], &targetFriendNode, sizeof(FRIEND_NODE));
	node._friendCount++;

	UINT8 changeState = FRIEND_CHANGE_STATE_INSERT;
	UINT8 length = (UINT8)(i3String::Length(targetFriendNode._nick) + 1);
	UINT8 idxFriend = (UINT8)(node._friendCount - 1);

	FRIEND_INFO info;
	info._fuid = targetFriendNode._fuid;
	info._rank = targetFriendNode._rank;
	info._state = targetFriendNode._state;

	i3NetworkPacket packet(PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK);
	packet.WriteData(&node._sessionidx,			sizeof(INT32));
	packet.WriteData(&node._iDbIdx,				sizeof(UINT32));
	packet.WriteData(&changeState,				sizeof(UINT8));
	packet.WriteData(&idxFriend,				sizeof(UINT8));
	packet.WriteData(&length,					sizeof(UINT8));
	packet.WriteData(targetFriendNode._nick,	sizeof(char) * length);
	packet.WriteData(&info,						sizeof(FRIEND_INFO));

	g_pGateway->SendPacket(node._serveridx, &packet);

	g_pUserFinder->UpdateByUid(&node);

	return EVENT_ERROR_SUCCESS;
}


/// 수락대기 중인 친구를 수락한다.
///   수락한 상대의 현재 상태를 전달한다.
///   상대목록에 자신이 없다면 상대 상태를 오프라인으로 표시한다.
///   수락한 상대에게 자신의 현재 상태를 전달한다.
///   상대목록에 자신이 없다면 전달하지 않는다.
/// param uid	요청 userId(dbIdx)
/// param idx	친구목록의 수락 대상 배열 idx
/// return		error code
///				EVENT_ERROR_SUCCESS								성공
///				EVENT_ERROR_EVENT_FRIEND_ACCEPT_PARAMETER_USER	잘못된 uid
///				EVENT_ERROR_EVENT_FRIEND_ACCEPT_PARAMETER_IDX	잘못된 idx
///				EVENT_ERROR_EVENT_FRIEND_ACCEPT_QUERY			db error
INT32 CCommunity::AuthFriend(UINT32 uid,UINT32 idx)
{
	USER_NODE node;

	if (FALSE == g_pUserFinder->FindUid(uid, &node))
	{
		return EVENT_ERROR_EVENT_FRIEND_ACCEPT_PARAMETER_USER;
	}

	if (idx >= node._friendCount)
	{
		return EVENT_ERROR_EVENT_FRIEND_ACCEPT_PARAMETER_IDX;
	}

	if (0 > g_pDataBase->AuthFriend(uid, node._friend[idx]._fuid))
	{
		return EVENT_ERROR_EVENT_FRIEND_ACCEPT_QUERY;
	}


	// 로그를 위해 게임서버에 친구 수락통보해야한다
	{
		UINT8 state = FRIEND_CHANGE_STATE_ACCEPT; 
		UINT8 friendIdx = (UINT8)idx;

		i3NetworkPacket authPacket(PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK);
		
		authPacket.WriteData(&node._sessionidx,	sizeof(INT32));
		authPacket.WriteData(&node._iDbIdx,		sizeof(UINT32));
		authPacket.WriteData(&state,			sizeof(UINT8));
		authPacket.WriteData(&friendIdx,		sizeof(UINT8));

		g_pGateway->SendPacket(node._serveridx, &authPacket);
	}


	USER_NODE friendNode;

	node._friend[idx]._state = MAKE_FRIEND_STATE(FRIEND_STATE_OFFLINE, 0, 0, 0);

	if (g_pUserFinder->FindUid(node._friend[idx]._fuid, &friendNode))
	{
		for(UINT32 i = 0; i < friendNode._friendCount; i++)
		{
			if (friendNode._friend[i]._fuid == uid)
			{
				node._friend[idx]._state = friendNode._connectState;
				friendNode._friend[i]._state = node._connectState;

				UINT8 changeState = FRIEND_CHANGE_STATE_CHANGE;
				UINT8 length = (UINT8)(i3String::Length(friendNode._friend[i]._nick) + 1);
				UINT8 friendIdx = (UINT8)i;

				FRIEND_INFO info;
				info._fuid = friendNode._friend[i]._fuid;
				info._rank = friendNode._friend[i]._rank;
				info._state = friendNode._friend[i]._state;

				i3NetworkPacket friendPacket(PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK);
				friendPacket.WriteData(&friendNode._sessionidx,		sizeof(INT32));
				friendPacket.WriteData(&friendNode._iDbIdx,			sizeof(UINT32));
				friendPacket.WriteData(&changeState,				sizeof(UINT8));
				friendPacket.WriteData(&friendIdx,					sizeof(UINT8));
				friendPacket.WriteData(&length,						sizeof(UINT8));
				friendPacket.WriteData(friendNode._friend[i]._nick,	sizeof(char) * length);
				friendPacket.WriteData(&info,						sizeof(FRIEND_INFO));

				g_pGateway->SendPacket(friendNode._serveridx, &friendPacket);

				g_pUserFinder->UpdateByUid(&friendNode);

				break;
			}
		}
	}

	UINT8 changeState = FRIEND_CHANGE_STATE_CHANGE;
	UINT8 length = (UINT8)(i3String::Length(node._friend[idx]._nick) + 1);
	UINT8 friendIdx = (UINT8)idx;

	FRIEND_INFO info;
	info._fuid = node._friend[idx]._fuid;
	info._rank = node._friend[idx]._rank;
	info._state = node._friend[idx]._state;

	i3NetworkPacket packet(PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK);
	packet.WriteData(&node._sessionidx,			sizeof(INT32));
	packet.WriteData(&node._iDbIdx,				sizeof(UINT32));
	packet.WriteData(&changeState,				sizeof(UINT8));
	packet.WriteData(&friendIdx,				sizeof(UINT8));
	packet.WriteData(&length,					sizeof(UINT8));
	packet.WriteData(node._friend[idx]._nick,	sizeof(char) * length);
	packet.WriteData(&info,						sizeof(FRIEND_INFO));

	g_pGateway->SendPacket(node._serveridx, &packet);

	g_pUserFinder->UpdateByUid(&node);

	return EVENT_ERROR_SUCCESS;
}

/// 목록에서 친구를 삭제한다.
///   삭제 대상을 게임서버에 통보한다.
///   변경된 친구목록을 다시 받아 전달한다.
///   삭제 대상에게 자신을 오프라인 상태로 전달한다.
/// param uid	요청 userId
/// param idx	친구목록에 삭제 대상 배열 idx
/// return		error code
///				EVENT_ERROR_SUCCESS								성공
///				EVENT_ERROR_EVENT_FRIEND_ACCEPT_PARAMETER_USER	잘못된 uid
///				EVENT_ERROR_EVENT_FRIEND_ACCEPT_PARAMETER_IDX	잘못된 idx
///				EVENT_ERROR_EVENT_FRIEND_ACCEPT_QUERY			db error
INT32 CCommunity::DeleteFriend(UINT32 uid,UINT32 idx)
{
	USER_NODE node;

	if (FALSE == g_pUserFinder->FindUid(uid, &node))
	{
		return EVENT_ERROR_EVENT_FRIEND_ACCEPT_PARAMETER_USER;
	}

	if (idx >= node._friendCount)
	{
		return EVENT_ERROR_EVENT_FRIEND_ACCEPT_PARAMETER_IDX;
	}

	if (0 > g_pDataBase->DeleteFriend(uid, node._friend[idx]._fuid))
	{
		return EVENT_ERROR_EVENT_FRIEND_ACCEPT_QUERY;
	}


	// 로그를 위해 게임서버에 친구 삭제통보해야한다
	{
		UINT8 state = FRIEND_CHANGE_STATE_DELETE;
		UINT8 friendIdx = (UINT8)idx;

		i3NetworkPacket deletePacket(PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK);
		
		deletePacket.WriteData(&node._sessionidx,		sizeof(INT32));
		deletePacket.WriteData(&node._iDbIdx,			sizeof(UINT32));
		deletePacket.WriteData(&state,					sizeof(UINT8));
		deletePacket.WriteData(&friendIdx,				sizeof(UINT8));

		g_pGateway->SendPacket(node._serveridx, &deletePacket);
	}


	UINT32 fuid = node._friend[idx]._fuid;
	MakeFriendList(uid);
	
	USER_NODE friendNode;
	
	if (g_pUserFinder->FindUid(fuid, &friendNode))
	{
		for(UINT32 j = 0; j < friendNode._friendCount; j++)
		{
			if (friendNode._friend[j]._fuid == uid)
			{
				UINT8 currentState = (UINT8)GET_FRIEND_STATE(friendNode._friend[j]._state);

				if (FRIEND_STATE_OFFLINE <= currentState)
				{
					friendNode._friend[j]._state = MAKE_FRIEND_STATE(FRIEND_STATE_OFFLINE, 0, 0, 0);

					UINT8 changeState = FRIEND_CHANGE_STATE_CHANGE;
					UINT8 length = (UINT8)(i3String::Length(friendNode._friend[j]._nick) + 1);
					UINT8 friendIdx = (UINT8)j;

					FRIEND_INFO info;
					info._fuid = friendNode._friend[j]._fuid;
					info._rank = friendNode._friend[j]._rank;
					info._state = friendNode._friend[j]._state;

					i3NetworkPacket friendPacket(PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK);
					friendPacket.WriteData(&friendNode._sessionidx,		sizeof(INT32));
					friendPacket.WriteData(&friendNode._iDbIdx,			sizeof(UINT32));
					friendPacket.WriteData(&changeState,				sizeof(UINT8));
					friendPacket.WriteData(&friendIdx,					sizeof(UINT8));
					friendPacket.WriteData(&length,						sizeof(UINT8));
					friendPacket.WriteData(friendNode._friend[j]._nick,	sizeof(char) * length);
					friendPacket.WriteData(&info,						sizeof(FRIEND_INFO));

					g_pGateway->SendPacket(friendNode._serveridx, &friendPacket);

					g_pUserFinder->UpdateByUid(&friendNode);					
				}

				break;
			}
		}
	}

	return EVENT_ERROR_SUCCESS;
}

/// 친구에게 초대 메시지를 보낸다
/// param uid	요청 userId(dbIdx)
/// param idx	친구목록의 초대 대상 배열 idx
/// return		error code
///				EVENT_ERROR_SUCCESS								성공
///				EVENT_ERROR_EVENT_FRIEND_INVITED_PARAMETER_USER	잘못된 uid
///				EVENT_ERROR_EVENT_FRIEND_INVITED_PARAMETER_IDX	잘못된 idx
///				EVENT_ERROR_EVENT_FRIEND_INVITED_NOT_CONNECTED	접속되지 않은 대상
///				EVENT_ERROR_EVENT_FRIEND_INVITED_NOT_MATCHED	상대방 목록에 요청자가 없다
INT32 CCommunity::InvitedFriend(UINT32 uid,UINT32 idx)
{
	USER_NODE node;

	if (FALSE == g_pUserFinder->FindUid(uid, &node))
	{
		return EVENT_ERROR_EVENT_FRIEND_INVITED_PARAMETER_USER;
	}

	if (idx >= node._friendCount)
	{
		return EVENT_ERROR_EVENT_FRIEND_INVITED_PARAMETER_IDX;
	}
	
	USER_NODE friendNode;

	if (FALSE == g_pUserFinder->FindUid(node._friend[idx]._fuid, &friendNode))
	{
		return EVENT_ERROR_EVENT_FRIEND_INVITED_NOT_CONNECTED;
	}

	for(UINT32 i = 0; i < friendNode._friendCount; i++)
	{
		if (friendNode._friend[i]._fuid == uid)
		{
			UINT8 friendIdx = (UINT8)i;

			i3NetworkPacket packet(PROTOCOL_MESSENGER_FRIEND_INVITED_REQUEST_ACK);

			packet.WriteData(&friendNode._sessionidx, sizeof(INT32));
			packet.WriteData(&friendNode._iDbIdx, sizeof(UINT32));
			packet.WriteData(&friendIdx, sizeof(UINT8));
			g_pGateway->SendPacket(friendNode._serveridx, &packet);

			return EVENT_ERROR_SUCCESS;
		}
	}

	return EVENT_ERROR_EVENT_FRIEND_INVITED_NOT_MATCHED;
}

/// 상태를 변경하고 접속중인 모든 친구에게 전달한다.
/// param uid	요청 userId(dbIdx)
/// param state	변경할 상태
/// return		error code
///				EVENT_ERROR_SUCCESS	성공
///				EVENT_ERROR_FAIL	실패
INT32 CCommunity::ChangeState(UINT32 uid, UINT32 state)
{
	USER_NODE node;

	if (FALSE == g_pUserFinder->FindUid(uid, &node))
	{
		return EVENT_ERROR_FAIL;
	}

	node._connectState = state;	
	g_pUserFinder->UpdateByUid(&node);

	for(UINT32 i = 0; i < node._friendCount; i++)
	{
		if (FRIEND_STATE_ONLINE <= GET_FRIEND_STATE(node._friend[i]._state))
		{
			USER_NODE friendNode;
			
			if (g_pUserFinder->FindUid(node._friend[i]._fuid, &friendNode))
			{
				for(UINT32 j = 0; j < friendNode._friendCount; j++)
				{
					if (friendNode._friend[j]._fuid == uid)
					{
						friendNode._friend[j]._state = node._connectState;

						UINT8 changeState = FRIEND_CHANGE_STATE_CHANGE;
						UINT8 length = (UINT8)(i3String::Length(friendNode._friend[j]._nick) + 1);
						UINT8 friendIdx = (UINT8)j;

						FRIEND_INFO info;
						info._fuid = friendNode._friend[j]._fuid;
						info._rank = friendNode._friend[j]._rank;
						info._state = friendNode._friend[j]._state;

						i3NetworkPacket friendPacket(PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK);
						friendPacket.WriteData(&friendNode._sessionidx,		sizeof(INT32));
						friendPacket.WriteData(&friendNode._iDbIdx,			sizeof(UINT32));
						friendPacket.WriteData(&changeState,				sizeof(UINT8));
						friendPacket.WriteData(&friendIdx,					sizeof(UINT8));
						friendPacket.WriteData(&length,						sizeof(UINT8));
						friendPacket.WriteData(friendNode._friend[j]._nick,	sizeof(char) * length);
						friendPacket.WriteData(&info,						sizeof(FRIEND_INFO));

						g_pGateway->SendPacket(friendNode._serveridx, &friendPacket);

						g_pUserFinder->UpdateByUid(&friendNode);

						break;
					}
				}
			}
		}
	}	
	
	return EVENT_ERROR_SUCCESS;
}

void CCommunity::ChangeNickname(UINT32 uid,const char* nickname)
{
	USER_NODE node;
	
	if (g_pUserFinder->FindUid(uid, &node))
	{
		// 접속 중인 모든 친구에게 변경을 통보한다.
		for(UINT32 i = 0; i < node._friendCount; i++)
		{
			USER_NODE friendNode;

			if (g_pUserFinder->FindUid(node._friend[i]._fuid, &friendNode))
			{
				// 친구의 친구목록에서 자신의 정보를 찾는다.
				for(UINT32 j = 0; j < friendNode._friendCount; j++)
				{
					if (friendNode._friend[j]._fuid == uid)
					{
						// 닉네임 정보 변경
						i3String::NCopy(friendNode._friend[j]._nick, nickname, NET_NICK_NAME_SIZE);
						friendNode._friend[j]._nick[NET_NICK_NAME_SIZE - 1] = '\0';
						
						g_pUserFinder->UpdateByUid(&friendNode);


						// 변경 내역 전송
						UINT8 changeState = FRIEND_CHANGE_STATE_CHANGE;
						UINT8 friendIdx = (UINT8)j;
						UINT8 nickSize = (UINT8)i3String::Length(friendNode._friend[j]._nick);

						FRIEND_INFO info;
						info._fuid = friendNode._friend[j]._fuid;
						info._rank = friendNode._friend[j]._rank;
						info._state = friendNode._friend[j]._state;


						i3NetworkPacket packet(PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK);

						packet.WriteData(&friendNode._sessionidx, sizeof(INT32));
						packet.WriteData(&friendNode._iDbIdx, sizeof(UINT32));
						packet.WriteData(&changeState, sizeof(UINT8));
						packet.WriteData(&friendIdx, sizeof(UINT8));
						packet.WriteData(&nickSize, sizeof(UINT8));
						packet.WriteData(friendNode._friend[j]._nick, nickSize);
						packet.WriteData(&info, sizeof(FRIEND_INFO));

						g_pGateway->SendPacket(friendNode._serveridx, &packet);

						break;
					}
				}
			}
		}
	}
}

void CCommunity::ChangeRank(UINT32 uid,UINT32 rank)
{
	USER_NODE node;

	if (g_pUserFinder->FindUid(uid, &node))
	{
		// 접속 중인 모든 친구에게 변경을 통보한다.
		for(UINT32 i = 0; i < node._friendCount; i++)
		{
			USER_NODE friendNode;

			if (g_pUserFinder->FindUid(node._friend[i]._fuid, &friendNode))
			{
				// 친구의 친구목록에서 자신의 정보를 찾는다.
				for(UINT32 j = 0; j < friendNode._friendCount; j++)
				{
					if (friendNode._friend[j]._fuid == uid)
					{
						// 계급정보 변경
						friendNode._friend[j]._rank = (UINT8) rank;						
						g_pUserFinder->UpdateByUid(&friendNode);


						// 변경 내역 전송
						UINT8 changeState = FRIEND_CHANGE_STATE_CHANGE;
						UINT8 friendIdx = (UINT8)j;
						UINT8 nickSize = (UINT8)i3String::Length(friendNode._friend[j]._nick);

						FRIEND_INFO info;
						info._fuid = friendNode._friend[j]._fuid;
						info._rank = friendNode._friend[j]._rank;
						info._state = friendNode._friend[j]._state;


						i3NetworkPacket packet(PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK);

						packet.WriteData(&friendNode._sessionidx, sizeof(INT32));
						packet.WriteData(&friendNode._iDbIdx, sizeof(UINT32));
						packet.WriteData(&changeState, sizeof(UINT8));
						packet.WriteData(&friendIdx, sizeof(UINT8));
						packet.WriteData(&nickSize, sizeof(UINT8));
						packet.WriteData(friendNode._friend[j]._nick, nickSize);
						packet.WriteData(&info, sizeof(FRIEND_INFO));

						g_pGateway->SendPacket(friendNode._serveridx, &packet);

						break;
					}
				}
			}
		}
	}
}

INT32 CCommunity::ResetCommunityInfo(UINT32 uid)
{
	return g_pDataBase->ResetFriendInfo(uid);
}

