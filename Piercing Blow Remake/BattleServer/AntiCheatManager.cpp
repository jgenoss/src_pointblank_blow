#include "pch.h"
#include "AntiCheatManager.h"
#include "BattleRoom.h"
#include "BattleSession.h"
#include "BattleMember.h"
#include "GameCharacter.h"
#include "ConfigXML.h"
#include <cmath>
#include <cstdio>

// ============================================================================
// ACPlayerState
// ============================================================================

void ACPlayerState::Reset()
{
	i32ViolationPoints = 0;
	i32ViolationCount = 0;
	memset(i32ViolationsByType, 0, sizeof(i32ViolationsByType));
	i32ViolationHead = 0;
	for (int i = 0; i < AC_MAX_VIOLATION_HISTORY; i++)
		Violations[i] = ACViolation();

	// Rate of fire
	dwLastFireTime = 0;
	ui32LastFireWeaponId = 0;
	i32RapidFireCount = 0;
	i32ShotCount = 0;
	dwShotWindowStart = 0;

	// Ammo
	memset(i32ExpectedLoadedAmmo, 0, sizeof(i32ExpectedLoadedAmmo));
	memset(i32ExpectedReserveAmmo, 0, sizeof(i32ExpectedReserveAmmo));
	bAmmoInitialized = false;
	i32InfiniteAmmoCount = 0;

	// Kill consistency
	i32RecentKills = 0;
	i32RecentDeaths = 0;
	dwKillWindowStart = 0;
	i32KillStreakCurrent = 0;
	i32KillStreakMax = 0;
	fKillsPerMinute = 0.0f;

	// Aimbot
	i32TotalHits = 0;
	i32HeadshotHits = 0;
	fHeadshotRatio = 0.0f;
	i32SnapAngleCount = 0;
	fLastAimAngle = 0.0f;
	dwLastHitTime = 0;
	i32ConsecutiveHeadshots = 0;
	i32HitAfterSnapCount = 0;

	// Memory
	ui32LastChecksum = 0;
	i32ChecksumFailCount = 0;
	dwLastChecksumTime = 0;

	// Session
	bFlagged = false;
	bKicked = false;
	eLastAction = AC_ACTION_NONE;
}

// ============================================================================
// ACThresholds
// ============================================================================

void ACThresholds::SetDefaults()
{
	// Escalation thresholds
	i32WarnThreshold = 50;
	i32KickThreshold = 150;
	i32BanThreshold = 300;

	// Point weights
	i32PointsLow = 5;
	i32PointsMedium = 15;
	i32PointsHigh = 40;
	i32PointsCritical = 100;

	// DamageValidator
	fDamageTolerancePercent = 20.0f;		// 20% over max allowed
	i32MaxDamageViolationsBeforeFlag = 3;

	// RateOfFireChecker
	fFireRateTolerancePercent = 30.0f;		// 30% faster than min interval allowed
	i32MaxRapidFireBeforeFlag = 5;			// 5 rapid fires before flag

	// AmmoTracker
	i32MaxInfiniteAmmoBeforeFlag = 5;		// 5 times ammo didn't decrease

	// KillConsistencyChecker
	fMaxKillsPerMinute = 8.0f;				// 8 KPM is extremely suspicious
	i32MaxKillStreak = 25;					// 25 kills without dying
	fMaxKDRatio = 15.0f;					// 15:1 K/D is suspicious with enough kills

	// AimbotDetector
	fMaxHeadshotRatio = 0.80f;				// 80% headshot ratio is very suspicious
	i32MinHitsForHSCheck = 20;				// Need 20+ hits before checking ratio
	fSnapAngleThreshold = 60.0f;			// 60 degree snap in one tick
	i32MaxSnapAngleCount = 8;				// 8 snap angles before flag
	i32MaxConsecutiveHeadshots = 10;		// 10 headshots in a row
	fSnapToHitTimeMs = 50.0f;				// Hit within 50ms of snap = suspicious

	// MemoryIntegrityChecker
	i32MaxChecksumFailures = 3;				// 3 consecutive failures
}

// ============================================================================
// AntiCheatManager - Constructor / Lifecycle
// ============================================================================

AntiCheatManager::AntiCheatManager()
	: m_pRoom(nullptr)
	, m_pSession(nullptr)
	, m_dwLastUpdateTime(0)
	, m_i32TotalViolations(0)
	, m_bEnabled(true)
{
	m_Thresholds.SetDefaults();

	for (int i = 0; i < BATTLE_SLOT_MAX; i++)
		m_PlayerStates[i].Reset();
}

