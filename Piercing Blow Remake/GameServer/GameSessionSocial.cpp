#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "GameSessionManager.h"

// ============================================================================
// Social/Community Handlers (Protocol_Auth 0x0300)
// Friends, Whisper, Block, Find User
// ============================================================================

void GameSession::OnFriendInfoReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_AUTH_FRIEND_INFO_REQ -> ACK
	// Send friend list to client
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	i3NetworkPacket packet;
	char buffer[2048];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_AUTH_FRIEND_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Friend count
	uint16_t friendCount = (uint16_t)m_i32FriendCount;
	memcpy(buffer + offset, &friendCount, 2);				offset += 2;

	// Per-friend: uid(8) + nickname(64) + state(1) + level(4) + rankId(4)
	for (int i = 0; i < m_i32FriendCount; i++)
	{
		if (offset + 81 > (int)sizeof(buffer))
			break;

		const GameFriendInfo& f = m_Friends[i];
		memcpy(buffer + offset, &f.i64UID, 8);				offset += 8;
		memcpy(buffer + offset, f.szNickname, 64);			offset += 64;

		// Check if friend is online
		uint8_t state = 0;	// offline
		if (g_pGameSessionManager)
		{
			GameSession* pFriend = g_pGameSessionManager->FindSessionByUID(f.i64UID);
			if (pFriend)
			{
				if (pFriend->GetTask() >= GAME_TASK_BATTLE)
					state = 3;	// in battle
				else if (pFriend->GetTask() >= GAME_TASK_READY_ROOM)
					state = 2;	// in room
				else
					state = 1;	// online
			}
		}
		memcpy(buffer + offset, &state, 1);					offset += 1;

		int32_t level = f.i32Level;
		int32_t rankId = f.i32RankId;
		memcpy(buffer + offset, &level, 4);					offset += 4;
		memcpy(buffer + offset, &rankId, 4);				offset += 4;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnFriendInsertReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_AUTH_FRIEND_INSERT_REQ -> ACK
	// Send friend request by nickname
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 64)
		return;

	char targetNick[64];
	memcpy(targetNick, pData, 64);
	targetNick[63] = '\0';

	int32_t result = 0;

	// Check friend list capacity
	if (m_i32FriendCount >= MAX_FRIEND_COUNT)
	{
		result = 1;	// Friend list full
	}
	// Check if already friends
	else if (FindFriend(targetNick))
	{
		result = 2;	// Already friends
	}
	else
	{
		// Find target player online
		GameSession* pTarget = nullptr;
		if (g_pGameSessionManager)
			pTarget = g_pGameSessionManager->FindSessionByNickname(targetNick);

		if (!pTarget)
		{
			result = 3;	// Player not found/offline
		}
		else if (pTarget == this)
		{
			result = 4;	// Cannot add self
		}
		else
		{
			// Add to our friend list
			GameFriendInfo& newFriend = m_Friends[m_i32FriendCount];
			newFriend.i64UID = pTarget->GetUID();
			strncpy_s(newFriend.szNickname, pTarget->GetNickname(), 63);
			newFriend.i32Level = pTarget->GetLevel();
			newFriend.i32RankId = pTarget->GetRankId();
			m_i32FriendCount++;

			// Also add us to their friend list (mutual)
			if (pTarget->m_i32FriendCount < MAX_FRIEND_COUNT)
			{
				GameFriendInfo& reverse = pTarget->m_Friends[pTarget->m_i32FriendCount];
				reverse.i64UID = m_i64UID;
				strncpy_s(reverse.szNickname, m_szNickname, 63);
				reverse.i32Level = m_i32Level;
				reverse.i32RankId = m_i32RankId;
				pTarget->m_i32FriendCount++;

				// Notify target about the new friend
				pTarget->SendSimpleAck(PROTOCOL_AUTH_FRIEND_INFO_CHANGE_ACK, 0);
			}

			printf("[GameSession] Friend added - UID=%lld added %s (UID=%lld)\n",
				m_i64UID, targetNick, pTarget->GetUID());
		}
	}

	SendSimpleAck(PROTOCOL_AUTH_FRIEND_INSERT_ACK, result);
}

