#ifndef __EVENTDEF_GAME_H__
#define __EVENTDEF_GAME_H__

#pragma once

// Simplified event/attendance system for GameServer remake

#define MAX_ATTENDANCE_DAYS		30		// Max days in attendance tracking
#define MAX_DAILY_REWARDS		7		// Days per weekly reward cycle

// Attendance states
enum GameAttendanceState
{
	ATTENDANCE_NOT_ATTENDED = 0,
	ATTENDANCE_ATTENDED,
	ATTENDANCE_CLEAR,				// Completed weekly cycle
	ATTENDANCE_MAX,
};

#pragma pack(push, 1)

// Daily attendance reward
struct GameAttendanceReward
{
	uint32_t	ui32RewardGP;
	uint32_t	ui32RewardExp;
	uint32_t	ui32RewardItemId;		// 0 = no item

	void Reset()
	{
		ui32RewardGP = 0;
		ui32RewardExp = 0;
		ui32RewardItemId = 0;
	}
};

// Per-player attendance data
struct GameAttendanceData
{
	uint8_t		ui8Days[MAX_ATTENDANCE_DAYS];	// ATTENDANCE_* state per day
	int			i32TotalDays;					// Total days attended
	int			i32CurrentStreak;				// Consecutive days
	uint32_t	ui32LastAttendDate;				// YYYYMMDD format
	bool		bAttendedToday;

	void Reset()
	{
		memset(ui8Days, 0, sizeof(ui8Days));
		i32TotalDays = 0;
		i32CurrentStreak = 0;
		ui32LastAttendDate = 0;
		bAttendedToday = false;
	}

	// Get daily reward based on streak
	GameAttendanceReward GetDailyReward(int dayIndex) const
	{
		GameAttendanceReward reward;
		reward.ui32RewardGP = 500 + (dayIndex % MAX_DAILY_REWARDS) * 200;
		reward.ui32RewardExp = 100 + (dayIndex % MAX_DAILY_REWARDS) * 50;
		reward.ui32RewardItemId = 0;
		// Day 7 bonus
		if ((dayIndex + 1) % MAX_DAILY_REWARDS == 0)
		{
			reward.ui32RewardGP = 3000;
			reward.ui32RewardExp = 500;
		}
		return reward;
	}
};

// Daily record (battle stats for today)
struct GameDailyRecord
{
	int		i32Kills;
	int		i32Deaths;
	int		i32Wins;
	int		i32Losses;
	int		i32GamesPlayed;
	int64_t	i64ExpGained;
	int		i32GPGained;

	void Reset()
	{
		i32Kills = 0;
		i32Deaths = 0;
		i32Wins = 0;
		i32Losses = 0;
		i32GamesPlayed = 0;
		i64ExpGained = 0;
		i32GPGained = 0;
	}
};

// Boost event system (Phase 14B)
#define MAX_BOOST_EVENTS	4

enum GameBoostType
{
	BOOST_TYPE_EXP = 0,		// EXP multiplier
	BOOST_TYPE_POINT,		// GP multiplier
	BOOST_TYPE_MAX,
};

struct GameBoostEvent
{
	bool		bActive;
	uint8_t		ui8BoostType;		// GameBoostType
	uint16_t	ui16Multiplier;		// 100 = 1x, 150 = 1.5x, 200 = 2x
	uint32_t	ui32StartTime;		// Hour of day (0-23) * 100 + minute
	uint32_t	ui32EndTime;		// Hour of day (0-23) * 100 + minute
	uint8_t		ui8DayOfWeek;		// 0=everyday, 1=Mon, 2=Tue, ... 7=Sun
	char		szDescription[64];

	void Reset()
	{
		bActive = false;
		ui8BoostType = BOOST_TYPE_EXP;
		ui16Multiplier = 100;
		ui32StartTime = 0;
		ui32EndTime = 2400;
		ui8DayOfWeek = 0;
		szDescription[0] = '\0';
	}
};

#pragma pack(pop)

#endif // __EVENTDEF_GAME_H__
