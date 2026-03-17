#include "pch.h"
#include "UdpRelay.h"
#include "BattleRoom.h"
#include "BattleRoomManager.h"
#include "BattleMember.h"
#include "HitValidator.h"
#include "RespawnManager.h"
#include "MapData.h"

UdpRelay::UdpRelay()
	: m_bInitialized(false)
	, m_Socket(INVALID_SOCKET)
	, m_ui16BasePort(0)
{
	memset(m_RecvBuffer, 0, sizeof(m_RecvBuffer));
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

	// Create UDP socket
	m_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_Socket == INVALID_SOCKET)
	{
		printf("[UdpRelay] ERROR: socket() failed - Error=%d\n", WSAGetLastError());
		return false;
	}

	// Set non-blocking
	u_long nonBlocking = 1;
	if (ioctlsocket(m_Socket, FIONBIO, &nonBlocking) == SOCKET_ERROR)
	{
		printf("[UdpRelay] ERROR: ioctlsocket(FIONBIO) failed - Error=%d\n", WSAGetLastError());
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
		return false;
	}

	// Set receive buffer size
	int recvBufSize = 1024 * 1024;	// 1MB
	setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (const char*)&recvBufSize, sizeof(recvBufSize));

	// Set send buffer size
	int sendBufSize = 1024 * 1024;	// 1MB
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
		return false;
	}

	m_bInitialized = true;

	printf("[UdpRelay] Initialized on UDP port %d\n", m_ui16BasePort);
	return true;
}

void UdpRelay::Update()
{
	if (!m_bInitialized || m_Socket == INVALID_SOCKET)
		return;

	// Non-blocking receive loop - process all pending packets
	for (int i = 0; i < 100; i++)	// Max 100 packets per update to prevent stalling
	{
		struct sockaddr_in senderAddr;
		int addrLen = sizeof(senderAddr);

		int recvLen = recvfrom(m_Socket, m_RecvBuffer, sizeof(m_RecvBuffer), 0,
							  (struct sockaddr*)&senderAddr, &addrLen);

		if (recvLen == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
				break;	// No more data

			// Other errors - log but continue
			if (err != WSAECONNRESET)
				printf("[UdpRelay] recvfrom error: %d\n", err);
			break;
		}

		if (recvLen <= 0)
			break;

		uint32_t senderIP = ntohl(senderAddr.sin_addr.s_addr);
		uint16_t senderPort = ntohs(senderAddr.sin_port);

		ProcessReceivedPacket(m_RecvBuffer, recvLen, senderIP, senderPort);
	}
}

void UdpRelay::Shutdown()
{
	if (m_Socket != INVALID_SOCKET)
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}
	m_bInitialized = false;

	printf("[UdpRelay] Shutdown\n");
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
	{
		pSender->UpdateLastPacketTime();
	}

	// Auto-start battle when first UDP packet arrives and room is in READY state
	if (pRoom->GetState() == BATTLE_ROOM_READY && pRoom->GetActiveMemberCount() > 0)
	{
		pRoom->StartBattle();
	}

	// Parse game packet for state tracking (position, death, respawn, etc.)
	// Game data starts after the UdpRelayHeader
	if (pSender && i32Size > (int)sizeof(UdpRelayHeader) + 2)
	{
		const char* pGameData = pData + sizeof(UdpRelayHeader);
		int i32GameDataSize = i32Size - sizeof(UdpRelayHeader);
		ParseGamePacket(pRoom, pSender, pGameData, i32GameDataSize);
	}

	// Relay to other members (always relay, even if validation fails)
	RelayPacket(pRoom, pData, i32Size, ui32SenderIP, ui16SenderPort);
}

void UdpRelay::ParseGamePacket(BattleRoom* pRoom, BattleMember* pSender,
							   const char* pGameData, int i32GameDataSize)
{
	if (!pRoom || !pSender || i32GameDataSize < 2)
		return;

	// First 2 bytes = packet type ID
	uint16_t ui16PacketType = *(const uint16_t*)pGameData;

	switch (ui16PacketType)
	{
	case UDP_PKT_CYCLEINFO:
		{
			// Position update: [type(2)] [x(4)] [y(4)] [z(4)] [rx(4)] [ry(4)] [rz(4)]
			if (i32GameDataSize >= 2 + 12)	// At least type + 3 floats for position
			{
				const float* pPos = (const float*)(pGameData + 2);
				pSender->SetPosition(pPos[0], pPos[1], pPos[2]);

				if (i32GameDataSize >= 2 + 24)	// Also has rotation
				{
					pSender->SetRotation(pPos[3], pPos[4], pPos[5]);
				}

				// Validate movement if hit validator available
				HitValidator* pValidator = pRoom->GetHitValidator();
				if (pValidator)
				{
					float newPos[3] = { pPos[0], pPos[1], pPos[2] };
					pValidator->ValidateMovement(pSender, newPos, GetTickCount());
				}
			}
		}
		break;

	case UDP_PKT_DEATH:
		{
			// Death event: [type(2)] [killerSlot(1)] [headshot(1)]
			pSender->OnDeath();

			if (i32GameDataSize >= 4)
			{
				uint8_t killerSlot = (uint8_t)pGameData[2];
				uint8_t bHeadshot = (uint8_t)pGameData[3];

				BattleMember* pKiller = pRoom->GetMember(killerSlot);
				if (pKiller && pKiller->IsMember())
				{
					pKiller->AddKill();
					if (bHeadshot)
						pKiller->AddHeadshot();

					// Update team scores
					if (pKiller->GetTeam() == BATTLE_TEAM_RED)
						pRoom->AddRedScore();
					else
						pRoom->AddBlueScore();
				}
			}
		}
		break;

	case UDP_PKT_RESPAWN:
		{
			// Respawn event: [type(2)]
			// Get spawn point from map data
			float spawnPos[3] = { 0.0f, 0.0f, 0.0f };
			CMapData* pMapData = pRoom->GetMapData();
			if (pMapData)
			{
				RespawnManager respawnMgr;
				respawnMgr.GetSpawnPoint(pMapData, pSender->GetTeam(), spawnPos);
			}
			pSender->OnRespawn(spawnPos);
		}
		break;

	case UDP_PKT_WEAPON_CHANGE:
		{
			// Weapon change: [type(2)] [weaponID(4)]
			if (i32GameDataSize >= 6)
			{
				uint32_t weaponID = *(const uint32_t*)(pGameData + 2);
				pSender->SetWeaponID(weaponID);
			}
		}
		break;

	case UDP_PKT_FIRE:
	case UDP_PKT_HIT:
	case UDP_PKT_ACTIONKEY:
		// These are tracked but not validated in this version
		break;

	default:
		// Unknown packet type - just relay
		break;
	}
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

		// Send to this member
		struct sockaddr_in destAddr;
		memset(&destAddr, 0, sizeof(destAddr));
		destAddr.sin_family = AF_INET;
		destAddr.sin_addr.s_addr = htonl(pMember->GetIP());
		destAddr.sin_port = htons(pMember->GetPort());

		sendto(m_Socket, pData, i32Size, 0,
			   (struct sockaddr*)&destAddr, sizeof(destAddr));
	}
}
