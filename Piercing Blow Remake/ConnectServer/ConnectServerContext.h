#ifndef __CONNECTSERVERCONTEXT_H__
#define __CONNECTSERVERCONTEXT_H__

#pragma once
#include "BaseServer.h"
#include "ServerList.h"

class ConnectSessionManager;
class GameServerRegistry;
class ModuleDataClient;

// Configuracion especifica del ConnectServer
struct ConnectServerConfig : public BaseServerConfig
{
	// Puerto para registro de GameServers (puerto separado del de clientes)
	uint16_t	ui16GameServerPort;
	int			i32MaxClientSessions;

	// DataServer connection
	char		szDataServerIP[MAX_SERVER_IP_LENGTH];
	uint16_t	ui16DataServerPort;

	ConnectServerConfig()
		: ui16GameServerPort(0)
		, i32MaxClientSessions(500)
		, ui16DataServerPort(0)
	{
		szDataServerIP[0] = '\0';
	}
};

// Contexto del ConnectServer
// Patron: CServerContext -> i3NetworkServerContext
// Primer punto de contacto del cliente. Maneja autenticacion y seleccion de servidor.
class ConnectServerContext : public i3NetworkServerContext
{
	I3_CLASS_DEFINE(ConnectServerContext);

public:
	ConnectServerContext();
	virtual ~ConnectServerContext();

	// i3NetworkServerContext overrides
	virtual BOOL OnCreate(UINT8 SocketCount, UINT32* pAddress, UINT16* pPort,
						  UINT8* pTimeOut, INT32 WorkCount,
						  i3NetworkSessionManager* pSessionManager) override;
	virtual void OnUpdate(INT32 Command) override;
	virtual BOOL OnDestroy() override;

	// Accessors
	ConnectSessionManager*	GetSessionManager()			{ return m_pConnectSessionManager; }
	GameServerRegistry*		GetRegistry()				{ return m_pRegistry; }
	ServerList*				GetServerList()				{ return &m_ServerList; }
	ModuleDataClient*		GetModuleDataClient()		{ return m_pModuleDataClient; }

	// Initialize inter-server modules
	bool					InitializeModules(const char* pszDataServerIP, uint16_t ui16DataServerPort);

private:
	ConnectSessionManager*	m_pConnectSessionManager;
	GameServerRegistry*		m_pRegistry;
	ServerList				m_ServerList;
	ModuleDataClient*		m_pModuleDataClient;
};

extern ConnectServerContext* g_pConnectServerContext;

// Wrapper de servidor que integra BaseServer con ConnectServerContext
class ConnectServer : public BaseServer
{
public:
	ConnectServer();
	virtual ~ConnectServer();

	// IServer
	virtual ServerType	GetType() const override		{ return ServerType::Connect; }
	virtual const char*	GetName() const override		{ return "ConnectServer"; }

	// Accessors
	ConnectServerContext*	GetContext()					{ return m_pConnectContext; }
	const ConnectServerConfig& GetConnectConfig() const	{ return m_ConnectConfig; }

protected:
	// BaseServer overrides
	virtual bool		OnLoadConfig(const char* pszConfigPath) override;
	virtual bool		OnInitialize() override;
	virtual i3NetworkServerContext*		OnCreateContext() override;
	virtual i3NetworkSessionManager*	OnCreateSessionManager() override;
	virtual void		OnUpdate() override;
	virtual void		OnShutdown() override;

private:
	ConnectServerContext*	m_pConnectContext;
	ConnectServerConfig		m_ConnectConfig;
};

#endif // __CONNECTSERVERCONTEXT_H__
