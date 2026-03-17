#include "pch.h"
#include "HMSParser.h"
#include "BattleRoom.h"
#include "GameCharacter.h"
#include <cmath>
#include <cstdio>

// ============================================================================
// Distance calculation
// ============================================================================

float HMSParser::Distance3D(const float* a, const float* b)
{
	float dx = b[0] - a[0];
	float dy = b[1] - a[1];
	float dz = b[2] - a[2];
	return sqrtf(dx * dx + dy * dy + dz * dz);
}

// ============================================================================
// Hit Validation
// ============================================================================

HackType HMSParser::CheckWeaponRange(BattleRoom* pRoom, uint32_t ui32SlotIdx,
									  const HitInfo* pHitInfo)
{
	if (!pRoom || !pHitInfo)
		return HACK_TYPE_NONE;

	// Get weapon range from table
	float fRange = WeaponTable::GetInstance().GetWeaponRange(pHitInfo->ui32WeaponID);
	if (fRange <= 0.0f)
		return HACK_TYPE_NONE;	// Unknown weapon, skip check

	// Extension increases range by 20%
	if (pHitInfo->bExtension)
		fRange *= 1.2f;

	// Calculate distance between attacker and target
	float fDist = Distance3D(pHitInfo->fAttackerPos, pHitInfo->fTargetPos);

	// Allow 30% tolerance for latency compensation
	if (fDist > fRange * 1.3f)
		return HACK_TYPE_WEAPON_RANGE;

	return HACK_TYPE_NONE;
}

HackType HMSParser::CheckFireSpeed(BattleRoom* pRoom, uint32_t ui32SlotIdx,
									const FireInfo* pFire)
{
	if (!pRoom || !pFire)
		return HACK_TYPE_NONE;

	GameCharacter* pChara = pRoom->GetCharacter(ui32SlotIdx);
	if (!pChara)
		return HACK_TYPE_NONE;

	// Get weapon fire delay
	const WeaponDamageInfo* pWeaponInfo = WeaponTable::GetInstance().GetWeaponInfo(pFire->ui32WeaponID);
	if (!pWeaponInfo)
		return HACK_TYPE_NONE;

	float fFireDelay = pWeaponInfo->fFireDelay;
	if (fFireDelay <= 0.0f)
		return HACK_TYPE_NONE;

	// Check time since last fire
	float fLastFire = pChara->GetLastFireTime();
	if (fLastFire <= 0.0f)
		return HACK_TYPE_NONE;	// First shot

	float fElapsed = pFire->fPacketTime - fLastFire;

	// Fire speed hack: firing faster than weapon allows (with tolerance)
	if (fElapsed < fFireDelay * FIRE_SPEED_TOLERANCE)
	{
		// Increment fire count for repeated violations
		pChara->IncFireCount();

		// Only flag after multiple fast fires (reduce false positives from lag)
		if (pChara->GetFireCount() >= 5)
			return HACK_TYPE_FIRE_SPEED;
	}
	else
	{
		// Reset fire count on normal fire speed
		pChara->ResetFireCount();
	}

	return HACK_TYPE_NONE;
}

HackType HMSParser::CheckWeaponSync(BattleRoom* pRoom, uint32_t ui32SlotIdx,
									  uint32_t ui32WeaponID)
{
	if (!pRoom || ui32WeaponID == 0)
		return HACK_TYPE_NONE;

	GameCharacter* pChara = pRoom->GetCharacter(ui32SlotIdx);
	if (!pChara)
		return HACK_TYPE_NONE;

	// Check if player has this weapon equipped in any slot
	for (uint8_t slot = 0; slot < WEAPON_SLOT_COUNT; slot++)
	{
		if (pChara->GetEquipWeaponID(slot) == ui32WeaponID)
			return HACK_TYPE_NONE;
	}

	// Player is using a weapon they don't have
	return HACK_TYPE_WEAPON_SYNC;
}

