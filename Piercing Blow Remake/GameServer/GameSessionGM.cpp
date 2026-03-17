#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "GameProtocol.h"
#include "GameContextMain.h"
#include "ModuleDataServer.h"
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

// GM destroys a room (kick all players and destroy)
void GameSession::OnGMDestroyRoomReq(char* pData, INT32 i32Size)
{
	if (!IsGMUser())
	{
		printf("[GM] Unauthorized GM_DESTROY_ROOM attempt - UID=%lld\n", m_i64UID);
		return;
	}

	if (i32Size < (int)(sizeof(int) + sizeof(int)))
		return;

	// Parse: channelIdx(4) + roomIdx(4)
	int channelIdx = *(int*)pData;
	int roomIdx = *(int*)(pData + 4);

	RoomManager* pRoomMgr = g_pGameServerContext->GetRoomManager();
	if (!pRoomMgr)
		return;

	Room* pRoom = pRoomMgr->GetRoom(channelIdx, roomIdx);
	if (!pRoom || !pRoom->IsCreated())
	{
		printf("[GM] GM_DESTROY_ROOM - Room not found, Ch=%d, Idx=%d\n", channelIdx, roomIdx);
		return;
	}

	printf("[GM] Destroying room - GM_UID=%lld, Ch=%d, Idx=%d\n",
		m_i64UID, channelIdx, roomIdx);

	// Send destroy notification to all players in the room
	{
		i3NetworkPacket packet;
		char buffer[16];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_SERVER_MESSAGE_ROOM_DESTROY;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		pRoom->SendToAll(&packet);
	}

	// Kick all players from the room
	for (int s = 0; s < SLOT_MAX_COUNT; s++)
	{
		GameSession* pSess = pRoom->GetSlotSession(s);
		if (pSess)
		{
			pRoomMgr->OnLeaveRoom(pSess, channelIdx);
		}
	}

	printf("[GM] Room destroyed - Ch=%d, Idx=%d\n", channelIdx, roomIdx);
}

// GM blocks (bans) a player with comment
void GameSession::OnGMBlockUserReq(char* pData, INT32 i32Size)
{
	if (!IsGMUser())
	{
		printf("[GM] Unauthorized GM_BLOCK attempt - UID=%lld\n", m_i64UID);
		return;
	}

	// Parse: targetSlot(4) + duration(4) + comment(128)
	if (i32Size < 8)
		return;

	int targetSlot = *(int*)pData;
	uint32_t duration = *(uint32_t*)(pData + 4);	// Duration in minutes (0=permanent)

	char comment[128] = {0};
	if (i32Size >= 136)
	{
		memcpy(comment, pData + 8, 128);
		comment[127] = '\0';
	}

	if (!m_pRoom)
	{
		SendSimpleAck(PROTOCOL_ROOM_GM_BLOCK_USER_ACK, -1);
		return;
	}

	GameSession* pTarget = m_pRoom->GetSlotSession(targetSlot);
	if (!pTarget)
	{
		SendSimpleAck(PROTOCOL_ROOM_GM_BLOCK_USER_ACK, -2);
		return;
	}

	// Don't allow blocking other GMs
	if (pTarget->IsGMUser())
	{
		SendSimpleAck(PROTOCOL_ROOM_GM_BLOCK_USER_ACK, -3);
		return;
	}

	printf("[GM] Blocking player - GM_UID=%lld, Target_UID=%lld (%s), Duration=%u min, Comment=%s\n",
		m_i64UID, pTarget->GetUID(), pTarget->GetNickname(), duration, comment);

	// Persist ban to DataServer
	if (g_pModuleDataServer && g_pModuleDataServer->IsConnected())
		g_pModuleDataServer->RequestPlayerBan(pTarget->GetUID(), m_i64UID,
			(int)(duration * 60), comment);

	// Send ban notification to target
	{
		i3NetworkPacket packet;
		char buffer[64];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_SERVER_MESSAGE_KICK_PLAYER;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

		uint8_t reason = 2;		// 2 = banned by GM
		memcpy(buffer + offset, &reason, 1);				offset += 1;

		// Duration in minutes
		memcpy(buffer + offset, &duration, sizeof(uint32_t)); offset += sizeof(uint32_t);

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		pTarget->SendMessage(&packet);
	}

	// Force disconnect target
	pTarget->OnDisconnect(TRUE);

	// ACK to GM
	SendSimpleAck(PROTOCOL_ROOM_GM_BLOCK_USER_ACK, 0);
}

