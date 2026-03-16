#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "GameContextMain.h"
#include "Room.h"
#include "RoomManager.h"

// ============================================================================
// Battle Flow (7D)
// ============================================================================

void GameSession::OnBattleReadyBattleReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom)
		return;

	if (m_i32SlotIdx == m_pRoom->GetOwnerSlot())
	{
		// Owner starts the battle
		bool started = m_pRoom->OnStartBattle();
		if (!started)
		{
			SendSimpleAck(PROTOCOL_BATTLE_READYBATTLE_ACK, 1);
			return;
		}

		// Transition all players to BATTLE task
		for (int i = 0; i < SLOT_MAX_COUNT; i++)
		{
			GameSession* pSlot = m_pRoom->GetSlotSession(i);
			if (pSlot)
				pSlot->SetTask(GAME_TASK_BATTLE);
		}

		// Send READYBATTLE_ACK to all with room/slot info
		i3NetworkPacket packet;
		char buffer[1024];
		int offset = 0;

		uint16_t sz = 0;
		uint16_t proto = PROTOCOL_BATTLE_READYBATTLE_ACK;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

		int32_t result = 0;
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

		uint8_t gameMode = m_pRoom->GetGameMode();
		uint8_t mapIndex = m_pRoom->GetMapIndex();
		uint32_t stageId = m_pRoom->GetStageId();
		memcpy(buffer + offset, &gameMode, 1);		offset += 1;
		memcpy(buffer + offset, &mapIndex, 1);		offset += 1;
		memcpy(buffer + offset, &stageId, 4);		offset += 4;

		for (int i = 0; i < SLOT_MAX_COUNT; i++)
		{
			const GameSlotInfo& si = m_pRoom->GetSlotInfo(i);
			memcpy(buffer + offset, &si.ui8State, 1);	offset += 1;
			memcpy(buffer + offset, &si.ui8Team, 1);	offset += 1;
		}

		sz = (uint16_t)offset;
		memcpy(buffer, &sz, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		m_pRoom->SendToAll(&packet);

		printf("[GameSession] Battle started - Room=%d, Mode=%d, Map=%d\n",
			m_i32RoomIdx, gameMode, mapIndex);
	}
	else
	{
		// Normal player toggles ready state
		const GameSlotInfo& mySlot = m_pRoom->GetSlotInfo(m_i32SlotIdx);
		bool bReady = (mySlot.ui8State != SLOT_STATE_READY);

		m_pRoom->OnReady(this, bReady);

		// Broadcast slot state change
		i3NetworkPacket packet;
		char buffer[16];
		int offset = 0;

		uint16_t sz = 0;
		uint16_t proto = PROTOCOL_BATTLE_SLOT_STATE_CHANGE_ACK;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

		uint8_t slotIdx = (uint8_t)m_i32SlotIdx;
		uint8_t newState = bReady ? SLOT_STATE_READY : SLOT_STATE_NORMAL;
		memcpy(buffer + offset, &slotIdx, 1);				offset += 1;
		memcpy(buffer + offset, &newState, 1);				offset += 1;

		sz = (uint16_t)offset;
		memcpy(buffer, &sz, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		m_pRoom->SendToAll(&packet);
	}
}

void GameSession::OnBattlePreStartBattleReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	// Client finished loading, ready for UDP
	m_pRoom->SetRoomState(ROOM_STATE_PRE_BATTLE);

	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BATTLE_PRESTARTBATTLE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	// Use BattleServer UDP IP/port stored in Room from OnBattleCreateAck
	uint32_t udpIP = m_pRoom->GetBattleUdpIPAddr();
	uint16_t udpPort = m_pRoom->GetBattleUdpPort();
	if (udpIP == 0)
	{
		udpIP = 0x0100007F;		// Fallback: 127.0.0.1
		udpPort = 41000;
	}
	memcpy(buffer + offset, &udpIP, 4);		offset += 4;
	memcpy(buffer + offset, &udpPort, 2);	offset += 2;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnBattleStartBattleReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	m_pRoom->SetRoomState(ROOM_STATE_BATTLE);

	// Send battle time info to all
	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BATTLE_STARTBATTLE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	const GameRoomScore& score = m_pRoom->GetScore();
	uint16_t maxTime = score.ui16MaxTime;
	int32_t nowRound = score.i32NowRound;
	int32_t maxRound = score.i32MaxRound;
	memcpy(buffer + offset, &maxTime, 2);		offset += 2;
	memcpy(buffer + offset, &nowRound, 4);		offset += 4;
	memcpy(buffer + offset, &maxRound, 4);		offset += 4;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	m_pRoom->SendToAll(&packet);
}

void GameSession::OnBattleGiveUpBattleReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom)
		return;

	m_pRoom->OnLeave(this);

	m_i32RoomIdx = -1;
	m_i32SlotIdx = -1;
	m_pRoom = nullptr;
	m_eMainTask = GAME_TASK_LOBBY;

	SendSimpleAck(PROTOCOL_BATTLE_GIVEUPBATTLE_ACK, 0);
}

