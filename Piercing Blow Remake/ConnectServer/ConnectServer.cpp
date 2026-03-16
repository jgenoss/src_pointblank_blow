// ConnectServer.cpp
// Piercing Blow Remake - ConnectServer
//
// Primer punto de contacto del cliente.
// Maneja autenticacion, lista de servidores y routing a GameServers.
// Arquitectura distribuida estilo MU Online.
//
// Flujo:
//   Cliente -> ConnectServer (auth + server list) -> GameServer (token)
//

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <cstdio>
#include <csignal>

#include "ConnectServerContext.h"

#pragma comment(lib, "ws2_32.lib")

// Flag global para control del loop principal
static volatile bool g_bRunning = true;

// Handler de Ctrl+C para shutdown limpio
void SignalHandler(int signal)
{
	if (signal == SIGINT || signal == SIGBREAK)
	{
		printf("\n[ConnectServer] Shutdown signal received...\n");
		g_bRunning = false;
	}
}

int main(int argc, char* argv[])
{
	printf("============================================\n");
	printf("  Piercing Blow Remake - ConnectServer\n");
	printf("  Build: %s %s\n", __DATE__, __TIME__);
	printf("============================================\n\n");

	// Registrar signal handlers
	signal(SIGINT, SignalHandler);
	signal(SIGBREAK, SignalHandler);

	// Inicializar Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("[ConnectServer] ERROR: WSAStartup failed\n");
		return 1;
	}

	// Determinar ruta de configuracion
	const char* pszConfigPath = "config.ini";
	if (argc > 1)
		pszConfigPath = argv[1];

	// Crear e inicializar el servidor
	ConnectServer server;

	if (!server.Initialize(pszConfigPath))
	{
		printf("[ConnectServer] ERROR: Initialization failed\n");
		WSACleanup();
		return 1;
	}

	if (!server.Start())
	{
		printf("[ConnectServer] ERROR: Start failed\n");
		WSACleanup();
		return 1;
	}

	printf("\n[ConnectServer] Running. Press Ctrl+C to stop.\n\n");

	// Loop principal del servidor
	// Patron de GameServerSev.cpp: loop con Sleep(1) para no quemar CPU
	while (g_bRunning)
	{
		server.Update();
		Sleep(1);
	}

	// Shutdown
	printf("\n[ConnectServer] Shutting down...\n");
	server.Shutdown(0);

	WSACleanup();

	printf("[ConnectServer] Goodbye.\n");
	return 0;
}
