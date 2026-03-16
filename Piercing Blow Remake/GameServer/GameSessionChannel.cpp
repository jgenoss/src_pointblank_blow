#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "GameContextMain.h"
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
	char buffer[256];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_GET_CHANNELLIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));			offset += sizeof(uint16_t);

	uint8_t channelCount = g_pContextMain->m_ui8ChannelCount;
	memcpy(buffer + offset, &channelCount, sizeof(uint8_t));	offset += sizeof(uint8_t);

	for (uint8_t i = 0; i < channelCount; i++)
	{
		uint16_t userCount = 0;	// TODO: Get from GameSessionManager
		memcpy(buffer + offset, &userCount, sizeof(uint16_t));	offset += sizeof(uint16_t);
	}

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnChannelEnterReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < (int)sizeof(uint32_t))
		return;

	uint32_t channel = *(uint32_t*)pData;

	// TODO: Validate channel type restrictions (beginner, expert, clan, etc.)
	m_i32ChannelNum = (int)channel;
	m_eMainTask = GAME_TASK_CHANNEL;

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_SELECT_CHANNEL_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &channel, sizeof(uint32_t));	offset += sizeof(uint32_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);

	printf("[GameSession] Channel enter - Index=%d, Channel=%d\n", GetIndex(), channel);
}

void GameSession::OnChannelLeaveReq(char* pData, INT32 i32Size)
{
	if (m_i32ChannelNum < 0)
		return;

	int oldChannel = m_i32ChannelNum;
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

	// TODO: Notify GameSessionManager::OnEnterLobby
	m_eMainTask = GAME_TASK_LOBBY;
	SendSimpleAck(PROTOCOL_LOBBY_ENTER_ACK, 0);
}

void GameSession::OnLobbyLeaveReq(char* pData, INT32 i32Size)
{
	// TODO: Notify GameSessionManager::OnLeaveLobby
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
	// TODO: Forward to GameSessionManager::OnSendLobbyChatting for broadcast
	// For now, broadcast via PROTOCOL_LOBBY_CHATTING_ACK is not implemented
}

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
