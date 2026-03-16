#include "pch.h"
#include "Room.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "GameContextMain.h"

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
	, m_ui8GameMode(STAGE_MODE_DEATHMATCH)
	, m_ui8MapIndex(0)
	, m_ui8RoundType(BATTLE_ROUND_TYPE_3)
	, m_ui8SubType(0)
	, m_ui8WeaponFlag(LOCK_BASIC)
	, m_ui8InfoFlag(0)
	, m_ui32OptionFlag(0)
	, m_bIsClanMatch(false)
	, m_dwBattleStartTime(0)
	, m_dwStateStartTime(0)
	, m_dwRoundStartTime(0)
	, m_ui8RespawnType(RESPAWN_TYPE_NO)
	, m_bBombInstalled(false)
	, m_i32BombInstallerSlot(-1)
	, m_ui8BombArea(BOMB_AREA_A)
	, m_dwBombInstallTime(0)
	, m_ui32BombExplosionTime(BOMB_EXPLOSION_DELAY_MS)
	, m_bAtkDefSwap(false)
	, m_i32GeneratorMaxHP(GENERATOR_MAX_HP)
	, m_i32GeneratorCount(1)
	, m_i32VIPSlot(ESCAPE_VIP_SLOT_NONE)
	, m_dwLoadingTimeout(LOADING_TIMEOUT * 1000)
	, m_i32BattleRoomIdx(-1)
	, m_ui16BattleUdpPort(0)
	, m_ui32BattleUdpIPAddr(0)
	, m_bKickVoteActive(false)
	, m_i32KickVoteTarget(-1)
	, m_i32KickVoteSuggestor(-1)
	, m_dwKickVoteStartTime(0)
	, m_dwLastKickVoteTime(0)
{
	m_szTitle[0] = '\0';
	m_szPassword[0] = '\0';
	m_szBattleUdpIP[0] = '\0';
	memset(m_KickVotes, 0, sizeof(m_KickVotes));

	for (int i = 0; i < GENERATOR_COUNT_MAX; i++)
	{
		m_i32GeneratorHP[i] = GENERATOR_MAX_HP;
		m_bGeneratorDestroyed[i] = false;
	}

	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		m_Slots[i].Reset();
		m_pSlotSession[i] = nullptr;
		m_SlotStats[i].Reset();
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

	strncpy_s(m_szTitle, pInfo->szTitle, _TRUNCATE);
	strncpy_s(m_szPassword, pInfo->szPassword, _TRUNCATE);

	// Stage settings
	m_ui32StageId = pInfo->ui32StageId;
	m_ui8GameMode = pInfo->ui8GameMode;
	m_ui8MapIndex = pInfo->ui8MapIndex;
	m_i32MaxPlayers = pInfo->ui8MaxPlayers;
	m_ui8RoundType = pInfo->ui8RoundType;
	m_ui8SubType = pInfo->ui8SubType;
	m_ui8WeaponFlag = pInfo->ui8WeaponFlag;
	m_ui8InfoFlag = pInfo->ui8InfoFlag;
	m_ui32OptionFlag = pInfo->ui32OptionFlag;
	m_bIsClanMatch = pInfo->bIsClanMatch;

	// Set info flags based on settings
	if (m_szPassword[0] != '\0')
		m_ui8InfoFlag |= ROOM_INFO_FLAG_PRIVATE_MODE;

	// Initialize score
	m_Score.Reset();
	m_Score.i32MaxRound = GetRoundCount(m_ui8RoundType);
	m_Score.ui16MaxTime = GetTimeLimit(m_ui8SubType, m_ui8GameMode);

	// Reset bomb state
	m_bBombInstalled = false;
	m_i32BombInstallerSlot = -1;
	m_ui8BombArea = BOMB_AREA_A;
	m_dwBombInstallTime = 0;
	m_bAtkDefSwap = false;

	// Reset destroy mode state
	for (int i = 0; i < GENERATOR_COUNT_MAX; i++)
	{
		m_i32GeneratorHP[i] = m_i32GeneratorMaxHP;
		m_bGeneratorDestroyed[i] = false;
	}
	m_i32GeneratorCount = 1;	// Default 1 generator, maps can override

	// Reset escape/VIP mode state
	m_i32VIPSlot = ESCAPE_VIP_SLOT_NONE;

	if (m_i32MaxPlayers > SLOT_MAX_COUNT)
		m_i32MaxPlayers = SLOT_MAX_COUNT;
	if (m_i32MaxPlayers < 2)
		m_i32MaxPlayers = 2;

	// Reset all slots
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		m_Slots[i].Reset();
		m_pSlotSession[i] = nullptr;

		// Close slots beyond max players
		if (i >= m_i32MaxPlayers)
			m_Slots[i].ui8State = SLOT_STATE_CLOSE;
	}

	// Set team balance flag
	bool bTeamBalance = (pInfo->ui8TeamBalance != 0);
	if (bTeamBalance)
		m_ui8InfoFlag |= ROOM_INFO_FLAG_TEAM_BALANCE;

	// Owner joins automatically to slot 0 (RED team)
	int slot = OnJoin(pOwner, TEAM_RED);
	if (slot < 0)
	{
		m_bIsCreate = false;
		return false;
	}

	m_i32OwnerSlot = slot;

	printf("[Room] Created - Ch=%d, Mode=%d, Map=%d, MaxP=%d, Round=%d, Title=%s\n",
		m_i32ChannelNum, m_ui8GameMode, m_ui8MapIndex,
		m_i32MaxPlayers, m_Score.i32MaxRound, m_szTitle);

	return true;
}

void Room::OnDestroy()
{
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
	m_Score.Reset();
}

int Room::OnJoin(GameSession* pSession, int i32Team)
{
	if (!pSession || !m_bIsCreate)
		return -1;

	if (m_i32PlayerCount >= m_i32MaxPlayers)
		return -1;

	// Cannot join during battle (unless inter-enter is allowed)
	if (m_ui8RoomState >= ROOM_STATE_LOADING && m_ui8RoomState <= ROOM_STATE_BATTLE)
	{
		if (!(m_ui8InfoFlag & ROOM_INFO_FLAG_INTER_ENTER))
			return -1;
	}

	// Auto-balance teams if team balance is enabled
	if (m_ui8InfoFlag & ROOM_INFO_FLAG_TEAM_BALANCE)
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
	m_Slots[slot].ui8Rank = (uint8_t)pSession->GetRankId();
	m_Slots[slot].ui32ClanIdx = 0;	// TODO: from session clan info
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

			if (i == m_i32OwnerSlot)
				UpdateOwner();

			break;
		}
	}

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

	DWORD dwNow = GetTickCount();
	m_ui8RoomState = ROOM_STATE_COUNTDOWN_R;
	m_dwStateStartTime = dwNow;

	// Set all slots to loading state
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i])
		{
			m_Slots[i].ui8State = SLOT_STATE_LOAD;
			m_SlotStats[i].Reset();
		}
	}

	m_dwBattleStartTime = dwNow;

	// Reset score for new match
	m_Score.Reset();
	m_Score.i32MaxRound = GetRoundCount(m_ui8RoundType);
	m_Score.ui16MaxTime = GetTimeLimit(m_ui8SubType, m_ui8GameMode);

	// Reset bomb state
	m_bBombInstalled = false;
	m_i32BombInstallerSlot = -1;
	m_dwBombInstallTime = 0;
	m_bAtkDefSwap = false;

	// Reset destroy mode generators
	if (m_ui8GameMode == STAGE_MODE_DESTROY || m_ui8GameMode == STAGE_MODE_DEFENCE)
	{
		for (int i = 0; i < GENERATOR_COUNT_MAX; i++)
		{
			m_i32GeneratorHP[i] = m_i32GeneratorMaxHP;
			m_bGeneratorDestroyed[i] = false;
		}
	}

	// Select VIP for escape mode
	if (m_ui8GameMode == STAGE_MODE_ESCAPE)
		SelectVIP();

	printf("[Room] Battle started - Ch=%d, Room=%d, Mode=%d, Players=%d\n",
		m_i32ChannelNum, m_i32RoomIdx, m_ui8GameMode, m_i32PlayerCount);
	return true;
}

