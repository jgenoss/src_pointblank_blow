#ifndef __SERVERSTATISTICS_H__
#define __SERVERSTATISTICS_H__

#pragma once

// ============================================================================
// ServerStatistics - Thread-safe bandwidth and packet tracking
// Production port of original CServerStatistics from Dedication/Dedi/ServerStatistics.h
//
// All counters use Windows Interlocked operations for thread safety.
// Called from IOCP/network threads (Increment*) and task threads (AddPacketProcess).
// Back-buffer swap pattern for per-second counters: accumulate in _back,
// swap to front on Update() once per second from main thread.
// ============================================================================

#define DS_MAX_TASK_THREAD_COUNT	4

class ServerStatistics
{
public:
	ServerStatistics();
	~ServerStatistics();

	// Called from IOCP/network threads - MUST be thread-safe
	void IncrementReceivedBytes(uint32_t dwReceivedBytes);
	void IncrementSentBytes(uint32_t dwSentBytes);

	// Called from task threads - MUST be thread-safe
	void AddPacketProcess(int32_t i32TaskThreadIdx, double r64PacketProcessTime);

	// Statistics retrieval (called from main thread)
	void GetStatisticsInfo(int64_t* pSentBytesPerHour,
						   int64_t* pReceivedBytesPerHour,
						   int32_t* pAvgSentBPS,
						   int32_t* pAvgReceivedBPS,
						   int64_t* pSentPacketsPerHour,
						   int64_t* pReceivedPacketsPerHour,
						   int32_t* pAvgSentPPS,
						   int32_t* pAvgReceivedPPS,
						   uint32_t* pMaxSentPacketSize,
						   uint32_t* pMaxReceivedPacketSize,
						   uint64_t* pPacketCountPerHour,
						   double* pr64PacketTimePerHour);

	// Quick accessors (reads of aligned 32-bit values are atomic on x86)
	int32_t GetMaxReceivedPacketSize() const	{ return m_i32MaxReceivedPacketSize; }
	int32_t GetMaxSentPacketSize() const		{ return m_i32MaxSentPacketSize; }
	int32_t GetCurrentReceiveBPS() const		{ return m_i32ReceivedBytesPerSec; }
	int32_t GetCurrentSentBPS() const			{ return m_i32SentBytesPerSec; }
	int32_t GetCurrentReceivePPS() const		{ return m_i32ReceivedPacketsPerSec; }
	int32_t GetCurrentSentPPS() const			{ return m_i32SentPacketsPerSec; }

	// Periodic update (call once per second from main loop)
	void Update(uint32_t dwNow);

	// Reset hourly counters
	void ResetHourly();

	// Print to console (debug)
	void PrintStatistics();

private:
	void Reset();

	// Max packet sizes (interlocked compare-exchange for thread-safe max)
	volatile LONG	m_i32MaxReceivedPacketSize;
	volatile LONG	m_i32MaxSentPacketSize;

	// Hourly accumulators (interlocked increment)
	volatile LONG64	m_i64SentBytesPerHour;
	volatile LONG64	m_i64ReceivedBytesPerHour;
	volatile LONG64	m_i64SentPacketsPerHour;
	volatile LONG64	m_i64ReceivedPacketsPerHour;

	// Per-second counters: back = accumulator (interlocked), front = displayed (main thread only)
	volatile LONG	m_i32SentBytesPerSec_back;
	volatile LONG	m_i32ReceivedBytesPerSec_back;
	volatile LONG	m_i32SentBytesPerSec;
	volatile LONG	m_i32ReceivedBytesPerSec;

	volatile LONG	m_i32SentPacketsPerSec_back;
	volatile LONG	m_i32ReceivedPacketsPerSec_back;
	volatile LONG	m_i32SentPacketsPerSec;
	volatile LONG	m_i32ReceivedPacketsPerSec;

	// Timing
	uint32_t		m_dwLastUpdateTime;
	uint32_t		m_dwStartTime;

	// Task thread stats (each index accessed by only one thread, so no contention)
	volatile LONG64	m_i64PacketCount[DS_MAX_TASK_THREAD_COUNT];
	double			m_r64PacketTime[DS_MAX_TASK_THREAD_COUNT];
	CRITICAL_SECTION m_csTaskStats;		// Protects m_r64PacketTime (doubles aren't atomic)
};

// Global instance
extern ServerStatistics* g_pStatistics;

#endif // __SERVERSTATISTICS_H__
