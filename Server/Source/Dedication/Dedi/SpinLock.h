// SpinLock.h
//
// YouJong Ha
//	Last update : 2010-10-06 (yyyy-mm-dd)
//	
// Description:
//	CRITICAL_SECTION 대신 SpinLock을 테스트 하기 위한 클래스
//
// Modified by Youjong Ha
// 2010-10-06 타임아웃 발생시 로그를 외부에서 기록하기 위해 리턴값 정의
// 2010-10-05 인터페이스의 일관성을 위해 Enter/Leave에 zero parameter 허용 
// 2010-09-27 타임아웃 검사 기능을 추가하였음
// original code : http://www.codeproject.com/KB/threads/spinlocks.aspx
// 
//
//	compile: 
//				
//		

#ifndef __SPINLOCK_H_
#define __SPINLOCK_H_

#define SPINLOCK_TIMEOUT	1000	// spinlock timeout value(ms)

// Enter() 함수의 리턴 값
#define LOCK_SUCCESS	1		// ownership 획득
#define LOCK_BLOCKED	1		// ...
#define LOCK_NONBLOCKED	2		// 이미 owner ship을 가진 경우 blocking 하지 않고 리턴 
#define LOCK_FAIL		0		// time out

////////////////////////////////////////////////////////////////////////////
// compiler directives
#pragma warning( disable : 4353 ) // 비표준 확장을 사용 경고 끔. (line:30-38)


#ifndef TRACE
#   define TRACE     1? (void)0 : (void)0
#endif  // TRACE

#ifndef ASSERT
#   define ASSERT    1? (void)0 : (void)0
#endif  // ASSERT

////////////////////////////////////////////////////////////////////////////
// scope the class inside a namespace
namespace i3base {

// THIS LOCK IS NOT-REENTRANT !! (CSpinLock의 멤버 변수로 사용)
class CPP_SpinLock
{
public:    // inlined constructor
    // inlined NON-virtual destructor
    inline CPP_SpinLock() { m_s = 1;}
    inline ~CPP_SpinLock() {}    
    
    // enter the lock, spinlocks (with/without Sleep)
    // when mutex is already locked
    inline int Enter(void)
    {
        int prev_s;
		DWORD	dwTickOld = GetTickCount();	// get start tickCount
		
		//volatile ULONGLONG ullTimeOut;				// get timeout value
		//::InterlockedExchange64((LONGLONG*)&ullTimeOut, m_ullTimeOut);
        do
        {
            prev_s = TestAndSet(&m_s, 0);			// try lock
            if (m_s == 0 && prev_s == 1)
            {
                break;								// lock success
            }

			// 현재 tick count와 timeout을 얻는다
			DWORD dwTick = GetTickCount();			// get current tick
						
			// 현재 tick count와 시작 tick count 비교
			if(dwTick > dwTickOld + SPINLOCK_TIMEOUT)
			{
				// test code beg
				TestAndSet(&m_s, 1); // unlock
				// test code end

				return LOCK_FAIL;
			}
#ifdef WIN32			
			SwitchToThread();	// winnt specific
#else
			Sleep(0);			// thread switching
#endif 
        }
        while (1);

		return LOCK_SUCCESS;
    }
	// Tries to enter the lock, returns 0
    // when mutex is already locked, 
    // returns != 0 when success
    inline int TryEnter(void)
    {
        int prev_s = TestAndSet(&m_s, 0);
        if (m_s == 0 && prev_s == 1)
        {
            return LOCK_SUCCESS;
        }
        return LOCK_FAIL;
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

    inline int Enter(unsigned int PNumber = 0)
    {
        if(!PNumber) PNumber = GetCurrentThreadId();
		
		if (PNumber == m_nOwner)
        {
            m_nLockCount++;
            return LOCK_NONBLOCKED;
        }
        
		if(m_RealCS.Enter() == LOCK_SUCCESS)
		{
			m_nOwner = PNumber;
			m_nLockCount++;
			return LOCK_SUCCESS;
		}
        
		return LOCK_FAIL;
    }

    inline int TryEnter(unsigned int PNumber = 0)
    {
		if(!PNumber) PNumber = GetCurrentThreadId();

        if (PNumber == m_nOwner) // we own it
        {
            m_nLockCount++;
            return LOCK_NONBLOCKED;
        }
        if (m_RealCS.TryEnter())
        {
            m_nOwner = PNumber;
            m_nLockCount++;
            return LOCK_SUCCESS;
        }
        return LOCK_FAIL;
    }

    inline void Leave(unsigned int PNumber = 0)
    {
        if(!PNumber) PNumber = GetCurrentThreadId();
		
		if(PNumber != m_nOwner) return;
        
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

}   // namespace i3base

#endif __SPINLOCK_H_