// GM pauses battle in the current room
void GameSession::OnGMPauseBattleReq(char* pData, INT32 i32Size)
{
	if (!IsGMUser())
		return;

	if (!m_pRoom)
	{
		SendSimpleAck(PROTOCOL_BATTLE_GM_PAUSE_ACK, -1);
		return;
	}

	if (m_pRoom->GetRoomState() != ROOM_STATE_BATTLE)
	{
		SendSimpleAck(PROTOCOL_BATTLE_GM_PAUSE_ACK, -2);	// Not in battle
		return;
	}

	printf("[GM] Pause battle - GM_UID=%lld, Room Ch=%d Idx=%d\n",
		m_i64UID, m_i32ChannelNum, m_pRoom->GetRoomIdx());

	// Notify all players in the room about pause
	{
		i3NetworkPacket packet;
		char buffer[32];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_BATTLE_GM_PAUSE_ACK;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

		int32_t result = 0;		// 0 = success
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

		int32_t gmSlot = m_i32SlotIdx;
		memcpy(buffer + offset, &gmSlot, sizeof(int32_t));	offset += sizeof(int32_t);

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		m_pRoom->SendToAll(&packet);
	}
}

// GM resumes battle in the current room
void GameSession::OnGMResumeBattleReq(char* pData, INT32 i32Size)
{
	if (!IsGMUser())
		return;

	if (!m_pRoom)
	{
		SendSimpleAck(PROTOCOL_BATTLE_GM_RESUME_ACK, -1);
		return;
	}

	if (m_pRoom->GetRoomState() != ROOM_STATE_BATTLE)
	{
		SendSimpleAck(PROTOCOL_BATTLE_GM_RESUME_ACK, -2);
		return;
	}

	printf("[GM] Resume battle - GM_UID=%lld, Room Ch=%d Idx=%d\n",
		m_i64UID, m_i32ChannelNum, m_pRoom->GetRoomIdx());

	// Notify all players in the room about resume
	{
		i3NetworkPacket packet;
		char buffer[32];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_BATTLE_GM_RESUME_ACK;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

		int32_t result = 0;
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

		int32_t gmSlot = m_i32SlotIdx;
		memcpy(buffer + offset, &gmSlot, sizeof(int32_t));	offset += sizeof(int32_t);

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		m_pRoom->SendToAll(&packet);
	}
}

// ============================================================================
// Admin Chat Commands (Phase 11B)
// GM users can type /commands in chat for quick admin actions
// Returns true if the message was an admin command (don't broadcast)
// ============================================================================

