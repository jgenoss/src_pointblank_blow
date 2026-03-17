#ifndef __EVENTDEF_GAME_H__
#define __EVENTDEF_GAME_H__

#pragma once

// Simplified event/attendance system for GameServer remake

#define MAX_ATTENDANCE_DAYS		30		// Max days in attendance tracking
#define MAX_DAILY_REWARDS		7		// Days per weekly reward cycle

// Attendance milestone item IDs (using MAKE_ITEM_ID convention from InventoryDef.h)
// These can be overridden by config later
#define ATTENDANCE_ITEM_DAY3	0x00130001	// Supply box (type 19, sub 0, num 1)
#define ATTENDANCE_ITEM_DAY7	0x00100001	// 1-day EXP boost card (type 16, sub 0, num 1 = XP_PLUS_110)
#define ATTENDANCE_ITEM_DAY14	0x00100004	// 3-day GP boost card (type 16, sub 0, num 4 = PT_PLUS_130)
#define ATTENDANCE_ITEM_DAY21	0x000F0001	// Weapon coupon (type 15, sub 0, num 1)
#define ATTENDANCE_ITEM_DAY28	0x000D0101	// Special character (type 13, sub 1, num 1)

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

	// Get daily reward based on consecutive streak day
	// Streak-based: better rewards for longer streaks, item rewards at milestones
	GameAttendanceReward GetDailyReward(int streakDay) const
	{
		GameAttendanceReward reward;
		reward.Reset();

		// Base GP/Exp scales with streak position in weekly cycle
		int dayInCycle = streakDay % MAX_DAILY_REWARDS;
		reward.ui32RewardGP = 500 + dayInCycle * 200;
		reward.ui32RewardExp = 100 + dayInCycle * 50;

		// Day 7 weekly bonus (every 7th consecutive day)
		if (dayInCycle == MAX_DAILY_REWARDS - 1)
		{
			reward.ui32RewardGP = 3000;
			reward.ui32RewardExp = 500;
		}

		// Item rewards at streak milestones
		// Day 3: supply box (ITEM_TYPE_SUPPLY = type 19, subtype 0, number 1)
		if (streakDay == 2)
			reward.ui32RewardItemId = ATTENDANCE_ITEM_DAY3;
		// Day 7: 1-day EXP boost (ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_XP_PLUS_110)
		else if (streakDay == 6)
			reward.ui32RewardItemId = ATTENDANCE_ITEM_DAY7;
		// Day 14: 3-day GP boost
		else if (streakDay == 13)
			reward.ui32RewardItemId = ATTENDANCE_ITEM_DAY14;
		// Day 21: random weapon coupon
		else if (streakDay == 20)
			reward.ui32RewardItemId = ATTENDANCE_ITEM_DAY21;
		// Day 28 (monthly): special character skin
		else if (streakDay == 27)
			reward.ui32RewardItemId = ATTENDANCE_ITEM_DAY28;

		return reward;
	}

	// Check if month changed and reset
	bool ShouldMonthlyReset(uint32_t today) const
	{
		if (ui32LastAttendDate == 0)
			return false;

		// Extract month from YYYYMMDD
		uint32_t lastMonth = (ui32LastAttendDate / 100) % 100;
		uint32_t lastYear = ui32LastAttendDate / 10000;
		uint32_t curMonth = (today / 100) % 100;
		uint32_t curYear = today / 10000;

		return (curYear > lastYear) || (curYear == lastYear && curMonth > lastMonth);
	}

	void MonthlyReset()
	{
		memset(ui8Days, 0, sizeof(ui8Days));
		i32TotalDays = 0;
		i32CurrentStreak = 0;
		bAttendedToday = false;
		// ui32LastAttendDate preserved - updated on next attend
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
