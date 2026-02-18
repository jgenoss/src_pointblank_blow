#include "i3CommonType.h"
#include "i3MetaTemplate.h"
#include "i3Signal.h"
#include "i3ErrorHandler.h"

I3_CLASS_INSTANCE( i3Signal, i3ElementBase);

i3Signal::i3Signal(void)
{
#if defined( I3_WINDOWS)
	m_hEvent = NULL;
#endif
}

i3Signal::~i3Signal(void)
{
#if defined( I3_WINDOWS)
	if( m_hEvent != NULL)
	{
		CloseHandle( m_hEvent);
		m_hEvent = NULL;
	}
#endif
}

bool i3Signal::Create( bool bManualReset, bool bInitState)
{
#if defined( I3_WINDOWS)
	m_hEvent = CreateEvent( NULL, bManualReset, bInitState, NULL);
	if( m_hEvent == NULL)
	{
		I3FATAL( "Could not create signal object.\n");
		return false;
	}
#endif

	return true;
}

bool	i3Signal::Raise(void)
{
#if defined( I3_WINDOWS)
	I3ASSERT( m_hEvent != NULL);

	return SetEvent( m_hEvent) != FALSE;
#else
	return TRUE;
#endif
}

bool i3Signal::Reset(void)
{
#if defined( I3_WINDOWS)
	I3ASSERT( m_hEvent != NULL);

	return ResetEvent( m_hEvent) != FALSE;
#else
	return TRUE;
#endif
}

bool i3Signal::WaitFor( UINT32 timeout)
{
#if defined( I3_WINDOWS)
	DWORD rv;
	I3ASSERT( m_hEvent != NULL);

	if( timeout == 0xFFFFFFFF)		timeout = INFINITE;

	rv = WaitForSingleObjectEx( m_hEvent, timeout, TRUE);

	return (rv == WAIT_OBJECT_0);
#else
	return TRUE;
#endif
}

