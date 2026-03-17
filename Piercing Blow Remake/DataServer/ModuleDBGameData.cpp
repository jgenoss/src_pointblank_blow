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

// ============================================================================
// Equipment (extendido con IDX linking)
// Cada slot tiene IDX (inventory link) + ID (item type)
// ============================================================================

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

// ============================================================================
// Medals (individual)
// ============================================================================

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

// ============================================================================
// Medal Sets
// ============================================================================

bool ModuleDBGameData::SaveMedalSet(int64_t i64UID, IS_MEDAL_SET_SAVE_REQ* pReq)
{
	if (!m_pPool || !pReq)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szUID[32], szType[8], szIdx[8];
	char szM[6][16], szReward[8];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);
	snprintf(szType, sizeof(szType), "%d", pReq->ui8SetType);
	snprintf(szIdx, sizeof(szIdx), "%d", pReq->i16MedalSetIdx);
	snprintf(szM[0], sizeof(szM[0]), "%d", pReq->i16Medal1Count);
	snprintf(szM[1], sizeof(szM[1]), "%d", pReq->i16Medal2Count);
	snprintf(szM[2], sizeof(szM[2]), "%d", pReq->i16Medal3Count);
	snprintf(szM[3], sizeof(szM[3]), "%d", pReq->i16Medal4Count);
	snprintf(szM[4], sizeof(szM[4]), "%d", pReq->i16Medal5Count);
	snprintf(szM[5], sizeof(szM[5]), "%d", pReq->i16Medal6Count);
	snprintf(szReward, sizeof(szReward), "%d", pReq->ui8GetReward);

	bool bSuccess = true;

	if (pReq->ui8IsComplete)
	{
		// Move to complete table
		const char* params[10] = { szUID, szType, szIdx,
			szM[0], szM[1], szM[2], szM[3], szM[4], szM[5], szReward };
		DBResult result = pConn->ExecuteParams(
			"INSERT INTO pb_medal_sets_complete (uid, set_type, medal_set_idx, recv_date, complete_date, "
			"medal1_count, medal2_count, medal3_count, medal4_count, medal5_count, medal6_count, get_reward) "
			"VALUES ($1, $2, $3, NOW(), NOW(), $4, $5, $6, $7, $8, $9, $10) "
			"ON CONFLICT (uid, set_type, medal_set_idx) DO UPDATE SET "
			"complete_date = NOW(), medal1_count = $4, medal2_count = $5, medal3_count = $6, "
			"medal4_count = $7, medal5_count = $8, medal6_count = $9, get_reward = $10",
			10, params);
		bSuccess = result.IsSuccess();
	}
	else
	{
		// Update current set
		const char* params[10] = { szUID, szType, szIdx,
			szM[0], szM[1], szM[2], szM[3], szM[4], szM[5], szReward };
		DBResult result = pConn->ExecuteParams(
			"INSERT INTO pb_medal_sets_current (uid, set_type, medal_set_idx, recv_date, "
			"medal1_count, medal2_count, medal3_count, medal4_count, medal5_count, medal6_count, get_reward) "
			"VALUES ($1, $2, $3, NOW(), $4, $5, $6, $7, $8, $9, $10) "
			"ON CONFLICT (uid, set_type) DO UPDATE SET "
			"medal_set_idx = $3, medal1_count = $4, medal2_count = $5, medal3_count = $6, "
			"medal4_count = $7, medal5_count = $8, medal6_count = $9, get_reward = $10",
			10, params);
		bSuccess = result.IsSuccess();
	}

	m_pPool->ReleaseConnection(pConn);
	return bSuccess;
}

int ModuleDBGameData::LoadMedalSets(int64_t i64UID, IS_MEDAL_SET_CURRENT_ENTRY* pOut, int i32MaxCount)
{
	if (!m_pPool || !pOut)
		return 0;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return 0;

	char szUID[32];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);

	const char* params[1] = { szUID };
	DBResult result = pConn->ExecuteParams(
		"SELECT set_type, medal_set_idx, medal1_count, medal2_count, medal3_count, "
		"medal4_count, medal5_count, medal6_count, get_reward "
		"FROM pb_medal_sets_current WHERE uid = $1",
		1, params);

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
		return 0;

	int count = result.GetRowCount();
	if (count > i32MaxCount)
		count = i32MaxCount;

	for (int i = 0; i < count; i++)
	{
		pOut[i].ui8SetType		= (uint8_t)atoi(result.GetValue(i, 0));
		pOut[i].i16MedalSetIdx	= (int16_t)atoi(result.GetValue(i, 1));
		pOut[i].i16Medal1Count	= (int16_t)atoi(result.GetValue(i, 2));
		pOut[i].i16Medal2Count	= (int16_t)atoi(result.GetValue(i, 3));
		pOut[i].i16Medal3Count	= (int16_t)atoi(result.GetValue(i, 4));
		pOut[i].i16Medal4Count	= (int16_t)atoi(result.GetValue(i, 5));
		pOut[i].i16Medal5Count	= (int16_t)atoi(result.GetValue(i, 6));
		pOut[i].i16Medal6Count	= (int16_t)atoi(result.GetValue(i, 7));
		pOut[i].ui8GetReward	= (uint8_t)atoi(result.GetValue(i, 8));
	}

	return count;
}

