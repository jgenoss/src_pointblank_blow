#include "pch.h"
#include "BattleRoom.h"
#include "CollisionSystem.h"
#include "GameObjectManager.h"
#include "HitValidator.h"
#include "RespawnManager.h"
#include "MapManager.h"
#include "ConfigXML.h"

I3_CLASS_INSTANCE(BattleRoom);

BattleRoom::BattleRoom()
{
	Reset();
}

BattleRoom::~BattleRoom()
{
}

void BattleRoom::Reset()
{
	m_eState				= BATTLE_ROOM_INVALID;
	m_i32RoomIdx			= -1;
	m_i32GameServerRoomIdx	= -1;
	m_i32ChannelNum			= -1;
	m_ui8GameMode			= 0;
	m_ui8MapIndex			= 0;
	m_ui8MaxPlayers			= 0;
	m_i32PlayerCount		= 0;
	m_ui16UdpPort			= 0;
	m_dwCreateTime			= 0;
	m_dwBattleStartTime		= 0;
	m_dwLastActivityTime	= 0;
	m_i32RedScore			= 0;
	m_i32BlueScore			= 0;
	m_i32OwnerSessionIdx	= -1;

	for (int i = 0; i < BATTLE_SLOT_MAX; i++)
		m_Members[i].Init();

	// Cleanup physics/objects
	if (m_pCollision)
	{
		m_pCollision->Shutdown();
		delete m_pCollision;
		m_pCollision = nullptr;
	}
	if (m_pObjectManager)
	{
		m_pObjectManager->Shutdown();
		delete m_pObjectManager;
		m_pObjectManager = nullptr;
	}
	if (m_pHitValidator)
	{
		delete m_pHitValidator;
		m_pHitValidator = nullptr;
	}
	if (m_pRespawnManager)
	{
		delete m_pRespawnManager;
		m_pRespawnManager = nullptr;
	}
	m_pMapData = nullptr;	// Not owned
}

bool BattleRoom::Create(IS_BATTLE_CREATE_REQ* pReq, int i32RoomIdx, uint16_t ui16UdpPort, int i32SessionIdx)
{
	if (!pReq)
		return false;

	Reset();

	m_eState				= BATTLE_ROOM_READY;
	m_i32RoomIdx			= i32RoomIdx;
	m_i32GameServerRoomIdx	= pReq->i32RoomIdx;
	m_i32ChannelNum			= pReq->i32ChannelNum;
	m_ui8GameMode			= pReq->ui8GameMode;
	m_ui8MapIndex			= pReq->ui8MapIndex;
	m_ui8MaxPlayers			= pReq->ui8MaxPlayers;
	m_i32PlayerCount		= pReq->i32PlayerCount;
	m_ui16UdpPort			= ui16UdpPort;
	m_i32OwnerSessionIdx	= i32SessionIdx;

	DWORD dwNow = GetTickCount();
	m_dwCreateTime			= dwNow;
	m_dwLastActivityTime	= dwNow;

	// Initialize physics and game objects from map data
	if (g_pMapManager)
	{
		m_pMapData = g_pMapManager->GetMapData(m_ui8MapIndex);
		if (m_pMapData && m_pMapData->IsLoaded())
		{
			// Initialize collision system
			if (m_pMapData->GetCollisionTriangleCount() > 0)
			{
				m_pCollision = new CollisionSystem();
				m_pCollision->Initialize(m_pMapData->GetCollisionTriangles(),
										 m_pMapData->GetCollisionTriangleCount());
			}

			// Initialize game objects
			if (m_pMapData->GetGameObjectCount() > 0)
			{
				m_pObjectManager = new GameObjectManager();
				m_pObjectManager->Initialize(m_pMapData);
			}

			// Initialize hit validator
			m_pHitValidator = new HitValidator();
			m_pHitValidator->SetCollisionSystem(m_pCollision);
			if (g_pConfigXML)
				m_pHitValidator->SetConfigXML(g_pConfigXML);

			// Initialize respawn manager
			m_pRespawnManager = new RespawnManager();

			printf("[BattleRoom] Room %d physics: Collision=%s, Objects=%d, Respawns=%d\n",
				i32RoomIdx,
				m_pCollision ? "YES" : "NO",
				m_pMapData->GetGameObjectCount(),
				m_pMapData->GetRespawnObjectCount());
		}
	}

	printf("[BattleRoom] Created room %d - GameMode=%d, Map=%d, MaxPlayers=%d, UdpPort=%d\n",
		i32RoomIdx, m_ui8GameMode, m_ui8MapIndex, m_ui8MaxPlayers, m_ui16UdpPort);

	return true;
}