void Room::OnEndBattle()
{
	m_ui8RoomState = ROOM_STATE_READY;

	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i])
			m_Slots[i].ui8State = SLOT_STATE_NORMAL;
	}

	printf("[Room] Battle ended - Ch=%d, Room=%d, Score R%d-B%d\n",
		m_i32ChannelNum, m_i32RoomIdx, m_Score.i32RedScore, m_Score.i32BlueScore);
}

bool Room::OnChangeTeam(GameSession* pSession, int i32NewTeam)
{
	if (!pSession || m_ui8RoomState != ROOM_STATE_READY)
		return false;

	int oldSlot = -1;
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i] == pSession)
		{
			oldSlot = i;
			break;
		}
	}

	if (oldSlot < 0)
		return false;

	if (m_Slots[oldSlot].ui8Team == (uint8_t)i32NewTeam)
		return false;	// Already on that team

	int newSlot = FindEmptySlot(i32NewTeam);
	if (newSlot < 0)
		return false;

	// Move to new slot
	m_Slots[newSlot] = m_Slots[oldSlot];
	m_Slots[newSlot].ui8Team = (uint8_t)i32NewTeam;
	m_pSlotSession[newSlot] = pSession;

	m_Slots[oldSlot].Reset();
	m_pSlotSession[oldSlot] = nullptr;

	// Update session slot index
	pSession->SetSlotIdx(newSlot);

	if (oldSlot == m_i32OwnerSlot)
		m_i32OwnerSlot = newSlot;

	return true;
}

bool Room::OnChangeRoomInfo(GameSession* pSession, GameRoomCreateInfo* pNewInfo)
{
	if (!pSession || !pNewInfo)
		return false;

	// Only owner can change room info
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i] == pSession && i == m_i32OwnerSlot)
		{
			// Only allow changes while in ready state
			if (m_ui8RoomState != ROOM_STATE_READY)
				return false;

			strncpy_s(m_szTitle, pNewInfo->szTitle, _TRUNCATE);
			m_ui32StageId = pNewInfo->ui32StageId;
			m_ui8GameMode = pNewInfo->ui8GameMode;
			m_ui8MapIndex = pNewInfo->ui8MapIndex;
			m_ui8RoundType = pNewInfo->ui8RoundType;
			m_ui8SubType = pNewInfo->ui8SubType;
			m_ui8WeaponFlag = pNewInfo->ui8WeaponFlag;
			m_ui8InfoFlag = pNewInfo->ui8InfoFlag;
			m_ui32OptionFlag = pNewInfo->ui32OptionFlag;

			// Don't change max players (could kick people)
			// Don't change password via this method

			// Update score limits
			m_Score.i32MaxRound = GetRoundCount(m_ui8RoundType);
			m_Score.ui16MaxTime = GetTimeLimit(m_ui8SubType, m_ui8GameMode);

			// Unready all players when settings change
			for (int j = 0; j < SLOT_MAX_COUNT; j++)
			{
				if (m_pSlotSession[j] && j != m_i32OwnerSlot)
					m_Slots[j].ui8State = SLOT_STATE_NORMAL;
			}

			return true;
		}
	}
	return false;
}

bool Room::OnChangePassword(GameSession* pSession, const char* pszNewPassword)
{
	if (!pSession)
		return false;

	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i] == pSession && i == m_i32OwnerSlot)
		{
			if (pszNewPassword)
				strncpy_s(m_szPassword, pszNewPassword, _TRUNCATE);
			else
				m_szPassword[0] = '\0';

			// Update private mode flag
			if (m_szPassword[0] != '\0')
				m_ui8InfoFlag |= ROOM_INFO_FLAG_PRIVATE_MODE;
			else
				m_ui8InfoFlag &= ~ROOM_INFO_FLAG_PRIVATE_MODE;

			return true;
		}
	}
	return false;
}

bool Room::OnKickPlayer(GameSession* pOwner, int i32TargetSlot)
{
	if (!pOwner || i32TargetSlot < 0 || i32TargetSlot >= SLOT_MAX_COUNT)
		return false;

	// Verify owner
	bool isOwner = false;
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i] == pOwner && i == m_i32OwnerSlot)
		{
			isOwner = true;
			break;
		}
	}
	if (!isOwner)
		return false;

	// Can't kick yourself
	if (i32TargetSlot == m_i32OwnerSlot)
		return false;

	if (!m_pSlotSession[i32TargetSlot])
		return false;

	// Remove the player
	GameSession* pTarget = m_pSlotSession[i32TargetSlot];
	m_Slots[i32TargetSlot].Reset();
	m_pSlotSession[i32TargetSlot] = nullptr;
	m_i32PlayerCount--;

	// Update kicked player's state
	pTarget->SetRoomIdx(-1);
	pTarget->SetSlotIdx(-1);
	pTarget->SetRoom(nullptr);

	return true;
}

bool Room::OnTransferOwner(GameSession* pOwner, int i32TargetSlot)
{
	if (!pOwner || i32TargetSlot < 0 || i32TargetSlot >= SLOT_MAX_COUNT)
		return false;

	// Verify current owner
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i] == pOwner && i == m_i32OwnerSlot)
		{
			if (!m_pSlotSession[i32TargetSlot])
				return false;

			m_i32OwnerSlot = i32TargetSlot;
			return true;
		}
	}
	return false;
}

void Room::OnRoundEnd(int i32WinnerTeam)
{
	if (i32WinnerTeam == TEAM_RED)
		m_Score.i32RedScore++;
	else if (i32WinnerTeam == TEAM_BLUE)
		m_Score.i32BlueScore++;

	m_Score.i32NowRound++;
}

void Room::OnAddKill(int i32Team)
{
	if (i32Team == TEAM_RED)
		m_Score.i32RedScore++;
	else if (i32Team == TEAM_BLUE)
		m_Score.i32BlueScore++;
}

bool Room::CheckMatchEnd() const
{
	if (IsMissionMode(m_ui8GameMode))
	{
		// Mission modes: check if max rounds reached
		int winsNeeded = (m_Score.i32MaxRound / 2) + 1;
		if (m_Score.i32RedScore >= winsNeeded || m_Score.i32BlueScore >= winsNeeded)
			return true;
		if (m_Score.i32NowRound >= m_Score.i32MaxRound)
			return true;
	}
	else
	{
		// Deathmatch: check kill limit
		int killLimit = GetKillLimit(m_ui8SubType);
		if (m_Score.i32RedScore >= killLimit || m_Score.i32BlueScore >= killLimit)
			return true;
	}
	return false;
}

bool Room::CheckPassword(const char* pw) const
{
	if (m_szPassword[0] == '\0')
		return true;	// No password set

	if (!pw)
		return false;

	return (strncmp(m_szPassword, pw, NET_ROOM_PW_SIZE) == 0);
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

void Room::SendToAllExcept(GameSession* pExcept, i3NetworkPacket* pPacket)
{
	if (!pPacket)
		return;

	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i] && m_pSlotSession[i] != pExcept)
			m_pSlotSession[i]->SendMessage(pPacket);
	}
}

