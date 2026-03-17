#ifndef __ANTIADDICTION_H__
#define __ANTIADDICTION_H__

#pragma once

// ============================================================================
// Anti-Addiction System (AAS)
// ============================================================================
// Tracks playtime per session and daily totals.
// Warns at configurable thresholds (2h, 3h, 5h by default).
// Penalizes EXP/GP gain after thresholds.
// Sends popup notifications to client.
// Configurable per-locale (China requires strict AAS).
//
// Original references: AAS_OPTION_DATA, CATCH_DAILY_USAGE from ServerDef.h
// ============================================================================

class GameSession;

// AAS warning level
enum AASWarningLevel
{
	AAS_LEVEL_NONE = 0,			// Under threshold 1
	AAS_LEVEL_WARNING1,				// Over threshold 1 (default 2h)
	AAS_LEVEL_WARNING2,				// Over threshold 2 (default 3h)
	AAS_LEVEL_PENALTY,				// Over penalty threshold (default 5h) - rewards reduced
	AAS_LEVEL_COUNT,
};

// AAS notification types sent to client
enum AASNotifyType
{
	AAS_NOTIFY_NONE = 0,
	AAS_NOTIFY_WARNING1,			// "You have been playing for 2 hours"
	AAS_NOTIFY_WARNING2,			// "You have been playing for 3 hours"
	AAS_NOTIFY_PENALTY_START,		// "Rewards reduced - take a break"
	AAS_NOTIFY_PERIODIC_REMINDER,	// Periodic reminder while in penalty
};

// Configuration for AAS
struct AASConfig
{
	bool		bEnabled;
	int			i32WarnThreshold1_Min;		// 120 min = 2h
	int			i32WarnThreshold2_Min;		// 180 min = 3h
	int			i32PenaltyThreshold_Min;	// 300 min = 5h
	int			i32ExpPenaltyPercent;		// 50 = half exp after penalty threshold
	int			i32GPPenaltyPercent;		// 50 = half gp after penalty threshold
	int			i32ReminderInterval_Min;	// Reminder interval after penalty (default 30 min)
	bool		bForceLogout;				// Force logout after max time (China strict)
	int			i32ForceLogoutThreshold_Min;// Force logout threshold (default 0 = disabled)

	AASConfig()
		: bEnabled(false)
		, i32WarnThreshold1_Min(120)
		, i32WarnThreshold2_Min(180)
		, i32PenaltyThreshold_Min(300)
		, i32ExpPenaltyPercent(50)
		, i32GPPenaltyPercent(50)
		, i32ReminderInterval_Min(30)
		, bForceLogout(false)
		, i32ForceLogoutThreshold_Min(0)
	{
	}
};

// Per-player tracking data
struct AASPlayerData
{
	int64_t		i64UID;
	DWORD		dwSessionStartTime;			// GetTickCount() when player logged in
	int			i32SessionPlaytime_Min;		// Current session playtime in minutes
	int			i32DailyPlaytime_Min;		// Accumulated daily playtime in minutes
	AASWarningLevel eCurrentLevel;			// Current warning level
	DWORD		dwLastReminderTime;			// Last time a periodic reminder was sent
	bool		bActive;					// Is this slot in use

	void Reset()
	{
		i64UID = 0;
		dwSessionStartTime = 0;
		i32SessionPlaytime_Min = 0;
		i32DailyPlaytime_Min = 0;
		eCurrentLevel = AAS_LEVEL_NONE;
		dwLastReminderTime = 0;
		bActive = false;
	}
};

// Maximum tracked players (matches MAX_GAME_SESSIONS)
#define AAS_MAX_PLAYERS		1000

class AntiAddictionSystem
{
public:
	AntiAddictionSystem();
	~AntiAddictionSystem();

	// Initialize with config
	bool		Initialize(const AASConfig& config);

	// Called when player logs in - starts session timer
	void		OnPlayerLogin(int i32SessionIdx, int64_t i64UID, int i32PreviousDailyMin = 0);

	// Called when player logs out - finalizes session data
	void		OnPlayerLogout(int i32SessionIdx);

	// Periodic update - checks thresholds and sends warnings
	// Should be called from GameServerContext::OnUpdate
	void		Update();

	// Get EXP multiplier for a session (100-based, 100 = full, 50 = half)
	int			GetExpMultiplier(int i32SessionIdx) const;

	// Get GP multiplier for a session (100-based)
	int			GetGPMultiplier(int i32SessionIdx) const;

	// Get current warning level for a session
	AASWarningLevel GetWarningLevel(int i32SessionIdx) const;

	// Get session playtime in minutes
	int			GetSessionPlaytime(int i32SessionIdx) const;

	// Get daily accumulated playtime in minutes
	int			GetDailyPlaytime(int i32SessionIdx) const;

	// Check if a player should be force-disconnected
	bool		ShouldForceLogout(int i32SessionIdx) const;

	// Configuration
	const AASConfig& GetConfig() const { return m_Config; }
	void		SetConfig(const AASConfig& config) { m_Config = config; }
	bool		IsEnabled() const { return m_Config.bEnabled; }

private:
	// Send warning popup to a specific player
	void		SendWarningPopup(int i32SessionIdx, AASNotifyType eType);

	// Update a single player's state
	void		UpdatePlayer(int i32SessionIdx, DWORD dwNow);

	// Calculate warning level from playtime
	AASWarningLevel CalculateWarningLevel(int i32TotalMinutes) const;

private:
	AASConfig		m_Config;
	AASPlayerData	m_Players[AAS_MAX_PLAYERS];
	DWORD			m_dwLastUpdateTime;
};

extern AntiAddictionSystem* g_pAntiAddiction;

#endif // __ANTIADDICTION_H__
