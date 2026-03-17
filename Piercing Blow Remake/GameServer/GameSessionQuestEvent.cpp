#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"

// ============================================================================
// Quest Event Handlers (1.5 quest system - event, honor, period quests)
// Protocol section: PROTOCOL_GET_ALL_QUEST_INFO, PROTOCOL_GET_HONOR_QUEST_INFO,
//                   PROTOCOL_PERIOD_QUEST_SETTING
// ============================================================================

// ----------------------------------------------------------------------------
// Helper: Serialize a single quest objective into buffer
// Returns number of bytes written
// ----------------------------------------------------------------------------
static int WriteQuestObjective(char* buffer, const GameQuestObjective& obj)
{
	int offset = 0;
	memcpy(buffer + offset, &obj.ui8Condition, 1);			offset += 1;
	memcpy(buffer + offset, &obj.ui32Param, 4);			offset += 4;
	memcpy(buffer + offset, &obj.ui32Target, 4);			offset += 4;
	memcpy(buffer + offset, &obj.ui32Current, 4);			offset += 4;
	return offset;
}

// ----------------------------------------------------------------------------
// Helper: Update a single quest objective based on battle results
// ----------------------------------------------------------------------------
static void UpdateObjectiveProgress(GameQuestObjective& obj,
	int kills, int deaths, int headshots, bool won,
	uint32_t weaponClass, uint32_t stageId,
	int bombPlants, int bombDefuses, int multiKills,
	int consecutiveKills, int playTimeMinutes, int damageDealt)
{
	if (obj.ui32Target == 0 || obj.IsComplete())
		return;

	switch (obj.ui8Condition)
	{
	case QUEST_COND_KILLS:
		obj.ui32Current += kills;
		break;
	case QUEST_COND_HEADSHOTS:
		obj.ui32Current += headshots;
		break;
	case QUEST_COND_WINS:
		if (won) obj.ui32Current++;
		break;
	case QUEST_COND_GAMES_PLAYED:
		obj.ui32Current++;
		break;
	case QUEST_COND_DEATHS:
		obj.ui32Current += deaths;
		break;
	case QUEST_COND_ASSISTS:
		// Assists tracked separately; placeholder for future integration
		break;
	case QUEST_COND_WEAPON_KILLS:
		if (weaponClass == obj.ui32Param)
			obj.ui32Current += kills;
		break;
	case QUEST_COND_STAGE_WINS:
		if (stageId == obj.ui32Param && won)
			obj.ui32Current++;
		break;
	case QUEST_COND_BOMB_PLANTS:
		obj.ui32Current += bombPlants;
		break;
	case QUEST_COND_BOMB_DEFUSES:
		obj.ui32Current += bombDefuses;
		break;
	case QUEST_COND_MULTI_KILLS:
		// ui32Param = min kill count per multi-kill event
		// multiKills = number of multi-kill events this match that meet threshold
		obj.ui32Current += multiKills;
		break;
	case QUEST_COND_CONSECUTIVE_KILLS:
		// ui32Param = min streak length required
		// consecutiveKills = longest streak this match
		if ((uint32_t)consecutiveKills >= obj.ui32Param)
			obj.ui32Current++;
		break;
	case QUEST_COND_PLAY_TIME:
		obj.ui32Current += playTimeMinutes;
		break;
	case QUEST_COND_DAMAGE_DEALT:
		obj.ui32Current += damageDealt;
		break;
	}

	// Cap at target
	if (obj.ui32Current > obj.ui32Target)
		obj.ui32Current = obj.ui32Target;
}

