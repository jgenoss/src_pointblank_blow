#include "ModuleDBGameData.h"
#include "DBConnectionPool.h"
#include <cstdio>
#include <cstring>

ModuleDBGameData::ModuleDBGameData(DBConnectionPool* pPool)
	: m_pPool(pPool)
{
}

ModuleDBGameData::~ModuleDBGameData()
{
}

bool ModuleDBGameData::SaveEquipment(int64_t i64UID, IS_EQUIPMENT_SAVE_REQ* pReq)
{
	if (!m_pPool || !pReq)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szUID[32], szSlot[8], szState[8], szCharaId[16];
	char szW[5][16], szP[10][16];

	snprintf(szUID, sizeof(szUID), "%lld", i64UID);
	snprintf(szSlot, sizeof(szSlot), "%d", pReq->ui8CharaSlot);
	snprintf(szState, sizeof(szState), "%d", pReq->ui8SlotState);
	snprintf(szCharaId, sizeof(szCharaId), "%u", pReq->ui32CharaId);

	for (int i = 0; i < 5; i++)
		snprintf(szW[i], sizeof(szW[i]), "%u", pReq->ui32WeaponIds[i]);
	for (int i = 0; i < 10; i++)
		snprintf(szP[i], sizeof(szP[i]), "%u", pReq->ui32PartsIds[i]);

	const char* paramValues[19] = {
		szUID, szSlot, szState, szCharaId,
		szW[0], szW[1], szW[2], szW[3], szW[4],
		szP[0], szP[1], szP[2], szP[3], szP[4],
		szP[5], szP[6], szP[7], szP[8], szP[9]
	};

	DBResult result = pConn->ExecuteParams(
		"INSERT INTO pb_equipment (uid, chara_slot, slot_state, chara_id, "
		"weapon_primary, weapon_secondary, weapon_melee, weapon_throw1, weapon_throw2, "
		"parts_chara, parts_head, parts_face, parts_upper, parts_lower, "
		"parts_glove, parts_belt, parts_holster, parts_skin, parts_beret) "
		"VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15, $16, $17, $18, $19) "
		"ON CONFLICT (uid, chara_slot) DO UPDATE SET "
		"slot_state = $3, chara_id = $4, "
		"weapon_primary = $5, weapon_secondary = $6, weapon_melee = $7, weapon_throw1 = $8, weapon_throw2 = $9, "
		"parts_chara = $10, parts_head = $11, parts_face = $12, parts_upper = $13, parts_lower = $14, "
		"parts_glove = $15, parts_belt = $16, parts_holster = $17, parts_skin = $18, parts_beret = $19",
		19, paramValues);

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
	{
		printf("[ModuleDBGameData] ERROR: SaveEquipment failed for UID=%lld slot=%d\n",
			i64UID, pReq->ui8CharaSlot);
		return false;
	}

	printf("[ModuleDBGameData] Equipment saved UID=%lld slot=%d\n", i64UID, pReq->ui8CharaSlot);
	return true;
}

bool ModuleDBGameData::SaveMedal(int64_t i64UID, IS_MEDAL_SAVE_REQ* pReq)
{
	if (!m_pPool || !pReq)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szUID[32], szIdx[8], szAction[8], szCount[16], szReward[8];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);
	snprintf(szIdx, sizeof(szIdx), "%d", pReq->ui16MedalIdx);
	snprintf(szAction, sizeof(szAction), "%d", pReq->ui8ActionId);
	snprintf(szCount, sizeof(szCount), "%d", pReq->ui16Count);
	snprintf(szReward, sizeof(szReward), "%d", pReq->ui8GetReward);

	const char* paramValues[5] = { szUID, szIdx, szAction, szCount, szReward };
	DBResult result = pConn->ExecuteParams(
		"INSERT INTO pb_medals (uid, medal_idx, action_id, current_count, get_reward) "
		"VALUES ($1, $2, $3, $4, $5) "
		"ON CONFLICT (uid, medal_idx) DO UPDATE SET "
		"current_count = $4, get_reward = $5",
		5, paramValues);

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
	{
		printf("[ModuleDBGameData] ERROR: SaveMedal failed for UID=%lld medal=%d\n",
			i64UID, pReq->ui16MedalIdx);
		return false;
	}

	return true;
}

