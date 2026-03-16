#ifndef __ROULETTEDEF_GAME_H__
#define __ROULETTEDEF_GAME_H__

#pragma once

// Simplified Roulette/Gacha system for GameServer remake
// Original has complex probability tiers loaded from server config.

#define MAX_ROULETTE_ITEMS		50		// Max items in roulette pool
#define MAX_ROULETTE_TABS		5		// GP tab, Cash tab, Special tabs

// Roulette item grade (probability tier)
enum GameRouletteGrade
{
	ROULETTE_GRADE_LOSE = 0,		// No prize (consolation)
	ROULETTE_GRADE_1,				// 1 star (common)
	ROULETTE_GRADE_2,				// 2 star (uncommon)
	ROULETTE_GRADE_3,				// 3 star (rare)
	ROULETTE_GRADE_HIDDEN,			// Hidden (ultra rare)
	ROULETTE_GRADE_JACKPOT,			// Jackpot
	ROULETTE_GRADE_MAX,
};

#pragma pack(push, 1)

// Single roulette item
struct RouletteItem
{
	uint32_t	ui32ItemId;
	uint8_t		ui8Grade;			// GameRouletteGrade
	uint16_t	ui16Weight;			// Probability weight (higher = more likely)
	uint32_t	ui32Duration;		// Duration in days (0 = permanent)
	int			i32Count;			// Item count

	void Reset()
	{
		ui32ItemId = 0;
		ui8Grade = ROULETTE_GRADE_LOSE;
		ui16Weight = 0;
		ui32Duration = 0;
		i32Count = 0;
	}
};

// Roulette tab (category)
struct RouletteTab
{
	uint8_t			ui8TabId;
	char			szName[32];
	int				i32CostGP;		// GP cost per spin (0 = cash only)
	int				i32CostCash;	// Cash cost per spin (0 = GP only)
	RouletteItem	items[MAX_ROULETTE_ITEMS];
	int				i32ItemCount;
	uint32_t		ui32TotalWeight;
	bool			bActive;

	void Reset()
	{
		ui8TabId = 0;
		szName[0] = '\0';
		i32CostGP = 0;
		i32CostCash = 0;
		i32ItemCount = 0;
		ui32TotalWeight = 0;
		bActive = false;
		for (int i = 0; i < MAX_ROULETTE_ITEMS; i++)
			items[i].Reset();
	}

	void RecalcTotalWeight()
	{
		ui32TotalWeight = 0;
		for (int i = 0; i < i32ItemCount; i++)
			ui32TotalWeight += items[i].ui16Weight;
	}

	// Pick a random item based on weights
	int SpinRoulette() const
	{
		if (i32ItemCount == 0 || ui32TotalWeight == 0)
			return -1;

		uint32_t roll = (uint32_t)(rand() % ui32TotalWeight);
		uint32_t cumulative = 0;

		for (int i = 0; i < i32ItemCount; i++)
		{
			cumulative += items[i].ui16Weight;
			if (roll < cumulative)
				return i;
		}

		return i32ItemCount - 1;	// Fallback to last
	}
};

// Roulette manager
struct GameRouletteData
{
	RouletteTab tabs[MAX_ROULETTE_TABS];
	int i32TabCount;

	void Reset()
	{
		for (int i = 0; i < MAX_ROULETTE_TABS; i++)
			tabs[i].Reset();
		i32TabCount = 0;
	}

	void InitDefaults()
	{
		i32TabCount = 2;

		// Tab 0: GP Roulette (3000 GP per spin)
		RouletteTab& gpTab = tabs[0];
		gpTab.ui8TabId = 0;
		strncpy_s(gpTab.szName, "GP Roulette", _TRUNCATE);
		gpTab.i32CostGP = 3000;
		gpTab.i32CostCash = 0;
		gpTab.bActive = true;
		gpTab.i32ItemCount = 0;

		// Lose items (consolation GP)
		auto addItem = [](RouletteTab& tab, uint32_t itemId, uint8_t grade, uint16_t weight, uint32_t dur, int count) {
			if (tab.i32ItemCount >= MAX_ROULETTE_ITEMS) return;
			RouletteItem& item = tab.items[tab.i32ItemCount++];
			item.ui32ItemId = itemId;
			item.ui8Grade = grade;
			item.ui16Weight = weight;
			item.ui32Duration = dur;
			item.i32Count = count;
		};

		// GP Tab items
		addItem(gpTab, 0,		ROULETTE_GRADE_LOSE,	400, 0, 500);	// 500 GP consolation
		addItem(gpTab, 0,		ROULETTE_GRADE_LOSE,	300, 0, 1000);	// 1000 GP consolation
		addItem(gpTab, 0x1144,	ROULETTE_GRADE_1,		120, 7, 1);		// Common weapon (7d)
		addItem(gpTab, 0x1244,	ROULETTE_GRADE_1,		100, 7, 1);		// Common weapon (7d)
		addItem(gpTab, 0x2144,	ROULETTE_GRADE_2,		50,  30, 1);	// Uncommon weapon (30d)
		addItem(gpTab, 0x2244,	ROULETTE_GRADE_2,		40,  30, 1);	// Uncommon weapon (30d)
		addItem(gpTab, 0x3144,	ROULETTE_GRADE_3,		15,  0, 1);		// Rare weapon (permanent)
		addItem(gpTab, 0x3244,	ROULETTE_GRADE_3,		10,  0, 1);		// Rare weapon (permanent)
		addItem(gpTab, 0x4144,	ROULETTE_GRADE_HIDDEN,	3,   0, 1);		// Hidden item (permanent)
		addItem(gpTab, 0x5144,	ROULETTE_GRADE_JACKPOT,	1,   0, 1);		// Jackpot item
		gpTab.RecalcTotalWeight();

		// Tab 1: Cash Roulette (100 Cash per spin)
		RouletteTab& cashTab = tabs[1];
		cashTab.ui8TabId = 1;
		strncpy_s(cashTab.szName, "Cash Roulette", _TRUNCATE);
		cashTab.i32CostGP = 0;
		cashTab.i32CostCash = 100;
		cashTab.bActive = true;
		cashTab.i32ItemCount = 0;

		addItem(cashTab, 0,		ROULETTE_GRADE_LOSE,	300, 0, 2000);	// 2000 GP consolation
		addItem(cashTab, 0x1144,ROULETTE_GRADE_1,		150, 30, 1);	// Common weapon (30d)
		addItem(cashTab, 0x1244,ROULETTE_GRADE_1,		120, 30, 1);
		addItem(cashTab, 0x2144,ROULETTE_GRADE_2,		80,  0, 1);		// Uncommon (permanent)
		addItem(cashTab, 0x2244,ROULETTE_GRADE_2,		60,  0, 1);
		addItem(cashTab, 0x3144,ROULETTE_GRADE_3,		30,  0, 1);		// Rare (permanent)
		addItem(cashTab, 0x3244,ROULETTE_GRADE_3,		20,  0, 1);
		addItem(cashTab, 0x4144,ROULETTE_GRADE_HIDDEN,	8,   0, 1);		// Hidden
		addItem(cashTab, 0x5144,ROULETTE_GRADE_JACKPOT,	2,   0, 1);		// Jackpot
		cashTab.RecalcTotalWeight();
	}
};

#pragma pack(pop)

extern GameRouletteData* g_pRouletteData;

#endif // __ROULETTEDEF_GAME_H__
