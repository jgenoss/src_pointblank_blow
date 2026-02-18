#include "pch.h"
#include "ServerStatistics.h"

CServerStatistics	* g_pStatistics = NULL;

CServerStatistics::CServerStatistics()
{
	m_i32CurrentIdx	= STAT_BUFFER_MAIN;

	i3mem::FillZero( &m_stThreadCount, sizeof( STAT_THREADCOUNT ) ); 

	for( INT32 i = 0; i < STAT_TYPE_MAX ; ++i )
	{
		m_pStatMain[i] = nullptr;
		m_pStatBack[i] = nullptr;
	}
}

CServerStatistics::~CServerStatistics(void)
{
	for (INT32 i = 0; i < STAT_TYPE_MAX; i++)
	{
		I3MEM_SAFE_FREE(m_pStatMain[i]);
		I3MEM_SAFE_FREE(m_pStatBack[i]);
	}
}

BOOL CServerStatistics::Create( STAT_THREADCOUNT *pstThreadCount )
{	
	i3mem::Copy(  &m_stThreadCount, pstThreadCount, sizeof(STAT_THREADCOUNT) );

	for( INT32 i = 0; i < STAT_TYPE_MAX ; ++i )
	{
		INT32 i32ThreadCount = m_stThreadCount.m_i32Count[i];
		if( i32ThreadCount <= 0 ) continue;

		m_pStatMain[i]	= (STAT_DATA*)i3MemAlloc(sizeof(STAT_DATA) * i32ThreadCount);
		m_pStatBack[i]	= (STAT_DATA*)i3MemAlloc(sizeof(STAT_DATA) * i32ThreadCount);
	}

	return TRUE;
}

STAT_DATA*	CServerStatistics::_GetptrStat(STAT_TYPE eStatType, INT32 i32Current )
{
	if (eStatType <= STAT_TYPE_MIN) return nullptr;
	if (eStatType >= STAT_TYPE_MAX) return nullptr;

	switch( i32Current ^ m_i32CurrentIdx )  // m_i32CurrentIdx 종류가 2이상이면 수식을 다시 확인해야 합니다.
	{
		case 1:		return m_pStatMain[eStatType];
		case 0:		return m_pStatBack[eStatType];
	}

	return nullptr;
}

void	CServerStatistics::AddStat(STAT_TYPE eStatType, INT32 i32ThreadIdx, REAL64 r64Time )
{
	if (eStatType <= STAT_TYPE_MIN) return;
	if (eStatType >= STAT_TYPE_MAX) return;

	INT32 i32ThreadMax = m_stThreadCount.m_i32Count[eStatType];
	if( i32ThreadMax <= i32ThreadIdx ) return;
 
	STAT_DATA *pStat = _GetptrStat( eStatType );
	if( nullptr == pStat ) return;
	
	++pStat[i32ThreadIdx].m_ui64Count;
	pStat[i32ThreadIdx].m_r64Time += r64Time;

	//pStat[i32ThreadIdx]

}
 
void CServerStatistics::GetAllStat( STAT_DATA* pAllStat  )
{
	for( INT32 i = 0; i < STAT_TYPE_MAX ; ++i )
	{
		STAT_DATA *pStat = _GetptrStat( (STAT_TYPE)i );
		if( nullptr == pStat ) continue;

		INT32 i32ThreadCount = m_stThreadCount.m_i32Count[i];
		if( i32ThreadCount <= 0 ) continue;

		for( INT32 k = 0; k < i32ThreadCount ; ++k )
		{
			pAllStat[i].m_ui64Count		+= pStat[k].m_ui64Count;
			pAllStat[i].m_r64Time		+= pStat[k].m_r64Time;
		}
	}
}

void CServerStatistics::Reset()
{
	m_i32CurrentIdx++;
	if( m_i32CurrentIdx >= STAT_BUFFER_MAX ) m_i32CurrentIdx = STAT_BUFFER_MAIN;

	for( INT32 i = 0; i < STAT_TYPE_MAX ; ++i )
	{
		// 사용(Add)하지 않는 버퍼 초기화.
		STAT_DATA *pStat = _GetptrStat( (STAT_TYPE)i, 0 );
		if( nullptr == pStat ) continue;

		INT32 i32ThreadCount = m_stThreadCount.m_i32Count[i];
		if( i32ThreadCount <= 0 ) continue;

		i3mem::FillZero( pStat, sizeof(STAT_DATA) * i32ThreadCount ); 
	}
}
 