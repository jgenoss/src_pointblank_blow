#include "pch.h"
#include "RingBuffer.h"

#define BUFFER_MAX_INT_COUNT			4000000000


NSM_CRingBuffer::NSM_CRingBuffer()
{
	m_pBuffer			= NULL; 
	m_ui32WriteIdx		= 0; 
	m_ui32ReadIdx		= 0; 
	m_ui32BufferSize	= 0; 
	m_ui32BufferCount	= 0; 

}
NSM_CRingBuffer::~NSM_CRingBuffer()
{
	OnDestroy(); 
}

bool NSM_CRingBuffer::OnCreate(INT32 i32BufferSize, INT32 i32BufferCount)
{
	m_ui32WriteIdx		= 0; 
	m_ui32ReadIdx		= 0; 
	m_ui32BufferSize	= i32BufferSize; 
	m_ui32BufferCount	= i32BufferCount; 

	m_pBuffer = (char*)i3MemAlloc(i32BufferSize * i32BufferCount); 

	if( m_pBuffer == NULL )return false; 

	return true; 
}

bool NSM_CRingBuffer::OnDestroy(void)
{
	I3MEM_SAFE_FREE( m_pBuffer );
	return true; 
}

INT32 NSM_CRingBuffer::GetEmptyBufferCount(void)
{	
	return m_ui32BufferCount - GetBufferCount();
}

INT32 NSM_CRingBuffer::GetBufferCount(void)
{
	INT32 i32Rv = 0; 
	if(m_ui32WriteIdx > m_ui32ReadIdx)
	{
		i32Rv = m_ui32WriteIdx - m_ui32ReadIdx; 
	}	
	return i32Rv;
}

INT32 NSM_CRingBuffer::Push(void * pBuffer)
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

	i3mem::Copy( &m_pBuffer[i32WriteIdx * m_ui32BufferSize], pBuffer, m_ui32BufferSize );
	
	m_ui32WriteIdx++;

	return RINGBUFFER_SUCCESS; 
}


void * NSM_CRingBuffer::PushPointer(void)
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

void NSM_CRingBuffer::PushPointerIdx(void)
{
	m_ui32WriteIdx++;
	return; 
}

void * NSM_CRingBuffer::Pop(void)
{
	void * pBuffer = NULL; 

	if( GetBufferCount() > 0 )
	{
		INT32 iReadIdx = m_ui32ReadIdx % m_ui32BufferCount; 
		pBuffer = &m_pBuffer[iReadIdx * m_ui32BufferSize]; 
	}

	return pBuffer; 
}

void NSM_CRingBuffer::PopIdx(void)
{
	if( GetBufferCount() > 0 )m_ui32ReadIdx++;
	return;
}

