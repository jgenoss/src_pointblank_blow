#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "GameContextMain.h"
#include "GameSessionManager.h"
#include "Room.h"
#include "RoomManager.h"
#include "ObserverHelper.h"

// ============================================================================
// Room Creation & Join (7B)
// ============================================================================

void GameSession::OnRoomCreateReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask != GAME_TASK_LOBBY || m_i32ChannelNum < 0 || !g_pRoomManager)
		return;

	// Parse full GameRoomCreateInfo from client packet
	// Format: szTitle[64] + szPassword[16] + ui32StageId(4) + ui8GameMode(1) + ui8MapIndex(1)
	//   + ui8MaxPlayers(1) + ui8RoundType(1) + ui8SubType(1) + ui8WeaponFlag(1)
	//   + ui8InfoFlag(1) + ui32OptionFlag(4) + ui8IsClanMatch(1)
	GameRoomCreateInfo info;
	int pos = 0;

	if (i32Size >= pos + 64)
	{
		memcpy(info.szTitle, pData + pos, 64);
		info.szTitle[NET_ROOM_NAME_SIZE - 1] = '\0';
	}
	pos += 64;

	if (i32Size >= pos + 16)
	{
		memcpy(info.szPassword, pData + pos, 16);
		info.szPassword[NET_ROOM_PW_SIZE - 1] = '\0';
	}
	pos += 16;

	if (i32Size >= pos + (int)sizeof(uint32_t))
		info.ui32StageId = *(uint32_t*)(pData + pos);
	pos += sizeof(uint32_t);

	if (i32Size >= pos + 1) info.ui8GameMode = *(uint8_t*)(pData + pos);
	pos += 1;
	if (i32Size >= pos + 1) info.ui8MapIndex = *(uint8_t*)(pData + pos);
	pos += 1;
	if (i32Size >= pos + 1) info.ui8MaxPlayers = *(uint8_t*)(pData + pos);
	pos += 1;
	if (i32Size >= pos + 1) info.ui8RoundType = *(uint8_t*)(pData + pos);
	pos += 1;
	if (i32Size >= pos + 1) info.ui8SubType = *(uint8_t*)(pData + pos);
	pos += 1;
	if (i32Size >= pos + 1) info.ui8WeaponFlag = *(uint8_t*)(pData + pos);
	pos += 1;
	if (i32Size >= pos + 1) info.ui8InfoFlag = *(uint8_t*)(pData + pos);
	pos += 1;
	if (i32Size >= pos + (int)sizeof(uint32_t))
		info.ui32OptionFlag = *(uint32_t*)(pData + pos);
	pos += sizeof(uint32_t);
	if (i32Size >= pos + 1)
		info.bIsClanMatch = (*(uint8_t*)(pData + pos) != 0);

	// Validate
	if (info.ui8MaxPlayers < 2) info.ui8MaxPlayers = 2;
	if (info.ui8MaxPlayers > SLOT_MAX_COUNT) info.ui8MaxPlayers = SLOT_MAX_COUNT;
	if (info.ui8GameMode >= STAGE_MODE_MAX) info.ui8GameMode = STAGE_MODE_DEATHMATCH;

	int roomIdx = g_pRoomManager->OnCreateRoom(this, &info);

	// Send ACK
	int32_t result = (roomIdx >= 0) ? 0 : 1;
	uint8_t slotIdx = 0;

	if (roomIdx >= 0)
	{
		Room* pRoom = g_pRoomManager->GetRoom(m_i32ChannelNum, roomIdx);
		m_i32RoomIdx = roomIdx;
		m_i32SlotIdx = 0;
		m_pRoom = pRoom;
		m_eMainTask = GAME_TASK_READY_ROOM;

		printf("[GameSession] Room created - Index=%d, Room=%d, Ch=%d, Mode=%d, Map=%d\n",
			GetIndex(), roomIdx, m_i32ChannelNum, info.ui8GameMode, info.ui8MapIndex);
	}
	i3NetworkPacket packet(PROTOCOL_ROOM_CREATE_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&roomIdx, sizeof(int));
	packet.WriteData(&slotIdx, sizeof(uint8_t));
	SendPacketMessage(&packet);
}

