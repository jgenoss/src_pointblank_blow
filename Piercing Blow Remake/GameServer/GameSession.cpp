#include "pch.h"
#include "GameSession.h"
#include "GameContextMain.h"
#include "Room.h"
#include "RoomManager.h"

// Protocol IDs from ProtocolDef.h (S2MO)
// Base protocols
#define PROTOCOL_BASE_CONNECT_REQ		0x0100
#define PROTOCOL_BASE_CONNECT_ACK		0x0101
#define PROTOCOL_BASE_HEART_BIT_REQ		0x0104
#define PROTOCOL_BASE_HEART_BIT_ACK		0x0105

// Login protocols
#define PROTOCOL_LOGIN_REQ				0x0200
#define PROTOCOL_LOGIN_ACK				0x0201

// User info
#define PROTOCOL_BASE_GET_USERINFO_REQ	0x0300
#define PROTOCOL_BASE_GET_USERINFO_ACK	0x0301

// Channel protocols
#define PROTOCOL_LOBBY_GET_CHANNELLIST_REQ	0x0400
#define PROTOCOL_LOBBY_GET_CHANNELLIST_ACK	0x0401
#define PROTOCOL_LOBBY_ENTER_CHANNEL_REQ	0x0402
#define PROTOCOL_LOBBY_ENTER_CHANNEL_ACK	0x0403
#define PROTOCOL_LOBBY_LEAVE_CHANNEL_REQ	0x0404
#define PROTOCOL_LOBBY_LEAVE_CHANNEL_ACK	0x0405

// Lobby protocols
#define PROTOCOL_LOBBY_ENTER_REQ			0x0500
#define PROTOCOL_LOBBY_ENTER_ACK			0x0501
#define PROTOCOL_LOBBY_LEAVE_REQ			0x0502
#define PROTOCOL_LOBBY_LEAVE_ACK			0x0503
#define PROTOCOL_LOBBY_GET_ROOMLIST_REQ		0x0504
#define PROTOCOL_LOBBY_GET_ROOMLIST_ACK		0x0505
#define PROTOCOL_LOBBY_CHAT_REQ				0x0506
#define PROTOCOL_LOBBY_CHAT_ACK				0x0507

// Room protocols
#define PROTOCOL_ROOM_CREATE_REQ			0x0600
#define PROTOCOL_ROOM_CREATE_ACK			0x0601
#define PROTOCOL_ROOM_JOIN_REQ				0x0602
#define PROTOCOL_ROOM_JOIN_ACK				0x0603
#define PROTOCOL_ROOM_LEAVE_REQ				0x0604
#define PROTOCOL_ROOM_LEAVE_ACK				0x0605
#define PROTOCOL_ROOM_READY_REQ				0x0606
#define PROTOCOL_ROOM_READY_ACK				0x0607
#define PROTOCOL_ROOM_START_REQ				0x0608
#define PROTOCOL_ROOM_START_ACK				0x0609
#define PROTOCOL_ROOM_CHAT_REQ				0x060A
#define PROTOCOL_ROOM_CHAT_ACK				0x060B

#define SOCKET_HEAD_SIZE	4	// 2 bytes size + 2 bytes protocol

I3_CLASS_INSTANCE(GameSession);

GameSession::GameSession()
	: m_eMainTask(GAME_TASK_NONE)
	, m_i64UID(0)
	, m_ui32EncriptKey(0)
	, m_ui32AuthToken(0)
	, m_i32ChannelNum(-1)
	, m_i32RoomIdx(-1)
	, m_i32SlotIdx(-1)
	, m_pRoom(nullptr)
	, m_i32Level(1)
	, m_i64Exp(0)
	, m_i32Cash(0)
	, m_i32GP(0)
	, m_i32RankId(0)
	, m_i32ClanId(0)
	, m_i32Kills(0)
	, m_i32Deaths(0)
	, m_i32Headshots(0)
	, m_i32Wins(0)
	, m_i32Losses(0)
	, m_i32InventoryCount(0)
	, m_dwConnectTime(0)
	, m_dwLastPacketTime(0)
{
	m_szUsername[0] = '\0';
	m_szNickname[0] = '\0';
	memset(m_Inventory, 0, sizeof(m_Inventory));
}