// ============================================================================
// Attendance
// ============================================================================

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

// ============================================================================
// Skills
// ============================================================================

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

// ============================================================================
// Quests
// ============================================================================

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

// ============================================================================
// Account Cosmetics
// ============================================================================

bool ModuleDBGameData::LoadCosmetics(int64_t i64UID, IS_COSMETICS_LOAD_ACK* pOut)
{
	if (!m_pPool || !pOut)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szUID[32];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);

	const char* params[1] = { szUID };
	DBResult result = pConn->ExecuteParams(
		"SELECT nickname_color, crosshair_color, chatting_color, disguise_rank, fake_nick "
		"FROM pb_account_cosmetics WHERE uid = $1",
		1, params);

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess() || result.GetRowCount() == 0)
	{
		// Return defaults
		pOut->ui8NicknameColor = 0;
		pOut->ui8CrosshairColor = 0;
		pOut->ui8ChattingColor = 0;
		pOut->i32DisguiseRank = 0;
		memset(pOut->szFakeNick, 0, sizeof(pOut->szFakeNick));
		return true;
	}

	pOut->ui8NicknameColor	= (uint8_t)atoi(result.GetValue(0, 0));
	pOut->ui8CrosshairColor	= (uint8_t)atoi(result.GetValue(0, 1));
	pOut->ui8ChattingColor	= (uint8_t)atoi(result.GetValue(0, 2));
	pOut->i32DisguiseRank	= atoi(result.GetValue(0, 3));
	const char* pszFake = result.GetValue(0, 4);
	if (pszFake)
		strncpy(pOut->szFakeNick, pszFake, sizeof(pOut->szFakeNick) - 1);

	return true;
}

bool ModuleDBGameData::SaveCosmetics(int64_t i64UID, IS_COSMETICS_SAVE_REQ* pReq)
{
	if (!m_pPool || !pReq)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szUID[32], szNickColor[8], szCrossColor[8], szChatColor[8], szDisguise[16];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);
	snprintf(szNickColor, sizeof(szNickColor), "%d", pReq->ui8NicknameColor);
	snprintf(szCrossColor, sizeof(szCrossColor), "%d", pReq->ui8CrosshairColor);
	snprintf(szChatColor, sizeof(szChatColor), "%d", pReq->ui8ChattingColor);
	snprintf(szDisguise, sizeof(szDisguise), "%d", pReq->i32DisguiseRank);

	const char* params[6] = { szUID, szNickColor, szCrossColor, szChatColor, szDisguise, pReq->szFakeNick };
	DBResult result = pConn->ExecuteParams(
		"INSERT INTO pb_account_cosmetics (uid, nickname_color, crosshair_color, chatting_color, disguise_rank, fake_nick) "
		"VALUES ($1, $2, $3, $4, $5, $6) "
		"ON CONFLICT (uid) DO UPDATE SET "
		"nickname_color = $2, crosshair_color = $3, chatting_color = $4, disguise_rank = $5, fake_nick = $6",
		6, params);

	m_pPool->ReleaseConnection(pConn);
	return result.IsSuccess();
}

// ============================================================================
// Mode Records (per game mode stats)
// ============================================================================