bool ModuleDBGameData::SaveAttendance(int64_t i64UID, IS_ATTENDANCE_SAVE_REQ* pReq)
{
	if (!m_pPool || !pReq)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szUID[32], szTotal[16], szStreak[16], szLastDate[16];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);
	snprintf(szTotal, sizeof(szTotal), "%d", pReq->i32TotalDays);
	snprintf(szStreak, sizeof(szStreak), "%d", pReq->i32CurrentStreak);
	snprintf(szLastDate, sizeof(szLastDate), "%u", pReq->ui32LastAttendDate);

	// For BYTEA, we need to pass binary data with length info
	// Use text representation via escape for simplicity
	char szDaysHex[64];
	szDaysHex[0] = '\\';
	szDaysHex[1] = 'x';
	for (int i = 0; i < 30; i++)
		snprintf(szDaysHex + 2 + i * 2, 3, "%02x", pReq->ui8Days[i]);

	const char* paramValues[5] = { szUID, szTotal, szStreak, szLastDate, szDaysHex };
	DBResult result = pConn->ExecuteParams(
		"INSERT INTO pb_attendance (uid, total_days, current_streak, last_attend_date, attend_data) "
		"VALUES ($1, $2, $3, $4, $5::bytea) "
		"ON CONFLICT (uid) DO UPDATE SET "
		"total_days = $2, current_streak = $3, last_attend_date = $4, attend_data = $5::bytea",
		5, paramValues);

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
	{
		printf("[ModuleDBGameData] ERROR: SaveAttendance failed for UID=%lld\n", i64UID);
		return false;
	}

	printf("[ModuleDBGameData] Attendance saved UID=%lld days=%d streak=%d\n",
		i64UID, pReq->i32TotalDays, pReq->i32CurrentStreak);
	return true;
}

bool ModuleDBGameData::SaveSkill(int64_t i64UID, IS_SKILL_SAVE_REQ* pReq)
{
	if (!m_pPool || !pReq)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szUID[32], szClass[8], szPoints[16];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);
	snprintf(szClass, sizeof(szClass), "%d", pReq->ui8ClassId);
	snprintf(szPoints, sizeof(szPoints), "%d", pReq->ui16SkillPoints);

	// Encode skill levels as hex bytea
	char szMainHex[16], szAssistHex[16], szCommonHex[16];
	szMainHex[0] = '\\'; szMainHex[1] = 'x';
	for (int i = 0; i < 5; i++)
		snprintf(szMainHex + 2 + i * 2, 3, "%02x", pReq->ui8MainLevels[i]);

	szAssistHex[0] = '\\'; szAssistHex[1] = 'x';
	for (int i = 0; i < 4; i++)
		snprintf(szAssistHex + 2 + i * 2, 3, "%02x", pReq->ui8AssistLevels[i]);

	szCommonHex[0] = '\\'; szCommonHex[1] = 'x';
	for (int i = 0; i < 5; i++)
		snprintf(szCommonHex + 2 + i * 2, 3, "%02x", pReq->ui8CommonLevels[i]);

	const char* paramValues[6] = { szUID, szClass, szMainHex, szAssistHex, szCommonHex, szPoints };
	DBResult result = pConn->ExecuteParams(
		"INSERT INTO pb_skills (uid, class_id, main_levels, assist_levels, common_levels, skill_points) "
		"VALUES ($1, $2, $3::bytea, $4::bytea, $5::bytea, $6) "
		"ON CONFLICT (uid, class_id) DO UPDATE SET "
		"main_levels = $3::bytea, assist_levels = $4::bytea, common_levels = $5::bytea, skill_points = $6",
		6, paramValues);

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
	{
		printf("[ModuleDBGameData] ERROR: SaveSkill failed for UID=%lld class=%d\n",
			i64UID, pReq->ui8ClassId);
		return false;
	}

	printf("[ModuleDBGameData] Skill saved UID=%lld class=%d points=%d\n",
		i64UID, pReq->ui8ClassId, pReq->ui16SkillPoints);
	return true;
}