AntiCheatManager::~AntiCheatManager()
{
}

void AntiCheatManager::Initialize(BattleRoom* pRoom, BattleSession* pSession)
{
	m_pRoom = pRoom;
	m_pSession = pSession;
	m_dwLastUpdateTime = GetTickCount();
	m_i32TotalViolations = 0;
	m_bEnabled = true;

	for (int i = 0; i < BATTLE_SLOT_MAX; i++)
		m_PlayerStates[i].Reset();

	printf("[AntiCheat] Initialized for room %d\n",
		pRoom ? pRoom->GetRoomIdx() : -1);
}

void AntiCheatManager::Reset()
{
	for (int i = 0; i < BATTLE_SLOT_MAX; i++)
		m_PlayerStates[i].Reset();

	m_dwLastUpdateTime = 0;
	m_i32TotalViolations = 0;
}

void AntiCheatManager::SetThresholds(const ACThresholds& thresholds)
{
	m_Thresholds = thresholds;
}

// ============================================================================
// DamageValidator
// ============================================================================

bool AntiCheatManager::ValidateDamage(uint32_t ui32ShooterSlot, uint32_t ui32TargetSlot,
									   uint32_t ui32WeaponId, int i32Damage,
									   bool bHeadshot, CharaHitPart ePart) const
{
	if (!m_bEnabled || ui32ShooterSlot >= BATTLE_SLOT_MAX)
		return true;

	// Zero or negative damage is always valid
	if (i32Damage <= 0)
		return true;

	// 1. Check damage against weapon table limits
	if (!CheckDamageRange(ui32WeaponId, i32Damage, bHeadshot))
	{
		char desc[64];
		snprintf(desc, sizeof(desc), "DMG %d > max for weapon 0x%08X hs=%d",
			i32Damage, ui32WeaponId, bHeadshot ? 1 : 0);

		// Use const_cast here: recording violations is a side effect of validation
		// that doesn't change the logical state of the validator
		const_cast<AntiCheatManager*>(this)->RecordViolation(
			ui32ShooterSlot, HACK_DAMAGE, SEVERITY_HIGH, desc);
		return false;
	}

	// 2. Check for impossible damage values (negative weapon, etc.)
	if (!CheckDamageConsistency(ui32ShooterSlot, i32Damage))
	{
		char desc[64];
		snprintf(desc, sizeof(desc), "Inconsistent DMG %d from slot %u",
			i32Damage, ui32ShooterSlot);

		const_cast<AntiCheatManager*>(this)->RecordViolation(
			ui32ShooterSlot, HACK_DAMAGE, SEVERITY_MEDIUM, desc);
		return false;
	}

	return true;
}

bool AntiCheatManager::CheckDamageRange(uint32_t ui32WeaponId, int i32Damage, bool bHeadshot) const
{
	const ACWeaponTable& table = ACWeaponTable::GetInstance();
	return table.IsDamageValid(ui32WeaponId, i32Damage, bHeadshot);
}

bool AntiCheatManager::CheckDamageConsistency(uint32_t ui32ShooterSlot, int i32Damage) const
{
	// Damage should never exceed max HP in a single non-explosive hit
	// PB max HP is typically 100-120 with equipment
	// Single-hit kills are possible with snipers (HS) but the per-weapon check handles that
	// This catches absurd values like 9999 damage
	if (i32Damage > 2000)
		return false;

	return true;
}

// ============================================================================
// RateOfFireChecker
// ============================================================================

