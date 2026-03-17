#include "pch.h"
#include "AntiAddiction.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "GameProtocol.h"
#include "ServerLog.h"

// ============================================================================
// Anti-Addiction System Implementation
// ============================================================================
// Tracks per-session and daily playtime. Sends warnings at configurable
// thresholds and reduces EXP/GP rewards after the penalty threshold.
//
// Original: AAS_OPTION_DATA / CATCH_DAILY_USAGE from ServerDef.h
// ============================================================================

AntiAddictionSystem* g_pAntiAddiction = nullptr;

// Protocol for AAS popup notification to client
// Uses AUTH_FCM range from original ProtocolDef.h
#define PROTOCOL_AAS_NOTIFY_ACK		0x0F10

#pragma pack(push, 1)
struct AAS_NOTIFY_PACKET
{
	uint16_t	ui16Size;
	uint16_t	ui16Protocol;
	int32_t		i32Result;				// 0 = success
	uint8_t		ui8NotifyType;			// AASNotifyType
	int32_t		i32PlaytimeMin;			// Total playtime in minutes
	int32_t		i32ExpMultiplier;		// Current EXP multiplier (100-based)
	int32_t		i32GPMultiplier;		// Current GP multiplier (100-based)
};
#pragma pack(pop)

// ============================================================================
// Constructor / Destructor
// ============================================================================

AntiAddictionSystem::AntiAddictionSystem()
	: m_dwLastUpdateTime(0)
{
	for (int i = 0; i < AAS_MAX_PLAYERS; ++i)
		m_Players[i].Reset();
}

AntiAddictionSystem::~AntiAddictionSystem()
{
}

// ============================================================================
// Initialize
// ============================================================================

bool AntiAddictionSystem::Initialize(const AASConfig& config)
{
	m_Config = config;
	m_dwLastUpdateTime = GetTickCount();

	printf("[AAS] Anti-Addiction System initialized: Enabled=%s, Warn1=%dm, Warn2=%dm, Penalty=%dm\n",
		m_Config.bEnabled ? "YES" : "NO",
		m_Config.i32WarnThreshold1_Min,
		m_Config.i32WarnThreshold2_Min,
		m_Config.i32PenaltyThreshold_Min);

	if (m_Config.bForceLogout)
	{
		printf("[AAS] Force logout enabled at %d minutes\n",
			m_Config.i32ForceLogoutThreshold_Min);
	}

	return true;
}

// ============================================================================
// OnPlayerLogin
// ============================================================================

void AntiAddictionSystem::OnPlayerLogin(int i32SessionIdx, int64_t i64UID, int i32PreviousDailyMin)
{
	if (!m_Config.bEnabled)
		return;

	if (i32SessionIdx < 0 || i32SessionIdx >= AAS_MAX_PLAYERS)
		return;

	AASPlayerData& data = m_Players[i32SessionIdx];
	data.i64UID = i64UID;
	data.dwSessionStartTime = GetTickCount();
	data.i32SessionPlaytime_Min = 0;
	data.i32DailyPlaytime_Min = i32PreviousDailyMin;	// Carried over from DB
	data.eCurrentLevel = CalculateWarningLevel(i32PreviousDailyMin);
	data.dwLastReminderTime = GetTickCount();
	data.bActive = true;

	SLOG_INFO("[AAS] Player UID=%lld logged in, previous daily=%dm, level=%d",
		i64UID, i32PreviousDailyMin, (int)data.eCurrentLevel);

	// If already in penalty from previous session, notify immediately
	if (data.eCurrentLevel >= AAS_LEVEL_PENALTY)
	{
		SendWarningPopup(i32SessionIdx, AAS_NOTIFY_PENALTY_START);
	}
	else if (data.eCurrentLevel >= AAS_LEVEL_WARNING2)
	{
		SendWarningPopup(i32SessionIdx, AAS_NOTIFY_WARNING2);
	}
	else if (data.eCurrentLevel >= AAS_LEVEL_WARNING1)
	{
		SendWarningPopup(i32SessionIdx, AAS_NOTIFY_WARNING1);
	}
}

// ============================================================================
// OnPlayerLogout
// ============================================================================