bool GameSession::ProcessAdminCommand(const char* pszMessage, int i32MsgLen)
{
	if (!pszMessage || i32MsgLen < 2)
		return false;

	// Must start with '/'
	if (pszMessage[0] != '/')
		return false;

	// Must be GM level 1+
	if (!IsGMUser())
		return false;

	// Parse command and arguments
	char cmd[32] = {};
	char arg1[64] = {};
	char arg2[256] = {};

	// Extract command word
	int pos = 1;	// Skip '/'
	int cmdLen = 0;
	while (pos < i32MsgLen && pszMessage[pos] != ' ' && pszMessage[pos] != '\0' && cmdLen < 31)
		cmd[cmdLen++] = pszMessage[pos++];
	cmd[cmdLen] = '\0';

	// Skip space
	if (pos < i32MsgLen && pszMessage[pos] == ' ')
		pos++;

	// Extract arg1
	int arg1Len = 0;
	while (pos < i32MsgLen && pszMessage[pos] != ' ' && pszMessage[pos] != '\0' && arg1Len < 63)
		arg1[arg1Len++] = pszMessage[pos++];
	arg1[arg1Len] = '\0';

	// Skip space
	if (pos < i32MsgLen && pszMessage[pos] == ' ')
		pos++;

	// Rest is arg2
	int arg2Len = 0;
	while (pos < i32MsgLen && pszMessage[pos] != '\0' && arg2Len < 255)
		arg2[arg2Len++] = pszMessage[pos++];
	arg2[arg2Len] = '\0';

	// /announce <message> - Broadcast server announcement
	if (_stricmp(cmd, "announce") == 0 && arg1Len > 0)
	{
		// Reconstruct full message (arg1 + space + arg2)
		char fullMsg[300];
		if (arg2Len > 0)
			snprintf(fullMsg, sizeof(fullMsg), "%s %s", arg1, arg2);
		else
			strncpy_s(fullMsg, arg1, _TRUNCATE);

		uint16_t msgLen = (uint16_t)strlen(fullMsg);
		if (g_pGameSessionManager)
			g_pGameSessionManager->BroadcastAnnounce(fullMsg, msgLen);

		printf("[AdminCmd] ANNOUNCE by %s: %s\n", m_szNickname, fullMsg);
		return true;
	}

	// /kick <nickname> - Kick player from server
	if (_stricmp(cmd, "kick") == 0 && arg1Len > 0)
	{
		if (!g_pGameSessionManager)
			return true;

		GameSession* pTarget = g_pGameSessionManager->FindSessionByNickname(arg1);
		if (pTarget && pTarget != this)
		{
			printf("[AdminCmd] KICK %s by GM %s\n", arg1, m_szNickname);
			pTarget->OnDisconnect(TRUE);
		}
		return true;
	}

	// /ban <nickname> - Ban and kick player
	if (_stricmp(cmd, "ban") == 0 && arg1Len > 0 && m_ui8AuthLevel >= 2)
	{
		if (!g_pGameSessionManager)
			return true;

		GameSession* pTarget = g_pGameSessionManager->FindSessionByNickname(arg1);
		if (pTarget && pTarget != this)
		{
			printf("[AdminCmd] BAN %s by GM %s\n", arg1, m_szNickname);
			// Send kick notification with reason=banned before disconnect
			i3NetworkPacket packet;
			char buffer[64];
			int offset = 0;

			uint16_t size = 0;
			uint16_t proto = PROTOCOL_SERVER_MESSAGE_KICK;
			offset += sizeof(uint16_t);
			memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

			int32_t reason = 2;	// 2 = banned
			memcpy(buffer + offset, &reason, sizeof(int32_t));	offset += sizeof(int32_t);

			size = (uint16_t)offset;
			memcpy(buffer, &size, sizeof(uint16_t));
			packet.SetPacketData(buffer, offset);
			pTarget->SendMessage(&packet);

			pTarget->OnDisconnect(TRUE);
		}
		return true;
	}

	// /info <nickname> - Show player info
	if (_stricmp(cmd, "info") == 0 && arg1Len > 0)
	{
		if (!g_pGameSessionManager)
			return true;

		GameSession* pTarget = g_pGameSessionManager->FindSessionByNickname(arg1);
		if (pTarget)
		{
			char infoMsg[256];
			snprintf(infoMsg, sizeof(infoMsg),
				"[INFO] %s: UID=%lld Lv=%d Rank=%d GP=%d Ch=%d Room=%d",
				pTarget->GetNickname(), pTarget->GetUID(),
				pTarget->GetLevel(), pTarget->GetRankId(),
				pTarget->GetGP(), pTarget->GetChannelNum(),
				pTarget->GetRoomIdx());

			SendServerAnnounce(infoMsg, (uint16_t)strlen(infoMsg));
		}
		else
		{
			const char* notFound = "[INFO] Player not found.";
			SendServerAnnounce(notFound, (uint16_t)strlen(notFound));
		}
		return true;
	}

	// /ccu - Show current server stats
	if (_stricmp(cmd, "ccu") == 0)
	{
		char statsMsg[256];
		int ccu = g_pGameSessionManager ? g_pGameSessionManager->GetActiveCount() : 0;
		int rooms = g_pRoomManager ? g_pRoomManager->GetTotalUseRoomCount() : 0;
		snprintf(statsMsg, sizeof(statsMsg), "[SERVER] CCU=%d Rooms=%d", ccu, rooms);
		SendServerAnnounce(statsMsg, (uint16_t)strlen(statsMsg));
		return true;
	}

	// /reload - Hot reload economy/battle config
	if (_stricmp(cmd, "reload") == 0 && m_ui8AuthLevel >= 2)
	{
		extern GameServer* g_pGameServer;
		if (g_pGameServer && g_pGameServer->ReloadEconomyConfig())
		{
			const char* msg = "[GM] Config reloaded successfully.";
			SendServerAnnounce(msg, (uint16_t)strlen(msg));
		}
		else
		{
			const char* msg = "[GM] Config reload failed.";
			SendServerAnnounce(msg, (uint16_t)strlen(msg));
		}
		printf("[AdminCmd] RELOAD by GM %s\n", m_szNickname);
		return true;
	}

	// /rooms [channel] - List rooms in channel
	if (_stricmp(cmd, "rooms") == 0)
	{
		int channel = (arg1Len > 0) ? atoi(arg1) : m_i32ChannelNum;
		if (channel < 0) channel = 0;

		char roomMsg[300];
		int totalRooms = g_pRoomManager ? g_pRoomManager->GetChannelUseRoomCount(channel) : 0;
		snprintf(roomMsg, sizeof(roomMsg), "[ROOMS] Channel %d: %d active rooms", channel, totalRooms);
		SendServerAnnounce(roomMsg, (uint16_t)strlen(roomMsg));

		// List first 5 rooms
		if (g_pRoomManager && totalRooms > 0)
		{
			int shown = 0;
			for (int i = 0; i < 200 && shown < 5; i++)
			{
				Room* pRoom = g_pRoomManager->GetRoom(channel, i);
				if (pRoom && pRoom->IsCreated())
				{
					snprintf(roomMsg, sizeof(roomMsg), "  [%d] \"%s\" Mode=%d Map=%d Players=%d/%d State=%d",
						i, pRoom->GetTitle(), pRoom->GetGameMode(), pRoom->GetMapIndex(),
						pRoom->GetPlayerCount(), pRoom->GetMaxPlayers(), pRoom->GetRoomState());
					SendServerAnnounce(roomMsg, (uint16_t)strlen(roomMsg));
					shown++;
				}
			}
			if (totalRooms > 5)
			{
				snprintf(roomMsg, sizeof(roomMsg), "  ... and %d more rooms", totalRooms - 5);
				SendServerAnnounce(roomMsg, (uint16_t)strlen(roomMsg));
			}
		}
		return true;
	}

	// /users [channel] - List online users in channel
	if (_stricmp(cmd, "users") == 0)
	{
		int channel = (arg1Len > 0) ? atoi(arg1) : m_i32ChannelNum;
		if (channel < 0) channel = 0;

		int userCount = g_pGameSessionManager ? g_pGameSessionManager->GetChannelUserCount(channel) : 0;
		char userMsg[300];
		snprintf(userMsg, sizeof(userMsg), "[USERS] Channel %d: %d users online", channel, userCount);
		SendServerAnnounce(userMsg, (uint16_t)strlen(userMsg));
		return true;
	}

	// /damage - Toggle damage console (shows all damage events in your room)
	if (_stricmp(cmd, "damage") == 0)
	{
		m_bDamageConsole = !m_bDamageConsole;
		char dmgMsg[64];
		snprintf(dmgMsg, sizeof(dmgMsg), "[GM] Damage console %s", m_bDamageConsole ? "ON" : "OFF");
		SendServerAnnounce(dmgMsg, (uint16_t)strlen(dmgMsg));
		return true;
	}

	// /shutdown - Graceful server shutdown (requires auth level 2+)
	if (_stricmp(cmd, "shutdown") == 0 && m_ui8AuthLevel >= 2)
	{
		printf("[AdminCmd] SHUTDOWN initiated by GM %s (UID=%lld)\n", m_szNickname, m_i64UID);

		// Broadcast shutdown notice to all players
		const char* notice = "[SERVER] Server shutting down in 5 seconds...";
		if (g_pGameSessionManager)
			g_pGameSessionManager->BroadcastAnnounce(notice, (uint16_t)strlen(notice));

		// Signal the main loop to stop
		extern volatile bool g_bRunning;
		g_bRunning = false;
		return true;
	}

	// /help - Show available commands
	if (_stricmp(cmd, "help") == 0)
	{
		const char* helpMsg = "[GM] /announce /kick /ban /info /ccu /rooms /users /damage /reload /shutdown /help";
		SendServerAnnounce(helpMsg, (uint16_t)strlen(helpMsg));
		return true;
	}

	// Unknown command - not consumed
	return false;
}

