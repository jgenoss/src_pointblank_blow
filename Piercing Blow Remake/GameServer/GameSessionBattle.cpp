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
		int32_t result = 0;

		uint8_t gameMode = m_pRoom->GetGameMode();
		uint8_t mapIndex = m_pRoom->GetMapIndex();
		uint32_t stageId = m_pRoom->GetStageId();

		i3NetworkPacket packet(PROTOCOL_BATTLE_READYBATTLE_ACK);
		packet.WriteData(&result, sizeof(int32_t));
		packet.WriteData(&gameMode, 1);
		packet.WriteData(&mapIndex, 1);
		packet.WriteData(&stageId, 4);
		for (int i = 0; i < SLOT_MAX_COUNT; i++)
		{
			const GameSlotInfo& si = m_pRoom->GetSlotInfo(i);
			packet.WriteData(&si.ui8State, 1);
			packet.WriteData(&si.ui8Team, 1);
		}
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
		uint8_t slotIdx = (uint8_t)m_i32SlotIdx;
		uint8_t newState = bReady ? SLOT_STATE_READY : SLOT_STATE_NORMAL;
		i3NetworkPacket packet(PROTOCOL_BATTLE_SLOT_STATE_CHANGE_ACK);
		packet.WriteData(&slotIdx, 1);
		packet.WriteData(&newState, 1);
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
	int32_t result = 0;

	// Use BattleServer UDP IP/port stored in Room from OnBattleCreateAck
	uint32_t udpIP = m_pRoom->GetBattleUdpIPAddr();
	uint16_t udpPort = m_pRoom->GetBattleUdpPort();
	if (udpIP == 0)
	{
		udpIP = 0x0100007F;		// Fallback: 127.0.0.1
		udpPort = 41000;
	}
	i3NetworkPacket packet(PROTOCOL_BATTLE_PRESTARTBATTLE_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&udpIP, 4);
	packet.WriteData(&udpPort, 2);
	SendPacketMessage(&packet);
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
	uint8_t slotIdx = (uint8_t)m_i32SlotIdx;

	uint8_t interEnter = 0;

	// Battle user count
	uint16_t battleUser = (uint16_t)m_pRoom->GetPlayerCount();

	// Total round count per team
	const GameRoomScore& score = m_pRoom->GetScore();
	uint16_t redRounds = (uint16_t)score.i32RedScore;
	uint16_t blueRounds = (uint16_t)score.i32BlueScore;

	// Round start user count
	uint16_t roundStartUser = battleUser;

	// Now round count
	uint8_t nowRound = (uint8_t)score.i32NowRound;
	i3NetworkPacket packet(PROTOCOL_BATTLE_STARTBATTLE_ACK);
	packet.WriteData(&slotIdx, 1);
	packet.WriteData(&interEnter, 1);
	packet.WriteData(&battleUser, 2);
	packet.WriteData(&redRounds, 2);
	packet.WriteData(&blueRounds, 2);
	packet.WriteData(&roundStartUser, 2);
	packet.WriteData(&nowRound, 1);
	SendPacketMessage(&packet);
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

	const GameRoomScore& score = m_pRoom->GetScore();
	int32_t nowRound = score.i32NowRound;
	i3NetworkPacket packet(PROTOCOL_BATTLE_MISSION_ROUND_PRE_START_ACK);
	packet.WriteData(&nowRound, 4);
	m_pRoom->SendToAll(&packet);
}

void GameSession::OnBattleMissionRoundStartReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	const GameRoomScore& score = m_pRoom->GetScore();
	int32_t nowRound = score.i32NowRound;
	uint16_t maxTime = score.ui16MaxTime;
	i3NetworkPacket packet(PROTOCOL_BATTLE_MISSION_ROUND_START_ACK);
	packet.WriteData(&nowRound, 4);
	packet.WriteData(&maxTime, 2);
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
	uint8_t winner = (uint8_t)winnerTeam;
	const GameRoomScore& score = m_pRoom->GetScore();
	int32_t redScore = score.i32RedScore;
	int32_t blueScore = score.i32BlueScore;
	int32_t nowRound = score.i32NowRound;
	i3NetworkPacket packet(PROTOCOL_BATTLE_MISSION_ROUND_END_ACK);
	packet.WriteData(&winner, 1);
	packet.WriteData(&redScore, 4);
	packet.WriteData(&blueScore, 4);
	packet.WriteData(&nowRound, 4);
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
// Destroy Mode Handlers (Phase 2C)
// ============================================================================

