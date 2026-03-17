#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "GameContextMain.h"
#include "GameSessionManager.h"
#include "Room.h"
#include "RoomManager.h"

// ============================================================================
// Channel Handlers
// ============================================================================

void GameSession::OnChannelListReq(char* pData, INT32 i32Size)
{
	if (!g_pContextMain)
		return;

	uint8_t channelCount = g_pContextMain->m_ui8ChannelCount;

	i3NetworkPacket packet(PROTOCOL_BASE_GET_CHANNELLIST_ACK);
	packet.WriteData(&channelCount, sizeof(uint8_t));
	// Per-channel: type(1) + userCount(2) + maxUsers(2)
	for (uint8_t i = 0; i < channelCount; i++)
	{
		const GameChannelInfo& ch = g_pContextMain->GetChannelInfo(i);
		uint8_t chType = (uint8_t)ch.eType;
		packet.WriteData(&chType, 1);
		packet.WriteData(&ch.ui16CurrentUsers, 2);
		packet.WriteData(&ch.ui16MaxUsers, 2);
	}
	SendPacketMessage(&packet);
}

void GameSession::OnChannelEnterReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL || !g_pContextMain)
		return;

	if (i32Size < (int)sizeof(uint32_t))
		return;

	uint32_t channel = *(uint32_t*)pData;
	int32_t result = 0;

	if (channel >= g_pContextMain->m_ui8ChannelCount)
	{
		result = 1;	// Invalid channel
	}
	else
	{
		const GameChannelInfo& ch = g_pContextMain->GetChannelInfo((int)channel);

		// Check channel capacity
		if (ch.ui16CurrentUsers >= ch.ui16MaxUsers)
		{
			result = 2;	// Channel full
		}
		// Check channel type restrictions
		else switch (ch.eType)
		{
		case GAME_CHANNEL_TYPE_BEGIN1:
		case GAME_CHANNEL_TYPE_BEGIN2:
			if (ch.i32MaxLevel > 0 && m_i32Level > ch.i32MaxLevel)
				result = 3;	// Level too high for beginner channel
			break;

		case GAME_CHANNEL_TYPE_EXPERT:
		case GAME_CHANNEL_TYPE_EXPERT2:
			if (ch.i32MinLevel > 0 && m_i32Level < ch.i32MinLevel)
				result = 4;	// Level too low for expert channel
			break;

		case GAME_CHANNEL_TYPE_CLAN:
		case GAME_CHANNEL_TYPE_PCCAFE_CLAN:
			if (m_i32ClanId <= 0)
				result = 5;	// Must be in a clan
			break;

		default:
			break;
		}
	}

	if (result == 0)
	{
		// Leave old channel if any
		if (m_i32ChannelNum >= 0 && m_i32ChannelNum < MAX_GAME_CHANNELS)
		{
			GameChannelInfo& oldCh = g_pContextMain->GetChannelInfoMut(m_i32ChannelNum);
			if (oldCh.ui16CurrentUsers > 0)
				oldCh.ui16CurrentUsers--;
		}

		m_i32ChannelNum = (int)channel;
		m_eMainTask = GAME_TASK_CHANNEL;

		// Increment user count
		g_pContextMain->GetChannelInfoMut((int)channel).ui16CurrentUsers++;

		// Notify friends we're online (Phase 7A)
		NotifyFriendsStatusChange(1);	// 1 = online
	}

	i3NetworkPacket packet(PROTOCOL_BASE_SELECT_CHANNEL_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&channel, sizeof(uint32_t));
	SendPacketMessage(&packet);

	if (result == 0)
	{
		printf("[GameSession] Channel enter - Index=%d, Channel=%d, Type=%d\n", GetIndex(), channel, g_pContextMain->GetChannelInfo((int)channel).eType);//el identificador "GetIndex" no está definido

		// Send random map list for this channel (Phase 14D)
		OnMapRandomListReq(nullptr, 0);
	}
}

void GameSession::OnChannelLeaveReq(char* pData, INT32 i32Size)
{
	if (m_i32ChannelNum < 0)
		return;

	int oldChannel = m_i32ChannelNum;

	// Decrement user count
	if (g_pContextMain && oldChannel >= 0 && oldChannel < MAX_GAME_CHANNELS)
	{
		GameChannelInfo& ch = g_pContextMain->GetChannelInfoMut(oldChannel);
		if (ch.ui16CurrentUsers > 0)
			ch.ui16CurrentUsers--;
	}

	m_i32ChannelNum = -1;
	m_eMainTask = GAME_TASK_CHANNEL;

	SendSimpleAck(PROTOCOL_BASE_SELECT_CHANNEL_ACK, 0);

	printf("[GameSession] Channel leave - Index=%d, OldChannel=%d\n", GetIndex(), oldChannel);
}

