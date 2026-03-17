#include "pch.h"
#include "ServerStatistics.h"
#include <cstring>
#include <cstdio>

ServerStatistics::ServerStatistics()
{
	Reset();
}

ServerStatistics::~ServerStatistics()
{
}

void ServerStatistics::Reset()
{
	m_ui32MaxReceivedPacketSize = 0;
	m_ui32MaxSentPacketSize = 0;

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

	memset(m_ui64PacketCount, 0, sizeof(m_ui64PacketCount));
	memset(m_r64PacketTime, 0, sizeof(m_r64PacketTime));
}

void ServerStatistics::IncrementReceivedBytes(uint32_t dwReceivedBytes)
{
	m_i32ReceivedBytesPerSec_back += (int32_t)dwReceivedBytes;
	m_i64ReceivedBytesPerHour += (int64_t)dwReceivedBytes;
	m_i64ReceivedPacketsPerHour++;
	m_i32ReceivedPacketsPerSec_back++;

	if (dwReceivedBytes > m_ui32MaxReceivedPacketSize)
		m_ui32MaxReceivedPacketSize = dwReceivedBytes;
}

void ServerStatistics::IncrementSentBytes(uint32_t dwSentBytes)
{
	m_i32SentBytesPerSec_back += (int32_t)dwSentBytes;
	m_i64SentBytesPerHour += (int64_t)dwSentBytes;
	m_i64SentPacketsPerHour++;
	m_i32SentPacketsPerSec_back++;

	if (dwSentBytes > m_ui32MaxSentPacketSize)
		m_ui32MaxSentPacketSize = dwSentBytes;
}

void ServerStatistics::AddPacketProcess(int32_t i32TaskThreadIdx, double r64PacketProcessTime)
{
	if (i32TaskThreadIdx < 0 || i32TaskThreadIdx >= DS_MAX_TASK_THREAD_COUNT)
		return;

	m_ui64PacketCount[i32TaskThreadIdx]++;
	m_r64PacketTime[i32TaskThreadIdx] += r64PacketProcessTime;
}

void ServerStatistics::Update(uint32_t dwNow)
{
	if (m_dwStartTime == 0)
		m_dwStartTime = dwNow;

	// Update once per second
	if (dwNow - m_dwLastUpdateTime < 1000)
		return;

	m_dwLastUpdateTime = dwNow;

	// Swap back buffers to front
	m_i32SentBytesPerSec = m_i32SentBytesPerSec_back;
	m_i32ReceivedBytesPerSec = m_i32ReceivedBytesPerSec_back;
	m_i32SentPacketsPerSec = m_i32SentPacketsPerSec_back;
	m_i32ReceivedPacketsPerSec = m_i32ReceivedPacketsPerSec_back;

	// Reset back buffers
	m_i32SentBytesPerSec_back = 0;
	m_i32ReceivedBytesPerSec_back = 0;
	m_i32SentPacketsPerSec_back = 0;
	m_i32ReceivedPacketsPerSec_back = 0;
}

void ServerStatistics::ResetHourly()
{
	m_i64SentBytesPerHour = 0;
	m_i64ReceivedBytesPerHour = 0;
	m_i64SentPacketsPerHour = 0;
	m_i64ReceivedPacketsPerHour = 0;

	memset(m_ui64PacketCount, 0, sizeof(m_ui64PacketCount));
	memset(m_r64PacketTime, 0, sizeof(m_r64PacketTime));
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
	if (pMaxSentPacketSize)		*pMaxSentPacketSize = m_ui32MaxSentPacketSize;
	if (pMaxReceivedPacketSize)	*pMaxReceivedPacketSize = m_ui32MaxReceivedPacketSize;

	// Aggregate task thread stats
	if (pPacketCountPerHour)
	{
		uint64_t total = 0;
		for (int i = 0; i < DS_MAX_TASK_THREAD_COUNT; i++)
			total += m_ui64PacketCount[i];
		*pPacketCountPerHour = total;
	}
	if (pr64PacketTimePerHour)
	{
		double total = 0.0;
		for (int i = 0; i < DS_MAX_TASK_THREAD_COUNT; i++)
			total += m_r64PacketTime[i];
		*pr64PacketTimePerHour = total;
	}
}

void ServerStatistics::PrintStatistics()
{
	printf("=== Server Statistics ===\n");
	printf("Send: %d B/s (%d pkt/s) | Recv: %d B/s (%d pkt/s)\n",
		   m_i32SentBytesPerSec, m_i32SentPacketsPerSec,
		   m_i32ReceivedBytesPerSec, m_i32ReceivedPacketsPerSec);
	printf("Max pkt: Send=%u Recv=%u\n",
		   m_ui32MaxSentPacketSize, m_ui32MaxReceivedPacketSize);
	printf("Hourly: Send=%lld B / %lld pkt | Recv=%lld B / %lld pkt\n",
		   (long long)m_i64SentBytesPerHour, (long long)m_i64SentPacketsPerHour,
		   (long long)m_i64ReceivedBytesPerHour, (long long)m_i64ReceivedPacketsPerHour);
}
