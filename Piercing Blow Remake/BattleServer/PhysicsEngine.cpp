#include "pch.h"
#include "PhysicsEngine.h"
#include "CollisionSystem.h"
#include <cmath>
#include <cstring>

#if defined(I3_PHYSX)
#include "i3Scene/i3PhysixDefine.h"
#endif

// ============================================================================
// Constructor / Destructor
// ============================================================================

PhysicsEngine::PhysicsEngine()
	: m_bInitialized(false)
	, m_bUsingPhysX(false)
	, m_pCollisionFallback(nullptr)
#if defined(I3_PHYSX)
	, m_pPhysicsSDK(nullptr)
	, m_pScene(nullptr)
	, m_pStaticMeshActor(nullptr)
	, m_pTriangleMesh(nullptr)
#endif
{
}

PhysicsEngine::~PhysicsEngine()
{
	Shutdown();
}

// ============================================================================
// Initialization
// ============================================================================

bool PhysicsEngine::Initialize(CMapData* pMapData)
{
	if (!pMapData || !pMapData->IsLoaded())
		return false;

	Shutdown();

#if defined(I3_PHYSX)
	// Try PhysX initialization first
	if (InitializePhysX(pMapData))
	{
		m_bUsingPhysX = true;
		m_bInitialized = true;
		printf("[PhysicsEngine] Initialized with NVIDIA PhysX backend\n");
		return true;
	}

	// PhysX failed, fall through to CPU
	printf("[PhysicsEngine] PhysX init failed, falling back to CPU collision\n");
#endif

	// CPU fallback (Moller-Trumbore)
	if (InitializeCPU(pMapData))
	{
		m_bUsingPhysX = false;
		m_bInitialized = true;
		printf("[PhysicsEngine] Initialized with CPU collision backend (%d triangles)\n",
			pMapData->GetCollisionTriangleCount());
		return true;
	}

	return false;
}

bool PhysicsEngine::InitializeCPU(CMapData* pMapData)
{
	if (pMapData->GetCollisionTriangleCount() <= 0)
	{
		// No collision data - still valid, just no collision checks
		m_bInitialized = true;
		return true;
	}

	m_pCollisionFallback = new CollisionSystem();
	if (!m_pCollisionFallback->Initialize(pMapData->GetCollisionTriangles(),
										   pMapData->GetCollisionTriangleCount()))
	{
		delete m_pCollisionFallback;
		m_pCollisionFallback = nullptr;
		return false;
	}

	return true;
}