void GameSession::OnFriendDeleteReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_AUTH_FRIEND_DELETE_REQ -> ACK
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 8)
		return;

	int64_t targetUID = *(int64_t*)pData;
	int32_t result = 0;

	// Find and remove from our list
	bool found = false;
	for (int i = 0; i < m_i32FriendCount; i++)
	{
		if (m_Friends[i].i64UID == targetUID)
		{
			// Shift remaining friends down
			for (int j = i; j < m_i32FriendCount - 1; j++)
				m_Friends[j] = m_Friends[j + 1];
			m_i32FriendCount--;
			m_Friends[m_i32FriendCount].Reset();
			found = true;
			break;
		}
	}

	if (!found)
		result = 1;	// Friend not found

	// Also remove from their list if online
	if (found && g_pGameSessionManager)
	{
		GameSession* pTarget = g_pGameSessionManager->FindSessionByUID(targetUID);
		if (pTarget)
		{
			for (int i = 0; i < pTarget->m_i32FriendCount; i++)
			{
				if (pTarget->m_Friends[i].i64UID == m_i64UID)
				{
					for (int j = i; j < pTarget->m_i32FriendCount - 1; j++)
						pTarget->m_Friends[j] = pTarget->m_Friends[j + 1];
					pTarget->m_i32FriendCount--;
					pTarget->m_Friends[pTarget->m_i32FriendCount].Reset();
					pTarget->SendSimpleAck(PROTOCOL_AUTH_FRIEND_INFO_CHANGE_ACK, 0);
					break;
				}
			}
		}
	}

	SendSimpleAck(PROTOCOL_AUTH_FRIEND_DELETE_ACK, result);
}

void GameSession::OnWhisperReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_AUTH_SEND_WHISPER_REQ -> ACK (to sender) + RECV (to target)
	// Parse: targetNick(64) + messageLen(2) + message(variable)
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 66)
		return;

	char targetNick[64];
	memcpy(targetNick, pData, 64);
	targetNick[63] = '\0';

	uint16_t msgLen = *(uint16_t*)(pData + 64);
	if (msgLen > 256) msgLen = 256;

	if (i32Size < (int)(66 + msgLen))
		return;

	char* pMsg = pData + 66;

	int32_t result = 0;

	// Find target
	GameSession* pTarget = nullptr;
	if (g_pGameSessionManager)
		pTarget = g_pGameSessionManager->FindSessionByNickname(targetNick);

	if (!pTarget)
	{
		result = 1;	// Player not found/offline
	}
	else if (pTarget == this)
	{
		result = 2;	// Cannot whisper self
	}
	else if (pTarget->IsBlocked(m_i64UID))
	{
		result = 3;	// Blocked by target
	}
	else
	{
		// Send whisper to target
		i3NetworkPacket recvPacket;
		char recvBuf[512];
		int rOffset = 0;

		uint16_t rSz = 0;
		uint16_t rProto = PROTOCOL_AUTH_RECV_WHISPER_ACK;
		rOffset += sizeof(uint16_t);
		memcpy(recvBuf + rOffset, &rProto, sizeof(uint16_t));		rOffset += sizeof(uint16_t);

		// Sender nickname
		memcpy(recvBuf + rOffset, m_szNickname, 64);				rOffset += 64;
		// Message
		memcpy(recvBuf + rOffset, &msgLen, 2);						rOffset += 2;
		memcpy(recvBuf + rOffset, pMsg, msgLen);					rOffset += msgLen;

		rSz = (uint16_t)rOffset;
		memcpy(recvBuf, &rSz, sizeof(uint16_t));

		recvPacket.SetPacketData(recvBuf, rOffset);
		pTarget->SendMessage(&recvPacket);
	}

	// ACK to sender
	SendSimpleAck(PROTOCOL_AUTH_SEND_WHISPER_ACK, result);
}

void GameSession::OnBlockInsertReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_AUTH_BLOCK_INSERT_REQ -> ACK
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 64)
		return;

	char targetNick[64];
	memcpy(targetNick, pData, 64);
	targetNick[63] = '\0';

	int32_t result = 0;

	if (m_i32BlockCount >= MAX_BLOCK_COUNT)
	{
		result = 1;	// Block list full
	}
	else
	{
		// Find target UID (must be online for simplicity in v1)
		GameSession* pTarget = nullptr;
		if (g_pGameSessionManager)
			pTarget = g_pGameSessionManager->FindSessionByNickname(targetNick);

		if (!pTarget)
		{
			result = 2;	// Player not found
		}
		else if (pTarget == this)
		{
			result = 3;	// Cannot block self
		}
		else
		{
			GameBlockInfo& block = m_BlockList[m_i32BlockCount];
			block.i64UID = pTarget->GetUID();
			strncpy_s(block.szNickname, targetNick, 63);
			m_i32BlockCount++;
		}
	}

	SendSimpleAck(PROTOCOL_AUTH_BLOCK_INSERT_ACK, result);
}

