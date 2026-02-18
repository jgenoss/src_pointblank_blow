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
	void	GetStaticsticsInfo (INT64* pi64RequestsPerHour, 
								INT64* pi64ErrorsPerHour, 
								INT64* pi64WallShotsPerHour );
	void	PrintStaticsticsInfo() const;
	// Task Process에서 호출
	void	IncrementRequests()			{	::InterlockedIncrement64(&m_i64RequestsPerHour);	}
	void	IncrementErrors()			{	::InterlockedIncrement64(&m_i64ErrorsPerHour);		}
	void	IncrementWallShots()		{	::InterlockedIncrement64(&m_i64WallShotsPerHour);	}
	
	CServerStatistics( ); 
	~CServerStatistics(void);

private:
	// Reset
	void	Reset();
	
	// hours
	INT64	m_i64RequestsPerHour;			// 시간당 raycast 요청 회수 : physics에 요청한 회수
	INT64	m_i64ErrorsPerHour;				// 시간당 잘못된 패킷 전송 회수 : 로봇 테스트시
	INT64	m_i64WallShotsPerHour;			// 시간당 월샷 판정 개수 : 유저수

	S2CriticalSection m_cs;
};

extern CServerStatistics* g_pStatistics;



#endif