#if defined(I3_PHYSX)
bool PhysicsEngine::InitializePhysX(CMapData* pMapData)
{
	// Create PhysX SDK
	m_pPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
	if (!m_pPhysicsSDK)
	{
		printf("[PhysicsEngine] Failed to create PhysX SDK\n");
		return false;
	}

	// Set SDK parameters
	m_pPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.01f);
	m_pPhysicsSDK->setParameter(NX_DEFAULT_SLEEP_LIN_VEL_SQUARED, 0.15f * 0.15f);
	m_pPhysicsSDK->setParameter(NX_DEFAULT_SLEEP_ANG_VEL_SQUARED, 0.14f * 0.14f);

	// Create scene
	NxSceneDesc sceneDesc;
	sceneDesc.gravity = NxVec3(0.0f, PHYSICS_GRAVITY, 0.0f);
	sceneDesc.simType = NX_SIMULATION_SW;	// Software simulation (no GPU requirement)

	m_pScene = m_pPhysicsSDK->createScene(sceneDesc);
	if (!m_pScene)
	{
		printf("[PhysicsEngine] Failed to create PhysX scene\n");
		m_pPhysicsSDK->release();
		m_pPhysicsSDK = nullptr;
		return false;
	}

	// Set default material
	NxMaterial* pDefaultMat = m_pScene->getMaterialFromIndex(0);
	if (pDefaultMat)
	{
		pDefaultMat->setRestitution(0.3f);
		pDefaultMat->setStaticFriction(0.5f);
		pDefaultMat->setDynamicFriction(0.5f);
	}

	// Cook and create the collision triangle mesh from map data
	CollisionTriangle* pTris = pMapData->GetCollisionTriangles();
	int triCount = pMapData->GetCollisionTriangleCount();

	if (triCount > 0 && pTris)
	{
		// Build vertex and index arrays from collision triangles
		int vertexCount = triCount * 3;
		NxVec3* vertices = new NxVec3[vertexCount];
		NxU32* indices = new NxU32[triCount * 3];

		for (int i = 0; i < triCount; i++)
		{
			vertices[i * 3 + 0] = NxVec3(pTris[i].v0[0], pTris[i].v0[1], pTris[i].v0[2]);
			vertices[i * 3 + 1] = NxVec3(pTris[i].v1[0], pTris[i].v1[1], pTris[i].v1[2]);
			vertices[i * 3 + 2] = NxVec3(pTris[i].v2[0], pTris[i].v2[1], pTris[i].v2[2]);
			indices[i * 3 + 0] = i * 3 + 0;
			indices[i * 3 + 1] = i * 3 + 1;
			indices[i * 3 + 2] = i * 3 + 2;
		}

		// Create triangle mesh descriptor
		NxTriangleMeshDesc meshDesc;
		meshDesc.numVertices = vertexCount;
		meshDesc.numTriangles = triCount;
		meshDesc.pointStrideBytes = sizeof(NxVec3);
		meshDesc.triangleStrideBytes = 3 * sizeof(NxU32);
		meshDesc.points = vertices;
		meshDesc.triangles = indices;
		meshDesc.flags = 0;

		// Cook the mesh
		NxInitCooking();
		MemoryWriteBuffer buf;
		bool bCooked = NxCookTriangleMesh(meshDesc, buf);
		if (bCooked)
		{
			MemoryReadBuffer readBuf(buf.data);
			m_pTriangleMesh = m_pPhysicsSDK->createTriangleMesh(readBuf);
		}
		NxCloseCooking();

		delete[] vertices;
		delete[] indices;

		if (!m_pTriangleMesh)
		{
			printf("[PhysicsEngine] Failed to cook triangle mesh\n");
			m_pScene->release();
			m_pPhysicsSDK->release();
			m_pScene = nullptr;
			m_pPhysicsSDK = nullptr;
			return false;
		}

		// Create static actor with the triangle mesh shape
		NxActorDesc actorDesc;
		NxTriangleMeshShapeDesc meshShapeDesc;
		meshShapeDesc.meshData = m_pTriangleMesh;
		meshShapeDesc.group = 0;	// Static collision group
		actorDesc.shapes.pushBack(&meshShapeDesc);
		actorDesc.body = NULL;		// Static (no rigid body)

		m_pStaticMeshActor = m_pScene->createActor(actorDesc);
		if (!m_pStaticMeshActor)
		{
			printf("[PhysicsEngine] Failed to create static mesh actor\n");
			m_pPhysicsSDK->releaseTriangleMesh(*m_pTriangleMesh);
			m_pScene->release();
			m_pPhysicsSDK->release();
			m_pTriangleMesh = nullptr;
			m_pScene = nullptr;
			m_pPhysicsSDK = nullptr;
			return false;
		}

		printf("[PhysicsEngine] PhysX mesh created: %d triangles, %d vertices\n",
			triCount, vertexCount);
	}

	// Also initialize CPU fallback for cases where PhysX isn't needed
	InitializeCPU(pMapData);

	return true;
}
#else
bool PhysicsEngine::InitializePhysX(CMapData* /*pMapData*/)
{
	return false;	// PhysX not available
}
#endif

void PhysicsEngine::Shutdown()
{
#if defined(I3_PHYSX)
	// Destroy character capsule actors
	for (int i = 0; i < BATTLE_SLOT_MAX; i++)
	{
		if (m_Capsules[i].pActor && m_pScene)
		{
			m_pScene->releaseActor(*m_Capsules[i].pActor);
			m_Capsules[i].pActor = nullptr;
		}
		m_Capsules[i].bActive = false;
	}

	if (m_pStaticMeshActor && m_pScene)
	{
		m_pScene->releaseActor(*m_pStaticMeshActor);
		m_pStaticMeshActor = nullptr;
	}

	if (m_pTriangleMesh && m_pPhysicsSDK)
	{
		m_pPhysicsSDK->releaseTriangleMesh(*m_pTriangleMesh);
		m_pTriangleMesh = nullptr;
	}

	if (m_pScene)
	{
		m_pPhysicsSDK->releaseScene(*m_pScene);
		m_pScene = nullptr;
	}

	if (m_pPhysicsSDK)
	{
		m_pPhysicsSDK->release();
		m_pPhysicsSDK = nullptr;
	}
#else
	for (int i = 0; i < BATTLE_SLOT_MAX; i++)
		m_Capsules[i].bActive = false;
#endif

	if (m_pCollisionFallback)
	{
		m_pCollisionFallback->Shutdown();
		delete m_pCollisionFallback;
		m_pCollisionFallback = nullptr;
	}

	m_bInitialized = false;
	m_bUsingPhysX = false;
}