int ModuleDBGameData::LoadModeRecords(int64_t i64UID, IS_MODE_RECORD_ENTRY* pOut, int i32MaxCount)
{
	if (!m_pPool || !pOut)
		return 0;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return 0;

	char szUID[32];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);

	const char* params[1] = { szUID };
	DBResult result = pConn->ExecuteParams(
		"SELECT record_type, matches, wins, losses, draws, kills, deaths, headshots, "
		"disconnects, daily_matches, daily_kills "
		"FROM pb_mode_records WHERE uid = $1 ORDER BY record_type",
		1, params);

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
		return 0;

	int count = result.GetRowCount();
	if (count > i32MaxCount)
		count = i32MaxCount;

	for (int i = 0; i < count; i++)
	{
		pOut[i].ui8RecordType	= (uint8_t)atoi(result.GetValue(i, 0));
		pOut[i].i32Matches		= atoi(result.GetValue(i, 1));
		pOut[i].i32Wins			= atoi(result.GetValue(i, 2));
		pOut[i].i32Losses		= atoi(result.GetValue(i, 3));
		pOut[i].i32Draws		= atoi(result.GetValue(i, 4));
		pOut[i].i32Kills		= atoi(result.GetValue(i, 5));
		pOut[i].i32Deaths		= atoi(result.GetValue(i, 6));
		pOut[i].i32Headshots	= atoi(result.GetValue(i, 7));
		pOut[i].i32Disconnects	= atoi(result.GetValue(i, 8));
		pOut[i].i32DailyMatches	= atoi(result.GetValue(i, 9));
		pOut[i].i32DailyKills	= atoi(result.GetValue(i, 10));
	}

	return count;
}

bool ModuleDBGameData::SaveModeRecord(int64_t i64UID, IS_MODE_RECORDS_SAVE_REQ* pReq)
{
	if (!m_pPool || !pReq)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szUID[32], szType[8];
	char szDM[8], szDW[8], szDL[8], szDD[8], szDK[8], szDDe[8], szDH[8], szDDi[8];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);
	snprintf(szType, sizeof(szType), "%d", pReq->ui8RecordType);
	snprintf(szDM, sizeof(szDM), "%d", pReq->i32DeltaMatches);
	snprintf(szDW, sizeof(szDW), "%d", pReq->i32DeltaWins);
	snprintf(szDL, sizeof(szDL), "%d", pReq->i32DeltaLosses);
	snprintf(szDD, sizeof(szDD), "%d", pReq->i32DeltaDraws);
	snprintf(szDK, sizeof(szDK), "%d", pReq->i32DeltaKills);
	snprintf(szDDe, sizeof(szDDe), "%d", pReq->i32DeltaDeaths);
	snprintf(szDH, sizeof(szDH), "%d", pReq->i32DeltaHeadshots);
	snprintf(szDDi, sizeof(szDDi), "%d", pReq->i32DeltaDisconnects);

	const char* params[10] = { szUID, szType, szDM, szDW, szDL, szDD, szDK, szDDe, szDH, szDDi };
	DBResult result = pConn->ExecuteParams(
		"UPDATE pb_mode_records SET "
		"matches = matches + $3, wins = wins + $4, losses = losses + $5, draws = draws + $6, "
		"kills = kills + $7, deaths = deaths + $8, headshots = headshots + $9, "
		"disconnects = disconnects + $10, "
		"daily_matches = daily_matches + $3, daily_kills = daily_kills + $7 "
		"WHERE uid = $1 AND record_type = $2",
		10, params);

	m_pPool->ReleaseConnection(pConn);
	return result.IsSuccess();
}

// ============================================================================
// Daily Records
// ============================================================================

bool ModuleDBGameData::LoadDailyRecord(int64_t i64UID, IS_DAILY_RECORD_ENTRY* pOut)
{
	if (!m_pPool || !pOut)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szUID[32];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);

	const char* params[1] = { szUID };
	DBResult result = pConn->ExecuteParams(
		"SELECT record_date, wins, losses, draws, kills, deaths, headshots, "
		"exp_earned, gp_earned, play_time, gift_play_time, got_daily_item "
		"FROM pb_daily_records WHERE uid = $1",
		1, params);

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess() || result.GetRowCount() == 0)
	{
		memset(pOut, 0, sizeof(*pOut));
		return true;
	}

	pOut->i32RecordDate		= atoi(result.GetValue(0, 0));
	pOut->i32Wins			= atoi(result.GetValue(0, 1));
	pOut->i32Losses			= atoi(result.GetValue(0, 2));
	pOut->i32Draws			= atoi(result.GetValue(0, 3));
	pOut->i32Kills			= atoi(result.GetValue(0, 4));
	pOut->i32Deaths			= atoi(result.GetValue(0, 5));
	pOut->i32Headshots		= atoi(result.GetValue(0, 6));
	pOut->i32ExpEarned		= atoi(result.GetValue(0, 7));
	pOut->i32GPEarned		= atoi(result.GetValue(0, 8));
	pOut->i32PlayTime		= atoi(result.GetValue(0, 9));
	pOut->i32GiftPlayTime	= atoi(result.GetValue(0, 10));
	pOut->ui8GotDailyItem	= (uint8_t)atoi(result.GetValue(0, 11));

	return true;
}

