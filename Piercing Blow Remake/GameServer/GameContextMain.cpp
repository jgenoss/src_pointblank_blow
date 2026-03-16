#include "pch.h"
#include "GameContextMain.h"

I3_CLASS_INSTANCE(GameContextMain);

GameContextMain* g_pContextMain = nullptr;

GameContextMain::GameContextMain()
	: m_i32ServerId(0)
	, m_i32SessionCount(0)
	, m_ui8ChannelCount(4)
	, m_ui16MaxUsersPerChannel(200)
	, m_ui16MaxRoomsPerChannel(200)
	, m_i16RSAKeySize(0)
	, m_i32UpdateTimeSS(0)
	, m_i32LastUpdateTime(0)
	, m_i32KillGPReward(50)
	, m_i32WinGPReward(200)
	, m_i32LoseGPReward(50)
	, m_i32KillExpReward(100)
	, m_i32WinExpReward(500)
	, m_i32LoseExpReward(100)
	, m_i32StartingGP(10000)
	, m_i32StartingCash(0)
	, m_i32DefaultTimeLimit(300)
	, m_i32DefaultMaxRound(7)
	, m_i32MinPlayersToStart(1)
	, m_i32AFKTimeout(0)
	, m_i32BoostEventCount(0)
{
	// Initialize boost events
	for (int i = 0; i < MAX_BOOST_EVENTS; i++)
		m_BoostEvents[i].Reset();

	// Default boost events (can be overridden by config)
	// Weekend double EXP
	m_BoostEvents[0].bActive = true;
	m_BoostEvents[0].ui8BoostType = BOOST_TYPE_EXP;
	m_BoostEvents[0].ui16Multiplier = 200;		// 2x EXP
	m_BoostEvents[0].ui32StartTime = 0;
	m_BoostEvents[0].ui32EndTime = 2400;
	m_BoostEvents[0].ui8DayOfWeek = 6;			// Saturday
	strcpy_s(m_BoostEvents[0].szDescription, "Weekend Double EXP (Sat)");

	m_BoostEvents[1].bActive = true;
	m_BoostEvents[1].ui8BoostType = BOOST_TYPE_EXP;
	m_BoostEvents[1].ui16Multiplier = 200;		// 2x EXP
	m_BoostEvents[1].ui32StartTime = 0;
	m_BoostEvents[1].ui32EndTime = 2400;
	m_BoostEvents[1].ui8DayOfWeek = 7;			// Sunday
	strcpy_s(m_BoostEvents[1].szDescription, "Weekend Double EXP (Sun)");

	m_i32BoostEventCount = 2;

	m_szServerName[0] = '\0';
	memset(m_pRSAKey, 0, sizeof(m_pRSAKey));

	// Initialize default channel layout
	for (int i = 0; i < MAX_GAME_CHANNELS; i++)
		m_Channels[i].Reset();

	// Default 4 channels: Normal, Beginner, Expert, Clan
	m_Channels[0].eType = GAME_CHANNEL_TYPE_NORMAL;
	strcpy_s(m_Channels[0].szName, "Normal");

	m_Channels[1].eType = GAME_CHANNEL_TYPE_BEGIN1;
	strcpy_s(m_Channels[1].szName, "Beginner");
	m_Channels[1].i32MaxLevel = 10;

	m_Channels[2].eType = GAME_CHANNEL_TYPE_EXPERT;
	strcpy_s(m_Channels[2].szName, "Expert");
	m_Channels[2].i32MinLevel = 20;

	m_Channels[3].eType = GAME_CHANNEL_TYPE_CLAN;
	strcpy_s(m_Channels[3].szName, "Clan");

	// Initialize rank EXP table (Point Blank military ranks)
	// Trainee(0) through General(51)
	memset(m_aRankExpTable, 0, sizeof(m_aRankExpTable));
	m_aRankExpTable[0]  = 0;		// Trainee
	m_aRankExpTable[1]  = 3000;		// Private
	m_aRankExpTable[2]  = 8000;		// Private First Class
	m_aRankExpTable[3]  = 15000;	// Corporal
	m_aRankExpTable[4]  = 25000;	// Sergeant
	m_aRankExpTable[5]  = 40000;	// Staff Sergeant
	m_aRankExpTable[6]  = 60000;	// Sergeant First Class
	m_aRankExpTable[7]  = 85000;	// Master Sergeant
	m_aRankExpTable[8]  = 115000;	// Sergeant Major
	m_aRankExpTable[9]  = 150000;	// Second Lieutenant
	m_aRankExpTable[10] = 200000;	// First Lieutenant
	m_aRankExpTable[11] = 260000;	// Captain
	m_aRankExpTable[12] = 330000;	// Major
	m_aRankExpTable[13] = 420000;	// Lieutenant Colonel
	m_aRankExpTable[14] = 530000;	// Colonel
	m_aRankExpTable[15] = 660000;	// Brigadier General
	m_aRankExpTable[16] = 820000;	// Major General
	m_aRankExpTable[17] = 1010000;	// Lieutenant General
	m_aRankExpTable[18] = 1240000;	// General
	// Beyond 18: each rank +300k
	for (int i = 19; i < MAX_RANK_COUNT; i++)
		m_aRankExpTable[i] = m_aRankExpTable[i - 1] + 300000 + (i - 18) * 50000;

	// Initialize map data with defaults
	m_i32MapCount = 0;
	m_ui32MapVersion = 1;
	for (int i = 0; i < MAX_MAP_COUNT; i++)
		m_Maps[i].Reset();

	// Default maps (common Point Blank maps)
	auto addMap = [this](uint32_t stageId, const char* name, uint8_t modes1, uint8_t modes2) {
		if (m_i32MapCount >= MAX_MAP_COUNT) return;
		int idx = m_i32MapCount++;
		m_Maps[idx].ui32StageId = stageId;
		strcpy_s(m_Maps[idx].szName, name);
		m_Maps[idx].ui8SupportedModes = modes1;
		m_Maps[idx].ui8SupportedModes2 = modes2;
		m_Maps[idx].bActive = true;
	};

	// stageId, name, mode bitmask (low=DM|Bomb|Annih|Destroy|Defence|Escape|Cross|Tut, high=Chall|Dino|Convoy|Run|Champ|Train|TDM|Zombie)
	addMap(1001, "Downtown",		0x07, 0x40);	// DM, Bomb, Annihilation, TDM
	addMap(1002, "Luxville",		0x07, 0x40);	// DM, Bomb, Annihilation, TDM
	addMap(1003, "Burning Hall",	0x07, 0x40);	// DM, Bomb, Annihilation, TDM
	addMap(1004, "Kick Point",		0x07, 0x40);	// DM, Bomb, Annihilation, TDM
	addMap(1005, "Storm Tube",		0x07, 0x40);	// DM, Bomb, Annihilation, TDM
	addMap(1006, "Mid Town",		0x07, 0x40);	// DM, Bomb, Annihilation, TDM
	addMap(1007, "Library",			0x07, 0x40);	// DM, Bomb, Annihilation, TDM
	addMap(1008, "Red Rock",		0x07, 0x40);	// DM, Bomb, Annihilation, TDM
	addMap(1009, "Base Camp",		0x0F, 0x40);	// DM, Bomb, Annihilation, Destroy, TDM
	addMap(1010, "Training Camp",	0x80, 0x20);	// Tutorial, Training
}