void GameSession::OnBattleMissionGeneratorDestroyReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	if (m_pRoom->GetRoomState() != ROOM_STATE_BATTLE)
		return;

	if (!m_pRoom->IsDestroyMode())
		return;

	if (i32Size < 5)	// generator index (1) + damage (4)
		return;

	// Parse: generator index + damage dealt
	int offset = 0;
	uint8_t genIdx = *(uint8_t*)(pData + offset);		offset += 1;
	int32_t damage = *(int32_t*)(pData + offset);		offset += 4;

	// Only ATK (RED) can destroy generators
	if (m_i32SlotIdx < 0 || m_i32SlotIdx >= SLOT_MAX_COUNT)
		return;

	const GameSlotInfo& mySlot = m_pRoom->GetSlotInfo(m_i32SlotIdx);
	if (mySlot.ui8Team != TEAM_RED)
		return;

	if (!m_pRoom->IsPlayerAlive(m_i32SlotIdx))
		return;

	// Clamp damage to reasonable range
	if (damage <= 0 || damage > 200)
		return;

	m_pRoom->OnGeneratorDamage(genIdx, damage, m_i32SlotIdx);
}

void GameSession::OnBattleMissionGeneratorInfoReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	if (!m_pRoom->IsDestroyMode())
		return;

	// Send current generator HP info to requesting client
	uint8_t genCount = (uint8_t)m_pRoom->GetGeneratorCount();

	i3NetworkPacket packet(PROTOCOL_BATTLE_MISSION_GENERATOR_INFO_ACK);
	packet.WriteData(&genCount, 1);
	for (int i = 0; i < genCount; i++)
	{
		int32_t hp = m_pRoom->GetGeneratorHP(i);
		int32_t maxHp = GENERATOR_MAX_HP;
		packet.WriteData(&hp, 4);
		packet.WriteData(&maxHp, 4);
	}
	SendPacketMessage(&packet);
}

// ============================================================================
// Escape/VIP Mode Handler (Phase 2E)
// ============================================================================

void GameSession::OnBattleMissionTouchdownReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	if (m_pRoom->GetRoomState() != ROOM_STATE_BATTLE)
		return;

	if (!m_pRoom->IsEscapeMode())
		return;

	// Only VIP can touchdown
	if (m_i32SlotIdx != m_pRoom->GetVIPSlot())
		return;

	if (!m_pRoom->IsPlayerAlive(m_i32SlotIdx))
		return;

	m_pRoom->OnTouchdown(m_i32SlotIdx);
}

// ============================================================================
// Defence Mode Handler
// ============================================================================

void GameSession::OnBattleMissionDefenceInfoReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	if (m_pRoom->GetGameMode() != STAGE_MODE_DEFENCE)
		return;

	// Packet layout:
	// [2] proto | [1] currentWave | [1] maxWaves | [2] npcsAlive | [2] npcsTotal
	// | [1] genCount | [genCount × (4+4)] hp + maxHp per generator
	uint8_t  currentWave = (uint8_t)m_pRoom->GetDefenceWave();
	uint8_t  maxWaves    = (uint8_t)m_pRoom->GetDefenceMaxWaves();
	uint16_t npcsAlive   = (uint16_t)m_pRoom->GetDefenceNPCsAlive();
	uint16_t npcsTotal   = (uint16_t)m_pRoom->GetDefenceNPCsTotal();

	// Generator HP (the central objective players defend)
	uint8_t genCount = (uint8_t)m_pRoom->GetGeneratorCount();

	i3NetworkPacket packet(PROTOCOL_BATTLE_MISSION_DEFENCE_INFO_ACK);
	packet.WriteData(&currentWave, 1);
	packet.WriteData(&maxWaves, 1);
	packet.WriteData(&npcsAlive, sizeof(uint16_t));
	packet.WriteData(&npcsTotal, sizeof(uint16_t));
	packet.WriteData(&genCount, 1);
	for (int i = 0; i < genCount; i++)
	{
		int32_t hp    = m_pRoom->GetGeneratorHP(i);
		int32_t maxHp = GENERATOR_MAX_HP;
		packet.WriteData(&hp, 4);
		packet.WriteData(&maxHp, 4);
	}
	SendPacketMessage(&packet);
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

	// Phase 3A: Use Room's kick vote system with cooldown tracking
	if (!m_pRoom->StartKickVote(m_i32SlotIdx, targetSlot))
	{
		SendSimpleAck(PROTOCOL_BATTLE_SUGGEST_KICKVOTE_ACK, 5);	// Vote in progress or cooldown
		return;
	}

	// Accept the vote suggestion, broadcast to all players to vote
	int32_t result = 0;

	uint8_t suggestSlot = (uint8_t)m_i32SlotIdx;
	i3NetworkPacket packet(PROTOCOL_BATTLE_SUGGEST_KICKVOTE_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&suggestSlot, 1);
	packet.WriteData(&targetSlot, 1);
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

	// Phase 3A: Register vote with Room's kick vote system
	m_pRoom->CastKickVote(m_i32SlotIdx, targetSlot, vote);

	// Broadcast the vote to all players
	int32_t result = 0;

	uint8_t voterSlot = (uint8_t)m_i32SlotIdx;
	i3NetworkPacket packet(PROTOCOL_BATTLE_VOTE_KICKVOTE_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&voterSlot, 1);
	packet.WriteData(&targetSlot, 1);
	packet.WriteData(&vote, 1);
	m_pRoom->SendToAll(&packet);
}

