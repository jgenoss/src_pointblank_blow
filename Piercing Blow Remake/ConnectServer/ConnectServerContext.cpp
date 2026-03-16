#include "ConnectServerContext.h"
#include "ConnectSessionManager.h"
#include "GameServerRegistry.h"
#include <cstdio>
#include <cstring>

// Global
ConnectServerContext* g_pConnectServerContext = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConnectServerContext
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

I3_CLASS_INSTANCE(ConnectServerContext, i3NetworkServerContext);

ConnectServerContext::ConnectServerContext()
	: m_pConnectSessionManager(nullptr)
	, m_pRegistry(nullptr)
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

	printf("[ConnectServerContext] Created. Listening for clients.\n");
	return TRUE;
}

void ConnectServerContext::OnUpdate(INT32 Command)
{
	// Actualizar registry de GameServers (verificar heartbeats, remover muertos)
	if (m_pRegistry)
		m_pRegistry->Update();
}

BOOL ConnectServerContext::OnDestroy()
{
	if (m_pRegistry)
	{
		delete m_pRegistry;
		m_pRegistry = nullptr;
	}

	g_pConnectServerContext = nullptr;

	return i3NetworkServerContext::OnDestroy();
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
	// TODO: Cargar desde archivo INI real
	// Por ahora usar valores por defecto para desarrollo
	strcpy(m_ConnectConfig.szBindIP, "0.0.0.0");
	m_ConnectConfig.ui16BindPort			= 40000;
	m_ConnectConfig.i32MaxSessions			= 500;
	m_ConnectConfig.i32WorkerThreadCount	= 4;
	m_ConnectConfig.ui8SocketTimeout		= 30;
	m_ConnectConfig.ui16GameServerPort		= 40001;
	m_ConnectConfig.i32MaxClientSessions	= 500;
	strcpy(m_ConnectConfig.szDataServerIP, "127.0.0.1");
	m_ConnectConfig.ui16DataServerPort		= 40100;

	// Copiar al config base
	m_Config = m_ConnectConfig;

	printf("[ConnectServer] Config loaded: ClientPort=%d, GameServerPort=%d\n",
		m_ConnectConfig.ui16BindPort, m_ConnectConfig.ui16GameServerPort);

	return true;
}

bool ConnectServer::OnInitialize()
{
	// Inicializaciones adicionales del ConnectServer
	// (Conexion a DataServer se hara via ModuleBase en fase posterior)
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
