#include "pch.h"
#include "UdpRelay.h"
#include "BattleRoom.h"
#include "BattleRoomManager.h"
#include "BattleMember.h"
#include "PacketQueue.h"
#include "UdpBufferPool.h"
#include "ServerStatistics.h"

UdpRelay::UdpRelay()
	: m_bInitialized(false)
	, m_Socket(INVALID_SOCKET)
	, m_ui16BasePort(0)
	, m_pRecvBuffer(nullptr)
	, m_ui64TotalReceived(0)
	, m_ui64TotalQueued(0)
	, m_ui64TotalDropped(0)
{
}

UdpRelay::~UdpRelay()
{
	Shutdown();
}

bool UdpRelay::Initialize(uint16_t ui16BasePort)
{
	if (m_bInitialized)
		return true;

	m_ui16BasePort = ui16BasePort;

	// Allocate receive buffer (heap, not stack - 64KB is too large for stack)
	m_pRecvBuffer = new char[BATTLE_UDP_RECV_BUFFER];
	if (!m_pRecvBuffer)
	{
		printf("[UdpRelay] ERROR: Failed to allocate receive buffer\n");
		return false;
	}

	// Create UDP socket
	m_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_Socket == INVALID_SOCKET)
	{
		printf("[UdpRelay] ERROR: socket() failed - Error=%d\n", WSAGetLastError());
		delete[] m_pRecvBuffer;
		m_pRecvBuffer = nullptr;
		return false;
	}

	// Set non-blocking
	u_long nonBlocking = 1;
	if (ioctlsocket(m_Socket, FIONBIO, &nonBlocking) == SOCKET_ERROR)
	{
		printf("[UdpRelay] ERROR: ioctlsocket(FIONBIO) failed - Error=%d\n", WSAGetLastError());
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
		delete[] m_pRecvBuffer;
		m_pRecvBuffer = nullptr;
		return false;
	}

	// Large socket buffers for high throughput
	int recvBufSize = 2 * 1024 * 1024;	// 2MB receive buffer
	setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (const char*)&recvBufSize, sizeof(recvBufSize));

	int sendBufSize = 2 * 1024 * 1024;	// 2MB send buffer
	setsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF, (const char*)&sendBufSize, sizeof(sendBufSize));

	// Bind
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(m_ui16BasePort);

	if (bind(m_Socket, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		printf("[UdpRelay] ERROR: bind() failed on port %d - Error=%d\n",
			m_ui16BasePort, WSAGetLastError());
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
		delete[] m_pRecvBuffer;
		m_pRecvBuffer = nullptr;
		return false;
	}

	m_bInitialized = true;

	printf("[UdpRelay] Initialized on UDP port %d (socket buffers: 2MB each)\n", m_ui16BasePort);
	return true;
}

void UdpRelay::Update()
{
	if (!m_bInitialized || m_Socket == INVALID_SOCKET)
		return;

	// Non-blocking receive loop - process ALL pending packets
	// No artificial limit (the old 100 limit was too restrictive for production)
	// Instead, we time-bound: process until WOULDBLOCK or 5ms elapsed
	DWORD dwStart = GetTickCount();

	for (;;)
	{
		struct sockaddr_in senderAddr;
		int addrLen = sizeof(senderAddr);

		int recvLen = recvfrom(m_Socket, m_pRecvBuffer, BATTLE_UDP_RECV_BUFFER, 0,
							  (struct sockaddr*)&senderAddr, &addrLen);

		if (recvLen == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
				break;	// No more data

			if (err != WSAECONNRESET)
				printf("[UdpRelay] recvfrom error: %d\n", err);
			break;
		}

		if (recvLen <= 0)
			break;

		InterlockedIncrement64((volatile LONG64*)&m_ui64TotalReceived);

		// Track received bytes in statistics
		if (g_pStatistics)
			g_pStatistics->IncrementReceivedBytes((uint32_t)recvLen);

		uint32_t senderIP = ntohl(senderAddr.sin_addr.s_addr);
		uint16_t senderPort = ntohs(senderAddr.sin_port);

		ProcessReceivedPacket(m_pRecvBuffer, recvLen, senderIP, senderPort);

		// Time bound: don't monopolize the main thread
		if (GetTickCount() - dwStart > 5)
			break;
	}
}

