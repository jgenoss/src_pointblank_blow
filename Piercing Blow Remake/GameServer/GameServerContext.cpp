#include "pch.h"
#include "GameServerContext.h"
#include "GameSessionManager.h"
#include "GameContextMain.h"
#include "RoomManager.h"
#include "ModuleConnectServer.h"
#include "ModuleDataServer.h"
#include "ModuleBattleServer.h"
#include "ClanDef.h"
#include "RouletteDef.h"
#include "ShopManager.h"
#include "i3IniParser.h"

// ============================================================================
// GameServerContext
// ============================================================================

I3_CLASS_INSTANCE(GameServerContext);

GameServerContext* g_pGameServerContext = nullptr;
GameServer* g_pGameServer = nullptr;
GameSessionManager* g_pGameSessionManager = nullptr;
GameClanManager* g_pClanManager = nullptr;
GameRouletteData* g_pRouletteData = nullptr;
ModuleDataServer* g_pModuleDataServer = nullptr;
ModuleBattleServer* g_pModuleBattleServer = nullptr;

GameServerContext::GameServerContext()
	: m_pGameSessionManager(nullptr)
	, m_pRoomManager(nullptr)
{
}

GameServerContext::~GameServerContext()
{
}

BOOL GameServerContext::OnCreate(UINT8 SocketCount, UINT32* pAddress, UINT16* pPort,
								UINT8* pTimeOut, INT32 WorkCount,
								i3NetworkSessionManager* pSessionManager)
{
	// Cast session manager
	m_pGameSessionManager = static_cast<GameSessionManager*>(pSessionManager);

	// Create room manager
	m_pRoomManager = new RoomManager();
	m_pRoomManager->OnCreate();
	g_pRoomManager = m_pRoomManager;

	// Set globals
	g_pGameServerContext = this;
	g_pGameSessionManager = m_pGameSessionManager;

	BOOL result = i3NetworkServerContext::OnCreate(SocketCount, pAddress, pPort,
												   pTimeOut, WorkCount, pSessionManager);

	printf("[GameServerContext] OnCreate - Result=%d\n", result);
	return result;
}

void GameServerContext::OnUpdate(INT32 Command)
{
	i3NetworkServerContext::OnUpdate(Command);

	// Update room manager
	if (m_pRoomManager)
		m_pRoomManager->OnUpdate();

	// Update context main
	if (g_pContextMain)
		g_pContextMain->OnUpdate();
}

BOOL GameServerContext::OnDestroy()
{
	if (m_pRoomManager)
	{
		m_pRoomManager->OnDestroy();
		delete m_pRoomManager;
		m_pRoomManager = nullptr;
		g_pRoomManager = nullptr;
	}

	g_pGameServerContext = nullptr;
	g_pGameSessionManager = nullptr;

	return i3NetworkServerContext::OnDestroy();
}

// ============================================================================
// GameServer (BaseServer wrapper)
// ============================================================================

GameServer::GameServer()
	: m_pGameContext(nullptr)
	, m_pModuleConnect(nullptr)
	, m_pModuleData(nullptr)
	, m_pModuleBattle(nullptr)
{
}

GameServer::~GameServer()
{
}