bool AntiCheatManager::ValidateFireRate(uint32_t ui32Slot, uint32_t ui32WeaponId, DWORD dwNow)
{
	if (!m_bEnabled || ui32Slot >= BATTLE_SLOT_MAX)
		return true;

	ACPlayerState& state = m_PlayerStates[ui32Slot];

	// First shot - just record
	if (state.dwLastFireTime == 0)
	{
		state.dwLastFireTime = dwNow;
		state.ui32LastFireWeaponId = ui32WeaponId;
		state.dwShotWindowStart = dwNow;
		state.i32ShotCount = 1;
		return true;
	}

	// Calculate elapsed time since last shot
	int elapsedMs = (int)(dwNow - state.dwLastFireTime);

	// Weapon switch resets fire rate tracking
	if (ui32WeaponId != state.ui32LastFireWeaponId)
	{
		state.dwLastFireTime = dwNow;
		state.ui32LastFireWeaponId = ui32WeaponId;
		state.i32RapidFireCount = 0;
		return true;
	}

	// Check fire interval against weapon table
	if (!CheckFireInterval(ui32WeaponId, elapsedMs))
	{
		state.i32RapidFireCount++;

		if (state.i32RapidFireCount >= m_Thresholds.i32MaxRapidFireBeforeFlag)
		{
			char desc[64];
			snprintf(desc, sizeof(desc), "RapidFire x%d weapon=0x%08X interval=%dms",
				state.i32RapidFireCount, ui32WeaponId, elapsedMs);

			HackSeverity sev = (state.i32RapidFireCount >= m_Thresholds.i32MaxRapidFireBeforeFlag * 2)
				? SEVERITY_HIGH : SEVERITY_MEDIUM;

			RecordViolation(ui32Slot, HACK_RATE_OF_FIRE, sev, desc);

			// Reset counter after recording
			state.i32RapidFireCount = 0;

			state.dwLastFireTime = dwNow;
			return false;
		}
	}
	else
	{
		// Valid fire rate - decay rapid fire counter
		if (state.i32RapidFireCount > 0)
			state.i32RapidFireCount--;
	}

	state.dwLastFireTime = dwNow;

	// Track overall shot count for the window
	state.i32ShotCount++;

	// Reset window every 10 seconds
	if (dwNow - state.dwShotWindowStart > 10000)
	{
		state.i32ShotCount = 0;
		state.dwShotWindowStart = dwNow;
	}

	return true;
}

bool AntiCheatManager::CheckFireInterval(uint32_t ui32WeaponId, int i32ElapsedMs) const
{
	const ACWeaponTable& table = ACWeaponTable::GetInstance();
	return table.IsFireRateValid(ui32WeaponId, i32ElapsedMs);
}

// ============================================================================
// AmmoTracker
// ============================================================================

void AntiCheatManager::InitPlayerAmmo(uint32_t ui32Slot, uint8_t ui8WeaponSlot,
									   int i32Loaded, int i32Reserve)
{
	if (ui32Slot >= BATTLE_SLOT_MAX || ui8WeaponSlot >= WEAPON_SLOT_COUNT)
		return;

	ACPlayerState& state = m_PlayerStates[ui32Slot];
	state.i32ExpectedLoadedAmmo[ui8WeaponSlot] = i32Loaded;
	state.i32ExpectedReserveAmmo[ui8WeaponSlot] = i32Reserve;
	state.bAmmoInitialized = true;
}

void AntiCheatManager::OnAmmoConsumed(uint32_t ui32Slot, uint8_t ui8WeaponSlot, int i32Count)
{
	if (ui32Slot >= BATTLE_SLOT_MAX || ui8WeaponSlot >= WEAPON_SLOT_COUNT)
		return;

	ACPlayerState& state = m_PlayerStates[ui32Slot];
	if (!state.bAmmoInitialized)
		return;

	state.i32ExpectedLoadedAmmo[ui8WeaponSlot] -= i32Count;
	if (state.i32ExpectedLoadedAmmo[ui8WeaponSlot] < 0)
		state.i32ExpectedLoadedAmmo[ui8WeaponSlot] = 0;
}

void AntiCheatManager::OnAmmoReloaded(uint32_t ui32Slot, uint8_t ui8WeaponSlot,
										int i32Loaded, int i32Reserve)
{
	if (ui32Slot >= BATTLE_SLOT_MAX || ui8WeaponSlot >= WEAPON_SLOT_COUNT)
		return;

	ACPlayerState& state = m_PlayerStates[ui32Slot];
	state.i32ExpectedLoadedAmmo[ui8WeaponSlot] = i32Loaded;
	state.i32ExpectedReserveAmmo[ui8WeaponSlot] = i32Reserve;
}

