#include "pch.h"
#include "BattleRoomManager.h"
#include "BattleRoom.h"

I3_CLASS_INSTANCE(BattleRoomManager);

BattleRoomManager* g_pBattleRoomManager = nullptr;

BattleRoomManager::BattleRoomManager()
	: m_pRooms(nullptr)
	, m_i32ActiveCount(0)
	, m_ui16NextUdpPort(BATTLE_UDP_BASE_PORT)
{
}

BattleRoomManager::~BattleRoomManager()
{
	OnDestroy();
}

void BattleRoomManager::OnCreate()
{
	m_pRooms = new BattleRoom[MAX_BATTLE_ROOMS];
	m_i32ActiveCount = 0;
	m_ui16NextUdpPort = BATTLE_UDP_BASE_PORT;

	printf("[BattleRoomManager] Created with %d room slots\n", MAX_BATTLE_ROOMS);
}

void BattleRoomManager::OnUpdate()
{
	for (int i = 0; i < MAX_BATTLE_ROOMS; i++)
	{
		if (!m_pRooms[i].IsActive())
			continue;

		// Update room (check timers, member timeouts)
		m_pRooms[i].Update();

		// Check if room should be destroyed
		if (m_pRooms[i].ShouldDestroy())
		{
			printf("[BattleRoomManager] Destroying expired room %d\n", i);
			DestroyRoom(i);
		}
	}
}

void BattleRoomManager::OnDestroy()
{
	if (m_pRooms)
	{
		delete[] m_pRooms;
		m_pRooms = nullptr;
	}
	m_i32ActiveCount = 0;
}

int BattleRoomManager::CreateRoom(IS_BATTLE_CREATE_REQ* pReq, int i32SessionIdx)
{
	if (!pReq || !m_pRooms)
		return -1;

	int idx = FindFreeRoomSlot();
	if (idx < 0)
	{
		printf("[BattleRoomManager] No free room slots available\n");
		return -1;
	}

	uint16_t udpPort = AllocateUdpPort();

	if (!m_pRooms[idx].Create(pReq, idx, udpPort, i32SessionIdx))
	{
		printf("[BattleRoomManager] Failed to create room at index %d\n", idx);
		return -1;
	}

	m_i32ActiveCount++;

	printf("[BattleRoomManager] Room created at index %d - Active: %d/%d\n",
		idx, m_i32ActiveCount, MAX_BATTLE_ROOMS);

	return idx;
}

BattleRoom* BattleRoomManager::GetRoom(int i32Idx)
{
	if (i32Idx < 0 || i32Idx >= MAX_BATTLE_ROOMS || !m_pRooms)
		return nullptr;

	return &m_pRooms[i32Idx];
}

void BattleRoomManager::DestroyRoom(int i32Idx)
{
	if (i32Idx < 0 || i32Idx >= MAX_BATTLE_ROOMS || !m_pRooms)
		return;

	if (!m_pRooms[i32Idx].IsActive())
		return;

	m_pRooms[i32Idx].Reset();
	m_i32ActiveCount--;

	if (m_i32ActiveCount < 0)
		m_i32ActiveCount = 0;
}

uint16_t BattleRoomManager::AllocateUdpPort()
{
	// Simple port allocation: incrementar y wrap
	uint16_t port = m_ui16NextUdpPort;
	m_ui16NextUdpPort++;

	// Wrap around si llegamos a un puerto alto
	if (m_ui16NextUdpPort >= BATTLE_UDP_BASE_PORT + MAX_BATTLE_ROOMS)
		m_ui16NextUdpPort = BATTLE_UDP_BASE_PORT;

	return port;
}

int BattleRoomManager::FindFreeRoomSlot() const
{
	if (!m_pRooms)
		return -1;

	for (int i = 0; i < MAX_BATTLE_ROOMS; i++)
	{
		if (!m_pRooms[i].IsActive())
			return i;
	}
	return -1;
}
