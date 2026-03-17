#ifndef __DATASERVERCONTEXT_H__
#define __DATASERVERCONTEXT_H__

#pragma once
#include "BaseServer.h"
#include "DBConnectionPool.h"

class DataSessionManager;
class TaskProcessor;
class ModuleDBAuth;
class ModuleDBUserLoad;
class ModuleDBUserSave;
class ModuleDBGameData;
class ModuleDBSocial;

// Configuracion especifica del DataServer
struct DataServerConfig : public BaseServerConfig
{
	// PostgreSQL
	DBConfig	dbConfig;

	// Pool
	int			i32DBPoolSize;

	DataServerConfig()
		: i32DBPoolSize(8)
	{
	}
};

// Contexto del DataServer
// Patron portado de TransServer: acepta conexiones de ConnectServer, GameServer, BattleServer
// Cada servidor conectado tiene su propia sesion de control
class DataServerContext : public i3NetworkServerContext
{
	I3_CLASS_DEFINE(DataServerContext);

public:
	DataServerContext();
	virtual ~DataServerContext();

	// i3NetworkServerContext overrides
	virtual BOOL OnCreate(UINT8 SocketCount, UINT32* pAddress, UINT16* pPort,
						  UINT8* pTimeOut, INT32 WorkCount,
						  i3NetworkSessionManager* pSessionManager) override;
	virtual void OnUpdate(INT32 Command) override;
	virtual BOOL OnDestroy() override;

	// Accessors
	DBConnectionPool*	GetDBPool()						{ return m_pDBPool; }
	TaskProcessor*		GetTaskProcessor()				{ return m_pTaskProcessor; }
	ModuleDBAuth*		GetModuleDBAuth()				{ return m_pModuleAuth; }
	ModuleDBUserLoad*	GetModuleDBUserLoad()			{ return m_pModuleUserLoad; }
	ModuleDBUserSave*	GetModuleDBUserSave()			{ return m_pModuleUserSave; }
	ModuleDBGameData*	GetModuleDBGameData()			{ return m_pModuleGameData; }
	ModuleDBSocial*		GetModuleDBSocial()				{ return m_pModuleSocial; }
	DataSessionManager*	GetDataSessionManager()			{ return m_pDataSessionManager; }

	// Inicializar modulos de DB (llamado despues de OnCreate)
	bool				InitializeDBModules(const DBConfig& config, int i32PoolSize);

private:
	DataSessionManager*		m_pDataSessionManager;
	DBConnectionPool*		m_pDBPool;
	TaskProcessor*			m_pTaskProcessor;

	// Modulos de DB (patron i3Thread del original)
	ModuleDBAuth*			m_pModuleAuth;
	ModuleDBUserLoad*		m_pModuleUserLoad;
	ModuleDBUserSave*		m_pModuleUserSave;
	ModuleDBGameData*		m_pModuleGameData;
	ModuleDBSocial*			m_pModuleSocial;
};

extern DataServerContext* g_pDataServerContext;

// Wrapper de servidor
class DataServer : public BaseServer
{
public:
	DataServer();
	virtual ~DataServer();

	virtual ServerType	GetType() const override		{ return ServerType::Data; }
	virtual const char*	GetName() const override		{ return "DataServer"; }

	DataServerContext*		GetDataContext()				{ return m_pDataContext; }
	const DataServerConfig&	GetDataConfig() const		{ return m_DataConfig; }

protected:
	virtual bool		OnLoadConfig(const char* pszConfigPath) override;
	virtual bool		OnInitialize() override;
	virtual i3NetworkServerContext*		OnCreateContext() override;
	virtual i3NetworkSessionManager*	OnCreateSessionManager() override;
	virtual void		OnUpdate() override;
	virtual void		OnShutdown() override;

private:
	DataServerContext*	m_pDataContext;
	DataServerConfig	m_DataConfig;
};

#endif // __DATASERVERCONTEXT_H__
