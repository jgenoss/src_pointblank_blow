// criticalSection.h
//
// YouJong Ha
//	Last update : 2011-05-27 (yyyy:mm:dd)
//	
// Description:
//	criticalSection wrapper class
//	- S2S2CriticalSection은 CRITICAL_SECTION 오브젝트 형태로 사용한다.
//	- CThreadSync는 블록 내에서 automatic 변수처럼 사용한다.
//
#ifndef _S2CriticalSection_H
#define _S2CriticalSection_H

//#define DEBUG_LOCK	1

class S2CriticalSection
{
	CRITICAL_SECTION m_cs;					// internal critical section
	volatile LONG	 m_lLockCount;

public:
	S2CriticalSection()
	{
		InitializeCriticalSection(&m_cs);	// setup
		m_lLockCount = 0;
	}
	~S2CriticalSection()
	{
		DeleteCriticalSection(&m_cs);		// cleanup
	}
	void Lock()
	{
		EnterCriticalSection(&m_cs);		// Enter/Leave 형태로 lock/unlock
#ifdef DEBUG_LOCK		
		++m_lLockCount;
		TRACE("Enter:Thread Id = %d, Lock Count = %d\n", GetCurrentThreadId(), m_lLockCount);
#endif
	}
	void Unlock()
	{
#ifdef DEBUG_LOCK
		--m_lLockCount;
		TRACE("Leave:Thread Id = %d, Lock Count = %d\n", GetCurrentThreadId(), m_lLockCount);
#endif
		LeaveCriticalSection(&m_cs);
	}
};

class CThreadSync
{
	S2CriticalSection c;
public:
	CThreadSync()
	{
		c.Lock();							// 생성과 동시에 lock
	}
	~CThreadSync()
	{
		c.Unlock();							// 삭제될 때 unlock
	}
};

#endif