HackType HMSParser::CheckWeaponParam(BattleRoom* pRoom, uint32_t ui32SlotIdx,
									   uint32_t ui32WeaponID, uint8_t ui8AttackType)
{
	if (!pRoom)
		return HACK_TYPE_NONE;

	// Validate attack type is within range
	if (ui8AttackType >= ATTACK_TYPE_MAX)
		return HACK_TYPE_WEAPON_PARAM;

	// Validate weapon exists in table
	const WeaponDamageInfo* pInfo = WeaponTable::GetInstance().GetWeaponInfo(ui32WeaponID);
	if (!pInfo)
		return HACK_TYPE_WEAPON_PARAM;

	// Validate attack type has damage defined for this weapon
	if (ui8AttackType != ATTACK_TYPE_NONE && pInfo->afDamage[ui8AttackType] <= 0.0f)
	{
		// Some weapons legitimately have 0 damage for certain attack types
		// Only flag if the primary attack type also has 0 damage
		if (pInfo->afDamage[ATTACK_TYPE_L1] <= 0.0f)
			return HACK_TYPE_WEAPON_PARAM;
	}

	return HACK_TYPE_NONE;
}

// ============================================================================
// Movement Validation
// ============================================================================

void HMSParser::CheckGhost(BattleRoom* pRoom, GameCharacter* pChara,
							float fPacketTime, uint32_t ui32SlotIdx,
							const float* fNewPos)
{
	if (!pRoom || !pChara || !fNewPos)
		return;

	const float* fOldPos = pChara->GetPosition();
	float fDist = Distance3D(fOldPos, fNewPos);

	// Check for teleport/noclip (large distance in single packet)
	if (fDist > GHOST_MAX_DISTANCE)
	{
		// Ghost detected - log but don't immediately kick
		// Could be legitimate in certain game modes (respawn teleport)
		// The BattleRoom should handle the response based on game state
	}

	// Update speed state for speed hack averaging
	float fElapsed = fPacketTime - pChara->GetPosRecvTime();
	if (fElapsed > 0.001f)
	{
		float fSpeed = fDist / fElapsed;
		SpeedState* pSpeed = pChara->GetSpeedState();
		if (pSpeed)
			pSpeed->InsertSpeed(fSpeed);
	}
}

HackType HMSParser::CheckSpeedHack(BattleRoom* pRoom, uint32_t ui32SlotIdx,
									 const float* fOldPos, const float* fNewPos,
									 float fElapsed)
{
	if (!pRoom || !fOldPos || !fNewPos)
		return HACK_TYPE_NONE;

	GameCharacter* pChara = pRoom->GetCharacter(ui32SlotIdx);
	if (!pChara)
		return HACK_TYPE_NONE;

	if (fElapsed < 0.001f)
		return HACK_TYPE_NONE;

	float fDist = Distance3D(fOldPos, fNewPos);
	float fSpeed = fDist / fElapsed;

	// Insert into speed state for averaging
	SpeedState* pSpeed = pChara->GetSpeedState();
	if (!pSpeed)
		return HACK_TYPE_NONE;

	pSpeed->InsertSpeed(fSpeed);

	// Check averaged speed
	float fAvgSpeed = 0.0f;
	if (pSpeed->IsSpeedHack(&fAvgSpeed))
		return HACK_TYPE_SPEED_HACK;

	return HACK_TYPE_NONE;
}

// ============================================================================
// Death Validation
// ============================================================================

bool HMSParser::CheckRecallHack(BattleRoom* pRoom, uint32_t ui32SlotIdx,
								 const float* fDeathPos0, const float* fDeathPos1)
{
	if (!pRoom || !fDeathPos0 || !fDeathPos1)
		return false;

	// Recall hack: death position in kill packet doesn't match actual position
	float fDist = Distance3D(fDeathPos0, fDeathPos1);
	return (fDist > RECALL_MAX_DISTANCE);
}

