#include "pch.h"
#include "Room.h"
#include "GameSession.h"

I3_CLASS_INSTANCE(Room);

Room::Room()
	: m_bIsCreate(false)
	, m_i32ChannelNum(-1)
	, m_i32RoomIdx(-1)
	, m_ui32StageId(0)
	, m_ui8RoomState(ROOM_STATE_READY)
	, m_i32MaxPlayers(SLOT_MAX_COUNT)
	, m_i32PlayerCount(0)
	, m_i32OwnerSlot(-1)
	, m_ui8TeamBalance(1)
{
	m_szTitle[0] = '\0';
	m_szPassword[0] = '\0';

	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		m_Slots[i].Reset();
		m_pSlotSession[i] = nullptr;
	}
}

Room::~Room()
{
}

bool Room::OnCreate(GameSession* pOwner, GameRoomCreateInfo* pInfo, int i32ChannelNum)
{
	if (!pOwner || !pInfo)
		return false;

	m_bIsCreate = true;
	m_i32ChannelNum = i32ChannelNum;
	m_ui8RoomState = ROOM_STATE_READY;
	m_i32PlayerCount = 0;
	m_i32OwnerSlot = -1;
	m_ui8TeamBalance = pInfo->ui8TeamBalance;

	strncpy_s(m_szTitle, pInfo->szTitle, _TRUNCATE);
	strncpy_s(m_szPassword, pInfo->szPassword, _TRUNCATE);
	m_ui32StageId = pInfo->ui32StageId;
	m_i32MaxPlayers = pInfo->ui8MaxPlayers;

	if (m_i32MaxPlayers > SLOT_MAX_COUNT)
		m_i32MaxPlayers = SLOT_MAX_COUNT;

	// Reset all slots
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		m_Slots[i].Reset();
		m_pSlotSession[i] = nullptr;

		// Close slots beyond max players
		if (i >= m_i32MaxPlayers)
			m_Slots[i].ui8State = SLOT_STATE_CLOSE;
	}

	// Owner joins automatically to slot 0 (RED team)
	int slot = OnJoin(pOwner, TEAM_RED);
	if (slot < 0)
	{
		m_bIsCreate = false;
		return false;
	}

	m_i32OwnerSlot = slot;

	printf("[Room] Created - Channel=%d, Title=%s, MaxPlayers=%d\n",
		m_i32ChannelNum, m_szTitle, m_i32MaxPlayers);

	return true;
}

void Room::OnDestroy()
{
	// Kick all remaining players
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		m_Slots[i].Reset();
		m_pSlotSession[i] = nullptr;
	}

	m_bIsCreate = false;
	m_i32PlayerCount = 0;
	m_i32OwnerSlot = -1;
	m_ui8RoomState = ROOM_STATE_READY;

	m_szTitle[0] = '\0';
	m_szPassword[0] = '\0';
}

int Room::OnJoin(GameSession* pSession, int i32Team)
{
	if (!pSession || !m_bIsCreate)
		return -1;

	if (m_i32PlayerCount >= m_i32MaxPlayers)
		return -1;

	// Auto-balance teams if team balance is enabled
	if (m_ui8TeamBalance)
	{
		int redCount = 0, blueCount = 0;
		for (int i = 0; i < SLOT_MAX_COUNT; i++)
		{
			if (m_pSlotSession[i])
			{
				if (m_Slots[i].ui8Team == TEAM_RED) redCount++;
				else blueCount++;
			}
		}
		i32Team = (redCount <= blueCount) ? TEAM_RED : TEAM_BLUE;
	}

	int slot = FindEmptySlot(i32Team);
	if (slot < 0)
		return -1;

	m_Slots[slot].ui8State = SLOT_STATE_NORMAL;
	m_Slots[slot].ui8Team = (uint8_t)i32Team;
	m_Slots[slot].i32SessionIdx = pSession->GetIndex();
	m_pSlotSession[slot] = pSession;
	m_i32PlayerCount++;

	return slot;
}

void Room::OnLeave(GameSession* pSession)
{
	if (!pSession)
		return;

	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i] == pSession)
		{
			m_Slots[i].Reset();
			m_pSlotSession[i] = nullptr;
			m_i32PlayerCount--;

			// If the owner left, assign new owner
			if (i == m_i32OwnerSlot)
				UpdateOwner();

			break;
		}
	}

	// Destroy room if empty
	if (m_i32PlayerCount <= 0)
		OnDestroy();
}

