#ifndef __MODULEDBUSERLOAD_H__
#define __MODULEDBUSERLOAD_H__

#pragma once
#include "InterServerProtocol.h"

class DBConnectionPool;
class DataServerContext;

// Modulo de carga de datos de jugador
// Carga perfil, inventario y estadisticas desde PostgreSQL
// Patron portado de CModuleDBUserLoad (Server/Source/Trans/Trans/ModuleDBUserLoad.h)
// Version simplificada: sincronico, single-thread
class ModuleDBUserLoad
{
public:
	ModuleDBUserLoad(DBConnectionPool* pPool);
	~ModuleDBUserLoad();

	// Carga completa de datos del jugador
	// pPayload: buffer de salida con IS_PLAYER_LOAD_DATA + N * IS_PLAYER_INVENTORY_ITEM
	// pPayloadSize: tamanio del payload escrito
	bool LoadPlayerData(int64_t i64UID, IS_PLAYER_LOAD_ACK* pOut,
						char* pPayload, int* pPayloadSize);

	// Placeholder para futuro async
	void ProcessResponses(DataServerContext* pContext);

private:
	bool LoadProfile(DBConnection* pConn, int64_t i64UID, IS_PLAYER_LOAD_DATA* pData);
	bool LoadStats(DBConnection* pConn, int64_t i64UID, IS_PLAYER_LOAD_DATA* pData);
	int  LoadInventory(DBConnection* pConn, int64_t i64UID,
					   IS_PLAYER_INVENTORY_ITEM* pItems, int i32MaxItems);

private:
	DBConnectionPool*	m_pPool;
};

#endif // __MODULEDBUSERLOAD_H__
