#ifndef __LOCALESYSTEM_H__
#define __LOCALESYSTEM_H__

#pragma once

// ============================================================================
// Multi-Locale Support System
// ============================================================================
// The original Piercing Blow/Point Blank supported 22 regions with different
// feature flags, age verification requirements, currency settings, and
// localized string tables per locale.
//
// This system controls per-locale behavior including:
// - Anti-addiction enforcement (China)
// - Age verification requirements
// - PC cafe bonuses
// - Cash shop currency conversion
// - Feature flags per region
// - Localized string lookup
// ============================================================================

// ============================================================================
// Locale Enum - 22 regions from original
// ============================================================================

enum GameLocale
{
	LOCALE_KOREA = 0,
	LOCALE_CHINA,
	LOCALE_THAILAND,
	LOCALE_INDONESIA,
	LOCALE_RUSSIA,
	LOCALE_TURKEY,
	LOCALE_BRAZIL,
	LOCALE_LATAM,
	LOCALE_NA,
	LOCALE_ITALY,
	LOCALE_MIDDLE_EAST,
	LOCALE_PHILIPPINES,
	LOCALE_TAIWAN,
	LOCALE_JAPAN,
	LOCALE_EUROPE,
	LOCALE_INDIA,
	LOCALE_VIETNAM,
	LOCALE_MALAYSIA,
	LOCALE_SINGAPORE,
	LOCALE_AUSTRALIA,
	LOCALE_GLOBAL,
	LOCALE_TEST,
	LOCALE_COUNT,
};

// ============================================================================
// Feature Flags - per-locale feature toggles
// ============================================================================

enum LocaleFeatureFlag
{
	FEATURE_AAS = 0,					// Anti-addiction system
	FEATURE_AGE_VERIFY,					// Age verification required
	FEATURE_PCCAFE_BONUS,				// PC cafe bonus system
	FEATURE_CLAN_WAR,					// Clan war system
	FEATURE_MERCENARY,					// Mercenary hiring
	FEATURE_TOURNAMENT,					// Tournament system
	FEATURE_ROULETTE,					// Roulette/gacha system
	FEATURE_GIFT_SHOP,					// Gift shop
	FEATURE_RANKING_SYSTEM,				// Ranking leaderboards
	FEATURE_TITLE_SYSTEM,				// Title/designation system
	FEATURE_MEDAL_SYSTEM,				// Medal system
	FEATURE_CROSSCOUNT_MODE,			// Dino deathmatch mode
	FEATURE_CONVOY_MODE,				// Convoy mode
	FEATURE_CHAMPIONSHIP_MODE,			// Championship mode
	FEATURE_VOICE_CHAT,					// In-game voice chat
	FEATURE_TRADE,						// Player-to-player trade
	FEATURE_COUPON,						// Coupon redemption
	FEATURE_CASH_SHOP,					// Real-money cash shop
	FEATURE_GM_TOOLS,					// GM command tools
	FEATURE_SPECTATOR,					// Spectator/observer mode
	FEATURE_COUNT,
};

// ============================================================================
// Locale Config - per-locale configuration
// ============================================================================

struct LocaleConfig
{
	GameLocale	eLocale;
	char		szName[32];						// Display name ("Korea", "China", etc.)
	bool		bAASEnabled;					// Anti-addiction required
	bool		bAgeVerification;				// Age verification required
	uint8_t		ui8MinAge;						// Minimum age (0 = no restriction)
	bool		bPCCafeBonus;					// PC cafe bonus enabled
	char		szCurrencySymbol[8];			// "$", "R$", "Won", etc.
	float		fCashExchangeRate;				// Real money to cash ratio
	char		szRegionCode[8];				// ISO region code "KR", "CN", "BR"
	char		szStringTablePath[128];			// Path to localized strings file
	bool		bFeatureFlags[FEATURE_COUNT];	// Per-feature enable/disable

	LocaleConfig()
		: eLocale(LOCALE_GLOBAL)
		, bAASEnabled(false)
		, bAgeVerification(false)
		, ui8MinAge(0)
		, bPCCafeBonus(false)
		, fCashExchangeRate(1.0f)
	{
		szName[0] = '\0';
		szCurrencySymbol[0] = '\0';
		szRegionCode[0] = '\0';
		szStringTablePath[0] = '\0';
		memset(bFeatureFlags, 0, sizeof(bFeatureFlags));
	}
};

// ============================================================================
// Localized String Table
// ============================================================================

#define MAX_LOCALE_STRINGS		2048
#define MAX_LOCALE_STRING_LEN	512

struct LocaleStringEntry
{
	int			i32StringId;
	char		szText[MAX_LOCALE_STRING_LEN];
	bool		bUsed;

	LocaleStringEntry()
		: i32StringId(0), bUsed(false)
	{
		szText[0] = '\0';
	}
};

// ============================================================================
// LocaleSystem - singleton manager
// ============================================================================

class LocaleSystem
{
public:
	LocaleSystem();
	~LocaleSystem();

	// Initialize locale configs with defaults for all 22 regions
	bool		Initialize();

	// Load locale config from INI file
	bool		LoadLocaleConfig(const char* pszPath);

	// Load string table for current locale
	bool		LoadStringTable(const char* pszPath);

	// Set current active locale
	void		SetCurrentLocale(GameLocale eLocale);

	// Get current locale
	GameLocale	GetCurrentLocale() const { return m_eCurrentLocale; }

	// Get locale config for current or specific locale
	const LocaleConfig& GetCurrentConfig() const { return m_Configs[m_eCurrentLocale]; }
	const LocaleConfig& GetConfig(GameLocale eLocale) const { return m_Configs[eLocale]; }

	// Localized string lookup
	const char*	GetString(int i32StringId) const;

	// Feature flag check for current locale
	bool		IsFeatureEnabled(int i32FeatureId) const;

	// Feature flag check for specific locale
	bool		IsFeatureEnabled(GameLocale eLocale, int i32FeatureId) const;

	// Convenience accessors for current locale
	bool		IsAASRequired() const;
	bool		IsAgeVerificationRequired() const;
	uint8_t		GetMinAge() const;
	bool		IsPCCafeEnabled() const;
	float		GetCashExchangeRate() const;
	const char*	GetCurrencySymbol() const;
	const char*	GetRegionCode() const;

	// Get locale name by enum
	static const char* GetLocaleName(GameLocale eLocale);

	// Parse locale from string ("KR", "CN", "BR", etc.)
	static GameLocale ParseLocaleFromRegionCode(const char* pszCode);

private:
	// Initialize default configs for all regions
	void		InitializeDefaults();

	// Set up a single locale config with default values
	void		SetupLocaleDefaults(GameLocale eLocale, const char* pszName,
									const char* pszCode, const char* pszCurrency,
									float fExchangeRate, bool bAAS, bool bAgeVerify,
									uint8_t ui8MinAge, bool bPCCafe);

private:
	GameLocale			m_eCurrentLocale;
	LocaleConfig		m_Configs[LOCALE_COUNT];
	LocaleStringEntry	m_StringTable[MAX_LOCALE_STRINGS];
	int					m_i32StringCount;
};

extern LocaleSystem* g_pLocaleSystem;

#endif // __LOCALESYSTEM_H__
