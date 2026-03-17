#ifndef __BATTLEDEF_H__
#define __BATTLEDEF_H__

#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BattleServer Definitions
// Port simplificado de DediDef.h con las constantes y enums necesarios para el BattleServer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Limites
#define MAX_BATTLE_ROOMS			200			// Max battle rooms simultaneas
#define MAX_BATTLE_SESSIONS			32			// Max GameServers conectados
#define BATTLE_SLOT_MAX				16			// SLOT_MAX_COUNT - max jugadores por room
#define BATTLE_TEAM_COUNT			2			// RED y BLUE
#define BATTLE_MAX_PLAYER_RESULTS	16			// Max resultados por batalla

// Timeouts y timers
#define BATTLE_DESTROY_TIMEOUT		120			// Segundos sin actividad para destruir room (GROUP_DESTORY_TIME)
#define BATTLE_DEFAULT_ROUND_TIME	180			// 3 minutos por round default
#define BATTLE_MEMBER_TIMEOUT		30			// Segundos sin paquetes para considerar miembro inactivo

// UDP
#define BATTLE_UDP_BASE_PORT		41000		// Puerto UDP base para relay
#define BATTLE_UDP_RECV_BUFFER		65536		// Buffer de recepcion UDP
#define BATTLE_UDP_PACKET_HEADER	8			// Header de paquete UDP relay (roomIdx + slotIdx + size)

// Estado de la battle room (del ENUM_GROUP_STATE original)
enum BattleRoomState
{
	BATTLE_ROOM_INVALID		= -1,
	BATTLE_ROOM_READY		= 0,				// Room creada, esperando jugadores
	BATTLE_ROOM_USE			= 1,				// Batalla en progreso
	BATTLE_ROOM_ENDING		= 2,				// Batalla terminando, enviando resultados
};

// Estado del miembro en batalla
enum BattleMemberState
{
	BATTLE_MEMBER_NONE		= 0,
	BATTLE_MEMBER_JOINED,						// Se unio pero no ha empezado
	BATTLE_MEMBER_PLAYING,						// En batalla activa
	BATTLE_MEMBER_DEAD,							// Muerto en round actual
	BATTLE_MEMBER_LEFT,							// Salio de la batalla
};

// Equipos
enum BattleTeam
{
	BATTLE_TEAM_RED			= 0,
	BATTLE_TEAM_BLUE		= 1,
};

// Resultado de un jugador en la batalla
#pragma pack(push, 1)

struct BattlePlayerResult
{
	int64_t		i64UID;
	int			i32Kills;
	int			i32Deaths;
	int			i32Headshots;
	int			i32Assists;
	int			i32Team;

	void Reset()
	{
		i64UID = 0;
		i32Kills = 0;
		i32Deaths = 0;
		i32Headshots = 0;
		i32Assists = 0;
		i32Team = 0;
	}
};

// Resultado completo de la batalla
struct BattleResult
{
	int			i32RoomIdx;						// Room index en GameServer
	int			i32ChannelNum;					// Canal en GameServer
	int			i32WinnerTeam;					// BATTLE_TEAM_RED o BATTLE_TEAM_BLUE (-1 = draw)
	int			i32RedScore;
	int			i32BlueScore;
	int			i32PlayerCount;
	BattlePlayerResult	Players[BATTLE_MAX_PLAYER_RESULTS];

	void Reset()
	{
		i32RoomIdx = -1;
		i32ChannelNum = -1;
		i32WinnerTeam = -1;
		i32RedScore = 0;
		i32BlueScore = 0;
		i32PlayerCount = 0;
		for (int i = 0; i < BATTLE_MAX_PLAYER_RESULTS; i++)
			Players[i].Reset();
	}
};

// Header de paquete UDP relay
struct UdpRelayHeader
{
	uint16_t	ui16RoomIdx;					// Battle room index
	uint16_t	ui16SlotIdx;					// Slot del sender
	uint16_t	ui16DataSize;					// Tamano de datos que siguen
	uint16_t	ui16Reserved;					// Padding
};

#pragma pack(pop)

#endif // __BATTLEDEF_H__
