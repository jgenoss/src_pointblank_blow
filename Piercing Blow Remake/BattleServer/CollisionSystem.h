#ifndef __COLLISIONSYSTEM_H__
#define __COLLISIONSYSTEM_H__

#pragma once
#include "MapData.h"

// Spatial grid for broad-phase acceleration
#define COLLISION_GRID_SIZE		32		// Grid cells per axis
#define COLLISION_GRID_CELL_SIZE	100.0f	// World units per cell

struct CollisionGridCell
{
	int*	pTriangleIndices;
	int		i32Count;
	int		i32Capacity;
};

// CollisionSystem - CPU-based ray-triangle intersection
// Replaces PhysX NxScene raycast from the original DediServer
// Uses Moller-Trumbore algorithm with spatial grid acceleration
class CollisionSystem
{
public:
	CollisionSystem();
	~CollisionSystem();

	// Initialize from map collision data
	bool	Initialize(CollisionTriangle* pTriangles, int i32TriangleCount);
	void	Shutdown();

	// Ray casting
	// Returns true if ray hits any triangle within maxDist
	// If hit, hitPoint and hitDist are filled
	bool	RaycastClosest(const float* origin, const float* dir, float maxDist,
						   float* hitPoint, float* hitDist) const;

	// Line-of-sight check: returns true if path from->to is CLEAR (no collision)
	bool	CheckLineOfSight(const float* from, const float* to) const;

	// Point-in-solid test (approximate)
	bool	IsPositionValid(const float* pos) const;

	bool	IsInitialized() const	{ return m_bInitialized; }

private:
	// Moller-Trumbore ray-triangle intersection
	// Returns true if ray intersects triangle, with t = distance along ray
	static bool RayTriangleIntersect(const float* origin, const float* dir,
									 const float* v0, const float* v1, const float* v2,
									 float* outT);

	// Spatial grid helpers
	void	BuildGrid();
	void	GetGridCell(float x, float z, int& cellX, int& cellZ) const;

private:
	CollisionTriangle*	m_pTriangles;		// Reference to map data (not owned)
	int					m_i32TriangleCount;
	bool				m_bInitialized;

	// Spatial grid for acceleration
	CollisionGridCell	m_Grid[COLLISION_GRID_SIZE][COLLISION_GRID_SIZE];
	float				m_fGridOriginX;		// World origin of grid
	float				m_fGridOriginZ;
	float				m_fGridTotalSize;	// Total grid size in world units
};

#endif // __COLLISIONSYSTEM_H__
