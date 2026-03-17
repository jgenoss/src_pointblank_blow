#ifndef __SERVERSTATISTICS_H__
#define __SERVERSTATISTICS_H__

#pragma once

// ============================================================================
// ServerStatistics - Bandwidth and packet tracking
// Port of original CServerStatistics from Dedication/Dedi/ServerStatistics.h
// Tracks bytes/packets per second and per hour, max packet sizes,
// and per-task-thread processing stats
// ============================================================================

#define DS_MAX_TASK_THREAD_COUNT	4

class ServerStatistics
{
public:
	ServerStatistics();
	~ServerStatistics();

	// Called from IOCP/network threads
	void IncrementReceivedBytes(uint32_t dwReceivedBytes);
	void IncrementSentBytes(uint32_t dwSentBytes);

	// Called from task threads
	void AddPacketProcess(int32_t i32TaskThreadIdx, double r64PacketProcessTime);

	// Statistics retrieval
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

	// Quick accessors
	int32_t GetMaxReceivedPacketSize() const	{ return m_ui32MaxReceivedPacketSize; }
	int32_t GetMaxSentPacketSize() const		{ return m_ui32MaxSentPacketSize; }
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

	// Max packet sizes
	uint32_t	m_ui32MaxReceivedPacketSize;
	uint32_t	m_ui32MaxSentPacketSize;

	// Hourly accumulators
	int64_t		m_i64SentBytesPerHour;
	int64_t		m_i64ReceivedBytesPerHour;
	int64_t		m_i64SentPacketsPerHour;
	int64_t		m_i64ReceivedPacketsPerHour;

	// Per-second counters (back = accumulator, front = displayed)
	int32_t		m_i32SentBytesPerSec_back;
	int32_t		m_i32ReceivedBytesPerSec_back;
	int32_t		m_i32SentBytesPerSec;
	int32_t		m_i32ReceivedBytesPerSec;

	int32_t		m_i32SentPacketsPerSec_back;
	int32_t		m_i32ReceivedPacketsPerSec_back;
	int32_t		m_i32SentPacketsPerSec;
	int32_t		m_i32ReceivedPacketsPerSec;

	// Timing
	uint32_t	m_dwLastUpdateTime;
	uint32_t	m_dwStartTime;

	// Task thread stats
	uint64_t	m_ui64PacketCount[DS_MAX_TASK_THREAD_COUNT];
	double		m_r64PacketTime[DS_MAX_TASK_THREAD_COUNT];
};

#endif // __SERVERSTATISTICS_H__