void GameSession::OnBlockDeleteReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_AUTH_BLOCK_DELETE_REQ -> ACK
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 8)
		return;

	int64_t targetUID = *(int64_t*)pData;
	int32_t result = 0;

	bool found = false;
	for (int i = 0; i < m_i32BlockCount; i++)
	{
		if (m_BlockList[i].i64UID == targetUID)
		{
			for (int j = i; j < m_i32BlockCount - 1; j++)
				m_BlockList[j] = m_BlockList[j + 1];
			m_i32BlockCount--;
			m_BlockList[m_i32BlockCount].Reset();
			found = true;
			break;
		}
	}

	if (!found)
		result = 1;

	SendSimpleAck(PROTOCOL_AUTH_BLOCK_DELETE_ACK, result);
}

void GameSession::OnFindUserReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_AUTH_FIND_USER_REQ -> ACK
	// Search for a player by nickname
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 64)
		return;

	char searchNick[64];
	memcpy(searchNick, pData, 64);
	searchNick[63] = '\0';

	i3NetworkPacket packet;
	char buffer[256];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_AUTH_FIND_USER_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	GameSession* pTarget = nullptr;
	if (g_pGameSessionManager)
		pTarget = g_pGameSessionManager->FindSessionByNickname(searchNick);

	if (!pTarget)
	{
		result = 1;	// Not found
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	}
	else
	{
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

		int64_t uid = pTarget->GetUID();
		memcpy(buffer + offset, &uid, 8);					offset += 8;
		memcpy(buffer + offset, pTarget->GetNickname(), 64);offset += 64;

		int32_t level = pTarget->GetLevel();
		int32_t rankId = pTarget->GetRankId();
		int32_t clanId = pTarget->GetClanId();
		memcpy(buffer + offset, &level, 4);					offset += 4;
		memcpy(buffer + offset, &rankId, 4);				offset += 4;
		memcpy(buffer + offset, &clanId, 4);				offset += 4;

		// State
		uint8_t state = 1;	// online
		if (pTarget->GetTask() >= GAME_TASK_BATTLE)
			state = 3;
		else if (pTarget->GetTask() >= GAME_TASK_READY_ROOM)
			state = 2;
		memcpy(buffer + offset, &state, 1);					offset += 1;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

// ============================================================================
// Note/Mail System
// ============================================================================

void GameSession::OnNoteSendReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_MESSENGER_NOTE_SEND_REQ
	// Parse: targetNick(64) + subject(64) + bodyLen(2) + body(variable)
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 130)	// 64 + 64 + 2
		return;

	char targetNick[64];
	memcpy(targetNick, pData, 64);
	targetNick[63] = '\0';

	char subject[NOTE_SUBJECT_LEN];
	memcpy(subject, pData + 64, NOTE_SUBJECT_LEN);
	subject[NOTE_SUBJECT_LEN - 1] = '\0';

	uint16_t bodyLen = *(uint16_t*)(pData + 128);
	if (bodyLen > NOTE_BODY_LEN) bodyLen = NOTE_BODY_LEN;

	if (i32Size < (int)(130 + bodyLen))
		return;

	char body[NOTE_BODY_LEN] = {0};
	if (bodyLen > 0)
	{
		memcpy(body, pData + 130, bodyLen);
		body[bodyLen] = '\0';
	}

	int32_t result = 0;

	// Find target player
	GameSession* pTarget = nullptr;
	if (g_pGameSessionManager)
		pTarget = g_pGameSessionManager->FindSessionByNickname(targetNick);

	if (!pTarget)
	{
		result = 1;	// Player not found/offline
	}
	else if (pTarget == this)
	{
		result = 2;	// Cannot send to self
	}
	else if (pTarget->IsBlocked(m_i64UID))
	{
		result = 3;	// Blocked by target
	}
	else
	{
		if (!pTarget->ReceiveNote(m_i64UID, m_szNickname, subject, body, 0))
			result = 4;	// Target mailbox full
	}

	SendSimpleAck(PROTOCOL_MESSENGER_NOTE_SEND_ACK, result);
}

void GameSession::OnNoteListReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_MESSENGER_NOTE_LIST_ACK
	// Send note list to client
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	i3NetworkPacket packet;
	char buffer[4096];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_MESSENGER_NOTE_LIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Note count
	uint16_t noteCount = (uint16_t)m_i32NoteCount;
	memcpy(buffer + offset, &noteCount, 2);					offset += 2;

	// Per-note: noteId(4) + senderNick(64) + subject(64) + type(1) + read(1) + timestamp(4)
	for (int i = 0; i < m_i32NoteCount; i++)
	{
		if (offset + 138 > (int)sizeof(buffer))
			break;

		const GameNoteData& note = m_Notes[i];
		memcpy(buffer + offset, &note.ui32NoteId, 4);		offset += 4;
		memcpy(buffer + offset, note.szSenderNick, 64);	offset += 64;
		memcpy(buffer + offset, note.szSubject, 64);		offset += 64;
		memcpy(buffer + offset, &note.ui8Type, 1);			offset += 1;
		uint8_t read = note.bRead ? 1 : 0;
		memcpy(buffer + offset, &read, 1);					offset += 1;
		memcpy(buffer + offset, &note.dwTimestamp, 4);		offset += 4;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnNoteDeleteReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_MESSENGER_NOTE_DELETE_REQ
	// Parse: noteId(4)
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 4)
		return;

	uint32_t noteId = *(uint32_t*)pData;
	int32_t result = 1;	// Not found

	for (int i = 0; i < m_i32NoteCount; i++)
	{
		if (m_Notes[i].ui32NoteId == noteId)
		{
			// Shift remaining notes down
			for (int j = i; j < m_i32NoteCount - 1; j++)
				m_Notes[j] = m_Notes[j + 1];
			m_i32NoteCount--;
			m_Notes[m_i32NoteCount].Reset();
			result = 0;
			break;
		}
	}

	SendSimpleAck(PROTOCOL_MESSENGER_NOTE_DELETE_ACK, result);
}

