#ifndef __INTERSERVERPROTOCOL_H__
#define __INTERSERVERPROTOCOL_H__

#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inter-Server Protocol
// Rango 0x7000+ para evitar conflicto con protocolos de cliente (ProtocolDef.h usa 0x0100-0x1C00)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum Protocol_InterServer_Base
{
	PROTOCOL_IS_BASE = 0x7000,

	//----------------------------------------------------------
	// Heartbeat (todos los servidores)
	PROTOCOL_IS_HEARTBEAT_REQ,
	PROTOCOL_IS_HEARTBEAT_ACK,
};

enum Protocol_InterServer_Connect
{
	PROTOCOL_IS_CONNECT = 0x7100,

	//----------------------------------------------------------
	// ConnectServer <-> GameServer: Registro de servidor
	PROTOCOL_IS_SERVER_REGISTER_REQ,			// GameServer se registra con ConnectServer
	PROTOCOL_IS_SERVER_REGISTER_ACK,			// ConnectServer confirma registro

	// ConnectServer <-> GameServer: Estado del servidor
	PROTOCOL_IS_SERVER_STATUS_UPDATE_REQ,		// GameServer reporta su estado (players, state)
	PROTOCOL_IS_SERVER_STATUS_UPDATE_ACK,

	// ConnectServer -> GameServer: Transferencia de autenticacion
	PROTOCOL_IS_PLAYER_AUTH_TRANSFER_REQ,		// ConnectServer notifica token de jugador entrante
	PROTOCOL_IS_PLAYER_AUTH_TRANSFER_ACK,		// GameServer confirma recepcion del token

	// ConnectServer <-> GameServer: Server list
	PROTOCOL_IS_SERVER_LIST_REQ,
	PROTOCOL_IS_SERVER_LIST_ACK,
};

enum Protocol_InterServer_Data
{
	PROTOCOL_IS_DATA = 0x7200,

	//----------------------------------------------------------
	// Auth operations (ConnectServer -> DataServer)
	PROTOCOL_IS_AUTH_LOGIN_REQ,					// Validar credenciales
	PROTOCOL_IS_AUTH_LOGIN_ACK,

	// Player data operations (GameServer <-> DataServer)
	PROTOCOL_IS_PLAYER_LOAD_REQ,				// Cargar datos de jugador
	PROTOCOL_IS_PLAYER_LOAD_ACK,

	PROTOCOL_IS_PLAYER_SAVE_REQ,				// Guardar datos de jugador
	PROTOCOL_IS_PLAYER_SAVE_ACK,

	PROTOCOL_IS_PLAYER_CREATE_NICK_REQ,			// Crear nickname
	PROTOCOL_IS_PLAYER_CREATE_NICK_ACK,

	PROTOCOL_IS_PLAYER_CHECK_NICK_REQ,			// Verificar nickname
	PROTOCOL_IS_PLAYER_CHECK_NICK_ACK,

	// Shop operations (GameServer -> DataServer)
	PROTOCOL_IS_SHOP_BUY_REQ,
	PROTOCOL_IS_SHOP_BUY_ACK,

	PROTOCOL_IS_INVEN_UPDATE_REQ,
	PROTOCOL_IS_INVEN_UPDATE_ACK,

	// Stats operations (GameServer/BattleServer -> DataServer)
	PROTOCOL_IS_STATS_SAVE_REQ,
	PROTOCOL_IS_STATS_SAVE_ACK,
};

enum Protocol_InterServer_Battle
{
	PROTOCOL_IS_BATTLE = 0x7300,

	//----------------------------------------------------------
	// GameServer <-> BattleServer
	PROTOCOL_IS_BATTLE_REGISTER_REQ,			// BattleServer se registra con GameServer
	PROTOCOL_IS_BATTLE_REGISTER_ACK,

	PROTOCOL_IS_BATTLE_CREATE_REQ,				// Crear batalla
	PROTOCOL_IS_BATTLE_CREATE_ACK,

