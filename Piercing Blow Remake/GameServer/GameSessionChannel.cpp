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