// ============================================================================
// Lobby Handlers
// ============================================================================

void GameSession::OnLobbyEnterReq(char* pData, INT32 i32Size)
{
	if (m_i32ChannelNum < 0)
		return;

	if (g_pGameSessionManager)
		g_pGameSessionManager->OnEnterLobby(this, m_i32ChannelNum);

	m_eMainTask = GAME_TASK_LOBBY;
	SendSimpleAck(PROTOCOL_LOBBY_ENTER_ACK, 0);

	// Notify friends we entered lobby (Phase 7A)
	NotifyFriendLobbyEnter();
}

void GameSession::OnLobbyLeaveReq(char* pData, INT32 i32Size)
{
	if (g_pGameSessionManager && m_i32ChannelNum >= 0)
		g_pGameSessionManager->OnLeaveLobby(this, m_i32ChannelNum);

	m_eMainTask = GAME_TASK_CHANNEL;
	SendSimpleAck(PROTOCOL_LOBBY_LEAVE_ACK, 0);

	// Notify friends we left lobby (Phase 7A)
	NotifyFriendLobbyLeave();
}

void GameSession::OnGetRoomListReq(char* pData, INT32 i32Size)
{
	if (m_i32ChannelNum < 0 || !g_pRoomManager)
		return;

	g_pRoomManager->OnSendRoomList(this, m_i32ChannelNum);
}

void GameSession::OnLobbyChatReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_LOBBY || m_i32ChannelNum < 0)
		return;

	if (i32Size < 2)
		return;

	if (IsMuted())
		return;

	// Check for GM admin commands (starts with '/')
	if (i32Size >= 1 && pData[0] == '/')
	{
		if (ProcessAdminCommand(pData, i32Size))
			return;	// Command consumed, don't broadcast
	}

	// Forward to session manager for lobby broadcast
	if (g_pGameSessionManager)
		g_pGameSessionManager->OnSendLobbyChatting(this, pData, (uint16_t)i32Size);
}

// ============================================================================
// Map/Stage Data Handlers (Phase 4B)
// ============================================================================

void GameSession::OnMapVersionReq(char* pData, INT32 i32Size)
{
	if (!g_pContextMain)
		return;

	uint32_t mapVersion = g_pContextMain->m_ui32MapVersion;
	i3NetworkPacket packet(PROTOCOL_BASE_MAP_VERSION_ACK);
	packet.WriteData(&mapVersion, sizeof(uint32_t));
	SendPacketMessage(&packet);
}

void GameSession::OnMapListReq(char* pData, INT32 i32Size)
{
	if (!g_pContextMain)
		return;

	int32_t mapCount = g_pContextMain->m_i32MapCount;

	i3NetworkPacket packet(PROTOCOL_BASE_MAP_LIST_ACK);
	packet.WriteData(&mapCount, sizeof(int32_t));
	for (int i = 0; i < mapCount; i++)
	{
		const auto& map = g_pContextMain->m_Maps[i];
		packet.WriteData(&map.ui32StageId, sizeof(uint32_t));
		packet.WriteData(map.szName, 32);
		packet.WriteData(&map.ui8MinPlayers, 1);
		packet.WriteData(&map.ui8MaxPlayers, 1);
		uint8_t active = map.bActive ? 1 : 0;
		packet.WriteData(&active, 1);
	}
	SendPacketMessage(&packet);
}

void GameSession::OnMapRuleListReq(char* pData, INT32 i32Size)
{
	if (!g_pContextMain)
		return;

	// Send which maps support which game modes
	int32_t mapCount = g_pContextMain->m_i32MapCount;

	i3NetworkPacket packet(PROTOCOL_BASE_MAP_RULELIST_ACK);
	packet.WriteData(&mapCount, sizeof(int32_t));
	for (int i = 0; i < mapCount; i++)
	{
		const auto& map = g_pContextMain->m_Maps[i];
		packet.WriteData(&map.ui32StageId, sizeof(uint32_t));
		packet.WriteData(&map.ui8SupportedModes, 1);
		packet.WriteData(&map.ui8SupportedModes2, 1);
	}
	SendPacketMessage(&packet);
}

