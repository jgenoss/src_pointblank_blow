#include "pch.h"
#include "LocalRingBuffer.h"

#define LOCAL_BUFFER_MAX_INT_COUNT			4000000000

Local_CRingBuffer::Local_CRingBuffer()
{
	m_pBuffer			= NULL;
	m_ui32WriteIdx		= 0;
	m_ui32ReadIdx		= 0;
	m_ui32BufferSize	= 0;
	m_ui32BufferCount	= 0;
}

Local_CRingBuffer::~Local_CRingBuffer()
{
	OnDestroy(); 
}

bool Local_CRingBuffer::OnCreate(INT32 i32BufferSize, INT32 i32BufferCount)
{
	m_ui32WriteIdx		= 0;
	m_ui32ReadIdx		= 0;
	m_ui32BufferSize	= i32BufferSize;
	m_ui32BufferCount	= i32BufferCount;

	m_pBuffer = (char*)i3MemAlloc(i32BufferSize * i32BufferCount);

	if( m_pBuffer == NULL )return false; 

	return true; 
}

bool Local_CRingBuffer::OnDestroy(void)
{
	I3MEM_SAFE_FREE( m_pBuffer );
	return true; 
}

INT32 Local_CRingBuffer::GetEmptyBufferCount(void)
{	
	return m_ui32BufferCount - GetBufferCount();
}

INT32 Local_CRingBuffer::GetBufferCount(void)
{
	INT32 i32Rv = 0; 
	if(m_ui32WriteIdx > m_ui32ReadIdx)
	{
		i32Rv = m_ui32WriteIdx - m_ui32ReadIdx;
	}	
	return i32Rv;
}

INT32 Local_CRingBuffer::Push(void * pBuffer)
{
	if( m_ui32WriteIdx > (LOCAL_BUFFER_MAX_INT_COUNT - 100) )
	{
		if(m_ui32WriteIdx == m_ui32ReadIdx)
		{
			m_ui32WriteIdx	= 0; 
			m_ui32ReadIdx	= 0; 
		}
	}

	if( m_ui32ReadIdx > LOCAL_BUFFER_MAX_INT_COUNT )return LOCAL_RINGBUFFER_ERROR_BUFFER_FULLIDX;
	if( (m_ui32WriteIdx - m_ui32ReadIdx) >= m_ui32BufferCount )return LOCAL_RINGBUFFER_ERROR_BUFFER_FULL;

	INT32 i32WriteIdx = m_ui32WriteIdx % m_ui32BufferCount;

	i3mem::Copy( &m_pBuffer[i32WriteIdx * m_ui32BufferSize], pBuffer, m_ui32BufferSize);
	
	m_ui32WriteIdx++;

	return LOCAL_RINGBUFFER_SUCCESS; 
}


void * Local_CRingBuffer::PushPointer(void)
{
	if( m_ui32WriteIdx > (LOCAL_BUFFER_MAX_INT_COUNT - 100) )
	{
		if(m_ui32WriteIdx == m_ui32ReadIdx)
		{
			m_ui32WriteIdx	= 0; 
			m_ui32ReadIdx	= 0; 
		}
	}
	if( m_ui32ReadIdx > LOCAL_BUFFER_MAX_INT_COUNT )return NULL;
	if( (m_ui32WriteIdx - m_ui32ReadIdx) >= m_ui32BufferCount )return NULL;

	INT32 i32WriteIdx = m_ui32WriteIdx % m_ui32BufferCount;

	return &m_pBuffer[i32WriteIdx * m_ui32BufferSize];
}

void Local_CRingBuffer::PushPointerIdx(void)
{
	m_ui32WriteIdx++;
	return; 
}

void * Local_CRingBuffer::Pop(void)
{
	void * pBuffer = NULL; 

	if( GetBufferCount() > 0 )
	{
		INT32 i32ReadIdx = m_ui32ReadIdx % m_ui32BufferCount;
		pBuffer = &m_pBuffer[i32ReadIdx * m_ui32BufferSize];
	}

	return pBuffer; 
}

void Local_CRingBuffer::PopIdx(void)
{
	if( GetBufferCount() > 0 )m_ui32ReadIdx++;
	return;
}