// ============================================================================
// OnGetAllQuestInfoReq
// PROTOCOL_GET_ALL_QUEST_INFO_REQ -> PROTOCOL_GET_ALL_QUEST_INFO_ACK
// Sends all quest data: regular cardsets + event quests + period quests
// ============================================================================
void GameSession::OnGetAllQuestInfoReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	i3NetworkPacket packet;
	char buffer[8192];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_GET_ALL_QUEST_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));			offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));			offset += sizeof(int32_t);

	// --- Section 1: Regular quest card sets ---
	memcpy(buffer + offset, &m_QuestData.ui8ActiveSetIndex, 1);	offset += 1;

	uint8_t cardSetCount = 0;
	for (int i = 0; i < MAX_CARDSET_PER_USER; i++)
	{
		if (m_QuestData.cardSets[i].IsActive())
			cardSetCount++;
	}
	memcpy(buffer + offset, &cardSetCount, 1);					offset += 1;

	for (int i = 0; i < MAX_CARDSET_PER_USER; i++)
	{
		const GameQuestCardSet& cs = m_QuestData.cardSets[i];
		if (!cs.IsActive())
			continue;

		uint8_t setIdx = (uint8_t)i;
		memcpy(buffer + offset, &setIdx, 1);					offset += 1;
		memcpy(buffer + offset, &cs.ui8Type, 1);				offset += 1;
		memcpy(buffer + offset, &cs.ui8ActiveCard, 1);			offset += 1;

		// Send completion state for each card
		for (int c = 0; c < MAX_CARD_PER_CARDSET; c++)
		{
			uint8_t complete = cs.cards[c].bComplete ? 1 : 0;
			memcpy(buffer + offset, &complete, 1);				offset += 1;
		}
	}

	// --- Section 2: Event quests ---
	uint8_t eventCount = (uint8_t)m_EventQuestData.i32EventQuestCount;
	memcpy(buffer + offset, &eventCount, 1);					offset += 1;

	for (int i = 0; i < m_EventQuestData.i32EventQuestCount && i < MAX_EVENT_QUEST_COUNT; i++)
	{
		const GameEventQuest& eq = m_EventQuestData.eventQuests[i];
		if (offset + 64 > (int)sizeof(buffer))
			break;

		memcpy(buffer + offset, &eq.ui32QuestId, 4);			offset += 4;
		memcpy(buffer + offset, &eq.ui8Type, 1);				offset += 1;
		memcpy(buffer + offset, &eq.ui8ObjectiveCount, 1);		offset += 1;

		for (int o = 0; o < eq.ui8ObjectiveCount && o < MAX_QUEST_OBJECTIVES; o++)
		{
			offset += WriteQuestObjective(buffer + offset, eq.objectives[o]);
		}

		memcpy(buffer + offset, &eq.ui32RewardGP, 4);			offset += 4;
		memcpy(buffer + offset, &eq.ui32RewardExp, 4);			offset += 4;
		memcpy(buffer + offset, &eq.ui32RewardItemId, 4);		offset += 4;
		memcpy(buffer + offset, &eq.ui32StartTime, 4);			offset += 4;
		memcpy(buffer + offset, &eq.ui32EndTime, 4);			offset += 4;

		uint8_t completed = eq.bCompleted ? 1 : 0;
		uint8_t claimed = eq.bRewardClaimed ? 1 : 0;
		memcpy(buffer + offset, &completed, 1);					offset += 1;
		memcpy(buffer + offset, &claimed, 1);					offset += 1;
	}

	// --- Section 3: Period quests ---
	uint8_t periodCount = (uint8_t)m_EventQuestData.i32PeriodQuestCount;
	memcpy(buffer + offset, &periodCount, 1);					offset += 1;

	for (int i = 0; i < m_EventQuestData.i32PeriodQuestCount && i < MAX_PERIOD_QUEST_COUNT; i++)
	{
		const GamePeriodQuest& pq = m_EventQuestData.periodQuests[i];
		if (offset + 64 > (int)sizeof(buffer))
			break;

		memcpy(buffer + offset, &pq.ui32QuestId, 4);			offset += 4;
		memcpy(buffer + offset, &pq.ui8Cycle, 1);				offset += 1;
		memcpy(buffer + offset, &pq.ui8ObjectiveCount, 1);		offset += 1;

		for (int o = 0; o < pq.ui8ObjectiveCount && o < MAX_QUEST_OBJECTIVES; o++)
		{
			offset += WriteQuestObjective(buffer + offset, pq.objectives[o]);
		}

		memcpy(buffer + offset, &pq.ui32RewardGP, 4);			offset += 4;
		memcpy(buffer + offset, &pq.ui32RewardExp, 4);			offset += 4;
		memcpy(buffer + offset, &pq.ui32RewardItemId, 4);		offset += 4;
		memcpy(buffer + offset, &pq.ui32ResetTime, 4);			offset += 4;

		uint8_t completed = pq.bCompleted ? 1 : 0;
		uint8_t claimed = pq.bRewardClaimed ? 1 : 0;
		uint8_t selected = pq.bSelected ? 1 : 0;
		memcpy(buffer + offset, &completed, 1);					offset += 1;
		memcpy(buffer + offset, &claimed, 1);					offset += 1;
		memcpy(buffer + offset, &selected, 1);					offset += 1;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);

	printf("[GameSession] AllQuestInfo sent - UID=%lld, EventQ=%d, PeriodQ=%d\n",
		m_i64UID, m_EventQuestData.i32EventQuestCount, m_EventQuestData.i32PeriodQuestCount);
}

