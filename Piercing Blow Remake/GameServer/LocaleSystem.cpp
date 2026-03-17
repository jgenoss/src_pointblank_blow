#include "pch.h"
#include "LocaleSystem.h"
#include "ServerLog.h"

// ============================================================================
// Multi-Locale Support Implementation
// ============================================================================

LocaleSystem* g_pLocaleSystem = nullptr;

// ============================================================================
// Locale name table
// ============================================================================

static const char* s_LocaleNames[LOCALE_COUNT] =
{
	"Korea",		// LOCALE_KOREA
	"China",		// LOCALE_CHINA
	"Thailand",		// LOCALE_THAILAND
	"Indonesia",	// LOCALE_INDONESIA
	"Russia",		// LOCALE_RUSSIA
	"Turkey",		// LOCALE_TURKEY
	"Brazil",		// LOCALE_BRAZIL
	"LatAm",		// LOCALE_LATAM
	"NorthAmerica",	// LOCALE_NA
	"Italy",		// LOCALE_ITALY
	"MiddleEast",	// LOCALE_MIDDLE_EAST
	"Philippines",	// LOCALE_PHILIPPINES
	"Taiwan",		// LOCALE_TAIWAN
	"Japan",		// LOCALE_JAPAN
	"Europe",		// LOCALE_EUROPE
	"India",		// LOCALE_INDIA
	"Vietnam",		// LOCALE_VIETNAM
	"Malaysia",		// LOCALE_MALAYSIA
	"Singapore",	// LOCALE_SINGAPORE
	"Australia",	// LOCALE_AUSTRALIA
	"Global",		// LOCALE_GLOBAL
	"Test",			// LOCALE_TEST
};

static const char* s_RegionCodes[LOCALE_COUNT] =
{
	"KR", "CN", "TH", "ID", "RU", "TR", "BR", "LA",
	"NA", "IT", "ME", "PH", "TW", "JP", "EU", "IN",
	"VN", "MY", "SG", "AU", "GL", "TS",
};

// ============================================================================
// Constructor / Destructor
// ============================================================================

LocaleSystem::LocaleSystem()
	: m_eCurrentLocale(LOCALE_GLOBAL)
	, m_i32StringCount(0)
{
	memset(m_StringTable, 0, sizeof(m_StringTable));
}

LocaleSystem::~LocaleSystem()
{
}

// ============================================================================
// Initialize
// ============================================================================

bool LocaleSystem::Initialize()
{
	InitializeDefaults();
	printf("[LocaleSystem] Initialized with %d locale configurations\n", LOCALE_COUNT);
	return true;
}

// ============================================================================
// InitializeDefaults - set up all 22 regions with sensible defaults
// ============================================================================

