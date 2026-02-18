#ifndef _ServerStatistics_H_
#define _ServerStatistics_H_

enum STAT_BUFFER_IDX
{
	STAT_BUFFER_MAIN = 0,
	STAT_BUFFER_BACK,
	STAT_BUFFER_MAX,
};


enum STAT_TYPE
{
	STAT_TYPE_MIN = -1,
	STAT_TYPE_DB_LOGIN,
	STAT_TYPE_DB_LOGOUT,
	STAT_TYPE_DB_SHOP,
	STAT_TYPE_LINK_LOGIN,
	STAT_TYPE_LINK_BILLING,
	STAT_TYPE_LINK_PC_CAFE,
	STAT_TYPE_MAX,
};

#pragma pack( push, 1 )

struct STAT_THREADCOUNT
{
	INT32	m_i32Count[STAT_TYPE_MAX];
};

struct STAT_DATA
{
	UINT64	m_ui64Count;
	REAL64	m_r64Time;
};

#pragma pack( pop)

class CServerStatistics
{
public:
	
	void	GetStaticsticsInfo( void );
 
	

	void	AddStat(STAT_TYPE eStatType, INT32 i32ThreadIdx, REAL64 r64Time );
	void	GetAllStat( STAT_DATA* pAllStat  );

	CServerStatistics( ); 
	~CServerStatistics(void);

	BOOL	Create( STAT_THREADCOUNT *pstThreadCount);
	void	Reset();

private:
	// Reset
	
	
	STAT_DATA*	_GetptrStat(STAT_TYPE eStatType, INT32 i32Current = 1 );

	INT32	m_i32CurrentIdx;	// 0 : m_pStatMain. 1 : m_pStatBack

	STAT_DATA*	m_pStatMain[STAT_TYPE_MAX];
	STAT_DATA*	m_pStatBack[STAT_TYPE_MAX];
	STAT_THREADCOUNT	m_stThreadCount;

};

extern CServerStatistics* g_pStatistics;


#endif