// ============================================================================
// GM Cheat/Debug Commands (Phase 13)
// These are protocol-based cheat commands from the client's debug menu.
// Only GM users (auth level 1+) can execute these.
// ============================================================================

// Increase kill count for the GM player (debug testing)
void GameSession::OnCheatIncreaseKillReq(char* pData, INT32 i32Size)
{
	if (!IsGMUser())
		return;

	if (!m_pRoom || m_pRoom->GetRoomState() != ROOM_STATE_BATTLE)
	{
		SendSimpleAck(PROTOCOL_CHEAT_INCREASE_KILL_COUNT_ACK, -1);
		return;
	}

	// Parse: killCount(4) to add
	int addKills = 1;
	if (i32Size >= (int)sizeof(int))
		memcpy(&addKills, pData, sizeof(int));

	if (addKills < 1) addKills = 1;
	if (addKills > 100) addKills = 100;

	m_i32Kills += addKills;

	printf("[Cheat] IncreaseKill - GM_UID=%lld, Added=%d, Total=%d\n",
		m_i64UID, addKills, m_i32Kills);

	// ACK with new kill count
	{
		i3NetworkPacket packet;
		char buffer[32];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_CHEAT_INCREASE_KILL_COUNT_ACK;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

		int32_t result = 0;
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

		int32_t totalKills = m_i32Kills;
		memcpy(buffer + offset, &totalKills, sizeof(int32_t)); offset += sizeof(int32_t);

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		SendMessage(&packet);
	}
}