void LocaleSystem::InitializeDefaults()
{
	//                        Locale              Name             Code  Currency  Rate  AAS    AgeV   MinAge PCCafe
	SetupLocaleDefaults(LOCALE_KOREA,        "Korea",           "KR", "Won",   1000.0f, false, true,  18, true);
	SetupLocaleDefaults(LOCALE_CHINA,        "China",           "CN", "CNY",   7.0f,    true,  true,  18, true);
	SetupLocaleDefaults(LOCALE_THAILAND,     "Thailand",        "TH", "THB",   33.0f,   false, false, 0,  true);
	SetupLocaleDefaults(LOCALE_INDONESIA,    "Indonesia",       "ID", "IDR",   15000.0f,false, false, 0,  true);
	SetupLocaleDefaults(LOCALE_RUSSIA,       "Russia",          "RU", "RUB",   90.0f,   false, false, 0,  false);
	SetupLocaleDefaults(LOCALE_TURKEY,       "Turkey",          "TR", "TRY",   30.0f,   false, false, 0,  false);
	SetupLocaleDefaults(LOCALE_BRAZIL,       "Brazil",          "BR", "R$",    5.0f,    false, false, 0,  false);
	SetupLocaleDefaults(LOCALE_LATAM,        "LatAm",           "LA", "USD",   1.0f,    false, false, 0,  false);
	SetupLocaleDefaults(LOCALE_NA,           "NorthAmerica",    "NA", "$",     1.0f,    false, false, 0,  false);
	SetupLocaleDefaults(LOCALE_ITALY,        "Italy",           "IT", "EUR",   0.9f,    false, false, 0,  false);
	SetupLocaleDefaults(LOCALE_MIDDLE_EAST,  "MiddleEast",      "ME", "SAR",   3.75f,   false, false, 0,  false);
	SetupLocaleDefaults(LOCALE_PHILIPPINES,  "Philippines",     "PH", "PHP",   56.0f,   false, false, 0,  true);
	SetupLocaleDefaults(LOCALE_TAIWAN,       "Taiwan",          "TW", "TWD",   31.0f,   false, false, 0,  true);
	SetupLocaleDefaults(LOCALE_JAPAN,        "Japan",           "JP", "JPY",   150.0f,  false, false, 0,  true);
	SetupLocaleDefaults(LOCALE_EUROPE,       "Europe",          "EU", "EUR",   0.9f,    false, false, 0,  false);
	SetupLocaleDefaults(LOCALE_INDIA,        "India",           "IN", "INR",   83.0f,   false, false, 0,  false);
	SetupLocaleDefaults(LOCALE_VIETNAM,      "Vietnam",         "VN", "VND",   24000.0f,true,  false, 0,  true);
	SetupLocaleDefaults(LOCALE_MALAYSIA,     "Malaysia",        "MY", "MYR",   4.7f,    false, false, 0,  true);
	SetupLocaleDefaults(LOCALE_SINGAPORE,    "Singapore",       "SG", "SGD",   1.35f,   false, false, 0,  false);
	SetupLocaleDefaults(LOCALE_AUSTRALIA,    "Australia",       "AU", "AUD",   1.5f,    false, false, 0,  false);
	SetupLocaleDefaults(LOCALE_GLOBAL,       "Global",          "GL", "$",     1.0f,    false, false, 0,  false);
	SetupLocaleDefaults(LOCALE_TEST,         "Test",            "TS", "$",     1.0f,    false, false, 0,  false);

	// China: strict AAS with force logout and all features except some
	m_Configs[LOCALE_CHINA].bFeatureFlags[FEATURE_AAS] = true;
	m_Configs[LOCALE_CHINA].bFeatureFlags[FEATURE_AGE_VERIFY] = true;

	// Korea: full feature set
	for (int i = 0; i < FEATURE_COUNT; ++i)
		m_Configs[LOCALE_KOREA].bFeatureFlags[i] = true;
	m_Configs[LOCALE_KOREA].bFeatureFlags[FEATURE_AAS] = false;

	// Enable common features for all locales
	for (int loc = 0; loc < LOCALE_COUNT; ++loc)
	{
		m_Configs[loc].bFeatureFlags[FEATURE_CLAN_WAR] = true;
		m_Configs[loc].bFeatureFlags[FEATURE_RANKING_SYSTEM] = true;
		m_Configs[loc].bFeatureFlags[FEATURE_TITLE_SYSTEM] = true;
		m_Configs[loc].bFeatureFlags[FEATURE_MEDAL_SYSTEM] = true;
		m_Configs[loc].bFeatureFlags[FEATURE_ROULETTE] = true;
		m_Configs[loc].bFeatureFlags[FEATURE_CASH_SHOP] = true;
		m_Configs[loc].bFeatureFlags[FEATURE_GM_TOOLS] = true;
		m_Configs[loc].bFeatureFlags[FEATURE_SPECTATOR] = true;
	}

	// Test locale: everything enabled
	for (int i = 0; i < FEATURE_COUNT; ++i)
		m_Configs[LOCALE_TEST].bFeatureFlags[i] = true;
}

