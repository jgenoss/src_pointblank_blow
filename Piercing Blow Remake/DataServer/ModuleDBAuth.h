#ifndef __MODULEDBAUTH_H__
#define __MODULEDBAUTH_H__

#pragma once
#include "InterServerProtocol.h"

class DBConnectionPool;
class DataServerContext;

// Modulo de autenticacion de base de datos
// Valida credenciales, gestiona nicknames
// Patron portado de CModuleAuth (Server/Source/Game/Game/ModuleAuth.h)
// Version simplificada: sincronico (sin i3Thread), queries directas via pool
class ModuleDBAuth
{
public:
	ModuleDBAuth(DBConnectionPool* pPool);
	~ModuleDBAuth();

	// Autenticacion
	bool AuthenticateUser(const char* pszUsername, const char* pszPassword,
						  IS_AUTH_LOGIN_ACK* pOut);

	// Nicknames
	int  CreateNickname(int64_t i64UID, const char* pszNickname);	// 0=OK, 1=exists, 2=invalid
	bool CheckNicknameExists(const char* pszNickname);

	// Placeholder para futuro async (patron del original)
	void ProcessResponses(DataServerContext* pContext);

private:
	DBConnectionPool*	m_pPool;
};

#endif // __MODULEDBAUTH_H__