// ============================================================================
// OnGetHonorQuestInfoReq
// PROTOCOL_GET_HONOR_QUEST_INFO_REQ -> PROTOCOL_GET_HONOT_QUEST_INFO_ACK
// Sends honor quest data (prestige quests for high-rank players)
// Note: ACK protocol has typo "HONOT" from original ProtocolDef.h
// ============================================================================
void GameSession::OnGetHonorQuestInfoReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	i3NetworkPacket packet;
	char buffer[4096];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_GET_HONOT_QUEST_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));			offset += sizeof(uint16_t);

	int32_t result = 0;

	// Check if player rank qualifies for any honor quests
	int qualifiedCount = 0;
	for (int i = 0; i < m_EventQuestData.i32HonorQuestCount && i < MAX_HONOR_QUEST_COUNT; i++)
	{
		const GameHonorQuest& hq = m_EventQuestData.honorQuests[i];
		if (hq.ui32QuestId == 0)
			continue;

		// Check rank requirement against player rank
		// Rank thresholds: SERGEANT=10, LIEUTENANT=20, CAPTAIN=30, MAJOR=40, COLONEL=50, GENERAL=60
		int requiredRankId = hq.ui8RequiredRank * 10;
		if (m_i32RankId >= requiredRankId)
			qualifiedCount++;
	}

	memcpy(buffer + offset, &result, sizeof(int32_t));			offset += sizeof(int32_t);

	// Player's current rank
	int32_t rankId = m_i32RankId;
	memcpy(buffer + offset, &rankId, 4);						offset += 4;

	// Honor quest count (only those the player qualifies for)
	uint8_t honorCount = (uint8_t)qualifiedCount;
	memcpy(buffer + offset, &honorCount, 1);					offset += 1;

	for (int i = 0; i < m_EventQuestData.i32HonorQuestCount && i < MAX_HONOR_QUEST_COUNT; i++)
	{
		const GameHonorQuest& hq = m_EventQuestData.honorQuests[i];
		if (hq.ui32QuestId == 0)
			continue;

		int requiredRankId = hq.ui8RequiredRank * 10;
		if (m_i32RankId < requiredRankId)
			continue;

		if (offset + 64 > (int)sizeof(buffer))
			break;

		memcpy(buffer + offset, &hq.ui32QuestId, 4);			offset += 4;
		memcpy(buffer + offset, &hq.ui8RequiredRank, 1);		offset += 1;
		memcpy(buffer + offset, &hq.ui8ObjectiveCount, 1);		offset += 1;

		for (int o = 0; o < hq.ui8ObjectiveCount && o < MAX_QUEST_OBJECTIVES; o++)
		{
			offset += WriteQuestObjective(buffer + offset, hq.objectives[o]);
		}

		memcpy(buffer + offset, &hq.ui32RewardGP, 4);			offset += 4;
		memcpy(buffer + offset, &hq.ui32RewardExp, 4);			offset += 4;
		memcpy(buffer + offset, &hq.ui32RewardItemId, 4);		offset += 4;
		memcpy(buffer + offset, &hq.ui32HonorPoints, 4);		offset += 4;

		uint8_t completed = hq.bCompleted ? 1 : 0;
		uint8_t claimed = hq.bRewardClaimed ? 1 : 0;
		memcpy(buffer + offset, &completed, 1);					offset += 1;
		memcpy(buffer + offset, &claimed, 1);					offset += 1;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);

	printf("[GameSession] HonorQuestInfo sent - UID=%lld, Rank=%d, Qualified=%d\n",
		m_i64UID, m_i32RankId, qualifiedCount);
}

