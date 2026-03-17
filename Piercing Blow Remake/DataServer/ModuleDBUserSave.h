#ifndef __MODULEDBUSERSAVE_H__
#define __MODULEDBUSERSAVE_H__

#pragma once

class DBConnectionPool;
class DataServerContext;

// Modulo de guardado de datos de jugador
// Guarda perfil, inventario, estadisticas (con draws, disconnects),
// tiempo de conexion y batalla
class ModuleDBUserSave
{
public:
	ModuleDBUserSave(DBConnectionPool* pPool);
	~ModuleDBUserSave();

	// Guardar datos basicos del jugador (con coin)
	bool SavePlayerData(int64_t i64UID, int i32Level, int64_t i64Exp,
						int i32Cash, int i32GP, int i32Coin = 0);

	// Guardar estadisticas de batalla (acumulativo, con draws y disconnects)
	bool SaveBattleStats(int64_t i64UID, int i32Kills, int i32Deaths,
						 int i32Headshots, int i32Wins, int i32Losses,
						 int i32Draws = 0, int i32Disconnects = 0);

	// Guardar tiempo de conexion y batalla (acumulativo)
	bool SaveConnectionTime(int64_t i64UID, int i32ConnectTimeDelta, int i32BattleTimeDelta);

	// Guardar logout timestamp
	bool SaveLogout(int64_t i64UID);

	// Placeholder para futuro async
	void ProcessResponses(DataServerContext* pContext);

private:
	DBConnectionPool*	m_pPool;
};

#endif // __MODULEDBUSERSAVE_H__
