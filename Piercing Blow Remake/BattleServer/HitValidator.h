#ifndef __HITVALIDATOR_H__
#define __HITVALIDATOR_H__

#pragma once

class CollisionSystem;
class PhysicsEngine;
class BattleMember;
class ConfigXML;

// Maximum allowed character speed for speed hack detection (units/sec)
#define MAX_CHARACTER_SPEED		500.0f		// ~1.67x normal speed allowance
#define MAX_POSITION_TELEPORT	1000.0f		// Max distance jump between updates

// HitValidator - Server-side hit and movement validation
// Replaces the HMSParser anti-cheat system from the original DediServer
// Performs basic validations: line-of-sight, range, speed, position
class HitValidator
{
public:
	HitValidator();
	~HitValidator();

	// Set collision system reference (per-room, from MapData)
	void	SetCollisionSystem(CollisionSystem* pCollision)		{ m_pCollision = pCollision; }
	void	SetPhysicsEngine(PhysicsEngine* pPhysics)			{ m_pPhysicsEngine = pPhysics; }
	void	SetConfigXML(ConfigXML* pConfig)						{ m_pConfig = pConfig; }

	// ===== Hit Validation =====

	// Validate a hit: checks line-of-sight through collision mesh and weapon range
	// Returns true if the hit is valid
	bool	ValidateHit(BattleMember* pShooter, BattleMember* pTarget,
						uint32_t ui32WeaponID, bool bHeadshot) const;

	// ===== Movement Validation =====

	// Check if a position update is valid (not too fast, not through walls)
	// Returns true if movement is valid
	bool	ValidateMovement(BattleMember* pMember, const float* newPos, DWORD dwNow) const;

	// Check if a position is within map bounds (has ground below)
	bool	ValidatePosition(const float* pos) const;

	// ===== Speed Hack Detection =====

	// Calculate distance traveled per second
	static float	CalculateSpeed(const float* posOld, const float* posNew, DWORD dwElapsedMs);

	// Simple 3D distance
	static float	Distance3D(const float* a, const float* b);

private:
	CollisionSystem*	m_pCollision;
	PhysicsEngine*		m_pPhysicsEngine;
	ConfigXML*			m_pConfig;
};

#endif // __HITVALIDATOR_H__