bool GameServer::OnLoadConfig(const char* pszConfigPath)
{
	i3IniParser ini;
	if (!ini.Load(pszConfigPath))
	{
		printf("[GameServer] ERROR: Cannot load config: %s\n", pszConfigPath);
		return false;
	}

	// [GameServer] section
	const char* pszBindIP = ini.GetString("GameServer", "BindIP", "0.0.0.0");
	strncpy_s(m_GameConfig.szBindIP, pszBindIP, _TRUNCATE);
	m_GameConfig.ui16BindPort = (uint16_t)ini.GetInt("GameServer", "BindPort", 40000);
	m_GameConfig.i32MaxSessions = ini.GetInt("GameServer", "MaxSessions", 1000);
	m_GameConfig.i32WorkerThreadCount = ini.GetInt("GameServer", "WorkerThreads", 8);
	m_GameConfig.ui8SocketTimeout = (uint8_t)ini.GetInt("GameServer", "SocketTimeout", 30);
	m_GameConfig.i32ServerId = ini.GetInt("GameServer", "ServerId", 1);

	const char* pszName = ini.GetString("GameServer", "ServerName", "GameServer-1");
	strncpy_s(m_GameConfig.szServerName, pszName, _TRUNCATE);

	const char* pszPublicIP = ini.GetString("GameServer", "PublicIP", "127.0.0.1");
	strncpy_s(m_GameConfig.szPublicIP, pszPublicIP, _TRUNCATE);
	m_GameConfig.ui16PublicPort = (uint16_t)ini.GetInt("GameServer", "PublicPort", 40000);

	// [Channels] section
	m_GameConfig.ui8ChannelCount = (uint8_t)ini.GetInt("Channels", "ChannelCount", 4);
	m_GameConfig.ui16MaxUsersPerChannel = (uint16_t)ini.GetInt("Channels", "MaxUsersPerChannel", 200);
	m_GameConfig.ui16MaxRoomsPerChannel = (uint16_t)ini.GetInt("Channels", "MaxRoomsPerChannel", 200);

	// [ConnectServer] section
	const char* pszCSIP = ini.GetString("ConnectServer", "IP", "127.0.0.1");
	strncpy_s(m_GameConfig.szConnectServerIP, pszCSIP, _TRUNCATE);
	m_GameConfig.ui16ConnectServerPort = (uint16_t)ini.GetInt("ConnectServer", "Port", 40001);

	// [DataServer] section
	const char* pszDSIP = ini.GetString("DataServer", "IP", "127.0.0.1");
	strncpy_s(m_GameConfig.szDataServerIP, pszDSIP, _TRUNCATE);
	m_GameConfig.ui16DataServerPort = (uint16_t)ini.GetInt("DataServer", "Port", 40100);

	// [BattleServer] section
	const char* pszBSIP = ini.GetString("BattleServer", "IP", "127.0.0.1");
	strncpy_s(m_GameConfig.szBattleServerIP, pszBSIP, _TRUNCATE);
	m_GameConfig.ui16BattleServerPort = (uint16_t)ini.GetInt("BattleServer", "Port", 40200);

	// Copy base config
	m_Config = m_GameConfig;

	printf("[GameServer] Config loaded:\n");
	printf("  Bind: %s:%d, MaxSessions=%d, Workers=%d\n",
		m_GameConfig.szBindIP, m_GameConfig.ui16BindPort,
		m_GameConfig.i32MaxSessions, m_GameConfig.i32WorkerThreadCount);
	printf("  Server: Id=%d, Name=%s, Public=%s:%d\n",
		m_GameConfig.i32ServerId, m_GameConfig.szServerName,
		m_GameConfig.szPublicIP, m_GameConfig.ui16PublicPort);
	printf("  Channels: %d, MaxUsers/Ch=%d, MaxRooms/Ch=%d\n",
		m_GameConfig.ui8ChannelCount, m_GameConfig.ui16MaxUsersPerChannel,
		m_GameConfig.ui16MaxRoomsPerChannel);
	printf("  ConnectServer: %s:%d\n", m_GameConfig.szConnectServerIP, m_GameConfig.ui16ConnectServerPort);
	printf("  DataServer: %s:%d\n", m_GameConfig.szDataServerIP, m_GameConfig.ui16DataServerPort);
	printf("  BattleServer: %s:%d\n", m_GameConfig.szBattleServerIP, m_GameConfig.ui16BattleServerPort);

	return true;
}

