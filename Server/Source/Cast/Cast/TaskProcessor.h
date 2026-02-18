#ifndef __TASK_PROCESSOR_H__
#define __TASK_PROCESSOR_H__

#include "UserManager.h"

class CTaskProcessor: public i3Thread
{
	I3_CLASS_DEFINE(CTaskProcessor);

	BOOL						m_bIsRunning;

	i3NetworkServerContext*		m_pServerContext;
	CUserSessionManager*		m_pSessionManager;
	
private:
	

	BOOL						_TaskProcessorPacket();
	BOOL						_SesseionPacketParsing( INT32 i32ServerIdx, i3NetworkPacket* pPacket );

public:
	CTaskProcessor();
	virtual ~CTaskProcessor();

	BOOL						Create( UINT32 ui32DataSize );

	virtual UINT32				OnRunning(void* pUserData);
};

extern CTaskProcessor* g_pTaskProcessor;

#endif //_TaskProcessor_h
