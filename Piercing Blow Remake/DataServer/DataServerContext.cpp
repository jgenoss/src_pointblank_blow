#include "DataServerContext.h"
#include "DataSession.h"
#include "DataSessionManager.h"
#include "TaskProcessor.h"
#include "ModuleDBAuth.h"
#include "ModuleDBUserLoad.h"
#include "ModuleDBUserSave.h"
#include <cstdio>
#include <cstring>

// Global
DataServerContext* g_pDataServerContext = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DataServerContext
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

I3_CLASS_INSTANCE(DataServerContext, i3NetworkServerContext);

DataServerContext::DataServerContext()
	: m_pDataSessionManager(nullptr)
	, m_pDBPool(nullptr)
	, m_pTaskProcessor(nullptr)
	, m_pModuleAuth(nullptr)
	, m_pModuleUserLoad(nullptr)
	, m_pModuleUserSave(nullptr)
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

	// Crear TaskProcessor
	m_pTaskProcessor = new TaskProcessor(this);

	// Crear modulos de DB
	m_pModuleAuth = new ModuleDBAuth(m_pDBPool);
	m_pModuleUserLoad = new ModuleDBUserLoad(m_pDBPool);
	m_pModuleUserSave = new ModuleDBUserSave(m_pDBPool);

	printf("[DataServerContext] DB modules initialized\n");
	return true;
}

void DataServerContext::OnUpdate(INT32 Command)
{
	// Procesar respuestas de los modulos de DB
	if (m_pModuleAuth)
		m_pModuleAuth->ProcessResponses(this);
	if (m_pModuleUserLoad)
		m_pModuleUserLoad->ProcessResponses(this);
	if (m_pModuleUserSave)
		m_pModuleUserSave->ProcessResponses(this);
}

BOOL DataServerContext::OnDestroy()
{
	// Destruir modulos
	if (m_pModuleUserSave)	{ delete m_pModuleUserSave; m_pModuleUserSave = nullptr; }
	if (m_pModuleUserLoad)	{ delete m_pModuleUserLoad; m_pModuleUserLoad = nullptr; }
	if (m_pModuleAuth)		{ delete m_pModuleAuth; m_pModuleAuth = nullptr; }
	if (m_pTaskProcessor)	{ delete m_pTaskProcessor; m_pTaskProcessor = nullptr; }

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
	// TODO: Cargar desde archivo INI real
	strcpy(m_DataConfig.szBindIP, "0.0.0.0");
	m_DataConfig.ui16BindPort			= 40100;
	m_DataConfig.i32MaxSessions			= 32;		// Solo servidores se conectan, no clientes
	m_DataConfig.i32WorkerThreadCount	= 4;
	m_DataConfig.ui8SocketTimeout		= 60;

	// PostgreSQL config
	strcpy(m_DataConfig.dbConfig.szHost, "127.0.0.1");
	m_DataConfig.dbConfig.ui16Port		= 5432;
	strcpy(m_DataConfig.dbConfig.szDatabase, "piercing_blow");
	strcpy(m_DataConfig.dbConfig.szUser, "pb_server");
	strcpy(m_DataConfig.dbConfig.szPassword, "pb_password");
	m_DataConfig.i32DBPoolSize			= 8;

	m_Config = m_DataConfig;

	printf("[DataServer] Config loaded: Port=%d, DB=%s@%s:%d/%s, Pool=%d\n",
		m_DataConfig.ui16BindPort,
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