bool ModuleDBGameData::SaveQuest(int64_t i64UID, IS_QUEST_SAVE_REQ* pReq,
								 const char* pQuestData, int i32DataSize)
{
	if (!m_pPool || !pReq)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szUID[32], szSetIdx[8], szSetType[8], szActiveCard[8];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);
	snprintf(szSetIdx, sizeof(szSetIdx), "%d", pReq->ui8SetIndex);
	snprintf(szSetType, sizeof(szSetType), "%d", pReq->ui8SetType);
	snprintf(szActiveCard, sizeof(szActiveCard), "%d", pReq->ui8ActiveCard);

	// Encode quest_data as hex bytea
	char szDataHex[8192];
	szDataHex[0] = '\\'; szDataHex[1] = 'x';
	int hexOff = 2;
	for (int i = 0; i < i32DataSize && hexOff < (int)sizeof(szDataHex) - 2; i++)
	{
		snprintf(szDataHex + hexOff, 3, "%02x", (uint8_t)pQuestData[i]);
		hexOff += 2;
	}
	szDataHex[hexOff] = '\0';

	const char* paramValues[5] = { szUID, szSetIdx, szSetType, szActiveCard, szDataHex };
	DBResult result = pConn->ExecuteParams(
		"INSERT INTO pb_quests (uid, set_index, set_type, active_card, quest_data) "
		"VALUES ($1, $2, $3, $4, $5::bytea) "
		"ON CONFLICT (uid, set_index) DO UPDATE SET "
		"set_type = $3, active_card = $4, quest_data = $5::bytea",
		5, paramValues);

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
	{
		printf("[ModuleDBGameData] ERROR: SaveQuest failed for UID=%lld set=%d\n",
			i64UID, pReq->ui8SetIndex);
		return false;
	}

	printf("[ModuleDBGameData] Quest saved UID=%lld set=%d type=%d\n",
		i64UID, pReq->ui8SetIndex, pReq->ui8SetType);
	return true;
}

int ModuleDBGameData::LoadShopItems(IS_SHOP_ITEM_ENTRY* pOut, int i32MaxCount)
{
	if (!m_pPool || !pOut)
		return 0;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return 0;

	DBResult result = pConn->Execute(
		"SELECT goods_id, item_id, item_type, price_gp, price_cash, duration, category "
		"FROM pb_shop_items WHERE is_active = TRUE ORDER BY goods_id");

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
	{
		printf("[ModuleDBGameData] ERROR: LoadShopItems failed\n");
		return 0;
	}

	int count = 0;
	for (int i = 0; i < result.GetRowCount() && count < i32MaxCount; i++)
	{
		pOut[count].ui32GoodsId	= (uint32_t)atoi(result.GetValue(i, 0));
		pOut[count].ui32ItemId	= (uint32_t)atoi(result.GetValue(i, 1));
		pOut[count].ui8ItemType	= (uint8_t)atoi(result.GetValue(i, 2));
		pOut[count].i32PriceGP	= atoi(result.GetValue(i, 3));
		pOut[count].i32PriceCash = atoi(result.GetValue(i, 4));
		pOut[count].ui32Duration = (uint32_t)atoi(result.GetValue(i, 5));
		pOut[count].ui8Category	= (uint8_t)atoi(result.GetValue(i, 6));
		count++;
	}

	printf("[ModuleDBGameData] Loaded %d shop items\n", count);
	return count;
}