GameSession::~GameSession()
{
}

BOOL GameSession::Create()
{
	ResetSessionData();
	return TRUE;
}

void GameSession::Destroy()
{
	ResetSessionData();
}

BOOL GameSession::OnConnect(SOCKET Socket, struct sockaddr_in* pAddr)
{
	m_eMainTask = GAME_TASK_CONNECT;
	m_dwConnectTime = GetTickCount();
	m_dwLastPacketTime = m_dwConnectTime;

	printf("[GameSession] Client connected - Index=%d\n", GetIndex());
	return TRUE;
}

BOOL GameSession::OnConnectInit()
{
	return TRUE;
}

BOOL GameSession::OnDisconnect(BOOL bForceMainThread)
{
	printf("[GameSession] Client disconnected - Index=%d, UID=%lld, Task=%d\n",
		GetIndex(), m_i64UID, m_eMainTask);

	// Leave room if in one
	if (m_pRoom && m_i32RoomIdx >= 0 && g_pRoomManager)
	{
		g_pRoomManager->OnLeaveRoom(this, m_i32ChannelNum);
	}

	// Leave channel handled by session manager

	ResetSessionData();
	return TRUE;
}

INT32 GameSession::PacketParsing(char* pPacket, INT32 iSize)
{
	if (iSize < SOCKET_HEAD_SIZE)
		return 0;

	// Packet header: 2 bytes size + 2 bytes protocol ID
	uint16_t packetSize = *(uint16_t*)pPacket;
	uint16_t protocolId = *(uint16_t*)(pPacket + 2);

	if (iSize < packetSize)
		return 0;	// Incomplete packet

	m_dwLastPacketTime = GetTickCount();

	char* pData = pPacket + SOCKET_HEAD_SIZE;
	INT32 dataSize = packetSize - SOCKET_HEAD_SIZE;

	switch (protocolId)
	{
	// Base protocols (any state)
	case PROTOCOL_BASE_CONNECT_REQ:
		OnBaseConnectReq(pData, dataSize);
		break;

	case PROTOCOL_BASE_HEART_BIT_REQ:
		OnBaseHeartBitReq(pData, dataSize);
		break;

	// Login
	case PROTOCOL_LOGIN_REQ:
		OnLoginReq(pData, dataSize);
		break;

	// User info
	case PROTOCOL_BASE_GET_USERINFO_REQ:
		OnGetUserInfoReq(pData, dataSize);
		break;

	// Channel
	case PROTOCOL_LOBBY_GET_CHANNELLIST_REQ:
		OnChannelListReq(pData, dataSize);
		break;

	case PROTOCOL_LOBBY_ENTER_CHANNEL_REQ:
		OnChannelEnterReq(pData, dataSize);
		break;

	case PROTOCOL_LOBBY_LEAVE_CHANNEL_REQ:
		OnChannelLeaveReq(pData, dataSize);
		break;

	// Lobby
	case PROTOCOL_LOBBY_ENTER_REQ:
		OnLobbyEnterReq(pData, dataSize);
		break;

	case PROTOCOL_LOBBY_LEAVE_REQ:
		OnLobbyLeaveReq(pData, dataSize);
		break;

	case PROTOCOL_LOBBY_GET_ROOMLIST_REQ:
		OnGetRoomListReq(pData, dataSize);
		break;

	case PROTOCOL_LOBBY_CHAT_REQ:
		OnLobbyChatReq(pData, dataSize);
		break;

	// Room
	case PROTOCOL_ROOM_CREATE_REQ:
		OnRoomCreateReq(pData, dataSize);
		break;

	case PROTOCOL_ROOM_JOIN_REQ:
		OnRoomJoinReq(pData, dataSize);
		break;

	case PROTOCOL_ROOM_LEAVE_REQ:
		OnRoomLeaveReq(pData, dataSize);
		break;

	case PROTOCOL_ROOM_READY_REQ:
		OnRoomReadyReq(pData, dataSize);
		break;

	case PROTOCOL_ROOM_START_REQ:
		OnRoomStartReq(pData, dataSize);
		break;

	case PROTOCOL_ROOM_CHAT_REQ:
		OnRoomChatReq(pData, dataSize);
		break;

	default:
		printf("[GameSession] Unknown protocol 0x%04X from Index=%d\n", protocolId, GetIndex());
		break;
	}

	return packetSize;
}

