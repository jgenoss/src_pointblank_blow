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
	, m_ui8GameMode(STAGE_MODE_DEATHMATCH)
	, m_ui8MapIndex(0)
	, m_ui8RoundType(BATTLE_ROUND_TYPE_3)
	, m_ui8SubType(0)
	, m_ui8WeaponFlag(LOCK_BASIC)
	, m_ui8InfoFlag(0)
	, m_ui32OptionFlag(0)
	, m_bIsClanMatch(false)
	, m_dwBattleStartTime(0)
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

	m_ui8RoomState = ROOM_STATE_COUNTDOWN_R;

	// Set all slots to loading state
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (m_pSlotSession[i])
			m_Slots[i].ui8State = SLOT_STATE_LOAD;
	}

	m_dwBattleStartTime = GetTickCount();

	// Reset score for new match
	m_Score.Reset();
	m_Score.i32MaxRound = GetRoundCount(m_ui8RoundType);
	m_Score.ui16MaxTime = GetTimeLimit(m_ui8SubType, m_ui8GameMode);

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
	// Serialize in ROOM_INFO_BASIC compatible format
	if (!pBuffer || !pSize)
		return;

	int offset = 0;

	// _Idx
	memcpy(pBuffer + offset, &m_i32RoomIdx, sizeof(int32_t));		offset += sizeof(int32_t);
	// _Title
	memcpy(pBuffer + offset, m_szTitle, NET_ROOM_NAME_SIZE);		offset += NET_ROOM_NAME_SIZE;
	// _StageID
	memcpy(pBuffer + offset, &m_ui32StageId, sizeof(uint32_t));		offset += sizeof(uint32_t);
	// _State
	memcpy(pBuffer + offset, &m_ui8RoomState, sizeof(uint8_t));		offset += sizeof(uint8_t);
	// _PersonNow
	uint8_t personNow = (uint8_t)m_i32PlayerCount;
	memcpy(pBuffer + offset, &personNow, sizeof(uint8_t));			offset += sizeof(uint8_t);
	// _PersonPlayerMax
	uint8_t personMax = (uint8_t)m_i32MaxPlayers;
	memcpy(pBuffer + offset, &personMax, sizeof(uint8_t));			offset += sizeof(uint8_t);
	// _Ping
	uint8_t ping = 0;
	memcpy(pBuffer + offset, &ping, sizeof(uint8_t));				offset += sizeof(uint8_t);
	// _WeaponFlag
	memcpy(pBuffer + offset, &m_ui8WeaponFlag, sizeof(uint8_t));	offset += sizeof(uint8_t);
	// _InfoFlag
	memcpy(pBuffer + offset, &m_ui8InfoFlag, sizeof(uint8_t));		offset += sizeof(uint8_t);
	// _OptionFlag
	memcpy(pBuffer + offset, &m_ui32OptionFlag, sizeof(uint32_t));	offset += sizeof(uint32_t);
	// _IsClanMatch
	uint8_t isClan = m_bIsClanMatch ? 1 : 0;
	memcpy(pBuffer + offset, &isClan, sizeof(uint8_t));				offset += sizeof(uint8_t);

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
