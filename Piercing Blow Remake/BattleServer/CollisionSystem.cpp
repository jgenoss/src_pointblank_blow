#include "pch.h"
#include "CollisionSystem.h"
#include <cmath>
#include <cfloat>

CollisionSystem::CollisionSystem()
	: m_pTriangles(nullptr)
	, m_i32TriangleCount(0)
	, m_bInitialized(false)
	, m_fGridOriginX(0.0f)
	, m_fGridOriginZ(0.0f)
	, m_fGridTotalSize(0.0f)
{
	memset(m_Grid, 0, sizeof(m_Grid));
}

CollisionSystem::~CollisionSystem()
{
	Shutdown();
}

bool CollisionSystem::Initialize(CollisionTriangle* pTriangles, int i32TriangleCount)
{
	if (!pTriangles || i32TriangleCount <= 0)
		return false;

	Shutdown();

	m_pTriangles = pTriangles;
	m_i32TriangleCount = i32TriangleCount;

	BuildGrid();

	m_bInitialized = true;
	printf("[CollisionSystem] Initialized with %d triangles\n", m_i32TriangleCount);
	return true;
}

void CollisionSystem::Shutdown()
{
	// Free grid cell indices
	for (int x = 0; x < COLLISION_GRID_SIZE; x++)
	{
		for (int z = 0; z < COLLISION_GRID_SIZE; z++)
		{
			if (m_Grid[x][z].pTriangleIndices)
			{
				delete[] m_Grid[x][z].pTriangleIndices;
				m_Grid[x][z].pTriangleIndices = nullptr;
			}
			m_Grid[x][z].i32Count = 0;
			m_Grid[x][z].i32Capacity = 0;
		}
	}

	m_pTriangles = nullptr;
	m_i32TriangleCount = 0;
	m_bInitialized = false;
}

// ============================================================================
// Moller-Trumbore ray-triangle intersection
// ============================================================================
bool CollisionSystem::RayTriangleIntersect(const float* origin, const float* dir,
										   const float* v0, const float* v1, const float* v2,
										   float* outT)
{
	const float EPSILON = 1e-6f;

	// Edge vectors
	float e1[3] = { v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2] };
	float e2[3] = { v2[0] - v0[0], v2[1] - v0[1], v2[2] - v0[2] };

	// Cross product: h = dir x e2
	float h[3] = {
		dir[1] * e2[2] - dir[2] * e2[1],
		dir[2] * e2[0] - dir[0] * e2[2],
		dir[0] * e2[1] - dir[1] * e2[0]
	};

	// Dot product: a = e1 . h
	float a = e1[0] * h[0] + e1[1] * h[1] + e1[2] * h[2];

	if (a > -EPSILON && a < EPSILON)
		return false;	// Ray parallel to triangle

	float f = 1.0f / a;

	// s = origin - v0
	float s[3] = { origin[0] - v0[0], origin[1] - v0[1], origin[2] - v0[2] };

	// u = f * (s . h)
	float u = f * (s[0] * h[0] + s[1] * h[1] + s[2] * h[2]);
	if (u < 0.0f || u > 1.0f)
		return false;

	// q = s x e1
	float q[3] = {
		s[1] * e1[2] - s[2] * e1[1],
		s[2] * e1[0] - s[0] * e1[2],
		s[0] * e1[1] - s[1] * e1[0]
	};

	// v = f * (dir . q)
	float v = f * (dir[0] * q[0] + dir[1] * q[1] + dir[2] * q[2]);
	if (v < 0.0f || u + v > 1.0f)
		return false;

	// t = f * (e2 . q)
	float t = f * (e2[0] * q[0] + e2[1] * q[1] + e2[2] * q[2]);

	if (t > EPSILON)
	{
		*outT = t;
		return true;
	}

	return false;
}