// ============================================================================
// Packet Handlers
// ============================================================================

void GameSession::OnBaseConnectReq(char* pData, INT32 i32Size)
{
	// RSA handshake - simplified: generate XOR key and send ACK
	m_ui32EncriptKey = (uint32_t)(GetTickCount() ^ (DWORD)GetIndex());
	m_eMainTask = GAME_TASK_CONNECT;

	SendConnectAck();
}

void GameSession::OnBaseHeartBitReq(char* pData, INT32 i32Size)
{
	SendHeartBitAck();
}

void GameSession::OnLoginReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask != GAME_TASK_CONNECT)
	{
		SendLoginAck(1);	// Invalid state
		return;
	}

	m_eMainTask = GAME_TASK_LOGIN_ING;

	// Extract auth token and username from packet
	// Packet format: auth_token(4) + username(64)
	if (i32Size < (int)(sizeof(uint32_t) + 64))
	{
		SendLoginAck(2);	// Invalid packet
		return;
	}

	uint32_t authToken = *(uint32_t*)pData;
	memcpy(m_szUsername, pData + sizeof(uint32_t), 64);
	m_szUsername[63] = '\0';

	// TODO: Validate auth token via ModuleConnectServer
	// For now, accept all connections with valid packet format
	m_ui32AuthToken = authToken;

	m_eMainTask = GAME_TASK_LOGIN;

	// TODO: Request player data from DataServer via ModuleDataServer
	// For now, set default data
	m_i64UID = (int64_t)(GetIndex() + 1);
	strncpy_s(m_szNickname, m_szUsername, _TRUNCATE);
	m_i32Level = 1;
	m_i64Exp = 0;
	m_i32GP = 10000;

	m_eMainTask = GAME_TASK_INFO;

	SendLoginAck(0);	// Success

	printf("[GameSession] Login success - Index=%d, User=%s, UID=%lld\n",
		GetIndex(), m_szUsername, m_i64UID);
}

void GameSession::OnGetUserInfoReq(char* pData, INT32 i32Size)
{
	// Send user info ACK
	i3NetworkPacket packet;
	char buffer[512];
	int offset = 0;

	uint16_t size = 0;	// Will fill later
	uint16_t proto = PROTOCOL_BASE_GET_USERINFO_ACK;
	offset += sizeof(uint16_t);	// Reserve for size
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	// User data
	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, m_szNickname, 64);				offset += 64;
	memcpy(buffer + offset, &m_i32Level, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_i64Exp, sizeof(int64_t));	offset += sizeof(int64_t);
	memcpy(buffer + offset, &m_i32Cash, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_i32GP, sizeof(int));			offset += sizeof(int);
	memcpy(buffer + offset, &m_i32RankId, sizeof(int));		offset += sizeof(int);

	// Fill size
	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);

	m_eMainTask = GAME_TASK_CHANNEL;
}

