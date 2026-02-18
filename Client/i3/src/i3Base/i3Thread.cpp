#include "i3CommonType.h"
#include "i3Debug.h"
#include "i3Thread.h"
#include "i3System.h"
#include "i3Macro.h"


I3_CLASS_INSTANCE( i3Thread);

i3Thread * i3Thread::s_pThreadHead = nullptr;

#if defined( I3_WINDOWS)
static UINT32 _DefThreadProc( i3Thread *pThread)
{
	UINT32 Rv;

	pThread->BeforeRunning( pThread->GetParam());

	Rv = pThread->OnRunning( pThread->GetParam());
	pThread->m_ReturnCode = Rv;

	pThread->AfterRunning( pThread->GetParam());

	return Rv;
}

#elif defined( I3_PSP)

static INT32 _DefThreadProc( SceSize Sz, void * pUserData)
{
	i3Thread * pThread = ((I3_PSP_THREAD_USERDATA *)pUserData)->p;
	INT32 Rv;

	pThread->BeforeRunning( pThread->GetParam());

	Rv = pThread->OnRunning( pThread->GetParam());
	pThread->m_ReturnCode = Rv;

	pThread->AfterRunning( pThread->GetParam());

	return Rv;
}
#endif

i3Thread::~i3Thread(void)
{
	i3Thread * pPrev;

	//  생성된 모든 Thread는 내부적인 Single-Linked List로 연결되어 있다.
	// 종료 전에 Linked-List에서 자신을 제거한다.
	{
		pPrev = s_pThreadHead;
		while((pPrev != nullptr) && (pPrev->m_pNext != this))
			pPrev = pPrev->m_pNext;

		if( pPrev != nullptr)
			pPrev->m_pNext = m_pNext;

		if( s_pThreadHead == this)
			s_pThreadHead = m_pNext;
	}

	if( m_State != THREAD_STOPPED) 
		Terminate();
}

bool i3Thread::Create( char * pszName, UINT32 Attr, UINT32 StackSize, void * pUserData, INT32 priority)
{
	// Name
//		i3::string_ncopy_nullpad( m_szName, pszName, sizeof(m_szName) - 1);
//		m_szName[ sizeof(m_szName) - 1] = 0;

#if defined( I3_DEBUG)
	if( pszName != nullptr)
		m_name = pszName;
#endif

	m_Attribute = Attr;
	m_StackSize = StackSize;
	m_Parameter = pUserData;
	m_Priority = priority;

	m_State = THREAD_SUSPENDED;

	#if defined( I3_WINDOWS)
		m_Handle = CreateThread(NULL,									// Security Attribute
								m_StackSize,							// Stack Size
								(LPTHREAD_START_ROUTINE)_DefThreadProc,	// Thread Routine
								this,									// User-Defined Parameter
								0,										// Creation Flag
								(DWORD *)&m_ThreadID);

		#if defined( I3_DEBUG)
			if( m_Handle == nullptr) {
				i3Log( "i3Thread::Create()", "Thread를 실행할 수 없습니다");
				return false;
			}
		#endif
	#elif defined( I3_PSP)
		m_Handle = sceKernelCreateThread( "Thread", 
								(int (*)(SceSize, void *)) _DefThreadProc,
								SCE_KERNEL_MODULE_INIT_PRIORITY,			// Initial Priority
								m_StackSize,
								m_Attribute,
								NULL);
		if( m_Handle <= 0)
		{
			i3Log( "i3Thread::Create()", "Thread를 생성할 수 없습니다.");
			return false;
		}
		
		m_UserData.p = this;

		if( sceKernelStartThread( m_Handle, sizeof(I3_PSP_THREAD_USERDATA), &m_UserData) != SCE_KERNEL_ERROR_OK)
		{
			i3Log( "i3Thread::Create()", "sceKernelStartThread() failed.");
			return false;
		}
	#endif

	SetPriority( m_Priority);

	return true;
}

