#include "pch.h"
#include "UdpBufferPool.h"
#include "UdpBuffer.h"

I3_CLASS_INSTANCE(CUdpBufferPool, i3ElementBase);


CUdpBufferPool* g_pUdpBufferPool = NULL;

CUdpBufferPool::CUdpBufferPool()
{
	m_pBuffer			= NULL;
	m_ui32Count			= 0;	

	m_pui32EmptyBuffer	= NULL;
	m_ui32EmptyCount	= NULL;
	m_i32EmptyFrontIdx	= NULL;
	m_i32EmptyRearIdx	= NULL;
}

CUdpBufferPool::~CUdpBufferPool()
{
	I3_SAFE_DELETE_ARRAY( m_pBuffer );
	I3MEM_SAFE_FREE( m_pui32EmptyBuffer);	
}

BOOL CUdpBufferPool::Create( UINT32 ui32BufferCount)
{
	I3ASSERT(m_pBuffer == NULL);
	I3ASSERT(m_pui32EmptyBuffer == NULL);

	// 滚欺 积己
	m_pBuffer = new (nothrow) CUdpBuffer[ui32BufferCount];
	I3ASSERT( m_pBuffer != NULL);

	m_ui32Count = ui32BufferCount;

	// free memory 格废 积己
	m_pui32EmptyBuffer = (UINT32*)i3MemAlloc( sizeof( INT32 ) * ui32BufferCount);
	I3ASSERT( m_pui32EmptyBuffer != NULL );
	
	// free memory 格废俊 idx 持扁
	m_ui32EmptyCount = ui32BufferCount;
	m_i32EmptyFrontIdx = 0;
	m_i32EmptyRearIdx  = 0;

	for( UINT32 i = 0; i < ui32BufferCount; i++)
	{
		m_pui32EmptyBuffer[i] = i;
	}

	return TRUE;
}

BOOL CUdpBufferPool::GetBuffer( CUdpBuffer** pBuffer)
{
	I3ASSERT( pBuffer != NULL);
	I3ASSERT( m_pBuffer != NULL);
	
	m_cs.Lock();

	if ( m_ui32EmptyCount <= 0)
	{
		m_cs.Unlock();
		return FALSE;
	}

	INT32 idx = m_pui32EmptyBuffer[ m_i32EmptyFrontIdx ];
	m_i32EmptyFrontIdx = ( m_i32EmptyFrontIdx + 1 ) % m_ui32Count;
	m_ui32EmptyCount--;

	*pBuffer = &m_pBuffer[ idx ];
	(*pBuffer)->SetIdx( idx );
	(*pBuffer)->m_cRef = 1;

	m_cs.Unlock();

	return TRUE;
}

void CUdpBufferPool::ReleaseBuffer(CUdpBuffer* pBuffer)
{
	I3ASSERT( pBuffer != NULL);
	I3ASSERT( m_pBuffer != NULL);
	I3ASSERT( m_ui32EmptyCount < m_ui32Count);

	if( pBuffer == NULL )
	{
		DSERROR("[CUdpBufferPool::ReleaseBuffer] pBuffer is null\n");
		return;
	}
	
	INT32 i32Idx = pBuffer->GetIdx();
	pBuffer->m_cRef--;

	if ( i32Idx < 0 || m_ui32Count - 1 < (UINT)i32Idx)
	{
		DSERROR("[CUdpBufferPool::ReleaseBuffer] ReleaseBuffer \n");
		return;
	}

	if( pBuffer->m_cRef != 0) 
	{
		return;
	}

	m_cs.Lock();

	m_pui32EmptyBuffer[ m_i32EmptyRearIdx ] = i32Idx;
	m_i32EmptyRearIdx = (m_i32EmptyRearIdx + 1) % m_ui32Count;
	m_ui32EmptyCount++;

	m_cs.Unlock();
}