bool GameServer::OnInitialize()
{
	// Create and initialize GameContextMain
	GameContextMain* pCtxMain = new GameContextMain();
	pCtxMain->m_i32ServerId = m_GameConfig.i32ServerId;
	strncpy_s(pCtxMain->m_szServerName, m_GameConfig.szServerName, _TRUNCATE);
	pCtxMain->m_i32SessionCount = m_GameConfig.i32MaxSessions;
	pCtxMain->m_ui8ChannelCount = m_GameConfig.ui8ChannelCount;
	pCtxMain->m_ui16MaxUsersPerChannel = m_GameConfig.ui16MaxUsersPerChannel;
	pCtxMain->m_ui16MaxRoomsPerChannel = m_GameConfig.ui16MaxRoomsPerChannel;

	// Load Economy config
	{
		i3IniParser ini;
		if (ini.Load("config.ini"))
		{
			pCtxMain->m_i32KillGPReward = ini.GetInt("Economy", "KillGPReward", 50);
			pCtxMain->m_i32WinGPReward = ini.GetInt("Economy", "WinGPReward", 200);
			pCtxMain->m_i32LoseGPReward = ini.GetInt("Economy", "LoseGPReward", 50);
			pCtxMain->m_i32KillExpReward = ini.GetInt("Economy", "KillExpReward", 100);
			pCtxMain->m_i32WinExpReward = ini.GetInt("Economy", "WinExpReward", 500);
			pCtxMain->m_i32LoseExpReward = ini.GetInt("Economy", "LoseExpReward", 100);
			pCtxMain->m_i32StartingGP = ini.GetInt("Economy", "StartingGP", 10000);
			pCtxMain->m_i32StartingCash = ini.GetInt("Economy", "StartingCash", 0);

			pCtxMain->m_i32DefaultTimeLimit = ini.GetInt("Battle", "DefaultTimeLimit", 300);
			pCtxMain->m_i32DefaultMaxRound = ini.GetInt("Battle", "DefaultMaxRound", 7);
			pCtxMain->m_i32MinPlayersToStart = ini.GetInt("Battle", "MinPlayersToStart", 1);
			pCtxMain->m_i32AFKTimeout = ini.GetInt("Battle", "AFKTimeout", 0);

			printf("[GameServer] Economy config: KillGP=%d, WinGP=%d, LoseGP=%d, StartGP=%d\n",
				pCtxMain->m_i32KillGPReward, pCtxMain->m_i32WinGPReward,
				pCtxMain->m_i32LoseGPReward, pCtxMain->m_i32StartingGP);
			printf("[GameServer] Battle config: TimeLimit=%d, MaxRound=%d, MinPlayers=%d\n",
				pCtxMain->m_i32DefaultTimeLimit, pCtxMain->m_i32DefaultMaxRound,
				pCtxMain->m_i32MinPlayersToStart);
		}
	}

	if (!pCtxMain->Create())
	{
		delete pCtxMain;
		return false;
	}

	g_pContextMain = pCtxMain;

	// Create shop manager
	g_pShopManager = new ShopManager();

	// Create clan manager
	g_pClanManager = new GameClanManager();

	// Create roulette data with defaults
	g_pRouletteData = new GameRouletteData();
	g_pRouletteData->InitDefaults();

	printf("[GameServer] Initialized successfully\n");
	return true;
}

i3NetworkServerContext* GameServer::OnCreateContext()
{
	m_pGameContext = new GameServerContext();
	return m_pGameContext;
}

i3NetworkSessionManager* GameServer::OnCreateSessionManager()
{
	return new GameSessionManager();
}

void GameServer::OnUpdate()
{
	// Inter-server module updates are handled by their own threads
}

void GameServer::OnShutdown()
{
	// Shutdown inter-server modules
	if (m_pModuleConnect)
	{
		m_pModuleConnect->Destroy();
		delete m_pModuleConnect;
		m_pModuleConnect = nullptr;
	}

	if (m_pModuleData)
	{
		m_pModuleData->Destroy();
		delete m_pModuleData;
		m_pModuleData = nullptr;
		g_pModuleDataServer = nullptr;
	}

	if (m_pModuleBattle)
	{
		m_pModuleBattle->Destroy();
		delete m_pModuleBattle;
		m_pModuleBattle = nullptr;
		g_pModuleBattleServer = nullptr;
	}

	// Cleanup shop manager
	if (g_pShopManager)
	{
		delete g_pShopManager;
		g_pShopManager = nullptr;
	}

	// Cleanup roulette data
	if (g_pRouletteData)
	{
		delete g_pRouletteData;
		g_pRouletteData = nullptr;
	}

	// Cleanup clan manager
	if (g_pClanManager)
	{
		delete g_pClanManager;
		g_pClanManager = nullptr;
	}

	// Cleanup global context
	if (g_pContextMain)
	{
		delete g_pContextMain;
		g_pContextMain = nullptr;
	}

	printf("[GameServer] Shutdown complete\n");
}

