#include "pch.h"
#include "DataServerContext.h"
#include "DataSession.h"
#include "DataSessionManager.h"
#include "TaskProcessor.h"
#include "ModuleDBAuth.h"
#include "ModuleDBUserLoad.h"
#include "ModuleDBUserSave.h"
#include "ModuleDBGameData.h"
#include "ModuleDBSocial.h"
#include "i3IniParser.h"
#include <cstdio>
#include <cstring>

// Global
DataServerContext* g_pDataServerContext = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DataServerContext
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

I3_CLASS_INSTANCE(DataServerContext, i3NetworkServerContext);//ninguna instancia del constructor "i3MetaTemplate<T>::i3MetaTemplate [con T=DataServerContext]" coincide con la lista de argumentos

DataServerContext::DataServerContext()
	: m_pDataSessionManager(nullptr)
	, m_pDBPool(nullptr)
	, m_pTaskProcessor(nullptr)
	, m_pModuleAuth(nullptr)
	, m_pModuleUserLoad(nullptr)
	, m_pModuleUserSave(nullptr)
	, m_pModuleGameData(nullptr)
	, m_pModuleSocial(nullptr)
{
}

DataServerContext::~DataServerContext()
{
}

BOOL DataServerContext::OnCreate(UINT8 SocketCount, UINT32* pAddress, UINT16* pPort,
								  UINT8* pTimeOut, INT32 WorkCount,
								  i3NetworkSessionManager* pSessionManager)
{
	m_pDataSessionManager = static_cast<DataSessionManager*>(pSessionManager);

	if (!i3NetworkServerContext::OnCreate(SocketCount, pAddress, pPort, pTimeOut, WorkCount, pSessionManager))
		return FALSE;

	g_pDataServerContext = this;

	printf("[DataServerContext] Created. Ready to accept server connections.\n");
	return TRUE;
}

bool DataServerContext::InitializeDBModules(const DBConfig& config, int i32PoolSize)
{
	// Crear pool de conexiones PostgreSQL
	m_pDBPool = new DBConnectionPool();
	if (!m_pDBPool->Initialize(config, i32PoolSize))
	{
		printf("[DataServerContext] ERROR: Failed to initialize DB connection pool\n");
		return false;
	}

	// Crear modulos de DB (must be created before TaskProcessor since workers use them)
	m_pModuleAuth = new ModuleDBAuth(m_pDBPool);
	m_pModuleUserLoad = new ModuleDBUserLoad(m_pDBPool);
	m_pModuleUserSave = new ModuleDBUserSave(m_pDBPool);
	m_pModuleGameData = new ModuleDBGameData(m_pDBPool);
	m_pModuleSocial = new ModuleDBSocial(m_pDBPool);

	// Crear TaskProcessor with async worker threads (one worker per DB connection)
	m_pTaskProcessor = new TaskProcessor(this);
	if (!m_pTaskProcessor->Initialize(i32PoolSize))
	{
		printf("[DataServerContext] WARNING: TaskProcessor async workers failed, falling back to sync\n");
	}

	printf("[DataServerContext] DB modules initialized (5 modules, %d async workers)\n", i32PoolSize);
	return true;
}

void DataServerContext::OnUpdate(INT32 Command)
{
	// Process async worker responses (drains response queue and sends packets)
	if (m_pTaskProcessor)
		m_pTaskProcessor->ProcessResponses();
}

