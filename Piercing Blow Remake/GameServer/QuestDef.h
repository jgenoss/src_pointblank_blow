#ifndef __QUESTDEF_H__
#define __QUESTDEF_H__

#pragma once

// Quest system definitions (from CommonDef_Quest.h)
#define MAX_QUEST_PER_CARD		4		// Quests per card
#define MAX_CARD_PER_CARDSET	10		// Cards per cardset (P,O,I,N,T,B,L,A,N2,K)
#define MAX_CARDSET_PER_USER	4		// Active cardsets per user

// Quest card set types
enum GameQuestCardSetType
{
	QUEST_CARDSET_NONE = 0,
	QUEST_CARDSET_TUTORIAL = 1,
	QUEST_CARDSET_DINO_TUTORIAL = 2,
	QUEST_CARDSET_HUMAN_TUTORIAL = 3,
	QUEST_CARDSET_GOLD = 4,
	QUEST_CARDSET_ASSAULT = 5,
	QUEST_CARDSET_BACKUP = 6,
	QUEST_CARDSET_INFILTRATION = 7,
	QUEST_CARDSET_SPECIAL = 8,
	QUEST_CARDSET_DEFCON = 9,
	QUEST_CARDSET_COMMISSIONED = 10,
	QUEST_CARDSET_COMPANY = 11,
	QUEST_CARDSET_FIELD = 12,
	QUEST_CARDSET_EVENT = 13,
	QUEST_CARDSET_COUNT,
};

// Quest condition types
enum GameQuestCondition
{
	QUEST_COND_KILLS = 0,
	QUEST_COND_HEADSHOTS,
	QUEST_COND_WINS,
	QUEST_COND_GAMES_PLAYED,
	QUEST_COND_DEATHS,
	QUEST_COND_ASSISTS,

	// Extended condition types (1.5 quest system)
	QUEST_COND_WEAPON_KILLS,		// Kills with specific weapon class (ui32Param = weapon class ID)
	QUEST_COND_STAGE_WINS,			// Wins on specific stage (ui32Param = stage/map ID)
	QUEST_COND_BOMB_PLANTS,			// Bomb plant count (sabotage/demolition modes)
	QUEST_COND_BOMB_DEFUSES,		// Bomb defuse count
	QUEST_COND_MULTI_KILLS,			// Double/triple/multi kills (ui32Param = min kill count per multi)
	QUEST_COND_CONSECUTIVE_KILLS,	// Kill streak without dying (ui32Param = min streak length)
	QUEST_COND_PLAY_TIME,			// Total minutes played
	QUEST_COND_DAMAGE_DEALT,		// Total damage dealt
	QUEST_COND_COUNT,
};

// Single quest within a card
struct GameQuestEntry
{
	uint8_t		ui8Condition;		// GameQuestCondition
	uint32_t	ui32Target;			// Target count
	uint32_t	ui32Current;		// Current progress
	uint32_t	ui32RewardGP;		// GP reward
	uint32_t	ui32RewardExp;		// EXP reward
	uint32_t	ui32Param;			// Condition parameter (weapon class, stage ID, etc.)

	bool IsComplete() const { return ui32Current >= ui32Target; }

	void Reset()
	{
		ui8Condition = 0;
		ui32Target = 0;
		ui32Current = 0;
		ui32RewardGP = 0;
		ui32RewardExp = 0;
		ui32Param = 0;
	}
};

// A card containing 4 quests
struct GameQuestCard
{
	GameQuestEntry	quests[MAX_QUEST_PER_CARD];
	bool			bComplete;		// All quests in this card are done

	void Reset()
	{
		for (int i = 0; i < MAX_QUEST_PER_CARD; i++)
			quests[i].Reset();
		bComplete = false;
	}

	void CheckComplete()
	{
		bComplete = true;
		for (int i = 0; i < MAX_QUEST_PER_CARD; i++)
		{
			if (quests[i].ui32Target > 0 && !quests[i].IsComplete())
			{
				bComplete = false;
				break;
			}
		}
	}
};

// A cardset containing 10 cards
struct GameQuestCardSet
{
	uint8_t			ui8Type;		// GameQuestCardSetType
	GameQuestCard	cards[MAX_CARD_PER_CARDSET];
	uint8_t			ui8ActiveCard;	// Currently active card index (0-9)

	void Reset()
	{
		ui8Type = QUEST_CARDSET_NONE;
		for (int i = 0; i < MAX_CARD_PER_CARDSET; i++)
			cards[i].Reset();
		ui8ActiveCard = 0;
	}

	bool IsActive() const { return ui8Type != QUEST_CARDSET_NONE; }
};

