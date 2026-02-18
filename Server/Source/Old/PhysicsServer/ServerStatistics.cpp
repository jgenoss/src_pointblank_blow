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

void CServerStatistics::GetStaticsticsInfo( INT64* pi64RequestsPerHour, 
											INT64* pi64ErrorsPerHour, 
											INT64* pi64WallShotsPerHour
											)
{
	I3ASSERT( pi64RequestsPerHour	!= NULL ); 
	I3ASSERT( pi64ErrorsPerHour		!= NULL ); 
	I3ASSERT( pi64WallShotsPerHour	!= NULL );
 
	*pi64RequestsPerHour	= m_i64RequestsPerHour;
	*pi64ErrorsPerHour		= m_i64ErrorsPerHour;		
	*pi64WallShotsPerHour	= m_i64WallShotsPerHour;
	
	// 모든 데이터 삭제
	Reset();
}

void CServerStatistics::Reset()
{
	m_cs.Lock();
	
	m_i64RequestsPerHour	= 0;		// 시간당 raycast 요청 회수
	m_i64ErrorsPerHour		= 0;		// 시간당 잘못된 패킷 전송 회수(로봇)
	m_i64WallShotsPerHour	= 0;		// 월샷 판정 개수

	m_cs.Unlock();
}

void CServerStatistics::PrintStaticsticsInfo() const
{
#ifdef _DEBUG
	char	szMessage[512] = { '\0', };
	sprintf_s( szMessage,  sizeof(szMessage), 
							"[STATISTICS]\n"
							"\t* Raycast Requests Per Hour:\t %I64d\n"
							"\t* Error Packets Per Hour:\t %I64d\n" 
							"\t* Wall Shot Users Per Hour:\t %I64d\n",
											m_i64RequestsPerHour, 
											m_i64ErrorsPerHour, 
											m_i64WallShotsPerHour); 
	I3TRACE( szMessage );
#endif
}
