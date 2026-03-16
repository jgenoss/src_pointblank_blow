// DataServer.cpp
// Piercing Blow Remake - DataServer
//
// Servidor centralizado de operaciones de base de datos (PostgreSQL).
// Acepta conexiones de ConnectServer, GameServer y BattleServer.
// Procesa operaciones de auth, carga/guardado de jugadores y estadisticas.
//
// Flujo:
//   ConnectServer/GameServer -> DataServer (auth/load/save) -> PostgreSQL
//

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <cstdio>
#include <csignal>

#include "DataServerContext.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libpq.lib")

// Flag global para control del loop principal
static volatile bool g_bRunning = true;

// Handler de Ctrl+C para shutdown limpio
void SignalHandler(int signal)
{
	if (signal == SIGINT || signal == SIGBREAK)
	{
		printf("\n[DataServer] Shutdown signal received...\n");
		g_bRunning = false;
	}
}

int main(int argc, char* argv[])
{
	printf("============================================\n");
	printf("  Piercing Blow Remake - DataServer\n");
	printf("  Build: %s %s\n", __DATE__, __TIME__);
	printf("============================================\n\n");

	// Registrar signal handlers
	signal(SIGINT, SignalHandler);
	signal(SIGBREAK, SignalHandler);

	// Inicializar Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("[DataServer] ERROR: WSAStartup failed\n");
		return 1;
	}

	// Determinar ruta de configuracion
	const char* pszConfigPath = "config.ini";
	if (argc > 1)
		pszConfigPath = argv[1];

	// Crear e inicializar el servidor
	DataServer server;

	if (!server.Initialize(pszConfigPath))
	{
		printf("[DataServer] ERROR: Initialization failed\n");
		WSACleanup();
		return 1;
	}

	// Inicializar modulos de DB despues de Start
	if (!server.Start())
	{
		printf("[DataServer] ERROR: Start failed\n");
		WSACleanup();
		return 1;
	}

	// Inicializar pool de conexiones y modulos de DB
	DataServerContext* pContext = server.GetDataContext();
	if (pContext)
	{
		const DataServerConfig& config = server.GetDataConfig();
		if (!pContext->InitializeDBModules(config.dbConfig, config.i32DBPoolSize))
		{
			printf("[DataServer] WARNING: DB modules initialization failed. "
				   "Server will run without database.\n");
		}
	}

	printf("\n[DataServer] Running. Press Ctrl+C to stop.\n\n");

	// Loop principal del servidor
	// Patron de GameServerSev.cpp: loop con Sleep(1) para no quemar CPU
	while (g_bRunning)
	{
		server.Update();
		Sleep(1);
	}

	// Shutdown
	printf("\n[DataServer] Shutting down...\n");
	server.Shutdown(0);

	WSACleanup();

	printf("[DataServer] Goodbye.\n");
	return 0;
}
