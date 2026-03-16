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
{
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
