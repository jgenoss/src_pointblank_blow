#ifndef _UdpBufferPool_h
#define _UdpBufferPool_h

#include "S2CriticalSection.h"

class CUdpBuffer;

class CUdpBufferPool : public i3ElementBase
{
	I3_CLASS_DEFINE(CUdpBufferPool);
public:
	BOOL	GetBuffer( CUdpBuffer** pBuffer);
	void	ReleaseBuffer( CUdpBuffer* pBuffer);

	BOOL	Create( UINT32 ui32BufferCount );
	UINT32	GetEmptyBufferCount() const { return m_ui32EmptyCount; }
	
	CUdpBufferPool();
	virtual ~CUdpBufferPool();

private:
	CUdpBuffer*		m_pBuffer;
	UINT32* 		m_pui32EmptyBuffer;
	
	UINT32			m_ui32Count;
	UINT32			m_ui32EmptyCount;
	UINT32			m_ui32EmptyFrontIdx;
	UINT32			m_ui32EmptyRearIdx;
	
	S2CriticalSection m_cs;
};

extern CUdpBufferPool* g_pUdpBufferPool;
#endif
