#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"

// ============================================================================
// Event/Attendance Handlers (Protocol_Base 0x0200 attendance section)
// ============================================================================

void GameSession::OnAttendanceReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BASE_ATTENDANCE_REQ -> ACK
	// Mark attendance for today
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	int32_t result = 0;
	uint32_t rewardGP = 0;
	uint32_t rewardExp = 0;

	if (m_AttendanceData.bAttendedToday)
	{
		result = 1;		// Already attended today
	}
	else
	{
		// Mark today as attended
		int dayIdx = m_AttendanceData.i32TotalDays % MAX_ATTENDANCE_DAYS;
		m_AttendanceData.ui8Days[dayIdx] = ATTENDANCE_ATTENDED;
		m_AttendanceData.i32TotalDays++;
		m_AttendanceData.i32CurrentStreak++;
		m_AttendanceData.bAttendedToday = true;

		// Get reward
		GameAttendanceReward reward = m_AttendanceData.GetDailyReward(dayIdx);
		rewardGP = reward.ui32RewardGP;
		rewardExp = reward.ui32RewardExp;

		m_i32GP += rewardGP;
		m_i64Exp += rewardExp;

		printf("[GameSession] Attendance checked - UID=%lld, Day=%d, Streak=%d, GP+=%u, Exp+=%u\n",
			m_i64UID, m_AttendanceData.i32TotalDays, m_AttendanceData.i32CurrentStreak,
			rewardGP, rewardExp);
	}

	// Send ACK
	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_ATTENDANCE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Total days attended
	int32_t totalDays = m_AttendanceData.i32TotalDays;
	memcpy(buffer + offset, &totalDays, sizeof(int32_t));	offset += sizeof(int32_t);

	// Current streak
	int32_t streak = m_AttendanceData.i32CurrentStreak;
	memcpy(buffer + offset, &streak, sizeof(int32_t));		offset += sizeof(int32_t);

	// Reward info
	memcpy(buffer + offset, &rewardGP, sizeof(uint32_t));	offset += sizeof(uint32_t);
	memcpy(buffer + offset, &rewardExp, sizeof(uint32_t));	offset += sizeof(uint32_t);

	// Updated balance
	uint32_t gp = (uint32_t)m_i32GP;
	memcpy(buffer + offset, &gp, sizeof(uint32_t));			offset += sizeof(uint32_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnAttendanceClearItemReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BASE_ATTENDANCE_CLEAR_ITEM_REQ -> ACK
	// Claim weekly completion bonus
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	int32_t result = 0;
	uint32_t bonusGP = 0;

	if (m_AttendanceData.i32CurrentStreak < MAX_DAILY_REWARDS)
	{
		result = 1;		// Not enough consecutive days
	}
	else
	{
		// Grant weekly bonus
		bonusGP = 5000;
		m_i32GP += bonusGP;

		// Mark cycle as cleared
		int dayIdx = (m_AttendanceData.i32TotalDays - 1) % MAX_ATTENDANCE_DAYS;
		m_AttendanceData.ui8Days[dayIdx] = ATTENDANCE_CLEAR;

		printf("[GameSession] Attendance clear bonus - UID=%lld, BonusGP=%u\n",
			m_i64UID, bonusGP);
	}

	// Send ACK
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_ATTENDANCE_CLEAR_ITEM_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &bonusGP, sizeof(uint32_t));	offset += sizeof(uint32_t);

	uint32_t gp = (uint32_t)m_i32GP;
	memcpy(buffer + offset, &gp, sizeof(uint32_t));			offset += sizeof(uint32_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

// ============================================================================
// Attendance Login Check (Phase 14A)
// ============================================================================

void GameSession::CheckAttendanceOnLogin()
{
	// Check if it's a new day since last login and reset daily record
	time_t now = time(nullptr);
	struct tm tmNow;
	localtime_s(&tmNow, &now);
	uint32_t today = (uint32_t)((tmNow.tm_year + 1900) * 10000 + (tmNow.tm_mon + 1) * 100 + tmNow.tm_mday);

	if (m_AttendanceData.ui32LastAttendDate != today)
	{
		// New day - reset daily flag and record
		m_AttendanceData.bAttendedToday = false;
		m_DailyRecord.Reset();

		// Check if streak was broken (missed a day)
		if (m_AttendanceData.ui32LastAttendDate > 0)
		{
			uint32_t yesterday = today - 1;	// Simplified (doesn't handle month boundaries perfectly)
			if (m_AttendanceData.ui32LastAttendDate < yesterday)
			{
				// Streak broken - reset
				m_AttendanceData.i32CurrentStreak = 0;
				printf("[GameSession] Attendance streak broken - UID=%lld, LastDate=%u, Today=%u\n",
					m_i64UID, m_AttendanceData.ui32LastAttendDate, today);
			}
		}

		m_AttendanceData.ui32LastAttendDate = today;
	}

	// Send attendance status to client
	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_ATTENDANCE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = m_AttendanceData.bAttendedToday ? 1 : 0;	// 0 = not attended yet, 1 = already attended
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	int32_t totalDays = m_AttendanceData.i32TotalDays;
	memcpy(buffer + offset, &totalDays, sizeof(int32_t));	offset += sizeof(int32_t);

	int32_t streak = m_AttendanceData.i32CurrentStreak;
	memcpy(buffer + offset, &streak, sizeof(int32_t));		offset += sizeof(int32_t);

	// Days array for UI display
	uint8_t dayCount = (uint8_t)(totalDays < MAX_ATTENDANCE_DAYS ? totalDays : MAX_ATTENDANCE_DAYS);
	memcpy(buffer + offset, &dayCount, 1);					offset += 1;
	for (int i = 0; i < dayCount && offset < 120; i++)
	{
		memcpy(buffer + offset, &m_AttendanceData.ui8Days[i], 1);	offset += 1;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);

	printf("[GameSession] Attendance status sent - UID=%lld, TotalDays=%d, Streak=%d, AttendedToday=%d\n",
		m_i64UID, totalDays, streak, m_AttendanceData.bAttendedToday ? 1 : 0);
}

void GameSession::OnDailyRecordReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_BASE_DAILY_RECORD_REQ -> ACK
	// Send today's battle stats
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_DAILY_RECORD_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Daily stats
	memcpy(buffer + offset, &m_DailyRecord.i32Kills, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_DailyRecord.i32Deaths, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_DailyRecord.i32Wins, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_DailyRecord.i32Losses, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_DailyRecord.i32GamesPlayed, sizeof(int));offset += sizeof(int);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}