void Room::FillRoomInfoBasic(char* pBuffer, int* pSize) const
{
	// Serialize in ROOM_INFO_BASIC compatible format (Phase 3B - complete)
	if (!pBuffer || !pSize)
		return;

	int offset = 0;

	// _Idx
	memcpy(pBuffer + offset, &m_i32RoomIdx, sizeof(int32_t));		offset += sizeof(int32_t);
	// _Title
	memcpy(pBuffer + offset, m_szTitle, NET_ROOM_NAME_SIZE);		offset += NET_ROOM_NAME_SIZE;
	// _StageID
	memcpy(pBuffer + offset, &m_ui32StageId, sizeof(uint32_t));		offset += sizeof(uint32_t);
	// _GameMode
	memcpy(pBuffer + offset, &m_ui8GameMode, sizeof(uint8_t));		offset += sizeof(uint8_t);
	// _MapIndex
	memcpy(pBuffer + offset, &m_ui8MapIndex, sizeof(uint8_t));		offset += sizeof(uint8_t);
	// _State
	memcpy(pBuffer + offset, &m_ui8RoomState, sizeof(uint8_t));		offset += sizeof(uint8_t);
	// _PersonNow
	uint8_t personNow = (uint8_t)m_i32PlayerCount;
	memcpy(pBuffer + offset, &personNow, sizeof(uint8_t));			offset += sizeof(uint8_t);
	// _PersonPlayerMax
	uint8_t personMax = (uint8_t)m_i32MaxPlayers;
	memcpy(pBuffer + offset, &personMax, sizeof(uint8_t));			offset += sizeof(uint8_t);
	// _Ping (average of all players - 0 for now)
	uint8_t ping = 0;
	memcpy(pBuffer + offset, &ping, sizeof(uint8_t));				offset += sizeof(uint8_t);
	// _RoundType
	memcpy(pBuffer + offset, &m_ui8RoundType, sizeof(uint8_t));		offset += sizeof(uint8_t);
	// _SubType (kill count | time limit)
	memcpy(pBuffer + offset, &m_ui8SubType, sizeof(uint8_t));		offset += sizeof(uint8_t);
	// _WeaponFlag
	memcpy(pBuffer + offset, &m_ui8WeaponFlag, sizeof(uint8_t));	offset += sizeof(uint8_t);
	// _InfoFlag
	memcpy(pBuffer + offset, &m_ui8InfoFlag, sizeof(uint8_t));		offset += sizeof(uint8_t);
	// _OptionFlag
	memcpy(pBuffer + offset, &m_ui32OptionFlag, sizeof(uint32_t));	offset += sizeof(uint32_t);
	// _IsClanMatch
	uint8_t isClan = m_bIsClanMatch ? 1 : 0;
	memcpy(pBuffer + offset, &isClan, sizeof(uint8_t));				offset += sizeof(uint8_t);
	// _HasPassword
	uint8_t hasPw = HasPassword() ? 1 : 0;
	memcpy(pBuffer + offset, &hasPw, sizeof(uint8_t));				offset += sizeof(uint8_t);
	// _OwnerSlot
	uint8_t ownerSlot = (uint8_t)m_i32OwnerSlot;
	memcpy(pBuffer + offset, &ownerSlot, sizeof(uint8_t));			offset += sizeof(uint8_t);
	// _Score (Red, Blue, NowRound)
	memcpy(pBuffer + offset, &m_Score.i32RedScore, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(pBuffer + offset, &m_Score.i32BlueScore, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(pBuffer + offset, &m_Score.i32NowRound, sizeof(int32_t));	offset += sizeof(int32_t);
	// _MaxRound
	memcpy(pBuffer + offset, &m_Score.i32MaxRound, sizeof(int32_t));	offset += sizeof(int32_t);
	// _TimeElapsed (seconds since battle started, 0 if not in battle)
	uint16_t timeElapsed = 0;
	if (m_ui8RoomState >= ROOM_STATE_BATTLE && m_dwBattleStartTime > 0)
		timeElapsed = (uint16_t)((GetTickCount() - m_dwBattleStartTime) / 1000);
	memcpy(pBuffer + offset, &timeElapsed, sizeof(uint16_t));			offset += sizeof(uint16_t);
	// _TimeMax
	memcpy(pBuffer + offset, &m_Score.ui16MaxTime, sizeof(uint16_t));	offset += sizeof(uint16_t);

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

	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i])
		{
			m_i32OwnerSlot = i;
			break;
		}
	}
}

void Room::SetBattleInfo(int i32BattleRoomIdx, const char* pszUdpIP, uint16_t ui16UdpPort)
{
	m_i32BattleRoomIdx = i32BattleRoomIdx;
	m_ui16BattleUdpPort = ui16UdpPort;

	if (pszUdpIP)
		strncpy_s(m_szBattleUdpIP, pszUdpIP, _TRUNCATE);

	// Convert IP string to packed uint32 for client
	m_ui32BattleUdpIPAddr = 0;
	if (pszUdpIP)
	{
		struct in_addr addr;
		if (inet_pton(AF_INET, pszUdpIP, &addr) == 1)
			m_ui32BattleUdpIPAddr = addr.s_addr;
	}
}

void Room::ClearBattleInfo()
{
	m_i32BattleRoomIdx = -1;
	m_szBattleUdpIP[0] = '\0';
	m_ui16BattleUdpPort = 0;
	m_ui32BattleUdpIPAddr = 0;
}

// ============================================================================
// Timer-driven Room State Machine
// Called from RoomManager::OnUpdate() each tick
// ============================================================================

void Room::OnUpdateRoom(DWORD dwNow)
{
	if (!m_bIsCreate)
		return;

	switch (m_ui8RoomState)
	{
	case ROOM_STATE_READY:
		// Nothing to do in ready state
		break;
	case ROOM_STATE_COUNTDOWN_R:
		OnUpdateRoom_CountdownR(dwNow);
		break;
	case ROOM_STATE_LOADING:
		OnUpdateRoom_Loading(dwNow);
		break;
	case ROOM_STATE_RENDEZVOUS:
		OnUpdateRoom_Rendezvous(dwNow);
		break;
	case ROOM_STATE_PRE_BATTLE:
		OnUpdateRoom_PreBattle(dwNow);
		break;
	case ROOM_STATE_COUNTDOWN_B:
		OnUpdateRoom_CountdownB(dwNow);
		break;
	case ROOM_STATE_BATTLE:
		OnUpdateRoom_Battle(dwNow);
		break;
	case ROOM_STATE_BATTLE_RESULT:
		OnUpdateRoom_BattleResult(dwNow);
		break;
	case ROOM_STATE_BATTLE_END:
		OnUpdateRoom_BattleEnd(dwNow);
		break;
	}
}

void Room::OnUpdateRoom_CountdownR(DWORD dwNow)
{
	// 3 second countdown before loading
	DWORD dwElapsed = dwNow - m_dwStateStartTime;
	if (dwElapsed >= COUNT_DOWN_TIME * 1000)
	{
		m_ui8RoomState = ROOM_STATE_LOADING;
		m_dwStateStartTime = dwNow;

		printf("[Room] COUNTDOWN_R -> LOADING - Room=%d\n", m_i32RoomIdx);
	}
}

void Room::OnUpdateRoom_Loading(DWORD dwNow)
{
	// Wait for all players to finish loading (SLOT_STATE_LOAD -> SLOT_STATE_BATTLE_LOADOK)
	// Timeout: kick players who haven't loaded in time
	DWORD dwElapsed = dwNow - m_dwStateStartTime;

	if (dwElapsed >= m_dwLoadingTimeout)
	{
		// Kick players who haven't loaded
		for (int i = 0; i < SLOT_MAX_COUNT; i++)
		{
			if (m_pSlotSession[i] && m_Slots[i].ui8State == SLOT_STATE_LOAD)
			{
				printf("[Room] Loading timeout - kicking slot %d from Room=%d\n", i, m_i32RoomIdx);
				GameSession* pKicked = m_pSlotSession[i];
				m_Slots[i].Reset();
				m_pSlotSession[i] = nullptr;
				m_i32PlayerCount--;
				pKicked->SetRoomIdx(-1);
				pKicked->SetSlotIdx(-1);
				pKicked->SetRoom(nullptr);
				pKicked->SetTask(GAME_TASK_LOBBY);
			}
		}

		// If no players remain, destroy room
		if (m_i32PlayerCount <= 0)
		{
			OnDestroy();
			return;
		}
	}

	// Check if all remaining players have loaded
	bool allLoaded = true;
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i])
		{
			if (m_Slots[i].ui8State < SLOT_STATE_BATTLE_LOADOK)
			{
				allLoaded = false;
				break;
			}
		}
	}

	if (allLoaded)
	{
		m_ui8RoomState = ROOM_STATE_RENDEZVOUS;
		m_dwStateStartTime = dwNow;

		// Set all slots to rendezvous state
		for (int i = 0; i < SLOT_MAX_COUNT; i++)
		{
			if (m_pSlotSession[i])
				m_Slots[i].ui8State = SLOT_STATE_RENDEZVOUS;
		}

		printf("[Room] LOADING -> RENDEZVOUS - Room=%d\n", m_i32RoomIdx);
	}
}

