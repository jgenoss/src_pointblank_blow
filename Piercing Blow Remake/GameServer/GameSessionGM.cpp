#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "GameProtocol.h"
#include "GameContextMain.h"
#include "Room.h"
#include "RoomManager.h"
#include "GameServerContext.h"

// ============================================================================
// GM Command Handlers (Phase 11A)
// ============================================================================

bool GameSession::IsGMUser() const
{
	return (m_ui8AuthLevel >= 1);
}

// GM kicks a player from a room
void GameSession::OnGMKickUserReq(char* pData, INT32 i32Size)
{
	if (!IsGMUser())
	{
		printf("[GM] Unauthorized GM_KICK attempt - UID=%lld, AuthLevel=%d\n",
			m_i64UID, m_ui8AuthLevel);
		return;
	}

	if (i32Size < (int)sizeof(int))
		return;

	// Parse target slot index
	int targetSlot = 0;
	memcpy(&targetSlot, pData, sizeof(int));

	if (!m_pRoom)
	{
		printf("[GM] GM_KICK failed - GM not in room, UID=%lld\n", m_i64UID);
		return;
	}

	GameSession* pTarget = m_pRoom->GetSlotSession(targetSlot);
	if (!pTarget)
	{
		printf("[GM] GM_KICK failed - Target slot %d empty, GM_UID=%lld\n",
			targetSlot, m_i64UID);
		return;
	}

	printf("[GM] Kicking player - GM_UID=%lld, Target_UID=%lld (%s), Slot=%d\n",
		m_i64UID, pTarget->GetUID(), pTarget->GetNickname(), targetSlot);

	// Send kick notification to target
	{
		i3NetworkPacket packet;
		char buffer[64];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_SERVER_MESSAGE_KICK_PLAYER;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

		// Kick reason: 0 = GM kick
		uint8_t reason = 0;
		memcpy(buffer + offset, &reason, 1);				offset += 1;

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		pTarget->SendMessage(&packet);
	}

	// Remove target from room
	int targetChannel = pTarget->GetChannelNum();
	int targetRoomIdx = pTarget->GetRoomIdx();
	if (targetChannel >= 0 && targetRoomIdx >= 0)
	{
		RoomManager* pRoomMgr = g_pGameServerContext->GetRoomManager();
		if (pRoomMgr)
			pRoomMgr->OnLeaveRoom(pTarget, targetChannel);
	}

	// Send ACK to GM
	{
		i3NetworkPacket packet;
		char buffer[32];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_ROOM_GM_KICK_USER_ACK;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

		int32_t result = 0;
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		SendMessage(&packet);
	}
}

// GM force disconnects a player from room
void GameSession::OnGMExitUserReq(char* pData, INT32 i32Size)
{
	if (!IsGMUser())
	{
		printf("[GM] Unauthorized GM_EXIT attempt - UID=%lld\n", m_i64UID);
		return;
	}

	if (i32Size < (int)sizeof(int))
		return;

	int targetSlot = 0;
	memcpy(&targetSlot, pData, sizeof(int));

	if (!m_pRoom)
		return;

	GameSession* pTarget = m_pRoom->GetSlotSession(targetSlot);
	if (!pTarget)
		return;

	printf("[GM] Force disconnect - GM_UID=%lld, Target_UID=%lld (%s)\n",
		m_i64UID, pTarget->GetUID(), pTarget->GetNickname());

	// Send disconnect notification
	{
		i3NetworkPacket packet;
		char buffer[64];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_SERVER_MESSAGE_KICK_PLAYER;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

		uint8_t reason = 1;		// 1 = forced by GM
		memcpy(buffer + offset, &reason, 1);				offset += 1;

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		pTarget->SendMessage(&packet);
	}

	// Force disconnect target
	pTarget->OnDisconnect(TRUE);

	// ACK to GM
	{
		i3NetworkPacket packet;
		char buffer[32];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_ROOM_GM_EXIT_USER_ACK;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

		int32_t result = 0;
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		SendMessage(&packet);
	}
}

// GM force disconnects a player from lobby
void GameSession::OnLobbyGMExitUserReq(char* pData, INT32 i32Size)
{
	if (!IsGMUser())
	{
		printf("[GM] Unauthorized LOBBY_GM_EXIT attempt - UID=%lld\n", m_i64UID);
		return;
	}

	if (i32Size < (int)sizeof(int64_t))
		return;

	// Parse target UID
	int64_t targetUID = 0;
	memcpy(&targetUID, pData, sizeof(int64_t));

	// Find target session by UID
	GameSessionManager* pMgr = (GameSessionManager*)g_pGameServerContext->GetSessionManager();
	if (!pMgr)
		return;

	GameSession* pTarget = nullptr;
	for (int i = 0; i < g_pContextMain->m_i32SessionCount; i++)
	{
		GameSession* pSess = pMgr->GetSession(i);
		if (pSess && pSess->GetUID() == targetUID && pSess->GetTask() >= GAME_TASK_CHANNEL)
		{
			pTarget = pSess;
			break;
		}
	}

	if (!pTarget)
	{
		printf("[GM] LOBBY_GM_EXIT - Target UID=%lld not found online\n", targetUID);
		SendSimpleAck(PROTOCOL_LOBBY_GM_EXIT_USER_ACK, -1);
		return;
	}

	printf("[GM] Lobby force disconnect - GM_UID=%lld, Target_UID=%lld (%s)\n",
		m_i64UID, pTarget->GetUID(), pTarget->GetNickname());

	// Send kick notification
	{
		i3NetworkPacket packet;
		char buffer[64];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_SERVER_MESSAGE_KICK_PLAYER;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

		uint8_t reason = 1;
		memcpy(buffer + offset, &reason, 1);				offset += 1;

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		pTarget->SendMessage(&packet);
	}

	pTarget->OnDisconnect(TRUE);

	SendSimpleAck(PROTOCOL_LOBBY_GM_EXIT_USER_ACK, 0);
}

// Send server announcement to this session
void GameSession::SendServerAnnounce(const char* pszMessage, uint16_t ui16MsgLen)
{
	if (!pszMessage || ui16MsgLen == 0)
		return;

	i3NetworkPacket packet;
	char buffer[512];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_SERVER_MESSAGE_ANNOUNCE;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	// Message length
	memcpy(buffer + offset, &ui16MsgLen, sizeof(uint16_t));	offset += sizeof(uint16_t);

	// Message content
	uint16_t copyLen = (ui16MsgLen > 480) ? 480 : ui16MsgLen;
	memcpy(buffer + offset, pszMessage, copyLen);			offset += copyLen;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}