bool AntiCheatManager::ValidateAmmo(uint32_t ui32Slot, uint8_t ui8WeaponSlot,
									 int i32Loaded, int i32Reserve)
{
	if (!m_bEnabled || ui32Slot >= BATTLE_SLOT_MAX || ui8WeaponSlot >= WEAPON_SLOT_COUNT)
		return true;

	ACPlayerState& state = m_PlayerStates[ui32Slot];
	if (!state.bAmmoInitialized)
		return true;

	// Get weapon ID from the room's character data
	uint32_t weaponId = 0;
	if (m_pRoom)
	{
		const GameCharacter* pChar = m_pRoom->GetCharacter(ui32Slot);
		if (pChar)
			weaponId = pChar->GetEquipWeaponID(ui8WeaponSlot);
	}

	// 1. Check ammo against weapon table maximums
	if (weaponId != 0)
	{
		const ACWeaponTable& table = ACWeaponTable::GetInstance();
		if (!table.IsAmmoValid(weaponId, i32Loaded, i32Reserve))
		{
			char desc[64];
			snprintf(desc, sizeof(desc), "AmmoExceed slot=%d loaded=%d reserve=%d weapon=0x%08X",
				ui8WeaponSlot, i32Loaded, i32Reserve, weaponId);

			RecordViolation(ui32Slot, HACK_AMMO, SEVERITY_HIGH, desc);
			return false;
		}
	}

	// 2. Check for infinite ammo: ammo should decrease after shots
	// Compare with expected values (which are updated by OnAmmoConsumed)
	int expectedLoaded = state.i32ExpectedLoadedAmmo[ui8WeaponSlot];

	// If player's loaded ammo is higher than our expected count AND we expected
	// it to decrease, this is suspicious
	if (expectedLoaded >= 0 && i32Loaded > expectedLoaded + 1)
	{
		// Allow a small tolerance for reload packets arriving out of order
		// But if this keeps happening, flag it
		state.i32InfiniteAmmoCount++;

		if (state.i32InfiniteAmmoCount >= m_Thresholds.i32MaxInfiniteAmmoBeforeFlag)
		{
			char desc[64];
			snprintf(desc, sizeof(desc), "InfiniteAmmo x%d slot=%d actual=%d expected=%d",
				state.i32InfiniteAmmoCount, ui8WeaponSlot, i32Loaded, expectedLoaded);

			RecordViolation(ui32Slot, HACK_AMMO, SEVERITY_HIGH, desc);
			state.i32InfiniteAmmoCount = 0;
			return false;
		}
	}
	else
	{
		// Ammo tracking is consistent, decay counter
		if (state.i32InfiniteAmmoCount > 0)
			state.i32InfiniteAmmoCount--;
	}

	// Update expected values to match reported (they may legitimately differ
	// due to reload or weapon pickup)
	state.i32ExpectedLoadedAmmo[ui8WeaponSlot] = i32Loaded;
	state.i32ExpectedReserveAmmo[ui8WeaponSlot] = i32Reserve;

	return true;
}

// ============================================================================
// KillConsistencyChecker
// ============================================================================

void AntiCheatManager::OnKill(uint32_t ui32KillerSlot, uint32_t ui32VictimSlot, DWORD dwNow)
{
	if (!m_bEnabled || ui32KillerSlot >= BATTLE_SLOT_MAX)
		return;

	ACPlayerState& state = m_PlayerStates[ui32KillerSlot];

	// Initialize kill window
	if (state.dwKillWindowStart == 0)
		state.dwKillWindowStart = dwNow;

	state.i32RecentKills++;
	state.i32KillStreakCurrent++;

	if (state.i32KillStreakCurrent > state.i32KillStreakMax)
		state.i32KillStreakMax = state.i32KillStreakCurrent;

	// Update KPM
	DWORD dwWindowMs = dwNow - state.dwKillWindowStart;
	if (dwWindowMs > 0)
	{
		float minutesElapsed = (float)dwWindowMs / 60000.0f;
		if (minutesElapsed > 0.1f)	// Need at least 6 seconds of data
			state.fKillsPerMinute = (float)state.i32RecentKills / minutesElapsed;
	}

	// Check kill rate anomalies
	if (!CheckKillRate(ui32KillerSlot, dwNow))
	{
		char desc[64];
		snprintf(desc, sizeof(desc), "KillRate %.1f KPM, %d kills in window",
			state.fKillsPerMinute, state.i32RecentKills);

		RecordViolation(ui32KillerSlot, HACK_KILL_ANOMALY, SEVERITY_MEDIUM, desc);
	}

	// Check kill streak
	if (!CheckKillStreak(ui32KillerSlot))
	{
		char desc[64];
		snprintf(desc, sizeof(desc), "KillStreak %d (max threshold %d)",
			state.i32KillStreakCurrent, m_Thresholds.i32MaxKillStreak);

		RecordViolation(ui32KillerSlot, HACK_KILL_ANOMALY, SEVERITY_MEDIUM, desc);
	}

	// Reset window every 2 minutes to keep data fresh
	if (dwWindowMs > 120000)
	{
		state.i32RecentKills = 0;
		state.dwKillWindowStart = dwNow;
	}
}

void AntiCheatManager::OnDeath(uint32_t ui32Slot, DWORD dwNow)
{
	if (ui32Slot >= BATTLE_SLOT_MAX)
		return;

	ACPlayerState& state = m_PlayerStates[ui32Slot];
	state.i32RecentDeaths++;
	state.i32KillStreakCurrent = 0;	// Kill streak broken
}

