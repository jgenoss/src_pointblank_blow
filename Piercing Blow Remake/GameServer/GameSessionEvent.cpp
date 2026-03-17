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
	uint32_t rewardItemId = 0;

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

		// Get reward based on current streak
		GameAttendanceReward reward = m_AttendanceData.GetDailyReward(m_AttendanceData.i32CurrentStreak - 1);
		rewardGP = reward.ui32RewardGP;
		rewardExp = reward.ui32RewardExp;
		rewardItemId = reward.ui32RewardItemId;

		m_i32GP += rewardGP;
		m_i64Exp += rewardExp;

		// Grant item reward if any
		if (rewardItemId != 0 && m_i32InventoryCount < MAX_INVEN_COUNT)
		{
			GameInventoryItem newItem;
			newItem.ui32ItemDBIdx = (uint32_t)(GetTickCount() & 0xFFFFFF) | ((uint32_t)m_i32InventoryCount << 24);
			newItem.ui32ItemId = rewardItemId;
			newItem.ui8ItemType = ITEM_ATTR_NORMAL;
			newItem.ui32ItemArg = 0;

			// Period-based items (boost cards) get 1-day or 3-day expiration
			int itemType = GET_ITEM_TYPE(rewardItemId);
			if (itemType == ITEM_TYPE_MAINTENANCE)
			{
				newItem.ui8ItemType = ITEM_ATTR_PERIOD;
				// Day 7 = 1-day boost, Day 14 = 3-day boost
				uint32_t durationSec = (rewardItemId == ATTENDANCE_ITEM_DAY7) ? 86400 : 86400 * 3;
				newItem.ui32ItemArg = (uint32_t)time(nullptr) + durationSec;
			}

			AddInventoryItem(newItem);

			printf("[GameSession] Attendance item reward - UID=%lld, ItemId=0x%08X, Streak=%d\n",
				m_i64UID, rewardItemId, m_AttendanceData.i32CurrentStreak);
		}

		printf("[GameSession] Attendance checked - UID=%lld, Day=%d, Streak=%d, GP+=%u, Exp+=%u\n",
			m_i64UID, m_AttendanceData.i32TotalDays, m_AttendanceData.i32CurrentStreak,
			rewardGP, rewardExp);
	}

	// Send ACK
	i3NetworkPacket packet;
	offset += sizeof(uint16_t);


	// Total days attended
	int32_t totalDays = m_AttendanceData.i32TotalDays;

	// Current streak
	int32_t streak = m_AttendanceData.i32CurrentStreak;

	// Reward info

	// Updated balance
	uint32_t gp = (uint32_t)m_i32GP;
	i3NetworkPacket packet(PROTOCOL_BASE_ATTENDANCE_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&totalDays, sizeof(int32_t));
	packet.WriteData(&streak, sizeof(int32_t));
	packet.WriteData(&rewardGP, sizeof(uint32_t));
	packet.WriteData(&rewardExp, sizeof(uint32_t));
	packet.WriteData(&rewardItemId, sizeof(uint32_t));
	packet.WriteData(&gp, sizeof(uint32_t));
	SendPacketMessage(&packet);
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
	offset += sizeof(uint16_t);

	uint32_t gp = (uint32_t)m_i32GP;
	i3NetworkPacket packet(PROTOCOL_BASE_ATTENDANCE_CLEAR_ITEM_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&bonusGP, sizeof(uint32_t));
	packet.WriteData(&gp, sizeof(uint32_t));
	SendPacketMessage(&packet);
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

	// Monthly reset check - new month clears all attendance data
	if (m_AttendanceData.ShouldMonthlyReset(today))
	{
		printf("[GameSession] Attendance monthly reset - UID=%lld, LastDate=%u, Today=%u\n",
			m_i64UID, m_AttendanceData.ui32LastAttendDate, today);
		m_AttendanceData.MonthlyReset();
	}

	if (m_AttendanceData.ui32LastAttendDate != today)
	{
		// New day - reset daily flag and record
		m_AttendanceData.bAttendedToday = false;
		m_DailyRecord.Reset();

		// Check if streak was broken (missed a day)
		if (m_AttendanceData.ui32LastAttendDate > 0)
		{
			// Proper day difference check using tm struct
			uint32_t lastY = m_AttendanceData.ui32LastAttendDate / 10000;
			uint32_t lastM = (m_AttendanceData.ui32LastAttendDate / 100) % 100;
			uint32_t lastD = m_AttendanceData.ui32LastAttendDate % 100;
			uint32_t curY = today / 10000;
			uint32_t curM = (today / 100) % 100;
			uint32_t curD = today % 100;

			// Simple gap check: if more than 1 calendar day apart, streak broken
			bool isConsecutive = false;
			if (curY == lastY && curM == lastM && curD == lastD + 1)
				isConsecutive = true;
			else if (curY == lastY && curM == lastM + 1 && curD == 1)
				isConsecutive = true;	// First of next month (simplified)
			else if (curY == lastY + 1 && curM == 1 && lastM == 12 && curD == 1)
				isConsecutive = true;	// Jan 1 after Dec 31

			if (!isConsecutive)
			{
				m_AttendanceData.i32CurrentStreak = 0;
				printf("[GameSession] Attendance streak broken - UID=%lld, LastDate=%u, Today=%u\n",
					m_i64UID, m_AttendanceData.ui32LastAttendDate, today);
			}
		}

		m_AttendanceData.ui32LastAttendDate = today;
	}

	// Send attendance status to client
	i3NetworkPacket packet;
	offset += sizeof(uint16_t);

	int32_t result = m_AttendanceData.bAttendedToday ? 1 : 0;	// 0 = not attended yet, 1 = already attended

	int32_t totalDays = m_AttendanceData.i32TotalDays;

	int32_t streak = m_AttendanceData.i32CurrentStreak;

	// Days array for UI display
	uint8_t dayCount = (uint8_t)(totalDays < MAX_ATTENDANCE_DAYS ? totalDays : MAX_ATTENDANCE_DAYS);
	for (int i = 0; i < dayCount && offset < 120; i++)
	{
		memcpy(buffer + offset, &m_AttendanceData.ui8Days[i], 1);	offset += 1;
	}
	i3NetworkPacket packet(PROTOCOL_BASE_ATTENDANCE_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&totalDays, sizeof(int32_t));
	packet.WriteData(&streak, sizeof(int32_t));
	packet.WriteData(&dayCount, 1);
	SendPacketMessage(&packet);

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
	offset += sizeof(uint16_t);

	int32_t result = 0;

	// Daily stats
	memcpy(buffer + offset, &m_DailyRecord.i32Kills, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_DailyRecord.i32Deaths, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_DailyRecord.i32Wins, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_DailyRecord.i32Losses, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_DailyRecord.i32GamesPlayed, sizeof(int));offset += sizeof(int);
	i3NetworkPacket packet(PROTOCOL_BASE_DAILY_RECORD_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	SendPacketMessage(&packet);
}
