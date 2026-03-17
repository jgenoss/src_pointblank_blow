#ifndef __UDPBUFFERPOOL_H__
#define __UDPBUFFERPOOL_H__

#pragma once

// ============================================================================
// UdpBufferPool - Pre-allocated UDP buffer memory pool
// Replaces original CUdpBufferPool from Dedication/Dedi/UdpBufferPool.h
//
// Thread-safe buffer pool for zero-allocation UDP packet handling.
// All buffers are pre-allocated at startup. GetBuffer/ReleaseBuffer are
// lock-free using Windows Interlocked Singly-Linked List (SLIST), matching
// the original's use of InterlockedPushEntrySList/InterlockedPopEntrySList.
//
// Usage pattern:
//   UdpBuffer* pBuf = g_pUdpBufferPool->GetBuffer();
//   // ... fill pBuf->aData, set pBuf->ui16DataSize ...
//   g_pUdpBufferPool->ReleaseBuffer(pBuf);
// ============================================================================

// Maximum UDP payload we handle
#define UDP_BUFFER_MAX_SIZE		2048

// A single buffer from the pool
struct UdpBuffer
{
	SLIST_ENTRY		SListEntry;			// For lock-free linked list (must be first)
	char			aData[UDP_BUFFER_MAX_SIZE];
	uint16_t		ui16DataSize;		// Actual data length
	uint16_t		ui16RoomIdx;		// Target room
	uint16_t		ui16SlotIdx;		// Sender slot
	uint16_t		ui16Flags;			// Reserved
	uint32_t		ui32SenderIP;		// Sender address
	uint16_t		ui16SenderPort;
	uint16_t		ui16Pad;
	DWORD			dwRecvTime;			// GetTickCount() at receive
	uint32_t		ui32EncryptKey;		// Packet encryption key

	void Reset()
	{
		ui16DataSize = 0;
		ui16RoomIdx = 0;
		ui16SlotIdx = 0;
		ui16Flags = 0;
		ui32SenderIP = 0;
		ui16SenderPort = 0;
		ui16Pad = 0;
		dwRecvTime = 0;
		ui32EncryptKey = 0;
	}
};

class UdpBufferPool
{
public:
	UdpBufferPool();
	~UdpBufferPool();

	// Initialize with pre-allocated buffer count
	bool			Initialize(int32_t i32BufferCount);
	void			Shutdown();

	// Get a buffer from the pool (lock-free, O(1))
	// Returns nullptr if pool is exhausted
	UdpBuffer*		GetBuffer();

	// Return a buffer to the pool (lock-free, O(1))
	void			ReleaseBuffer(UdpBuffer* pBuffer);

	// Statistics
	int32_t			GetTotalCount() const		{ return m_i32TotalCount; }
	int32_t			GetFreeCount() const;
	int32_t			GetUsedCount() const		{ return m_i32TotalCount - GetFreeCount(); }
	uint64_t		GetTotalAllocations() const	{ return m_ui64TotalAllocations; }

	// High-water mark (peak usage)
	int32_t			GetPeakUsed() const			{ return m_i32PeakUsed; }

private:
	SLIST_HEADER		m_FreeList;			// Lock-free free list (Windows SLIST)
	UdpBuffer*			m_pBuffers;			// Pre-allocated buffer array
	int32_t				m_i32TotalCount;	// Total buffers
	bool				m_bInitialized;

	// Statistics (interlocked)
	volatile int64_t	m_ui64TotalAllocations;
	volatile int32_t	m_i32CurrentUsed;
	volatile int32_t	m_i32PeakUsed;
};

// Global instance
extern UdpBufferPool* g_pUdpBufferPool;

#endif // __UDPBUFFERPOOL_H__
