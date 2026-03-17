#include "ConnectServerContext.h"
#include "ConnectSessionManager.h"
#include "GameServerRegistry.h"
#include "ModuleDataClient.h"
#include "i3IniParser.h"
#include <cstdio>
#include <cstring>

// Global
ConnectServerContext* g_pConnectServerContext = nullptr;
ConnectServerStatistics* g_pConnectStats = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConnectServerContext
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

I3_CLASS_INSTANCE(ConnectServerContext, i3NetworkServerContext);

ConnectServerContext::ConnectServerContext()
	: m_pConnectSessionManager(nullptr)
	, m_pRegistry(nullptr)
	, m_pModuleDataClient(nullptr)
{
}

ConnectServerContext::~ConnectServerContext()
{
}

BOOL ConnectServerContext::OnCreate(UINT8 SocketCount, UINT32* pAddress, UINT16* pPort,
									UINT8* pTimeOut, INT32 WorkCount,
									i3NetworkSessionManager* pSessionManager)
{
	// Guardar referencia al session manager
	m_pConnectSessionManager = static_cast<ConnectSessionManager*>(pSessionManager);

	// Crear el registry de GameServers
	m_pRegistry = new GameServerRegistry();
	if (!m_pRegistry)
		return FALSE;

	// Llamar al padre para configurar acceptor + IOCP
	if (!i3NetworkServerContext::OnCreate(SocketCount, pAddress, pPort, pTimeOut, WorkCount, pSessionManager))
		return FALSE;

	g_pConnectServerContext = this;

	m_Stats.Reset();
	m_Statistics.Reset();
	g_pConnectStats = &m_Statistics;

	printf("[ConnectServerContext] Created. Listening for clients.\n");
	return TRUE;
}

bool ConnectServerContext::InitializeModules(const char* pszDataServerIP, uint16_t ui16DataServerPort)
{
	// Create and initialize DataServer client module
	m_pModuleDataClient = new ModuleDataClient();
	m_pModuleDataClient->SetSessionManager(m_pConnectSessionManager);

	if (!m_pModuleDataClient->Initialize("DataClient", pszDataServerIP, ui16DataServerPort, 1))
	{
		printf("[ConnectServerContext] ERROR: Failed to initialize ModuleDataClient\n");
		delete m_pModuleDataClient;
		m_pModuleDataClient = nullptr;
		return false;
	}

	// Start the module thread
	m_pModuleDataClient->StartThread(nullptr);

	printf("[ConnectServerContext] ModuleDataClient initialized -> %s:%d\n",
		pszDataServerIP, ui16DataServerPort);
	return true;
}

void ConnectServerContext::OnUpdate(INT32 Command)
{
	// Actualizar registry de GameServers (verificar heartbeats, remover muertos)
	if (m_pRegistry)
		m_pRegistry->Update();

	LogStatsPeriodic();

	// Log I/O statistics periodically
	m_Statistics.LogStatistics();
}

void ConnectServerContext::LogStatsPeriodic()
{
	DWORD dwNow = GetTickCount();
	if (dwNow - m_Stats.dwLastStatsLog < 60000)
		return;
	m_Stats.dwLastStatsLog = dwNow;

	DWORD uptimeSec = (dwNow - m_Stats.dwStartTime) / 1000;
	int hours = uptimeSec / 3600;
	int mins = (uptimeSec % 3600) / 60;

	int activeConns = m_pConnectSessionManager ? m_pConnectSessionManager->GetActiveCount() : 0;
	int peakConns = m_pConnectSessionManager ? m_pConnectSessionManager->GetPeakActive() : 0;
	int onlineServers = m_pRegistry ? m_pRegistry->GetOnlineServerCount() : 0;

	printf("[ConnectServer Stats] Uptime=%dh%dm | Active=%d (peak=%d) | Servers=%d | "
		   "Auths=%ld (ok=%ld fail=%ld) | Selects=%ld\n",
		hours, mins, activeConns, peakConns, onlineServers,
		m_Stats.lTotalAuths, m_Stats.lSuccessfulAuths, m_Stats.lFailedAuths,
		m_Stats.lServerSelections);
}

