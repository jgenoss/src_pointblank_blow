#ifndef __ANTICHEATMANAGER_H__
#define __ANTICHEATMANAGER_H__

#pragma once
#include "BattleDef.h"
#include "WeaponTable.h"

class BattleRoom;
class BattleMember;
class BattleSession;
class GameCharacter;
class ConfigXML;

// ============================================================================
// Anti-Cheat Enums
// ============================================================================

// Types of hacks detected (matches IS_BATTLE_HACK_NOTIFY.ui8HackType)
enum HackType
{
	HACK_SPEEDHACK = 0,
	HACK_WALLHACK,
	HACK_AIMBOT,
	HACK_DAMAGE,
	HACK_AMMO,
	HACK_RATE_OF_FIRE,
	HACK_TELEPORT,
	HACK_MEMORY,
	HACK_KILL_ANOMALY,
	HACK_TYPE_COUNT,
};

// Severity levels (matches IS_BATTLE_HACK_NOTIFY.ui8Severity)
enum HackSeverity
{
	SEVERITY_LOW = 0,		// Log only, minor anomaly
	SEVERITY_MEDIUM,		// Warn: accumulates toward kick
	SEVERITY_HIGH,			// Likely cheating: fast-track to kick
	SEVERITY_CRITICAL,		// Definite cheat: immediate action
};

// Action taken by the anti-cheat system
enum ACAction
{
	AC_ACTION_NONE = 0,		// Below threshold
	AC_ACTION_LOG,			// Log the violation
	AC_ACTION_WARN,			// Send warning to GameServer
	AC_ACTION_KICK,			// Request kick from GameServer
	AC_ACTION_BAN,			// Request ban from GameServer
};

// ============================================================================
// Per-player violation tracking
// ============================================================================

#define AC_MAX_VIOLATION_HISTORY	64		// Rolling history of violations
#define AC_VIOLATION_DECAY_MS		60000	// Violations decay after 60 seconds

struct ACViolation
{
	HackType	eType;
	HackSeverity eSeverity;
	DWORD		dwTimestamp;
	char		szDescription[64];

	ACViolation()
		: eType(HACK_SPEEDHACK)
		, eSeverity(SEVERITY_LOW)
		, dwTimestamp(0)
	{
		szDescription[0] = '\0';
	}
};

struct ACPlayerState
{
	// Violation tracking
	int			i32ViolationPoints;			// Weighted violation score
	int			i32ViolationCount;			// Total violation count this session
	int			i32ViolationsByType[HACK_TYPE_COUNT];
	ACViolation	Violations[AC_MAX_VIOLATION_HISTORY];
	int			i32ViolationHead;			// Circular buffer index

	// Rate of fire tracking
	DWORD		dwLastFireTime;				// Tick of last shot
	uint32_t	ui32LastFireWeaponId;		// Weapon used last shot
	int			i32RapidFireCount;			// Consecutive rapid fire detections
	int			i32ShotCount;				// Total shots in current check window
	DWORD		dwShotWindowStart;			// Start of current shot count window

	// Ammo tracking
	int			i32ExpectedLoadedAmmo[WEAPON_SLOT_COUNT];
	int			i32ExpectedReserveAmmo[WEAPON_SLOT_COUNT];
	bool		bAmmoInitialized;
	int			i32InfiniteAmmoCount;		// Times ammo didn't decrease when expected

	// Kill consistency tracking
	int			i32RecentKills;				// Kills in current window
	int			i32RecentDeaths;			// Deaths in current window
	DWORD		dwKillWindowStart;			// Start of kill tracking window
	int			i32KillStreakCurrent;		// Current unbroken kill streak
	int			i32KillStreakMax;			// Max kill streak this session
	float		fKillsPerMinute;			// Rolling KPM

	// Aimbot detection
	int			i32TotalHits;				// Total hits landed
	int			i32HeadshotHits;			// Headshot hits
	float		fHeadshotRatio;				// Running HS ratio
	int			i32SnapAngleCount;			// Detected snap-to-target rotations
	float		fLastAimAngle;				// Last known aim angle (yaw)
	DWORD		dwLastHitTime;				// Time of last registered hit
	int			i32ConsecutiveHeadshots;	// Unbroken HS streak
	int			i32HitAfterSnapCount;		// Hits immediately following large angle snap