bool BattleRoom::AddMember(int64_t i64UID, uint32_t ui32IP, uint16_t ui16Port, int i32Team, int i32Slot)
{
	if (i32Slot < 0 || i32Slot >= BATTLE_SLOT_MAX)
		return false;

	if (m_Members[i32Slot].IsMember())
		return false;

	if (!m_Members[i32Slot].Join(i64UID, ui32IP, ui16Port, i32Team, i32Slot))
		return false;

	m_dwLastActivityTime = GetTickCount();

	printf("[BattleRoom] Member added to room %d - UID=%lld, Slot=%d, Team=%d\n",
		m_i32RoomIdx, i64UID, i32Slot, i32Team);

	return true;
}

void BattleRoom::RemoveMember(int i32Slot)
{
	if (i32Slot < 0 || i32Slot >= BATTLE_SLOT_MAX)
		return;

	if (!m_Members[i32Slot].IsMember())
		return;

	printf("[BattleRoom] Member removed from room %d - UID=%lld, Slot=%d\n",
		m_i32RoomIdx, m_Members[i32Slot].GetUID(), i32Slot);

	m_Members[i32Slot].Leave();
	m_dwLastActivityTime = GetTickCount();

	// Si no quedan miembros activos durante la batalla, terminar
	if (m_eState == BATTLE_ROOM_USE && GetActiveMemberCount() == 0)
	{
		EndBattle(-1);	// Draw
	}
}

int BattleRoom::FindFreeSlot() const
{
	for (int i = 0; i < BATTLE_SLOT_MAX; i++)
	{
		if (!m_Members[i].IsMember())
			return i;
	}
	return -1;
}

int BattleRoom::GetActiveMemberCount() const
{
	int count = 0;
	for (int i = 0; i < BATTLE_SLOT_MAX; i++)
	{
		if (m_Members[i].IsMember())
			count++;
	}
	return count;
}

BattleMember* BattleRoom::GetMember(int i32Slot)
{
	if (i32Slot < 0 || i32Slot >= BATTLE_SLOT_MAX)
		return nullptr;

	return &m_Members[i32Slot];
}

BattleMember* BattleRoom::FindMemberByUID(int64_t i64UID)
{
	for (int i = 0; i < BATTLE_SLOT_MAX; i++)
	{
		if (m_Members[i].IsMember() && m_Members[i].GetUID() == i64UID)
			return &m_Members[i];
	}
	return nullptr;
}

BattleMember* BattleRoom::FindMemberByAddress(uint32_t ui32IP, uint16_t ui16Port)
{
	for (int i = 0; i < BATTLE_SLOT_MAX; i++)
	{
		if (m_Members[i].IsMember() && m_Members[i].IsSameAddress(ui32IP, ui16Port))
			return &m_Members[i];
	}
	return nullptr;
}

bool BattleRoom::ShouldDestroy() const
{
	if (m_eState == BATTLE_ROOM_INVALID)
		return false;

	DWORD dwNow = GetTickCount();
	DWORD dwElapsed = (dwNow - m_dwLastActivityTime) / 1000;

	// Destroy si no hay actividad por BATTLE_DESTROY_TIMEOUT segundos
	if (dwElapsed >= BATTLE_DESTROY_TIMEOUT)
		return true;

	// Destroy si esta en ENDING por mas de 10 segundos (tiempo suficiente para enviar resultados)
	if (m_eState == BATTLE_ROOM_ENDING)
	{
		DWORD dwEndElapsed = (dwNow - m_dwLastActivityTime) / 1000;
		if (dwEndElapsed >= 10)
			return true;
	}

	return false;
}