// ============================================================================
// Raycasting
// ============================================================================

bool PhysicsEngine::RaycastClosest(const float* origin, const float* direction,
								   float fMaxDistance, PhysicsRaycastResult* pResult) const
{
	if (!m_bInitialized || !pResult)
		return false;

#if defined(I3_PHYSX)
	if (m_bUsingPhysX && m_pScene)
	{
		NxRay ray;
		ray.orig = NxVec3(origin[0], origin[1], origin[2]);
		ray.dir = NxVec3(direction[0], direction[1], direction[2]);

		NxRaycastHit hit;
		NxShape* pShape = m_pScene->raycastClosestShape(ray, NX_ALL_SHAPES, hit,
			0xFFFFFFFF, fMaxDistance);

		if (pShape)
		{
			pResult->bHit = true;
			pResult->fHitPoint[0] = hit.worldImpact.x;
			pResult->fHitPoint[1] = hit.worldImpact.y;
			pResult->fHitPoint[2] = hit.worldImpact.z;
			pResult->fHitNormal[0] = hit.worldNormal.x;
			pResult->fHitNormal[1] = hit.worldNormal.y;
			pResult->fHitNormal[2] = hit.worldNormal.z;
			pResult->fHitDistance = hit.distance;
			pResult->i32FaceIndex = hit.faceID;
			pResult->i32MaterialIndex = hit.materialIndex;
			return true;
		}

		pResult->bHit = false;
		return false;
	}
#endif

	// CPU fallback
	if (m_pCollisionFallback && m_pCollisionFallback->IsInitialized())
	{
		float hitPoint[3];
		float hitDist;

		if (m_pCollisionFallback->RaycastClosest(origin, direction, fMaxDistance, hitPoint, &hitDist))
		{
			pResult->bHit = true;
			pResult->fHitPoint[0] = hitPoint[0];
			pResult->fHitPoint[1] = hitPoint[1];
			pResult->fHitPoint[2] = hitPoint[2];
			pResult->fHitDistance = hitDist;
			// CPU fallback doesn't provide normal or material
			pResult->fHitNormal[0] = 0.0f;
			pResult->fHitNormal[1] = 1.0f;
			pResult->fHitNormal[2] = 0.0f;
			pResult->i32FaceIndex = -1;
			pResult->i32MaterialIndex = -1;
			return true;
		}
	}

	pResult->bHit = false;
	return false;
}

bool PhysicsEngine::CheckLineOfSight(const float* from, const float* to) const
{
	if (!m_bInitialized)
		return true;	// No collision = assume clear

#if defined(I3_PHYSX)
	if (m_bUsingPhysX && m_pScene)
	{
		NxVec3 dir(to[0] - from[0], to[1] - from[1], to[2] - from[2]);
		float dist = dir.magnitude();
		if (dist < 1e-6f)
			return true;

		dir.normalize();

		NxRay ray;
		ray.orig = NxVec3(from[0], from[1], from[2]);
		ray.dir = dir;

		NxRaycastHit hit;
		NxShape* pShape = m_pScene->raycastClosestShape(ray, NX_STATIC_SHAPES, hit,
			0xFFFFFFFF, dist);

		if (pShape && hit.distance < dist - 0.1f)
			return false;	// Blocked

		return true;
	}
#endif

	// CPU fallback
	if (m_pCollisionFallback && m_pCollisionFallback->IsInitialized())
		return m_pCollisionFallback->CheckLineOfSight(from, to);

	return true;
}

bool PhysicsEngine::IsPositionValid(const float* pos) const
{
	if (!m_bInitialized)
		return true;

#if defined(I3_PHYSX)
	if (m_bUsingPhysX && m_pScene)
	{
		// Cast ray downward
		NxRay ray;
		ray.orig = NxVec3(pos[0], pos[1], pos[2]);
		ray.dir = NxVec3(0.0f, -1.0f, 0.0f);

		NxRaycastHit hit;
		NxShape* pShape = m_pScene->raycastClosestShape(ray, NX_STATIC_SHAPES, hit,
			0xFFFFFFFF, 500.0f);

		return (pShape != nullptr);
	}
#endif

	if (m_pCollisionFallback && m_pCollisionFallback->IsInitialized())
		return m_pCollisionFallback->IsPositionValid(pos);

	return true;
}

