#include "stdafx.h"
#include "Memory.h"
#include "RingBuffer.h"

//#define BUFFER_MAX_INT_COUNT			2000000000
#define BUFFER_MAX_INT_COUNT			4000000000


NSM_CRingBuffer::NSM_CRingBuffer()
{
	m_pBuffer		= NULL; 
	m_iWriteIdx		= 0; 
	m_iReadIdx		= 0; 
	m_iBufferSize	= 0; 
	m_iBufferCount	= 0; 

}
NSM_CRingBuffer::~NSM_CRingBuffer()
{
	OnDestroy(); 
}

bool NSM_CRingBuffer::OnCreate(INT32 iBufferSize, INT32 iBufferCount)
{
	m_iWriteIdx		= 0; 
	m_iReadIdx		= 0; 
	m_iBufferSize	= iBufferSize; 
	m_iBufferCount	= iBufferCount; 

	m_pBuffer = (char*)NSM_ALLOC(iBufferSize * iBufferCount); 

	if( m_pBuffer == NULL )return false; 

	return true; 
}

bool NSM_CRingBuffer::OnDestroy(void)
{
	NSM_SAFE_FREE( m_pBuffer );
	return true; 
}

INT32 NSM_CRingBuffer::GetEmptyBufferCount(void)
{	
	return m_iBufferCount - GetBufferCount();
}

INT32 NSM_CRingBuffer::GetBufferCount(void)
{
	INT32 Rv = 0; 
	if(m_iWriteIdx > m_iReadIdx)
	{
		Rv = m_iWriteIdx - m_iReadIdx; 
	}	
	return Rv;
}

INT32 NSM_CRingBuffer::Push(void * pBuffer)
{
	if( m_iWriteIdx > (BUFFER_MAX_INT_COUNT - 100) )
	{
		if(m_iWriteIdx == m_iReadIdx)
		{
			m_iWriteIdx	= 0; 
			m_iReadIdx	= 0; 
		}
	}

	if( m_iReadIdx > BUFFER_MAX_INT_COUNT )return RINGBUFFER_ERROR_BUFFER_FULLIDX;
	if( (m_iWriteIdx - m_iReadIdx) >= m_iBufferCount )return RINGBUFFER_ERROR_BUFFER_FULL;

	INT32 iWriteIdx = m_iWriteIdx % m_iBufferCount; 

	NSM_nsMemory::Copy( &m_pBuffer[iWriteIdx * m_iBufferSize], pBuffer, m_iBufferSize); 
	
	m_iWriteIdx++;

	return RINGBUFFER_SUCCESS; 
}


void * NSM_CRingBuffer::PushPointer(void)
{
	if( m_iWriteIdx > (BUFFER_MAX_INT_COUNT - 100) )
	{
		if(m_iWriteIdx == m_iReadIdx)
		{
			m_iWriteIdx	= 0; 
			m_iReadIdx	= 0; 
		}
	}
	if( m_iReadIdx > BUFFER_MAX_INT_COUNT )return NULL;
	if( (m_iWriteIdx - m_iReadIdx) >= m_iBufferCount )return NULL;

	INT32 iWriteIdx = m_iWriteIdx % m_iBufferCount; 

	return &m_pBuffer[iWriteIdx * m_iBufferSize]; 
}

void NSM_CRingBuffer::PushPointerIdx(void)
{
	m_iWriteIdx++;
	return; 
}

void * NSM_CRingBuffer::Pop(void)
{
	void * pBuffer = NULL; 

	if( GetBufferCount() > 0 )
	{
		INT32 iReadIdx = m_iReadIdx % m_iBufferCount; 
		pBuffer = &m_pBuffer[iReadIdx * m_iBufferSize]; 
	}

	return pBuffer; 
}

void NSM_CRingBuffer::PopIdx(void)
{
	if( GetBufferCount() > 0 )m_iReadIdx++;
	return;
}

