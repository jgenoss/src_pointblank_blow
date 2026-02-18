// Modified by Youjong Ha
// date: 2010-09-27 (yyyy-mm-dd)
// original code : http://www.codeproject.com/KB/threads/spinlocks.aspx
// 타임아웃 검사 기능을 추가하였음
// SpinLock.h


#ifndef __SPINLOCK_H_
#define __SPINLOCK_H_

//#pragma intrinsic(_InterlockedXor)


#define SPINLOCK_TIMEOUT	1000	// spinlock timeout value(ms)

#ifndef TRACE
#   define TRACE    1? (void)0 : (void)0
#endif  // TRACE

#ifndef ASSERT
#   define ASSERT   1? (void)0 : (void)0
#endif  // ASSERT

// scope the class inside a namespace
// this is always good practice to avoid clashes
// with classes from external sources
namespace i3base {

// THIS LOCK IS NOT-REENTRANT !! (CSpinLock의 멤버 변수)
class CPP_SpinLock
{
public:    // inlined constructor
    // inlined NON-virtual destructor
    inline CPP_SpinLock() { m_s = 1; m_ullTimeOut = SPINLOCK_TIMEOUT;}
    inline ~CPP_SpinLock() {}    
    
    // enter the lock, spinlocks (with/without Sleep)
    // when mutex is already locked
    inline BOOL Enter(void)
    {
        int prev_s;
		ULONGLONG	ullTickOld = GetTickCount64();	// 시작 tick count를 얻음
		
		volatile ULONGLONG ullTimeOut;				// get timeout value
		::InterlockedExchange64((LONGLONG*)&ullTimeOut, m_ullTimeOut);
        do
        {
            prev_s = TestAndSet(&m_s, 0);
            if (m_s == 0 && prev_s == 1)
            {
                break;
            }

			// 현재 tick count와 timeout을 얻는다
			ULONGLONG ullTick = GetTickCount64();		// get current tik
						
			// 현재 tick count와 시작 tick count 비교
			if(ullTick > ullTickOld + ullTimeOut)
			{
				// output error message
				TRACE("0x%08x timeout error\n", this);
				return FALSE;
			}
            
			// reluinquish current timeslice (can only
            // be used when OS available and
            // we do NOT want to 'spin')
			// HWSleep(0);
			Sleep(0); // thread switching
        }
        while (1);

		return TRUE;
    }
    // Tries to enter the lock, returns 0
    // when mutex is already locked, 
    // returns != 0 when success
    inline int TryEnter(void)
    {
        int prev_s = TestAndSet(&m_s, 0);
        if (m_s == 0 && prev_s == 1)
        {
            return 1;
        }
        return 0;
    }
    // Leaves or unlocks the mutex
    // (should only be called by lock owner)
    inline void Leave(void)
    {
        TestAndSet(&m_s, 1);
    }
    
    protected:
    // sets BIT value and returns previous
    // value.in 1 atomic un-interruptable operation
    int TestAndSet(int* pTargetAddress, int nValue);
    
    private:
    int m_s;

	// 타임아웃을 검사하기 위한 변수
	mutable ULONGLONG	m_ullTimeOut;
};

// This part is Platform dependent!
#ifdef WIN32
inline int CPP_SpinLock::TestAndSet(int* pTargetAddress, 
                                              int nValue)
{
    __asm
    {
        mov edx, dword ptr [pTargetAddress]
        mov eax, nValue
        lock xchg eax, dword ptr [edx]
    }
    // mov = 1 CPU cycle
    // lock = 1 CPU cycle
    // xchg = 3 CPU cycles
}
#endif // WIN32


// THIS LOCK IS REENTRANT !!
// Thanks to George V. Reilly for pointing out
// the flaws in the first inefficient implementation.
// So here is the second try.
// NB PNumber is used as the Task, Thread or
// Process Number, on Windows you should
// pass GetCurrentThreadId() into it.
// For Embedded systems you pass your owned
// assigned task (or interrupt) numbers.
class CSpinLock
{
public:

    inline 
      CSpinLock() { m_nLockCount = 0; m_nOwner = (0xffffffff); }

    inline 
      ~CSpinLock() {};

    inline void Enter(unsigned int PNumber)
    {
        if (PNumber == m_nOwner)
        {
            m_nLockCount++;
            return;
        }
        m_RealCS.Enter();
        m_nOwner = PNumber;
        m_nLockCount++;
        return;
    }

    inline int TryEnter(unsigned int PNumber)
    {
        if (PNumber == m_nOwner) // we own it
        {
            m_nLockCount++;
            return 1;
        }
        if (m_RealCS.TryEnter())
        {
            m_nOwner = PNumber;
            m_nLockCount++;
            return 1;
        }
        return 0;
    }

    inline void Leave(unsigned int PNumber)
    {
        ASSERT(PNumber == m_nOwner);
        m_nLockCount--;
        if (m_nLockCount == 0)
        {
            m_nOwner = 0xffffffff;
            m_RealCS.Leave();
        }
    }

protected:

private:
    CPP_SpinLock m_RealCS;
    unsigned int m_nLockCount;
    unsigned int m_nOwner;
};

}   // namespace cdmh

#endif __SPINLOCK_H_
