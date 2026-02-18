#include "i3CommonType.h"
#include "i3Signal.h"
#include "i3ErrorHandler.h"

I3_CLASS_INSTANCE( i3Signal);

i3Signal::~i3Signal(void)
{
#if defined( I3_WINDOWS)
	if( m_hEvent != nullptr)
	{
		CloseHandle( m_hEvent);
		m_hEvent = nullptr;
	}
#endif
}

bool i3Signal::Create( bool bManualReset, bool bInitState)
{
#if defined( I3_WINDOWS)
	m_hEvent = ::CreateEvent(NULL, bManualReset, bInitState, NULL);
	if( m_hEvent == nullptr)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not create signal object.");
		return false;
	}
#endif

	return true;
}

bool	i3Signal::Raise(void)
{
#if defined( I3_WINDOWS)
	I3ASSERT( m_hEvent != nullptr);

	return ::SetEvent( m_hEvent) != FALSE;
#else
	return true;
#endif
}

bool i3Signal::Reset(void)
{
#if defined( I3_WINDOWS)
	I3ASSERT( m_hEvent != nullptr);

	return ::ResetEvent( m_hEvent) != FALSE;
#else
	return true;
#endif
}

bool i3Signal::WaitFor( UINT32 timeout)
{
#if defined( I3_WINDOWS)
	DWORD rv;
	I3ASSERT( m_hEvent != nullptr);

	if( timeout == 0xFFFFFFFF)		timeout = INFINITE;

	rv = ::WaitForSingleObjectEx( m_hEvent, timeout, TRUE);

	return (rv == WAIT_OBJECT_0);
#else
	return true;
#endif
}