// ============================================================================
// Battle Equipment Info (view equipment during battle)
// ============================================================================

void GameSession::OnBattleEquipmentInfoReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BATTLE_EQUIPMENT_INFO_REQ -> ACK
	// Request equipment info for all players in battle
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	// Number of active slots
	uint8_t slotCount = 0;
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pRoom->GetSlotSession(i))
			slotCount++;
	}

	i3NetworkPacket packet(PROTOCOL_BATTLE_EQUIPMENT_INFO_ACK);
	packet.WriteData(&slotCount, 1);
	// Per-slot equipment data
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		GameSession* pSlot = m_pRoom->GetSlotSession(i);
		if (!pSlot)
			continue;

		uint8_t slotIdx = (uint8_t)i;
		packet.WriteData(&slotIdx, 1);

		const GameCharaEquip& equip = pSlot->GetActiveEquipment();
		for (int w = 0; w < CHAR_EQUIP_WEAPON_COUNT; w++)
		{
			packet.WriteData(&equip.ui32WeaponIds[w], 4);
		}
		for (int p = 0; p < CHAR_EQUIP_PARTS_COUNT; p++)
		{
			packet.WriteData(&equip.ui32PartsIds[p], 4);
		}
	}
	SendPacketMessage(&packet);
}

// ============================================================================
// Weapon Pickup (pick up weapon from ground during battle)
// ============================================================================

void GameSession::OnBattlePickupWeaponReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BATTLE_PICKUP_WEAPON_REQ -> ACK (broadcast)
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	if (m_pRoom->GetRoomState() != ROOM_STATE_BATTLE)
		return;

	if (i32Size < 4)
		return;

	uint32_t weaponId = *(uint32_t*)pData;

	// Broadcast pickup to all players in room
	uint8_t slotIdx = (uint8_t)m_i32SlotIdx;
	i3NetworkPacket packet(PROTOCOL_BATTLE_PICKUP_WEAPON_ACK);
	packet.WriteData(&slotIdx, 1);
	packet.WriteData(&weaponId, 4);
	m_pRoom->SendToAll(&packet);
}

// ============================================================================
// Timer Sync (client sends accumulated time every 5 seconds)
// ============================================================================

void GameSession::OnBattleTimerSyncReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BATTLE_TIMERSYNC_REQ -> ACK (only on invalid sync)
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	if (i32Size < 4)
		return;

	uint32_t clientTime = *(uint32_t*)pData;

	// Validate timer - client sends cumulative battle time in ms
	// Compare with server elapsed time
	DWORD serverElapsed = m_pRoom->GetBattleElapsedTime();

	// Allow 10 second tolerance
	int diff = (int)clientTime - (int)serverElapsed;
	if (diff < -10000 || diff > 10000)
	{
		// Invalid sync - notify client
		uint32_t correctedTime = serverElapsed;
		i3NetworkPacket packet(PROTOCOL_BATTLE_TIMERSYNC_ACK);
		packet.WriteData(&correctedTime, 4);
		SendPacketMessage(&packet);
	}
}