void GameSession::OnNoteCheckReadedReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_MESSENGER_NOTE_CHECK_READED_REQ
	// Parse: noteId(4)
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 4)
		return;

	uint32_t noteId = *(uint32_t*)pData;
	int32_t result = 1;	// Not found

	for (int i = 0; i < m_i32NoteCount; i++)
	{
		if (m_Notes[i].ui32NoteId == noteId)
		{
			m_Notes[i].bRead = true;
			result = 0;
			break;
		}
	}

	SendSimpleAck(PROTOCOL_MESSENGER_NOTE_CHECK_READED_ACK, result);
}

bool GameSession::ReceiveNote(int64_t senderUID, const char* senderNick,
							const char* subject, const char* body, uint8_t type)
{
	if (m_i32NoteCount >= MAX_NOTE_COUNT)
		return false;

	GameNoteData& note = m_Notes[m_i32NoteCount];
	note.ui32NoteId = m_ui32NextNoteId++;
	note.i64SenderUID = senderUID;
	strncpy_s(note.szSenderNick, senderNick, 63);
	strncpy_s(note.szSubject, subject, NOTE_SUBJECT_LEN - 1);
	strncpy_s(note.szBody, body, NOTE_BODY_LEN - 1);
	note.dwTimestamp = GetTickCount();
	note.ui8Type = type;
	note.bRead = false;
	m_i32NoteCount++;

	// Notify recipient that a new note arrived
	{
		i3NetworkPacket packet;
		char buf[128];
		int off = 0;

		uint16_t sz = 0;
		uint16_t proto = PROTOCOL_MESSENGER_NOTE_RECEIVE_ACK;
		off += sizeof(uint16_t);
		memcpy(buf + off, &proto, sizeof(uint16_t));	off += sizeof(uint16_t);

		memcpy(buf + off, &note.ui32NoteId, 4);		off += 4;
		memcpy(buf + off, note.szSenderNick, 64);		off += 64;

		sz = (uint16_t)off;
		memcpy(buf, &sz, sizeof(uint16_t));

		packet.SetPacketData(buf, off);
		SendMessage(&packet);
	}

	return true;
}

// ============================================================================
// Friend Status Notifications (Phase 7A)
// ============================================================================

void GameSession::NotifyFriendsStatusChange(uint8_t ui8NewState)
{
	if (!g_pGameSessionManager || m_i32FriendCount == 0)
		return;

	// Build status change notification packet
	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_AUTH_FRIEND_INFO_CHANGE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Our info
	int64_t uid = m_i64UID;
	memcpy(buffer + offset, &uid, 8);						offset += 8;
	memcpy(buffer + offset, m_szNickname, 64);				offset += 64;
	memcpy(buffer + offset, &ui8NewState, 1);				offset += 1;

	int32_t level = m_i32Level;
	int32_t rankId = m_i32RankId;
	memcpy(buffer + offset, &level, 4);						offset += 4;
	memcpy(buffer + offset, &rankId, 4);					offset += 4;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));
	packet.SetPacketData(buffer, offset);

	// Send to each online friend
	for (int i = 0; i < m_i32FriendCount; i++)
	{
		GameSession* pFriend = g_pGameSessionManager->FindSessionByUID(m_Friends[i].i64UID);
		if (pFriend && pFriend->GetTask() >= GAME_TASK_CHANNEL)
		{
			pFriend->SendMessage(&packet);
		}
	}
}

