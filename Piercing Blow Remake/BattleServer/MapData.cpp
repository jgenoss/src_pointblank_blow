#include "pch.h"
#include "MapData.h"

CMapData::CMapData()
	: m_bLoaded(false)
	, m_i32GameObjectCount(0)
	, m_i32RespawnObjectCount(0)
	, m_pCollisionTriangles(nullptr)
	, m_i32CollisionTriangleCount(0)
{
	m_szCodeName[0] = '\0';
	memset(m_GameObjects, 0, sizeof(m_GameObjects));
	for (int i = 0; i < DS_MAX_RESPAWN_OBJECT; i++)
		m_RespawnObjects[i].Reset();
}

CMapData::~CMapData()
{
	Unload();
}

bool CMapData::Load(const char* pszCodeName)
{
	if (!pszCodeName || pszCodeName[0] == '\0')
		return false;

	Unload();

	strncpy_s(m_szCodeName, pszCodeName, _TRUNCATE);

	bool bHasData = false;

	// Load each file type (any can be missing - not all maps have all types)
	if (LoadGameObjects(pszCodeName))
		bHasData = true;

	if (LoadRespawnObjects(pszCodeName))
		bHasData = true;

	if (LoadCollisionMeshes(pszCodeName))
		bHasData = true;

	m_bLoaded = bHasData;

	if (m_bLoaded)
	{
		printf("[MapData] Loaded '%s': Objects=%d, Respawns=%d, ColTris=%d\n",
			m_szCodeName, m_i32GameObjectCount, m_i32RespawnObjectCount, m_i32CollisionTriangleCount);
	}

	return m_bLoaded;
}

void CMapData::Unload()
{
	m_bLoaded = false;
	m_i32GameObjectCount = 0;
	m_i32RespawnObjectCount = 0;

	if (m_pCollisionTriangles)
	{
		delete[] m_pCollisionTriangles;
		m_pCollisionTriangles = nullptr;
	}
	m_i32CollisionTriangleCount = 0;
}

