#include "pch.h"
#include "MapManager.h"

MapManager* g_pMapManager = nullptr;

MapManager::MapManager()
	: m_i32LoadedCount(0)
	, m_bInitialized(false)
{
}

MapManager::~MapManager()
{
	Shutdown();
}

bool MapManager::Initialize(const char* pszMediaPath)
{
	if (m_bInitialized)
		return true;

	printf("[MapManager] Scanning '%s' for map data files...\n", pszMediaPath);

	// Scan Media/ folder for .i3sobj files to discover available maps
	char szSearchPath[MAX_PATH];
	sprintf_s(szSearchPath, "%s\\*.%s", pszMediaPath, SOBJ_FILE_EXTENSION);

	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFileA(szSearchPath, &findData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		// Try .i3srpn as fallback (some maps may only have respawn data)
		sprintf_s(szSearchPath, "%s\\*.%s", pszMediaPath, SRPN_FILE_EXTENSION);
		hFind = FindFirstFileA(szSearchPath, &findData);
	}

	if (hFind == INVALID_HANDLE_VALUE)
	{
		// Also try .i3scol
		sprintf_s(szSearchPath, "%s\\*.%s", pszMediaPath, SCOL_FILE_EXTENSION);
		hFind = FindFirstFileA(szSearchPath, &findData);
	}

	if (hFind == INVALID_HANDLE_VALUE)
	{
		printf("[MapManager] No map files found in '%s' (this is OK for development)\n", pszMediaPath);
		m_bInitialized = true;
		return true;
	}

	// Collect unique map names (strip extension)
	char mapNames[DS_MAX_MAP][DS_MAX_CODENAME];
	int mapCount = 0;

	do
	{
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;

		// Extract map name (filename without extension)
		char szMapName[DS_MAX_CODENAME];
		strncpy_s(szMapName, findData.cFileName, _TRUNCATE);

		// Remove extension
		char* pDot = strrchr(szMapName, '.');
		if (pDot)
			*pDot = '\0';

		// Check if already in list
		bool bDuplicate = false;
		for (int i = 0; i < mapCount; i++)
		{
			if (_stricmp(mapNames[i], szMapName) == 0)
			{
				bDuplicate = true;
				break;
			}
		}

		if (!bDuplicate && mapCount < DS_MAX_MAP)
		{
			strncpy_s(mapNames[mapCount], szMapName, _TRUNCATE);
			mapCount++;
		}
	} while (FindNextFileA(hFind, &findData));

	FindClose(hFind);

	// Also scan for .i3srpn and .i3scol to find maps that only have those
	const char* extensions[] = { SRPN_FILE_EXTENSION, SCOL_FILE_EXTENSION };
	for (int e = 0; e < 2; e++)
	{
		sprintf_s(szSearchPath, "%s\\*.%s", pszMediaPath, extensions[e]);
		hFind = FindFirstFileA(szSearchPath, &findData);
		if (hFind == INVALID_HANDLE_VALUE)
			continue;

		do
		{
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;

			char szMapName[DS_MAX_CODENAME];
			strncpy_s(szMapName, findData.cFileName, _TRUNCATE);
			char* pDot = strrchr(szMapName, '.');
			if (pDot) *pDot = '\0';

			bool bDuplicate = false;
			for (int i = 0; i < mapCount; i++)
			{
				if (_stricmp(mapNames[i], szMapName) == 0)
				{
					bDuplicate = true;
					break;
				}
			}

			if (!bDuplicate && mapCount < DS_MAX_MAP)
			{
				strncpy_s(mapNames[mapCount], szMapName, _TRUNCATE);
				mapCount++;
			}
		} while (FindNextFileA(hFind, &findData));

		FindClose(hFind);
	}

	// Load each discovered map
	for (int i = 0; i < mapCount && m_i32LoadedCount < DS_MAX_MAP; i++)
	{
		if (m_Maps[m_i32LoadedCount].Load(mapNames[i]))
		{
			m_i32LoadedCount++;
		}
	}

	m_bInitialized = true;

	printf("[MapManager] Initialized: %d maps loaded from '%s'\n", m_i32LoadedCount, pszMediaPath);
	return true;
}

void MapManager::Shutdown()
{
	for (int i = 0; i < DS_MAX_MAP; i++)
		m_Maps[i].Unload();

	m_i32LoadedCount = 0;
	m_bInitialized = false;
}

CMapData* MapManager::GetMapData(int i32MapIndex)
{
	// Search by index (linear scan - maps may not be contiguous)
	if (i32MapIndex < 0 || i32MapIndex >= DS_MAX_MAP)
		return nullptr;

	// First try direct index
	if (m_Maps[i32MapIndex].IsLoaded())
		return &m_Maps[i32MapIndex];

	// Otherwise search loaded maps
	for (int i = 0; i < DS_MAX_MAP; i++)
	{
		if (m_Maps[i].IsLoaded())
		{
			// Maps are loaded sequentially, so map at index i
			// For now return by sequential order
			if (i32MapIndex == i)
				return &m_Maps[i];
		}
	}

	return nullptr;
}