void GameSession::OnRoomJoinReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask != GAME_TASK_LOBBY || m_i32ChannelNum < 0 || !g_pRoomManager)
		return;

	if (i32Size < (int)sizeof(int))
		return;

	int roomIdx = *(int*)pData;

	Room* pRoom = g_pRoomManager->GetRoom(m_i32ChannelNum, roomIdx);
	if (!pRoom || !pRoom->IsCreated())
	{
		SendSimpleAck(PROTOCOL_ROOM_JOIN_ACK, 1);	// Not found
		return;
	}

	// Check battle state (inter-enter flag)
	if (pRoom->GetRoomState() >= ROOM_STATE_LOADING &&
		!(pRoom->GetInfoFlag() & ROOM_INFO_FLAG_INTER_ENTER))
	{
		SendSimpleAck(PROTOCOL_ROOM_JOIN_ACK, 2);	// In battle
		return;
	}

	// Password check
	if (pRoom->HasPassword())
	{
		char szPassword[NET_ROOM_PW_SIZE] = {0};
		if (i32Size >= (int)(sizeof(int) + NET_ROOM_PW_SIZE))
		{
			memcpy(szPassword, pData + sizeof(int), NET_ROOM_PW_SIZE);
			szPassword[NET_ROOM_PW_SIZE - 1] = '\0';
		}

		if (!pRoom->CheckPassword(szPassword))
		{
			SendSimpleAck(PROTOCOL_ROOM_JOIN_ACK, 3);	// Wrong pw
			return;
		}
	}

	// Auto team balance
	int redCount = 0, blueCount = 0;
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (!pRoom->IsSlotEmpty(i))
		{
			if (pRoom->GetSlotInfo(i).ui8Team == TEAM_RED) redCount++;
			else blueCount++;
		}
	}
	int autoTeam = (redCount <= blueCount) ? TEAM_RED : TEAM_BLUE;

	int slot = g_pRoomManager->OnJoinRoom(this, m_i32ChannelNum, roomIdx, autoTeam);

	int32_t result = (slot >= 0) ? 0 : 4;	// 4 = full
	uint8_t slotByte = 0;

	if (slot >= 0)
	{
		slotByte = (uint8_t)slot;
		m_i32RoomIdx = roomIdx;
		m_i32SlotIdx = slot;
		m_pRoom = pRoom;
		m_eMainTask = GAME_TASK_READY_ROOM;
	}
	i3NetworkPacket packet(PROTOCOL_ROOM_JOIN_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&slotByte, sizeof(uint8_t));
	SendPacketMessage(&packet);

	// Notify others
	if (slot >= 0)
		SendSlotInfoToAll();
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

	SendSimpleAck(PROTOCOL_ROOM_LEAVE_ROOM_ACK, 0);
}

// ============================================================================
// Room Operations (7C)
// ============================================================================

void GameSession::OnRoomGetSlotInfoReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom)
		return;

	// Room info header
	int roomIdx = m_pRoom->GetRoomIdx();
	uint8_t gameMode = m_pRoom->GetGameMode();
	uint8_t mapIndex = m_pRoom->GetMapIndex();
	uint8_t maxPlayers = (uint8_t)m_pRoom->GetMaxPlayers();
	uint8_t roomState = m_pRoom->GetRoomState();
	uint8_t ownerSlot = (uint8_t)m_pRoom->GetOwnerSlot();

	i3NetworkPacket packet(PROTOCOL_ROOM_GET_SLOTINFO_ACK);
	packet.WriteData(&roomIdx, sizeof(int));
	packet.WriteData(&gameMode, 1);
	packet.WriteData(&mapIndex, 1);
	packet.WriteData(&maxPlayers, 1);
	packet.WriteData(&roomState, 1);
	packet.WriteData(&ownerSlot, 1);

	// Per-slot info (16 slots)
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		const GameSlotInfo& si = m_pRoom->GetSlotInfo(i);
		packet.WriteData(&si.ui8State, 1);
		packet.WriteData(&si.ui8Team, 1);
		packet.WriteData(&si.i32SessionIdx, 4);
		packet.WriteData(&si.ui8Rank, 1);
		packet.WriteData(&si.ui32ClanIdx, 4);
		packet.WriteData(&si.ui32ClanMark, 4);

		// Nickname for occupied slots
		char nick[64] = {0};
		GameSession* pSlotSession = m_pRoom->GetSlotSession(i);
		if (pSlotSession)
			strncpy_s(nick, pSlotSession->GetNickname(), _TRUNCATE);
		packet.WriteData(nick, 64);
	}
	SendPacketMessage(&packet);
}

void GameSession::OnRoomGetPlayerInfoReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || i32Size < 1)
		return;

	uint8_t targetSlot = *(uint8_t*)pData;
	if (targetSlot >= SLOT_MAX_COUNT)
		return;

	GameSession* pTarget = m_pRoom->GetSlotSession(targetSlot);
	if (!pTarget)
		return;

	char nick[64] = {0};
	strncpy_s(nick, pTarget->GetNickname(), _TRUNCATE);

	int level = pTarget->GetLevel();
	int rankId = pTarget->GetRankId();
	int kills = pTarget->GetKills();
	int deaths = pTarget->GetDeaths();
	i3NetworkPacket packet(PROTOCOL_ROOM_GET_PLAYERINFO_ACK);
	packet.WriteData(&targetSlot, 1);
	packet.WriteData(nick, 64);
	packet.WriteData(&level, sizeof(int));
	packet.WriteData(&rankId, sizeof(int));
	packet.WriteData(&kills, sizeof(int));
	packet.WriteData(&deaths, sizeof(int));
	SendPacketMessage(&packet);
}

void GameSession::OnRoomChangeRoomInfoReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_i32SlotIdx != m_pRoom->GetOwnerSlot())
	{
		SendSimpleAck(PROTOCOL_ROOM_CHANGE_ROOMINFO_ACK, 1);
		return;
	}

	GameRoomCreateInfo newInfo;
	int pos = 0;

	if (i32Size >= pos + 64) memcpy(newInfo.szTitle, pData + pos, 64);
	pos += 64;

	if (i32Size >= pos + (int)sizeof(uint32_t))
		newInfo.ui32StageId = *(uint32_t*)(pData + pos);
	pos += sizeof(uint32_t);

	if (i32Size >= pos + 1) newInfo.ui8GameMode = *(uint8_t*)(pData + pos);	pos += 1;
	if (i32Size >= pos + 1) newInfo.ui8MapIndex = *(uint8_t*)(pData + pos);	pos += 1;
	if (i32Size >= pos + 1) newInfo.ui8MaxPlayers = *(uint8_t*)(pData + pos);	pos += 1;
	if (i32Size >= pos + 1) newInfo.ui8RoundType = *(uint8_t*)(pData + pos);	pos += 1;
	if (i32Size >= pos + 1) newInfo.ui8SubType = *(uint8_t*)(pData + pos);	pos += 1;
	if (i32Size >= pos + 1) newInfo.ui8WeaponFlag = *(uint8_t*)(pData + pos);	pos += 1;
	if (i32Size >= pos + 1) newInfo.ui8InfoFlag = *(uint8_t*)(pData + pos);

	bool ok = m_pRoom->OnChangeRoomInfo(this, &newInfo);

	int32_t result = ok ? 0 : 1;
	uint8_t gm = 0, mi = 0, rt = 0, st = 0, wf = 0, ifl = 0;
	uint32_t stageId = 0;
	int mp = 0;

	if (ok)
	{
		gm = m_pRoom->GetGameMode();
		mi = m_pRoom->GetMapIndex();
		rt = m_pRoom->GetRoundType();
		st = m_pRoom->GetSubType();
		wf = m_pRoom->GetWeaponFlag();
		ifl = m_pRoom->GetInfoFlag();
		stageId = m_pRoom->GetStageId();
		mp = m_pRoom->GetMaxPlayers();
	}
	i3NetworkPacket packet(PROTOCOL_ROOM_CHANGE_ROOMINFO_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&stageId, 4);
	packet.WriteData(&gm, 1);
	packet.WriteData(&mi, 1);
	packet.WriteData(&mp, 4);
	packet.WriteData(&rt, 1);
	packet.WriteData(&st, 1);
	packet.WriteData(&wf, 1);
	packet.WriteData(&ifl, 1);
	if (ok)
		m_pRoom->SendToAll(&packet);
	else
		SendPacketMessage(&packet);
}

void GameSession::OnRoomChangePasswdReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_i32SlotIdx != m_pRoom->GetOwnerSlot())
	{
		SendSimpleAck(PROTOCOL_ROOM_CHANGE_PASSWD_ACK, 1);
		return;
	}

	char newPassword[NET_ROOM_PW_SIZE] = {0};
	if (i32Size >= NET_ROOM_PW_SIZE)
		memcpy(newPassword, pData, NET_ROOM_PW_SIZE);
	newPassword[NET_ROOM_PW_SIZE - 1] = '\0';

	bool ok = m_pRoom->OnChangePassword(this, newPassword);
	SendSimpleAck(PROTOCOL_ROOM_CHANGE_PASSWD_ACK, ok ? 0 : 1);
}

void GameSession::OnRoomChangeSlotReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_i32SlotIdx != m_pRoom->GetOwnerSlot())
	{
		SendSimpleAck(PROTOCOL_ROOM_CHANGE_SLOT_ACK, 1);
		return;
	}

	if (i32Size < 1)
		return;

	int targetSlot = *(uint8_t*)pData;
	bool ok = m_pRoom->OnKickPlayer(this, targetSlot);

	if (ok)
	{
		GameSession* pTarget = m_pRoom->GetSlotSession(targetSlot);
		if (pTarget)
		{
			pTarget->SendSimpleAck(PROTOCOL_ROOM_CHANGE_SLOT_ACK, 0);
			pTarget->SetRoom(nullptr);
			pTarget->SetRoomIdx(-1);
			pTarget->SetSlotIdx(-1);
			pTarget->SetTask(GAME_TASK_LOBBY);
		}
		SendSlotInfoToAll();
	}

	SendSimpleAck(PROTOCOL_ROOM_CHANGE_SLOT_ACK, ok ? 0 : 1);
}

void GameSession::OnRoomTeamChangeReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || i32Size < 1)
		return;

	int newTeam = *(uint8_t*)pData;
	bool ok = m_pRoom->OnChangeTeam(this, newTeam);

	int32_t result = ok ? 0 : 1;
	uint8_t slotIdx = (uint8_t)m_i32SlotIdx;
	uint8_t team = (uint8_t)newTeam;

	i3NetworkPacket packet(PROTOCOL_ROOM_PERSONAL_TEAM_CHANGE_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&slotIdx, 1);
	packet.WriteData(&team, 1);
	if (ok) m_pRoom->SendToAll(&packet);
	else SendPacketMessage(&packet);
}

void GameSession::OnRoomRequestMainReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom)
		return;

	int32_t result = (m_i32SlotIdx == m_pRoom->GetOwnerSlot()) ? 0 : 1;
	SendSimpleAck(PROTOCOL_ROOM_REQUEST_MAIN_ACK, result);
}

void GameSession::OnRoomRequestMainChangeReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_i32SlotIdx != m_pRoom->GetOwnerSlot())
	{
		SendSimpleAck(PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_ACK, 1);
		return;
	}

	if (i32Size < 1)
		return;

	int targetSlot = *(uint8_t*)pData;
	bool ok = m_pRoom->OnTransferOwner(this, targetSlot);

	int32_t result = ok ? 0 : 1;
	uint8_t newOwner = ok ? (uint8_t)m_pRoom->GetOwnerSlot() : 0;

	i3NetworkPacket packet(PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&newOwner, 1);
	if (ok) m_pRoom->SendToAll(&packet);
	else SendPacketMessage(&packet);
}

void GameSession::OnRoomChangeOptionInfoReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_i32SlotIdx != m_pRoom->GetOwnerSlot())
	{
		SendSimpleAck(PROTOCOL_ROOM_CHANGE_ROOM_OPTIONINFO_ACK, 1);
		return;
	}

	SendSimpleAck(PROTOCOL_ROOM_CHANGE_ROOM_OPTIONINFO_ACK, 0);
}

void GameSession::OnRoomChatReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || i32Size <= 0)
		return;

	if (IsMuted())
		return;

	// Check for GM admin commands (starts with '/')
	if (i32Size >= 1 && pData[0] == '/')
	{
		if (ProcessAdminCommand(pData, i32Size))
			return;	// Command consumed, don't broadcast
	}

	uint8_t slotIdx = (uint8_t)m_i32SlotIdx;
	uint16_t msgSize = (uint16_t)min(i32Size, 256);

	i3NetworkPacket packet(PROTOCOL_ROOM_CHATTING_ACK);
	packet.WriteData(&slotIdx, sizeof(uint8_t));
	packet.WriteData(&msgSize, sizeof(uint16_t));
	packet.WriteData(pData, msgSize);
	m_pRoom->SendToAll(&packet);
}

// ============================================================================
// Room Invite, Observer, Loading (Phase 3A)
// ============================================================================

void GameSession::OnRoomInviteLobbyUserReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_i32ChannelNum < 0)
	{
		SendSimpleAck(PROTOCOL_ROOM_INVITE_LOBBY_USER_ACK, 1);
		return;
	}

	// Parse: count(4) + sessionIdx(4) * count (max 5)
	if (i32Size < (int)sizeof(int32_t))
		return;

	int32_t inviteCount = *(int32_t*)pData;
	if (inviteCount <= 0 || inviteCount > 5)
	{
		SendSimpleAck(PROTOCOL_ROOM_INVITE_LOBBY_USER_ACK, 2);
		return;
	}

	if (i32Size < (int)(sizeof(int32_t) + inviteCount * sizeof(int32_t)))
		return;

	int32_t* pSessionIds = (int32_t*)(pData + sizeof(int32_t));

	// Send invite notification to each target session
	for (int i = 0; i < inviteCount; i++)
	{
		int32_t targetIdx = pSessionIds[i];
		if (!g_pGameSessionManager)
			continue;

		GameSession* pTarget = g_pGameSessionManager->GetSession(targetIdx);
		if (!pTarget || pTarget->GetTask() != GAME_TASK_LOBBY)
			continue;

		// Build invite notification for target
		int32_t invResult = 0;
		int32_t roomIdx = m_i32RoomIdx;
		char nick[64] = {0};
		strncpy_s(nick, m_szNickname, _TRUNCATE);

		i3NetworkPacket invitePacket(PROTOCOL_ROOM_INVITE_LOBBY_USER_ACK);
		invitePacket.WriteData(&invResult, sizeof(int32_t));
		invitePacket.WriteData(&roomIdx, sizeof(int32_t));
		invitePacket.WriteData(nick, 64);
		pTarget->SendPacketMessage(&invitePacket);
	}

	SendSimpleAck(PROTOCOL_ROOM_INVITE_LOBBY_USER_ACK, 0);
}

void GameSession::OnRoomChangeObserverSlotReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || i32Size < 1)
	{
		SendSimpleAck(PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_ACK, 1);
		return;
	}

	// ui8Direction: 0 = move to normal slot, 1 = move to observer slot
	uint8_t direction = *(uint8_t*)pData;
	int32_t result = 0;

	// Room must have observer mode enabled
	if (!(m_pRoom->GetInfoFlag() & ROOM_INFO_FLAG_OBSERVER_MODE))
	{
		result = 2;		// Observer mode not enabled
	}
	// Cannot change during battle
	else if (m_pRoom->GetRoomState() >= ROOM_STATE_COUNTDOWN_R)
	{
		result = 3;		// Cannot change during battle
	}
	else if (m_i32SlotIdx < 0 || m_i32SlotIdx >= SLOT_MAX_COUNT)
	{
		result = 1;
	}
	else
	{
		GameSlotInfo& slot = m_pRoom->GetSlotInfoMutable(m_i32SlotIdx);

		if (direction == 1 && !slot.bIsObserver)
		{
			// Move to observer
			slot.bIsObserver = true;
			slot.ui8State = SLOT_STATE_NORMAL;	// Reset ready state
			ObserverHelper::OnObserverJoined(m_pRoom, this, m_i32SlotIdx);
			printf("[GameSession] Observer: slot %d -> observer, UID=%lld\n", m_i32SlotIdx, m_i64UID);
		}
		else if (direction == 0 && slot.bIsObserver)
		{
			// Move back to normal
			ObserverHelper::OnObserverLeft(m_pRoom, this);
			slot.bIsObserver = false;
			slot.ui8State = SLOT_STATE_NORMAL;
			printf("[GameSession] Observer: slot %d -> normal, UID=%lld\n", m_i32SlotIdx, m_i64UID);
		}
		else
		{
			result = 4;		// Already in requested state
		}
	}

	// Broadcast to all in room
	uint8_t slotIdx = (uint8_t)m_i32SlotIdx;
	i3NetworkPacket packet(PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&slotIdx, 1);
	packet.WriteData(&direction, 1);
	m_pRoom->SendToAll(&packet);
}