bool ModuleDBGameData::SaveDailyRecord(int64_t i64UID, IS_DAILY_RECORD_SAVE_REQ* pReq)
{
	if (!m_pPool || !pReq)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szUID[32], szW[8], szL[8], szD[8], szK[8], szDe[8], szH[8], szE[16], szG[16], szP[16], szDI[8];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);
	snprintf(szW, sizeof(szW), "%d", pReq->i32DeltaWins);
	snprintf(szL, sizeof(szL), "%d", pReq->i32DeltaLosses);
	snprintf(szD, sizeof(szD), "%d", pReq->i32DeltaDraws);
	snprintf(szK, sizeof(szK), "%d", pReq->i32DeltaKills);
	snprintf(szDe, sizeof(szDe), "%d", pReq->i32DeltaDeaths);
	snprintf(szH, sizeof(szH), "%d", pReq->i32DeltaHeadshots);
	snprintf(szE, sizeof(szE), "%d", pReq->i32DeltaExp);
	snprintf(szG, sizeof(szG), "%d", pReq->i32DeltaGP);
	snprintf(szP, sizeof(szP), "%d", pReq->i32DeltaPlayTime);
	snprintf(szDI, sizeof(szDI), "%d", pReq->ui8GotDailyItem);

	const char* params[11] = { szUID, szW, szL, szD, szK, szDe, szH, szE, szG, szP, szDI };
	DBResult result = pConn->ExecuteParams(
		"UPDATE pb_daily_records SET "
		"wins = wins + $2, losses = losses + $3, draws = draws + $4, "
		"kills = kills + $5, deaths = deaths + $6, headshots = headshots + $7, "
		"exp_earned = exp_earned + $8, gp_earned = gp_earned + $9, "
		"play_time = play_time + $10, "
		"got_daily_item = GREATEST(got_daily_item, $11::SMALLINT) "
		"WHERE uid = $1",
		11, params);

	m_pPool->ReleaseConnection(pConn);
	return result.IsSuccess();
}

// ============================================================================
// Map System
// ============================================================================

int ModuleDBGameData::LoadMapModes(IS_MAP_MODE_ENTRY* pOut, int i32MaxCount)
{
	if (!m_pPool || !pOut)
		return 0;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return 0;

	DBResult result = pConn->Execute(
		"SELECT stage_key, stage_uid, mode_id, max_players, mark, special_mode "
		"FROM pb_map_mode_matching WHERE is_open > 0 ORDER BY stage_key");

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
		return 0;

	int count = result.GetRowCount();
	if (count > i32MaxCount)
		count = i32MaxCount;

	for (int i = 0; i < count; i++)
	{
		pOut[i].ui16StageKey	= (uint16_t)atoi(result.GetValue(i, 0));
		pOut[i].ui16StageUID	= (uint16_t)atoi(result.GetValue(i, 1));
		pOut[i].ui8Mode			= (uint8_t)atoi(result.GetValue(i, 2));
		pOut[i].ui8MaxPlayers	= (uint8_t)atoi(result.GetValue(i, 3));
		pOut[i].ui8Mark			= (uint8_t)atoi(result.GetValue(i, 4));
		pOut[i].ui8SpecialMode	= (uint8_t)atoi(result.GetValue(i, 5));
	}

	printf("[ModuleDBGameData] Loaded %d map-mode entries\n", count);
	return count;
}

bool ModuleDBGameData::SaveMapStats(IS_MAP_STATS_SAVE_REQ* pReq)
{
	if (!m_pPool || !pReq)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szMapId[16], szStageType[16], szPlayTime[16];
	snprintf(szMapId, sizeof(szMapId), "%d", pReq->i32MapId);
	snprintf(szStageType, sizeof(szStageType), "%d", pReq->i32StageType);
	snprintf(szPlayTime, sizeof(szPlayTime), "%d", pReq->i32PlayTime);

	const char* params[3] = { szMapId, szStageType, szPlayTime };
	DBResult result = pConn->ExecuteParams(
		"INSERT INTO pb_map_stats (reg_date, map_id, stage_type, play_time, play_rounds) "
		"VALUES (CURRENT_DATE, $1, $2, $3, 1) "
		"ON CONFLICT (reg_date, map_id, stage_type) DO UPDATE SET "
		"play_time = pb_map_stats.play_time + $3, play_rounds = pb_map_stats.play_rounds + 1",
		3, params);

	m_pPool->ReleaseConnection(pConn);
	return result.IsSuccess();
}