bool HMSParser::IsInvincibleTime(BattleRoom* pRoom, uint32_t ui32AttackerSlot,
								  uint32_t ui32TargetSlot)
{
	if (!pRoom)
		return false;

	// Check if target is in invincibility period (e.g., just respawned)
	// This is not a hack check per se - it's to prevent damage during spawn protection
	// The BattleRoom manages invincibility timers
	return false;	// Default: not invincible
}

// ============================================================================
// Weapon Exploit Checks
// ============================================================================

HackType HMSParser::CheckNaturalRPGGet(BattleRoom* pRoom, uint32_t ui32SlotIdx,
										 uint32_t ui32Class, uint32_t ui32Number)
{
	if (!pRoom)
		return HACK_TYPE_NONE;

	// Check if the weapon pickup is legitimate
	// RPG/special weapons can only be obtained from weapon boxes on the map
	// If a player claims to have picked up a special weapon but there's no
	// weapon box event, it's an exploit

	uint8_t ui8Type = (uint8_t)ui32Class;

	// Rocket launchers and special weapons are restricted
	if (ui8Type == WEAPON_CLASS_ROCKET)
	{
		// Only allowed from weapon box pickups - the caller must verify
		// the weapon box interaction happened legitimately
		// For now, just validate the weapon exists in the table
		uint32_t ui32ItemID = (ui32Class << 16) | (ui32Number & 0xFFFF);
		const WeaponDamageInfo* pInfo = WeaponTable::GetInstance().GetWeaponInfo(ui32ItemID);
		if (!pInfo)
			return HACK_TYPE_RPG_EXPLOIT;
	}

	return HACK_TYPE_NONE;
}

HackType HMSParser::CheckUseWeapon(BattleRoom* pRoom, uint32_t ui32SlotIdx,
									uint32_t ui32ItemID)
{
	if (!pRoom || ui32ItemID == 0)
		return HACK_TYPE_NONE;

	// Verify weapon exists in the weapon table
	const WeaponDamageInfo* pInfo = WeaponTable::GetInstance().GetWeaponInfo(ui32ItemID);
	if (!pInfo)
		return HACK_TYPE_WEAPON_SYNC;

	// Also check weapon sync
	return CheckWeaponSync(pRoom, ui32SlotIdx, ui32ItemID);
}

// ============================================================================
// Composite Validation
// ============================================================================

