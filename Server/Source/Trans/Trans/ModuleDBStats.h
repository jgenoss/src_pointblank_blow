#ifndef __MODULE_DB_STATS_H__
#define __MODULE_DB_STATS_H__

#include "S2RingBuffer.h"

class CSystemTimer;
class i3ODBC; 
class CADODatabase; 

class CModuleDBStats
{
	INT32					m_i32LogIdx;
	HANDLE					m_hThread;
	bool					m_bIsRunning;

	CADODatabase*			m_pADO;
	S2RingBuffer*			m_pRingPush;

	CDBConfig*				m_pZPLogDBConfig;

	CSystemTimer*			m_pSystemTimer;

protected:

	void					_SetDBCCU( UINT32 ui32Date, UINT32 ui32Value );

	void					ErrorDBStats( wchar_t * wstrSQL);
	void					_DataBaseResultFailLog( wchar_t * wstrSQL);

public:

	CModuleDBStats();
	virtual ~CModuleDBStats();

	bool					Create();
	void					Destroy();
	void					Working(void);

	INT32					Push( INT8 i8Type, UINT32 ui32Date, UINT64 ui64Value );
};

#endif