bool GameServer::InitializeModules()
{
	// Initialize ModuleConnectServer
	m_pModuleConnect = new ModuleConnectServer();
	if (!m_pModuleConnect->Initialize("ModuleConnectServer",
		m_GameConfig.szConnectServerIP, m_GameConfig.ui16ConnectServerPort,
		m_GameConfig.i32WorkerThreadCount))
	{
		printf("[GameServer] WARNING: ModuleConnectServer initialization failed\n");
	}
	else
	{
		m_pModuleConnect->SetServerInfo(
			m_GameConfig.i32ServerId,
			m_GameConfig.szServerName,
			m_GameConfig.szPublicIP,
			m_GameConfig.ui16PublicPort,
			m_GameConfig.i32MaxSessions);
	}

	// Initialize ModuleDataServer
	m_pModuleData = new ModuleDataServer();
	if (!m_pModuleData->Initialize("ModuleDataServer",
		m_GameConfig.szDataServerIP, m_GameConfig.ui16DataServerPort,
		m_GameConfig.i32WorkerThreadCount))
	{
		printf("[GameServer] WARNING: ModuleDataServer initialization failed\n");
	}
	g_pModuleDataServer = m_pModuleData;

	// Initialize ModuleBattleServer
	m_pModuleBattle = new ModuleBattleServer();
	if (!m_pModuleBattle->Initialize("ModuleBattleServer",
		m_GameConfig.szBattleServerIP, m_GameConfig.ui16BattleServerPort,
		m_GameConfig.i32WorkerThreadCount))
	{
		printf("[GameServer] WARNING: ModuleBattleServer initialization failed\n");
	}
	else
	{
		m_pModuleBattle->SetServerInfo(
			m_GameConfig.i32ServerId,
			m_GameConfig.szServerName,
			m_GameConfig.szPublicIP,
			m_GameConfig.ui16PublicPort,
			m_GameConfig.i32MaxSessions);
	}
	g_pModuleBattleServer = m_pModuleBattle;

	return true;
}

bool GameServer::ReloadEconomyConfig()
{
	if (!g_pContextMain)
		return false;

	i3IniParser ini;
	if (!ini.Load("config.ini"))
	{
		printf("[GameServer] ReloadConfig: Cannot load config.ini\n");
		return false;
	}

	// Reload economy
	g_pContextMain->m_i32KillGPReward = ini.GetInt("Economy", "KillGPReward", 50);
	g_pContextMain->m_i32WinGPReward = ini.GetInt("Economy", "WinGPReward", 200);
	g_pContextMain->m_i32LoseGPReward = ini.GetInt("Economy", "LoseGPReward", 50);
	g_pContextMain->m_i32KillExpReward = ini.GetInt("Economy", "KillExpReward", 100);
	g_pContextMain->m_i32WinExpReward = ini.GetInt("Economy", "WinExpReward", 500);
	g_pContextMain->m_i32LoseExpReward = ini.GetInt("Economy", "LoseExpReward", 100);
	g_pContextMain->m_i32StartingGP = ini.GetInt("Economy", "StartingGP", 10000);
	g_pContextMain->m_i32StartingCash = ini.GetInt("Economy", "StartingCash", 0);

	// Reload battle config
	g_pContextMain->m_i32DefaultTimeLimit = ini.GetInt("Battle", "DefaultTimeLimit", 300);
	g_pContextMain->m_i32DefaultMaxRound = ini.GetInt("Battle", "DefaultMaxRound", 7);
	g_pContextMain->m_i32MinPlayersToStart = ini.GetInt("Battle", "MinPlayersToStart", 1);
	g_pContextMain->m_i32AFKTimeout = ini.GetInt("Battle", "AFKTimeout", 0);

	printf("[GameServer] Config reloaded: KillGP=%d WinGP=%d LoseGP=%d TimeLimit=%d MaxRound=%d\n",
		g_pContextMain->m_i32KillGPReward, g_pContextMain->m_i32WinGPReward,
		g_pContextMain->m_i32LoseGPReward, g_pContextMain->m_i32DefaultTimeLimit,
		g_pContextMain->m_i32DefaultMaxRound);

	return true;
}