// ============================================================================
// Grenade Physics Simulation
// ============================================================================

bool PhysicsEngine::SimulateGrenade(GrenadeSimState* pState, float fDeltaTime) const
{
	if (!pState || pState->bExploded || pState->bLanded)
		return !pState->bExploded;

	// Check fuse timer
	pState->fElapsedTime += fDeltaTime;
	if (pState->fElapsedTime >= PHYSICS_GRENADE_FUSE_TIME)
	{
		pState->bExploded = true;
		return false;
	}

	// Apply gravity to velocity
	pState->fVelocity[1] += PHYSICS_GRAVITY * fDeltaTime;

	// Calculate new position
	float newPos[3];
	newPos[0] = pState->fPos[0] + pState->fVelocity[0] * fDeltaTime;
	newPos[1] = pState->fPos[1] + pState->fVelocity[1] * fDeltaTime;
	newPos[2] = pState->fPos[2] + pState->fVelocity[2] * fDeltaTime;

	// Check for collision along movement path
	float dir[3] = {
		newPos[0] - pState->fPos[0],
		newPos[1] - pState->fPos[1],
		newPos[2] - pState->fPos[2]
	};
	float moveDist = sqrtf(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);

	if (moveDist > 0.01f)
	{
		float invDist = 1.0f / moveDist;
		float normDir[3] = { dir[0] * invDist, dir[1] * invDist, dir[2] * invDist };

		PhysicsRaycastResult result;
		if (RaycastClosest(pState->fPos, normDir, moveDist + 5.0f, &result))
		{
			if (result.fHitDistance < moveDist)
			{
				// Hit a surface - bounce
				pState->i32BounceCount++;

				// Place grenade at hit point (slightly offset along normal)
				pState->fPos[0] = result.fHitPoint[0] + result.fHitNormal[0] * 2.0f;
				pState->fPos[1] = result.fHitPoint[1] + result.fHitNormal[1] * 2.0f;
				pState->fPos[2] = result.fHitPoint[2] + result.fHitNormal[2] * 2.0f;

				// Reflect velocity along hit normal
				float dot = pState->fVelocity[0] * result.fHitNormal[0] +
							pState->fVelocity[1] * result.fHitNormal[1] +
							pState->fVelocity[2] * result.fHitNormal[2];

				pState->fVelocity[0] = (pState->fVelocity[0] - 2.0f * dot * result.fHitNormal[0]) * PHYSICS_GRENADE_BOUNCE_DAMP;
				pState->fVelocity[1] = (pState->fVelocity[1] - 2.0f * dot * result.fHitNormal[1]) * PHYSICS_GRENADE_BOUNCE_DAMP;
				pState->fVelocity[2] = (pState->fVelocity[2] - 2.0f * dot * result.fHitNormal[2]) * PHYSICS_GRENADE_BOUNCE_DAMP;

				// Check if velocity is low enough to stop
				float speed = sqrtf(pState->fVelocity[0] * pState->fVelocity[0] +
									pState->fVelocity[1] * pState->fVelocity[1] +
									pState->fVelocity[2] * pState->fVelocity[2]);

				if (speed < PHYSICS_GRENADE_REST_VEL || pState->i32BounceCount >= PHYSICS_GRENADE_MAX_BOUNCES)
				{
					pState->bLanded = true;
					pState->fVelocity[0] = pState->fVelocity[1] = pState->fVelocity[2] = 0.0f;
				}

				return true;
			}
		}
	}

	// No collision - update position
	pState->fPos[0] = newPos[0];
	pState->fPos[1] = newPos[1];
	pState->fPos[2] = newPos[2];

	return true;
}