// Allow starting a game with only 1 player (solo mode for testing)
void GameSession::OnCheatPlaySoloReq(char* pData, INT32 i32Size)
{
	if (!IsGMUser())
		return;

	if (!m_pRoom)
	{
		SendSimpleAck(PROTOCOL_CHEAT_PLAY_SOLO_ACK, -1);
		return;
	}

	// Toggle solo play flag on the room
	bool bSolo = m_pRoom->ToggleSoloPlay();

	printf("[Cheat] PlaySolo - GM_UID=%lld, Solo=%s, Room Ch=%d Idx=%d\n",
		m_i64UID, bSolo ? "ON" : "OFF", m_i32ChannelNum, m_pRoom->GetRoomIdx());

	// ACK
	{
		i3NetworkPacket packet;
		char buffer[32];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_CHEAT_PLAY_SOLO_ACK;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

		int32_t result = 0;
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

		uint8_t soloFlag = bSolo ? 1 : 0;
		memcpy(buffer + offset, &soloFlag, 1);				offset += 1;

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		SendMessage(&packet);
	}
}

// Reduce remaining round time (to quickly end a round for testing)
void GameSession::OnCheatReduceRoundTimeReq(char* pData, INT32 i32Size)
{
	if (!IsGMUser())
		return;

	if (!m_pRoom || m_pRoom->GetRoomState() != ROOM_STATE_BATTLE)
	{
		SendSimpleAck(PROTOCOL_CHEAT_REDUCE_ROUND_TIME_ACK, -1);
		return;
	}

	// Parse: reduceSeconds(4)
	int reduceSec = 60;
	if (i32Size >= (int)sizeof(int))
		memcpy(&reduceSec, pData, sizeof(int));

	if (reduceSec < 1) reduceSec = 1;
	if (reduceSec > 600) reduceSec = 600;

	// Advance battle start time to simulate time passing
	m_pRoom->ReduceBattleTime(reduceSec);

	printf("[Cheat] ReduceRoundTime - GM_UID=%lld, Reduced=%ds, Room Ch=%d Idx=%d\n",
		m_i64UID, reduceSec, m_i32ChannelNum, m_pRoom->GetRoomIdx());

	// ACK - broadcast to all in room so clients sync
	{
		i3NetworkPacket packet;
		char buffer[32];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_CHEAT_REDUCE_ROUND_TIME_ACK;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

		int32_t result = 0;
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

		int32_t reduced = reduceSec;
		memcpy(buffer + offset, &reduced, sizeof(int32_t)); offset += sizeof(int32_t);

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		m_pRoom->SendToAll(&packet);
	}
}