void GameSession::OnRoomLoadingStartReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom)
		return;

	// Room master signals that loading should begin
	if (m_i32SlotIdx != m_pRoom->GetOwnerSlot())
	{
		SendSimpleAck(PROTOCOL_ROOM_LOADING_START_ACK, 1);
		return;
	}

	// Broadcast loading start to all players in room
	int32_t result = 0;
	i3NetworkPacket packet(PROTOCOL_ROOM_LOADING_START_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	m_pRoom->SendToAll(&packet);
}

// ============================================================================
// Slot Info Broadcast Helper
// ============================================================================

void GameSession::SendSlotInfoToAll()
{
	if (!m_pRoom)
		return;

	i3NetworkPacket packet(PROTOCOL_BATTLE_SLOT_STATE_CHANGE_ACK);
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		const GameSlotInfo& si = m_pRoom->GetSlotInfo(i);
		packet.WriteData(&si.ui8State, 1);
		packet.WriteData(&si.ui8Team, 1);
	}
	m_pRoom->SendToAll(&packet);
}

// ============================================================================
// Room Info & Misc Operations (Batch 15)
// ============================================================================

void GameSession::OnRoomInfoEnterReq(char* pData, INT32 i32Size)
{
	// View room info without joining (from lobby)
	if (m_eMainTask < GAME_TASK_LOBBY)
		return;

	if (i32Size < 6)
	{
		SendSimpleAck(PROTOCOL_ROOM_INFO_ENTER_ACK, -1);
		return;
	}

	uint16_t channel = *(uint16_t*)pData;
	int32_t roomIdx = *(int32_t*)(pData + 2);

	Room* pRoom = g_pRoomManager->GetRoom(channel, roomIdx);
	if (!pRoom || !pRoom->IsCreated())
	{
		SendSimpleAck(PROTOCOL_ROOM_INFO_ENTER_ACK, -1);
		return;
	}

	int32_t result = 0;

	// Room basic info
	uint8_t mode = pRoom->GetGameMode();
	uint8_t mapIdx = pRoom->GetMapIndex();
	uint8_t state = pRoom->GetRoomState();
	uint8_t maxPlayers = pRoom->GetMaxPlayers();
	uint8_t playerCount = (uint8_t)pRoom->GetPlayerCount();

	i3NetworkPacket packet(PROTOCOL_ROOM_INFO_ENTER_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&mode, 1);
	packet.WriteData(&mapIdx, 1);
	packet.WriteData(&state, 1);
	packet.WriteData(&maxPlayers, 1);
	packet.WriteData(&playerCount, 1);

	// Slot info with nicknames
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		const GameSlotInfo& si = pRoom->GetSlotInfo(i);
		packet.WriteData(&si.ui8State, 1);
		packet.WriteData(&si.ui8Team, 1);

		GameSession* pSlotSession = pRoom->GetSlotSession(i);
		uint8_t level = 0;
		uint8_t rank = 0;
		char nick[16] = {0};
		if (pSlotSession)
		{
			level = (uint8_t)pSlotSession->GetLevel();
			rank = (uint8_t)pSlotSession->GetRankId();
			int nickLen = (int)strlen(pSlotSession->GetNickname());
			if (nickLen > 16) nickLen = 16;
			memcpy(nick, pSlotSession->GetNickname(), nickLen);
		}
		packet.WriteData(&level, 1);
		packet.WriteData(&rank, 1);
		packet.WriteData(nick, 16);
	}
	SendPacketMessage(&packet);
}

void GameSession::OnRoomInfoLeaveReq(char* pData, INT32 i32Size)
{
	SendSimpleAck(PROTOCOL_ROOM_INFO_LEAVE_ACK, 0);
}

