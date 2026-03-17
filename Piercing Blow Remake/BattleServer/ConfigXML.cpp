#include "pch.h"
#include "ConfigXML.h"

ConfigXML* g_pConfigXML = nullptr;

ConfigXML::ConfigXML()
	: m_i32WeaponCount(0)
	, m_i32CharacterCount(0)
	, m_i32StageCount(0)
	, m_bInitialized(false)
{
	memset(m_Weapons, 0, sizeof(m_Weapons));
	memset(m_Characters, 0, sizeof(m_Characters));
	memset(m_Stages, 0, sizeof(m_Stages));
}

ConfigXML::~ConfigXML()
{
	Shutdown();
}

bool ConfigXML::Initialize(const char* pszMediaPath)
{
	if (m_bInitialized)
		return true;

	printf("[ConfigXML] Loading config XMLs from '%s/config/'...\n", pszMediaPath);

	// Set reasonable defaults first (used when XML files are missing)
	SetDefaults();

	// Try to load each XML config file
	char szPath[MAX_PATH];

	sprintf_s(szPath, "%s/config/Weapon.i3RegXML", pszMediaPath);
	if (LoadWeaponConfig(szPath))
		printf("[ConfigXML] Loaded Weapon.i3RegXML: %d weapons\n", m_i32WeaponCount);

	sprintf_s(szPath, "%s/config/Character.i3RegXML", pszMediaPath);
	if (LoadCharacterConfig(szPath))
		printf("[ConfigXML] Loaded Character.i3RegXML: %d characters\n", m_i32CharacterCount);

	sprintf_s(szPath, "%s/config/Stage.i3RegXML", pszMediaPath);
	if (LoadStageConfig(szPath))
		printf("[ConfigXML] Loaded Stage.i3RegXML: %d stages\n", m_i32StageCount);

	m_bInitialized = true;

	printf("[ConfigXML] Initialized: Weapons=%d, Characters=%d, Stages=%d\n",
		m_i32WeaponCount, m_i32CharacterCount, m_i32StageCount);
	return true;
}

void ConfigXML::Shutdown()
{
	m_i32WeaponCount = 0;
	m_i32CharacterCount = 0;
	m_i32StageCount = 0;
	m_bInitialized = false;
}

// ============================================================================
// Set reasonable defaults when XML files are not available
// ============================================================================
void ConfigXML::SetDefaults()
{
	// Default character: 100 HP, 300 units/sec speed
	m_Characters[0].ui32CharID = 0;
	m_Characters[0].i32MaxHP = 100;
	m_Characters[0].fMoveSpeed = 300.0f;
	m_Characters[0].fHitboxWidth = 30.0f;
	m_Characters[0].fHitboxHeight = 170.0f;
	m_i32CharacterCount = 1;

	// Default weapon ranges for validation
	// Knife
	WeaponConfigData& knife = m_Weapons[0];
	knife.ui32ItemID = 1;
	knife.fDamage = 50.0f;
	knife.fRange = 200.0f;
	knife.fFireRate = 2.0f;
	knife.fAccuracy = 1.0f;
	knife.ui8SlotType = 4;	// Melee

	// Rifle (default)
	WeaponConfigData& rifle = m_Weapons[1];
	rifle.ui32ItemID = 2;
	rifle.fDamage = 30.0f;
	rifle.fRange = 5000.0f;
	rifle.fFireRate = 10.0f;
	rifle.fAccuracy = 0.8f;
	rifle.ui8SlotType = 0;	// Primary

	// Sniper
	WeaponConfigData& sniper = m_Weapons[2];
	sniper.ui32ItemID = 3;
	sniper.fDamage = 90.0f;
	sniper.fRange = 10000.0f;
	sniper.fFireRate = 1.0f;
	sniper.fAccuracy = 0.95f;
	sniper.ui8SlotType = 0;	// Primary

	// Pistol
	WeaponConfigData& pistol = m_Weapons[3];
	pistol.ui32ItemID = 4;
	pistol.fDamage = 25.0f;
	pistol.fRange = 3000.0f;
	pistol.fFireRate = 5.0f;
	pistol.fAccuracy = 0.7f;
	pistol.ui8SlotType = 1;	// Secondary

	m_i32WeaponCount = 4;
}

// ============================================================================
// XML Loading (simplified - i3RegXML is a property editor format)
// The .i3RegXML format is essentially XML with key-value pairs per item
// ============================================================================

