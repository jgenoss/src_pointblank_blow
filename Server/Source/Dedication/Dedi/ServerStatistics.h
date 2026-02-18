// ServerStatistics.h
//
// YouJong Ha
//	Last update : 2010-10-08 (yyyy:mm:dd)
//	
// Description:
//	통계 정보를 출력하기 위한 클래스. Log 보다 먼저 선언해야 한다.
//
//
// Dependency: 
//		
//	

#ifndef _ServerStatistics_H_
#define _ServerStatistics_H_

#include "S2CriticalSection.h"

class CServerStatistics
{
public:
	// LOG에서 1시간마다 한번(파일 오픈시) 호출.(호출후 리셋됨)
	void	GetStaticsticsInfo( INT64* pSentBytesPerHour, 
								INT64* pReceivedBytesPerHour, 
								INT32* pAverageSentBytesPerSeconds,
								INT32* pAverageReceivedBytesPerSeconds,

								INT64* pSentPacketsPerHour, 
								INT64* pReceivedPacketsPerHour, 
								INT32* pAverageSentPacketsPerSeconds,
								INT32* pAverageReceivedPacketsPerSeconds,
								
								UINT32*	pMaxSentPacketSize,
								UINT32* pMaxReceivedPacketSize,
								UINT64*	pui64PacketCountPerHour,
								REAL64* pr64PacketTimePerHour);

	
	void	PrintStatistics();	// 디버그용
	
	INT32	GetMaxReceivedPacketSize()	{	return m_ui32MaxReceivedPacketSize;	};
	INT32	GetMaxSentPacketSize()		{	return m_ui32MaxSentPacketSize;		};
	INT32	GetCurrentReceiveBPS()		{	return m_i32ReceivedBytesPerSec;	};
	INT32	GetCurrentSentBPS()			{	return m_i32SentBytesPerSec;		};
	INT32	GetCurrentReceivePPS()		{	return m_i32ReceivedPacketsPerSec;	};
	INT32	GetCurrentSentPPS()			{	return m_i32SentPacketsPerSec;		};

	void	AddPacketProcess(INT32 i32TaskThreadIdx, REAL64 r64PacketProcessTime);
	// IOCP WORKER에서만 호출
	void	IncrementReceivedBytes(DWORD dwReceivedBytes); 
	void	IncrementSentBytes(DWORD dwSentBytes);
	
	CServerStatistics( ); 
	~CServerStatistics(void);

private:
	// Reset
	void	Reset();
	
	// max packet size
	UINT32	m_ui32MaxReceivedPacketSize;		// 개발용
	UINT32	m_ui32MaxSentPacketSize;			// 개발용
	
	// hours
	INT64	m_i64SentBytesPerHour;				// 패킷마다 update, log 찍고 리셋
	INT64	m_i64ReceivedBytesPerHour;

	INT64	m_i64SentPacketsPerHour;			// 패킷마다 update, log 찍고 리셋
	INT64	m_i64ReceivedPacketsPerHour;
	
	// seconds
	INT32	m_i32SentBytesPerSec_back;			// 패킷마다 update
	INT32	m_i32ReceivedBytesPerSec_back;		// 패킷마다 update
	INT32	m_i32SentBytesPerSec;				// 1초마다	update
	INT32	m_i32ReceivedBytesPerSec;			// 1초마다	update

	INT32	m_i32SentPacketsPerSec_back;		// 패킷마다 update
	INT32	m_i32ReceivedPacketsPerSec_back;	// 패킷마다 update
	INT32	m_i32SentPacketsPerSec;				// 1초마다	update
	INT32	m_i32ReceivedPacketsPerSec;			// 1초마다	update

	// BPS를 위한 시간
	DWORD	m_dwLastUpdateReceiveTime;
	DWORD	m_dwLastUpdateSentTime;

	DWORD	m_dwStartTime;
#ifdef _DEBUG	
	DWORD	m_dwLastPrintTime;					// debug
#endif

	// Task Thread용
	UINT64	m_ui64PacketCount[DS_MAX_TASK_THREAD_COUNT];	// Task에서 처리한 패킷 카운트 ( 시간당 )
	REAL64	m_r64PacketTime[DS_MAX_TASK_THREAD_COUNT];		// Task에서 처리한 패킷 처리 시간 ( 시간당 )

	S2CriticalSection m_cs;
};

extern CServerStatistics* g_pStatistics;



#endif