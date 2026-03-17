#include "pch.h"
#include "ServerStatistics.h"
#include <cstring>
#include <cstdio>

ServerStatistics* g_pStatistics = nullptr;

ServerStatistics::ServerStatistics()
{
	InitializeCriticalSectionAndSpinCount(&m_csTaskStats, 4000);
	Reset();
}

ServerStatistics::~ServerStatistics()
{
	DeleteCriticalSection(&m_csTaskStats);
}

void ServerStatistics::Reset()
{
	m_i32MaxReceivedPacketSize = 0;
	m_i32MaxSentPacketSize = 0;

	m_i64SentBytesPerHour = 0;
	m_i64ReceivedBytesPerHour = 0;
	m_i64SentPacketsPerHour = 0;
	m_i64ReceivedPacketsPerHour = 0;

	m_i32SentBytesPerSec_back = 0;
	m_i32ReceivedBytesPerSec_back = 0;
	m_i32SentBytesPerSec = 0;
	m_i32ReceivedBytesPerSec = 0;

	m_i32SentPacketsPerSec_back = 0;
	m_i32ReceivedPacketsPerSec_back = 0;
	m_i32SentPacketsPerSec = 0;
	m_i32ReceivedPacketsPerSec = 0;

	m_dwLastUpdateTime = 0;
	m_dwStartTime = 0;

	memset((void*)m_i64PacketCount, 0, sizeof(m_i64PacketCount));
	memset(m_r64PacketTime, 0, sizeof(m_r64PacketTime));
}

void ServerStatistics::IncrementReceivedBytes(uint32_t dwReceivedBytes)
{
	// All operations are interlocked - safe from any thread
	InterlockedAdd(&m_i32ReceivedBytesPerSec_back, (LONG)dwReceivedBytes);
	InterlockedAdd64(&m_i64ReceivedBytesPerHour, (LONG64)dwReceivedBytes);
	InterlockedIncrement64(&m_i64ReceivedPacketsPerHour);
	InterlockedIncrement(&m_i32ReceivedPacketsPerSec_back);

	// Thread-safe max update using compare-exchange loop
	LONG current = m_i32MaxReceivedPacketSize;
	while ((LONG)dwReceivedBytes > current)
	{
		LONG old = InterlockedCompareExchange(&m_i32MaxReceivedPacketSize,
											   (LONG)dwReceivedBytes, current);
		if (old == current)
			break;
		current = old;
	}
}

void ServerStatistics::IncrementSentBytes(uint32_t dwSentBytes)
{
	InterlockedAdd(&m_i32SentBytesPerSec_back, (LONG)dwSentBytes);
	InterlockedAdd64(&m_i64SentBytesPerHour, (LONG64)dwSentBytes);
	InterlockedIncrement64(&m_i64SentPacketsPerHour);
	InterlockedIncrement(&m_i32SentPacketsPerSec_back);

	LONG current = m_i32MaxSentPacketSize;
	while ((LONG)dwSentBytes > current)
	{
		LONG old = InterlockedCompareExchange(&m_i32MaxSentPacketSize,
											   (LONG)dwSentBytes, current);
		if (old == current)
			break;
		current = old;
	}
}

void ServerStatistics::AddPacketProcess(int32_t i32TaskThreadIdx, double r64PacketProcessTime)
{
	if (i32TaskThreadIdx < 0 || i32TaskThreadIdx >= DS_MAX_TASK_THREAD_COUNT)
		return;

	// Packet count is interlocked (LONG64 is atomically incrementable)
	InterlockedIncrement64(&m_i64PacketCount[i32TaskThreadIdx]);

	// Double accumulation needs critical section
	EnterCriticalSection(&m_csTaskStats);
	m_r64PacketTime[i32TaskThreadIdx] += r64PacketProcessTime;
	LeaveCriticalSection(&m_csTaskStats);
}

void ServerStatistics::Update(uint32_t dwNow)
{
	if (m_dwStartTime == 0)
		m_dwStartTime = dwNow;

	// Update once per second
	if (dwNow - m_dwLastUpdateTime < 1000)
		return;

	m_dwLastUpdateTime = dwNow;

	// Swap back buffers to front using interlocked exchange
	// This atomically reads and resets the back buffer
	m_i32SentBytesPerSec = InterlockedExchange(&m_i32SentBytesPerSec_back, 0);
	m_i32ReceivedBytesPerSec = InterlockedExchange(&m_i32ReceivedBytesPerSec_back, 0);
	m_i32SentPacketsPerSec = InterlockedExchange(&m_i32SentPacketsPerSec_back, 0);
	m_i32ReceivedPacketsPerSec = InterlockedExchange(&m_i32ReceivedPacketsPerSec_back, 0);
}