BOOL ConnectServerContext::OnDestroy()
{
	if (m_pModuleDataClient)
	{
		m_pModuleDataClient->Destroy();
		delete m_pModuleDataClient;
		m_pModuleDataClient = nullptr;
	}

	if (m_pRegistry)
	{
		delete m_pRegistry;
		m_pRegistry = nullptr;
	}

	g_pConnectStats = nullptr;
	g_pConnectServerContext = nullptr;

	return i3NetworkServerContext::OnDestroy();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConnectServerStatistics
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConnectServerStatistics::LogStatistics()
{
	DWORD dwNow = GetTickCount();
	if (dwNow - dwLastLogTime < 60000)
		return;
	dwLastLogTime = dwNow;

	DWORD uptimeSec = (dwNow - dwStartTime) / 1000;
	int hours = uptimeSec / 3600;
	int mins = (uptimeSec % 3600) / 60;
	int secs = uptimeSec % 60;

	LONG64 pktsIn = l64TotalPacketsIn;
	LONG64 pktsOut = l64TotalPacketsOut;
	LONG64 bytesIn = l64TotalBytesIn;
	LONG64 bytesOut = l64TotalBytesOut;

	// Convert bytes to KB for readability
	double fKBIn = (double)bytesIn / 1024.0;
	double fKBOut = (double)bytesOut / 1024.0;

	// Auth throughput: total auths / uptime minutes
	double fAuthPerMin = (uptimeSec > 0) ? ((double)lActiveAuths * 60.0 / (double)uptimeSec) : 0.0;

	printf("[ConnectServer I/O] Uptime=%dh%dm%ds | PktsIn=%lld PktsOut=%lld | "
		   "BytesIn=%.1fKB BytesOut=%.1fKB | ActiveAuths=%ld (peak=%ld)\n",
		hours, mins, secs,
		pktsIn, pktsOut,
		fKBIn, fKBOut,
		lActiveAuths, lPeakActiveAuths);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConnectServer (BaseServer wrapper)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ConnectServer::ConnectServer()
	: m_pConnectContext(nullptr)
{
}

ConnectServer::~ConnectServer()
{
}

bool ConnectServer::OnLoadConfig(const char* pszConfigPath)
{
	// Defaults
	strcpy(m_ConnectConfig.szBindIP, "0.0.0.0");
	m_ConnectConfig.ui16BindPort			= 40000;
	m_ConnectConfig.i32MaxSessions			= 500;
	m_ConnectConfig.i32WorkerThreadCount	= 4;
	m_ConnectConfig.ui8SocketTimeout		= 30;
	m_ConnectConfig.ui16GameServerPort		= 40001;
	m_ConnectConfig.i32MaxClientSessions	= 500;
	strcpy(m_ConnectConfig.szDataServerIP, "127.0.0.1");
	m_ConnectConfig.ui16DataServerPort		= 40100;

	// Load from INI file (override defaults)
	i3IniParser ini;
	if (ini.Load(pszConfigPath))
	{
		const char* pszBindIP = ini.GetString("ConnectServer", "BindIP", "0.0.0.0");
		strncpy_s(m_ConnectConfig.szBindIP, pszBindIP, _TRUNCATE);
		m_ConnectConfig.ui16BindPort		= (uint16_t)ini.GetInt("ConnectServer", "BindPort", 40000);
		m_ConnectConfig.i32MaxSessions		= ini.GetInt("ConnectServer", "MaxSessions", 500);
		m_ConnectConfig.i32WorkerThreadCount = ini.GetInt("ConnectServer", "WorkerThreads", 4);
		m_ConnectConfig.ui8SocketTimeout	= (uint8_t)ini.GetInt("ConnectServer", "SocketTimeout", 30);
		m_ConnectConfig.ui16GameServerPort	= (uint16_t)ini.GetInt("ConnectServer", "GameServerPort", 40001);
		m_ConnectConfig.i32MaxClientSessions = ini.GetInt("ConnectServer", "MaxClientSessions", 500);

		const char* pszDSIP = ini.GetString("DataServer", "IP", "127.0.0.1");
		strncpy_s(m_ConnectConfig.szDataServerIP, pszDSIP, _TRUNCATE);
		m_ConnectConfig.ui16DataServerPort	= (uint16_t)ini.GetInt("DataServer", "Port", 40100);
	}
	else
	{
		printf("[ConnectServer] WARNING: Cannot load config '%s', using defaults\n", pszConfigPath);
	}

	// Copy to base config
	m_Config = m_ConnectConfig;

	printf("[ConnectServer] Config loaded: Bind=%s:%d, GameServerPort=%d\n",
		m_ConnectConfig.szBindIP, m_ConnectConfig.ui16BindPort, m_ConnectConfig.ui16GameServerPort);
	printf("  DataServer: %s:%d\n",
		m_ConnectConfig.szDataServerIP, m_ConnectConfig.ui16DataServerPort);

	return true;
}

bool ConnectServer::OnInitialize()
{
	// Initialize DataServer connection module
	if (m_pConnectContext)
	{
		m_pConnectContext->InitializeModules(
			m_ConnectConfig.szDataServerIP,
			m_ConnectConfig.ui16DataServerPort);
	}
	return true;
}

i3NetworkServerContext* ConnectServer::OnCreateContext()
{
	m_pConnectContext = new ConnectServerContext();
	return m_pConnectContext;
}

i3NetworkSessionManager* ConnectServer::OnCreateSessionManager()
{
	ConnectSessionManager* pManager = new ConnectSessionManager();
	return pManager;
}

void ConnectServer::OnUpdate()
{
	// Update adicional del ConnectServer si es necesario
}

void ConnectServer::OnShutdown()
{
	m_pConnectContext = nullptr;
}