void GameSession::OnChannelListReq(char* pData, INT32 i32Size)
{
	if (!g_pContextMain)
		return;

	i3NetworkPacket packet;
	char buffer[256];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_LOBBY_GET_CHANNELLIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	uint8_t channelCount = g_pContextMain->m_ui8ChannelCount;
	memcpy(buffer + offset, &channelCount, sizeof(uint8_t));	offset += sizeof(uint8_t);

	// Per-channel: user count (placeholder)
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

	// TODO: Validate channel via GameSessionManager::OnEnterChannel
	m_i32ChannelNum = (int)channel;
	m_eMainTask = GAME_TASK_CHANNEL;

	// Send ACK
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_LOBBY_ENTER_CHANNEL_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;	// Success
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

	// TODO: Notify GameSessionManager::OnLeaveChannel
	int oldChannel = m_i32ChannelNum;
	m_i32ChannelNum = -1;
	m_eMainTask = GAME_TASK_CHANNEL;

	// Send ACK
	i3NetworkPacket packet;
	char buffer[16];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_LOBBY_LEAVE_CHANNEL_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);

	printf("[GameSession] Channel leave - Index=%d, OldChannel=%d\n", GetIndex(), oldChannel);
}

void GameSession::OnLobbyEnterReq(char* pData, INT32 i32Size)
{
	if (m_i32ChannelNum < 0)
		return;

	// TODO: Notify GameSessionManager::OnEnterLobby
	m_eMainTask = GAME_TASK_LOBBY;

	// Send ACK
	i3NetworkPacket packet;
	char buffer[16];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_LOBBY_ENTER_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnLobbyLeaveReq(char* pData, INT32 i32Size)
{
	// TODO: Notify GameSessionManager::OnLeaveLobby
	m_eMainTask = GAME_TASK_CHANNEL;

	i3NetworkPacket packet;
	char buffer[16];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_LOBBY_LEAVE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnGetRoomListReq(char* pData, INT32 i32Size)
{
	if (m_i32ChannelNum < 0 || !g_pRoomManager)
		return;

	g_pRoomManager->OnSendRoomList(this, m_i32ChannelNum);
}

void GameSession::OnLobbyChatReq(char* pData, INT32 i32Size)
{
	// TODO: Forward to GameSessionManager::OnSendLobbyChatting
	// For now, echo back
}

void GameSession::OnRoomCreateReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask != GAME_TASK_LOBBY || m_i32ChannelNum < 0 || !g_pRoomManager)
		return;

	// Parse room creation info
	GameRoomCreateInfo info;

	if (i32Size >= 64)
		memcpy(info.szTitle, pData, 64);
	if (i32Size >= 64 + (int)sizeof(uint32_t))
		memcpy(&info.ui32StageId, pData + 64, sizeof(uint32_t));
	if (i32Size >= 64 + (int)sizeof(uint32_t) + 1)
		info.ui8MaxPlayers = *(uint8_t*)(pData + 64 + sizeof(uint32_t));

	info.szTitle[63] = '\0';

	int roomIdx = g_pRoomManager->OnCreateRoom(this, &info);

	// Send ACK
	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_ROOM_CREATE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = (roomIdx >= 0) ? 0 : 1;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &roomIdx, sizeof(int));			offset += sizeof(int);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);

	if (roomIdx >= 0)
	{
		Room* pRoom = g_pRoomManager->GetRoom(m_i32ChannelNum, roomIdx);
		m_i32RoomIdx = roomIdx;
		m_i32SlotIdx = 0;
		m_pRoom = pRoom;
		m_eMainTask = GAME_TASK_READY_ROOM;

		printf("[GameSession] Room created - Index=%d, Room=%d, Channel=%d\n",
			GetIndex(), roomIdx, m_i32ChannelNum);
	}
}