void GameSession::NotifyFriendLobbyEnter()
{
	if (!g_pGameSessionManager || m_i32FriendCount == 0)
		return;

	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_AUTH_FRIEND_ROOM_ENTER_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int64_t uid = m_i64UID;
	memcpy(buffer + offset, &uid, 8);						offset += 8;
	memcpy(buffer + offset, m_szNickname, 64);				offset += 64;

	int32_t channelNum = m_i32ChannelNum;
	memcpy(buffer + offset, &channelNum, 4);				offset += 4;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));
	packet.SetPacketData(buffer, offset);

	for (int i = 0; i < m_i32FriendCount; i++)
	{
		GameSession* pFriend = g_pGameSessionManager->FindSessionByUID(m_Friends[i].i64UID);
		if (pFriend && pFriend->GetTask() >= GAME_TASK_CHANNEL)
			pFriend->SendMessage(&packet);
	}
}

void GameSession::NotifyFriendLobbyLeave()
{
	if (!g_pGameSessionManager || m_i32FriendCount == 0)
		return;

	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_AUTH_FRIEND_ROOM_LEAVE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int64_t uid = m_i64UID;
	memcpy(buffer + offset, &uid, 8);						offset += 8;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));
	packet.SetPacketData(buffer, offset);

	for (int i = 0; i < m_i32FriendCount; i++)
	{
		GameSession* pFriend = g_pGameSessionManager->FindSessionByUID(m_Friends[i].i64UID);
		if (pFriend && pFriend->GetTask() >= GAME_TASK_CHANNEL)
			pFriend->SendMessage(&packet);
	}
}

// ============================================================================
// Friend Accept (accept/deny friend request)
// ============================================================================

void GameSession::OnFriendAcceptReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_AUTH_FRIEND_ACCEPT_REQ -> ACK
	// Accept or deny a pending friend request
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 9)	// uid(8) + accept(1)
		return;

	int64_t requesterUID = *(int64_t*)pData;
	uint8_t accept = *(uint8_t*)(pData + 8);

	int32_t result = 0;

	if (accept == 1)
	{
		// Accept - already added mutually by OnFriendInsertReq, just confirm
		// If not already in list, add now
		if (!FindFriendByUID(requesterUID))
		{
			if (m_i32FriendCount >= MAX_FRIEND_COUNT)
			{
				result = 1;	// Friend list full
			}
			else
			{
				GameSession* pRequester = nullptr;
				if (g_pGameSessionManager)
					pRequester = g_pGameSessionManager->FindSessionByUID(requesterUID);

				if (pRequester)
				{
					GameFriendInfo& newFriend = m_Friends[m_i32FriendCount];
					newFriend.i64UID = pRequester->GetUID();
					strncpy_s(newFriend.szNickname, pRequester->GetNickname(), 63);
					newFriend.i32Level = pRequester->GetLevel();
					newFriend.i32RankId = pRequester->GetRankId();
					m_i32FriendCount++;
				}
				else
				{
					result = 2;	// Requester offline
				}
			}
		}
	}
	// If deny, just acknowledge

	SendSimpleAck(PROTOCOL_AUTH_FRIEND_ACCEPT_ACK, result);
}

// ============================================================================
// Friend Invite to Room
// ============================================================================

void GameSession::OnFriendInviteReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_AUTH_FRIEND_INVITED_REQ -> ACK (to sender) + REQUEST_ACK (to target)
	// Invite a friend to current room
	if (m_eMainTask < GAME_TASK_READY_ROOM || !m_pRoom)
	{
		SendSimpleAck(PROTOCOL_AUTH_FRIEND_INVITED_ACK, 1);	// Not in a room
		return;
	}

	if (i32Size < 8)
		return;

	int64_t targetUID = *(int64_t*)pData;
	int32_t result = 0;

	GameSession* pTarget = nullptr;
	if (g_pGameSessionManager)
		pTarget = g_pGameSessionManager->FindSessionByUID(targetUID);

	if (!pTarget)
	{
		result = 1;	// Not online
	}
	else if (pTarget->GetTask() >= GAME_TASK_READY_ROOM)
	{
		result = 2;	// Already in a room
	}
	else if (pTarget->GetTask() < GAME_TASK_LOBBY)
	{
		result = 3;	// Not in lobby
	}
	else
	{
		// Send invite to target
		i3NetworkPacket invitePacket;
		char buf[256];
		int off = 0;

		uint16_t rSz = 0;
		uint16_t rProto = PROTOCOL_AUTH_FRIEND_INVITED_REQUEST_ACK;
		off += sizeof(uint16_t);
		memcpy(buf + off, &rProto, sizeof(uint16_t));		off += sizeof(uint16_t);

		int64_t inviterUID = m_i64UID;
		memcpy(buf + off, &inviterUID, 8);					off += 8;
		memcpy(buf + off, m_szNickname, 64);				off += 64;

		int32_t channelNum = m_i32ChannelNum;
		int32_t roomIdx = m_i32RoomIdx;
		memcpy(buf + off, &channelNum, 4);					off += 4;
		memcpy(buf + off, &roomIdx, 4);						off += 4;

		rSz = (uint16_t)off;
		memcpy(buf, &rSz, sizeof(uint16_t));

		invitePacket.SetPacketData(buf, off);
		pTarget->SendMessage(&invitePacket);
	}

	SendSimpleAck(PROTOCOL_AUTH_FRIEND_INVITED_ACK, result);
}