void Room::OnUpdateRoom_Rendezvous(DWORD dwNow)
{
	// Wait for BattleServer to be ready / rendezvous complete
	// Auto-advance after MISSION_PREBATTLE_TIME seconds or when all slots confirm
	DWORD dwElapsed = dwNow - m_dwStateStartTime;

	bool allReady = true;
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i] && m_Slots[i].ui8State < SLOT_STATE_PRESTART)
		{
			allReady = false;
			break;
		}
	}

	if (allReady || dwElapsed >= MISSION_PREBATTLE_TIME * 1000)
	{
		m_ui8RoomState = ROOM_STATE_PRE_BATTLE;
		m_dwStateStartTime = dwNow;

		for (int i = 0; i < SLOT_MAX_COUNT; i++)
		{
			if (m_pSlotSession[i])
				m_Slots[i].ui8State = SLOT_STATE_PRESTART;
		}

		printf("[Room] RENDEZVOUS -> PRE_BATTLE - Room=%d\n", m_i32RoomIdx);
	}
}

void Room::OnUpdateRoom_PreBattle(DWORD dwNow)
{
	// Wait for hole punching / NAT traversal to complete
	// Auto-advance after MISSION_PREBATTLE_TIME seconds or when all slots ready
	DWORD dwElapsed = dwNow - m_dwStateStartTime;

	bool allReady = true;
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i] && m_Slots[i].ui8State < SLOT_STATE_BATTLE_READY)
		{
			allReady = false;
			break;
		}
	}

	if (allReady || dwElapsed >= MISSION_PREBATTLE_TIME * 1000)
	{
		m_ui8RoomState = ROOM_STATE_COUNTDOWN_B;
		m_dwStateStartTime = dwNow;

		for (int i = 0; i < SLOT_MAX_COUNT; i++)
		{
			if (m_pSlotSession[i])
				m_Slots[i].ui8State = SLOT_STATE_BATTLE_READY;
		}

		printf("[Room] PRE_BATTLE -> COUNTDOWN_B - Room=%d\n", m_i32RoomIdx);
	}
}

void Room::OnUpdateRoom_CountdownB(DWORD dwNow)
{
	// 3 second countdown before battle begins
	DWORD dwElapsed = dwNow - m_dwStateStartTime;
	if (dwElapsed >= COUNT_DOWN_TIME * 1000)
	{
		m_ui8RoomState = ROOM_STATE_BATTLE;
		m_dwStateStartTime = dwNow;
		m_dwRoundStartTime = dwNow;

		// Set all slots to battle state, all alive
		for (int i = 0; i < SLOT_MAX_COUNT; i++)
		{
			if (m_pSlotSession[i])
			{
				m_Slots[i].ui8State = SLOT_STATE_BATTLE;
				m_SlotStats[i].bAlive = true;
			}
		}

		printf("[Room] COUNTDOWN_B -> BATTLE - Room=%d, Round=%d\n",
			m_i32RoomIdx, m_Score.i32NowRound);
	}
}

void Room::OnUpdateRoom_Battle(DWORD dwNow)
{
	// Check kick vote timeout (Phase 3A)
	if (m_bKickVoteActive && (dwNow - m_dwKickVoteStartTime) >= KICK_VOTE_TIMEOUT_MS)
		ResolveKickVote();

	// Check time limit
	if (m_dwRoundStartTime > 0 && m_Score.ui16MaxTime > 0)
	{
		DWORD dwElapsed = (dwNow - m_dwRoundStartTime) / 1000;
		if (dwElapsed >= m_Score.ui16MaxTime)
		{
			// Time's up
			int winnerTeam = -1;
			if (m_Score.i32RedScore > m_Score.i32BlueScore)
				winnerTeam = TEAM_RED;
			else if (m_Score.i32BlueScore > m_Score.i32RedScore)
				winnerTeam = TEAM_BLUE;

			printf("[Room] Battle timer expired - Room=%d, Winner=%s\n",
				m_i32RoomIdx,
				winnerTeam == TEAM_RED ? "RED" :
				winnerTeam == TEAM_BLUE ? "BLUE" : "DRAW");

			if (IsMissionMode(m_ui8GameMode))
			{
				// In bomb mode with bomb planted, let the bomb timer handle it
				if (m_ui8GameMode == STAGE_MODE_BOMB && m_bBombInstalled)
					return;	// Bomb timer will handle round end

				// In mission mode, time expiry = DEF wins the round
				OnRoundEnd(TEAM_BLUE);	// DEF wins on timeout

				if (CheckMatchEnd())
				{
					int finalWinner = (m_Score.i32RedScore > m_Score.i32BlueScore) ? TEAM_RED : TEAM_BLUE;
					CalculateBattleRewards(finalWinner);
					m_ui8RoomState = ROOM_STATE_BATTLE_RESULT;
					m_dwStateStartTime = dwNow;
					SendBattleResultToAll(finalWinner);
				}
				else
				{
					// Next round - reset round timer, alive status, mode state
					m_dwRoundStartTime = dwNow;
					m_bBombInstalled = false;
					m_i32BombInstallerSlot = -1;
					m_dwBombInstallTime = 0;
					for (int i = 0; i < SLOT_MAX_COUNT; i++)
					{
						if (m_pSlotSession[i])
							m_SlotStats[i].bAlive = true;
					}

					// Reset destroy mode generators
					if (m_ui8GameMode == STAGE_MODE_DESTROY)
					{
						for (int i = 0; i < GENERATOR_COUNT_MAX; i++)
						{
							m_i32GeneratorHP[i] = m_i32GeneratorMaxHP;
							m_bGeneratorDestroyed[i] = false;
						}
					}

					if (IsAtkDefMode(m_ui8GameMode) &&
						(m_ui8InfoFlag & ROOM_INFO_FLAG_ATK_DEF_AUTO_CHANGE))
						SwapTeams();

					// Re-select VIP for escape mode
					if (m_ui8GameMode == STAGE_MODE_ESCAPE)
						SelectVIP();
				}
			}
			else
			{
				// Deathmatch time up -> battle result
				int finalWinner = -1;
				if (m_Score.i32RedScore > m_Score.i32BlueScore)
					finalWinner = TEAM_RED;
				else if (m_Score.i32BlueScore > m_Score.i32RedScore)
					finalWinner = TEAM_BLUE;

				CalculateBattleRewards(finalWinner);
				m_ui8RoomState = ROOM_STATE_BATTLE_RESULT;
				m_dwStateStartTime = dwNow;
				SendBattleResultToAll(finalWinner);
			}
			return;
		}
	}

	// Check bomb explosion timer (Phase 2A)
	if (m_ui8GameMode == STAGE_MODE_BOMB && m_bBombInstalled && m_dwBombInstallTime > 0)
	{
		DWORD dwBombElapsed = dwNow - m_dwBombInstallTime;
		if (dwBombElapsed >= m_ui32BombExplosionTime)
		{
			OnBombExplode();
			return;
		}
	}

	// Check respawn timers for dead players in deathmatch
	if (!IsMissionMode(m_ui8GameMode))
	{
		int respawnMs = GetRespawnTime() * 1000;
		for (int i = 0; i < SLOT_MAX_COUNT; i++)
		{
			if (m_pSlotSession[i] && m_SlotStats[i].bRespawnPending)
			{
				if (m_SlotStats[i].dwDeathTime > 0 &&
					(dwNow - m_SlotStats[i].dwDeathTime) >= (DWORD)respawnMs)
				{
					// Auto-respawn ready
					m_SlotStats[i].bRespawnPending = false;
				}
			}
		}
	}

	// For mission mode: check if all players on one team are dead
	if (IsMissionMode(m_ui8GameMode))
	{
		int redAlive = GetAliveCount(TEAM_RED);
		int blueAlive = GetAliveCount(TEAM_BLUE);

		// In bomb mode: if ATK all dead but bomb is planted, bomb timer keeps running
		if (m_ui8GameMode == STAGE_MODE_BOMB && m_bBombInstalled && redAlive == 0)
			return;	// Bomb is ticking, DEF must defuse

		// In escape mode: VIP death means DEF wins immediately
		if (m_ui8GameMode == STAGE_MODE_ESCAPE && m_i32VIPSlot >= 0)
		{
			if (!IsPlayerAlive(m_i32VIPSlot))
			{
				// VIP is dead -> DEF (BLUE) wins the round
				OnRoundEnd(TEAM_BLUE);

				if (CheckMatchEnd())
				{
					int finalWinner = (m_Score.i32RedScore > m_Score.i32BlueScore) ? TEAM_RED : TEAM_BLUE;
					CalculateBattleRewards(finalWinner);
					m_ui8RoomState = ROOM_STATE_BATTLE_RESULT;
					m_dwStateStartTime = dwNow;
					SendBattleResultToAll(finalWinner);
				}
				else
				{
					m_dwRoundStartTime = dwNow;
					for (int i = 0; i < SLOT_MAX_COUNT; i++)
					{
						if (m_pSlotSession[i])
							m_SlotStats[i].bAlive = true;
					}
					if (m_ui8InfoFlag & ROOM_INFO_FLAG_ATK_DEF_AUTO_CHANGE)
						SwapTeams();
					SelectVIP();
					BroadcastRoomStateChange();
				}
				return;
			}
		}

		if (redAlive == 0 || blueAlive == 0)
		{
			int roundWinner = (redAlive == 0) ? TEAM_BLUE : TEAM_RED;
			OnRoundEnd(roundWinner);

			// Reset bomb state for next round
			m_bBombInstalled = false;
			m_i32BombInstallerSlot = -1;
			m_dwBombInstallTime = 0;

			if (CheckMatchEnd())
			{
				int finalWinner = (m_Score.i32RedScore > m_Score.i32BlueScore) ? TEAM_RED : TEAM_BLUE;
				CalculateBattleRewards(finalWinner);
				m_ui8RoomState = ROOM_STATE_BATTLE_RESULT;
				m_dwStateStartTime = dwNow;
				SendBattleResultToAll(finalWinner);
			}
			else
			{
				// Next round
				m_dwRoundStartTime = dwNow;
				for (int i = 0; i < SLOT_MAX_COUNT; i++)
				{
					if (m_pSlotSession[i])
						m_SlotStats[i].bAlive = true;
				}

				// Reset destroy mode generators for next round
				if (m_ui8GameMode == STAGE_MODE_DESTROY)
				{
					for (int i = 0; i < GENERATOR_COUNT_MAX; i++)
					{
						m_i32GeneratorHP[i] = m_i32GeneratorMaxHP;
						m_bGeneratorDestroyed[i] = false;
					}
				}

				if (IsAtkDefMode(m_ui8GameMode) &&
					(m_ui8InfoFlag & ROOM_INFO_FLAG_ATK_DEF_AUTO_CHANGE))
					SwapTeams();

				// Re-select VIP for escape mode
				if (m_ui8GameMode == STAGE_MODE_ESCAPE)
					SelectVIP();

				// Broadcast round end/start
				BroadcastRoomStateChange();
			}
		}
	}
}