// ============================================================================
// Rankings
// ============================================================================

int ModuleDBGameData::LoadRankingsIndividual(uint8_t ui8TermType, IS_RANKING_INDIVIDUAL_ENTRY* pOut, int i32MaxCount)
{
	if (!m_pPool || !pOut)
		return 0;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return 0;

	char szTermType[8];
	snprintf(szTermType, sizeof(szTermType), "%d", ui8TermType);

	const char* params[1] = { szTermType };
	DBResult result = pConn->ExecuteParams(
		"SELECT rank_position, uid, nickname, level, exp, kills, deaths, headshots "
		"FROM pb_rankings_individual "
		"WHERE rank_date = CURRENT_DATE AND term_type = $1 "
		"ORDER BY rank_position LIMIT 100",
		1, params);

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
		return 0;

	int count = result.GetRowCount();
	if (count > i32MaxCount)
		count = i32MaxCount;

	for (int i = 0; i < count; i++)
	{
		pOut[i].i32RankPosition	= atoi(result.GetValue(i, 0));
		pOut[i].i64UID			= _atoi64(result.GetValue(i, 1));
		const char* pszNick = result.GetValue(i, 2);
		if (pszNick)
			strncpy(pOut[i].szNickname, pszNick, sizeof(pOut[i].szNickname) - 1);
		pOut[i].i32Level		= atoi(result.GetValue(i, 3));
		pOut[i].i64Exp			= _atoi64(result.GetValue(i, 4));
		pOut[i].i32Kills		= atoi(result.GetValue(i, 5));
		pOut[i].i32Deaths		= atoi(result.GetValue(i, 6));
		pOut[i].i32Headshots	= atoi(result.GetValue(i, 7));
	}

	return count;
}

int ModuleDBGameData::LoadRankingsClan(uint8_t ui8TermType, IS_RANKING_CLAN_ENTRY* pOut, int i32MaxCount)
{
	if (!m_pPool || !pOut)
		return 0;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return 0;

	char szTermType[8];
	snprintf(szTermType, sizeof(szTermType), "%d", ui8TermType);

	const char* params[1] = { szTermType };
	DBResult result = pConn->ExecuteParams(
		"SELECT rank_position, clan_id, clan_name, clan_exp, wins, losses, member_count "
		"FROM pb_rankings_clan "
		"WHERE rank_date = CURRENT_DATE AND term_type = $1 "
		"ORDER BY rank_position LIMIT 50",
		1, params);

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
		return 0;

	int count = result.GetRowCount();
	if (count > i32MaxCount)
		count = i32MaxCount;

	for (int i = 0; i < count; i++)
	{
		pOut[i].i32RankPosition	= atoi(result.GetValue(i, 0));
		pOut[i].i32ClanId		= atoi(result.GetValue(i, 1));
		const char* pszName = result.GetValue(i, 2);
		if (pszName)
			strncpy(pOut[i].szClanName, pszName, sizeof(pOut[i].szClanName) - 1);
		pOut[i].i64ClanExp		= _atoi64(result.GetValue(i, 3));
		pOut[i].i32Wins			= atoi(result.GetValue(i, 4));
		pOut[i].i32Losses		= atoi(result.GetValue(i, 5));
		pOut[i].i32MemberCount	= atoi(result.GetValue(i, 6));
	}

	return count;
}

bool ModuleDBGameData::CalculateRankings()
{
	if (!m_pPool)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	// Calculate individual rankings
	pConn->Execute("SELECT fn_calculate_daily_rankings()");
	// Calculate clan rankings
	pConn->Execute("SELECT fn_calculate_clan_rankings()");

	m_pPool->ReleaseConnection(pConn);

	printf("[ModuleDBGameData] Daily rankings calculated\n");
	return true;
}

// ============================================================================
// Server Config / Notices / Boost Events
// ============================================================================