bool AntiCheatManager::CheckKillRate(uint32_t ui32Slot, DWORD dwNow) const
{
	if (ui32Slot >= BATTLE_SLOT_MAX)
		return true;

	const ACPlayerState& state = m_PlayerStates[ui32Slot];

	// Need at least 30 seconds of data
	DWORD dwWindowMs = dwNow - state.dwKillWindowStart;
	if (dwWindowMs < 30000)
		return true;

	// Check KPM
	if (state.fKillsPerMinute > m_Thresholds.fMaxKillsPerMinute)
		return false;

	return true;
}

bool AntiCheatManager::CheckKillStreak(uint32_t ui32Slot) const
{
	if (ui32Slot >= BATTLE_SLOT_MAX)
		return true;

	const ACPlayerState& state = m_PlayerStates[ui32Slot];
	return state.i32KillStreakCurrent <= m_Thresholds.i32MaxKillStreak;
}

// ============================================================================
// AimbotDetector
// ============================================================================

void AntiCheatManager::OnHitRegistered(uint32_t ui32ShooterSlot, bool bHeadshot,
										float fAimAngle, DWORD dwNow)
{
	if (!m_bEnabled || ui32ShooterSlot >= BATTLE_SLOT_MAX)
		return;

	ACPlayerState& state = m_PlayerStates[ui32ShooterSlot];

	state.i32TotalHits++;

	if (bHeadshot)
	{
		state.i32HeadshotHits++;
		state.i32ConsecutiveHeadshots++;
	}
	else
	{
		state.i32ConsecutiveHeadshots = 0;
	}

	// Update headshot ratio
	if (state.i32TotalHits > 0)
		state.fHeadshotRatio = (float)state.i32HeadshotHits / (float)state.i32TotalHits;

	// Check for snap-to-hit: if a large angle snap happened recently, and now we have a hit
	// This detects aimbot snap targeting
	if (state.dwLastHitTime > 0 && dwNow > 0)
	{
		// Check if there was a recent snap angle (within the snap-to-hit window)
		float angleDelta = fabsf(fAimAngle - state.fLastAimAngle);
		// Normalize angle delta to [0, 180]
		if (angleDelta > 180.0f)
			angleDelta = 360.0f - angleDelta;

		if (angleDelta > m_Thresholds.fSnapAngleThreshold)
		{
			DWORD dwSinceLastHit = dwNow - state.dwLastHitTime;
			if (dwSinceLastHit < (DWORD)m_Thresholds.fSnapToHitTimeMs)
			{
				state.i32HitAfterSnapCount++;
			}
		}
	}

	state.dwLastHitTime = dwNow;
	state.fLastAimAngle = fAimAngle;

	// Run aimbot checks
	if (!CheckHeadshotRatio(ui32ShooterSlot))
	{
		char desc[64];
		snprintf(desc, sizeof(desc), "HS ratio %.2f (%d/%d hits)",
			state.fHeadshotRatio, state.i32HeadshotHits, state.i32TotalHits);

		RecordViolation(ui32ShooterSlot, HACK_AIMBOT, SEVERITY_HIGH, desc);
	}

	if (!CheckConsecutiveHeadshots(ui32ShooterSlot))
	{
		char desc[64];
		snprintf(desc, sizeof(desc), "ConsecHS %d in a row",
			state.i32ConsecutiveHeadshots);

		RecordViolation(ui32ShooterSlot, HACK_AIMBOT, SEVERITY_HIGH, desc);
	}

	if (!CheckSnapAngles(ui32ShooterSlot))
	{
		char desc[64];
		snprintf(desc, sizeof(desc), "SnapAim x%d snap-hits x%d",
			state.i32SnapAngleCount, state.i32HitAfterSnapCount);

		RecordViolation(ui32ShooterSlot, HACK_AIMBOT, SEVERITY_HIGH, desc);
	}
}

void AntiCheatManager::OnAimUpdate(uint32_t ui32Slot, float fNewAimAngle, DWORD dwNow)
{
	if (ui32Slot >= BATTLE_SLOT_MAX)
		return;

	ACPlayerState& state = m_PlayerStates[ui32Slot];

	// Calculate angle delta
	float angleDelta = fabsf(fNewAimAngle - state.fLastAimAngle);
	if (angleDelta > 180.0f)
		angleDelta = 360.0f - angleDelta;

	// Detect snap angle (sudden large rotation)
	if (angleDelta > m_Thresholds.fSnapAngleThreshold)
	{
		state.i32SnapAngleCount++;
	}

	state.fLastAimAngle = fNewAimAngle;
}

