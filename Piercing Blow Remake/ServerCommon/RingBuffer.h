#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#pragma once

// Lock-free single-producer single-consumer ring buffer for inter-thread
// communication. Similar to NSM_CRingBuffer from the original codebase.
// Uses volatile read/write indices with MemoryBarrier for thread safety.
template<int CAPACITY = 4096>
class RingBuffer
{
public:
	RingBuffer()
		: m_i32ReadIdx(0)
		, m_i32WriteIdx(0)
	{
		memset(m_Buffer, 0, sizeof(m_Buffer));
	}

	// Write data into the buffer. Returns bytes written, 0 if not enough space.
	int Write(const char* pData, int i32Size)
	{
		if (i32Size <= 0 || !pData)
			return 0;

		// Need 4 bytes for length prefix + data
		int totalNeeded = (int)sizeof(int) + i32Size;

		int writeIdx = m_i32WriteIdx;
		int readIdx = m_i32ReadIdx;

		int available = GetFreeSpace(readIdx, writeIdx);
		if (available < totalNeeded)
			return 0;	// Not enough space

		// Write length prefix
		WriteBytes((const char*)&i32Size, sizeof(int), writeIdx);
		writeIdx = (writeIdx + sizeof(int)) % CAPACITY;

		// Write data
		WriteBytes(pData, i32Size, writeIdx);
		writeIdx = (writeIdx + i32Size) % CAPACITY;

		// Memory barrier before publishing write index
		MemoryBarrier();
		m_i32WriteIdx = writeIdx;

		return i32Size;
	}

	// Read one message from the buffer. Returns bytes read, 0 if empty.
	int Read(char* pOutBuffer, int i32MaxSize)
	{
		if (!pOutBuffer || i32MaxSize <= 0)
			return 0;

		int readIdx = m_i32ReadIdx;
		int writeIdx = m_i32WriteIdx;

		int used = GetUsedSpace(readIdx, writeIdx);
		if (used < (int)sizeof(int))
			return 0;	// Not enough data for even a length prefix

		// Read length prefix
		int msgSize = 0;
		ReadBytes((char*)&msgSize, sizeof(int), readIdx);

		if (msgSize <= 0 || msgSize > i32MaxSize)
		{
			// Corrupt or too large - skip this message
			readIdx = (readIdx + sizeof(int) + (msgSize > 0 ? msgSize : 0)) % CAPACITY;
			MemoryBarrier();
			m_i32ReadIdx = readIdx;
			return -1;
		}

		if (used < (int)sizeof(int) + msgSize)
			return 0;	// Incomplete message (shouldn't happen in SPSC)

		readIdx = (readIdx + sizeof(int)) % CAPACITY;

		// Read message data
		ReadBytes(pOutBuffer, msgSize, readIdx);
		readIdx = (readIdx + msgSize) % CAPACITY;

		// Memory barrier before publishing read index
		MemoryBarrier();
		m_i32ReadIdx = readIdx;

		return msgSize;
	}

	// Check if buffer has data
	bool HasData() const
	{
		return m_i32ReadIdx != m_i32WriteIdx;
	}

	// Get number of bytes available to read
	int GetUsedBytes() const
	{
		return GetUsedSpace(m_i32ReadIdx, m_i32WriteIdx);
	}

	// Get number of bytes free for writing
	int GetFreeBytes() const
	{
		return GetFreeSpace(m_i32ReadIdx, m_i32WriteIdx);
	}

	// Reset the buffer (NOT thread-safe, call only when both threads are idle)
	void Reset()
	{
		m_i32ReadIdx = 0;
		m_i32WriteIdx = 0;
	}

private:
	int GetUsedSpace(int readIdx, int writeIdx) const
	{
		if (writeIdx >= readIdx)
			return writeIdx - readIdx;
		return CAPACITY - readIdx + writeIdx;
	}

	int GetFreeSpace(int readIdx, int writeIdx) const
	{
		return CAPACITY - 1 - GetUsedSpace(readIdx, writeIdx);
	}

	void WriteBytes(const char* pSrc, int count, int startIdx)
	{
		for (int i = 0; i < count; i++)
			m_Buffer[(startIdx + i) % CAPACITY] = pSrc[i];
	}

	void ReadBytes(char* pDst, int count, int startIdx) const
	{
		for (int i = 0; i < count; i++)
			pDst[i] = m_Buffer[(startIdx + i) % CAPACITY];
	}

private:
	char			m_Buffer[CAPACITY];
	volatile int	m_i32ReadIdx;
	volatile int	m_i32WriteIdx;
};

// Pre-defined sizes
typedef RingBuffer<4096>	RingBuffer4K;
typedef RingBuffer<16384>	RingBuffer16K;
typedef RingBuffer<65536>	RingBuffer64K;

#endif // __RINGBUFFER_H__