// ============================================================================
// Timeout Client (host reports client unresponsive)
// ============================================================================

void GameSession::OnBattleTimeoutClientReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BATTLE_TIMEOUTCLIENT_REQ -> ACK
	// Room host reports that a client hasn't responded for 7 seconds
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	// Only room owner can report timeouts
	if (m_i32SlotIdx != m_pRoom->GetOwnerSlot())
		return;

	if (i32Size < 1)
		return;

	uint8_t targetSlot = *(uint8_t*)pData;
	if (targetSlot >= SLOT_MAX_COUNT)
		return;

	GameSession* pTarget = m_pRoom->GetSlotSession(targetSlot);
	if (!pTarget || pTarget == this)
		return;

	// Send timeout notification to target - they must return to lobby
	uint8_t slotIdx = targetSlot;
	i3NetworkPacket packet(PROTOCOL_BATTLE_TIMEOUTCLIENT_ACK);
	packet.WriteData(&slotIdx, 1);
	pTarget->SendPacketMessage(&packet);
}

// ============================================================================
// Leave P2P Server (host left, need new host)
// ============================================================================

void GameSession::OnBattleLeaveP2PServerReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BATTLE_LEAVEP2PSERVER_REQ -> ACK
	// Host has left the battle, reassign host
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	// Find next valid slot to be new host
	int newHostSlot = -1;
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		GameSession* pSlot = m_pRoom->GetSlotSession(i);
		if (pSlot && pSlot != this && i != m_i32SlotIdx)
		{
			newHostSlot = i;
			break;
		}
	}

	if (newHostSlot < 0)
		return;

	// Broadcast new host to all
	uint8_t oldHost = (uint8_t)m_i32SlotIdx;
	uint8_t newHost = (uint8_t)newHostSlot;
	i3NetworkPacket packet(PROTOCOL_BATTLE_LEAVEP2PSERVER_ACK);
	packet.WriteData(&oldHost, 1);
	packet.WriteData(&newHost, 1);
	m_pRoom->SendToAll(&packet);
}

// ============================================================================
// Use Item (consumable items during battle)
// ============================================================================

void GameSession::OnBattleUseItemReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BATTLE_USE_ITEM_REQ -> ACK (broadcast)
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	if (m_pRoom->GetRoomState() != ROOM_STATE_BATTLE)
		return;

	if (i32Size < 4)
		return;

	uint32_t itemId = *(uint32_t*)pData;

	// Verify player has the item
	if (!HasInventoryItem(itemId))
	{
		SendSimpleAck(PROTOCOL_BATTLE_USE_ITEM_ACK, 1);
		return;
	}

	// Broadcast item use to all players
	int32_t result = 0;

	uint8_t slotIdx = (uint8_t)m_i32SlotIdx;
	i3NetworkPacket packet(PROTOCOL_BATTLE_USE_ITEM_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&slotIdx, 1);
	packet.WriteData(&itemId, 4);
	m_pRoom->SendToAll(&packet);
}

// ============================================================================
// End Battle (server-decided, but client can request)
// ============================================================================

void GameSession::OnBattleEndBattleReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BATTLE_ENDBATTLE_REQ
	// Normally server decides, but GM/admin can force end
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	// Only allow from room owner or GM
	if (m_i32SlotIdx != m_pRoom->GetOwnerSlot() && !IsGMUser())
		return;

	const GameRoomScore& score = m_pRoom->GetScore();
	int winnerTeam = (score.i32RedScore >= score.i32BlueScore) ? TEAM_RED : TEAM_BLUE;

	SendBattleEndToAll(winnerTeam);
}

// ============================================================================
// Hole Check (P2P hole punching verification)
// ============================================================================

void GameSession::OnBattleHoleCheckReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BATTLE_HOLE_CHECK_REQ -> ACK
	// Simple echo back for P2P connectivity check
	if (!m_pRoom)
		return;

	SendSimpleAck(PROTOCOL_BATTLE_HOLE_CHECK_ACK, 0);
}

// ============================================================================
// Wave Mode (Domination - wave pre-start, ready info)
// ============================================================================

void GameSession::OnBattleWaveReadyInfoReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BATTLE_WAVE_READY_INFO_REQ -> ACK
	// Request wave/domination mode ready info
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	// Current wave info
	int32_t currentWave = m_pRoom->GetDefenceWave();
	int32_t maxWaves = DEFENCE_MAX_WAVES;
	uint8_t playerCount = (uint8_t)m_pRoom->GetPlayerCount();
	i3NetworkPacket packet(PROTOCOL_BATTLE_WAVE_READY_INFO_ACK);
	packet.WriteData(&currentWave, 4);
	packet.WriteData(&maxWaves, 4);
	packet.WriteData(&playerCount, 1);
	SendPacketMessage(&packet);
}

void GameSession::OnBattleWaveReadyInfoCancelReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BATTLE_WAVE_READY_INFO_CANCEL_REQ -> ACK (not used per protocol def)
	// Cancel wave ready state
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	// Simply acknowledge
	SendSimpleAck(PROTOCOL_BATTLE_WAVE_READY_INFO_ACK, 0);
}

// ============================================================================
// Battle Record (send current score to late joiners)
// ============================================================================

void GameSession::OnBattleRecordReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BATTLE_RECORD_REQ -> ACK
	// Send current battle record to a player who joined mid-battle
	if (!m_pRoom || m_eMainTask != GAME_TASK_BATTLE)
		return;

	const GameRoomScore& score = m_pRoom->GetScore();
	int32_t redScore = score.i32RedScore;
	int32_t blueScore = score.i32BlueScore;
	int32_t nowRound = score.i32NowRound;

	i3NetworkPacket packet(PROTOCOL_BATTLE_RECORD_ACK);
	packet.WriteData(&redScore, 4);
	packet.WriteData(&blueScore, 4);
	packet.WriteData(&nowRound, 4);
	// Per-slot kills/deaths
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		const Room::SlotBattleStats& stats = m_pRoom->GetSlotBattleStats(i);
		int16_t kills = (int16_t)stats.i32Kills;
		int16_t deaths = (int16_t)stats.i32Deaths;
		packet.WriteData(&kills, 2);
		packet.WriteData(&deaths, 2);
	}
	SendPacketMessage(&packet);
}

void GameSession::OnBattleSendPingReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom)
		return;

	if (i32Size < (int)sizeof(uint32_t))
		return;

	uint32_t pingMs = *(uint32_t*)pData;

	// Broadcast ping to all players in room
	uint8_t slotIdx = (uint8_t)m_i32SlotIdx;
	i3NetworkPacket packet(PROTOCOL_BATTLE_SENDPING_ACK);
	packet.WriteData(&slotIdx, 1);
	packet.WriteData(&pingMs, sizeof(uint32_t));
	m_pRoom->SendToAll(&packet);
}

// ============================================================================
// Battle Extended Handlers (Batch 15)
// ============================================================================

void GameSession::OnBattleChangeDifficultyReq(char* pData, INT32 i32Size)
{
	// Challenge mode difficulty change
	if (m_eMainTask != GAME_TASK_BATTLE || !m_pRoom)
		return;

	if (i32Size < 1)
		return;

	uint8_t diffLevel = *(uint8_t*)pData;

	// Broadcast difficulty change to all players
	i3NetworkPacket packet(PROTOCOL_BATTLE_CHANGE_DIFFICULTY_LEVEL_ACK);
	packet.WriteData(&diffLevel, 1);
	m_pRoom->SendToAll(&packet);
}

void GameSession::OnBattleMissionTouchdownCountReq(char* pData, INT32 i32Size)
{
	// Touchdown count update (cross counter mode)
	if (m_eMainTask != GAME_TASK_BATTLE || !m_pRoom)
		return;

	if (i32Size < 4)
		return;

	uint16_t redCount = *(uint16_t*)pData;
	uint16_t blueCount = *(uint16_t*)(pData + 2);

	i3NetworkPacket packet(PROTOCOL_BATTLE_MISSION_TOUCHDOWN_COUNT_ACK);
	packet.WriteData(&redCount, 2);
	packet.WriteData(&blueCount, 2);
	m_pRoom->SendToAll(&packet);
}