bool AntiCheatManager::CheckHeadshotRatio(uint32_t ui32Slot) const
{
	if (ui32Slot >= BATTLE_SLOT_MAX)
		return true;

	const ACPlayerState& state = m_PlayerStates[ui32Slot];

	// Need minimum hits before checking ratio
	if (state.i32TotalHits < m_Thresholds.i32MinHitsForHSCheck)
		return true;

	return state.fHeadshotRatio <= m_Thresholds.fMaxHeadshotRatio;
}

bool AntiCheatManager::CheckSnapAngles(uint32_t ui32Slot) const
{
	if (ui32Slot >= BATTLE_SLOT_MAX)
		return true;

	const ACPlayerState& state = m_PlayerStates[ui32Slot];

	// Snap angles alone are suspicious but not conclusive
	// Snap angles + hits immediately after = very suspicious
	if (state.i32HitAfterSnapCount >= m_Thresholds.i32MaxSnapAngleCount)
		return false;

	return true;
}

bool AntiCheatManager::CheckConsecutiveHeadshots(uint32_t ui32Slot) const
{
	if (ui32Slot >= BATTLE_SLOT_MAX)
		return true;

	const ACPlayerState& state = m_PlayerStates[ui32Slot];
	return state.i32ConsecutiveHeadshots <= m_Thresholds.i32MaxConsecutiveHeadshots;
}

// ============================================================================
// MemoryIntegrityChecker
// ============================================================================

bool AntiCheatManager::ValidateChecksum(uint32_t ui32Slot, uint32_t ui32Checksum, DWORD dwNow)
{
	if (!m_bEnabled || ui32Slot >= BATTLE_SLOT_MAX)
		return true;

	ACPlayerState& state = m_PlayerStates[ui32Slot];

	// First checksum - accept and store as reference
	if (state.ui32LastChecksum == 0)
	{
		state.ui32LastChecksum = ui32Checksum;
		state.dwLastChecksumTime = dwNow;
		state.i32ChecksumFailCount = 0;
		return true;
	}

	// Checksum should match the expected value
	// The client computes a checksum over critical memory regions (weapon data,
	// player stats, game code segments). If the checksum changes unexpectedly,
	// the client's memory has been tampered with.
	//
	// NOTE: The checksum value itself is not validated against a server-computed
	// value (we don't have the client's memory). Instead, we check for:
	// 1. Checksum changing too frequently (memory being actively modified)
	// 2. Checksum becoming zero (hook/bypass of checksum routine)
	// 3. Checksum changing between checks when game state hasn't changed

	bool bValid = true;

	// Zero checksum = checksum routine bypassed
	if (ui32Checksum == 0)
	{
		state.i32ChecksumFailCount++;
		bValid = false;
	}
	// Checksum changed from last known value
	else if (ui32Checksum != state.ui32LastChecksum)
	{
		// How quickly did it change?
		DWORD dwElapsed = dwNow - state.dwLastChecksumTime;

		// Checksum changing very rapidly (< 5 seconds) suggests active memory patching
		if (dwElapsed < 5000 && state.dwLastChecksumTime > 0)
		{
			state.i32ChecksumFailCount++;
			bValid = false;
		}
		else
		{
			// Checksum changed but over a longer period - could be legitimate
			// (game updates, map transitions, etc.)
			// Accept the new checksum
			state.i32ChecksumFailCount = 0;
		}
	}
	else
	{
		// Checksum matches - good
		state.i32ChecksumFailCount = 0;
	}

	state.ui32LastChecksum = ui32Checksum;
	state.dwLastChecksumTime = dwNow;

	if (!bValid && state.i32ChecksumFailCount >= m_Thresholds.i32MaxChecksumFailures)
	{
		char desc[64];
		snprintf(desc, sizeof(desc), "MemIntegrity fail x%d checksum=0x%08X",
			state.i32ChecksumFailCount, ui32Checksum);

		RecordViolation(ui32Slot, HACK_MEMORY, SEVERITY_CRITICAL, desc);
		return false;
	}

	return true;
}

// ============================================================================
// Periodic Update
// ============================================================================

