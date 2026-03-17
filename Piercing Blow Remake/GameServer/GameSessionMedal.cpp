#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "ModuleDataServer.h"

// ============================================================================
// Medal Handlers (Protocol_Base 0x0200 medal section + Protocol_Medal 0x1200)
// ============================================================================

void GameSession::OnGetMedalSystemReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BASE_GET_MEDALSYSTEM_REQ -> ACK
	// Send medal system overview (NEW flags)
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	int32_t result = 0;

	// Medal count
	uint16_t medalCount = (uint16_t)m_MedalData.i32MedalCount;

	i3NetworkPacket packet(PROTOCOL_BASE_GET_MEDALSYSTEM_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&medalCount, sizeof(uint16_t));
	// NEW flags (32 bytes = 256 bits)
	packet.WriteData(m_MedalData.ui8NewFlags, 32);
	SendPacketMessage(&packet);
}

void GameSession::OnRefreshMedalSystemReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BASE_REFRESH_MEDALSYSTEM_REQ -> ACK
	// Clear NEW flags
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	// Clear all NEW flags
	memset(m_MedalData.ui8NewFlags, 0, sizeof(m_MedalData.ui8NewFlags));

	SendSimpleAck(PROTOCOL_BASE_REFRESH_MEDALSYSTEM_ACK, 0);
}

void GameSession::OnMedalGetInfoReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BASE_MEDAL_GET_INFO_REQ -> ACK (or PROTOCOL_GET_MEDAL_INFO_REQ)
	// Send all medal info
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	// Header: result, totalCount, count in this packet, startIdx
	int32_t result = 0;

	uint16_t totalCount = (uint16_t)m_MedalData.i32MedalCount;
	uint16_t count = totalCount;
	uint16_t startIdx = 0;

	i3NetworkPacket packet(PROTOCOL_GET_MEDAL_INFO_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&totalCount, sizeof(uint16_t));
	packet.WriteData(&count, sizeof(uint16_t));
	packet.WriteData(&startIdx, sizeof(uint16_t));
	// Per-medal: idx(2) + count(2) + getReward(1) = 5 bytes each (MEDAL struct)
	for (int i = 0; i < m_MedalData.i32MedalCount; i++)
	{
		const GameMedalEntry& m = m_MedalData.medals[i];
		packet.WriteData(&m.ui16Idx, 2);
		packet.WriteData(&m.ui16Count, 2);
		packet.WriteData(&m.ui8GetReward, 1);
	}
	SendPacketMessage(&packet);
}

void GameSession::OnGetCurMedalSetInfoReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_GET_CUR_MEDAL_SET_INFO_REQ -> ACK
	// Send current (in-progress) medal sets
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	int32_t result = 0;

	// Count active sets
	uint16_t totalCount = 0;
	for (int i = 0; i < GAME_MEDAL_SET_MAX; i++)
	{
		if (m_MedalData.curSets[i].bActive)
			totalCount++;
	}

	uint16_t count = totalCount;
	uint16_t startIdx = 0;

	i3NetworkPacket packet(PROTOCOL_GET_CUR_MEDAL_SET_INFO_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&totalCount, sizeof(uint16_t));
	packet.WriteData(&count, sizeof(uint16_t));
	packet.WriteData(&startIdx, sizeof(uint16_t));
	// Per active set: CUR_MEDAL_SET = type(1) + idx(2) + recvDate(4) + count[6](12) + getReward(1) = 20 bytes
	for (int i = 0; i < GAME_MEDAL_SET_MAX; i++)
	{
		const GameMedalSet& s = m_MedalData.curSets[i];
		if (!s.bActive)
			continue;

		uint32_t recvDate = 0;
		packet.WriteData(&s.ui8Type, 1);
		packet.WriteData(&s.ui16Idx, 2);
		packet.WriteData(&recvDate, 4);
		packet.WriteData(s.ui16Count, 12);	// 6 x uint16_t
		packet.WriteData(&s.ui8GetReward, 1);
	}
	SendPacketMessage(&packet);
}

void GameSession::OnGetComMedalSetInfoReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	if (m_i64UID > 0 && g_pModuleDataServer && g_pModuleDataServer->IsConnected())
	{
		g_pModuleDataServer->RequestMedalSetLoad(m_i64UID, GetIndex());
		return;
	}

	// DataServer unavailable — send empty response
	int32_t result = 0;

	uint16_t zero = 0;
	i3NetworkPacket packet(PROTOCOL_GET_COM_MEDAL_SET_INFO_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&zero, sizeof(uint16_t));
	packet.WriteData(&zero, sizeof(uint16_t));
	packet.WriteData(&zero, sizeof(uint16_t));
	SendPacketMessage(&packet);
}

