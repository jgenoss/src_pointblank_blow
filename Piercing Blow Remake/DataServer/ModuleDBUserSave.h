#ifndef __MODULEDBUSERSAVE_H__
#define __MODULEDBUSERSAVE_H__

#pragma once

class DBConnectionPool;
class DataServerContext;

// Modulo de guardado de datos de jugador
// Guarda perfil, inventario y estadisticas a PostgreSQL
// Patron portado de CModuleDBUserSave (Server/Source/Trans/Trans/ModuleDBUserSave.h)
// Version simplificada: sincronico, single-thread
class ModuleDBUserSave
{
public:
	ModuleDBUserSave(DBConnectionPool* pPool);
	~ModuleDBUserSave();

	// Guardar datos basicos del jugador
	bool SavePlayerData(int64_t i64UID, int i32Level, int64_t i64Exp,
						int i32Cash, int i32GP);

	// Guardar estadisticas de batalla (acumulativo)
	bool SaveBattleStats(int64_t i64UID, int i32Kills, int i32Deaths,
						 int i32Headshots, int i32Wins, int i32Losses);

	// Placeholder para futuro async
	void ProcessResponses(DataServerContext* pContext);

private:
	DBConnectionPool*	m_pPool;
};

#endif // __MODULEDBUSERSAVE_H__
