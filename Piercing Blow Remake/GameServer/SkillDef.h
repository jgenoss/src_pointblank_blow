#ifndef __SKILLDEF_GAME_H__
#define __SKILLDEF_GAME_H__

#pragma once

// Simplified skill system for GameServer remake
// Original has CHARA_CLASS-based skill trees with main/assist/common skills.

#define MAX_SKILL_SLOTS			5		// Character class slots
#define MAX_MAIN_SKILLS			5		// Main weapon skills per class
#define MAX_ASSIST_SKILLS		4		// Assist skills per class
#define MAX_COMMON_SKILLS		5		// Common skills per class
#define MAX_SKILL_LEVEL			10		// Max level for main/common skills
#define MAX_ASSIST_SKILL_LEVEL	5		// Max level for assist skills

// Character classes
enum GameCharaClass
{
	CHARA_CLASS_ASSAULT = 0,
	CHARA_CLASS_SMG,
	CHARA_CLASS_SNIPER,
	CHARA_CLASS_SHOTGUN,
	CHARA_CLASS_MG,
	CHARA_CLASS_MAX,
};

// Main skill types (weapon proficiency)
enum GameMainSkillType
{
	MAIN_SKILL_DAMAGE = 0,		// Weapon damage
	MAIN_SKILL_ACCURACY,		// Weapon accuracy
	MAIN_SKILL_RECOIL,			// Recoil control
	MAIN_SKILL_RELOAD,			// Reload speed
	MAIN_SKILL_RANGE,			// Effective range
	MAIN_SKILL_MAX,
};

// Assist skill types
enum GameAssistSkillType
{
	ASSIST_SKILL_HANDGUN = 0,	// Sidearm proficiency
	ASSIST_SKILL_MELEE,			// Melee proficiency
	ASSIST_SKILL_THROW,			// Grenade proficiency
	ASSIST_SKILL_ETC,			// Special equipment
	ASSIST_SKILL_MAX,
};

// Common skill types
enum GameCommonSkillType
{
	COMMON_SKILL_RUN = 0,		// Movement speed
	COMMON_SKILL_WALK,			// Walk speed (quiet)
	COMMON_SKILL_DAMAGE_RATE,	// Damage resistance
	COMMON_SKILL_HELMET_DEF,	// Head protection
	COMMON_SKILL_JUMP,			// Jump ability
	COMMON_SKILL_MAX,
};

#pragma pack(push, 1)

// Skills for one character class
struct GameClassSkills
{
	uint8_t		ui8Class;								// GameCharaClass
	uint8_t		ui8MainLevels[MAX_MAIN_SKILLS];			// Level per main skill (0-10)
	uint8_t		ui8AssistLevels[MAX_ASSIST_SKILLS];		// Level per assist skill (0-5)
	uint8_t		ui8CommonLevels[MAX_COMMON_SKILLS];		// Level per common skill (0-10)
	uint16_t	ui16SkillPoints;						// Available skill points

	void Reset()
	{
		ui8Class = 0;
		memset(ui8MainLevels, 0, sizeof(ui8MainLevels));
		memset(ui8AssistLevels, 0, sizeof(ui8AssistLevels));
		memset(ui8CommonLevels, 0, sizeof(ui8CommonLevels));
		ui16SkillPoints = 0;
	}

	int GetTotalSkillPoints() const
	{
		int total = 0;
		for (int i = 0; i < MAX_MAIN_SKILLS; i++) total += ui8MainLevels[i];
		for (int i = 0; i < MAX_ASSIST_SKILLS; i++) total += ui8AssistLevels[i];
		for (int i = 0; i < MAX_COMMON_SKILLS; i++) total += ui8CommonLevels[i];
		return total;
	}
};

// Per-player skill data
struct GameSkillData
{
	uint8_t			ui8ActiveClass;						// Currently selected class
	GameClassSkills	classSkills[CHARA_CLASS_MAX];

	void Reset()
	{
		ui8ActiveClass = CHARA_CLASS_ASSAULT;
		for (int i = 0; i < CHARA_CLASS_MAX; i++)
		{
			classSkills[i].Reset();
			classSkills[i].ui8Class = (uint8_t)i;
			classSkills[i].ui16SkillPoints = 10;		// Start with 10 points
		}
	}
};

#pragma pack(pop)

#endif // __SKILLDEF_GAME_H__
