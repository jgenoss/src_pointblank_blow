#include "i3CommonType.h"
#include "i3RingBuffer.h"

I3_CLASS_INSTANCE( i3RingBuffer, i3ElementBase );

#define RING_BUFFER_MAX_INT_COUNT			4000000000

i3RingBuffer::i3RingBuffer()
{
	m_pBuffer			= NULL; 
	m_ui32WriteIdx		= 0; 
	m_ui32ReadIdx		= 0; 
	m_ui32BufferSize	= 0; 
	m_ui32BufferCount	= 0; 

}
i3RingBuffer::~i3RingBuffer()
{
	Destroy(); 
}

BOOL i3RingBuffer::Create( INT32 i32BufferSize, INT32 i32BufferCount )
{
	m_ui32WriteIdx		= 0; 
	m_ui32ReadIdx		= 0; 
	m_ui32BufferSize	= i32BufferSize; 
	m_ui32BufferCount	= i32BufferCount; 

	m_pBuffer = (char*)i3MemAlloc( i32BufferSize * i32BufferCount ); 
	if( m_pBuffer == NULL )	return FALSE; 

	return TRUE; 
}

void i3RingBuffer::Destroy(void)
{
	I3MEM_SAFE_FREE( m_pBuffer );
}

INT32 i3RingBuffer::GetEmptyBufferCount(void)
{	
	return m_ui32BufferCount - GetBufferCount();
}

INT32 i3RingBuffer::GetBufferCount(void)
{
	INT32 Rv = 0; 
	if(m_ui32WriteIdx > m_ui32ReadIdx)
	{
		Rv = m_ui32WriteIdx - m_ui32ReadIdx; 
	}	
	return Rv;
}

INT32 i3RingBuffer::Push(void * pBuffer)
{
	if( m_ui32WriteIdx > (RING_BUFFER_MAX_INT_COUNT - 100) )
	{
		if(m_ui32WriteIdx == m_ui32ReadIdx)
		{
			m_ui32WriteIdx	= 0;
			m_ui32ReadIdx	= 0;
		}
	}

	if( m_ui32ReadIdx > RING_BUFFER_MAX_INT_COUNT )return RING_BUFFER_ERROR_BUFFER_FULLIDX;
	if( (m_ui32WriteIdx - m_ui32ReadIdx) >= m_ui32BufferCount )return RING_BUFFER_ERROR_BUFFER_FULL;

	INT32 iWriteIdx = m_ui32WriteIdx % m_ui32BufferCount; 

	i3mem::Copy( &m_pBuffer[iWriteIdx * m_ui32BufferSize], pBuffer, m_ui32BufferSize); 
	
	m_ui32WriteIdx++;

	return RING_BUFFER_SUCCESS; 
}


void * i3RingBuffer::PushPointer(void)
{
	if( m_ui32WriteIdx > (RING_BUFFER_MAX_INT_COUNT - 100) )
	{
		if(m_ui32WriteIdx == m_ui32ReadIdx)
		{
			m_ui32WriteIdx	= 0; 
			m_ui32ReadIdx	= 0; 
		}
	}
	if( m_ui32ReadIdx > RING_BUFFER_MAX_INT_COUNT )return NULL;
	if( (m_ui32WriteIdx - m_ui32ReadIdx) >= m_ui32BufferCount )return NULL;

	INT32 iWriteIdx = m_ui32WriteIdx % m_ui32BufferCount; 

	return &m_pBuffer[iWriteIdx * m_ui32BufferSize]; 
}

void i3RingBuffer::PushPointerIdx(void)
{
	m_ui32WriteIdx++;
	return; 
}

void * i3RingBuffer::Pop(void)
{
	void * pBuffer = NULL; 

	if( GetBufferCount() > 0 )
	{
		INT32 iReadIdx = m_ui32ReadIdx % m_ui32BufferCount; 
		pBuffer = &m_pBuffer[iReadIdx * m_ui32BufferSize]; 
	}

	return pBuffer; 
}

void i3RingBuffer::PopIdx(void)
{
	if( GetBufferCount() > 0 )m_ui32ReadIdx++;
	return;
}

