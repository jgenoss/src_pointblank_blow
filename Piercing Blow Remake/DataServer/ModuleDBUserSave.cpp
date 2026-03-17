#include "ModuleDBUserSave.h"
#include "DBConnectionPool.h"
#include <cstdio>
#include <cstring>

ModuleDBUserSave::ModuleDBUserSave(DBConnectionPool* pPool)
	: m_pPool(pPool)
{
}

ModuleDBUserSave::~ModuleDBUserSave()
{
}

bool ModuleDBUserSave::SavePlayerData(int64_t i64UID, int i32Level, int64_t i64Exp,
									  int i32Cash, int i32GP, int i32Coin)
{
	if (!m_pPool)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szLevel[16], szExp[32], szCash[16], szGP[16], szCoin[16], szUID[32];
	snprintf(szLevel, sizeof(szLevel), "%d", i32Level);
	snprintf(szExp, sizeof(szExp), "%lld", i64Exp);
	snprintf(szCash, sizeof(szCash), "%d", i32Cash);
	snprintf(szGP, sizeof(szGP), "%d", i32GP);
	snprintf(szCoin, sizeof(szCoin), "%d", i32Coin);
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);

	const char* paramValues[6] = { szLevel, szExp, szCash, szGP, szCoin, szUID };
	DBResult result = pConn->ExecuteParams(
		"UPDATE pb_users SET level = $1, exp = $2, cash = $3, gp = $4, coin = $5 "
		"WHERE uid = $6",
		6, paramValues);

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
	{
		printf("[ModuleDBUserSave] ERROR: SavePlayerData failed for UID=%lld\n", i64UID);
		return false;
	}

	printf("[ModuleDBUserSave] Saved UID=%lld: level=%d exp=%lld cash=%d gp=%d coin=%d\n",
		i64UID, i32Level, i64Exp, i32Cash, i32GP, i32Coin);

	return true;
}

bool ModuleDBUserSave::SaveBattleStats(int64_t i64UID, int i32Kills, int i32Deaths,
									   int i32Headshots, int i32Wins, int i32Losses,
									   int i32Draws, int i32Disconnects)
{
	if (!m_pPool)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szKills[16], szDeaths[16], szHeadshots[16], szWins[16], szLosses[16];
	char szDraws[16], szDisconnects[16], szUID[32];
	snprintf(szKills, sizeof(szKills), "%d", i32Kills);
	snprintf(szDeaths, sizeof(szDeaths), "%d", i32Deaths);
	snprintf(szHeadshots, sizeof(szHeadshots), "%d", i32Headshots);
	snprintf(szWins, sizeof(szWins), "%d", i32Wins);
	snprintf(szLosses, sizeof(szLosses), "%d", i32Losses);
	snprintf(szDraws, sizeof(szDraws), "%d", i32Draws);
	snprintf(szDisconnects, sizeof(szDisconnects), "%d", i32Disconnects);
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);

	const char* paramValues[8] = { szKills, szDeaths, szHeadshots, szWins, szLosses,
								   szDraws, szDisconnects, szUID };
	DBResult result = pConn->ExecuteParams(
		"UPDATE pb_stats SET "
		"kills = kills + $1, deaths = deaths + $2, headshots = headshots + $3, "
		"wins = wins + $4, losses = losses + $5, "
		"draws = draws + $6, disconnects = disconnects + $7 "
		"WHERE uid = $8",
		8, paramValues);

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
	{
		printf("[ModuleDBUserSave] ERROR: SaveBattleStats failed for UID=%lld\n", i64UID);
		return false;
	}

	printf("[ModuleDBUserSave] Stats saved UID=%lld: +%dk +%dd +%dh +%dw +%dl +%ddr +%ddc\n",
		i64UID, i32Kills, i32Deaths, i32Headshots, i32Wins, i32Losses, i32Draws, i32Disconnects);

	return true;
}

bool ModuleDBUserSave::SaveConnectionTime(int64_t i64UID, int i32ConnectTimeDelta, int i32BattleTimeDelta)
{
	if (!m_pPool)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szConnTime[16], szBattleTime[16], szUID[32];
	snprintf(szConnTime, sizeof(szConnTime), "%d", i32ConnectTimeDelta);
	snprintf(szBattleTime, sizeof(szBattleTime), "%d", i32BattleTimeDelta);
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);

	const char* paramValues[3] = { szConnTime, szBattleTime, szUID };
	DBResult result = pConn->ExecuteParams(
		"UPDATE pb_users SET "
		"connect_time = connect_time + $1, "
		"total_battle_time = total_battle_time + $2 "
		"WHERE uid = $3",
		3, paramValues);

	m_pPool->ReleaseConnection(pConn);
	return result.IsSuccess();
}

bool ModuleDBUserSave::SaveLogout(int64_t i64UID)
{
	if (!m_pPool)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szUID[32];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);

	const char* paramValues[1] = { szUID };
	DBResult result = pConn->ExecuteParams(
		"UPDATE pb_users SET last_logout = NOW() WHERE uid = $1",
		1, paramValues);

	m_pPool->ReleaseConnection(pConn);
	return result.IsSuccess();
}

void ModuleDBUserSave::ProcessResponses(DataServerContext* pContext)
{
	// Placeholder para futuro async con ring buffers
}
