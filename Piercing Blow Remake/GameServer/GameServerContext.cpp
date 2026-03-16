#include "pch.h"
#include "GameServerContext.h"
#include "GameSessionManager.h"
#include "GameContextMain.h"
#include "RoomManager.h"
#include "ModuleConnectServer.h"
#include "ModuleDataServer.h"
#include "i3IniParser.h"

// ============================================================================
// GameServerContext
// ============================================================================

I3_CLASS_INSTANCE(GameServerContext);

GameServerContext* g_pGameServerContext = nullptr;

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

	// Set global
	g_pGameServerContext = this;

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

	return i3NetworkServerContext::OnDestroy();
}

// ============================================================================
// GameServer (BaseServer wrapper)
// ============================================================================

GameServer::GameServer()
	: m_pGameContext(nullptr)
	, m_pModuleConnect(nullptr)
	, m_pModuleData(nullptr)
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

	if (!pCtxMain->Create())
	{
		delete pCtxMain;
		return false;
	}

	g_pContextMain = pCtxMain;

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

	return true;
}
