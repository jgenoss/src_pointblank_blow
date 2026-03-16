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