// ============================================================================
// Community User Invite (from lobby user list)
// ============================================================================

void GameSession::OnCommunityUserInviteReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_COMMUNITY_USER_INVITED_REQ -> ACK/REQUEST_ACK
	// Invite any user (from lobby search or user list) to room
	if (m_eMainTask < GAME_TASK_READY_ROOM || !m_pRoom)
	{
		SendSimpleAck(PROTOCOL_COMMUNITY_USER_INVITED_ACK, 1);
		return;
	}

	if (i32Size < 64)
		return;

	char targetNick[64];
	memcpy(targetNick, pData, 64);
	targetNick[63] = '\0';

	int32_t result = 0;

	GameSession* pTarget = nullptr;
	if (g_pGameSessionManager)
		pTarget = g_pGameSessionManager->FindSessionByNickname(targetNick);

	if (!pTarget)
	{
		result = 1;
	}
	else if (pTarget == this)
	{
		result = 2;
	}
	else if (pTarget->GetTask() >= GAME_TASK_READY_ROOM)
	{
		result = 3;	// Already in room
	}
	else if (pTarget->GetTask() < GAME_TASK_LOBBY)
	{
		result = 4;	// Not in lobby
	}
	else
	{
		// Send invite notification to target
		i3NetworkPacket invitePacket;
		char buf[256];
		int off = 0;

		uint16_t rSz = 0;
		uint16_t rProto = PROTOCOL_COMMUNITY_USER_INVITED_REQUEST_ACK;
		off += sizeof(uint16_t);
		memcpy(buf + off, &rProto, sizeof(uint16_t));		off += sizeof(uint16_t);

		int64_t inviterUID = m_i64UID;
		memcpy(buf + off, &inviterUID, 8);					off += 8;
		memcpy(buf + off, m_szNickname, 64);				off += 64;

		int32_t channelNum = m_i32ChannelNum;
		int32_t roomIdx = m_i32RoomIdx;
		memcpy(buf + off, &channelNum, 4);					off += 4;
		memcpy(buf + off, &roomIdx, 4);						off += 4;

		rSz = (uint16_t)off;
		memcpy(buf, &rSz, sizeof(uint16_t));

		invitePacket.SetPacketData(buf, off);
		pTarget->SendMessage(&invitePacket);
	}

	SendSimpleAck(PROTOCOL_COMMUNITY_USER_INVITED_ACK, result);
}

// ============================================================================
// Nickname/Color Nick Change
// ============================================================================

void GameSession::OnChangeNicknameReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_AUTH_CHANGE_NICKNAME_REQ -> ACK
	// Request nickname change (costs GP)
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 64)
		return;

	char newNick[64];
	memcpy(newNick, pData, 64);
	newNick[63] = '\0';

	int32_t result = 0;
	int nickCost = 5000;	// Default cost

	// Validate nickname length
	int len = (int)strlen(newNick);
	if (len < 2 || len > 16)
	{
		result = 1;	// Invalid length
	}
	else if (m_i32GP < nickCost)
	{
		result = 2;	// Not enough GP
	}
	else
	{
		// Check if nickname is taken
		GameSession* pExisting = nullptr;
		if (g_pGameSessionManager)
			pExisting = g_pGameSessionManager->FindSessionByNickname(newNick);

		if (pExisting && pExisting != this)
		{
			result = 3;	// Nickname taken
		}
		else
		{
			m_i32GP -= nickCost;
			strncpy_s(m_szNickname, newNick, 63);
			printf("[GameSession] Nickname changed - UID=%lld, NewNick=%s\n", m_i64UID, newNick);
		}
	}

	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_AUTH_CHANGE_NICKNAME_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	if (result == 0)
	{
		memcpy(buffer + offset, m_szNickname, 64);			offset += 64;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnChangeColorNickReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_AUTH_CHANGE_COLOR_NICK_REQ -> ACK
	// Change color of nickname (requires color nick item)
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 4)
		return;

	uint32_t colorItemId = *(uint32_t*)pData;
	int32_t result = 0;

	// Check if player has color nick item in inventory
	if (!HasInventoryItem(colorItemId))
	{
		result = 1;	// Item not found
	}
	else
	{
		// Consume the color nick item
		GameInventoryItem* pItem = FindInventoryItem(colorItemId);
		if (pItem)
			RemoveInventoryItem(pItem->ui32ItemDBIdx);
	}

	SendSimpleAck(PROTOCOL_AUTH_CHANGE_COLOR_NICK_ACK, result);
}