HackCheckResult HMSParser::ValidateHit(BattleRoom* pRoom, const HitInfo* pHitInfo,
										 float fPacketTime)
{
	HackCheckResult result;

	if (!pRoom || !pHitInfo)
		return result;

	// 1. Check weapon sync
	HackType eHack = CheckWeaponSync(pRoom, pHitInfo->ui32AttackerSlot, pHitInfo->ui32WeaponID);
	if (eHack != HACK_TYPE_NONE)
	{
		result.eType = eHack;
		result.eSeverity = HACK_SEVERITY_HIGH;
		result.ui32SlotIdx = pHitInfo->ui32AttackerSlot;
		snprintf(result.szDescription, sizeof(result.szDescription),
				 "Weapon sync hack: slot %u using weapon %u",
				 pHitInfo->ui32AttackerSlot, pHitInfo->ui32WeaponID);
		return result;
	}

	// 2. Check weapon parameters
	eHack = CheckWeaponParam(pRoom, pHitInfo->ui32AttackerSlot,
							 pHitInfo->ui32WeaponID, pHitInfo->ui8AttackType);
	if (eHack != HACK_TYPE_NONE)
	{
		result.eType = eHack;
		result.eSeverity = HACK_SEVERITY_MEDIUM;
		result.ui32SlotIdx = pHitInfo->ui32AttackerSlot;
		snprintf(result.szDescription, sizeof(result.szDescription),
				 "Invalid weapon param: weapon %u attack %u",
				 pHitInfo->ui32WeaponID, pHitInfo->ui8AttackType);
		return result;
	}

	// 3. Check weapon range
	eHack = CheckWeaponRange(pRoom, pHitInfo->ui32AttackerSlot, pHitInfo);
	if (eHack != HACK_TYPE_NONE)
	{
		result.eType = eHack;
		result.eSeverity = HACK_SEVERITY_MEDIUM;
		result.ui32SlotIdx = pHitInfo->ui32AttackerSlot;
		snprintf(result.szDescription, sizeof(result.szDescription),
				 "Range hack: slot %u weapon %u",
				 pHitInfo->ui32AttackerSlot, pHitInfo->ui32WeaponID);
		return result;
	}

	// 4. Check fire speed
	FireInfo fire;
	fire.ui32SlotIdx = pHitInfo->ui32AttackerSlot;
	fire.ui32WeaponID = pHitInfo->ui32WeaponID;
	fire.ui8AttackType = pHitInfo->ui8AttackType;
	fire.fPacketTime = fPacketTime;

	eHack = CheckFireSpeed(pRoom, pHitInfo->ui32AttackerSlot, &fire);
	if (eHack != HACK_TYPE_NONE)
	{
		result.eType = eHack;
		result.eSeverity = HACK_SEVERITY_HIGH;
		result.ui32SlotIdx = pHitInfo->ui32AttackerSlot;
		snprintf(result.szDescription, sizeof(result.szDescription),
				 "Fire speed hack: slot %u weapon %u",
				 pHitInfo->ui32AttackerSlot, pHitInfo->ui32WeaponID);
		return result;
	}

	// 5. Check recall hack (death position mismatch)
	if (pHitInfo->fDeathPos[0] != 0.0f || pHitInfo->fDeathPos[1] != 0.0f || pHitInfo->fDeathPos[2] != 0.0f)
	{
		if (CheckRecallHack(pRoom, pHitInfo->ui32TargetSlot,
							pHitInfo->fTargetPos, pHitInfo->fDeathPos))
		{
			result.eType = HACK_TYPE_RECALL;
			result.eSeverity = HACK_SEVERITY_HIGH;
			result.ui32SlotIdx = pHitInfo->ui32TargetSlot;
			snprintf(result.szDescription, sizeof(result.szDescription),
					 "Recall hack: slot %u death pos mismatch",
					 pHitInfo->ui32TargetSlot);
			return result;
		}
	}

	return result;	// HACK_TYPE_NONE
}

HackCheckResult HMSParser::ValidatePosition(BattleRoom* pRoom, uint32_t ui32SlotIdx,
											  const float* fNewPos, float fPacketTime)
{
	HackCheckResult result;

	if (!pRoom || !fNewPos)
		return result;

	GameCharacter* pChara = pRoom->GetCharacter(ui32SlotIdx);
	if (!pChara)
		return result;

	const float* fOldPos = pChara->GetPosition();
	float fElapsed = fPacketTime - pChara->GetPosRecvTime();

	// 1. Check ghost/teleport
	float fDist = Distance3D(fOldPos, fNewPos);
	if (fDist > GHOST_MAX_DISTANCE && fElapsed < 2.0f)
	{
		result.eType = HACK_TYPE_GHOST;
		result.eSeverity = HACK_SEVERITY_HIGH;
		result.ui32SlotIdx = ui32SlotIdx;
		snprintf(result.szDescription, sizeof(result.szDescription),
				 "Ghost hack: slot %u moved %.0f units in %.2fs",
				 ui32SlotIdx, fDist, fElapsed);
		return result;
	}

	// 2. Check speed hack (averaged)
	HackType eHack = CheckSpeedHack(pRoom, ui32SlotIdx, fOldPos, fNewPos, fElapsed);
	if (eHack != HACK_TYPE_NONE)
	{
		result.eType = eHack;
		result.eSeverity = HACK_SEVERITY_MEDIUM;
		result.ui32SlotIdx = ui32SlotIdx;

		float fAvg = pChara->GetSpeedState()->GetAverageSpeed();
		snprintf(result.szDescription, sizeof(result.szDescription),
				 "Speed hack: slot %u avg speed %.0f",
				 ui32SlotIdx, fAvg);
		return result;
	}

	return result;	// HACK_TYPE_NONE
}