void GameSession::OnMapMatchingListReq(char* pData, INT32 i32Size)
{
	if (!g_pContextMain)
		return;

	// Send mode -> map matching table
	// For each game mode, list available map indices
	uint8_t modeCount = GameContextMain::MAX_STAGE_MODE_COUNT;

	i3NetworkPacket packet(PROTOCOL_BASE_MAP_MATCHINGLIST_ACK);
	packet.WriteData(&modeCount, 1);
	for (int mode = 0; mode < modeCount; mode++)
	{
		uint8_t modeId = (uint8_t)mode;
		uint8_t mapCountForMode = 0;
		for (int m = 0; m < g_pContextMain->m_i32MapCount; m++)
		{
			if (g_pContextMain->IsMapValidForMode(m, mode))
				mapCountForMode++;
		}
		packet.WriteData(&modeId, 1);
		packet.WriteData(&mapCountForMode, 1);
		for (int m = 0; m < g_pContextMain->m_i32MapCount; m++)
		{
			if (g_pContextMain->IsMapValidForMode(m, mode))
			{
				uint32_t stageId = g_pContextMain->m_Maps[m].ui32StageId;
				packet.WriteData(&stageId, sizeof(uint32_t));
			}
		}
	}
	SendPacketMessage(&packet);
}

// ============================================================================
// Quick Join
// ============================================================================

void GameSession::OnQuickJoinRoomReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask != GAME_TASK_LOBBY || m_i32ChannelNum < 0 || !g_pRoomManager)
		return;

	int roomIdx = g_pRoomManager->OnQuickJoinRoom(this, m_i32ChannelNum);

	int32_t result = (roomIdx >= 0) ? 0 : 1;

	if (roomIdx >= 0)
	{

		Room* pRoom = g_pRoomManager->GetRoom(m_i32ChannelNum, roomIdx);
		m_i32RoomIdx = roomIdx;
		m_i32SlotIdx = 0;
		m_pRoom = pRoom;
		m_eMainTask = GAME_TASK_READY_ROOM;
	}
	i3NetworkPacket packet(PROTOCOL_LOBBY_QUICKJOIN_ROOM_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&roomIdx, sizeof(int));
	SendPacketMessage(&packet);
}

// ============================================================================
// QuickJoin System (Phase 8)
// ============================================================================

void GameSession::OnQuickJoinStartReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask != GAME_TASK_LOBBY && m_eMainTask != GAME_TASK_CHANNEL)
		return;

	if (!g_pRoomManager)
		return;

	// Parse: ui8QuickJoinIdx(1) + QUICKJOIN_INFO[3] (each is uint32_t stageID)
	uint8_t quickJoinIdx = 0;
	uint32_t stageIds[MAX_QUICK_JOIN_INFO_COUNT] = {0};

	int off = 0;
	if (i32Size >= 1)
	{
		quickJoinIdx = *(uint8_t*)(pData + off);	off += 1;
	}

	for (int i = 0; i < MAX_QUICK_JOIN_INFO_COUNT && off + 4 <= i32Size; i++)
	{
		stageIds[i] = *(uint32_t*)(pData + off);	off += 4;
	}

	// Search for matching rooms using the selected stage ID
	uint32_t searchStageId = (quickJoinIdx < MAX_QUICK_JOIN_INFO_COUNT) ? stageIds[quickJoinIdx] : 0;
	RoomManager::QuickJoinResult qjResult = g_pRoomManager->SearchQuickJoinRoom(searchStageId, m_i32ChannelNum);

	int32_t result = 1;	// Default: not found

	if (qjResult.bFound)
	{
		// Try to join if same channel
		if (qjResult.i32ChannelIdx == m_i32ChannelNum)
		{
			int joinSlot = g_pRoomManager->OnJoinRoom(this, qjResult.i32ChannelIdx, qjResult.i32RoomIdx, TEAM_RED);
			if (joinSlot >= 0)
			{
				Room* pRoom = g_pRoomManager->GetRoom(qjResult.i32ChannelIdx, qjResult.i32RoomIdx);
				m_i32RoomIdx = qjResult.i32RoomIdx;
				m_pRoom = pRoom;
				m_eMainTask = GAME_TASK_READY_ROOM;
				result = 0;		// Perfect match, joined
			}
			else
			{
				result = 1;		// Failed to join
			}
		}
		else
		{
			result = 2;	// 2nd best - needs channel switch
		}
	}
	else if (qjResult.bHasFallback)
	{
		result = 2;		// 2nd best available
	}

	// Channel/Room info for 2nd best
	uint8_t chIdx = 0;
	uint32_t rmIdx = 0;
	uint8_t chType = 0;
	uint8_t rmState = 0;

	if (result == 0 && qjResult.bFound)
	{
		chIdx = (uint8_t)qjResult.i32ChannelIdx;
		rmIdx = (uint32_t)qjResult.i32RoomIdx;
	}
	else if (result == 2)
	{
		if (qjResult.bFound)
		{
			chIdx = (uint8_t)qjResult.i32ChannelIdx;
			rmIdx = (uint32_t)qjResult.i32RoomIdx;
		}
		else if (qjResult.bHasFallback)
		{
			chIdx = (uint8_t)qjResult.i32FallbackChannelIdx;
			rmIdx = (uint32_t)qjResult.i32FallbackRoomIdx;
		}
	}

	i3NetworkPacket packet(PROTOCOL_QUICKJOIN_START_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	for (int i = 0; i < MAX_QUICK_JOIN_INFO_COUNT; i++)
		packet.WriteData(&stageIds[i], sizeof(uint32_t));
	packet.WriteData(&chIdx, sizeof(uint8_t));
	packet.WriteData(&rmIdx, sizeof(uint32_t));
	packet.WriteData(&chType, sizeof(uint8_t));
	packet.WriteData(&rmState, sizeof(uint8_t));
	SendPacketMessage(&packet);
}

