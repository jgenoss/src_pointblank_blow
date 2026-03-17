#ifndef __HMSPARSER_H__
#define __HMSPARSER_H__

#pragma once
#include "WeaponSystem.h"

// ============================================================================
// HMSParser - Anti-cheat packet validation
// Port of original HMSParser from Dedication/Dedi/HMSParser.h
// Validates game packets for hacking: weapon range, fire speed, weapon sync,
// ghost detection, recall hack, RPG exploit, etc.
// ============================================================================

class BattleRoom;
class GameCharacter;

// Hack detection types (from DS_HACK_TYPE)
enum HackType
{
	HACK_TYPE_NONE = 0,
	HACK_TYPE_WEAPON_RANGE,			// Hit outside weapon range
	HACK_TYPE_FIRE_SPEED,			// Firing faster than weapon allows
	HACK_TYPE_WEAPON_SYNC,			// Weapon mismatch (using unequipped weapon)
	HACK_TYPE_WEAPON_PARAM,			// Invalid weapon parameters
	HACK_TYPE_SPEED_HACK,			// Moving too fast
	HACK_TYPE_GHOST,				// Position doesn't match (teleport/noclip)
	HACK_TYPE_RECALL,				// Recall hack (instant death position change)
	HACK_TYPE_RPG_EXPLOIT,			// RPG exploit (illegal weapon pickup)
	HACK_TYPE_BULLET_HACK,			// Infinite bullets
	HACK_TYPE_HP_HACK,				// HP manipulation
	HACK_TYPE_INVINCIBLE,			// Taking no damage
	HACK_TYPE_WALLSHOT,				// Shooting through walls
	HACK_TYPE_COUNT,
};

// Hack severity
enum HackSeverity
{
	HACK_SEVERITY_LOW = 0,		// Log only
	HACK_SEVERITY_MEDIUM,		// Log + flag user
	HACK_SEVERITY_HIGH,			// Log + kick from battle
	HACK_SEVERITY_CRITICAL,		// Log + kick + ban
};

// Hack detection result
struct HackCheckResult
{
	HackType		eType;
	HackSeverity	eSeverity;
	uint32_t		ui32SlotIdx;
	char			szDescription[128];

	HackCheckResult()
		: eType(HACK_TYPE_NONE)
		, eSeverity(HACK_SEVERITY_LOW)
		, ui32SlotIdx(0)
	{
		szDescription[0] = '\0';
	}
};

// Hit info for validation (simplified from N_PCINFO_HIT_DEDICATED2)
struct HitInfo
{
	uint32_t	ui32AttackerSlot;
	uint32_t	ui32TargetSlot;
	uint32_t	ui32WeaponID;
	uint8_t		ui8AttackType;		// AttackType
	uint8_t		ui8HitPart;			// CharaHitPart
	bool		bExtension;			// Weapon extension active
	bool		bHeadshot;
	float		fAttackerPos[3];
	float		fTargetPos[3];
	float		fDeathPos[3];		// Position at death (for recall check)
};

// Fire info for validation
struct FireInfo
{
	uint32_t	ui32SlotIdx;
	uint32_t	ui32WeaponID;
	uint8_t		ui8AttackType;
	float		fPacketTime;		// Client-side timestamp
};

class HMSParser
{
public:
	// ============================================================
	// Hit Validation
	// ============================================================

	// Check if hit is within weapon range
	static HackType CheckWeaponRange(BattleRoom* pRoom, uint32_t ui32SlotIdx,
									 const HitInfo* pHitInfo);

	// Check fire rate isn't too fast
	static HackType CheckFireSpeed(BattleRoom* pRoom, uint32_t ui32SlotIdx,
								   const FireInfo* pFire);

	// Check weapon sync (player using weapon they have)
	static HackType CheckWeaponSync(BattleRoom* pRoom, uint32_t ui32SlotIdx,
									uint32_t ui32WeaponID);

	// Check weapon parameters
	static HackType CheckWeaponParam(BattleRoom* pRoom, uint32_t ui32SlotIdx,
									 uint32_t ui32WeaponID, uint8_t ui8AttackType);

	// ============================================================
	// Movement Validation
	// ============================================================

	// Check for ghost/teleport/noclip
	static void CheckGhost(BattleRoom* pRoom, GameCharacter* pChara,
						   float fPacketTime, uint32_t ui32SlotIdx,
						   const float* fNewPos);

	// Check for speed hack via speed state averaging
	static HackType CheckSpeedHack(BattleRoom* pRoom, uint32_t ui32SlotIdx,
								   const float* fOldPos, const float* fNewPos,
								   float fElapsed);

	// ============================================================
	// Death Validation
	// ============================================================

	// Check recall hack (death position mismatch)
	static bool CheckRecallHack(BattleRoom* pRoom, uint32_t ui32SlotIdx,
								const float* fDeathPos0, const float* fDeathPos1);

	// Check invincibility (should have taken damage)
	static bool IsInvincibleTime(BattleRoom* pRoom, uint32_t ui32AttackerSlot,
								 uint32_t ui32TargetSlot);

	// ============================================================
	// Weapon Exploit Checks
	// ============================================================

	// Check for illegal RPG/special weapon pickup
	static HackType CheckNaturalRPGGet(BattleRoom* pRoom, uint32_t ui32SlotIdx,
										uint32_t ui32Class, uint32_t ui32Number);

	// Check if weapon is in the allowed list
	static HackType CheckUseWeapon(BattleRoom* pRoom, uint32_t ui32SlotIdx,
								   uint32_t ui32ItemID);

	// ============================================================
	// Composite validation
	// ============================================================

	// Full hit validation: range + LOS + weapon sync + fire speed
	static HackCheckResult ValidateHit(BattleRoom* pRoom, const HitInfo* pHitInfo,
									   float fPacketTime);

	// Full position validation: speed + ghost + bounds
	static HackCheckResult ValidatePosition(BattleRoom* pRoom, uint32_t ui32SlotIdx,
											const float* fNewPos, float fPacketTime);

private:
	// Distance calculation
	static float Distance3D(const float* a, const float* b);

	// Fire speed constants (from original)
	static constexpr float FIRE_SPEED_TOLERANCE = 0.8f;	// 80% of fire delay minimum
	static constexpr float GHOST_MAX_DISTANCE = 2000.0f;	// Max teleport before flagged
	static constexpr float RECALL_MAX_DISTANCE = 500.0f;	// Max death pos difference
};

#endif // __HMSPARSER_H__