void GameSession::OnGetMedalRewardReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_GET_MEDAL_REWARD_REQ -> ACK
	// Claim medal reward
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 1)
		return;

	uint8_t reqCount = *(uint8_t*)pData;
	if (reqCount == 0 || i32Size < (int)(1 + reqCount * 4))
	{
		SendSimpleAck(PROTOCOL_GET_MEDAL_REWARD_ACK, 1);
		return;
	}

	// Parse MEDAL_REWARD entries: type(1) + idx(2) + level(1) = 4 bytes each
	uint32_t totalGP = 0;
	uint32_t totalExp = 0;
	int processedCount = 0;

	for (int i = 0; i < reqCount && i < MAX_GAME_MEDALS; i++)
	{
		int off = 1 + i * 4;
		uint8_t medalType = *(uint8_t*)(pData + off);
		uint16_t medalIdx = *(uint16_t*)(pData + off + 1);
		uint8_t medalLevel = *(uint8_t*)(pData + off + 3);

		if (medalType != GAME_MEDAL_TYPE_NORMAL)
			continue;

		GameMedalEntry* pMedal = m_MedalData.FindMedal(medalIdx);
		if (!pMedal)
			continue;

		if (!pMedal->IsLevelComplete(medalLevel))
			continue;

		if (pMedal->IsRewardClaimed(medalLevel))
			continue;

		// Claim reward
		pMedal->ClaimReward(medalLevel);
		totalGP += pMedal->ui32RewardGP[medalLevel - 1];
		totalExp += pMedal->ui32RewardExp[medalLevel - 1];
		processedCount++;
	}

	m_i32GP += totalGP;
	m_i64Exp += totalExp;

	if (processedCount > 0)
	{
		printf("[GameSession] Medal reward claimed - UID=%lld, GP+=%u, Exp+=%u\n",
			m_i64UID, totalGP, totalExp);
	}

	// Send ACK
	// PACKET_H_GET_MEDAL_REWARD_ACK
	bool bLastPacket = true;
	uint8_t count = (uint8_t)processedCount;
	uint32_t updatedExp = (uint32_t)m_i64Exp;
	uint32_t updatedPoint = (uint32_t)m_i32GP;
	uint16_t masterMedal = 0;
	i3NetworkPacket packet(PROTOCOL_GET_MEDAL_REWARD_ACK);
	packet.WriteData(&bLastPacket, 1);
	packet.WriteData(&count, 1);
	packet.WriteData(&updatedExp, 4);
	packet.WriteData(&updatedPoint, 4);
	packet.WriteData(&masterMedal, 2);
	SendPacketMessage(&packet);
}

void GameSession::OnSetNotifyMedalReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_SET_NOTIFY_MEDAL_REQ -> ACK
	// Set which medals to show in notifications
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	// Parse notify medals: 4 entries x (type(1) + idx(2) + count(2)) = 20 bytes
	if (i32Size >= (int)(MAX_NOTIFY_MEDAL_COUNT * 5))
	{
		for (int i = 0; i < MAX_NOTIFY_MEDAL_COUNT; i++)
		{
			int off = i * 5;
			m_MedalData.notifyMedals[i].ui8MedalType = *(uint8_t*)(pData + off);
			m_MedalData.notifyMedals[i].ui16Idx = *(uint16_t*)(pData + off + 1);
			m_MedalData.notifyMedals[i].ui16Count = *(uint16_t*)(pData + off + 3);
		}
	}

	// Send ACK with current notify medals
	int32_t result = 0;

	i3NetworkPacket packet(PROTOCOL_SET_NOTIFY_MEDAL_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	for (int i = 0; i < MAX_NOTIFY_MEDAL_COUNT; i++)
	{
		packet.WriteData(&m_MedalData.notifyMedals[i].ui8MedalType, 1);
		packet.WriteData(&m_MedalData.notifyMedals[i].ui16Idx, 2);
		packet.WriteData(&m_MedalData.notifyMedals[i].ui16Count, 2);
	}
	SendPacketMessage(&packet);
}