void GameSession::OnQuickJoinEnter2ndBestReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask != GAME_TASK_LOBBY && m_eMainTask != GAME_TASK_CHANNEL)
		return;

	if (!g_pRoomManager || i32Size < 8)
		return;

	int off = 0;
	uint32_t channelIdx = *(uint32_t*)(pData + off);	off += 4;
	uint32_t roomIdx = *(uint32_t*)(pData + off);		off += 4;

	if ((int)channelIdx >= (int)g_pRoomManager->GetChannelCount())
		return;

	int joinResult = g_pRoomManager->OnJoinRoom(this, (int)channelIdx, (int)roomIdx, TEAM_RED);

	int32_t result = (joinResult >= 0) ? 0 : 1;

	if (joinResult >= 0)
	{
		Room* pRoom = g_pRoomManager->GetRoom((int)channelIdx, (int)roomIdx);
		m_i32RoomIdx = (int)roomIdx;
		m_pRoom = pRoom;
		m_eMainTask = GAME_TASK_READY_ROOM;
	}

	i3NetworkPacket packet(PROTOCOL_QUICKJOIN_ENTER_REC_ROOM_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	if (joinResult >= 0)
	{
		packet.WriteData(&channelIdx, sizeof(uint32_t));
		packet.WriteData(&roomIdx, sizeof(uint32_t));
	}
	SendPacketMessage(&packet);
}

// ============================================================================
// Lobby User Features (Phase 7B)
// ============================================================================

void GameSession::OnLobbyViewUserItemReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_LOBBY)
		return;

	// Parse target nickname
	if (i32Size < 64)
	{
		SendSimpleAck(PROTOCOL_LOBBY_VIEW_USER_ITEM_ACK, 1);
		return;
	}

	char targetNick[64] = {0};
	memcpy(targetNick, pData, 64);
	targetNick[63] = '\0';

	// Find target session
	GameSession* pTarget = nullptr;
	if (g_pGameSessionManager)
		pTarget = g_pGameSessionManager->FindSessionByNickname(targetNick);

	int32_t result = 1;
	char nick[64] = {0};
	int32_t level = 0, rankId = 0, kills = 0, deaths = 0;
	GameCharaEquip equip = {};

	if (pTarget)
	{
		result = 0;
		strncpy_s(nick, pTarget->GetNickname(), _TRUNCATE);
		level = pTarget->GetLevel();
		rankId = pTarget->GetRankId();
		kills = pTarget->GetKills();
		deaths = pTarget->GetDeaths();
		equip = pTarget->GetActiveEquipment();
	}
	i3NetworkPacket packet(PROTOCOL_LOBBY_VIEW_USER_ITEM_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(nick, 64);
	packet.WriteData(&level, 4);
	packet.WriteData(&rankId, 4);
	packet.WriteData(&kills, 4);
	packet.WriteData(&deaths, 4);
	if (result == 0)
	{
		uint32_t charaId = GetCharaSlot(m_ui8ActiveCharaSlot).ui32CharaId;
		packet.WriteData(&charaId, 4);
		for (int i = 0; i < CHAR_EQUIP_WEAPON_COUNT; i++)
			packet.WriteData(&equip.ui32WeaponIds[i], 4);
	}
	SendPacketMessage(&packet);
}

void GameSession::OnLobbyFindNickNameReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_LOBBY)
		return;

	if (i32Size < 64)
	{
		SendSimpleAck(PROTOCOL_LOBBY_FIND_NICK_NAME_ACK, 1);//el identificador "PROTOCOL_LOBBY_FIND_NICK_NAME_ACK" no está definido
		return;
	}

	char searchNick[64] = {0};
	memcpy(searchNick, pData, 64);
	searchNick[63] = '\0';

	// Search for user online
	GameSession* pTarget = nullptr;
	if (g_pGameSessionManager)
		pTarget = g_pGameSessionManager->FindSessionByNickname(searchNick);

	int32_t result = 1;
	char nick[64] = {0};
	int32_t level = 0, rankId = 0, channelNum = -1, roomIdx = -1;

	if (pTarget)
	{
		result = 0;
		strncpy_s(nick, pTarget->GetNickname(), _TRUNCATE);
		level = pTarget->GetLevel();
		rankId = pTarget->GetRankId();
		channelNum = pTarget->GetChannelNum();
		roomIdx = pTarget->GetRoomIdx();
	}
	i3NetworkPacket packet(PROTOCOL_LOBBY_FIND_NICK_NAME_ACK);//el identificador "PROTOCOL_LOBBY_FIND_NICK_NAME_ACK" no está definido
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(nick, 64);
	packet.WriteData(&level, 4);
	packet.WriteData(&rankId, 4);
	packet.WriteData(&channelNum, 4);
	packet.WriteData(&roomIdx, 4);
	SendPacketMessage(&packet);
}

// ============================================================================
// Megaphone (Phase 7B)
// ============================================================================

void GameSession::OnMegaphoneReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	// Parse message
	if (i32Size < 2)
	{
		SendSimpleAck(PROTOCOL_BASE_MEGAPHONE_ACK, 1);
		return;
	}

	uint16_t msgLen = *(uint16_t*)pData;
	if (msgLen <= 0 || msgLen > 256 || i32Size < 2 + (int)msgLen)
	{
		SendSimpleAck(PROTOCOL_BASE_MEGAPHONE_ACK, 2);
		return;
	}

	char message[258] = {0};
	memcpy(message, pData + 2, msgLen);
	message[msgLen] = '\0';

	printf("[GameSession] Megaphone - %s: %s\n", m_szNickname, message);

	// Broadcast to all players in the same channel
	int32_t result = 0;

	// Sender nickname
	char nick[64] = {0};
	strncpy_s(nick, m_szNickname, _TRUNCATE);

	i3NetworkPacket packet(PROTOCOL_BASE_MEGAPHONE_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(nick, 64);
	packet.WriteData(&msgLen, sizeof(uint16_t));
	packet.WriteData(message, msgLen);
	// Broadcast to all sessions in same channel
	if (g_pGameSessionManager)
		g_pGameSessionManager->BroadcastToChannel(m_i32ChannelNum, &packet);
}

// ============================================================================
// Random Map System (Phase 14D)
// ============================================================================

