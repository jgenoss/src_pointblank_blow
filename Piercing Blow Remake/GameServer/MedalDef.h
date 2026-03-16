#ifndef __MEDALDEF_GAME_H__
#define __MEDALDEF_GAME_H__

#pragma once

// Simplified medal system for GameServer remake
// Original uses MQF files with complex medal definitions.
// This is a simplified in-memory version.

#define MAX_GAME_MEDALS				20		// Max medals tracked per player
#define MAX_GAME_MEDAL_SETS			4		// Max medal sets (daily, event, clan, normal)
#define MAX_MEDALS_PER_SET			6		// Medals per set
#define MAX_NOTIFY_MEDAL_COUNT		4		// Notify medal slots

// Medal types
enum GameMedalType
{
	GAME_MEDAL_TYPE_NORMAL = 0,
	GAME_MEDAL_TYPE_SET,
	GAME_MEDAL_TYPE_MAX,
};

// Medal set types
enum GameMedalSetType
{
	GAME_MEDAL_SET_NORMAL = 0,
	GAME_MEDAL_SET_EVENT,
	GAME_MEDAL_SET_DAILY,
	GAME_MEDAL_SET_CLAN,
	GAME_MEDAL_SET_MAX,
};

// Medal levels
enum GameMedalLevel
{
	GAME_MEDAL_LEVEL_NONE = 0,
	GAME_MEDAL_LEVEL_1,
	GAME_MEDAL_LEVEL_2,
	GAME_MEDAL_LEVEL_3,
	GAME_MEDAL_LEVEL_4,
	GAME_MEDAL_LEVEL_MAX = GAME_MEDAL_LEVEL_4,
};

// Medal action IDs (conditions for progress)
enum GameMedalActionId
{
	GAME_MEDAL_ACTION_NONE = 0,
	GAME_MEDAL_ACTION_KILL,
	GAME_MEDAL_ACTION_HEADSHOT,
	GAME_MEDAL_ACTION_WIN,
	GAME_MEDAL_ACTION_DEATH,
	GAME_MEDAL_ACTION_GAMES_PLAYED,
	GAME_MEDAL_ACTION_DOUBLE_KILL,
	GAME_MEDAL_ACTION_C4_PLANT,
	GAME_MEDAL_ACTION_C4_DEFUSE,
	GAME_MEDAL_ACTION_ASSIST,
	GAME_MEDAL_ACTION_BUY,
	GAME_MEDAL_ACTION_MAX,
};

#pragma pack(push, 1)

// Single medal tracking
struct GameMedalEntry
{
	uint16_t	ui16Idx;			// Medal unique index
	uint8_t		ui8ActionId;		// GameMedalActionId
	uint16_t	ui16Count;			// Current count
	uint16_t	ui16Target[GAME_MEDAL_LEVEL_MAX];	// Target per level (1-4)
	uint8_t		ui8GetReward;		// Bitmask: bit N = level N reward claimed
	uint32_t	ui32RewardGP[GAME_MEDAL_LEVEL_MAX];
	uint32_t	ui32RewardExp[GAME_MEDAL_LEVEL_MAX];

	uint8_t GetCurrentLevel() const
	{
		for (int i = GAME_MEDAL_LEVEL_MAX - 1; i >= GAME_MEDAL_LEVEL_1; i--)
		{
			if (ui16Target[i - 1] > 0 && ui16Count >= ui16Target[i - 1])
				return (uint8_t)i;
		}
		return GAME_MEDAL_LEVEL_NONE;
	}

	bool IsLevelComplete(int level) const
	{
		if (level < GAME_MEDAL_LEVEL_1 || level > GAME_MEDAL_LEVEL_MAX)
			return false;
		return ui16Target[level - 1] > 0 && ui16Count >= ui16Target[level - 1];
	}

	bool IsRewardClaimed(int level) const
	{
		return (ui8GetReward & (1 << (level - 1))) != 0;
	}

	void ClaimReward(int level)
	{
		ui8GetReward |= (1 << (level - 1));
	}

	void Reset()
	{
		ui16Idx = 0xFFFF;
		ui8ActionId = GAME_MEDAL_ACTION_NONE;
		ui16Count = 0;
		memset(ui16Target, 0, sizeof(ui16Target));
		ui8GetReward = 0;
		memset(ui32RewardGP, 0, sizeof(ui32RewardGP));
		memset(ui32RewardExp, 0, sizeof(ui32RewardExp));
	}
};

// Current medal set (in progress)
struct GameMedalSet
{
	uint8_t		ui8Type;			// GameMedalSetType
	uint16_t	ui16Idx;			// Set index
	uint16_t	ui16MedalIndices[MAX_MEDALS_PER_SET];	// Medal indices in this set
	uint16_t	ui16Count[MAX_MEDALS_PER_SET];			// Per-medal mission counts
	uint8_t		ui8GetReward;		// Bitmask per medal
	bool		bActive;

	void Reset()
	{
		ui8Type = GAME_MEDAL_SET_NORMAL;
		ui16Idx = 0xFFFF;
		memset(ui16MedalIndices, 0xFF, sizeof(ui16MedalIndices));
		memset(ui16Count, 0, sizeof(ui16Count));
		ui8GetReward = 0;
		bActive = false;
	}
};

// Notify medal entry
struct GameNotifyMedal
{
	uint8_t		ui8MedalType;		// GameMedalType
	uint16_t	ui16Idx;			// Medal index
	uint16_t	ui16Count;			// Current count

	void Reset()
	{
		ui8MedalType = GAME_MEDAL_TYPE_NORMAL;
		ui16Idx = 0xFFFF;
		ui16Count = 0;
	}
};

