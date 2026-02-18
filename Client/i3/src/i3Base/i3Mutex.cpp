#include "i3CommonType.h"
#include "i3Mutex.h"
#include "i3Macro.h"

I3_CLASS_INSTANCE(i3Mutex);

i3Mutex::i3Mutex()
{
#if defined( I3_WINDOWS ) 
	::InitializeCriticalSection( &m_Mutex );
#else

#endif	
}
i3Mutex::~i3Mutex()
{
#if defined( I3_WINDOWS ) 
	::DeleteCriticalSection( &m_Mutex );
#else

#endif	
}

void i3Mutex::Enter(void)
{
#if defined( I3_WINDOWS ) 
	::EnterCriticalSection( &m_Mutex );
#else

#endif
}

bool i3Mutex::TryEnter(void)
{
#if defined( I3_WINDOWS ) 
	return ::TryEnterCriticalSection( &m_Mutex ) == TRUE;
#else
	return false;
#endif
}
void i3Mutex::Leave(void)
{
#if defined( I3_WINDOWS ) 
	::LeaveCriticalSection( &m_Mutex );
#else 

#endif	
}

/////////////////////////////////////////////////////////////////////////
//
i3Mutex2::i3Mutex2()
{
#if defined( I3_WINDOWS ) 
	::InitializeCriticalSection( &m_Mutex );
#else

#endif	
}
i3Mutex2::~i3Mutex2()
{
#if defined( I3_WINDOWS ) 
	::DeleteCriticalSection( &m_Mutex );
#else

#endif	
}

void i3Mutex2::Enter(void)
{
#if defined( I3_WINDOWS ) 
	::EnterCriticalSection( &m_Mutex );
#else

#endif
}

bool i3Mutex2::TryEnter(void)
{	
#if defined( I3_WINDOWS ) 
	return ::TryEnterCriticalSection( &m_Mutex ) == TRUE;
#else
	return false;
#endif
}
void i3Mutex2::Leave(void)
{
#if defined( I3_WINDOWS ) 
	::LeaveCriticalSection( &m_Mutex );
#else 

#endif	
}

/////////////////////////////////////////////////////////////////////
i3Monitor2::i3Monitor2( i3Mutex2 * pMutex)
{
	m_pMutex = pMutex;

	m_pMutex->Enter();
}

i3Monitor2::~i3Monitor2(void)
{
	m_pMutex->Leave();
}