void GameSession::OnBattleMissionDeathblowReq(char* pData, INT32 i32Size)
{
	// Dino mission deathblow event
	if (m_eMainTask != GAME_TASK_BATTLE || !m_pRoom)
		return;

	if (i32Size < 5)
		return;

	uint8_t slotIdx = *(uint8_t*)pData;
	uint32_t targetId = *(uint32_t*)(pData + 1);

	i3NetworkPacket packet(PROTOCOL_BATTLE_MISSION_DEATHBLOW_ACK);
	packet.WriteData(&slotIdx, 1);
	packet.WriteData(&targetId, 4);
	m_pRoom->SendToAll(&packet);
}

void GameSession::OnBattleSupplyBoxResultReq(char* pData, INT32 i32Size)
{
	// Supply box result (random item drop during battle)
	if (m_eMainTask != GAME_TASK_BATTLE || !m_pRoom)
		return;

	if (i32Size < 5)
		return;

	uint8_t slotIdx = *(uint8_t*)pData;
	uint32_t itemId = *(uint32_t*)(pData + 1);

	// ACK with the item granted
	int32_t result = 0;
	i3NetworkPacket packet(PROTOCOL_BATTLE_SUPPLY_BOX_RESULT_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&slotIdx, 1);
	packet.WriteData(&itemId, 4);
	m_pRoom->SendToAll(&packet);
}

void GameSession::OnBattleUserSkillUpgradeReq(char* pData, INT32 i32Size)
{
	// In-battle skill upgrade
	if (m_eMainTask != GAME_TASK_BATTLE || !m_pRoom)
		return;

	if (i32Size < 2)
		return;

	uint8_t skillType = *(uint8_t*)pData;
	uint8_t skillIdx = *(uint8_t*)(pData + 1);

	// Broadcast skill upgrade to all (for visual effects)
	uint8_t mySlot = (uint8_t)m_i32SlotIdx;
	i3NetworkPacket packet(PROTOCOL_BATTLE_USER_SKILL_UPGRADE_ACK);
	packet.WriteData(&mySlot, 1);
	packet.WriteData(&skillType, 1);
	packet.WriteData(&skillIdx, 1);
	m_pRoom->SendToAll(&packet);
}

void GameSession::OnBattleSlotEquipmentReq(char* pData, INT32 i32Size)
{
	// In-battle weapon swap request
	if (m_eMainTask != GAME_TASK_BATTLE || !m_pRoom)
		return;

	if (i32Size < 5)
		return;

	uint8_t weaponSlot = *(uint8_t*)pData;
	uint32_t newWeaponId = *(uint32_t*)(pData + 1);

	// Validate weapon exists in inventory
	bool found = false;
	for (int i = 0; i < m_i32InventoryCount; i++)
	{
		if (m_Inventory[i].ui32ItemId == newWeaponId)
		{
			found = true;
			break;
		}
	}

	int32_t result = found ? 0 : -1;
	uint8_t mySlot = (uint8_t)m_i32SlotIdx;

	i3NetworkPacket packet(PROTOCOL_BATTLE_SLOT_EQUIPMENT_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&mySlot, 1);
	packet.WriteData(&weaponSlot, 1);
	packet.WriteData(&newWeaponId, 4);
	if (found)
		m_pRoom->SendToAll(&packet);
	else
		SendPacketMessage(&packet);
}

// ============================================================================
// Battle Extended Handlers (Batch 16)
// ============================================================================

void GameSession::OnBattleMissionTutorialRoundEndReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask != GAME_TASK_BATTLE || !m_pRoom)
		return;

	// Tutorial round end - broadcast to all
	uint8_t slotIdx = (uint8_t)m_i32SlotIdx;
	i3NetworkPacket packet(PROTOCOL_BATTLE_MISSION_TUTORIAL_ROUND_END_ACK);
	packet.WriteData(&slotIdx, 1);
	m_pRoom->SendToAll(&packet);
}

void GameSession::OnBattleUserScopeTypeReq(char* pData, INT32 i32Size)
{
	// Scope type change during battle (sniper scope style)
	if (m_eMainTask != GAME_TASK_BATTLE || !m_pRoom)
		return;
	if (i32Size < 1)
		return;

	uint8_t scopeType = *(uint8_t*)pData;

	uint8_t slotIdx = (uint8_t)m_i32SlotIdx;
	i3NetworkPacket packet(PROTOCOL_BATTLE_USER_SOPETYPE_ACK);
	packet.WriteData(&slotIdx, 1);
	packet.WriteData(&scopeType, 1);
	m_pRoom->SendToAll(&packet);
}

