#include "pch.h"
#include "RingBuffer.h"

#define LOCAL_BUFFER_MAX_INT_COUNT			4000000000

I3_CLASS_INSTANCE( CRingBuffer, i3ElementBase );

CRingBuffer::CRingBuffer()
{
	m_pBuffer			= NULL;
	m_iWriteIdx			= 0;
	m_iReadIdx			= 0;
	m_iBufferSize		= 0;
	m_iBufferCount		= 0;

	m_ui32UseCount		= 0;
}

CRingBuffer::~CRingBuffer()
{
	OnDestroy(); 
}

bool CRingBuffer::OnCreate(INT32 iBufferSize, INT32 iBufferCount)
{
	m_iWriteIdx		= 0; 
	m_iReadIdx		= 0; 
	m_iBufferSize	= iBufferSize; 
	m_iBufferCount	= iBufferCount; 

	SetResetUseCount();

	m_pBuffer = (char*)i3MemAlloc(iBufferSize * iBufferCount); 

	if( m_pBuffer == NULL )return false; 

	return true; 
}

bool CRingBuffer::OnDestroy(void)
{
	I3MEM_SAFE_FREE( m_pBuffer );
	return true; 
}

INT32 CRingBuffer::GetEmptyBufferCount(void)
{	
	return m_iBufferCount - GetBufferCount();
}

INT32 CRingBuffer::GetBufferCount(void)
{
	INT32 Rv = 0; 
	if(m_iWriteIdx > m_iReadIdx)
	{
		Rv = m_iWriteIdx - m_iReadIdx; 
	}	
	return Rv;
}

INT32 CRingBuffer::Push(void * pBuffer)
{
	if( m_iWriteIdx > (LOCAL_BUFFER_MAX_INT_COUNT - 100) )
	{
		if(m_iWriteIdx == m_iReadIdx)
		{
			m_iWriteIdx	= 0; 
			m_iReadIdx	= 0; 
		}
	}

	if( m_iReadIdx > LOCAL_BUFFER_MAX_INT_COUNT )return RINGBUFFER_ERROR_BUFFER_FULLIDX;
	if( (m_iWriteIdx - m_iReadIdx) >= m_iBufferCount )return RINGBUFFER_ERROR_BUFFER_FULL;

	INT32 iWriteIdx = m_iWriteIdx % m_iBufferCount; 

	i3mem::Copy( &m_pBuffer[iWriteIdx * m_iBufferSize], pBuffer, m_iBufferSize); 
	
	m_iWriteIdx++;
	m_ui32UseCount++;

	return RINGBUFFER_SUCCESS; 
}


void * CRingBuffer::PushPointer(void)
{
	if( m_iWriteIdx > (LOCAL_BUFFER_MAX_INT_COUNT - 100) )
	{
		if(m_iWriteIdx == m_iReadIdx)
		{
			m_iWriteIdx	= 0; 
			m_iReadIdx	= 0; 
		}
	}
	if( m_iReadIdx > LOCAL_BUFFER_MAX_INT_COUNT )return NULL;
	if( (m_iWriteIdx - m_iReadIdx) >= m_iBufferCount )return NULL;

	INT32 iWriteIdx = m_iWriteIdx % m_iBufferCount; 

	return &m_pBuffer[iWriteIdx * m_iBufferSize]; 
}

void CRingBuffer::PushPointerIdx(void)
{
	m_iWriteIdx++;
	m_ui32UseCount++;
	return; 
}

void * CRingBuffer::Pop(void)
{
	void * pBuffer = NULL; 

	if( GetBufferCount() > 0 )
	{
		INT32 iReadIdx = m_iReadIdx % m_iBufferCount; 
		pBuffer = &m_pBuffer[iReadIdx * m_iBufferSize]; 
	}

	return pBuffer; 
}

void CRingBuffer::PopIdx(void)
{
	if( GetBufferCount() > 0 )m_iReadIdx++;
	return;
}