// Per-player quest data
struct GameQuestData
{
	GameQuestCardSet	cardSets[MAX_CARDSET_PER_USER];
	uint8_t				ui8ActiveSetIndex;	// Currently active cardset (0-3)

	void Reset()
	{
		for (int i = 0; i < MAX_CARDSET_PER_USER; i++)
			cardSets[i].Reset();
		ui8ActiveSetIndex = 0;
	}

	// Initialize tutorial cardset with default quests
	void InitTutorial()
	{
		GameQuestCardSet& tut = cardSets[0];
		tut.ui8Type = QUEST_CARDSET_TUTORIAL;
		tut.ui8ActiveCard = 0;

		// Card 0: Basic combat quests
		tut.cards[0].quests[0] = { QUEST_COND_KILLS, 10, 0, 500, 100, 0 };
		tut.cards[0].quests[1] = { QUEST_COND_GAMES_PLAYED, 3, 0, 300, 50, 0 };
		tut.cards[0].quests[2] = { QUEST_COND_HEADSHOTS, 3, 0, 500, 100, 0 };
		tut.cards[0].quests[3] = { QUEST_COND_WINS, 2, 0, 400, 80, 0 };

		// Card 1: Intermediate quests
		tut.cards[1].quests[0] = { QUEST_COND_KILLS, 30, 0, 1000, 200, 0 };
		tut.cards[1].quests[1] = { QUEST_COND_WINS, 5, 0, 800, 150, 0 };
		tut.cards[1].quests[2] = { QUEST_COND_HEADSHOTS, 10, 0, 1000, 200, 0 };
		tut.cards[1].quests[3] = { QUEST_COND_GAMES_PLAYED, 10, 0, 600, 100, 0 };

		ui8ActiveSetIndex = 0;
	}
};

// ============================================================================
// Event Quest System (1.5 quest system - event, honor, period quests)
// ============================================================================

#define MAX_EVENT_QUEST_COUNT		8		// Max active event quests
#define MAX_HONOR_QUEST_COUNT		4		// Max active honor quests
#define MAX_PERIOD_QUEST_COUNT		6		// Max active period quests (weekly/monthly)
#define MAX_QUEST_OBJECTIVES		4		// Objectives per event/honor/period quest

// Event quest types
enum GameEventQuestType
{
	EVENT_QUEST_TYPE_NONE = 0,
	EVENT_QUEST_TYPE_TIME_LIMITED,			// Has start/end timestamps
	EVENT_QUEST_TYPE_LOGIN_EVENT,			// Login during event period
	EVENT_QUEST_TYPE_SPECIAL_MODE,			// Play specific game mode during event
	EVENT_QUEST_TYPE_COUNT,
};

// Honor quest rank requirement
enum GameHonorQuestRank
{
	HONOR_RANK_NONE = 0,
	HONOR_RANK_SERGEANT = 1,				// Rank 10+
	HONOR_RANK_LIEUTENANT = 2,				// Rank 20+
	HONOR_RANK_CAPTAIN = 3,				// Rank 30+
	HONOR_RANK_MAJOR = 4,					// Rank 40+
	HONOR_RANK_COLONEL = 5,				// Rank 50+
	HONOR_RANK_GENERAL = 6,				// Rank 60+
	HONOR_RANK_COUNT,
};

// Period quest cycle type
enum GamePeriodQuestCycle
{
	PERIOD_QUEST_DAILY = 0,
	PERIOD_QUEST_WEEKLY,
	PERIOD_QUEST_MONTHLY,
	PERIOD_QUEST_EVENT,						// Tied to a specific event period
	PERIOD_QUEST_CYCLE_COUNT,
};

// Quest objective within an event/honor/period quest
struct GameQuestObjective
{
	uint8_t		ui8Condition;				// GameQuestCondition
	uint32_t	ui32Param;					// Condition parameter (weapon class, stage ID, etc.)
	uint32_t	ui32Target;					// Target count
	uint32_t	ui32Current;				// Current progress

	bool IsComplete() const { return ui32Current >= ui32Target; }

	void Reset()
	{
		ui8Condition = 0;
		ui32Param = 0;
		ui32Target = 0;
		ui32Current = 0;
	}
};

