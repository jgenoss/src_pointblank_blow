#include "pch.h"
#include "BattleServerContext.h"
#include "BattleSessionManager.h"
#include "BattleRoomManager.h"
#include "UdpRelay.h"
#include "MapManager.h"
#include "ConfigXML.h"
#include "i3IniParser.h"

// ============================================================================
// BattleServerContext
// ============================================================================

I3_CLASS_INSTANCE(BattleServerContext);

BattleServerContext* g_pBattleServerContext = nullptr;

BattleServerContext::BattleServerContext()
	: m_pBattleSessionManager(nullptr)
	, m_pBattleRoomManager(nullptr)
{
}

BattleServerContext::~BattleServerContext()
{
}

BOOL BattleServerContext::OnCreate(UINT8 SocketCount, UINT32* pAddress, UINT16* pPort,
								   UINT8* pTimeOut, INT32 WorkCount,
								   i3NetworkSessionManager* pSessionManager)
{
	m_pBattleSessionManager = static_cast<BattleSessionManager*>(pSessionManager);

	// Create room manager
	m_pBattleRoomManager = new BattleRoomManager();
	m_pBattleRoomManager->OnCreate();
	g_pBattleRoomManager = m_pBattleRoomManager;

	// Set global
	g_pBattleServerContext = this;

	BOOL result = i3NetworkServerContext::OnCreate(SocketCount, pAddress, pPort,
												   pTimeOut, WorkCount, pSessionManager);

	printf("[BattleServerContext] OnCreate - Result=%d\n", result);
	return result;
}

void BattleServerContext::OnUpdate(INT32 Command)
{
	i3NetworkServerContext::OnUpdate(Command);

	// Update room manager (check timeouts, destroy expired rooms)
	if (m_pBattleRoomManager)
		m_pBattleRoomManager->OnUpdate();
}

BOOL BattleServerContext::OnDestroy()
{
	if (m_pBattleRoomManager)
	{
		m_pBattleRoomManager->OnDestroy();
		delete m_pBattleRoomManager;
		m_pBattleRoomManager = nullptr;
		g_pBattleRoomManager = nullptr;
	}

	g_pBattleServerContext = nullptr;

	return i3NetworkServerContext::OnDestroy();
}

// ============================================================================
// BattleServer (BaseServer wrapper)
// ============================================================================

BattleServer::BattleServer()
	: m_pBattleContext(nullptr)
	, m_pUdpRelay(nullptr)
	, m_pMapManager(nullptr)
	, m_pConfigXML(nullptr)
{
}

BattleServer::~BattleServer()
{
}