void ModuleDBGameData::BuyShopItem(IS_SHOP_BUY_REQ* pReq, IS_SHOP_BUY_ACK* pAck)
{
	if (!m_pPool || !pReq || !pAck)
	{
		if (pAck) pAck->i32Result = 3;	// error
		return;
	}

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
	{
		pAck->i32Result = 3;
		return;
	}

	// Check current balance
	char szUID[32];
	snprintf(szUID, sizeof(szUID), "%lld", pReq->i64UID);

	const char* balParams[1] = { szUID };
	DBResult balResult = pConn->ExecuteParams(
		"SELECT gp, cash FROM pb_users WHERE uid = $1", 1, balParams);

	if (!balResult.IsSuccess() || balResult.GetRowCount() == 0)
	{
		m_pPool->ReleaseConnection(pConn);
		pAck->i32Result = 3;	// DB error
		return;
	}

	int currentGP = atoi(balResult.GetValue(0, 0));
	int currentCash = atoi(balResult.GetValue(0, 1));
	int price = pReq->i32Price;

	// Check sufficient funds
	if (pReq->ui8PayType == 0)	// GP
	{
		if (currentGP < price)
		{
			m_pPool->ReleaseConnection(pConn);
			pAck->i32Result = 1;	// Insufficient GP
			pAck->i32RemainingGP = currentGP;
			pAck->i32RemainingCash = currentCash;
			return;
		}
		currentGP -= price;
	}
	else	// Cash
	{
		if (currentCash < price)
		{
			m_pPool->ReleaseConnection(pConn);
			pAck->i32Result = 1;	// Insufficient Cash
			pAck->i32RemainingGP = currentGP;
			pAck->i32RemainingCash = currentCash;
			return;
		}
		currentCash -= price;
	}

	// Deduct currency
	char szGP[16], szCash[16];
	snprintf(szGP, sizeof(szGP), "%d", currentGP);
	snprintf(szCash, sizeof(szCash), "%d", currentCash);

	const char* updateParams[3] = { szGP, szCash, szUID };
	DBResult updateResult = pConn->ExecuteParams(
		"UPDATE pb_users SET gp = $1, cash = $2 WHERE uid = $3", 3, updateParams);

	if (!updateResult.IsSuccess())
	{
		m_pPool->ReleaseConnection(pConn);
		pAck->i32Result = 3;
		return;
	}

	// Add item to inventory
	char szItemId[16], szItemCount[8], szSlot[8], szEquipped[8];
	snprintf(szItemId, sizeof(szItemId), "%u", pReq->ui32ItemId);
	snprintf(szItemCount, sizeof(szItemCount), "1");
	snprintf(szSlot, sizeof(szSlot), "-1");
	snprintf(szEquipped, sizeof(szEquipped), "0");

	const char* invenParams[5] = { szUID, szItemId, szItemCount, szSlot, szEquipped };
	DBResult invenResult = pConn->ExecuteParams(
		"INSERT INTO pb_inventory (uid, item_id, item_count, slot_idx, is_equipped) "
		"VALUES ($1, $2, $3, $4, $5::boolean)",
		5, invenParams);

	m_pPool->ReleaseConnection(pConn);

	if (!invenResult.IsSuccess())
	{
		pAck->i32Result = 3;	// DB error
		return;
	}

	pAck->i32Result = 0;	// OK
	pAck->i32RemainingGP = currentGP;
	pAck->i32RemainingCash = currentCash;

	printf("[ModuleDBGameData] Shop buy OK - UID=%lld, Item=0x%08X, GP=%d, Cash=%d\n",
		pReq->i64UID, pReq->ui32ItemId, currentGP, currentCash);
}

bool ModuleDBGameData::UpdateInventory(IS_INVEN_UPDATE_REQ* pReq)
{
	if (!m_pPool || !pReq)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szUID[32], szItemId[16], szCount[16], szSlot[16], szEquipped[8];
	snprintf(szUID, sizeof(szUID), "%lld", pReq->i64UID);
	snprintf(szItemId, sizeof(szItemId), "%u", pReq->ui32ItemId);
	snprintf(szCount, sizeof(szCount), "%d", pReq->i32ItemCount);
	snprintf(szSlot, sizeof(szSlot), "%d", pReq->i32SlotIdx);
	snprintf(szEquipped, sizeof(szEquipped), "%d", pReq->ui8IsEquipped ? 1 : 0);

	DBResult result;

	switch (pReq->ui8Operation)
	{
	case 0:	// Add item
	{
		const char* params[5] = { szUID, szItemId, szCount, szSlot, szEquipped };
		result = pConn->ExecuteParams(
			"INSERT INTO pb_inventory (uid, item_id, item_count, slot_idx, is_equipped) "
			"VALUES ($1, $2, $3, $4, $5::boolean)",
			5, params);
		break;
	}
	case 1:	// Remove item
	{
		const char* params[2] = { szUID, szItemId };
		result = pConn->ExecuteParams(
			"DELETE FROM pb_inventory WHERE uid = $1 AND item_id = $2",
			2, params);
		break;
	}
	case 2:	// Update count/equipped
	{
		const char* params[4] = { szCount, szEquipped, szUID, szItemId };
		result = pConn->ExecuteParams(
			"UPDATE pb_inventory SET item_count = $1, is_equipped = $2::boolean "
			"WHERE uid = $3 AND item_id = $4",
			4, params);
		break;
	}
	default:
		m_pPool->ReleaseConnection(pConn);
		return false;
	}

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
	{
		printf("[ModuleDBGameData] ERROR: UpdateInventory failed - UID=%lld, Op=%d, Item=0x%08X\n",
			pReq->i64UID, pReq->ui8Operation, pReq->ui32ItemId);
		return false;
	}

	return true;
}

void ModuleDBGameData::ProcessResponses(DataServerContext* pContext)
{
	// Placeholder para futuro async con ring buffers
}