// ============================================================================
// Raycast - find closest hit
// ============================================================================
bool CollisionSystem::RaycastClosest(const float* origin, const float* dir, float maxDist,
									 float* hitPoint, float* hitDist) const
{
	if (!m_bInitialized)
		return false;

	float closestT = maxDist;
	bool bHit = false;

	// Brute force through all triangles (grid acceleration for broad-phase)
	// For moderate triangle counts (<50k), brute force is fast enough
	for (int i = 0; i < m_i32TriangleCount; i++)
	{
		float t;
		if (RayTriangleIntersect(origin, dir,
			m_pTriangles[i].v0, m_pTriangles[i].v1, m_pTriangles[i].v2, &t))
		{
			if (t < closestT && t > 0.0f)
			{
				closestT = t;
				bHit = true;
			}
		}
	}

	if (bHit)
	{
		if (hitDist)
			*hitDist = closestT;
		if (hitPoint)
		{
			hitPoint[0] = origin[0] + dir[0] * closestT;
			hitPoint[1] = origin[1] + dir[1] * closestT;
			hitPoint[2] = origin[2] + dir[2] * closestT;
		}
	}

	return bHit;
}

// ============================================================================
// Line of sight check
// ============================================================================
bool CollisionSystem::CheckLineOfSight(const float* from, const float* to) const
{
	if (!m_bInitialized)
		return true;	// No collision data = assume clear

	// Direction vector
	float dir[3] = { to[0] - from[0], to[1] - from[1], to[2] - from[2] };
	float dist = sqrtf(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);

	if (dist < 1e-6f)
		return true;	// Same point

	// Normalize
	float invDist = 1.0f / dist;
	dir[0] *= invDist;
	dir[1] *= invDist;
	dir[2] *= invDist;

	// Check if any triangle blocks the path
	for (int i = 0; i < m_i32TriangleCount; i++)
	{
		float t;
		if (RayTriangleIntersect(from, dir,
			m_pTriangles[i].v0, m_pTriangles[i].v1, m_pTriangles[i].v2, &t))
		{
			if (t > 0.001f && t < dist)
				return false;	// Blocked
		}
	}

	return true;	// Clear line of sight
}

// ============================================================================
// Position validation (approximate - check if near floor triangles)
// ============================================================================
bool CollisionSystem::IsPositionValid(const float* pos) const
{
	if (!m_bInitialized)
		return true;

	// Cast a ray downward from position to check if there's ground below
	float down[3] = { 0.0f, -1.0f, 0.0f };
	float t;
	float hitPoint[3];

	if (RaycastClosest(pos, down, 1000.0f, hitPoint, &t))
	{
		// Position is valid if there's ground within reasonable distance below
		return (t < 500.0f);
	}

	return false;	// No ground found - position might be outside map
}

