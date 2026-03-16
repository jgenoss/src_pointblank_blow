// GameServer.cpp
// Piercing Blow Remake - GameServer
//
// Servidor principal de juego. Acepta conexiones de clientes (con auth token
// del ConnectServer), gestiona sesiones, canales, lobbies y rooms.
// Se conecta a ConnectServer (registro + tokens) y DataServer (load/save).
//
// Flujo:
//   Cliente -> ConnectServer (auth) -> GameServer (token) -> DataServer (load)
//

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <cstdio>
#include <csignal>

#include "GameServerContext.h"

#pragma comment(lib, "ws2_32.lib")

// Flag global para control del loop principal
static volatile bool g_bRunning = true;

// Handler de Ctrl+C para shutdown limpio
void SignalHandler(int signal)
{
	if (signal == SIGINT || signal == SIGBREAK)
	{
		printf("\n[GameServer] Shutdown signal received...\n");
		g_bRunning = false;
	}
}

int main(int argc, char* argv[])
{
	printf("============================================\n");
	printf("  Piercing Blow Remake - GameServer\n");
	printf("  Build: %s %s\n", __DATE__, __TIME__);
	printf("============================================\n\n");

	// Registrar signal handlers
	signal(SIGINT, SignalHandler);
	signal(SIGBREAK, SignalHandler);

	// Inicializar Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("[GameServer] ERROR: WSAStartup failed\n");
		return 1;
	}

	// Determinar ruta de configuracion
	const char* pszConfigPath = "config.ini";
	if (argc > 1)
		pszConfigPath = argv[1];

	// Crear e inicializar el servidor
	GameServer server;

	if (!server.Initialize(pszConfigPath))
	{
		printf("[GameServer] ERROR: Initialization failed\n");
		WSACleanup();
		return 1;
	}

	if (!server.Start())
	{
		printf("[GameServer] ERROR: Start failed\n");
		WSACleanup();
		return 1;
	}

	// Inicializar modulos inter-servidor (ConnectServer, DataServer)
	if (!server.InitializeModules())
	{
		printf("[GameServer] WARNING: Inter-server module initialization failed. "
			   "Server will run without external connections.\n");
	}

	printf("\n[GameServer] Running. Press Ctrl+C to stop.\n\n");

	// Loop principal del servidor
	// Patron de GameServerSev.cpp: loop con Sleep(1) para no quemar CPU
	while (g_bRunning)
	{
		server.Update();
		Sleep(1);
	}

	// Shutdown
	printf("\n[GameServer] Shutting down...\n");
	server.Shutdown(0);

	WSACleanup();

	printf("[GameServer] Goodbye.\n");
	return 0;
}
