#include "pch.h"
#include "UdpBufferPool.h"

UdpBufferPool* g_pUdpBufferPool = nullptr;

UdpBufferPool::UdpBufferPool()
	: m_pBuffers(nullptr)
	, m_i32TotalCount(0)
	, m_bInitialized(false)
	, m_ui64TotalAllocations(0)
	, m_i32CurrentUsed(0)
	, m_i32PeakUsed(0)
{
}

UdpBufferPool::~UdpBufferPool()
{
	Shutdown();
}

bool UdpBufferPool::Initialize(int32_t i32BufferCount)
{
	if (m_bInitialized)
		return true;

	if (i32BufferCount <= 0)
		return false;

	// Initialize the lock-free SLIST
	InitializeSListHead(&m_FreeList);

	// Allocate all buffers in a single contiguous block
	// Alignment: SLIST_ENTRY requires MEMORY_ALLOCATION_ALIGNMENT
	m_pBuffers = (UdpBuffer*)_aligned_malloc(sizeof(UdpBuffer) * i32BufferCount,
											  MEMORY_ALLOCATION_ALIGNMENT);
	if (!m_pBuffers)
	{
		printf("[UdpBufferPool] ERROR: Failed to allocate %d buffers (%zu bytes)\n",
			i32BufferCount, sizeof(UdpBuffer) * i32BufferCount);
		return false;
	}

	m_i32TotalCount = i32BufferCount;

	// Initialize all buffers and push them onto the free list
	for (int32_t i = 0; i < i32BufferCount; i++)
	{
		m_pBuffers[i].Reset();
		InterlockedPushEntrySList(&m_FreeList, &m_pBuffers[i].SListEntry);
	}

	m_ui64TotalAllocations = 0;
	m_i32CurrentUsed = 0;
	m_i32PeakUsed = 0;
	m_bInitialized = true;

	printf("[UdpBufferPool] Initialized: %d buffers x %zu bytes = %zu KB total\n",
		i32BufferCount, sizeof(UdpBuffer),
		(sizeof(UdpBuffer) * i32BufferCount) / 1024);

	return true;
}

void UdpBufferPool::Shutdown()
{
	if (!m_bInitialized)
		return;

	// Flush the SLIST
	InterlockedFlushSList(&m_FreeList);

	// Free the buffer block
	if (m_pBuffers)
	{
		_aligned_free(m_pBuffers);
		m_pBuffers = nullptr;
	}

	printf("[UdpBufferPool] Shutdown. Peak usage: %d/%d buffers, Total allocs: %llu\n",
		m_i32PeakUsed, m_i32TotalCount,
		(unsigned long long)m_ui64TotalAllocations);

	m_i32TotalCount = 0;
	m_bInitialized = false;
}

UdpBuffer* UdpBufferPool::GetBuffer()
{
	if (!m_bInitialized)
		return nullptr;

	// Lock-free pop from SLIST
	PSLIST_ENTRY pEntry = InterlockedPopEntrySList(&m_FreeList);
	if (!pEntry)
	{
		// Pool exhausted - this is a serious condition in production
		// The caller should handle nullptr (drop packet or expand pool)
		return nullptr;
	}

	UdpBuffer* pBuffer = CONTAINING_RECORD(pEntry, UdpBuffer, SListEntry);
	pBuffer->Reset();

	// Track usage stats
	InterlockedIncrement64(&m_ui64TotalAllocations);
	LONG used = InterlockedIncrement(&m_i32CurrentUsed);

	// Update peak (compare-and-swap loop)
	LONG peak = m_i32PeakUsed;
	while (used > peak)
	{
		LONG oldPeak = InterlockedCompareExchange(&m_i32PeakUsed, used, peak);
		if (oldPeak == peak)
			break;
		peak = oldPeak;
	}

	return pBuffer;
}

void UdpBufferPool::ReleaseBuffer(UdpBuffer* pBuffer)
{
	if (!m_bInitialized || !pBuffer)
		return;

	// Verify the buffer belongs to our pool
	ptrdiff_t offset = (char*)pBuffer - (char*)m_pBuffers;
	if (offset < 0 || offset >= (ptrdiff_t)(sizeof(UdpBuffer) * m_i32TotalCount))
	{
		printf("[UdpBufferPool] ERROR: Attempted to release buffer not from this pool!\n");
		return;
	}

	pBuffer->Reset();

	// Lock-free push back onto SLIST
	InterlockedPushEntrySList(&m_FreeList, &pBuffer->SListEntry);
	InterlockedDecrement(&m_i32CurrentUsed);
}

int32_t UdpBufferPool::GetFreeCount() const
{
	if (!m_bInitialized)
		return 0;

	return (int32_t)QueryDepthSList(&m_FreeList);
}
