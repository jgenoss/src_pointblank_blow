#ifndef __BATTLESERVERCONTEXT_H__
#define __BATTLESERVERCONTEXT_H__

#pragma once
#include "BaseServer.h"
#include "BattleDef.h"

class BattleSessionManager;
class BattleRoomManager;
class UdpRelay;
class MapManager;
class ConfigXML;

// Configuracion especifica del BattleServer
struct BattleServerConfig : public BaseServerConfig
{
	// Server identity
	int			i32ServerId;
	char		szServerName[64];
	char		szPublicIP[MAX_SERVER_IP_LENGTH];
	uint16_t	ui16PublicPort;

	// UDP
	uint16_t	ui16UdpBasePort;
	uint16_t	ui16UdpClientPort;		// Client P2P UDP port (sent to clients)
	int			i32MaxBattleRooms;

	// Media path
	char		szMediaPath[MAX_PATH];

	BattleServerConfig()
		: i32ServerId(1)
		, ui16PublicPort(40200)
		, ui16UdpBasePort(BATTLE_UDP_BASE_PORT)
		, ui16UdpClientPort(29890)
		, i32MaxBattleRooms(MAX_BATTLE_ROOMS)
	{
		szServerName[0] = '\0';
		szPublicIP[0] = '\0';
		strcpy_s(szMediaPath, "Media");
	}
};

// Contexto del BattleServer
// Acepta conexiones TCP de GameServers, gestiona battle rooms
class BattleServerContext : public i3NetworkServerContext
{
	I3_CLASS_DEFINE(BattleServerContext);

public:
	BattleServerContext();
	virtual ~BattleServerContext();

	// i3NetworkServerContext overrides
	virtual BOOL OnCreate(UINT8 SocketCount, UINT32* pAddress, UINT16* pPort,
						  UINT8* pTimeOut, INT32 WorkCount,
						  i3NetworkSessionManager* pSessionManager) override;
	virtual void OnUpdate(INT32 Command) override;
	virtual BOOL OnDestroy() override;

	// Accessors
	BattleSessionManager*	GetSessionManager()		{ return m_pBattleSessionManager; }
	BattleRoomManager*		GetRoomManager()		{ return m_pBattleRoomManager; }

private:
	BattleSessionManager*	m_pBattleSessionManager;
	BattleRoomManager*		m_pBattleRoomManager;
};

extern BattleServerContext* g_pBattleServerContext;

// Wrapper de servidor que integra BaseServer con BattleServerContext
class BattleServer : public BaseServer
{
public:
	BattleServer();
	virtual ~BattleServer();

	// IServer
	virtual ServerType	GetType() const override		{ return ServerType::Battle; }
	virtual const char*	GetName() const override		{ return "BattleServer"; }

	// Accessors
	BattleServerContext*		GetBattleContext()					{ return m_pBattleContext; }
	const BattleServerConfig&	GetBattleConfig() const				{ return m_BattleConfig; }
	UdpRelay*					GetUdpRelay()						{ return m_pUdpRelay; }

protected:
	// BaseServer overrides
	virtual bool		OnLoadConfig(const char* pszConfigPath) override;
	virtual bool		OnInitialize() override;
	virtual i3NetworkServerContext*		OnCreateContext() override;
	virtual i3NetworkSessionManager*	OnCreateSessionManager() override;
	virtual void		OnUpdate() override;
	virtual void		OnShutdown() override;

private:
	BattleServerContext*	m_pBattleContext;
	BattleServerConfig		m_BattleConfig;
	UdpRelay*				m_pUdpRelay;
	MapManager*				m_pMapManager;
	ConfigXML*				m_pConfigXML;
};

#endif // __BATTLESERVERCONTEXT_H__