BOOL DataServerContext::OnDestroy()
{
	// Shutdown TaskProcessor FIRST - workers reference modules and DB pool
	if (m_pTaskProcessor)
	{
		m_pTaskProcessor->Shutdown();
		delete m_pTaskProcessor;
		m_pTaskProcessor = nullptr;
	}

	// Destruir modulos (safe now that no workers are running)
	if (m_pModuleSocial)	{ delete m_pModuleSocial; m_pModuleSocial = nullptr; }
	if (m_pModuleGameData)	{ delete m_pModuleGameData; m_pModuleGameData = nullptr; }
	if (m_pModuleUserSave)	{ delete m_pModuleUserSave; m_pModuleUserSave = nullptr; }
	if (m_pModuleUserLoad)	{ delete m_pModuleUserLoad; m_pModuleUserLoad = nullptr; }
	if (m_pModuleAuth)		{ delete m_pModuleAuth; m_pModuleAuth = nullptr; }

	// Destruir pool de conexiones
	if (m_pDBPool)
	{
		m_pDBPool->Shutdown();
		delete m_pDBPool;
		m_pDBPool = nullptr;
	}

	g_pDataServerContext = nullptr;

	return i3NetworkServerContext::OnDestroy();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DataServer (BaseServer wrapper)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DataServer::DataServer()
	: m_pDataContext(nullptr)
{
}

DataServer::~DataServer()
{
}

bool DataServer::OnLoadConfig(const char* pszConfigPath)
{
	// Defaults
	strcpy(m_DataConfig.szBindIP, "0.0.0.0");
	m_DataConfig.ui16BindPort			= 40100;
	m_DataConfig.i32MaxSessions			= 32;
	m_DataConfig.i32WorkerThreadCount	= 4;
	m_DataConfig.ui8SocketTimeout		= 60;

	strcpy(m_DataConfig.dbConfig.szHost, "127.0.0.1");
	m_DataConfig.dbConfig.ui16Port		= 5432;
	strcpy(m_DataConfig.dbConfig.szDatabase, "piercing_blow");
	strcpy(m_DataConfig.dbConfig.szUser, "pb_server");
	strcpy(m_DataConfig.dbConfig.szPassword, "pb_password");
	m_DataConfig.i32DBPoolSize			= 8;

	// Load from INI file (override defaults)
	i3IniParser ini;
	if (!ini.OpenFromFile(pszConfigPath))
	{
		printf("[DataServer] WARNING: Cannot load config '%s', using defaults\n", pszConfigPath);
	}
	else
	{
		INT32 nVal = 0;
		char  szVal[256];

		// [DataServer] section
		if (ini.ReadSection("DataServer"))
		{
			ini.GetValue("BindIP", "0.0.0.0", szVal, sizeof(szVal));
			strncpy_s(m_DataConfig.szBindIP, szVal, _TRUNCATE);

			ini.GetValue("BindPort", (INT32)40100, &nVal);
			m_DataConfig.ui16BindPort = (uint16_t)nVal;

			ini.GetValue("MaxSessions", (INT32)32, &nVal);
			m_DataConfig.i32MaxSessions = nVal;

			ini.GetValue("WorkerThreads", (INT32)4, &nVal);
			m_DataConfig.i32WorkerThreadCount = nVal;

			ini.GetValue("SocketTimeout", (INT32)60, &nVal);
			m_DataConfig.ui8SocketTimeout = (uint8_t)nVal;
		}

		// [Database] section
		if (ini.ReadSection("Database"))
		{
			ini.GetValue("Host", "127.0.0.1", szVal, sizeof(szVal));
			strncpy_s(m_DataConfig.dbConfig.szHost, szVal, _TRUNCATE);

			ini.GetValue("Port", (INT32)5432, &nVal);
			m_DataConfig.dbConfig.ui16Port = (uint16_t)nVal;

			ini.GetValue("Database", "piercing_blow", szVal, sizeof(szVal));
			strncpy_s(m_DataConfig.dbConfig.szDatabase, szVal, _TRUNCATE);

			ini.GetValue("User", "pb_server", szVal, sizeof(szVal));
			strncpy_s(m_DataConfig.dbConfig.szUser, szVal, _TRUNCATE);

			ini.GetValue("Password", "pb_password", szVal, sizeof(szVal));
			strncpy_s(m_DataConfig.dbConfig.szPassword, szVal, _TRUNCATE);

			ini.GetValue("PoolSize", (INT32)8, &nVal);
			m_DataConfig.i32DBPoolSize = nVal;
		}
	}

	m_Config = m_DataConfig;

	printf("[DataServer] Config loaded: Bind=%s:%d, MaxSessions=%d\n",
		m_DataConfig.szBindIP, m_DataConfig.ui16BindPort, m_DataConfig.i32MaxSessions);
	printf("  DB: %s@%s:%d/%s, Pool=%d\n",
		m_DataConfig.dbConfig.szUser,
		m_DataConfig.dbConfig.szHost,
		m_DataConfig.dbConfig.ui16Port,
		m_DataConfig.dbConfig.szDatabase,
		m_DataConfig.i32DBPoolSize);

	return true;
}

bool DataServer::OnInitialize()
{
	return true;
}

i3NetworkServerContext* DataServer::OnCreateContext()
{
	m_pDataContext = new DataServerContext();
	return m_pDataContext;
}

i3NetworkSessionManager* DataServer::OnCreateSessionManager()
{
	DataSessionManager* pManager = new DataSessionManager();
	return pManager;
}

void DataServer::OnUpdate()
{
}

void DataServer::OnShutdown()
{
	m_pDataContext = nullptr;
}