int ModuleDBGameData::LoadServerConfig(IS_SERVER_CONFIG_ENTRY* pOut, int i32MaxCount)
{
	if (!m_pPool || !pOut)
		return 0;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return 0;

	DBResult result = pConn->Execute(
		"SELECT config_key, config_value FROM pb_server_config ORDER BY config_key");

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
		return 0;

	int count = result.GetRowCount();
	if (count > i32MaxCount)
		count = i32MaxCount;

	for (int i = 0; i < count; i++)
	{
		memset(&pOut[i], 0, sizeof(IS_SERVER_CONFIG_ENTRY));
		const char* key = result.GetValue(i, 0);
		const char* val = result.GetValue(i, 1);
		if (key) strncpy(pOut[i].szKey, key, sizeof(pOut[i].szKey) - 1);
		if (val) strncpy(pOut[i].szValue, val, sizeof(pOut[i].szValue) - 1);
	}

	printf("[ModuleDBGameData] Loaded %d server config entries\n", count);
	return count;
}

int ModuleDBGameData::LoadNotices(IS_NOTICE_ENTRY* pOut, int i32MaxCount)
{
	if (!m_pPool || !pOut)
		return 0;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return 0;

	DBResult result = pConn->Execute(
		"SELECT id, repeat_interval, notice_text FROM pb_notices "
		"WHERE is_active = TRUE AND begin_time <= NOW() AND end_time >= NOW() "
		"ORDER BY id");

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
		return 0;

	int count = result.GetRowCount();
	if (count > i32MaxCount)
		count = i32MaxCount;

	for (int i = 0; i < count; i++)
	{
		memset(&pOut[i], 0, sizeof(IS_NOTICE_ENTRY));
		pOut[i].i32Id = atoi(result.GetValue(i, 0));
		pOut[i].i32RepeatInterval = atoi(result.GetValue(i, 1));
		const char* text = result.GetValue(i, 2);
		if (text) strncpy(pOut[i].szText, text, sizeof(pOut[i].szText) - 1);
	}

	printf("[ModuleDBGameData] Loaded %d notices\n", count);
	return count;
}

int ModuleDBGameData::LoadBoostEvents(IS_BOOST_EVENT_ENTRY* pOut, int i32MaxCount)
{
	if (!m_pPool || !pOut)
		return 0;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return 0;

	DBResult result = pConn->Execute(
		"SELECT event_type, exp_multiplier, gp_multiplier, "
		"EXTRACT(EPOCH FROM start_date)::INTEGER, EXTRACT(EPOCH FROM end_date)::INTEGER "
		"FROM pb_boost_events "
		"WHERE end_date >= NOW() ORDER BY start_date");

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
		return 0;

	int count = result.GetRowCount();
	if (count > i32MaxCount)
		count = i32MaxCount;

	for (int i = 0; i < count; i++)
	{
		pOut[i].i32EventType		= atoi(result.GetValue(i, 0));
		pOut[i].i32ExpMultiplier	= atoi(result.GetValue(i, 1));
		pOut[i].i32GPMultiplier		= atoi(result.GetValue(i, 2));
		pOut[i].ui32StartTime		= (uint32_t)atoi(result.GetValue(i, 3));
		pOut[i].ui32EndTime			= (uint32_t)atoi(result.GetValue(i, 4));
	}

	printf("[ModuleDBGameData] Loaded %d boost events\n", count);
	return count;
}

// ============================================================================
// Shop catalog (unchanged from before)
// ============================================================================

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
		if (pAck) pAck->i32Result = 3;
		return;
	}

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
	{
		pAck->i32Result = 3;
		return;
	}

	char szUID[32];
	snprintf(szUID, sizeof(szUID), "%lld", pReq->i64UID);

	const char* balParams[1] = { szUID };
	DBResult balResult = pConn->ExecuteParams(
		"SELECT gp, cash FROM pb_users WHERE uid = $1", 1, balParams);

	if (!balResult.IsSuccess() || balResult.GetRowCount() == 0)
	{
		m_pPool->ReleaseConnection(pConn);
		pAck->i32Result = 3;
		return;
	}

	int currentGP = atoi(balResult.GetValue(0, 0));
	int currentCash = atoi(balResult.GetValue(0, 1));
	int price = pReq->i32Price;

	if (pReq->ui8PayType == 0)
	{
		if (currentGP < price)
		{
			m_pPool->ReleaseConnection(pConn);
			pAck->i32Result = 1;
			pAck->i32RemainingGP = currentGP;
			pAck->i32RemainingCash = currentCash;
			return;
		}
		currentGP -= price;
	}
	else
	{
		if (currentCash < price)
		{
			m_pPool->ReleaseConnection(pConn);
			pAck->i32Result = 1;
			pAck->i32RemainingGP = currentGP;
			pAck->i32RemainingCash = currentCash;
			return;
		}
		currentCash -= price;
	}

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
		pAck->i32Result = 3;
		return;
	}

	pAck->i32Result = 0;
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