	// Memory integrity
	uint32_t	ui32LastChecksum;			// Last client-reported checksum
	int			i32ChecksumFailCount;		// Consecutive checksum failures
	DWORD		dwLastChecksumTime;			// Time of last checksum check

	// Session state
	bool		bFlagged;					// Flagged for review
	bool		bKicked;					// Already kicked
	ACAction	eLastAction;				// Last escalation action taken

	void Reset();
};

// ============================================================================
// Anti-Cheat Thresholds (configurable)
// ============================================================================

struct ACThresholds
{
	// Violation point thresholds for escalation
	int		i32WarnThreshold;			// Points before warning
	int		i32KickThreshold;			// Points before kick
	int		i32BanThreshold;			// Points before ban request

	// Violation point weights by severity
	int		i32PointsLow;
	int		i32PointsMedium;
	int		i32PointsHigh;
	int		i32PointsCritical;

	// DamageValidator
	float	fDamageTolerancePercent;	// Extra % allowed over max damage
	int		i32MaxDamageViolationsBeforeFlag;

	// RateOfFireChecker
	float	fFireRateTolerancePercent;	// Tolerance below min fire interval
	int		i32MaxRapidFireBeforeFlag;

	// AmmoTracker
	int		i32MaxInfiniteAmmoBeforeFlag;

	// KillConsistencyChecker
	float	fMaxKillsPerMinute;			// Max plausible KPM
	int		i32MaxKillStreak;			// Suspiciously long kill streak
	float	fMaxKDRatio;				// Max plausible K/D for flag

	// AimbotDetector
	float	fMaxHeadshotRatio;			// Max plausible HS ratio (after N hits)
	int		i32MinHitsForHSCheck;		// Minimum hits before HS ratio check
	float	fSnapAngleThreshold;		// Degrees: angle change considered a "snap"
	int		i32MaxSnapAngleCount;		// Max snap angles before flag
	int		i32MaxConsecutiveHeadshots;	// Max consecutive HS before flag
	float	fSnapToHitTimeMs;			// Max ms between snap and hit for flag

	// MemoryIntegrityChecker
	int		i32MaxChecksumFailures;		// Consecutive failures before flag

	void SetDefaults();
};

// ============================================================================
// AntiCheatManager - Central coordinator for all anti-cheat subsystems
// Created per BattleRoom. Tracks violations per player (slot).
// Escalation path: log -> warn -> kick -> ban
// ============================================================================

class AntiCheatManager
{
public:
	AntiCheatManager();
	~AntiCheatManager();

	// Lifecycle (called from BattleRoom)
	void	Initialize(BattleRoom* pRoom, BattleSession* pSession);
	void	Reset();

	// Set custom thresholds (from config)
	void	SetThresholds(const ACThresholds& thresholds);
	const ACThresholds& GetThresholds() const		{ return m_Thresholds; }

	// ===== Subsystem entry points =====
	// Each returns true if the event is VALID (allowed), false if INVALID (hack detected)

	// DamageValidator: called when processing a hit with damage
	bool	ValidateDamage(uint32_t ui32ShooterSlot, uint32_t ui32TargetSlot,
						   uint32_t ui32WeaponId, int i32Damage,
						   bool bHeadshot, CharaHitPart ePart) const;

	// RateOfFireChecker: called when a shot is fired
	bool	ValidateFireRate(uint32_t ui32Slot, uint32_t ui32WeaponId, DWORD dwNow);

	// AmmoTracker: called to validate ammo counts
	bool	ValidateAmmo(uint32_t ui32Slot, uint8_t ui8WeaponSlot,
						 int i32Loaded, int i32Reserve);

	// AmmoTracker: call when ammo is consumed (shot fired)
	void	OnAmmoConsumed(uint32_t ui32Slot, uint8_t ui8WeaponSlot, int i32Count);

