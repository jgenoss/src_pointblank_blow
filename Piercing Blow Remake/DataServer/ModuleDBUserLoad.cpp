#include "ModuleDBUserLoad.h"
#include "DBConnectionPool.h"
#include <cstdio>
#include <cstring>

#define MAX_INVENTORY_ITEMS		200		// Maximo items por jugador

ModuleDBUserLoad::ModuleDBUserLoad(DBConnectionPool* pPool)
	: m_pPool(pPool)
{
}

ModuleDBUserLoad::~ModuleDBUserLoad()
{
}

bool ModuleDBUserLoad::LoadPlayerData(int64_t i64UID, IS_PLAYER_LOAD_ACK* pOut,
									  char* pPayload, int* pPayloadSize)
{
	if (!m_pPool || !pOut || !pPayload || !pPayloadSize)
		return false;

	*pPayloadSize = 0;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
	{
		pOut->i32Result = -1;
		return false;
	}

	// Preparar estructura de datos
	IS_PLAYER_LOAD_DATA loadData;
	memset(&loadData, 0, sizeof(loadData));
	loadData.i64UID = i64UID;

	// Cargar perfil (con campos extendidos)
	if (!LoadProfile(pConn, i64UID, &loadData))
	{
		pOut->i32Result = 1;	// Usuario no encontrado
		m_pPool->ReleaseConnection(pConn);
		return false;
	}

	// Cargar estadisticas (con draws y disconnects)
	LoadStats(pConn, i64UID, &loadData);

	// Cargar cosmeticos
	LoadCosmetics(pConn, i64UID, &loadData);

	// Cargar inventario
	IS_PLAYER_INVENTORY_ITEM items[MAX_INVENTORY_ITEMS];
	int i32ItemCount = LoadInventory(pConn, i64UID, items, MAX_INVENTORY_ITEMS);
	loadData.i32InventoryCount = i32ItemCount;

	m_pPool->ReleaseConnection(pConn);

	// Serializar payload: IS_PLAYER_LOAD_DATA + items[]
	memcpy(pPayload, &loadData, sizeof(loadData));
	int i32Offset = sizeof(loadData);

	if (i32ItemCount > 0)
	{
		int i32ItemsSize = i32ItemCount * (int)sizeof(IS_PLAYER_INVENTORY_ITEM);
		memcpy(pPayload + i32Offset, items, i32ItemsSize);
		i32Offset += i32ItemsSize;
	}

	*pPayloadSize = i32Offset;
	pOut->i32Result = 0;	// OK

	printf("[ModuleDBUserLoad] Loaded UID=%lld: nick='%s' level=%d items=%d\n",
		i64UID, loadData.szNickname, loadData.i32Level, i32ItemCount);

	return true;
}

bool ModuleDBUserLoad::LoadProfile(DBConnection* pConn, int64_t i64UID,
								   IS_PLAYER_LOAD_DATA* pData)
{
	char szUID[32];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);

	const char* paramValues[1] = { szUID };
	DBResult result = pConn->ExecuteParams(
		"SELECT nickname, level, exp, cash, gp, coin, rank_id, keep_rank, clan_id, "
		"auth_level, connect_count, connect_time, total_battle_time, "
		"tutorial_done, guide_complete "
		"FROM pb_users WHERE uid = $1",
		1, paramValues);

	if (!result.IsSuccess() || result.GetRowCount() == 0)
		return false;

	// Copiar nickname con seguridad
	const char* pszNick = result.GetString(0, "nickname");
	if (pszNick && pszNick[0])
		strncpy(pData->szNickname, pszNick, sizeof(pData->szNickname) - 1);

	pData->i32Level				= result.GetInt32(0, "level");
	pData->i64Exp				= result.GetInt64(0, "exp");
	pData->i32Cash				= result.GetInt32(0, "cash");
	pData->i32GP				= result.GetInt32(0, "gp");
	pData->i32Coin				= result.GetInt32(0, "coin");
	pData->i32RankId			= result.GetInt32(0, "rank_id");
	pData->i32KeepRank			= result.GetInt32(0, "keep_rank");
	pData->i32ClanId			= result.GetInt32(0, "clan_id");
	pData->ui8AuthLevel			= (uint8_t)result.GetInt32(0, "auth_level");
	pData->i32ConnectCount		= result.GetInt32(0, "connect_count");
	pData->i32ConnectTime		= result.GetInt32(0, "connect_time");
	pData->i32TotalBattleTime	= result.GetInt32(0, "total_battle_time");
	pData->ui8TutorialDone		= (uint8_t)result.GetInt32(0, "tutorial_done");
	pData->ui64GuideComplete	= (uint64_t)result.GetInt64(0, "guide_complete");

	// Update connect count and last_login
	pConn->ExecuteParams(
		"UPDATE pb_users SET connect_count = connect_count + 1, last_login = NOW() WHERE uid = $1",
		1, paramValues);

	return true;
}