bool ConfigXML::LoadWeaponConfig(const char* pszPath)
{
	HANDLE hFile = CreateFileA(pszPath, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("[ConfigXML] Weapon config not found: %s (using defaults)\n", pszPath);
		return false;
	}

	// Get file size
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	if (dwFileSize == 0 || dwFileSize == INVALID_FILE_SIZE || dwFileSize > 10 * 1024 * 1024)
	{
		CloseHandle(hFile);
		return false;
	}

	// Read entire file
	char* pBuffer = new char[dwFileSize + 1];
	DWORD dwRead = 0;
	ReadFile(hFile, pBuffer, dwFileSize, &dwRead, NULL);
	pBuffer[dwFileSize] = '\0';
	CloseHandle(hFile);

	// Simple XML parsing: look for weapon entries
	// Format varies, but typically <Item ID="xxx" Damage="xxx" Range="xxx" .../>
	// This is a simplified parser that handles the basic case
	m_i32WeaponCount = 0;

	char* pCursor = pBuffer;
	while ((pCursor = strstr(pCursor, "<Item")) != nullptr && m_i32WeaponCount < MAX_WEAPON_CONFIG)
	{
		WeaponConfigData& wep = m_Weapons[m_i32WeaponCount];
		wep.Reset();

		// Parse attributes
		char* pEnd = strchr(pCursor, '>');
		if (!pEnd) break;

		// ID
		char* pAttr = strstr(pCursor, "ID=\"");
		if (pAttr && pAttr < pEnd)
			wep.ui32ItemID = (uint32_t)atoi(pAttr + 4);

		pAttr = strstr(pCursor, "Damage=\"");
		if (pAttr && pAttr < pEnd)
			wep.fDamage = (float)atof(pAttr + 8);

		pAttr = strstr(pCursor, "Range=\"");
		if (pAttr && pAttr < pEnd)
			wep.fRange = (float)atof(pAttr + 7);

		pAttr = strstr(pCursor, "FireRate=\"");
		if (pAttr && pAttr < pEnd)
			wep.fFireRate = (float)atof(pAttr + 10);

		if (wep.ui32ItemID > 0)
			m_i32WeaponCount++;

		pCursor = pEnd + 1;
	}

	delete[] pBuffer;
	return (m_i32WeaponCount > 0);
}

bool ConfigXML::LoadCharacterConfig(const char* pszPath)
{
	HANDLE hFile = CreateFileA(pszPath, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("[ConfigXML] Character config not found: %s (using defaults)\n", pszPath);
		return false;
	}

	DWORD dwFileSize = GetFileSize(hFile, NULL);
	if (dwFileSize == 0 || dwFileSize == INVALID_FILE_SIZE || dwFileSize > 10 * 1024 * 1024)
	{
		CloseHandle(hFile);
		return false;
	}

	char* pBuffer = new char[dwFileSize + 1];
	DWORD dwRead = 0;
	ReadFile(hFile, pBuffer, dwFileSize, &dwRead, NULL);
	pBuffer[dwFileSize] = '\0';
	CloseHandle(hFile);

	m_i32CharacterCount = 0;

	char* pCursor = pBuffer;
	while ((pCursor = strstr(pCursor, "<Character")) != nullptr && m_i32CharacterCount < MAX_CHARACTER_CONFIG)
	{
		CharacterConfigData& chr = m_Characters[m_i32CharacterCount];
		chr.Reset();

		char* pEnd = strchr(pCursor, '>');
		if (!pEnd) break;

		char* pAttr = strstr(pCursor, "ID=\"");
		if (pAttr && pAttr < pEnd)
			chr.ui32CharID = (uint32_t)atoi(pAttr + 4);

		pAttr = strstr(pCursor, "HP=\"");
		if (pAttr && pAttr < pEnd)
			chr.i32MaxHP = atoi(pAttr + 4);

		pAttr = strstr(pCursor, "Speed=\"");
		if (pAttr && pAttr < pEnd)
			chr.fMoveSpeed = (float)atof(pAttr + 7);

		if (chr.ui32CharID > 0 || m_i32CharacterCount == 0)
			m_i32CharacterCount++;

		pCursor = pEnd + 1;
	}

	delete[] pBuffer;
	return (m_i32CharacterCount > 0);
}

