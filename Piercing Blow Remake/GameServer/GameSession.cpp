#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "GameContextMain.h"
#include "Room.h"
#include "RoomManager.h"

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

	if (m_pRoom && m_i32RoomIdx >= 0 && g_pRoomManager)
		g_pRoomManager->OnLeaveRoom(this, m_i32ChannelNum);

	ResetSessionData();
	return TRUE;
}

// ============================================================================
// Packet Router - dispatches to handlers in GameSessionChannel/Room/Battle.cpp
// ============================================================================

INT32 GameSession::PacketParsing(char* pPacket, INT32 iSize)
{
	if (iSize < SOCKET_HEAD_SIZE)
		return 0;

	uint16_t packetSize = *(uint16_t*)pPacket;
	uint16_t protocolId = *(uint16_t*)(pPacket + 2);

	if (iSize < packetSize)
		return 0;

	m_dwLastPacketTime = GetTickCount();

	char* pData = pPacket + SOCKET_HEAD_SIZE;
	INT32 dataSize = packetSize - SOCKET_HEAD_SIZE;

	switch (protocolId)
	{
	// ---- Base protocols ----
	case PROTOCOL_BASE_CONNECT_REQ:		OnBaseConnectReq(pData, dataSize);		break;
	case PROTOCOL_BASE_HEART_BIT_REQ:	OnBaseHeartBitReq(pData, dataSize);		break;
	case PROTOCOL_LOGIN_REQ:			OnLoginReq(pData, dataSize);			break;
	case PROTOCOL_BASE_GET_USER_INFO_REQ:	OnGetUserInfoReq(pData, dataSize);	break;

	// ---- Channel (GameSessionChannel.cpp) ----
	case PROTOCOL_BASE_GET_CHANNELLIST_REQ:	OnChannelListReq(pData, dataSize);	break;
	case PROTOCOL_BASE_SELECT_CHANNEL_REQ:	OnChannelEnterReq(pData, dataSize);	break;

	// ---- Lobby (GameSessionChannel.cpp) ----
	case PROTOCOL_LOBBY_ENTER_REQ:			OnLobbyEnterReq(pData, dataSize);		break;
	case PROTOCOL_LOBBY_LEAVE_REQ:			OnLobbyLeaveReq(pData, dataSize);		break;
	case PROTOCOL_LOBBY_GET_ROOMLIST_REQ:	OnGetRoomListReq(pData, dataSize);		break;
	case PROTOCOL_LOBBY_QUICKJOIN_ROOM_REQ:	OnQuickJoinRoomReq(pData, dataSize);	break;
	case PROTOCOL_BASE_CHATTING_REQ:
		if (m_eMainTask == GAME_TASK_READY_ROOM || m_eMainTask == GAME_TASK_BATTLE)
			OnRoomChatReq(pData, dataSize);
		else
			OnLobbyChatReq(pData, dataSize);
		break;

	// ---- Room (GameSessionRoom.cpp) ----
	case PROTOCOL_ROOM_CREATE_REQ:				OnRoomCreateReq(pData, dataSize);			break;
	case PROTOCOL_ROOM_JOIN_REQ:				OnRoomJoinReq(pData, dataSize);				break;
	case PROTOCOL_ROOM_LEAVE_ROOM_REQ:			OnRoomLeaveReq(pData, dataSize);			break;
	case PROTOCOL_ROOM_GET_SLOTINFO_REQ:
	case PROTOCOL_ROOM_GET_SLOTONEINFO_REQ:		OnRoomGetSlotInfoReq(pData, dataSize);		break;
	case PROTOCOL_ROOM_GET_PLAYERINFO_REQ:		OnRoomGetPlayerInfoReq(pData, dataSize);		break;
	case PROTOCOL_ROOM_CHANGE_ROOMINFO_REQ:		OnRoomChangeRoomInfoReq(pData, dataSize);	break;
	case PROTOCOL_ROOM_CHANGE_PASSWD_REQ:		OnRoomChangePasswdReq(pData, dataSize);		break;
	case PROTOCOL_ROOM_CHANGE_SLOT_REQ:			OnRoomChangeSlotReq(pData, dataSize);		break;
	case PROTOCOL_ROOM_PERSONAL_TEAM_CHANGE_REQ:	OnRoomTeamChangeReq(pData, dataSize);	break;
	case PROTOCOL_ROOM_REQUEST_MAIN_REQ:		OnRoomRequestMainReq(pData, dataSize);		break;
	case PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_REQ:	OnRoomRequestMainChangeReq(pData, dataSize);break;
	case PROTOCOL_ROOM_CHANGE_ROOM_OPTIONINFO_REQ:	OnRoomChangeOptionInfoReq(pData, dataSize);break;

	// ---- Battle (GameSessionBattle.cpp) ----
	case PROTOCOL_BATTLE_READYBATTLE_REQ:				OnBattleReadyBattleReq(pData, dataSize);			break;
	case PROTOCOL_BATTLE_PRESTARTBATTLE_REQ:			OnBattlePreStartBattleReq(pData, dataSize);			break;
	case PROTOCOL_BATTLE_STARTBATTLE_REQ:				OnBattleStartBattleReq(pData, dataSize);			break;
	case PROTOCOL_BATTLE_GIVEUPBATTLE_REQ:				OnBattleGiveUpBattleReq(pData, dataSize);			break;
	case PROTOCOL_BATTLE_DEATH_REQ:						OnBattleDeathReq(pData, dataSize);					break;
	case PROTOCOL_BATTLE_RESPAWN_REQ:					OnBattleRespawnReq(pData, dataSize);				break;
	case PROTOCOL_BATTLE_MISSION_ROUND_PRE_START_REQ:	OnBattleMissionRoundPreStartReq(pData, dataSize);	break;
	case PROTOCOL_BATTLE_MISSION_ROUND_START_REQ:		OnBattleMissionRoundStartReq(pData, dataSize);		break;
	case PROTOCOL_BATTLE_MISSION_ROUND_END_REQ:			OnBattleMissionRoundEndReq(pData, dataSize);		break;

	default:
		printf("[GameSession] Unknown protocol 0x%04X from Index=%d\n", protocolId, GetIndex());
		break;
	}

	return packetSize;
}

