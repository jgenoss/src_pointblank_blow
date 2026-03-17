#ifndef __PACKETQUEUE_H__
#define __PACKETQUEUE_H__

#pragma once
#include "BattleDef.h"

// ============================================================================
// PacketQueue - Thread-safe packet queuing system
// Replaces original PacketLocker/TrackBuffer/DataArchive pattern
//
// Architecture:
// - Pre-allocated circular ring buffer (no malloc in hot path)
// - Producer: UdpRelay thread pushes received packets
// - Consumer: TaskProcessor thread(s) pop and process packets
// - Uses Windows CRITICAL_SECTION for synchronization (same as original)
// - Multi-track: one queue per room group for parallelism
//
// Original reference: CPacketLocker (multi-track), CTrackBuffer (ring buffer)
// ============================================================================

// Buffer entry - holds a single UDP packet with metadata
struct PacketEntry
{
	uint16_t	ui16RoomIdx;			// Target battle room
	uint16_t	ui16SlotIdx;			// Sender slot index
	uint32_t	ui32SenderIP;			// Sender IP (network order)
	uint16_t	ui16SenderPort;			// Sender port (host order)
	uint16_t	ui16DataSize;			// Actual data size
	uint32_t	ui32EncryptKey;			// Encryption key (if any)
	DWORD		dwRecvTime;				// Tick when received
	char		aData[2048];			// Packet data (max UDP payload)

	void Reset()
	{
		ui16RoomIdx = 0;
		ui16SlotIdx = 0;
		ui32SenderIP = 0;
		ui16SenderPort = 0;
		ui16DataSize = 0;
		ui32EncryptKey = 0;
		dwRecvTime = 0;
	}
};

// Single-track ring buffer with critical section
// One track per room group (original: CTrackBuffer)
class PacketTrack
{
public:
	PacketTrack();
	~PacketTrack();

	bool		Initialize(int32_t i32Capacity);
	void		Shutdown();

	// Push a packet into the ring (producer side, thread-safe)
	bool		Push(const PacketEntry* pEntry);

	// Pop a packet from the ring (consumer side, thread-safe)
	bool		Pop(PacketEntry* pOutEntry);

	// Get number of pending packets (approximate, for scheduling)
	int32_t		GetCount() const;

	// Is the buffer full?
	bool		IsFull() const;

private:
	PacketEntry*		m_pBuffer;			// Ring buffer
	int32_t				m_i32Capacity;		// Total slots
	volatile int32_t	m_i32Head;			// Write position (producer)
	volatile int32_t	m_i32Tail;			// Read position (consumer)
	volatile int32_t	m_i32Count;			// Current packet count

	CRITICAL_SECTION	m_cs;				// Thread synchronization
	bool				m_bInitialized;
};

// ============================================================================
// PacketQueue - Multi-track packet queue (replaces CPacketLocker)
// Maps room groups to tracks for parallel processing
// ============================================================================

#define PACKET_QUEUE_TRACK_COUNT	8		// Number of parallel tracks
#define PACKET_QUEUE_TRACK_CAPACITY	4096	// Packets per track

class PacketQueue
{
public:
	PacketQueue();
	~PacketQueue();

	bool		Initialize(int32_t i32TrackCount = PACKET_QUEUE_TRACK_COUNT,
						   int32_t i32TrackCapacity = PACKET_QUEUE_TRACK_CAPACITY);
	void		Shutdown();

	// Push a packet to the appropriate track (based on room index)
	bool		Push(const PacketEntry* pEntry);

	// Pop a packet from a specific track (used by task threads)
	bool		Pop(int32_t i32TrackIdx, PacketEntry* pOutEntry);

	// Get packet count for a track
	int32_t		GetTrackCount(int32_t i32TrackIdx) const;

	// Get total pending packets across all tracks
	int32_t		GetTotalCount() const;

	// Get number of tracks
	int32_t		GetTrackCount() const { return m_i32TrackCount; }

	// Statistics
	uint64_t	GetTotalPushed() const	{ return m_ui64TotalPushed; }
	uint64_t	GetTotalPopped() const	{ return m_ui64TotalPopped; }
	uint64_t	GetTotalDropped() const	{ return m_ui64TotalDropped; }

private:
	// Map room index to track index
	int32_t		RoomToTrack(uint16_t ui16RoomIdx) const;

	PacketTrack*		m_pTracks;
	int32_t				m_i32TrackCount;
	bool				m_bInitialized;

	// Global statistics (atomics for thread safety)
	volatile uint64_t	m_ui64TotalPushed;
	volatile uint64_t	m_ui64TotalPopped;
	volatile uint64_t	m_ui64TotalDropped;
};

// Global instance
extern PacketQueue* g_pPacketQueue;

#endif // __PACKETQUEUE_H__