GameContextMain::~GameContextMain()
{
}

BOOL GameContextMain::Create()
{
	printf("[GameContextMain] Create - ServerId=%d, Name=%s, Channels=%d, MaxUsers/Ch=%d\n",
		m_i32ServerId, m_szServerName, m_ui8ChannelCount, m_ui16MaxUsersPerChannel);
	return TRUE;
}

void GameContextMain::OnUpdate()
{
	DWORD dwNow = GetTickCount();
	m_i32LastUpdateTime = (int)dwNow;
}

int64_t GameContextMain::GetRankExp(int rankId) const
{
	if (rankId < 0 || rankId >= MAX_RANK_COUNT)
		return 0;
	return m_aRankExpTable[rankId];
}

int GameContextMain::GetRankForExp(int64_t exp) const
{
	int rank = 0;
	for (int i = 1; i < MAX_RANK_COUNT; i++)
	{
		if (exp >= m_aRankExpTable[i])
			rank = i;
		else
			break;
	}
	return rank;
}

bool GameContextMain::IsMapValidForMode(int mapIdx, int mode) const
{
	if (mapIdx < 0 || mapIdx >= m_i32MapCount)
		return false;
	if (!m_Maps[mapIdx].bActive)
		return false;

	if (mode < 8)
		return (m_Maps[mapIdx].ui8SupportedModes & (1 << mode)) != 0;
	else if (mode < 16)
		return (m_Maps[mapIdx].ui8SupportedModes2 & (1 << (mode - 8))) != 0;
	return false;
}

// ============================================================================
// Boost Events (Phase 14B)
// ============================================================================

bool GameContextMain::IsBoostEventActive(int idx) const
{
	if (idx < 0 || idx >= m_i32BoostEventCount)
		return false;

	const GameBoostEvent& evt = m_BoostEvents[idx];
	if (!evt.bActive)
		return false;

	// Get current time
	SYSTEMTIME st;
	GetLocalTime(&st);

	// Check day of week (0=everyday, 1=Mon...7=Sun)
	if (evt.ui8DayOfWeek != 0)
	{
		// SYSTEMTIME.wDayOfWeek: 0=Sun, 1=Mon...6=Sat
		int dayMap = (st.wDayOfWeek == 0) ? 7 : st.wDayOfWeek;
		if (dayMap != evt.ui8DayOfWeek)
			return false;
	}

	// Check time range (HHMM format)
	uint32_t currentTime = st.wHour * 100 + st.wMinute;
	if (currentTime < evt.ui32StartTime || currentTime >= evt.ui32EndTime)
		return false;

	return true;
}

uint16_t GameContextMain::GetCurrentExpMultiplier() const
{
	uint16_t maxMult = 100;
	for (int i = 0; i < m_i32BoostEventCount; i++)
	{
		if (m_BoostEvents[i].ui8BoostType == BOOST_TYPE_EXP && IsBoostEventActive(i))
		{
			if (m_BoostEvents[i].ui16Multiplier > maxMult)
				maxMult = m_BoostEvents[i].ui16Multiplier;
		}
	}
	return maxMult;
}

uint16_t GameContextMain::GetCurrentPointMultiplier() const
{
	uint16_t maxMult = 100;
	for (int i = 0; i < m_i32BoostEventCount; i++)
	{
		if (m_BoostEvents[i].ui8BoostType == BOOST_TYPE_POINT && IsBoostEventActive(i))
		{
			if (m_BoostEvents[i].ui16Multiplier > maxMult)
				maxMult = m_BoostEvents[i].ui16Multiplier;
		}
	}
	return maxMult;
}
