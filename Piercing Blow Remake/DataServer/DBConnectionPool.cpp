#include "DBConnectionPool.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

// libpq header
#include <libpq-fe.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DBConfig
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DBConfig::BuildConnString(char* pszOut, int i32MaxLen) const
{
	snprintf(pszOut, i32MaxLen,
		"host=%s port=%d dbname=%s user=%s password=%s",
		szHost, ui16Port, szDatabase, szUser, szPassword);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DBResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DBResult::DBResult()
	: m_pResult(nullptr)
{
}

DBResult::DBResult(PGresult* pResult)
	: m_pResult(pResult)
{
}

DBResult::~DBResult()
{
	Clear();
}

DBResult::DBResult(DBResult&& other) noexcept
	: m_pResult(other.m_pResult)
{
	other.m_pResult = nullptr;
}

DBResult& DBResult::operator=(DBResult&& other) noexcept
{
	if (this != &other)
	{
		Clear();
		m_pResult = other.m_pResult;
		other.m_pResult = nullptr;
	}
	return *this;
}

bool DBResult::IsValid() const
{
	return m_pResult != nullptr;
}

bool DBResult::IsSuccess() const
{
	if (!m_pResult)
		return false;

	ExecStatusType status = PQresultStatus(m_pResult);
	return (status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK);
}

int DBResult::GetRowCount() const
{
	if (!m_pResult)
		return 0;
	return PQntuples(m_pResult);
}

int DBResult::GetColumnCount() const
{
	if (!m_pResult)
		return 0;
	return PQnfields(m_pResult);
}

const char* DBResult::GetString(int row, int col) const
{
	if (!m_pResult || PQgetisnull(m_pResult, row, col))
		return "";
	return PQgetvalue(m_pResult, row, col);
}

int DBResult::GetInt32(int row, int col) const
{
	const char* val = GetString(row, col);
	return val[0] ? atoi(val) : 0;
}

int64_t DBResult::GetInt64(int row, int col) const
{
	const char* val = GetString(row, col);
	return val[0] ? _atoi64(val) : 0;
}

float DBResult::GetFloat(int row, int col) const
{
	const char* val = GetString(row, col);
	return val[0] ? (float)atof(val) : 0.0f;
}

bool DBResult::GetBool(int row, int col) const
{
	const char* val = GetString(row, col);
	return (val[0] == 't' || val[0] == '1');
}

const char* DBResult::GetString(int row, const char* colName) const
{
	if (!m_pResult)
		return "";
	int col = PQfnumber(m_pResult, colName);
	if (col < 0)
		return "";
	return GetString(row, col);
}

int DBResult::GetInt32(int row, const char* colName) const
{
	if (!m_pResult)
		return 0;
	int col = PQfnumber(m_pResult, colName);
	if (col < 0)
		return 0;
	return GetInt32(row, col);
}

int64_t DBResult::GetInt64(int row, const char* colName) const
{
	if (!m_pResult)
		return 0;
	int col = PQfnumber(m_pResult, colName);
	if (col < 0)
		return 0;
	return GetInt64(row, col);
}

const char* DBResult::GetBinary(int row, int col, int* pOutSize) const
{
	if (!m_pResult || PQgetisnull(m_pResult, row, col))
	{
		if (pOutSize) *pOutSize = 0;
		return nullptr;
	}

	if (pOutSize)
		*pOutSize = PQgetlength(m_pResult, row, col);

	return PQgetvalue(m_pResult, row, col);
}

void DBResult::Clear()
{
	if (m_pResult)
	{
		PQclear(m_pResult);
		m_pResult = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DBConnection
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DBConnection::DBConnection()
	: m_pConn(nullptr)
	, m_i32Index(0)
	, m_bInTransaction(false)
{
}

DBConnection::~DBConnection()
{
	Disconnect();
}

bool DBConnection::Connect(const DBConfig& config)
{
	m_Config = config;

	char szConnStr[MAX_DB_CONNSTR_LENGTH];
	config.BuildConnString(szConnStr, sizeof(szConnStr));

	m_pConn = PQconnectdb(szConnStr);
	if (PQstatus(m_pConn) != CONNECTION_OK)
	{
		printf("[DBConnection:%d] Connect failed: %s\n", m_i32Index, PQerrorMessage(m_pConn));
		PQfinish(m_pConn);
		m_pConn = nullptr;
		return false;
	}

	printf("[DBConnection:%d] Connected to %s:%d/%s\n",
		m_i32Index, config.szHost, config.ui16Port, config.szDatabase);

	return true;
}

void DBConnection::Disconnect()
{
	if (m_pConn)
	{
		if (m_bInTransaction)
			RollbackTransaction();

		PQfinish(m_pConn);
		m_pConn = nullptr;
	}
}

bool DBConnection::IsConnected() const
{
	if (!m_pConn)
		return false;
	return (PQstatus(m_pConn) == CONNECTION_OK);
}

bool DBConnection::Reconnect()
{
	Disconnect();
	return Connect(m_Config);
}

DBResult DBConnection::Execute(const char* pszQuery)
{
	if (!m_pConn)
		return DBResult();

	PGresult* pResult = PQexec(m_pConn, pszQuery);
	ExecStatusType status = PQresultStatus(pResult);

	if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK)
	{
		printf("[DBConnection:%d] Query error: %s\nQuery: %.200s\n",
			m_i32Index, PQerrorMessage(m_pConn), pszQuery);

		// Intentar reconectar si se perdio la conexion
		if (PQstatus(m_pConn) != CONNECTION_OK)
		{
			PQclear(pResult);
			if (Reconnect())
			{
				pResult = PQexec(m_pConn, pszQuery);
				status = PQresultStatus(pResult);
				if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK)
				{
					PQclear(pResult);
					return DBResult();
				}
				return DBResult(pResult);
			}
			return DBResult();
		}
	}

	return DBResult(pResult);
}

DBResult DBConnection::ExecuteParams(const char* pszQuery, int nParams,
									  const char* const* paramValues)
{
	if (!m_pConn)
		return DBResult();

	PGresult* pResult = PQexecParams(m_pConn, pszQuery, nParams,
									  nullptr, paramValues, nullptr, nullptr, 0);

	ExecStatusType status = PQresultStatus(pResult);
	if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK)
	{
		printf("[DBConnection:%d] Parameterized query error: %s\n",
			m_i32Index, PQerrorMessage(m_pConn));
		PQclear(pResult);
		return DBResult();
	}

	return DBResult(pResult);
}

