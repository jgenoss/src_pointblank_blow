#ifndef __MODULEBASE_H__
#define __MODULEBASE_H__

#pragma once
#include "i3Thread.h"
#include "i3RingBuffer.h"
#include "i3NetworkPacket.h"

#define MODULE_RING_BUFFER_SIZE		4096
#define MODULE_RING_BUFFER_COUNT	256

#define MODULE_RECONNECT_INTERVAL	5000		// 5 segundos entre intentos de reconexion
#define MODULE_HEARTBEAT_INTERVAL	10000		// 10 segundos entre heartbeats

// Estructura para buffers de mensajes por thread (patron del original CModuleAuth)
struct ModuleNetBuffer
{
	i3RingBuffer*		pRingBuffer;
	int					i32ThreadIdx;

	ModuleNetBuffer()
		: pRingBuffer(nullptr)
		, i32ThreadIdx(0)
	{
	}
};

// Base abstracta para modulos de comunicacion inter-servidor
// Sigue el patron de CModuleAuth: i3Thread + ring buffers + socket cliente
// Cada modulo corre en su propio thread y se conecta a un servidor remoto
class ModuleBase : public i3Thread
{
public:
	ModuleBase();
	virtual ~ModuleBase();

	// Inicializacion
	bool				Initialize(const char* pszName, const char* pszRemoteIP, uint16_t ui16RemotePort, int i32WorkerThreadCount);
	void				Destroy();

	// Estado
	bool				IsConnected() const				{ return m_bConnected; }
	const char*			GetRemoteIP() const				{ return m_szRemoteIP; }
	uint16_t			GetRemotePort() const			{ return m_ui16RemotePort; }

	// Envio de paquetes al ring buffer desde worker threads (thread-safe)
	bool				SendToModule(int i32ThreadIdx, i3NetworkPacket* pPacket);

	// Envio de paquetes al ring buffer del thread principal
	bool				SendToModuleMain(i3NetworkPacket* pPacket);

protected:
	// i3Thread overrides
	virtual void		BeforeRunning(void* pUserData) override;
	virtual UINT32		OnRunning(void* pUserData) override;
	virtual void		AfterRunning(void* pUserData) override;

	// Metodos virtuales para servidores especificos
	virtual bool		OnConnect() = 0;					// Llamado despues de conectar
	virtual void		OnDisconnect() = 0;					// Llamado al desconectar
	virtual void		OnProcessPacket(char* pData, int i32Size) = 0;	// Procesar paquete recibido
	virtual void		OnHeartbeat() {}					// Enviar heartbeat (opcional)

	// Utilidades de red
	bool				ConnectToRemote();
	void				DisconnectFromRemote();
	bool				SendPacket(i3NetworkPacket* pPacket);
	void				ProcessRingBuffers();
	void				ProcessReceivedData();

protected:
	// Estado de conexion
	bool				m_bConnected;
	SOCKET				m_Socket;
	char				m_szRemoteIP[32];
	uint16_t			m_ui16RemotePort;

	// Kill event para shutdown del thread
	HANDLE				m_hKillEvent;

	// Ring buffers: uno por worker thread + uno para main thread
	ModuleNetBuffer		m_MainBuffer;					// Buffer del thread principal
	ModuleNetBuffer*	m_pWorkerBuffers;				// Array de buffers por worker thread
	int					m_i32WorkerThreadCount;

	// Timers
	DWORD				m_dwLastHeartbeat;
	DWORD				m_dwLastReconnectAttempt;

	// Buffer de recepcion
	char				m_RecvBuffer[PACKETBUFFERSIZE];
	int					m_i32RecvSize;
};

#endif // __MODULEBASE_H__