	PROTOCOL_IS_BATTLE_END_NOTIFY,				// Notificar fin de batalla con resultados

	PROTOCOL_IS_PLAYER_MIGRATE_REQ,				// Migrar jugador a BattleServer
	PROTOCOL_IS_PLAYER_MIGRATE_ACK,

	PROTOCOL_IS_BATTLE_STATUS_UPDATE,			// Estado de batalla en progreso
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inter-Server Packet Structures
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma pack(push, 1)

// Heartbeat
struct IS_HEARTBEAT_REQ
{
	int			i32ServerId;
	int			i32ServerType;			// ServerType enum
	int			i32CurrentPlayers;
};

struct IS_HEARTBEAT_ACK
{
	int			i32Result;				// 0 = OK
};

// Server Registration
struct IS_SERVER_REGISTER_REQ
{
	int			i32ServerId;
	int			i32ServerType;			// ServerType enum
	char		szServerName[MAX_SERVER_NAME_LENGTH];
	char		szPublicIP[MAX_SERVER_IP_LENGTH];
	uint16_t	ui16PublicPort;
	int			i32MaxPlayers;
	char		szRegion[MAX_SERVER_REGION_LENGTH];
};

struct IS_SERVER_REGISTER_ACK
{
	int			i32Result;				// 0 = OK, 1 = Rejected
	int			i32AssignedId;			// ID asignado por ConnectServer
};

// Server Status Update
struct IS_SERVER_STATUS_UPDATE_REQ
{
	int			i32ServerId;
	int			i32State;				// ServerOnlineState enum
	int			i32CurrentPlayers;
};

// Auth Transfer (ConnectServer -> GameServer)
struct IS_PLAYER_AUTH_TRANSFER_REQ
{
	int64_t		i64UID;					// User database ID
	uint32_t	ui32AuthToken;			// Token de autenticacion temporal
	char		szUsername[64];
	uint32_t	ui32ClientIP;			// IP del cliente para verificacion
};

struct IS_PLAYER_AUTH_TRANSFER_ACK
{
	int64_t		i64UID;
	int			i32Result;				// 0 = Accepted, 1 = Rejected
};

// Auth Login (ConnectServer -> DataServer)
struct IS_AUTH_LOGIN_REQ
{
	int			i32SessionIdx;			// Session index en ConnectServer
	char		szUsername[64];
	char		szPassword[64];
	uint32_t	ui32ClientIP;
};

struct IS_AUTH_LOGIN_ACK
{
	int			i32SessionIdx;
	int			i32Result;				// 0 = OK, error codes
	int64_t		i64UID;					// User ID if successful
	uint8_t		ui8AuthLevel;			// Authority level
};

// Player Data Load (GameServer -> DataServer)
struct IS_PLAYER_LOAD_REQ
{
	int64_t		i64UID;
	int			i32SessionIdx;			// Session index en GameServer
};

struct IS_PLAYER_LOAD_ACK
{
	int64_t		i64UID;
	int			i32SessionIdx;
	int			i32Result;				// 0 = OK
	// El resto de los datos del jugador se envia como payload adicional
};

// Battle Create (GameServer -> BattleServer)
struct IS_BATTLE_CREATE_REQ
{
	int			i32RoomIdx;
	int			i32ChannelNum;
	uint8_t		ui8GameMode;
	uint8_t		ui8MapIndex;
	uint8_t		ui8MaxPlayers;
	int			i32PlayerCount;
};

struct IS_BATTLE_CREATE_ACK
{
	int			i32RoomIdx;
	int			i32Result;				// 0 = OK
	int			i32BattleRoomIdx;		// Index en BattleServer
	char		szUdpIP[MAX_SERVER_IP_LENGTH];
	uint16_t	ui16UdpPort;
};

#pragma pack(pop)

#endif // __INTERSERVERPROTOCOL_H__
