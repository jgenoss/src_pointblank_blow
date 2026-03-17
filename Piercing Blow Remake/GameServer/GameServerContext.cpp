#include "pch.h"
#include "GameServerContext.h"
#include "GameSessionManager.h"
#include "GameContextMain.h"
#include "RoomManager.h"
#include "ModuleConnectServer.h"
#include "ModuleDataServer.h"
#include "ModuleBattleServer.h"
#include "ClanDef.h"
#include "ClanMatchManager.h"
#include "RouletteDef.h"
#include "ShopManager.h"
#include "ModuleControlAdmin.h"
#include "i3IniParser.h"
#include "ServerLog.h"

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

	// Update clan match manager (cleanup stale teams)
	if (g_pClanMatchManager)
		g_pClanMatchManager->Update();
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
	, m_pModuleControl(nullptr)
{
}

GameServer::~GameServer()
{
}

bool GameServer::OnLoadConfig(const char* pszConfigPath)
{
	// Defaults
	strcpy(m_GameConfig.szBindIP, "0.0.0.0");
	m_GameConfig.ui16BindPort			= 40000;
	m_GameConfig.i32MaxSessions			= 1000;
	m_GameConfig.i32WorkerThreadCount	= 8;
	m_GameConfig.ui8SocketTimeout		= 30;
	m_GameConfig.i32ServerId			= 1;
	strcpy(m_GameConfig.szServerName, "GameServer-1");
	strcpy(m_GameConfig.szPublicIP, "127.0.0.1");
	m_GameConfig.ui16PublicPort			= 40000;
	m_GameConfig.ui8ChannelCount		= 4;
	m_GameConfig.ui16MaxUsersPerChannel	= 200;
	m_GameConfig.ui16MaxRoomsPerChannel	= 200;
	strcpy(m_GameConfig.szConnectServerIP, "127.0.0.1");
	m_GameConfig.ui16ConnectServerPort	= 40001;
	strcpy(m_GameConfig.szDataServerIP, "127.0.0.1");
	m_GameConfig.ui16DataServerPort		= 40100;
	strcpy(m_GameConfig.szBattleServerIP, "127.0.0.1");
	m_GameConfig.ui16BattleServerPort	= 40200;
	m_GameConfig.bEnableControlAdmin	= true;
	strcpy(m_GameConfig.szControlAdminIP, "127.0.0.1");
	m_GameConfig.ui16ControlAdminPort	= 40500;
	m_GameConfig.ui16UdpClientPort		= 29890;
	m_GameConfig.ui16Port0				= 39190;

	i3IniParser ini;
	if (!ini.OpenFromFile(pszConfigPath))
	{
		printf("[GameServer] WARNING: Cannot load config '%s', using defaults\n", pszConfigPath);
	}
	else
	{
		INT32 nVal = 0;
		char  szVal[256];

		// [GameServer] section
		if (ini.ReadSection("GameServer"))
		{
			ini.GetValue("BindIP", "0.0.0.0", szVal, sizeof(szVal));
			strncpy_s(m_GameConfig.szBindIP, szVal, _TRUNCATE);

			ini.GetValue("BindPort", (INT32)40000, &nVal);
			m_GameConfig.ui16BindPort = (uint16_t)nVal;

			ini.GetValue("MaxSessions", (INT32)1000, &nVal);
			m_GameConfig.i32MaxSessions = nVal;

			ini.GetValue("WorkerThreads", (INT32)8, &nVal);
			m_GameConfig.i32WorkerThreadCount = nVal;

			ini.GetValue("SocketTimeout", (INT32)30, &nVal);
			m_GameConfig.ui8SocketTimeout = (uint8_t)nVal;

			ini.GetValue("ServerId", (INT32)1, &nVal);
			m_GameConfig.i32ServerId = nVal;

			ini.GetValue("ServerName", "GameServer-1", szVal, sizeof(szVal));
			strncpy_s(m_GameConfig.szServerName, szVal, _TRUNCATE);

			ini.GetValue("PublicIP", "127.0.0.1", szVal, sizeof(szVal));
			strncpy_s(m_GameConfig.szPublicIP, szVal, _TRUNCATE);

			ini.GetValue("PublicPort", (INT32)40000, &nVal);
			m_GameConfig.ui16PublicPort = (uint16_t)nVal;
		}

		// [Channels] section
		if (ini.ReadSection("Channels"))
		{
			ini.GetValue("ChannelCount", (INT32)4, &nVal);
			m_GameConfig.ui8ChannelCount = (uint8_t)nVal;

			ini.GetValue("MaxUsersPerChannel", (INT32)200, &nVal);
			m_GameConfig.ui16MaxUsersPerChannel = (uint16_t)nVal;

			ini.GetValue("MaxRoomsPerChannel", (INT32)200, &nVal);
			m_GameConfig.ui16MaxRoomsPerChannel = (uint16_t)nVal;
		}

		// [ConnectServer] section
		if (ini.ReadSection("ConnectServer"))
		{
			ini.GetValue("IP", "127.0.0.1", szVal, sizeof(szVal));
			strncpy_s(m_GameConfig.szConnectServerIP, szVal, _TRUNCATE);

			ini.GetValue("Port", (INT32)40001, &nVal);
			m_GameConfig.ui16ConnectServerPort = (uint16_t)nVal;
		}

		// [DataServer] section
		if (ini.ReadSection("DataServer"))
		{
			ini.GetValue("IP", "127.0.0.1", szVal, sizeof(szVal));
			strncpy_s(m_GameConfig.szDataServerIP, szVal, _TRUNCATE);

			ini.GetValue("Port", (INT32)40100, &nVal);
			m_GameConfig.ui16DataServerPort = (uint16_t)nVal;
		}

		// [BattleServer] section
		if (ini.ReadSection("BattleServer"))
		{
			ini.GetValue("IP", "127.0.0.1", szVal, sizeof(szVal));
			strncpy_s(m_GameConfig.szBattleServerIP, szVal, _TRUNCATE);

			ini.GetValue("Port", (INT32)40200, &nVal);
			m_GameConfig.ui16BattleServerPort = (uint16_t)nVal;
		}

		// [ControlAdmin] section
		if (ini.ReadSection("ControlAdmin"))
		{
			ini.GetValue("Enable", (INT32)1, &nVal);
			m_GameConfig.bEnableControlAdmin = (nVal != 0);

			ini.GetValue("BindIP", "127.0.0.1", szVal, sizeof(szVal));
			strncpy_s(m_GameConfig.szControlAdminIP, szVal, _TRUNCATE);

			ini.GetValue("BindPort", (INT32)40500, &nVal);
			m_GameConfig.ui16ControlAdminPort = (uint16_t)nVal;
		}

		// [Ports] section
		if (ini.ReadSection("Ports"))
		{
			ini.GetValue("UdpClientPort", (INT32)29890, &nVal);
			m_GameConfig.ui16UdpClientPort = (uint16_t)nVal;

			ini.GetValue("Port0", (INT32)39190, &nVal);
			m_GameConfig.ui16Port0 = (uint16_t)nVal;
		}
	}

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
		if (ini.OpenFromFile("config.ini"))
		{
			INT32 nVal = 0;

			if (ini.ReadSection("Economy"))
			{
				ini.GetValue("KillGPReward",      (INT32)50,    &nVal); pCtxMain->m_i32KillGPReward      = nVal;
				ini.GetValue("WinGPReward",       (INT32)200,   &nVal); pCtxMain->m_i32WinGPReward       = nVal;
				ini.GetValue("LoseGPReward",      (INT32)50,    &nVal); pCtxMain->m_i32LoseGPReward      = nVal;
				ini.GetValue("KillExpReward",     (INT32)100,   &nVal); pCtxMain->m_i32KillExpReward     = nVal;
				ini.GetValue("WinExpReward",      (INT32)500,   &nVal); pCtxMain->m_i32WinExpReward      = nVal;
				ini.GetValue("LoseExpReward",     (INT32)100,   &nVal); pCtxMain->m_i32LoseExpReward     = nVal;
				ini.GetValue("StartingGP",        (INT32)10000, &nVal); pCtxMain->m_i32StartingGP        = nVal;
				ini.GetValue("StartingCash",      (INT32)0,     &nVal); pCtxMain->m_i32StartingCash      = nVal;
				ini.GetValue("ClanCreateCostGP",  (INT32)50000, &nVal); pCtxMain->m_i32ClanCreateCostGP  = nVal;
				ini.GetValue("ClanCreateMinLevel",(INT32)5,     &nVal); pCtxMain->m_i32ClanCreateMinLevel = nVal;
			}

			if (ini.ReadSection("Battle"))
			{
				ini.GetValue("DefaultTimeLimit",  (INT32)300, &nVal); pCtxMain->m_i32DefaultTimeLimit  = nVal;
				ini.GetValue("DefaultMaxRound",   (INT32)7,   &nVal); pCtxMain->m_i32DefaultMaxRound   = nVal;
				ini.GetValue("MinPlayersToStart", (INT32)1,   &nVal); pCtxMain->m_i32MinPlayersToStart = nVal;
				ini.GetValue("AFKTimeout",        (INT32)0,   &nVal); pCtxMain->m_i32AFKTimeout        = nVal;
			}

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

	// Initialize persistent logging
	ServerLog::GetInstance()->Initialize(m_GameConfig.szServerName, "logs");
	SLOG_INFO("GameServer initializing - Id=%d, Name=%s",
		m_GameConfig.i32ServerId, m_GameConfig.szServerName);

	// Create shop manager
	g_pShopManager = new ShopManager();

	// Create clan manager
	g_pClanManager = new GameClanManager();

	// Create clan match manager
	g_pClanMatchManager = new ClanMatchManager();

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

	if (m_pModuleControl)
	{
		m_pModuleControl->Shutdown();
		delete m_pModuleControl;
		m_pModuleControl = nullptr;
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

	// Cleanup clan match manager
	if (g_pClanMatchManager)
	{
		delete g_pClanMatchManager;
		g_pClanMatchManager = nullptr;
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

	SLOG_INFO("GameServer shutdown complete");
	ServerLog::GetInstance()->Shutdown();

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

	// Initialize ModuleControlAdmin (integrated admin TCP interface)
	if (m_GameConfig.bEnableControlAdmin)
	{
		m_pModuleControl = new ModuleControlAdmin();
		if (!m_pModuleControl->Initialize(m_GameConfig.szControlAdminIP,
			m_GameConfig.ui16ControlAdminPort))
		{
			printf("[GameServer] WARNING: ModuleControlAdmin initialization failed\n");
			delete m_pModuleControl;
			m_pModuleControl = nullptr;
		}
		else
		{
			// Load admin accounts from config
			i3IniParser ini;
			if (ini.OpenFromFile(m_szConfigPath))
			{
				for (int i = 0; i < MAX_ADMIN_ACCOUNTS; ++i)
				{
					char szSection[32];
					snprintf(szSection, sizeof(szSection), "Admin.%d", i);
					if (!ini.ReadSection(szSection))
						break;
					char  szUser[64] = {0};
					char  szPass[64] = {0};
					INT32 nLevel     = -1;
					ini.GetValue("Username", "", szUser, sizeof(szUser));
					ini.GetValue("Password", "", szPass, sizeof(szPass));
					ini.GetValue("Level", (INT32)-1, &nLevel);
					if (szUser[0] && szPass[0] && nLevel >= 0)
						m_pModuleControl->AddAdminAccount(szUser, szPass, (uint8_t)nLevel);
					else
						break;
				}
			}
			// Add default dev account if none configured
			uint8_t dummyLevel = 0;
			if (!m_pModuleControl->ValidateAdmin("admin", "admin", &dummyLevel))
			{
				printf("[GameServer] No admin accounts found, adding default admin/admin (level=3)\n");
				m_pModuleControl->AddAdminAccount("admin", "admin", 3);
			}
			printf("[GameServer] ControlAdmin module ready on %s:%d\n",
				m_GameConfig.szControlAdminIP, m_GameConfig.ui16ControlAdminPort);
		}
	}

	return true;
}

bool GameServer::ReloadEconomyConfig()
{
	if (!g_pContextMain)
		return false;

	i3IniParser ini;
	if (!ini.OpenFromFile("config.ini"))
	{
		printf("[GameServer] ReloadConfig: Cannot load config.ini\n");
		return false;
	}

	INT32 nVal = 0;

	// Reload economy
	if (ini.ReadSection("Economy"))
	{
		ini.GetValue("KillGPReward",      (INT32)50,    &nVal); g_pContextMain->m_i32KillGPReward      = nVal;
		ini.GetValue("WinGPReward",       (INT32)200,   &nVal); g_pContextMain->m_i32WinGPReward       = nVal;
		ini.GetValue("LoseGPReward",      (INT32)50,    &nVal); g_pContextMain->m_i32LoseGPReward      = nVal;
		ini.GetValue("KillExpReward",     (INT32)100,   &nVal); g_pContextMain->m_i32KillExpReward     = nVal;
		ini.GetValue("WinExpReward",      (INT32)500,   &nVal); g_pContextMain->m_i32WinExpReward      = nVal;
		ini.GetValue("LoseExpReward",     (INT32)100,   &nVal); g_pContextMain->m_i32LoseExpReward     = nVal;
		ini.GetValue("StartingGP",        (INT32)10000, &nVal); g_pContextMain->m_i32StartingGP        = nVal;
		ini.GetValue("StartingCash",      (INT32)0,     &nVal); g_pContextMain->m_i32StartingCash      = nVal;
		ini.GetValue("ClanCreateCostGP",  (INT32)50000, &nVal); g_pContextMain->m_i32ClanCreateCostGP  = nVal;
		ini.GetValue("ClanCreateMinLevel",(INT32)5,     &nVal); g_pContextMain->m_i32ClanCreateMinLevel = nVal;
	}

	// Reload battle config
	if (ini.ReadSection("Battle"))
	{
		ini.GetValue("DefaultTimeLimit",  (INT32)300, &nVal); g_pContextMain->m_i32DefaultTimeLimit  = nVal;
		ini.GetValue("DefaultMaxRound",   (INT32)7,   &nVal); g_pContextMain->m_i32DefaultMaxRound   = nVal;
		ini.GetValue("MinPlayersToStart", (INT32)1,   &nVal); g_pContextMain->m_i32MinPlayersToStart = nVal;
		ini.GetValue("AFKTimeout",        (INT32)0,   &nVal); g_pContextMain->m_i32AFKTimeout        = nVal;
	}

	printf("[GameServer] Config reloaded: KillGP=%d WinGP=%d LoseGP=%d TimeLimit=%d MaxRound=%d\n",
		g_pContextMain->m_i32KillGPReward, g_pContextMain->m_i32WinGPReward,
		g_pContextMain->m_i32LoseGPReward, g_pContextMain->m_i32DefaultTimeLimit,
		g_pContextMain->m_i32DefaultMaxRound);

	return true;
}