void GameSession::OnBattleTimeoutMainClientReq(char* pData, INT32 i32Size)
{
	// Non-host player reports host is unresponsive
	if (m_eMainTask != GAME_TASK_BATTLE || !m_pRoom)
		return;

	// Similar to OnBattleTimeoutClientReq but for main/host
	// Log and potentially trigger host migration
	printf("[GameSession] Battle host timeout reported by slot %d in room %d\n",
		m_i32SlotIdx, m_pRoom->GetRoomIdx());

	// No ACK defined - this is informational
}

void GameSession::OnBattleNewJoinRoomScoreReq(char* pData, INT32 i32Size)
{
	// Late-joining player requests current game score
	if (m_eMainTask != GAME_TASK_BATTLE || !m_pRoom)
		return;

	const GameRoomScore& score = m_pRoom->GetScore();

	int32_t redScore = score.i32RedScore;
	int32_t blueScore = score.i32BlueScore;
	int32_t nowRound = score.i32NowRound;
	int32_t maxRound = score.i32MaxRound;

	i3NetworkPacket packet(PROTOCOL_BATTLE_NEW_JOIN_ROOM_SCORE_ACK);
	packet.WriteData(&redScore, 4);
	packet.WriteData(&blueScore, 4);
	packet.WriteData(&nowRound, 4);
	packet.WriteData(&maxRound, 4);
	// Per-slot kills/deaths
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		const Room::SlotBattleStats& stats = m_pRoom->GetSlotBattleStats(i);
		int16_t kills = (int16_t)stats.i32Kills;
		int16_t deaths = (int16_t)stats.i32Deaths;
		packet.WriteData(&kills, 2);
		packet.WriteData(&deaths, 2);
	}
	SendPacketMessage(&packet);
}

void GameSession::OnBattleReportScreenshotReq(char* pData, INT32 i32Size)
{
	// Player reports another player (screenshot-based)
	if (m_eMainTask != GAME_TASK_BATTLE || !m_pRoom)
		return;

	if (i32Size < 1)
		return;

	uint8_t reportedSlot = *(uint8_t*)pData;
	printf("[GameSession] Screenshot report from slot %d against slot %d in room %d\n",
		m_i32SlotIdx, reportedSlot, m_pRoom->GetRoomIdx());

	SendSimpleAck(PROTOCOL_BATTLE_REPORTSCREENSHOT_ACK, 0);
}

// ============================================================================
// Batch 17 - Battle extras
// ============================================================================

void GameSession::OnBattleStartKickVoteReq(char* pData, INT32 i32Size)
{
	// Player initiates a kick vote against another player
	if (m_eMainTask != GAME_TASK_BATTLE || !m_pRoom)
	{
		SendSimpleAck(PROTOCOL_BATTLE_START_KICKVOTE_ACK, -1);
		return;
	}

	if (i32Size < (int)sizeof(uint8_t))
	{
		SendSimpleAck(PROTOCOL_BATTLE_START_KICKVOTE_ACK, -1);
		return;
	}

	uint8_t targetSlot = *(uint8_t*)pData;

	if (targetSlot >= SLOT_MAX_COUNT || targetSlot == (uint8_t)m_i32SlotIdx)
	{
		SendSimpleAck(PROTOCOL_BATTLE_START_KICKVOTE_ACK, -2);
		return;
	}

	// Notify all players in the room about the kick vote (except initiator and target)
	int32_t result = 0;

	uint8_t initiatorSlot = (uint8_t)m_i32SlotIdx;
	i3NetworkPacket packet(PROTOCOL_BATTLE_START_KICKVOTE_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&initiatorSlot, sizeof(uint8_t));
	packet.WriteData(&targetSlot, sizeof(uint8_t));
	m_pRoom->SendToAll(&packet);
}

void GameSession::OnBattleCheatMessageReq(char* pData, INT32 i32Size)
{
	// Anti-cheat report from client (fire-and-forget, ACK is _notuse)
	if (m_eMainTask != GAME_TASK_BATTLE)
		return;

	if (i32Size < (int)(sizeof(uint8_t) + sizeof(uint32_t)))
		return;

	int offset = 0;
	uint8_t cheatType = 0;
	uint32_t cheatData = 0;
	memcpy(&cheatType, pData + offset, sizeof(uint8_t));	offset += sizeof(uint8_t);
	memcpy(&cheatData, pData + offset, sizeof(uint32_t));

	printf("[GameSession] Cheat report: type=%d, data=%u from slot=%d UID=%lld\n",
		cheatType, cheatData, m_i32SlotIdx, m_i64UID);
}

