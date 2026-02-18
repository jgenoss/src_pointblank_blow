#ifndef __I3_LOG_PIPE_BASE_H__
#define __I3_LOG_PIPE_BASE_H__

#include "i3ElementBase.h"
#include "i3Mutex.h"
#include "i3RingBuffer.h"
#include "i3Warning.h"


class I3_EXPORT_BASE i3LogPipe : public i3ElementBase
{
	I3_CLASS_DEFINE( i3LogPipe );

protected:
	//For Thread 
	BOOL					m_bRunning;
	HANDLE					m_hThread;

	wchar_t					m_wstrFileName[128];
	HANDLE					m_hPipe;

protected:	

	void					Connect();
	void					InitializeCriticalSection();
	void					DeleteCriticalSection();
	void					EnterCriticalSection();
	void					LeaveCriticalSection();
public: 	
	
	i3LogPipe();
	virtual ~i3LogPipe(void);

	BOOL					CreatePipe(wchar_t* wstrFileName, INT32 iThreadPri);
	void					Destroy(void);
	void					Working(void);
	void					WriteLog(wchar_t * wstrLogString);
};

#endif