// ============================================================================
// Base & Login Handlers
// ============================================================================

void GameSession::OnBaseConnectReq(char* pData, INT32 i32Size)
{
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
		SendLoginAck(1);
		return;
	}

	m_eMainTask = GAME_TASK_LOGIN_ING;

	if (i32Size < (int)(sizeof(uint32_t) + 64))
	{
		SendLoginAck(2);
		return;
	}

	uint32_t authToken = *(uint32_t*)pData;
	memcpy(m_szUsername, pData + sizeof(uint32_t), 64);
	m_szUsername[63] = '\0';

	// TODO: Validate auth token via ModuleConnectServer
	m_ui32AuthToken = authToken;
	m_eMainTask = GAME_TASK_LOGIN;

	// TODO: Request player data from DataServer
	m_i64UID = (int64_t)(GetIndex() + 1);
	strncpy_s(m_szNickname, m_szUsername, _TRUNCATE);
	m_i32Level = 1;
	m_i64Exp = 0;
	m_i32GP = 10000;

	m_eMainTask = GAME_TASK_INFO;
	SendLoginAck(0);

	printf("[GameSession] Login success - Index=%d, User=%s, UID=%lld\n",
		GetIndex(), m_szUsername, m_i64UID);
}

void GameSession::OnGetUserInfoReq(char* pData, INT32 i32Size)
{
	i3NetworkPacket packet;
	char buffer[512];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_GET_USER_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, m_szNickname, 64);				offset += 64;
	memcpy(buffer + offset, &m_i32Level, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_i64Exp, sizeof(int64_t));	offset += sizeof(int64_t);
	memcpy(buffer + offset, &m_i32Cash, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_i32GP, sizeof(int));			offset += sizeof(int);
	memcpy(buffer + offset, &m_i32RankId, sizeof(int));		offset += sizeof(int);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);

	m_eMainTask = GAME_TASK_CHANNEL;
}

// ============================================================================
// Player Data Load (from DataServer)
// ============================================================================

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

// ============================================================================
// Timeout & Reset
// ============================================================================

bool GameSession::IsTimedOut() const
{
	DWORD dwNow = GetTickCount();
	DWORD dwElapsed = (dwNow - m_dwLastPacketTime) / 1000;

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

// ============================================================================
// Send Helpers
// ============================================================================

void GameSession::SendConnectAck()
{
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_CONNECT_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));				offset += sizeof(uint16_t);
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
		memcpy(buffer + offset, &m_i64UID, sizeof(int64_t));	offset += sizeof(int64_t);
		memcpy(buffer + offset, m_szNickname, 64);				offset += 64;
	}

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::SendSimpleAck(uint16_t protocolAck, int32_t result)
{
	i3NetworkPacket packet;
	char buffer[16];
	int offset = 0;

	uint16_t size = 0;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &protocolAck, sizeof(uint16_t));	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));			offset += sizeof(int32_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}
