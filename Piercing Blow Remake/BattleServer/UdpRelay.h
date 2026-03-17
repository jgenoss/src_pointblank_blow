#ifndef __UDPRELAY_H__
#define __UDPRELAY_H__

#pragma once
#include "BattleDef.h"

class BattleRoom;
class BattleRoomManager;

// ============================================================================
// UdpRelay - Production UDP packet receive and queue system
//
// Architecture (matches original DediUdpParser pattern):
// 1. Single UDP socket bound to base port
// 2. Receive loop runs from main thread (or dedicated recv thread)
// 3. Received packets are queued into PacketQueue (thread-safe)
// 4. TaskProcessor worker threads pop and process from PacketQueue
// 5. After processing, workers multicast results back via UDP socket
//
// The UdpRelay NO LONGER does inline processing or direct relay.
// It is purely a receiver that feeds PacketQueue.
// ============================================================================
class UdpRelay
{
public:
	UdpRelay();
	~UdpRelay();

	// Lifecycle
	bool			Initialize(uint16_t ui16BasePort);
	void			Update();
	void			Shutdown();

	// State
	bool			IsInitialized() const		{ return m_bInitialized; }
	uint16_t		GetPort() const				{ return m_ui16BasePort; }
	SOCKET			GetSocket() const			{ return m_Socket; }

	// Statistics
	uint64_t		GetTotalReceived() const		{ return m_ui64TotalReceived; }
	uint64_t		GetTotalQueued() const		{ return m_ui64TotalQueued; }
	uint64_t		GetTotalDropped() const		{ return m_ui64TotalDropped; }

	// UDP packet type IDs (from CommonDediCli.h)
	enum UdpGamePacketType
	{
		UDP_PKT_CYCLEINFO		= 0x0001,
		UDP_PKT_FIRE			= 0x0002,
		UDP_PKT_HIT				= 0x0003,
		UDP_PKT_DEATH			= 0x0004,
		UDP_PKT_RESPAWN			= 0x0005,
		UDP_PKT_ACTIONKEY		= 0x0006,
		UDP_PKT_WEAPON_CHANGE	= 0x0007,
	};

private:
	void			ProcessReceivedPacket(const char* pData, int i32Size,
										 uint32_t ui32SenderIP, uint16_t ui16SenderPort);

	// Direct relay for immediate forwarding (used alongside queue)
	void			RelayPacket(BattleRoom* pRoom, const char* pData, int i32Size,
							   uint32_t ui32SenderIP, uint16_t ui16SenderPort);

private:
	bool			m_bInitialized;
	SOCKET			m_Socket;
	uint16_t		m_ui16BasePort;

	// Per-call receive buffer (stack would be too large at 64KB)
	char*			m_pRecvBuffer;

	// Statistics (interlocked for thread safety)
	volatile uint64_t	m_ui64TotalReceived;
	volatile uint64_t	m_ui64TotalQueued;
	volatile uint64_t	m_ui64TotalDropped;
};

#endif // __UDPRELAY_H__
