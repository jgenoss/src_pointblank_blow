#ifndef __MODULEDBUSERLOAD_H__
#define __MODULEDBUSERLOAD_H__

#pragma once
#include "InterServerProtocol.h"

class DBConnectionPool;
class DataServerContext;

// Modulo de carga de datos de jugador
// Carga perfil, inventario, estadisticas, cosmetics, mode records, daily records
class ModuleDBUserLoad
{
public:
	ModuleDBUserLoad(DBConnectionPool* pPool);
	~ModuleDBUserLoad();

	// Carga completa de datos del jugador
	bool LoadPlayerData(int64_t i64UID, IS_PLAYER_LOAD_ACK* pOut,
						char* pPayload, int* pPayloadSize);

	// Placeholder para futuro async
	void ProcessResponses(DataServerContext* pContext);

private:
	bool LoadProfile(DBConnection* pConn, int64_t i64UID, IS_PLAYER_LOAD_DATA* pData);
	bool LoadStats(DBConnection* pConn, int64_t i64UID, IS_PLAYER_LOAD_DATA* pData);
	int  LoadInventory(DBConnection* pConn, int64_t i64UID,
					   IS_PLAYER_INVENTORY_ITEM* pItems, int i32MaxItems);
	bool LoadCosmetics(DBConnection* pConn, int64_t i64UID, IS_PLAYER_LOAD_DATA* pData);
	bool LoadDailyRecord(DBConnection* pConn, int64_t i64UID, IS_PLAYER_LOAD_DATA* pData);

private:
	DBConnectionPool*	m_pPool;
};

#endif // __MODULEDBUSERLOAD_H__