void Room::OnUpdateRoom_BattleResult(DWORD dwNow)
{
	// Display result screen for BATTLE_RESULT_DISPLAY_TIME seconds
	DWORD dwElapsed = dwNow - m_dwStateStartTime;
	if (dwElapsed >= BATTLE_RESULT_DISPLAY_TIME * 1000)
	{
		m_ui8RoomState = ROOM_STATE_BATTLE_END;
		m_dwStateStartTime = dwNow;

		printf("[Room] BATTLE_RESULT -> BATTLE_END - Room=%d\n", m_i32RoomIdx);
	}
}

void Room::OnUpdateRoom_BattleEnd(DWORD dwNow)
{
	// Brief cleanup phase, then back to READY
	DWORD dwElapsed = dwNow - m_dwStateStartTime;
	if (dwElapsed >= 1000)	// 1 second cleanup
	{
		OnEndBattle();

		// Return all players to ready room task
		for (int i = 0; i < SLOT_MAX_COUNT; i++)
		{
			if (m_pSlotSession[i])
				m_pSlotSession[i]->SetTask(GAME_TASK_READY_ROOM);
		}

		printf("[Room] BATTLE_END -> READY - Room=%d\n", m_i32RoomIdx);
	}
}

// ============================================================================
// Death & Kill Processing
// ============================================================================

void Room::OnPlayerDeath(int i32DeadSlot, int i32KillerSlot, uint32_t ui32WeaponId,
					 uint8_t ui8HitPart, float fX, float fY, float fZ, int i32AssistSlot)
{
	if (i32DeadSlot < 0 || i32DeadSlot >= SLOT_MAX_COUNT)
		return;
	if (!m_pSlotSession[i32DeadSlot])
		return;

	// Mark dead
	m_SlotStats[i32DeadSlot].bAlive = false;
	m_SlotStats[i32DeadSlot].i32Deaths++;
	m_SlotStats[i32DeadSlot].dwDeathTime = GetTickCount();
	m_SlotStats[i32DeadSlot].i32ConsecutiveKills = 0;	// Reset streak

	// In deathmatch, set respawn pending
	if (!IsMissionMode(m_ui8GameMode))
		m_SlotStats[i32DeadSlot].bRespawnPending = true;

	// Process killer
	uint8_t ui8MultiKill = MULTI_KILL_NONE;
	if (i32KillerSlot >= 0 && i32KillerSlot < SLOT_MAX_COUNT &&
		i32KillerSlot != i32DeadSlot && m_pSlotSession[i32KillerSlot])
	{
		m_SlotStats[i32KillerSlot].i32Kills++;
		m_SlotStats[i32KillerSlot].i32ConsecutiveKills++;

		if (m_SlotStats[i32KillerSlot].i32ConsecutiveKills > m_SlotStats[i32KillerSlot].i32MaxConsecutiveKills)
			m_SlotStats[i32KillerSlot].i32MaxConsecutiveKills = m_SlotStats[i32KillerSlot].i32ConsecutiveKills;

		ui8MultiKill = GetMultiKillType(m_SlotStats[i32KillerSlot].i32ConsecutiveKills);

		// Headshot tracking
		if (ui8HitPart == HIT_PART_HEAD)
			m_SlotStats[i32KillerSlot].i32Headshots++;

		// Add kill to team score (deathmatch only)
		if (!IsMissionMode(m_ui8GameMode))
		{
			const GameSlotInfo& killerInfo = m_Slots[i32KillerSlot];
			OnAddKill(killerInfo.ui8Team);
		}
	}

	// Process assist
	if (i32AssistSlot >= 0 && i32AssistSlot < SLOT_MAX_COUNT &&
		i32AssistSlot != i32KillerSlot && i32AssistSlot != i32DeadSlot &&
		m_pSlotSession[i32AssistSlot])
	{
		m_SlotStats[i32AssistSlot].i32Assists++;
	}

	// Broadcast death packet to all players
	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BATTLE_DEATH_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	uint8_t deadSlot = (uint8_t)i32DeadSlot;
	uint8_t killerSlot = (uint8_t)(i32KillerSlot >= 0 ? i32KillerSlot : 0xFF);
	uint8_t assistSlot = (uint8_t)(i32AssistSlot >= 0 ? i32AssistSlot : 0xFF);
	uint8_t hitPart = ui8HitPart;

	memcpy(buffer + offset, &deadSlot, 1);		offset += 1;
	memcpy(buffer + offset, &killerSlot, 1);	offset += 1;
	memcpy(buffer + offset, &ui32WeaponId, 4);	offset += 4;
	memcpy(buffer + offset, &hitPart, 1);		offset += 1;
	memcpy(buffer + offset, &assistSlot, 1);	offset += 1;
	memcpy(buffer + offset, &ui8MultiKill, 1);	offset += 1;

	// Position
	memcpy(buffer + offset, &fX, 4);	offset += 4;
	memcpy(buffer + offset, &fY, 4);	offset += 4;
	memcpy(buffer + offset, &fZ, 4);	offset += 4;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendToAll(&packet);

	// Check deathmatch kill limit
	if (!IsMissionMode(m_ui8GameMode) && CheckMatchEnd())
	{
		int finalWinner = (m_Score.i32RedScore > m_Score.i32BlueScore) ? TEAM_RED : TEAM_BLUE;
		CalculateBattleRewards(finalWinner);
		m_ui8RoomState = ROOM_STATE_BATTLE_RESULT;
		m_dwStateStartTime = GetTickCount();
		SendBattleResultToAll(finalWinner);
	}
}

