#ifndef __GAMESERVERCONTEXT_H__
#define __GAMESERVERCONTEXT_H__

#pragma once
#include "BaseServer.h"

class GameSessionManager;
class GameContextMain;
class RoomManager;
class ModuleConnectServer;
class ModuleDataServer;

// Configuracion especifica del GameServer
struct GameServerConfig : public BaseServerConfig
{
	// Server identity
	int			i32ServerId;
	char		szServerName[64];
	char		szPublicIP[MAX_SERVER_IP_LENGTH];
	uint16_t	ui16PublicPort;

	// Channels
	uint8_t		ui8ChannelCount;
	uint16_t	ui16MaxUsersPerChannel;
	uint16_t	ui16MaxRoomsPerChannel;

	// ConnectServer connection
	char		szConnectServerIP[MAX_SERVER_IP_LENGTH];
	uint16_t	ui16ConnectServerPort;

	// DataServer connection
	char		szDataServerIP[MAX_SERVER_IP_LENGTH];
	uint16_t	ui16DataServerPort;

	GameServerConfig()
		: i32ServerId(1)
		, ui16PublicPort(40000)
		, ui8ChannelCount(4)
		, ui16MaxUsersPerChannel(200)
		, ui16MaxRoomsPerChannel(200)
		, ui16ConnectServerPort(40001)
		, ui16DataServerPort(40100)
	{
		szServerName[0] = '\0';
		szPublicIP[0] = '\0';
		szConnectServerIP[0] = '\0';
		szDataServerIP[0] = '\0';
	}
};

// Contexto del GameServer
// Patron: CServerContext -> i3NetworkServerContext
// Servidor principal de juego: sesiones de clientes, canales, lobbies, rooms
class GameServerContext : public i3NetworkServerContext
{
	I3_CLASS_DEFINE(GameServerContext);

public:
	GameServerContext();
	virtual ~GameServerContext();

	// i3NetworkServerContext overrides
	virtual BOOL OnCreate(UINT8 SocketCount, UINT32* pAddress, UINT16* pPort,
						  UINT8* pTimeOut, INT32 WorkCount,
						  i3NetworkSessionManager* pSessionManager) override;
	virtual void OnUpdate(INT32 Command) override;
	virtual BOOL OnDestroy() override;

	// Accessors
	GameSessionManager*		GetSessionManager()		{ return m_pGameSessionManager; }
	RoomManager*			GetRoomManager()		{ return m_pRoomManager; }

private:
	GameSessionManager*		m_pGameSessionManager;
	RoomManager*			m_pRoomManager;
};

extern GameServerContext* g_pGameServerContext;

// Wrapper de servidor que integra BaseServer con GameServerContext
class GameServer : public BaseServer
{
public:
	GameServer();
	virtual ~GameServer();

	// IServer
	virtual ServerType	GetType() const override		{ return ServerType::Game; }
	virtual const char*	GetName() const override		{ return "GameServer"; }

	// Accessors
	GameServerContext*		GetGameContext()					{ return m_pGameContext; }
	const GameServerConfig&	GetGameConfig() const			{ return m_GameConfig; }

	// Inter-server modules
	ModuleConnectServer*	GetModuleConnectServer()		{ return m_pModuleConnect; }
	ModuleDataServer*		GetModuleDataServer()			{ return m_pModuleData; }

	// Initialize inter-server modules (called after Start)
	bool					InitializeModules();

protected:
	// BaseServer overrides
	virtual bool		OnLoadConfig(const char* pszConfigPath) override;
	virtual bool		OnInitialize() override;
	virtual i3NetworkServerContext*		OnCreateContext() override;
	virtual i3NetworkSessionManager*	OnCreateSessionManager() override;
	virtual void		OnUpdate() override;
	virtual void		OnShutdown() override;

private:
	GameServerContext*		m_pGameContext;
	GameServerConfig		m_GameConfig;

	// Inter-server modules
	ModuleConnectServer*	m_pModuleConnect;
	ModuleDataServer*		m_pModuleData;
};

#endif // __GAMESERVERCONTEXT_H__
