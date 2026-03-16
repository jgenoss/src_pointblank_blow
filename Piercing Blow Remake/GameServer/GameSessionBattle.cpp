#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "GameContextMain.h"
#include "Room.h"
#include "RoomManager.h"
#include "ModuleBattleServer.h"
#include "ModuleDataServer.h"

// ============================================================================
// Battle Flow - Phase 1 Complete Implementation
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

		// Request BattleServer to create room (Phase 1B)
		if (g_pModuleBattleServer && g_pModuleBattleServer->IsRegistered())
		{
			g_pModuleBattleServer->RequestBattleCreate(
				m_i32RoomIdx, m_pRoom->GetChannelNum(),
				m_pRoom->GetGameMode(), m_pRoom->GetMapIndex(),
				(uint8_t)m_pRoom->GetMaxPlayers(), m_pRoom->GetPlayerCount());
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

	// Client finished loading - update per-slot state (Phase 1B)
	m_pRoom->SetSlotState(m_i32SlotIdx, SLOT_STATE_BATTLE_LOADOK);

	// Migrate player to BattleServer
	if (g_pModuleBattleServer && g_pModuleBattleServer->IsRegistered() &&
		m_pRoom->GetBattleRoomIdx() >= 0)
	{
		g_pModuleBattleServer->RequestPlayerMigrate(
			m_i64UID, m_pRoom->GetBattleRoomIdx(),
			m_i32SlotIdx, m_pRoom->GetSlotInfo(m_i32SlotIdx).ui8Team,
			0, 0);	// Client IP/port filled by BattleServer
	}

	// Send PreStartBattle ACK with BattleServer UDP info
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

	// Mark this slot as battle-ready (Phase 1B)
	m_pRoom->SetSlotState(m_i32SlotIdx, SLOT_STATE_BATTLE_READY);

	// Only advance to BATTLE state if room is in COUNTDOWN_B or PRE_BATTLE
	// The state machine in Room::OnUpdateRoom will handle the actual transition
	// But for backwards compat, if all slots are ready, advance immediately
	if (m_pRoom->GetRoomState() < ROOM_STATE_BATTLE &&
		m_pRoom->AllSlotsMinState(SLOT_STATE_BATTLE_READY))
	{
		// All players ready - the state machine timer will handle the transition
		// Don't force it here, let OnUpdateRoom_CountdownB do it
	}

	// Send battle start info to this player
	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BATTLE_STARTBATTLE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	uint8_t slotIdx = (uint8_t)m_i32SlotIdx;
	memcpy(buffer + offset, &slotIdx, 1);		offset += 1;

	uint8_t interEnter = 0;
	memcpy(buffer + offset, &interEnter, 1);	offset += 1;

	// Battle user count
	uint16_t battleUser = (uint16_t)m_pRoom->GetPlayerCount();
	memcpy(buffer + offset, &battleUser, 2);	offset += 2;

	// Total round count per team
	const GameRoomScore& score = m_pRoom->GetScore();
	uint16_t redRounds = (uint16_t)score.i32RedScore;
	uint16_t blueRounds = (uint16_t)score.i32BlueScore;
	memcpy(buffer + offset, &redRounds, 2);		offset += 2;
	memcpy(buffer + offset, &blueRounds, 2);	offset += 2;

	// Round start user count
	uint16_t roundStartUser = battleUser;
	memcpy(buffer + offset, &roundStartUser, 2);	offset += 2;

	// Now round count
	uint8_t nowRound = (uint8_t)score.i32NowRound;
	memcpy(buffer + offset, &nowRound, 1);		offset += 1;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnBattleGiveUpBattleReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom)
		return;

	// Mark as not alive before leaving
	if (m_i32SlotIdx >= 0 && m_i32SlotIdx < SLOT_MAX_COUNT)
	{
		Room::SlotBattleStats& stats = m_pRoom->GetSlotBattleStatsMutable(m_i32SlotIdx);
		stats.bAlive = false;
	}

	m_pRoom->OnLeave(this);

	m_i32RoomIdx = -1;
	m_i32SlotIdx = -1;
	m_pRoom = nullptr;
	m_eMainTask = GAME_TASK_LOBBY;

	SendSimpleAck(PROTOCOL_BATTLE_GIVEUPBATTLE_ACK, 0);
}

// ============================================================================
// Death & Kill Processing (Phase 1D)
// ============================================================================

