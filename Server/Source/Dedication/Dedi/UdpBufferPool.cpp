#include "pch.h"
#include "UdpBufferPool.h"
#include "UdpBuffer.h"

CUdpBufferPool* g_pUdpBufferPool = NULL;

I3_CLASS_INSTANCE(CUdpBufferPool, i3ElementBase);

CUdpBufferPool::CUdpBufferPool()
{
	// memory
	m_pBuffer			= NULL;
	m_pui32EmptyBuffer	= NULL;
	// size
	m_ui32Count			= 0;	
	m_ui32EmptyCount	= 0;
	// index
	m_ui32EmptyFrontIdx	= 0;
	m_ui32EmptyRearIdx	= 0;
}

CUdpBufferPool::~CUdpBufferPool()
{
	I3_SAFE_DELETE_ARRAY( m_pBuffer );
	I3MEM_SAFE_FREE( m_pui32EmptyBuffer);	
}

BOOL CUdpBufferPool::Create( UINT32 ui32BufferCount)
{
	I3ASSERT( ui32BufferCount > 0 );

	// bulk memory
	m_pBuffer = new CUdpBuffer[ ui32BufferCount ];
	I3ASSERT( m_pBuffer != NULL);
	if( !m_pBuffer ) return FALSE;

	// free memory index array
	m_pui32EmptyBuffer = (UINT32*)i3MemAlloc( sizeof( UINT32 ) * ui32BufferCount);
	I3ASSERT( m_pui32EmptyBuffer != NULL );
	if( !m_pui32EmptyBuffer ) return FALSE;
	
	for( UINT32 i = 0; i < ui32BufferCount; i++ ) m_pui32EmptyBuffer[i] = i;
	
	// reset index
	m_ui32EmptyFrontIdx = m_ui32EmptyRearIdx  = 0;
	m_ui32EmptyCount = m_ui32Count = ui32BufferCount;
	
	return TRUE;
}

BOOL CUdpBufferPool::GetBuffer( CUdpBuffer** pBuffer)
{
	CThreadSync cs(m_cs);

	I3ASSERT( pBuffer != NULL);
	I3ASSERT( m_pBuffer != NULL);
	I3ASSERT( m_ui32Count > 0 );

	if ( m_ui32EmptyCount == 0) { *pBuffer = NULL; return FALSE; }

	UINT32 ui32Idx		= m_pui32EmptyBuffer[ m_ui32EmptyFrontIdx ];
	I3ASSERT( ui32Idx < m_ui32Count );
	m_ui32EmptyFrontIdx = ( m_ui32EmptyFrontIdx + 1 ) % m_ui32Count;
	m_ui32EmptyCount--;

	*pBuffer = &m_pBuffer[ ui32Idx ];
	(*pBuffer)->InitRef();
	(*pBuffer)->AddRef();

	return TRUE;
}

void CUdpBufferPool::ReleaseBuffer(CUdpBuffer* pBuffer)
{
	I3ASSERT( pBuffer != NULL);
	I3ASSERT( m_pBuffer != NULL);
	I3ASSERT( m_ui32EmptyCount < m_ui32Count);

	CThreadSync cs(m_cs);

	if( pBuffer == NULL )
	{
		I3ASSERT(0);
		DSERROR("[CUdpBufferPool::ReleaseBuffer] pBuffer is null\n");
		return;
	}
	
	UINT32 ui32Idx = (UINT32)(pBuffer - m_pBuffer);
	I3ASSERT(ui32Idx < m_ui32Count );
	if( ui32Idx >= m_ui32Count )
	{
		I3ASSERT(0);
		DSERROR("[CUdpBufferPool::ReleaseBuffer] ReleaseBuffer \n");
		return;
	}

	UINT8 ui8RefCount = pBuffer->Release();
	I3ASSERT( ui8RefCount == 0 );
	if( ui8RefCount != 0 )
	{
		pBuffer->InitRef();
		DSERROR("[CUdpBufferPool::ReleaseBuffer] pBuffer->m_cRef != 0\n");
	}
	
	m_pui32EmptyBuffer[ m_ui32EmptyRearIdx ] = ui32Idx;
	m_ui32EmptyRearIdx = (m_ui32EmptyRearIdx + 1) % m_ui32Count;
	m_ui32EmptyCount++;
}