bool ModuleDBUserLoad::LoadStats(DBConnection* pConn, int64_t i64UID,
								 IS_PLAYER_LOAD_DATA* pData)
{
	char szUID[32];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);

	const char* paramValues[1] = { szUID };
	DBResult result = pConn->ExecuteParams(
		"SELECT kills, deaths, headshots, wins, losses, draws, disconnects "
		"FROM pb_stats WHERE uid = $1",
		1, paramValues);

	if (!result.IsSuccess() || result.GetRowCount() == 0)
		return false;

	pData->i32Kills			= result.GetInt32(0, "kills");
	pData->i32Deaths		= result.GetInt32(0, "deaths");
	pData->i32Headshots		= result.GetInt32(0, "headshots");
	pData->i32Wins			= result.GetInt32(0, "wins");
	pData->i32Losses		= result.GetInt32(0, "losses");
	pData->i32Draws			= result.GetInt32(0, "draws");
	pData->i32Disconnects	= result.GetInt32(0, "disconnects");

	return true;
}

bool ModuleDBUserLoad::LoadCosmetics(DBConnection* pConn, int64_t i64UID,
									  IS_PLAYER_LOAD_DATA* pData)
{
	char szUID[32];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);

	const char* paramValues[1] = { szUID };
	DBResult result = pConn->ExecuteParams(
		"SELECT nickname_color, crosshair_color, chatting_color, disguise_rank "
		"FROM pb_account_cosmetics WHERE uid = $1",
		1, paramValues);

	if (!result.IsSuccess() || result.GetRowCount() == 0)
	{
		pData->ui8NicknameColor = 0;
		pData->ui8CrosshairColor = 0;
		pData->ui8ChattingColor = 0;
		pData->i32DisguiseRank = 0;
		return true;
	}

	pData->ui8NicknameColor		= (uint8_t)atoi(result.GetValue(0, 0));
	pData->ui8CrosshairColor	= (uint8_t)atoi(result.GetValue(0, 1));
	pData->ui8ChattingColor		= (uint8_t)atoi(result.GetValue(0, 2));
	pData->i32DisguiseRank		= atoi(result.GetValue(0, 3));

	return true;
}

int ModuleDBUserLoad::LoadInventory(DBConnection* pConn, int64_t i64UID,
									IS_PLAYER_INVENTORY_ITEM* pItems, int i32MaxItems)
{
	char szUID[32];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);

	const char* paramValues[1] = { szUID };
	DBResult result = pConn->ExecuteParams(
		"SELECT item_id, item_count, slot_idx, is_equipped "
		"FROM pb_inventory WHERE uid = $1 ORDER BY slot_idx",
		1, paramValues);

	if (!result.IsSuccess())
		return 0;

	int i32Count = result.GetRowCount();
	if (i32Count > i32MaxItems)
		i32Count = i32MaxItems;

	for (int i = 0; i < i32Count; ++i)
	{
		pItems[i].i32ItemId		= result.GetInt32(i, "item_id");
		pItems[i].i32ItemCount	= result.GetInt32(i, "item_count");
		pItems[i].i32SlotIdx	= result.GetInt32(i, "slot_idx");
		pItems[i].ui8IsEquipped	= result.GetBool(i, "is_equipped") ? 1 : 0;
	}

	return i32Count;
}

void ModuleDBUserLoad::ProcessResponses(DataServerContext* pContext)
{
	// Placeholder para futuro async con ring buffers
}
