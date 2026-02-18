#include "pch.h"
#include "TrackBuffer.h"

I3_CLASS_INSTANCE(CTrackBuffer, i3ElementBase);

CTrackBuffer::CTrackBuffer()
{
	m_pui8Buffer		= NULL;

	m_ui32BufferSize	= 0;
	m_ui32DataSize		= 0;
	m_ui32idxWrite		= 0;
	m_ui32idxRead		= 0;
	m_ui32MaxIdx		= 0;

	m_ui32MaxCount		= 0;
}

CTrackBuffer::~CTrackBuffer()
{
	I3MEM_SAFE_FREE(m_pui8Buffer);
}

void CTrackBuffer::Create(UINT32 ui32BufferSize, UINT32 ui32DataSize, UINT32 ui32MaxIdx)
{
	I3ASSERT(NULL == m_pui8Buffer);
	I3ASSERT(ui32BufferSize);
	I3ASSERT(ui32DataSize);
	I3ASSERT(ui32MaxIdx);

	m_ui32DataSize		= ui32DataSize + sizeof(HEADER);
	m_ui32BufferSize	= ui32BufferSize;					//뭔상관이 있는 사이즈인지 궁금
	m_ui32MaxIdx		= ui32MaxIdx;						//뭐에 쓰는 물건인고? 

	m_pui8Buffer = (UINT8*)i3MemAlloc(m_ui32DataSize * ui32BufferSize);
}

BOOL CTrackBuffer::Push(UINT8* pui8Data,UINT32 ui32DataSize)
{
	I3ASSERT(m_pui8Buffer);

	if (ui32DataSize > m_ui32DataSize)
	{
		// DataSize가 할당된 메모리에 비해 크다
		return FALSE;
	}

	UINT32 nCount = m_ui32idxWrite - m_ui32idxRead;

	if (0 == nCount)
	{
		// 초기화 순서 중요; 다중 쓰레드 접근 변수
		m_ui32idxWrite = 0;		// 1st
		m_ui32idxRead = 0;		// 2nd
	}

	if (m_ui32MaxIdx == m_ui32idxWrite)
	{
		// Index overflow
		return FALSE;
	}

	if (nCount < m_ui32BufferSize)
	{
		UINT32 ui32idx = m_ui32idxWrite % m_ui32BufferSize;
		UINT8* pui8Dest = m_pui8Buffer + (m_ui32DataSize * ui32idx);

		// Write header
		((HEADER*)pui8Dest)->_ui32Length = ui32DataSize;
		pui8Dest += sizeof(HEADER);

		// Write data
		i3mem::Copy(pui8Dest, pui8Data, ui32DataSize);

		m_ui32idxWrite++;

		if( m_ui32idxWrite > m_ui32MaxCount ) m_ui32MaxCount = m_ui32idxWrite;

		return TRUE;
	}

	// Buffer is full
	return FALSE;
}

UINT32 CTrackBuffer::Pop(UINT8* pui8Data,UINT32 ui32DataSize)
{
	I3ASSERT(m_pui8Buffer);

	if (m_ui32idxWrite <= m_ui32idxRead)
	{
		// idxWrite가 작으면 초기화 중
		// 같으면 empty
		return 0;
	}

	UINT32 ui32idx = m_ui32idxRead % m_ui32BufferSize;
	UINT8* pui8Src = m_pui8Buffer + (m_ui32DataSize * ui32idx);
	
	// Read header
	UINT32 ui32Length = ((HEADER*)pui8Src)->_ui32Length;
	
	if (ui32DataSize < ui32Length)
	{
		// Pop buffer size가 작다
		return 0;
	}

	pui8Src += sizeof(HEADER);

	// Read data
	i3mem::Copy(pui8Data, pui8Src, ui32Length);

	m_ui32idxRead++;

	return ui32Length;
}
