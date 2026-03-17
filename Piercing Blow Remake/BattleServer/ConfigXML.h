#ifndef __CONFIGXML_H__
#define __CONFIGXML_H__

#pragma once

// Maximum weapons/characters tracked
#define MAX_WEAPON_CONFIG		512
#define MAX_CHARACTER_CONFIG		32
#define MAX_STAGE_CONFIG		64

// Weapon configuration data (from Weapon.i3RegXML)
struct WeaponConfigData
{
	uint32_t	ui32ItemID;			// Weapon item ID
	float		fDamage;			// Base damage
	float		fRange;				// Max effective range
	float		fFireRate;			// Rounds per second
	float		fAccuracy;			// Accuracy (0-1)
	uint8_t		ui8SlotType;		// WEAPON_SLOT_TYPE
	uint8_t		ui8ClassType;		// WEAPON_CLASS_TYPE

	void Reset()
	{
		ui32ItemID = 0;
		fDamage = 30.0f;
		fRange = 5000.0f;
		fFireRate = 10.0f;
		fAccuracy = 0.8f;
		ui8SlotType = 0;
		ui8ClassType = 0;
	}
};

// Character configuration data (from Character.i3RegXML)
struct CharacterConfigData
{
	uint32_t	ui32CharID;			// Character ID
	int			i32MaxHP;			// Max HP
	float		fMoveSpeed;			// Movement speed (units/sec)
	float		fHitboxWidth;		// Hitbox width
	float		fHitboxHeight;		// Hitbox height

	void Reset()
	{
		ui32CharID = 0;
		i32MaxHP = 100;
		fMoveSpeed = 300.0f;
		fHitboxWidth = 30.0f;
		fHitboxHeight = 170.0f;
	}
};

// Stage/Map configuration data (from Stage.i3RegXML)
struct StageConfigData
{
	uint32_t	ui32StageID;
	char		szCodeName[32];		// Map codename (filename in Media/)
	uint8_t		ui8SupportedModes;	// Bitmask of supported game modes

	void Reset()
	{
		ui32StageID = 0;
		szCodeName[0] = '\0';
		ui8SupportedModes = 0xFF;
	}
};

// ConfigXML - Loads .i3RegXML configuration files from Media/config/
// These XML files define weapon stats, character data, equipment modifiers, etc.
// Reference: Client/Source/ClientSource/RscTool/ItemInfoMgr.cpp
class ConfigXML
{
public:
	ConfigXML();
	~ConfigXML();

	// Load all config XMLs from the specified path
	bool	Initialize(const char* pszMediaPath = "Media");
	void	Shutdown();

	// Weapon queries
	const WeaponConfigData*		GetWeaponConfig(uint32_t ui32ItemID) const;
	float						GetWeaponDamage(uint32_t ui32ItemID) const;
	float						GetWeaponRange(uint32_t ui32ItemID) const;
	int							GetWeaponCount() const		{ return m_i32WeaponCount; }

	// Character queries
	const CharacterConfigData*	GetCharacterConfig(uint32_t ui32CharID) const;
	int							GetCharacterMaxHP(uint32_t ui32CharID) const;
	float						GetCharacterMoveSpeed(uint32_t ui32CharID) const;
	int							GetCharacterCount() const	{ return m_i32CharacterCount; }

	// Stage queries
	const StageConfigData*		GetStageConfig(uint32_t ui32StageID) const;
	const char*					GetStageCodeName(uint32_t ui32StageID) const;
	int							GetStageCount() const		{ return m_i32StageCount; }

	bool	IsInitialized() const	{ return m_bInitialized; }

private:
	bool	LoadWeaponConfig(const char* pszPath);
	bool	LoadCharacterConfig(const char* pszPath);
	bool	LoadStageConfig(const char* pszPath);

	// Set defaults when XML files are not available
	void	SetDefaults();

private:
	WeaponConfigData	m_Weapons[MAX_WEAPON_CONFIG];
	int					m_i32WeaponCount;

	CharacterConfigData	m_Characters[MAX_CHARACTER_CONFIG];
	int					m_i32CharacterCount;

	StageConfigData		m_Stages[MAX_STAGE_CONFIG];
	int					m_i32StageCount;

	bool				m_bInitialized;
};

extern ConfigXML* g_pConfigXML;

#endif // __CONFIGXML_H__