bool ConfigXML::LoadStageConfig(const char* pszPath)
{
	HANDLE hFile = CreateFileA(pszPath, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("[ConfigXML] Stage config not found: %s (using defaults)\n", pszPath);
		return false;
	}

	DWORD dwFileSize = GetFileSize(hFile, NULL);
	if (dwFileSize == 0 || dwFileSize == INVALID_FILE_SIZE || dwFileSize > 10 * 1024 * 1024)
	{
		CloseHandle(hFile);
		return false;
	}

	char* pBuffer = new char[dwFileSize + 1];
	DWORD dwRead = 0;
	ReadFile(hFile, pBuffer, dwFileSize, &dwRead, NULL);
	pBuffer[dwFileSize] = '\0';
	CloseHandle(hFile);

	m_i32StageCount = 0;

	char* pCursor = pBuffer;
	while ((pCursor = strstr(pCursor, "<Stage")) != nullptr && m_i32StageCount < MAX_STAGE_CONFIG)
	{
		StageConfigData& stage = m_Stages[m_i32StageCount];
		stage.Reset();

		char* pEnd = strchr(pCursor, '>');
		if (!pEnd) break;

		char* pAttr = strstr(pCursor, "ID=\"");
		if (pAttr && pAttr < pEnd)
			stage.ui32StageID = (uint32_t)atoi(pAttr + 4);

		pAttr = strstr(pCursor, "CodeName=\"");
		if (pAttr && pAttr < pEnd)
		{
			pAttr += 10;
			char* pQuote = strchr(pAttr, '"');
			if (pQuote)
			{
				int len = (int)(pQuote - pAttr);
				if (len > 0 && len < 31)
				{
					memcpy(stage.szCodeName, pAttr, len);
					stage.szCodeName[len] = '\0';
				}
			}
		}

		if (stage.ui32StageID > 0)
			m_i32StageCount++;

		pCursor = pEnd + 1;
	}

	delete[] pBuffer;
	return (m_i32StageCount > 0);
}

// ============================================================================
// Query methods
// ============================================================================

const WeaponConfigData* ConfigXML::GetWeaponConfig(uint32_t ui32ItemID) const
{
	for (int i = 0; i < m_i32WeaponCount; i++)
	{
		if (m_Weapons[i].ui32ItemID == ui32ItemID)
			return &m_Weapons[i];
	}
	// Return default rifle if not found
	return (m_i32WeaponCount > 1) ? &m_Weapons[1] : nullptr;
}

float ConfigXML::GetWeaponDamage(uint32_t ui32ItemID) const
{
	const WeaponConfigData* pWep = GetWeaponConfig(ui32ItemID);
	return pWep ? pWep->fDamage : 30.0f;
}

float ConfigXML::GetWeaponRange(uint32_t ui32ItemID) const
{
	const WeaponConfigData* pWep = GetWeaponConfig(ui32ItemID);
	return pWep ? pWep->fRange : 5000.0f;
}

const CharacterConfigData* ConfigXML::GetCharacterConfig(uint32_t ui32CharID) const
{
	for (int i = 0; i < m_i32CharacterCount; i++)
	{
		if (m_Characters[i].ui32CharID == ui32CharID)
			return &m_Characters[i];
	}
	// Return default character
	return (m_i32CharacterCount > 0) ? &m_Characters[0] : nullptr;
}

int ConfigXML::GetCharacterMaxHP(uint32_t ui32CharID) const
{
	const CharacterConfigData* pChr = GetCharacterConfig(ui32CharID);
	return pChr ? pChr->i32MaxHP : 100;
}

float ConfigXML::GetCharacterMoveSpeed(uint32_t ui32CharID) const
{
	const CharacterConfigData* pChr = GetCharacterConfig(ui32CharID);
	return pChr ? pChr->fMoveSpeed : 300.0f;
}

const StageConfigData* ConfigXML::GetStageConfig(uint32_t ui32StageID) const
{
	for (int i = 0; i < m_i32StageCount; i++)
	{
		if (m_Stages[i].ui32StageID == ui32StageID)
			return &m_Stages[i];
	}
	return nullptr;
}

const char* ConfigXML::GetStageCodeName(uint32_t ui32StageID) const
{
	const StageConfigData* pStage = GetStageConfig(ui32StageID);
	return pStage ? pStage->szCodeName : nullptr;
}