void AntiAddictionSystem::OnPlayerLogout(int i32SessionIdx)
{
	if (!m_Config.bEnabled)
		return;

	if (i32SessionIdx < 0 || i32SessionIdx >= AAS_MAX_PLAYERS)
		return;

	AASPlayerData& data = m_Players[i32SessionIdx];
	if (!data.bActive)
		return;

	// Calculate final session playtime
	DWORD dwNow = GetTickCount();
	DWORD dwElapsed = dwNow - data.dwSessionStartTime;
	data.i32SessionPlaytime_Min = (int)(dwElapsed / 60000);

	SLOG_INFO("[AAS] Player UID=%lld logged out, session=%dm, daily=%dm",
		data.i64UID, data.i32SessionPlaytime_Min,
		data.i32DailyPlaytime_Min + data.i32SessionPlaytime_Min);

	data.Reset();
}

// ============================================================================
// Update - called periodically from server update loop
// ============================================================================

void AntiAddictionSystem::Update()
{
	if (!m_Config.bEnabled)
		return;

	DWORD dwNow = GetTickCount();

	// Update every 60 seconds (1 minute granularity)
	if (dwNow - m_dwLastUpdateTime < 60000)
		return;

	m_dwLastUpdateTime = dwNow;

	for (int i = 0; i < AAS_MAX_PLAYERS; ++i)
	{
		if (m_Players[i].bActive)
		{
			UpdatePlayer(i, dwNow);
		}
	}
}

// ============================================================================
// UpdatePlayer - update single player state
// ============================================================================

void AntiAddictionSystem::UpdatePlayer(int i32SessionIdx, DWORD dwNow)
{
	AASPlayerData& data = m_Players[i32SessionIdx];

	// Calculate current session playtime
	DWORD dwElapsed = dwNow - data.dwSessionStartTime;
	data.i32SessionPlaytime_Min = (int)(dwElapsed / 60000);

	// Total playtime = daily accumulated + current session
	int i32TotalMin = data.i32DailyPlaytime_Min + data.i32SessionPlaytime_Min;

	// Check for level transitions
	AASWarningLevel eNewLevel = CalculateWarningLevel(i32TotalMin);

	if (eNewLevel != data.eCurrentLevel)
	{
		AASWarningLevel eOldLevel = data.eCurrentLevel;
		data.eCurrentLevel = eNewLevel;

		// Send notification for level transitions
		if (eNewLevel == AAS_LEVEL_WARNING1 && eOldLevel < AAS_LEVEL_WARNING1)
		{
			SendWarningPopup(i32SessionIdx, AAS_NOTIFY_WARNING1);
			SLOG_INFO("[AAS] Player UID=%lld reached warning 1 (%dm)",
				data.i64UID, i32TotalMin);
		}
		else if (eNewLevel == AAS_LEVEL_WARNING2 && eOldLevel < AAS_LEVEL_WARNING2)
		{
			SendWarningPopup(i32SessionIdx, AAS_NOTIFY_WARNING2);
			SLOG_INFO("[AAS] Player UID=%lld reached warning 2 (%dm)",
				data.i64UID, i32TotalMin);
		}
		else if (eNewLevel == AAS_LEVEL_PENALTY && eOldLevel < AAS_LEVEL_PENALTY)
		{
			SendWarningPopup(i32SessionIdx, AAS_NOTIFY_PENALTY_START);
			SLOG_INFO("[AAS] Player UID=%lld reached penalty threshold (%dm)",
				data.i64UID, i32TotalMin);
		}
	}

	// Periodic reminder while in penalty state
	if (data.eCurrentLevel >= AAS_LEVEL_PENALTY)
	{
		DWORD dwReminderInterval = (DWORD)(m_Config.i32ReminderInterval_Min * 60000);
		if (dwNow - data.dwLastReminderTime >= dwReminderInterval)
		{
			SendWarningPopup(i32SessionIdx, AAS_NOTIFY_PERIODIC_REMINDER);
			data.dwLastReminderTime = dwNow;
		}
	}

	// Force logout check (for strict locales like China)
	if (m_Config.bForceLogout && m_Config.i32ForceLogoutThreshold_Min > 0)
	{
		if (i32TotalMin >= m_Config.i32ForceLogoutThreshold_Min)
		{
			// The actual disconnect is handled by the caller checking ShouldForceLogout()
			SLOG_INFO("[AAS] Player UID=%lld reached force logout threshold (%dm)",
				data.i64UID, i32TotalMin);
		}
	}
}

// ============================================================================
// GetExpMultiplier / GetGPMultiplier
// ============================================================================

int AntiAddictionSystem::GetExpMultiplier(int i32SessionIdx) const
{
	if (!m_Config.bEnabled)
		return 100;

	if (i32SessionIdx < 0 || i32SessionIdx >= AAS_MAX_PLAYERS)
		return 100;

	const AASPlayerData& data = m_Players[i32SessionIdx];
	if (!data.bActive)
		return 100;

	if (data.eCurrentLevel >= AAS_LEVEL_PENALTY)
		return m_Config.i32ExpPenaltyPercent;

	return 100;
}