bool DBConnection::BeginTransaction()
{
	DBResult result = Execute("BEGIN");
	if (result.IsSuccess())
	{
		m_bInTransaction = true;
		return true;
	}
	return false;
}

bool DBConnection::CommitTransaction()
{
	DBResult result = Execute("COMMIT");
	m_bInTransaction = false;
	return result.IsSuccess();
}

bool DBConnection::RollbackTransaction()
{
	DBResult result = Execute("ROLLBACK");
	m_bInTransaction = false;
	return result.IsSuccess();
}

const char* DBConnection::GetLastError() const
{
	if (!m_pConn)
		return "No connection";
	return PQerrorMessage(m_pConn);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DBConnectionPool
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DBConnectionPool::DBConnectionPool()
	: m_i32PoolSize(0)
	, m_i32ActiveCount(0)
	, m_bInitialized(false)
{
	memset(m_bAvailable, 0, sizeof(m_bAvailable));
	InitializeCriticalSection(&m_CritSection);
}

DBConnectionPool::~DBConnectionPool()
{
	Shutdown();
	DeleteCriticalSection(&m_CritSection);
}

bool DBConnectionPool::Initialize(const DBConfig& config, int i32PoolSize)
{
	if (m_bInitialized)
		return false;

	if (i32PoolSize <= 0 || i32PoolSize > MAX_DB_CONNECTIONS)
		i32PoolSize = MAX_DB_CONNECTIONS;

	m_Config = config;
	m_i32PoolSize = i32PoolSize;

	printf("[DBConnectionPool] Initializing pool with %d connections to %s:%d/%s\n",
		i32PoolSize, config.szHost, config.ui16Port, config.szDatabase);

	int i32Connected = 0;
	for (int i = 0; i < i32PoolSize; ++i)
	{
		m_Connections[i].SetIndex(i);
		if (m_Connections[i].Connect(config))
		{
			m_bAvailable[i] = true;
			++i32Connected;
		}
		else
		{
			m_bAvailable[i] = false;
		}
	}

	if (i32Connected == 0)
	{
		printf("[DBConnectionPool] ERROR: No connections could be established!\n");
		return false;
	}

	printf("[DBConnectionPool] Pool ready: %d/%d connections established\n",
		i32Connected, i32PoolSize);

	m_bInitialized = true;
	return true;
}

void DBConnectionPool::Shutdown()
{
	if (!m_bInitialized)
		return;

	EnterCriticalSection(&m_CritSection);

	for (int i = 0; i < m_i32PoolSize; ++i)
	{
		m_Connections[i].Disconnect();
		m_bAvailable[i] = false;
	}

	m_i32ActiveCount = 0;
	m_bInitialized = false;

	LeaveCriticalSection(&m_CritSection);

	printf("[DBConnectionPool] Shutdown complete\n");
}

DBConnection* DBConnectionPool::AcquireConnection()
{
	EnterCriticalSection(&m_CritSection);

	for (int i = 0; i < m_i32PoolSize; ++i)
	{
		if (m_bAvailable[i])
		{
			// Verificar que la conexion sigue viva
			if (!m_Connections[i].IsConnected())
			{
				if (!m_Connections[i].Reconnect())
					continue;
			}

			m_bAvailable[i] = false;
			++m_i32ActiveCount;
			LeaveCriticalSection(&m_CritSection);
			return &m_Connections[i];
		}
	}

	LeaveCriticalSection(&m_CritSection);
	printf("[DBConnectionPool] WARNING: No connections available! (active=%d/%d)\n",
		m_i32ActiveCount, m_i32PoolSize);
	return nullptr;
}

void DBConnectionPool::ReleaseConnection(DBConnection* pConn)
{
	if (!pConn)
		return;

	EnterCriticalSection(&m_CritSection);

	int idx = pConn->GetIndex();
	if (idx >= 0 && idx < m_i32PoolSize)
	{
		m_bAvailable[idx] = true;
		--m_i32ActiveCount;
	}

	LeaveCriticalSection(&m_CritSection);
}