void GameSession::OnRoomGetLobbyUserListReq(char* pData, INT32 i32Size)
{
	// Lists users in lobby for same channel (for room invite)
	if (m_eMainTask < GAME_TASK_READY_ROOM)
		return;

	int32_t result = 0;
	uint16_t userCount = 0;

	// Collect lobby users first to get count
	const int MAX_LOBBY_USERS = 50;
	struct LobbyUserEntry { int32_t sessionIdx; uint8_t level; uint8_t rank; char nick[16]; };
	LobbyUserEntry lobbyUsers[MAX_LOBBY_USERS];

	for (int i = 0; i < MAX_GAME_SESSIONS && userCount < MAX_LOBBY_USERS; i++)
	{
		GameSession* pOther = g_pGameSessionManager->GetSession(i);
		if (!pOther || pOther == this)
			continue;
		if (pOther->GetMainTask() != GAME_TASK_LOBBY)
			continue;
		if (pOther->GetChannelNum() != m_i32ChannelNum)
			continue;

		LobbyUserEntry& e = lobbyUsers[userCount];
		e.sessionIdx = pOther->GetIndex();
		e.level = (uint8_t)pOther->GetLevel();
		e.rank = (uint8_t)pOther->GetRankId();
		memset(e.nick, 0, 16);
		const char* nick = pOther->GetNickname();
		int nickLen = (int)strlen(nick);
		if (nickLen > 16) nickLen = 16;
		memcpy(e.nick, nick, nickLen);
		userCount++;
	}

	i3NetworkPacket packet(PROTOCOL_ROOM_GET_LOBBY_USER_LIST_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&userCount, sizeof(uint16_t));
	for (int i = 0; i < userCount; i++)
	{
		packet.WriteData(&lobbyUsers[i].sessionIdx, 4);
		packet.WriteData(&lobbyUsers[i].level, 1);
		packet.WriteData(&lobbyUsers[i].rank, 1);
		packet.WriteData(lobbyUsers[i].nick, 16);
	}
	SendPacketMessage(&packet);
}

void GameSession::OnRoomCheckMainReq(char* pData, INT32 i32Size)
{
	// Returns the room owner's slot index
	if (m_eMainTask < GAME_TASK_READY_ROOM || !m_pRoom)
	{
		SendSimpleAck(PROTOCOL_ROOM_CHECK_MAIN_ACK, -1);
		return;
	}

	int32_t result = 0;

	uint8_t ownerSlot = (uint8_t)m_pRoom->GetOwnerSlot();
	i3NetworkPacket packet(PROTOCOL_ROOM_CHECK_MAIN_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&ownerSlot, 1);
	SendPacketMessage(&packet);
}

void GameSession::OnRoomMasterTeamChangeReq(char* pData, INT32 i32Size)
{
	// Room master forces a player to change teams
	if (m_eMainTask < GAME_TASK_READY_ROOM || !m_pRoom)
	{
		SendSimpleAck(PROTOCOL_ROOM_MASTER_TEAM_CHANGE_ACK, -1);
		return;
	}

	if (m_i32SlotIdx != m_pRoom->GetOwnerSlot())
	{
		SendSimpleAck(PROTOCOL_ROOM_MASTER_TEAM_CHANGE_ACK, -2);
		return;
	}

	if (i32Size < 1)
		return;

	uint8_t targetSlot = *(uint8_t*)pData;
	if (targetSlot >= SLOT_MAX_COUNT)
	{
		SendSimpleAck(PROTOCOL_ROOM_MASTER_TEAM_CHANGE_ACK, -3);
		return;
	}

	GameSession* pTarget = m_pRoom->GetSlotSession(targetSlot);
	if (!pTarget)
	{
		SendSimpleAck(PROTOCOL_ROOM_MASTER_TEAM_CHANGE_ACK, -4);
		return;
	}

	// Toggle team
	GameSlotInfo& si = const_cast<GameSlotInfo&>(m_pRoom->GetSlotInfo(targetSlot));
	si.ui8Team = (si.ui8Team == 0) ? 1 : 0;

	// Broadcast slot info change to all
	SendSlotInfoToAll();
	SendSimpleAck(PROTOCOL_ROOM_MASTER_TEAM_CHANGE_ACK, 0);
}

void GameSession::OnRoomGetNicknameReq(char* pData, INT32 i32Size)
{
	// Get player nickname by slot index
	if (m_eMainTask < GAME_TASK_READY_ROOM || !m_pRoom)
	{
		SendSimpleAck(PROTOCOL_ROOM_GET_NICKNAME_ACK, -1);
		return;
	}

	if (i32Size < 1)
		return;

	uint8_t slotIdx = *(uint8_t*)pData;

	int32_t result = 0;
	GameSession* pTarget = nullptr;
	if (slotIdx < SLOT_MAX_COUNT)
		pTarget = m_pRoom->GetSlotSession(slotIdx);

	if (!pTarget)
		result = -1;

	char nick[32] = {0};
	if (pTarget)
	{
		int nickLen = (int)strlen(pTarget->GetNickname());
		if (nickLen > 32) nickLen = 32;
		memcpy(nick, pTarget->GetNickname(), nickLen);
	}
	i3NetworkPacket packet(PROTOCOL_ROOM_GET_NICKNAME_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&slotIdx, 1);
	packet.WriteData(nick, 32);
	SendPacketMessage(&packet);
}