// ============================================================================
// OnPeriodQuestSettingReq
// PROTOCOL_PERIOD_QUEST_SETTING_REQ -> PROTOCOL_PERIOD_QUEST_SETTING_ACK
// Player selects/activates a period quest from the available pool
// Payload: uint32_t questId + uint8_t action (1=select, 0=deselect)
// ============================================================================
void GameSession::OnPeriodQuestSettingReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 5)
		return;

	int readOfs = 0;
	uint32_t questId = 0;
	memcpy(&questId, pData + readOfs, 4);						readOfs += 4;
	uint8_t action = *(uint8_t*)(pData + readOfs);				readOfs += 1;

	int32_t result = 0;

	// Find the period quest by ID
	GamePeriodQuest* pFound = nullptr;
	for (int i = 0; i < m_EventQuestData.i32PeriodQuestCount && i < MAX_PERIOD_QUEST_COUNT; i++)
	{
		if (m_EventQuestData.periodQuests[i].ui32QuestId == questId)
		{
			pFound = &m_EventQuestData.periodQuests[i];
			break;
		}
	}

	if (pFound == nullptr)
	{
		result = 1;		// Quest not found
	}
	else if (pFound->bRewardClaimed)
	{
		result = 2;		// Already claimed reward
	}
	else
	{
		if (action == 1)
		{
			// Select this quest
			if (pFound->bSelected)
			{
				result = 3;		// Already selected
			}
			else
			{
				pFound->bSelected = true;
				printf("[GameSession] PeriodQuest selected - UID=%lld, QuestId=%u\n",
					m_i64UID, questId);
			}
		}
		else
		{
			// Deselect this quest (only if not yet started progress)
			if (!pFound->bSelected)
			{
				result = 4;		// Not selected
			}
			else
			{
				// Check if any progress has been made
				bool hasProgress = false;
				for (int o = 0; o < pFound->ui8ObjectiveCount && o < MAX_QUEST_OBJECTIVES; o++)
				{
					if (pFound->objectives[o].ui32Current > 0)
					{
						hasProgress = true;
						break;
					}
				}

				if (hasProgress)
				{
					result = 5;		// Cannot deselect quest with progress
				}
				else
				{
					pFound->bSelected = false;
					printf("[GameSession] PeriodQuest deselected - UID=%lld, QuestId=%u\n",
						m_i64UID, questId);
				}
			}
		}
	}

	// Send ACK
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_PERIOD_QUEST_SETTING_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));			offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));			offset += sizeof(int32_t);
	memcpy(buffer + offset, &questId, 4);						offset += 4;
	memcpy(buffer + offset, &action, 1);						offset += 1;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

