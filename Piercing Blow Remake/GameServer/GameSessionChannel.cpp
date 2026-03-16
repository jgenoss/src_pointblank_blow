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

	i3NetworkPacket packet;
	char buffer[512];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_GET_CHANNELLIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));			offset += sizeof(uint16_t);

	uint8_t channelCount = g_pContextMain->m_ui8ChannelCount;
	memcpy(buffer + offset, &channelCount, sizeof(uint8_t));	offset += sizeof(uint8_t);

	// Per-channel: type(1) + userCount(2) + maxUsers(2)
	for (uint8_t i = 0; i < channelCount; i++)
	{
		const GameChannelInfo& ch = g_pContextMain->GetChannelInfo(i);
		uint8_t chType = (uint8_t)ch.eType;
		memcpy(buffer + offset, &chType, 1);					offset += 1;
		memcpy(buffer + offset, &ch.ui16CurrentUsers, 2);		offset += 2;
		memcpy(buffer + offset, &ch.ui16MaxUsers, 2);			offset += 2;
	}

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
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
	}

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_SELECT_CHANNEL_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &channel, sizeof(uint32_t));	offset += sizeof(uint32_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);

	if (result == 0)
		printf("[GameSession] Channel enter - Index=%d, Channel=%d, Type=%d\n", GetIndex(), channel, g_pContextMain->GetChannelInfo((int)channel).eType);
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
}

