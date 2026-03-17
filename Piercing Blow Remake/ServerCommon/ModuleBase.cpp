#include "i3CommonType.h"
#include "ModuleBase.h"
#include <cstdio>
#include <cstring>

#ifndef _WINSOCK2API_
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

ModuleBase::ModuleBase()
	: m_bConnected(false)
	, m_Socket(INVALID_SOCKET)
	, m_ui16RemotePort(0)
	, m_hKillEvent(NULL)
	, m_pWorkerBuffers(nullptr)
	, m_i32WorkerThreadCount(0)
	, m_dwLastHeartbeat(0)
	, m_dwLastReconnectAttempt(0)
	, m_i32RecvSize(0)
{
	m_szRemoteIP[0] = '\0';
	memset(m_RecvBuffer, 0, sizeof(m_RecvBuffer));
}

ModuleBase::~ModuleBase()
{
	Destroy();
}

bool ModuleBase::Initialize(const char* pszName, const char* pszRemoteIP, uint16_t ui16RemotePort, int i32WorkerThreadCount)
{
	// Guardar configuracion de conexion
	strncpy(m_szRemoteIP, pszRemoteIP, sizeof(m_szRemoteIP) - 1);
	m_ui16RemotePort = ui16RemotePort;
	m_i32WorkerThreadCount = i32WorkerThreadCount;

	// Crear kill event
	m_hKillEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hKillEvent == NULL)
		return false;

	// Crear ring buffer del thread principal
	m_MainBuffer.pRingBuffer = new i3RingBuffer();
	if (!m_MainBuffer.pRingBuffer->Create(MODULE_RING_BUFFER_SIZE, MODULE_RING_BUFFER_COUNT))
		return false;
	m_MainBuffer.i32ThreadIdx = -1; // Main thread

	// Crear ring buffers por worker thread
	if (i32WorkerThreadCount > 0)
	{
		m_pWorkerBuffers = new ModuleNetBuffer[i32WorkerThreadCount];
		for (int i = 0; i < i32WorkerThreadCount; ++i)
		{
			m_pWorkerBuffers[i].pRingBuffer = new i3RingBuffer();
			if (!m_pWorkerBuffers[i].pRingBuffer->Create(MODULE_RING_BUFFER_SIZE, MODULE_RING_BUFFER_COUNT))
				return false;
			m_pWorkerBuffers[i].i32ThreadIdx = i;
		}
	}

	// Crear el thread del modulo
	if (!Create((char*)pszName, 0, 8192, nullptr, PRIORITY_NORMAL))
		return false;

	printf("[Module:%s] Initialized. Target: %s:%d\n", pszName, pszRemoteIP, ui16RemotePort);
	return true;
}

void ModuleBase::Destroy()
{
	// Senalizar kill
	if (m_hKillEvent)
	{
		SetEvent(m_hKillEvent);
		WaitForFinish();
		CloseHandle(m_hKillEvent);
		m_hKillEvent = NULL;
	}

	// Desconectar
	DisconnectFromRemote();

	// Limpiar ring buffers
	if (m_MainBuffer.pRingBuffer)
	{
		m_MainBuffer.pRingBuffer->Destroy();
		delete m_MainBuffer.pRingBuffer;
		m_MainBuffer.pRingBuffer = nullptr;
	}

	if (m_pWorkerBuffers)
	{
		for (int i = 0; i < m_i32WorkerThreadCount; ++i)
		{
			if (m_pWorkerBuffers[i].pRingBuffer)
			{
				m_pWorkerBuffers[i].pRingBuffer->Destroy();
				delete m_pWorkerBuffers[i].pRingBuffer;
			}
		}
		delete[] m_pWorkerBuffers;
		m_pWorkerBuffers = nullptr;
	}
}

bool ModuleBase::SendToModule(int i32ThreadIdx, i3NetworkPacket* pPacket)
{
	if (i32ThreadIdx < 0 || i32ThreadIdx >= m_i32WorkerThreadCount)
		return false;

	if (!m_pWorkerBuffers[i32ThreadIdx].pRingBuffer)
		return false;

	void* pBuf = m_pWorkerBuffers[i32ThreadIdx].pRingBuffer->PushPointer();
	if (!pBuf)
		return false;

	memcpy(pBuf, pPacket->GetPacketBuffer(), pPacket->GetPacketSize());
	m_pWorkerBuffers[i32ThreadIdx].pRingBuffer->PushPointerIdx();
	return true;
}

bool ModuleBase::SendToModuleMain(i3NetworkPacket* pPacket)
{
	if (!m_MainBuffer.pRingBuffer)
		return false;

	void* pBuf = m_MainBuffer.pRingBuffer->PushPointer();
	if (!pBuf)
		return false;

	memcpy(pBuf, pPacket->GetPacketBuffer(), pPacket->GetPacketSize());
	m_MainBuffer.pRingBuffer->PushPointerIdx();
	return true;
}

// -- Thread lifecycle --

void ModuleBase::BeforeRunning(void* pUserData)
{
	m_dwLastHeartbeat = GetTickCount();
	m_dwLastReconnectAttempt = 0;
}