int AntiAddictionSystem::GetGPMultiplier(int i32SessionIdx) const
{
	if (!m_Config.bEnabled)
		return 100;

	if (i32SessionIdx < 0 || i32SessionIdx >= AAS_MAX_PLAYERS)
		return 100;

	const AASPlayerData& data = m_Players[i32SessionIdx];
	if (!data.bActive)
		return 100;

	if (data.eCurrentLevel >= AAS_LEVEL_PENALTY)
		return m_Config.i32GPPenaltyPercent;

	return 100;
}

// ============================================================================
// GetWarningLevel / GetSessionPlaytime / GetDailyPlaytime
// ============================================================================

AASWarningLevel AntiAddictionSystem::GetWarningLevel(int i32SessionIdx) const
{
	if (i32SessionIdx < 0 || i32SessionIdx >= AAS_MAX_PLAYERS)
		return AAS_LEVEL_NONE;

	if (!m_Players[i32SessionIdx].bActive)
		return AAS_LEVEL_NONE;

	return m_Players[i32SessionIdx].eCurrentLevel;
}

int AntiAddictionSystem::GetSessionPlaytime(int i32SessionIdx) const
{
	if (i32SessionIdx < 0 || i32SessionIdx >= AAS_MAX_PLAYERS)
		return 0;

	if (!m_Players[i32SessionIdx].bActive)
		return 0;

	return m_Players[i32SessionIdx].i32SessionPlaytime_Min;
}

int AntiAddictionSystem::GetDailyPlaytime(int i32SessionIdx) const
{
	if (i32SessionIdx < 0 || i32SessionIdx >= AAS_MAX_PLAYERS)
		return 0;

	const AASPlayerData& data = m_Players[i32SessionIdx];
	if (!data.bActive)
		return 0;

	return data.i32DailyPlaytime_Min + data.i32SessionPlaytime_Min;
}

// ============================================================================
// ShouldForceLogout
// ============================================================================

bool AntiAddictionSystem::ShouldForceLogout(int i32SessionIdx) const
{
	if (!m_Config.bEnabled || !m_Config.bForceLogout)
		return false;

	if (m_Config.i32ForceLogoutThreshold_Min <= 0)
		return false;

	if (i32SessionIdx < 0 || i32SessionIdx >= AAS_MAX_PLAYERS)
		return false;

	const AASPlayerData& data = m_Players[i32SessionIdx];
	if (!data.bActive)
		return false;

	int i32TotalMin = data.i32DailyPlaytime_Min + data.i32SessionPlaytime_Min;
	return (i32TotalMin >= m_Config.i32ForceLogoutThreshold_Min);
}

// ============================================================================
// SendWarningPopup
// ============================================================================

void AntiAddictionSystem::SendWarningPopup(int i32SessionIdx, AASNotifyType eType)
{
	if (!g_pGameSessionManager)
		return;

	GameSession* pSession = g_pGameSessionManager->GetSession(i32SessionIdx);
	if (!pSession)
		return;

	AAS_NOTIFY_PACKET pkt;
	memset(&pkt, 0, sizeof(pkt));
	pkt.ui16Size = sizeof(AAS_NOTIFY_PACKET);
	pkt.ui16Protocol = PROTOCOL_AAS_NOTIFY_ACK;
	pkt.i32Result = 0;
	pkt.ui8NotifyType = (uint8_t)eType;
	pkt.i32PlaytimeMin = GetDailyPlaytime(i32SessionIdx);
	pkt.i32ExpMultiplier = GetExpMultiplier(i32SessionIdx);
	pkt.i32GPMultiplier = GetGPMultiplier(i32SessionIdx);

	pSession->SendGamePacket((char*)&pkt, sizeof(pkt));
}

// ============================================================================
// CalculateWarningLevel
// ============================================================================

AASWarningLevel AntiAddictionSystem::CalculateWarningLevel(int i32TotalMinutes) const
{
	if (i32TotalMinutes >= m_Config.i32PenaltyThreshold_Min)
		return AAS_LEVEL_PENALTY;
	if (i32TotalMinutes >= m_Config.i32WarnThreshold2_Min)
		return AAS_LEVEL_WARNING2;
	if (i32TotalMinutes >= m_Config.i32WarnThreshold1_Min)
		return AAS_LEVEL_WARNING1;
	return AAS_LEVEL_NONE;
}