void GameSession::OnMapRandomListReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BASE_MAP_RANDOM_LIST_ACK - Send random map pool for current channel
	// Server-push protocol (ACK only, sent on channel enter or on request)
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	// Generate random map pool based on channel
	// Each channel gets a shuffled selection of maps
	// Default map pool (map UIDs that support random selection)
	static const uint8_t s_MapPool[] = {
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10,	// Standard maps
		11, 12, 13, 14, 15, 16, 17, 18		// Extended maps
	};
	static const int MAP_POOL_SIZE = sizeof(s_MapPool) / sizeof(s_MapPool[0]);

	// Random map count for this channel (Phase 14D: config-driven)
	int randomMapCount = 6;
	if (g_pContextMain && g_pContextMain->m_i32RandomMapCount > 0)
		randomMapCount = g_pContextMain->m_i32RandomMapCount;
	if (randomMapCount > MAP_POOL_SIZE)
		randomMapCount = MAP_POOL_SIZE;

	uint8_t count8 = (uint8_t)randomMapCount;

	// Seed RNG with channel + date for daily rotation
	time_t now = time(nullptr);
	struct tm tmNow;
	localtime_s(&tmNow, &now);
	uint32_t seed = (uint32_t)(tmNow.tm_yday * 100 + m_i32ChannelNum);
	srand(seed);

	// Fisher-Yates shuffle of map pool and pick first N
	uint8_t shuffled[32];
	memcpy(shuffled, s_MapPool, MAP_POOL_SIZE);
	for (int i = MAP_POOL_SIZE - 1; i > 0; i--)
	{
		int j = rand() % (i + 1);
		uint8_t tmp = shuffled[i];
		shuffled[i] = shuffled[j];
		shuffled[j] = tmp;
	}

	i3NetworkPacket packet(PROTOCOL_BASE_MAP_RANDOM_LIST_ACK);
	packet.WriteData(&count8, 1);
	for (int i = 0; i < randomMapCount; i++)
	{
		packet.WriteData(&shuffled[i], 1);
	}
	SendPacketMessage(&packet);
}

// ============================================================================
// Lobby Extended Handlers (Batch 15)
// ============================================================================

void GameSession::OnLobbyGetRoomInfoReq(char* pData, INT32 i32Size)
{
	// Get detailed info of a specific room (from lobby)
	if (m_eMainTask < GAME_TASK_LOBBY)
		return;

	if (i32Size < 4)
	{
		SendSimpleAck(PROTOCOL_LOBBY_GET_ROOMINFO_ACK, -1);
		return;
	}

	int32_t roomIdx = *(int32_t*)pData;
	Room* pRoom = g_pRoomManager->GetRoom(m_i32ChannelNum, roomIdx);
	if (!pRoom || !pRoom->IsCreated())
	{
		SendSimpleAck(PROTOCOL_LOBBY_GET_ROOMINFO_ACK, -1);
		return;
	}

	int32_t result = 0;

	// Room info
	int32_t rIdx = roomIdx;
	uint8_t mode = pRoom->GetGameMode();
	uint8_t mapIdx = pRoom->GetMapIndex();
	uint8_t state = pRoom->GetRoomState();
	uint8_t maxPlayers = pRoom->GetMaxPlayers();
	uint8_t playerCount = (uint8_t)pRoom->GetPlayerCount();
	uint8_t hasPassword = pRoom->HasPassword() ? 1 : 0;

	// Room title (32 bytes, null-padded)
	const char* title = pRoom->GetTitle();
	char title32[32] = {0};
	int titleLen = (int)strlen(title);
	if (titleLen > 32) titleLen = 32;
	memcpy(title32, title, titleLen);

	i3NetworkPacket packet(PROTOCOL_LOBBY_GET_ROOMINFO_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&rIdx, 4);
	packet.WriteData(&mode, 1);
	packet.WriteData(&mapIdx, 1);
	packet.WriteData(&state, 1);
	packet.WriteData(&maxPlayers, 1);
	packet.WriteData(&playerCount, 1);
	packet.WriteData(&hasPassword, 1);
	packet.WriteData(title32, 32);
	// Slot states
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		const GameSlotInfo& si = pRoom->GetSlotInfo(i);
		packet.WriteData(&si.ui8State, 1);
		packet.WriteData(&si.ui8Team, 1);
	}
	SendPacketMessage(&packet);
}

