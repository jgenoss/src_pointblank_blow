// BattleServer.cpp
// Piercing Blow Remake - BattleServer
//
// Servidor dedicado de combate. Recibe requests de GameServers para crear
// batallas, gestiona battle rooms con UDP relay para trafico real-time de
// juego, y reporta resultados al finalizar.
//
// Flujo:
//   GameServer -> BattleServer (crear batalla)
//   Clientes -> BattleServer UDP (game packets)
//   BattleServer -> GameServer (resultados)
//

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <cstdio>
#include <csignal>

#include "BattleServerContext.h"
#include "UdpRelay.h"

#pragma comment(lib, "ws2_32.lib")

// Flag global para control del loop principal
static volatile bool g_bRunning = true;

// Public IP for UDP relay (read from config, used by BattleSession)
char g_szBattlePublicIP[MAX_SERVER_IP_LENGTH] = {};

// Handler de Ctrl+C para shutdown limpio
void SignalHandler(int signal)
{
	if (signal == SIGINT || signal == SIGBREAK)
	{
		printf("\n[BattleServer] Shutdown signal received...\n");
		g_bRunning = false;
	}
}

int main(int argc, char* argv[])
{
	printf("============================================\n");
	printf("  Piercing Blow Remake - BattleServer\n");
	printf("  Build: %s %s\n", __DATE__, __TIME__);
	printf("============================================\n\n");

	// Registrar signal handlers
	signal(SIGINT, SignalHandler);
	signal(SIGBREAK, SignalHandler);

	// Inicializar Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("[BattleServer] ERROR: WSAStartup failed\n");
		return 1;
	}

	// Determinar ruta de configuracion
	const char* pszConfigPath = "config.ini";
	if (argc > 1)
		pszConfigPath = argv[1];

	// Crear e inicializar el servidor
	BattleServer server;

	if (!server.Initialize(pszConfigPath))
	{
		printf("[BattleServer] ERROR: Initialization failed\n");
		WSACleanup();
		return 1;
	}

	if (!server.Start())
	{
		printf("[BattleServer] ERROR: Start failed\n");
		WSACleanup();
		return 1;
	}

	// Copy public IP to global for BattleSession to use
	strncpy_s(g_szBattlePublicIP, server.GetBattleConfig().szPublicIP, _TRUNCATE);

	// Inicializar UDP relay
	UdpRelay* pUdpRelay = new UdpRelay();
	if (!pUdpRelay->Initialize(server.GetBattleConfig().ui16UdpBasePort))
	{
		printf("[BattleServer] WARNING: UDP relay initialization failed. "
			   "Battle relay will not function.\n");
	}

	printf("\n[BattleServer] Running. Press Ctrl+C to stop.\n");
	printf("[BattleServer] TCP port: %d (GameServer connections)\n",
		server.GetBattleConfig().ui16BindPort);
	printf("[BattleServer] UDP port: %d (Game relay)\n\n",
		server.GetBattleConfig().ui16UdpBasePort);

	// Loop principal del servidor
	while (g_bRunning)
	{
		server.Update();

		// Update UDP relay
		if (pUdpRelay)
			pUdpRelay->Update();

		Sleep(1);
	}

	// Shutdown
	printf("\n[BattleServer] Shutting down...\n");

	if (pUdpRelay)
	{
		pUdpRelay->Shutdown();
		delete pUdpRelay;
		pUdpRelay = nullptr;
	}

	server.Shutdown(0);

	WSACleanup();

	printf("[BattleServer] Goodbye.\n");
	return 0;
}