// Per-player medal data
struct GameMedalData
{
	GameMedalEntry	medals[MAX_GAME_MEDALS];
	int				i32MedalCount;
	GameMedalSet	curSets[GAME_MEDAL_SET_MAX];		// Current in-progress sets
	GameNotifyMedal	notifyMedals[MAX_NOTIFY_MEDAL_COUNT];
	uint8_t			ui8NewFlags[32];		// NEW flag storage (256 bits)

	void Reset()
	{
		for (int i = 0; i < MAX_GAME_MEDALS; i++)
			medals[i].Reset();
		i32MedalCount = 0;
		for (int i = 0; i < GAME_MEDAL_SET_MAX; i++)
			curSets[i].Reset();
		for (int i = 0; i < MAX_NOTIFY_MEDAL_COUNT; i++)
			notifyMedals[i].Reset();
		memset(ui8NewFlags, 0, sizeof(ui8NewFlags));
	}

	// Initialize default medals for new players
	void InitDefaults()
	{
		i32MedalCount = 0;

		// Medal 0: Kill medal
		medals[0].ui16Idx = 0;
		medals[0].ui8ActionId = GAME_MEDAL_ACTION_KILL;
		medals[0].ui16Count = 0;
		medals[0].ui16Target[0] = 50;	medals[0].ui32RewardGP[0] = 1000;	medals[0].ui32RewardExp[0] = 200;
		medals[0].ui16Target[1] = 200;	medals[0].ui32RewardGP[1] = 3000;	medals[0].ui32RewardExp[1] = 500;
		medals[0].ui16Target[2] = 500;	medals[0].ui32RewardGP[2] = 5000;	medals[0].ui32RewardExp[2] = 1000;
		medals[0].ui16Target[3] = 1000;	medals[0].ui32RewardGP[3] = 10000;	medals[0].ui32RewardExp[3] = 2000;
		medals[0].ui8GetReward = 0;
		i32MedalCount++;

		// Medal 1: Headshot medal
		medals[1].ui16Idx = 1;
		medals[1].ui8ActionId = GAME_MEDAL_ACTION_HEADSHOT;
		medals[1].ui16Count = 0;
		medals[1].ui16Target[0] = 20;	medals[1].ui32RewardGP[0] = 1500;	medals[1].ui32RewardExp[0] = 300;
		medals[1].ui16Target[1] = 100;	medals[1].ui32RewardGP[1] = 4000;	medals[1].ui32RewardExp[1] = 700;
		medals[1].ui16Target[2] = 300;	medals[1].ui32RewardGP[2] = 8000;	medals[1].ui32RewardExp[2] = 1500;
		medals[1].ui16Target[3] = 500;	medals[1].ui32RewardGP[3] = 15000;	medals[1].ui32RewardExp[3] = 3000;
		medals[1].ui8GetReward = 0;
		i32MedalCount++;

		// Medal 2: Win medal
		medals[2].ui16Idx = 2;
		medals[2].ui8ActionId = GAME_MEDAL_ACTION_WIN;
		medals[2].ui16Count = 0;
		medals[2].ui16Target[0] = 10;	medals[2].ui32RewardGP[0] = 2000;	medals[2].ui32RewardExp[0] = 400;
		medals[2].ui16Target[1] = 50;	medals[2].ui32RewardGP[1] = 5000;	medals[2].ui32RewardExp[1] = 1000;
		medals[2].ui16Target[2] = 200;	medals[2].ui32RewardGP[2] = 10000;	medals[2].ui32RewardExp[2] = 2000;
		medals[2].ui16Target[3] = 500;	medals[2].ui32RewardGP[3] = 20000;	medals[2].ui32RewardExp[3] = 5000;
		medals[2].ui8GetReward = 0;
		i32MedalCount++;

		// Medal 3: Games played medal
		medals[3].ui16Idx = 3;
		medals[3].ui8ActionId = GAME_MEDAL_ACTION_GAMES_PLAYED;
		medals[3].ui16Count = 0;
		medals[3].ui16Target[0] = 20;	medals[3].ui32RewardGP[0] = 1000;	medals[3].ui32RewardExp[0] = 200;
		medals[3].ui16Target[1] = 100;	medals[3].ui32RewardGP[1] = 3000;	medals[3].ui32RewardExp[1] = 500;
		medals[3].ui16Target[2] = 300;	medals[3].ui32RewardGP[2] = 6000;	medals[3].ui32RewardExp[2] = 1200;
		medals[3].ui16Target[3] = 1000;	medals[3].ui32RewardGP[3] = 12000;	medals[3].ui32RewardExp[3] = 2500;
		medals[3].ui8GetReward = 0;
		i32MedalCount++;
	}

	// Update medal progress after a battle
	void UpdateProgress(int kills, int deaths, int headshots, bool won)
	{
		for (int i = 0; i < i32MedalCount; i++)
		{
			GameMedalEntry& m = medals[i];
			switch (m.ui8ActionId)
			{
			case GAME_MEDAL_ACTION_KILL:		m.ui16Count += (uint16_t)kills;		break;
			case GAME_MEDAL_ACTION_HEADSHOT:	m.ui16Count += (uint16_t)headshots;	break;
			case GAME_MEDAL_ACTION_WIN:			if (won) m.ui16Count++;				break;
			case GAME_MEDAL_ACTION_DEATH:		m.ui16Count += (uint16_t)deaths;	break;
			case GAME_MEDAL_ACTION_GAMES_PLAYED: m.ui16Count++;					break;
			}
		}
	}

	GameMedalEntry* FindMedal(uint16_t idx)
	{
		for (int i = 0; i < i32MedalCount; i++)
		{
			if (medals[i].ui16Idx == idx)
				return &medals[i];
		}
		return nullptr;
	}
};

#pragma pack(pop)

#endif // __MEDALDEF_GAME_H__
