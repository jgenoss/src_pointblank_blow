#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "GameContextMain.h"
#include "GameSessionManager.h"
#include "Room.h"
#include "RoomManager.h"

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
	i3NetworkPacket packet;
	char buffer[256];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_ROOM_CREATE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = (roomIdx >= 0) ? 0 : 1;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	if (roomIdx >= 0)
	{
		memcpy(buffer + offset, &roomIdx, sizeof(int));		offset += sizeof(int);
		uint8_t slotIdx = 0;
		memcpy(buffer + offset, &slotIdx, sizeof(uint8_t));	offset += sizeof(uint8_t);

		Room* pRoom = g_pRoomManager->GetRoom(m_i32ChannelNum, roomIdx);
		m_i32RoomIdx = roomIdx;
		m_i32SlotIdx = 0;
		m_pRoom = pRoom;
		m_eMainTask = GAME_TASK_READY_ROOM;

		printf("[GameSession] Room created - Index=%d, Room=%d, Ch=%d, Mode=%d, Map=%d\n",
			GetIndex(), roomIdx, m_i32ChannelNum, info.ui8GameMode, info.ui8MapIndex);
	}

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
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

	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_ROOM_JOIN_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = (slot >= 0) ? 0 : 4;	// 4 = full
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	if (slot >= 0)
	{
		uint8_t slotByte = (uint8_t)slot;
		memcpy(buffer + offset, &slotByte, sizeof(uint8_t));	offset += sizeof(uint8_t);

		m_i32RoomIdx = roomIdx;
		m_i32SlotIdx = slot;
		m_pRoom = pRoom;
		m_eMainTask = GAME_TASK_READY_ROOM;
	}

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);

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

	i3NetworkPacket packet;
	char buffer[2048];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_ROOM_GET_SLOTINFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	// Room info header
	int roomIdx = m_pRoom->GetRoomIdx();
	uint8_t gameMode = m_pRoom->GetGameMode();
	uint8_t mapIndex = m_pRoom->GetMapIndex();
	uint8_t maxPlayers = (uint8_t)m_pRoom->GetMaxPlayers();
	uint8_t roomState = m_pRoom->GetRoomState();
	uint8_t ownerSlot = (uint8_t)m_pRoom->GetOwnerSlot();

	memcpy(buffer + offset, &roomIdx, sizeof(int));	offset += sizeof(int);
	memcpy(buffer + offset, &gameMode, 1);			offset += 1;
	memcpy(buffer + offset, &mapIndex, 1);			offset += 1;
	memcpy(buffer + offset, &maxPlayers, 1);		offset += 1;
	memcpy(buffer + offset, &roomState, 1);			offset += 1;
	memcpy(buffer + offset, &ownerSlot, 1);			offset += 1;

	// Per-slot info (16 slots)
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		const GameSlotInfo& si = m_pRoom->GetSlotInfo(i);
		memcpy(buffer + offset, &si.ui8State, 1);			offset += 1;
		memcpy(buffer + offset, &si.ui8Team, 1);			offset += 1;
		memcpy(buffer + offset, &si.i32SessionIdx, 4);		offset += 4;
		memcpy(buffer + offset, &si.ui8Rank, 1);			offset += 1;
		memcpy(buffer + offset, &si.ui32ClanIdx, 4);		offset += 4;
		memcpy(buffer + offset, &si.ui32ClanMark, 4);		offset += 4;

		// Nickname for occupied slots
		char nick[64] = {0};
		GameSession* pSlotSession = m_pRoom->GetSlotSession(i);
		if (pSlotSession)
			strncpy_s(nick, pSlotSession->GetNickname(), _TRUNCATE);
		memcpy(buffer + offset, nick, 64);					offset += 64;
	}

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
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

	i3NetworkPacket packet;
	char buffer[256];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_ROOM_GET_PLAYERINFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	memcpy(buffer + offset, &targetSlot, 1);				offset += 1;

	char nick[64] = {0};
	strncpy_s(nick, pTarget->GetNickname(), _TRUNCATE);
	memcpy(buffer + offset, nick, 64);						offset += 64;

	int level = pTarget->GetLevel();
	int rankId = pTarget->GetRankId();
	int kills = pTarget->GetKills();
	int deaths = pTarget->GetDeaths();
	memcpy(buffer + offset, &level, sizeof(int));			offset += sizeof(int);
	memcpy(buffer + offset, &rankId, sizeof(int));			offset += sizeof(int);
	memcpy(buffer + offset, &kills, sizeof(int));			offset += sizeof(int);
	memcpy(buffer + offset, &deaths, sizeof(int));			offset += sizeof(int);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
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

	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_ROOM_CHANGE_ROOMINFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = ok ? 0 : 1;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	if (ok)
	{
		uint8_t gm = m_pRoom->GetGameMode();
		uint8_t mi = m_pRoom->GetMapIndex();
		uint8_t rt = m_pRoom->GetRoundType();
		uint8_t st = m_pRoom->GetSubType();
		uint8_t wf = m_pRoom->GetWeaponFlag();
		uint8_t ifl = m_pRoom->GetInfoFlag();
		uint32_t stageId = m_pRoom->GetStageId();
		int mp = m_pRoom->GetMaxPlayers();

		memcpy(buffer + offset, &stageId, 4);	offset += 4;
		memcpy(buffer + offset, &gm, 1);		offset += 1;
		memcpy(buffer + offset, &mi, 1);		offset += 1;
		memcpy(buffer + offset, &mp, 4);		offset += 4;
		memcpy(buffer + offset, &rt, 1);		offset += 1;
		memcpy(buffer + offset, &st, 1);		offset += 1;
		memcpy(buffer + offset, &wf, 1);		offset += 1;
		memcpy(buffer + offset, &ifl, 1);		offset += 1;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	if (ok)
		m_pRoom->SendToAll(&packet);
	else
		SendMessage(&packet);
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

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_ROOM_PERSONAL_TEAM_CHANGE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = ok ? 0 : 1;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	if (ok)
	{
		uint8_t slotIdx = (uint8_t)m_i32SlotIdx;
		uint8_t team = (uint8_t)newTeam;
		memcpy(buffer + offset, &slotIdx, 1);	offset += 1;
		memcpy(buffer + offset, &team, 1);		offset += 1;
	}

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	if (ok) m_pRoom->SendToAll(&packet);
	else SendMessage(&packet);
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

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = ok ? 0 : 1;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	if (ok)
	{
		uint8_t newOwner = (uint8_t)m_pRoom->GetOwnerSlot();
		memcpy(buffer + offset, &newOwner, 1);	offset += 1;
	}

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	if (ok) m_pRoom->SendToAll(&packet);
	else SendMessage(&packet);
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

	// Check for GM admin commands (starts with '/')
	if (i32Size >= 1 && pData[0] == '/')
	{
		if (ProcessAdminCommand(pData, i32Size))
			return;	// Command consumed, don't broadcast
	}

	i3NetworkPacket packet;
	char buffer[512];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_ROOM_CHATTING_ACK;
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
		i3NetworkPacket invitePacket;
		char invBuf[256];
		int invOff = 0;

		uint16_t invSize = 0;
		uint16_t invProto = PROTOCOL_ROOM_INVITE_LOBBY_USER_ACK;
		invOff += sizeof(uint16_t);
		memcpy(invBuf + invOff, &invProto, sizeof(uint16_t));	invOff += sizeof(uint16_t);

		int32_t invResult = 0;
		memcpy(invBuf + invOff, &invResult, sizeof(int32_t));	invOff += sizeof(int32_t);

		// Inviter info
		int32_t roomIdx = m_i32RoomIdx;
		memcpy(invBuf + invOff, &roomIdx, sizeof(int32_t));	invOff += sizeof(int32_t);

		char nick[64] = {0};
		strncpy_s(nick, m_szNickname, _TRUNCATE);
		memcpy(invBuf + invOff, nick, 64);						invOff += 64;

		invSize = (uint16_t)invOff;
		memcpy(invBuf, &invSize, sizeof(uint16_t));

		invitePacket.SetPacketData(invBuf, invOff);
		pTarget->SendMessage(&invitePacket);
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
			printf("[GameSession] Observer: slot %d -> observer, UID=%lld\n", m_i32SlotIdx, m_i64UID);
		}
		else if (direction == 0 && slot.bIsObserver)
		{
			// Move back to normal
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
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	uint8_t slotIdx = (uint8_t)m_i32SlotIdx;
	memcpy(buffer + offset, &slotIdx, 1);					offset += 1;
	memcpy(buffer + offset, &direction, 1);					offset += 1;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
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
	i3NetworkPacket packet;
	char buffer[16];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_ROOM_LOADING_START_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	m_pRoom->SendToAll(&packet);
}

