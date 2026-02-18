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
}

CTrackBuffer::~CTrackBuffer()
{
	I3MEM_SAFE_FREE(m_pui8Buffer);
}

void CTrackBuffer::Create(UINT32 nBufferSize,UINT32 nDataSize,UINT32 nMaxIdx)
{
	I3ASSERT(NULL == m_pui8Buffer);
	I3ASSERT(nBufferSize);
	I3ASSERT(nDataSize);
	I3ASSERT(nMaxIdx);

	m_ui32DataSize = nDataSize + sizeof(HEADER);
	m_ui32BufferSize = nBufferSize;
	m_ui32MaxIdx = nMaxIdx;

	m_pui8Buffer = (UINT8*)i3MemAlloc(m_ui32DataSize * nBufferSize);
	i3mem::FillZero( m_pui8Buffer, m_ui32DataSize * nBufferSize );
}

BOOL CTrackBuffer::Push(UINT8* pData,UINT32 nDataSize)
{
	I3ASSERT(m_pui8Buffer);

	if (nDataSize > m_ui32DataSize)
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
		UINT32 idx = m_ui32idxWrite % m_ui32BufferSize;
		UINT8* pDest = m_pui8Buffer + (m_ui32DataSize * idx);

		// Write header
		((HEADER*)pDest)->_nLength = nDataSize;
		pDest += sizeof(HEADER);

		// Write data
		i3mem::Copy(pDest, pData, nDataSize);

		m_ui32idxWrite++;

		return TRUE;
	}

	// Buffer is full
	return FALSE;
}

UINT32 CTrackBuffer::Pop(UINT8* pData,UINT32 nDataSize)
{
	I3ASSERT(m_pui8Buffer);

	if (m_ui32idxWrite <= m_ui32idxRead)
	{
		// idxWrite가 작으면 초기화 중
		// 같으면 empty
		return 0;
	}

	UINT32 idx = m_ui32idxRead % m_ui32BufferSize;
	UINT8* pSrc = m_pui8Buffer + (m_ui32DataSize * idx);
	
	// Read header
	UINT32 nLength = ((HEADER*)pSrc)->_nLength;
	
	if (nDataSize < nLength)
	{
		// Pop buffer size가 작다
		return 0;
	}

	pSrc += sizeof(HEADER);

	// Read data
	i3mem::Copy(pData, pSrc, nLength);

	m_ui32idxRead++;

	return nLength;
}
