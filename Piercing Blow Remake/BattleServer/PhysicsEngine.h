#ifndef __PHYSICSENGINE_H__
#define __PHYSICSENGINE_H__

#pragma once
#include "BattleDef.h"
#include "MapData.h"

// ============================================================================
// PhysicsEngine - Unified physics abstraction for BattleServer
//
// When I3_PHYSX is defined and the PhysX SDK is available, uses NVIDIA PhysX
// for raycast, scene management, and rigid body simulation.
// Otherwise, falls back to the existing CPU-based CollisionSystem
// (Moller-Trumbore ray-triangle intersection with spatial grid).
//
// This replaces direct use of CollisionSystem in BattleRoom, providing:
// 1. Raycast / Line-of-sight through PhysX scene or CPU fallback
// 2. Grenade trajectory simulation with gravity
// 3. Character capsule bounds for more accurate hit detection
// 4. Material-aware collision (terrain types for footstep/reverb)
// ============================================================================

#if defined(I3_PHYSX)
// Forward declare PhysX types when SDK is available
class NxPhysicsSDK;
class NxScene;
class NxActor;
class NxShape;
class NxTriangleMesh;
class NxTriangleMeshDesc;
#endif

class CollisionSystem;

// ============================================================================
// Physics raycast result
// ============================================================================
struct PhysicsRaycastResult
{
	bool	bHit;
	float	fHitPoint[3];
	float	fHitNormal[3];
	float	fHitDistance;
	int		i32FaceIndex;		// Triangle face index (-1 if N/A)
	int		i32MaterialIndex;	// Material/terrain index (-1 if N/A)

	PhysicsRaycastResult()
		: bHit(false), fHitDistance(0.0f), i32FaceIndex(-1), i32MaterialIndex(-1)
	{
		fHitPoint[0] = fHitPoint[1] = fHitPoint[2] = 0.0f;
		fHitNormal[0] = 0.0f; fHitNormal[1] = 1.0f; fHitNormal[2] = 0.0f;
	}
};

// ============================================================================
// Grenade simulation state
// ============================================================================
struct GrenadeSimState
{
	float	fPos[3];			// Current position
	float	fVelocity[3];		// Current velocity
	float	fElapsedTime;		// Time since throw
	bool	bLanded;			// Has hit ground/wall and stopped
	bool	bExploded;			// Has detonated
	int		i32BounceCount;		// Number of wall/floor bounces

	void Reset()
	{
		fPos[0] = fPos[1] = fPos[2] = 0.0f;
		fVelocity[0] = fVelocity[1] = fVelocity[2] = 0.0f;
		fElapsedTime = 0.0f;
		bLanded = false;
		bExploded = false;
		i32BounceCount = 0;
	}
};

// ============================================================================
// Character capsule for physics collision
// ============================================================================
struct PhysicsCapsule
{
	float	fPos[3];			// Base position (feet)
	float	fRadius;			// Capsule radius (typically 25cm)
	float	fHeight;			// Capsule height (typically 170cm)
	bool	bActive;

#if defined(I3_PHYSX)
	NxActor*	pActor;			// PhysX actor (when available)
#endif

	PhysicsCapsule()
		: fRadius(25.0f), fHeight(170.0f), bActive(false)
	{
		fPos[0] = fPos[1] = fPos[2] = 0.0f;
#if defined(I3_PHYSX)
		pActor = nullptr;
#endif
	}
};

// ============================================================================
// Physics configuration constants
// ============================================================================
#define PHYSICS_GRAVITY				-980.0f		// cm/s^2 (9.8 m/s^2 in cm)
#define PHYSICS_GRENADE_FUSE_TIME	3.0f		// Grenade detonation time (seconds)
#define PHYSICS_GRENADE_BOUNCE_DAMP	0.4f		// Velocity retention per bounce (40%)
#define PHYSICS_GRENADE_MAX_BOUNCES	5			// Max bounces before landing
#define PHYSICS_GRENADE_REST_VEL	10.0f		// Velocity below which grenade stops
#define PHYSICS_SIM_TIMESTEP		(1.0f / 60.0f)	// 60 Hz simulation
#define PHYSICS_CHARACTER_RADIUS	25.0f		// Character capsule radius (cm)
#define PHYSICS_CHARACTER_HEIGHT	170.0f		// Character capsule height (cm)
#define PHYSICS_CHARACTER_MASS_CENTER 85.0f		// Center of mass height (cm)
#define PHYSICS_HEAD_HEIGHT			160.0f		// Head position height (cm)

