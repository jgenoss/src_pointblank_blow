#ifndef __SERVERINFO_H__
#define __SERVERINFO_H__

#pragma once
#include <cstdint>
#include "IServer.h"

#define MAX_SERVER_NAME_LENGTH		64
#define MAX_SERVER_IP_LENGTH		32
#define MAX_SERVER_REGION_LENGTH	32
#define MAX_SERVER_COUNT			32

enum class ServerOnlineState
{
	Offline = 0,
	Online,
	Full,
};

struct ServerInfo
{
	int					id;
	ServerType			type;
	char				name[MAX_SERVER_NAME_LENGTH];
	char				ip[MAX_SERVER_IP_LENGTH];
	uint16_t			port;
	ServerOnlineState	state;
	int					maxPlayers;
	int					currentPlayers;
	char				region[MAX_SERVER_REGION_LENGTH];

	ServerInfo()
	{
		Reset();
	}

	void Reset()
	{
		id				= 0;
		type			= ServerType::Game;
		name[0]			= '\0';
		ip[0]			= '\0';
		port			= 0;
		state			= ServerOnlineState::Offline;
		maxPlayers		= 0;
		currentPlayers	= 0;
		region[0]		= '\0';
	}
};

#endif // __SERVERINFO_H__
