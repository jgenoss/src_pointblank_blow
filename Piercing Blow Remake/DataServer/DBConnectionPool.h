#ifndef __DBCONNECTIONPOOL_H__
#define __DBCONNECTIONPOOL_H__

#pragma once
#include <cstdint>

// Forward declaration de libpq (para no exponer el header de PostgreSQL en todo el proyecto)
struct pg_conn;
typedef struct pg_conn PGconn;
struct pg_result;
typedef struct pg_result PGresult;

#define MAX_DB_CONNECTIONS		16
#define MAX_DB_NAME_LENGTH		64
#define MAX_DB_CONNSTR_LENGTH	512
#define MAX_DB_QUERY_LENGTH		4096

// Configuracion de conexion a PostgreSQL
struct DBConfig
{
	char	szHost[MAX_DB_NAME_LENGTH];
	uint16_t ui16Port;
	char	szDatabase[MAX_DB_NAME_LENGTH];
	char	szUser[MAX_DB_NAME_LENGTH];
	char	szPassword[MAX_DB_NAME_LENGTH];

	DBConfig()
		: ui16Port(5432)
	{
		szHost[0]		= '\0';
		szDatabase[0]	= '\0';
		szUser[0]		= '\0';
		szPassword[0]	= '\0';
	}

	// Construir connection string para libpq
	void BuildConnString(char* pszOut, int i32MaxLen) const;
};

// Resultado de query wrapeado
class DBResult
{
public:
	DBResult();
	DBResult(PGresult* pResult);
	~DBResult();

	// No copiable, solo movible
	DBResult(const DBResult&) = delete;
	DBResult& operator=(const DBResult&) = delete;
	DBResult(DBResult&& other) noexcept;
	DBResult& operator=(DBResult&& other) noexcept;

	bool		IsValid() const;
	bool		IsSuccess() const;
	int			GetRowCount() const;
	int			GetColumnCount() const;

	// Getters por fila/columna
	const char*	GetString(int row, int col) const;
	int			GetInt32(int row, int col) const;
	int64_t		GetInt64(int row, int col) const;
	float		GetFloat(int row, int col) const;
	bool		GetBool(int row, int col) const;

	// Getter por nombre de columna
	const char*	GetString(int row, const char* colName) const;
	int			GetInt32(int row, const char* colName) const;
	int64_t		GetInt64(int row, const char* colName) const;

	// Acceso a datos binarios (BYTEA)
	const char*	GetBinary(int row, int col, int* pOutSize) const;

	void		Clear();

private:
	PGresult*	m_pResult;
};

// Conexion individual a PostgreSQL
class DBConnection
{
public:
	DBConnection();
	~DBConnection();

	bool		Connect(const DBConfig& config);
	void		Disconnect();
	bool		IsConnected() const;
	bool		Reconnect();

	// Ejecucion de queries
	DBResult	Execute(const char* pszQuery);
	DBResult	ExecuteParams(const char* pszQuery, int nParams,
							 const char* const* paramValues);

	// Transacciones
	bool		BeginTransaction();
	bool		CommitTransaction();
	bool		RollbackTransaction();

	// Estado
	const char*	GetLastError() const;
	int			GetIndex() const					{ return m_i32Index; }
	void		SetIndex(int idx)					{ m_i32Index = idx; }

private:
	PGconn*		m_pConn;
	DBConfig	m_Config;
	int			m_i32Index;
	bool		m_bInTransaction;
};

// Pool de conexiones a PostgreSQL
// Reemplaza el ADO wrapper original (Ado.h) con libpq nativo
class DBConnectionPool
{
public:
	DBConnectionPool();
	~DBConnectionPool();

	bool			Initialize(const DBConfig& config, int i32PoolSize);
	void			Shutdown();

	// Obtener/devolver conexion del pool
	DBConnection*	AcquireConnection();
	void			ReleaseConnection(DBConnection* pConn);

	// Estado
	int				GetPoolSize() const				{ return m_i32PoolSize; }
	int				GetActiveCount() const			{ return m_i32ActiveCount; }
	bool			IsInitialized() const			{ return m_bInitialized; }

private:
	DBConnection	m_Connections[MAX_DB_CONNECTIONS];
	bool			m_bAvailable[MAX_DB_CONNECTIONS];
	int				m_i32PoolSize;
	int				m_i32ActiveCount;
	bool			m_bInitialized;
	DBConfig		m_Config;

	// Mutex para thread-safety
	CRITICAL_SECTION	m_CritSection;
};

#endif // __DBCONNECTIONPOOL_H__