void GameSession::OnRoomJoinReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask != GAME_TASK_LOBBY || m_i32ChannelNum < 0 || !g_pRoomManager)
		return;

	if (i32Size < (int)sizeof(int))
		return;

	int roomIdx = *(int*)pData;
	int slot = g_pRoomManager->OnJoinRoom(this, m_i32ChannelNum, roomIdx, TEAM_RED);

	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_ROOM_JOIN_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = (slot >= 0) ? 0 : 1;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	uint8_t slotByte = (slot >= 0) ? (uint8_t)slot : 0xFF;
	memcpy(buffer + offset, &slotByte, sizeof(uint8_t));	offset += sizeof(uint8_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);

	if (slot >= 0)
	{
		Room* pRoom = g_pRoomManager->GetRoom(m_i32ChannelNum, roomIdx);
		m_i32RoomIdx = roomIdx;
		m_i32SlotIdx = slot;
		m_pRoom = pRoom;
		m_eMainTask = GAME_TASK_READY_ROOM;
	}
}

void GameSession::OnRoomLeaveReq(char* pData, INT32 i32Size)
{
	if (m_i32RoomIdx < 0 || m_i32ChannelNum < 0 || !g_pRoomManager)
		return;

	g_pRoomManager->OnLeaveRoom(this, m_i32ChannelNum);

	m_i32RoomIdx = -1;
	m_i32SlotIdx = -1;
	m_pRoom = nullptr;
	m_eMainTask = GAME_TASK_LOBBY;

	i3NetworkPacket packet;
	char buffer[16];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_ROOM_LEAVE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnRoomReadyReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom)
		return;

	bool bReady = (i32Size > 0) ? (pData[0] != 0) : true;
	m_pRoom->OnReady(this, bReady);

	// Broadcast ready state to all in room
	i3NetworkPacket packet;
	char buffer[16];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_ROOM_READY_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	uint8_t slotIdx = (uint8_t)m_i32SlotIdx;
	uint8_t readyState = bReady ? SLOT_STATE_READY : SLOT_STATE_NORMAL;
	memcpy(buffer + offset, &slotIdx, sizeof(uint8_t));		offset += sizeof(uint8_t);
	memcpy(buffer + offset, &readyState, sizeof(uint8_t));	offset += sizeof(uint8_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	m_pRoom->SendToAll(&packet);
}

void GameSession::OnRoomStartReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom)
		return;

	// Only the owner can start
	if (m_i32SlotIdx != m_pRoom->GetOwnerSlot())
		return;

	bool started = m_pRoom->OnStartBattle();

	i3NetworkPacket packet;
	char buffer[16];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_ROOM_START_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = started ? 0 : 1;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);

	if (started)
	{
		m_pRoom->SendToAll(&packet);
		m_eMainTask = GAME_TASK_BATTLE;
	}
	else
	{
		SendMessage(&packet);
	}
}

void GameSession::OnRoomChatReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || i32Size <= 0)
		return;

	// Build chat ACK and broadcast to room
	i3NetworkPacket packet;
	char buffer[512];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_ROOM_CHAT_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	uint8_t slotIdx = (uint8_t)m_i32SlotIdx;
	memcpy(buffer + offset, &slotIdx, sizeof(uint8_t));		offset += sizeof(uint8_t);

	uint16_t msgSize = (uint16_t)min(i32Size, 256);
	memcpy(buffer + offset, &msgSize, sizeof(uint16_t));	offset += sizeof(uint16_t);
	memcpy(buffer + offset, pData, msgSize);				offset += msgSize;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	m_pRoom->SendToAll(&packet);
}

// ============================================================================
// Helper methods
// ============================================================================