void GameSession::OnRoomGetRankReq(char* pData, INT32 i32Size)
{
	// Get player rank by slot index
	if (m_eMainTask < GAME_TASK_READY_ROOM || !m_pRoom)
	{
		SendSimpleAck(PROTOCOL_ROOM_GET_RANK_ACK, -1);
		return;
	}

	if (i32Size < 1)
		return;

	uint8_t slotIdx = *(uint8_t*)pData;

	int32_t result = 0;
	GameSession* pTarget = nullptr;
	if (slotIdx < SLOT_MAX_COUNT)
		pTarget = m_pRoom->GetSlotSession(slotIdx);

	if (!pTarget)
		result = -1;

	int32_t rank = 0, level = 0, kills = 0, deaths = 0;
	if (pTarget)
	{
		rank = pTarget->GetRankId();
		level = pTarget->GetLevel();
		kills = pTarget->GetKills();
		deaths = pTarget->GetDeaths();
	}
	i3NetworkPacket packet(PROTOCOL_ROOM_GET_RANK_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&slotIdx, 1);
	packet.WriteData(&rank, 4);
	packet.WriteData(&level, 4);
	packet.WriteData(&kills, 4);
	packet.WriteData(&deaths, 4);
	SendPacketMessage(&packet);
}

// ============================================================================
// Room Extended Operations (Batch 16)
// ============================================================================

void GameSession::OnRoomChangeRoomAllInfoReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_READY_ROOM || !m_pRoom)
	{
		SendSimpleAck(PROTOCOL_ROOM_CHANGE_ROOM_ALLINFO_ACK, -1);
		return;
	}
	if (m_i32SlotIdx != m_pRoom->GetOwnerSlot())
	{
		SendSimpleAck(PROTOCOL_ROOM_CHANGE_ROOM_ALLINFO_ACK, -2);
		return;
	}
	if (i32Size < 6)
	{
		SendSimpleAck(PROTOCOL_ROOM_CHANGE_ROOM_ALLINFO_ACK, -3);
		return;
	}
	SendSimpleAck(PROTOCOL_ROOM_CHANGE_ROOM_ALLINFO_ACK, 0);
	SendSlotInfoToAll();
}

void GameSession::OnRoomGetColorNickReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_READY_ROOM || !m_pRoom)
		return;
	if (i32Size < 1)
		return;

	uint8_t slotIdx = *(uint8_t*)pData;

	uint32_t colorNick = 0;
	GameSession* pTarget = nullptr;
	if (slotIdx < SLOT_MAX_COUNT)
		pTarget = m_pRoom->GetSlotSession(slotIdx);
	if (pTarget)
		colorNick = pTarget->GetColorNick();
	i3NetworkPacket packet(PROTOCOL_ROOM_GET_COLOR_NICK_ACK);
	packet.WriteData(&slotIdx, 1);
	packet.WriteData(&colorNick, 4);
	SendPacketMessage(&packet);
}

void GameSession::OnRoomGetColorMuzzleFlashReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_READY_ROOM || !m_pRoom)
		return;
	if (i32Size < 1)
		return;

	uint8_t slotIdx = *(uint8_t*)pData;

	uint32_t muzzleColor = 0;
	i3NetworkPacket packet(PROTOCOL_ROOM_GET_COLOR_MUZZLE_FLASH_ACK);
	packet.WriteData(&slotIdx, 1);
	packet.WriteData(&muzzleColor, 4);
	SendPacketMessage(&packet);
}

void GameSession::OnRoomCharaShiftPosReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_READY_ROOM || !m_pRoom)
	{
		SendSimpleAck(PROTOCOL_ROOM_CHARA_SHIFT_POS_ACK, -1);
		return;
	}
	if (i32Size < 1)
		return;

	uint8_t newSlot = *(uint8_t*)pData;
	if (newSlot >= MAX_CHARA_SLOT)
	{
		SendSimpleAck(PROTOCOL_ROOM_CHARA_SHIFT_POS_ACK, -2);
		return;
	}
	m_ui8ActiveCharaSlot = newSlot;
	SendSimpleAck(PROTOCOL_ROOM_CHARA_SHIFT_POS_ACK, 0);

	// Notify others
	uint8_t mySlot = (uint8_t)m_i32SlotIdx;
	i3NetworkPacket packet(PROTOCOL_ROOM_CHARA_SHIFT_POS_OTHER_ACK);
	packet.WriteData(&mySlot, 1);
	packet.WriteData(&newSlot, 1);
	m_pRoom->SendToAllExcept(this, &packet);
}

void GameSession::OnRoomEquipmentTempWeaponReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_READY_ROOM || !m_pRoom)
	{
		SendSimpleAck(PROTOCOL_ROOM_EQUIPMENT_TEMP_WEAPON_ACK, -1);
		return;
	}
	if (i32Size < 5)
		return;

	uint8_t weaponSlot = *(uint8_t*)pData;
	uint32_t weaponId = *(uint32_t*)(pData + 1);

	int32_t result = 0;

	uint8_t mySlot = (uint8_t)m_i32SlotIdx;
	i3NetworkPacket packet(PROTOCOL_ROOM_EQUIPMENT_TEMP_WEAPON_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&mySlot, 1);
	packet.WriteData(&weaponSlot, 1);
	packet.WriteData(&weaponId, 4);
	m_pRoom->SendToAll(&packet);
}

void GameSession::OnRoomInviteLobbyUserListReq(char* pData, INT32 i32Size)
{
	// Reuse lobby user list logic with different proto
	OnRoomGetLobbyUserListReq(pData, i32Size);
}

