#ifndef __CONNECTSESSION_H__
#define __CONNECTSESSION_H__

#pragma once
#include "i3NetworkSession.h"
#include "i3NetworkPacket.h"
#include "InterServerProtocol.h"

// Estados de la sesion del ConnectServer
// Sesion de vida corta: el cliente conecta, se autentica, selecciona servidor y se desconecta
enum ConnectSessionTask
{
	CONNECT_TASK_NONE = 0,
	CONNECT_TASK_CONNECTED,			// Conexion TCP establecida
	CONNECT_TASK_HANDSHAKE,			// Handshake RSA completado (XOR key intercambiado)
	CONNECT_TASK_AUTHENTICATED,		// Login validado exitosamente
	CONNECT_TASK_SERVER_SELECT,		// Seleccionando GameServer
	CONNECT_TASK_DISCONNECTING,		// En proceso de desconexion
};

#define CONNECT_SESSION_TIMEOUT		30		// 30 segundos timeout para sesion completa

// Sesion del ConnectServer
// Patron portado de CUserSession (solo la fase de login)
// Cada cliente que conecta obtiene una ConnectSession temporal
class ConnectSession : public i3NetworkSession
{
	I3_CLASS_DEFINE(ConnectSession);

public:
	ConnectSession();
	virtual ~ConnectSession();

	// i3NetworkSession overrides
	virtual BOOL		Create() override;
	virtual void		Destroy() override;
	virtual BOOL		OnConnect(SOCKET Socket, struct sockaddr_in* pAddr) override;
	virtual BOOL		OnConnectInit() override;
	virtual BOOL		OnDisconnect(BOOL bForceMainThread = FALSE) override;
	virtual INT32		PacketParsing(char* pPacket, INT32 iSize) override;

	// Estado
	ConnectSessionTask	GetTask() const					{ return m_eTask; }
	void				SetTask(ConnectSessionTask eTask){ m_eTask = eTask; }

	// Auth data
	int64_t				GetUID() const					{ return m_i64UID; }
	void				SetUID(int64_t i64UID)			{ m_i64UID = i64UID; }
	const char*			GetUsername() const				{ return m_szUsername; }
	uint32_t			GetAuthToken() const			{ return m_ui32AuthToken; }
	uint32_t			GetXorKey() const				{ return m_ui32XorKey; }

	// Timeout
	bool				IsTimedOut() const;

	// Callback from ModuleDataClient
	void				OnLoginResult(int i32Result, int64_t i64UID, uint8_t ui8AuthLevel);

private:
	// Packet handlers
	void				OnPacketConnectReq(char* pData, INT32 i32Size);
	void				OnPacketLoginReq(char* pData, INT32 i32Size);
	void				OnPacketServerListReq(char* pData, INT32 i32Size);
	void				OnPacketServerSelectReq(char* pData, INT32 i32Size);
	void				OnPacketHeartBitReq(char* pData, INT32 i32Size);

	// Inter-server packet handlers (from GameServer connections)
	void				OnISServerRegisterReq(char* pData, INT32 i32Size);
	void				OnISHeartbeatReq(char* pData, INT32 i32Size);
	void				OnISServerStatusUpdateReq(char* pData, INT32 i32Size);

	// Send auth token to GameServer session
	bool				SendAuthTransferToGameServer(int i32ServerId,
								int64_t i64UID, uint32_t ui32AuthToken,
								const char* pszUsername, uint32_t ui32ClientIP);

	// Helpers
	void				SendConnectAck();
	void				SendLoginAck(int i32Result);
	void				SendServerList();
	void				SendServerSelectAck(int i32Result, const char* pszIP, uint16_t ui16Port);
	uint32_t			GenerateAuthToken();

private:
	ConnectSessionTask	m_eTask;
	int64_t				m_i64UID;
	char				m_szUsername[64];
	uint32_t			m_ui32AuthToken;
	uint32_t			m_ui32XorKey;
	DWORD				m_dwConnectTime;		// Tick count al conectar
};

#endif // __CONNECTSESSION_H__
