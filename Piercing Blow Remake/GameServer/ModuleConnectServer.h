#ifndef __MODULECONNECTSERVER_H__
#define __MODULECONNECTSERVER_H__

#pragma once
#include "ModuleBase.h"

#define MAX_PENDING_AUTH_TOKENS		256
#define AUTH_TOKEN_EXPIRE_TIME		60000	// 60 seconds

// Estructura para auth tokens pendientes
struct PendingAuthToken
{
	bool		bActive;
	uint32_t	ui32Token;
	int64_t		i64UID;
	char		szUsername[64];
	uint32_t	ui32ClientIP;
	DWORD		dwExpireTime;

	PendingAuthToken()
		: bActive(false)
		, ui32Token(0)
		, i64UID(0)
		, ui32ClientIP(0)
		, dwExpireTime(0)
	{
		szUsername[0] = '\0';
	}
};

// Modulo de comunicacion con ConnectServer
// Se registra al conectar, recibe auth tokens, envia status updates
class ModuleConnectServer : public ModuleBase
{
public:
	ModuleConnectServer();
	virtual ~ModuleConnectServer();

	// Server info (set before Initialize)
	void		SetServerInfo(int i32ServerId, const char* pszName,
							  const char* pszPublicIP, uint16_t ui16PublicPort,
							  int i32MaxPlayers);

	// Auth token validation (called by GameSession::OnLoginReq)
	bool		ValidateToken(uint32_t ui32Token, int64_t* pUID, char* pUsername);

	// Send status update
	void		SendStatusUpdate(int i32CurrentPlayers);

protected:
	// ModuleBase overrides
	virtual bool	OnConnect() override;
	virtual void	OnDisconnect() override;
	virtual void	OnProcessPacket(char* pData, int i32Size) override;
	virtual void	OnHeartbeat() override;

private:
	void			OnServerRegisterAck(char* pData, int i32Size);
	void			OnPlayerAuthTransferReq(char* pData, int i32Size);
	void			CleanExpiredTokens();

private:
	// Server identity
	int				m_i32ServerId;
	char			m_szServerName[64];
	char			m_szPublicIP[MAX_SERVER_IP_LENGTH];
	uint16_t		m_ui16PublicPort;
	int				m_i32MaxPlayers;

	// Registration state
	bool			m_bRegistered;

	// Auth tokens storage
	PendingAuthToken	m_AuthTokens[MAX_PENDING_AUTH_TOKENS];
	int				m_i32TokenWriteIdx;
};

#endif // __MODULECONNECTSERVER_H__
