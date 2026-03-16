#include "ModuleDBAuth.h"
#include "DBConnectionPool.h"
#include <cstdio>
#include <cstring>

ModuleDBAuth::ModuleDBAuth(DBConnectionPool* pPool)
	: m_pPool(pPool)
{
}

ModuleDBAuth::~ModuleDBAuth()
{
}

bool ModuleDBAuth::AuthenticateUser(const char* pszUsername, const char* pszPassword,
									IS_AUTH_LOGIN_ACK* pOut)
{
	if (!m_pPool || !pOut)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
	{
		pOut->i32Result = -1;	// Error interno
		return false;
	}

	// Query parametrizada para evitar SQL injection
	const char* paramValues[1] = { pszUsername };
	DBResult result = pConn->ExecuteParams(
		"SELECT uid, password_hash, auth_level, is_banned "
		"FROM pb_users WHERE username = $1",
		1, paramValues);

	if (!result.IsSuccess() || result.GetRowCount() == 0)
	{
		// Usuario no encontrado
		pOut->i32Result = 1;	// AUTH_RESULT_INVALID_USER
		m_pPool->ReleaseConnection(pConn);
		return false;
	}

	// Verificar ban
	if (result.GetBool(0, "is_banned"))
	{
		pOut->i32Result = 3;	// AUTH_RESULT_BANNED
		m_pPool->ReleaseConnection(pConn);
		return false;
	}

	// Verificar password (plaintext en v1, TODO: bcrypt)
	const char* pszStoredHash = result.GetString(0, "password_hash");
	if (strcmp(pszPassword, pszStoredHash) != 0)
	{
		pOut->i32Result = 2;	// AUTH_RESULT_INVALID_PASSWORD
		m_pPool->ReleaseConnection(pConn);
		return false;
	}

	// Autenticacion exitosa
	pOut->i64UID = result.GetInt64(0, "uid");
	pOut->ui8AuthLevel = (uint8_t)result.GetInt32(0, "auth_level");
	pOut->i32Result = 0;		// AUTH_RESULT_OK

	// Actualizar last_login
	char szQuery[MAX_DB_QUERY_LENGTH];
	snprintf(szQuery, sizeof(szQuery),
		"UPDATE pb_users SET last_login = NOW() WHERE uid = %lld",
		pOut->i64UID);
	pConn->Execute(szQuery);

	m_pPool->ReleaseConnection(pConn);
	return true;
}

int ModuleDBAuth::CreateNickname(int64_t i64UID, const char* pszNickname)
{
	if (!m_pPool || !pszNickname || pszNickname[0] == '\0')
		return 2;	// Invalid

	// Verificar longitud
	size_t len = strlen(pszNickname);
	if (len < 2 || len > 16)
		return 2;	// Invalid

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return -1;

	// Verificar si el nickname ya existe
	const char* checkParams[1] = { pszNickname };
	DBResult checkResult = pConn->ExecuteParams(
		"SELECT 1 FROM pb_users WHERE nickname = $1",
		1, checkParams);

	if (checkResult.IsSuccess() && checkResult.GetRowCount() > 0)
	{
		m_pPool->ReleaseConnection(pConn);
		return 1;	// Exists
	}

	// Intentar asignar nickname (solo si no tiene uno)
	char szUID[32];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);
	const char* updateParams[2] = { pszNickname, szUID };
	DBResult updateResult = pConn->ExecuteParams(
		"UPDATE pb_users SET nickname = $1 WHERE uid = $2 AND nickname IS NULL",
		2, updateParams);

	m_pPool->ReleaseConnection(pConn);

	if (!updateResult.IsSuccess())
		return -1;	// Error

	return 0;	// OK
}

bool ModuleDBAuth::CheckNicknameExists(const char* pszNickname)
{
	if (!m_pPool || !pszNickname || pszNickname[0] == '\0')
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	const char* paramValues[1] = { pszNickname };
	DBResult result = pConn->ExecuteParams(
		"SELECT 1 FROM pb_users WHERE nickname = $1",
		1, paramValues);

	bool bExists = (result.IsSuccess() && result.GetRowCount() > 0);

	m_pPool->ReleaseConnection(pConn);
	return bExists;
}

void ModuleDBAuth::ProcessResponses(DataServerContext* pContext)
{
	// Placeholder para futuro async con ring buffers
	// En v1 sincronico, las respuestas se envian directamente en cada metodo
}