void ServerStatistics::ResetHourly()
{
	InterlockedExchange64(&m_i64SentBytesPerHour, 0);
	InterlockedExchange64(&m_i64ReceivedBytesPerHour, 0);
	InterlockedExchange64(&m_i64SentPacketsPerHour, 0);
	InterlockedExchange64(&m_i64ReceivedPacketsPerHour, 0);

	EnterCriticalSection(&m_csTaskStats);
	memset((void*)m_i64PacketCount, 0, sizeof(m_i64PacketCount));
	memset(m_r64PacketTime, 0, sizeof(m_r64PacketTime));
	LeaveCriticalSection(&m_csTaskStats);
}

void ServerStatistics::GetStatisticsInfo(int64_t* pSentBytesPerHour,
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
										  double* pr64PacketTimePerHour)
{
	if (pSentBytesPerHour)		*pSentBytesPerHour = m_i64SentBytesPerHour;
	if (pReceivedBytesPerHour)	*pReceivedBytesPerHour = m_i64ReceivedBytesPerHour;
	if (pAvgSentBPS)			*pAvgSentBPS = m_i32SentBytesPerSec;
	if (pAvgReceivedBPS)		*pAvgReceivedBPS = m_i32ReceivedBytesPerSec;
	if (pSentPacketsPerHour)	*pSentPacketsPerHour = m_i64SentPacketsPerHour;
	if (pReceivedPacketsPerHour)*pReceivedPacketsPerHour = m_i64ReceivedPacketsPerHour;
	if (pAvgSentPPS)			*pAvgSentPPS = m_i32SentPacketsPerSec;
	if (pAvgReceivedPPS)		*pAvgReceivedPPS = m_i32ReceivedPacketsPerSec;
	if (pMaxSentPacketSize)		*pMaxSentPacketSize = (uint32_t)m_i32MaxSentPacketSize;
	if (pMaxReceivedPacketSize)	*pMaxReceivedPacketSize = (uint32_t)m_i32MaxReceivedPacketSize;

	// Aggregate task thread stats
	if (pPacketCountPerHour)
	{
		uint64_t total = 0;
		for (int i = 0; i < DS_MAX_TASK_THREAD_COUNT; i++)
			total += (uint64_t)m_i64PacketCount[i];
		*pPacketCountPerHour = total;
	}
	if (pr64PacketTimePerHour)
	{
		EnterCriticalSection(&m_csTaskStats);
		double total = 0.0;
		for (int i = 0; i < DS_MAX_TASK_THREAD_COUNT; i++)
			total += m_r64PacketTime[i];
		*pr64PacketTimePerHour = total;
		LeaveCriticalSection(&m_csTaskStats);
	}
}

void ServerStatistics::PrintStatistics()
{
	printf("=== Server Statistics ===\n");
	printf("Send: %d B/s (%d pkt/s) | Recv: %d B/s (%d pkt/s)\n",
		   (int)m_i32SentBytesPerSec, (int)m_i32SentPacketsPerSec,
		   (int)m_i32ReceivedBytesPerSec, (int)m_i32ReceivedPacketsPerSec);
	printf("Max pkt: Send=%d Recv=%d\n",
		   (int)m_i32MaxSentPacketSize, (int)m_i32MaxReceivedPacketSize);
	printf("Hourly: Send=%lld B / %lld pkt | Recv=%lld B / %lld pkt\n",
		   (long long)m_i64SentBytesPerHour, (long long)m_i64SentPacketsPerHour,
		   (long long)m_i64ReceivedBytesPerHour, (long long)m_i64ReceivedPacketsPerHour);

	// Print per-thread stats
	EnterCriticalSection(&m_csTaskStats);
	for (int i = 0; i < DS_MAX_TASK_THREAD_COUNT; i++)
	{
		if (m_i64PacketCount[i] > 0)
		{
			double avgMs = (m_r64PacketTime[i] / (double)m_i64PacketCount[i]) * 1000.0;
			printf("Thread %d: %lld packets, avg %.3f ms/tick\n",
				   i, (long long)m_i64PacketCount[i], avgMs);
		}
	}
	LeaveCriticalSection(&m_csTaskStats);
}