void GameSession::OnRoomTsEventReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_READY_ROOM || !m_pRoom)
		return;

	int copyLen = (i32Size > 240) ? 240 : i32Size;
	i3NetworkPacket packet(PROTOCOL_ROOM_TS_EVENT_ACK);
	if (copyLen > 0)
		packet.WriteData(pData, copyLen);
	m_pRoom->SendToAll(&packet);
}

void GameSession::OnRoomRequestMainChangeWhoReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_READY_ROOM || !m_pRoom)
	{
		SendSimpleAck(PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_WHO_ACK, -1);
		return;
	}
	if (m_i32SlotIdx != m_pRoom->GetOwnerSlot())
	{
		SendSimpleAck(PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_WHO_ACK, -2);
		return;
	}
	if (i32Size < 1)
		return;

	uint8_t targetSlot = *(uint8_t*)pData;
	if (targetSlot >= SLOT_MAX_COUNT || !m_pRoom->GetSlotSession(targetSlot))
	{
		SendSimpleAck(PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_WHO_ACK, -3);
		return;
	}

	m_pRoom->SetOwnerSlot(targetSlot);

	int32_t result = 0;
	i3NetworkPacket packet(PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_WHO_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&targetSlot, 1);
	m_pRoom->SendToAll(&packet);
}

// ============================================================================
// Batch 18 - Room extras
// ============================================================================

void GameSession::OnRoomNewGetSlotOneInfoReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || i32Size < (int)sizeof(uint8_t))
	{
		SendSimpleAck(PROTOCOL_ROOM_NEW_GET_SLOTONEINFO_ACK, -1);
		return;
	}

	uint8_t slotIdx = *(uint8_t*)pData;
	if (slotIdx >= SLOT_MAX_COUNT)
	{
		SendSimpleAck(PROTOCOL_ROOM_NEW_GET_SLOTONEINFO_ACK, -2);
		return;
	}

	const GameSlotInfo& slot = m_pRoom->GetSlotInfo(slotIdx);

	int32_t result = 0;

	char nick[32] = {};
	GameSession* pSlotSess = m_pRoom->GetSlotSession(slotIdx);
	if (pSlotSess)
		strncpy(nick, pSlotSess->GetNickname(), 31);
	i3NetworkPacket packet(PROTOCOL_ROOM_NEW_GET_SLOTONEINFO_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&slotIdx, 1);
	packet.WriteData(&slot.ui8State, 1);
	packet.WriteData(&slot.ui8Team, 1);
	packet.WriteData(&slot.i32SessionIdx, 4);
	packet.WriteData(&slot.ui8Rank, 1);
	packet.WriteData(&slot.ui32ClanIdx, 4);
	packet.WriteData(&slot.ui32ClanMark, 4);
	packet.WriteData(&slot.bIsObserver, 1);
	packet.WriteData(nick, 32);
	SendPacketMessage(&packet);
}

void GameSession::OnRoomChangeHiddenSlotReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_i32SlotIdx != m_pRoom->GetOwnerSlot())
	{
		SendSimpleAck(PROTOCOL_ROOM_CHANGE_HIDDEN_SLOT_ACK, -1);
		return;
	}

	if (i32Size < 2)
	{
		SendSimpleAck(PROTOCOL_ROOM_CHANGE_HIDDEN_SLOT_ACK, -1);
		return;
	}

	uint8_t slotIdx = *(uint8_t*)pData;
	uint8_t hidden = *(uint8_t*)(pData + 1);

	if (slotIdx >= SLOT_MAX_COUNT)
	{
		SendSimpleAck(PROTOCOL_ROOM_CHANGE_HIDDEN_SLOT_ACK, -2);
		return;
	}

	if (!m_pRoom->IsSlotEmpty(slotIdx))
	{
		SendSimpleAck(PROTOCOL_ROOM_CHANGE_HIDDEN_SLOT_ACK, -3);
		return;
	}

	int32_t resultVal = 0;
	i3NetworkPacket packet(PROTOCOL_ROOM_CHANGE_HIDDEN_SLOT_ACK);
	packet.WriteData(&resultVal, sizeof(int32_t));
	packet.WriteData(&slotIdx, 1);
	packet.WriteData(&hidden, 1);
	m_pRoom->SendToAll(&packet);
}

void GameSession::OnRoomTeamBalanceReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_i32SlotIdx != m_pRoom->GetOwnerSlot())
	{
		SendSimpleAck(PROTOCOL_ROOM_TEAM_BALANCE_ACK, -1);
		return;
	}

	int assignTeam = 0;
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (!m_pRoom->IsSlotEmpty(i))
		{
			GameSlotInfo& slot = m_pRoom->GetSlotInfoMutable(i);
			slot.ui8Team = (uint8_t)(assignTeam % 2);
			assignTeam++;
		}
	}

	int32_t result = 0;

	i3NetworkPacket packet(PROTOCOL_ROOM_TEAM_BALANCE_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		uint8_t team = m_pRoom->GetSlotInfo(i).ui8Team;
		packet.WriteData(&team, 1);
	}
	m_pRoom->SendToAll(&packet);
}