void Room::OnPlayerRespawn(int i32Slot)
{
	if (i32Slot < 0 || i32Slot >= SLOT_MAX_COUNT)
		return;
	if (!m_pSlotSession[i32Slot])
		return;

	// In mission mode, no respawn
	if (IsMissionMode(m_ui8GameMode))
		return;

	// Check respawn timer
	if (m_SlotStats[i32Slot].bRespawnPending)
	{
		DWORD dwNow = GetTickCount();
		int respawnMs = GetRespawnTime() * 1000;
		if ((dwNow - m_SlotStats[i32Slot].dwDeathTime) < (DWORD)respawnMs)
			return;	// Not ready yet
	}

	m_SlotStats[i32Slot].bAlive = true;
	m_SlotStats[i32Slot].bRespawnPending = false;

	// Broadcast respawn to all
	i3NetworkPacket packet;
	char buffer[16];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BATTLE_RESPAWN_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	uint8_t slotIdx = (uint8_t)i32Slot;
	memcpy(buffer + offset, &slotIdx, 1);	offset += 1;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendToAll(&packet);
}

bool Room::IsPlayerAlive(int i32Slot) const
{
	if (i32Slot < 0 || i32Slot >= SLOT_MAX_COUNT)
		return false;
	if (!m_pSlotSession[i32Slot])
		return false;
	return m_SlotStats[i32Slot].bAlive;
}

int Room::GetAliveCount(int i32Team) const
{
	int count = 0;
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i] && m_Slots[i].ui8Team == (uint8_t)i32Team &&
			m_SlotStats[i].bAlive)
		{
			count++;
		}
	}
	return count;
}

int Room::GetConsecutiveKills(int i32Slot) const
{
	if (i32Slot < 0 || i32Slot >= SLOT_MAX_COUNT)
		return 0;
	return m_SlotStats[i32Slot].i32ConsecutiveKills;
}

int Room::GetRespawnTime() const
{
	return GetRespawnTimeByType(m_ui8RespawnType);
}

// ============================================================================
// Slot state helpers
// ============================================================================

bool Room::AllSlotsInState(uint8_t ui8State) const
{
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i] && m_Slots[i].ui8State != ui8State)
			return false;
	}
	return true;
}

bool Room::AllSlotsMinState(uint8_t ui8MinState) const
{
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i] && m_Slots[i].ui8State < ui8MinState)
			return false;
	}
	return true;
}

int Room::CountSlotsInState(uint8_t ui8State) const
{
	int count = 0;
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i] && m_Slots[i].ui8State == ui8State)
			count++;
	}
	return count;
}

void Room::SetSlotState(int i32Slot, uint8_t ui8State)
{
	if (i32Slot >= 0 && i32Slot < SLOT_MAX_COUNT)
		m_Slots[i32Slot].ui8State = ui8State;
}

// ============================================================================
// Battle Result & Rewards
// ============================================================================

void Room::CalculateBattleRewards(int i32WinnerTeam)
{
	// EXP/Point constants
	const uint16_t BASE_EXP = 50;
	const uint16_t WIN_BONUS_EXP = 30;
	const uint16_t KILL_EXP = 5;
	const uint16_t HEADSHOT_EXP = 3;
	const uint16_t ASSIST_EXP = 2;

	const uint16_t BASE_POINT = 30;
	const uint16_t WIN_BONUS_POINT = 20;
	const uint16_t KILL_POINT = 3;
	const uint16_t HEADSHOT_POINT = 2;
	const uint16_t ASSIST_POINT = 1;

	// Boost event multipliers (Phase 14B)
	uint16_t expMultiplier = 100;
	uint16_t pointMultiplier = 100;
	if (g_pContextMain)
	{
		expMultiplier = g_pContextMain->GetCurrentExpMultiplier();
		pointMultiplier = g_pContextMain->GetCurrentPointMultiplier();
	}

	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (!m_pSlotSession[i])
			continue;

		SlotBattleStats& stats = m_SlotStats[i];
		bool bWin = (i32WinnerTeam >= 0 && m_Slots[i].ui8Team == (uint8_t)i32WinnerTeam);

		uint32_t totalExp = BASE_EXP;
		uint32_t totalPoint = BASE_POINT;

		if (bWin)
		{
			totalExp += WIN_BONUS_EXP;
			totalPoint += WIN_BONUS_POINT;
		}

		totalExp += (uint32_t)(stats.i32Kills * KILL_EXP);
		totalPoint += (uint32_t)(stats.i32Kills * KILL_POINT);

		totalExp += (uint32_t)(stats.i32Headshots * HEADSHOT_EXP);
		totalPoint += (uint32_t)(stats.i32Headshots * HEADSHOT_POINT);

		totalExp += (uint32_t)(stats.i32Assists * ASSIST_EXP);
		totalPoint += (uint32_t)(stats.i32Assists * ASSIST_POINT);

		// Apply boost event multipliers (Phase 14B)
		totalExp = totalExp * expMultiplier / 100;
		totalPoint = totalPoint * pointMultiplier / 100;

		// Cap to uint16_t range
		if (totalExp > 0xFFFF) totalExp = 0xFFFF;
		if (totalPoint > 0xFFFF) totalPoint = 0xFFFF;

		stats.ui16AccExp = (uint16_t)totalExp;
		stats.ui16AccPoint = (uint16_t)totalPoint;

		// Apply to session
		m_pSlotSession[i]->ApplyBattleResult(
			stats.i32Kills, stats.i32Deaths, stats.i32Headshots, bWin);
	}
}

void Room::SendBattleResultToAll(int i32WinnerTeam)
{
	// Build BATTLE_ENDBATTLE_ACK packet with full per-player stats
	i3NetworkPacket packet;
	char buffer[2048];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BATTLE_ENDBATTLE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	// Winner
	uint8_t winner = (uint8_t)(i32WinnerTeam < 0 ? 0xFF : i32WinnerTeam);
	memcpy(buffer + offset, &winner, 1);	offset += 1;

	// Battle end user flag (bitmask of occupied slots)
	uint16_t battleEndUserFlag = 0;
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i])
			battleEndUserFlag |= (1 << i);
	}
	memcpy(buffer + offset, &battleEndUserFlag, 2);	offset += 2;

	// Total round count per team
	uint16_t redRounds = (uint16_t)m_Score.i32RedScore;
	uint16_t blueRounds = (uint16_t)m_Score.i32BlueScore;
	memcpy(buffer + offset, &redRounds, 2);		offset += 2;
	memcpy(buffer + offset, &blueRounds, 2);	offset += 2;

	// Per-slot accumulated EXP
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		uint16_t exp = m_pSlotSession[i] ? m_SlotStats[i].ui16AccExp : 0;
		memcpy(buffer + offset, &exp, 2);	offset += 2;
	}

	// Per-slot accumulated Points
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		uint16_t pt = m_pSlotSession[i] ? m_SlotStats[i].ui16AccPoint : 0;
		memcpy(buffer + offset, &pt, 2);	offset += 2;
	}

	// Per-slot result icon (0=loss, 1=win, 2=draw, 3=MVP)
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		uint8_t icon = 0;
		if (m_pSlotSession[i])
		{
			if (i32WinnerTeam < 0)
				icon = 2;	// Draw
			else if (m_Slots[i].ui8Team == (uint8_t)i32WinnerTeam)
				icon = 1;	// Win
		}
		memcpy(buffer + offset, &icon, 1);	offset += 1;
	}

	// Per-slot mission count (kills for DM, round kills for mission)
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		uint8_t missionCount = m_pSlotSession[i] ? (uint8_t)m_SlotStats[i].i32Kills : 0;
		memcpy(buffer + offset, &missionCount, 1);	offset += 1;
	}

	// Per-slot challenge score (headshots as a simple metric)
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		uint16_t challengeScore = m_pSlotSession[i] ? (uint16_t)m_SlotStats[i].i32Headshots : 0;
		memcpy(buffer + offset, &challengeScore, 2);	offset += 2;
	}

	// Bonus EXP array [SLOT_MAX_COUNT * TYPE_BATTLE_RESULT_EVENT_COUNT]
	for (int i = 0; i < SLOT_MAX_COUNT * TYPE_BATTLE_RESULT_EVENT_COUNT; i++)
	{
		uint16_t bonus = 0;
		memcpy(buffer + offset, &bonus, 2);	offset += 2;
	}

	// Bonus Point array [SLOT_MAX_COUNT * TYPE_BATTLE_RESULT_EVENT_COUNT]
	for (int i = 0; i < SLOT_MAX_COUNT * TYPE_BATTLE_RESULT_EVENT_COUNT; i++)
	{
		uint16_t bonus = 0;
		memcpy(buffer + offset, &bonus, 2);	offset += 2;
	}

	// Battle time in seconds
	uint32_t battleTime = 0;
	if (m_dwBattleStartTime > 0)
		battleTime = (GetTickCount() - m_dwBattleStartTime) / 1000;
	memcpy(buffer + offset, &battleTime, 4);	offset += 4;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendToAll(&packet);

	printf("[Room] Battle result sent - Room=%d, Winner=%s, Score R%d-B%d, Time=%ds\n",
		m_i32RoomIdx,
		winner == TEAM_RED ? "RED" :
		winner == TEAM_BLUE ? "BLUE" : "DRAW",
		m_Score.i32RedScore, m_Score.i32BlueScore, battleTime);
}