// ============================================================================
// Shop Coupons
// ============================================================================

bool ModuleDBGameData::RedeemCoupon(IS_SHOP_COUPON_REQ* pReq, IS_SHOP_COUPON_ACK* pAck)
{
	if (!m_pPool || !pReq || !pAck)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
	{
		pAck->i32Result = 4;
		return false;
	}

	// Find coupon
	const char* codeParams[1] = { pReq->szCouponCode };
	DBResult codeResult = pConn->ExecuteParams(
		"SELECT id, discount_type, discount_value, limit_type, limit_value, times_used, "
		"start_date, end_date "
		"FROM pb_shop_coupons WHERE coupon_code = $1",
		1, codeParams);

	if (!codeResult.IsSuccess() || codeResult.GetRowCount() == 0)
	{
		m_pPool->ReleaseConnection(pConn);
		pAck->i32Result = 1;	// Invalid code
		return false;
	}

	int couponId = atoi(codeResult.GetValue(0, 0));
	int discountType = atoi(codeResult.GetValue(0, 1));
	int discountValue = atoi(codeResult.GetValue(0, 2));
	int limitType = atoi(codeResult.GetValue(0, 3));
	int limitValue = atoi(codeResult.GetValue(0, 4));
	int timesUsed = atoi(codeResult.GetValue(0, 5));

	// Check global limit
	if (limitType == 2 && timesUsed >= limitValue)
	{
		m_pPool->ReleaseConnection(pConn);
		pAck->i32Result = 4;	// Limit reached
		return false;
	}

	// Check per-user usage
	char szUID[32], szCouponId[16];
	snprintf(szUID, sizeof(szUID), "%lld", pReq->i64UID);
	snprintf(szCouponId, sizeof(szCouponId), "%d", couponId);

	const char* usageParams[2] = { szUID, szCouponId };
	DBResult usageResult = pConn->ExecuteParams(
		"SELECT 1 FROM pb_shop_coupon_usage WHERE uid = $1 AND coupon_id = $2",
		2, usageParams);

	if (usageResult.IsSuccess() && usageResult.GetRowCount() > 0)
	{
		m_pPool->ReleaseConnection(pConn);
		pAck->i32Result = 3;	// Already used
		return false;
	}

	// Record usage
	pConn->ExecuteParams(
		"INSERT INTO pb_shop_coupon_usage (uid, coupon_id) VALUES ($1, $2) ON CONFLICT DO NOTHING",
		2, usageParams);

	// Increment global counter
	const char* incParams[1] = { szCouponId };
	pConn->ExecuteParams(
		"UPDATE pb_shop_coupons SET times_used = times_used + 1 WHERE id = $1",
		1, incParams);

	m_pPool->ReleaseConnection(pConn);

	pAck->i32Result = 0;
	pAck->i32DiscountValue = discountValue;
	pAck->ui8DiscountType = (uint8_t)discountType;

	printf("[ModuleDBGameData] Coupon redeemed: UID=%lld code='%s' discount=%d\n",
		pReq->i64UID, pReq->szCouponCode, discountValue);
	return true;
}

// ============================================================================
// Gift Delivery System
// ============================================================================

bool ModuleDBGameData::SendGift(IS_GIFT_SEND_REQ* pReq)
{
	if (!m_pPool || !pReq)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szRecvUID[32], szSenderUID[32], szGoodsId[16], szItemId[16], szType[8];
	snprintf(szRecvUID, sizeof(szRecvUID), "%lld", pReq->i64ReceiverUID);
	snprintf(szSenderUID, sizeof(szSenderUID), "%lld", pReq->i64SenderUID);
	snprintf(szGoodsId, sizeof(szGoodsId), "%d", pReq->i32GoodsId);
	snprintf(szItemId, sizeof(szItemId), "%d", pReq->i32ItemId);
	snprintf(szType, sizeof(szType), "%d", pReq->ui8GiftType);

	const char* params[7] = { szRecvUID, szSenderUID, pReq->szSenderNick,
							   szGoodsId, szItemId, szType, pReq->szMessage };
	DBResult result = pConn->ExecuteParams(
		"INSERT INTO pb_gifts (receiver_uid, sender_uid, sender_nick, goods_id, item_id, gift_type, message) "
		"VALUES ($1, $2, $3, $4, $5, $6, $7)",
		7, params);

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
	{
		printf("[ModuleDBGameData] ERROR: SendGift failed for receiver=%lld\n", pReq->i64ReceiverUID);
		return false;
	}

	printf("[ModuleDBGameData] Gift sent: sender=%lld -> receiver=%lld goods=%d\n",
		pReq->i64SenderUID, pReq->i64ReceiverUID, pReq->i32GoodsId);
	return true;
}