// ============================================================================
// UpdateEventQuestProgress
// Called after a battle ends to update event/honor/period quest progress
// Extends the base UpdateQuestProgress with additional condition types
// ============================================================================
void GameSession::UpdateEventQuestProgress(int kills, int deaths, int headshots, bool won,
	uint32_t weaponClass, uint32_t stageId,
	int bombPlants, int bombDefuses, int multiKills,
	int consecutiveKills, int playTimeMinutes, int damageDealt)
{
	// --- Update event quests ---
	for (int i = 0; i < m_EventQuestData.i32EventQuestCount && i < MAX_EVENT_QUEST_COUNT; i++)
	{
		GameEventQuest& eq = m_EventQuestData.eventQuests[i];
		if (!eq.IsActive() || eq.bCompleted)
			continue;

		for (int o = 0; o < eq.ui8ObjectiveCount && o < MAX_QUEST_OBJECTIVES; o++)
		{
			UpdateObjectiveProgress(eq.objectives[o],
				kills, deaths, headshots, won,
				weaponClass, stageId,
				bombPlants, bombDefuses, multiKills,
				consecutiveKills, playTimeMinutes, damageDealt);
		}

		if (eq.CheckComplete())
		{
			printf("[GameSession] EventQuest completed - UID=%lld, QuestId=%u\n",
				m_i64UID, eq.ui32QuestId);
		}
	}

	// --- Update honor quests (only if player meets rank requirement) ---
	for (int i = 0; i < m_EventQuestData.i32HonorQuestCount && i < MAX_HONOR_QUEST_COUNT; i++)
	{
		GameHonorQuest& hq = m_EventQuestData.honorQuests[i];
		if (!hq.IsActive() || hq.bCompleted)
			continue;

		// Verify rank requirement
		int requiredRankId = hq.ui8RequiredRank * 10;
		if (m_i32RankId < requiredRankId)
			continue;

		for (int o = 0; o < hq.ui8ObjectiveCount && o < MAX_QUEST_OBJECTIVES; o++)
		{
			UpdateObjectiveProgress(hq.objectives[o],
				kills, deaths, headshots, won,
				weaponClass, stageId,
				bombPlants, bombDefuses, multiKills,
				consecutiveKills, playTimeMinutes, damageDealt);
		}

		if (hq.CheckComplete())
		{
			printf("[GameSession] HonorQuest completed - UID=%lld, QuestId=%u\n",
				m_i64UID, hq.ui32QuestId);

			// Send honor quest clear notification
			SendSimpleAck(PROTOCOL_HONOR_QUEST_CLEAR_ACK, 0);
		}
	}

	// --- Update period quests (only selected ones) ---
	for (int i = 0; i < m_EventQuestData.i32PeriodQuestCount && i < MAX_PERIOD_QUEST_COUNT; i++)
	{
		GamePeriodQuest& pq = m_EventQuestData.periodQuests[i];
		if (!pq.IsActive() || pq.bCompleted)
			continue;

		for (int o = 0; o < pq.ui8ObjectiveCount && o < MAX_QUEST_OBJECTIVES; o++)
		{
			UpdateObjectiveProgress(pq.objectives[o],
				kills, deaths, headshots, won,
				weaponClass, stageId,
				bombPlants, bombDefuses, multiKills,
				consecutiveKills, playTimeMinutes, damageDealt);
		}

		if (pq.CheckComplete())
		{
			printf("[GameSession] PeriodQuest completed - UID=%lld, QuestId=%u, Cycle=%d\n",
				m_i64UID, pq.ui32QuestId, pq.ui8Cycle);
		}
	}

	// --- Also update base quest system extended condition types ---
	for (int s = 0; s < MAX_CARDSET_PER_USER; s++)
	{
		GameQuestCardSet& cs = m_QuestData.cardSets[s];
		if (!cs.IsActive())
			continue;

		GameQuestCard& card = cs.cards[cs.ui8ActiveCard];
		if (card.bComplete)
			continue;

		for (int q = 0; q < MAX_QUEST_PER_CARD; q++)
		{
			GameQuestEntry& quest = card.quests[q];
			if (quest.ui32Target == 0 || quest.IsComplete())
				continue;

			// Handle the new extended condition types that base
			// UpdateQuestProgress does not cover
			switch (quest.ui8Condition)
			{
			case QUEST_COND_WEAPON_KILLS:
				if (weaponClass == quest.ui32Param)
					quest.ui32Current += kills;
				break;
			case QUEST_COND_STAGE_WINS:
				if (stageId == quest.ui32Param && won)
					quest.ui32Current++;
				break;
			case QUEST_COND_BOMB_PLANTS:
				quest.ui32Current += bombPlants;
				break;
			case QUEST_COND_BOMB_DEFUSES:
				quest.ui32Current += bombDefuses;
				break;
			case QUEST_COND_MULTI_KILLS:
				quest.ui32Current += multiKills;
				break;
			case QUEST_COND_CONSECUTIVE_KILLS:
				if ((uint32_t)consecutiveKills >= quest.ui32Param)
					quest.ui32Current++;
				break;
			case QUEST_COND_PLAY_TIME:
				quest.ui32Current += playTimeMinutes;
				break;
			case QUEST_COND_DAMAGE_DEALT:
				quest.ui32Current += damageDealt;
				break;
			default:
				// Base conditions (kills, headshots, wins, etc.) already
				// handled by UpdateQuestProgress - skip here
				continue;
			}

			// Cap at target
			if (quest.ui32Current > quest.ui32Target)
				quest.ui32Current = quest.ui32Target;
		}

		card.CheckComplete();
	}
}