void BattleRoom::StartBattle()
{
	if (m_eState != BATTLE_ROOM_READY)
		return;

	m_eState = BATTLE_ROOM_USE;
	m_dwBattleStartTime = GetTickCount();
	m_dwLastActivityTime = GetTickCount();

	// Set all joined members to PLAYING
	for (int i = 0; i < BATTLE_SLOT_MAX; i++)
	{
		if (m_Members[i].IsMember() && m_Members[i].GetState() == BATTLE_MEMBER_JOINED)
		{
			m_Members[i].SetState(BATTLE_MEMBER_PLAYING);
			m_Members[i].SetAlive(true);
		}
	}

	printf("[BattleRoom] Battle started in room %d\n", m_i32RoomIdx);
}

void BattleRoom::EndBattle(int i32WinnerTeam)
{
	if (m_eState == BATTLE_ROOM_ENDING || m_eState == BATTLE_ROOM_INVALID)
		return;

	m_eState = BATTLE_ROOM_ENDING;
	m_dwLastActivityTime = GetTickCount();

	printf("[BattleRoom] Battle ended in room %d - Winner=%s, Score: RED %d - BLUE %d\n",
		m_i32RoomIdx,
		i32WinnerTeam == BATTLE_TEAM_RED ? "RED" :
		i32WinnerTeam == BATTLE_TEAM_BLUE ? "BLUE" : "DRAW",
		m_i32RedScore, m_i32BlueScore);
}

void BattleRoom::Update()
{
	if (m_eState != BATTLE_ROOM_USE)
		return;

	DWORD dwNow = GetTickCount();

	// Check round time limit
	DWORD dwElapsed = (dwNow - m_dwBattleStartTime) / 1000;
	if (dwElapsed >= BATTLE_DEFAULT_ROUND_TIME)
	{
		// Time's up - team with more score wins
		int winnerTeam = -1;
		if (m_i32RedScore > m_i32BlueScore)
			winnerTeam = BATTLE_TEAM_RED;
		else if (m_i32BlueScore > m_i32RedScore)
			winnerTeam = BATTLE_TEAM_BLUE;

		EndBattle(winnerTeam);
		return;
	}

	// Check member timeouts
	for (int i = 0; i < BATTLE_SLOT_MAX; i++)
	{
		if (!m_Members[i].IsMember())
			continue;

		DWORD dwMemberElapsed = (dwNow - m_Members[i].GetLastPacketTime()) / 1000;
		if (dwMemberElapsed >= BATTLE_MEMBER_TIMEOUT)
		{
			printf("[BattleRoom] Member timeout in room %d - Slot=%d, UID=%lld\n",
				m_i32RoomIdx, i, m_Members[i].GetUID());
			RemoveMember(i);
		}
	}

	// Update game objects (weapon box respawns, etc.)
	if (m_pObjectManager)
		m_pObjectManager->Update(dwNow);
}

void BattleRoom::FillBattleResult(BattleResult* pResult) const
{
	if (!pResult)
		return;

	pResult->Reset();
	pResult->i32RoomIdx		= m_i32GameServerRoomIdx;
	pResult->i32ChannelNum	= m_i32ChannelNum;
	pResult->i32RedScore	= m_i32RedScore;
	pResult->i32BlueScore	= m_i32BlueScore;

	// Determine winner
	if (m_i32RedScore > m_i32BlueScore)
		pResult->i32WinnerTeam = BATTLE_TEAM_RED;
	else if (m_i32BlueScore > m_i32RedScore)
		pResult->i32WinnerTeam = BATTLE_TEAM_BLUE;
	else
		pResult->i32WinnerTeam = -1;	// Draw

	// Fill player results
	int playerIdx = 0;
	for (int i = 0; i < BATTLE_SLOT_MAX && playerIdx < BATTLE_MAX_PLAYER_RESULTS; i++)
	{
		if (m_Members[i].GetUID() != 0)	// Was a member at some point
		{
			m_Members[i].FillPlayerResult(&pResult->Players[playerIdx]);
			playerIdx++;
		}
	}
	pResult->i32PlayerCount = playerIdx;
}
