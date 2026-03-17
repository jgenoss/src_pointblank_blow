#include "pch.h"
#include "PacketQueue.h"

PacketQueue* g_pPacketQueue = nullptr;

// ============================================================================
// PacketTrack - Single ring buffer with CRITICAL_SECTION
// ============================================================================

PacketTrack::PacketTrack()
	: m_pBuffer(nullptr)
	, m_i32Capacity(0)
	, m_i32Head(0)
	, m_i32Tail(0)
	, m_i32Count(0)
	, m_bInitialized(false)
{
}

PacketTrack::~PacketTrack()
{
	Shutdown();
}

bool PacketTrack::Initialize(int32_t i32Capacity)
{
	if (m_bInitialized)
		return true;

	m_i32Capacity = i32Capacity;
	m_pBuffer = new PacketEntry[i32Capacity];
	if (!m_pBuffer)
		return false;

	for (int32_t i = 0; i < i32Capacity; i++)
		m_pBuffer[i].Reset();

	m_i32Head = 0;
	m_i32Tail = 0;
	m_i32Count = 0;

	InitializeCriticalSectionAndSpinCount(&m_cs, 4000);
	m_bInitialized = true;

	return true;
}

void PacketTrack::Shutdown()
{
	if (!m_bInitialized)
		return;

	DeleteCriticalSection(&m_cs);

	delete[] m_pBuffer;
	m_pBuffer = nullptr;
	m_i32Capacity = 0;
	m_i32Head = 0;
	m_i32Tail = 0;
	m_i32Count = 0;
	m_bInitialized = false;
}

bool PacketTrack::Push(const PacketEntry* pEntry)
{
	if (!m_bInitialized || !pEntry)
		return false;

	EnterCriticalSection(&m_cs);

	if (m_i32Count >= m_i32Capacity)
	{
		// Buffer full - drop oldest packet (overwrite tail)
		// This matches original behavior: under extreme load, old packets are lost
		// rather than blocking the producer
		m_i32Tail = (m_i32Tail + 1) % m_i32Capacity;
		m_i32Count--;
	}

	// Copy packet into ring buffer slot
	memcpy(&m_pBuffer[m_i32Head], pEntry, sizeof(PacketEntry));
	m_i32Head = (m_i32Head + 1) % m_i32Capacity;
	m_i32Count++;

	LeaveCriticalSection(&m_cs);
	return true;
}

bool PacketTrack::Pop(PacketEntry* pOutEntry)
{
	if (!m_bInitialized || !pOutEntry)
		return false;

	EnterCriticalSection(&m_cs);

	if (m_i32Count <= 0)
	{
		LeaveCriticalSection(&m_cs);
		return false;
	}

	// Copy packet out of ring buffer
	memcpy(pOutEntry, &m_pBuffer[m_i32Tail], sizeof(PacketEntry));
	m_pBuffer[m_i32Tail].Reset();
	m_i32Tail = (m_i32Tail + 1) % m_i32Capacity;
	m_i32Count--;

	LeaveCriticalSection(&m_cs);
	return true;
}

int32_t PacketTrack::GetCount() const
{
	return m_i32Count;
}

bool PacketTrack::IsFull() const
{
	return m_i32Count >= m_i32Capacity;
}

// ============================================================================
// PacketQueue - Multi-track dispatcher
// ============================================================================

PacketQueue::PacketQueue()
	: m_pTracks(nullptr)
	, m_i32TrackCount(0)
	, m_bInitialized(false)
	, m_ui64TotalPushed(0)
	, m_ui64TotalPopped(0)
	, m_ui64TotalDropped(0)
{
}

PacketQueue::~PacketQueue()
{
	Shutdown();
}

bool PacketQueue::Initialize(int32_t i32TrackCount, int32_t i32TrackCapacity)
{
	if (m_bInitialized)
		return true;

	m_i32TrackCount = i32TrackCount;
	m_pTracks = new PacketTrack[i32TrackCount];
	if (!m_pTracks)
		return false;

	for (int32_t i = 0; i < i32TrackCount; i++)
	{
		if (!m_pTracks[i].Initialize(i32TrackCapacity))
		{
			printf("[PacketQueue] ERROR: Failed to initialize track %d\n", i);
			Shutdown();
			return false;
		}
	}

	m_ui64TotalPushed = 0;
	m_ui64TotalPopped = 0;
	m_ui64TotalDropped = 0;
	m_bInitialized = true;

	printf("[PacketQueue] Initialized: %d tracks x %d capacity = %d total slots\n",
		i32TrackCount, i32TrackCapacity, i32TrackCount * i32TrackCapacity);

	return true;
}

void PacketQueue::Shutdown()
{
	if (!m_bInitialized)
		return;

	if (m_pTracks)
	{
		for (int32_t i = 0; i < m_i32TrackCount; i++)
			m_pTracks[i].Shutdown();

		delete[] m_pTracks;
		m_pTracks = nullptr;
	}

	m_i32TrackCount = 0;
	m_bInitialized = false;

	printf("[PacketQueue] Shutdown. Total pushed=%llu, popped=%llu, dropped=%llu\n",
		(unsigned long long)m_ui64TotalPushed,
		(unsigned long long)m_ui64TotalPopped,
		(unsigned long long)m_ui64TotalDropped);
}

bool PacketQueue::Push(const PacketEntry* pEntry)
{
	if (!m_bInitialized || !pEntry)
		return false;

	int32_t trackIdx = RoomToTrack(pEntry->ui16RoomIdx);

	if (m_pTracks[trackIdx].IsFull())
	{
		InterlockedIncrement64((volatile LONG64*)&m_ui64TotalDropped);
	}

	bool bResult = m_pTracks[trackIdx].Push(pEntry);
	if (bResult)
		InterlockedIncrement64((volatile LONG64*)&m_ui64TotalPushed);

	return bResult;
}

bool PacketQueue::Pop(int32_t i32TrackIdx, PacketEntry* pOutEntry)
{
	if (!m_bInitialized || !pOutEntry)
		return false;

	if (i32TrackIdx < 0 || i32TrackIdx >= m_i32TrackCount)
		return false;

	bool bResult = m_pTracks[i32TrackIdx].Pop(pOutEntry);
	if (bResult)
		InterlockedIncrement64((volatile LONG64*)&m_ui64TotalPopped);

	return bResult;
}

int32_t PacketQueue::GetTrackCount(int32_t i32TrackIdx) const
{
	if (!m_bInitialized || i32TrackIdx < 0 || i32TrackIdx >= m_i32TrackCount)
		return 0;

	return m_pTracks[i32TrackIdx].GetCount();
}

int32_t PacketQueue::GetTotalCount() const
{
	if (!m_bInitialized)
		return 0;

	int32_t total = 0;
	for (int32_t i = 0; i < m_i32TrackCount; i++)
		total += m_pTracks[i].GetCount();

	return total;
}

int32_t PacketQueue::RoomToTrack(uint16_t ui16RoomIdx) const
{
	// Distribute rooms across tracks using modulo
	// This ensures all packets for the same room go to the same track,
	// enabling single-threaded processing per room (no room-level lock needed)
	return (int32_t)(ui16RoomIdx % m_i32TrackCount);
}