// ============================================================================
// Spatial Grid
// ============================================================================
void CollisionSystem::BuildGrid()
{
	if (m_i32TriangleCount <= 0)
		return;

	// Find bounding box of all triangles
	float minX = FLT_MAX, maxX = -FLT_MAX;
	float minZ = FLT_MAX, maxZ = -FLT_MAX;

	for (int i = 0; i < m_i32TriangleCount; i++)
	{
		for (int v = 0; v < 3; v++)
		{
			const float* vert = (v == 0) ? m_pTriangles[i].v0 :
								(v == 1) ? m_pTriangles[i].v1 : m_pTriangles[i].v2;
			if (vert[0] < minX) minX = vert[0];
			if (vert[0] > maxX) maxX = vert[0];
			if (vert[2] < minZ) minZ = vert[2];
			if (vert[2] > maxZ) maxZ = vert[2];
		}
	}

	m_fGridOriginX = minX;
	m_fGridOriginZ = minZ;
	float rangeX = maxX - minX;
	float rangeZ = maxZ - minZ;
	m_fGridTotalSize = (rangeX > rangeZ) ? rangeX : rangeZ;
	if (m_fGridTotalSize < 1.0f)
		m_fGridTotalSize = 1.0f;

	// First pass: count triangles per cell
	int counts[COLLISION_GRID_SIZE][COLLISION_GRID_SIZE];
	memset(counts, 0, sizeof(counts));

	for (int i = 0; i < m_i32TriangleCount; i++)
	{
		// Get AABB of triangle in XZ plane
		float triMinX = m_pTriangles[i].v0[0];
		float triMaxX = triMinX;
		float triMinZ = m_pTriangles[i].v0[2];
		float triMaxZ = triMinZ;

		for (int v = 1; v < 3; v++)
		{
			const float* vert = (v == 1) ? m_pTriangles[i].v1 : m_pTriangles[i].v2;
			if (vert[0] < triMinX) triMinX = vert[0];
			if (vert[0] > triMaxX) triMaxX = vert[0];
			if (vert[2] < triMinZ) triMinZ = vert[2];
			if (vert[2] > triMaxZ) triMaxZ = vert[2];
		}

		int cellMinX, cellMinZ, cellMaxX, cellMaxZ;
		GetGridCell(triMinX, triMinZ, cellMinX, cellMinZ);
		GetGridCell(triMaxX, triMaxZ, cellMaxX, cellMaxZ);

		for (int cx = cellMinX; cx <= cellMaxX; cx++)
			for (int cz = cellMinZ; cz <= cellMaxZ; cz++)
				counts[cx][cz]++;
	}

	// Allocate cell arrays
	for (int x = 0; x < COLLISION_GRID_SIZE; x++)
	{
		for (int z = 0; z < COLLISION_GRID_SIZE; z++)
		{
			if (counts[x][z] > 0)
			{
				m_Grid[x][z].pTriangleIndices = new int[counts[x][z]];
				m_Grid[x][z].i32Capacity = counts[x][z];
			}
		}
	}

	// Second pass: fill cells
	for (int i = 0; i < m_i32TriangleCount; i++)
	{
		float triMinX = m_pTriangles[i].v0[0];
		float triMaxX = triMinX;
		float triMinZ = m_pTriangles[i].v0[2];
		float triMaxZ = triMinZ;

		for (int v = 1; v < 3; v++)
		{
			const float* vert = (v == 1) ? m_pTriangles[i].v1 : m_pTriangles[i].v2;
			if (vert[0] < triMinX) triMinX = vert[0];
			if (vert[0] > triMaxX) triMaxX = vert[0];
			if (vert[2] < triMinZ) triMinZ = vert[2];
			if (vert[2] > triMaxZ) triMaxZ = vert[2];
		}

		int cellMinX, cellMinZ, cellMaxX, cellMaxZ;
		GetGridCell(triMinX, triMinZ, cellMinX, cellMinZ);
		GetGridCell(triMaxX, triMaxZ, cellMaxX, cellMaxZ);

		for (int cx = cellMinX; cx <= cellMaxX; cx++)
		{
			for (int cz = cellMinZ; cz <= cellMaxZ; cz++)
			{
				CollisionGridCell& cell = m_Grid[cx][cz];
				if (cell.i32Count < cell.i32Capacity)
				{
					cell.pTriangleIndices[cell.i32Count] = i;
					cell.i32Count++;
				}
			}
		}
	}
}

void CollisionSystem::GetGridCell(float x, float z, int& cellX, int& cellZ) const
{
	float cellSize = m_fGridTotalSize / COLLISION_GRID_SIZE;
	if (cellSize < 0.001f)
		cellSize = 0.001f;

	cellX = (int)((x - m_fGridOriginX) / cellSize);
	cellZ = (int)((z - m_fGridOriginZ) / cellSize);

	if (cellX < 0) cellX = 0;
	if (cellX >= COLLISION_GRID_SIZE) cellX = COLLISION_GRID_SIZE - 1;
	if (cellZ < 0) cellZ = 0;
	if (cellZ >= COLLISION_GRID_SIZE) cellZ = COLLISION_GRID_SIZE - 1;
}