// ============================================================================
// SetupLocaleDefaults
// ============================================================================

void LocaleSystem::SetupLocaleDefaults(GameLocale eLocale, const char* pszName,
										const char* pszCode, const char* pszCurrency,
										float fExchangeRate, bool bAAS, bool bAgeVerify,
										uint8_t ui8MinAge, bool bPCCafe)
{
	LocaleConfig& cfg = m_Configs[eLocale];
	cfg.eLocale = eLocale;
	strncpy_s(cfg.szName, pszName, _TRUNCATE);
	strncpy_s(cfg.szRegionCode, pszCode, _TRUNCATE);
	strncpy_s(cfg.szCurrencySymbol, pszCurrency, _TRUNCATE);
	cfg.fCashExchangeRate = fExchangeRate;
	cfg.bAASEnabled = bAAS;
	cfg.bAgeVerification = bAgeVerify;
	cfg.ui8MinAge = ui8MinAge;
	cfg.bPCCafeBonus = bPCCafe;

	// Default string table path
	_snprintf_s(cfg.szStringTablePath, _TRUNCATE, "data/strings/%s/strings.txt", pszCode);

	// Enable feature flags based on AAS/AgeVerify
	cfg.bFeatureFlags[FEATURE_AAS] = bAAS;
	cfg.bFeatureFlags[FEATURE_AGE_VERIFY] = bAgeVerify;
	cfg.bFeatureFlags[FEATURE_PCCAFE_BONUS] = bPCCafe;
}

// ============================================================================
// LoadLocaleConfig - load from INI file
// ============================================================================

bool LocaleSystem::LoadLocaleConfig(const char* pszPath)
{
	// INI parsing using i3IniParser (already used in GameServerContext)
	// Expected format:
	// [Locale]
	// Region=KR
	// AAS=0
	// AgeVerify=1
	// MinAge=18
	// PCCafe=1
	// Currency=Won
	// ExchangeRate=1000.0

	// For now, use the path to determine which region code to apply
	// Full INI parsing would use i3IniParser from the i3 engine

	printf("[LocaleSystem] LoadLocaleConfig: %s\n", pszPath);

	// Already initialized with defaults - config file overrides would go here
	// In a full implementation, parse the INI and override m_Configs entries

	return true;
}

// ============================================================================
// LoadStringTable
// ============================================================================

bool LocaleSystem::LoadStringTable(const char* pszPath)
{
	if (!pszPath || pszPath[0] == '\0')
		return false;

	printf("[LocaleSystem] Loading string table: %s\n", pszPath);

	// String table format: one entry per line
	// StringID|Text
	// Example:
	// 1001|Welcome to the server!
	// 1002|You have been disconnected.

	FILE* pFile = nullptr;
	fopen_s(&pFile, pszPath, "r");
	if (!pFile)
	{
		printf("[LocaleSystem] WARNING: Cannot open string table: %s\n", pszPath);
		// Not fatal - will use empty strings
		return false;
	}

	m_i32StringCount = 0;
	char szLine[MAX_LOCALE_STRING_LEN + 16];

	while (fgets(szLine, sizeof(szLine), pFile) && m_i32StringCount < MAX_LOCALE_STRINGS)
	{
		// Skip comments and empty lines
		if (szLine[0] == '#' || szLine[0] == '\n' || szLine[0] == '\r')
			continue;

		// Remove trailing newline
		int len = (int)strlen(szLine);
		while (len > 0 && (szLine[len - 1] == '\n' || szLine[len - 1] == '\r'))
			szLine[--len] = '\0';

		// Find separator
		char* pSep = strchr(szLine, '|');
		if (!pSep)
			continue;

		*pSep = '\0';
		int i32Id = atoi(szLine);
		const char* pszText = pSep + 1;

		if (i32Id >= 0 && i32Id < MAX_LOCALE_STRINGS)
		{
			m_StringTable[i32Id].i32StringId = i32Id;
			strncpy_s(m_StringTable[i32Id].szText, pszText, _TRUNCATE);
			m_StringTable[i32Id].bUsed = true;
			m_i32StringCount++;
		}
	}

	fclose(pFile);

	printf("[LocaleSystem] Loaded %d strings from %s\n", m_i32StringCount, pszPath);
	return true;
}