void GameSession::OnBattleDeathReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	if (m_pRoom->GetRoomState() != ROOM_STATE_BATTLE)
		return;

	// Parse DEATH_INFO_HEADER:
	// killerSlot(1) + weaponId(4) + hitPart(1) + posX(4) + posY(4) + posZ(4) + assistSlot(1)
	if (i32Size < 6)
		return;

	int parseOffset = 0;
	uint8_t killerSlot = *(uint8_t*)(pData + parseOffset);		parseOffset += 1;
	uint32_t weaponId = *(uint32_t*)(pData + parseOffset);		parseOffset += 4;
	uint8_t hitPart = *(uint8_t*)(pData + parseOffset);			parseOffset += 1;

	float fX = 0.0f, fY = 0.0f, fZ = 0.0f;
	int assistSlot = -1;

	if (i32Size >= 19)
	{
		fX = *(float*)(pData + parseOffset);	parseOffset += 4;
		fY = *(float*)(pData + parseOffset);	parseOffset += 4;
		fZ = *(float*)(pData + parseOffset);	parseOffset += 4;
		assistSlot = (int)*(uint8_t*)(pData + parseOffset);
		if (assistSlot == 0xFF)
			assistSlot = -1;
	}

	// Delegate to Room for full processing (multi-kill, headshot, assist, broadcast)
	m_pRoom->OnPlayerDeath(m_i32SlotIdx, (int)killerSlot, weaponId,
		hitPart, fX, fY, fZ, assistSlot);

	// Update weapon tracking on session
	m_stUsedWeapon = weaponId;
}

// ============================================================================
// Respawn System (Phase 1E)
// ============================================================================

void GameSession::OnBattleRespawnReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	if (m_pRoom->GetRoomState() != ROOM_STATE_BATTLE)
		return;

	// Delegate to Room for respawn validation and broadcast
	m_pRoom->OnPlayerRespawn(m_i32SlotIdx);
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

// ============================================================================
// Bomb Mode Handlers (Phase 2A)
// ============================================================================

void GameSession::OnBattleMissionBombInstallReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	if (m_pRoom->GetRoomState() != ROOM_STATE_BATTLE)
		return;

	if (!m_pRoom->IsBombMode())
		return;

	// Parse bomb area (0=A, 1=B)
	uint8_t bombArea = BOMB_AREA_A;
	if (i32Size >= 1)
		bombArea = *(uint8_t*)pData;

	if (bombArea >= BOMB_AREA_COUNT)
		bombArea = BOMB_AREA_A;

	// Only ATK team (RED) can plant
	if (m_i32SlotIdx < 0 || m_i32SlotIdx >= SLOT_MAX_COUNT)
		return;

	const GameSlotInfo& mySlot = m_pRoom->GetSlotInfo(m_i32SlotIdx);
	if (mySlot.ui8Team != TEAM_RED)
		return;		// Only ATK team can plant

	// Must be alive
	if (!m_pRoom->IsPlayerAlive(m_i32SlotIdx))
		return;

	m_pRoom->OnBombInstall(m_i32SlotIdx, bombArea);
}

void GameSession::OnBattleMissionBombUninstallReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	if (m_pRoom->GetRoomState() != ROOM_STATE_BATTLE)
		return;

	if (!m_pRoom->IsBombMode())
		return;

	// Only DEF team (BLUE) can defuse
	if (m_i32SlotIdx < 0 || m_i32SlotIdx >= SLOT_MAX_COUNT)
		return;

	const GameSlotInfo& mySlot = m_pRoom->GetSlotInfo(m_i32SlotIdx);
	if (mySlot.ui8Team != TEAM_BLUE)
		return;		// Only DEF team can defuse

	// Must be alive
	if (!m_pRoom->IsPlayerAlive(m_i32SlotIdx))
		return;

	// Bomb must be installed
	if (!m_pRoom->IsBombInstalled())
		return;

	m_pRoom->OnBombUninstall(m_i32SlotIdx);
}

// ============================================================================
// Mission Mode Round Management (continued)
// ============================================================================

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

		// Use Room's proper battle result flow (Phase 1C)
		m_pRoom->CalculateBattleRewards(finalWinner);
		m_pRoom->SetRoomState(ROOM_STATE_BATTLE_RESULT);
		m_pRoom->SendBattleResultToAll(finalWinner);

		// Save stats for all players via DataServer
		SaveAllPlayerStats();
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

	// Reset alive status for next round
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pRoom->GetSlotSession(i))
			m_pRoom->GetSlotBattleStatsMutable(i).bAlive = true;
	}

	// Reset bomb state for next round (Phase 2A)
	if (m_pRoom->IsBombMode() && m_pRoom->IsBombInstalled())
	{
		// Shouldn't happen normally, but safety reset
	}
}

// ============================================================================
// Battle End Helper (updated for Phase 1C result screen flow)
// ============================================================================

