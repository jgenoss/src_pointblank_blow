#include "pch.h"
#include "HMSParser.h"
#include "BattleRoom.h"
#include "BattleMember.h"
#include "GameCharacter.h"
#include "BattleSession.h"
#include "BattleSessionManager.h"
#include <cmath>
#include <cstdio>

// Static members
ViolationRecord HMSParser::s_Violations[BATTLE_SLOT_MAX];
CRITICAL_SECTION HMSParser::s_csViolations;
bool HMSParser::s_bInitialized = false;

void HMSParser::InitializeIfNeeded()
{
	if (s_bInitialized)
		return;

	InitializeCriticalSectionAndSpinCount(&s_csViolations, 4000);
	ResetViolationRecords();
	s_bInitialized = true;
}

ViolationRecord* HMSParser::GetViolationRecord(uint32_t ui32SlotIdx)
{
	if (ui32SlotIdx >= BATTLE_SLOT_MAX)
		return nullptr;

	InitializeIfNeeded();
	return &s_Violations[ui32SlotIdx];
}

void HMSParser::ResetViolationRecords()
{
	for (int i = 0; i < BATTLE_SLOT_MAX; i++)
		s_Violations[i].Reset();
}

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

	float fRange = WeaponTable::GetInstance().GetWeaponRange(pHitInfo->ui32WeaponID);
	if (fRange <= 0.0f)
		return HACK_TYPE_NONE;

	if (pHitInfo->bExtension)
		fRange *= 1.2f;

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

	const WeaponDamageInfo* pWeaponInfo = WeaponTable::GetInstance().GetWeaponInfo(pFire->ui32WeaponID);
	if (!pWeaponInfo)
		return HACK_TYPE_NONE;

	float fFireDelay = pWeaponInfo->fFireDelay;
	if (fFireDelay <= 0.0f)
		return HACK_TYPE_NONE;

	float fLastFire = pChara->GetLastFireTime();
	if (fLastFire <= 0.0f)
		return HACK_TYPE_NONE;

	float fElapsed = pFire->fPacketTime - fLastFire;

	if (fElapsed < fFireDelay * FIRE_SPEED_TOLERANCE)
	{
		pChara->IncFireCount();

		// Flag after multiple fast fires to reduce false positives from lag
		if (pChara->GetFireCount() >= 5)
			return HACK_TYPE_FIRE_SPEED;
	}
	else
	{
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

	for (uint8_t slot = 0; slot < WEAPON_SLOT_COUNT; slot++)
	{
		if (pChara->GetEquipWeaponID(slot) == ui32WeaponID)
			return HACK_TYPE_NONE;
	}

	return HACK_TYPE_WEAPON_SYNC;
}

HackType HMSParser::CheckWeaponParam(BattleRoom* pRoom, uint32_t ui32SlotIdx,
									   uint32_t ui32WeaponID, uint8_t ui8AttackType)
{
	if (!pRoom)
		return HACK_TYPE_NONE;

	if (ui8AttackType >= ATTACK_TYPE_MAX)
		return HACK_TYPE_WEAPON_PARAM;

	const WeaponDamageInfo* pInfo = WeaponTable::GetInstance().GetWeaponInfo(ui32WeaponID);
	if (!pInfo)
		return HACK_TYPE_WEAPON_PARAM;

	if (ui8AttackType != ATTACK_TYPE_NONE && pInfo->fDamage[ui8AttackType] <= 0.0f)
	{
		if (pInfo->fDamage[ATTACK_TYPE_L1] <= 0.0f)
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

	if (fDist > GHOST_MAX_DISTANCE)
	{
		// Ghost detected - enforce via violation tracking
		InitializeIfNeeded();
		ViolationRecord* pRecord = GetViolationRecord(ui32SlotIdx);
		if (pRecord)
		{
			pRecord->AddViolation(HACK_TYPE_GHOST);

			// Report if violations accumulate
			if (pRecord->ai32Counts[HACK_TYPE_GHOST] >= HMS_VIOLATION_THRESHOLD_MEDIUM)
			{
				HackCheckResult result;
				result.eType = HACK_TYPE_GHOST;
				result.eSeverity = HACK_SEVERITY_HIGH;
				result.ui32SlotIdx = ui32SlotIdx;
				snprintf(result.szDescription, sizeof(result.szDescription),
						 "Ghost: slot %u dist=%.0f count=%d",
						 ui32SlotIdx, fDist, pRecord->ai32Counts[HACK_TYPE_GHOST]);

				BattleMember* pMember = pRoom->GetMember(ui32SlotIdx);
				int64_t uid = pMember ? pMember->GetUID() : 0;
				EnforceHack(pRoom, &result, uid);
			}
		}
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

	SpeedState* pSpeed = pChara->GetSpeedState();
	if (!pSpeed)
		return HACK_TYPE_NONE;

	pSpeed->InsertSpeed(fSpeed);

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

	float fDist = Distance3D(fDeathPos0, fDeathPos1);
	return (fDist > RECALL_MAX_DISTANCE);
}

bool HMSParser::IsInvincibleTime(BattleRoom* pRoom, uint32_t ui32AttackerSlot,
								  uint32_t ui32TargetSlot)
{
	if (!pRoom)
		return false;

	// Check if target just respawned (spawn protection)
	BattleMember* pTarget = pRoom->GetMember(ui32TargetSlot);
	if (pTarget)
	{
		DWORD dwDeathTime = pTarget->GetDeathTime();
		if (dwDeathTime > 0)
		{
			DWORD dwElapsed = GetTickCount() - dwDeathTime;
			// 2 second spawn protection
			if (dwElapsed < 2000)
				return true;
		}
	}

	return false;
}

// ============================================================================
// Weapon Exploit Checks
// ============================================================================

HackType HMSParser::CheckNaturalRPGGet(BattleRoom* pRoom, uint32_t ui32SlotIdx,
										 uint32_t ui32Class, uint32_t ui32Number)
{
	if (!pRoom)
		return HACK_TYPE_NONE;

	uint8_t ui8Type = (uint8_t)ui32Class;

	if (ui8Type == WEAPON_CLASS_ROCKET_LAUNCHER)
	{
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

	const WeaponDamageInfo* pInfo = WeaponTable::GetInstance().GetWeaponInfo(ui32ItemID);
	if (!pInfo)
		return HACK_TYPE_WEAPON_SYNC;

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
				 "WeaponSync: slot %u weapon %u",
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
				 "WeaponParam: weapon %u attack %u",
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
				 "Range: slot %u weapon %u dist=%.0f",
				 pHitInfo->ui32AttackerSlot, pHitInfo->ui32WeaponID,
				 Distance3D(pHitInfo->fAttackerPos, pHitInfo->fTargetPos));
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
				 "FireSpeed: slot %u weapon %u",
				 pHitInfo->ui32AttackerSlot, pHitInfo->ui32WeaponID);
		return result;
	}

	// 5. Check recall hack
	if (pHitInfo->fDeathPos[0] != 0.0f || pHitInfo->fDeathPos[1] != 0.0f || pHitInfo->fDeathPos[2] != 0.0f)
	{
		if (CheckRecallHack(pRoom, pHitInfo->ui32TargetSlot,
							pHitInfo->fTargetPos, pHitInfo->fDeathPos))
		{
			result.eType = HACK_TYPE_RECALL;
			result.eSeverity = HACK_SEVERITY_HIGH;
			result.ui32SlotIdx = pHitInfo->ui32TargetSlot;
			snprintf(result.szDescription, sizeof(result.szDescription),
					 "Recall: slot %u death pos mismatch dist=%.0f",
					 pHitInfo->ui32TargetSlot,
					 Distance3D(pHitInfo->fTargetPos, pHitInfo->fDeathPos));
			return result;
		}
	}

	// 6. Check invincibility (spawn protection)
	if (IsInvincibleTime(pRoom, pHitInfo->ui32AttackerSlot, pHitInfo->ui32TargetSlot))
	{
		result.eType = HACK_TYPE_NONE;	// Not a hack, just ignore the hit
		return result;
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
				 "Ghost: slot %u moved %.0f in %.2fs",
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
				 "SpeedHack: slot %u avg=%.0f",
				 ui32SlotIdx, fAvg);
		return result;
	}

	return result;	// HACK_TYPE_NONE
}

