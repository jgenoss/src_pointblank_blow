#include "pch.h"
#include "HitValidator.h"
#include "CollisionSystem.h"
#include "PhysicsEngine.h"
#include "BattleMember.h"
#include "ConfigXML.h"
#include <cmath>

HitValidator::HitValidator()
	: m_pCollision(nullptr)
	, m_pPhysicsEngine(nullptr)
	, m_pConfig(nullptr)
{
}

HitValidator::~HitValidator()
{
}

// ============================================================================
// Hit Validation
// ============================================================================
bool HitValidator::ValidateHit(BattleMember* pShooter, BattleMember* pTarget,
							   uint32_t ui32WeaponID, bool bHeadshot) const
{
	if (!pShooter || !pTarget)
		return false;

	// Dead players can't shoot or be shot
	if (!pShooter->IsAlive() || !pTarget->IsAlive())
		return false;

	// Same team? (friendly fire check - usually disabled in PB)
	// Allow for now, let game mode handle this

	const float* shooterPos = pShooter->GetPosition();
	const float* targetPos = pTarget->GetPosition();

	// 1. Range check
	float dist = Distance3D(shooterPos, targetPos);
	float maxRange = 10000.0f;	// Default max range

	if (m_pConfig)
	{
		maxRange = m_pConfig->GetWeaponRange(ui32WeaponID);
		// Add 10% tolerance for network latency
		maxRange *= 1.1f;
	}

	if (dist > maxRange)
	{
		// Target out of weapon range
		return false;
	}

	// 2. Line-of-sight check (wall hack detection)
	// Adjust positions to center-mass height (about 85cm from feet)
	float shooterCenter[3] = { shooterPos[0], shooterPos[1] + PHYSICS_CHARACTER_MASS_CENTER, shooterPos[2] };
	float targetCenter[3] = { targetPos[0], targetPos[1] + PHYSICS_CHARACTER_MASS_CENTER, targetPos[2] };

	if (bHeadshot)
	{
		// For headshots, check line to head position (about 160cm)
		targetCenter[1] = targetPos[1] + PHYSICS_HEAD_HEIGHT;
	}

	// Prefer PhysicsEngine (PhysX or unified CPU) over legacy CollisionSystem
	if (m_pPhysicsEngine && m_pPhysicsEngine->IsInitialized())
	{
		if (!m_pPhysicsEngine->CheckLineOfSight(shooterCenter, targetCenter))
		{
			// Wall between shooter and target
			return false;
		}
	}
	else if (m_pCollision && m_pCollision->IsInitialized())
	{
		// Legacy fallback
		if (!m_pCollision->CheckLineOfSight(shooterCenter, targetCenter))
		{
			return false;
		}
	}

	return true;
}

// ============================================================================
// Movement Validation
// ============================================================================
bool HitValidator::ValidateMovement(BattleMember* pMember, const float* newPos, DWORD dwNow) const
{
	if (!pMember || !newPos)
		return false;

	const float* oldPos = pMember->GetPosition();
	DWORD dwLastUpdate = pMember->GetLastPosUpdateTime();

	// First position update - always valid
	if (dwLastUpdate == 0)
		return true;

	DWORD dwElapsedMs = dwNow - dwLastUpdate;

	// Very short time between updates is normal for high-frequency packets
	if (dwElapsedMs < 10)
		return true;

	// Calculate speed
	float speed = CalculateSpeed(oldPos, newPos, dwElapsedMs);

	// Check for teleport (sudden large jump)
	float dist = Distance3D(oldPos, newPos);
	if (dist > MAX_POSITION_TELEPORT)
	{
		// Could be respawn or legitimate teleport
		// Only flag if not recently dead
		if (pMember->IsAlive() && dwElapsedMs < 5000)
			return false;
	}

	// Speed check with generous tolerance (network jitter, interpolation)
	float maxSpeed = MAX_CHARACTER_SPEED;
	if (m_pConfig)
	{
		float configSpeed = m_pConfig->GetCharacterMoveSpeed(0);
		maxSpeed = configSpeed * 1.5f;	// 50% tolerance
	}

	if (speed > maxSpeed && dwElapsedMs > 100)
	{
		// Speed hack detected
		return false;
	}

	// Position validity check (optional - only if collision data loaded)
	// Don't validate every update - too expensive
	// Only check every ~1 second
	if (dwElapsedMs > 1000)
	{
		if (m_pPhysicsEngine && m_pPhysicsEngine->IsInitialized())
		{
			if (!m_pPhysicsEngine->IsPositionValid(newPos))
				return false;
		}
		else if (m_pCollision && m_pCollision->IsInitialized())
		{
			if (!m_pCollision->IsPositionValid(newPos))
				return false;
		}
	}

	return true;
}

bool HitValidator::ValidatePosition(const float* pos) const
{
	if (!pos)
		return false;

	// Prefer PhysicsEngine over legacy CollisionSystem
	if (m_pPhysicsEngine && m_pPhysicsEngine->IsInitialized())
		return m_pPhysicsEngine->IsPositionValid(pos);

	if (m_pCollision && m_pCollision->IsInitialized())
		return m_pCollision->IsPositionValid(pos);

	return true;	// No collision data - accept all positions
}

// ============================================================================
// Utility
// ============================================================================
float HitValidator::CalculateSpeed(const float* posOld, const float* posNew, DWORD dwElapsedMs)
{
	if (dwElapsedMs == 0)
		return 0.0f;

	float dist = Distance3D(posOld, posNew);
	return dist / ((float)dwElapsedMs / 1000.0f);
}

float HitValidator::Distance3D(const float* a, const float* b)
{
	float dx = b[0] - a[0];
	float dy = b[1] - a[1];
	float dz = b[2] - a[2];
	return sqrtf(dx * dx + dy * dy + dz * dz);
}