// ============================================================================
// SetCurrentLocale
// ============================================================================

void LocaleSystem::SetCurrentLocale(GameLocale eLocale)
{
	if (eLocale < 0 || eLocale >= LOCALE_COUNT)
	{
		printf("[LocaleSystem] WARNING: Invalid locale %d, using GLOBAL\n", eLocale);
		eLocale = LOCALE_GLOBAL;
	}

	m_eCurrentLocale = eLocale;

	printf("[LocaleSystem] Current locale set to: %s (%s)\n",
		m_Configs[eLocale].szName, m_Configs[eLocale].szRegionCode);

	// Load string table for new locale
	if (m_Configs[eLocale].szStringTablePath[0] != '\0')
	{
		LoadStringTable(m_Configs[eLocale].szStringTablePath);
	}
}

// ============================================================================
// GetString
// ============================================================================

const char* LocaleSystem::GetString(int i32StringId) const
{
	if (i32StringId < 0 || i32StringId >= MAX_LOCALE_STRINGS)
		return "";

	if (!m_StringTable[i32StringId].bUsed)
		return "";

	return m_StringTable[i32StringId].szText;
}

// ============================================================================
// IsFeatureEnabled
// ============================================================================

bool LocaleSystem::IsFeatureEnabled(int i32FeatureId) const
{
	return IsFeatureEnabled(m_eCurrentLocale, i32FeatureId);
}

bool LocaleSystem::IsFeatureEnabled(GameLocale eLocale, int i32FeatureId) const
{
	if (eLocale < 0 || eLocale >= LOCALE_COUNT)
		return false;

	if (i32FeatureId < 0 || i32FeatureId >= FEATURE_COUNT)
		return false;

	return m_Configs[eLocale].bFeatureFlags[i32FeatureId];
}

// ============================================================================
// Convenience accessors
// ============================================================================

bool LocaleSystem::IsAASRequired() const
{
	return m_Configs[m_eCurrentLocale].bAASEnabled;
}

bool LocaleSystem::IsAgeVerificationRequired() const
{
	return m_Configs[m_eCurrentLocale].bAgeVerification;
}

uint8_t LocaleSystem::GetMinAge() const
{
	return m_Configs[m_eCurrentLocale].ui8MinAge;
}

bool LocaleSystem::IsPCCafeEnabled() const
{
	return m_Configs[m_eCurrentLocale].bPCCafeBonus;
}

float LocaleSystem::GetCashExchangeRate() const
{
	return m_Configs[m_eCurrentLocale].fCashExchangeRate;
}

const char* LocaleSystem::GetCurrencySymbol() const
{
	return m_Configs[m_eCurrentLocale].szCurrencySymbol;
}

const char* LocaleSystem::GetRegionCode() const
{
	return m_Configs[m_eCurrentLocale].szRegionCode;
}

// ============================================================================
// Static helpers
// ============================================================================

const char* LocaleSystem::GetLocaleName(GameLocale eLocale)
{
	if (eLocale < 0 || eLocale >= LOCALE_COUNT)
		return "Unknown";
	return s_LocaleNames[eLocale];
}

GameLocale LocaleSystem::ParseLocaleFromRegionCode(const char* pszCode)
{
	if (!pszCode || pszCode[0] == '\0')
		return LOCALE_GLOBAL;

	for (int i = 0; i < LOCALE_COUNT; ++i)
	{
		if (_stricmp(pszCode, s_RegionCodes[i]) == 0)
			return (GameLocale)i;
	}

	return LOCALE_GLOBAL;
}
