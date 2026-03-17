#ifndef __MAPMANAGER_H__
#define __MAPMANAGER_H__

#pragma once
#include "MapData.h"

// Maximum maps (matches original DS_MAX_MAP = STAGE_UID_MAX)
#define DS_MAX_MAP		64

// MapManager - Pre-loads all map data from Media/ folder at server startup
// Port of the map management portion of original DediServer
class MapManager
{
public:
	MapManager();
	~MapManager();

	// Load all maps from Media/ folder
	bool		Initialize(const char* pszMediaPath = MEDIA_PATH);
	void		Shutdown();

	// Get map data by map index (same as mapIndex/stageUID sent in battle create)
	CMapData*	GetMapData(int i32MapIndex);

	// Stats
	int			GetLoadedMapCount() const	{ return m_i32LoadedCount; }
	bool		IsInitialized() const		{ return m_bInitialized; }

private:
	CMapData	m_Maps[DS_MAX_MAP];
	int			m_i32LoadedCount;
	bool		m_bInitialized;
};

extern MapManager* g_pMapManager;

#endif // __MAPMANAGER_H__
