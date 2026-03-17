#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"

// ============================================================================
// Quest Handlers (Protocol_Base 0x0200 quest section)
// ============================================================================

void GameSession::OnQuestGetReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BASE_QUEST_GET_REQ -> ACK
	// Send quest cardset overview to client
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	i3NetworkPacket packet;
	char buffer[1024];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_QUEST_GET_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Active cardset index
	memcpy(buffer + offset, &m_QuestData.ui8ActiveSetIndex, 1);	offset += 1;

	// Per-cardset: type(1) + activeCard(1)
	for (int i = 0; i < MAX_CARDSET_PER_USER; i++)
	{
		const GameQuestCardSet& cs = m_QuestData.cardSets[i];
		memcpy(buffer + offset, &cs.ui8Type, 1);			offset += 1;
		memcpy(buffer + offset, &cs.ui8ActiveCard, 1);		offset += 1;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnQuestGetInfoReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BASE_QUEST_GET_INFO_REQ -> ACK
	// Send detailed quest info for a specific cardset
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	if (i32Size < 1)
		return;

	uint8_t setIdx = *(uint8_t*)pData;
	if (setIdx >= MAX_CARDSET_PER_USER)
		return;

	const GameQuestCardSet& cs = m_QuestData.cardSets[setIdx];

	i3NetworkPacket packet;
	char buffer[4096];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_QUEST_GET_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Cardset index and type
	memcpy(buffer + offset, &setIdx, 1);					offset += 1;
	memcpy(buffer + offset, &cs.ui8Type, 1);				offset += 1;
	memcpy(buffer + offset, &cs.ui8ActiveCard, 1);			offset += 1;

	// Per-card data: 10 cards x 4 quests each
	for (int c = 0; c < MAX_CARD_PER_CARDSET; c++)
	{
		const GameQuestCard& card = cs.cards[c];
		uint8_t complete = card.bComplete ? 1 : 0;
		memcpy(buffer + offset, &complete, 1);				offset += 1;

		for (int q = 0; q < MAX_QUEST_PER_CARD; q++)
		{
			if (offset + 17 > (int)sizeof(buffer))
				break;

			const GameQuestEntry& quest = card.quests[q];
			memcpy(buffer + offset, &quest.ui8Condition, 1);	offset += 1;
			memcpy(buffer + offset, &quest.ui32Target, 4);		offset += 4;
			memcpy(buffer + offset, &quest.ui32Current, 4);	offset += 4;
			memcpy(buffer + offset, &quest.ui32RewardGP, 4);	offset += 4;
			memcpy(buffer + offset, &quest.ui32RewardExp, 4);	offset += 4;
		}
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnQuestGetItemReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BASE_QUEST_GET_ITEM_REQ -> ACK
	// Claim quest card reward
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 2)
		return;

	uint8_t setIdx = *(uint8_t*)pData;
	uint8_t cardIdx = *(uint8_t*)(pData + 1);

	int32_t result = 0;

	if (setIdx >= MAX_CARDSET_PER_USER || cardIdx >= MAX_CARD_PER_CARDSET)
	{
		result = 1;
	}
	else
	{
		GameQuestCardSet& cs = m_QuestData.cardSets[setIdx];
		if (!cs.IsActive())
		{
			result = 2;	// Cardset not active
		}
		else
		{
			GameQuestCard& card = cs.cards[cardIdx];
			card.CheckComplete();
			if (!card.bComplete)
			{
				result = 3;	// Card not complete
			}
			else
			{
				// Grant rewards
				uint32_t totalGP = 0;
				uint32_t totalExp = 0;
				for (int q = 0; q < MAX_QUEST_PER_CARD; q++)
				{
					totalGP += card.quests[q].ui32RewardGP;
					totalExp += card.quests[q].ui32RewardExp;
				}

				m_i32GP += totalGP;
				m_i64Exp += totalExp;

				printf("[GameSession] Quest reward claimed - UID=%lld, Set=%d, Card=%d, GP=%u, Exp=%u\n",
					m_i64UID, setIdx, cardIdx, totalGP, totalExp);
			}
		}
	}

	// Send ACK with updated balance
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_QUEST_GET_ITEM_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	uint32_t gp = (uint32_t)m_i32GP;
	memcpy(buffer + offset, &gp, 4);						offset += 4;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnQuestChangeReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BASE_QUEST_CHANGE_REQ -> ACK
	// Change active cardset slot index
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 1)
		return;

	uint8_t newIdx = *(uint8_t*)pData;
	int32_t result = 0;

	if (newIdx >= MAX_CARDSET_PER_USER)
	{
		result = 1;
	}
	else if (!m_QuestData.cardSets[newIdx].IsActive())
	{
		result = 2;
	}
	else
	{
		m_QuestData.ui8ActiveSetIndex = newIdx;
	}

	SendSimpleAck(PROTOCOL_BASE_QUEST_CHANGE_ACK, result);
}

void GameSession::OnQuestActiveIdxChangeReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_REQ -> ACK
	// Change active card within current cardset
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 2)
		return;

	uint8_t setIdx = *(uint8_t*)pData;
	uint8_t cardIdx = *(uint8_t*)(pData + 1);

	int32_t result = 0;

	if (setIdx >= MAX_CARDSET_PER_USER || cardIdx >= MAX_CARD_PER_CARDSET)
	{
		result = 1;
	}
	else if (!m_QuestData.cardSets[setIdx].IsActive())
	{
		result = 2;
	}
	else
	{
		m_QuestData.cardSets[setIdx].ui8ActiveCard = cardIdx;
	}

	SendSimpleAck(PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_ACK, result);
}

void GameSession::UpdateQuestProgress(int kills, int deaths, int headshots, bool won)
{
	// Called after a battle ends to update quest progress
	for (int s = 0; s < MAX_CARDSET_PER_USER; s++)
	{
		GameQuestCardSet& cs = m_QuestData.cardSets[s];
		if (!cs.IsActive())
			continue;

		// Update all quests in the active card
		GameQuestCard& card = cs.cards[cs.ui8ActiveCard];
		if (card.bComplete)
			continue;

		for (int q = 0; q < MAX_QUEST_PER_CARD; q++)
		{
			GameQuestEntry& quest = card.quests[q];
			if (quest.ui32Target == 0 || quest.IsComplete())
				continue;

			switch (quest.ui8Condition)
			{
			case QUEST_COND_KILLS:
				quest.ui32Current += kills;
				break;
			case QUEST_COND_HEADSHOTS:
				quest.ui32Current += headshots;
				break;
			case QUEST_COND_WINS:
				if (won) quest.ui32Current++;
				break;
			case QUEST_COND_GAMES_PLAYED:
				quest.ui32Current++;
				break;
			case QUEST_COND_DEATHS:
				quest.ui32Current += deaths;
				break;
			}

			// Cap at target
			if (quest.ui32Current > quest.ui32Target)
				quest.ui32Current = quest.ui32Target;
		}

		card.CheckComplete();
	}
}