// ============================================================================
// Slot Info Broadcast Helper
// ============================================================================

void GameSession::SendSlotInfoToAll()
{
	if (!m_pRoom)
		return;

	i3NetworkPacket packet;
	char buffer[256];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BATTLE_SLOT_STATE_CHANGE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		const GameSlotInfo& si = m_pRoom->GetSlotInfo(i);
		memcpy(buffer + offset, &si.ui8State, 1);	offset += 1;
		memcpy(buffer + offset, &si.ui8Team, 1);	offset += 1;
	}

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
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

	i3NetworkPacket packet;
	char buffer[1024];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_ROOM_INFO_ENTER_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	// Room basic info
	uint8_t mode = pRoom->GetGameMode();
	uint8_t mapIdx = pRoom->GetMapIndex();
	uint8_t state = pRoom->GetRoomState();
	uint8_t maxPlayers = pRoom->GetMaxPlayers();
	uint8_t playerCount = (uint8_t)pRoom->GetPlayerCount();

	memcpy(buffer + offset, &mode, 1);			offset += 1;
	memcpy(buffer + offset, &mapIdx, 1);		offset += 1;
	memcpy(buffer + offset, &state, 1);			offset += 1;
	memcpy(buffer + offset, &maxPlayers, 1);	offset += 1;
	memcpy(buffer + offset, &playerCount, 1);	offset += 1;

	// Slot info with nicknames
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (offset + 80 > (int)sizeof(buffer))
			break;

		const GameSlotInfo& si = pRoom->GetSlotInfo(i);
		memcpy(buffer + offset, &si.ui8State, 1);	offset += 1;
		memcpy(buffer + offset, &si.ui8Team, 1);	offset += 1;

		GameSession* pSlotSession = pRoom->GetSlotSession(i);
		if (pSlotSession)
		{
			uint8_t level = (uint8_t)pSlotSession->GetLevel();
			uint8_t rank = (uint8_t)pSlotSession->GetRankId();
			memcpy(buffer + offset, &level, 1);	offset += 1;
			memcpy(buffer + offset, &rank, 1);		offset += 1;
			int nickLen = (int)strlen(pSlotSession->GetNickname());
			if (nickLen > 16) nickLen = 16;
			memcpy(buffer + offset, pSlotSession->GetNickname(), nickLen);
			memset(buffer + offset + nickLen, 0, 16 - nickLen);
			offset += 16;
		}
		else
		{
			memset(buffer + offset, 0, 18);
			offset += 18;
		}
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
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

	i3NetworkPacket packet;
	char buffer[4096];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_ROOM_GET_LOBBY_USER_LIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	// Reserve space for user count
	int countOffset = offset;
	uint16_t userCount = 0;
	offset += sizeof(uint16_t);

	// Iterate all sessions and find those in lobby of same channel
	const int MAX_LOBBY_USERS = 50;
	for (int i = 0; i < MAX_GAME_SESSIONS && userCount < MAX_LOBBY_USERS; i++)
	{
		GameSession* pOther = g_pGameSessionManager->GetSession(i);
		if (!pOther || pOther == this)
			continue;
		if (pOther->GetMainTask() != GAME_TASK_LOBBY)
			continue;
		if (pOther->GetChannelNum() != m_i32ChannelNum)
			continue;

		if (offset + 28 > (int)sizeof(buffer))
			break;

		int32_t sessionIdx = pOther->GetIndex();
		uint8_t level = (uint8_t)pOther->GetLevel();
		uint8_t rank = (uint8_t)pOther->GetRankId();
		memcpy(buffer + offset, &sessionIdx, 4);	offset += 4;
		memcpy(buffer + offset, &level, 1);			offset += 1;
		memcpy(buffer + offset, &rank, 1);			offset += 1;

		const char* nick = pOther->GetNickname();
		int nickLen = (int)strlen(nick);
		if (nickLen > 16) nickLen = 16;
		memcpy(buffer + offset, nick, nickLen);
		memset(buffer + offset + nickLen, 0, 16 - nickLen);
		offset += 16;

		userCount++;
	}

	memcpy(buffer + countOffset, &userCount, sizeof(uint16_t));

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnRoomCheckMainReq(char* pData, INT32 i32Size)
{
	// Returns the room owner's slot index
	if (m_eMainTask < GAME_TASK_READY_ROOM || !m_pRoom)
	{
		SendSimpleAck(PROTOCOL_ROOM_CHECK_MAIN_ACK, -1);
		return;
	}

	i3NetworkPacket packet;
	char buffer[16];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_ROOM_CHECK_MAIN_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	uint8_t ownerSlot = (uint8_t)m_pRoom->GetOwnerSlot();
	memcpy(buffer + offset, &ownerSlot, 1);	offset += 1;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
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

	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_ROOM_GET_NICKNAME_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	GameSession* pTarget = nullptr;
	if (slotIdx < SLOT_MAX_COUNT)
		pTarget = m_pRoom->GetSlotSession(slotIdx);

	if (!pTarget)
		result = -1;

	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(buffer + offset, &slotIdx, 1);				offset += 1;

	if (pTarget)
	{
		const char* nick = pTarget->GetNickname();
		int nickLen = (int)strlen(nick);
		if (nickLen > 32) nickLen = 32;
		memcpy(buffer + offset, nick, nickLen);
		memset(buffer + offset + nickLen, 0, 32 - nickLen);
		offset += 32;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
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

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_ROOM_GET_RANK_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	GameSession* pTarget = nullptr;
	if (slotIdx < SLOT_MAX_COUNT)
		pTarget = m_pRoom->GetSlotSession(slotIdx);

	if (!pTarget)
		result = -1;

	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(buffer + offset, &slotIdx, 1);				offset += 1;

	if (pTarget)
	{
		int32_t rank = pTarget->GetRankId();
		int32_t level = pTarget->GetLevel();
		int32_t kills = pTarget->GetKills();
		int32_t deaths = pTarget->GetDeaths();
		memcpy(buffer + offset, &rank, 4);		offset += 4;
		memcpy(buffer + offset, &level, 4);		offset += 4;
		memcpy(buffer + offset, &kills, 4);		offset += 4;
		memcpy(buffer + offset, &deaths, 4);	offset += 4;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}
