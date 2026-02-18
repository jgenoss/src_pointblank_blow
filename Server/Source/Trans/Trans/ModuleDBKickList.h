#ifndef __MODULE_DB_KICK_LIST_H__
#define __MODULE_DB_KICK_LIST_H__

#include "S2RingBuffer.h"

class i3ODBC; 
class CADODatabase; 

class CModuleDBKickList
{
	HANDLE					m_hThread;
	DWORD					m_dwThreadID;
	BOOL					m_bIsRunning;

	CADODatabase*			m_pADO;
	S2RingBuffer*			m_pRingIN;
	S2RingBuffer*			m_pRingOut;

	CDBConfig*				m_pLinkDBConfig;

public:
	CModuleDBKickList(void);
	~CModuleDBKickList(void);

	BOOL					Create();
	void					Destroy();
	void					Working(void);

	void					ErrorDBKickList( wchar_t * wstrSQL, BOOL bReconnect = FALSE );

	UINT32					GetBufferCount();
	DB_USER_KICK_IN*		Pop();
	INT32					Push( DB_USER_KICK_OUT *pDBUserKickOut);
	void					PopIdx();
};

#endif