UINT32 ModuleBase::OnRunning(void* pUserData)
{
	while (WaitForSingleObject(m_hKillEvent, 1) == WAIT_TIMEOUT)
	{
		DWORD dwNow = GetTickCount();

		// Intentar conectar si no estamos conectados
		if (!m_bConnected)
		{
			if (dwNow - m_dwLastReconnectAttempt >= MODULE_RECONNECT_INTERVAL)
			{
				m_dwLastReconnectAttempt = dwNow;
				ConnectToRemote();
			}
			continue;
		}

		// Procesar ring buffers (enviar paquetes encolados)
		ProcessRingBuffers();

		// Recibir datos
		ProcessReceivedData();

		// Heartbeat periodico
		if (dwNow - m_dwLastHeartbeat >= MODULE_HEARTBEAT_INTERVAL)
		{
			m_dwLastHeartbeat = dwNow;
			OnHeartbeat();
		}
	}

	return 0;
}

void ModuleBase::AfterRunning(void* pUserData)
{
	DisconnectFromRemote();
}

// -- Networking --

bool ModuleBase::ConnectToRemote()
{
	if (m_bConnected)
		return true;

	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_Socket == INVALID_SOCKET)
		return false;

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_ui16RemotePort);
	addr.sin_addr.s_addr = inet_addr(m_szRemoteIP);

	if (connect(m_Socket, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
		return false;
	}

	// Poner socket en modo no-bloqueante
	u_long ulMode = 1;
	ioctlsocket(m_Socket, FIONBIO, &ulMode);

	m_bConnected = true;
	m_i32RecvSize = 0;

	printf("[Module] Connected to %s:%d\n", m_szRemoteIP, m_ui16RemotePort);

	return OnConnect();
}

void ModuleBase::DisconnectFromRemote()
{
	if (m_Socket != INVALID_SOCKET)
	{
		OnDisconnect();
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
		m_bConnected = false;
		m_i32RecvSize = 0;
	}
}

bool ModuleBase::SendPacket(i3NetworkPacket* pPacket)
{
	if (!m_bConnected || m_Socket == INVALID_SOCKET)
		return false;

	int i32Size = pPacket->GetPacketSize();
	const char* pData = pPacket->GetPacketBuffer();
	int i32Sent = 0;

	while (i32Sent < i32Size)
	{
		int result = send(m_Socket, pData + i32Sent, i32Size - i32Sent, 0);
		if (result == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
				continue;
			DisconnectFromRemote();
			return false;
		}
		i32Sent += result;
	}

	return true;
}

void ModuleBase::ProcessRingBuffers()
{
	// Procesar buffer del main thread
	if (m_MainBuffer.pRingBuffer)
	{
		while (m_MainBuffer.pRingBuffer->GetBufferCount() > 0)
		{
			void* pData = m_MainBuffer.pRingBuffer->Pop();
			if (pData)
			{
				i3NetworkPacket packet;
				memcpy(packet.GetPacketBuffer(), pData, MODULE_RING_BUFFER_SIZE);
				SendPacket(&packet);
				m_MainBuffer.pRingBuffer->PopIdx();
			}
		}
	}

	// Procesar buffers de worker threads
	for (int i = 0; i < m_i32WorkerThreadCount; ++i)
	{
		if (!m_pWorkerBuffers[i].pRingBuffer)
			continue;

		while (m_pWorkerBuffers[i].pRingBuffer->GetBufferCount() > 0)
		{
			void* pData = m_pWorkerBuffers[i].pRingBuffer->Pop();
			if (pData)
			{
				i3NetworkPacket packet;
				memcpy(packet.GetPacketBuffer(), pData, MODULE_RING_BUFFER_SIZE);
				SendPacket(&packet);
				m_pWorkerBuffers[i].pRingBuffer->PopIdx();
			}
		}
	}
}

void ModuleBase::ProcessReceivedData()
{
	if (!m_bConnected || m_Socket == INVALID_SOCKET)
		return;

	// Recibir datos disponibles
	int i32Space = (int)sizeof(m_RecvBuffer) - m_i32RecvSize;
	if (i32Space <= 0)
		return;

	int i32Received = recv(m_Socket, m_RecvBuffer + m_i32RecvSize, i32Space, 0);
	if (i32Received == 0)
	{
		// Conexion cerrada
		DisconnectFromRemote();
		return;
	}
	else if (i32Received == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		if (err != WSAEWOULDBLOCK)
			DisconnectFromRemote();
		return;
	}

	m_i32RecvSize += i32Received;

	// Parsear paquetes completos (header = 4 bytes: 2 size + 2 protocol)
	while (m_i32RecvSize >= SOCKET_HEAD_SIZE)
	{
		// Leer tamano del paquete del header
		UINT16 ui16DataSize = *(UINT16*)m_RecvBuffer;
		ui16DataSize &= 0x7FFF; // Quitar flag de encriptacion
		int i32PacketSize = ui16DataSize + SOCKET_HEAD_SIZE;

		if (m_i32RecvSize < i32PacketSize)
			break; // Paquete incompleto

		// Procesar el paquete completo
		OnProcessPacket(m_RecvBuffer, i32PacketSize);

		// Mover datos restantes al inicio
		if (m_i32RecvSize > i32PacketSize)
			memmove(m_RecvBuffer, m_RecvBuffer + i32PacketSize, m_i32RecvSize - i32PacketSize);
		m_i32RecvSize -= i32PacketSize;
	}
}