// ============================================================================
// Bomb Mode (Phase 2A)
// ============================================================================

void Room::OnBombInstall(int i32InstallerSlot, uint8_t ui8BombArea)
{
	if (m_ui8GameMode != STAGE_MODE_BOMB)
		return;
	if (m_bBombInstalled)
		return;		// Already installed

	m_bBombInstalled = true;
	m_i32BombInstallerSlot = i32InstallerSlot;
	m_ui8BombArea = ui8BombArea;
	m_dwBombInstallTime = GetTickCount();

	printf("[Room] Bomb installed - Room=%d, Slot=%d, Area=%c\n",
		m_i32RoomIdx, i32InstallerSlot, (ui8BombArea == BOMB_AREA_A) ? 'A' : 'B');

	// Broadcast BOMB_INSTALL_ACK to all
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BATTLE_MISSION_BOMB_INSTALL_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	uint8_t installerSlot = (uint8_t)i32InstallerSlot;
	memcpy(buffer + offset, &installerSlot, 1);		offset += 1;
	memcpy(buffer + offset, &ui8BombArea, 1);		offset += 1;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendToAll(&packet);
}

void Room::OnBombUninstall(int i32DefuserSlot)
{
	if (m_ui8GameMode != STAGE_MODE_BOMB)
		return;
	if (!m_bBombInstalled)
		return;		// Nothing to defuse

	m_bBombInstalled = false;
	m_i32BombInstallerSlot = -1;
	m_dwBombInstallTime = 0;

	printf("[Room] Bomb defused - Room=%d, Defuser=%d\n", m_i32RoomIdx, i32DefuserSlot);

	// Broadcast BOMB_UNINSTALL_ACK to all
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BATTLE_MISSION_BOMB_UNINSTALL_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	uint8_t defuserSlot = (uint8_t)i32DefuserSlot;
	memcpy(buffer + offset, &defuserSlot, 1);		offset += 1;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendToAll(&packet);

	// Bomb defused = DEF (BLUE) wins the round
	OnRoundEnd(TEAM_BLUE);

	if (CheckMatchEnd())
	{
		int finalWinner = (m_Score.i32RedScore > m_Score.i32BlueScore) ? TEAM_RED : TEAM_BLUE;
		CalculateBattleRewards(finalWinner);
		m_ui8RoomState = ROOM_STATE_BATTLE_RESULT;
		m_dwStateStartTime = GetTickCount();
		SendBattleResultToAll(finalWinner);
	}
	else
	{
		// Next round with team swap
		m_dwRoundStartTime = GetTickCount();
		for (int i = 0; i < SLOT_MAX_COUNT; i++)
		{
			if (m_pSlotSession[i])
				m_SlotStats[i].bAlive = true;
		}

		if (m_ui8InfoFlag & ROOM_INFO_FLAG_ATK_DEF_AUTO_CHANGE)
			SwapTeams();

		BroadcastRoomStateChange();
	}
}

void Room::OnBombExplode()
{
	if (!m_bBombInstalled)
		return;

	m_bBombInstalled = false;
	m_dwBombInstallTime = 0;

	printf("[Room] Bomb exploded - Room=%d, Area=%c\n",
		m_i32RoomIdx, (m_ui8BombArea == BOMB_AREA_A) ? 'A' : 'B');

	// Bomb explosion = ATK (RED) wins the round
	OnRoundEnd(TEAM_RED);

	if (CheckMatchEnd())
	{
		int finalWinner = (m_Score.i32RedScore > m_Score.i32BlueScore) ? TEAM_RED : TEAM_BLUE;
		CalculateBattleRewards(finalWinner);
		m_ui8RoomState = ROOM_STATE_BATTLE_RESULT;
		m_dwStateStartTime = GetTickCount();
		SendBattleResultToAll(finalWinner);
	}
	else
	{
		// Next round with team swap
		m_dwRoundStartTime = GetTickCount();
		for (int i = 0; i < SLOT_MAX_COUNT; i++)
		{
			if (m_pSlotSession[i])
				m_SlotStats[i].bAlive = true;
		}

		if (m_ui8InfoFlag & ROOM_INFO_FLAG_ATK_DEF_AUTO_CHANGE)
			SwapTeams();

		BroadcastRoomStateChange();
	}
}

void Room::SwapTeams()
{
	// Swap RED/BLUE for all players (ATK/DEF role swap)
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i])
		{
			m_Slots[i].ui8Team = (m_Slots[i].ui8Team == TEAM_RED) ? TEAM_BLUE : TEAM_RED;
		}
	}
	m_bAtkDefSwap = !m_bAtkDefSwap;

	printf("[Room] Teams swapped - Room=%d, SwapState=%d\n", m_i32RoomIdx, m_bAtkDefSwap ? 1 : 0);
}

// ============================================================================
// Destroy Mode (Phase 2C) - Generator HP tracking and destruction
// ============================================================================

void Room::OnGeneratorDamage(int i32GeneratorIdx, int i32Damage, int i32AttackerSlot)
{
	if (m_ui8GameMode != STAGE_MODE_DESTROY)
		return;

	if (i32GeneratorIdx < 0 || i32GeneratorIdx >= m_i32GeneratorCount)
		return;

	if (m_bGeneratorDestroyed[i32GeneratorIdx])
		return;

	if (i32Damage <= 0)
		return;

	m_i32GeneratorHP[i32GeneratorIdx] -= i32Damage;

	// Broadcast generator HP update to all players
	{
		i3NetworkPacket packet;
		char buffer[32];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_BATTLE_MISSION_GENERATOR_INFO_ACK;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

		uint8_t genIdx = (uint8_t)i32GeneratorIdx;
		int32_t hp = m_i32GeneratorHP[i32GeneratorIdx];
		int32_t maxHp = m_i32GeneratorMaxHP;

		memcpy(buffer + offset, &genIdx, 1);		offset += 1;
		memcpy(buffer + offset, &hp, 4);			offset += 4;
		memcpy(buffer + offset, &maxHp, 4);			offset += 4;

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		SendToAll(&packet);
	}

	if (m_i32GeneratorHP[i32GeneratorIdx] <= 0)
	{
		m_i32GeneratorHP[i32GeneratorIdx] = 0;
		OnGeneratorDestroy(i32GeneratorIdx);
	}
}

void Room::OnGeneratorDestroy(int i32GeneratorIdx)
{
	if (i32GeneratorIdx < 0 || i32GeneratorIdx >= m_i32GeneratorCount)
		return;

	m_bGeneratorDestroyed[i32GeneratorIdx] = true;

	printf("[Room] Generator destroyed - Room=%d, Generator=%d\n",
		m_i32RoomIdx, i32GeneratorIdx);

	// Broadcast generator destroy
	{
		i3NetworkPacket packet;
		char buffer[16];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_BATTLE_MISSION_GENERATOR_DESTROY_ACK;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

		uint8_t genIdx = (uint8_t)i32GeneratorIdx;
		memcpy(buffer + offset, &genIdx, 1);		offset += 1;

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		SendToAll(&packet);
	}

	// Check if all generators are destroyed -> ATK wins the round
	bool allDestroyed = true;
	for (int i = 0; i < m_i32GeneratorCount; i++)
	{
		if (!m_bGeneratorDestroyed[i])
		{
			allDestroyed = false;
			break;
		}
	}

	if (allDestroyed)
	{
		// ATK (RED) wins by destroying all objectives
		OnRoundEnd(TEAM_RED);

		DWORD dwNow = GetTickCount();
		if (CheckMatchEnd())
		{
			int finalWinner = (m_Score.i32RedScore > m_Score.i32BlueScore) ? TEAM_RED : TEAM_BLUE;
			CalculateBattleRewards(finalWinner);
			m_ui8RoomState = ROOM_STATE_BATTLE_RESULT;
			m_dwStateStartTime = dwNow;
			SendBattleResultToAll(finalWinner);
		}
		else
		{
			// Reset for next round
			m_dwRoundStartTime = dwNow;
			for (int i = 0; i < GENERATOR_COUNT_MAX; i++)
			{
				m_i32GeneratorHP[i] = m_i32GeneratorMaxHP;
				m_bGeneratorDestroyed[i] = false;
			}
			for (int i = 0; i < SLOT_MAX_COUNT; i++)
			{
				if (m_pSlotSession[i])
					m_SlotStats[i].bAlive = true;
			}
			if (m_ui8InfoFlag & ROOM_INFO_FLAG_ATK_DEF_AUTO_CHANGE)
				SwapTeams();
			BroadcastRoomStateChange();
		}
	}
}

