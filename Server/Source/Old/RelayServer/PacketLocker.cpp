#include "pch.h"
#include "PacketLocker.h"
#include "TrackBuffer.h"
#include "Gateway.h"
#include "LogFile.h"

I3_CLASS_INSTANCE(CPacketLocker, i3ElementBase);

CPacketLocker* g_pPacketLocker = NULL;

CPacketLocker::CPacketLocker()
{
	m_ppTrackBuffer = NULL;
	m_nCount = 0;
}

CPacketLocker::~CPacketLocker()
{
	if( m_ppTrackBuffer )
	{
		for(UINT32 i = 0; i < m_nCount; i++)
		{
			I3_SAFE_RELEASE(m_ppTrackBuffer[i]);
		}

		I3MEM_SAFE_FREE(m_ppTrackBuffer);
	}
}

void CPacketLocker::Create(UINT32 nBufferCount,UINT32 nBufferSize,
						   UINT32 nDataSize,UINT32 nMaxIdx)
{
	I3ASSERT(NULL == m_ppTrackBuffer);
	I3ASSERT(nBufferCount);

	m_ppTrackBuffer = (CTrackBuffer**)i3MemAlloc(sizeof(CTrackBuffer*) * nBufferCount);

	for(UINT32 i = 0; i < nBufferCount; i++)
	{
		m_ppTrackBuffer[i] = CTrackBuffer::NewObject();

		UINT32 createSize = nDataSize;
		m_ppTrackBuffer[i]->Create(nBufferSize, createSize, nMaxIdx);
	}

	m_nCount = nBufferCount;
}

BOOL CPacketLocker::Push(UINT32 idxBuffer,UINT8* pData,UINT32 nDataSize)
{
	I3ASSERT(m_ppTrackBuffer);

	if (idxBuffer < m_nCount)
	{
		return m_ppTrackBuffer[idxBuffer]->Push(pData, nDataSize);
	}

	// Index overflow
	return FALSE;
}

UINT32 CPacketLocker::Pop(UINT32 idxBuffer,UINT8* pData,UINT32 nDataSize)
{
	I3ASSERT(m_ppTrackBuffer);

	if (idxBuffer < m_nCount)
	{
		return m_ppTrackBuffer[idxBuffer]->Pop(pData, nDataSize);
	}

	// Index overflow
	return 0;
}