void UdpRelay::Shutdown()
{
	if (m_Socket != INVALID_SOCKET)
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}

	if (m_pRecvBuffer)
	{
		delete[] m_pRecvBuffer;
		m_pRecvBuffer = nullptr;
	}

	m_bInitialized = false;

	printf("[UdpRelay] Shutdown. Total: recv=%llu, queued=%llu, dropped=%llu\n",
		(unsigned long long)m_ui64TotalReceived,
		(unsigned long long)m_ui64TotalQueued,
		(unsigned long long)m_ui64TotalDropped);
}

void UdpRelay::ProcessReceivedPacket(const char* pData, int i32Size,
									 uint32_t ui32SenderIP, uint16_t ui16SenderPort)
{
	// Minimum size: UdpRelayHeader
	if (i32Size < (int)sizeof(UdpRelayHeader))
		return;

	const UdpRelayHeader* pHeader = (const UdpRelayHeader*)pData;

	// Validate room index
	if (!g_pBattleRoomManager)
		return;

	BattleRoom* pRoom = g_pBattleRoomManager->GetRoom(pHeader->ui16RoomIdx);
	if (!pRoom || !pRoom->IsActive())
		return;

	// Find or register the sender member
	BattleMember* pSender = pRoom->FindMemberByAddress(ui32SenderIP, ui16SenderPort);
	if (pSender)
		pSender->UpdateLastPacketTime();

	// Auto-start battle when first UDP packet arrives and room is in READY state
	if (pRoom->GetState() == BATTLE_ROOM_READY && pRoom->GetActiveMemberCount() > 0)
		pRoom->StartBattle();

	// Queue the game data (after UdpRelayHeader) into PacketQueue for worker thread processing
	if (g_pPacketQueue && i32Size > (int)sizeof(UdpRelayHeader))
	{
		PacketEntry entry;
		entry.ui16RoomIdx = pHeader->ui16RoomIdx;
		entry.ui16SlotIdx = pHeader->ui16SlotIdx;
		entry.ui32SenderIP = ui32SenderIP;
		entry.ui16SenderPort = ui16SenderPort;
		entry.dwRecvTime = GetTickCount();

		int32_t gameDataSize = i32Size - (int32_t)sizeof(UdpRelayHeader);
		if (gameDataSize > (int32_t)sizeof(entry.aData))
			gameDataSize = (int32_t)sizeof(entry.aData);

		memcpy(entry.aData, pData + sizeof(UdpRelayHeader), gameDataSize);
		entry.ui16DataSize = (uint16_t)gameDataSize;

		if (g_pPacketQueue->Push(&entry))
		{
			InterlockedIncrement64((volatile LONG64*)&m_ui64TotalQueued);
		}
		else
		{
			InterlockedIncrement64((volatile LONG64*)&m_ui64TotalDropped);
		}
	}

	// Direct relay: forward the raw packet to all other room members immediately
	// This ensures low-latency relay while the queued packet gets processed for validation
	// (matches original pattern: relay first, validate in worker thread)
	RelayPacket(pRoom, pData, i32Size, ui32SenderIP, ui16SenderPort);
}

void UdpRelay::RelayPacket(BattleRoom* pRoom, const char* pData, int i32Size,
						   uint32_t ui32SenderIP, uint16_t ui16SenderPort)
{
	if (!pRoom || m_Socket == INVALID_SOCKET)
		return;

	for (int i = 0; i < BATTLE_SLOT_MAX; i++)
	{
		BattleMember* pMember = pRoom->GetMember(i);
		if (!pMember || !pMember->IsMember())
			continue;

		// Don't relay back to sender
		if (pMember->IsSameAddress(ui32SenderIP, ui16SenderPort))
			continue;

		uint32_t destIP = pMember->GetIP();
		uint16_t destPort = pMember->GetPort();
		if (destIP == 0 || destPort == 0)
			continue;

		struct sockaddr_in destAddr;
		memset(&destAddr, 0, sizeof(destAddr));
		destAddr.sin_family = AF_INET;
		destAddr.sin_addr.s_addr = htonl(destIP);
		destAddr.sin_port = htons(destPort);

		int sent = sendto(m_Socket, pData, i32Size, 0,
			   (struct sockaddr*)&destAddr, sizeof(destAddr));

		if (sent > 0 && g_pStatistics)
			g_pStatistics->IncrementSentBytes((uint32_t)sent);
	}
}