// Time-limited event quest
struct GameEventQuest
{
	uint32_t			ui32QuestId;			// Unique event quest ID
	uint8_t				ui8Type;				// GameEventQuestType
	uint8_t				ui8ObjectiveCount;		// Number of active objectives (1-4)
	GameQuestObjective	objectives[MAX_QUEST_OBJECTIVES];
	uint32_t			ui32RewardGP;
	uint32_t			ui32RewardExp;
	uint32_t			ui32RewardItemId;		// Bonus item reward (0 = none)
	uint32_t			ui32StartTime;			// Event start (YYYYMMDD * 10000 + HHMM)
	uint32_t			ui32EndTime;			// Event end (YYYYMMDD * 10000 + HHMM)
	bool				bCompleted;				// All objectives met
	bool				bRewardClaimed;			// Reward already claimed

	bool IsActive() const { return ui32QuestId != 0 && !bRewardClaimed; }

	bool CheckComplete()
	{
		if (ui8ObjectiveCount == 0)
			return false;
		for (int i = 0; i < ui8ObjectiveCount && i < MAX_QUEST_OBJECTIVES; i++)
		{
			if (!objectives[i].IsComplete())
			{
				bCompleted = false;
				return false;
			}
		}
		bCompleted = true;
		return true;
	}

	void Reset()
	{
		ui32QuestId = 0;
		ui8Type = EVENT_QUEST_TYPE_NONE;
		ui8ObjectiveCount = 0;
		for (int i = 0; i < MAX_QUEST_OBJECTIVES; i++)
			objectives[i].Reset();
		ui32RewardGP = 0;
		ui32RewardExp = 0;
		ui32RewardItemId = 0;
		ui32StartTime = 0;
		ui32EndTime = 0;
		bCompleted = false;
		bRewardClaimed = false;
	}
};

// Prestige quest for high-rank players
struct GameHonorQuest
{
	uint32_t			ui32QuestId;			// Unique honor quest ID
	uint8_t				ui8RequiredRank;		// GameHonorQuestRank minimum
	uint8_t				ui8ObjectiveCount;		// Number of active objectives (1-4)
	GameQuestObjective	objectives[MAX_QUEST_OBJECTIVES];
	uint32_t			ui32RewardGP;
	uint32_t			ui32RewardExp;
	uint32_t			ui32RewardItemId;		// Special honor item reward
	uint32_t			ui32HonorPoints;		// Bonus honor/prestige points
	bool				bCompleted;
	bool				bRewardClaimed;

	bool IsActive() const { return ui32QuestId != 0 && !bRewardClaimed; }

	bool CheckComplete()
	{
		if (ui8ObjectiveCount == 0)
			return false;
		for (int i = 0; i < ui8ObjectiveCount && i < MAX_QUEST_OBJECTIVES; i++)
		{
			if (!objectives[i].IsComplete())
			{
				bCompleted = false;
				return false;
			}
		}
		bCompleted = true;
		return true;
	}

	void Reset()
	{
		ui32QuestId = 0;
		ui8RequiredRank = HONOR_RANK_NONE;
		ui8ObjectiveCount = 0;
		for (int i = 0; i < MAX_QUEST_OBJECTIVES; i++)
			objectives[i].Reset();
		ui32RewardGP = 0;
		ui32RewardExp = 0;
		ui32RewardItemId = 0;
		ui32HonorPoints = 0;
		bCompleted = false;
		bRewardClaimed = false;
	}
};

// Weekly/monthly rotating quest
struct GamePeriodQuest
{
	uint32_t			ui32QuestId;			// Unique period quest ID
	uint8_t				ui8Cycle;				// GamePeriodQuestCycle
	uint8_t				ui8ObjectiveCount;		// Number of active objectives (1-4)
	GameQuestObjective	objectives[MAX_QUEST_OBJECTIVES];
	uint32_t			ui32RewardGP;
	uint32_t			ui32RewardExp;
	uint32_t			ui32RewardItemId;		// Bonus item reward (0 = none)
	uint32_t			ui32ResetTime;			// Next reset timestamp (YYYYMMDD * 10000 + HHMM)
	bool				bCompleted;
	bool				bRewardClaimed;
	bool				bSelected;				// Player has selected/activated this quest

	bool IsActive() const { return ui32QuestId != 0 && bSelected && !bRewardClaimed; }

	bool CheckComplete()
	{
		if (ui8ObjectiveCount == 0)
			return false;
		for (int i = 0; i < ui8ObjectiveCount && i < MAX_QUEST_OBJECTIVES; i++)
		{
			if (!objectives[i].IsComplete())
			{
				bCompleted = false;
				return false;
			}
		}
		bCompleted = true;
		return true;
	}

