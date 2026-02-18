#ifndef _UdpBufferPool_h
#define _UdpBufferPool_h

#include "S2CriticalSection.h"

class CUdpBuffer;

class CUdpBufferPool: public i3ElementBase
{
	I3_CLASS_DEFINE(CUdpBufferPool);
public:
	
	BOOL Create( UINT32 ui32BufferCount );
	UINT32	GetEmptyBufferCount() { return m_ui32EmptyCount; }
	
	CUdpBufferPool();
	virtual ~CUdpBufferPool();

public:

	BOOL GetBuffer( CUdpBuffer** pBuffer);
	void ReleaseBuffer( CUdpBuffer* pBuffer);


private:
	CUdpBuffer*		m_pBuffer;
	UINT32			m_ui32Count;

	UINT32* 		m_pui32EmptyBuffer;
	UINT32			m_ui32EmptyCount;
	INT32			m_i32EmptyFrontIdx;
	INT32			m_i32EmptyRearIdx;
	
	S2CriticalSection m_cs;
};

extern CUdpBufferPool* g_pUdpBufferPool;
#endif