bool i3Thread::Terminate( bool bWait)
{
#if defined( I3_WINDOWS)
	if( ::TerminateThread( m_Handle, m_ReturnCode) == FALSE) {
		return false;
	}

	if( bWait)
	{
		WaitForSingleObject( m_Handle, INFINITE);
	}
#elif defined( I3_PSP)
	if( sceKernelTerminateDeleteThread( m_Handle) != SCE_KERNEL_ERROR_OK)
	{
		return false;
	}
#endif

	return true;
}

bool i3Thread::Suspend(void)
{
#if defined( I3_WINDOWS)
	UINT32 Rv;

	Rv = SuspendThread( m_Handle);
	#if defined( I3_DEBUG)
	if( Rv == 0xFFFFFFFF) {
		i3Log( "i3Thread::Suspend()", "i3Thread::Suspend() failed.");
		return false;
	}
	#endif
#elif defined( I3_PSP)
	if( sceKernelSuspendThread( m_Handle) != SCE_KERNEL_ERROR_OK)
	{
		i3Log( "i3Thread::Suspend()", "i3Thread::Suspend() failed.");
		return false;
	}
#endif

	return true;
}

bool i3Thread::Resume(void)
{
#ifdef WIN32
	UINT32 Rv;

	Rv = ResumeThread( m_Handle);
	#if defined( I3_DEBUG)
	if( Rv == 0xFFFFFFFF) {
		i3Log( "i3Thread::Resume()", "i3Thread::Resume() failed.");
		return false;
	}
	#endif
#elif defined( I3_PSP)
	if( sceKernelResumeThread( m_Handle) != SCE_KERNEL_ERROR_OK)
	{
		i3Log( "i3Thread::Resume()", "i3Thread::Resume() failed.");
		return false;
	}

#endif

	return true;
}

UINT32 i3Thread::WaitForFinish(void)
{
#ifdef WIN32
	return ::WaitForSingleObject( m_Handle, INFINITE );
#else
	return 0;
#endif
}

bool i3Thread::SetPriority(INT32 priority)
{
	m_Priority = priority;

#if defined( WIN32)
	switch( priority)
	{
		case PRIORITY_HIGHEST :
			priority = THREAD_PRIORITY_HIGHEST;
			break;

		case PRIORITY_HIGH :
			priority = THREAD_PRIORITY_ABOVE_NORMAL;
			break;

		case PRIORITY_NORMAL :
			priority = THREAD_PRIORITY_NORMAL;
			break;

		case PRIORITY_LOW :
			priority = THREAD_PRIORITY_BELOW_NORMAL;
			break;

		case PRIORITY_LOWEST :
			priority = THREAD_PRIORITY_LOWEST;
			break;

		case PRIORITY_TIME_CRITICAL:
			priority = THREAD_PRIORITY_TIME_CRITICAL;
			break;
		
	}

	return ::SetThreadPriority( m_Handle, priority) == TRUE;
#elif defined( I3_PSP)
	switch( priority)
	{
		case PRIORITY_HIGHEST :	priority = SCE_KERNEL_USER_HIGHEST_PRIORITY;	break;	// 16
		case PRIORITY_HIGH :	priority = 24;									break;
		case PRIORITY_NORMAL :	priority = SCE_KERNEL_MODULE_INIT_PRIORITY;		break;	// 32
		case PRIORITY_LOW :		priority = 62;									break;
		case PRIORITY_LOWEST :	priority = SCE_KERNEL_USER_LOWEST_PRIORITY;		break;	// 111
	}

	if( sceKernelChangeThreadPriority( m_Handle, priority) != SCE_KERNEL_ERROR_OK)
	{
		i3Log( "i3Thread::SetPriority()", "sceKernelChangeThreadPriority() failed.");
		return false;
	}

	return true;
#elif defined( I3_WIPI)
	return false;
#endif
}

void i3Thread::BeforeRunning( void * /*pUserData*/)
{
	m_State = THREAD_RUNNING;
}

void i3Thread::AfterRunning( void * /*pUserData*/)
{
	m_State = THREAD_STOPPED;
}