// ============================================================================
// Gift Shop Enter/Leave
// ============================================================================

void GameSession::OnGiftShopEnterReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_AUTH_ENTER_GIFTSHOP_REQ -> ACK
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	SendSimpleAck(PROTOCOL_AUTH_ENTER_GIFTSHOP_ACK, 0);
}

void GameSession::OnGiftShopLeaveReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_AUTH_LEAVE_GIFTSHOP_REQ -> ACK
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	SendSimpleAck(PROTOCOL_AUTH_LEAVE_GIFTSHOP_ACK, 0);
}

// ============================================================================
// Helper methods
// ============================================================================

GameFriendInfo* GameSession::FindFriend(const char* nickname) const
{
	for (int i = 0; i < m_i32FriendCount; i++)
	{
		if (_stricmp(m_Friends[i].szNickname, nickname) == 0)
			return const_cast<GameFriendInfo*>(&m_Friends[i]);
	}
	return nullptr;
}

GameFriendInfo* GameSession::FindFriendByUID(int64_t uid) const
{
	for (int i = 0; i < m_i32FriendCount; i++)
	{
		if (m_Friends[i].i64UID == uid)
			return const_cast<GameFriendInfo*>(&m_Friends[i]);
	}
	return nullptr;
}

bool GameSession::IsBlocked(int64_t uid) const
{
	for (int i = 0; i < m_i32BlockCount; i++)
	{
		if (m_BlockList[i].i64UID == uid)
			return true;
	}
	return false;
}

// ============================================================================
// Batch 17 - Auth/Social extras
// ============================================================================

