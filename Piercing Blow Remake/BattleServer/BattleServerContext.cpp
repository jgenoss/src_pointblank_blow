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
	// Defaults
	strcpy(m_BattleConfig.szBindIP, "0.0.0.0");
	m_BattleConfig.ui16BindPort			= 40200;
	m_BattleConfig.i32MaxSessions		= MAX_BATTLE_SESSIONS;
	m_BattleConfig.i32WorkerThreadCount	= 4;
	m_BattleConfig.ui8SocketTimeout		= 60;
	m_BattleConfig.i32ServerId			= 1;
	strcpy(m_BattleConfig.szServerName, "BattleServer-1");
	strcpy(m_BattleConfig.szPublicIP, "127.0.0.1");
	m_BattleConfig.ui16PublicPort		= 40200;
	m_BattleConfig.ui16UdpBasePort		= BATTLE_UDP_BASE_PORT;
	m_BattleConfig.ui16UdpClientPort	= 29890;
	m_BattleConfig.i32MaxBattleRooms	= MAX_BATTLE_ROOMS;
	strcpy(m_BattleConfig.szMediaPath, "Media");

	i3IniParser ini;
	if (!ini.OpenFromFile(pszConfigPath))
	{
		printf("[BattleServer] WARNING: Cannot load config '%s', using defaults\n", pszConfigPath);
	}
	else
	{
		INT32 nVal = 0;
		char  szVal[256];

		// [BattleServer] section
		if (ini.ReadSection("BattleServer"))
		{
			ini.GetValue("BindIP", "0.0.0.0", szVal, sizeof(szVal));
			strncpy_s(m_BattleConfig.szBindIP, szVal, _TRUNCATE);

			ini.GetValue("BindPort", (INT32)40200, &nVal);
			m_BattleConfig.ui16BindPort = (uint16_t)nVal;

			ini.GetValue("MaxSessions", (INT32)MAX_BATTLE_SESSIONS, &nVal);
			m_BattleConfig.i32MaxSessions = nVal;

			ini.GetValue("WorkerThreads", (INT32)4, &nVal);
			m_BattleConfig.i32WorkerThreadCount = nVal;

			ini.GetValue("SocketTimeout", (INT32)60, &nVal);
			m_BattleConfig.ui8SocketTimeout = (uint8_t)nVal;

			ini.GetValue("ServerId", (INT32)1, &nVal);
			m_BattleConfig.i32ServerId = nVal;

			ini.GetValue("ServerName", "BattleServer-1", szVal, sizeof(szVal));
			strncpy_s(m_BattleConfig.szServerName, szVal, _TRUNCATE);

			ini.GetValue("PublicIP", "127.0.0.1", szVal, sizeof(szVal));
			strncpy_s(m_BattleConfig.szPublicIP, szVal, _TRUNCATE);

			ini.GetValue("PublicPort", (INT32)40200, &nVal);
			m_BattleConfig.ui16PublicPort = (uint16_t)nVal;
		}

		// [UDP] section
		if (ini.ReadSection("UDP"))
		{
			ini.GetValue("BasePort", (INT32)BATTLE_UDP_BASE_PORT, &nVal);
			m_BattleConfig.ui16UdpBasePort = (uint16_t)nVal;

			ini.GetValue("UdpClientPort", (INT32)29890, &nVal);
			m_BattleConfig.ui16UdpClientPort = (uint16_t)nVal;

			ini.GetValue("MaxBattleRooms", (INT32)MAX_BATTLE_ROOMS, &nVal);
			m_BattleConfig.i32MaxBattleRooms = nVal;
		}

		// [Media] section
		if (ini.ReadSection("Media"))
		{
			ini.GetValue("Path", "Media", szVal, sizeof(szVal));
			strncpy_s(m_BattleConfig.szMediaPath, szVal, _TRUNCATE);
		}
	}

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