// ============================================================================
// .i3sobj - Game Objects
// ============================================================================
bool CMapData::LoadGameObjects(const char* pszMapName)
{
	char szPath[MAX_PATH];
	sprintf_s(szPath, "%s/%s.%s", MEDIA_PATH, pszMapName, SOBJ_FILE_EXTENSION);

	HANDLE hFile = CreateFileA(szPath, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	DWORD dwRead = 0;

	// Read header
	DS_I3SOBJ_HEADER header;
	ReadFile(hFile, &header, sizeof(header), &dwRead, NULL);
	if (dwRead != sizeof(header))
	{
		CloseHandle(hFile);
		return false;
	}

	// Validate
	if (header.count == 0 || header.count > DS_MAX_OBJECT)
	{
		CloseHandle(hFile);
		if (header.count == 0)
			return true;	// Empty file is OK
		printf("[MapData] WARNING: %s has too many objects (%d > %d)\n", szPath, header.count, DS_MAX_OBJECT);
		return false;
	}

	// Read game objects
	for (DWORD i = 0; i < header.count; i++)
	{
		ReadFile(hFile, &m_GameObjects[i], sizeof(DS_GAME_OBJECT), &dwRead, NULL);
		if (dwRead != sizeof(DS_GAME_OBJECT))
		{
			printf("[MapData] WARNING: Truncated .i3sobj at object %d in %s\n", i, szPath);
			break;
		}
		m_i32GameObjectCount++;
	}

	CloseHandle(hFile);
	return true;
}

// ============================================================================
// .i3srpn - Respawn Objects
// ============================================================================
bool CMapData::LoadRespawnObjects(const char* pszMapName)
{
	char szPath[MAX_PATH];
	sprintf_s(szPath, "%s/%s.%s", MEDIA_PATH, pszMapName, SRPN_FILE_EXTENSION);

	HANDLE hFile = CreateFileA(szPath, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	DWORD dwRead = 0;

	// Read header (reuses I3SOBJ_HEADER)
	DS_I3SOBJ_HEADER header;
	ReadFile(hFile, &header, sizeof(header), &dwRead, NULL);
	if (dwRead != sizeof(header))
	{
		CloseHandle(hFile);
		return false;
	}

	if (header.count == 0 || header.count > DS_MAX_RESPAWN_OBJECT)
	{
		CloseHandle(hFile);
		if (header.count == 0)
			return true;
		printf("[MapData] WARNING: %s has too many respawns (%d > %d)\n", szPath, header.count, DS_MAX_RESPAWN_OBJECT);
		return false;
	}

	// Read respawn objects
	for (DWORD i = 0; i < header.count; i++)
	{
		ReadFile(hFile, &m_RespawnObjects[i], sizeof(DS_RESPAWN_OBJECT), &dwRead, NULL);
		if (dwRead != sizeof(DS_RESPAWN_OBJECT))
		{
			printf("[MapData] WARNING: Truncated .i3srpn at object %d in %s\n", i, szPath);
			break;
		}
		m_i32RespawnObjectCount++;
	}

	CloseHandle(hFile);
	return true;
}

// ============================================================================
// .i3scol - Collision Meshes (extract triangles for ray testing)
// ============================================================================
bool CMapData::LoadCollisionMeshes(const char* pszMapName)
{
	char szPath[MAX_PATH];
	sprintf_s(szPath, "%s/%s.%s", MEDIA_PATH, pszMapName, SCOL_FILE_EXTENSION);

	HANDLE hFile = CreateFileA(szPath, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	DWORD dwRead = 0;

	// Read header
	DS_I3SCOL_HEADER header;
	ReadFile(hFile, &header, sizeof(header), &dwRead, NULL);
	if (dwRead != sizeof(header))
	{
		CloseHandle(hFile);
		return false;
	}

	if (header.count == 0 || header.count > DS_MAX_ACTORS)
	{
		CloseHandle(hFile);
		if (header.count == 0)
			return true;
		printf("[MapData] WARNING: %s has too many collision objects (%d > %d)\n", szPath, header.count, DS_MAX_ACTORS);
		return false;
	}

	// Read all descriptors first
	DS_I3SCOL_DESC* descs = new DS_I3SCOL_DESC[header.count];
	for (DWORD i = 0; i < header.count; i++)
	{
		ReadFile(hFile, &descs[i], sizeof(DS_I3SCOL_DESC), &dwRead, NULL);
		if (dwRead != sizeof(DS_I3SCOL_DESC))
		{
			printf("[MapData] WARNING: Truncated .i3scol desc at %d in %s\n", i, szPath);
			delete[] descs;
			CloseHandle(hFile);
			return false;
		}
	}

	// Count total triangles across all meshes
	int totalTriangles = 0;
	for (DWORD i = 0; i < header.count; i++)
	{
		if (descs[i].vstep > 0 && descs[i].fstep > 0)
		{
			int vertCount = (descs[i].vsize / descs[i].vstep);
			int faceCount = (descs[i].fsize / descs[i].fstep);
			totalTriangles += faceCount;
			(void)vertCount;	// Used implicitly when reading
		}
	}

	if (totalTriangles <= 0)
	{
		delete[] descs;
		CloseHandle(hFile);
		return true;	// No triangles, but file was valid
	}

	// Allocate triangle storage
	m_pCollisionTriangles = new CollisionTriangle[totalTriangles];
	m_i32CollisionTriangleCount = 0;

	// Read mesh data for each descriptor
	for (DWORD d = 0; d < header.count; d++)
	{
		DS_I3SCOL_DESC& desc = descs[d];

		if (desc.vstep == 0 || desc.fstep == 0 || desc.vsize == 0 || desc.fsize == 0)
			continue;

		int vertCount = desc.vsize / desc.vstep;
		int faceCount = desc.fsize / desc.fstep;

		if (vertCount <= 0 || faceCount <= 0)
			continue;

		// Read vertices
		float* vertices = new float[vertCount * 3];
		SetFilePointer(hFile, desc.voffset, NULL, FILE_BEGIN);

		for (int v = 0; v < vertCount; v++)
		{
			float vx, vy, vz;
			ReadFile(hFile, &vx, sizeof(float), &dwRead, NULL);
			ReadFile(hFile, &vy, sizeof(float), &dwRead, NULL);
			ReadFile(hFile, &vz, sizeof(float), &dwRead, NULL);

			// Apply transform (rotation + translation from desc.transform)
			// transform[0..8] = 3x3 rotation, transform[9..11] = translation
			float ox = desc.transform[0] * vx + desc.transform[1] * vy + desc.transform[2] * vz + desc.transform[9];
			float oy = desc.transform[3] * vx + desc.transform[4] * vy + desc.transform[5] * vz + desc.transform[10];
			float oz = desc.transform[6] * vx + desc.transform[7] * vy + desc.transform[8] * vz + desc.transform[11];

			vertices[v * 3 + 0] = ox;
			vertices[v * 3 + 1] = oy;
			vertices[v * 3 + 2] = oz;

			// Skip remaining bytes per vertex step
			if (desc.vstep > 12)
			{
				DWORD skip = desc.vstep - 12;
				SetFilePointer(hFile, skip, NULL, FILE_CURRENT);
			}
		}

		// Read faces (indices) - assumed to be 3 uint32 (or uint16) per face
		SetFilePointer(hFile, desc.foffset, NULL, FILE_BEGIN);

		for (int f = 0; f < faceCount && m_i32CollisionTriangleCount < totalTriangles; f++)
		{
			uint32_t idx[3] = { 0, 0, 0 };

			if (desc.fstep == 6)
			{
				// 3 x uint16_t per face
				uint16_t idx16[3];
				ReadFile(hFile, idx16, sizeof(idx16), &dwRead, NULL);
				idx[0] = idx16[0]; idx[1] = idx16[1]; idx[2] = idx16[2];
			}
			else if (desc.fstep >= 12)
			{
				// 3 x uint32_t per face
				ReadFile(hFile, idx, sizeof(uint32_t) * 3, &dwRead, NULL);
				// Skip remaining face step bytes
				if (desc.fstep > 12)
					SetFilePointer(hFile, desc.fstep - 12, NULL, FILE_CURRENT);
			}
			else
			{
				// Unknown face format - skip
				SetFilePointer(hFile, desc.fstep, NULL, FILE_CURRENT);
				continue;
			}

			// Validate indices
			if (idx[0] < (uint32_t)vertCount && idx[1] < (uint32_t)vertCount && idx[2] < (uint32_t)vertCount)
			{
				CollisionTriangle& tri = m_pCollisionTriangles[m_i32CollisionTriangleCount];
				tri.v0[0] = vertices[idx[0] * 3 + 0];
				tri.v0[1] = vertices[idx[0] * 3 + 1];
				tri.v0[2] = vertices[idx[0] * 3 + 2];
				tri.v1[0] = vertices[idx[1] * 3 + 0];
				tri.v1[1] = vertices[idx[1] * 3 + 1];
				tri.v1[2] = vertices[idx[1] * 3 + 2];
				tri.v2[0] = vertices[idx[2] * 3 + 0];
				tri.v2[1] = vertices[idx[2] * 3 + 1];
				tri.v2[2] = vertices[idx[2] * 3 + 2];
				m_i32CollisionTriangleCount++;
			}
		}

		delete[] vertices;
	}

	delete[] descs;
	CloseHandle(hFile);
	return true;
}

// ============================================================================
// Respawn helpers
// ============================================================================

int CMapData::GetRespawnCountForTeam(int team) const
{
	int count = 0;
	for (int i = 0; i < m_i32RespawnObjectCount; i++)
	{
		if (m_RespawnObjects[i].m_ui8RespawnStyle == (uint8_t)team)
			count++;
	}
	return count;
}

const DS_RESPAWN_OBJECT* CMapData::GetRandomSpawnForTeam(int team) const
{
	// Collect matching spawns
	const DS_RESPAWN_OBJECT* candidates[DS_MAX_RESPAWN_OBJECT];
	int count = 0;

	for (int i = 0; i < m_i32RespawnObjectCount; i++)
	{
		if (m_RespawnObjects[i].m_ui8RespawnStyle == (uint8_t)team)
		{
			candidates[count++] = &m_RespawnObjects[i];
		}
	}

	if (count == 0)
		return nullptr;

	// Random selection
	return candidates[GetTickCount() % count];
}
