#include "pch.h"
#include "ServerStatistics.h"

CServerStatistics	* g_pStatistics = NULL;

CServerStatistics::CServerStatistics()
{
	Reset();
}

CServerStatistics::~CServerStatistics(void)
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: IncrementReceivedBytes
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
void CServerStatistics::IncrementReceivedBytes(DWORD dwReadBytes) 
{ 
	DWORD dwTick = GetTickCount();

	// init
	if( m_dwStartTime == 0 ) m_dwStartTime = dwTick;

	// check max (개발용)
	if ( dwReadBytes > m_ui32MaxReceivedPacketSize ) m_ui32MaxReceivedPacketSize = dwReadBytes;
	
	// bytes
	m_i64ReceivedBytesPerHour += dwReadBytes;
	m_i32ReceivedBytesPerSec_back += dwReadBytes;
	
	// packets
	m_i64ReceivedPacketsPerHour++;
	m_i32ReceivedPacketsPerSec_back++;
	
	if( dwTick - m_dwLastUpdateReceiveTime > 1000)
	{
		// get bps
		m_i32ReceivedBytesPerSec = m_i32ReceivedBytesPerSec_back;
		m_i32ReceivedBytesPerSec_back = 0;
		// get pps
		m_i32SentPacketsPerSec = m_i32ReceivedPacketsPerSec_back;
		m_i32ReceivedPacketsPerSec_back = 0;
		m_dwLastUpdateReceiveTime = dwTick;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: IncrementSentBytes
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
void CServerStatistics::IncrementSentBytes(DWORD dwSentBytes)
{
	DWORD dwTick = GetTickCount();

	// init
	if( m_dwStartTime == 0 ) m_dwStartTime = dwTick;

	// check max (개발용)
	if ( (INT32)dwSentBytes > m_ui32MaxSentPacketSize ) m_ui32MaxSentPacketSize = (INT32)dwSentBytes;

	// bytes
	m_i64SentBytesPerHour += dwSentBytes;
	m_i32SentBytesPerSec_back += dwSentBytes;
	// packets
	m_i64SentPacketsPerHour++;
	m_i32SentPacketsPerSec_back++;
	
	if( dwTick - m_dwLastUpdateSentTime > 1000)
	{
		// get bps
		m_i32SentBytesPerSec = m_i32SentBytesPerSec_back;
		m_i32SentBytesPerSec_back = 0;
		// get pps
		m_i32SentPacketsPerSec = m_i32SentPacketsPerSec_back;
		m_i32SentPacketsPerSec_back = 0;
		m_dwLastUpdateSentTime = dwTick;
	}

}


void CServerStatistics::GetStaticsticsInfo( INT64* pSentBytesPerHour, 
											INT64* pReceivedBytesPerHour, 
											INT32* pAverageSentBytesPerSeconds,
											INT32* pAverageReceivedBytesPerSeconds,

											INT64* pSentPacketsPerHour, 
											INT64* pReceivedPacketsPerHour, 
											INT32* pAverageSentPacketsPerSeconds,
											INT32* pAverageReceivedPacketsPerSeconds,
											
											UINT32* pMaxSentPacketSize,
											UINT32* pMaxReceivedPacketSize,
											UINT64*	pui64PacketCountPerHour,
											REAL64* pr64PacketTimePerHour
											)
{
	UINT32	ui32Seconds = (GetTickCount() - m_dwStartTime ) / 1000; // ms -> sec
	if( ui32Seconds == 0 ) ui32Seconds = 1;
	
	*pSentBytesPerHour					= m_i64SentBytesPerHour; 
	*pReceivedBytesPerHour				= m_i64ReceivedBytesPerHour; 
	*pAverageSentBytesPerSeconds		= (INT32)( m_i64SentBytesPerHour / ui32Seconds );		// 계산
	*pAverageReceivedBytesPerSeconds	= (INT32)( m_i64ReceivedBytesPerHour / ui32Seconds );	// 계산

	*pSentPacketsPerHour				= m_i64SentPacketsPerHour; 
	*pReceivedPacketsPerHour			= m_i64ReceivedPacketsPerHour;
	*pAverageSentPacketsPerSeconds		= (INT32)( m_i64SentPacketsPerHour / ui32Seconds );// 계산
	*pAverageReceivedPacketsPerSeconds	= (INT32)( m_i64ReceivedPacketsPerHour / ui32Seconds );// 계산

	*pMaxSentPacketSize					= m_ui32MaxSentPacketSize;
	*pMaxReceivedPacketSize				= m_ui32MaxReceivedPacketSize;

	for (INT32 i = 0; i < DS_MAX_TASK_THREAD_COUNT; ++i)
	{
		*pui64PacketCountPerHour += m_ui64PacketCount[i];
		*pr64PacketTimePerHour += m_r64PacketTime[i];
	}
	
	// 모든 데이터 삭제
	Reset();
}

void CServerStatistics::Reset()
{
	m_cs.Lock();

	m_ui32MaxReceivedPacketSize		= 0;	// 개발용
	m_ui32MaxSentPacketSize			= 0;	// 개발용
	
	m_i64SentBytesPerHour			= 0;	// 패킷마다 update, log 찍고 리셋
	m_i64ReceivedBytesPerHour		= 0;

	m_i64SentPacketsPerHour			= 0;	// 패킷마다 update, log 찍고 리셋
	m_i64ReceivedPacketsPerHour		= 0;

	m_i32SentBytesPerSec_back		= 0;	// 패킷마다 update
	m_i32ReceivedBytesPerSec_back	= 0;	// 패킷마다 update
	m_i32SentBytesPerSec			= 0;	// 1초마다	update
	m_i32ReceivedBytesPerSec		= 0;	// 1초마다	update

	m_i32SentPacketsPerSec_back		= 0;	// 패킷마다 update
	m_i32ReceivedPacketsPerSec_back	= 0;	// 패킷마다 update
	m_i32SentPacketsPerSec			= 0;	// 1초마다	update
	m_i32ReceivedPacketsPerSec		= 0;	// 1초마다	update

	m_dwLastUpdateReceiveTime		= 0;
	m_dwLastUpdateSentTime			= 0;

	m_dwStartTime					= 0;
#ifdef _DEBUG	
	m_dwLastPrintTime				= 0;
#endif
	
	i3mem::FillZero(m_ui64PacketCount, sizeof(UINT64) * DS_MAX_TASK_THREAD_COUNT);
	i3mem::FillZero(m_r64PacketTime, sizeof(REAL64) * DS_MAX_TASK_THREAD_COUNT);
	
	m_cs.Unlock();
}


void CServerStatistics::PrintStatistics()
{
#ifdef _DEBUG
	char strMessage[1024] = { '\0' };
	
	DWORD dwTime = GetTickCount();
	UINT32 ui32MsgLen = 0;

	if( dwTime - m_dwLastPrintTime > 1000)
	{
		INT64 i64SentBytesPerHour					= 0; 
		INT64 i64ReceivedBytesPerHour				= 0; 
		INT32 i32AverageSentBytesPerSeconds			= 0;
		INT32 i32AverageReceivedBytesPerSeconds		= 0;

		INT64 i64SentPacketsPerHour					= 0; 
		INT64 i64ReceivedPacketsPerHour				= 0; 
		INT32 i32AverageSentPacketsPerSeconds		= 0;
		INT32 i32AverageReceivedPacketsPerSeconds	= 0; 

		UINT32 ui32MaxSentPacketSize				= 0;
		UINT32 ui32MaxReceivedPacketSize			= 0;

		UINT64 ui64PacketCountPerHour				= 0;
		REAL64 r64PacketTimePerHour					= 0.f;

		g_pStatistics->GetStaticsticsInfo(	&i64SentBytesPerHour, 
											&i64ReceivedBytesPerHour, 
											&i32AverageSentBytesPerSeconds,
											&i32AverageReceivedBytesPerSeconds,

											&i64SentPacketsPerHour, 
											&i64ReceivedPacketsPerHour, 
											&i32AverageSentPacketsPerSeconds,
											&i32AverageReceivedPacketsPerSeconds,
											
											&ui32MaxSentPacketSize,
											&ui32MaxReceivedPacketSize,
											&ui64PacketCountPerHour,
											&r64PacketTimePerHour
										);

		ui32MsgLen = sprintf_s( strMessage,  sizeof(strMessage), 
											"[STATISTICS]\r\n"
											"\t* [Bytes]\r\n"
											"\t* Sent Bytes Per Hour:\t %I64d\r\n"
											"\t* Received Bytes Per Hour:\t %I64d\r\n" 
											"\t* Average Sent Bytes Per Seconds:\t %I32d\r\n"
											"\t* Average Received Bytes Per Seconds:\t %I32d\r\n"
											"\t* [Packets] \r\n"
											"\t* Sent Packets Per Hour:\t %I64d\r\n" 
											"\t* Received Packets Per Hour:\t %I64d\r\n" 
											"\t* Average Sent Packets Per Seconds:\t %I32d\r\n"
											"\t* Average Received Packets Per Seconds:\t %I32d\r\n"
											"\t* [Max]\r\n"
											"\t* Max Sent Packet Size In An Hour:\t %I32d\r\n"
											"\t* Max Received Packet Size In An Hour:\t %I32d\r\n"
											"\t* Recv Packet Count In An Hour:\t %I64d\r\n"
											"\t* Recv Packet Process Time In An Hour(Sec):\t %.6lf\r\n"
											,
											i64SentBytesPerHour, 
											i64ReceivedBytesPerHour, 
											i32AverageSentBytesPerSeconds,
											i32AverageReceivedBytesPerSeconds,

											i64SentPacketsPerHour, 
											i64ReceivedPacketsPerHour, 
											i32AverageSentPacketsPerSeconds,
											i32AverageReceivedPacketsPerSeconds,
											
											ui32MaxSentPacketSize,
											ui32MaxReceivedPacketSize
										); 
		I3TRACE( strMessage );

		m_dwLastPrintTime = dwTime;
	}
#endif
}

void	CServerStatistics::AddPacketProcess(INT32 i32TaskThreadIdx, REAL64 r64PacketProcessTime)
{
	if (i32TaskThreadIdx < 0) return;
	if (i32TaskThreadIdx >= DS_MAX_TASK_THREAD_COUNT) return;

	++m_ui64PacketCount[i32TaskThreadIdx];
	m_r64PacketTime[i32TaskThreadIdx] += r64PacketProcessTime;
}