int ModuleDBGameData::LoadGifts(int64_t i64UID, IS_GIFT_ENTRY* pOut, int i32MaxCount)
{
	if (!m_pPool || !pOut)
		return 0;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return 0;

	char szUID[32];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);

	const char* params[1] = { szUID };
	DBResult result = pConn->ExecuteParams(
		"SELECT id, sender_uid, sender_nick, goods_id, item_id, gift_type, message, "
		"EXTRACT(EPOCH FROM created_at)::INTEGER "
		"FROM pb_gifts WHERE receiver_uid = $1 AND is_delivered = FALSE "
		"ORDER BY created_at DESC LIMIT 50",
		1, params);

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
		return 0;

	int count = result.GetRowCount();
	if (count > i32MaxCount)
		count = i32MaxCount;

	for (int i = 0; i < count; i++)
	{
		memset(&pOut[i], 0, sizeof(IS_GIFT_ENTRY));
		pOut[i].i64GiftId		= _atoi64(result.GetValue(i, 0));
		pOut[i].i64SenderUID	= _atoi64(result.GetValue(i, 1));
		const char* nick = result.GetValue(i, 2);
		if (nick) strncpy(pOut[i].szSenderNick, nick, sizeof(pOut[i].szSenderNick) - 1);
		pOut[i].i32GoodsId		= atoi(result.GetValue(i, 3));
		pOut[i].i32ItemId		= atoi(result.GetValue(i, 4));
		pOut[i].ui8GiftType		= (uint8_t)atoi(result.GetValue(i, 5));
		const char* msg = result.GetValue(i, 6);
		if (msg) strncpy(pOut[i].szMessage, msg, sizeof(pOut[i].szMessage) - 1);
		pOut[i].ui32Timestamp	= (uint32_t)atoi(result.GetValue(i, 7));
	}

	return count;
}

bool ModuleDBGameData::ReceiveGift(int64_t i64UID, int64_t i64GiftId, IS_GIFT_RECEIVE_ACK* pAck)
{
	if (!m_pPool || !pAck)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
	{
		pAck->i32Result = 3;
		return false;
	}

	char szUID[32], szGiftId[32];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);
	snprintf(szGiftId, sizeof(szGiftId), "%lld", i64GiftId);

	// Mark gift as delivered and get item info
	const char* params[2] = { szGiftId, szUID };
	DBResult result = pConn->ExecuteParams(
		"UPDATE pb_gifts SET is_delivered = TRUE "
		"WHERE id = $1 AND receiver_uid = $2 AND is_delivered = FALSE "
		"RETURNING item_id, goods_id",
		2, params);

	if (!result.IsSuccess() || result.GetRowCount() == 0)
	{
		m_pPool->ReleaseConnection(pConn);
		pAck->i32Result = 1;	// Not found or already received
		return false;
	}

	pAck->i32ItemId = atoi(result.GetValue(0, 0));
	pAck->i32GoodsId = atoi(result.GetValue(0, 1));

	// Add item to inventory if item_id > 0
	if (pAck->i32ItemId > 0)
	{
		char szItemId[16];
		snprintf(szItemId, sizeof(szItemId), "%d", pAck->i32ItemId);

		const char* invenParams[2] = { szUID, szItemId };
		pConn->ExecuteParams(
			"INSERT INTO pb_inventory (uid, item_id, item_count) VALUES ($1, $2, 1)",
			2, invenParams);
	}

	m_pPool->ReleaseConnection(pConn);

	pAck->i32Result = 0;
	pAck->i64UID = i64UID;
	pAck->i64GiftId = i64GiftId;

	printf("[ModuleDBGameData] Gift received: UID=%lld gift=%lld item=%d\n",
		i64UID, i64GiftId, pAck->i32ItemId);
	return true;
}

void ModuleDBGameData::ProcessResponses(DataServerContext* pContext)
{
	// Placeholder para futuro async con ring buffers
}