void GameSession::SendConnectAck()
{
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_CONNECT_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));			offset += sizeof(uint16_t);
	memcpy(buffer + offset, &m_ui32EncriptKey, sizeof(uint32_t));	offset += sizeof(uint32_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::SendHeartBitAck()
{
	i3NetworkPacket packet;
	char buffer[8];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_HEART_BIT_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::SendLoginAck(int i32Result)
{
	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_LOGIN_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, &i32Result, sizeof(int32_t));	offset += sizeof(int32_t);

	if (i32Result == 0)
	{
		// Success: include user info
		memcpy(buffer + offset, &m_i64UID, sizeof(int64_t));	offset += sizeof(int64_t);
		memcpy(buffer + offset, m_szNickname, 64);				offset += 64;
	}

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnPlayerDataLoaded(const char* pPayload, int i32PayloadSize)
{
	if (!pPayload || i32PayloadSize < (int)sizeof(IS_PLAYER_LOAD_DATA))
		return;

	const IS_PLAYER_LOAD_DATA* pData = (const IS_PLAYER_LOAD_DATA*)pPayload;

	m_i64UID = pData->i64UID;
	strncpy_s(m_szNickname, pData->szNickname, _TRUNCATE);
	m_i32Level = pData->i32Level;
	m_i64Exp = pData->i64Exp;
	m_i32Cash = pData->i32Cash;
	m_i32GP = pData->i32GP;
	m_i32RankId = pData->i32RankId;
	m_i32ClanId = pData->i32ClanId;

	m_i32Kills = pData->i32Kills;
	m_i32Deaths = pData->i32Deaths;
	m_i32Headshots = pData->i32Headshots;
	m_i32Wins = pData->i32Wins;
	m_i32Losses = pData->i32Losses;

	// Load inventory
	m_i32InventoryCount = pData->i32InventoryCount;
	if (m_i32InventoryCount > MAX_GAME_INVENTORY)
		m_i32InventoryCount = MAX_GAME_INVENTORY;

	const IS_PLAYER_INVENTORY_ITEM* pItems =
		(const IS_PLAYER_INVENTORY_ITEM*)(pPayload + sizeof(IS_PLAYER_LOAD_DATA));

	for (int i = 0; i < m_i32InventoryCount; i++)
	{
		m_Inventory[i].i32ItemId = pItems[i].i32ItemId;
		m_Inventory[i].i32ItemCount = pItems[i].i32ItemCount;
		m_Inventory[i].i32SlotIdx = pItems[i].i32SlotIdx;
		m_Inventory[i].ui8IsEquipped = pItems[i].ui8IsEquipped;
	}

	m_eMainTask = GAME_TASK_INFO;

	printf("[GameSession] Player data loaded - UID=%lld, Nick=%s, Level=%d\n",
		m_i64UID, m_szNickname, m_i32Level);
}

bool GameSession::IsTimedOut() const
{
	DWORD dwNow = GetTickCount();
	DWORD dwElapsed = (dwNow - m_dwLastPacketTime) / 1000;	// Seconds

	switch (m_eMainTask)
	{
	case GAME_TASK_NONE:
	case GAME_TASK_CONNECT:
		return (dwElapsed > SESSION_CHECK_TIMEOUT_NOT_LOGIN);

	case GAME_TASK_LOGIN_ING:
	case GAME_TASK_LOGIN:
		return (dwElapsed > SESSION_CHECK_TIMEOUT_LOGIN);

	case GAME_TASK_CHANNEL:
		return (dwElapsed > SESSION_CHECK_TIMEOUT_CHANNEL);

	default:
		return (dwElapsed > SESSION_CHECK_TIMEOUT_NORMAL);
	}
}

void GameSession::ResetSessionData()
{
	m_eMainTask = GAME_TASK_NONE;
	m_i64UID = 0;
	m_szUsername[0] = '\0';
	m_szNickname[0] = '\0';
	m_ui32EncriptKey = 0;
	m_ui32AuthToken = 0;

	m_i32ChannelNum = -1;
	m_i32RoomIdx = -1;
	m_i32SlotIdx = -1;
	m_pRoom = nullptr;

	m_i32Level = 1;
	m_i64Exp = 0;
	m_i32Cash = 0;
	m_i32GP = 0;
	m_i32RankId = 0;
	m_i32ClanId = 0;

	m_i32Kills = 0;
	m_i32Deaths = 0;
	m_i32Headshots = 0;
	m_i32Wins = 0;
	m_i32Losses = 0;

	m_i32InventoryCount = 0;
	memset(m_Inventory, 0, sizeof(m_Inventory));

	m_dwConnectTime = 0;
	m_dwLastPacketTime = 0;
}