bool PhysicsEngine::PredictGrenadeImpact(const float* throwPos, const float* throwVelocity,
										  float fFuseTime, float* outImpactPos) const
{
	if (!throwPos || !throwVelocity || !outImpactPos)
		return false;

	GrenadeSimState state;
	state.fPos[0] = throwPos[0];
	state.fPos[1] = throwPos[1];
	state.fPos[2] = throwPos[2];
	state.fVelocity[0] = throwVelocity[0];
	state.fVelocity[1] = throwVelocity[1];
	state.fVelocity[2] = throwVelocity[2];
	state.fElapsedTime = 0.0f;
	state.bLanded = false;
	state.bExploded = false;
	state.i32BounceCount = 0;

	// Simulate until fuse runs out or landed
	float dt = PHYSICS_SIM_TIMESTEP;
	int maxSteps = (int)(fFuseTime / dt) + 1;

	for (int i = 0; i < maxSteps; i++)
	{
		float remainingTime = fFuseTime - state.fElapsedTime;
		float stepTime = (remainingTime < dt) ? remainingTime : dt;
		if (stepTime <= 0.0f)
			break;

		// Temporarily override fuse time to not trigger explosion during prediction
		float savedFuse = state.fElapsedTime;
		state.fElapsedTime = 0.0f;
		SimulateGrenade(&state, stepTime);
		state.fElapsedTime = savedFuse + stepTime;

		if (state.bLanded)
			break;
	}

	outImpactPos[0] = state.fPos[0];
	outImpactPos[1] = state.fPos[1];
	outImpactPos[2] = state.fPos[2];

	return true;
}

// ============================================================================
// Character Capsule Management
// ============================================================================

bool PhysicsEngine::CreateCharacterCapsule(int i32Slot, const float* pos)
{
	if (i32Slot < 0 || i32Slot >= BATTLE_SLOT_MAX || !pos)
		return false;

	PhysicsCapsule& cap = m_Capsules[i32Slot];
	cap.fPos[0] = pos[0];
	cap.fPos[1] = pos[1];
	cap.fPos[2] = pos[2];
	cap.fRadius = PHYSICS_CHARACTER_RADIUS;
	cap.fHeight = PHYSICS_CHARACTER_HEIGHT;
	cap.bActive = true;

#if defined(I3_PHYSX)
	if (m_bUsingPhysX && m_pScene)
	{
		NxActorDesc actorDesc;
		NxCapsuleShapeDesc capsuleDesc;
		capsuleDesc.radius = cap.fRadius;
		capsuleDesc.height = cap.fHeight - (2.0f * cap.fRadius);	// PhysX capsule height = cylinder height
		capsuleDesc.group = 1;	// Character collision group

		actorDesc.shapes.pushBack(&capsuleDesc);
		actorDesc.body = NULL;	// Kinematic (position updated by server)
		actorDesc.globalPose.t = NxVec3(pos[0], pos[1] + cap.fHeight * 0.5f, pos[2]);

		cap.pActor = m_pScene->createActor(actorDesc);
		if (!cap.pActor)
		{
			printf("[PhysicsEngine] Failed to create capsule actor for slot %d\n", i32Slot);
			cap.bActive = false;
			return false;
		}
	}
#endif

	return true;
}

void PhysicsEngine::DestroyCharacterCapsule(int i32Slot)
{
	if (i32Slot < 0 || i32Slot >= BATTLE_SLOT_MAX)
		return;

	PhysicsCapsule& cap = m_Capsules[i32Slot];

#if defined(I3_PHYSX)
	if (cap.pActor && m_pScene)
	{
		m_pScene->releaseActor(*cap.pActor);
		cap.pActor = nullptr;
	}
#endif

	cap.bActive = false;
}

void PhysicsEngine::UpdateCharacterPosition(int i32Slot, const float* pos)
{
	if (i32Slot < 0 || i32Slot >= BATTLE_SLOT_MAX || !pos)
		return;

	PhysicsCapsule& cap = m_Capsules[i32Slot];
	if (!cap.bActive)
		return;

	cap.fPos[0] = pos[0];
	cap.fPos[1] = pos[1];
	cap.fPos[2] = pos[2];

#if defined(I3_PHYSX)
	if (cap.pActor && m_bUsingPhysX)
	{
		NxMat34 pose;
		pose.t = NxVec3(pos[0], pos[1] + cap.fHeight * 0.5f, pos[2]);
		pose.M.id();
		cap.pActor->setGlobalPose(pose);
	}
#endif
}

