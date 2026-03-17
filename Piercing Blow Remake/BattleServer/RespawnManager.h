#ifndef __RESPAWNMANAGER_H__
#define __RESPAWNMANAGER_H__

#pragma once
#include "MapData.h"

// RespawnManager - Handles player respawn position selection
// Uses loaded .i3srpn data to assign spawn positions by team
class RespawnManager
{
public:
	RespawnManager();
	~RespawnManager();

	// Get a spawn point for the given team from the map's respawn data
	// team: RESPAWN_STYLE_RED(0), RESPAWN_STYLE_BLUE(1)
	// Returns false if no spawn points available for this team
	bool	GetSpawnPoint(CMapData* pMapData, int team, float* outPos) const;

	// Get the default respawn delay in seconds
	static float	GetRespawnDelay()		{ return DS_CHARACTER_RESPAWN_TIME; }
};

#endif // __RESPAWNMANAGER_H__