void GameSession::OnBattleDeathReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	// Parse: killerSlot(1) + weaponId(4) + hitPart(1)
	if (i32Size < 6)
		return;

	uint8_t killerSlot = *(uint8_t*)pData;
	uint32_t weaponId = *(uint32_t*)(pData + 1);
	uint8_t hitPart = *(uint8_t*)(pData + 5);

	// Add kill to killer's team
	if (killerSlot != m_i32SlotIdx && killerSlot < SLOT_MAX_COUNT)
	{
		const GameSlotInfo& killerInfo = m_pRoom->GetSlotInfo(killerSlot);
		if (killerInfo.ui8State != SLOT_STATE_EMPTY)
			m_pRoom->OnAddKill(killerInfo.ui8Team);
	}

	// Broadcast death
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BATTLE_DEATH_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	uint8_t deadSlot = (uint8_t)m_i32SlotIdx;
	memcpy(buffer + offset, &deadSlot, 1);		offset += 1;
	memcpy(buffer + offset, &killerSlot, 1);	offset += 1;
	memcpy(buffer + offset, &weaponId, 4);		offset += 4;
	memcpy(buffer + offset, &hitPart, 1);		offset += 1;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	m_pRoom->SendToAll(&packet);

	// Check kill limit for deathmatch
	if (m_pRoom->CheckMatchEnd())
	{
		const GameRoomScore& score = m_pRoom->GetScore();
		int winner = (score.i32RedScore > score.i32BlueScore) ? TEAM_RED : TEAM_BLUE;
		SendBattleEndToAll(winner);
	}
}

void GameSession::OnBattleRespawnReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	i3NetworkPacket packet;
	char buffer[16];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BATTLE_RESPAWN_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	uint8_t slotIdx = (uint8_t)m_i32SlotIdx;
	memcpy(buffer + offset, &slotIdx, 1);				offset += 1;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	m_pRoom->SendToAll(&packet);
}

// ============================================================================
// Mission Mode Round Management
// ============================================================================

void GameSession::OnBattleMissionRoundPreStartReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BATTLE_MISSION_ROUND_PRE_START_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	const GameRoomScore& score = m_pRoom->GetScore();
	int32_t nowRound = score.i32NowRound;
	memcpy(buffer + offset, &nowRound, 4);	offset += 4;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	m_pRoom->SendToAll(&packet);
}

void GameSession::OnBattleMissionRoundStartReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BATTLE_MISSION_ROUND_START_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	const GameRoomScore& score = m_pRoom->GetScore();
	int32_t nowRound = score.i32NowRound;
	uint16_t maxTime = score.ui16MaxTime;
	memcpy(buffer + offset, &nowRound, 4);		offset += 4;
	memcpy(buffer + offset, &maxTime, 2);		offset += 2;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	m_pRoom->SendToAll(&packet);
}

void GameSession::OnBattleMissionRoundEndReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	int winnerTeam = TEAM_RED;
	if (i32Size >= 1)
		winnerTeam = *(uint8_t*)pData;

	m_pRoom->OnRoundEnd(winnerTeam);

	// Check if match is over
	if (m_pRoom->CheckMatchEnd())
	{
		const GameRoomScore& score = m_pRoom->GetScore();
		int finalWinner = (score.i32RedScore > score.i32BlueScore) ? TEAM_RED : TEAM_BLUE;
		SendBattleEndToAll(finalWinner);
		return;
	}

	// Send round end with scores
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BATTLE_MISSION_ROUND_END_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	uint8_t winner = (uint8_t)winnerTeam;
	const GameRoomScore& score = m_pRoom->GetScore();
	int32_t redScore = score.i32RedScore;
	int32_t blueScore = score.i32BlueScore;
	int32_t nowRound = score.i32NowRound;

	memcpy(buffer + offset, &winner, 1);		offset += 1;
	memcpy(buffer + offset, &redScore, 4);		offset += 4;
	memcpy(buffer + offset, &blueScore, 4);		offset += 4;
	memcpy(buffer + offset, &nowRound, 4);		offset += 4;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	m_pRoom->SendToAll(&packet);
}

// ============================================================================
// Battle End Helper
// ============================================================================

void GameSession::SendBattleEndToAll(int i32WinnerTeam)
{
	if (!m_pRoom)
		return;

	m_pRoom->OnEndBattle();

	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BATTLE_ENDBATTLE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	uint8_t winner = (uint8_t)i32WinnerTeam;
	const GameRoomScore& score = m_pRoom->GetScore();
	int32_t redScore = score.i32RedScore;
	int32_t blueScore = score.i32BlueScore;

	memcpy(buffer + offset, &winner, 1);		offset += 1;
	memcpy(buffer + offset, &redScore, 4);		offset += 4;
	memcpy(buffer + offset, &blueScore, 4);		offset += 4;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	m_pRoom->SendToAll(&packet);

	// Return all players to ready room
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		GameSession* pSlot = m_pRoom->GetSlotSession(i);
		if (pSlot)
			pSlot->SetTask(GAME_TASK_READY_ROOM);
	}

	printf("[GameSession] Battle ended - Room=%d, Winner=%s, Red=%d, Blue=%d\n",
		m_i32RoomIdx,
		(i32WinnerTeam == TEAM_RED) ? "RED" : "BLUE",
		redScore, blueScore);
}
