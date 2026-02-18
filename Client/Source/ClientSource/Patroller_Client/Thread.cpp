#include "stdafx.h"
#include "Thread.h"

static UINT32 _DefThreadProc( Thread *pThread)
{
	UINT32 Rv;
	Rv = pThread->OnRunning();

	return Rv;
}

Thread::Thread()
{

}

Thread::~Thread()
{
	CloseHandle(m_Handle);
}

bool Thread::CreateModule(void)
{
	m_Handle = ::CreateThread(  NULL,				// Security Attribute
		0,										// Stack Size
		(LPTHREAD_START_ROUTINE)_DefThreadProc,	// Thread Routine
		this,									// User-Defined Parameter
		0,										// Creation Flag
		NULL);

	if( m_Handle <= 0)
	{
		return false;
	}

	return true;
}