#include "pch.h"
//#include "S2Memory.h"
#include "S2RingBuffer.h"

//#define BUFFER_MAX_INT_COUNT			2000000000
//#define BUFFER_MAX_INT_COUNT			4000000000


S2RingBuffer::S2RingBuffer()
{
	m_pBuffer			= NULL; 
	m_ui32WriteIdx		= 0;
	m_ui32ReadIdx		= 0;
	m_ui32BufferSize	= 0;
	m_ui32BufferCount	= 0;
	m_ui32WriteMax		= 0;

}
S2RingBuffer::~S2RingBuffer()
{
	Destroy(); 
}

bool S2RingBuffer::Create(INT32 iBufferSize, INT32 iBufferCount)
{
	m_ui32WriteIdx		= 0;
	m_ui32ReadIdx		= 0;
	m_ui32BufferSize	= iBufferSize;
	m_ui32BufferCount	= iBufferCount;

	//m_pBuffer = (char*)S2_ALLOC(iBufferSize * iBufferCount); 
	m_pBuffer = new char[ iBufferSize * iBufferCount ];//(char*)S2_ALLOC(iBufferSize * iBufferCount); 

	if( m_pBuffer == NULL )return false; 

	return true; 
}

bool S2RingBuffer::Destroy(void)
{
	NEW_SAFE_DELETE_ARRAY( m_pBuffer );
	return true; 
}

UINT32 S2RingBuffer::GetEmptyBufferCount(void)
{	
	return m_ui32BufferCount - GetBufferCount();
}

UINT32 S2RingBuffer::GetBufferCount(void)
{
	UINT32 ui32Rv = 0; 
	if( m_ui32WriteIdx > m_ui32ReadIdx )
	{
		ui32Rv = m_ui32WriteIdx - m_ui32ReadIdx; 
	}	
	return ui32Rv;
}

INT32 S2RingBuffer::Push(void * pBuffer)
{
	return Push( pBuffer, m_ui32BufferSize );
}

INT32 S2RingBuffer::Push(void * pBuffer, INT32 i32Size)
{
	if( m_ui32WriteIdx > (BUFFER_MAX_INT_COUNT - 100) )
	{
		if(m_ui32WriteIdx == m_ui32ReadIdx)
		{
			m_ui32WriteIdx	= 0;
			m_ui32ReadIdx	= 0;
		}
	}

	if( m_ui32ReadIdx > BUFFER_MAX_INT_COUNT )					return RINGBUFFER_ERROR_BUFFER_FULLIDX;
	if( (m_ui32WriteIdx - m_ui32ReadIdx) >= m_ui32BufferCount )	return RINGBUFFER_ERROR_BUFFER_FULL;

	INT32 i32WriteIdx = m_ui32WriteIdx % m_ui32BufferCount; 

	memcpy( &m_pBuffer[i32WriteIdx * m_ui32BufferSize], pBuffer, i32Size );
	//S2Memory::Copy( &m_pBuffer[iWriteIdx * m_ui32BufferSize], pBuffer, i32Size); 
	
	m_ui32WriteIdx++;

	if( m_ui32WriteIdx > m_ui32WriteMax )	m_ui32WriteMax = m_ui32WriteIdx;

	return RINGBUFFER_SUCCESS; 
}

void * S2RingBuffer::PushPointer(void)
{
	if( m_ui32WriteIdx > (BUFFER_MAX_INT_COUNT - 100) )
	{
		if(m_ui32WriteIdx == m_ui32ReadIdx)
		{
			m_ui32WriteIdx	= 0;
			m_ui32ReadIdx	= 0;
		}
	}
	if( m_ui32ReadIdx > BUFFER_MAX_INT_COUNT )					return NULL;
	if( (m_ui32WriteIdx - m_ui32ReadIdx) >= m_ui32BufferCount )	return NULL;

	INT32 iWriteIdx = m_ui32WriteIdx % m_ui32BufferCount; 

	return &m_pBuffer[iWriteIdx * m_ui32BufferSize]; 
}

void S2RingBuffer::PushPointerIdx(void)
{
	m_ui32WriteIdx++;
	return; 
}

void * S2RingBuffer::Pop(void)
{
	void * pBuffer = NULL; 

	if( 0 < GetBufferCount() )
	{
		INT32 i32ReadIdx = m_ui32ReadIdx % m_ui32BufferCount; 
		pBuffer = &m_pBuffer[i32ReadIdx * m_ui32BufferSize]; 
	}

	return pBuffer; 
}

void S2RingBuffer::PopIdx(void)
{
	if( GetBufferCount() > 0 )m_ui32ReadIdx++;
	return;
}