int Room::GetGeneratorHP(int i32Idx) const
{
	if (i32Idx < 0 || i32Idx >= m_i32GeneratorCount)
		return 0;
	return m_i32GeneratorHP[i32Idx];
}

// ============================================================================
// Escape/VIP Mode (Phase 2E)
// ============================================================================

void Room::SelectVIP()
{
	if (m_ui8GameMode != STAGE_MODE_ESCAPE)
		return;

	m_i32VIPSlot = ESCAPE_VIP_SLOT_NONE;

	// Collect ATK (RED) team members
	int atkSlots[SLOT_MAX_COUNT];
	int atkCount = 0;
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i] && m_Slots[i].ui8Team == TEAM_RED)
		{
			atkSlots[atkCount++] = i;
		}
	}

	if (atkCount == 0)
		return;

	// Random selection
	m_i32VIPSlot = atkSlots[rand() % atkCount];

	printf("[Room] VIP selected - Room=%d, Slot=%d\n", m_i32RoomIdx, m_i32VIPSlot);

	// Broadcast VIP selection to all players
	{
		i3NetworkPacket packet;
		char buffer[16];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_BATTLE_MISSION_TOUCHDOWN_ACK;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

		uint8_t vipSlot = (uint8_t)m_i32VIPSlot;
		uint8_t action = 0;	// 0 = VIP selected
		memcpy(buffer + offset, &action, 1);		offset += 1;
		memcpy(buffer + offset, &vipSlot, 1);		offset += 1;

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		SendToAll(&packet);
	}
}

void Room::OnTouchdown(int i32Slot)
{
	if (m_ui8GameMode != STAGE_MODE_ESCAPE)
		return;

	if (i32Slot != m_i32VIPSlot)
		return;		// Only VIP can touchdown

	if (!IsPlayerAlive(i32Slot))
		return;

	printf("[Room] VIP touchdown! - Room=%d, VIPSlot=%d\n", m_i32RoomIdx, i32Slot);

	// Broadcast touchdown
	{
		i3NetworkPacket packet;
		char buffer[16];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_BATTLE_MISSION_TOUCHDOWN_ACK;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

		uint8_t vipSlot = (uint8_t)m_i32VIPSlot;
		uint8_t action = 1;	// 1 = touchdown success
		memcpy(buffer + offset, &action, 1);		offset += 1;
		memcpy(buffer + offset, &vipSlot, 1);		offset += 1;

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		SendToAll(&packet);
	}

	// ATK wins the round via touchdown
	OnRoundEnd(TEAM_RED);

	DWORD dwNow = GetTickCount();
	if (CheckMatchEnd())
	{
		int finalWinner = (m_Score.i32RedScore > m_Score.i32BlueScore) ? TEAM_RED : TEAM_BLUE;
		CalculateBattleRewards(finalWinner);
		m_ui8RoomState = ROOM_STATE_BATTLE_RESULT;
		m_dwStateStartTime = dwNow;
		SendBattleResultToAll(finalWinner);
	}
	else
	{
		// Next round - reset, re-select VIP
		m_dwRoundStartTime = dwNow;
		for (int i = 0; i < SLOT_MAX_COUNT; i++)
		{
			if (m_pSlotSession[i])
				m_SlotStats[i].bAlive = true;
		}
		if (m_ui8InfoFlag & ROOM_INFO_FLAG_ATK_DEF_AUTO_CHANGE)
			SwapTeams();
		SelectVIP();
		BroadcastRoomStateChange();
	}
}

void Room::BroadcastRoomStateChange()
{
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BATTLE_MISSION_ROUND_END_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	uint8_t winner = (m_Score.i32RedScore > m_Score.i32BlueScore) ? TEAM_RED : TEAM_BLUE;
	int32_t redScore = m_Score.i32RedScore;
	int32_t blueScore = m_Score.i32BlueScore;
	int32_t nowRound = m_Score.i32NowRound;

	memcpy(buffer + offset, &winner, 1);		offset += 1;
	memcpy(buffer + offset, &redScore, 4);		offset += 4;
	memcpy(buffer + offset, &blueScore, 4);		offset += 4;
	memcpy(buffer + offset, &nowRound, 4);		offset += 4;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendToAll(&packet);
}

// ============================================================================
// Kick Vote System (Phase 3A)
// ============================================================================

#define KICK_VOTE_TIMEOUT_MS		20000	// 20 seconds to vote
#define KICK_VOTE_COOLDOWN_MS		60000	// 60 seconds between votes

bool Room::StartKickVote(int suggestSlot, int targetSlot)
{
	// Already a vote in progress
	if (m_bKickVoteActive)
		return false;

	// Cooldown check
	DWORD dwNow = GetTickCount();
	if (m_dwLastKickVoteTime > 0 && (dwNow - m_dwLastKickVoteTime) < KICK_VOTE_COOLDOWN_MS)
		return false;

	m_bKickVoteActive = true;
	m_i32KickVoteTarget = targetSlot;
	m_i32KickVoteSuggestor = suggestSlot;
	m_dwKickVoteStartTime = dwNow;
	memset(m_KickVotes, 0, sizeof(m_KickVotes));

	// Suggestor automatically votes agree
	m_KickVotes[suggestSlot] = 1;

	return true;
}

bool Room::CastKickVote(int voterSlot, int targetSlot, uint8_t vote)
{
	if (!m_bKickVoteActive || targetSlot != m_i32KickVoteTarget)
		return false;

	if (voterSlot < 0 || voterSlot >= SLOT_MAX_COUNT)
		return false;

	// Can't vote on yourself being kicked
	if (voterSlot == m_i32KickVoteTarget)
		return false;

	// Already voted
	if (m_KickVotes[voterSlot] != 0)
		return false;

	m_KickVotes[voterSlot] = (vote > 0) ? 1 : 2;	// 1=agree, 2=disagree

	// Check if all eligible players have voted
	int totalVoters = 0, totalVoted = 0;
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i] && i != m_i32KickVoteTarget)
		{
			totalVoters++;
			if (m_KickVotes[i] != 0)
				totalVoted++;
		}
	}

	// All votes are in - resolve
	if (totalVoted >= totalVoters)
		ResolveKickVote();

	return true;
}

void Room::ResolveKickVote()
{
	if (!m_bKickVoteActive)
		return;

	int agrees = 0, disagrees = 0;
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_KickVotes[i] == 1)
			agrees++;
		else if (m_KickVotes[i] == 2)
			disagrees++;
	}

	// Majority vote wins
	if (agrees > disagrees && m_i32KickVoteTarget >= 0 &&
		m_i32KickVoteTarget < SLOT_MAX_COUNT && m_pSlotSession[m_i32KickVoteTarget])
	{
		OnKickPlayer(nullptr, m_i32KickVoteTarget);
		printf("[Room] Kick vote passed - target slot %d kicked (%d agree, %d disagree)\n",
			m_i32KickVoteTarget, agrees, disagrees);
	}
	else
	{
		printf("[Room] Kick vote failed - target slot %d stays (%d agree, %d disagree)\n",
			m_i32KickVoteTarget, agrees, disagrees);
	}

	// Reset vote state
	m_bKickVoteActive = false;
	m_i32KickVoteTarget = -1;
	m_i32KickVoteSuggestor = -1;
	m_dwLastKickVoteTime = GetTickCount();
	memset(m_KickVotes, 0, sizeof(m_KickVotes));
}
