#ifndef __ROOMDEF_H__
#define __ROOMDEF_H__

#pragma once

// Room definitions para el GameServer remake
// Usa las definiciones de Common cuando sea posible, define las propias para simplificacion

// Constantes de room (del original CommonDef.h)
// SLOT_MAX_COUNT, TEAM_COUNT, GAME_CHANNEL_COUNT ya definidos en CommonDef.h
// Los re-definimos aqui solo si no estan disponibles via includes

#ifndef SLOT_MAX_COUNT
#define SLOT_MAX_COUNT			16
#endif

#ifndef TEAM_COUNT
#define TEAM_COUNT				2
#endif

#ifndef GAME_CHANNEL_COUNT
#define GAME_CHANNEL_COUNT		10
#endif

#define MAX_ROOM_COUNT_PER_CHANNEL	200

// Room states simplificados para el remake
// Los ROOM_STATE y SLOT_STATE originales ya estan en CommonDef.h
// Usamos los del original directamente

// Estructura simplificada de info de creacion de room para el remake
struct GameRoomCreateInfo
{
	char		szTitle[64];
	uint32_t	ui32StageId;
	uint8_t		ui8MaxPlayers;
	char		szPassword[16];
	uint8_t		ui8TeamBalance;

	GameRoomCreateInfo()
		: ui32StageId(0)
		, ui8MaxPlayers(16)
		, ui8TeamBalance(1)
	{
		szTitle[0] = '\0';
		szPassword[0] = '\0';
	}
};

// Estructura simplificada de slot para el remake
struct GameSlotInfo
{
	uint8_t		ui8State;		// SLOT_STATE enum
	uint8_t		ui8Team;		// TEAM_TYPE (0=RED, 1=BLUE)
	int			i32SessionIdx;	// Index de la sesion (-1 = vacio)

	GameSlotInfo()
		: ui8State(0)	// SLOT_STATE_EMPTY
		, ui8Team(0)
		, i32SessionIdx(-1)
	{
	}

	void Reset()
	{
		ui8State = 0;
		ui8Team = 0;
		i32SessionIdx = -1;
	}
};

#endif // __ROOMDEF_H__