bool BattleServer::OnLoadConfig(const char* pszConfigPath)
{
	i3IniParser ini;
	if (!ini.Load(pszConfigPath))
	{
		printf("[BattleServer] ERROR: Cannot load config: %s\n", pszConfigPath);
		return false;
	}

	// [BattleServer] section
	const char* pszBindIP = ini.GetString("BattleServer", "BindIP", "0.0.0.0");
	strncpy_s(m_BattleConfig.szBindIP, pszBindIP, _TRUNCATE);
	m_BattleConfig.ui16BindPort = (uint16_t)ini.GetInt("BattleServer", "BindPort", 40200);
	m_BattleConfig.i32MaxSessions = ini.GetInt("BattleServer", "MaxSessions", MAX_BATTLE_SESSIONS);
	m_BattleConfig.i32WorkerThreadCount = ini.GetInt("BattleServer", "WorkerThreads", 4);
	m_BattleConfig.ui8SocketTimeout = (uint8_t)ini.GetInt("BattleServer", "SocketTimeout", 60);
	m_BattleConfig.i32ServerId = ini.GetInt("BattleServer", "ServerId", 1);

	const char* pszName = ini.GetString("BattleServer", "ServerName", "BattleServer-1");
	strncpy_s(m_BattleConfig.szServerName, pszName, _TRUNCATE);

	const char* pszPublicIP = ini.GetString("BattleServer", "PublicIP", "127.0.0.1");
	strncpy_s(m_BattleConfig.szPublicIP, pszPublicIP, _TRUNCATE);
	m_BattleConfig.ui16PublicPort = (uint16_t)ini.GetInt("BattleServer", "PublicPort", 40200);

	// [UDP] section
	m_BattleConfig.ui16UdpBasePort = (uint16_t)ini.GetInt("UDP", "BasePort", BATTLE_UDP_BASE_PORT);
	m_BattleConfig.ui16UdpClientPort = (uint16_t)ini.GetInt("UDP", "UdpClientPort", 29890);
	m_BattleConfig.i32MaxBattleRooms = ini.GetInt("UDP", "MaxBattleRooms", MAX_BATTLE_ROOMS);

	// [Media] section
	const char* pszMediaPath = ini.GetString("Media", "Path", "Media");
	strncpy_s(m_BattleConfig.szMediaPath, pszMediaPath, _TRUNCATE);

	// Copy base config
	m_Config = m_BattleConfig;

	printf("[BattleServer] Config loaded:\n");
	printf("  Bind: %s:%d, MaxSessions=%d, Workers=%d\n",
		m_BattleConfig.szBindIP, m_BattleConfig.ui16BindPort,
		m_BattleConfig.i32MaxSessions, m_BattleConfig.i32WorkerThreadCount);
	printf("  Server: Id=%d, Name=%s, Public=%s:%d\n",
		m_BattleConfig.i32ServerId, m_BattleConfig.szServerName,
		m_BattleConfig.szPublicIP, m_BattleConfig.ui16PublicPort);
	printf("  UDP: BasePort=%d, UdpClientPort=%d, MaxRooms=%d\n",
		m_BattleConfig.ui16UdpBasePort, m_BattleConfig.ui16UdpClientPort,
		m_BattleConfig.i32MaxBattleRooms);
	printf("  Media: %s\n", m_BattleConfig.szMediaPath);

	return true;
}

bool BattleServer::OnInitialize()
{
	// Initialize ConfigXML (weapon, character, stage data from .i3RegXML)
	m_pConfigXML = new ConfigXML();
	m_pConfigXML->Initialize(m_BattleConfig.szMediaPath);
	g_pConfigXML = m_pConfigXML;

	// Initialize MapManager (load all maps from Media/)
	m_pMapManager = new MapManager();
	m_pMapManager->Initialize(m_BattleConfig.szMediaPath);
	g_pMapManager = m_pMapManager;

	printf("[BattleServer] Initialized successfully (Maps=%d, Weapons=%d, Characters=%d)\n",
		m_pMapManager->GetLoadedMapCount(),
		m_pConfigXML->GetWeaponCount(),
		m_pConfigXML->GetCharacterCount());
	return true;
}

i3NetworkServerContext* BattleServer::OnCreateContext()
{
	m_pBattleContext = new BattleServerContext();
	return m_pBattleContext;
}

i3NetworkSessionManager* BattleServer::OnCreateSessionManager()
{
	return new BattleSessionManager();
}

void BattleServer::OnUpdate()
{
	// Update UDP relay
	if (m_pUdpRelay)
		m_pUdpRelay->Update();
}

void BattleServer::OnShutdown()
{
	// Shutdown UDP relay
	if (m_pUdpRelay)
	{
		m_pUdpRelay->Shutdown();
		delete m_pUdpRelay;
		m_pUdpRelay = nullptr;
	}

	// Shutdown MapManager
	if (m_pMapManager)
	{
		m_pMapManager->Shutdown();
		delete m_pMapManager;
		m_pMapManager = nullptr;
		g_pMapManager = nullptr;
	}

	// Shutdown ConfigXML
	if (m_pConfigXML)
	{
		m_pConfigXML->Shutdown();
		delete m_pConfigXML;
		m_pConfigXML = nullptr;
		g_pConfigXML = nullptr;
	}

	printf("[BattleServer] Shutdown complete\n");
}
