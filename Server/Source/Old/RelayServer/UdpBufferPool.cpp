#include "pch.h"
#include "UdpBufferPool.h"
#include "UdpBuffer.h"

I3_CLASS_INSTANCE(CUdpBufferPool, i3ElementBase);

CUdpBufferPool::CUdpBufferPool()
{
	m_pBuffer = NULL;
	m_count = 0;	

	m_pEmptyBuffer = NULL;
	m_emptyCount = NULL;
	m_emptyFront = NULL;
	m_emptyRear = NULL;
}

CUdpBufferPool::~CUdpBufferPool()
{
	if (m_pBuffer != NULL)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}

	I3MEM_SAFE_FREE(m_pEmptyBuffer);	
}

BOOL CUdpBufferPool::Create(INT32 bufferCount)
{
	I3ASSERT(m_pBuffer == NULL);
	I3ASSERT(m_pEmptyBuffer == NULL);

	// 滚欺 积己
	m_pBuffer = new (nothrow) CUdpBuffer[bufferCount];
	I3ASSERT(m_pBuffer != NULL);

	if (m_pBuffer == NULL)
	{
		return FALSE;
	}

	m_count = bufferCount;


	// 后 滚欺 格废 积己
	m_pEmptyBuffer = (INT32*)i3MemAlloc(sizeof(INT32) * bufferCount);
	I3ASSERT(m_pEmptyBuffer != NULL);

	if (m_pEmptyBuffer == NULL)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;

		return FALSE;
	}


	// 后 滚欺 格废俊 idx 持扁
	m_emptyCount = bufferCount;
	m_emptyFront = 0;
	m_emptyRear = 0;

	for(INT32 i = 0; i < bufferCount; i++)
	{
		m_pEmptyBuffer[i] = i;
	}

	return TRUE;
}

BOOL CUdpBufferPool::GetBuffer(CUdpBuffer** pBuffer)
{
	I3ASSERT(m_pBuffer != NULL);
	I3ASSERT(pBuffer != NULL);

	m_emptyMutex.Enter();

	if (m_emptyCount == 0)
	{
		m_emptyMutex.Leave();
		return FALSE;
	}

	INT32 idx = m_pEmptyBuffer[m_emptyFront];
	m_emptyFront = (m_emptyFront + 1) % m_count;
	m_emptyCount--;

	
	m_emptyMutex.Leave();


	*pBuffer = &m_pBuffer[idx];
	(*pBuffer)->SetIdx(idx);

	return TRUE;
}

void CUdpBufferPool::ReleaseBuffer(CUdpBuffer* pBuffer)
{
	I3ASSERT(m_pBuffer != NULL);
	I3ASSERT(pBuffer != NULL);
	I3ASSERT(m_emptyCount < m_count);

	INT32 idx = pBuffer->GetIdx();

	m_emptyMutex.Enter();

	if (m_count <= m_emptyCount)
	{
		m_emptyMutex.Leave();
		return;
	}

	m_pEmptyBuffer[m_emptyRear] = idx;
	m_emptyRear = (m_emptyRear + 1) % m_count;
	m_emptyCount++;


	m_emptyMutex.Leave();
}
