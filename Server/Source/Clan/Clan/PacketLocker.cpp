#include "pch.h"
#include "PacketLocker.h"
#include "TrackBuffer.h"
#include "Gateway.h"
//#include "LogFile.h"

I3_CLASS_INSTANCE(CPacketLocker, i3ElementBase);

CPacketLocker* g_pPacketLocker = NULL;

CPacketLocker::CPacketLocker()
{
	m_ppTrackBuffer = NULL;
	m_ui32Count = 0;
}

CPacketLocker::~CPacketLocker()
{
	for(UINT32 i = 0; i < m_ui32Count; i++)
	{
		I3_SAFE_RELEASE(m_ppTrackBuffer[i]);
	}

	I3MEM_SAFE_FREE(m_ppTrackBuffer);
}

void CPacketLocker::Create(UINT32 ui32BufferCount,UINT32 ui32BufferSize,
						   UINT32 ui32DataSize,UINT32 ui32MaxIdx)
{
	I3ASSERT(NULL == m_ppTrackBuffer);
	I3ASSERT(ui32BufferCount);

	m_ppTrackBuffer = (CTrackBuffer**)i3MemAlloc(sizeof(CTrackBuffer*) * ui32BufferCount);
	i3mem::FillZero( m_ppTrackBuffer, sizeof( CTrackBuffer*)* ui32BufferCount );

	for(UINT32 i = 0; i < ui32BufferCount; i++)
	{
		m_ppTrackBuffer[i] = CTrackBuffer::NewObject();
		m_ppTrackBuffer[i]->Create(ui32BufferSize, ui32DataSize, ui32MaxIdx);
	}

	m_ui32Count = ui32BufferCount;
}

BOOL CPacketLocker::Push(UINT32 ui32idxBuffer,UINT8* pui8Data,UINT32 ui32DataSize)
{
	I3ASSERT(m_ppTrackBuffer);

	if (ui32idxBuffer < m_ui32Count)
	{
		return m_ppTrackBuffer[ui32idxBuffer]->Push(pui8Data, ui32DataSize);
	}

	// Index overflow
	return FALSE;
}

UINT32 CPacketLocker::Pop(UINT32 ui32idxBuffer,UINT8* pui8Data,UINT32 ui32DataSize)
{
	I3ASSERT(m_ppTrackBuffer);

	if (ui32idxBuffer < m_ui32Count)
	{
		return m_ppTrackBuffer[ui32idxBuffer]->Pop(pui8Data, ui32DataSize);
	}

	// Index overflow
	return 0;
}