void GameSession::OnAuthGetMyInfoReq(char* pData, INT32 i32Size)
{
	// Return player's own info summary
	i3NetworkPacket packet;
	char buffer[512];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_AUTH_GET_MYINFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	// UID
	memcpy(buffer + offset, &m_i64UID, sizeof(int64_t));	offset += sizeof(int64_t);

	// Nickname
	char nick[64] = {};
	strncpy(nick, m_szNickname, 63);
	memcpy(buffer + offset, nick, 64);						offset += 64;

	// Basic stats
	int32_t level = m_i32Level;
	int32_t rank = m_i32RankId;
	int64_t exp = m_i64Exp;
	int32_t gp = m_i32GP;
	int32_t cash = m_i32Cash;
	int32_t kills = m_i32Kills;
	int32_t deaths = m_i32Deaths;
	int32_t headshots = m_i32Headshots;
	int32_t wins = m_i32Wins;
	int32_t losses = m_i32Losses;
	int32_t clanId = m_i32ClanId;

	memcpy(buffer + offset, &level, 4);		offset += 4;
	memcpy(buffer + offset, &rank, 4);		offset += 4;
	memcpy(buffer + offset, &exp, 8);		offset += 8;
	memcpy(buffer + offset, &gp, 4);		offset += 4;
	memcpy(buffer + offset, &cash, 4);		offset += 4;
	memcpy(buffer + offset, &kills, 4);		offset += 4;
	memcpy(buffer + offset, &deaths, 4);	offset += 4;
	memcpy(buffer + offset, &headshots, 4);	offset += 4;
	memcpy(buffer + offset, &wins, 4);		offset += 4;
	memcpy(buffer + offset, &losses, 4);	offset += 4;
	memcpy(buffer + offset, &clanId, 4);	offset += 4;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnAuthFriendRoomEnterReq(char* pData, INT32 i32Size)
{
	// Request to enter the room where a friend is
	if (m_eMainTask < GAME_TASK_CHANNEL)
	{
		SendSimpleAck(PROTOCOL_AUTH_FRIEND_ROOM_ENTER_ACK, -1);
		return;
	}

	if (i32Size < (int)sizeof(int64_t))
	{
		SendSimpleAck(PROTOCOL_AUTH_FRIEND_ROOM_ENTER_ACK, -1);
		return;
	}

	int64_t friendUID = 0;
	memcpy(&friendUID, pData, sizeof(int64_t));

	GameSession* pFriend = g_pGameSessionManager ? g_pGameSessionManager->FindSessionByUID(friendUID) : nullptr;
	if (!pFriend)
	{
		SendSimpleAck(PROTOCOL_AUTH_FRIEND_ROOM_ENTER_ACK, -2);
		return;
	}

	if (!pFriend->GetRoom())
	{
		SendSimpleAck(PROTOCOL_AUTH_FRIEND_ROOM_ENTER_ACK, -3);
		return;
	}

	// Send friend's room info so client can join
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_AUTH_FRIEND_ROOM_ENTER_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	int32_t channel = pFriend->GetChannelNum();
	int32_t roomIdx = pFriend->GetRoomIdx();
	memcpy(buffer + offset, &channel, 4);	offset += 4;
	memcpy(buffer + offset, &roomIdx, 4);	offset += 4;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnAuthFriendRoomLeaveReq(char* pData, INT32 i32Size)
{
	// Notification that friend left room (client-side tracking)
	SendSimpleAck(PROTOCOL_AUTH_FRIEND_ROOM_LEAVE_ACK, 0);
}

void GameSession::OnWhisperFindUIDReq(char* pData, INT32 i32Size)
{
	// Whisper to a player by UID instead of nickname
	if (i32Size < (int)(sizeof(int64_t) + 4))
	{
		SendSimpleAck(PROTOCOL_AUTH_SEND_WHISPER_ACK, -1);
		return;
	}

	int offset = 0;
	int64_t targetUID = 0;
	memcpy(&targetUID, pData + offset, sizeof(int64_t));	offset += sizeof(int64_t);

	uint16_t msgLen = 0;
	memcpy(&msgLen, pData + offset, sizeof(uint16_t));		offset += sizeof(uint16_t);

	if (msgLen == 0 || msgLen > 256 || offset + msgLen > i32Size)
	{
		SendSimpleAck(PROTOCOL_AUTH_SEND_WHISPER_ACK, -1);
		return;
	}

	char msg[260] = {};
	memcpy(msg, pData + offset, msgLen);

	GameSession* pTarget = g_pGameSessionManager ? g_pGameSessionManager->FindSessionByUID(targetUID) : nullptr;
	if (!pTarget)
	{
		SendSimpleAck(PROTOCOL_AUTH_SEND_WHISPER_ACK, -2);
		return;
	}

	if (pTarget->IsBlocked(m_i64UID))
	{
		SendSimpleAck(PROTOCOL_AUTH_SEND_WHISPER_ACK, -3);
		return;
	}

	// Send whisper to target
	i3NetworkPacket recvPacket;
	char recvBuf[400];
	int recvOff = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_AUTH_RECV_WHISPER_ACK;
	recvOff += sizeof(uint16_t);
	memcpy(recvBuf + recvOff, &proto, sizeof(uint16_t));	recvOff += sizeof(uint16_t);

	memcpy(recvBuf + recvOff, &m_i64UID, sizeof(int64_t));	recvOff += sizeof(int64_t);

	char senderNick[64] = {};
	strncpy(senderNick, m_szNickname, 63);
	memcpy(recvBuf + recvOff, senderNick, 64);				recvOff += 64;

	memcpy(recvBuf + recvOff, &msgLen, sizeof(uint16_t));	recvOff += sizeof(uint16_t);
	memcpy(recvBuf + recvOff, msg, msgLen);					recvOff += msgLen;

	sz = (uint16_t)recvOff;
	memcpy(recvBuf, &sz, sizeof(uint16_t));

	recvPacket.SetPacketData(recvBuf, recvOff);
	pTarget->SendMessage(&recvPacket);

	// Confirm to sender
	SendSimpleAck(PROTOCOL_AUTH_SEND_WHISPER_ACK, 0);
}

void GameSession::OnAuthRecvWhisperReq(char* pData, INT32 i32Size)
{
	// Client acknowledges receiving a whisper
	SendSimpleAck(PROTOCOL_AUTH_RECV_WHISPER_ACK, 0);
}

void GameSession::OnAuthUsedWeaponReq(char* pData, INT32 i32Size)
{
	// Request player's used weapon stats
	if (i32Size < (int)sizeof(int64_t))
	{
		SendSimpleAck(PROTOCOL_AUTH_USED_WEAPON_ACK, -1);
		return;
	}

	int64_t targetUID = 0;
	memcpy(&targetUID, pData, sizeof(int64_t));

	// Return empty weapon usage list for now
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_AUTH_USED_WEAPON_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	memcpy(buffer + offset, &targetUID, sizeof(int64_t));	offset += sizeof(int64_t);

	uint16_t weaponCount = 0;
	memcpy(buffer + offset, &weaponCount, sizeof(uint16_t));	offset += sizeof(uint16_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnAuthFCMInfoReq(char* pData, INT32 i32Size)
{
	// FCM (Fatigue Control Mechanism) - anti-addiction system
	// Return that player is not subject to FCM restrictions
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_AUTH_FCM_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;  // 0 = not restricted
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	uint32_t playTimeSec = 0;
	memcpy(buffer + offset, &playTimeSec, sizeof(uint32_t));	offset += sizeof(uint32_t);

	uint8_t fcmState = 0;  // 0 = normal, 1 = warning, 2 = restricted
	memcpy(buffer + offset, &fcmState, sizeof(uint8_t));		offset += sizeof(uint8_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}
