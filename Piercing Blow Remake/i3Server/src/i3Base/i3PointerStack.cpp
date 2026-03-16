#include "i3CommonType.h"
#include "i3PointerStack.h"

I3_CLASS_INSTANCE( i3PointerStack, i3ElementBase);

i3PointerStack::i3PointerStack( UINT32 allocUnit)
{
	m_pBuffer = NULL;
	m_Count = 0;
	m_MaxCount = 0;
	m_AllocUnit = allocUnit;
}

i3PointerStack::~i3PointerStack(void)
{
	Reset();
}

void i3PointerStack::AdjustBufferSize( UINT32 newsz)
{
	m_MaxCount = (newsz + (m_AllocUnit - 1)) / m_AllocUnit;
	m_MaxCount *= m_AllocUnit;

	if( m_pBuffer == NULL)
		m_pBuffer = (void **)i3MemAlloc( m_MaxCount * sizeof(void *));
	else
		m_pBuffer = (void **) i3MemReAlloc( m_pBuffer, m_MaxCount * sizeof(void *));
}

void i3PointerStack::Push( void * val)
{
	if( (m_Count + 1) > m_MaxCount)
	{
		AdjustBufferSize( m_MaxCount + 1);
	}

	m_pBuffer[ m_Count] = val;

	m_Count++;
}

void	i3PointerStack::PushToTop( void * p)
{
	if( (m_Count + 1) > m_MaxCount)
	{
		AdjustBufferSize( m_MaxCount + 1);
	}

	if( m_Count > 0)
	{
		memmove( &m_pBuffer[ 1], &m_pBuffer[0], sizeof(void *) * m_Count);
	}

	m_pBuffer[ 0] = p;

	m_Count++;
}

bool i3PointerStack::Pop( void ** pVal)
{
	if( m_Count <= 0)
		return false;

	m_Count --;

	if( pVal != NULL)
		*pVal = m_pBuffer[ m_Count];

	return true;
}

void i3PointerStack::Reset( bool bFreeMem)
{
	if( bFreeMem)
	{
		if( m_pBuffer != NULL)
		{
			i3MemFree( m_pBuffer );
			m_pBuffer = NULL;
		}

		m_MaxCount = 0;
	}

	m_Count = 0;	
}