void AntiCheatManager::Update(DWORD dwNow)
{
	if (!m_bEnabled || !m_pRoom)
		return;

	// Don't update too frequently
	if (dwNow - m_dwLastUpdateTime < 1000)
		return;

	m_dwLastUpdateTime = dwNow;

	for (uint32_t slot = 0; slot < BATTLE_SLOT_MAX; slot++)
	{
		BattleMember* pMember = m_pRoom->GetMember((int)slot);
		if (!pMember || !pMember->IsMember())
			continue;

		// Skip already kicked players
		if (m_PlayerStates[slot].bKicked)
			continue;

		// Decay old violations
		DecayViolations(slot, dwNow);

		// Periodic kill rate check with enough data
		ACPlayerState& state = m_PlayerStates[slot];
		if (state.dwKillWindowStart > 0 && (dwNow - state.dwKillWindowStart) > 30000)
		{
			// Check K/D ratio if player has enough kills
			if (state.i32RecentKills > 10 && state.i32RecentDeaths > 0)
			{
				float kd = (float)state.i32RecentKills / (float)state.i32RecentDeaths;
				if (kd > m_Thresholds.fMaxKDRatio)
				{
					char desc[64];
					snprintf(desc, sizeof(desc), "KD ratio %.1f (%d/%d) in window",
						kd, state.i32RecentKills, state.i32RecentDeaths);

					RecordViolation(slot, HACK_KILL_ANOMALY, SEVERITY_MEDIUM, desc);
				}
			}
			// Player with many kills and zero deaths is also suspicious
			else if (state.i32RecentKills > 15 && state.i32RecentDeaths == 0)
			{
				char desc[64];
				snprintf(desc, sizeof(desc), "KD %d/0 in window", state.i32RecentKills);

				RecordViolation(slot, HACK_KILL_ANOMALY, SEVERITY_LOW, desc);
			}
		}
	}
}

// ============================================================================
// Violation Recording and Escalation
// ============================================================================

ACAction AntiCheatManager::RecordViolation(uint32_t ui32Slot, HackType eType,
											HackSeverity eSeverity, const char* pszDescription)
{
	if (ui32Slot >= BATTLE_SLOT_MAX)
		return AC_ACTION_NONE;

	ACPlayerState& state = m_PlayerStates[ui32Slot];

	// Already kicked - no point recording more
	if (state.bKicked)
		return AC_ACTION_NONE;

	// Record in circular buffer
	ACViolation& v = state.Violations[state.i32ViolationHead];
	v.eType = eType;
	v.eSeverity = eSeverity;
	v.dwTimestamp = GetTickCount();
	if (pszDescription)
		strncpy_s(v.szDescription, pszDescription, _TRUNCATE);
	else
		v.szDescription[0] = '\0';

	state.i32ViolationHead = (state.i32ViolationHead + 1) % AC_MAX_VIOLATION_HISTORY;

	// Update counters
	state.i32ViolationCount++;
	state.i32ViolationsByType[eType]++;
	state.i32ViolationPoints += GetSeverityPoints(eSeverity);
	m_i32TotalViolations++;

	// Log
	printf("[AntiCheat] Room=%d Slot=%u Type=%d Sev=%d Points=%d: %s\n",
		m_pRoom ? m_pRoom->GetRoomIdx() : -1,
		ui32Slot, (int)eType, (int)eSeverity,
		state.i32ViolationPoints,
		pszDescription ? pszDescription : "");

	// Determine and execute action
	ACAction action = DetermineAction(ui32Slot);
	state.eLastAction = action;

	if (action >= AC_ACTION_WARN)
	{
		SendHackNotify(ui32Slot, eType, eSeverity, pszDescription);
	}

	if (action >= AC_ACTION_KICK)
	{
		state.bKicked = true;
		state.bFlagged = true;
	}

	if (action == AC_ACTION_BAN)
	{
		state.bFlagged = true;
	}

	return action;
}

ACAction AntiCheatManager::DetermineAction(uint32_t ui32Slot) const
{
	if (ui32Slot >= BATTLE_SLOT_MAX)
		return AC_ACTION_NONE;

	const ACPlayerState& state = m_PlayerStates[ui32Slot];
	int points = state.i32ViolationPoints;

	if (points >= m_Thresholds.i32BanThreshold)
		return AC_ACTION_BAN;

	if (points >= m_Thresholds.i32KickThreshold)
		return AC_ACTION_KICK;

	if (points >= m_Thresholds.i32WarnThreshold)
		return AC_ACTION_WARN;

	// Even below warn threshold, log violations
	if (state.i32ViolationCount > 0)
		return AC_ACTION_LOG;

	return AC_ACTION_NONE;
}