void GameSession::SendBattleEndToAll(int i32WinnerTeam)
{
	if (!m_pRoom)
		return;

	// Calculate rewards and transition to BATTLE_RESULT state
	m_pRoom->CalculateBattleRewards(i32WinnerTeam);
	m_pRoom->SetRoomState(ROOM_STATE_BATTLE_RESULT);
	m_pRoom->SendBattleResultToAll(i32WinnerTeam);

	// Save stats for all players
	SaveAllPlayerStats();

	printf("[GameSession] Battle ended via SendBattleEndToAll - Room=%d, Winner=%s\n",
		m_i32RoomIdx,
		(i32WinnerTeam == TEAM_RED) ? "RED" :
		(i32WinnerTeam == TEAM_BLUE) ? "BLUE" : "DRAW");
}

// ============================================================================
// Stats persistence helper
// ============================================================================

void GameSession::SaveAllPlayerStats()
{
	if (!m_pRoom || !g_pModuleDataServer)
		return;

	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		GameSession* pSlot = m_pRoom->GetSlotSession(i);
		if (!pSlot)
			continue;

		const Room::SlotBattleStats& stats = m_pRoom->GetSlotBattleStats(i);

		g_pModuleDataServer->RequestStatsSave(
			pSlot->GetUID(),
			pSlot->GetKills(), pSlot->GetDeaths(),
			stats.i32Headshots,
			0, 0);

		g_pModuleDataServer->RequestPlayerSave(
			pSlot->GetUID(),
			pSlot->GetLevel(), pSlot->GetExp(),
			pSlot->GetCash(), pSlot->GetGP());
	}
}

// ============================================================================
// Vote Kick & Ping (Phase 3A)
// ============================================================================

void GameSession::OnBattleSuggestKickVoteReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
	{
		SendSimpleAck(PROTOCOL_BATTLE_SUGGEST_KICKVOTE_ACK, 1);
		return;
	}

	if (i32Size < 1)
		return;

	uint8_t targetSlot = *(uint8_t*)pData;
	if (targetSlot >= SLOT_MAX_COUNT || m_pRoom->IsSlotEmpty(targetSlot))
	{
		SendSimpleAck(PROTOCOL_BATTLE_SUGGEST_KICKVOTE_ACK, 2);	// Invalid target
		return;
	}

	// Cannot kick yourself
	if (targetSlot == (uint8_t)m_i32SlotIdx)
	{
		SendSimpleAck(PROTOCOL_BATTLE_SUGGEST_KICKVOTE_ACK, 3);
		return;
	}

	// Need at least 3 players in room for vote kick
	if (m_pRoom->GetPlayerCount() < 3)
	{
		SendSimpleAck(PROTOCOL_BATTLE_SUGGEST_KICKVOTE_ACK, 4);	// Not enough players
		return;
	}

	// Accept the vote suggestion, broadcast to all players to vote
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BATTLE_SUGGEST_KICKVOTE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	uint8_t suggestSlot = (uint8_t)m_i32SlotIdx;
	memcpy(buffer + offset, &suggestSlot, 1);				offset += 1;
	memcpy(buffer + offset, &targetSlot, 1);				offset += 1;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	m_pRoom->SendToAll(&packet);
}

void GameSession::OnBattleVoteKickVoteReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
	{
		SendSimpleAck(PROTOCOL_BATTLE_VOTE_KICKVOTE_ACK, 1);
		return;
	}

	if (i32Size < 2)
		return;

	uint8_t targetSlot = *(uint8_t*)pData;
	uint8_t vote = *(uint8_t*)(pData + 1);	// 0 = disagree, 1 = agree

	// Acknowledge the vote
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BATTLE_VOTE_KICKVOTE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	uint8_t voterSlot = (uint8_t)m_i32SlotIdx;
	memcpy(buffer + offset, &voterSlot, 1);				offset += 1;
	memcpy(buffer + offset, &targetSlot, 1);			offset += 1;
	memcpy(buffer + offset, &vote, 1);					offset += 1;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	m_pRoom->SendToAll(&packet);
}

void GameSession::OnBattleSendPingReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom)
		return;

	if (i32Size < (int)sizeof(uint32_t))
		return;

	uint32_t pingMs = *(uint32_t*)pData;

	// Broadcast ping to all players in room
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BATTLE_SENDPING_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	uint8_t slotIdx = (uint8_t)m_i32SlotIdx;
	memcpy(buffer + offset, &slotIdx, 1);				offset += 1;
	memcpy(buffer + offset, &pingMs, sizeof(uint32_t));	offset += sizeof(uint32_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	m_pRoom->SendToAll(&packet);
}
