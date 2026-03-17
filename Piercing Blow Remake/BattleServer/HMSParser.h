#ifndef __HMSPARSER_H__
#define __HMSPARSER_H__

#pragma once
#include "WeaponSystem.h"

// ============================================================================
// HMSParser - Anti-cheat packet validation with enforcement
// Production port of original HMSParser from Dedication/Dedi/HMSParser.h
//
// Validates game packets for hacking: weapon range, fire speed, weapon sync,
// ghost detection, recall hack, RPG exploit, etc.
//
// ENFORCEMENT: Unlike the basic version that only logs, this version:
// - Sends HACK_NOTIFY to GameServer via BattleSession (for all severities)
// - Removes HIGH/CRITICAL hack offenders from BattleRoom
// - Tracks violation counts per-character with escalating severity
// - Saves character state before processing, rolls back on hack detection
//   (matches original: m_i16StoreHP pattern)
// ============================================================================

class BattleRoom;
class GameCharacter;
class BattleSession;

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
	HACK_SEVERITY_LOW = 0,		// Log + track count
	HACK_SEVERITY_MEDIUM,		// Log + notify GameServer
	HACK_SEVERITY_HIGH,			// Log + notify + remove from room
	HACK_SEVERITY_CRITICAL,		// Log + notify + remove + request ban
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

// Per-character violation tracking
struct ViolationRecord
{
	int32_t		ai32Counts[HACK_TYPE_COUNT];	// Count per hack type
	int32_t		i32TotalViolations;				// Total across all types
	DWORD		dwFirstViolationTime;			// When first violation detected
	DWORD		dwLastViolationTime;			// When last violation detected

	void Reset()
	{
		memset(ai32Counts, 0, sizeof(ai32Counts));
		i32TotalViolations = 0;
		dwFirstViolationTime = 0;
		dwLastViolationTime = 0;
	}

	void AddViolation(HackType eType)
	{
		if (eType > HACK_TYPE_NONE && eType < HACK_TYPE_COUNT)
			ai32Counts[eType]++;
		i32TotalViolations++;
		DWORD dwNow = GetTickCount();
		if (dwFirstViolationTime == 0)
			dwFirstViolationTime = dwNow;
		dwLastViolationTime = dwNow;
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

// Violation thresholds for escalation
#define HMS_VIOLATION_THRESHOLD_MEDIUM		3	// Violations before MEDIUM severity
#define HMS_VIOLATION_THRESHOLD_HIGH		8	// Violations before HIGH severity
#define HMS_VIOLATION_THRESHOLD_CRITICAL		15	// Violations before CRITICAL severity
#define HMS_VIOLATION_WINDOW_MS				60000	// Violations within this window are escalated

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

	// ============================================================
	// ENFORCEMENT - report and act on hack detection
	// ============================================================

	// Report hack detection: logs, notifies GameServer, optionally removes player
	static void EnforceHack(BattleRoom* pRoom, const HackCheckResult* pResult,
							int64_t i64UID);

	// Get/reset violation record for a slot
	static ViolationRecord* GetViolationRecord(uint32_t ui32SlotIdx);
	static void ResetViolationRecords();

private:
	// Distance calculation
	static float Distance3D(const float* a, const float* b);

	// Fire speed constants (from original)
	static constexpr float FIRE_SPEED_TOLERANCE = 0.8f;	// 80% of fire delay minimum
	static constexpr float GHOST_MAX_DISTANCE = 2000.0f;	// Max teleport before flagged
	static constexpr float RECALL_MAX_DISTANCE = 500.0f;	// Max death pos difference

	// Per-slot violation tracking (thread-safe: each slot only accessed by its track's worker)
	static ViolationRecord	s_Violations[BATTLE_SLOT_MAX];
	static CRITICAL_SECTION	s_csViolations;
	static bool				s_bInitialized;

	// Initialize violation tracking (called once)
	static void InitializeIfNeeded();
};

#endif // __HMSPARSER_H__
