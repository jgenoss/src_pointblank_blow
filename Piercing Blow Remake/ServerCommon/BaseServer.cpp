#include "BaseServer.h"
#include <cstdio>
#include <cstdarg>
#include <cstring>

BaseServer::BaseServer()
	: m_eState(ServerState::None)
	, m_pContext(nullptr)
	, m_pSessionManager(nullptr)
{
	m_szConfigPath[0]	= '\0';
	m_szLogPath[0]		= '\0';
}

BaseServer::~BaseServer()
{
	if (m_eState != ServerState::None)
		Shutdown(0);
}

bool BaseServer::Initialize(const char* pszConfigPath)
{
	if (m_eState != ServerState::None)
		return false;

	SetState(ServerState::Loading);
	LogMessage("[%s] Initializing server...", GetName());

	// Guardar ruta de configuracion
	if (pszConfigPath)
		strncpy(m_szConfigPath, pszConfigPath, MAX_CONFIG_PATH - 1);

	// Cargar configuracion especifica del servidor
	if (!OnLoadConfig(pszConfigPath))
	{
		LogMessage("[%s] ERROR: Failed to load config from '%s'", GetName(), pszConfigPath);
		SetState(ServerState::None);
		return false;
	}

	SetState(ServerState::Init);

	// Inicializacion especifica del servidor
	if (!OnInitialize())
	{
		LogMessage("[%s] ERROR: Server-specific initialization failed", GetName());
		SetState(ServerState::None);
		return false;
	}

	LogMessage("[%s] Initialization complete.", GetName());
	return true;
}

bool BaseServer::Start()
{
	if (m_eState != ServerState::Init)
		return false;

	SetState(ServerState::Initing);
	LogMessage("[%s] Starting server on %s:%d...", GetName(), m_Config.szBindIP, m_Config.ui16BindPort);

	// Crear el contexto del servidor (i3NetworkServerContext)
	m_pContext = OnCreateContext();
	if (!m_pContext)
	{
		LogMessage("[%s] ERROR: Failed to create server context", GetName());
		SetState(ServerState::Init);
		return false;
	}

	// Crear el session manager
	m_pSessionManager = OnCreateSessionManager();
	if (!m_pSessionManager)
	{
		LogMessage("[%s] ERROR: Failed to create session manager", GetName());
		SetState(ServerState::Init);
		return false;
	}

	// Configurar red y crear acceptor via i3NetworkServerContext::OnCreate
	UINT32 ui32Address = 0; // INADDR_ANY
	if (m_Config.szBindIP[0] != '\0' && strcmp(m_Config.szBindIP, "0.0.0.0") != 0)
	{
		// Parsear IP string a UINT32
		unsigned int a, b, c, d;
		if (sscanf(m_Config.szBindIP, "%u.%u.%u.%u", &a, &b, &c, &d) == 4)
			ui32Address = (a) | (b << 8) | (c << 16) | (d << 24);
	}

	UINT16 ui16Port = m_Config.ui16BindPort;
	UINT8 ui8Timeout = m_Config.ui8SocketTimeout;

	BOOL bResult = m_pContext->OnCreate(
		1,									// SocketCount (1 acceptor)
		&ui32Address,						// Bind address
		&ui16Port,							// Bind port
		&ui8Timeout,						// Socket timeout
		m_Config.i32WorkerThreadCount,		// Worker thread count
		m_pSessionManager					// Session manager
	);

	if (!bResult)
	{
		LogMessage("[%s] ERROR: i3NetworkServerContext::OnCreate failed (port %d)", GetName(), ui16Port);
		SetState(ServerState::Init);
		return false;
	}

	SetState(ServerState::Start);
	LogMessage("[%s] Server started successfully. Listening on %s:%d",
		GetName(), m_Config.szBindIP, m_Config.ui16BindPort);

	return true;
}

void BaseServer::Update()
{
	if (m_eState != ServerState::Start)
		return;

	// Update del contexto i3
	if (m_pContext)
		m_pContext->OnUpdate(0);

	// Update especifico del servidor
	OnUpdate();
}

void BaseServer::Shutdown(int i32Reason)
{
	LogMessage("[%s] Shutting down server (reason: %d)...", GetName(), i32Reason);

	// Shutdown especifico del servidor
	OnShutdown();

	// Destruir contexto i3
	if (m_pContext)
	{
		m_pContext->OnDestroy();
		m_pContext = nullptr;
	}

	m_pSessionManager = nullptr;
	SetState(ServerState::None);

	LogMessage("[%s] Server shutdown complete.", GetName());
}

void BaseServer::LogMessage(const char* pszFormat, ...)
{
	char szBuffer[2048];
	va_list args;
	va_start(args, pszFormat);
	vsnprintf(szBuffer, sizeof(szBuffer), pszFormat, args);
	va_end(args);

	printf("%s\n", szBuffer);
}