void AntiCheatManager::SendHackNotify(uint32_t ui32Slot, HackType eType,
									   HackSeverity eSeverity, const char* pszDescription)
{
	if (!m_pSession || !m_pRoom)
		return;

	BattleMember* pMember = m_pRoom->GetMember((int)ui32Slot);
	if (!pMember || !pMember->IsMember())
		return;

	ACAction action = DetermineAction(ui32Slot);

	// Build description with action
	char fullDesc[128];
	const char* actionStr = "LOG";
	switch (action)
	{
	case AC_ACTION_WARN:	actionStr = "WARN"; break;
	case AC_ACTION_KICK:	actionStr = "KICK"; break;
	case AC_ACTION_BAN:		actionStr = "BAN"; break;
	default:				actionStr = "LOG"; break;
	}

	snprintf(fullDesc, sizeof(fullDesc), "[%s] %s",
		actionStr, pszDescription ? pszDescription : "");

	m_pSession->SendHackNotify(
		m_pRoom->GetGameServerRoomIdx(),
		m_pRoom->GetChannelNum(),
		ui32Slot,
		pMember->GetUID(),
		(uint8_t)eType,
		(uint8_t)eSeverity,
		fullDesc
	);
}

void AntiCheatManager::DecayViolations(uint32_t ui32Slot, DWORD dwNow)
{
	if (ui32Slot >= BATTLE_SLOT_MAX)
		return;

	ACPlayerState& state = m_PlayerStates[ui32Slot];

	// Decay 1 point per second, but never below zero
	// This is called once per second from Update()
	if (state.i32ViolationPoints > 0)
	{
		// Faster decay for low severity (1 point/sec)
		// Slower decay for accumulated high severity (still 1 point/sec but
		// high severity violations add more points)
		state.i32ViolationPoints--;
	}

	// Also decay rapid fire counter over time
	// (already handled per-shot, but this catches idle decay)
	if (state.i32RapidFireCount > 0 && (dwNow - state.dwLastFireTime) > 5000)
	{
		state.i32RapidFireCount = 0;
	}

	// Decay snap angle count over time
	if (state.i32SnapAngleCount > 0 && (dwNow - state.dwLastHitTime) > 30000)
	{
		state.i32SnapAngleCount = 0;
		state.i32HitAfterSnapCount = 0;
	}
}

int AntiCheatManager::GetSeverityPoints(HackSeverity eSeverity) const
{
	switch (eSeverity)
	{
	case SEVERITY_LOW:		return m_Thresholds.i32PointsLow;
	case SEVERITY_MEDIUM:	return m_Thresholds.i32PointsMedium;
	case SEVERITY_HIGH:		return m_Thresholds.i32PointsHigh;
	case SEVERITY_CRITICAL:	return m_Thresholds.i32PointsCritical;
	default:				return m_Thresholds.i32PointsLow;
	}
}

// ============================================================================
// Helper
// ============================================================================

const WeaponStats* AntiCheatManager::GetWeaponStatsForValidation(uint32_t ui32WeaponId) const
{
	const ACWeaponTable& table = ACWeaponTable::GetInstance();

	const WeaponStats* pStats = table.GetStats(ui32WeaponId);
	if (pStats)
		return pStats;

	// Fall back to class defaults
	ACWeaponClass wclass = ACWeaponTable::ResolveWeaponClass(ui32WeaponId);
	return table.GetClassDefaults((uint8_t)wclass);
}

// ============================================================================
// Player State Queries
// ============================================================================

const ACPlayerState* AntiCheatManager::GetPlayerState(uint32_t ui32Slot) const
{
	if (ui32Slot >= BATTLE_SLOT_MAX)
		return nullptr;

	return &m_PlayerStates[ui32Slot];
}

bool AntiCheatManager::IsPlayerFlagged(uint32_t ui32Slot) const
{
	if (ui32Slot >= BATTLE_SLOT_MAX)
		return false;

	return m_PlayerStates[ui32Slot].bFlagged;
}

bool AntiCheatManager::IsPlayerKicked(uint32_t ui32Slot) const
{
	if (ui32Slot >= BATTLE_SLOT_MAX)
		return false;

	return m_PlayerStates[ui32Slot].bKicked;
}

int AntiCheatManager::GetViolationPoints(uint32_t ui32Slot) const
{
	if (ui32Slot >= BATTLE_SLOT_MAX)
		return 0;

	return m_PlayerStates[ui32Slot].i32ViolationPoints;
}

void AntiCheatManager::ClearViolations(uint32_t ui32Slot)
{
	if (ui32Slot >= BATTLE_SLOT_MAX)
		return;

	m_PlayerStates[ui32Slot].Reset();
}