void Room::OnReady(GameSession* pSession, bool bReady)
{
	if (!pSession)
		return;

	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i] == pSession)
		{
			m_Slots[i].ui8State = bReady ? SLOT_STATE_READY : SLOT_STATE_NORMAL;
			break;
		}
	}
}

bool Room::OnStartBattle()
{
	if (m_ui8RoomState != ROOM_STATE_READY)
		return false;

	if (m_i32PlayerCount < 1)
		return false;

	// Check all non-owner players are ready
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i] && i != m_i32OwnerSlot)
		{
			if (m_Slots[i].ui8State != SLOT_STATE_READY)
				return false;
		}
	}

	m_ui8RoomState = ROOM_STATE_LOADING;

	// Set all slots to loading state
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i])
			m_Slots[i].ui8State = SLOT_STATE_LOAD;
	}

	printf("[Room] Battle started - Channel=%d, RoomIdx=%d\n", m_i32ChannelNum, m_i32RoomIdx);
	return true;
}

void Room::OnEndBattle()
{
	m_ui8RoomState = ROOM_STATE_READY;

	// Reset all occupied slots to NORMAL state
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i])
			m_Slots[i].ui8State = SLOT_STATE_NORMAL;
	}

	printf("[Room] Battle ended - Channel=%d, RoomIdx=%d\n", m_i32ChannelNum, m_i32RoomIdx);
}

bool Room::IsSlotEmpty(int i32Slot) const
{
	if (i32Slot < 0 || i32Slot >= SLOT_MAX_COUNT)
		return false;

	return (m_pSlotSession[i32Slot] == nullptr);
}

GameSession* Room::GetSlotSession(int i32Slot) const
{
	if (i32Slot < 0 || i32Slot >= SLOT_MAX_COUNT)
		return nullptr;

	return m_pSlotSession[i32Slot];
}

void Room::SendToAll(i3NetworkPacket* pPacket)
{
	if (!pPacket)
		return;

	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i])
			m_pSlotSession[i]->SendMessage(pPacket);
	}
}

void Room::SendToTeam(int i32Team, i3NetworkPacket* pPacket)
{
	if (!pPacket)
		return;

	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i] && m_Slots[i].ui8Team == (uint8_t)i32Team)
			m_pSlotSession[i]->SendMessage(pPacket);
	}
}

void Room::FillRoomInfoBasic(char* pBuffer, int* pSize) const
{
	// Simplified room info for lobby list
	// In production this would serialize ROOM_INFO_BASIC from Common
	if (!pBuffer || !pSize)
		return;

	int offset = 0;

	memcpy(pBuffer + offset, &m_i32RoomIdx, sizeof(int));		offset += sizeof(int);
	memcpy(pBuffer + offset, m_szTitle, 64);					offset += 64;
	memcpy(pBuffer + offset, &m_ui32StageId, sizeof(uint32_t));	offset += sizeof(uint32_t);
	memcpy(pBuffer + offset, &m_ui8RoomState, sizeof(uint8_t));	offset += sizeof(uint8_t);

	uint8_t personNow = (uint8_t)m_i32PlayerCount;
	uint8_t personMax = (uint8_t)m_i32MaxPlayers;
	memcpy(pBuffer + offset, &personNow, sizeof(uint8_t));		offset += sizeof(uint8_t);
	memcpy(pBuffer + offset, &personMax, sizeof(uint8_t));		offset += sizeof(uint8_t);

	bool hasPassword = (m_szPassword[0] != '\0');
	uint8_t flags = hasPassword ? 0x04 : 0x00;	// ROOM_INFO_FLAG_PRIVATE_MODE
	memcpy(pBuffer + offset, &flags, sizeof(uint8_t));			offset += sizeof(uint8_t);

	*pSize = offset;
}

int Room::FindEmptySlot(int i32Team) const
{
	// Team-based slot assignment:
	// RED team: even slots (0,2,4,6,8,10,12,14)
	// BLUE team: odd slots (1,3,5,7,9,11,13,15)
	int start = (i32Team == TEAM_BLUE) ? 1 : 0;

	for (int i = start; i < SLOT_MAX_COUNT; i += 2)
	{
		if (m_Slots[i].ui8State == SLOT_STATE_EMPTY && m_pSlotSession[i] == nullptr)
			return i;
	}

	// Fallback: try any empty slot
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_Slots[i].ui8State == SLOT_STATE_EMPTY && m_pSlotSession[i] == nullptr)
			return i;
	}

	return -1;
}

void Room::UpdateOwner()
{
	m_i32OwnerSlot = -1;

	// Find first occupied slot as new owner
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i])
		{
			m_i32OwnerSlot = i;
			break;
		}
	}
}
