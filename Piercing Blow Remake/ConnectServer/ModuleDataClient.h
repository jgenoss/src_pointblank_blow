#ifndef __MODULEDATACLIENT_H__
#define __MODULEDATACLIENT_H__

#pragma once
#include "ModuleBase.h"
#include "InterServerProtocol.h"

class ConnectSessionManager;

// Modulo de comunicacion ConnectServer -> DataServer
// Patron: ModuleBase (i3Thread + socket cliente)
// Envia requests de autenticacion y recibe resultados
class ModuleDataClient : public ModuleBase
{
public:
	ModuleDataClient();
	virtual ~ModuleDataClient();

	// Session manager reference (for callbacks)
	void	SetSessionManager(ConnectSessionManager* pMgr)	{ m_pSessionManager = pMgr; }

	// Auth request
	void	RequestLogin(int i32SessionIdx, const char* pszUsername,
						 const char* pszPassword, uint32_t ui32ClientIP);

protected:
	// ModuleBase overrides
	virtual bool	OnConnect() override;
	virtual void	OnDisconnect() override;
	virtual void	OnProcessPacket(char* pData, int i32Size) override;
	virtual void	OnHeartbeat() override;

private:
	// Response handlers
	void	OnLoginAck(char* pData, int i32Size);

	// Packet helper
	void	SendRequest(uint16_t ui16Protocol, const void* pStruct, int i32StructSize);

private:
	ConnectSessionManager*	m_pSessionManager;
};

#endif // __MODULEDATACLIENT_H__