// ============================================================================
// PhysicsEngine class
// ============================================================================
class PhysicsEngine
{
public:
	PhysicsEngine();
	~PhysicsEngine();

	// ===== Initialization =====

	// Initialize from map collision data
	// If I3_PHYSX: creates NxScene with triangle mesh actors
	// Otherwise: initializes CollisionSystem with Moller-Trumbore
	bool	Initialize(CMapData* pMapData);
	void	Shutdown();
	bool	IsInitialized() const	{ return m_bInitialized; }

	// ===== Raycasting =====

	// Cast a ray and find the closest intersection
	bool	RaycastClosest(const float* origin, const float* direction,
						   float fMaxDistance, PhysicsRaycastResult* pResult) const;

	// Check line-of-sight between two points (returns true if CLEAR)
	bool	CheckLineOfSight(const float* from, const float* to) const;

	// Check if a position is valid (has ground below within reasonable distance)
	bool	IsPositionValid(const float* pos) const;

	// ===== Grenade Physics Simulation =====

	// Simulate a grenade trajectory step
	// Returns true if grenade is still active (not exploded)
	bool	SimulateGrenade(GrenadeSimState* pState, float fDeltaTime) const;

	// Simulate full grenade trajectory to find final landing position
	// Used for server-side validation of reported explosion positions
	bool	PredictGrenadeImpact(const float* throwPos, const float* throwVelocity,
								 float fFuseTime, float* outImpactPos) const;

	// ===== Character Capsule Management =====

	// Create/destroy character capsule for a slot
	bool	CreateCharacterCapsule(int i32Slot, const float* pos);
	void	DestroyCharacterCapsule(int i32Slot);
	void	UpdateCharacterPosition(int i32Slot, const float* pos);

	// Check if a bullet ray intersects a character capsule
	// Returns distance to hit, or -1.0f if no hit
	float	RaycastCharacterCapsule(int i32Slot, const float* rayOrigin,
								   const float* rayDir, float fMaxDist) const;

	// ===== Scene Simulation =====

	// Step the physics simulation (for PhysX; no-op for CPU fallback)
	void	StepSimulation(float fDeltaTime);

	// ===== Accessors =====

	// Get the underlying CollisionSystem (for backward compatibility)
	CollisionSystem*	GetCollisionSystem()	{ return m_pCollisionFallback; }

#if defined(I3_PHYSX)
	NxScene*			GetScene()				{ return m_pScene; }
#endif

private:
	// Internal helpers
	bool	InitializePhysX(CMapData* pMapData);
	bool	InitializeCPU(CMapData* pMapData);

	// Capsule-ray intersection test (CPU)
	static float	RayCapsuleIntersect(const float* rayOrigin, const float* rayDir,
										const float* capsuleBase, float capsuleRadius,
										float capsuleHeight);

private:
	bool				m_bInitialized;
	bool				m_bUsingPhysX;		// true = PhysX active, false = CPU fallback

	// CPU fallback
	CollisionSystem*	m_pCollisionFallback;

	// Character capsules (per slot)
	PhysicsCapsule		m_Capsules[BATTLE_SLOT_MAX];

#if defined(I3_PHYSX)
	// PhysX objects
	NxPhysicsSDK*		m_pPhysicsSDK;
	NxScene*			m_pScene;
	NxActor*			m_pStaticMeshActor;	// Static collision mesh
	NxTriangleMesh*		m_pTriangleMesh;	// Cooked mesh
#endif
};

#endif // __PHYSICSENGINE_H__
