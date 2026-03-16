#ifndef __BASESERVER_H__
#define __BASESERVER_H__

#pragma once
#include "IServer.h"
#include "ServerInfo.h"
#include "i3NetworkServerContext.h"

#define MAX_CONFIG_PATH		512
#define MAX_SERVER_LOG_PATH	512

// Configuracion base compartida por todos los servidores
struct BaseServerConfig
{
	char			szBindIP[MAX_SERVER_IP_LENGTH];
	uint16_t		ui16BindPort;
	int				i32MaxSessions;
	int				i32WorkerThreadCount;
	uint8_t			ui8SocketTimeout;

	BaseServerConfig()
	{
		szBindIP[0]				= '\0';
		ui16BindPort			= 0;
		i32MaxSessions			= 100;
		i32WorkerThreadCount	= 4;
		ui8SocketTimeout		= 30;
	}
};

// Clase base abstracta para todos los servidores del Remake
// Encapsula i3NetworkServerContext y provee el ciclo de vida comun
class BaseServer : public IServer
{
public:
	BaseServer();
	virtual ~BaseServer();

	// IServer interface
	virtual bool		Initialize(const char* pszConfigPath) override;
	virtual bool		Start() override;
	virtual void		Update() override;
	virtual void		Shutdown(int i32Reason) override;

	// Accessors
	ServerState			GetServerState() const				{ return m_eState; }
	const BaseServerConfig&	GetConfig() const				{ return m_Config; }
	i3NetworkServerContext*	GetContext()						{ return m_pContext; }

protected:
	// Metodos virtuales para que cada servidor especializado implemente
	virtual bool		OnLoadConfig(const char* pszConfigPath) = 0;
	virtual bool		OnInitialize() = 0;
	virtual i3NetworkServerContext*		OnCreateContext() = 0;
	virtual i3NetworkSessionManager*	OnCreateSessionManager() = 0;
	virtual void		OnUpdate() = 0;
	virtual void		OnShutdown() = 0;

	// Utilidades
	void				SetState(ServerState eState)		{ m_eState = eState; }
	void				LogMessage(const char* pszFormat, ...);

protected:
	ServerState					m_eState;
	BaseServerConfig			m_Config;
	i3NetworkServerContext*		m_pContext;
	i3NetworkSessionManager*	m_pSessionManager;

	char						m_szConfigPath[MAX_CONFIG_PATH];
	char						m_szLogPath[MAX_SERVER_LOG_PATH];
};

#endif // __BASESERVER_H__