// ============================================================================
// Batch 18 - Battle extras
// ============================================================================

void GameSession::OnBattlePrestartBattleDServerReq(char* pData, INT32 i32Size)
{
	// Client finished loading and is ready for dedicated server battle
	if (!m_pRoom)
	{
		SendSimpleAck(PROTOCOL_BATTLE_PRESTARTBATTLE_DSERVER_ACK, -1);
		return;
	}

	// Send battle server connection info
	int32_t result = 0;

	// UDP IP and port from room's battle info
	uint32_t udpIP = 0;
	uint16_t udpPort = 0;
	if (m_pRoom->GetBattleRoomIdx() >= 0)
	{
		inet_pton(AF_INET, m_pRoom->GetBattleUdpIP(), &udpIP);
		udpPort = m_pRoom->GetBattleUdpPort();
	}
	i3NetworkPacket packet(PROTOCOL_BATTLE_PRESTARTBATTLE_DSERVER_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&udpIP, sizeof(uint32_t));
	packet.WriteData(&udpPort, sizeof(uint16_t));
	SendPacketMessage(&packet);
}

void GameSession::OnBattlePrestartBattleRelayReq(char* pData, INT32 i32Size)
{
	// Client ready for relay-mode battle (P2P/relay instead of dedicated server)
	if (!m_pRoom)
	{
		SendSimpleAck(PROTOCOL_BATTLE_PRESTARTBATTLE_RELAY_ACK, -1);
		return;
	}

	SendSimpleAck(PROTOCOL_BATTLE_PRESTARTBATTLE_RELAY_ACK, 0);
}

void GameSession::OnBattleRespawnForAIReq(char* pData, INT32 i32Size)
{
	// AI/bot respawn request (PvE modes like Dino, Zombie)
	if (m_eMainTask != GAME_TASK_BATTLE || !m_pRoom)
	{
		SendSimpleAck(PROTOCOL_BATTLE_RESPAWN_FOR_AI_ACK, -1);
		return;
	}

	if (i32Size < (int)sizeof(uint8_t))
	{
		SendSimpleAck(PROTOCOL_BATTLE_RESPAWN_FOR_AI_ACK, -1);
		return;
	}

	uint8_t aiSlot = *(uint8_t*)pData;

	// Broadcast AI respawn to room
	int32_t result = 0;
	i3NetworkPacket packet(PROTOCOL_BATTLE_RESPAWN_FOR_AI_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&aiSlot, sizeof(uint8_t));
	m_pRoom->SendToAll(&packet);
}

void GameSession::OnBattleNotifyKickVoteResultReq(char* pData, INT32 i32Size)
{
	// Client reports their kick vote (agree/disagree)
	if (m_eMainTask != GAME_TASK_BATTLE || !m_pRoom)
		return;

	if (i32Size < 2)
		return;

	uint8_t targetSlot = *(uint8_t*)pData;
	uint8_t vote = *(uint8_t*)(pData + 1);  // 1=agree, 2=disagree

	// Broadcast vote result to room
	uint8_t voterSlot = (uint8_t)m_i32SlotIdx;
	i3NetworkPacket packet(PROTOCOL_BATTLE_NOTIFY_KICKVOTE_RESULT_ACK);
	packet.WriteData(&voterSlot, 1);
	packet.WriteData(&targetSlot, 1);
	packet.WriteData(&vote, 1);
	m_pRoom->SendToAll(&packet);
}

void GameSession::OnBattleNotifyKickVoteCancelReq(char* pData, INT32 i32Size)
{
	// Kick vote is cancelled (timeout or initiator left)
	if (m_eMainTask != GAME_TASK_BATTLE || !m_pRoom)
		return;

	// Broadcast cancellation to room
	int32_t result = 0;
	i3NetworkPacket packet(PROTOCOL_BATTLE_NOTIFY_KICKVOTE_CANCEL_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	m_pRoom->SendToAll(&packet);
}