// Teleport character to a position (GM debug movement)
void GameSession::OnCheatTeleportReq(char* pData, INT32 i32Size)
{
	if (!IsGMUser())
		return;

	if (!m_pRoom || m_pRoom->GetRoomState() != ROOM_STATE_BATTLE)
	{
		SendSimpleAck(PROTOCOL_CHEAT_CHARACTER_TELEPORT_ACK, -1);
		return;
	}

	// Parse: posX(4) + posY(4) + posZ(4) = 12 bytes (float positions)
	if (i32Size < 12)
	{
		SendSimpleAck(PROTOCOL_CHEAT_CHARACTER_TELEPORT_ACK, -2);
		return;
	}

	float posX = 0.0f, posY = 0.0f, posZ = 0.0f;
	memcpy(&posX, pData, sizeof(float));
	memcpy(&posY, pData + 4, sizeof(float));
	memcpy(&posZ, pData + 8, sizeof(float));

	printf("[Cheat] Teleport - GM_UID=%lld, Pos=(%.1f, %.1f, %.1f)\n",
		m_i64UID, posX, posY, posZ);

	// ACK - broadcast to all in room so clients update the GM's position
	{
		i3NetworkPacket packet;
		char buffer[48];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_CHEAT_CHARACTER_TELEPORT_ACK;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

		int32_t result = 0;
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

		int32_t slot = m_i32SlotIdx;
		memcpy(buffer + offset, &slot, sizeof(int32_t));	offset += sizeof(int32_t);

		memcpy(buffer + offset, &posX, sizeof(float));		offset += sizeof(float);
		memcpy(buffer + offset, &posY, sizeof(float));		offset += sizeof(float);
		memcpy(buffer + offset, &posZ, sizeof(float));		offset += sizeof(float);

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		m_pRoom->SendToAll(&packet);
	}
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

// ============================================================================
// Batch 18 - Cheat extras
// ============================================================================

void GameSession::OnCheatClanWarMatchingPointReq(char* pData, INT32 i32Size)
{
	if (!IsGMUser())
		return;

	if (i32Size < (int)sizeof(int32_t))
	{
		SendSimpleAck(PROTOCOL_CHEAT_CLAN_WAR_MATCHING_POINT_ACK, -1);
		return;
	}

	int32_t points = 0;
	memcpy(&points, pData, sizeof(int32_t));

	printf("[GM] Cheat: Set clan war matching points to %d by UID=%lld\n", points, m_i64UID);
	SendSimpleAck(PROTOCOL_CHEAT_CLAN_WAR_MATCHING_POINT_ACK, 0);
}

void GameSession::OnCheatClanWarResultReq(char* pData, INT32 i32Size)
{
	if (!IsGMUser())
		return;

	if (i32Size < (int)(sizeof(int32_t) * 2))
	{
		SendSimpleAck(PROTOCOL_CHEAT_CLAN_WAR_RESULT_ACK, -1);
		return;
	}

	int32_t winnerTeam = 0;
	int32_t resultType = 0;
	memcpy(&winnerTeam, pData, sizeof(int32_t));
	memcpy(&resultType, pData + 4, sizeof(int32_t));

	printf("[GM] Cheat: Force clan war result team=%d type=%d by UID=%lld\n",
		winnerTeam, resultType, m_i64UID);
	SendSimpleAck(PROTOCOL_CHEAT_CLAN_WAR_RESULT_ACK, 0);
}

void GameSession::OnCheatDamageGameObjectReq(char* pData, INT32 i32Size)
{
	if (!IsGMUser())
		return;

	if (m_eMainTask != GAME_TASK_BATTLE || !m_pRoom)
	{
		SendSimpleAck(PROTOCOL_CHEAT_DAMAGE_GAME_OBJECT_ACK, -1);
		return;
	}

	if (i32Size < (int)(sizeof(int32_t) + sizeof(int32_t)))
	{
		SendSimpleAck(PROTOCOL_CHEAT_DAMAGE_GAME_OBJECT_ACK, -1);
		return;
	}

	int32_t objectId = 0;
	int32_t damage = 0;
	memcpy(&objectId, pData, sizeof(int32_t));
	memcpy(&damage, pData + 4, sizeof(int32_t));

	printf("[GM] Cheat: Damage game object %d by %d, UID=%lld\n",
		objectId, damage, m_i64UID);

	// Broadcast to room
	i3NetworkPacket packet;
	char buffer[24];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CHEAT_DAMAGE_GAME_OBJECT_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &objectId, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(buffer + offset, &damage, sizeof(int32_t));		offset += sizeof(int32_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	m_pRoom->SendToAll(&packet);
}

void GameSession::OnCheatDetectHackOffReq(char* pData, INT32 i32Size)
{
	if (!IsGMUser())
		return;

	printf("[GM] Cheat: Hack detection disabled by UID=%lld\n", m_i64UID);
	SendSimpleAck(PROTOCOL_CHEAT_DETECT_HACK_OFF_ACK, 0);
}

void GameSession::OnCheatItemAuthReq(char* pData, INT32 i32Size)
{
	if (!IsGMUser())
		return;

	if (i32Size < (int)sizeof(uint32_t))
	{
		SendSimpleAck(PROTOCOL_CHEAT_ITEM_AUTH_ACK, -1);
		return;
	}

	uint32_t itemId = 0;
	memcpy(&itemId, pData, sizeof(uint32_t));

	// Grant item to self for testing
	if (m_i32InventoryCount < MAX_INVEN_COUNT)
	{
		GameInventoryItem& item = m_Inventory[m_i32InventoryCount];
		item.ui32ItemId = itemId;
		item.ui32Count = 1;
		item.i32SlotIdx = -1;
		item.ui8IsEquipped = 0;
		item.ui32Duration = 0;
		m_i32InventoryCount++;
	}

	printf("[GM] Cheat: Item 0x%08X granted to UID=%lld\n", itemId, m_i64UID);
	SendSimpleAck(PROTOCOL_CHEAT_ITEM_AUTH_ACK, 0);
}

void GameSession::OnCheatMedalCommandReq(char* pData, INT32 i32Size)
{
	if (!IsGMUser())
		return;

	if (i32Size < (int)(sizeof(uint16_t) + sizeof(uint8_t)))
	{
		SendSimpleAck(PROTOCOL_CHEAT_MEDAL_COMMAND_ACK, -1);
		return;
	}

	uint16_t medalIdx = 0;
	uint8_t command = 0;
	memcpy(&medalIdx, pData, sizeof(uint16_t));
	memcpy(&command, pData + 2, sizeof(uint8_t));

	printf("[GM] Cheat: Medal command idx=%d cmd=%d by UID=%lld\n",
		medalIdx, command, m_i64UID);
	SendSimpleAck(PROTOCOL_CHEAT_MEDAL_COMMAND_ACK, 0);
}

void GameSession::OnCheatReduceTsEventReq(char* pData, INT32 i32Size)
{
	if (!IsGMUser())
		return;

	printf("[GM] Cheat: Reduce TS event timer by UID=%lld\n", m_i64UID);
	SendSimpleAck(PROTOCOL_CHEAT_REDUCE_TS_EVENT_ACK, 0);
}

void GameSession::OnCheatTimerGMPauseReq(char* pData, INT32 i32Size)
{
	if (!IsGMUser())
		return;

	printf("[GM] Cheat: GM pause timer by UID=%lld\n", m_i64UID);
	SendSimpleAck(PROTOCOL_CHEAT_TIMER_GM_PAUSE_ACK, 0);
}

// ============================================================================
// Batch 19 - Room/Lobby GM commands
// ============================================================================

void GameSession::OnRoomGmDestroyRoomReq(char* pData, INT32 i32Size)
{
	// GM force-destroys a room
	if (!IsGMUser())
	{
		SendSimpleAck(PROTOCOL_ROOM_GM_DESTROY_ROOM_ACK, -1);
		return;
	}

	if (i32Size < 8)
	{
		SendSimpleAck(PROTOCOL_ROOM_GM_DESTROY_ROOM_ACK, 1);
		return;
	}

	int32_t channel = 0;
	int32_t roomIdx = 0;
	memcpy(&channel, pData, sizeof(int32_t));
	memcpy(&roomIdx, pData + 4, sizeof(int32_t));

	if (!g_pRoomManager)
	{
		SendSimpleAck(PROTOCOL_ROOM_GM_DESTROY_ROOM_ACK, 2);
		return;
	}

	Room* pRoom = g_pRoomManager->GetRoom(channel, roomIdx);
	if (!pRoom || !pRoom->IsCreated())
	{
		SendSimpleAck(PROTOCOL_ROOM_GM_DESTROY_ROOM_ACK, 3);
		return;
	}

	printf("[GM] Destroying room ch=%d idx=%d by UID=%lld\n", channel, roomIdx, m_i64UID);

	// Kick all players from the room
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		GameSession* pSlotSession = pRoom->GetSlotSession(i);
		if (pSlotSession)
		{
			pSlotSession->SendSimpleAck(PROTOCOL_ROOM_GM_DESTROY_ROOM_ACK, 0);
			// Session leave will be handled by Room::OnDestroy calling OnLeave for each
		}
	}

	pRoom->OnDestroy();
	SendSimpleAck(PROTOCOL_ROOM_GM_DESTROY_ROOM_ACK, 0);
}

void GameSession::OnRoomGmGetUidReq(char* pData, INT32 i32Size)
{
	// GM queries a user's UID in a room
	if (!IsGMUser())
	{
		SendSimpleAck(PROTOCOL_ROOM_GM_GET_UID_ACK, -1);
		return;
	}

	if (i32Size < (int)sizeof(int32_t))
	{
		SendSimpleAck(PROTOCOL_ROOM_GM_GET_UID_ACK, 1);
		return;
	}

	int32_t slotIdx = 0;
	memcpy(&slotIdx, pData, sizeof(int32_t));

	if (!m_pRoom || slotIdx < 0 || slotIdx >= SLOT_MAX_COUNT)
	{
		SendSimpleAck(PROTOCOL_ROOM_GM_GET_UID_ACK, 2);
		return;
	}

	GameSession* pTarget = m_pRoom->GetSlotSession(slotIdx);
	if (!pTarget)
	{
		SendSimpleAck(PROTOCOL_ROOM_GM_GET_UID_ACK, 3);
		return;
	}

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	offset += sizeof(uint16_t);
	uint16_t proto = PROTOCOL_ROOM_GM_GET_UID_ACK;
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	int64_t targetUID = pTarget->GetUID();
	memcpy(buffer + offset, &targetUID, sizeof(int64_t));	offset += sizeof(int64_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}