void GameSession::OnLobbyGetRoomInfoAddReq(char* pData, INT32 i32Size)
{
	// Get additional room info (score, time, round)
	if (m_eMainTask < GAME_TASK_LOBBY)
		return;

	if (i32Size < 4)
	{
		SendSimpleAck(PROTOCOL_LOBBY_GET_ROOMINFOADD_ACK, -1);
		return;
	}

	int32_t roomIdx = *(int32_t*)pData;
	Room* pRoom = g_pRoomManager->GetRoom(m_i32ChannelNum, roomIdx);
	if (!pRoom || !pRoom->IsCreated())
	{
		SendSimpleAck(PROTOCOL_LOBBY_GET_ROOMINFOADD_ACK, -1);
		return;
	}

	int32_t result = 0;

	int32_t rIdx = roomIdx;

	// Battle info from score struct
	const GameRoomScore& score = pRoom->GetScore();
	uint8_t roundCount = (uint8_t)score.i32MaxRound;
	uint8_t currentRound = (uint8_t)score.i32NowRound;
	uint8_t timeLimit = (uint8_t)(score.ui16MaxTime / 60);	// Convert seconds to minutes
	int32_t redScore = score.i32RedScore;
	int32_t blueScore = score.i32BlueScore;
	DWORD elapsed = pRoom->GetBattleElapsedTime();
	uint32_t elapsedSec = (uint32_t)(elapsed / 1000);
	i3NetworkPacket packet(PROTOCOL_LOBBY_GET_ROOMINFOADD_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&rIdx, 4);
	packet.WriteData(&roundCount, 1);
	packet.WriteData(&currentRound, 1);
	packet.WriteData(&timeLimit, 1);
	packet.WriteData(&redScore, 4);
	packet.WriteData(&blueScore, 4);
	packet.WriteData(&elapsedSec, 4);
	SendPacketMessage(&packet);
}

void GameSession::OnLobbyCreateTrainingReq(char* pData, INT32 i32Size)
{
	// Create a training/shooting range room
	if (m_eMainTask != GAME_TASK_LOBBY)
	{
		SendSimpleAck(PROTOCOL_LOBBY_CREATE_TRAINING_ACK, -1);
		return;
	}

	// Create a room with training mode settings
	GameRoomCreateInfo createInfo;
	memset(&createInfo, 0, sizeof(createInfo));
	strncpy_s(createInfo.szTitle, sizeof(createInfo.szTitle), "Training Room", _TRUNCATE);
	createInfo.ui8GameMode = STAGE_MODE_TUTORIAL;
	createInfo.ui8MapIndex = 0;		// Default training map
	createInfo.ui8MaxPlayers = 1;
	createInfo.ui8RoundType = BATTLE_ROUND_TYPE_1;
	createInfo.ui8SubType = BATTLE_TIME_TYPE_3;

	int roomIdx = g_pRoomManager->OnCreateRoom(this, &createInfo);
	if (roomIdx < 0)
	{
		SendSimpleAck(PROTOCOL_LOBBY_CREATE_TRAINING_ACK, -2);
		return;
	}

	// Send ACK with room info
	int32_t result = 0;

	int32_t rIdx = roomIdx;
	i3NetworkPacket packet(PROTOCOL_LOBBY_CREATE_TRAINING_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&rIdx, 4);
	SendPacketMessage(&packet);
}

void GameSession::OnLobbyQuickJoinSettingReq(char* pData, INT32 i32Size)
{
	// Save quick join filter settings (no ACK defined - fire and forget)
	if (m_eMainTask < GAME_TASK_LOBBY)
		return;

	// Settings are stored client-side; server just acknowledges receipt
	// No response needed as protocol has no ACK counterpart
}

void GameSession::OnLobbySetBirthdayReq(char* pData, INT32 i32Size)
{
	// Set birthday (one-time setting)
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 4)
	{
		SendSimpleAck(PROTOCOL_LOBBY_SET_BIRTHDAY_ACK, -1);
		return;
	}

	// Parse: year(2) + month(1) + day(1)
	// Just acknowledge - birthday stored server-side for events
	SendSimpleAck(PROTOCOL_LOBBY_SET_BIRTHDAY_ACK, 0);
}

void GameSession::OnLobbyGetUIDByNickNameReq(char* pData, INT32 i32Size)
{
	// Get UID by nickname
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 2)
	{
		SendSimpleAck(PROTOCOL_LOBBY_GET_UID_BY_NICK_NAME_ACK, -1);
		return;
	}

	// Parse nickname (null-terminated, up to 32 bytes)
	char nickname[33] = {};
	int copyLen = (i32Size > 32) ? 32 : i32Size;
	memcpy(nickname, pData, copyLen);
	nickname[32] = '\0';

	int32_t result = -1;
	int64_t foundUID = 0;

	// Search online sessions
	GameSession* pFound = g_pGameSessionManager->FindSessionByNickname(nickname);
	if (pFound)
	{
		result = 0;
		foundUID = pFound->GetUID();
	}


	// Echo nickname back
	i3NetworkPacket packet(PROTOCOL_LOBBY_GET_UID_BY_NICK_NAME_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&foundUID, sizeof(int64_t));
	packet.WriteData(nickname, 32);
	SendPacketMessage(&packet);
}