// ============================================================================
// ENFORCEMENT - The key production feature
// ============================================================================

void HMSParser::EnforceHack(BattleRoom* pRoom, const HackCheckResult* pResult,
							int64_t i64UID)
{
	if (!pRoom || !pResult || pResult->eType == HACK_TYPE_NONE)
		return;

	InitializeIfNeeded();

	// Track violation
	EnterCriticalSection(&s_csViolations);
	ViolationRecord* pRecord = GetViolationRecord(pResult->ui32SlotIdx);
	if (pRecord)
	{
		pRecord->AddViolation(pResult->eType);

		// Escalate severity based on accumulated violations
		HackSeverity effectiveSeverity = pResult->eSeverity;
		DWORD dwNow = GetTickCount();
		DWORD dwWindow = dwNow - pRecord->dwFirstViolationTime;

		if (dwWindow < HMS_VIOLATION_WINDOW_MS)
		{
			if (pRecord->i32TotalViolations >= HMS_VIOLATION_THRESHOLD_CRITICAL)
				effectiveSeverity = HACK_SEVERITY_CRITICAL;
			else if (pRecord->i32TotalViolations >= HMS_VIOLATION_THRESHOLD_HIGH)
				effectiveSeverity = HACK_SEVERITY_HIGH;
			else if (pRecord->i32TotalViolations >= HMS_VIOLATION_THRESHOLD_MEDIUM)
				effectiveSeverity = HACK_SEVERITY_MEDIUM;
		}
		else
		{
			// Window expired - reset and use base severity
			pRecord->Reset();
			pRecord->AddViolation(pResult->eType);
		}

		LeaveCriticalSection(&s_csViolations);

		// Log to console
		printf("[HMS] HACK: %s (severity=%d, slot=%u, uid=%lld, total=%d)\n",
			pResult->szDescription, (int)effectiveSeverity,
			pResult->ui32SlotIdx, (long long)i64UID,
			pRecord->i32TotalViolations);

		// Send notification to GameServer via BattleSession
		if (effectiveSeverity >= HACK_SEVERITY_MEDIUM)
		{
			extern BattleSessionManager* g_pBattleSessionManager;
			if (g_pBattleSessionManager)
			{
				BattleSession* pSession = (BattleSession*)g_pBattleSessionManager->GetSession(
					pRoom->GetOwnerSessionIdx());
				if (pSession)
				{
					pSession->SendHackNotify(
						pRoom->GetGameServerRoomIdx(),
						pRoom->GetChannelNum(),
						pResult->ui32SlotIdx,
						i64UID,
						(uint8_t)pResult->eType,
						(uint8_t)effectiveSeverity,
						pResult->szDescription);
				}
			}
		}

		// Enforce based on severity
		if (effectiveSeverity >= HACK_SEVERITY_HIGH)
		{
			// Remove player from battle room
			pRoom->RemoveMember(pResult->ui32SlotIdx);
			printf("[HMS] REMOVED slot %u from room %d (severity=%d)\n",
				pResult->ui32SlotIdx, pRoom->GetRoomIdx(), (int)effectiveSeverity);
		}
	}
	else
	{
		LeaveCriticalSection(&s_csViolations);
	}
}
