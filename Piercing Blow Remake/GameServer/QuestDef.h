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

	bool IsComplete() const { return ui32Current >= ui32Target; }

	void Reset()
	{
		ui8Condition = 0;
		ui32Target = 0;
		ui32Current = 0;
		ui32RewardGP = 0;
		ui32RewardExp = 0;
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
		tut.cards[0].quests[0] = { QUEST_COND_KILLS, 10, 0, 500, 100 };
		tut.cards[0].quests[1] = { QUEST_COND_GAMES_PLAYED, 3, 0, 300, 50 };
		tut.cards[0].quests[2] = { QUEST_COND_HEADSHOTS, 3, 0, 500, 100 };
		tut.cards[0].quests[3] = { QUEST_COND_WINS, 2, 0, 400, 80 };

		// Card 1: Intermediate quests
		tut.cards[1].quests[0] = { QUEST_COND_KILLS, 30, 0, 1000, 200 };
		tut.cards[1].quests[1] = { QUEST_COND_WINS, 5, 0, 800, 150 };
		tut.cards[1].quests[2] = { QUEST_COND_HEADSHOTS, 10, 0, 1000, 200 };
		tut.cards[1].quests[3] = { QUEST_COND_GAMES_PLAYED, 10, 0, 600, 100 };

		ui8ActiveSetIndex = 0;
	}
};

#endif // __QUESTDEF_H__