// ============================================================================
// Batch 18 - Lobby extras
// ============================================================================

void GameSession::OnLobbyNewViewUserItemReq(char* pData, INT32 i32Size)
{
	// View another user's items in lobby
	if (i32Size < (int)sizeof(int64_t))
	{
		SendSimpleAck(PROTOCOL_LOBBY_NEW_VIEW_USER_ITEM_ACK, -1);
		return;
	}

	int64_t targetUID = 0;
	memcpy(&targetUID, pData, sizeof(int64_t));

	GameSession* pTarget = g_pGameSessionManager ? g_pGameSessionManager->FindSessionByUID(targetUID) : nullptr;
	if (!pTarget)
	{
		SendSimpleAck(PROTOCOL_LOBBY_NEW_VIEW_USER_ITEM_ACK, -2);
		return;
	}

	// Send target's equipped items
	int32_t result = 0;

	// Equipment item count
	uint16_t equipCount = 0;
	for (int i = 0; i < pTarget->GetInventoryCount() && equipCount < 50; i++)
	{
		if (pTarget->m_Inventory[i].ui8IsEquipped)
			equipCount++;
	}

	i3NetworkPacket packet(PROTOCOL_LOBBY_NEW_VIEW_USER_ITEM_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&targetUID, sizeof(int64_t));
	packet.WriteData(&equipCount, sizeof(uint16_t));
	// Write equipped items
	for (int i = 0; i < pTarget->GetInventoryCount(); i++)
	{
		if (pTarget->m_Inventory[i].ui8IsEquipped)
		{
			packet.WriteData(&pTarget->m_Inventory[i].ui32ItemId, 4);
			packet.WriteData(&pTarget->m_Inventory[i].ui32Duration, 4);
		}
	}
	SendPacketMessage(&packet);
}

// ============================================================================
// Batch 19 - Lobby GM/misc handlers
// ============================================================================

void GameSession::OnLobbyGmGetUidReq(char* pData, INT32 i32Size)
{
	// GM queries a user's UID by nickname (from lobby context)
	if (!IsGMUser())
	{
		SendSimpleAck(PROTOCOL_LOBBY_GM_GET_UID_ACK, -1);
		return;
	}

	if (i32Size < 2)
	{
		SendSimpleAck(PROTOCOL_LOBBY_GM_GET_UID_ACK, 1);
		return;
	}

	char nickname[64] = {};
	int copyLen = (i32Size < 63) ? i32Size : 63;
	memcpy(nickname, pData, copyLen);

	GameSession* pTarget = g_pGameSessionManager ? g_pGameSessionManager->FindSessionByNickname(nickname) : nullptr;
	if (!pTarget)
	{
		SendSimpleAck(PROTOCOL_LOBBY_GM_GET_UID_ACK, 2);
		return;
	}

	int32_t result = 0;

	int64_t targetUID = pTarget->GetUID();
	i3NetworkPacket packet(PROTOCOL_LOBBY_GM_GET_UID_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&targetUID, sizeof(int64_t));
	SendPacketMessage(&packet);
}

void GameSession::OnLobbyAbusingPopupEndReq(char* pData, INT32 i32Size)
{
	// Abusing-flagged user waited out the penalty popup timer
	// Now grant them the previously withheld EXP/GP rewards
	int32_t result = 0;

	// Grant held rewards if this player was flagged for abusing
	int32_t grantedGP  = 0;
	int32_t grantedExp = 0;

	if (m_bAbusing && (m_i64HeldExp > 0 || m_i32HeldGP > 0))
	{
		grantedGP  = m_i32HeldGP;
		grantedExp = (int32_t)(m_i64HeldExp > INT32_MAX ? INT32_MAX : m_i64HeldExp);

		m_i32GP    += m_i32HeldGP;
		m_i64Exp   += m_i64HeldExp;

		m_bAbusing    = false;
		m_i32HeldGP   = 0;
		m_i64HeldExp  = 0;

		// Check for rank up after receiving EXP
		CheckRankUp();

		printf("[GameSession] Abusing popup confirmed - UID=%lld, granted GP=%d EXP=%d\n",
			m_i64UID, grantedGP, grantedExp);
	}
	i3NetworkPacket packet(PROTOCOL_LOBBY_ABUSING_POPUP_END_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&grantedExp, sizeof(int32_t));
	packet.WriteData(&grantedGP, sizeof(int32_t));
	SendPacketMessage(&packet);
}