void GameSession::OnLobbyLeaveReq(char* pData, INT32 i32Size)
{
	if (g_pGameSessionManager && m_i32ChannelNum >= 0)
		g_pGameSessionManager->OnLeaveLobby(this, m_i32ChannelNum);

	m_eMainTask = GAME_TASK_CHANNEL;
	SendSimpleAck(PROTOCOL_LOBBY_LEAVE_ACK, 0);
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

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_MAP_VERSION_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	uint32_t mapVersion = g_pContextMain->m_ui32MapVersion;
	memcpy(buffer + offset, &mapVersion, sizeof(uint32_t));	offset += sizeof(uint32_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnMapListReq(char* pData, INT32 i32Size)
{
	if (!g_pContextMain)
		return;

	i3NetworkPacket packet;
	char buffer[4096];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_MAP_LIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t mapCount = g_pContextMain->m_i32MapCount;
	memcpy(buffer + offset, &mapCount, sizeof(int32_t));	offset += sizeof(int32_t);

	for (int i = 0; i < mapCount && offset < 4000; i++)
	{
		const auto& map = g_pContextMain->m_Maps[i];
		memcpy(buffer + offset, &map.ui32StageId, sizeof(uint32_t));	offset += sizeof(uint32_t);
		memcpy(buffer + offset, map.szName, 32);						offset += 32;
		memcpy(buffer + offset, &map.ui8MinPlayers, 1);				offset += 1;
		memcpy(buffer + offset, &map.ui8MaxPlayers, 1);				offset += 1;
		uint8_t active = map.bActive ? 1 : 0;
		memcpy(buffer + offset, &active, 1);							offset += 1;
	}

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnMapRuleListReq(char* pData, INT32 i32Size)
{
	if (!g_pContextMain)
		return;

	// Send which maps support which game modes
	i3NetworkPacket packet;
	char buffer[4096];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_MAP_RULELIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t mapCount = g_pContextMain->m_i32MapCount;
	memcpy(buffer + offset, &mapCount, sizeof(int32_t));	offset += sizeof(int32_t);

	for (int i = 0; i < mapCount && offset < 4000; i++)
	{
		const auto& map = g_pContextMain->m_Maps[i];
		memcpy(buffer + offset, &map.ui32StageId, sizeof(uint32_t));	offset += sizeof(uint32_t);
		memcpy(buffer + offset, &map.ui8SupportedModes, 1);			offset += 1;
		memcpy(buffer + offset, &map.ui8SupportedModes2, 1);			offset += 1;
	}

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnMapMatchingListReq(char* pData, INT32 i32Size)
{
	if (!g_pContextMain)
		return;

	// Send mode -> map matching table
	i3NetworkPacket packet;
	char buffer[4096];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_MAP_MATCHINGLIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	// For each game mode, list available map indices
	uint8_t modeCount = GameContextMain::MAX_STAGE_MODE_COUNT;
	memcpy(buffer + offset, &modeCount, 1);					offset += 1;

	for (int mode = 0; mode < modeCount && offset < 3900; mode++)
	{
		uint8_t modeId = (uint8_t)mode;
		memcpy(buffer + offset, &modeId, 1);				offset += 1;

		// Count maps for this mode
		int countPos = offset;
		uint8_t mapCountForMode = 0;
		offset += 1;	// Reserve for count

		for (int m = 0; m < g_pContextMain->m_i32MapCount && offset < 3950; m++)
		{
			if (g_pContextMain->IsMapValidForMode(m, mode))
			{
				uint32_t stageId = g_pContextMain->m_Maps[m].ui32StageId;
				memcpy(buffer + offset, &stageId, sizeof(uint32_t));	offset += sizeof(uint32_t);
				mapCountForMode++;
			}
		}
		memcpy(buffer + countPos, &mapCountForMode, 1);
	}

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

// ============================================================================
// Quick Join
// ============================================================================

void GameSession::OnQuickJoinRoomReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask != GAME_TASK_LOBBY || m_i32ChannelNum < 0 || !g_pRoomManager)
		return;

	int roomIdx = g_pRoomManager->OnQuickJoinRoom(this, m_i32ChannelNum);

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_LOBBY_QUICKJOIN_ROOM_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = (roomIdx >= 0) ? 0 : 1;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	if (roomIdx >= 0)
	{
		memcpy(buffer + offset, &roomIdx, sizeof(int));		offset += sizeof(int);

		Room* pRoom = g_pRoomManager->GetRoom(m_i32ChannelNum, roomIdx);
		m_i32RoomIdx = roomIdx;
		m_i32SlotIdx = 0;
		m_pRoom = pRoom;
		m_eMainTask = GAME_TASK_READY_ROOM;
	}

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
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

	// Build response
	i3NetworkPacket packet;
	char buffer[128];
	int wOff = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_QUICKJOIN_START_ACK;
	wOff += sizeof(uint16_t);
	memcpy(buffer + wOff, &proto, sizeof(uint16_t));	wOff += sizeof(uint16_t);

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

	memcpy(buffer + wOff, &result, sizeof(int32_t));	wOff += sizeof(int32_t);

	// Echo quickjoin info
	for (int i = 0; i < MAX_QUICK_JOIN_INFO_COUNT; i++)
	{
		memcpy(buffer + wOff, &stageIds[i], sizeof(uint32_t));	wOff += sizeof(uint32_t);
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

	memcpy(buffer + wOff, &chIdx, 1);		wOff += 1;
	memcpy(buffer + wOff, &rmIdx, 4);		wOff += 4;
	memcpy(buffer + wOff, &chType, 1);		wOff += 1;
	memcpy(buffer + wOff, &rmState, 1);		wOff += 1;

	size = (uint16_t)wOff;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, wOff);
	SendMessage(&packet);
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

	i3NetworkPacket packet;
	char buffer[32];
	int wOff = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_QUICKJOIN_ENTER_REC_ROOM_ACK;
	wOff += sizeof(uint16_t);
	memcpy(buffer + wOff, &proto, sizeof(uint16_t));	wOff += sizeof(uint16_t);

	int32_t result = (joinResult >= 0) ? 0 : 1;
	memcpy(buffer + wOff, &result, sizeof(int32_t));	wOff += sizeof(int32_t);

	if (joinResult >= 0)
	{
		Room* pRoom = g_pRoomManager->GetRoom((int)channelIdx, (int)roomIdx);
		m_i32RoomIdx = (int)roomIdx;
		m_pRoom = pRoom;
		m_eMainTask = GAME_TASK_READY_ROOM;

		memcpy(buffer + wOff, &channelIdx, 4);	wOff += 4;
		memcpy(buffer + wOff, &roomIdx, 4);		wOff += 4;
	}

	size = (uint16_t)wOff;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, wOff);
	SendMessage(&packet);
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

	i3NetworkPacket packet;
	char buffer[2048];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_LOBBY_VIEW_USER_ITEM_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	if (!pTarget)
	{
		int32_t result = 1;	// User not found
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	}
	else
	{
		int32_t result = 0;
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

		// Target player info
		char nick[64] = {0};
		strncpy_s(nick, pTarget->GetNickname(), _TRUNCATE);
		memcpy(buffer + offset, nick, 64);					offset += 64;

		int32_t level = pTarget->GetLevel();
		int32_t rankId = pTarget->GetRankId();
		int32_t kills = pTarget->GetKills();
		int32_t deaths = pTarget->GetDeaths();
		memcpy(buffer + offset, &level, 4);					offset += 4;
		memcpy(buffer + offset, &rankId, 4);				offset += 4;
		memcpy(buffer + offset, &kills, 4);					offset += 4;
		memcpy(buffer + offset, &deaths, 4);				offset += 4;

		// Equipment info
		const GameCharaEquip& equip = pTarget->GetActiveEquipment();
		memcpy(buffer + offset, &equip.ui32CharaId, 4);	offset += 4;
		for (int i = 0; i < WEAPON_SLOT_COUNT; i++)
		{
			memcpy(buffer + offset, &equip.ui32WeaponIds[i], 4);	offset += 4;
		}
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnLobbyFindNickNameReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_LOBBY)
		return;

	if (i32Size < 64)
	{
		SendSimpleAck(PROTOCOL_LOBBY_FIND_NICK_NAME_ACK, 1);
		return;
	}

	char searchNick[64] = {0};
	memcpy(searchNick, pData, 64);
	searchNick[63] = '\0';

	// Search for user online
	GameSession* pTarget = nullptr;
	if (g_pGameSessionManager)
		pTarget = g_pGameSessionManager->FindSessionByNickname(searchNick);

	i3NetworkPacket packet;
	char buffer[256];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_LOBBY_FIND_NICK_NAME_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	if (!pTarget)
	{
		int32_t result = 1;	// Not found
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	}
	else
	{
		int32_t result = 0;
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

		char nick[64] = {0};
		strncpy_s(nick, pTarget->GetNickname(), _TRUNCATE);
		memcpy(buffer + offset, nick, 64);					offset += 64;

		int32_t level = pTarget->GetLevel();
		int32_t rankId = pTarget->GetRankId();
		int32_t channelNum = pTarget->GetChannelNum();
		int32_t roomIdx = pTarget->GetRoomIdx();
		memcpy(buffer + offset, &level, 4);					offset += 4;
		memcpy(buffer + offset, &rankId, 4);				offset += 4;
		memcpy(buffer + offset, &channelNum, 4);			offset += 4;
		memcpy(buffer + offset, &roomIdx, 4);				offset += 4;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
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
	i3NetworkPacket packet;
	char buffer[512];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_MEGAPHONE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Sender nickname
	char nick[64] = {0};
	strncpy_s(nick, m_szNickname, _TRUNCATE);
	memcpy(buffer + offset, nick, 64);						offset += 64;

	// Message
	memcpy(buffer + offset, &msgLen, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, message, msgLen);				offset += msgLen;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);

	// Broadcast to all sessions in same channel
	if (g_pGameSessionManager)
		g_pGameSessionManager->BroadcastToChannel(m_i32ChannelNum, &packet);
}
