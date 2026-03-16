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
									  int i32Cash, int i32GP)
{
	if (!m_pPool)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	// Usar transaccion para atomicidad
	if (!pConn->BeginTransaction())
	{
		m_pPool->ReleaseConnection(pConn);
		return false;
	}

	char szLevel[16], szExp[32], szCash[16], szGP[16], szUID[32];
	snprintf(szLevel, sizeof(szLevel), "%d", i32Level);
	snprintf(szExp, sizeof(szExp), "%lld", i64Exp);
	snprintf(szCash, sizeof(szCash), "%d", i32Cash);
	snprintf(szGP, sizeof(szGP), "%d", i32GP);
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);

	const char* paramValues[5] = { szLevel, szExp, szCash, szGP, szUID };
	DBResult result = pConn->ExecuteParams(
		"UPDATE pb_users SET level = $1, exp = $2, cash = $3, gp = $4 "
		"WHERE uid = $5",
		5, paramValues);

	if (!result.IsSuccess())
	{
		pConn->RollbackTransaction();
		m_pPool->ReleaseConnection(pConn);
		printf("[ModuleDBUserSave] ERROR: SavePlayerData failed for UID=%lld\n", i64UID);
		return false;
	}

	pConn->CommitTransaction();
	m_pPool->ReleaseConnection(pConn);

	printf("[ModuleDBUserSave] Saved UID=%lld: level=%d exp=%lld cash=%d gp=%d\n",
		i64UID, i32Level, i64Exp, i32Cash, i32GP);

	return true;
}

bool ModuleDBUserSave::SaveBattleStats(int64_t i64UID, int i32Kills, int i32Deaths,
									   int i32Headshots, int i32Wins, int i32Losses)
{
	if (!m_pPool)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szKills[16], szDeaths[16], szHeadshots[16], szWins[16], szLosses[16], szUID[32];
	snprintf(szKills, sizeof(szKills), "%d", i32Kills);
	snprintf(szDeaths, sizeof(szDeaths), "%d", i32Deaths);
	snprintf(szHeadshots, sizeof(szHeadshots), "%d", i32Headshots);
	snprintf(szWins, sizeof(szWins), "%d", i32Wins);
	snprintf(szLosses, sizeof(szLosses), "%d", i32Losses);
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);

	const char* paramValues[6] = { szKills, szDeaths, szHeadshots, szWins, szLosses, szUID };
	DBResult result = pConn->ExecuteParams(
		"UPDATE pb_stats SET "
		"kills = kills + $1, deaths = deaths + $2, headshots = headshots + $3, "
		"wins = wins + $4, losses = losses + $5 "
		"WHERE uid = $6",
		6, paramValues);

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
	{
		printf("[ModuleDBUserSave] ERROR: SaveBattleStats failed for UID=%lld\n", i64UID);
		return false;
	}

	printf("[ModuleDBUserSave] Stats saved UID=%lld: +%dk +%dd +%dh +%dw +%dl\n",
		i64UID, i32Kills, i32Deaths, i32Headshots, i32Wins, i32Losses);

	return true;
}

void ModuleDBUserSave::ProcessResponses(DataServerContext* pContext)
{
	// Placeholder para futuro async con ring buffers
}
