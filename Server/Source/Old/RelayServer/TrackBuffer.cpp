#include "pch.h"
#include "TrackBuffer.h"

I3_CLASS_INSTANCE(CTrackBuffer, i3ElementBase);

CTrackBuffer::CTrackBuffer()
{
	m_pBuffer = NULL;

	m_nBufferSize = 0;
	m_nDataSize = 0;
	m_idxWrite = 0;
	m_idxRead = 0;
	m_nMaxIdx = 0;
}

CTrackBuffer::~CTrackBuffer()
{
	I3MEM_SAFE_FREE(m_pBuffer);
}

void CTrackBuffer::Create(UINT32 nBufferSize,UINT32 nDataSize,UINT32 nMaxIdx)
{
	I3ASSERT(NULL == m_pBuffer);
	I3ASSERT(nBufferSize);
	I3ASSERT(nDataSize);
	I3ASSERT(nMaxIdx);

	m_nDataSize = nDataSize + sizeof(HEADER);
	m_nBufferSize = nBufferSize;
	m_nMaxIdx = nMaxIdx;

	m_pBuffer = (UINT8*)i3MemAlloc(m_nDataSize * nBufferSize);
}

BOOL CTrackBuffer::Push(UINT8* pData,UINT32 nDataSize)
{
	I3ASSERT(m_pBuffer);

	if (nDataSize > m_nDataSize)
	{
		// DataSize가 할당된 메모리에 비해 크다
		return FALSE;
	}

	UINT32 nCount = m_idxWrite - m_idxRead;

	if (0 == nCount)
	{
		// 초기화 순서 중요; 다중 쓰레드 접근 변수
		m_idxWrite = 0;		// 1st
		m_idxRead = 0;		// 2nd
	}

	if (m_nMaxIdx == m_idxWrite)
	{
		// Index overflow
		return FALSE;
	}

	if (nCount < m_nBufferSize)
	{
		UINT32 idx = m_idxWrite % m_nBufferSize;
		UINT8* pDest = m_pBuffer + (m_nDataSize * idx);

		// Write header
		((HEADER*)pDest)->_nLength = nDataSize;
		pDest += sizeof(HEADER);

		// Write data
		i3mem::Copy(pDest, pData, nDataSize);

		m_idxWrite++;

		return TRUE;
	}

	// Buffer is full
	return FALSE;
}

UINT32 CTrackBuffer::Pop(UINT8* pData,UINT32 nDataSize)
{
	I3ASSERT(m_pBuffer);

	if (m_idxWrite <= m_idxRead)
	{
		// idxWrite가 작으면 초기화 중
		// 같으면 empty
		return 0;
	}

	UINT32 idx = m_idxRead % m_nBufferSize;
	UINT8* pSrc = m_pBuffer + (m_nDataSize * idx);
	
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

	m_idxRead++;

	return nLength;
}