	// AmmoTracker: call when ammo is reloaded
	void	OnAmmoReloaded(uint32_t ui32Slot, uint8_t ui8WeaponSlot,
						   int i32Loaded, int i32Reserve);

	// AmmoTracker: initialize expected ammo for a slot
	void	InitPlayerAmmo(uint32_t ui32Slot, uint8_t ui8WeaponSlot,
						   int i32Loaded, int i32Reserve);

	// KillConsistencyChecker: called when a kill is registered
	void	OnKill(uint32_t ui32KillerSlot, uint32_t ui32VictimSlot, DWORD dwNow);

	// KillConsistencyChecker: called when a death is registered
	void	OnDeath(uint32_t ui32Slot, DWORD dwNow);

	// AimbotDetector: called when a hit is registered
	void	OnHitRegistered(uint32_t ui32ShooterSlot, bool bHeadshot,
							float fAimAngle, DWORD dwNow);

	// AimbotDetector: called when aim angle changes significantly
	void	OnAimUpdate(uint32_t ui32Slot, float fNewAimAngle, DWORD dwNow);

	// MemoryIntegrityChecker: called with client-reported checksum
	bool	ValidateChecksum(uint32_t ui32Slot, uint32_t ui32Checksum, DWORD dwNow);

	// ===== Periodic update =====
	// Called from BattleRoom::Update() to check ongoing anomalies
	void	Update(DWORD dwNow);

	// ===== Player state queries =====
	const ACPlayerState*	GetPlayerState(uint32_t ui32Slot) const;
	bool					IsPlayerFlagged(uint32_t ui32Slot) const;
	bool					IsPlayerKicked(uint32_t ui32Slot) const;
	int						GetViolationPoints(uint32_t ui32Slot) const;

	// ===== Manual actions =====
	void	ClearViolations(uint32_t ui32Slot);

private:
	// Record a violation and escalate if thresholds exceeded
	ACAction	RecordViolation(uint32_t ui32Slot, HackType eType,
								HackSeverity eSeverity, const char* pszDescription);

	// Determine action based on current violation score
	ACAction	DetermineAction(uint32_t ui32Slot) const;

	// Send hack notification to GameServer via BattleSession
	void		SendHackNotify(uint32_t ui32Slot, HackType eType,
							   HackSeverity eSeverity, const char* pszDescription);

	// Decay old violations (time-based)
	void		DecayViolations(uint32_t ui32Slot, DWORD dwNow);

	// Severity point value
	int			GetSeverityPoints(HackSeverity eSeverity) const;

	// ===== Internal subsystem checks (const where possible) =====

	// DamageValidator internals
	bool		CheckDamageRange(uint32_t ui32WeaponId, int i32Damage, bool bHeadshot) const;
	bool		CheckDamageConsistency(uint32_t ui32ShooterSlot, int i32Damage) const;

	// RateOfFire internals
	bool		CheckFireInterval(uint32_t ui32WeaponId, int i32ElapsedMs) const;

	// KillConsistency internals
	bool		CheckKillRate(uint32_t ui32Slot, DWORD dwNow) const;
	bool		CheckKillStreak(uint32_t ui32Slot) const;

	// Aimbot internals
	bool		CheckHeadshotRatio(uint32_t ui32Slot) const;
	bool		CheckSnapAngles(uint32_t ui32Slot) const;
	bool		CheckConsecutiveHeadshots(uint32_t ui32Slot) const;

	// Helper: get weapon stats (from ACWeaponTable or class defaults)
	const WeaponStats*	GetWeaponStatsForValidation(uint32_t ui32WeaponId) const;

private:
	BattleRoom*		m_pRoom;			// Owning room (not owned)
	BattleSession*	m_pSession;			// Session for sending notifications (not owned)
	ACThresholds	m_Thresholds;		// Configurable thresholds

	// Per-slot player anti-cheat state
	ACPlayerState	m_PlayerStates[BATTLE_SLOT_MAX];

	// Global stats
	DWORD			m_dwLastUpdateTime;
	int				m_i32TotalViolations;
	bool			m_bEnabled;			// Master enable/disable
};

#endif // __ANTICHEATMANAGER_H__