	void Reset()
	{
		ui32QuestId = 0;
		ui8Cycle = PERIOD_QUEST_DAILY;
		ui8ObjectiveCount = 0;
		for (int i = 0; i < MAX_QUEST_OBJECTIVES; i++)
			objectives[i].Reset();
		ui32RewardGP = 0;
		ui32RewardExp = 0;
		ui32RewardItemId = 0;
		ui32ResetTime = 0;
		bCompleted = false;
		bRewardClaimed = false;
		bSelected = false;
	}
};

// Per-player extended quest data (event/honor/period quests)
struct GameEventQuestData
{
	GameEventQuest		eventQuests[MAX_EVENT_QUEST_COUNT];
	int					i32EventQuestCount;

	GameHonorQuest		honorQuests[MAX_HONOR_QUEST_COUNT];
	int					i32HonorQuestCount;

	GamePeriodQuest		periodQuests[MAX_PERIOD_QUEST_COUNT];
	int					i32PeriodQuestCount;

	void Reset()
	{
		for (int i = 0; i < MAX_EVENT_QUEST_COUNT; i++)
			eventQuests[i].Reset();
		i32EventQuestCount = 0;

		for (int i = 0; i < MAX_HONOR_QUEST_COUNT; i++)
			honorQuests[i].Reset();
		i32HonorQuestCount = 0;

		for (int i = 0; i < MAX_PERIOD_QUEST_COUNT; i++)
			periodQuests[i].Reset();
		i32PeriodQuestCount = 0;
	}

	// Initialize default event quests (server-side content; normally loaded from DB/config)
	void InitDefaults()
	{
		// Default event quest: Kill 50 enemies during event
		GameEventQuest& eq = eventQuests[0];
		eq.ui32QuestId = 9001;
		eq.ui8Type = EVENT_QUEST_TYPE_TIME_LIMITED;
		eq.ui8ObjectiveCount = 2;
		eq.objectives[0] = { QUEST_COND_KILLS, 0, 50, 0 };
		eq.objectives[1] = { QUEST_COND_WINS, 0, 10, 0 };
		eq.ui32RewardGP = 5000;
		eq.ui32RewardExp = 1000;
		eq.ui32RewardItemId = 0;
		eq.ui32StartTime = 202601010000;
		eq.ui32EndTime = 203112312359;
		eq.bCompleted = false;
		eq.bRewardClaimed = false;
		i32EventQuestCount = 1;

		// Default honor quest: Get 100 headshots (requires Sergeant rank)
		GameHonorQuest& hq = honorQuests[0];
		hq.ui32QuestId = 8001;
		hq.ui8RequiredRank = HONOR_RANK_SERGEANT;
		hq.ui8ObjectiveCount = 2;
		hq.objectives[0] = { QUEST_COND_HEADSHOTS, 0, 100, 0 };
		hq.objectives[1] = { QUEST_COND_CONSECUTIVE_KILLS, 5, 3, 0 };
		hq.ui32RewardGP = 10000;
		hq.ui32RewardExp = 3000;
		hq.ui32RewardItemId = 0;
		hq.ui32HonorPoints = 50;
		hq.bCompleted = false;
		hq.bRewardClaimed = false;
		i32HonorQuestCount = 1;

		// Default weekly period quest: Win 20 games this week
		GamePeriodQuest& pq = periodQuests[0];
		pq.ui32QuestId = 7001;
		pq.ui8Cycle = PERIOD_QUEST_WEEKLY;
		pq.ui8ObjectiveCount = 2;
		pq.objectives[0] = { QUEST_COND_WINS, 0, 20, 0 };
		pq.objectives[1] = { QUEST_COND_GAMES_PLAYED, 0, 30, 0 };
		pq.ui32RewardGP = 3000;
		pq.ui32RewardExp = 500;
		pq.ui32RewardItemId = 0;
		pq.ui32ResetTime = 0;
		pq.bCompleted = false;
		pq.bRewardClaimed = false;
		pq.bSelected = false;
		i32PeriodQuestCount = 1;

		// Default monthly period quest: Deal 50000 damage
		GamePeriodQuest& pq2 = periodQuests[1];
		pq2.ui32QuestId = 7002;
		pq2.ui8Cycle = PERIOD_QUEST_MONTHLY;
		pq2.ui8ObjectiveCount = 1;
		pq2.objectives[0] = { QUEST_COND_DAMAGE_DEALT, 0, 50000, 0 };
		pq2.ui32RewardGP = 8000;
		pq2.ui32RewardExp = 1500;
		pq2.ui32RewardItemId = 0;
		pq2.ui32ResetTime = 0;
		pq2.bCompleted = false;
		pq2.bRewardClaimed = false;
		pq2.bSelected = false;
		i32PeriodQuestCount = 2;
	}
};

#endif // __QUESTDEF_H__