// ============================================================================
// Ray-Capsule Intersection (CPU implementation)
//
// Tests a ray against a capsule defined by a base position, radius, and height.
// The capsule is axis-aligned vertically (Y-up).
// Returns distance to intersection, or -1.0f if no hit.
// ============================================================================
float PhysicsEngine::RayCapsuleIntersect(const float* rayOrigin, const float* rayDir,
										 const float* capsuleBase, float capsuleRadius,
										 float capsuleHeight)
{
	// Capsule = cylinder from capsuleBase.y to capsuleBase.y+capsuleHeight
	//         + sphere caps at both ends
	// For simplicity, test against the cylinder portion (the main body)

	// Cylinder axis is vertical (Y), center at capsuleBase + (0, height/2, 0)
	float cx = capsuleBase[0];
	float cy = capsuleBase[1] + capsuleHeight * 0.5f;
	float cz = capsuleBase[2];
	float halfH = capsuleHeight * 0.5f;

	// Transform ray to cylinder-local space (cylinder centered at origin, Y-axis aligned)
	float ox = rayOrigin[0] - cx;
	float oy = rayOrigin[1] - cy;
	float oz = rayOrigin[2] - cz;

	// Test infinite cylinder in XZ plane
	float dx = rayDir[0];
	float dz = rayDir[2];

	float a = dx * dx + dz * dz;
	float b = 2.0f * (ox * dx + oz * dz);
	float c = ox * ox + oz * oz - capsuleRadius * capsuleRadius;

	if (a < 1e-8f)
	{
		// Ray parallel to cylinder axis - check if inside radius
		if (c > 0.0f)
			return -1.0f;

		// Check Y bounds
		float tMin = (-halfH - oy) / rayDir[1];
		float tMax = (halfH - oy) / rayDir[1];
		if (tMin > tMax) { float tmp = tMin; tMin = tMax; tMax = tmp; }
		return (tMin > 0.0f) ? tMin : ((tMax > 0.0f) ? tMax : -1.0f);
	}

	float discriminant = b * b - 4.0f * a * c;
	if (discriminant < 0.0f)
		return -1.0f;

	float sqrtD = sqrtf(discriminant);
	float t1 = (-b - sqrtD) / (2.0f * a);
	float t2 = (-b + sqrtD) / (2.0f * a);

	// Check both intersection points for Y-bounds
	float bestT = -1.0f;

	for (int i = 0; i < 2; i++)
	{
		float t = (i == 0) ? t1 : t2;
		if (t < 0.0f)
			continue;

		float hitY = oy + rayDir[1] * t;
		if (hitY >= -halfH && hitY <= halfH)
		{
			bestT = t;
			break;	// First valid hit is closest
		}
	}

	// Also test hemisphere caps (sphere at top and bottom)
	for (int cap = 0; cap < 2; cap++)
	{
		float capY = (cap == 0) ? -halfH : halfH;
		float scx = 0.0f, scy = capY, scz = 0.0f;

		float sox = ox - scx;
		float soy = oy - scy;
		float soz = oz - scz;

		float sa = rayDir[0] * rayDir[0] + rayDir[1] * rayDir[1] + rayDir[2] * rayDir[2];
		float sb = 2.0f * (sox * rayDir[0] + soy * rayDir[1] + soz * rayDir[2]);
		float sc = sox * sox + soy * soy + soz * soz - capsuleRadius * capsuleRadius;

		float sDisc = sb * sb - 4.0f * sa * sc;
		if (sDisc >= 0.0f)
		{
			float sSqrt = sqrtf(sDisc);
			float st = (-sb - sSqrt) / (2.0f * sa);
			if (st > 0.0f && (bestT < 0.0f || st < bestT))
				bestT = st;
		}
	}

	return bestT;
}

float PhysicsEngine::RaycastCharacterCapsule(int i32Slot, const float* rayOrigin,
											  const float* rayDir, float fMaxDist) const
{
	if (i32Slot < 0 || i32Slot >= BATTLE_SLOT_MAX)
		return -1.0f;

	const PhysicsCapsule& cap = m_Capsules[i32Slot];
	if (!cap.bActive)
		return -1.0f;

	float t = RayCapsuleIntersect(rayOrigin, rayDir, cap.fPos, cap.fRadius, cap.fHeight);
	if (t >= 0.0f && t <= fMaxDist)
		return t;

	return -1.0f;
}

// ============================================================================
// Scene Simulation
// ============================================================================

void PhysicsEngine::StepSimulation(float fDeltaTime)
{
#if defined(I3_PHYSX)
	if (m_bUsingPhysX && m_pScene)
	{
		m_pScene->simulate(fDeltaTime);
		m_pScene->flushStream();
		m_pScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
	}
#else
	(void)fDeltaTime;	// No-op for CPU fallback
#endif
}
